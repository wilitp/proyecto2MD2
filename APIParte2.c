#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "APIParte2.h"


#define MAX_COLOR __UINT32_MAX__

static u32 colorear(u32 * coloresUsados, u32 umbral, u32 v, Grafo G, u32 *Color) {
  if (Grado(v, G) == 0) {
    // no tiene vecinos
    return 0;
  }
  for (u32 index = 0; index < Grado(v, G); ++index) {
    if(Color[IndiceVecino(index, v, G)] != MAX_COLOR){
      coloresUsados[Color[IndiceVecino(index, v, G)]] = umbral + 1;
    }
  }
  for (u32 index = 0; index < NumeroDeVertices(G); ++index) {
    if (coloresUsados[index] <= umbral) {
        return index;
    } 
  }
  return MAX_COLOR;
  // si terminamos es por que no habia hueco, necesitamos un nuevo color
}

// en Orden vienen especificados una serie de vertices ordenados a colorear.
// precon estan desde el 1 hasta n en Color en el indice n-simo esta el color
// con el que se se coloreo el vertice n-simo del grafo (notar que no nos
// importan los nombres de los vertices) si color en alguna posicion tiene el
// valor MAX_COLOR significa que ese vertice no fue pintado aun
u32 Greedy(Grafo G, u32 *Orden, u32 *Color) {
  u32 n = NumeroDeVertices(G);
  // ya sabemos hasta donde ir coloreando en orden
  for (u32 index = 0; index < n; ++index) {
    Color[index] = MAX_COLOR;
  }
  u32 max_color = 0;
  u32 * coloresUsados = calloc(NumeroDeVertices(G), sizeof(u32)); 
  for (u32 index = 0; index < n; ++index) {
    // cual vamos a ir coloreado:
    u32 vertice_a_colorear = Orden[index];
    // tenemos que colorer este vertice con el menor color posible que no tengan
    // los vecinos
    u32 nuevo_color = colorear(coloresUsados, index, vertice_a_colorear, G, Color);

    if (nuevo_color == MAX_COLOR) {
      // fallo colorear
      return (2 ^ 32) - 1;
    }
    if (nuevo_color+1 > max_color) {
      max_color = nuevo_color+1;
    }
    Color[Orden[index]] = nuevo_color;
  }

  return max_color;
}

u32 contarColores(u32 n, u32 *Color) {

  u32 colorc = 0;

  // Buscamos el color maximo (r-1 en la especificacion)
  for (u32 i = 0; i < n; ++i) {
    if (Color[i] > colorc)
      colorc = Color[i];
  }
  // Corrijo para que sea la cantidad de colores en vez del maximo color
  ++colorc;
  return colorc;
}

struct ColorInfo {
  u32 colorc;
  u32 **V;
  u32 *cantidades;
};

// Devuelve los V_i's, sus cardinales y la cantidad de colores
// el usuario de la funcion es el duenio de la referencia devuelta
struct ColorInfo *getColorInfo(u32 n, u32 *Color) {

  struct ColorInfo *colorInfo = malloc(sizeof(struct ColorInfo));

  colorInfo->colorc = contarColores(n, Color);
  // Los V_i del teorico
  colorInfo->V = malloc(colorInfo->colorc * sizeof(u32 *));

  // Cantidad de vertices por color
  // cantidades Color -> Nat
  colorInfo->cantidades = calloc(colorInfo->colorc, sizeof(u32));

  // Computamos las cantidades
  for (u32 i = 0; i < n; ++i) {
    ++(colorInfo->cantidades[Color[i]]);
  }

  // hago espacio para los V_i
  colorInfo->V[0] = malloc(n * sizeof(u32));
  u32 *nextV = colorInfo->V[0] + colorInfo->cantidades[0];
  // Los pongo uno seguido del otro
  for (u32 i = 1; i < colorInfo->colorc; ++i) {
    colorInfo->V[i] = nextV;
    nextV = colorInfo->V[i] + colorInfo->cantidades[i];
  }

  u32 *contadores = calloc((colorInfo->colorc), sizeof(u32));

  // agrego los vertices a sus V_i
  for (u32 i = 0; i < n; ++i) {
    colorInfo->V[Color[i]][contadores[Color[i]]] = i;
    ++contadores[Color[i]];
  }
  // al final de esto debe cumplirse contadores = cantidades

  free(contadores);

  return colorInfo;
}

void colorInfoDestroy(struct ColorInfo *ci) {
  // for (u32 i = 0; i < ci->colorc; ++i) {
  //   free(ci->V[i]);
  // }
  free(ci->cantidades);
  free(ci->V);
}

char OrdenImparPar(u32 n, u32 *Orden, u32 *Color) {
  // Conseguimos los V_i's, sus cardinales y la cantidad de colores
  struct ColorInfo *colorInfo = getColorInfo(n, Color);

  u32 k = 0;

  u32 upperOdd;
  u32 upperEven;
  if (colorInfo->colorc % 2 == 0) {
    upperOdd = colorInfo->colorc - 1;
    upperEven = colorInfo->colorc - 2;
  } else {
    upperEven = colorInfo->colorc - 1;
    upperOdd = colorInfo->colorc - 2;
  }
  // Pongo los impares
  for (u32 j = upperOdd; 0 <= j && j < colorInfo->colorc; j -= 2) {
    for (u32 i = 0; i < colorInfo->cantidades[j]; ++i) {
      Orden[k] = colorInfo->V[j][i];
      ++k;
    }
  }

  // Pongo los pares
  for (u32 j = upperEven; 0 <= j && j < colorInfo->colorc; j -= 2) {
    for (u32 i = 0; i < colorInfo->cantidades[j]; ++i) {
      Orden[k] = colorInfo->V[j][i];
      ++k;
    }
  }

  colorInfoDestroy(colorInfo);

  return 0;
}

// F definida en la spec para el orden jedi
// Tuve que hacerla variable global porq qsort_r no esta permitida en c99 (y los
// profes compilan con ese standard)
static u32 *F;

// Comparador para ordenar V_i's en el orden Jedi
// Asume que cuando se la llama F ya esta inicializada ya punta a un lugar de
// memoria valido
static int compar(const void *v1, const void *v2) {
  u32 vfirst = *((u32 *)v1);
  u32 vsecond = *((u32 *)v2);
  if (F[vfirst] > F[vsecond]) {
    return -1;
  } else if (F[vfirst] == F[vsecond]) {
    return 0;
  } else
    return 1;
}

// ordena indices en la forma especial dada en las especificaciones
char OrdenJedi(Grafo G, u32 *Orden, u32 *Color) {

  assert(F == NULL);
  // Conseguimos los V_i's, sus cardinales y la cantidad de colores
  struct ColorInfo *colorInfo = getColorInfo(NumeroDeVertices(G), Color);

  // Orden de los colores segun F
  u32 *ordenColores = malloc((colorInfo->colorc) * sizeof(u32));

  // Funcion F de la spec
  F = calloc(colorInfo->colorc, sizeof(u32));

  // Computamos las sumatorias de F
  for (u32 i = 0; i < NumeroDeVertices(G); ++i) {
    F[Color[i]] += Grado(i, G);
  }
  // Computamos el x * sumatoria
  for (u32 i = 0; i < colorInfo->colorc; ++i) {
    F[i] *= i;
  }

  // inicializo ordenColores
  for (u32 i = 0; i < colorInfo->colorc; ++i) {
    ordenColores[i] = i;
  }

  // Ordeno los V_i's segun F
  qsort(ordenColores, colorInfo->colorc, sizeof(u32), compar);

  // Usando ordenColores y cantidades, aplano V para obtener el Orden Jedi
  for (u32 j = 0, k = 0; j < colorInfo->colorc; ++j) {
    for (u32 i = 0; i < colorInfo->cantidades[ordenColores[j]]; ++i) {
      Orden[k] = colorInfo->V[ordenColores[j]][i];
      ++k;
    }
  }

  // Free de todo
  colorInfoDestroy(colorInfo);
  free(F);
  F = NULL; // Dejo F como NULL para proximas iteraciones

  return 0;
}

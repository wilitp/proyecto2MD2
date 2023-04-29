#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "APIG23.h"
#include "APIParte2.h"


#define MAX_COLOR __UINT32_MAX__

u32 Greedy(Grafo G, u32 *Orden, u32 *Color)
{
    u32 maxColors = 0;
    const u32 numVertices = NumeroDeVertices(G);
    // const u32* indices = Indices(G);

    // Iteramos sobre cada uno de los vertices en el orden dado
    for(u32 i = 0; i < numVertices; i++) {
        Color[i] = MAX_COLOR;
    }

    u32 *used_colors = calloc(numVertices, sizeof(u32));
    for (u32 i = 0; i < numVertices; i++) // O(3N^2) ~~ O(N^2)
    {
        u32 vertex = Orden[i];
        Color[vertex] = 0; // Inicializamos el color del vertice actual en 0 (el primero)

        for(u32 j = 0; j < Grado(vertex, G); j++) { // O(N)
            u32 neighbor = IndiceVecino(j, vertex, G);
            if(Color[neighbor] != MAX_COLOR) {
                used_colors[Color[neighbor]] = i + 1;
            }
        }

        u32 color = 0;
        while(used_colors[color] > i) // O(r) ~~ O(N)
            color++;

        // Actualizamos el numero de colores utilizados hasta el momento.
        Color[vertex] = color;
        if(color > maxColors) {
            maxColors = color;
        }
    }
    free(used_colors);
    // Devolvemos la cantidad de colores utilizados
    return maxColors + 1; // +1 porque empezamos desde el 0.
}

static int cmp(const void *v1, const void *v2) {
  u32 vfirst = *((u32 *)v1);
  u32 vsecond = *((u32 *)v2);
  if (vfirst < vsecond) {
    return -1;
  } else if (vfirst == vsecond) {
    return 0;
  } else
    return 1;
}

u32 *CMP_COLOR_ARRAY;

int cmpColorByParity(const void *a,
                     const void *b) { // Primero los impares luego los pares.
  u32 a_color = CMP_COLOR_ARRAY[*(u32 *)a];
  u32 b_color = CMP_COLOR_ARRAY[*(u32 *)b];
  bool a_color_odd = (a_color % 2);
  bool b_color_odd = (b_color % 2);
  if (a_color_odd == b_color_odd) // Si ambos tienen la misma paridad, los
                                  // ordenamos en base a su valor numerico
    return b_color - a_color;
  else if (a_color_odd) // Si a es impar y b es par entonces a es menor a b y
                        // por ende va primero
    return -1;
  else // Si a es par y b es impar entonces a es mayor a b y por ende a va
       // despues de b
    return 1;
}
static u32 colorear(u32 v, Grafo G, u32 *Color) {
  // creemos la lista de colores que tiene cada vecino
  if (Grado(v, G) == 0) {
    // no tiene vecinos
    return 1;
  }
  u32 *color_vecinos = calloc(Grado(v, G), sizeof(u32));
  if (color_vecinos == NULL) {
    return 0;
  }
  for (u32 index = 0; index < Grado(v, G); ++index) {
    color_vecinos[index] = Color[IndiceVecino(index, v, G)];
  }
  qsort(color_vecinos, Grado(v, G), sizeof(u32), cmp);
  // busquemos el primer espacio libre, si no hay ningun coloreamos con un nuevo
  // color xdnt
  u32 color_actual = 0;
  for (u32 index = 0; index < Grado(v, G); ++index) {
    if (color_vecinos[index] != 0) {
      if (color_actual + 1 <
          color_vecinos[index]) { // si el color que estoy apuntando es por mas
                                  // de 1 mayor al anterior pues tengo un hueco,
                                  // osea mi primer colo dispobible
        return color_actual + 1;
      } else {
        color_actual = color_vecinos[index];
      }
    }
  }
  // si terminamos es por que no habia hueco, necesitamos un nuevo color
  return color_actual + 1;
}
// en Orden vienen especificados una serie de vertices ordenados a colorear.
// precon estan desde el 1 hasta n en Color en el indice n-simo esta el color
// con el que se se coloreo el vertice n-simo del grafo (notar que no nos
// importan los nombres de los vertices) si color en alguna posicion tiene el
// valor 0 significa que ese vertice no fue pintado aun
// u32 Greedy(Grafo G, u32 *Orden, u32 *Color) {
//   u32 n = NumeroDeVertices(G);
//   // ya sabemos hasta donde ir coloreando en orden
//   for (u32 index = 0; index < n; ++index) {
//     Color[index] = 0;
//   }
//   u32 max_color = 0;
//   for (u32 index = 0; index < n; ++index) {
//     // cual vamos a ir coloreado:
//     u32 vertice_a_colorear = Orden[index];
//     // tenemos que colorer este vertice con el menor color posible que no tengan
//     // los vecinos
//     u32 nuevo_color = colorear(vertice_a_colorear, G, Color);
//
//     if (nuevo_color == 0) {
//       // fallo colorear
//       return (2 ^ 32) - 1;
//     }
//     if (nuevo_color > max_color) {
//       max_color = nuevo_color;
//     }
//     Color[index] = nuevo_color;
//   }
//   return max_color;
// }

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

  // hago espacio para cada V_i
  for (u32 i = 0; i < colorInfo->colorc; ++i) {
    colorInfo->V[i] = malloc(colorInfo->cantidades[i] * (sizeof(u32)));
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
  for (u32 i = 0; i < ci->colorc; ++i) {
    free(ci->V[i]);
  }
  free(ci->cantidades);
  free(ci->V);
}

char OrdenImparPar(u32 n, u32 *Orden, u32 *Color) {
  // Implementacion mucho mas simple pero nlog(n) igual usa menos memoria
  CMP_COLOR_ARRAY = Color;
  qsort(Orden, n, sizeof(u32), cmpColorByParity);
  return '0';
  // // Conseguimos los V_i's, sus cardinales y la cantidad de colores
  // struct ColorInfo *colorInfo = getColorInfo(n, Color);
  //
  // u32 k = 0;
  //
  // u32 upperOdd;
  // u32 upperEven;
  // if (colorInfo->colorc % 2 == 0) {
  //   upperOdd = colorInfo->colorc - 1;
  //   upperEven = colorInfo->colorc - 2;
  // } else {
  //   upperEven = colorInfo->colorc - 1;
  //   upperOdd = colorInfo->colorc - 2;
  // }
  // // Pongo los impares
  // for (u32 j = upperOdd; 0 <= j && j < colorInfo->colorc; j -= 2) {
  //   for (u32 i = 0; i < colorInfo->cantidades[j]; ++i) {
  //     Orden[k] = colorInfo->V[j][i];
  //     ++k;
  //   }
  // }
  //
  // // Pongo los pares
  // for (u32 j = upperEven; 0 <= j && j < colorInfo->colorc; j -= 2) {
  //   for (u32 i = 0; i < colorInfo->cantidades[j]; ++i) {
  //     Orden[k] = colorInfo->V[j][i];
  //     ++k;
  //   }
  // }
  //
  // colorInfoDestroy(colorInfo);
  //
  // return '0';
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
  if (F[vfirst] < F[vsecond]) {
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
    F[i] *= Color[i];
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

  return '0';
}

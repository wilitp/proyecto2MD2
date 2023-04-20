#include <assert.h>
#include <stdlib.h>

#include "APIG23.h"
#include "APIParte2.h"

u32 Greedy(Grafo G, u32 *Orden, u32 *Color) { return 0; }

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
  u32 **V = malloc(colorInfo->colorc * sizeof(u32));

  // Cantidad de vertices por color
  // cantidades Color -> Nat
  colorInfo->cantidades = malloc(colorInfo->colorc * sizeof(u32));

  // Computamos F y las cantidades
  for (u32 i = 0; i < n; ++i) {
    ++(colorInfo->cantidades[Color[i]]);
  }

  // hago espacio para cada V_i e inicializo ordenColores
  for (u32 i = 0; i < colorInfo->colorc; ++i) {
    V[i] = malloc(colorInfo->cantidades[i] * (sizeof(u32)));
  }

  u32 *contadores = malloc((colorInfo->colorc) * sizeof(u32));

  // agrego los vertices a sus V_i
  for (u32 i = 0; i < n; ++i) {
    V[Color[i]][contadores[Color[i]]] = i;
    ++contadores[Color[i]];
  }
  // al final de esto debe cumplirse contadores = cantidades

  return colorInfo;
}

void colorInfoDestroy(struct ColorInfo *ci) {
  for (u32 i = 0; i < ci->colorc; ++i) {
    free(ci->V[i]);
  }
  free(ci->cantidades);
  free(ci->V);
  free(ci->V);

}

char OrdenImparPar(u32 n, u32 *Orden, u32 *Color) {
  // Conseguimos los V_i's, sus cardinales y la cantidad de colores
  struct ColorInfo *colorInfo = getColorInfo(n, Color);

  u32 k = 0;
  // Pongo los impares
  for (u32 j = 1; j < colorInfo->colorc; j += 2) {
    for (u32 i = 0; i < colorInfo->cantidades[j]; ++i) {
      Orden[k] = colorInfo->V[j][i];
    }
  }

  // Pongo los pares
  for (u32 j = 0; j < colorInfo->colorc; j += 2) {
    for (u32 i = 0; i < colorInfo->cantidades[j]; ++i) {
      Orden[k] = colorInfo->V[j][i];
    }
  }

  colorInfoDestroy(colorInfo);

  return '0';
}

// F definida en la spec para el orden jedi
// Tuve que hacerla variable global porq qsort_r no esta permitida en c99 (y los
// profes compilan con ese standard)
static u32 *F;

// Comparador para ordenar V_i's en el orden Jedi
// Asume que cuando se la llama F ya esta inicializada ya punta a un lugar de
// memoria valido
int compar(const void *v1, const void *v2) {
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

  // Computamos F
  for (u32 i = 0; i < NumeroDeVertices(G); ++i) {
    F[Color[i]] += Grado(i, G);
  }

  // inicializo ordenColores
  for (u32 i = 0; i < colorInfo->colorc; ++i) {
    ordenColores[i] = i;
  }

  // Ordeno los V_i's segun F
  qsort(ordenColores, colorInfo->colorc, sizeof(u32), compar);

  // Usando ordenColores y cantidades, aplano V para obtener el Orden Jedi
  for (u32 j = 0, i = 0, k = 0;
       k < NumeroDeVertices(G) && j < colorInfo->colorc; ++k) {

    Orden[k] = colorInfo->V[ordenColores[j]][i];
    ++i;

    // Si ya terminamos con este color pasamos al siguiente
    if (i >= colorInfo->cantidades[j]) {
      i = 0;
      ++j;
    }
  }

  // Free de todo
  colorInfoDestroy(colorInfo);
  free(F);
  F = NULL; // Dejo F como NULL para proximas iteraciones

  return '0';
}

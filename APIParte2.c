#include <stdlib.h>

#include "APIParte2.h"
#include "APIG23.h"


u32 Greedy(Grafo G, u32 *Orden, u32 *Color) { return 0; }

char OrdenImparPar(u32 n, u32 *Orden, u32 *Color) { return '0'; }

// F definida en la spec para el orden jedi
static u32 *F;

// Comparador para ordenar V_i's en el orden Jedi
// Asume que cuando se la llama F ya esta inicializada ya punta a un lugar de memoria valido
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
  // Esta implementacion evita ordenar n vertices. Pero capaz era mas facil y usabamos mucha menos memoria xd
  u32 colorc = 0;

  // Buscamos el color maximo (r-1 en la especificacion)
  for (u32 i = 0; i < NumeroDeVertices(G); ++i) {
    if (Color[i] > colorc)
      colorc = Color[i];
  }
  // Corrijo para que sea la cantidad de colores en vez del maximo color
  ++colorc;

  // Los V_i del teorico
  u32 **V = malloc((colorc) * sizeof(u32));

  // Cantidad de vertices por color
  // cantidades Color -> Nat
  u32 *cantidades = malloc((colorc) * sizeof(u32));
  u32 *contadores = malloc((colorc) * sizeof(u32));
  // Orden de los colores segun F
  u32 *ordenColores = malloc((colorc) * sizeof(u32));

  // Funcion F de la spec
  F = malloc((colorc) * sizeof(u32));

  // Computamos F y las cantidades
  for (u32 i = 0; i < NumeroDeVertices(G); ++i) {
    F[Color[i]] += Grado(i, G);
    ++cantidades[Color[i]];
  }

  // hago espacio para cada V_i e inicializo ordenColores
  for (u32 i = 0; i < colorc; ++i) {
    V[i] = malloc(cantidades[i] * (sizeof(u32)));
    ordenColores[i] = i;
  }

  // agrego los vertices a sus V_i
  for (u32 i = 0; i < NumeroDeVertices(G); ++i) {
    V[Color[i]][contadores[Color[i]]] = i;
    ++contadores[Color[i]];
  }
  // al final de esto debe cumplirse contadores = cantidades
  
  // Ordeno los V_i's segun F
  qsort(ordenColores, colorc, sizeof(u32), compar);

  // Usando ordenColores y cantidades, aplano V para obtener el Orden Jedi
  for (u32 i = 0, j = 0; i < NumeroDeVertices(G) && j < colorc; ++i, ++j) {
    while (contadores[j] > 0) {
      Orden[i] = V[j][i];
      --contadores[j];
    }
  }
  // al final de esto contadores[i] = 0 forall i

  // TODO: free de todo xd

  return '0'; 
}

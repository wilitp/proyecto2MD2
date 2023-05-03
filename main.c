
// INTEGRANTES
// Alejo Corral alejo.corral@mi.unc.edu.ar
// Ignacion Martinez  ignacio.martinez.goloboff@mi.unc.edu.ar
// Guillermo de Ipola guillermo.ipola@mi.unc.edu.ar

#include "APIG23.h"
#include "APIParte2.h"
#include "stdio.h"
#define MAX_COLOR __UINT32_MAX__

bool checkear_coloreo(Grafo G, u32 *Color) {
  // O(m)
  for (u32 index = 0; index < NumeroDeVertices(G); ++index) {
    if (Color[index] ==
        MAX_COLOR) { // si hay un vertice que no tiene color esta mal
      return false;
    }
    for (u32 vecino = 0; vecino < Grado(index, G); ++vecino) {
      if (Color[index] == Color[IndiceVecino(vecino, index, G)]) {
        return false;
      }
    }
  }
  return true;
}

void probarImparPar(u32 n, u32 *Orden, u32 *Color) {
  u32 evenyet = 0;
  if(Color[Orden[n-1]] != 0) {
    printf("Troll el ultimo no es 0, es %u\n", Color[Orden[n-1]]);
    exit(0);
  }

  for (u32 i = 0; i < n; i++) {
    evenyet = evenyet || Color[Orden[i]] % 2 == 0;
    
    if(evenyet && Color[Orden[i]] % 2 != 0) {
      printf("Troll paridad\n");
      exit(0);
    }
    
  }
  
}

int main() {
  Grafo g = ConstruirGrafo();
  // DestruirGrafo(g);
  printf("se termino de cargar\n");
  u32 *color = malloc(NumeroDeVertices(g) * sizeof(u32));
  u32 *color1 = malloc(NumeroDeVertices(g) * sizeof(u32));
  u32 *orden = malloc(NumeroDeVertices(g) * sizeof(u32));
  u32 *orden1 = malloc(NumeroDeVertices(g) * sizeof(u32));
  for (u32 i = 0; i < NumeroDeVertices(g); ++i) {
    orden[i] = i;
    orden1[i] = i;
  }

  u32 colorcJedi = Greedy(g, orden, color);

  for (u32 i = 0; i < NumeroDeVertices(g); ++i) {
    color1[i] = color[i];
  }

  u32 colorcImp = colorcJedi;

  u32 colorcJediNuevo;
  u32 colorcImpNuevo;

  printf("Primer coloreo para la estrategia 1: %u colores\n", colorcJedi);
  printf("Primer coloreo para la estrategia 2: %u colores\n\n", colorcImp);

  for (u32 i = 1; i <= 500; ++i) {
    if (i % 16 == 0) {
      // printf("Alternando estrategias \n\n");
      u32 *aux;
      aux = orden1;
      orden1 = orden;
      orden = aux;

      aux = color1;
      color1 = color;
      color = aux;

      u32 aux1;
      aux1 = colorcJedi;
      colorcJedi = colorcImp;
      colorcImp = aux1;
    }

    OrdenJedi(g, orden, color);
    colorcJediNuevo = Greedy(g, orden, color);
    if (colorcJediNuevo > colorcJedi) {
      printf("Error: Jedi empeoro cantidad de colores: %u\n", colorcJediNuevo);
      exit(0);
    } else {
      colorcJedi = colorcJediNuevo;
      // printf("Estrategia 1: %u colores\n", colorcJedi);
    }

    OrdenImparPar(NumeroDeVertices(g), orden1, color1);
    colorcImpNuevo = Greedy(g, orden1, color1);
    if (colorcImpNuevo > colorcImp) {
      printf("Error: ImparPar empeoro cantidad de colores: %u\n", colorcImpNuevo);
      exit(0);
    } else {
      colorcImp = colorcImpNuevo;
      // printf("Estrategia 2: %u colores\n", colorcImp);
    }
  }

  printf("X(G) ~ %u\n", colorcImp < colorcJedi ? colorcImp : colorcJedi);

  DestruirGrafo(g);
  return 0;
}

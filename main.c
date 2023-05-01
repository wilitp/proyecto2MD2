#include "stdio.h"
#include "APIG23.h"
#include "APIParte2.h"
#define MAX_COLOR __UINT32_MAX__

bool checkear_coloreo(Grafo G, u32 * Color){
  //O(m)
  for(u32 index = 0; index < NumeroDeVertices(G); ++index){
    if(Color[index] == MAX_COLOR){  //si hay un vertice que no tiene color esta mal
      return false;
    }
    for(u32 vecino = 0; vecino < Grado(index, G); ++vecino){
      if(Color[index] == Color[IndiceVecino(vecino, index, G)]){
        return false;
      }
    }
  }
  return true;
}

int main()
{
    Grafo g = ConstruirGrafo();
    // DestruirGrafo(g);
    printf("se termino de cargar\n");
    u32 * color = malloc(NumeroDeVertices(g) * sizeof(u32));
    u32 * orden = malloc(NumeroDeVertices(g) * sizeof(u32));
    for(u32 i = 0; i<NumeroDeVertices(g); ++i){
      orden[i] = i;
    }
    u32 colores_usados = Greedy(g, orden, color);
    printf("se usaron %u colores\n\n", colores_usados);
    OrdenJedi(g, orden, color);
    colores_usados = Greedy(g, orden, color);
    printf("se usaron %u colores\n\n", colores_usados);
    // for(u32 i = 0; i<NumeroDeVertices(g); ++i){
    //   if(orden[i] >= NumeroDeVertices(g)) {
    //     printf("troleaste mano: \n \t Vertice: %u\n \t Total: %u\n", orden[i], NumeroDeVertices(g));
    //   }
    // }
    // colores_usados = Greedy(g, orden, color);
    // printf("se usaron %u colores dsp de orden jedi\n\n", colores_usados);

    bool b = checkear_coloreo(g, color);
    printf("es coloreo propio? %s\n", b ? "si" : "no");

    DestruirGrafo(g);
    return 0;
}

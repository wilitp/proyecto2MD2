#include "stdio.h"
#include "APIG23.h"
#include "APIParte2.h"
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
    DestruirGrafo(g);
    return 0;
}

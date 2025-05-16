#ifndef GRAPH_H
#define GRAPH_H
#include "region.h"

// Nó da lista de adjacência (aresta)
typedef struct EdgeNode {
    int dest;               // Índice do vértice de destino
    float weight;           // Peso da aresta (distância)
    struct EdgeNode* next;  // Próxima aresta
} EdgeNode;

// Estrutura do grafo
typedef struct {
    int num_vertices;       // Número de vértices (regiões)
    EdgeNode** adj_list;    // Vetor de listas de adjacência
    Region** regions;       // Vetor de ponteiros para regiões
} Graph;

// Funções da biblioteca de grafos
Graph* create_graph(int num_vertices);
void add_edge(Graph* graph, int src, int dest, float weight);
void set_region(Graph* graph, int index, Region* region);
Region* get_region(const Graph* graph, int index);
void free_graph(Graph* graph);

#endif // GRAPH_H


#ifndef GRAPH_H
#define GRAPH_H
#include "region.h"

#define MAX_VERTICES 100

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

// Persistência das arestas do grafo
int save_edges(const char* filename, const Graph* graph);
int load_edges(const char* filename, Graph* graph);

// Algoritmos de grafos
float dijkstra(const Graph* graph, int origem, int destino, int* caminho, int* tam_caminho);
void bfs(const Graph* graph, int origem, void (*visit)(int));
void dfs(const Graph* graph, int origem, void (*visit)(int));

#endif // GRAPH_H


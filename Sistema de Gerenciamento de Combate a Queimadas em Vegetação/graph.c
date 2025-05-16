#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

// Estrutura para criar um novo nó de aresta
EdgeNode* create_edge(int dest, float weight) {
    EdgeNode* new_edge = (EdgeNode*)malloc(sizeof(EdgeNode));
    if (!new_edge) return NULL;
    new_edge->dest = dest;
    new_edge->weight = weight;
    new_edge->next = NULL;
    return new_edge;
}

// Inicializa o grafo
Graph* create_graph(int num_vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    if (!graph) return NULL;
    graph->num_vertices = num_vertices;
    graph->adj_list = (EdgeNode**)malloc(num_vertices * sizeof(EdgeNode*));
    graph->regions = (Region**)malloc(num_vertices * sizeof(Region*));
    if (!graph->adj_list || !graph->regions) {
        free(graph->adj_list);
        free(graph->regions);
        free(graph);
        return NULL;
    }
    for (int i = 0; i < num_vertices; i++) {
        graph->adj_list[i] = NULL;
        graph->regions[i] = NULL;
    }
    return graph;
}

// Adiciona uma aresta (não direcionada)
void add_edge(Graph* graph, int src, int dest, float weight) {
    EdgeNode* new_edge = create_edge(dest, weight);
    new_edge->next = graph->adj_list[src];
    graph->adj_list[src] = new_edge;

    // Como é não direcionado, adiciona a aresta inversa
    new_edge = create_edge(src, weight);
    new_edge->next = graph->adj_list[dest];
    graph->adj_list[dest] = new_edge;
}

void set_region(Graph* graph, int index, Region* region) {
    if (graph && index >= 0 && index < graph->num_vertices) {
        if (graph->regions[index]) {
            free_region(graph->regions[index]);
        }
        graph->regions[index] = region;
    }
}

Region* get_region(const Graph* graph, int index) {
    if (graph && index >= 0 && index < graph->num_vertices) {
        return graph->regions[index];
    }
    return NULL;
}

// Libera memória do grafo
void free_graph(Graph* graph) {
    for (int i = 0; i < graph->num_vertices; i++) {
        EdgeNode* current = graph->adj_list[i];
        while (current) {
            EdgeNode* temp = current;
            current = current->next;
            free(temp);
        }
        if (graph->regions[i]) {
            free_region(graph->regions[i]);
        }
    }
    free(graph->adj_list);
    free(graph->regions);
    free(graph);
}


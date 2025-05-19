#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
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

// Persistência das arestas do grafo
int save_edges(const char* filename, const Graph* graph) {
    FILE* f = fopen(filename, "wb");
    if (!f) return 0;
    int num_vertices = graph->num_vertices;
    fwrite(&num_vertices, sizeof(int), 1, f);
    for (int i = 0; i < num_vertices; i++) {
        // Conta quantas arestas saem de i
        int count = 0;
        EdgeNode* e = graph->adj_list[i];
        while (e) {
            count++;
            e = e->next;
        }
        fwrite(&count, sizeof(int), 1, f);
        e = graph->adj_list[i];
        while (e) {
            fwrite(&e->dest, sizeof(int), 1, f);
            fwrite(&e->weight, sizeof(float), 1, f);
            e = e->next;
        }
    }
    fclose(f);
    return 1;
}

int load_edges(const char* filename, Graph* graph) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    int nv;
    if (fread(&nv, sizeof(int), 1, f) != 1 || nv != graph->num_vertices) {
        fclose(f);
        return 0;
    }
    // limpa arestas atuais
    for (int i = 0; i < nv; i++) {
        EdgeNode* e = graph->adj_list[i];
        while (e) {
            EdgeNode* tmp = e;
            e = e->next;
            free(tmp);
        }
        graph->adj_list[i] = NULL;
    }
    // lê e recria arestas
    for (int i = 0; i < nv; i++) {
        int count;
        fread(&count, sizeof(int), 1, f);
        for (int j = 0; j < count; j++) {
            int dest;
            float w;
            fread(&dest, sizeof(int), 1, f);
            fread(&w, sizeof(float), 1, f);
            EdgeNode* ne = create_edge(dest, w);
            ne->next = graph->adj_list[i];
            graph->adj_list[i] = ne;
        }
    }
    fclose(f);
    return 1;
}

// Algoritmo de Dijkstra para menor caminho
float dijkstra(const Graph* graph, int origem, int destino, int* caminho, int* tam_caminho) {
    int n = graph->num_vertices;
    float* dist = (float*)malloc(n * sizeof(float));
    int* prev = (int*)malloc(n * sizeof(int));
    int* visitado = (int*)calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        dist[i] = FLT_MAX;
        prev[i] = -1;
    }
    dist[origem] = 0.0f;

    for (int count = 0; count < n; count++) {
        // Encontra o vértice não visitado com menor distância
        float min = FLT_MAX;
        int u = -1;
        for (int i = 0; i < n; i++) {
            if (!visitado[i] && dist[i] < min) {
                min = dist[i];
                u = i;
            }
        }
        if (u == -1) break;
        visitado[u] = 1;
        EdgeNode* e = graph->adj_list[u];
        while (e) {
            if (!visitado[e->dest] && dist[u] + e->weight < dist[e->dest]) {
                dist[e->dest] = dist[u] + e->weight;
                prev[e->dest] = u;
            }
            e = e->next;
        }
    }

    // Reconstrói o caminho
    *tam_caminho = 0;
    int atual = destino;
    while (atual != -1) {
        caminho[(*tam_caminho)++] = atual;
        atual = prev[atual];
    }
    // Inverte o caminho
    for (int i = 0; i < *tam_caminho / 2; i++) {
        int tmp = caminho[i];
        caminho[i] = caminho[*tam_caminho - i - 1];
        caminho[*tam_caminho - i - 1] = tmp;
    }

    float resultado = dist[destino];
    free(dist);
    free(prev);
    free(visitado);
    return resultado;
}

// Busca em largura (BFS)
void bfs(const Graph* graph, int origem, void (*visit)(int)) {
    int n = graph->num_vertices;
    int* visitado = (int*)calloc(n, sizeof(int));
    int* fila = (int*)malloc(n * sizeof(int));
    int ini = 0, fim = 0;
    fila[fim++] = origem;
    visitado[origem] = 1;
    while (ini < fim) {
        int u = fila[ini++];
        visit(u);
        EdgeNode* e = graph->adj_list[u];
        while (e) {
            if (!visitado[e->dest]) {
                fila[fim++] = e->dest;
                visitado[e->dest] = 1;
            }
            e = e->next;
        }
    }
    free(visitado);
    free(fila);
}

// Busca em profundidade (DFS)
void dfs_visit(const Graph* graph, int u, int* visitado, void (*visit)(int)) {
    visitado[u] = 1;
    visit(u);
    EdgeNode* e = graph->adj_list[u];
    while (e) {
        if (!visitado[e->dest]) {
            dfs_visit(graph, e->dest, visitado, visit);
        }
        e = e->next;
    }
}

void dfs(const Graph* graph, int origem, void (*visit)(int)) {
    int n = graph->num_vertices;
    int* visitado = (int*)calloc(n, sizeof(int));
    dfs_visit(graph, origem, visitado, visit);
    free(visitado);
}


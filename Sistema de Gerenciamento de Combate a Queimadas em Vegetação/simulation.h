#ifndef SIMULATION_H
#define SIMULATION_H

#include "graph.h"
#include "brigade.h"

typedef struct {
    int pos;           // Região atual do caminhão
    int agua;          // Água restante
    int capacidade;    // Capacidade máxima
    int ativo;         // 1 se está disponível, 0 se não
} CaminhaoBrigada;

typedef struct {
    CaminhaoBrigada* caminhoes;
    int num_caminhoes;
    int tempo_total;
    int vertices_salvos;
    int agua_usada;
} ResultadoSimulacao;

typedef struct {
    int vertices_salvos;
    int vertices_queimados;
    int tempo_total;
    int agua_usada;
} SimulationResult;

// Simula a propagação do fogo e o combate automático
ResultadoSimulacao simular_fogo(Graph* graph, int inicio_fogo, int capacidade_caminhao);
SimulationResult simular_propagacao_fogo(Graph* graph, BrigadeSystem* brigadas, int vertice_inicial);

#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simulation.h"

#define MAX_EQUIPE 3

// Função auxiliar: retorna 1 se região é ponto de água
int is_water_source(const Graph* graph, int idx) {
    return graph->regions[idx]->is_water_source || graph->regions[idx]->is_brigade_post;
}

// Função auxiliar: encontra o ponto de água mais próximo usando Dijkstra
int ponto_agua_mais_proximo(const Graph* graph, int origem) {
    float menor = 1e9;
    int melhor = -1;
    int n = graph->num_vertices;
    int caminho[n], tam;
    for (int i = 0; i < n; i++) {
        if (is_water_source(graph, i)) {
            float dist = dijkstra(graph, origem, i, caminho, &tam);
            if (dist < menor) {
                menor = dist;
                melhor = i;
            }
        }
    }
    return melhor;
}

// Inicializa brigadistas nos postos
void inicializar_brigadistas(const Graph* graph, CaminhaoBrigada* caminhoes, int capacidade) {
    int idx = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        if (graph->regions[i]->is_brigade_post && idx < MAX_EQUIPE) {
            caminhoes[idx].pos = i;
            caminhoes[idx].agua = capacidade;
            caminhoes[idx].capacidade = capacidade;
            caminhoes[idx].ativo = 1;
            idx++;
        }
    }
    // Se não houver 3 postos, preenche o resto como inativos
    for (; idx < MAX_EQUIPE; idx++) {
        caminhoes[idx].ativo = 0;
    }
}

// Reseta o estado de fogo de todas as regiões
static void resetar_estado_fogo(Graph* graph) {
    for (int i = 0; i < graph->num_vertices; i++) {
        if (graph->regions[i]) {
            graph->regions[i]->on_fire = 0;
            graph->regions[i]->burned = 0;
        }
    }
}

// Conta vértices salvos e queimados
static void contar_resultados(Graph* graph, int* salvos, int* queimados) {
    *salvos = 0;
    *queimados = 0;
    for (int i = 0; i < graph->num_vertices; i++) {
        if (graph->regions[i]) {
            if (graph->regions[i]->burned)
                (*queimados)++;
            else
                (*salvos)++;
        }
    }
}

SimulationResult simular_propagacao_fogo(Graph* graph, BrigadeSystem* brigadas, int vertice_inicial) {
    int n = graph->num_vertices;
    int tempo = 0, agua_usada = 0, salvos = 0;
    int* fila        = malloc(n * sizeof(int));
    int* tempo_fogo  = calloc(n, sizeof(int));
    int* burned_old  = calloc(n, sizeof(int));  // rastreia o que já contabilizamos
    int ini = 0, fim = 0;

    resetar_estado_fogo(graph);
    graph->regions[vertice_inicial]->on_fire = 1;
    fila[fim++] = vertice_inicial;

    int fogo_ativo = 1;
    while (fogo_ativo) {
        fogo_ativo = 0;
        int tamanho = fim - ini;

        // 1) propagação em BFS discreto
        for (int k = 0; k < tamanho; ++k) {
            int u = fila[ini++];
            if (!graph->regions[u]->on_fire || graph->regions[u]->burned) continue;
            if (tempo_fogo[u] >= 1) {
                graph->regions[u]->burned  = 1;
                graph->regions[u]->on_fire = 0;
                continue;
            }
            for (EdgeNode* e = graph->adj_list[u]; e; e = e->next) {
                int v = e->dest;
                if (!graph->regions[v]->on_fire && !graph->regions[v]->burned) {
                    graph->regions[v]->on_fire = 2;
                    tempo_fogo[v] = 0;
                }
            }
            tempo_fogo[u]++;
            fogo_ativo = 1;
        }

        // 2) agenda brigadistas (não conta aqui)
        for (int i = 0; i < n; ++i) {
            if (graph->regions[i]->on_fire == 1 && !graph->regions[i]->burned) {
                despachar_brigadistas_para_fogo(brigadas, graph, graph->regions, i);
            }
        }

        // 3) torna fogo agendado efetivo
        for (int i = 0; i < n; ++i) {
            if (graph->regions[i]->on_fire == 2) {
                graph->regions[i]->on_fire = 1;
                fogo_ativo = 1;
            }
        }

        // 4) move caminhões, faz extinção e prints em brigade.c
        atualizar_brigade_system(brigadas, graph, graph->regions, n);

        // 5) contabiliza as extinções feitas em atualizar_brigade_system
        for (int i = 0; i < n; ++i) {
            if (graph->regions[i]->burned && !burned_old[i]) {
                burned_old[i] = 1;
                agua_usada   += graph->regions[i]->water_required;
                salvos++;
            }
        }

        // 6) reconstrói fila de vértices ainda em chamas
        ini = 0; fim = 0;
        for (int i = 0; i < n; ++i) {
            if (graph->regions[i]->on_fire == 1 && !graph->regions[i]->burned) {
                fila[fim++] = i;
            }
        }

        tempo++;
    }

    free(fila);
    free(tempo_fogo);
    free(burned_old);

    SimulationResult result;
    result.vertices_salvos    = salvos;
    result.vertices_queimados = n - salvos;
    result.tempo_total        = tempo;
    result.agua_usada         = agua_usada;
    return result;
}

// Simulação principal
ResultadoSimulacao simular_fogo(Graph* graph, int inicio_fogo, int capacidade_caminhao) {
    int n = graph->num_vertices;
    int tempo = 0, agua_usada = 0, vertices_salvos = 0;
    CaminhaoBrigada caminhoes[MAX_EQUIPE];
    int fogo[n], queimado[n], salvo[n];
    memset(fogo, 0, sizeof(fogo));
    memset(queimado, 0, sizeof(queimado));
    memset(salvo, 0, sizeof(salvo));

    // Inicializa brigadistas
    inicializar_brigadistas(graph, caminhoes, capacidade_caminhao);

    // Inicia o fogo
    fogo[inicio_fogo] = 1;

    int propagando = 1;
    while (propagando) {
        propagando = 0;
        // 1. Brigadistas tentam apagar o fogo nas regiões em chamas
        for (int i = 0; i < MAX_EQUIPE; i++) {
            if (!caminhoes[i].ativo) continue;
            // Busca região em fogo mais próxima
            int alvo = -1, tam_caminho = 0;
            float menor = 1e9;
            int caminho[n];
            for (int j = 0; j < n; j++) {
                if (fogo[j] && !queimado[j]) {
                    float dist = dijkstra(graph, caminhoes[i].pos, j, caminho, &tam_caminho);
                    if (dist < menor) {
                        menor = dist;
                        alvo = j;
                    }
                }
            }
            if (alvo == -1) continue; // Nada para apagar

            // Caminhão vai até o alvo (ignora tempo de deslocamento para simplificar)
            caminhoes[i].pos = alvo;

            // Verifica se tem água suficiente
            int agua_necessaria = graph->regions[alvo]->water_required;
            if (caminhoes[i].agua < agua_necessaria) {
                // Vai até ponto de água mais próximo
                int recarga = ponto_agua_mais_proximo(graph, caminhoes[i].pos);
                caminhoes[i].pos = recarga;
                caminhoes[i].agua = caminhoes[i].capacidade;
                // Volta para o alvo (ignora tempo)
                caminhoes[i].pos = alvo;
            }
            // Apaga o fogo se houver equipes suficientes
            if (graph->regions[alvo]->teams_required <= 1) { // 1 caminhão = 1 equipe
                caminhoes[i].agua -= agua_necessaria;
                agua_usada += agua_necessaria;
                fogo[alvo] = 0;
                salvo[alvo] = 1;
            }
        }

        // 2. Propagação do fogo para vizinhos
        int novo_fogo[n];
        memcpy(novo_fogo, fogo, sizeof(fogo));
        for (int i = 0; i < n; i++) {
            if (fogo[i] && !queimado[i]) {
                EdgeNode* e = graph->adj_list[i];
                while (e) {
                    if (!fogo[e->dest] && !queimado[e->dest]) {
                        novo_fogo[e->dest] = 1;
                        propagando = 1;
                    }
                    e = e->next;
                }
                queimado[i] = 1;
            }
        }
        memcpy(fogo, novo_fogo, sizeof(fogo));
        tempo++;
    }

    // Conta vértices salvos
    for (int i = 0; i < n; i++) {
        if (!queimado[i] || salvo[i]) vertices_salvos++;
    }

    ResultadoSimulacao res;
    res.caminhoes = malloc(sizeof(CaminhaoBrigada) * MAX_EQUIPE);
    memcpy(res.caminhoes, caminhoes, sizeof(CaminhaoBrigada) * MAX_EQUIPE);
    res.num_caminhoes = MAX_EQUIPE;
    res.tempo_total = tempo;
    res.vertices_salvos = vertices_salvos;
    res.agua_usada = agua_usada;
    return res;
}


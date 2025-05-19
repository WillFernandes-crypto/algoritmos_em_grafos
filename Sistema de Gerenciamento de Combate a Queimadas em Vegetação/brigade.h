#ifndef BRIGADE_H
#define BRIGADE_H

#include "region.h"
#include "graph.h"

// Parâmetros do caminhão
typedef struct {
    int id;
    int capacidade_agua;      // Capacidade máxima (litros)
    int agua_atual;           // Quantidade atual de água (litros)
    int posicao;              // Índice do vértice atual no grafo
    int ocupado;              // 1 se está em missão, 0 se está disponível
    int caminho[MAX_VERTICES]; // Caminho atual (sequência de vértices)
    int caminho_tam;          // Tamanho do caminho
    int caminho_idx;          // Próximo passo no caminho
} Truck;

// Equipe de brigadistas
typedef struct {
    int id;
    int posicao;              // Índice do vértice atual no grafo
    int ocupado;              // 1 se está em missão, 0 se está disponível
    Truck *caminhao;          // Caminhão associado
} BrigadeTeam;

// Posto de brigadistas
typedef struct {
    int id;
    int vertice;              // Índice do vértice no grafo
    BrigadeTeam *equipes;
    int num_equipes;
    Truck *caminhoes;
    int num_caminhoes;
} BrigadePost;

// Estrutura principal de gerenciamento de brigadas
typedef struct {
    BrigadePost *postos;
    int num_postos;
    int capacidade_caminhao; // Capacidade padrão dos caminhões
} BrigadeSystem;

// Funções de criação e destruição
BrigadeSystem* criar_brigade_system(Graph *grafo, int num_postos, int num_equipes_por_posto, int capacidade_caminhao);
void destruir_brigade_system(BrigadeSystem *bs);

// Alocação inicial dos postos de brigadistas (aleatória)
void distribuir_postos_brigadistas(Graph *grafo, BrigadeSystem *bs, Region *regioes, int num_regioes);

// Busca o posto mais próximo de um vértice alvo
int encontrar_posto_mais_proximo(BrigadeSystem *bs, Graph *grafo, int alvo);

// Envia equipes e caminhões para combater incêndio em um vértice
// Retorna 1 se conseguiu enviar recursos suficientes, 0 se não foi possível
int despachar_brigadistas_para_fogo(BrigadeSystem *bs, Graph *grafo, Region *regioes, int vertice_fogo);

// Atualiza o estado dos caminhões e equipes (movimentação, reabastecimento, etc)
void atualizar_brigade_system(BrigadeSystem *bs, Graph *grafo, Region *regioes, int num_regioes);

// Relatórios
void imprimir_status_brigade_system(BrigadeSystem *bs, Graph *grafo, Region *regioes, int num_regioes);
void imprimir_caminhos_percorridos(BrigadeSystem *bs);

#endif // BRIGADE_H


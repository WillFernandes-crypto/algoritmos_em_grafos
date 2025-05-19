#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "brigade.h"

// Função auxiliar: sorteia vértices distintos para postos
static void sortear_vertices_distintos(int *dest, int n, int max) {
    int i, j, v, ok;
    for (i = 0; i < n; ++i) {
        do {
            ok = 1;
            v = rand() % max;
            for (j = 0; j < i; ++j)
                if (dest[j] == v) ok = 0;
        } while (!ok);
        dest[i] = v;
    }
}

// Sorteia 'n' vértices distintos em [0,max), pulando 'excluida'
static void sortear_vertices_distintos_exc(int *dest, int n, int max, int excluida) {
    int count = 0;
    while (count < n) {
        int v = rand() % max;
        if (v == excluida) continue;
        int ok = 1;
        for (int j = 0; j < count; ++j)
            if (dest[j] == v) { ok = 0; break; }
        if (ok) dest[count++] = v;
    }
}

BrigadeSystem* criar_brigade_system(Graph *grafo, int num_postos, int num_equipes_por_posto, int capacidade_caminhao) {
    BrigadeSystem *bs = malloc(sizeof(BrigadeSystem));
    bs->num_postos = num_postos;
    bs->capacidade_caminhao = capacidade_caminhao;
    bs->postos = malloc(num_postos * sizeof(BrigadePost));
    for (int i = 0; i < num_postos; ++i) {
        bs->postos[i].id = i;
        bs->postos[i].equipes = malloc(num_equipes_por_posto * sizeof(BrigadeTeam));
        bs->postos[i].num_equipes = num_equipes_por_posto;
        bs->postos[i].caminhoes = malloc(num_equipes_por_posto * sizeof(Truck));
        bs->postos[i].num_caminhoes = num_equipes_por_posto;
        for (int j = 0; j < num_equipes_por_posto; ++j) {
            bs->postos[i].equipes[j].id = j;
            bs->postos[i].equipes[j].ocupado = 0;
            bs->postos[i].equipes[j].caminhao = &bs->postos[i].caminhoes[j];
            bs->postos[i].caminhoes[j].id = j;
            bs->postos[i].caminhoes[j].capacidade_agua = capacidade_caminhao;
            bs->postos[i].caminhoes[j].agua_atual = capacidade_caminhao;
            bs->postos[i].caminhoes[j].ocupado = 0;
            bs->postos[i].caminhoes[j].caminho_tam = 0;
            bs->postos[i].caminhoes[j].caminho_idx = 0;
        }
    }
    return bs;
}

void destruir_brigade_system(BrigadeSystem *bs) {
    for (int i = 0; i < bs->num_postos; ++i) {
        free(bs->postos[i].equipes);
        free(bs->postos[i].caminhoes);
    }
    free(bs->postos);
    free(bs);
}

void distribuir_postos_brigadistas(Graph *grafo, BrigadeSystem *bs, Region **regioes, int num_regioes) {
    int *vertices = malloc(bs->num_postos * sizeof(int));
    sortear_vertices_distintos(vertices, bs->num_postos, num_regioes);
    for (int i = 0; i < bs->num_postos; ++i) {
        bs->postos[i].vertice = vertices[i];
        for (int j = 0; j < bs->postos[i].num_equipes; ++j) {
            bs->postos[i].equipes[j].posicao = vertices[i];
            bs->postos[i].caminhoes[j].posicao = vertices[i];
        }
        regioes[vertices[i]]->is_brigade_post = 1;
    }
    free(vertices);
}

// Distribuição garantindo que não haja posto em 'excluida'
void distribuir_postos_brigadistas_exc(Graph *grafo, BrigadeSystem *bs, Region **regioes, int num_regioes, int excluida) {
    int *vertices = malloc(bs->num_postos * sizeof(int));
    sortear_vertices_distintos_exc(vertices, bs->num_postos, num_regioes, excluida);
    for (int i = 0; i < bs->num_postos; ++i) {
        bs->postos[i].vertice = vertices[i];
        for (int j = 0; j < bs->postos[i].num_equipes; ++j) {
            bs->postos[i].equipes[j].posicao   = vertices[i];
            bs->postos[i].caminhoes[j].posicao = vertices[i];
        }
        regioes[vertices[i]]->is_brigade_post = 1;
    }
    free(vertices);
}

// Função auxiliar: Dijkstra para distâncias mínimas a partir de um vértice
static void dijkstra_distances(const Graph *grafo, int origem, int *dist, int *prev) {
    int n = grafo->num_vertices;
    int visitado[MAX_VERTICES] = {0};
    for (int i = 0; i < n; ++i) {
        dist[i] = 1e9;
        prev[i] = -1;
    }
    dist[origem] = 0;
    for (int count = 0; count < n; ++count) {
        int u = -1, min = 1e9;
        for (int i = 0; i < n; ++i)
            if (!visitado[i] && dist[i] < min) { min = dist[i]; u = i; }
        if (u == -1) break;
        visitado[u] = 1;
        EdgeNode* e = grafo->adj_list[u];
        while (e) {
            int v = e->dest;
            int peso = e->weight;
            if (dist[u] + peso < dist[v]) {
                dist[v] = dist[u] + peso;
                prev[v] = u;
            }
            e = e->next;
        }
    }
}

// Retorna índice do posto mais próximo do alvo
int encontrar_posto_mais_proximo(BrigadeSystem *bs, Graph *grafo, int alvo) {
    int menor = 1e9, idx = -1;
    for (int i = 0; i < bs->num_postos; ++i) {
        int dist[MAX_VERTICES], prev[MAX_VERTICES];
        dijkstra_distances(grafo, bs->postos[i].vertice, dist, prev);
        if (dist[alvo] < menor) {
            menor = dist[alvo];
            idx = i;
        }
    }
    return idx;
}

// Monta caminho de prev[] para caminho[]
static int montar_caminho(int *prev, int destino, int *caminho) {
    int tam = 0, v = destino;
    int temp[MAX_VERTICES];
    while (v != -1) {
        temp[tam++] = v;
        v = prev[v];
    }
    // Inverter
    for (int i = 0; i < tam; ++i)
        caminho[i] = temp[tam - 1 - i];
    return tam;
}

// Busca ponto de coleta de água mais próximo
static int encontrar_ponto_agua_mais_proximo(Graph *grafo, Region *regioes, int num_regioes, int origem) {
    int menor = 1e9, idx = -1;
    int dist[MAX_VERTICES], prev[MAX_VERTICES];
    dijkstra_distances(grafo, origem, dist, prev);
    for (int i = 0; i < num_regioes; ++i) {
        if (regioes[i].is_water_source || regioes[i].is_brigade_post) {
            if (dist[i] < menor) {
                menor = dist[i];
                idx = i;
            }
        }
    }
    return idx;
}

// Despacha equipes e caminhões para combater fogo em vertice_fogo
int despachar_brigadistas_para_fogo(BrigadeSystem *bs, Graph *grafo, Region *regioes, int vertice_fogo) {
    int posto_idx = encontrar_posto_mais_proximo(bs, grafo, vertice_fogo);
    if (posto_idx == -1) return 0;
    BrigadePost *posto = &bs->postos[posto_idx];
    int equipes_necessarias = regioes[vertice_fogo].teams_required;
    int agua_necessaria = regioes[vertice_fogo].water_required;
    int equipes_enviadas = 0, agua_enviada = 0;

    // Envia equipes e caminhões disponíveis
    for (int i = 0; i < posto->num_equipes && equipes_enviadas < equipes_necessarias; ++i) {
        BrigadeTeam *equipe = &posto->equipes[i];
        Truck *caminhao = equipe->caminhao;
        if (!equipe->ocupado && !caminhao->ocupado) {
            // Monta caminho até o fogo
            int dist[MAX_VERTICES], prev[MAX_VERTICES];
            dijkstra_distances(grafo, caminhao->posicao, dist, prev);
            caminhao->caminho_tam = montar_caminho(prev, vertice_fogo, caminhao->caminho);
            caminhao->caminho_idx = 1; // 0 é posição atual
            caminhao->ocupado = 1;
            equipe->ocupado = 1;
            equipes_enviadas++;
            agua_enviada += caminhao->agua_atual;
            // Se não tiver água suficiente, já programa reabastecimento
            if (agua_enviada < agua_necessaria && caminhao->agua_atual < agua_necessaria) {
                int ponto_agua = encontrar_ponto_agua_mais_proximo(grafo, regioes, grafo->num_vertices, caminhao->posicao);
                if (ponto_agua != -1) {
                    int dist2[MAX_VERTICES], prev2[MAX_VERTICES];
                    dijkstra_distances(grafo, caminhao->posicao, dist2, prev2);
                    int tam1 = montar_caminho(prev2, ponto_agua, caminhao->caminho);
                    dijkstra_distances(grafo, ponto_agua, dist2, prev2);
                    int tam2 = montar_caminho(prev2, vertice_fogo, caminhao->caminho + tam1 - 1);
                    caminhao->caminho_tam = tam1 + tam2 - 1;
                    caminhao->caminho_idx = 1;
                }
            }
        }
    }
    return (equipes_enviadas >= equipes_necessarias);
}

// Atualiza movimentação dos caminhões/equipes, reabastecimento, etc
void atualizar_brigade_system(BrigadeSystem *bs, Graph *grafo, Region *regioes, int num_regioes) {
    for (int p = 0; p < bs->num_postos; ++p) {
        BrigadePost *posto = &bs->postos[p];
        for (int i = 0; i < posto->num_equipes; ++i) {
            BrigadeTeam *equipe = &posto->equipes[i];
            Truck *caminhao = equipe->caminhao;
            if (caminhao->ocupado && caminhao->caminho_idx < caminhao->caminho_tam) {
                // Move um passo
                caminhao->posicao = caminhao->caminho[caminhao->caminho_idx];
                equipe->posicao = caminhao->posicao;
                caminhao->caminho_idx++;
                // Chegou ao destino?
                if (caminhao->caminho_idx == caminhao->caminho_tam) {
                    int v = caminhao->posicao;
                    // Se for ponto de água, reabastece
                    if (regioes[v].is_water_source || regioes[v].is_brigade_post) {
                        caminhao->agua_atual = caminhao->capacidade_agua;
                    }
                    // Se for local de fogo, combate
                    if (regioes[v].on_fire) {
                        int agua_usada = regioes[v].water_required;
                        if (caminhao->agua_atual >= agua_usada) {
                            caminhao->agua_atual -= agua_usada;
                            regioes[v].on_fire = 0;
                            regioes[v].burned = 0;
                            printf("Equipe %d do posto %d apagou o fogo em %s!\n", equipe->id, posto->id, regioes[v].nome);
                        } else {
                            // Precisa reabastecer e voltar
                            int ponto_agua = encontrar_ponto_agua_mais_proximo(grafo, regioes, num_regioes, v);
                            if (ponto_agua != -1) {
                                int dist[MAX_VERTICES], prev[MAX_VERTICES];
                                dijkstra_distances(grafo, v, dist, prev);
                                int tam1 = montar_caminho(prev, ponto_agua, caminhao->caminho);
                                dijkstra_distances(grafo, ponto_agua, dist, prev);
                                int tam2 = montar_caminho(prev, v, caminhao->caminho + tam1 - 1);
                                caminhao->caminho_tam = tam1 + tam2 - 1;
                                caminhao->caminho_idx = 1;
                            }
                        }
                    }
                    // Missão concluída
                    caminhao->ocupado = 0;
                    equipe->ocupado = 0;
                }
            }
        }
    }
}

void imprimir_status_brigade_system(BrigadeSystem *bs, Graph *grafo, Region *regioes, int num_regioes) {
    printf("=== STATUS DAS BRIGADAS ===\n");
    for (int p = 0; p < bs->num_postos; ++p) {
        BrigadePost *posto = &bs->postos[p];
        printf("Posto %d em %s (v%d):\n", posto->id, regioes[posto->vertice].nome, posto->vertice);
        for (int i = 0; i < posto->num_equipes; ++i) {
            BrigadeTeam *equipe = &posto->equipes[i];
            Truck *caminhao = equipe->caminhao;
            printf("  Equipe %d: posicao=%s (v%d), caminhão: agua=%d/%d, ocupado=%d\n",
                equipe->id, regioes[equipe->posicao].nome, equipe->posicao,
                caminhao->agua_atual, caminhao->capacidade_agua, equipe->ocupado);
        }
    }
}

void imprimir_caminhos_percorridos(BrigadeSystem *bs) {
    printf("=== CAMINHOS PERCORRIDOS PELAS EQUIPES ===\n");
    for (int p = 0; p < bs->num_postos; ++p) {
        BrigadePost *posto = &bs->postos[p];
        for (int i = 0; i < posto->num_caminhoes; ++i) {
            Truck *caminhao = &posto->caminhoes[i];
            printf("Equipe %d do Posto %d: ", i, p);
            if (caminhao->caminho_tam > 1) {
                for (int j = 0; j < caminhao->caminho_tam; ++j) {
                    printf("%d ", caminhao->caminho[j]);
                }
                printf("\n");
            } else {
                printf("Não saiu do posto.\n");
            }
        }
    }
}


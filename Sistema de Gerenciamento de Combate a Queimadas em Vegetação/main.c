#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>           // ← adicionar
#include "graph.h"
#include "region.h"
#include "wildfire_management.h"
#include "report.h"
#include "utils.h"
#include "simulation.h"
#include "brigade.h"

#define MAX_REGIOES 100

static int proxima_regiao = 0; // fora do main(), ou como variável estática no main()

void menu() {
    printf("\n=== Sistema de Gerenciamento de Combate a Queimadas ===\n");
    printf("1. Cadastrar região\n");
    printf("2. Adicionar aresta (distância entre regiões)\n");
    printf("3. Registrar queimada\n");
    printf("4. Relatório de regiões\n");
    printf("5. Relatório de queimadas\n");
    printf("6. Relatório de queimadas por região\n");
    printf("7. Salvar dados\n");
    printf("8. Carregar dados\n");
    printf("9. Simular propagação do fogo\n");
    printf("10. Simular fogo em todos os vértices (exceto postos de brigada)\n");
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    srand((unsigned)time(NULL));   // ← inicializa rand()

    int num_regioes = read_positive_int("Informe o número de regiões: ");
    if (num_regioes <= 0 || num_regioes > MAX_REGIOES) {
        printf("Número de regiões inválido.\n");
        return 1;
    }

    Graph* graph = create_graph(num_regioes);
    WildfireList* wildfires = create_wildfire_list();

    int opcao;
    do {
        menu();
        opcao = read_int("");
        switch (opcao) {
            case 1: {
                char nome[100], tipo_vegetacao[100];
                float area;
                int is_water_source, water_required, teams_required;

                printf("Nome da região: ");
                scanf(" %[^\n]", nome);

                printf("Tipo de vegetação: ");
                scanf(" %[^\n]", tipo_vegetacao);

                printf("Área (em hectares): ");
                scanf("%f", &area);

                printf("É ponto de coleta de água? (1-Sim, 0-Não): ");
                scanf("%d", &is_water_source);

                printf("Quantidade de água necessária para apagar o fogo (em litros): ");
                scanf("%d", &water_required);

                printf("Número de equipes necessárias: ");
                scanf("%d", &teams_required);

                Region* reg = create_region(nome, tipo_vegetacao, area,
                                           0, is_water_source,
                                           water_required, teams_required);
                if (reg) {
                    set_region(graph, proxima_regiao, reg);
                    printf("Região cadastrada com sucesso! Índice: %d\n", proxima_regiao);
                    proxima_regiao++;
                } else {
                    printf("Erro ao cadastrar região.\n");
                }
                break;
            }
            case 2: {
                int src = read_positive_int("Região origem: ");
                int dest = read_positive_int("Região destino: ");
                float dist = read_positive_float("Distância: ");
                if (src < 0 || src >= num_regioes || dest < 0 || dest >= num_regioes) {
                    printf("Índice inválido.\n");
                    break;
                }
                add_edge(graph, src, dest, dist);
                printf("Aresta adicionada!\n");
                break;
            }
            case 3: {
                int reg = read_positive_int("Índice da região: ");
                if (reg < 0 || reg >= num_regioes) {
                    printf("Índice inválido.\n");
                    break;
                }
                char data[MAX_DATA];
                float intensidade;
                do {
                    read_string("Data (DD/MM/AAAA): ", data, MAX_DATA);
                    if (!validate_date(data))
                        printf("Data inválida! Use o formato DD/MM/AAAA.\n");
                } while (!validate_date(data));
                intensidade = read_positive_float("Intensidade: ");
                Wildfire* wf = create_wildfire(reg, data, intensidade);
                add_wildfire(wildfires, wf);
                printf("Queimada registrada!\n");
                break;
            }
            case 4:
                report_all_regions(graph);
                break;
            case 5:
                report_all_wildfires(wildfires);
                break;
            case 6: {
                int reg = read_positive_int("Índice da região: ");
                if (reg < 0 || reg >= num_regioes) {
                    printf("Índice inválido.\n");
                    break;
                }
                report_wildfires_by_region(graph, wildfires, reg);
                break;
            }
            case 7: // Salvar dados
                if (save_regions("regioes.bin", graph->regions, num_regioes) &&
                    save_edges("arestas.bin", graph) &&
                    save_wildfires("queimadas.bin", wildfires)) {
                    printf("Dados salvos com sucesso!\n");
                } else {
                    printf("Erro ao salvar dados!\n");
                }
                break;
            case 8: // Carregar dados
                if (load_regions("regioes.bin", graph->regions, num_regioes) &&
                    load_edges("arestas.bin", graph) &&
                    load_wildfires("queimadas.bin", wildfires)) {
                    printf("Dados carregados com sucesso!\n");
                } else {
                    printf("Erro ao carregar dados!\n");
                }
                break;
            case 9: {  // Simulação em um vértice
                int capacidade = read_positive_int("Capacidade do caminhão (litros): ");
                int inicio;
                do {
                    inicio = read_positive_int("Região inicial do fogo: ");
                    if (inicio < 0 || inicio >= graph->num_vertices)
                        printf("Índice inválido. Tente novamente.\n");
                } while (inicio < 0 || inicio >= graph->num_vertices);

                // zera flags de posto e sorteia sem colocar em 'inicio'
                for (int i = 0; i < graph->num_vertices; i++)
                    graph->regions[i]->is_brigade_post = 0;
                BrigadeSystem* bs9 = criar_brigade_system(graph, 3, 1, capacidade);
                distribuir_postos_brigadistas_exc(graph, bs9, graph->regions, graph->num_vertices, inicio);

                // executa simulação “completa”
                SimulationResult res9 = simular_propagacao_fogo(graph, bs9, inicio);
                printf("Tempo total: %d\n", res9.tempo_total);
                printf("Vértices salvos: %d\n", res9.vertices_salvos);
                printf("Água usada: %d\n", res9.agua_usada);
                imprimir_caminhos_percorridos(bs9);
                destruir_brigade_system(bs9);
                break;
            }

            case 10: { // Simulação em todos os vértices possíveis
                int capacidade = read_positive_int("Capacidade do caminhão (litros): ");
                int total_salvos = 0, total_queimados = 0;
                int total_tempo = 0, total_agua = 0, num_sim = 0;

                printf("=== RELATÓRIO CONSOLIDADO DE SIMULAÇÕES ===\n");
                for (int v = 0; v < graph->num_vertices; v++) {
                    // zera flags de posto e sorteia sem colocar em 'v'
                    for (int i = 0; i < graph->num_vertices; i++)
                        graph->regions[i]->is_brigade_post = 0;
                    BrigadeSystem* bs10 = criar_brigade_system(graph, 3, 1, capacidade);
                    distribuir_postos_brigadistas_exc(graph, bs10, graph->regions, graph->num_vertices, v);

                    SimulationResult res10 = simular_propagacao_fogo(graph, bs10, v);
                    printf("\nSimulação %d (fogo em %d): Tempo=%d, Salvos=%d, Água=%d\n",
                           v, v, res10.tempo_total, res10.vertices_salvos, res10.agua_usada);
                    imprimir_caminhos_percorridos(bs10);

                    total_salvos    += res10.vertices_salvos;
                    total_queimados += res10.vertices_queimados;
                    total_tempo     += res10.tempo_total;
                    total_agua      += res10.agua_usada;
                    num_sim++;
                    destruir_brigade_system(bs10);
                }

                printf("\n=== RESULTADOS GERAIS ===\n");
                printf("Média de vértices salvos: %.2f\n",
                       num_sim ? (float)total_salvos/num_sim : 0.0f);
                printf("Média de tempo: %.2f\n",
                       num_sim ? (float)total_tempo/num_sim : 0.0f);
                printf("Média de água usada: %.2f\n",
                       num_sim ? (float)total_agua/num_sim : 0.0f);
                break;
            }

            case 0:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    } while (opcao != 0);

    free_graph(graph);
    free_wildfire_list(wildfires);
    return 0;
}

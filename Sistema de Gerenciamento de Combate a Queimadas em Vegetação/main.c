#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "graph.h"
#include "region.h"
#include "wildfire_management.h"
#include "report.h"
#include "utils.h"

#define MAX_REGIOES 100

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
    printf("0. Sair\n");
    printf("Escolha uma opção: ");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

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
                int idx = read_positive_int("Índice da região (0 a N-1): ");
                if (idx < 0 || idx >= num_regioes) {
                    printf("Índice inválido.\n");
                    break;
                }
                char nome[MAX_NOME], tipo[MAX_TIPO];
                float area;
                read_string("Nome: ", nome, MAX_NOME);
                read_string("Tipo de vegetação: ", tipo, MAX_TIPO);
                area = read_positive_float("Área (ha): ");
                Region* reg = create_region(nome, tipo, area);
                set_region(graph, idx, reg);
                printf("Região cadastrada!\n");
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

#include <stdio.h>
#include "report.h"
#include "graph.h"
#include "region.h"
#include "wildfire_management.h"

// Relatório: lista todas as regiões cadastradas
void report_all_regions(const Graph* graph) {
    printf("=== Regiões Cadastradas ===\n");
    for (int i = 0; i < graph->num_vertices; i++) {
        printf("Região %d:\n", i);
        print_region(graph->regions[i]);
        printf("\n");
    }
}

// Relatório: lista todas as queimadas registradas
void report_all_wildfires(const WildfireList* list) {
    printf("=== Queimadas Registradas ===\n");
    print_wildfires(list);
}

// Relatório: mostra queimadas por região
void report_wildfires_by_region(const Graph* graph, const WildfireList* list, int region_index) {
    printf("=== Queimadas na Região %d (%s) ===\n", region_index, graph->regions[region_index]->nome);
    Wildfire* current = list->head;
    int found = 0;
    while (current) {
        if (current->region_index == region_index) {
            printf("Data: %s | Intensidade: %.2f\n", current->data, current->intensidade);
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        printf("Nenhuma queimada registrada nesta região.\n");
    }
}


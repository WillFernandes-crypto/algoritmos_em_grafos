#include <stdio.h>
#include <stdlib.h>
#include "wildfire_management.h"

// Cria um novo registro de queimada
Wildfire* create_wildfire(int region_index, const char* data, float intensidade) {
    Wildfire* wf = (Wildfire*)malloc(sizeof(Wildfire));
    if (!wf) return NULL;
    wf->region_index = region_index;
    strncpy(wf->data, data, MAX_DATA);
    wf->data[MAX_DATA-1] = '\0';
    wf->intensidade = intensidade;
    wf->next = NULL;
    return wf;
}

// Adiciona uma queimada à lista
void add_wildfire(WildfireList* list, Wildfire* wf) {
    if (!list || !wf) return;
    wf->next = list->head;
    list->head = wf;
}

// Inicializa a lista de queimadas
WildfireList* create_wildfire_list() {
    WildfireList* list = (WildfireList*)malloc(sizeof(WildfireList));
    if (!list) return NULL;
    list->head = NULL;
    return list;
}

// Libera memória da lista de queimadas
void free_wildfire_list(WildfireList* list) {
    if (!list) return;
    Wildfire* current = list->head;
    while (current) {
        Wildfire* temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
}

// Exibe todas as queimadas registradas
void print_wildfires(const WildfireList* list) {
    if (!list) return;
    Wildfire* current = list->head;
    while (current) {
        printf("Região: %d | Data: %s | Intensidade: %.2f\n", current->region_index, current->data, current->intensidade);
        current = current->next;
    }
}
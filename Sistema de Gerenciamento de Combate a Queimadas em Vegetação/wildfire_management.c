#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// Salva queimadas em arquivo binário
int save_wildfires(const char* filename, const WildfireList* list) {
    FILE* f = fopen(filename, "wb");
    if (!f) return 0;
    // Conta quantas queimadas existem
    int count = 0;
    Wildfire* curr = list->head;
    while (curr) {
        count++;
        curr = curr->next;
    }
    fwrite(&count, sizeof(int), 1, f);
    curr = list->head;
    while (curr) {
        fwrite(&curr->region_index, sizeof(int), 1, f);
        fwrite(curr->data, sizeof(char), MAX_DATA, f);
        fwrite(&curr->intensidade, sizeof(float), 1, f);
        curr = curr->next;
    }
    fclose(f);
    return 1;
}

// Carrega queimadas de arquivo binário
int load_wildfires(const char* filename, WildfireList* list) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    // limpa só os nós
    Wildfire* cur = list->head;
    while (cur) {
        Wildfire* tmp = cur;
        cur = cur->next;
        free(tmp);
    }
    list->head = NULL;
    int count = 0;
    fread(&count, sizeof(int), 1, f);
    for (int i = 0; i < count; i++) {
        int region_index;
        char data[MAX_DATA];
        float intensidade;
        fread(&region_index, sizeof(int), 1, f);
        fread(data, sizeof(char), MAX_DATA, f);
        fread(&intensidade, sizeof(float), 1, f);
        Wildfire* wf = create_wildfire(region_index, data, intensidade);
        add_wildfire(list, wf);
    }
    fclose(f);
    return 1;
}
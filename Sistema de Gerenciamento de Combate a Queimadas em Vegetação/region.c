#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "region.h"

// Cria uma nova região
Region* create_region(const char* nome, const char* tipo_vegetacao, float area) {
    Region* reg = (Region*)malloc(sizeof(Region));
    if (!reg) return NULL;
    strncpy(reg->nome, nome, MAX_NOME);
    reg->nome[MAX_NOME-1] = '\0';
    strncpy(reg->tipo_vegetacao, tipo_vegetacao, MAX_TIPO);
    reg->tipo_vegetacao[MAX_TIPO-1] = '\0';
    reg->area = area;
    return reg;
}

// Libera memória de uma região
void free_region(Region* reg) {
    if (reg) free(reg);
}

// Exibe informações da região
void print_region(const Region* reg) {
    if (!reg) return;
    printf("Nome: %s\n", reg->nome);
    printf("Tipo de Vegetação: %s\n", reg->tipo_vegetacao);
    printf("Área: %.2f hectares\n", reg->area);
}

// Salva as regiões em arquivo binário
int save_regions(const char* filename, Region** regions, int num_regions) {
    FILE* f = fopen(filename, "wb");
    if (!f) return 0;
    fwrite(&num_regions, sizeof(int), 1, f);
    for (int i = 0; i < num_regions; i++) {
        if (regions[i]) {
            fwrite(regions[i], sizeof(Region), 1, f);
        } else {
            Region empty = { "", "", 0.0f };
            fwrite(&empty, sizeof(Region), 1, f);
        }
    }
    fclose(f);
    return 1;
}

// Carrega as regiões de arquivo binário
int load_regions(const char* filename, Region** regions, int max_regions) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    int num_regions = 0;
    fread(&num_regions, sizeof(int), 1, f);
    if (num_regions > max_regions) num_regions = max_regions;
    for (int i = 0; i < num_regions; i++) {
        Region temp;
        fread(&temp, sizeof(Region), 1, f);
        if (regions[i]) free_region(regions[i]);
        regions[i] = create_region(temp.nome, temp.tipo_vegetacao, temp.area);
    }
    fclose(f);
    return num_regions;
}


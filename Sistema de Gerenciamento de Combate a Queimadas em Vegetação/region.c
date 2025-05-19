#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "region.h"

// Cria uma nova região
Region* create_region(const char* nome, const char* tipo_vegetacao, float area,
                     int is_brigade_post, int is_water_source,
                     int water_required, int teams_required) {
    Region* reg = malloc(sizeof(Region));
    if (!reg) return NULL;
    strncpy(reg->nome, nome, sizeof(reg->nome));
    strncpy(reg->tipo_vegetacao, tipo_vegetacao, sizeof(reg->tipo_vegetacao));
    reg->area = area;
    reg->is_brigade_post = is_brigade_post;
    reg->is_water_source = is_water_source;
    reg->water_required = water_required;
    reg->teams_required = teams_required;
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
    printf("Posto de Brigadistas: %s\n", reg->is_brigade_post ? "Sim" : "Não");
    printf("Ponto de Coleta de Água: %s\n", reg->is_water_source ? "Sim" : "Não");
    printf("Água necessária para apagar o fogo: %d litros\n", reg->water_required);
    printf("Equipes necessárias para apagar o fogo: %d\n", reg->teams_required);
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
            Region empty = { "", "", 0.0f, 0, 0, 0, 0 };
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
        regions[i] = create_region(
            temp.nome,
            temp.tipo_vegetacao,
            temp.area,
            temp.is_brigade_post,
            temp.is_water_source,
            temp.water_required,
            temp.teams_required
        );
    }
    fclose(f);
    return num_regions;
}


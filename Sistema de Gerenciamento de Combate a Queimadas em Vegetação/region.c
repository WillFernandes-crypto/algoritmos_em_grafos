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


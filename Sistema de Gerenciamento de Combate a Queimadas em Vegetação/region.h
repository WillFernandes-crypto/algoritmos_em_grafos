#ifndef REGION_H
#define REGION_H

#define MAX_NOME 100
#define MAX_TIPO 50

typedef struct {
    char nome[MAX_NOME];
    char tipo_vegetacao[MAX_TIPO];
    float area; // em hectares
} Region;

// Funções para manipulação de regiões
Region* create_region(const char* nome, const char* tipo_vegetacao, float area);
void free_region(Region* reg);
void print_region(const Region* reg);

// Funções de persistência
int save_regions(const char* filename, Region** regions, int num_regions);
int load_regions(const char* filename, Region** regions, int max_regions);

#endif // REGION_H


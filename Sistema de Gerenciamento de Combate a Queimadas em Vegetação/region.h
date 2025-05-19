#ifndef REGION_H
#define REGION_H

#define MAX_NOME 100
#define MAX_TIPO 50

typedef struct {
    char nome[MAX_NOME];
    char tipo_vegetacao[MAX_TIPO];
    float area; // em hectares

    // Novos campos para simulação
    int is_brigade_post;      // 1 se for posto de brigadistas, 0 caso contrário
    int is_water_source;      // 1 se for ponto de coleta de água, 0 caso contrário
    int water_required;       // quantidade de água necessária para apagar o fogo (litros)
    int teams_required;       // número de equipes necessárias para apagar o fogo
    int on_fire;              // 1 se está pegando fogo, 0 caso contrário
    int burned;               // 1 se já queimou, 0 caso contrário
} Region;

// Funções para manipulação de regiões
Region* create_region(const char* nome, const char* tipo_vegetacao, float area,
                      int is_brigade_post, int is_water_source,
                      int water_required, int teams_required);
void free_region(Region* reg);
void print_region(const Region* reg);

// Funções de persistência
int save_regions(const char* filename, Region** regions, int num_regions);
int load_regions(const char* filename, Region** regions, int max_regions);

#endif // REGION_H


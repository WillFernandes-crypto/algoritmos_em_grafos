#ifndef WILDFIRE_MANAGEMENT_H
#define WILDFIRE_MANAGEMENT_H

#define MAX_DATA 20

typedef struct Wildfire {
    int region_index;           // Índice da região afetada
    char data[MAX_DATA];        // Data do registro
    float intensidade;          // Intensidade da queimada
    struct Wildfire* next;      // Próximo registro
} Wildfire;

typedef struct {
    Wildfire* head;
} WildfireList;

// Funções para gerenciamento de queimadas
Wildfire* create_wildfire(int region_index, const char* data, float intensidade);
void add_wildfire(WildfireList* list, Wildfire* wf);
WildfireList* create_wildfire_list();
void free_wildfire_list(WildfireList* list);
void print_wildfires(const WildfireList* list);

#endif // WILDFIRE_MANAGEMENT_H


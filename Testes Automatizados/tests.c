#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Unity-master/src/unity.h"
#include "../Sistema de Gerenciamento de Combate a Queimadas em Vegetação\utils.h"
#include "../Sistema de Gerenciamento de Combate a Queimadas em Vegetação\graph.h"
#include "../Sistema de Gerenciamento de Combate a Queimadas em Vegetação\region.h"
#include "../Sistema de Gerenciamento de Combate a Queimadas em Vegetação\wildfire_management.h"

void setUp(void) {}
void tearDown(void) {}

void test_validate_date(void) {
    TEST_ASSERT_TRUE(validate_date("01/01/2020"));
    TEST_ASSERT_TRUE(validate_date("31/12/2023"));
    TEST_ASSERT_FALSE(validate_date("32/01/2020"));
    TEST_ASSERT_FALSE(validate_date("00/00/0000"));
    TEST_ASSERT_FALSE(validate_date("12-12-2020"));
    TEST_ASSERT_FALSE(validate_date("abc"));
}

void test_create_region(void) {
    Region* r = create_region("Região 1", "Cerrado", 100.0f);
    TEST_ASSERT_NOT_NULL(r);
    TEST_ASSERT_EQUAL_STRING("Região 1", r->nome);
    TEST_ASSERT_EQUAL_STRING("Cerrado", r->tipo_vegetacao);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, r->area);
    free_region(r);
}

void test_graph_edges(void) {
    Graph* g = create_graph(3);
    Region* r0 = create_region("A", "Cerrado", 10);
    Region* r1 = create_region("B", "Mata", 20);
    set_region(g, 0, r0);
    set_region(g, 1, r1);
    add_edge(g, 0, 1, 5.0f);

    // Verifica se a aresta existe em ambas as listas de adjacência
    EdgeNode* e = g->adj_list[0];
    int found = 0;
    while (e) {
        if (e->dest == 1 && e->weight == 5.0f) found = 1;
        e = e->next;
    }
    TEST_ASSERT_TRUE(found);

    e = g->adj_list[1];
    found = 0;
    while (e) {
        if (e->dest == 0 && e->weight == 5.0f) found = 1;
        e = e->next;
    }
    TEST_ASSERT_TRUE(found);

    free_graph(g);
}

void test_create_wildfire(void) {
    Wildfire* w = create_wildfire(2, "15/05/2024", 7.5f);
    TEST_ASSERT_NOT_NULL(w);
    TEST_ASSERT_EQUAL_INT(2, w->region_index);
    TEST_ASSERT_EQUAL_STRING("15/05/2024", w->data);
    TEST_ASSERT_EQUAL_FLOAT(7.5f, w->intensidade);
    free(w);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_validate_date);
    RUN_TEST(test_create_region);
    RUN_TEST(test_graph_edges);
    RUN_TEST(test_create_wildfire);
    return UNITY_END();
}


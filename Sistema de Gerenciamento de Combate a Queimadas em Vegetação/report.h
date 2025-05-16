#ifndef REPORT_H
#define REPORT_H

#include "graph.h"
#include "wildfire_management.h"

// Relatórios disponíveis
void report_all_regions(const Graph* graph);
void report_all_wildfires(const WildfireList* list);
void report_wildfires_by_region(const Graph* graph, const WildfireList* list, int region_index);

#endif // REPORT_H


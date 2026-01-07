#include<stdio.h>
#include<stdint.h>
#include "global_defines.h"
#include "table_import.h"
#include "player.h"
#define RANGE_BITE_SIZE 64

typedef struct{
    uint64_t *hands;
    int count;
    int capacity;
}Range;

typedef struct{
    Range *ranges;
    int no_players;
}Ranges;

typedef struct{
    const uint64_t *board_tables;
    const uint64_t *hole_tables;
}EquilatorTables;

EquilatorTables *initialize_equilator_tables();
void free_equilator_tables(EquilatorTables *t);
double *range_equilator(Ranges *ranges, EquilatorTables *eq_tables);
Ranges *create_ranges(int number_players);
void free_ranges(Ranges *r);
void add_hand_to_range(int hand_index, uint64_t *hole_tables, Ranges *r, int player_index);

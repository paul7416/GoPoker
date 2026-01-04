#include "range_equilator.h"

EquilatorTables *initialize_equilator_tables()
{
    EquilatorTables *t = malloc(sizeof(EquilatorTables));
    uint32_t i=0;
    t->board_tables = import_dat_file("./DataFiles/52C5.bin", &i, sizeof(uint64_t));
    t->hole_tables = import_dat_file("./DataFiles/52C2.bin", &i, sizeof(uint64_t));
    return t;
}
void free_equilator_tables(EquilatorTables *t)
{
    free(t->board_tables);
    free(t->hole_tables);
    free(t);
}
Ranges *create_ranges(int number_players)
{
    Ranges *r = calloc(1, sizeof(Ranges));
    r->no_players = number_players;
    r->ranges = calloc(number_players, sizeof(Range));
    return r;
}
void free_ranges(Ranges *r)
{
    for(int i = 0; i < r->no_players; i++)
    {
        free(r->ranges[i].hands);
    }
    free(r->ranges);
    free(r);
}

void add_hand_to_range(int hand_index, Ranges *r, int player_index)
{
    if(player_index >= r->no_players)
    {
        fprintf(stderr, "Error ranges set up for %d players but you attempted to edit index %d\n", r->no_players, player_index);
        return;
    }
    if(hand_index >= 1326)
    {
        fprintf(stderr, "Max hand index of 1325");
        return;
    }

    Range *range = &r->ranges[player_index];
    if (range->count >= range->capacity)
    {
        range->hands = realloc(range->hands, (RANGE_BITE_SIZE + range->capacity) * sizeof(uint64_t));
    }
    range->hands[range->count] = hole_tables[hand_index];
    range->count++;
    printf("%d\n", range->count);
}

double *range_equilator(Ranges *ranges, EquilatorTables *eq_tables)
{
    uint64_t hands[MAX_PLAYERS];
    for(int i = 0; i < ranges->no_players; i++)
    {
        printf("Player %d has %d hands in range\n", i, ranges->ranges->count);

    }

}

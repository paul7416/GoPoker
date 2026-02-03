#include "player.h"
#include"table_import.h"
//#include "debug.h"

void set_range_extent(Player *p, int16_t range_extent)
{
    p->range.range_extent = range_extent;
    for(int i = 0; i < 0x4000; i++)
    {
        p->range.playableHands[i] = (p->range.handRanks[i] <= range_extent);
    }
}
void set_folded(Player *p, int cards_index)
{
    p->folded = !p->range.playableHands[cards_index];
}


void increment_range_extent(Player *p, int16_t delta)
{
    Range range = p->range;
    int16_t new_range_extent = range.range_extent + delta;
    if (new_range_extent > 168) set_range_extent(p, 168);
    else if (new_range_extent < 0) set_range_extent(p, 0);
    else set_range_extent(p, new_range_extent);
}

bool get_hand_mask(Player *p, uint16_t hand_index)
{
    assert(hand_index < 0x4000);
    return p->range.playableHands[hand_index];
}

GameState *create_game_state(const uint32_t no_players, const int16_t *initial_range_extent, const double *stacks)
{
    GameState *G = calloc(1, sizeof(GameState));
    uint32_t count;
    G->handRanks = (uint8_t*)import_dat_file("DataFiles/holeCardHandRanks.dat", &count, sizeof(uint8_t));
    G->no_players = no_players;
    for(uint32_t i = 0; i < no_players; i++)
    {
        G->players[i].range.handRanks = G->handRanks;
        G->players[i].stack = stacks[i];
        G->players[i].range.playableHands = calloc(0x4000, sizeof(bool));
        G->players[i].index = (uint8_t)(i + 1);
        set_range_extent(&G->players[i], initial_range_extent[i]);
    }
    return G;
}
void free_game_state(GameState *G)
{
    free(G->bitMasks);
    free(G->handRanks);
    for(uint32_t i = 0; i < G->no_players; i++)
    {
        free(G->players[i].range.playableHands);
    }
    free(G);
}

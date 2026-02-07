
#ifndef PLAYER_H
#define PLAYER_H
#include<stdint.h>
#include<stdbool.h>
#include"global_defines.h"
#include<assert.h>

typedef struct
{
    uint8_t *handRanks;
    bool *playableHands;
    int16_t range_extent;
}Range;

typedef struct
{
    uint32_t stack;
    uint32_t bet;
    uint32_t remaining;
    uint8_t index;
    uint8_t rank;
    bool folded;
    Range range;
}Player;

typedef struct
{
    uint32_t no_players;
    Player players[MAX_PLAYERS];
    uint64_t *bitMasks;
    uint8_t *handRanks;
    uint32_t big_blind;
    uint32_t small_blind;
    float payouts[MAX_PLAYERS];
    uint8_t number_payouts;
}GameState;

void set_range_extent(Player *p, int16_t range_extent);
void increment_range_extent(Player *p, int16_t delta);
bool get_hand_mask(Player *p, uint16_t hand_index);
GameState *create_game_state(const uint32_t no_players, const int16_t *initial_range_extent, const uint32_t *stacks);
void set_folded(Player *p, int cards_index);
void free_game_state(GameState *G);
#endif

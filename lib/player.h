
#ifndef PLAYER_H
#define PLAYER_H
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include"global_defines.h"
#include<assert.h>

typedef struct
{
    uint64_t *bitMasks;
    uint8_t *handRanks;
    bool *playableHands;
    int16_t range_extent;
}Range;

typedef struct
{
    double stack;
    double bet;
    double remaining;
    double  push_regret;
    double  fold_regret;
    double ev;
    Range range;
    uint64_t hole_cards;
    uint8_t rank;
    uint8_t index;
    bool folded;
}Player;

typedef struct
{
    int no_players;
    Player players[MAX_PLAYERS];
    uint64_t community_cards;
    uint64_t *bitMasks;
    uint8_t *handRanks;
    size_t iterations;
    uint32_t big_blind;
    uint32_t small_blind;
    double *payouts;
    uint8_t active_count;
    uint8_t last_active;
}GameState;

void set_range_extent(Player *p, int16_t range_extent);
void increment_range_extent(Player *p, int16_t delta);
bool get_hand_mask(Player *p, uint16_t hand_index);
GameState *create_game_state(int no_players, uint16_t *initial_range_extent, double *stacks);
void set_folded(Player *p, int cards_index);
#endif

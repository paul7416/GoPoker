#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include"global_defines.h"
#include<assert.h>
#include"table_import.h"

typedef struct
{
    int no_players;
    Player *players;
    uint64_t *bitMasks;
    uint8_t *handRanks;
}Players;

typedef struct
{
    uint32_t stack;
    int64_t push_regret;
    int64_t fold_regret;
    double ev;
    Range range;
    uint8_t position;
}Player;

typedef struct
{
    uint64_t *bitMasks;
    uint8_t *handRanks;
    bool playableHands[1326];
    int16_t range_extent;
}Range;

void set_range_extent(Player *p, int16_t range_extent);
void increment_range_extent(Player *p, int16_t delta);
bool get_hand_mask(Player *p, uint16_t hand_index);


#ifndef EVALUATOR_H
#define EVALUATOR_H
#include <stdint.h>
#include <stdbool.h>
#include "global_defines.h"
#include "player.h"

#define HASH_TABLE_SIZE 0x20000
#define ARRAY_SIZE 0x2000
extern uint32_t probe_histogram[50];

typedef struct {
    uint64_t hole_cards;
    bool *playableHands;  // pointer to full Range data
    uint8_t index;
    bool folded;
    uint16_t padding1;
    uint32_t padding2;
} PlayerSim;

typedef struct {
    uint64_t community_cards;
    uint8_t no_players;
    uint8_t active_count;
    PlayerSim players[MAX_PLAYERS];
    uint8_t last_active;
} GameStateSim;

typedef struct{
    const uint16_t *Flushes;
    const uint64_t *Primes;
    const uint64_t *hashTable;
}evaluatorTables;


typedef struct{
    uint16_t score;
    uint8_t index;
    bool folded;
    uint8_t player_rank;
    uint8_t tied;
    uint16_t padding;
}playerResult;


const evaluatorTables *import_evaluator_tables();
uint16_t evaluateHand(const uint64_t bitMask, const uint16_t *Flushes, const uint64_t *Primes, const uint64_t *hashTable);
void free_evaluator_tables(const evaluatorTables *tables);
uint64_t evaluateRound(GameStateSim *G, const evaluatorTables *tables);
int decodeOutcomes(uint64_t code, playerResult results[MAX_PLAYERS]);
#endif

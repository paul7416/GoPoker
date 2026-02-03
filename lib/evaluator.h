#ifndef EVALUATOR_H
#define EVALUATOR_H
#include <stdint.h>
#include <stdbool.h>
#include "global_defines.h"
#include "player.h"

#define HASH_TABLE_SIZE 0x4000
#define DIRECT_LOOKUP_SIZE 0x80000
#define ARRAY_SIZE 0x2000

typedef struct {
    uint8_t hole_cards[2];
    uint8_t index;
    bool folded;
    uint32_t padding;
} PlayerSim;

typedef struct {
    uint64_t community_cards;
    uint8_t no_players;
    PlayerSim players[MAX_PLAYERS];
} GameStateSim;

typedef struct{
    const uint16_t *Flushes;
    uint32_t *Primes;
    const uint64_t *hashTable;
    const uint16_t *directLookup;
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
uint16_t evaluateHand(const uint64_t bitMask, const uint16_t *Flushes, const uint32_t *Primes, const uint64_t *hashTable, const uint16_t *directLookup);
void free_evaluator_tables(const evaluatorTables *tables);
uint64_t evaluateRound(GameStateSim *G, const evaluatorTables *tables);
int decodeOutcomes(const uint64_t code, playerResult *results);
#endif

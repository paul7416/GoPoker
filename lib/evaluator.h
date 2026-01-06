#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdbool.h>
#include"global_defines.h"

#define HASH_TABLE_SIZE 0x80000
#define ARRAY_SIZE 0x2000

typedef struct{
    uint16_t *Flushes;
    uint64_t *Primes;
    uint64_t *hashTable;
}evaluatorTables;

typedef struct{
    uint16_t score;
    uint8_t player_id;
    bool folded;
    bool tied;
    uint8_t player_rank;
}playerResult;

evaluatorTables *import_evaluator_tables();
uint16_t evaluateHand(uint64_t bitMask, uint16_t *Flushes, uint64_t *Primes, uint64_t *hashTable);
void free_evaluator_tables(evaluatorTables *tables);
uint64_t evaluateRound(uint64_t board, uint64_t *hole_cards, bool *folded, uint8_t *player_ids, int no_players, evaluatorTables *tables);
int decodeOutcomes(uint64_t code, playerResult results[MAX_PLAYERS]);

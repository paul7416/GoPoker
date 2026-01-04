#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<stdbool.h>

#define HASH_TABLE_SIZE 0x80000
#define ARRAY_SIZE 0x2000
#define MAX_PLAYERS 16

typedef struct{
    uint16_t *Flushes;
    uint64_t *Primes;
    uint64_t *hashTable;
}evaluatorTables;

typedef struct{
    uint16_t score;
    uint8_t player_id;
    bool folded;
}playerResult;

evaluatorTables *import_evaluator_tables();
uint16_t evaluateHand(uint64_t bitMask, uint16_t *Flushes, uint64_t *Primes, uint64_t *hashTable);
void free_evaluator_tables(evaluatorTables *tables);
uint64_t evaluateRound(uint64_t board, uint64_t *hole_cards, uint32_t *bets, uint8_t *player_ids, int no_players, evaluatorTables *tables, char rankString[MAX_PLAYERS]);

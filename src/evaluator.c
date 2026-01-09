/*
 * evaluator.c - Fast poker hand evaluator using lookup tables
 * 
 * Uses prime number hashing for non-flush hands and direct lookup for flushes.
 * Cards are represented as bits in a 52-bit mask (13 bits per suit).
 */
#include "table_import.h"
#include "evaluator.h"
#include <string.h>
#include "debug.h"

/*
 * Build hash table for prime->rank lookup.
 * Entry format: upper 48 bits = prime key, lower 16 bits = hand rank.
 * Uses linear probing for collision resolution.
 */
uint64_t *import_primes_dict(){
    uint32_t count;
    uint64_t *buffer = import_dat_file("./DataFiles/seven_rank_dict.bin", &count, sizeof(uint64_t));
    if(!buffer)
    {
        fprintf(stderr, "Failed to load seven_rank_dict.bin\n");
        return NULL;
    }
    uint64_t *hash_table = aligned_alloc(64, HASH_TABLE_SIZE * sizeof(uint64_t));
    if(!hash_table)
    {
        printf("hash table failed to allocate\n");
        exit(1);
    }
    memset(hash_table, 0, HASH_TABLE_SIZE * sizeof(uint64_t));

    /* Insert entries with linear probing */
    for(uint32_t i = 0; i < count; i++)
    {
        int index = (buffer[i] >> SCORE_BITS) & (HASH_TABLE_SIZE - 1);
        while(hash_table[index] != 0)
        {
            index = (index + 1)& (HASH_TABLE_SIZE - 1);
        }
        hash_table[index] = buffer[i];
    }
    free(buffer);
    return hash_table;
}
/* Load all lookup tables needed by the evaluator */
const evaluatorTables *import_evaluator_tables()
{
    evaluatorTables *eval_tables = malloc(sizeof(evaluatorTables));
    uint32_t count = 0x2000;
    eval_tables->Flushes = (uint16_t*)import_dat_file("./DataFiles/flush_ranks.bin", &count, sizeof(uint16_t));
    eval_tables->Primes = (uint64_t*)import_dat_file("./DataFiles/primes.bin", &count,  sizeof(uint64_t));
    eval_tables->hashTable = import_primes_dict();
    printf("Table importing completed\n");
    return eval_tables;
}

void free_evaluator_tables(const evaluatorTables *tables)
{
    free((void*)tables->Flushes);
    free((void*)tables->Primes);
    free((void*)tables->hashTable);
    free((void*)tables);
}

/* Look up hand rank by prime product using linear probing */
uint16_t hashLookup(uint64_t prime, const uint64_t *hash_table)
{
    int index = prime & (HASH_TABLE_SIZE - 1);
    #ifdef DEBUG
    int probes = 0;
    #endif
    while((hash_table[index] >> SCORE_BITS) != prime)
    {
        index = (index + 1) & (HASH_TABLE_SIZE - 1);
        #ifdef DEBUG
        if(++probes > HASH_TABLE_SIZE) {
            fprintf(stderr, "hashLookup: prime %lu not found\n", prime);
            abort();
        }
        #endif
    }
    return (hash_table[index] & HIGHEST_SCORE);
}

/*
 * Evaluate a 7-card hand represented as a 52-bit mask.
 * Bit layout: bits 0-12 = hearts, 13-25 = diamonds, 26-38 = clubs, 39-51 = spades
 * Returns hand rank (lower = better).
 */
uint16_t evaluateHand(const uint64_t bitMask, const uint16_t *Flushes, const uint64_t *Primes, const uint64_t *hashTable)
{
    /* Extract 13-bit suit masks --- definitions in global_defines.h*/
    uint16_t heartsMask   = GET_HEARTS_MASK(bitMask);
    uint16_t diamondsMask = GET_DIAMONDS_MASK(bitMask);
    uint16_t clubsMask    = GET_CLUBS_MASK(bitMask);
    uint16_t spadesMask   = GET_SPADES_MASK(bitMask);
    
    /* Check for flush - at most one suit can have a valid flush */
    uint16_t flushScore = 
            Flushes[heartsMask] + 
            Flushes[diamondsMask] + 
            Flushes[clubsMask] + 
            Flushes[spadesMask];
    if (flushScore != 0) return flushScore;

    /* Non-flush: multiply primes and look up rank */
    uint64_t prime = Primes[heartsMask] * Primes[diamondsMask] * Primes[clubsMask] * Primes[spadesMask];
    return hashLookup(prime, hashTable);
}


int cmp_playerResult(const void *a, const void *b) {
    playerResult *x = (playerResult*)a;
    playerResult *y = (playerResult*)b;
    return (x->score > y->score) - (x->score < y->score);
}
int getTies(playerResult results[MAX_PLAYERS], int rank)
{
    int count = 0;
    for(int i = 0;i < MAX_PLAYERS && !results[i].folded; i++)
    {
        count += (int)(results[i].player_rank == rank);
    }
    return count;
}
int decodeOutcomes(uint64_t code, playerResult results[MAX_PLAYERS])
{
    int i;
    for(i = 0; i < MAX_PLAYERS && code != 0; i++)
    {
        uint64_t player_bits = code & 0x3f;
        results[i].index = (uint8_t)(player_bits & 0xf);
        results[i].tied = (uint8_t)((player_bits & 0x10)!=0);
        results[i].folded = (bool)(player_bits & 0x20);
        if(i > 0 && results[i].tied)
        {
            results[i].player_rank = results[i - 1].player_rank;
        }
        else results[i].player_rank = i;
        code = code >> 6;
    }
    for(int j = 0;j < MAX_PLAYERS && !results[j].folded; j++)
    {
        results[j].tied = getTies(results, results[j].player_rank);
    }

    return i;
}
uint64_t evaluateRound(GameStateSim *G, const evaluatorTables *tables)
{
    if(G->active_count == 0)
    {
        return 0; // manually encode for when they all fold
    }
    if(G->active_count == 1)
    {
        return (uint64_t)G->last_active;
    }
    playerResult scores[MAX_PLAYERS];
    const uint64_t *hashTable = tables->hashTable;
    const uint64_t *Primes = tables->Primes;
    const uint16_t *Flushes = tables->Flushes;

    
    for(int i = 0; i < G->no_players; i++)
    {
        playerResult result;
        result.index = G->players[i].index;
        result.tied = false;
        
        if(G->players[i].folded)
        {
            result.score = HIGHEST_SCORE;
            result.folded = true;
        }
        else
        {
            result.score = evaluateHand(G->players[i].hole_cards|G->community_cards, Flushes, Primes, hashTable);
            result.folded = false;
        }
        
        // Insert in sorted position
        int j = i;
        while (j > 0 && scores[j-1].score > result.score)
        {
            scores[j] = scores[j-1];
            j--;
        }
        scores[j] = result;
    }
    
    uint64_t key = 0;
    for (int i = 0; i < G->no_players; i++)
    {
        uint64_t player_bits = scores[i].index;
        if (i > 0 && scores[i].score == scores[i-1].score)
        {
            player_bits |= 1 << 4;  // tied
        }
        player_bits |= scores[i].folded << 5;
        key |= player_bits << (i * 6);
    }
    return key;
}

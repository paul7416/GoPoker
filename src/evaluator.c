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
uint32_t evaluatehand_calls = 0;

/*
 * Build hash table for prime->rank lookup.
 * Entry format: upper 48 bits = prime key, lower 16 bits = hand rank.
 * Uses linear probing for collision resolution.
 */


/* Load all lookup tables needed by the evaluator */
const evaluatorTables *import_evaluator_tables()
{
    evaluatorTables *eval_tables = malloc(sizeof(evaluatorTables));
    uint32_t count = 0x2000;
    eval_tables->Flushes = (uint16_t*)import_dat_file("./DataFiles/flush_ranks.bin", &count, sizeof(uint16_t));
    uint64_t *tmp = (uint64_t*)import_dat_file("./DataFiles/primes.bin", &count,  sizeof(uint64_t));
    eval_tables->Primes = (uint32_t*)malloc(0x2000 * sizeof(uint32_t));
    for(int i = 0; i < 0x2000; i++)
    {
       eval_tables->Primes[i] = (uint32_t)tmp[i];
    }
    count = HASH_TABLE_SIZE;
    free(tmp);
    eval_tables->perfectHash = (uint16_t*)import_dat_file("./DataFiles/PerfectMap.bin", &count, sizeof(uint16_t));
    count = SEED_TABLE_SIZE;
    eval_tables->seeds = (uint32_t*)import_dat_file("./DataFiles/seeds.bin", &count, sizeof(uint32_t));
    printf("Table importing completed\n");
    return eval_tables;
}

void free_evaluator_tables(const evaluatorTables *tables)
{
    free((void*)tables->Flushes);
    free((void*)tables->Primes);
    free((void*)tables->perfectHash);
    free((void*)tables->seeds);
    free((void*)tables);
}

static inline uint16_t hashLookup(uint32_t prime, const uint32_t *seeds, const uint16_t *perfectHash)
{
    uint32_t seed = seeds[prime &  (SEED_TABLE_SIZE - 1)]; // Get seed
    uint32_t h = prime ^ seed; // apply seed
    h *= 0xc2b2ae35; //randomising prime number
    h ^= h >> 17; // shift higher order bits down
    uint32_t ht_index = h & (HASH_TABLE_SIZE - 1);
    return perfectHash[ht_index];
}

uint16_t evaluateHand(const uint64_t bitMask, const uint16_t *Flushes, const uint32_t *Primes, const uint32_t *seeds, const uint16_t *perfectHash)
{
    /* Extract 13-bit suit masks */
    uint16_t suitMasks[4];
    memcpy(&suitMasks, &bitMask, sizeof(bitMask));
    const uint16_t heartsMask   = suitMasks[0];
    const uint16_t diamondsMask = suitMasks[1];
    const uint16_t clubsMask    = suitMasks[2];
    const uint16_t spadesMask   = suitMasks[3];

    
    /* Check for flush - at most one suit can have a valid flush */
    int flushScore = 
            Flushes[heartsMask] + 
            Flushes[diamondsMask] + 
            Flushes[clubsMask] + 
            Flushes[spadesMask];
    if (flushScore != 0) return (uint16_t)flushScore;

    /* Non-flush: multiply primes and look up rank */
    uint32_t prime = (Primes[heartsMask] * Primes[diamondsMask] * Primes[clubsMask] * Primes[spadesMask]);
    return hashLookup(prime, seeds, perfectHash);
}

uint8_t getTies(playerResult results[MAX_PLAYERS], int rank)
{
    uint8_t count = 0;
    for(int i = 0;i < MAX_PLAYERS; i++)
    {
        if(results[i].player_rank == rank) count++;
    }
    return count;
}
int decodeFixedOutcome(uint64_t code, playerResult *results)
{
    results[0].index = (uint8_t)code - 1;
    results[0].tied = 1;
    results[0].folded = 0;
    results[0].player_rank = 0;
    uint8_t player_index = 1;
    for(uint64_t i = 1; i <= MAX_PLAYERS; i++)
    {
        if(i != code)
        {
            results[player_index].index = (uint8_t)i - 1;
            results[player_index].folded = true;
            results[player_index].player_rank = (uint8_t)i;
            results[player_index].tied = 1;
            player_index++;
        }
    }
    return 0;
}
int decodeOutcomes(uint64_t code, playerResult *results)
{
    memset(results,0xff, sizeof(playerResult) * MAX_PLAYERS);
    if((code >> 6) == 0)return decodeFixedOutcome(code, results);
    int i;
    for(i = 0; i < MAX_PLAYERS && code != 0; i++)
    {
        uint64_t player_bits = code & 0x3f;
        results[i].index = (uint8_t)(player_bits & 0xf) - 1;
        results[i].tied = (uint8_t)((player_bits & 0x10)!=0);
        results[i].folded = (bool)(player_bits & 0x20);
        if(i > 0 && results[i].tied)
        {
            results[i].player_rank = results[i - 1].player_rank;
        }
        else results[i].player_rank = (uint8_t)i;
        code = code >> 6;
    }
    for(int j = 0;j < MAX_PLAYERS && !results[j].folded; j++)
    {
        results[j].tied = getTies(results, results[j].player_rank);
    }

    return i;
}
void print_outcome(uint64_t outcome)
{
    playerResult results[MAX_PLAYERS] = {0};
    decodeOutcomes(outcome, results);
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        playerResult p = results[i];
        printf("Index: %3d Folded: %d rank: %d tied: %d\n", p.index, p.folded, p.player_rank, p.tied);
    }

}
uint64_t evaluateRound(GameStateSim *G, const evaluatorTables *tables)
{
    playerResult scores[MAX_PLAYERS];
    const uint16_t *perfectHash = tables->perfectHash;
    const uint32_t *Primes = tables->Primes;
    const uint16_t *Flushes = tables->Flushes;
    const uint32_t *seeds = tables->seeds;
    uint64_t player_mask;
    uint8_t *hole_cards;
    uint64_t community_cards = G->community_cards;

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
            hole_cards = G->players[i].hole_cards;
            player_mask = (1ull << hole_cards[0])| (1ull << hole_cards[1]);
            result.score = evaluateHand(player_mask|community_cards, Flushes, Primes, seeds, perfectHash);
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

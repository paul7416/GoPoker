/*
 * evaluator.c - Fast poker hand evaluator using lookup tables
 * 
 * Uses prime number hashing for non-flush hands and direct lookup for flushes.
 * Cards are represented as bits in a 52-bit mask (13 bits per suit).
 */
#include "table_import.h"
#include "evaluator.h"

/* Load flush rank lookup table (13-bit mask -> hand rank or 0) */
uint16_t *import_flushes()
{
    FILE *f = fopen("./DataFiles/flush_ranks.bin", "rb");
    if(f == NULL)
    {
        printf("Flush import failed\n");
        return NULL;
    }

    uint16_t *Flushes = aligned_alloc(64, sizeof(uint16_t) * ARRAY_SIZE);
    if(Flushes == NULL)
    {
        printf("Failed to allocate Flushes Memory\n");
        fclose(f);
        return NULL;
    }
    size_t file_length = fread(Flushes, sizeof(uint16_t), ARRAY_SIZE, f);
    if (file_length != ARRAY_SIZE)
    {
        printf("Primes data file incorrect size expected %d got %ld\n", ARRAY_SIZE, file_length);
    }
    fclose(f);
    return Flushes;
}

/* Load prime product table (13-bit mask -> prime product for ranks) */
uint64_t *import_primes()
{
    FILE *f = fopen("./DataFiles/primes.bin", "rb");
    if(f == NULL)
    {
        printf("Primes import failed\n");
        return NULL;
    }
    uint64_t *Primes = aligned_alloc(64, sizeof(uint64_t) * ARRAY_SIZE);
    if(Primes == NULL)
    {
        printf("primes import failed\n");
        fclose(f);
        return NULL;
    }
    size_t file_length = fread(Primes, sizeof(uint64_t), ARRAY_SIZE, f);
    if (file_length != ARRAY_SIZE)
    {
        printf("Primes data file incorrect size expected %d got %ld\n", ARRAY_SIZE, file_length);
    }
    fclose(f);
    return Primes;
}


/*
 * Build hash table for prime->rank lookup.
 * Entry format: upper 48 bits = prime key, lower 16 bits = hand rank.
 * Uses linear probing for collision resolution.
 */
uint64_t *import_primes_dict(){
    uint32_t count;
    uint64_t *buffer = import_dat_file("./DataFiles/seven_rank_dict.bin", &count, sizeof(uint64_t));
    uint64_t *hash_table = aligned_alloc(64, HASH_TABLE_SIZE * sizeof(uint64_t));
    if(!hash_table)
    {
        printf("hash table failed to allocate\n");
        exit(1);
    }
    if (hash_table) memset(hash_table, 0, HASH_TABLE_SIZE * sizeof(uint64_t));

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
evaluatorTables *import_evaluator_tables()
{
    evaluatorTables *eval_tables = malloc(sizeof(evaluatorTables));
    eval_tables->Flushes = import_flushes();
    printf("Imported Flushes\n");
    eval_tables->Primes = import_primes();
    printf("Imported Primes\n");
    eval_tables->hashTable = import_primes_dict();
    printf("Imported hashTable\n");
    printf("Table importing completed\n");
    return eval_tables;
}

void free_evaluator_tables(evaluatorTables *tables)
{
    free(tables->Flushes);
    free(tables->Primes);
    free(tables->hashTable);
    free(tables);
}

/* Debug: print 52-bit card mask as binary, grouped by suit */
void print_bin(uint64_t mask)
{
    for(int i = 51; i >= 0; i--)
    {
        printf("%ld",(mask >> i)&1);
        if (i%13 == 0)printf("|");
    }
}

/* Look up hand rank by prime product using linear probing */
uint16_t hashLookup(uint64_t prime, uint64_t *hash_table)
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
uint16_t evaluateHand(uint64_t bitMask, uint16_t *Flushes, uint64_t *Primes, uint64_t *hashTable)
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
uint16_t evaluateHandNoFlush(uint64_t bitMask, uint64_t *Primes, uint64_t *hashTable)
{
    /* Extract 13-bit suit masks --- definitions in global_defines.h*/
    uint16_t heartsMask   = GET_HEARTS_MASK(bitMask);
    uint16_t diamondsMask = GET_DIAMONDS_MASK(bitMask);
    uint16_t clubsMask    = GET_CLUBS_MASK(bitMask);
    uint16_t spadesMask   = GET_SPADES_MASK(bitMask);
    
    /* Non-flush: multiply primes and look up rank */
    uint64_t prime = Primes[heartsMask] * Primes[diamondsMask] * Primes[clubsMask] * Primes[spadesMask];
    return hashLookup(prime, hashTable);
}

int cmp_playerResult(const void *a, const void *b) {
    playerResult x = *(playerResult*)a;
    playerResult y = *(playerResult*)b;
    return (x.score > y.score) - (x.score < y.score);
}
uint64_t encodeOutcomes(playerResult results[MAX_PLAYERS], int no_players)
{
    qsort(results, no_players, sizeof(playerResult), cmp_playerResult);
    uint16_t previous_score = HIGHEST_SCORE;
    uint64_t key = 0;
    for (int i = 0; i < no_players; i++) {
        uint64_t player_bits = results[i].player_id & 0xF;
        if (i >= 1 && results[i].score == previous_score) {
            player_bits |= 0x10;  // tie bit
            key |= (0x10ULL << ((i-1) * 6));  // set previous tie bit too
        }
        if (results[i].folded) player_bits |= 0x20;
        key |= (player_bits << (i * 6));
        previous_score = results[i].score;
    }
    return key;
}
int decodeOutcomes(uint64_t code, playerResult results[MAX_PLAYERS])
{
    int i;
    for(i = 0; i < MAX_PLAYERS && code != 0; i++)
    {
        uint64_t player_bits = code & 0x3f;
        results[i].player_id = (uint8_t)((player_bits)&0xf);
        results[i].tied = (bool)((player_bits >> 4) & 1);
        results[i].folded = (bool)(player_bits & 0x20);
        results[i].player_rank = (uint8_t)i;
        code = code >> 6;
    }
    return i;
}

uint64_t evaluateRound(uint64_t board, uint64_t *hole_cards, uint32_t *bets, uint8_t *player_ids, int no_players, evaluatorTables *tables)
{
    playerResult scores[MAX_PLAYERS] = {0};
    uint64_t *hashTable = tables->hashTable;
    uint64_t *Primes = tables->Primes;
    if(board & NO_FLUSH_BIT)
    {
        for(int i = 0; i < no_players; i++)
        {
            scores[i].player_id = player_ids[i];
            if(bets[i] == 0)
            {
                scores[i].score = HIGHEST_SCORE;
                scores[i].folded = true;
                continue;
            }
            scores[i].score = evaluateHandNoFlush(hole_cards[i]|board, Primes, hashTable);
        }
    }
    else
    {
        uint16_t *Flushes = tables->Flushes;
        for(int i = 0; i < no_players; i++)
        {
            scores[i].player_id = player_ids[i];
            if(bets[i] == 0)
            {
                scores[i].score = HIGHEST_SCORE;
                scores[i].folded = true;
                continue;
            }
            scores[i].score = evaluateHand(hole_cards[i]|board, Flushes, Primes, hashTable);
        }
    }
    return encodeOutcomes(scores, no_players);
}


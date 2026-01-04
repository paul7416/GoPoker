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
    fread(Flushes, sizeof(uint16_t), ARRAY_SIZE, f);
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
    fread(Primes, sizeof(uint64_t), ARRAY_SIZE, f);
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
        int index = (buffer[i] >> 16) & (HASH_TABLE_SIZE - 1);
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
    while((hash_table[index] >> 16) != prime)
    {
        index = (index + 1) & (HASH_TABLE_SIZE - 1);
        #ifdef DEBUG
        if(++probes > HASH_TABLE_SIZE) {
            fprintf(stderr, "hashLookup: prime %lu not found\n", prime);
            abort();
        }
        #endif
    }
    return (hash_table[index] & 0xffff);
}
uint16_t evaluateKnownDrawHand(uint64_t bitMask, uint16_t *Flushes, uint64_t *Primes, uint64_t *hashTable, int flushSuit)
{
    /* Check for flush - at most one suit can have a valid flush */
    if(flushSuit)
    {
        uint16_t flushScore = Flushes[(bitMask >> (flushSuit >> 1)) & 0x1fff];
        if (flushScore != 0) return flushScore;
    }

    /* Extract 13-bit suit masks */
    uint16_t heartsMask = (bitMask & 0x1fff);
    uint16_t diamondsMask = (bitMask >> 13) & 0x1fff;
    uint16_t clubsMask = (bitMask >> 26) & 0x1fff;
    uint16_t spadesMask = (bitMask >> 39) & 0x1fff;
    

    /* Non-flush: multiply primes and look up rank */
    uint64_t prime = Primes[heartsMask] * Primes[diamondsMask] * Primes[clubsMask] * Primes[spadesMask];
    return hashLookup(prime, hashTable);
}

/*
 * Evaluate a 7-card hand represented as a 52-bit mask.
 * Bit layout: bits 0-12 = hearts, 13-25 = diamonds, 26-38 = clubs, 39-51 = spades
 * Returns hand rank (lower = better).
 */
uint16_t evaluateHand(uint64_t bitMask, uint16_t *Flushes, uint64_t *Primes, uint64_t *hashTable)
{
    /* Extract 13-bit suit masks */
    uint16_t heartsMask = (bitMask & 0x1fff);
    uint16_t diamondsMask = (bitMask >> 13) & 0x1fff;
    uint16_t clubsMask = (bitMask >> 26) & 0x1fff;
    uint16_t spadesMask = (bitMask >> 39) & 0x1fff;
    
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
    playerResult x = *(playerResult*)a;
    playerResult y = *(playerResult*)b;
    return (x.score > y.score) - (x.score < y.score);
}

uint64_t evaluateRound(uint64_t board, uint64_t *hole_cards, uint32_t *bets, uint8_t *player_ids, int no_players, evaluatorTables *tables, char rankString[MAX_PLAYERS])
{
    playerResult scores[MAX_PLAYERS] = {0};
    uint64_t *hashTable = tables->hashTable;
    uint64_t *Primes = tables->Primes;;
    uint16_t *Flushes = tables->Flushes;
    for(int i = 0; i < no_players; i++)
    {
        scores[i].player_id = player_ids[i];
        if(bets[i] == 0)
        {
            scores[i].score = 0xffff;
            scores[i].folded = true;
            continue;
        }
        scores[i].score = evaluateHand(hole_cards[i]|board, Flushes, Primes, hashTable);
    }
    qsort(scores, no_players, sizeof(playerResult), cmp_playerResult);
    uint16_t previous_score = 0xffff;
    for(int i = 0; i < no_players; i++)
    {
        rankString[i] = scores[i].player_id << 1;
        if(i >=1 && scores[i].score == previous_score)
        {
            rankString[i] |= 1;
            rankString[i - 1] |= 1;
        }
        if(scores[i].folded) rankString[i] |= (0x80);
        previous_score = scores[i].score;
    }
    return 0;
}


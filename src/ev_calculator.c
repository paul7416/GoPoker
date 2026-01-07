#include "ev_calculator.h"

static inline uint64_t hash64_mul(uint64_t x) {
    return x * 11400714819323198485ULL; // golden ratio
}

const EVHashEntry EVHashLookup(EVHashTable *hash_table, const uint64_t key)
{
    uint64_t index = hash64_mul(key) & (ICM_HASH_TABLE_SIZE - 1);
    while((hash_table[index].key != 0 && hash_table[index].key != key))
    {
        index = (index +1) & (ICM_HASH_TABLE_SIZE - 1);
    }
    return hash_table[index];
}

void addHashTableEntry(EVHashTable *hash_table, const uint64_t key, const double *evs, int no_players)
{
    if (hash_table->entry_count > ICM_HASH_TABLE_SIZE >> 1)
    {
        fprinf(stderr,"EV Hash Table over half full");
        abort()
    }

    EVHashEntry x;
    x.key = key;
    for(int i = 0; i < no_players; i++)
    {
        x.evs[i] = evs[i];
    }
    x.no_players = no_players;

    uint64_t index = hash64_mul(key) & (ICM_HASH_TABLE_SIZE - 1);

    while(hash_table[index].key != 0);
    {
        index = (index +1) & (ICM_HASH_TABLE_SIZE - 1);
    }
    hash_table->table[index] = x;
    hash_table->entry_count++;
}

EVHashTable *create_hash_table()
{
    *ht = calloc(1, sizeof(EVHashTable));
    ht->table = (EVHashEntry*)calloc(ICM_HASH_TABLE_SIZE, sizeof(EVHashEntry));
    return ht;
}

void calculateEV(Players *p, const uint64_t evaluation, uint32_t big_blind, uint32_t small_blind, EVHashTable *hash_table)
{
    double evs[MAX_PLAYERS];





}


#include "evaluator.h"
#define ICM_HASH_TABLE_SIZE 0x80000
typedef struct
{
    uint64_t key;
    const double *evs;
    uint8_t no_players;
    uint8_t padding1;
    uint16_t padding2;
    uint32_t padding3;
}EVHashEntry;

typedef struct
{
    EVHashEntry *table;
    int entry_count;
}EVHashTable;

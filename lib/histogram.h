#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include<stdint.h>
#include<stdlib.h>

#define HISTOGRAM_START_SIZE   0x10000
#define HISTOGRAM_MAX_SIZE     0x2000000

typedef struct
{
    uint64_t key;
    uint32_t count; 
    uint32_t flags;
}HistogramEntry;

typedef struct
{
    HistogramEntry *table;
    size_t entry_count;
    size_t capacity;
}HistogramTable;

HistogramTable *create_histogram_table(size_t hash_table_size);
void iterateHistogram(HistogramTable *hash_table, uint64_t key);
#endif

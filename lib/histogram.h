#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include<stdint.h>
#include<stdlib.h>

#define HISTOGRAM_START_SIZE   0x800
#define HISTOGRAM_MAX_SIZE     0x1000000
#define HISTOGRAM_MAX_LOAD_FACTOR 0.75

typedef struct
{
    uint64_t key;
    uint32_t count; 
    uint32_t ev_list_index:31;
    uint32_t ev_cached:1;
}HistogramEntry;

typedef struct
{
    HistogramEntry *table;
    size_t entry_count;
    size_t capacity;
    size_t max_entries;
}HistogramTable;

void free_histogram_table(HistogramTable *ht);
HistogramTable *create_histogram_table(size_t hash_table_size);
void iterateHistogram(HistogramTable *hash_table, const uint64_t key);
void cumulateHistogram(HistogramTable *hash_table, const uint64_t key, const uint32_t count);
void merge_histogram(HistogramTable *destination, const HistogramTable *source);
#endif

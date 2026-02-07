#include "histogram.h"
#include<stdio.h>
#include "debug.h"
static inline uint64_t hash64_mul(uint64_t x) {
    return x * 11400714819323198485ULL; // golden ratio
}

HistogramTable *create_histogram_table(size_t hash_table_size)
{
    if(hash_table_size > HISTOGRAM_MAX_SIZE)
    {
        fprintf(stderr, "Hash table has exceeded maximum bounds of %x items\n", HISTOGRAM_MAX_SIZE);
        exit(1);
    }
    HistogramTable *ht = calloc(1, sizeof(HistogramTable));
    if(!ht)
    {    
        fprintf(stderr, "Memory allocation failed in function %s\n", __func__);
        exit(1);
    }
    ht->capacity = hash_table_size;
    ht->max_entries = (size_t)((double)hash_table_size * HISTOGRAM_MAX_LOAD_FACTOR);
    ht->table = (HistogramEntry*)calloc(hash_table_size, sizeof(HistogramEntry));
    if(!ht->table)
    {    
        fprintf(stderr, "Memory allocation failed in function %s\n", __func__);
        exit(1);
    }
    return ht;
}
void free_histogram_table(HistogramTable *ht)
{
    free(ht->table);
    free(ht);
}

void resize_hash_table(HistogramTable *hash_table)
{
    size_t new_capacity = hash_table->capacity << 1;
    HistogramTable *new = create_histogram_table(new_capacity);
    //printf("Resized Histogram to %ld elements (%f MB)\n",new_capacity, sizeof(HistogramEntry) * ((double)new_capacity)/1024/1024);
    uint64_t index;

    for(size_t i = 0; i < hash_table->capacity; i++)
    {
        index = hash64_mul(hash_table->table[i].key) & (new_capacity - 1);
        while(new->table[index].key != 0)
        {
            index = (index +1) & (new_capacity - 1);
        }
        new->table[index] = hash_table->table[i];
    }
    free(hash_table->table);
    hash_table->capacity *= 2;
    hash_table->max_entries = (size_t)((double)hash_table->capacity * HISTOGRAM_MAX_LOAD_FACTOR);
    hash_table->table = new->table;
    free(new);
}
static inline HistogramEntry *get_entry(HistogramTable *hash_table, const uint64_t key)
{
    uint64_t index = hash64_mul(key) & (hash_table->capacity - 1);
    uint64_t start_index = index;
    while (1)
    {
        if (hash_table->table[index].key == key || hash_table->table[index].key == 0)
        {
            return &hash_table->table[index];
        }
        index = (index + 1) & (hash_table->capacity - 1);
        if (index==start_index)
        {
            printf("Buffer overflowed\n");
            exit(1);
        }
    }
}

void iterateHistogram(HistogramTable *hash_table, const uint64_t key)
{
    
    if (hash_table->entry_count > hash_table->max_entries)
    {
        resize_hash_table(hash_table);
    }

    HistogramEntry *entry = get_entry(hash_table, key);
    entry->count++;
    if(entry->key != 0) return;
    entry->key = key;
    hash_table->entry_count++;
}

void merge_histogram(HistogramTable *destination, const HistogramTable *source)
{
    uint32_t count_added = 0;
    for(int i = 0; i < (int)source->capacity; i++)
    {
        if (destination->entry_count > (destination->capacity >> 1))
        {
            resize_hash_table(destination);
        }
        HistogramEntry source_entry = source->table[i];
        if(source_entry.count == 0)continue;
        HistogramEntry *destination_entry = get_entry(destination, source_entry.key);
        destination_entry->count += source_entry.count;
        if(destination_entry->key != 0) continue;
        count_added += source_entry.count;
        destination_entry->key = source_entry.key;
        destination->entry_count++;
    }
}

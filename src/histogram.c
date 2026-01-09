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
    ht->table = (HistogramEntry*)calloc(hash_table_size, sizeof(HistogramEntry));
    if(!ht->table)
    {    
        fprintf(stderr, "Memory allocation failed in function %s\n", __func__);
        exit(1);
    }
    return ht;
}
void free_hash_table(HistogramTable *ht)
{
    free(ht->table);
    free(ht);
}

void resize_hash_table(HistogramTable *hash_table)
{
    size_t new_capacity = hash_table->capacity << 1;
    HistogramTable *new = create_histogram_table(new_capacity);
    printf("Resized Histogram to %ld elements (%f MB)\n",new_capacity, ((double)new_capacity)/1024/1024);
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
    hash_table->table = new->table;
    free(new);
}
void iterateHistogram(HistogramTable *hash_table, uint64_t key)
{
    
    if (hash_table->entry_count > (hash_table->capacity >> 2))
    {
        resize_hash_table(hash_table);
    }
    
    uint64_t index = hash64_mul(key) & (hash_table->capacity - 1);
    
    while (1)
    {
        uint64_t existing = hash_table->table[index].key;
        if (existing == key)
        {
            hash_table->table[index].count++;
            return;
        }
        if (existing == 0)
        {
            break;
        }
        index = (index + 1) & (hash_table->capacity - 1);
    }
    
    hash_table->table[index].key = key;
    hash_table->table[index].count++;
    hash_table->entry_count++;
}

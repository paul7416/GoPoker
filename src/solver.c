#include "solver.h"
#include <sys/time.h>
#include <assert.h>
#include <string.h>

evCache *create_cache(void)
{
    evCache *C = malloc(sizeof(evCache));
    C->capacity = START_CACHE_SIZE;
    C->data = calloc(START_CACHE_SIZE, sizeof(CacheEntry));
    C->no_entries = 0;
    return C;
}

void free_cache(evCache *C)
{
    free(C->data);
    free(C);
}

uint32_t get_cache_index(evCache *C, uint64_t key)
{
    uint32_t index = (uint32_t)(key & (C->capacity - 1));
    while(C->data[index].occupied == true && C->data[index].key != key)
    {
        index = (index + 1)&(C->capacity - 1);
    }
    return index;
}

void resize_cache(evCache *C)
{
    uint32_t old_capacity = C->capacity;
    CacheEntry *old_table = C->data;
    C->capacity = old_capacity * 2;
    C->data = calloc(C->capacity, sizeof(CacheEntry));
    for(uint32_t i = 0; i < old_capacity; i++)
    {
        if(old_table[i].occupied == false)continue;
        uint32_t new_index = get_cache_index(C, old_table[i].key);
        C->data[new_index] = old_table[i];
    }
    free(old_table);
}

void add_ev_to_cache(evCache *C, uint32_t cache_index, const uint64_t key, const float *ev)
{
    if(C->no_entries >= (C->capacity >> 1))
    {
        resize_cache(C);
        cache_index = get_cache_index(C, key);
    }
    memcpy(C->data[cache_index].evs, ev, sizeof(float) * MAX_PLAYERS);
    C->data[cache_index].key = key;
    C->data[cache_index].occupied = true;
    C->no_entries++;
}

SolverContext *get_solver_context(void)
{
    SolverContext *S = malloc(sizeof(SolverContext));
    S->T = import_evaluator_tables();
    S->C = create_cache();
    return S;
}



void solver(
        const int16_t ranges[MAX_PLAYERS], 
        const uint32_t iterations,
        SolverContext *S,
        const float *payouts,
        const uint32_t number_payouts,
        const uint32_t no_players,
        const uint32_t *stacks,
        const uint32_t n_threads,
        const uint32_t small_blind,
        const uint32_t big_blind,
        double *output_ev
        )
{
    printf("\n");
    struct timeval start, end;
    memset(output_ev, 0, MAX_PLAYERS * sizeof(double));
    GameState *G = create_game_state(no_players, ranges, stacks);
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        printf("%d, ",G->players[i].range.range_extent);
    }
    printf("\n");
    G->big_blind = big_blind;
    G->small_blind = small_blind;
    memcpy(G->payouts, payouts, MAX_PLAYERS * sizeof(float));
    G->number_payouts = (uint8_t)number_payouts;
    gettimeofday(&start, NULL);

    HistogramTable *H = create_histogram_table(HISTOGRAM_START_SIZE);
    multi_thread_iterator(iterations, G, S->T, n_threads, H);

    for(size_t i = 0; i < H->capacity; i++)
    {
        uint32_t count = H->table[i].count;
        if(count == 0)continue;
        uint64_t key = H->table[i].key;
        uint32_t cache_index = get_cache_index(S->C, key); 
        float local_ev[MAX_PLAYERS];
        float *ev;
        if(S->C->data[cache_index].key == key)
        {
            ev = S->C->data[cache_index].evs;
        }
        else
        {
            analyse_pot(G, key, local_ev);
            add_ev_to_cache(S->C, cache_index, key, local_ev);
            ev = local_ev;
        }
        for(uint32_t j = 0; j < no_players; j++)
        {
            output_ev[j] += (double)ev[j] * (double)count;
        }
    }
    for(uint32_t i = 0; i < no_players; i++)
    {
        output_ev[i] /= (double)iterations;
    }
    free_histogram_table(H);
    free_game_state(G);
    gettimeofday(&end, NULL);
    double ms = (double)(end.tv_sec - start.tv_sec) * 1000.0 + 
            (double)(end.tv_usec - start.tv_usec) / 1000.0;
    printf("Wall time: %.2f ms\n", ms);
    printf("Simulations per second %0.0f\n", iterations / ms * 1000);
    printf("Simulations:%d\n", iterations);
    printf("Simulations per thread:%d\n",iterations / n_threads);
    printf("Threads: %d\n",n_threads);
    printf("Hash Table Size:%d B\n",HASH_TABLE_SIZE * 8);
    printf("Direct Lookup Table Size:%d bytes\n",DIRECT_LOOKUP_SIZE * 2);
    printf("Starting size of histogram table:%ld bytes\n",HISTOGRAM_START_SIZE * sizeof(HistogramEntry));
    
}

int main(void)
{
    const uint32_t n_threads = 8;
    const uint32_t number_players = 6;
    const uint32_t iterations = 500000;
    printf("Imported Hands\n");
    const int16_t range_extents[4][9] = {{-1, -1, 169, -1, 100, 100, 100, 100, 100},
                                                 {84, 84, 84, 85, 85, 85, 85, 85, 85},
                                                 {169, 169, 169, 169, 169, 169, 169, 169, 169},
                                                 {24, 18, 24, 24, 24, 24, 34, 100, 34}};

    const uint32_t stacks[9] = {1001, 1002, 1003, 1004, 1005, 1006, 1000, 1000, 1000};
    printf("Number of Players:%d\n",number_players);
    printf("Calling Ranges (out of 169 possible hand types): ");
    for(uint32_t j = 0; j < number_players; j++)
    {
        printf("%d ", range_extents[3][j]);
    }
    printf("\n");
    SolverContext *S = get_solver_context();
    float payouts[3] = {50, 30, 20};
    uint32_t number_payouts = 3;
    uint32_t big_blind = 200;
    uint32_t small_blind = 100;
    double evs[MAX_PLAYERS] = {0};
    solver(range_extents[3], iterations, S, payouts, number_payouts, number_players, stacks, n_threads, small_blind, big_blind, evs);
    printf("evs: ");
    double total_ev = 0;
    for(uint32_t i = 0; i < number_players; i++)
    {
        printf("%0.4f, ",evs[i]);
        total_ev+=evs[i];
    }
    printf("\n");
    printf("total ev:%f\n",total_ev);
    printf("Range: %ld\n",sizeof(Range));
    printf("Player: %ld\n",sizeof(Player));
    printf("GameState: %ld\n",sizeof(GameState));
    return 0;
}

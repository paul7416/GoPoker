#include "player.h"
#include "evaluator.h"
#include "histogram.h"
#include "deck.h"
#include<immintrin.h>

typedef struct 
{
    uint32_t thread_id;
    uint32_t iterations;
    bool *local_playable_hands;
    GameStateSim sim_thread;
    HistogramTable *H_thread;
    const evaluatorTables *T;
} ThreadArgs;

void  multi_thread_iterator(uint32_t iterations, GameState *G, const evaluatorTables *T, const uint32_t n_threads, HistogramTable *H);

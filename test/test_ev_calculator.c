#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include "evaluator.h"
#include "ev_calculator.h"
#include "global_defines.h"
#include "player.h"
#include "histogram.h"
#include "table_import.h"
#include <sys/time.h>

typedef struct {
    int thread_id;
    uint32_t iterations;
    const uint64_t *_52C2;
    GameState *G;
    HistogramTable *H_thread;
    const evaluatorTables *T;
} ThreadArgs;

void* iterator_thread(void *arg) {
    ThreadArgs *a = (ThreadArgs*)arg;
    iterator(a->iterations, a->_52C2, a->G, a->H_thread, a->T);
    return NULL;
}

int main(void)
{
    uint16_t range_extent[6] = {85, 25, 23, 18, 66, 24};
    double stacks[6] = {500, 600, 350, 2000, 1000, 900};

    //HistogramTable *H = create_histogram_table(HISTOGRAM_START_SIZE);
    GameState *G = create_game_state(6, range_extent, stacks);
    const evaluatorTables *T = import_evaluator_tables();

    uint32_t i;
    const uint64_t *_52C2 = import_dat_file("DataFiles/52C2.dat", &i, sizeof(uint64_t));

    printf("Imported Hands\n");

    int n_threads = 8;                    // adjust to number of CPU cores
    pthread_t threads[n_threads];
    ThreadArgs args[n_threads];
    HistogramTable *H_threads[n_threads];
    struct timeval start, end;
    gettimeofday(&start, NULL);

    uint32_t iterations_per_thread = 100000000 / n_threads;
    // create thread-local histograms
    for (int t = 0; t < n_threads; t++) {
        H_threads[t] = create_histogram_table(HISTOGRAM_START_SIZE);
        args[t].thread_id = t;
        args[t].iterations = iterations_per_thread;
        args[t]._52C2 = _52C2;
        args[t].G = G;
        args[t].H_thread = H_threads[t];
        args[t].T = T;
        pthread_create(&threads[t], NULL, iterator_thread, &args[t]);
    }

    // join threads
    for (int t = 0; t < n_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    // merge thread-local histograms into master
    //for (int t = 0; t < n_threads; t++) {
     //   merge_histogram(H, H_threads[t]);
      //  free_histogram_table(H_threads[t]);
   // }
    gettimeofday(&end, NULL);

    double ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
            (end.tv_usec - start.tv_usec) / 1000.0;
    printf("Wall time: %.2f ms\n", ms);
    printf("Hands per second %0.0f\n",iterations_per_thread * n_threads / ms * 1000);
    printf("Simulations:%d\n",iterations_per_thread * n_threads);
    printf("Simulations per thread:%d\n",iterations_per_thread);
    printf("Threads: %d\n",n_threads);
    printf("Clocks Per Second:%ld\n",CLOCKS_PER_SEC);
    printf("Hash Table Size:%d\n",HASH_TABLE_SIZE);
    printf("Direct Lookup Table Size:%d\n",DIRECT_LOOKUP_SIZE);
   // printf("Time: %0.3f ms\n",duration);


    free_evaluator_tables(T);
    free((void*)_52C2);

    return 0;
}


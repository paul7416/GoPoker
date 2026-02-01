#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "evaluator.h"
#include "simulator.h"
#include "global_defines.h"
#include "player.h"
#include "histogram.h"
#include "table_import.h"
#include <sys/time.h>
#include "debug.h"
#include <assert.h>

#define NO_TESTS 4


int run_test_iteration(GameState *G, const evaluatorTables *T, const uint32_t iterations, const int n_threads)
{
    struct timeval start, end;
    gettimeofday(&start, NULL);

    HistogramTable *H = create_histogram_table(HISTOGRAM_START_SIZE);
    
    multi_thread_iterator(iterations, G, T, n_threads, H);

    gettimeofday(&end, NULL);

    free_histogram_table(H);

    double ms = (end.tv_sec - start.tv_sec) * 1000.0 + 
            (end.tv_usec - start.tv_usec) / 1000.0;
    
    printf("Wall time: %.2f ms\n", ms);
    printf("Simulations per second %0.0f\n", iterations / ms * 1000);
    printf("Simulations:%d\n", iterations);
    printf("Simulations per thread:%d\n",iterations / n_threads);
    printf("Threads: %d\n",n_threads);
    printf("Hash Table Size:%d B\n",HASH_TABLE_SIZE * 8);
    printf("Direct Lookup Table Size:%d bytes\n",DIRECT_LOOKUP_SIZE * 2);
    printf("Starting size of histogram table:%ld bytes\n",HISTOGRAM_START_SIZE * sizeof(HistogramEntry));
    



    return 0;
}

int main(void)
{
    const int TESTS_TO_RUN = 1;
    //assert(TESTS_TO_RUN <= NO_TESTS);
    const int n_threads = 8;
    const int number_players = 9;
    const int iterations = 50000000;
    printf("Imported Hands\n");
    const uint16_t range_extents[NO_TESTS][9] = {{24, 24, 24, 25, 25, 25, 25, 25, 25},
                                                 {84, 84, 84, 85, 85, 85, 85, 85, 85},
                                                 {169, 169, 169, 169, 169, 169, 169, 169, 169},
                                                 {34, 34, 34, 35, 35, 35, 35, 35, 35}};

    const double stacks[9] = {500, 600, 350, 2000, 1000, 900, 1000, 1000, 1000};
    const evaluatorTables *T = import_evaluator_tables();
    for(int i = 0; i < TESTS_TO_RUN; i ++)
    {
        
        printf("\n");
        printf("Number of Players:%d\n",number_players);
        printf("Calling Ranges (out of 169 possible hand types): ");
        
        for(int j = 0; j < number_players; j++)
        {
            printf("%d ", range_extents[0][j]);
        }
        printf("\n");
        GameState *G = create_game_state(number_players, range_extents[0], stacks);
        run_test_iteration(G, T, iterations, n_threads);
        free_game_state(G);
    }
    free_evaluator_tables(T);
    printf("Range: %ld\n",sizeof(Range));
    printf("Player: %ld\n",sizeof(Player));
    printf("GameState: %ld\n",sizeof(GameState));
    return 0;
}

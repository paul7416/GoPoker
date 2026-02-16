#include "solver.h"
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include "debug.h"
#include <stdio.h>

int main(void)
{
    struct timeval start, end;
    
    const uint32_t n_threads = 8;
    const uint32_t number_players = 6;
    const uint32_t iterations = 100000000;
    printf("Imported Hands\n");
    const int16_t range_extents[4][9] = {{-1, -1, 169, -1, 100, 100, 100, 100, 100},
                                                 {84, 84, 84, 85, 85, 85, 85, 85, 85},
                                                 {16, 16, 169, 169, 169, 169, 169, 169, 169},
                                                 {24, 18, 24, 24, 24, 24, 34, 34, 34}};
    const uint32_t stacks[9] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
    printf("Number of Players:%d\n",number_players);
    printf("Calling Ranges (out of 169 possible hand types): ");
    for(uint32_t j = 0; j < number_players; j++)
    {
        printf("%d ", range_extents[3][j]);
    }
    printf("\n");
    printf("pre hand stack sizes: ");
    for(uint32_t j = 0; j < number_players; j++)
    {
        printf("%d ", stacks[j]);
    }
    printf("\n");
    SolverContext *S = get_solver_context();
    float payouts[3] = {50, 30, 20};
    uint32_t number_payouts = 3;
    uint32_t big_blind = 200;
    uint32_t small_blind = 100;
    double evs[MAX_PLAYERS] = {0};
    
    gettimeofday(&start, NULL);
    solver(range_extents[3], iterations, S, payouts, number_payouts, number_players, stacks, n_threads, small_blind, big_blind, evs);
    gettimeofday(&end, NULL);
    
    double ms = (double)(end.tv_sec - start.tv_sec) * 1000 + 
                     (double)(end.tv_usec - start.tv_usec) / 1000.0;
    double millions_per_sec = (iterations / 1000.0) / ms;
    
    printf("\nWall time: %.3f ms\n", ms);
    printf("Iterations: %u\n", iterations);
    printf("Million iterations/sec: %.3f\n\n", millions_per_sec);
    
    printf("evs: ");
    double total_ev = 0;
    for(uint32_t i = 0; i < number_players; i++)
    {
        printf("%0.3f, ",evs[i]);
        total_ev+=evs[i];
    }
    printf("\n");
    printf("total ev:%f\n",total_ev);
    printf("Range: %ld\n",sizeof(Range));
    printf("Player: %ld\n",sizeof(Player));
    printf("GameState: %ld\n",sizeof(GameState));
    free_solver_context(S);
    return 0;
}

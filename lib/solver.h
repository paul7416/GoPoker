#include "evaluator.h"
#include "histogram.h"
#include "simulator.h"
#include "global_defines.h"
#include "player.h"
#include "ev.h"
#define START_EV_LIST_SIZE 0x2000

typedef struct
{
    EvEntry *data;
    uint32_t capacity;
    uint32_t length;
}EvList;

typedef struct{
    const evaluatorTables *T;
    EvList *E;
    HistogramTable *H;
}SolverContext;

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
        );

SolverContext *get_solver_context(void);
void free_solver_context(SolverContext *S);

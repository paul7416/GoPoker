#include "solver.h"
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "debug.h"

EvList *create_ev_list(void)
{
    EvList *tmp = malloc(sizeof(EvList));
    if(tmp == NULL)
    {
        fprintf(stderr, "bad memory allocation EvList\n");
        abort();
    }
    EvList *E = tmp;
    E->capacity = START_EV_LIST_SIZE;

    EvEntry *tmp_data = calloc(START_EV_LIST_SIZE, sizeof(EvEntry));
    if(tmp_data  == NULL)
    {
        fprintf(stderr, "bad memory allocation E->data\n");
        abort();
    }
    E->data = tmp_data;
    E->length = 0;
    return E;
}

void free_ev_list(EvList *E)
{
    free(E->data);
    free(E);
}

void resize_ev_list(EvList *E)
{
    E->capacity *= 2;
    EvEntry *tmp = realloc(E->data, E->capacity * sizeof(EvEntry));
    if(tmp == NULL)
    {
        fprintf(stderr, "ev reallocation error\n");
        abort();
    }
    E->data = tmp;
}

uint32_t add_ev_to_list(EvList *E, EvEntry e, uint64_t evaluation)
{
    uint32_t index = E->length;
    if(index >= E->capacity)resize_ev_list(E);
    memcpy(&E->data[index], &e, sizeof(EvEntry));
    assert(index < E->capacity);
    E->data[index].evaluation = evaluation;
    E->length++;
    return index;
}

SolverContext *get_solver_context(void)
{
    SolverContext *S = malloc(sizeof(SolverContext));
    S->T = import_evaluator_tables();
    S->E = create_ev_list();
    S->H = create_histogram_table(HISTOGRAM_START_SIZE);
    return S;
}
void free_solver_context(SolverContext *S)
{
    free_histogram_table(S->H);
    free_ev_list(S->E);
    free_evaluator_tables(S->T);
    free(S);
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
    memset(output_ev, 0, MAX_PLAYERS * sizeof(double));
    GameState *G = create_game_state(no_players, ranges, stacks);
    G->big_blind = big_blind;
    G->small_blind = small_blind;
    memcpy(G->payouts, payouts, MAX_PLAYERS * sizeof(float));
    G->number_payouts = (uint8_t)number_payouts;

    HistogramTable *H = S->H;
    EvList *E = S->E;
    multi_thread_iterator(iterations, G, S->T, n_threads, H);

    for(size_t i = 0; i < H->capacity; i++)
    {
        uint32_t count = H->table[i].count;
        if(count == 0)continue;
        EvEntry local_ev = {0}; 
        EvEntry *ev = NULL;
        HistogramEntry *he = &H->table[i];

        if(he->ev_cached)
        {
            ev = &E->data[he->ev_list_index];
        }
        else
        {
            ev = &local_ev;
            analyse_pot(G, he->key, ev);
            he->ev_list_index = add_ev_to_list(E, *ev, he->key) & 0x7fffffff;
            he->ev_cached = true;
        }
        for(uint32_t j = 0; j < no_players; j++)
        {
            output_ev[j] += (((double)(ev->evs[j])) * (double)count);
        }
    }
    for(uint32_t i = 0; i < no_players; i++)
    {
        output_ev[i] /= (double)iterations;
    }
    free_game_state(G);
}

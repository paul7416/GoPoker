#include "ev.h"
#include <stdint.h>
#include "evaluator.h"
#include <string.h>
#include <stdlib.h>
#include "debug.h"
#include<stdio.h>

int cmp_playerIndex(const void *a, const void *b) {
    Player *x = (Player*)a;
    Player *y = (Player*)b;
    return (x->index > y->index) - (x->index < y->index);
}

int cmp_playerBet(const void *a, const void *b) {
    Player *x = (Player*)a;
    Player *y = (Player*)b;
    return (x->bet > y->bet) - (x->bet < y->bet);
}
int cmp_playerRank(const void *a, const void *b) {
    Player *x = (Player*)a;
    Player *y = (Player*)b;
    return (x->rank > y->rank) - (x->rank < y->rank);
}

void icm_recurse(uint32_t *stacks, uint32_t remaining, float prob, int place,
                 float *payouts, float *ev, uint32_t no_players, int payout_places) {
    if (place >= payout_places) return;
    for (uint32_t i = 0; i < no_players; i++) {
        if (stacks[i] == 0) continue;
        
        float p = ((float)stacks[i]) / (float)remaining;
        ev[i] += prob * p * payouts[place];
        
        uint32_t saved = stacks[i];
        stacks[i] = 0;
        icm_recurse(stacks, remaining - saved, prob * p, place + 1, payouts, ev, no_players, payout_places);
        stacks[i] = saved;
    }
}

void icm(GameState *G, float *ev) {
    uint32_t stacks[MAX_PLAYERS];

    uint32_t total = 0;
    for (uint8_t i = 0; i < G->no_players; i++) {
        stacks[i] = G->players[i].stack;
        total += stacks[i];
    }
    icm_recurse(stacks, total, 1.0, 0, G->payouts, ev, G->no_players, G->number_payouts);
}
static inline uint32_t min_uint32(uint32_t A, uint32_t B)
{
    if(A < B)return A;
    return B;
}

void analyse_pot(GameState *p, const uint64_t evaluation, EvEntry *ev)
{
    // create temp players
    GameState temp_game_state;
    memcpy(&temp_game_state, p, sizeof(GameState));
    // set bets and stacks
    for(uint32_t i = 0; i < temp_game_state.no_players; i++)
    {
        temp_game_state.players[i].bet = 0;
        temp_game_state.players[i].remaining = 0;
        temp_game_state.players[i].rank = 0xff;
        temp_game_state.players[i].index = (uint8_t)i;
    }
    // CRITICAL --- under the gun is always at position 0 as to simplify monte carlo iterations.
    //
    uint8_t sb_index = (uint8_t)(temp_game_state.no_players - 2);
    uint8_t bb_index = (uint8_t)(temp_game_state.no_players - 1);
    Player *BB = &temp_game_state.players[bb_index];
    Player *SB = &temp_game_state.players[sb_index];
    SB->bet = min_uint32(SB->stack, temp_game_state.small_blind);
    BB->bet = min_uint32(BB->stack, temp_game_state.big_blind);
    BB->stack -= BB->bet;
    SB->stack -= SB->bet;

    //get player rankings for hand
    playerResult results[MAX_PLAYERS];
    (void)decodeOutcomes(evaluation, (playerResult*)results);
    //pass player_rankings to temp_game_state
    for(uint32_t i = 0; i < temp_game_state.no_players; i++)
    {
        if(!results[i].folded)
        {
            temp_game_state.players[results[i].index].rank = results[i].player_rank;
            temp_game_state.players[results[i].index].bet += temp_game_state.players[results[i].index].stack;
            temp_game_state.players[results[i].index].stack = 0;
        }
        if(results[i].index >= MAX_PLAYERS)
        {
            fprintf(stderr, "%d %d %d\n", results[i].index, results[i].player_rank, results[i].folded);
            exit(1);
        }
        temp_game_state.players[results[i].index].remaining = temp_game_state.players[results[i].index].bet;

    }
    //sort temp_game_state
    qsort(temp_game_state.players, temp_game_state.no_players, sizeof(Player), cmp_playerBet);

    // distribute pots
    for(uint32_t i = 0; i < temp_game_state.no_players; i++)
    {
        uint32_t pot_size = 0;
        uint32_t pot_increment = temp_game_state.players[i].remaining;
        if(pot_increment == 0) continue;

        uint8_t best_rank = 0xff;
        uint32_t best_rank_count = 0;
        for(uint32_t j = i; j < temp_game_state.no_players; j++)
        {
            pot_size += pot_increment;
            temp_game_state.players[j].remaining -= pot_increment;
            if(temp_game_state.players[j].rank < best_rank)
            {
                best_rank = temp_game_state.players[j].rank;
                best_rank_count = 1;
            }
            else if(temp_game_state.players[j].rank == best_rank)
            {
                best_rank_count++;
            }
        }
        uint32_t divided_pot =  pot_size / best_rank_count;
        for(uint32_t j = i;j < temp_game_state.no_players; j++)
        {
            if(temp_game_state.players[j].rank == best_rank)
            {
                temp_game_state.players[j].stack += divided_pot;
            }
        }
        pot_size = 0;
    }
    qsort(temp_game_state.players, temp_game_state.no_players, sizeof(Player), cmp_playerIndex);
    icm(&temp_game_state, ev->evs);
}

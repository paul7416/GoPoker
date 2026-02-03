#include "ev.h"
#include <stdint.h>
#include "evaluator.h"
#include <string.h>
#include "debug.h"

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
        ev[i] = 0;
    }
    icm_recurse(stacks, total, 1.0, 0, G->payouts, ev, G->no_players, G->number_payouts);
}

void analyse_pot(GameState *p, const uint64_t evaluation, float *ev)
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
    temp_game_state.players[sb_index].stack -= temp_game_state.small_blind;
    temp_game_state.players[sb_index].bet += temp_game_state.small_blind;
    temp_game_state.players[bb_index].stack -= temp_game_state.big_blind;
    temp_game_state.players[bb_index].bet += temp_game_state.big_blind;

    //get player rankings for hand
    playerResult results[MAX_PLAYERS] = {0};
    (void)decodeOutcomes(evaluation, (playerResult*)results);
    //printf("evals:%20lx, %d%d%d, %d%d%d, %d%d%d, %d%d%d, %d%d%d, %d%d%d\n",evaluation,results[0].index,results[0].player_rank,results[0].tied, results[1].index,results[1].player_rank,results[1].tied, results[2].index,results[2].player_rank,results[2].tied, results[3].index,results[3].player_rank,results[3].tied, results[4].index,results[4].player_rank,results[4].tied, results[5].index, results[5].player_rank,results[5].tied);
    //pass player_rankings to temp_game_state
    for(uint32_t i = 0; i < temp_game_state.no_players; i++)
    {
        if(!results[i].folded)
        {
            temp_game_state.players[results[i].index].rank = results[i].player_rank;
            temp_game_state.players[results[i].index].bet += temp_game_state.players[results[i].index].stack;
            temp_game_state.players[results[i].index].stack = 0;
        }
        temp_game_state.players[results[i].index].remaining = temp_game_state.players[results[i].index].bet;

    }
    //sort temp_game_state
    qsort(temp_game_state.players, temp_game_state.no_players, sizeof(Player), cmp_playerBet);

    // distribute pots
    for(uint32_t i = 0; i < temp_game_state.no_players; i++)
    {
        uint32_t pot_increment = temp_game_state.players[i].remaining;
        if(pot_increment == 0) continue;

        uint32_t pot_size = 0;
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
    }
    qsort(temp_game_state.players, temp_game_state.no_players, sizeof(Player), cmp_playerIndex);
    icm(&temp_game_state, ev);
}

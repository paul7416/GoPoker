#include "ev.h"
#include <stdint.h>
#include "evaluator.h"
#include <string.h>

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

void icm_recurse(int *stacks, double remaining, double prob, int place,
                 double *payouts, double *ev, int no_players, int payout_places) {
    if (place >= payout_places) return;
    for (int i = 0; i < no_players; i++) {
        if (stacks[i] == 0) continue;
        
        double p = (double)stacks[i] / remaining;
        ev[i] += prob * p * payouts[place];
        
        int saved = stacks[i];
        stacks[i] = 0;
        icm_recurse(stacks, remaining - saved, prob * p, place + 1, payouts, ev, no_players, payout_places);
        stacks[i] = saved;
    }
}

void icm(int *stacks, int no_players, double *payouts, int payout_places, double *ev) {
    double total = 0;
    for (int i = 0; i < no_players; i++) {
        total += stacks[i];
        ev[i] = 0;
    }
    icm_recurse(stacks, total, 1.0, 0, payouts, ev, no_players, payout_places);
}

void analyse_pot(GameState *p, const uint64_t evaluation)
{
    // create temp players
    GameState temp_game_state;
    memcpy(&temp_game_state, p, sizeof(GameState));
    // set bets and stacks
    for(int i = 0; i < temp_game_state.no_players; i++)
    {
        temp_game_state.players[i].bet = 0;
        temp_game_state.players[i].remaining = 0;
        temp_game_state.players[i].rank = 0xff;
        temp_game_state.players[i].index = i;
    }
    temp_game_state.players[1].stack -= temp_game_state.small_blind;
    temp_game_state.players[1].bet += temp_game_state.small_blind;
    int BB_index = 2 % temp_game_state.no_players;
    temp_game_state.players[BB_index].stack -= temp_game_state.big_blind;
    temp_game_state.players[BB_index].bet += temp_game_state.big_blind;

    //get player rankings for hand
    playerResult results[MAX_PLAYERS];
    (void)decodeOutcomes(evaluation, results);

    //pass player_rankings to temp_game_state
    for(int i = 0; i < temp_game_state.no_players; i++)
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
    for(int i = 0; i < temp_game_state.no_players; i++)
    {
        double pot_increment = temp_game_state.players[i].remaining;
        if(pot_increment == 0) continue;

        double pot_size = 0;
        uint8_t best_rank = 0xff;
        int best_rank_count = 0;
        for(int j = i; j < temp_game_state.no_players; j++)
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
        double divided_pot =  pot_size / best_rank_count;
        for(int j = i;j < temp_game_state.no_players; j++)
        {
            if(temp_game_state.players[j].rank == best_rank)
            {
                temp_game_state.players[j].stack += divided_pot;
            }
        }
    }
    // sort by index
    qsort(temp_game_state.players, temp_game_state.no_players, sizeof(Player), cmp_playerIndex);
    for(int i = 0; i < temp_game_state.no_players; i++)
    {
        printf("Index:%d   Rank:%d   Stack:%f\n", temp_game_state.players[i].index,temp_game_state.players[i].rank,temp_game_state.players[i].stack);
    }
}

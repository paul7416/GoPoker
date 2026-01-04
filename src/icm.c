#include "icm.h"

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


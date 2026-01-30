#include "ev_calculator.h"
#include "evaluator.h"
#include <stdint.h>
#include <sys/random.h>
#include <assert.h>
//#include "debug.h"

// Xorshift128+ generator

static inline uint64_t xorshift128plus(uint64_t s[2]) {
    uint64_t x = s[0];
    uint64_t y = s[1];
    s[0] = y;
    x ^= x << 23;
    s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
    return s[1] + y;
}
void seed(uint64_t s[2])
{
    assert(getrandom(s, 16, 0) == 16);
    for( int i = 0; i < 20; i++)xorshift128plus(s);
}

static inline uint64_t generate_community_cards(uint64_t *s)
{
    uint64_t cards = 0;
    int count = 0;
    while (count < 5)
    {
        //int card = xorshift128plus(s) % 52;
        int card = ((uint64_t)((uint32_t)xorshift128plus(s)) * 52) >> 32;
        uint64_t bit = 1ULL << card;
        if (!(bit & cards))
        {
            cards |= bit;
            count++;
        }
    }
    return cards;
}

void iterator(const int iterations, GameState *G, HistogramTable *H, const evaluatorTables *T)
{
    // Build lightweight copy
    uint16_t local_playable_hands[0x1000] = {0};
    GameStateSim sim;
    sim.no_players = G->no_players;
    for (int i = 0; i < sim.no_players; i++)
    {
        for(int j = 0; j < 0x1000; j++)
        {
            local_playable_hands[j] |= (G->players[i].range.playableHands[j] << i);
        }
        sim.players[i].index = G->players[i].index;
    }
    
    uint64_t s[2];
    seed(s);
    uint64_t card_1_mask, card_2_mask, used;
    uint8_t active_count, last_active;
    uint16_t card_1_index, card_2_index;

    for(int iteration = 0; iteration < iterations; iteration++)
    {
        active_count = 0;
        last_active = 0;
        sim.community_cards = generate_community_cards(s);
        used = sim.community_cards;
        for(int i = 0; i < sim.no_players; i++)
        {
            PlayerSim *p = &sim.players[i];

            while(1)
            {
                // Lemire's fast range mapping (biased, but negligible for Monte Carlo)
                card_1_index = ((uint64_t)((uint32_t)xorshift128plus(s)) * 52) >> 32;
                card_1_mask = 1ull << card_1_index;
                if(!(card_1_mask&used))break;
            }
            used |= card_1_mask;
            // After getting card_1_index, before second card loop:
            while(1)
            {
                card_2_index = ((uint64_t)((uint32_t)xorshift128plus(s)) * 52) >> 32;
                card_2_mask = 1ull << card_2_index;
                if(!(card_2_mask & used))break;
            }
            used |= card_2_mask;
            p->hole_cards = card_1_mask|card_2_mask;
            p->folded = !(local_playable_hands[card_1_index << 8 | card_2_index] & (1 << i));
            active_count += !p->folded;
            last_active = i * (!p->folded) +(last_active * p->folded);
        }
        sim.last_active = last_active;
        sim.active_count = active_count;
        uint64_t evaluation = evaluateRound(&sim, T);
        iterateHistogram(H, evaluation);
    }
}




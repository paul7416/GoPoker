#include "ev_calculator.h"
#include "evaluator.h"
#include <stdint.h>
#include <sys/random.h>
#include <assert.h>
//#include "debug.h"

cardDeck create_card_deck(int no_players)
{
    cardDeck d;
    d.current_index = 0;
    for(int i = 0; i < DECK_SIZE; i++)
    {
        d.card_array[i] = i;
    }
    d.number_of_shuffled_cards = 2 * no_players + 5;
    return d;
}

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

static inline void shuffle_deck(cardDeck *d, uint64_t s[2])
{
    uint32_t tmp;
    for(int i = 0; i < d->number_of_shuffled_cards; i++)
    {
        uint32_t rnd_index = (((uint64_t)((uint32_t)xorshift128plus(s)) * (DECK_SIZE - i)) >> 32) + i;
        tmp = d->card_array[i];
        d->card_array[i] = d->card_array[rnd_index];
        d->card_array[rnd_index] = tmp;
    }

}



static inline uint64_t generate_community_cards(cardDeck *d)
{
    d->current_index = 5;
    return (1ull << d->card_array[0])|
           (1ull << d->card_array[1])|
           (1ull << d->card_array[2])|
           (1ull << d->card_array[3])|
           (1ull << d->card_array[4]);
}

void iterator(const int iterations, GameState *G, HistogramTable *H, const evaluatorTables *T)
{
    // Build lightweight copy
    uint16_t local_playable_hands[0x3400] = {0};
    GameStateSim sim;
    sim.no_players = G->no_players;
    for (int i = 0; i < sim.no_players; i++)
    {
        for(int j = 0; j < 0x3400; j++)
        {
            local_playable_hands[j] |= (G->players[i].range.playableHands[j] << i);
        }
        sim.players[i].index = G->players[i].index;
    }
    cardDeck d = create_card_deck(G->no_players);
    
    // create and seed rng
    uint64_t s[2];
    seed(s);

    // loop variables
    uint8_t active_count, last_active;


    for(int iteration = 0; iteration < iterations; iteration++)
    {
        shuffle_deck(&d, s);
        active_count = 0;
        last_active = 0;
        sim.community_cards = generate_community_cards(&d);
        uint16_t *player_draw_cards = (uint16_t*)(d.card_array + 5);
        for(int i = 0; i < sim.no_players; i++)
        {
            PlayerSim *p = &sim.players[i];
            assert(player_draw_cards[i] < 0x3400);
            p->folded = !(local_playable_hands[player_draw_cards[i]] & (1 << i));
            if (!p->folded)
            {
                p->hole_cards = (1ull << d.card_array[5 + 2 * i])|(1ull << d.card_array[6 + 2 * i]);
                active_count++;
                last_active = i;
            }
        }
        sim.last_active = last_active;
        sim.active_count = active_count;
        uint64_t evaluation = evaluateRound(&sim, T);
        iterateHistogram(H, evaluation);
    }
}




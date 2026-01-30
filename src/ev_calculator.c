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

void shuffle_single_deck(uint8_t cdecks[DECK_SIZE][CONCURRENT_DECKS], int deck_no, uint64_t s[2])
{
    uint8_t tmp;
    for(int i = 0; i < DECK_SIZE - 1; i++)
    {
        uint32_t rnd_index = (((uint64_t)((uint32_t)xorshift128plus(s)) * (DECK_SIZE - i)) >> 32) + i;
        tmp = cdecks[i][deck_no];
        cdecks[i][deck_no] = cdecks[rnd_index][deck_no];
        cdecks[rnd_index][deck_no] = tmp;
    }

}

cardDeck create_card_deck(int no_players, uint64_t s[2])
{
    cardDeck d;
    d.current_index = 0;
    d.number_of_shuffled_cards = no_players * 2 + 5;
    uint8_t decks[DECK_SIZE][CONCURRENT_DECKS];
    for(int deck_number = 0; deck_number < CONCURRENT_DECKS; deck_number++)
    {
        for(int i = 0; i < DECK_SIZE; i++)
        {
            decks[i][deck_number] = i;
        }
        shuffle_single_deck(decks, deck_number, s);
    }
    __uint128_t *tmpptr = (__uint128_t*)decks;
    for(int i = 0; i < DECK_SIZE; i++)
    {
        d.card_array[i] = tmpptr[i];
    }
    return d;
}


__attribute__((noinline)) void shuffle_deck(cardDeck *d, uint64_t s[2])
{
    __m128i a;
    __m128i *arr = (__m128i*)d->card_array;
    
    for(int i = 0; i < d->number_of_shuffled_cards; i++)
    {
        uint32_t rnd_index = (((uint64_t)((uint32_t)xorshift128plus(s)) * (DECK_SIZE - i)) >> 32) + i;
        a = arr[i];
        arr[i] = arr[rnd_index];
        arr[rnd_index] = a;
    }

}

static inline uint64_t generate_community_cards(uint8_t cards[DECK_SIZE][CONCURRENT_DECKS], int sim_no)
{
    return (1ull << cards[0][sim_no])|
           (1ull << cards[1][sim_no])|
           (1ull << cards[2][sim_no])|
           (1ull << cards[3][sim_no])|
           (1ull << cards[4][sim_no]);
}

void iterator(int iterations, GameState *G, HistogramTable *H, const evaluatorTables *T)
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
    // create and seed rng
    uint64_t s[2];
    seed(s);
    cardDeck d = create_card_deck(G->no_players, s);
    // loop variables
    uint8_t active_count, last_active, card_1, card_2;
    uint8_t (*cards)[CONCURRENT_DECKS];
    iterations /= CONCURRENT_DECKS;
    uint64_t evaluation;


    for(int iteration = 0; iteration < iterations; iteration++)
    {
        shuffle_deck(&d, s);
        cards = (uint8_t(*)[CONCURRENT_DECKS])d.card_array;
        for(int sim_no = 0; sim_no < CONCURRENT_DECKS; sim_no++)
        {
            active_count = 0;
            last_active = 0;
            sim.community_cards = generate_community_cards(cards, sim_no);
            for(int i = 0; i < sim.no_players; i++)
            {
                PlayerSim *p = &sim.players[i];
                card_1 = cards[(i << 1) + 5][sim_no];
                card_2 = cards[(i << 1) + 6][sim_no];
                uint16_t playable_index = ((uint16_t)card_1 << 8)|(card_2);
                p->folded = !(local_playable_hands[playable_index] & (1 << i));

                if (!p->folded)
                {
                    uint16_t *ptr = (uint16_t*)&p->hole_cards;
                    *ptr = playable_index;
                    active_count++;
                    last_active = i;
                }
            }
            if(active_count <= 1)
            {
                evaluation = last_active;
            }
            else
            {
                evaluation = evaluateRound(&sim, T);
            }
            iterateHistogram(H, evaluation);
        }
    }
}

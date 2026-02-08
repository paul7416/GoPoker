#include "simulator.h"
#include "evaluator.h"
#include <stdint.h>
#include <sys/random.h>
#include <assert.h>
#include "debug.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>



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

void shuffle_single_deck(cardDeck *d, int deck_no, uint64_t s[2])
{
    uint8_t tmp;
    for(uint32_t i = 0; i < DECK_SIZE - 1; i++)
    {
        uint32_t rnd_index = (uint32_t)((((uint64_t)((uint32_t)xorshift128plus(s)) * (DECK_SIZE - i)) >> 32) + i);
        tmp = d->data[i].cards[deck_no];
        d->data[i].cards[deck_no] = d->data[rnd_index].cards[deck_no];
        d->data[rnd_index].cards[deck_no] = tmp;
    }
}

cardDeck create_card_deck(uint8_t no_players, uint64_t s[2])
{
    cardDeck d;
    d.current_index = 0;
    d.no_players = no_players;
    d.number_of_shuffled_cards = (uint8_t)(no_players * 2 + 5);
    for(int deck_number = 0; deck_number < CONCURRENT_DECKS; deck_number++)
    {
        for(int i = 0; i < DECK_SIZE; i++)
        {
            int suit = i / 13;
            int rank = i % 13;
            int card_int = (suit << 4) | rank;
            d.data[i].cards[deck_number] = (uint8_t)card_int;
        }
        shuffle_single_deck(&d, deck_number, s);
    }
    return d;
}

__attribute__((noinline))
static void shuffle_deck(cardDeck *d, uint64_t s[2])
{
    dec_vec a;
    union deckEntry *arr = d->data;

    for(uint32_t i = 0; i < d->number_of_shuffled_cards; i++)
    {
        uint32_t rnd_index = (uint32_t)((((uint64_t)((uint32_t)xorshift128plus(s)) * (DECK_SIZE - i)) >> 32) + i);
        a = arr[i].vectors;
        arr[i].vectors = arr[rnd_index].vectors;
        arr[rnd_index].vectors = a;
    }

    for(uint8_t player = 0, deck_index = 5; deck_index < d->number_of_shuffled_cards; player++, deck_index += 2)
    {
        // load vectors for this player's two cards
        dec_vec card_1_vector = d->data[deck_index].vectors;
        dec_vec card_2_vector = d->data[deck_index + 1].vectors;

        // extract ranks
        dec_vec rank1 = and_vec(card_1_vector, vec_set1_epi8(0x0F));
        dec_vec rank2 = and_vec(card_2_vector, vec_set1_epi8(0x0F));

        // extract suits (shift right by 4, then mask)
        dec_vec suit1 = and_vec(vec_srli_epi16(card_1_vector, 4), vec_set1_epi8(0x0F));
        dec_vec suit2 = and_vec(vec_srli_epi16(card_2_vector, 4), vec_set1_epi8(0x0F));

        // calculate hand_types: (rank1 << 4) | rank2
        dec_vec hand_types = or_vec(vec_slli_epi16(rank1, 4), rank2);

        // calculate suited flags (0x01 if suited, 0x00 if not)
        dec_vec suited_flags = and_vec(vec_cmpeq_epi8(suit1, suit2), vec_set1_epi8(0x01));

        // Combine into 16-bit values: hand_types in low byte, suited in high byte
        // unpacklo/hi interleaves bytes: result is [hand_types[0], suited[0], hand_types[1], suited[1], ...]
        dec_vec combined_lo = vec_unpacklo_epi8(hand_types, suited_flags);
        dec_vec combined_hi = vec_unpackhi_epi8(hand_types, suited_flags);

        // Store both halves as 16-bit indices
        d->hand_indices[player].vectors[0] = combined_lo;
        d->hand_indices[player].vectors[1] = combined_hi;
    }
}

static inline uint64_t generate_community_cards(cardDeck *d, int sim_no)
{
    return (1ull << d->data[0].cards[sim_no])|
           (1ull << d->data[1].cards[sim_no])|
           (1ull << d->data[2].cards[sim_no])|
           (1ull << d->data[3].cards[sim_no])|
           (1ull << d->data[4].cards[sim_no]);
}

void single_thread_iterator(
                const uint32_t iterations, 
                const bool *playable_hands, 
                GameStateSim sim,
                const cardDeck *original_deck,
                HistogramTable *H,
                const evaluatorTables *T)
{
    cardDeck d;
    memcpy(&d, original_deck, sizeof(cardDeck));  //Need to ensure local copy of base data

    bool (*local_playable_hands)[460] = (bool (*)[460])playable_hands;

    // create and seed rng_
    uint64_t s[2];
    seed(s);
    uint8_t active_count, card_1, card_2;
    int last_active;
    uint64_t evaluation;
    uint32_t concurrent_iterations = iterations;

    for(uint32_t iteration = 0; iteration < concurrent_iterations; iteration++)
    {
        shuffle_deck(&d, s);
        for(int sim_no = 0; sim_no < CONCURRENT_DECKS; sim_no++)
        {
            active_count = 0;
            last_active = 0;
            for(int i = 0; i < sim.no_players; i++)
            {
                PlayerSim *p = &sim.players[i];
                card_1 = d.data[(i << 1) + 5].cards[sim_no];
                card_2 = d.data[(i << 1) + 6].cards[sim_no];
                uint16_t playable_index = d.hand_indices[i].indices[sim_no];
                p->folded = !(local_playable_hands[i][playable_index]);
                if (!p->folded)
                {
                    p->hole_cards[0] = card_1;
                    p->hole_cards[1] = card_2;
                    active_count++;
                    last_active = i;
                }

            }
            if(active_count == 1)
            {
                evaluation = (uint64_t)(last_active) + 1;
            }
            else if(active_count == 0)
            {
                evaluation = (uint64_t)(sim.no_players);
            }
            else
            {
                sim.community_cards = generate_community_cards(&d, sim_no);
                evaluation = evaluateRound(&sim, T);

            }
            iterateHistogram(H, evaluation);
        }
    }
}


void* iterator_thread(void *arg) {
    ThreadArgs *a = (ThreadArgs*)arg;
    single_thread_iterator(a->iterations, a->local_playable_hands, a->sim_thread, a->d_thread, a->H_thread, a->T);
    return NULL;
}

void  multi_thread_iterator(uint32_t iterations, GameState *G, const evaluatorTables *T, const uint32_t n_threads, HistogramTable *H)
{
    pthread_t threads[n_threads];
    ThreadArgs args[n_threads];
    HistogramTable *H_threads[n_threads];
    assert(iterations % CONCURRENT_DECKS == 0);
    uint32_t iteration_sets = iterations / CONCURRENT_DECKS;
    uint32_t iterations_sets_per_thread = iteration_sets / n_threads;
    uint32_t iteration_mod = iteration_sets % n_threads;

    // Build lightweight copy
    bool local_playable_hands[MAX_PLAYERS][460] = {0};
    GameStateSim sim;
    sim.no_players = (uint8_t)G->no_players;
    for (int i = 0; i < sim.no_players; i++)
    {
        for(int j = 0; j < 460; j++)
        {
            local_playable_hands[i][j] = (G->players[i].range.playableHands[j]);
        }
        sim.players[i].index = G->players[i].index;
    }
    // create and seed rng
    uint64_t s[2];
    seed(s);
    cardDeck d = create_card_deck((uint8_t)G->no_players, s);
    
    // create thread-local histograms
    for (uint32_t t = 0; t < n_threads; t++) {
        H_threads[t] = create_histogram_table(HISTOGRAM_START_SIZE);
        args[t].thread_id = t;
        args[t].iterations = iterations_sets_per_thread;
        if(t < iteration_mod)args[t].iterations++;
        args[t].sim_thread = sim;
        args[t].d_thread = &d;
        args[t].H_thread = H_threads[t];
        args[t].T = T;
        args[t].local_playable_hands = (bool*)local_playable_hands;
        pthread_create(&threads[t], NULL, iterator_thread, &args[t]);
    }
    // join threads
    for (uint32_t t = 0; t < n_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    //merge thread-local histograms into master
    for (uint32_t t = 0; t < n_threads; t++) {
        merge_histogram(H, H_threads[t]);
        free_histogram_table(H_threads[t]);
    }
}

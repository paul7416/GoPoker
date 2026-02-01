#include "simulator.h"
#include "evaluator.h"
#include <stdint.h>
#include <sys/random.h>
#include <assert.h>
#include "debug.h"
#include <pthread.h>
#include <string.h>


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
            int suit = i / 13;
            int rank = i % 13;
            int card_int = 16 * suit + rank;
            decks[i][deck_number] = card_int;
        }
        shuffle_single_deck(decks, deck_number, s);
    }
    dec_vec *tmpptr = (dec_vec*)decks;
    for(int i = 0; i < DECK_SIZE; i++)
    {
        d.card_array[i] = tmpptr[i];
    }
    return d;
}


static inline void shuffle_deck(cardDeck *d, uint64_t s[2])
{
    dec_vec a;
    dec_vec *arr = (dec_vec*)d->card_array;
    
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

void single_thread_iterator(
                int iterations, 
                bool *playable_hands, 
                GameStateSim sim,
                cardDeck *original_deck,
                HistogramTable *H,
                const evaluatorTables *T)
{
    cardDeck d;
    memcpy(&d, original_deck, sizeof(cardDeck));  //Need to ensure local copy of base data

    bool (*local_playable_hands)[0x4000] = (bool (*)[0x4000])playable_hands;

    // create and seed rng_
    uint64_t s[2];
    seed(s);
    uint8_t (*cards)[CONCURRENT_DECKS];
    uint8_t active_count, last_active, card_1, card_2;
    uint64_t evaluation;
    int concurrent_iterations = iterations / CONCURRENT_DECKS;

    for(int iteration = 0; iteration < concurrent_iterations; iteration++)
    {
        shuffle_deck(&d, s);
        cards = (uint8_t(*)[CONCURRENT_DECKS])d.card_array;
        for(int sim_no = 0; sim_no < CONCURRENT_DECKS; sim_no++)
        {
            active_count = 0;
            last_active = 0;

            for(int i = 0; i < sim.no_players; i++)
            {
                PlayerSim *p = &sim.players[i];
                card_1 = cards[(i << 1) + 5][sim_no];
                card_2 = cards[(i << 1) + 6][sim_no];
                uint16_t playable_index = ((uint16_t)card_1 << 8)|(card_2);
                p->folded = !(local_playable_hands[i][playable_index]);

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
                evaluation = last_active + 1;
            }
            else
            {
                sim.community_cards = generate_community_cards(cards, sim_no);
                evaluation = evaluateRound(&sim, T);

            }
            iterateHistogram(H, evaluation);
        }
    }
}

typedef struct {
    int thread_id;
    uint32_t iterations;
    bool *local_playable_hands;
    GameStateSim sim_thread;
    cardDeck *d_thread;
    HistogramTable *H_thread;
    const evaluatorTables *T;
} ThreadArgs;

void* iterator_thread(void *arg) {
    ThreadArgs *a = (ThreadArgs*)arg;
    single_thread_iterator(a->iterations, a->local_playable_hands, a->sim_thread, a->d_thread, a->H_thread, a->T);
    return NULL;
}

void  multi_thread_iterator(int iterations, GameState *G, const evaluatorTables *T, const int n_threads, HistogramTable *H)
{
    pthread_t threads[n_threads];
    ThreadArgs args[n_threads];
    HistogramTable *H_threads[n_threads];
    uint32_t iterations_per_thread = iterations / n_threads;

    // Build lightweight copy
    bool local_playable_hands[MAX_PLAYERS][0x4000] = {0};
    GameStateSim sim;
    sim.no_players = G->no_players;
    for (int i = 0; i < sim.no_players; i++)
    {
        for(int j = 0; j < 0x4000; j++)
        {
            local_playable_hands[i][j] |= (G->players[i].range.playableHands[j]);
        }
        sim.players[i].index = G->players[i].index;
    }
    // create and seed rng
    uint64_t s[2];
    seed(s);
    cardDeck d = create_card_deck(G->no_players, s);
    
    // create thread-local histograms
    for (int t = 0; t < n_threads; t++) {
        H_threads[t] = create_histogram_table(HISTOGRAM_START_SIZE);
        args[t].thread_id = t;
        args[t].iterations = iterations_per_thread;
        args[t].sim_thread = sim;
        args[t].d_thread = &d;
        args[t].H_thread = H_threads[t];
        args[t].T = T;
        args[t].local_playable_hands = (bool*)local_playable_hands;
        pthread_create(&threads[t], NULL, iterator_thread, &args[t]);
    }
    // join threads
    for (int t = 0; t < n_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    //merge thread-local histograms into master
    for (int t = 0; t < n_threads; t++) {
        merge_histogram(H, H_threads[t]);
        free_histogram_table(H_threads[t]);
    }
    printf("total number of buckets:%ld\n",H->entry_count);
}

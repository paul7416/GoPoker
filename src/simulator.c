#include "simulator.h"
#include <stdint.h>
#include <sys/random.h>
#include <assert.h>
#include "debug.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>


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
                HistogramTable *H,
                const evaluatorTables *T)
{
    cardDeck d = create_card_deck(sim.no_players, DECK_SIZE);
    initialization_shuffle(&d);

    bool (*local_playable_hands)[460] = (bool (*)[460])playable_hands;

    // create and seed rng_
    uint8_t active_count, card_1, card_2;
    int last_active;
    uint64_t evaluation;
    uint32_t concurrent_iterations = iterations;

    for(uint32_t iteration = 0; iteration < concurrent_iterations; iteration++)
    {
        shuffle_deck(&d);
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
    single_thread_iterator(a->iterations, a->local_playable_hands, a->sim_thread, a->H_thread, a->T);
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
    
    // create thread-local histograms
    for (uint32_t t = 0; t < n_threads; t++) {
        H_threads[t] = create_histogram_table(HISTOGRAM_START_SIZE);
        args[t].thread_id = t;
        args[t].iterations = iterations_sets_per_thread;
        if(t < iteration_mod)args[t].iterations++;
        args[t].sim_thread = sim;
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

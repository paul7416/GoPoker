#include "player.h"
#include "evaluator.h"
#include "histogram.h"
#include<immintrin.h>

#define DECK_SIZE 52

#ifdef __AVX2__
    typedef __m256i dec_vec;
    #define CONCURRENT_DECKS 32
#else
    typedef __m128i dec_vec;
    #define CONCURRENT_DECKS 16
#endif

typedef struct __attribute__((aligned(64))){
    dec_vec card_array[DECK_SIZE];
    uint8_t current_index;
    uint8_t number_of_shuffled_cards;
}cardDeck;

void  multi_thread_iterator(int iterations, GameState *G, const evaluatorTables *T, const int n_threads, HistogramTable *H);

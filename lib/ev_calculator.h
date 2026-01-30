#include "player.h"
#include "evaluator.h"
#include "histogram.h"
#include<emmintrin.h>

#define DECK_SIZE 52
#define CONCURRENT_DECKS 16

typedef struct __attribute__((aligned(64))){
    __uint128_t card_array[DECK_SIZE];
    uint8_t current_index;
    uint8_t number_of_shuffled_cards;
}cardDeck;

typedef struct {
    uint64_t s[2];
    uint64_t bits;
    int remaining;
} RNG;

void iterator(int iterations, GameState *G, HistogramTable *H, const evaluatorTables *T);

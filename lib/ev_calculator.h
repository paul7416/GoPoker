#include "player.h"
#include "evaluator.h"
#include "histogram.h"

#define DECK_SIZE 52

typedef struct{
    uint8_t card_array[DECK_SIZE];
    uint8_t current_index;
    uint8_t number_of_shuffled_cards;
    uint8_t padding[2];
}cardDeck;

// Xorshift128+ state (must be seeded with non-zero values)
void iterator(const int iterations, GameState *G, HistogramTable *H, const evaluatorTables *T);

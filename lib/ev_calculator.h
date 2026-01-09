#include "player.h"
#include "evaluator.h"
#include "histogram.h"


// Xorshift128+ state (must be seeded with non-zero values)
void iterator(const int iterations, const uint64_t *_52C2, GameState *G, HistogramTable *H, const evaluatorTables *T);

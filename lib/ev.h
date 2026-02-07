#include "player.h"
#include "global_defines.h"
#include <stdint.h>


typedef struct
{
    uint64_t evaluation;
    float evs[MAX_PLAYERS];
}EvEntry;

void icm(GameState *G, float *ev);
void analyse_pot(GameState *p, const uint64_t evaluation, EvEntry *ev);


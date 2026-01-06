#include "player.h"

void set_range_extent(Player *p, int16_t range_extent)
{
    Range range = p->range;
    range.range_extent = range_extent;
    for(int i = 0; i < 1326; i++)
    {
        range.playableHands[i] = (range.handRanks[i] <= range_extent);
    }
}

void increment_range_extent(Player *p, int16_t delta)
{
    Range range = p->range;
    int16_t new_range_extent = range.range_extent + delta;
    if (new_range_extent > 168) set_range_extent(p, 168);
    else if (new_range_extent < 0) set_range_extent(p, 0);
    else set_range_extent(p, new_range_extent);
}

bool get_hand_mask(Player *p, uint16_t hand_index)
{
    assert(hand_index < 1326);
    return p->range.playableHands[hand_index];
}

Players *create_players(int no_players, int16_t initial_range_extent)
{
    Players *players = calloc(1, sizeof(Players));
    uint32_t count;
    players->bitMasks = (uint64_t*)import_dat_file("DataFiles/52C2.dat", &count, sizeof(uint64_t))
    players->handRanks = (uint8_t*)import_dat_file("DataFiles/holeCardHandRanks.dat", &count, sizeof(uint8_t))
    players->no_players = no_players;
    players->players = calloc(no_players, sizeof(Player));
    for(int i = 0; i < no_players; i++)
    {
        players->players[i].range.bitMasks = players->bitMasks;
        players->players[i].range.handRanks = players->handRanks;
        set_range_extent(players->players[i], initial_range_extent);
    }
    return players;
}

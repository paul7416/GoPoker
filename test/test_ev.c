#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "evaluator.h"
#include "player.h"
#include <string.h>
#include "ev.h"
#include "global_defines.h"
#include "debug.h"


char *hand_types[9] = {"Straight Flush",
                       "Quads",
                       "Full House",
                       "Flush",
                       "Straight",
                       "Trips",
                       "Two Pair",
                       "Pair",
                       "High Card"};

uint8_t get_rank(char c)
{
    char *ranks = "23456789TJQKA";
    for(uint8_t i = 0; i < 13; i++)
    {
        if (ranks[i] == c) return i;
    }
    return 255;
}
uint8_t get_suit(char c)
{
    char *suits = "hdcs";
    for(uint8_t i = 0; i < 4; i++)
    {
        if(suits[i] == c) return i;
    }
    return 255;
}
uint8_t get_card(char card_string[])
{
    uint8_t rank = get_rank(card_string[0]);
    uint8_t suit = get_suit(card_string[1]);
    return (suit << 4)|rank;
}
uint16_t print_hand_state(char hole_cards[MAX_PLAYERS][2][3], char community_cards[5][3], const evaluatorTables *T, const uint32_t no_players)
{
    printf("\n");
    printf("player|  seven card hand   |   hand_type   | rank within type\n");
    for(uint32_t player_index = 0; player_index < no_players; player_index++)
    {
        printf("%6d|%s %s", player_index, hole_cards[player_index][0], hole_cards[player_index][1]);
        uint64_t bitMask = (1ull << get_card(hole_cards[player_index][0])) | (1ull << get_card(hole_cards[player_index][1]));
        for(int i = 0; i < 5; i++)
        {
            printf(" %s", community_cards[i]);
            bitMask |= (1ull << get_card(community_cards[i]));
        }
        uint16_t score = evaluateHand(bitMask, T->Flushes, T->Primes, T->hashTable, T->directLookup);
        printf("|%s",hand_types[score>>12]);
        for(size_t i = strlen(hand_types[score>>12]); i < 15; i++)printf(" ");
        printf("|%3d\n",score&0xfff);
    }

    return 0;
}

void get_hole_cards_string(char hole_cards_string[5], uint8_t card_1, uint8_t card_2)
{
    char *suits = "hdcs";
    char *ranks = "23456789TJQKA";
    hole_cards_string[0] = ranks[card_1 & 0xf];
    hole_cards_string[1] = suits[card_1 >> 4];
    hole_cards_string[2] = ranks[card_2 & 0xf];
    hole_cards_string[3] = suits[card_2 >> 4];
}

void test_ev(const evaluatorTables *T, uint8_t community_cards[5], uint8_t player_cards[MAX_PLAYERS][2], const uint32_t noPlayers, const uint32_t *stacks, const float *payouts, int no_payouts)
{

    GameState G = {0};
    G.no_players = (uint8_t)noPlayers;
    GameStateSim sim = {0};
    sim.no_players = (uint8_t)noPlayers;

    for(int i = 0; i < 5; i++)
    {
        uint64_t card_mask = 1ull << community_cards[i];
        if (sim.community_cards & card_mask)
        {
            printf("Clash on community card %d %d\n", i, community_cards[i]);
            exit(1);
        }
        sim.community_cards |= card_mask;
    }
    G.big_blind = 200;
    G.small_blind = 100;
    G.number_payouts = (uint8_t)no_payouts;
    for(int i = 0; i < no_payouts; i++)
    {
        G.payouts[i] = payouts[i];
    }
    

    uint64_t used = sim.community_cards;
    
    for(uint8_t i = 0; i < noPlayers; i++)
    {
        G.players[i].stack = stacks[i];
        sim.players[i].hole_cards[0] = player_cards[i][0];
        sim.players[i].hole_cards[1] = player_cards[i][1];
        if(player_cards[i][0] == player_cards[i][1] 
                || ((1ull << player_cards[i][0]) | (1ull << player_cards[i][1])) & used)
        {
            printf("Clash on community card %d\n", i);
            exit(1);
        }
        sim.players[i].index = i;
        sim.players[i].folded = false;
    }
    sim.players[1].folded = true;

    uint64_t evaluation = evaluateRound(&sim, T);
    uint64_t outcome = evaluation;
    printf("Outcome:%lx\n",outcome);
    print_outcome(outcome);
    EvEntry ev_entry = {0};
    ev_entry.evaluation = evaluation;
    analyse_pot(&G, evaluation, &ev_entry);
    float total_evs = 0;
    for(uint32_t i = 0; i < noPlayers; i++)
    {
        total_evs += ev_entry.evs[i];
        printf("player %d ev: %f\n", i, ev_entry.evs[i]);
    }
    printf("Total Evs: %f\n", total_evs);
}

int main(void)
{
    const uint32_t noPlayers = 9;
    const evaluatorTables *tables = import_evaluator_tables();
    char playerHands[MAX_PLAYERS][2][3] = 
    {
        {"Td", "Kd"},
        {"Ts", "Kc"},
        {"Qs", "Js"},
        {"Ad", "Ac"},
        {"2c", "3c"},
        {"5d", "4h"},
        {"2c", "5d"},
        {"9c", "8c"},
        {"Tc", "7c"}
    };
    char communityCards[5][3] = {"8d", "9s", "As", "Jc", "Qc"};
    uint8_t int_player_hands[MAX_PLAYERS][2];
    uint8_t int_community_cards[5];
    for(uint32_t i = 0; i < noPlayers; i++)
    {
        int_player_hands[i][0] = get_card(playerHands[i][0]);
        int_player_hands[i][1] = get_card(playerHands[i][1]);
    }
    uint32_t stacks[MAX_PLAYERS] = {1000, 1000, 1000, 1000, 1000, 1000, 2000, 2000, 800};
    uint32_t total_stacks = 0;

    for(uint32_t i = 0; i < noPlayers; i++)
    {
        printf("Player %d starting stack:%d\n", i, stacks[i]);
        total_stacks += stacks[i];
    }
    printf("Total stacks: %d\n", total_stacks);
    for(int i = 0; i < 5; i++)
    {
        int_community_cards[i] = get_card(communityCards[i]);
    }

    print_hand_state(playerHands, communityCards, tables, noPlayers);

    int no_payouts = 3;
    float payouts[3] = {50, 30, 20};
    printf("Payouts: ");
    for(int i = 0; i < no_payouts; i ++)
    {
        printf("%f ",payouts[i]);
    }
    printf("\n");
    test_ev(tables,int_community_cards, int_player_hands, noPlayers, stacks, payouts, no_payouts);
    free_evaluator_tables(tables);
}

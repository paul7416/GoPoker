
#include<stdio.h>
#include<stdint.h>
#include "evaluator.h"
#include "player.h"
#include "ev.h"
#include "global_defines.h"
#include "debug.h"

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

    uint64_t evaluation = evaluateRound(&sim, T);
    //uint64_t evaluation = 2;
    uint64_t outcome = evaluation;
    printf("Outcome:%lx\n",outcome);
    char hole_cards_string[5];
    float ev[MAX_PLAYERS] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    analyse_pot(&G, evaluation, ev);
    hole_cards_string[4] = '\0';
    for(uint32_t i = 0; i < noPlayers; i++)
    {
        uint8_t player_outcome = (uint8_t)outcome & 0x3f;
        bool folded = (bool)(player_outcome & 0x20);
        uint8_t index = player_outcome & 0xf;
        bool tied = (bool)(player_outcome &0x10);
        get_hole_cards_string(hole_cards_string, player_cards[index][0], player_cards[index][1]);
        printf("rank: %d cards: %s index: %d folded: %d tied: %d\n", i, hole_cards_string, index, folded, tied );
        outcome = outcome >> 6;
    }
    for(uint32_t i = 0; i < noPlayers; i++)
    {
       printf("player %d ev: %f\n", i, ev[i]);
    }
}

int main(void)
{
    const uint32_t noPlayers = 6;
    const evaluatorTables *tables = import_evaluator_tables();
    char playerHands[MAX_PLAYERS][2][3] = 
    {
        {"Jd", "Qd"},
        {"Ks", "Kc"},
        {"Qs", "Js"},
        {"Ad", "Ac"},
        {"2c", "3c"},
        {"5d", "4h"},
        {"2c", "5d"},
        {"4c", "5c"},
        {"4c", "5c"}
    };
    char communityCards[5][3] = {"8d", "9s", "As", "Jc", "Qc"};
    uint8_t int_player_hands[MAX_PLAYERS][2];
    uint8_t int_community_cards[5];
    for(uint32_t i = 0; i < noPlayers; i++)
    {
        int_player_hands[i][0] = get_card(playerHands[i][0]);
        int_player_hands[i][1] = get_card(playerHands[i][1]);
        //printf("%s : %d\n", playerHands[i][0], int_player_hands[i][0]);
        //printf("%s : %d\n\n", playerHands[i][1], int_player_hands[i][1]);
    }
    uint32_t stacks[MAX_PLAYERS] = {1000, 1000, 1000, 1000, 1000, 1000, 2000, 2000, 2000};

    for(uint32_t i = 0; i < noPlayers; i++)
    {
        printf("Player %d starting stack:%d\n", i, stacks[i]);
    }
    printf("Community Cards: ");
    for(int i = 0; i < 5; i++)
    {
        int_community_cards[i] = get_card(communityCards[i]);
        printf("%s ", communityCards[i]);
    }
    printf("\n");
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

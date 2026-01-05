#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include "evaluator.h"
#include "global_defines.h"

void test_evaluator(evaluatorTables *tables)
{
    uint16_t *Flushes = tables->Flushes;
    uint64_t *Primes = tables->Primes;
    uint64_t *hashTable = tables->hashTable;
    uint32_t histogram[9] = {0};
    clock_t start = clock();
    for(int a=0; a < 52; a++){
        for(int b=a+1; b < 52; b++){
            for(int c=b+1; c < 52; c++){
                for(int d=c+1; d < 52; d++){
                    for(int e=d+1; e < 52; e++){
                        for(int f=e+1; f < 52; f++){
                            for(int g=f+1; g < 52; g++){
                                uint64_t bitmask = (1ll << a) |(1ll << b) |(1ll << c) |(1ll << d) |(1ll << e) |(1ll << f) |(1ll << g);
                                histogram[evaluateHand(bitmask, Flushes, Primes, hashTable)>>12]++;

                           }
                        }
                    }
                }
            }
        }
    }
    clock_t end = clock();
    double duration = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("Straight Flushes: %u\n",histogram[0]); //0
    printf("Four Of A Kind:   %u\n",histogram[1]); //1
    printf("Full House:       %u\n",histogram[2]); //2
    printf("Flushes:          %u\n",histogram[3]); //3
    printf("Straights:        %u\n",histogram[4]); //4
    printf("Three of a Kind:  %u\n",histogram[5]); //5
    printf("Two Pairs:        %u\n",histogram[6]); //6
    printf("Pairs:            %u\n",histogram[7]); //7
    printf("High Card:        %u\n",histogram[8]); //8
    int total = 0;
    for(int i = 0; i < 9; i++) total += histogram[i];
    printf("Total Hands:      %d\n", total);
    printf("Time taken: %fms\n", duration);
    printf("M Hands per second: %.3f\n", (double)total / 1000 / duration);
    printf("Hash Table Size:%dkB\n",HASH_TABLE_SIZE * 8 / 1024);
}
int get_rank(char c)
{
    char *ranks = "23456789TJQKA";
    for(int i = 0; i < 13; i++)
    {
        if (ranks[i] == c) return i;
    }
    return -1;
}
int get_suit(char c)
{
    char *suits = "hdcs";
    for(int i = 0; i < 4; i++)
    {
        if(suits[i] == c) return i;
    }
    return -1;
}
uint64_t get_card(char card_string[])
{
    int rank = get_rank(card_string[0]);
    int suit = get_suit(card_string[1]);
    return (1ll << (13 * suit + rank));
}
void test_evaluateRound(evaluatorTables *tables)
{
    uint64_t hole_cards[3];
    hole_cards[0] = get_card("Ks")|get_card("Td");
    hole_cards[1] = get_card("Ah")|get_card("Qh");
    hole_cards[2] = get_card("7d")|get_card("Kd");
    uint64_t board = 0;
    board |= get_card("Kc");
    board |= get_card("Kh");
    board |= get_card("6h");
    board |= get_card("Ts");
    board |= get_card("9h");
    uint32_t bets[3] = {1, 1, 1};
    uint8_t player_ids[3] = {1, 2, 3};
    int no_players = 3;
    uint64_t outcome = evaluateRound(board, hole_cards, bets, player_ids, no_players, tables);
    playerResult results[MAX_PLAYERS];
    (void)decodeOutcomes(outcome, results);
    printf("NO players %d\n", no_players);
    
    for(int i = 0; i < no_players; i++)
    {
        printf("Player Id:%d  Folded:%d  Tied:%d\n", results[i].player_id, results[i].folded, results[i].tied);
    }
    printf("\n");
    
    

}
int main()
{
    evaluatorTables *tables = import_evaluator_tables();
    //test_evaluator(tables);
    test_evaluateRound(tables);
    free_evaluator_tables(tables);
}

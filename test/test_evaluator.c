#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include "evaluator.h"
#include "global_defines.h"

int card_values[52] = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
                        16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28,
                        32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
                        48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60};
                    
static inline int get_card_no(int i)
{
    return card_values[i];

}
void test_evaluator(const evaluatorTables *tables)
{
    const uint16_t *Flushes = tables->Flushes;
    const uint32_t *Primes = tables->Primes;
    const uint64_t *hashTable = tables->hashTable;
    const uint16_t *directLookup = tables->directLookup;
    uint32_t histogram[9] = {0};
    clock_t start = clock();
    for(int a=0; a < 52; a++){
        int ai = get_card_no(a);
        for(int b=a+1; b < 52; b++){
            int bi = get_card_no(b);
            for(int c=b+1; c < 52; c++){
                int ci = get_card_no(c);
                for(int d=c+1; d < 52; d++){
                    int di = get_card_no(d);
                    for(int e=d+1; e < 52; e++){
                        int ei = get_card_no(e);
                        for(int f=e+1; f < 52; f++){
                            int fi = get_card_no(f);
                            for(int g=f+1; g < 52; g++){
                                int gi = get_card_no(g);
                                uint64_t bitMask = (1ull << ai)|(1ull << bi)|(1ull << ci)|(1ull << di)|(1ull << ei)|(1ull << fi)|(1ull << gi);
                                histogram[evaluateHand(bitMask, Flushes, Primes, hashTable, directLookup)>>12]++;

                           }
                        }
                    }
                }
            }
        }
    }
    clock_t end = clock();
    double duration = (double)(end - start) * 1000 / CLOCKS_PER_SEC;


    const uint32_t golden_histogram[9] = {
        41584,      // Straight Flush
        224848,     // Four Of A Kind
        3473184,    // Full House
        4047644,    // Flush
        6180020,    // Straight
        6461620,    // Three of a Kind
        31433400,   // Two Pairs
        58627800,   // Pairs
        23294460    // High Card
    };
    const char *names[9] = {
        "Straight Flush", "Four Of A Kind", "Full House", "Flush",
        "Straight", "Three of a Kind", "Two Pairs", "Pairs", "High Card"
    };
    
    int total = 0;
    for(int i = 0; i < 9; i++) total += histogram[i];

    int fail = 0;
    for(int i=0; i<9; i++){
        printf("%-16s: %u\n", names[i], histogram[i]);
        if(histogram[i] != golden_histogram[i]){
            fprintf(stderr, "\033[31mERROR: %s count mismatch! Got %d, Expected %u\033[0m\n", names[i], histogram[i], golden_histogram[i]);
            fail = 1;
        }
    }
    if(total != 133784560){
        fprintf(stderr, "\033[31mERROR: Total hands mismatch! Got %d, expected 133784560\033[0m\n", total);
        fail = 1;
    }
    
    if(fail){
        fprintf(stderr,"Evaluator test FAILED!\n");
    }else{
        printf("Evaluator test PASSED ✅\n");
    }

    printf("Time taken: %fms\n", duration);
    printf("M Hands per second: %.3f\n", (double)total / 1000 / duration);
    printf("Hash Table Size:%dkB\n",HASH_TABLE_SIZE * 8 / 1024);
    printf("Direct Lookup Size:%dkB\n",DIRECT_LOOKUP_SIZE * 2 / 1024);
//    for(int i =0; i < 50; i++)
//    {
//        printf("%d probes:%d\n",i,probe_histogram[i]);
//
//    }
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
int main()
{
    const evaluatorTables *tables = import_evaluator_tables();
    test_evaluator(tables);
    //test_evaluateRound(tables);
    free_evaluator_tables(tables);
}

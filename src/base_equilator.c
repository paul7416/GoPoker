#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<time.h>
#include<string.h>

#define ARRAY_SIZE 0x2000
#define HASH_TABLE_SIZE 0x80000
#define MAX_PLAYERS 16
typedef struct{
    uint16_t *Flushes;
    uint64_t *Primes;
    uint64_t *hashTable;
    uint64_t *_52C5;
    uint64_t *_52C3;
    uint64_t *_52C2;
}evaluatorTables;



uint16_t *import_flushes()
{
    FILE *f = fopen("./DataFiles/flush_ranks.bin", "rb");
    if(f == NULL)
    {
        printf("Flush import failed\n");
        return NULL;
    }

    uint16_t *Flushes = aligned_alloc(64, sizeof(uint16_t) * ARRAY_SIZE);
    if(Flushes == NULL)
    {
        printf("Failed to allocate Flushes Memory\n");
        fclose(f);
        return NULL;
    }
    fread(Flushes, sizeof(uint16_t), ARRAY_SIZE, f);
    fclose(f);
    return Flushes;
}

uint64_t *import_primes()
{
    FILE *f = fopen("./DataFiles/primes.bin", "rb");
    if(f == NULL)
    {
        printf("Primes import failed\n");
        return NULL;
    }
    uint64_t *Primes = aligned_alloc(64, sizeof(uint64_t) * ARRAY_SIZE);
    if(Primes == NULL)
    {
        printf("primes import failed\n");
        fclose(f);
        return NULL;
    }
    fread(Primes, sizeof(uint64_t), ARRAY_SIZE, f);
    fclose(f);
    return Primes;
}

void *import_dat_file(char filename[], uint32_t *count, size_t element_size){
    FILE *f = fopen(filename, "rb");
    if(f == NULL)
    {
        printf("File %s failed to open\n", filename);
        return NULL;
    }
    fread(count, sizeof(uint32_t), 1, f);
    void *buffer = malloc((*count) * element_size);
    if(buffer == NULL)
    {
        printf("%s buffer failed to allocate\n", filename);
        fclose(f);
        return NULL;
    }
    size_t read = fread(buffer, element_size, *count, f);
    if(read != *count)
    {
        printf("header data specified length of %d but only %ld read\n", *count, read);
    }
    fclose(f);
    return buffer;
}

uint64_t *import_primes_dict(){
    uint32_t count;
    uint64_t *buffer = import_dat_file("./DataFiles/seven_rank_dict.bin", &count, sizeof(uint64_t));
    uint64_t *hash_table = aligned_alloc(64, HASH_TABLE_SIZE * sizeof(uint64_t));
    if(!hash_table)
    {
        printf("hash table failed to allocate\n");
        exit(1);
    }
    if (hash_table) memset(hash_table, 0, HASH_TABLE_SIZE * sizeof(uint64_t));
    for(uint32_t i = 0; i < count; i++)
    {
        int index = (buffer[i] >> 16) & (HASH_TABLE_SIZE - 1);
        while(hash_table[index] != 0)
        {
            index = (index + 1)& (HASH_TABLE_SIZE - 1);
        }
        hash_table[index] = buffer[i];
    }
    free(buffer);
    return hash_table;
}

evaluatorTables *import_tables()
{
    uint32_t i;
    evaluatorTables *eval_tables = malloc(sizeof(evaluatorTables));
    eval_tables->Flushes = import_flushes();
    printf("Imported Flushes\n");
    eval_tables->Primes = import_primes();
    printf("Imported Primes\n");
    eval_tables->hashTable = import_primes_dict();
    printf("Imported hashTable\n");
    eval_tables->_52C5 = import_dat_file("./DataFiles/52C5.bin", &i, sizeof(uint64_t));
    printf("Imported 52C5\n");
    eval_tables->_52C3 = import_dat_file("./DataFiles/52C3.bin", &i, sizeof(uint64_t));
    printf("Imported 52C3\n");
    eval_tables->_52C2 = import_dat_file("./DataFiles/52C2.bin", &i, sizeof(uint64_t));
    printf("Imported 52C2\n");
    printf("tables imported\n");
    return eval_tables;
}

void free_tables(evaluatorTables *tables)
{
    free(tables->Flushes);
    free(tables->Primes);
    free(tables->hashTable);
    free(tables->_52C5);
}

void print_bin(uint64_t mask)
{
    for(int i = 51; i >= 0; i--)
    {
        printf("%ld",(mask >> i)&1);
        if (i%13 == 0)printf("|");
    }

}
uint16_t hashLookup(uint64_t prime, uint64_t *hash_table)
{
    int index = prime & (HASH_TABLE_SIZE - 1);
    #ifdef DEBUG
    int probes = 0;
    #endif
    while((hash_table[index] >> 16) != prime)
    {
        index = (index + 1) & (HASH_TABLE_SIZE - 1);
        #ifdef DEBUG
        if(++probes > HASH_TABLE_SIZE) {
            fprintf(stderr, "hashLookup: prime %lu not found\n", prime);
            abort();
        }
        #endif
    }
    return (hash_table[index] & 0xffff);
}

//__attribute__((noinline))
uint16_t solveHand(uint64_t bitMask, uint16_t *Flushes, uint64_t *Primes, uint64_t *hashTable)
{
    uint16_t heartsMask = (bitMask & 0x1fff);
    uint16_t diamondsMask = (bitMask >> 13) & 0x1fff;
    uint16_t clubsMask = (bitMask >> 26) & 0x1fff;
    uint16_t spadesMask = (bitMask >> 39) & 0x1fff;
    
    uint16_t flushScore = 
            Flushes[heartsMask] + 
            Flushes[diamondsMask] + 
            Flushes[clubsMask] + 
            Flushes[spadesMask];
    if (flushScore != 0) return flushScore;
    uint64_t prime = Primes[heartsMask] * Primes[diamondsMask] * Primes[clubsMask] * Primes[spadesMask];
    return hashLookup(prime, hashTable);
}

void update_player_probabilities(int no_players, uint64_t *hole_cards, double *probabilities, uint64_t board, uint16_t *Flushes, uint64_t *Primes, uint64_t *hashTable)
{
    uint16_t scores[MAX_PLAYERS];
    uint16_t min_score = 0xffff, score;
    int ties = 0;
    for(int i = 0; i < no_players; i++)
    {
        score = solveHand(hole_cards[i]|board, Flushes, Primes, hashTable);
        if (score < min_score)
        {
            min_score = score;
            ties = 1;
        }
        else if (score == min_score)
        {
            ties++;
        }
        scores[i] = score;
    }
    double win_value = (double)1 / ties;
    for(int i = 0; i < no_players; i++)
    {
        if(scores[i] == min_score) probabilities[i] += win_value;
    }
}

void enumerate_preflop_prob(int no_players, uint64_t *hole_cards, double *probabilities, evaluatorTables *tables)
{
    if(no_players > MAX_PLAYERS)
    {
        printf("Error %d players at table is greter than the maximum of %d\n", no_players, MAX_PLAYERS);
        exit(1);
    }

    uint64_t playerMasks = 0, board_mask;//, m1, m2, m3, m4, m5, board_mask;
    uint16_t *Flushes = tables->Flushes;
    uint64_t *Primes = tables->Primes;
    uint64_t *hashTable = tables->hashTable;
    uint64_t *combinations = tables->_52C5;
    int count = 0;

    for(int i = 0; i < no_players; i++)
    {
        if (playerMasks & hole_cards[i])
        {
            printf("Hole Card Clash");
            exit(1);
        }
        playerMasks |= hole_cards[i];
        probabilities[i] = 0.0;
    }
    for(int i = 0; i < 2598960; i++)
    {
        board_mask = combinations[i];
        if (board_mask & playerMasks)continue;
        update_player_probabilities(no_players, hole_cards, probabilities, board_mask, Flushes, Primes, hashTable);
        count++;
    }

    for(int i = 0; i < no_players; i++)
    {
        probabilities[i] /= count;
    }
}

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
                                histogram[solveHand(bitmask, Flushes, Primes, hashTable)>>12]++;

                           }
                        }
                    }
                }
            }
        }
    }
    clock_t end = clock();
    double duration = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("Straight Flushes: %u\n",histogram[0]);
    printf("Four Of A Kind:   %u\n",histogram[1]);
    printf("Full House:       %u\n",histogram[2]);
    printf("Flushes:          %u\n",histogram[3]);
    printf("Straights:        %u\n",histogram[4]);
    printf("Three of a Kind:  %u\n",histogram[5]);
    printf("Two Pairs:        %u\n",histogram[6]);
    printf("Pairs:            %u\n",histogram[7]);
    printf("High Card:        %u\n",histogram[8]);
    int total = 0;
    for(int i = 0; i < 9; i++) total += histogram[i];
    printf("Total Hands:      %d\n", total);
    printf("Time taken: %fms\n", duration);
    printf("M Hands per second: %.3f\n", (double)total / 1000 / duration);
    printf("Hash Table Size:%dMB\n",HASH_TABLE_SIZE * 8 / 1024 / 1024);
}
int get_rank(char c)
{
    char ranks[] = "23456789TJQKA";
    for(int i = 0; i < 13; i++)
        if (ranks[i] == c){
            return i;
        }
    printf("Error rank %c does not exist\n",c);
    exit(1);
}
int get_suit(char c)
{
    char suits[] = "hdcs";
    for(int i = 0; i < 4; i++)
        if (suits[i] == c) return i;
    printf("Error suit %c does not exist\n",c);
    exit(1);
}

uint64_t get_hole_card_mask(char hand_string[5])
{
    int r1 = get_rank(hand_string[0]);
    int r2 = get_rank(hand_string[2]);
    int s1 = get_suit(hand_string[1]);
    int s2 = get_suit(hand_string[3]);
    uint64_t hand_mask = (1ll << (r1 + 13*s1)) | (1ll << (r2 + 13*s2));

    return hand_mask;
}
void test_enumerate_preflop_prob(evaluatorTables *tables)
{
    double probabilities[MAX_PLAYERS];
    uint64_t hole_cards[MAX_PLAYERS];
    char player_1[] = "AhAc";
    char player_2[] = "AdKd";
    char player_3[] = "QdAs";
    hole_cards[0] = get_hole_card_mask(player_1);
    hole_cards[1] = get_hole_card_mask(player_2);
    hole_cards[2] = get_hole_card_mask(player_3);
        
    clock_t start = clock();
    enumerate_preflop_prob(3, hole_cards, probabilities, tables);
    printf("Player 1 %s: %0.3f\nPlayer 2 %s: %0.3f\nPlayer 3 %s: %0.3f\n", player_1, probabilities[0], player_2, probabilities[1], player_3, probabilities[2]);
    clock_t end = clock();
    double duration = (double)(end - start) * 1000 / CLOCKS_PER_SEC;
    printf("3 player full enumeration in %f.0 ms\n", duration);

}


int main()
{
    evaluatorTables *tables = import_tables();
    //test_enumerate_preflop_prob(tables);
    test_evaluator(tables);
    free_tables(tables);
}

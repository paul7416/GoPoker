#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    srand(time(NULL));
    FILE *f = fopen("random_hands.bin", "wb");
    
    int hands = 10000000;
    fwrite(&hands, sizeof(int), 1, f);
    
    for (int i = 0; i < hands; i++) {
        int deck[52];
        for (int j = 0; j < 52; j++) deck[j] = j;
        
        // Fisher-Yates shuffle first 7
        for (int j = 0; j < 7; j++) {
            int k = j + rand() % (52 - j);
            int tmp = deck[j];
            deck[j] = deck[k];
            deck[k] = tmp;
        }
        
        fwrite(deck, sizeof(int), 7, f);
    }
    
    fclose(f);
    return 0;
}

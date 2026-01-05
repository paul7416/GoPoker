#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

void write_file(char *filename, size_t element_size, int no_elements, void *pointer)
{
    FILE *f = fopen(filename, "wb");
    if(f == NULL)
    {
        printf("Failed to write file:%s\n",filename);
        exit(1);
    }
    fwrite(&no_elements, sizeof(int), 1, f);
    fwrite(pointer, element_size, no_elements, f);
}
void create_52C5()
{
    int table_size = 2598960; // 52 Choose 5
    uint64_t *output_data = malloc(table_size * sizeof(uint64_t));
    uint64_t *p = output_data;
    for(int a = 0; a < 52; a++){
        for(int b = a + 1; b < 52; b++){
            for(int c = b + 1; c < 52; c++){
                for(int d = c + 1; d < 52; d++){
                    for(int e = d + 1; e < 52; e++){
                        int suit_counts[4]={0};
                        suit_counts[a/13]++;
                        suit_counts[b/13]++;
                        suit_counts[c/13]++;
                        suit_counts[d/13]++;
                        suit_counts[e/13]++;
                        uint64_t no_flush_bit = 1ull << 53;
                        if((suit_counts[0] >= 3) || suit_counts[1] >= 3 || suit_counts[2] >= 3 || suit_counts[3] >= 3){
                            no_flush_bit = 0;
                        }
                        *p = (1ull << a)|(1ull << b)|(1ull << c)|(1ull << d)|(1ull << e)|no_flush_bit;
                        p++;
                    }
                }
            }
        }
    }
    write_file("DataFiles/52C5.bin", sizeof(uint64_t), table_size, output_data);
}
void create_52C3()
{
    int table_size = 22100; // 52 Choose 3
    uint64_t *output_data = malloc(table_size * sizeof(uint64_t));
    uint64_t *p = output_data;
    for(int a = 0; a < 52; a++){
        for(int b = a + 1; b < 52; b++){
            for(int c = b + 1; c < 52; c++){
                *p = (1ll << a)|(1ll << b)|(1ll << c);
                p++;
            }
        }
    }
    write_file("DataFiles/52C3.bin", sizeof(uint64_t), table_size, output_data);
}
void create_52C2()
{
    int table_size = 1326; // 52 Choose 2
    uint64_t *output_data = malloc(table_size * sizeof(uint64_t));
    uint64_t *p = output_data;
    for(int a = 0; a < 52; a++){
        for(int b = a + 1; b < 52; b++){
            *p = (1ll << a)|(1ll << b);
            p++;
            }
        }
    write_file("DataFiles/52C2.bin", sizeof(uint64_t), table_size, output_data);
}


int main() {
    create_52C2();
    create_52C3();
    create_52C5();
}

#include<immintrin.h>
#include "global_defines.h"

#define DECK_SIZE 52
#ifdef __AVX2__
    typedef __m256i dec_vec;
    #define and_vec _mm256_and_si256
    #define or_vec _mm256_or_si256
    #define vec_set1_epi8 _mm256_set1_epi8
    #define vec_set1_epi16 _mm256_set1_epi16
    #define vec_slli_epi16 _mm256_slli_epi16
    #define vec_srli_epi16 _mm256_srli_epi16
    #define vec_cmpeq_epi8 _mm256_cmpeq_epi8
    #define vec_unpacklo_epi8 _mm256_unpacklo_epi8
    #define vec_unpackhi_epi8 _mm256_unpackhi_epi8
    #define CONCURRENT_DECKS 32
#else
    typedef __m128i dec_vec;
    #define and_vec _mm_and_si128
    #define or_vec _mm_or_si128
    #define vec_set1_epi8 _mm_set1_epi8
    #define vec_set1_epi16 _mm_set1_epi16
    #define vec_slli_epi16 _mm_slli_epi16
    #define vec_srli_epi16 _mm_srli_epi16
    #define vec_cmpeq_epi8 _mm_cmpeq_epi8
    #define vec_unpacklo_epi8 _mm_unpacklo_epi8
    #define vec_unpackhi_epi8 _mm_unpackhi_epi8
    #define CONCURRENT_DECKS 16
#endif

union deckEntry
{
    dec_vec vectors;
    uint8_t cards[CONCURRENT_DECKS];
};

union handIndexEntry
{
    dec_vec vectors[2];  // Two vectors to hold 32x or 16x 16-bit values
    uint16_t indices[CONCURRENT_DECKS];
};

typedef struct __attribute__((aligned(64)))
{
    union deckEntry data[DECK_SIZE];
    union handIndexEntry hand_indices[MAX_PLAYERS];
    uint64_t s[2];
    uint8_t current_index;
    uint8_t number_of_shuffled_cards;
    uint8_t no_players;
    uint8_t deck_size;
}cardDeck;

cardDeck create_card_deck(uint8_t no_players, uint8_t deck_size);
void initialization_shuffle(cardDeck *d);
void shuffle_deck(cardDeck *d);

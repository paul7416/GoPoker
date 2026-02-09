#include <stdint.h>
#include <sys/random.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "deck.h"
//
// Xorshift128+ generator
static inline uint64_t xorshift128plus(uint64_t s[2]) {
    uint64_t x = s[0];
    uint64_t y = s[1];
    s[0] = y;
    x ^= x << 23;
    s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
    return s[1] + y;
}
void seed(cardDeck *d)
{
    uint64_t *s = d->s;
    assert(getrandom(s, 16, 0) == 16);
    for( int i = 0; i < 20; i++)xorshift128plus(s);
}

void initialization_shuffle(cardDeck *d){
    seed(d);
    for(int deck_no = 0; deck_no < CONCURRENT_DECKS; deck_no++)
    {
        uint8_t tmp;
        for(uint8_t i = 0; i < d->deck_size - 1; i++)
        {
            uint32_t rnd_index = (uint32_t)((((uint64_t)((uint32_t)xorshift128plus(d->s)) * (d->deck_size - i)) >> 32) + i);
            tmp = d->data[i].cards[deck_no];
            d->data[i].cards[deck_no] = d->data[rnd_index].cards[deck_no];
            d->data[rnd_index].cards[deck_no] = tmp;
        }
    }
}

cardDeck create_card_deck(uint8_t no_players, uint8_t deck_size)
{
    cardDeck d;
    d.deck_size = deck_size;
    d.current_index = 0;
    d.no_players = no_players;
    d.number_of_shuffled_cards = (uint8_t)(no_players * 2 + 5);
    for(int deck_number = 0; deck_number < CONCURRENT_DECKS; deck_number++)
    {
        for(int i = 0; i < DECK_SIZE; i++)
        {
            int suit = i / 13;
            int rank = i % 13;
            int card_int = (suit << 4) | rank;
            d.data[i].cards[deck_number] = (uint8_t)card_int;
        }
    }
    return d;
}

__attribute__((noinline))
void shuffle_deck(cardDeck *d)
{
    dec_vec a;
    uint64_t *s = d->s;
    union deckEntry *arr = d->data;

    for(uint32_t i = 0; i < d->number_of_shuffled_cards; i++)
    {
        uint32_t rnd_index = (uint32_t)((((uint64_t)((uint32_t)xorshift128plus(s)) * (DECK_SIZE - i)) >> 32) + i);
        a = arr[i].vectors;
        arr[i].vectors = arr[rnd_index].vectors;
        arr[rnd_index].vectors = a;
    }

    for(uint8_t player = 0, deck_index = 5; deck_index < d->number_of_shuffled_cards; player++, deck_index += 2)
    {
        // load vectors for this player's two cards
        dec_vec card_1_vector = d->data[deck_index].vectors;
        dec_vec card_2_vector = d->data[deck_index + 1].vectors;

        // extract ranks
        dec_vec rank1 = and_vec(card_1_vector, vec_set1_epi8(0x0F));
        dec_vec rank2 = and_vec(card_2_vector, vec_set1_epi8(0x0F));

        // extract suits (shift right by 4, then mask)
        dec_vec suit1 = and_vec(vec_srli_epi16(card_1_vector, 4), vec_set1_epi8(0x0F));
        dec_vec suit2 = and_vec(vec_srli_epi16(card_2_vector, 4), vec_set1_epi8(0x0F));

        // calculate hand_types: (rank1 << 4) | rank2
        dec_vec hand_types = or_vec(vec_slli_epi16(rank1, 4), rank2);

        // calculate suited flags (0x01 if suited, 0x00 if not)
        dec_vec suited_flags = and_vec(vec_cmpeq_epi8(suit1, suit2), vec_set1_epi8(0x01));

        // Combine into 16-bit values: hand_types in low byte, suited in high byte
        // unpacklo/hi interleaves bytes: result is [hand_types[0], suited[0], hand_types[1], suited[1], ...]
        dec_vec combined_lo = vec_unpacklo_epi8(hand_types, suited_flags);
        dec_vec combined_hi = vec_unpackhi_epi8(hand_types, suited_flags);

        // Store both halves as 16-bit indices
        d->hand_indices[player].vectors[0] = combined_lo;
        d->hand_indices[player].vectors[1] = combined_hi;
    }
}

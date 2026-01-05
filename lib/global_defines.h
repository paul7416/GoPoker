#define MAX_PLAYERS 16
#define NO_FLUSH_BIT (1ull<<53)
#define GET_HEARTS_MASK(b)   ((b) & 0x1fff)
#define GET_DIAMONDS_MASK(b) (((b) >> 13) & 0x1fff)
#define GET_CLUBS_MASK(b)    (((b) >> 26) & 0x1fff)
#define GET_SPADES_MASK(b)   (((b) >> 39) & 0x1fff)
//Scores: lower is better 0x0001 = Royal Flush, 0xFFFF = folded / worst
#define SCORE_BITS 16
#define HIGHEST_SCORE ((1 << SCORE_BITS) - 1)

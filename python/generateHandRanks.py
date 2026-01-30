import struct
import json
# hands from  https://www.preflophands.com/
hands = [
         "AA",  "KK",  "QQ", "AKs",  "JJ",
        "AQs", "KQs", "AJs", "KJs",  "TT",
        "AKo", "ATs", "QJs", "KTs", "QTs",
        "JTs",  "99", "AQo", "A9s", "KQo",
         "88", "K9s", "T9s", "A8s", "Q9s",
        "J9s", "AJo", "A5s",  "77", "A7s",
        "KJo", "A4s", "A3s", "A6s", "QJo",
         "66", "K8s", "T8s", "A2s", "98s",
        "J8s", "ATo", "Q8s", "K7s", "KTo",
         "55", "JTo", "87s", "QTo",  "44",
         "33",  "22", "K6s", "97s", "K5s",
        "76s", "T7s", "K4s", "K3s", "K2s",
        "Q7s", "86s", "65s", "J7s", "54s",
        "Q6s", "75s", "96s", "Q5s", "64s",
        "Q4s", "Q3s", "T9o", "T6s", "Q2s",
        "A9o", "53s", "85s", "J6s", "J9o",
        "K9o", "J5s", "Q9o", "43s", "74s",
        "J4s", "J3s", "95s", "J2s", "63s",
        "A8o", "52s", "T5s", "84s", "T4s",
        "T3s", "42s", "T2s", "98o", "T8o",
        "A5o", "A7o", "73s", "A4o", "32s",
        "94s", "93s", "J8o", "A3o", "62s",
        "92s", "K8o", "A6o", "87o", "Q8o",
        "83s", "A2o", "82s", "97o", "72s",
        "76o", "K7o", "65o", "T7o", "K6o",
        "86o", "54o", "K5o", "J7o", "75o",
        "Q7o", "K4o", "K3o", "96o", "K2o",
        "64o", "Q6o", "53o", "85o", "T6o",
        "Q5o", "43o", "Q4o", "Q3o", "74o",
        "Q2o", "J6o", "63o", "J5o", "95o",
        "52o", "J4o", "J3o", "42o", "J2o",
        "84o", "T5o", "T4o", "32o", "T3o",
        "73o", "T2o", "62o", "94o", "93o",
        "92o", "83o", "82o", "72o"
        ]
def get_16_bit_hand_no(card):
    suit = card // 13
    rank = card % 13
    return suit * 16 + rank
def add_frequency(hand, frequencies):
    if hand not in frequencies:
        frequencies[hand] = 1
    else:
        frequencies[hand] +=1

ranks = "23456789TJQKA"
frequencies = {}
output_list = [255] * 0x4000
for i in range(52):
    for j in range(i+1, 52):
        hand_ranks = sorted([i%13, j%13], reverse=True)
        if hand_ranks[0] == hand_ranks[1]:
            modifier = ""
        elif i//13 == j//13:
            modifier = "s"
        else:
            modifier = "o"
        card_string = f"{ranks[hand_ranks[0]]}{ranks[hand_ranks[1]]}{modifier}"
        hand_score = hands.index(card_string)
        output_list[get_16_bit_hand_no(i)<<8|get_16_bit_hand_no(j)] = hand_score
        output_list[get_16_bit_hand_no(j)<<8|get_16_bit_hand_no(i)] = hand_score
        add_frequency(card_string, frequencies)
pairs = sorted([(key, value) for key, value in frequencies.items()],key = lambda x: x[0])
for p in pairs:
    print(p)
for index, value in enumerate(output_list):
    if value==255:
        print(index & 0x3f,(index>>6)&0x3f)
num_elements = len(output_list)
header = struct.pack("<I", num_elements)
byte_data = bytes(output_list)
filename = "DataFiles/holeCardHandRanks.dat"
with open(filename, "wb") as f:
        f.write(header + byte_data)
print(f"Output written to {filename}")



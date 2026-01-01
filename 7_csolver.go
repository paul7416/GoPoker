package main
import (
	"fmt"
    "runtime/pprof"
    "os"
    "encoding/json"
    "strings"
)
var flushes[0x2000]uint16
var noPairs[0x2000]uint16
var noPairs7[0x2000]uint16
var rank_dict map[uint32]uint16
var seven_rank_dict map[uint64]uint16
var primes = [13]uint64{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41}

type Card struct{
    bitMask uint64
    prime uint64
}

type Tables struct{
    FlushRanks         []uint16           `json:"flush_ranks"`
    StraightRanks      []uint16           `json:"straight_ranks"`
    SevenStraightRanks []uint16           `json:"seven_straight_ranks"`
    RankDict           map[uint32]uint16  `json:"rank_dict"`
    SevenRankDict      map[uint64]uint16  `json:"seven_rank_dict"`
}
var deck [52]Card
const tableSize = 131072

var hashTable [tableSize]uint64

func initHashTable() {
    for key, value := range seven_rank_dict {
        index := key & (tableSize - 1)
        for hashTable[index] != 0 {
            index = (index + 1) & (tableSize - 1)
        }
        hashTable[index] = (uint64(value) << 38) | (uint64(key) & 0x3fffffffff)
    }
}

func hashLookup(prime uint64) uint16 {
    index := prime & (tableSize - 1)
    for hashTable[index] & 0x3fffffffff != prime {
        index = (index + 1) & (tableSize - 1)
    }
    return uint16(hashTable[index] >> 38)
}

func initDeck() {
    for i := 0; i < 52; i++ {
        rank := i % 13
        deck[i] = Card{
            bitMask: uint64(1) << (i),
            prime:   primes[rank],
        }
    }
}

func importTables(filename string) error {
    data, err := os.ReadFile(filename)
    if err != nil {
        return err
    }

    var tables Tables
    err = json.Unmarshal(data, &tables)
    if err != nil {
        return err
    }

    copy(flushes[:], tables.FlushRanks)
    copy(noPairs[:], tables.StraightRanks)
    copy(noPairs7[:], tables.SevenStraightRanks)
    seven_rank_dict = tables.SevenRankDict
    rank_dict = tables.RankDict

    return nil
}


func getHandType(rank uint16) string{
    if rank <= 10 {return "Straight Flush"}
    if rank <= 166 {return "Quads"}
    if rank <= 322 {return "Full House"}
    if rank <= 1599 {return "Flush"}
    if rank <= 1609 {return "Straight"}
    if rank <= 2467 {return "Trips"}
    if rank <= 3325 {return "Two Pair"}
    if rank <= 6185 {return "Pair"}
    return "High Card"
}

func getCard(cardStr string) Card{
    var ranks string = "23456789TJQKA"
    var suits string = "hdcs"
    rank := strings.Index(ranks, string(cardStr[0]))
    suit := strings.Index(suits, string(cardStr[1]))
    bitMask := uint64(1) << (rank + 13 * suit)
    card := Card{
        bitMask: bitMask,
        prime: primes[rank],
    }
    return card
}

func solve5CHand(hand *[7]Card, indices *[5]int) uint16{
    var bitMask uint64 = 0
    var prime uint32 = 1

    for _, index := range indices{
        bitMask |= hand[index].bitMask
    }
    score := noPairs[(bitMask | bitMask >> 13 | bitMask >> 26 | bitMask >>39) & 0x1fff]
    if score != 0{
        return score
    }

    for _, index := range indices{
        prime *= uint32(hand[index].prime)
    }
    return rank_dict[prime]
}

func solve7CHand(hand7C *[7]Card) uint16{
    bitMask := hand7C[0].bitMask | hand7C[1].bitMask | hand7C[2].bitMask | hand7C[3].bitMask | hand7C[4].bitMask | hand7C[5].bitMask | hand7C[6].bitMask
    heartsMask := (bitMask & 0x1fff)
    diamondsMask := (bitMask >> 13) & 0x1fff
    clubsMask := (bitMask >> 26) & 0x1fff
    spadesMask := (bitMask >> 39) & 0x1fff

    flushScore := 
            flushes[heartsMask] + 
            flushes[diamondsMask] + 
            flushes[clubsMask] + 
            flushes[spadesMask] 

    if flushScore != 0 {return flushScore}
    
    rankMask := uint16(heartsMask|diamondsMask|clubsMask|spadesMask)
    score := noPairs7[rankMask]
    if score != 0{
        return score
    }
    prime := hand7C[0].prime * hand7C[1].prime * hand7C[2].prime * hand7C[3].prime * hand7C[4].prime * hand7C[5].prime * hand7C[6].prime  
    return hashLookup(prime)


}
func test(){
    var histogram map[string]int
    histogram = make(map[string]int)
    var scores = [7463]int{}
    var hand7c [7]Card
    for a:= 0; a < 52; a++{
        for b:= a +1; b < 52; b++{
            for c:= b +1; c < 52; c++{
                for d:= c +1; d < 52; d++{
                    for e:= d +1; e < 52; e++{
                        for f:= e +1; f < 52; f++{
                            for g:= f +1; g < 52; g++{
                                hand7c[0] = deck[a]
                                hand7c[1] = deck[b]
                                hand7c[2] = deck[c]
                                hand7c[3] = deck[d]
                                hand7c[4] = deck[e]
                                hand7c[5] = deck[f]
                                hand7c[6] = deck[g]
                                scores[solve7CHand(&hand7c)]++
                            }
                        }
                    }
                }
            }
        }
    }
    for score, quantity := range scores{
        histogram[getHandType(uint16(score))] += quantity
    }
    for key, value := range histogram{
        fmt.Printf("%s: %d\n", key, value)
    }
}

func main(){
    f, _ := os.Create("cpu.prof")
    pprof.StartCPUProfile(f)
    defer pprof.StopCPUProfile()
    err := importTables("tables.json")
    if err != nil{
        panic(err)
    }
    initHashTable()
    initDeck()
    test()
//    
//    args := os.Args[1:]
//    if len(args) != 7{
//        fmt.Println("Usage 5_csolver Ah Kd Qc Js Th")
//        os.Exit(1)
//    }
//    var hand [7]Card
//    for i, arg := range args{
//        hand[i] = getCard(arg)
//    }
//    rank := solve7CHand(hand)
//    fmt.Printf("Hand Type: %s\nHand Rank: %d\n",getHandType(rank), rank)
}

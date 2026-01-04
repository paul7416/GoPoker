package main
import (
	"fmt"
    "os"
    "encoding/json"
    "strings"
)
var flush_ranks[0x2000]uint16
var straight_ranks[0x2000]uint16
var rank_dict map[uint32]uint16
var primes = [13]uint8{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41}

type Tables struct{
    FlushRanks    []uint16           `json:"flush_ranks"`
    StraightRanks []uint16           `json:"straight_ranks"`
    RankDict      map[uint32]uint16  `json:"rank_dict"`
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

    copy(flush_ranks[:], tables.FlushRanks)
    copy(straight_ranks[:], tables.StraightRanks)
    rank_dict = tables.RankDict

    return nil
}


type Card struct{
    rank_mask uint16
    suit_mask uint8
    prime uint8
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
    rank := uint8(strings.Index(ranks, string(cardStr[0])))
    suit := strings.Index(suits, string(cardStr[1]))
    card := Card{
        rank_mask: (1 << rank),
        suit_mask: (1 << suit),
        prime: primes[rank],
    }
    return card
}

func solve5CHand(hand [5]Card) uint16{
    var hand_rank_mask uint16 = 0
    var suit_mask uint8 = 0xff
    var prime uint32 = 1

    for _, card := range hand{
        suit_mask &= card.suit_mask
        hand_rank_mask |= card.rank_mask
        prime *= uint32(card.prime)
    }
    if(suit_mask != 0){
        return flush_ranks[hand_rank_mask]
    }
    straight_rank := straight_ranks[hand_rank_mask]
    if(straight_rank != 0){
        return straight_rank
    }
    return rank_dict[prime]
}

func solve7CHand(hand7C [7]Card) uint16{
    permutations :=  [21][5]int{
            {0, 1, 2, 3, 4},
            {0, 1, 2, 3, 5},
            {0, 1, 2, 3, 6},
            {0, 1, 2, 4, 5},
            {0, 1, 2, 4, 6},
            {0, 1, 2, 5, 6},
            {0, 1, 3, 4, 5},
            {0, 1, 3, 4, 6},
            {0, 1, 3, 5, 6},
            {0, 1, 4, 5, 6},
            {0, 2, 3, 4, 5},
            {0, 2, 3, 4, 6},
            {0, 2, 3, 5, 6},
            {0, 2, 4, 5, 6},
            {0, 3, 4, 5, 6},
            {1, 2, 3, 4, 5},
            {1, 2, 3, 4, 6},
            {1, 2, 3, 5, 6},
            {1, 2, 4, 5, 6},
            {1, 3, 4, 5, 6},
            {2, 3, 4, 5, 6}}
    
    bestScore := uint16(8000)
    var hand5C [5]Card
    for i:= 0; i < 21; i++{
        for j:=0; j < 5; j++{
            hand5C[j] = hand7C[permutations[i][j]]
        }
        score := solve5CHand(hand5C)
        if score < bestScore{
            bestScore = score
        }
    }
    return bestScore
}

func main(){
    err := importTables("tables.json")
    if err != nil{
        panic(err)
    }
    args := os.Args[1:]
    if len(args) != 7{
        fmt.Println("Usage 5_csolver Ah Kd Qc Js Th")
        os.Exit(1)
    }
    var hand [7]Card
    for i, arg := range args{
        hand[i] = getCard(arg)
    }
    rank := solve7CHand(hand)
    fmt.Printf("Hand Type: %s\nHand Rank: %d\n",getHandType(rank), rank)
}

package main
import (
	"fmt"
    "runtime/pprof"
    "os"
    "encoding/json"
    "encoding/binary"
    "time"
)
var flushes[0x2000]uint16
var noPairs[0x2000]uint16
var noPairs7[0x2000]uint16
var rank_dict map[uint32]uint16
var seven_rank_dict map[uint64]uint16
var primes = [13]uint64{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41}
var Primes[0x2000]uint64

type Card struct{
    bitMask uint64
    prime uint64
}

type Tables struct{
    FlushRanks         []uint16           `json:"flush_ranks"`
    StraightRanks      []uint16           `json:"straight_ranks"`
    SevenStraightRanks []uint16           `json:"seven_straight_ranks"`
    Primes             []uint64           `json:"primes"`
    RankDict           map[uint32]uint16  `json:"rank_dict"`
    SevenRankDict      map[uint64]uint16  `json:"seven_rank_dict"`
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
    copy(Primes[:], tables.Primes)
    seven_rank_dict = tables.SevenRankDict

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
const tableSize = 0x20000
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

func solve7CHand(bitMask uint64) uint16{
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
    
    prime := Primes[heartsMask] * Primes[diamondsMask] * Primes[clubsMask] * Primes[spadesMask]
    return hashLookup(prime)
}

func test(){
    var histogram map[string]int
    histogram = make(map[string]int)
    var scores = [7463]int{}
    start := time.Now()
    for a:= 0; a < 52; a++{
        for b:= a +1; b < 52; b++{
            for c:= b +1; c < 52; c++{
                for d:= c +1; d < 52; d++{
                    for e:= d +1; e < 52; e++{
                        for f:= e +1; f < 52; f++{
                            for g:= f +1; g < 52; g++{
                                mask := (uint64(1) << a)|(uint64(1) << b)|(uint64(1) << c)|(uint64(1) << d)|(uint64(1) << e)|(uint64(1) << f)|(uint64(1) << g)
                                scores[solve7CHand(mask)]++
                            }
                        }
                    }
                }
            }
        }
    }
    elapsed := time.Since(start)
    fmt.Printf("Evaluation time: %v\n", elapsed)
    fmt.Printf("Hands per second: %.2fM\n", 133784560.0 / elapsed.Seconds() / 1000000)
    fmt.Printf("Hash table size: %.2fMB\n", float64(tableSize) * 8 / 1024 / 1024)

    for score, quantity := range scores{
        histogram[getHandType(uint16(score))] += quantity
    }
    for key, value := range histogram{
        fmt.Printf("%s: %d\n", key, value)
    }
}
func testRandom() {
    f, err := os.Open("random_hands.bin")
    if err != nil {
        panic(err)
    }
    defer f.Close()

    var numHands int32
    binary.Read(f, binary.LittleEndian, &numHands)

    hands := make([][7]int32, numHands)
    for i := range hands {
        binary.Read(f, binary.LittleEndian, &hands[i])
    }

    fmt.Printf("Loaded %d hands\n", numHands)

    var scores = [7463]int{}
    var histogram = make(map[string]int)

    start := time.Now()

    for _, h := range hands {
        mask := (uint64(1) << h[0])|(uint64(1) << h[1])|(uint64(1) << h[2])|(uint64(1) << h[3])|(uint64(1) << h[4])|(uint64(1) << h[5])|(uint64(1) << h[6])
        scores[solve7CHand(mask)]++
    }
    elapsed := time.Since(start)
    fmt.Printf("Evaluation time: %v\n", elapsed)
    fmt.Printf("Hands per second: %.2fM\n", float64(numHands) / elapsed.Seconds() / 1000000)

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
    //testRandom()
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

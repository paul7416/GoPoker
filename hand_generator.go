package main
import (
	"os"
    "encoding/json"
    "math/bits"
)
var flush_ranks[0x2000]uint16
var straight_ranks[0x2000]uint16
var seven_c_ranks[0x2000]uint16
var rank_dict map[uint32]uint16
var seven_rank_dict map[uint64]uint16
var primes = [13]uint64{2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41}

type Tables struct{
    FlushRanks         []uint16           `json:"flush_ranks"`
    StraightRanks      []uint16           `json:"straight_ranks"`
    SevenStraightRanks []uint16           `json:"seven_straight_ranks"`
    RankDict           map[uint32]uint16  `json:"rank_dict"`
    SevenRankDict      map[uint64]uint16  `json:"seven_rank_dict"`
}
func exportTables() error {
    tables := Tables{
        FlushRanks:         flush_ranks[:],
        StraightRanks:      straight_ranks[:],
        RankDict:           rank_dict,
        SevenStraightRanks: seven_c_ranks[:],
        SevenRankDict:      seven_rank_dict,
    }
    data, err := json.Marshal(tables)
    if err != nil{
        return err
    }
    return os.WriteFile("tables.json", data, 0644)
}

func max(a, b int) int{
    if(a > b){
        return a
    }
    return b
}
func get_5c_rank_score(a int,b int, c int, d int, e int)uint16{
    bitMask := (1<<a)|(1<<b)|(1<<c)|(1<<d)|(1<<e)
    if straight_ranks[bitMask] != 0{
        return straight_ranks[bitMask]
    }
    prime := primes[a] * primes[b] * primes[c] * primes[d] * primes[e]
    return rank_dict[uint32(prime)]
}

func add_to_7c_rank_dict(a int, b int, c int, d int, e int, f int, g int){
    bitMask := (1<<a)|(1<<b)|(1<<c)|(1<<d)|(1<<e)|(1<<f)|(1<<g)
    if seven_c_ranks[bitMask] != 0{
        return
    }
    prime := primes[a] * primes[b] * primes[c] * primes[d] * primes[e] * primes[f] * primes[g]
    best_score := uint16(8000)
    var score uint16
    score = get_5c_rank_score(a,b,c,d,e)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,c,d,f)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,c,d,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,c,e,f)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,c,e,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,c,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,d,e,f)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,d,e,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,d,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,b,e,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,c,d,e,f)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,c,d,e,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,c,d,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,c,e,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(a,d,e,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(b,c,d,e,f)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(b,c,d,e,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(b,c,d,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(b,c,e,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(b,d,e,f,g)
    if score < best_score{best_score=score}
    score = get_5c_rank_score(c,d,e,f,g)
    if score < best_score{best_score=score}
    seven_rank_dict[prime] = best_score
}

func create_7c_rank_dict(){
    for a:= 0; a < 13; a++{
        for b:= a; b < 13; b++{
            for c:= b; c < 13; c++{
                for d:= c; d < 13; d++{
                    for e:= d; e < 13; e++{
                        for f:= e; f < 13; f++{
                            for g:= f; g < 13; g++{
                                if !hasMoreThanFour(a,b,c,d,e,f,g){
                                    add_to_7c_rank_dict(a,b,c,d,e,f,g)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
func hasMoreThanFour(a,b,c,d,e,f,g int) bool{
    counts := [13]int{}
    counts[a]++
    counts[b]++
    counts[c]++
    counts[d]++
    counts[e]++
    counts[f]++
    counts[g]++
    for _, c := range counts{
        if c > 4{
            return true
        }
    }
    return false

}


func add_high_card(flush_rank uint16, straight_rank uint16){
    for a:= 0; a < 13; a++{
        for b:= a + 1; b < 13; b++{
            for c:= b + 1; c < 13; c++{
                for d:= c + 1; d < 13; d++{
                    for e:= max(d + 1, a + 5); e < 13; e++{
                        if !(a == 0 && b == 1 && c == 2 && d == 3 && e == 12){
                            index := uint16((1 << a) | (1 << b) | (1 << c) | (1 << d) | (1 << e))
                            flush_ranks[index] = flush_rank
                            straight_ranks[index] = straight_rank
                            flush_rank--
                            straight_rank--
                        }
                    }
                }
            }
        }
    }
}
func add_straights(flush_rank uint16, straight_rank uint16){
    flush_ranks[0x100f] = flush_rank
    straight_ranks[0x100f] = straight_rank
    for i:= 0; i < 9; i++{
        flush_rank--
        straight_rank--
        mask := uint16(0x1f << i)
        flush_ranks[mask] = flush_rank
        straight_ranks[mask] = straight_rank
    }
}

func add_pairs(rank uint16){
    for a:= 0; a < 13; a++{
        for b:= 0; b < 13; b++{
            for c:= b + 1; c < 13; c++{
                for d:= c + 1; d < 13; d++{
                    if a == b || a == c || a == d {continue}
                    primeval := uint32(primes[a] * primes[a] * primes[b] * primes[c] * primes[d])
                    rank_dict[primeval] = rank
                    rank--
                }
            }
        }
    }
}
func add_2p(rank uint16){
    for a:= 0; a < 13; a++{
        for b:= a + 1; b < 13; b++{
            for c:= 0; c < 13; c++{
                if c == a || c == b {continue}
                primeval := uint32(primes[a] * primes[a] * primes[b] * primes[b] * primes[c])
                rank_dict[primeval] = rank
                rank--
            }
        }
    }
}
func add_trips(rank uint16){
    for a:= 0; a < 13; a++{
        for b:= 0; b < 13; b++{
            for c:= b + 1; c < 13; c++{
                if a == b || a == c {continue}
                primeval := uint32(primes[a] * primes[a] * primes[a] * primes[b] * primes[c])
                rank_dict[primeval] = rank
                rank--
            }
        }
    }
}
func add_fh(rank uint16){
    for a:= 0; a < 13; a++{
        for b:= 0; b < 13; b++{
            if a == b {continue}
            primeval := uint32(primes[a] * primes[a] * primes[a] * primes[b] * primes[b])
            rank_dict[primeval] = rank
            rank--
        }
    }
}
func add_quads(rank uint16){
    for a:= 0; a < 13; a++{
        for b:= 0; b < 13; b++{
            if a == b {continue}
            primeval := uint32(primes[a] * primes[a] * primes[a] * primes[a] * primes[b])
            rank_dict[primeval] = rank
            rank--
        }
    }
}
func getBest(mask uint16, lookUp [0x2000]uint16)uint16{
    bestRank := uint16(7462)
    for i := uint16(0); i < 0x2000; i++{
        if bits.OnesCount16(i) != 5{
            continue
        }
        if mask & i != i{
            continue
        }
        if (mask & i == i) && (lookUp[i] < bestRank){
            bestRank = lookUp[i]
        }
    }
    return bestRank

}
func add_7c_flushes(){
    for i:= uint16(0); i < 0x2000; i++{
        bits_used := bits.OnesCount16(i)
        if bits_used < 6 || bits_used > 7 {
            continue
        }
        flush_ranks[i] = getBest(i, flush_ranks)
    }
}
func add_7c_hc(){
    for i:= uint16(0); i < 0x2000; i++{
        bits_used := bits.OnesCount16(i)
        if bits_used < 5 || bits_used > 7{
            continue
        }
        best_score := getBest(i, straight_ranks)
        if (bits_used == 6 || bits_used == 5){
            if best_score <= 1609{
                seven_c_ranks[i] = best_score
            }
        }else if bits_used == 7{
            seven_c_ranks[i] = best_score
        }
    }
}

func main(){
    rank_dict = make(map[uint32]uint16)
    seven_rank_dict = make(map[uint64]uint16)
    add_high_card(1599, 7462)
    add_straights(10, 1609)
    add_pairs(6185)
    add_2p(3325)
    add_trips(2467)
    add_fh(322)
    add_quads(166)
    add_7c_flushes()
    add_7c_hc()
    create_7c_rank_dict()
    exportTables()
}

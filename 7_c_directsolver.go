package main
import (
	"fmt"
    "os"
    "encoding/json"
    "strings"
)



func solve7CHand(handMask uint64) uint16{
    heartsMask = handMask & 0x1fff
    diamondsMask = (handMask >> 13) & 0x1fff
    clubsMask = (handMask >> 26) & 0x1fff
    spadesMask = (handMask >> 39) & 0x1fff

    flushScore := 
            flushes[heartsMask] + 
            flushes[diamondsMask] + 
            flushes[clubsMask] + 
            flushes[spadesMask] 

    if flushScore != 0 {return flushScore}

    primes := primeLookup[heartsMask] * primeLookup[diamondsMask] * primeLookup[clubsMask] * primeLookup[spadesMask]
    return rankDict[primes]
}

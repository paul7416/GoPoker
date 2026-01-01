#!/bin/bash
echo "=== Testing Hand Types ==="

cards="Ah Kh Qh Jh Th 2c 3d"
echo -n "Royal Flush ($cards): "
./7_csolver $cards

cards="9h 8h 7h 6h 5h 2c 3d"
echo -n "Straight Flush ($cards): "
./7_csolver $cards

cards="Ah Ac Ad As 2h 3c 4d"
echo -n "Quads ($cards): "
./7_csolver $cards

cards="Ah Ac Ad As 2h 3c Kd"
echo -n "Quads ($cards): "
./7_csolver $cards

cards="Ah Ac Ad Kh Kc 2c 3d"
echo -n "Full House ($cards): "
./7_csolver $cards

cards="Ah 9h 7h 5h 3h 2c Kd"
echo -n "Flush ($cards): "
./7_csolver $cards

cards="Ah Kd Qc Js Th 2c 3d"
echo -n "Straight ($cards): "
./7_csolver $cards

cards="Ah Ac Ad 5h 7c 2c 3d"
echo -n "Trips ($cards): "
./7_csolver $cards

cards="Ah Ac Kh Kc 5h 2c 3d"
echo -n "Two Pair ($cards): "
./7_csolver $cards

cards="Ah Ac 5h 7c 9d 2c 3d"
echo -n "Pair ($cards): "
./7_csolver $cards

cards="Ah Ac 5h 7c 9d 2c Kd"
echo -n "Pair ($cards): "
./7_csolver $cards

cards="Ah 9c 7d 5h 3c 2s Kd"
echo -n "High Card ($cards): "
./7_csolver $cards

echo ""
echo "=== Edge Cases ==="

cards="Ah 2c 3d 4h 5c 9s Kd"
echo -n "Wheel ($cards): "
./7_csolver $cards

cards="Ah 2h 3h 4h 5h 9c Kd"
echo -n "Steel Wheel ($cards): "
./7_csolver $cards

cards="Ah Ac Ad 2h 2c 2d 3h"
echo -n "Full House from two trips ($cards): "
./7_csolver $cards

cards="Ah Kh Qh Jh 9h 2d 3h"
echo -n "Highest Flush ($cards): "
./7_csolver $cards

cards="2h 2d 2c 3h 3c 8d 7h"
echo -n "Lowest Full House ($cards): "
./7_csolver $cards

echo ""
echo "=== Done ==="

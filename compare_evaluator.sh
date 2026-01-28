#!/bin/bash
# Usage: ./compare_evaluator.sh [directory]
DIR="${1:-.}"
TOTAL_HANDS=133784560

FILES=$(find "$DIR" -maxdepth 1 -name "test_evaluator_*.txt" | sort)
if [ -z "$FILES" ]; then
    echo "No test_evaluator_*.txt files found in $DIR"
    exit 1
fi

printf "%-10s %-8s %-8s %10s %10s %12s %6s %8s %8s %8s %8s\n" \
    "Date" "Time" "Commit" "Time(ms)" "MHands/s" "Cycles/Hand" "IPC" "BrMiss%" "L1Miss%" "HashTbl" "Direct"
printf "%-10s %-8s %-8s %10s %10s %12s %6s %8s %8s %8s %8s\n" \
    "----------" "--------" "--------" "----------" "----------" "------------" "------" "--------" "--------" "--------" "--------"

for file in $FILES; do
    date=$(grep "Date:" "$file" | grep -oP '\d{1,2} \w+ \d{4}' | head -1)
    time_of_day=$(grep "Date:" "$file" | grep -oP '\d{2}:\d{2}:\d{2}' | head -1)
    sortkey=$(date -d "$date $time_of_day" +%s 2>/dev/null || echo "0")
    commit=$(grep "Git Short:" "$file" | awk '{print $3}')
    time=$(grep "Time taken:" "$file" | grep -oP '[\d.]+')
    mhands=$(grep "M Hands per second:" "$file" | grep -oP '[\d.]+')
    cycles=$(grep "cycles" "$file" | grep -oP '[\d,]+(?=\s+cycles)' | head -1 | tr -d ',')
    ipc=$(grep "insn per cycle" "$file" | grep -oP '[\d.]+(?=\s+insn)' | head -1)
    brmiss=$(grep "branch-misses" "$file" | grep -oP '[\d.]+(?=%)' | head -1)
    l1miss=$(grep "L1-dcache-load-misses" "$file" | grep -oP '[\d.]+(?=%)' | head -1)
    hashtbl=$(grep "Hash Table Size:" "$file" | grep -oP '[\d]+kB')
    direct=$(grep "Direct Lookup Size:" "$file" | grep -oP '[\d]+kB')
    
    if [ -n "$cycles" ]; then
        cph=$(echo "scale=2; $cycles / $TOTAL_HANDS" | bc)
    else
        cph="-"
    fi
    
    printf "%s|%-10s|%-8s|%-8s|%10s|%10s|%12s|%6s|%8s|%8s|%8s|%8s\n" \
        "$sortkey" "$date" "$time_of_day" "$commit" "$time" "$mhands" "$cph" "$ipc" "$brmiss" "$l1miss" "$hashtbl" "$direct"
done | sort -t'|' -k1 -n | cut -d'|' -f2- | while IFS='|' read -r date time_of_day commit time mhands cph ipc brmiss l1miss hashtbl direct; do
    printf "%-10s %-8s %-8s %10s %10s %12s %6s %8s %8s %8s %8s\n" \
        "$date" "$time_of_day" "$commit" "$time" "$mhands" "$cph" "$ipc" "$brmiss" "$l1miss" "$hashtbl" "$direct"
done

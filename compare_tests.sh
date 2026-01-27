#!/bin/bash
# Usage: ./compare_profiles.sh [directory]
# Default: current directory
DIR="${1:-.}"
TOTAL_HANDS=133784560  # 52 choose 7
# Find all profile files
FILES=$(find "$DIR" -maxdepth 1 -name "*.txt" | sort)
if [ -z "$FILES" ]; then
    echo "No profile_*.txt files found in $DIR"
    exit 1
fi
# Print header
printf "%-10s %-8s %-8s %10s %10s %12s %6s %8s %8s %8s %8s\n" \
    "Date" "Time" "Commit" "Time(ms)" "MHands/s" "Cycles/Hand" "IPC" "BrMiss%" "L1Miss%" "HashTbl" "Direct"
printf "%-10s %-8s %-8s %10s %10s %12s %6s %8s %8s %8s %8s\n" \
    "----------" "--------" "--------" "----------" "----------" "------------" "------" "--------" "--------" "--------" "--------"

# Build and sort results
for file in $FILES; do
    date=$(grep "Date:" "$file" | grep -oP '\d{1,2} \w+ \d{4}' | head -1)
    time_of_day=$(grep "Date:" "$file" | grep -oP '\d{2}:\d{2}:\d{2}' | head -1)
    sortkey=$(date -d "$date $time_of_day" +%s 2>/dev/null || echo "0")
    commit=$(grep "Git Short:" "$file" | awk '{print $3}')
    time=$(grep "Time taken:" "$file" | grep -oP '[\d.]+')
    mhands=$(grep "M Hands per second:" "$file" | grep -oP '[\d.]+')
    cycles=$(grep -P '^\s+[\d,]+\s+cycles' "$file" | awk '{gsub(/,/,"",$1); print $1}')
    ipc=$(grep "insn per cycle" "$file" | grep -oP '[\d.]+(?=\s+insn)')
    brmiss=$(grep "branch-misses" "$file" | grep -oP '[\d.]+(?=%)')
    l1miss=$(grep "L1-dcache-load-misses" "$file" | grep -oP '[\d.]+(?=%)')
    hashtbl=$(grep "Hash Table Size:" "$file" | grep -oP '[\d]+kB')
    direct=$(grep "Direct Lookup Size:" "$file" | grep -oP '[\d]+kB')
    
    # Calculate cycles per hand
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

echo ""
echo "=== Top Functions by File ==="
for file in $FILES; do
    commit=$(grep "Git Short:" "$file" | awk '{print $3}')
    time_of_day=$(grep "Date:" "$file" | grep -oP '\d{2}:\d{2}:\d{2}' | head -1)
    echo ""
    echo "$commit @ $time_of_day:"
    grep -P '^\s+[\d.]+%\s+\S+\s+\S+\s+\[.\]\s+\S+' "$file" | head -3 | \
        awk '{printf "  %6s  %s\n", $1, $NF}'
done

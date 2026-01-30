#!/bin/bash
# Usage: ./compare_ev_calculator.sh [directory]
DIR="${1:-.}"
FILES=$(find "$DIR" -maxdepth 1 -name "test_ev_calculator_*.txt" | sort)
if [ -z "$FILES" ]; then
    echo "No test_ev_calculator_*.txt files found in $DIR"
    exit 1
fi
printf "%-10s %-8s %-8s %10s %10s %10s %6s %6s %8s %7s %7s %7s %7s\n" \
    "Date" "Time" "Commit" "Wall(ms)" "Hands/s" "Sims" "Thrds" "IPC" "Cyc/Sim" "BrMiss" "L1Miss" "LLCMiss" "Direct"
printf "%-10s %-8s %-8s %10s %10s %10s %6s %6s %8s %7s %7s %7s %7s\n" \
    "----------" "--------" "--------" "----------" "----------" "----------" "------" "------" "--------" "-------" "-------" "-------" "-------"
for file in $FILES; do
    date=$(grep "Date:" "$file" | grep -oP '\d{1,2} \w+ \d{4}' | head -1)
    time_of_day=$(grep "Date:" "$file" | grep -oP '\d{2}:\d{2}:\d{2}' | head -1)
    sortkey=$(date -d "$date $time_of_day" +%s 2>/dev/null || echo "0")
    commit=$(grep "Git Short:" "$file" | awk '{print $3}')
    
    walltime=$(grep "Wall time:" "$file" | grep -oP '[\d.]+' | head -1)
    hands_sec=$(grep "Hands per second" "$file" | grep -oP '[\d]+' | head -1)
    sims_raw=$(grep "^Simulations:" "$file" | grep -oP '[\d]+' | head -1)
    threads=$(grep "^Threads:" "$file" | grep -oP '[\d]+' | head -1)
    
    ipc=$(grep "insn per cycle" "$file" | grep -oP '[\d.]+(?=\s+insn)' | head -1)
    brmiss=$(grep "branch-misses" "$file" | grep -oP '[\d.]+(?=%)' | head -1)
    l1miss=$(grep "L1-dcache-load-misses" "$file" | grep -oP '[\d.]+(?=%)' | head -1)
    llcmiss=$(grep "LLC-load-misses" "$file" | grep -oP '[\d.]+(?=%)' | head -1)
    
    cycles=$(grep "cycles" "$file" | grep -oP '[\d,]+(?=\s+cycles)' | tr -d ',' | head -1)
    
    direct=$(grep -i "Direct Lookup.*Size:" "$file" | grep -oP '[\d]+' | head -1)
    
    # Calculate cycles per simulation
    if [ -n "$cycles" ] && [ -n "$sims_raw" ] && [ "$sims_raw" -gt 0 ]; then
        cycles_per_sim=$(echo "scale=1; $cycles / $sims_raw" | bc)
    else
        cycles_per_sim="-"
    fi
    
    # Format to MB/kB
    if [ -n "$direct" ] && [ "$direct" -ge 1048576 ]; then
        direct="$((direct / 1024 / 1024))MB"
    elif [ -n "$direct" ] && [ "$direct" -ge 1024 ]; then
        direct="$((direct / 1024))kB"
    fi
    
    # Format hands/sec to M
    if [ -n "$hands_sec" ]; then
        hands_sec_m=$(echo "scale=1; $hands_sec / 1000000" | bc)
        hands_sec="${hands_sec_m}M"
    fi
    
    # Format sims to M
    if [ -n "$sims_raw" ] && [ "$sims_raw" -ge 1000000 ]; then
        sims="$((sims_raw / 1000000))M"
    else
        sims="$sims_raw"
    fi
    
    printf "%s|%-10s|%-8s|%-8s|%10s|%10s|%10s|%6s|%6s|%8s|%7s|%7s|%7s|%7s\n" \
        "$sortkey" "$date" "$time_of_day" "$commit" "$walltime" "$hands_sec" "$sims" "$threads" "$ipc" "$cycles_per_sim" "$brmiss" "$l1miss" "$llcmiss" "$direct"
done | sort -t'|' -k1 -n | cut -d'|' -f2- | while IFS='|' read -r date time_of_day commit walltime hands_sec sims threads ipc cycles_per_sim brmiss l1miss llcmiss direct; do
    printf "%-10s %-8s %-8s %10s %10s %10s %6s %6s %8s %7s %7s %7s %7s\n" \
        "$date" "$time_of_day" "$commit" "$walltime" "$hands_sec" "$sims" "$threads" "$ipc" "$cycles_per_sim" "$brmiss" "$l1miss" "$llcmiss" "$direct"
done
echo ""
#echo "=== Top Functions ==="
#for file in $FILES; do
#    time_of_day=$(grep "Date:" "$file" | grep -oP '\d{2}:\d{2}:\d{2}' | head -1)
#    echo ""
#    echo "$time_of_day:"
#    grep -P '^\s+[\d.]+%\s+\S+\s+\S+\s+\[.\]\s+\S+' "$file" | head -5 | \
#        awk '{printf "  %6s  %s\n", $1, $NF}'
#done

#!/bin/bash

# Usage: ./profile.sh <binary> [args...]
# Output goes to profile_<timestamp>.txt

if [ $# -lt 1 ]; then
    echo "Usage: $0 <binary> [args...]"
    exit 1
fi

BINARY="$1"
shift
ARGS="$@"

# Get git commit hash (short and long)
GIT_HASH=$(git rev-parse HEAD 2>/dev/null || echo "not a git repo")
GIT_HASH_SHORT=$(git rev-parse --short HEAD 2>/dev/null || echo "n/a")

TIMESTAMP=$(date +%Y%m%d_%H%M%S)
OUTPUT_FILE="profiles/${GIT_HASH_SHORT}_${TIMESTAMP}.txt"


# Write header info
{
    echo "========================================"
    echo "Performance Profile Report"
    echo "========================================"
    echo "Date:        $(date)"
    echo "Binary:      $BINARY"
    echo "Arguments:   $ARGS"
    echo "Git Commit:  $GIT_HASH"
    echo "Git Short:   $GIT_HASH_SHORT"
    echo "Host:        $(hostname)"
    echo "Kernel:      $(uname -r)"
    echo "========================================"
    echo ""
} > "$OUTPUT_FILE"

# Run perf stat and append output
echo "Running perf stat..."
{
    echo "=== perf stat ==="
    perf stat -d "$BINARY" $ARGS 2>&1
    echo ""
} >> "$OUTPUT_FILE"

# Run perf record + report
echo "Running perf record..."
perf record -g -o ./perf_outputs/perf_${TIMESTAMP}.data "$BINARY" $ARGS 2>/dev/null

{
    echo "=== perf report (top functions) ==="
    perf report -i ./perf_outputs/perf_${TIMESTAMP}.data --stdio --no-children 2>&1 | head -50
} >> "$OUTPUT_FILE"

echo "Done! Output written to: $OUTPUT_FILE"
echo "Raw perf data: perf_${TIMESTAMP}.data"

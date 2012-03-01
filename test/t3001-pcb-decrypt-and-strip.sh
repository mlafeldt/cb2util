#!/bin/sh

test_description="cb2util pcb: decrypt file and strip header"

. ./test-lib.sh

for file in $TEST_DIR/pcb/*.bin; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util pcb -s $file out &&
        test_cmp_bin $prefix.strip out
    "
done

test_done

#!/bin/sh

test_description="cb2util pcb: decrypt file"

. ./test-lib.sh

for file in $TEST_DIR/pcb/*.bin; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util pcb $file out &&
        test_cmp_bin $prefix.raw out
    "
done

test_done

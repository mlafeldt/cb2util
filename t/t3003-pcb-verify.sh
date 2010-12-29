#!/bin/sh

test_description="cb2util pcb: verify signature"

. ./test-lib.sh

for file in $TEST_DIR/pcb/*.bin; do
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util pcb -c $file >out &&
        echo '$file: OK' >expect &&
        test_cmp expect out
    "
done

test_done

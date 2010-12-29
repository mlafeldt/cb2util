#!/bin/sh

test_description="cb2util cbc: verify signature"

. ./test-lib.sh

for file in $TEST_DIR/cbc/*.cbc; do
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util cbc -c $file >out &&
        echo '$file: OK' >expect &&
        test_cmp expect out
    "
done

for file in $TEST_DIR/cbc7/*.cbc; do
    basename=$(basename "$file")

    test_expect_success "$basename (v7 has no signature)" "
        test_must_fail cb2util cbc -7 -c $file
    "
done

test_done

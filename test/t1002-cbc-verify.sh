#!/bin/sh

test_description="cb2util cbc: verify signature"

. ./test-lib.sh

for file in $TEST_DIR/cbc/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    if [ -f "$prefix.ok" ]; then
        test_expect_success "$basename valid" "
            cb2util cbc -c $file >out &&
            echo '$file: OK' >expect &&
            test_cmp expect out
        "
    else
        test_expect_success "$basename invalid" "
            test_must_fail cb2util cbc -c $file >out &&
            echo '$file: FAILED' >expect &&
            test_cmp expect out
        "
    fi
done

for file in $TEST_DIR/cbc7/*.cbc; do
    basename=$(basename "$file")

    test_expect_success "$basename (v7) not signed" "
        test_must_fail cb2util cbc -7 -c $file
    "
done

test_done
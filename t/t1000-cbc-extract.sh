#!/bin/sh

test_description="cb2util cbc: extract cheats"

. ./test-lib.sh

for file in $TEST_DIR/cbc/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util cbc $file >out &&
        test_cmp $prefix.enc out
    "
done

for file in $TEST_DIR/cbc7/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename (v7)" "
        cb2util cbc -7 $file >out &&
        test_cmp $prefix.enc out
    "
done

test_done

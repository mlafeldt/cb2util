#!/bin/sh

test_description="cb2util cbc: extract and decrypt cheats"

. ./test-lib.sh

for file in $TEST_DIR/cbc/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util cbc -d $file >out &&
        test_cmp $prefix.txt out
    "
done

for file in $TEST_DIR/cbc7/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename (v7)" "
        cb2util cbc -7 -d $file >out &&
        test_cmp $prefix.txt out
    "
done

test_done

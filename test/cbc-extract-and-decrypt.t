#!/bin/sh
# vi: set ft=sh :

test_description="cb2util cbc: extract and decrypt cheats"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/cbc/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util cbc --decrypt $file >out &&
        test_cmp $prefix.txt out
    "
done

for file in $SHARNESS_TEST_DIRECTORY/cbc7/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename (v7)" "
        cb2util cbc --decrypt -7 $file >out &&
        test_cmp $prefix.txt out
    "
done

test_done

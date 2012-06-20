#!/bin/sh
# vi: set ft=sh :

test_description="cb2util cbc: extract cheats"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/cbc/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util cbc $file >out &&
        test_cmp $prefix.enc out
    "
done

for file in $SHARNESS_TEST_DIRECTORY/cbc7/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename (v7)" "
        cb2util cbc -7 $file >out &&
        test_cmp $prefix.enc out
    "
done

test_done

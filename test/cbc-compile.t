#!/bin/sh
# vi: set ft=sh :

test_description="cb2util cbc: compile cheats"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/cbc/*.txt; do
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util cbc --compile $file out &&
        cb2util cbc out >out2 &&
        test_cmp $file out2
    "
done

for file in $SHARNESS_TEST_DIRECTORY/cbc7/*.txt; do
    basename=$(basename "$file")

    test_expect_success "$basename (v7)" "
        cb2util cbc -7 --compile $file out &&
        cb2util cbc -7 out >out2 &&
        test_cmp $file out2
    "
done

test_done

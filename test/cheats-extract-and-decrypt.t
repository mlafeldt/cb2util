#!/bin/sh
# vi: set ft=sh :

test_description="cb2util cheats: extract and decrypt cheats"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/cheats/*.cheats; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util cheats -d $file >out &&
        test_cmp $prefix.txt out
    "
done

test_done

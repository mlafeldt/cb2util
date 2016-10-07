#!/bin/sh
# vi: set ft=sh :

test_description="cb2util cbs: extract files from game saves"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/cbs/*.cbs; do
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util cbs $file
    "
done

test_done

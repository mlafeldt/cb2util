#!/bin/sh
# vi: set ft=sh :

test_description="cb2util pcb: decrypt file"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/pcb/*.bin; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util pcb $file out &&
        cmp -b -l $prefix.raw out
    "
done

test_done

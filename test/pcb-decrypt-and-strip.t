#!/bin/sh
# vi: set ft=sh :

test_description="cb2util pcb: decrypt file and strip header"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/pcb/*.bin; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util pcb -s $file out &&
        cmp -b -l $prefix.strip out
    "
done

test_done

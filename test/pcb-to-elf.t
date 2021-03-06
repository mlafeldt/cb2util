#!/bin/sh
# vi: set ft=sh :

test_description="cb2util pcb: convert to ELF"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/pcb/*.bin; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util pcb --elf $file out &&
        cmp --print-bytes --verbose $prefix.elf out
    "
done

test_done

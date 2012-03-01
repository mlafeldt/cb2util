#!/bin/sh

test_description="cb2util pcb: convert to ELF"

. ./test-lib.sh

for file in $TEST_DIR/pcb/*.bin; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "$basename" "
        cb2util pcb -e $file out &&
        test_cmp_bin $prefix.elf out
    "
done

test_done

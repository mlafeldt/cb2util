#!/bin/sh

test_description="Test 'pcb' command"

. ./test-lib.sh

for file in $TEST_DIR/pcb/*.bin; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "($basename) decrypt file" "
        cb2util pcb $file out &&
        test_cmp_bin $prefix.raw out
    "

    test_expect_success "($basename) decrypt file and strip header" "
        cb2util pcb -s $file out &&
        test_cmp_bin $prefix.strip out
    "

    test_expect_success "($basename) convert to ELF" "
        cb2util pcb -e $file out &&
        test_cmp_bin $prefix.elf out
    "

    test_expect_success "($basename) verify signature" "
        cb2util pcb -c $file >out &&
        echo '$file: OK' >expect &&
        test_cmp expect out
    "
done

test_done

#!/bin/sh

test_description="Test 'pcb' command"

. ./test-lib.sh

out=out

test_expect_success "setup" "
    cd pcb &&
    trap 'rm -f $out' EXIT
"

for file in *.bin; do
    prefix=${file%.*}

    test_expect_success "($file) decrypt file" "
        cb2util pcb $file $out &&
        test_cmp $prefix.raw $out
    "

    test_expect_success "($file) decrypt file and strip header" "
        cb2util pcb -s $file $out &&
        test_cmp $prefix.strip $out
    "

    test_expect_success "($file) convert to ELF" "
        cb2util pcb -e $file $out &&
        test_cmp $prefix.elf $out
    "

    test_expect_success "($file) verify signature" "
        cb2util pcb -c $file >$out &&
        test_cmp $prefix.sig $out
    "
done

test_done

#!/bin/sh

test_description="Test PCB commands."

. ./test-lib.sh

out=out

test_expect_success "setup" "
    cd pcb &&
    trap 'rm -f $out' EXIT
"

for file in *.bin; do
    prefix=${file%.*}

    test_expect_success "($file) decrypt file" "
        cb2util -t pcb $file $out &&
        test_cmp $prefix.decrypt $out
    "

    test_expect_success "($file) decrypt file and strip header" "
        cb2util -t pcb -s $file $out &&
        test_cmp $prefix.strip $out
    "

    test_expect_success "($file) convert to ELF" "
        cb2util -t pcb -e $file $out &&
        test_cmp $prefix.elf $out
    "

    test_expect_success "($file) verify signature" "
        cb2util -t pcb -c $file >$out &&
        test_cmp $prefix.verify $out
    "
done

test_done

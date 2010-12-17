#!/bin/sh

test_description="Test PCB commands."

. ./test-lib.sh

type=pcb

out=$(tempfile)

for file in $type/*.bin; do
    prefix=${file%.*}

    test_expect_success "($file) decrypt file" "
        cb2util -t $type $file $out &&
        test_cmp $out $prefix.decrypt
    "

    test_expect_success "($file) decrypt file and strip header" "
        cb2util -t $type -s $file $out &&
        test_cmp $out $prefix.strip
    "

    test_expect_success "($file) convert to ELF" "
        cb2util -t $type -e $file $out &&
        test_cmp $out $prefix.elf
    "

    test_expect_success "($file) verify signature" "
        cb2util -t $type -c $file >$out &&
        test_cmp $out $prefix.verify
    "
done

rm $out

test_done

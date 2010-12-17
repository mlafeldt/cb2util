#!/bin/sh

test_description="Test PCB commands."

. ./test-lib.sh

type=pcb

out="/dev/null"

for f in $type/*.bin; do
    test_expect_success "($f) decrypt file" "
        cb2util -t $type $f $out
    "

    test_expect_success "($f) decrypt file and strip header" "
        cb2util -t $type -s $f $out
    "

    test_expect_success "($f) convert to ELF" "
        cb2util -t $type -e $f $out
    "

    test_expect_success "($f) verify signature" "
        cb2util -t $type -c $f
    "
done

test_done

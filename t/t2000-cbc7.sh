#!/bin/sh

test_description="Test CBC v7 commands."

. ./test-lib.sh

type=cbc7

for f in $type/*.cbc; do
    test_expect_success "($f) extract cheats" "
        cb2util -t $type $f
    "

    test_expect_success "($f) extract and decrypt cheats" "
        cb2util -t $type -d $f
    "

    test_expect_success "($f) has no signature" "
        ! cb2util -t $type -c $f
    "
done

test_done

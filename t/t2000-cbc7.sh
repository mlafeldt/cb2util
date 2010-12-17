#!/bin/sh

test_description="Test CBC v7 commands."

. ./test-lib.sh

type=cbc7

out=$(tempfile)

for file in $type/*.cbc; do
    prefix=${file%.*}

    test_expect_success "($file) extract cheats" "
        cb2util -t $type $file >$out &&
        test_cmp $out $prefix.extract
    "

    test_expect_success "($file) extract and decrypt cheats" "
        cb2util -t $type -d $file >$out &&
        test_cmp $out $prefix.decrypt
    "

    test_expect_success "($file) has no signature" "
        ! cb2util -t $type -c $file
    "
done

rm $out

test_done

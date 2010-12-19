#!/bin/sh

test_description="Test CBC v7 commands."

. ./test-lib.sh

out=out

test_expect_success "setup" "
    cd cbc7 &&
    trap 'rm -f $out' EXIT
"

for file in *.cbc; do
    prefix=${file%.*}

    test_expect_success "($file) extract cheats" "
        cb2util -t cbc7 $file >$out &&
        test_cmp $out $prefix.extract
    "

    test_expect_success "($file) extract and decrypt cheats" "
        cb2util -t cbc7 -d $file >$out &&
        test_cmp $out $prefix.decrypt
    "

    test_expect_success "($file) has no signature" "
        test_must_fail cb2util -t cbc7 -c $file
    "
done

test_done

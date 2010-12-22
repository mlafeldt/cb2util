#!/bin/sh

test_description="Test 'cbc' command (v7 files)"

. ./test-lib.sh

out=out

test_expect_success "setup" "
    cd cbc7 &&
    trap 'rm -f $out' EXIT
"

for file in *.cbc; do
    prefix=${file%.*}

    test_expect_success "($file) extract cheats" "
        cb2util cbc -7 $file >$out &&
        test_cmp $prefix.enc $out
    "

    test_expect_success "($file) extract and decrypt cheats" "
        cb2util cbc -7 -d $file >$out &&
        test_cmp $prefix.raw $out
    "

    test_expect_success "($file) has no signature" "
        test_must_fail cb2util cbc -7 -c $file
    "
done

test_done

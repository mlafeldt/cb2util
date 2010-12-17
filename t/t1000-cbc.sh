#!/bin/sh

test_description="Test CBC commands."

. ./test-lib.sh

type=cbc

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

    test_expect_success "($file) verify signature" "
        cb2util -t $type -c $file >$out &&
        test_cmp $out $prefix.verify
    "
done

rm $out

test_done

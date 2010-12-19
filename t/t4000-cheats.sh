#!/bin/sh

test_description="Test 'cheats' command"

. ./test-lib.sh

test_expect_success "setup" "
    cd cheats &&
    trap 'rm -f out out2' EXIT
"

for file in *.cheats; do
    prefix=${file%.*}

    test_expect_success "($file) extract cheats" "
        cb2util cheats $file >out &&
        test_cmp $prefix.extract out
    "

    test_expect_success "($file) extract and decrypt cheats" "
        cb2util cheats -d $file >out &&
        test_cmp $prefix.decrypt out
    "
done

for file in *.decrypt; do
    test_expect_success "($file) compile cheats" "
        cb2util cheats -c $file out &&
        cb2util cheats out >out2 &&
        test_cmp $file out2
    "
done

test_done

#!/bin/sh

test_description="Test 'cheats' command"

. ./test-lib.sh

out=out

test_expect_success "setup" "
    cd cheats &&
    trap 'rm -f $out' EXIT
"

for file in *.cheats; do
    prefix=${file%.*}

    test_expect_success "($file) extract cheats" "
        cb2util cheats $file >$out &&
        test_cmp $prefix.extract $out
    "

    test_expect_success "($file) extract and decrypt cheats" "
        cb2util cheats -d $file >$out &&
        test_cmp $prefix.decrypt $out
    "
done

test_done

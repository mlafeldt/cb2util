#!/bin/sh

test_description="Test 'cbc' command (v7 files)"

. ./test-lib.sh

for file in $TEST_DIR/cbc7/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "($basename) extract cheats" "
        cb2util cbc -7 $file >out &&
        test_cmp $prefix.enc out
    "

    test_expect_success "($basename) extract and decrypt cheats" "
        cb2util cbc -7 -d $file >out &&
        test_cmp $prefix.raw out
    "

    test_expect_success "($basename) has no signature" "
        test_must_fail cb2util cbc -7 -c $file
    "
done

test_done

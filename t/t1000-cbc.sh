#!/bin/sh

test_description="Test 'cbc' command"

. ./test-lib.sh

for file in $TEST_DIR/cbc/*.cbc; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "($basename) extract cheats" "
        cb2util cbc $file >out &&
        test_cmp $prefix.enc out
    "

    test_expect_success "($basename) extract and decrypt cheats" "
        cb2util cbc -d $file >out &&
        test_cmp $prefix.raw out
    "

    test_expect_success "($basename) verify signature" "
        cb2util cbc -c $file >out &&
        echo '$file: OK' >expect &&
        test_cmp expect out
    "
done

test_done

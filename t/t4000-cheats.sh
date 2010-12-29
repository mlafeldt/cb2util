#!/bin/sh

test_description="Test 'cheats' command"

. ./test-lib.sh

for file in $TEST_DIR/cheats/*.cheats; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_success "($basename) extract cheats" "
        cb2util cheats $file >out &&
        test_cmp $prefix.enc out
    "

    test_expect_success "($basename) extract and decrypt cheats" "
        cb2util cheats -d $file >out &&
        test_cmp $prefix.raw out
    "
done

for file in $TEST_DIR/cheats/*.raw; do
    basename=$(basename "$file")

    test_expect_success "($basename) compile cheats" "
        cb2util cheats -c $file out &&
        cb2util cheats out >out2 &&
        test_cmp $file out2
    "
done

test_done

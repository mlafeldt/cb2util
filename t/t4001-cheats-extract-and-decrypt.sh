#!/bin/sh

test_description="cb2util cheats: extract and decrypt cheats"

. ./test-lib.sh

for file in $TEST_DIR/cheats/*.cheats; do
    prefix=${file%.*}
    basename=$(basename "$file")

    test_expect_failure "$basename" "
        cb2util cheats -d $file >out &&
        test_cmp $prefix.txt out
    "
done

test_done

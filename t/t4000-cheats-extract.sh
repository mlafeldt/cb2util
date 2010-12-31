#!/bin/sh

test_description="cb2util cheats: extract cheats"

. ./test-lib.sh

for file in $TEST_DIR/cheats/*.cheats; do
    prefix=${file%.*}
    basename=$(basename "$file")

    if [ -f "$prefix.enc" ]; then
        expect=$prefix.enc
    else
        expect=$prefix.txt
    fi

    test_expect_success "$basename" "
        cb2util cheats $file >out &&
        test_cmp $expect out
    "
done

test_done

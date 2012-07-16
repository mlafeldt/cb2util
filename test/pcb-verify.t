#!/bin/sh
# vi: set ft=sh :

test_description="cb2util pcb: verify signature"

. ./sharness.sh

for file in $SHARNESS_TEST_DIRECTORY/pcb/*.bin; do
    prefix=${file%.*}
    basename=$(basename "$file")

    if [ -f "$prefix.ok" ]; then
        test_expect_success "$basename valid" "
            cb2util pcb --check $file >out &&
            echo '$file: OK' >expect &&
            test_cmp expect out
        "
    else
        test_expect_success "$basename invalid" "
            test_must_fail cb2util pcb --check $file >out &&
            echo '$file: FAILED' >expect &&
            test_cmp expect out
        "
    fi
done

test_done

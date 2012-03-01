#!/bin/sh

test_description="Test very basics"

../cb2util >/dev/null
if [ $? != 1 ]; then
    echo >&2 'You do not seem to have built cb2util yet.'
    exit 1
fi

. ./sharness.sh

test_expect_success "success is reported like this" "
    :
"

test_done

#!/bin/sh
# vi: set ft=sh :

test_description="Test very basics"

. ./sharness.sh

"$SHARNESS_BUILD_DIRECTORY/cb2util" --version >/dev/null
if [ $? != 0 ]; then
    echo >&2 'You do not seem to have built cb2util yet.'
    exit 1
fi

test_expect_success "success is reported like this" "
    :
"

test_done

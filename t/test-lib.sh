#!/bin/sh
# based on Git's test-lib.sh by Junio C Hamano
# see https://github.com/git/git/blob/master/t/test-lib.sh

if [ "$1" = "-v" -o "$1" = "--verbose" ]; then
    verbose=t
fi

exec 5>&1
if [ "$verbose" = "t" ]; then
    exec 4>&2 3>&1
else
    exec 4>/dev/null 3>/dev/null
fi

test_count=0
test_success=0
test_failure=0

test_run() {
    test_cleanup=:
    eval >&3 2>&4 "$1"
    eval_ret=$?
    eval >&3 2>&4 "$test_cleanup"
    return $eval_ret
}

test_expect_success() {
    test_count=$((test_count + 1))
    echo >&3 "expecting success: $2"
    if test_run "$2"; then
        test_success=$((test_success + 1))
        echo "ok $test_count - $1"
    else
        test_failure=$((test_failure + 1))
        echo "not ok $test_count - $1"
    fi
    echo >&3 ""
}

test_must_fail() {
    "$@"
    exit_code=$?
    if [ $exit_code = 0 ]; then
        echo >&2 "test_must_fail: command succeeded: $*"
        return 1
    elif [ $exit_code -gt 129 -a $exit_code -le 192 ]; then
        echo >&2 "test_must_fail: died by signal: $*"
        return 1
    elif [ $exit_code = 127 ]; then
        echo >&2 "test_must_fail: command not found: $*"
        return 1
    fi
    return 0
}

test_cmp() {
    diff -u --strip-trailing-cr "$@"
}

test_done() {
    if [ $test_failure = 0 ]; then
        echo "# passed all $test_count test(s)"
    else
        echo "# failed $test_failure among $test_count test(s)"
    fi
    echo "1..$test_count"
}

: ${TEST_DIR:=$(pwd)}
BUILD_DIR="$TEST_DIR/.."
PATH="$BUILD_DIR:$PATH"
export PATH

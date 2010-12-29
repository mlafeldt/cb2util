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
test_fixed=0
test_broken=0

test_run() {
    test_cleanup=:
    eval >&3 2>&4 "$1"
    eval_ret=$?
    eval >&3 2>&4 "$test_cleanup"
    if [ "$verbose" = "t" -a -n "$HARNESS_ACTIVE" ]; then
        echo ""
    fi
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

test_expect_failure() {
    test_count=$((test_count + 1))
    echo >&3 "checking known breakage: $2"
    if test_run "$2"; then
        test_fixed=$((test_fixed + 1))
        echo "ok $test_count - $1 # TODO known breakage"
    else
        test_broken=$((test_broken + 1))
        echo "not ok $test_count - $1 # TODO known breakage"
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

test_cmp_bin() {
    cmp -b -l "$@"
}

test_done() {
    if [ -z "$HARNESS_ACTIVE" ]; then
        test_results_dir="$TEST_DIR/test-results"
        mkdir -p "$test_results_dir"
        test_results_path="$test_results_dir/${0%.sh}-$$.counts"

        echo "total $test_count" >> $test_results_path
        echo "success $test_success" >> $test_results_path
        echo "failed $test_failure" >> $test_results_path
        echo "fixed $test_fixed" >> $test_results_path
        echo "broken $test_broken" >> $test_results_path
        echo "" >> $test_results_path
    fi

    if [ $test_failure = 0 ]; then
        echo "# passed all $test_count test(s)"
        echo "1..$test_count"

        [ -d "$remove_trash" ] &&
        cd "$(dirname "$remove_trash")" &&
        rm -rf "$(basename "$remove_trash")"

        exit 0
    else
        echo "# failed $test_failure among $test_count test(s)"
        echo "1..$test_count"
        exit 1
    fi
}

: ${TEST_DIR:=$(pwd)}
BUILD_DIR="$TEST_DIR/.."
PATH="$BUILD_DIR:$PATH"
export PATH

#
# prepare test area
#
test="trash.$(basename "$0" .sh)"
case "$test" in
    /*) TRASH_DIR="$test" ;;
    *)  TRASH_DIR="$TEST_DIR/$test" ;;
esac
remove_trash="$TRASH_DIR"

rm -rf "$test" || {
    echo >&5 "FATAL: Cannot prepare test area"
    exit 1
}

mkdir -p "$test" || exit 1
cd -P "$test" || exit 1

HOME=$(pwd)
export HOME

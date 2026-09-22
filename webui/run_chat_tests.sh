#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

# NOTE: Runs the chat parsing tests and diffs the output against the captured
# expected output, following the same ".new" against ".tst" convention as the
# regression suite in "tests". Needs neither a running server nor a browser.
#
# Usage:  ./run_chat_tests.sh              run and compare
#         ./run_chat_tests.sh -recapture   replace the expected output

cd "$(dirname "$0")" || exit 1

test_name=chat_parse_test

if [ ! -f $test_name.js ]; then
 echo "Error: Did not find '$test_name.js'."
 exit 1
fi

node $test_name.js > $test_name.new 2>&1
test_status=$?

if [ "$1" = "-recapture" ]; then
 mv $test_name.new $test_name.tst
 echo "Recaptured '$test_name.tst'."
 exit $test_status
fi

if [ ! -f $test_name.tst ]; then
 echo "Error: Did not find '$test_name.tst' (use -recapture to create it)."
 rm -f $test_name.new
 exit 1
fi

if diff -q $test_name.tst $test_name.new >/dev/null; then
 rm -f $test_name.new
 echo "Output matches '$test_name.tst'."
else
 echo "Error: Output differs from '$test_name.tst':"
 diff $test_name.tst $test_name.new
 exit 1
fi

exit $test_status

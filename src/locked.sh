#!/bin/bash
# Copyright (c) 2024-2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

LOCK_NAME=$1

exit_code=0

echo -n $$ >>"$LOCK_NAME"

cleanup( )
{
 rm -f "$LOCK_NAME"
 exit $exit_code
}

trap cleanup EXIT ERR INT TERM

shift
$*

rm -f "$LOCK_NAME"

exit_code=$?

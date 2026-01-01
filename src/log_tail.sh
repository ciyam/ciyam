#!/bin/bash
# Copyright (c) 2024-2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.
if [ $# -lt 1 ]; then
 echo "Usage: log_tail.sh <log_file>"
else
 log_file=$1

 date_var=`date`
 date_len=${#date_var}
 date_line=`eval printf '=%.0s' {1..$date_len}`

 echo "$date_line" >> $log_file
 echo "$date_var" >> $log_file
fi

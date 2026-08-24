#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if [ -f .web_device_admin ]; then
 mkdir -p test_web_info
 mv .web* test_web_info
fi

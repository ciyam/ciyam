#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

for filename in ../webui/ciyam_*.keys; do
  if [ -f "$filename" ]; then
   mv $filename $filename.sav
  fi
done

if [ -f .web_device_admin ]; then
 mkdir -p test_web_info
 mv .web* test_web_info

 if [ -f ../webui/.devices ]; then
  mv ../webui/.devices test_web_info
 fi
fi

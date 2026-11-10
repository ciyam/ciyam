#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

for filename in ../webui/ciyam_*.keys.sav; do
  if [ -f "$filename" ]; then
   mv $filename ${filename%.*}
  fi
done

if [ -d test_web_info ]; then

 if [ ! -f test_web_info/.devices ]; then
  rm -f ../webui/.devices
 else
  mv test_web_info/.devices ../webui
 fi

 mv test_web_info/.web* .
 rmdir test_web_info
fi

#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

touch ciyam_server.cmd
sleep 0.5

if [ -f ciyam_server.cmd ]; then
 rm ciyam_server.cmd
else
 touch ciyam_server.stop

 for i in {1..20}
 do
  sleep 0.5
  if [ ! -f ciyam_server.stop ]; then
   break;
  fi
 done

fi

if [ ! -f ciyam_server.stop ]; then
 ./save_identity
 ./erase_identity >/dev/null

 ../tests/store_web_user_files.sh

 mkdir -p /tmp/ciyam/.files

 touch ciyam_server.stop

 ./ciyam_server -quiet -no_udp -no_auto -no_peers -files_area=/tmp/ciyam/.files &

 for i in {1..20}
 do
  sleep 0.5
  if [ ! -f ciyam_server.stop ]; then
   break;
  fi
 done

 if [ ! -f ciyam_server.stop ]; then
   touch /tmp/ciyam/.test_node
 fi
fi

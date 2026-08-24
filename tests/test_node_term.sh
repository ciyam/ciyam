#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if [ -f /tmp/ciyam/.test_node ]; then
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

 ./restore_identity

 rm -f .web_access_10301

 ../tests/restore_web_user_files.sh

 if [ -d /tmp/ciyam/.files ]; then
  rm -rf /tmp/ciyam/.files/*
  rmdir /tmp/ciyam/.files
 fi

 rm /tmp/ciyam/.test_node
fi

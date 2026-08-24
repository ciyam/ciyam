#!/bin/bash

sleep 0.5

./save_identity
./erase_identity >/dev/null

../tests/store_web_user_files.sh

./ciyam_server -quiet -no_udp -no_auto -no_peers &

sleep 0.5

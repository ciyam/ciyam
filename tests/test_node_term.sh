#!/bin/bash

touch ciyam_server.stop

sleep 1

./restore_identity

rm -f .web_access_10301

../tests/restore_web_user_files.sh


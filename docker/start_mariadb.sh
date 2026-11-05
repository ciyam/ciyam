#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

# NOTE: Starts MariaDB in the background and waits until it answers. A container has
# no systemd, so the server is started directly rather than with "systemctl". It is
# started with "--skip-networking" - CIYAM connects to "localhost", which the client
# library takes as the Unix socket, so no TCP port is needed or opened.
#
# Usage: start_mariadb.sh

mkdir -p /run/mysqld
chown mysql:mysql /run/mysqld

mariadbd --user=mysql --skip-networking >/var/log/mariadbd.log 2>&1 &

for i in $(seq 1 30); do
 if mariadb-admin -uroot ping >/dev/null 2>&1; then
  exit 0
 fi

 sleep 1
done

echo "Error: MariaDB did not start."
tail -20 /var/log/mariadbd.log

exit 1

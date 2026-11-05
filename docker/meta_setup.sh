#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

# NOTE: Creates the Meta application's storage at build time (for "CIYAM_META=1" in the Dockerfile) - the MariaDB database
# and user ("create_db Meta .") and then the storage itself ("init Meta Meta"), as in
# "linux_lubuntu.txt". MariaDB only runs for the length of one RUN step, so it is
# started here, used and then shut down again, leaving its data in the image.
#
# The test storage step from the guide ("create_db ciyam .", "init Meta ciyam" and
# "Meta_tests.sio") is left out - it only checks the Meta application before it is
# installed, and is dropped again straight afterwards.
#
# Usage: meta_setup.sh (run from "/ciyam/src")

set -e

/usr/local/bin/start_mariadb.sh

./create_db Meta .

# NOTE: "init" starts its own "ciyam_server" (with "-no_auto"), runs "init.cin" with
# "ciyam_client" and then stops the server - so nothing else may be listening on the
# RPC port while it runs, which at build time is always so.
./init Meta Meta

# NOTE: "init" waits up to ten seconds for its server to stop but does not fail if it
# has not, so make sure no "ciyam_server" is left behind in the image's last state.
pkill -x ciyam_server 2>/dev/null || true

if [ ! -f Meta.dat ]; then
 echo "Error: 'init Meta Meta' did not create the Meta storage."

 exit 1
fi

echo "Meta tables:"
mariadb -uroot Meta -e "SHOW TABLES" | head -10

mariadb-admin -uroot shutdown

#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if [ ! -f /tmp/ciyam/.test_node ]; then
 echo "Error: Did not find '/tmp/ciyam/.test_node' (testing was skipped)."
else
 echo "Create system identity and 'admin' access with 'none'."
 node ../webui/ciyam.js "" admin de604cee0755a3d81944ea96aed12681 "" none

 touch ciyam_base.restore
 sleep 1

 echo ""
 echo "Update 'admin' access password to 'test' (after restore)."
 node ../webui/ciyam.js "" de604cee0755a3d81944ea96aed12681 "" "" test

 echo ""
 echo "Attempt to connect using 10101 with 'test'."
 node ../webui/ciyam.js -quiet "" 10101 "" "" test

 echo ""
 echo "Attempt to connect using 10201 with 'test'."
 node ../webui/ciyam.js -quiet "" 10201 "" "" test

 echo ""
 echo "Attempt to connect using 10301 with 'none'."
 node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Connect using 10301 with 'test'."
 node ../webui/ciyam.js "" 10301 "" "" test

 echo ""
 echo "Attempt to update 'admin' access password to 'none' while locked."
 node ../webui/ciyam.js -quiet "" de604cee0755a3d81944ea96aed12681 "" "" none

 touch ciyam_base.restore
 sleep 1

 echo ""
 echo "Update 'admin' access password to 'none' (after restore)."
 node ../webui/ciyam.js "" de604cee0755a3d81944ea96aed12681 "" "" none

 echo ""
 echo "Attempt to connect using 10101 with 'none'."
 node ../webui/ciyam.js -quiet "" 10101 "" "" none

 echo ""
 echo "Attempt to connect using 10201 with 'none'."
 node ../webui/ciyam.js -quiet "" 10201 "" "" none

 echo ""
 echo "Attempt to connect using 10201 with 'test'."
 node ../webui/ciyam.js -quiet "" 10301 "" "" test

 echo ""
 echo "Connect using 10301 with 'none'."
 node ../webui/ciyam.js "" 10301 "" "" none

 echo ""
 echo "Fetch the list of non-admin users."
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Create a user with access pin '11111' then list users."
 env CIYAM_NODE_COMMAND="users create suggested=11111:test-1" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Attempt to create another user with access pin '11111' then list users."
 env CIYAM_NODE_COMMAND="users create suggested=11111:test-1" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Connect using 11111 with 'none'."
 node ../webui/ciyam.js "" 11111 "" "" none

 echo ""
 echo "Attempt to connect using 11111 with 'test'."
 node ../webui/ciyam.js -quiet "" 11111 "" "" test

 echo ""
 echo "Update own password for 11111 from 'none' to 'test'."
 env CIYAM_NODE_COMMAND="users update 11111 password=test" node ../webui/ciyam.js -quiet "" 11111 "" "" none

 echo ""
 echo "Connect using 11111 with 'test'."
 node ../webui/ciyam.js "" 11111 "" "" test

 echo ""
 echo "Update password for 11111 to 'none' as admin."
 env CIYAM_NODE_COMMAND="users update 11111 password=none" node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Connect using 11111 with 'none'."
 node ../webui/ciyam.js "" 11111 "" "" none

 echo ""
 echo "Create a user with access pin '22222' then list users."
 env CIYAM_NODE_COMMAND="users create suggested=22222:?test-2" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Attempt to connect using 22222:test with 'test'."
 node ../webui/ciyam.js -quiet "" 22222:test "" "" test

 echo ""
 echo "Connect using 22222:test with 'test' (after suggestion)."
 node ../webui/ciyam.js "" 22222:test "" "" test

 echo ""
 echo "Attempt to update password for 11111 to 'test' as 22222."
 env CIYAM_NODE_COMMAND="users update 11111 password=test" node ../webui/ciyam.js -quiet "" 22222 "" "" test

 echo ""
 echo "Create a user with access pin '33333' then list users."
 env CIYAM_NODE_COMMAND="users create suggested=33333:?test-3" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Attempt to connect using 33333:test with 'test'."
 node ../webui/ciyam.js -quiet "" 33333:test "" "" test

 echo ""
 echo "Attempt to connect using 33333:test with 'test' (after suggestion)."
 node ../webui/ciyam.js -quiet "" 33333:test "" "" test

 echo ""
 echo "Connect using 33333:testing with 'test'."
 node ../webui/ciyam.js "" 33333:testing "" "" test

 echo ""
 echo "Attempt to connect using 33333 with 'none'."
 node ../webui/ciyam.js -quiet "" 33333 "" "" none

 echo ""
 echo "Delete user with access pin '11111' then list users."
 env CIYAM_NODE_COMMAND="users delete 11111" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Attempt to delete user with access pin '11111' again then list users."
 env CIYAM_NODE_COMMAND="users delete 11111" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Attempt to connect using 11111 with 'none'."
 node ../webui/ciyam.js -quiet "" 11111 "" "" none

 echo ""
 echo "Create 'xxxxx' token for pin '12345'."
 echo -n "12345" > .web_access_xxxxx

 echo ""
 echo "Connect using xxxxx:test-x with 'test' then list users."
 node ../webui/ciyam.js "" xxxxx:test-x "" "" test
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Delete user with access pin '22222' then list users."
 env CIYAM_NODE_COMMAND="users delete 22222" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Delete user with access pin '33333' then list users."
 env CIYAM_NODE_COMMAND="users delete 33333" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none

 echo ""
 echo "Delete user with access pin '12345' then list users."
 env CIYAM_NODE_COMMAND="users delete 12345" node ../webui/ciyam.js -quiet "" 10301 "" "" none
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 "" "" none
fi

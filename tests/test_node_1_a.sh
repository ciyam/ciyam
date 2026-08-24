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
 sleep 0.5

 echo "Update 'admin' access password to 'test' (after restore)."
 node ../webui/ciyam.js "" de604cee0755a3d81944ea96aed12681 "" "" test

 echo "Attempt to connect using 10101 with 'test'."
 node ../webui/ciyam.js "" 10101 "" "" test

 echo ""
 echo "Attempt to connect using 10201 with 'test'."
 node ../webui/ciyam.js "" 10201 "" "" test

 echo ""
 echo "Attempt to connect using 10301 with 'none'."
 node ../webui/ciyam.js "" 10301 "" "" none

 echo ""
 echo "Connect using 10301 with 'test'."
 node ../webui/ciyam.js "" 10301 "" "" test

 echo "Attempt to update 'admin' access password to 'none' while locked."
 node ../webui/ciyam.js "" de604cee0755a3d81944ea96aed12681 "" "" none

 touch ciyam_base.restore
 sleep 0.5

 echo ""
 echo "Update 'admin' access password to 'none' (after restore)."
 node ../webui/ciyam.js "" de604cee0755a3d81944ea96aed12681 "" "" none

 echo "Attempt to connect using 10101 with 'none'."
 node ../webui/ciyam.js "" 10101 "" "" none

 echo ""
 echo "Attempt to connect using 10201 with 'none'."
 node ../webui/ciyam.js "" 10201 "" "" none

 echo ""
 echo "Attempt to connect using 10201 with 'test'."
 node ../webui/ciyam.js "" 10301 "" "" test

 echo ""
 echo "Connect using 10201 with 'none'."
 node ../webui/ciyam.js "" 10301 "" "" none
fi

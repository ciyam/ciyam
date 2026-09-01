#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if [ ! -f /tmp/ciyam/.test_node ]; then
 echo "Error: Did not find '/tmp/ciyam/.test_node' (testing was skipped)."
else
 echo "Create a new device for use with message tests."
 device=$(node ../webui/ciyam.js -test "" 10301 "" "" none)
 echo "(created device - length is ${#device})"

 ./system_variable set @no_irc ""
 ./system_variable set @irc_test 1
 ./system_variable set @irc_allow 1

 echo "Output the entrance room details."
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Create an initial test message."
 env CIYAM_NODE_COMMAND="messages create 0000001 text=testing..." node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Output the entrance room details."
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Review messages in room 0000001."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Create a new user '11111' named 'test-1'."
 env CIYAM_NODE_COMMAND="users create nominated=11111:test-1" node ../webui/ciyam.js -quiet "" 10301 $device "" none

 echo "Connect using 11111 with 'none' and output entrance room details."
 node ../webui/ciyam.js "" 11111 $device "" none
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 11111 $device "" none

 echo "Review messages in room 0000001 for 'test-1'."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 11111 $device "" none

 echo "Create an initial message for 'test-1' and output entrance room details."
 env CIYAM_NODE_COMMAND="messages create 0000001 text=test-1 here..." node ../webui/ciyam.js -test "" 11111 $device "" none
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 11111 $device "" none

 echo "Review messages in room 0000001 for 'test-1'."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 11111 $device "" none

 echo "Output the entrance room details for 'admin'."
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Review messages in room 0000001 for 'admin'."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Create a new user '22222' named 'test-2'."
 env CIYAM_NODE_COMMAND="users create nominated=22222:test-2" node ../webui/ciyam.js -quiet "" 10301 $device "" none

 echo "Connect using 22222 with 'none' and output entrance room details."
 node ../webui/ciyam.js "" 22222 $device "" none
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 22222 $device "" none

 echo "Review messages in room 0000001 for 'test-2'."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 22222 $device "" none

 echo "Create an initial message for 'test-2' and output entrance room details."
 env CIYAM_NODE_COMMAND="messages create 0000001 text=test-2 here..." node ../webui/ciyam.js -test "" 22222 $device "" none
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 22222 $device "" none

 echo "Review messages in room 0000001 for 'test-2'."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 22222 $device "" none

 echo "Output entrance room details for 'admin'."
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Review messages in room 0000001 for 'admin'."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Create a new romm for 'test-1' and 'test-2' and output entrance room details for 'test-1'."
 new_room=$(env CIYAM_NODE_COMMAND="messages create 0000000 for=test-2;text=Private (test-1 and test-2)" node ../webui/ciyam.js -test "" 11111 $device "" none)
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 11111 $device "" none

 echo "Review messages in room 0000001 again for 'test-2'."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 22222 $device "" none | sed "s/$new_room/0000002-NEW-ROOM-UUID-VALUE/g"

 echo "Join new romm for 'test-2', create an initial message for the new room and output entrace room details."
 env CIYAM_NODE_COMMAND="messages review $new_room" node ../webui/ciyam.js -test "" 22222 $device "" none | sed "s/$new_room/0000002-NEW-ROOM-UUID-VALUE/g"
 env CIYAM_NODE_COMMAND="messages create 0000002 text=hi..." node ../webui/ciyam.js -test "" 22222 $device "" none
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 22222 $device "" none

 echo "Output entrance room details for 'test-1' and then review messages in rooms 0000001 and 0000002."
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 11111 $device "" none
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 11111 $device "" none
 env CIYAM_NODE_COMMAND="messages review 0000002" node ../webui/ciyam.js -test "" 11111 $device "" none | sed "s/$new_room/0000002-NEW-ROOM-UUID-VALUE/g"
fi

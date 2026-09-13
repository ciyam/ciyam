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
 echo "messages" >/tmp/ciyam/$device.lst

 echo "echo Create an initial test message." >>/tmp/ciyam/$device.lst
 echo "messages create 0000001 text=testing..." >>/tmp/ciyam/$device.lst

 echo "echo Output the entrance room details." >>/tmp/ciyam/$device.lst
 echo "messages" >>/tmp/ciyam/$device.lst

 echo "echo Review messages in room 0000001." >>/tmp/ciyam/$device.lst
 echo "messages review 0000001" >>/tmp/ciyam/$device.lst

 echo "echo Create a new user '11111' named 'test-1'." >>/tmp/ciyam/$device.lst
 echo "users create nominated=11111:test-1" >>/tmp/ciyam/$device.lst

 env CIYAM_NODE_COMMAND=@/tmp/ciyam/$device.lst node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Connect using 11111 with 'none' and output entrance room details."
 node ../webui/ciyam.js "" 11111 $device "" none

 echo "messages" >/tmp/ciyam/$device.lst

 echo "echo Review messages in room 0000001 for 'test-1'." >>/tmp/ciyam/$device.lst
 echo "messages review 0000001" >>/tmp/ciyam/$device.lst

 echo "echo Create an initial message for 'test-1' and output entrance room details." >>/tmp/ciyam/$device.lst
 echo "messages create 0000001 text=test-1 here..." >>/tmp/ciyam/$device.lst
 echo "messages" >>/tmp/ciyam/$device.lst

 echo "echo Review messages in room 0000001 for 'test-1'." >>/tmp/ciyam/$device.lst
 echo "messages review 0000001" >>/tmp/ciyam/$device.lst

 env CIYAM_NODE_COMMAND=@/tmp/ciyam/$device.lst node ../webui/ciyam.js -test "" 11111 $device "" none

 echo "Output the entrance room details for 'admin'."
 echo "messages" >/tmp/ciyam/$device.lst

 echo "echo Review messages in room 0000001 for 'admin'." >>/tmp/ciyam/$device.lst
 echo "messages review 0000001" >>/tmp/ciyam/$device.lst

 echo "echo Create a new user '22222' named 'test-2'." >>/tmp/ciyam/$device.lst
 echo "users create nominated=22222:test-2" >>/tmp/ciyam/$device.lst

 env CIYAM_NODE_COMMAND=@/tmp/ciyam/$device.lst node ../webui/ciyam.js -test "" 10301 $device "" none

 echo "Connect using 22222 with 'none' and output entrance room details."
 node ../webui/ciyam.js "" 22222 $device "" none

 echo "messages" >/tmp/ciyam/$device.lst

 echo "echo Review messages in room 0000001 for 'test-2'." >>/tmp/ciyam/$device.lst
 echo "messages review 0000001" >>/tmp/ciyam/$device.lst

 echo "echo Create an initial message for 'test-2' and output entrance room details." >>/tmp/ciyam/$device.lst
 echo "messages create 0000001 text=test-2 here..." >>/tmp/ciyam/$device.lst
 echo "messages" >>/tmp/ciyam/$device.lst

 echo "echo Review messages in room 0000001 for 'test-2'." >>/tmp/ciyam/$device.lst
 echo "messages review 0000001" >>/tmp/ciyam/$device.lst

 env CIYAM_NODE_COMMAND=@/tmp/ciyam/$device.lst node ../webui/ciyam.js -test "" 22222 $device "" none

 echo "Output entrance room details for 'admin'."
 echo "messages" >/tmp/ciyam/$device.lst

 echo "echo Review messages in room 0000001 for 'admin'." >>/tmp/ciyam/$device.lst
 echo "messages review 0000001" >>/tmp/ciyam/$device.lst

 echo "echo Attempt to rename room 0000001 with an invalid name." >>/tmp/ciyam/$device.lst
 echo "messages create 0000000 for=0000001;text=Name_Not_Valid" >>/tmp/ciyam/$device.lst

 echo "echo Rename room 0000001 to Testing and output entrance room details." >>/tmp/ciyam/$device.lst
 echo "messages create 0000000 for=0000001;text=Testing" >>/tmp/ciyam/$device.lst
 echo "messages" >>/tmp/ciyam/$device.lst

 echo "echo Rename room 0000001 back to Administration and output entrance room details." >>/tmp/ciyam/$device.lst
 echo "messages create 0000000 for=0000001;text=Administration" >>/tmp/ciyam/$device.lst
 echo "messages" >>/tmp/ciyam/$device.lst

 env CIYAM_NODE_COMMAND=@/tmp/ciyam/$device.lst node ../webui/ciyam.js -test "" 10301 $device "" none

 rm -f /tmp/ciyam/$device.lst

 echo "Create a new romm for 'test-1' and 'test-2' and output entrance room details for 'test-1' then rename the new room and output entrance again."
 new_room=$(env CIYAM_NODE_COMMAND="messages create 0000000 for=test-2;text=Private (test-1 and test-2)" node ../webui/ciyam.js -test "" 11111 $device "" none)
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 11111 $device "" none
 env CIYAM_NODE_COMMAND="messages create 0000000 for=0000002;text=Testing (test-1 and test-2)" node ../webui/ciyam.js -test "" 11111 $device "" none
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 11111 $device "" none

 echo "Review messages in room 0000001 again for 'test-2' then attempt to rename room 0000002."
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 22222 $device "" none | sed "s/$new_room/0000002-NEW-ROOM-UUID-VALUE/g"
 env CIYAM_NODE_COMMAND="messages create 0000000 for=0000002;text=Renamed (test-1 and test-2)" node ../webui/ciyam.js -test "" 22222 $device "" none

 echo "Join new romm for 'test-2', create an initial message for the new room and output entrace room details."
 env CIYAM_NODE_COMMAND="messages review 0000002 from=${new_room:8}" node ../webui/ciyam.js -test "" 22222 $device "" none
 env CIYAM_NODE_COMMAND="messages create 0000002 from=0;text=hi..." node ../webui/ciyam.js -test "" 22222 $device "" none
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 22222 $device "" none

 echo "Output entrance room details for 'test-1' and then review messages in rooms 0000001 and 0000002."
 env CIYAM_NODE_COMMAND=messages node ../webui/ciyam.js -test "" 11111 $device "" none
 env CIYAM_NODE_COMMAND="messages review 0000001" node ../webui/ciyam.js -test "" 11111 $device "" none
 env CIYAM_NODE_COMMAND="messages review 0000002 from=0" node ../webui/ciyam.js -test "" 11111 $device "" none | sed "s/$new_room/0000002-NEW-ROOM-UUID-VALUE/g"
fi

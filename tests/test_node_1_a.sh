#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

check_if_started( )
{
 for i in {1..10}
 do
  sleep 0.5
  if [ ! -f ciyam_server.stop ]; then
   break;
  fi
 done
}

if [ ! -f /tmp/ciyam/.test_node ]; then
 echo "Error: Did not find '/tmp/ciyam/.test_node' (testing was skipped)."
else
 echo "Initial basic check for node and 'ciyam.js'."
 node ../webui/ciyam.js ""

 test_password=a_test_password_long_enough_for_no_hashes
 dummy_password=not_an_actual_password_but_is_a_long_one

 echo "Create external system identity and 'admin' access with '\$dummy_password'."
 device=$(node ../webui/ciyam.js -test "" admin df4b24aeadd7a6762a12d1f29dcd0eaccf8b2496189ee6ea1304704862822d52873c0e69ed11f90d90487e9c32d8fce7e54e96d59f2a8f3abd10e76c11c6202e686190cd "" $dummy_password)
 echo "(created device - length is ${#device})"

 ADMIN=$(<.web_access_admin)

 echo ""
 echo "Connect using ADMIN with '\$dummy_password'."
 node ../webui/ciyam.js "" $ADMIN $device "" $dummy_password

 echo ""
 echo "Employ master password as an unlock key."
 env CIYAM_NODE_COMMAND="unlock-keys employ need_to_use_a_very_good_password" node ../webui/ciyam.js -quiet "" $ADMIN $device "" $dummy_password

 echo ""
 echo "Connect using ADMIN with '\$dummy_password'."
 node ../webui/ciyam.js "" $ADMIN $device "" $dummy_password

 touch ciyam_base.restore ciyam_server.stop

 check_if_started

 echo ""
 echo "Attempt restoring and updating the 'admin' password using incorrect entropy."
 node ../webui/ciyam.js "" de604cee0755a3d81944ea96aed12680 $device "" $test_password

 echo ""
 echo "Update 'admin' access password to '\$test_password' and change identity to internal (after restore)."
 node ../webui/ciyam.js "" de604cee0755a3d81944ea96aed12681 $device "" $test_password

 echo ""
 echo "Attempt to connect using 'dummy' with '\$test_password'."
 node ../webui/ciyam.js -quiet "" dummy "" "" $test_password

 echo ""
 echo "Attempt to connect using 10101 with '\$test_password'."
 node ../webui/ciyam.js -quiet "" 10101 "" "" $test_password

 echo ""
 echo "Attempt to connect using 10201 with '\$test_password'."
 node ../webui/ciyam.js -quiet "" 10201 "" "" $test_password

 echo ""
 echo "Attempt to connect using 10301 with '\$dummy_password'."
 node ../webui/ciyam.js -quiet "" 10301 "" "" $dummy_password

 echo ""
 echo "Connect using 10301 with '\$test_password'."
 node ../webui/ciyam.js "" 10301 $device "" $test_password

 echo ""
 echo "Attempt to update 'admin' access password to '\$dummy_password' while not locked."
 node ../webui/ciyam.js -quiet "" de604cee0755a3d81944ea96aed12681 "" "" $dummy_password

 touch ciyam_base.restore ciyam_server.stop

 check_if_started

 echo ""
 echo "Connect using 10301 with '\$test_password' (after restore)."
 node ../webui/ciyam.js "" 10301 $device "" $test_password

 echo ""
 echo "Update 'admin' access password to '\$dummy_password' (after restore)."
 node ../webui/ciyam.js "" de604cee0755a3d81944ea96aed12681 $device "" $dummy_password

 echo ""
 echo "Attempt to connect using 10101 with '\$dummy_password'."
 node ../webui/ciyam.js -quiet "" 10101 "" "" $dummy_password

 echo ""
 echo "Attempt to connect using 10201 with '\$dummy_password'."
 node ../webui/ciyam.js -quiet "" 10201 "" "" $dummy_password

 echo ""
 echo "Attempt to connect using 10201 with '\$test_password'."
 node ../webui/ciyam.js -quiet "" 10301 "" "" $test_password

 echo ""
 echo "Connect using 10301 with '\$dummy_password' and create an unlock key."
 node ../webui/ciyam.js "" 10301 $device "" $dummy_password
 unlock_key=$(env CIYAM_NODE_COMMAND="unlock-keys create" node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password)

 touch ciyam_base.restore ciyam_server.stop

 check_if_started

 echo ""
 echo "Employ the previously created unlock key (after restore)."
 echo "unlock-keys employ $unlock_key" >/tmp/ciyam/$device.lst

 echo "echo" >>/tmp/ciyam/$device.lst
 echo "echo Fetch the list of non-admin users." >>/tmp/ciyam/$device.lst
 echo "users" >>/tmp/ciyam/$device.lst

 echo "echo" >>/tmp/ciyam/$device.lst
 echo "echo Create a user with access pin '11111' then list users." >>/tmp/ciyam/$device.lst
 echo "users create nominated=11111:test-1" >>/tmp/ciyam/$device.lst
 echo "users" >>/tmp/ciyam/$device.lst

 echo "echo" >>/tmp/ciyam/$device.lst
 echo "echo Attempt to create another user with access pin '11111' then list users." >>/tmp/ciyam/$device.lst
 echo "users create nominated=11111:test-1" >>/tmp/ciyam/$device.lst
 echo "users" >>/tmp/ciyam/$device.lst

 echo "echo" >>/tmp/ciyam/$device.lst
 echo "echo Attempt to create a user with invalid access pin '1111' then list users." >>/tmp/ciyam/$device.lst
 echo "users create nominated=1111:test-1" >>/tmp/ciyam/$device.lst
 echo "users" >>/tmp/ciyam/$device.lst

 env CIYAM_NODE_COMMAND=@/tmp/ciyam/$device.lst node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password

 echo ""
 echo "Connect using 11111 with '\$dummy_password' and list users."
 node ../webui/ciyam.js "" 11111 $device "" $dummy_password
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 11111 $device "" $dummy_password

 echo ""
 echo "Attempt to connect using 11111 with '\$test_password'."
 node ../webui/ciyam.js -quiet "" 11111 "" "" $test_password

 echo ""
 echo "Update own password for 11111 from '\$dummy_password' to '\$test_password'."
 env CIYAM_NODE_COMMAND="users update 11111 password=$test_password" node ../webui/ciyam.js -quiet "" 11111 $device "" $dummy_password

 echo ""
 echo "Connect using 11111 with '\$test_password'."
 node ../webui/ciyam.js "" 11111 $device "" $test_password

 echo ""
 echo "Update password for 11111 to '\$dummy_password' as admin."
 env CIYAM_NODE_COMMAND="users update 11111 password=$dummy_password" node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password

 echo ""
 echo "Connect using 11111 with '\$dummy_password'."
 node ../webui/ciyam.js "" 11111 $device "" $dummy_password

 echo ""
 echo "Create a user with access pin '22222' then list users."
 env CIYAM_NODE_COMMAND="users create nominated=22222:?test-2" node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password

 echo ""
 echo "Attempt to connect using 22222:test with '\$test_password'."
 node ../webui/ciyam.js -quiet "" 22222:test "" "" $test_password

 echo ""
 echo "Connect using 22222:test with '\$test_password' (after suggestion)."
 node ../webui/ciyam.js "" 22222:test $device "" $test_password

 echo ""
 echo "Attempt to update password for 11111 to '\$test_password' as 22222."
 env CIYAM_NODE_COMMAND="users update 11111 password=$test_password" node ../webui/ciyam.js -quiet "" 22222 "" "" $test_password

 echo ""
 echo "Create a user with access pin '33333' then list users."
 env CIYAM_NODE_COMMAND="users create nominated=33333:?test-3" node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password

 echo ""
 echo "Attempt to connect using 33333:test with '\$test_password'."
 node ../webui/ciyam.js -quiet "" 33333:test "" "" $test_password

 echo ""
 echo "Attempt to connect using 33333:test with '\$test_password' (after suggestion)."
 node ../webui/ciyam.js -quiet "" 33333:test "" "" $test_password

 echo ""
 echo "Connect using 33333:testing with '\$test_password' and list users"
 node ../webui/ciyam.js "" 33333:testing $device "" $test_password
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 33333 $device "" $test_password

 echo ""
 echo "Attempt to connect using 33333 with '\$dummy_password'."
 node ../webui/ciyam.js -quiet "" 33333 "" "" $dummy_password

 echo ""
 echo "Delete user with access pin '11111' then list users."
 env CIYAM_NODE_COMMAND="users delete 11111" node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password

 echo ""
 echo "Attempt to delete user with access pin '11111' again then list users."
 env CIYAM_NODE_COMMAND="users delete 11111" node ../webui/ciyam.js -quiet "" 10301 "" "" $dummy_password
 env CIYAM_NODE_COMMAND=users node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password

 echo ""
 echo "Attempt to connect using 11111 with '\$dummy_password'."
 node ../webui/ciyam.js -quiet "" 11111 "" "" $dummy_password

 echo ""
 echo "Create a secret token using 10301."
 secret=$(env CIYAM_NODE_COMMAND="users create '' secret" node ../webui/ciyam.js -test "" 10301 "" "" $dummy_password)

 echo ""
 echo "Force 'the <secret>' token to be for pin '12345'."
 echo -n "12345" > .web_$secret

 echo ""
 echo "Connect using <secret>:test-x with '\$test_password' then list users."
 node ../webui/ciyam.js "" $secret:test-x $device "" $test_password

 echo "users" >/tmp/ciyam/$device.lst

 echo "echo" >>/tmp/ciyam/$device.lst
 echo "echo Delete user with access pin '22222' then list users." >>/tmp/ciyam/$device.lst
 echo "users delete 22222" >>/tmp/ciyam/$device.lst
 echo "users" >>/tmp/ciyam/$device.lst

 echo "echo" >>/tmp/ciyam/$device.lst
 echo "echo Delete user with access pin '33333' then list users." >>/tmp/ciyam/$device.lst
 echo "users delete 33333" >>/tmp/ciyam/$device.lst
 echo "users" >>/tmp/ciyam/$device.lst

 echo "echo" >>/tmp/ciyam/$device.lst
 echo "echo Delete user with access pin '12345' then list users." >>/tmp/ciyam/$device.lst
 echo "users delete 12345" >>/tmp/ciyam/$device.lst
 echo "users" >>/tmp/ciyam/$device.lst

 env CIYAM_NODE_COMMAND=@/tmp/ciyam/$device.lst node ../webui/ciyam.js -quiet "" 10301 $device "" $dummy_password

 rm -f /tmp/ciyam/$device.lst
fi

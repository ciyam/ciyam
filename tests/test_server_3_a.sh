#!/bin/bash

# Simple API tests.
curl -s "localhost:13031/ip-addr"
curl -s "localhost:13031/version"
curl -s -4 "localhost:13031/ip-addr?format=text"
curl -s "localhost:13031/ip-addr?format=json"
curl -s "localhost:13031/version?format=json"
curl -s -4 "localhost:13031/ip-addr?format=json"
curl -s "localhost:13031/version"
curl -s "localhost:13031/version?format=xxxx"
curl -s "localhost:13031/version?format=text&extra="
curl -s "localhost:13031/post-limit"
curl -s "localhost:13031/post-limit?format=json"
curl -s "localhost:13031/post-limit?format=json&verbose=true"
curl -s -d "testing..." "localhost:13031/echo"
curl -s -d "testing..." "localhost:13031/echo?format=json"

# Basic file upload API tests.
filename=$(curl -s -F "file=@test.jpg" localhost:13031/upload)
diff test.jpg $filename
rm -f $filename
filename=$(curl -s -F "file=@test.png" localhost:13031/upload)
diff test.png $filename
rm -f $filename

# Basic web session API tests.
../tests/store_web_user_files.sh
echo -n "12345" > .web_access_admin
echo -n "f2ca1bb6c7e907d06dafe4687e579fce76b37e4e93b7605022da52e6ccc26fd2" > .web_access_12345
touch cws_force_admin_update
device=$(curl -s -X POST "localhost:13031/cws/devices?access=12345")
unique=$(curl -s -X POST "localhost:13031/cws/sessions?access=12345&device=$device")
checked=$(echo -n "f2ca1bb6c7e907d06dafe4687e579fce76b37e4e93b7605022da52e6ccc26fd2$device" | sha256sum | head -c 64)
session=$(echo -n "$checked$unique" | sha256sum | head -c 20)
curl -s -X GET "localhost:13031/cws/status?access=12345&device=bad_device&session=$session"
curl -s -X GET "localhost:13031/cws/status?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/status?access=12345&device=$device&session=bad_session"
curl -s -X GET "localhost:13031/cws/status?access=12345&device=$device&session=$session"
curl -s -X POST "localhost:13031/cws/unlock-keys?access=12345&device=$device&session=$session"
echo "Reserve a new access token and suggest the username 'test-1'."
new_access=$(curl -s -X POST "localhost:13031/cws/users?access=12345&device=$device&options=suggested%3d88888%3atest-1&session=$session")
echo "Prepare to activate the newly reserved access token."
curl -s -X POST "localhost:13031/cws?access=$new_access"
echo "Attempt to use 'test_1' rather than 'test-1' username."
new_passwd=$(echo -n "test_1:password" | base64)
curl -s -X POST "localhost:13031/cws/devices?access=$new_access&passwd=$new_passwd"
new_passwd=$(echo -n "test-1:password" | base64)
echo "Activates the access token using the suggested 'test-1' username."
new_device=$(curl -s -X POST "localhost:13031/cws/devices?access=$new_access&passwd=$new_passwd")
new_unique=$(curl -s -X POST "localhost:13031/cws/sessions?access=$new_access&device=$new_device")
new_checked=$(echo -n "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8$new_device" | sha256sum | head -c 64)
new_session=$(echo -n "$new_checked$new_unique" | sha256sum | head -c 20)
curl -s "localhost:13031/cws/help?access=$new_access&device=$new_device&session=bad_session"
curl -s "localhost:13031/cws/help?access=$new_access&device=$new_device&session=$new_session"
echo "Check for messages and then create a test message and a new room."
summary=$(curl -s "localhost:13031/cws/messages?access=$new_access&device=$new_device&session=$new_session")
echo ${summary#* }
message=$(curl -s -X POST "localhost:13031/cws/messages/0000001?access=$new_access&device=$new_device&options=text%3Dtesting...&session=$new_session" | tail -n 1)
echo ${message#* }
room_info=$(curl -s -X POST "localhost:13031/cws/messages/0000000?access=$new_access&device=$new_device&options=text%3DTesting&session=$new_session")
echo ${room_info%-*}
echo "Now check for messages as admin and join the room created by test-1."
summary=$(curl -s "localhost:13031/cws/messages?access=12345&device=$device&session=$session")
echo ${summary#* }
curl -s "localhost:13031/cws/messages/0000001?access=12345&device=$device&session=$session" | cut -d " " -f 2-
new_room_num=${room_info%-*}
new_room_uuid=${room_info#*-}
message=$(curl -s "localhost:13031/cws/messages/$new_room_num?access=12345&device=$device&options=from%3D$new_room_uuid&session=$session")
echo ${message#* }
echo "Now will perform structured I/O queries for enums/lists and views after attaching to the Meta storage."
curl -s -X POST "localhost:13031/cws/storages/meta?access=$new_access&device=$new_device&session=$new_session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/enums?access=$new_access&device=$new_device&session=$new_session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/lists?access=$new_access&device=$new_device&session=$new_session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/views?access=$new_access&device=$new_device&session=$new_session"
curl -s -X DELETE "localhost:13031/cws/sessions/$new_session?access=$new_access&device=$new_device"
echo "Now will list all standard access tokens and then remove the previously activated new user access token."
curl -s "localhost:13031/cws/users?access=12345&device=$device&session=$session"
curl -s -X DELETE "localhost:13031/cws/users/$new_access?access=12345&device=$device&session=$session"
curl -s -X POST "localhost:13031/cws/storages/meta?access=12345&device=$device&session=$session"
echo "Finally will perform admin structured I/O queries for enums/lists and views after attaching to the Meta storage."
curl -s "localhost:13031/cws/help?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-modules?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-modules/99/enums?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/enums?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/enums/575?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/lists?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/lists/601010?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/views?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-modules/100/views/601020?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/100100?access=12345&device=$device&options=fields%3dUsername%2cUser_Hash&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/100100/guest?access=12345&device=$device&options=fields%3dUsername%2cUser_Hash&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/102100?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/102100/guests-bool?access=12345&device=$device&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/102100/guests-bool?access=12345&device=$device&options=key%3dguests_xool&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/102100/guests-bool?access=12345&device=$device&options=key%3dguests_bool&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/102100?access=12345&device=$device&options=num%3D-4%3Bpath%3DWorkgroup%3Dguests%2CName%3Dint%3Bfields%3DName%2CId&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/102100?access=12345&device=$device&options=num%3D%2B4%3Bpath%3DWorkgroup%3Dguests%2CName%3Dint%3Bfields%3DName%2CId&session=$session"
curl -s -X GET "localhost:13031/cws/storage-instances/100/102100?access=12345&device=$device&options=num%3D%2B4%3Bpath%3DWorkgroup%3Dguests%2CName%3Dint%3Bquery%3DName%3D*e*%3Bfields%3DName%2CId&session=$session"
curl -s -X DELETE "localhost:13031/cws/sessions/$session?access=12345&device=$device"
curl -s -X GET "localhost:13031/cws/status?access=12345&device=$device&session=$session"
rm -f .web_access_12345 .web_access_admin .web_device_admin
../tests/restore_web_user_files.sh

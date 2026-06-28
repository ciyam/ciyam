#!/bin/bash

# Simple API tests.
curl -s "localhost:13031/api.ip_addr"
curl -s "localhost:13031/api.version"
curl -s -4 "localhost:13031/api.ip_addr?format=text"
curl -s "localhost:13031/api.ip_addr?format=json"
curl -s "localhost:13031/api.version?format=json"
curl -s -4 "localhost:13031/api.ip_addr?format=json"
curl -s "localhost:13031/api.version"
curl -s "localhost:13031/api.version?format=xxxx"
curl -s "localhost:13031/api.version?format=text&extra="
curl -s "localhost:13031/api.post_limit"
curl -s "localhost:13031/api.post_limit?format=json"
curl -s "localhost:13031/api.post_limit?format=json&verbose=true"
curl -s -d "testing..." "localhost:13031/api.echo"
curl -s -d "testing..." "localhost:13031/api.echo?format=json"

# Basic file upload API tests.
filename=$(curl -s -F "file=@test.jpg" localhost:13031/api.upload)
diff test.jpg $filename
rm -f $filename
filename=$(curl -s -F "file=@test.png" localhost:13031/api.upload)
diff test.png $filename
rm -f $filename

# Basic web session API tests.
../tests/store_web_user_files.sh
echo -n "12345" > .web_access_admin
echo -n "f2ca1bb6c7e907d06dafe4687e579fce76b37e4e93b7605022da52e6ccc26fd2" > .web_access_12345
touch cws_force_admin_update
device=$(curl -s -X POST "localhost:13031/api.cws?access=12345")
unique=$(curl -s -X POST "localhost:13031/api.cws?access=12345&device=$device")
checked=$(echo -n "f2ca1bb6c7e907d06dafe4687e579fce76b37e4e93b7605022da52e6ccc26fd2$device" | sha256sum | head -c 64)
session=$(echo -n "$checked$unique" | sha256sum | head -c 20)
curl -s -X POST "localhost:13031/api.cws?access=12345&device=bad_device&session=$session"
curl -s -X POST "localhost:13031/api.cws?access=12345&device=$device&session=$session"
curl -s -X POST "localhost:13031/api.cws?access=12345&device=$device&session=bad_session"
curl -s -X POST "localhost:13031/api.cws?access=12345&device=$device&session=$session"
curl -s -X POST "localhost:13031/api.cws?access=12345&device=$device&request=unlock&session=$session"
echo "Reserve a new access token and suggest the username 'test-1'."
new_access=$(curl -s -X POST "localhost:13031/api.cws?access=12345&device=$device&request=access_create%20-user_info%3d88888%3atest-1&session=$session")
echo "Prepare to activate the newly reserved access token."
curl -s -X POST "localhost:13031/api.cws?access=$new_access"
echo "Attempt to use 'test_1' rather than 'test-1' username."
new_passwd=$(echo -n "test_1:password" | base64)
curl -s -X POST "localhost:13031/api.cws?access=$new_access&passwd=$new_passwd"
new_passwd=$(echo -n "test-1:password" | base64)
echo "Activates the access token using the suggested 'test-1' username."
new_device=$(curl -s -X POST "localhost:13031/api.cws?access=$new_access&passwd=$new_passwd")
new_unique=$(curl -s -X POST "localhost:13031/api.cws?access=$new_access&device=$new_device")
new_checked=$(echo -n "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8$new_device" | sha256sum | head -c 64)
new_session=$(echo -n "$new_checked$new_unique" | sha256sum | head -c 20)
curl -s "localhost:13031/api.cws?access=$new_access&device=$new_device&request=help&session=bad_session"
curl -s "localhost:13031/api.cws?access=$new_access&device=$new_device&request=help&session=$new_session"
curl -s -X POST "localhost:13031/api.cws?access=$new_access&device=$new_device&request=quit&session=$new_session"
echo "Now will list all standard access tokens and then remove the previously activated new user access token."
curl -s "localhost:13031/api.cws?access=12345&device=$device&request=access_tokens&session=$session"
curl -s -X DELETE "localhost:13031/api.cws?access=12345&device=$device&request=access_remove%20$new_access&session=$session"
curl -s -X POST "localhost:13031/api.cws?access=12345&device=$device&request=quit&session=$session"
curl -s -X POST "localhost:13031/api.cws?access=12345&device=$device&session=$session"
rm -f .web_access_12345 .web_access_admin .web_device_admin
../tests/restore_web_user_files.sh

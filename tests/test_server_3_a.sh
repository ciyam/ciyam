#!/bin/bash

# Simple API tests.
curl -s "localhost:13031/ip_addr"
curl -s "localhost:13031/version"
curl -s -4 "localhost:13031/ip_addr?format=text"
curl -s "localhost:13031/ip_addr?format=json"
curl -s "localhost:13031/version?format=json"
curl -s -4 "localhost:13031/ip_addr?format=json"
curl -s "localhost:13031/version?format=none"
curl -s "localhost:13031/version?format=text&extra="
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
echo -n "pwd_hash_value" > .web_session.test
device=$(curl -s "localhost:13031/cws?token=test")
unique=$(curl -s "localhost:13031/cws?token=test&device=$device")
session=$(echo -n "pwd_hash_value$unique" | sha256sum | head -c 10)
curl -s "localhost:13031/cws?token=test&device=bad_device&session=$session"
curl -s "localhost:13031/cws?token=test&device=$device&session=$session"
curl -s "localhost:13031/cws?token=test&device=$device&session=bad_session"
curl -s "localhost:13031/cws?token=test&device=$device&session=$session"
curl -s "localhost:13031/cws?token=test&device=$device&session=$session&function=terminate"
curl -s "localhost:13031/cws?token=test&device=$device&session=$session"
rm -f .web_session.test

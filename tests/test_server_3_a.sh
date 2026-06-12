#!/bin/bash

# Simple API tests.
curl -s "localhost:13031/api.ip_addr"
curl -s "localhost:13031/api.version"
curl -s -4 "localhost:13031/api.ip_addr?format=text"
curl -s "localhost:13031/api.ip_addr?format=json"
curl -s "localhost:13031/api.version?format=json"
curl -s -4 "localhost:13031/api.ip_addr?format=json"
curl -s "localhost:13031/api.version?format=none"
curl -s "localhost:13031/api.version?format=text&extra="
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
echo -n "pwd_hash_value" > .web_session.test
device=$(curl -s "localhost:13031/api.cws?token=test")
unique=$(curl -s "localhost:13031/api.cws?token=test&device=$device")
checked=$(echo -n "pwd_hash_value$device" | sha256sum | head -c 64)
session=$(echo -n "$checked$unique" | sha256sum | head -c 10)
curl -s "localhost:13031/api.cws?token=test&device=bad_device&session=$session"
curl -s "localhost:13031/api.cws?token=test&device=$device&session=$session"
curl -s "localhost:13031/api.cws?token=test&device=$device&session=bad_session"
curl -s "localhost:13031/api.cws?token=test&device=$device&session=$session"
curl -s "localhost:13031/api.cws?token=test&device=$device&session=$session&function=terminate"
curl -s "localhost:13031/api.cws?token=test&device=$device&session=$session"
rm -f .web_session.test

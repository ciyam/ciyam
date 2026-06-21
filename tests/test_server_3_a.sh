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
echo -n "f2ca1bb6c7e907d06dafe4687e579fce76b37e4e93b7605022da52e6ccc26fd2" > .web_session.test
device=$(curl -s "localhost:13031/api.cws?access=test")
unique=$(curl -s "localhost:13031/api.cws?access=test&device=$device")
checked=$(echo -n "f2ca1bb6c7e907d06dafe4687e579fce76b37e4e93b7605022da52e6ccc26fd2$device" | sha256sum | head -c 64)
session=$(echo -n "$checked$unique" | sha256sum | head -c 10)
curl -s "localhost:13031/api.cws?access=test&device=bad_device&session=$session"
curl -s "localhost:13031/api.cws?access=test&device=$device&session=$session"
curl -s "localhost:13031/api.cws?access=test&device=$device&session=bad_session"
curl -s "localhost:13031/api.cws?access=test&device=$device&session=$session"
curl -s "localhost:13031/api.cws?access=test&device=$device&request=unlock&session=$session"
curl -s "localhost:13031/api.cws?access=test&device=$device&request=access_token&session=$session"
curl -s "localhost:13031/api.cws?access=test&device=$device&request=quit&session=$session"
curl -s "localhost:13031/api.cws?access=test&device=$device&session=$session"
rm -f .web_session.test

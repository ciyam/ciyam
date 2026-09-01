Create a new device for use with message tests.
(created device - length is 15)
Output the entrance room details.
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "admin #0000001 0/0 Administration"
  }
]
Create an initial test message.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "admin  testing..."
  }
]
Output the entrance room details.
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "admin #0000001 0/1 Administration"
  }
]
Review messages in room 0000001.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "(no new messages)"
  }
]
Create a new user '11111' named 'test-1'.
{"token":"11111"}
Connect using 11111 with 'none' and output entrance room details.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[std]@test-1
ciyam.username = test-1
ciyam.is_admin = false
ciyam.is_locked = false
{"message":"Session terminated."}
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1 #0000001 1/1 Administration"
  }
]
Review messages in room 0000001 for 'test-1'.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1 :joined"
  }
]
Create an initial message for 'test-1' and output entrance room details.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1  test-1 here..."
  }
]
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1 #0000001 0/2 Administration"
  }
]
Review messages in room 0000001 for 'test-1'.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "(no new messages)"
  }
]
Output the entrance room details for 'admin'.
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "1"
  },
  {
    "name": "test-1",
    "count": "0"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "admin #0000001 2/3 Administration"
  }
]
Review messages in room 0000001 for 'admin'.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "1"
  },
  {
    "name": "test-1",
    "count": "0"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1 :joined"
  },
  {
    "when": "0000000000002",
    "payload": "test-1  test-1 here..."
  }
]
Create a new user '22222' named 'test-2'.
{"token":"22222"}
Connect using 22222 with 'none' and output entrance room details.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[std]@test-2
ciyam.username = test-2
ciyam.is_admin = false
ciyam.is_locked = false
{"message":"Session terminated."}
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-2 #0000001 1/1 Administration"
  }
]
Review messages in room 0000001 for 'test-2'.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-2 :joined"
  }
]
Create an initial message for 'test-2' and output entrance room details.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-2  test-2 here..."
  }
]
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-2 #0000001 0/2 Administration"
  }
]
Review messages in room 0000001 for 'test-2'.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "(no new messages)"
  }
]
Output entrance room details for 'admin'.
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "1"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "0"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "admin #0000001 2/5 Administration"
  }
]
Review messages in room 0000001 for 'admin'.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "1"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "0"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-2 :joined"
  },
  {
    "when": "0000000000002",
    "payload": "test-2  test-2 here..."
  }
]
Create a new romm for 'test-1' and 'test-2' and output entrance room details for 'test-1'.
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "1"
  },
  {
    "name": "test-2",
    "count": "0"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1 #0000001 3/5 Administration"
  },
  {
    "when": "0000000000002",
    "payload": "test-1 #0000002 1/1 Private (test-1 and test-2)"
  }
]
Review messages in room 0000001 again for 'test-2'.
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1 :invite 0000002-NEW-ROOM-UUID-VALUE Private (test-1 and test-2)"
  }
]
Join new romm for 'test-2', create an initial message for the new room and output entrace room details.
{"room":"0000002-NEW-ROOM-UUID-VALUE"}
[
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "(no new messages)"
  }
]
{"room":"0000002"}
[
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-2  hi..."
  }
]
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "0"
  },
  {
    "name": "test-2",
    "count": "1"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-2 #0000001 0/3 Administration"
  },
  {
    "when": "0000000000002",
    "payload": "test-2 #0000002 0/1 Private (test-1 and test-2)"
  }
]
Output entrance room details for 'test-1' and then review messages in rooms 0000001 and 0000002.
{"room":"0000000"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "1"
  },
  {
    "name": "test-2",
    "count": "0"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1 #0000001 3/5 Administration"
  },
  {
    "when": "0000000000002",
    "payload": "test-1 #0000002 2/2 Private (test-1 and test-2)"
  }
]
{"room":"0000001"}
[
  {
    "name": "admin",
    "count": "0"
  },
  {
    "name": "test-1",
    "count": "1"
  },
  {
    "name": "test-2",
    "count": "0"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-2 :joined"
  },
  {
    "when": "0000000000002",
    "payload": "test-2  test-2 here..."
  },
  {
    "when": "0000000000003",
    "payload": "test-1  (private message sent to test-2)"
  }
]
{"room":"0000002"}
[
  {
    "name": "test-1",
    "count": "1"
  },
  {
    "name": "test-2",
    "count": "0"
  }
]
[
  {
    "when": "0000000000001",
    "payload": "test-1 :create 0000002-NEW-ROOM-UUID-VALUE Private (test-1 and test-2)"
  },
  {
    "when": "0000000000002",
    "payload": "test-2  hi..."
  }
]

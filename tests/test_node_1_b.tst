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

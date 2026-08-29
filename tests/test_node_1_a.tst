Create system identity and 'admin' access with 'none'.
CIYAM [http://localhost:13031]
{"name":"*CIYAM*", "version":"0.0.0"}
[adm]@admin
{"message":"Session terminated."}

Update 'admin' access password to 'test' (after restore).
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[adm]@admin
{"message":"Session terminated."}

Attempt to connect using 10101 with 'test'.
Error: This web session is not valid (or has expired).

Attempt to connect using 10201 with 'test'.
Error: This web session is not valid (or has expired).

Attempt to connect using 10301 with 'none'.
Error: This web session is not valid (or has expired).

Connect using 10301 with 'test'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[adm]@admin
{"message":"Session terminated."}

Attempt to update 'admin' access password to 'none' while locked.
Error: System identity is not currently locked.

Update 'admin' access password to 'none' (after restore).
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[adm]@admin
{"message":"Session terminated."}

Attempt to connect using 10101 with 'none'.
Error: This web session is not valid (or has expired).

Attempt to connect using 10201 with 'none'.
Error: This web session is not valid (or has expired).

Attempt to connect using 10201 with 'test'.
Error: This web session is not valid (or has expired).

Connect using 10301 with 'none'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[adm]@admin
{"message":"Session terminated."}

Fetch the list of non-admin users.
[]

Create a user with access pin '11111' then list users.
{"token":"11111"}
[
  {
    "pin": "11111",
    "name": ""
  }
]

Attempt to create another user with access pin '11111' then list users.
{"error":"Access token '11111' has already been allocated."}
[
  {
    "pin": "11111",
    "name": ""
  }
]

Connect using 11111 with 'none' and list users.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[std]@test-1
{"message":"Session terminated."}
[
  {
    "pin": "11111",
    "name": "test-1"
  }
]

Attempt to connect using 11111 with 'test'.
Error: This web session is not valid (or has expired).

Update own password for 11111 from 'none' to 'test'.
{"response":"[okay]"}

Connect using 11111 with 'test'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[std]@test-1
{"message":"Session terminated."}

Update password for 11111 to 'none' as admin.
{"response":"[okay]"}

Connect using 11111 with 'none'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[std]@test-1
{"message":"Session terminated."}

Create a user with access pin '22222' then list users.
{"token":"22222"}
[
  {
    "pin": "11111",
    "name": "test-1"
  },
  {
    "pin": "22222",
    "name": ""
  }
]

Attempt to connect using 22222:test with 'test'.
Error: ?test-2

Connect using 22222:test with 'test' (after suggestion).
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[std]@test
{"message":"Session terminated."}

Attempt to update password for 11111 to 'test' as 22222.
{"error":"Users can only be maintained by the administrator."}

Create a user with access pin '33333' then list users.
{"token":"33333"}
[
  {
    "pin": "11111",
    "name": "test-1"
  },
  {
    "pin": "22222",
    "name": "test"
  },
  {
    "pin": "33333",
    "name": ""
  }
]

Attempt to connect using 33333:test with 'test'.
Error: ?test-3

Attempt to connect using 33333:test with 'test' (after suggestion).
Error: Username 'test' has already been taken.

Connect using 33333:testing with 'test' and list users
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[std]@testing
{"message":"Session terminated."}
[
  {
    "pin": "33333",
    "name": "testing"
  }
]

Attempt to connect using 33333 with 'none'.
Error: This web session is not valid (or has expired).

Delete user with access pin '11111' then list users.
{"response":"[okay]"}
[
  {
    "pin": "22222",
    "name": "test"
  },
  {
    "pin": "33333",
    "name": "testing"
  }
]

Attempt to delete user with access pin '11111' again then list users.
{"error":"Unkknown user access '11111' for removal."}
[
  {
    "pin": "22222",
    "name": "test"
  },
  {
    "pin": "33333",
    "name": "testing"
  }
]

Attempt to connect using 11111 with 'none'.
Error: This web session is not valid (or has expired).

Create 'xxxxx' token for pin '12345'.

Connect using xxxxx:test-x with 'test' then list users.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[std]@test-x
{"message":"Session terminated."}
[
  {
    "pin": "12345",
    "name": "test-x"
  },
  {
    "pin": "22222",
    "name": "test"
  },
  {
    "pin": "33333",
    "name": "testing"
  }
]

Delete user with access pin '22222' then list users.
{"response":"[okay]"}
[
  {
    "pin": "12345",
    "name": "test-x"
  },
  {
    "pin": "33333",
    "name": "testing"
  }
]

Delete user with access pin '33333' then list users.
{"response":"[okay]"}
[
  {
    "pin": "12345",
    "name": "test-x"
  }
]

Delete user with access pin '12345' then list users.
{"response":"[okay]"}
[]

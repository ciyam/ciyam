Create system identity and 'admin' access with 'none'.
CIYAM [http://localhost:13031]
{"name":"*CIYAM*", "version":"0.0.0"}
[admin]
{"message":"Session terminated."}

Update 'admin' access password to 'test' (after restore).
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[admin]
{"message":"Session terminated."}

Attempt to connect using 10101 with 'test'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
Error: No current session exists.

Attempt to connect using 10201 with 'test'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
Error: No current session exists.

Attempt to connect using 10301 with 'none'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
Error: This web session is not valid (or has expired).

Connect using 10301 with 'test'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[admin]
{"message":"Session terminated."}

Attempt to update 'admin' access password to 'none' while locked.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
Error: System identity is not currently locked.

Update 'admin' access password to 'none' (after restore).
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[admin]
{"message":"Session terminated."}

Attempt to connect using 10101 with 'none'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
Error: No current session exists.

Attempt to connect using 10201 with 'none'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
Error: No current session exists.

Attempt to connect using 10201 with 'test'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
Error: This web session is not valid (or has expired).

Connect using 10201 with 'none'.
CIYAM [http://localhost:13031]
{"name":"CIYAM", "version":"0.0.0"}
[admin]
{"message":"Session terminated."}



== codes =======================================================
ok   the server's own form
ok   in capitals, with spaces
ok   without its hyphens
ok   a letter short
ok   a digit is not a code
ok   nothing
ok   a PIN

== what the server says to a code or a PIN =====================
ok   a code, just used - a new PIN, no username
ok   a PIN with the username admin chose
ok   a PIN with a suggested username
ok   a name that is not a valid username is not offered
ok   a PIN that already has a password - a device token
ok   an unknown PIN
ok   anything else
ok   nothing

== people ======================================================
ok   admin first, then by name, then unclaimed by PIN
ok   a row
ok   admin is not in the review - added from the session
ok   nobody, and no session
ok   Windows line ends

== a PIN admin chooses =========================================
ok   both
ok   a suggestion
ok   a PIN alone
ok   a username alone
ok   neither - a random PIN
ok   fine
ok   both empty is fine
ok   a short PIN
ok   a PIN in use
ok   a bad username
ok   a username in use

== a new username and password =================================
ok   fine
ok   no username
ok   too short a password
ok   not the same twice

== the address in the QR code ==================================
ok   the code after #
ok   read back
ok   read back, typed oddly
ok   a bad code still opens Welcome, empty
ok   #welcome
ok   nothing

All checks passed.

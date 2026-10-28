
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

== a device token the node has never seen ======================
ok   the server's words
ok   anything else

== a code or PIN the node does not know ========================
ok   a code
ok   a PIN
ok   without the prefix
ok   anything else is passed on
ok   nothing

== this account's devices ======================================
ok   each device, this browser first
ok   a device signed out - its session twenty zeros
ok   - signed in: Sign out, or Block; signed out: Block; this browser neither
ok   - each asked first, as it is
ok   - and what each says once done
ok   with Windows line ends too
ok   nothing, or anything else, is none

== your keys ===================================================
ok   the card's state - from this browser and the node
ok   - keys in an older form, as none - set up again
ok   - an error, or not locked keys, unreadable
ok   the last step of joining - Later until the keys are set up, then on to the chat
ok   words to check back - three different, in order
ok   a public key as pasted - capitals, spaces, broken over lines
ok   adding someone's key - fine
ok   - not a username, no key, your own, one already kept, or theirs under another name
ok   the keys kept, by name, shortened
ok   My account's sections, in order
ok   - the one an address asks for, else the first
ok   the room keys kept - none, one, or several in order

All checks passed.

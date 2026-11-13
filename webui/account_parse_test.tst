
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
ok   keys - none, not backed up, backed up; none for one unclaimed
ok   - said under the name
ok   disabled - inactive, the mark taken off, no keys said, among the others by name
ok   - their PIN and username still taken

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
ok   a saved password refused - the password or the device, nothing else

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
ok   - guarded keys read as keys; a bad guard unreadable
ok   the last step of joining - what keys are for, then set up
ok   words to check back - three different, in order
ok   a public key as pasted - capitals, spaces, broken over lines
ok   a contact code - the name as hex, an "I", padded to 25, then the key
ok   - none for a name or key that cannot be one, or too little padding
ok   reading one - the console's, ours, and one copied with spaces, a break and capitals
ok   - not one: a bare key, the wrong length, no "I", a name that isn't a username, a short key
ok   adding someone - fine
ok   - not a code, your own key or name, one already kept, or theirs under another name
ok   in the chat's exchange with someone - theirs fine; another's, or not one, said
ok   the keys kept, by name, shortened
ok   My account's sections, in order
ok   - the one an address asks for, else the first
ok   the room keys kept - said under them, or none yet
ok   - listed by name, then number; one not in the listing last, by number (Damon, 2026-11-04)
ok   another device - the ask, this page with its public key after #
ok   - read back, whole, after # only, or pasted broken over lines in capitals
ok   - the reply, read back
ok   - not one: the old address with the access key, a key too short, a reply with no check, nothing
ok   keys from the administrator - the ask and the answer, each its own kind
ok   - an answer with an access key's length not taken as one, nor another device's with an entropy's
ok   a claim not signed in - said why, with the way on
ok   admin's keys at set up or recovery - made, opened, left, or not made
ok   the node's words said plainly under Your keys - admin's; nothing for everyone else, never offered them
ok   Give keys - everything it lacks, by field, at once
ok   - another device's code not taken for theirs; all there, nothing
ok   xor of two hex strings - and back
ok   - not the same length, or not hex

All checks passed.

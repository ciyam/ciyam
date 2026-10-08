
== /system - the node's state ==================================
ok   ready over plain HTTP
ok   locked
ok   new - no identity yet
ok   a server older than the cipher - no security said
ok   a trailing line break
ok   not a node
ok   nothing
ok   another name

== /system - the connection's security =========================
ok   Chrome 154 - ML-KEM
ok   OpenSSL 3.5's curl - ML-KEM
ok   an older curl - X25519 alone
ok   ML-KEM alone, any case
ok   a cipher with no group
ok   locked, over HTTPS
ok   the pill - none
ok   the pill - encrypted
ok   the pill - quantum
ok   the pill - not said
ok   the master password is warned against over plain HTTP
ok   not over TLS
ok   not over ML-KEM
ok   nor when the node did not say

== arriving ====================================================
ok   ready - the sign in
ok   locked - Unlock
ok   new - Set up
ok   no answer - unreachable
ok   a locked node's refusal said plainly
ok   any other refusal left as it was
ok   no error

== unlock keys =================================================
ok   as the node gives it
ok   with spaces, as the docs show it
ok   with no separators
ok   a hyphen inside a group
ok   case is kept
ok   too short
ok   a character base64 does not have
ok   the master password is not a key
ok   nothing
ok   keys made - a count
ok   keys made - none stored
ok   keys made - not a count
ok   keys made - below none

== admin's Overview ============================================
ok   people - admin not counted
ok   people - nobody
ok   uptime - as the node gives it
ok   uptime - weeks
ok   uptime - an error
ok   uptime in words - minutes
ok   uptime in words - the two largest, a nothing skipped
ok   uptime in words - one of each
ok   uptime in words - just started
ok   uptime in words - an error

== logs ========================================================
ok   the log names
ok   the log names - blank lines and repeats
ok   the log names - an error is not a name
ok   an error line
ok   a failure
ok   a warning
ok   an ordinary line
ok   a word holding 'error' is not one
ok   the log - the last lines
ok   the log - filtered, any case
ok   the log - fewer than asked
ok   the log - nothing matches
ok   the log - no count is the default

== apps and sections ===========================================
ok   a member's apps
ok   admin's - the console too
ok   anyone's on a development system
ok   no console on a phone
ok   the pages
ok   admin's sections
ok   a member has none
ok   a copy, not the list itself

All checks passed.

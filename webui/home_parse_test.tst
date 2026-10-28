
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
ok   any refusal, when the node is known to be locked
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
ok   keys - asked of the node
ok   keys - the node's count
ok   keys - no answer is not none
ok   keys - not a count

== admin's Overview ============================================
ok   people - admin not counted
ok   people - nobody
ok   uptime - as the node gives it
ok   uptime - weeks
ok   uptime - an error
ok   the node's line - its version and how long it has been up
ok   uptime in words - minutes, no seconds (Ian, 2026-10-11)
ok   uptime in words - an hour and seconds is the hour
ok   uptime in words - under a minute, just started
ok   uptime in words - the two largest, a nothing skipped
ok   uptime in words - one of each
ok   uptime in words - just started
ok   uptime in words - an error

== logs ========================================================
ok   the log names
ok   the log names - blank lines and repeats
ok   the log names - an error is not a name
ok   a log's lines
ok   a log with no last break
ok   an empty log
ok   a blank line inside is kept
ok   an empty log - the node's [none]
ok   the last 200 lines asked for
ok   every line asked for
ok   people - codes waiting
ok   people - one code
ok   people - all claimed
ok   people waiting - one
ok   people waiting - two
ok   people waiting - none
ok   secure - asked of the node
ok   secure - '1' is held encrypted
ok   secure - nothing is not - a development or demo node
ok   secure - '[okay]', how the node answers a variable with nothing in it, is not either
ok   secure - no answer, or anything else, is not known
ok   secure - not, said plainly
ok   keys - none left
ok   keys - one left
ok   keys - three left
ok   keys - the node not saying
ok   a key too soon after another, said plainly
ok   any other refusal as it came
ok   an error line
ok   a failure
ok   a warning
ok   an ordinary line
ok   a word holding 'error' is not one
ok   a log line's time and words
ok   a line of another shape - all words
ok   problems since the restart - not before it
ok   no start among the lines read
ok   no lines
ok   the problem lines - since the restart, errors and warnings, 'failed' among them
ok   and exactly what is counted
ok   no start read - every problem line read
ok   in words - both
ok   in words - none
ok   in words - one, no start read
ok   an unlock with a key since the restart - when, by the node's clock
ok   - one before the last restart
ok   - none read
ok   - in words
ok   rooms filling - at 80% or more, fullest first
ok   rooms filling - none
ok   rooms filling - in words
ok   a filling room
ok   a full room
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
ok   admin's accounts app is My account too - People is in Home (2026-10-11)
ok   a member's is My account
ok   the tabs the apps reuse
ok   admin's sections - People since 2026-10-11, Health and Alerts since 2026-10-12
ok   a member has none
ok   a copy, not the list itself
ok   admin opens on the Overview
ok   admin asking for the logs
ok   admin asking for something not theirs
ok   a member opens on Home
ok   a member asking for an admin section
ok   the roles

== a member's Home =============================================
ok   the chat's summary - a member
ok   the badge is the chat's own count
ok   admin counts Administration too
ok   nothing at all
ok   the Chat tile's line
ok   one of each, singular
ok   requests only, plural
ok   nothing new
ok   needs you - a request and an invitation
ok   needs you - nothing
ok   needs you - keys not set up, first
ok   - said as such
ok   a device shortened
ok   people matching - nothing typed, everyone
ok   - a name, case aside
ok   - a PIN, or part of one
ok   - the status as shown
ok   - every word found
ok   a page of people
ok   - held within the pages there are
ok   the count over the list
ok   a short one kept
ok   the devices
ok   devices shown - this browser, then signed in, four in all
ok   and how many more
ok   the signed out last, when there is room
ok   a few - all of them, none more
ok   none
ok   more - three
ok   more - one
ok   more - none

== health ======================================================
ok   what needs you - problems first, then the rest
ok   - the log's, counted since the restart, its latest line and time
ok   - a full room, and one filling
ok   - no unlock keys left - to make one
ok   nothing wrong - nothing
ok   warnings alone - a warning
ok   no keys on a node without a secure identity - not a problem
ok   what the node did not say raises nothing
ok   the fullest rooms - filling or not, rooms only, as many as asked
ok   the heading
ok   the node's kind
ok   a flag - '1' or nothing

== alerts ======================================================
ok   the ntfy server's example - this page's machine, ntfy's port
ok   - with no machine, a placeholder
ok   the test - its title quoted whole

== setting up a new node =======================================
ok   refused - the PIN held by another's setup, or gone
ok   Enter in the password - on to typing it again, once long enough
ok   Enter in typing it again - makes the words, once they match
ok   a node time, in UTC, as a unique
ok   the web sessions - a line each, the heading and rule passed over
ok   - each read whole
ok   - one with no username
ok   by person, the most recently seen first - yours marked
ok   - idle an hour or more counted - since 02:53 and 01:00
ok   - said in a line
ok   redacted - each line still read
ok   this session known by the end of it, the start redacted
ok   redacted - grouped by name, yours marked
ok   the Overview's figure - how many open, how many idle; or not reported
ok   the schedule - when next, the name, the rest as given
ok   what listens - named, the peer port's chains counted
ok   the still-alive token - twelve hex of the hash of the PIN and the suggestion
ok   the answer - all X's kept it waiting, an address did not, anything else says nothing
ok   the second refused - a PIN the node never gave (it restarted)
ok   the second refused - its PIN gone
ok   the first request's answer - admin's PIN and the node's suggestion
ok   a second first request - refused, not an answer
ok   nothing - not an answer
ok   kept for the tab - read back
ok   kept for the tab - and nothing else from it
ok   kept for the tab - anything else is nothing
ok   the person's randomness - hex, as long as asked
ok   - the same pops and browser numbers, the same
ok   - a pop a millisecond later, different
ok   - other browser numbers, different
ok   - longer than one hash when asked
ok   - a pop's colour counts too
ok   a bubble's colour - the hue from two bytes, how vivid and light from the third
ok   - any three bytes, a colour
ok   checking a word - three choices, the right one among them
ok   - the others are not the person's other words
ok   - no word twice

All checks passed.

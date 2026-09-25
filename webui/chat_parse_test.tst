
== member list and presence ====================================
ok   three members parsed
ok   offline member
ok   two sessions
ok   hyphenated name kept
ok   extra=TIME suffix tolerated
ok   empty line yields none
ok   online first then alphabetical

== ordinary chat messages ======================================
ok   kind
ok   unique
ok   sender
ok   leading marker space removed
ok   not edited
ok   inner spacing preserved

== edited messages =============================================
ok   edited flag
ok   marker stripped from sender
ok   text

== system events ===============================================
ok   kind
ok   verb
ok   sender
ok   invite verb
ok   invite room
ok   invite token
ok   invite room name
ok   create verb
ok   create room
ok   create name
ok   rename verb
ok   rename from
ok   rename to
ok   issued verb
ok   issued kind
ok   issued has no room
ok   issued recipients
ok   issued invite kind
ok   issued invite room
ok   issued invite recipients
ok   several recipients

== entrance room listing =======================================
ok   kind
ok   owner
ok   room number
ok   unread
ok   total
ok   name
ok   name with spaces

== posting restrictions ========================================
ok   unrestricted
ok   owner only marker
ok   locked marker
ok   suffix stripped from total (own)
ok   suffix stripped from total (none)
ok   name still parsed past a suffix
ok   locked convenience flag
ok   not locked when owner only

== who may post ================================================
ok   entrance room never accepts posts
ok   starting room refused for a standard user
ok   starting room allowed for admin
ok   open room allows anyone
ok   owner-only refuses a member
ok   owner-only allows the owner
ok   owner-only allows admin
ok   locked refuses the owner too
ok   locked is flagged as locked
ok   a reason is always given when refused
ok   starting room recognised
ok   other room is not the starting room

== no new messages =============================================
ok   kind
ok   unique retained

== whole responses =============================================
ok   members
ok   messages
ok   no rooms
ok   has new
ok   entrance rooms
ok   entrance has no messages
ok   quiet poll has members
ok   quiet poll has no messages
ok   quiet poll has_new false
ok   error detected
ok   error yields no members

== polling start point =========================================
ok   one past the highest unique
ok   padded to 13 digits
ok   empty when nothing seen

== room ordering ===============================================
ok   unread first, then by number

== room name validation ========================================
ok   accepts a plain name
ok   accepts parentheses
ok   accepts an apostrophe
ok   rejects underscores
ok   rejects too short
ok   rejects a leading digit
ok   rejects a trailing space
ok   rejects a double space
ok   rejects doubled ampersands
ok   rejects empty parentheses
ok   rejects reversed parentheses
ok   rejects two open parentheses

== username validation =========================================
ok   accepts a simple name
ok   accepts an inner hyphen
ok   rejects capitals
ok   rejects too short
ok   rejects too long
ok   rejects a trailing hyphen
ok   rejects a doubled hyphen
ok   rejects a leading digit

== sender colours ==============================================
ok   stable for a given name
ok   within range
alice: 2
jun-w: 6
mira: 3
admin: 4

== helpers =====================================================
ok   entrance room recognised
ok   other room not entrance
ok   time shape
ok   time of a bad unique

== system events ===============================================
ok   allows verb
ok   allows keeps its detail
ok   assign verb
ok   assign names the old owner
ok   assign names the new owner
ok   a bare verb has no detail
ok   a bare verb still parses

== invitations already sent ====================================
ok   single recipient found
ok   every name in a list found
ok   another room ignored
ok   a private message receipt ignored
ok   nothing for an unknown room
ok   no messages is no invitations

== pending invitations =========================================
ok   one per room, joined rooms left out
ok   newest first, latest details win
ok   inviter is the sender
ok   a name with brackets kept
ok   joining clears it
ok   no messages, none pending
ok   never the starting room, never without a token

== rooms shown in the rail =====================================
ok   admin sees Administration
ok   others do not
ok   the list itself is untouched

== session handover fields =====================================
ok   hyphen is encoded
ok   colon is encoded
ok   equals is encoded
ok   comma is encoded
ok   plain name unchanged
ok   round trip
ok   empty encodes empty
ok   malformed decodes empty

== saved account list ==========================================
ok   no key yet
ok   empty string is no accounts
ok   a single account
ok   leading blank dropped
ok   trailing blank dropped
ok   interior blank dropped
ok   surrounding space trimmed
ok   duplicates collapsed
ok   emptied list removes the key
ok   list of blanks removes the key
ok   one account formats
ok   accounts are sorted
ok   round trip repairs corruption

== day labels ==================================================
ok   same day is Today
ok   one day back is Yesterday
ok   two days back is a weekday
ok   six days back is still a weekday
ok   seven days back gives a date
ok   seven days back drops the bare weekday
ok   a bad unique has no label
ok   day key shape
ok   day key of a bad unique
ok   same day shares a key
ok   different days differ
ok   days between counts calendar days
ok   full stamp mentions the year
ok   full stamp of a bad unique

All checks passed.

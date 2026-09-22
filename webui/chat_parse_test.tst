
== member list and presence ====================================
ok   three members parsed
ok   offline member
ok   two sessions
ok   hyphenated name kept
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

All checks passed.

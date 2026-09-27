
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
ok   ignore names its room
ok   reject names its room
ok   an answered invitation is not pending
ok   one per room, joined rooms left out
ok   newest first, latest details win
ok   inviter is the sender
ok   a name with brackets kept
ok   joining clears it
ok   no messages, none pending
ok   never the starting room, never without a token

== line breaks and backslashes =================================
ok   five messages, none lost to a break
ok   a line break comes back as one
ok   a stored backslash comes back single
ok   backslash then break
ok   an ending backslash does not swallow the next message
ok   sending doubles a backslash
ok   sending leaves breaks alone
ok   a round trip keeps what was typed

== message size ================================================
ok   letters are a byte each
ok   an accented letter is two
ok   an emoji is four
ok   a line break is one
ok   the limit sits under the smallest measured

== room events in plain words ==================================
ok   joined
ok   left
ok   created
ok   invited you
ok   renamed
ok   handed over
ok   posting set
ok   posting locked
ok   invitation sent
ok   private message sent
ok   answered
ok   declined
ok   an unknown verb keeps its words, without the colon

== rooms shown in the rail =====================================
ok   admin sees Administration
ok   others do not
ok   the list itself is untouched

== emoji =======================================================
ok   eight categories, none empty
ok   each entry has a name
ok   no emoji is listed twice
ok   none is longer than 16 bytes
ok   at least 500 in all
ok   code points become the character
ok   with a variation selector
ok   a keycap
ok   search by the start of a word
ok   every word must match
ok   extra words count
ok   a name match comes before a keyword match
ok   hyphenated names split
ok   nothing typed, nothing found
ok   no match
ok   autocomplete: a colon and two letters
ok   autocomplete: at the very start
ok   autocomplete: only up to the cursor
ok   autocomplete: one letter is not enough
ok   autocomplete: not a time
ok   autocomplete: not a link
ok   autocomplete: not a smiley
ok   autocomplete: not once a space is typed
ok   autocomplete: not glued to a word
ok   autocomplete: after a line break
ok   shortcode from a name
ok   shortcode without the punctuation
ok   shortcode keeps + and -
ok   suggestions: _ for a space
ok   suggestions: at most eight
ok   suggestions: as many as asked
ok   recent: newest first
ok   recent: once each
ok   recent: capped
ok   recent: read back
ok   recent: damaged is empty
ok   recent: stray entries dropped

== private messages ============================================
ok   a public message is not private
ok   a private one is, without its marker
ok   the sender's copy and its receipt become one
ok   the copy carries who it went to, less the sender
ok   a receipt with no copy stays a notice
ok   an edited message is not private for its !
ok   and the ! is not part of its text
ok   an edited one in the old form is unchanged
ok   a message starting ! in the old format is not private
ok   and keeps its !
ok   label on a received one
ok   label on a sent copy
ok   the sender is added to for
ok   only once
ok   not when unknown

== announcements ===============================================
ok   admin's messages, newest first
ok   a multi-line one keeps its lines
ok   an edited one is still admin's
ok   dismissed ones are left out
ok   nothing read, nothing shown
ok   dismissed list read back
ok   a damaged value is nothing dismissed
ok   nothing stored is nothing dismissed
ok   stray entries are dropped
ok   dismissing adds the id once
ok   and keeps the newest
ok   the list stays capped

== unread while the rail is out of sight =======================
ok   the open room is not counted
ok   each invitation counts one
ok   Administration counts for admin only
ok   nothing open counts every room
ok   no rooms, no count
ok   badge for none
ok   badge for some
ok   badge capped

== password strength ===========================================
ok   empty says nothing
ok   under seven is unsatisfactory
ok   seven digits is weak
ok   twelve lower case is moderate
ok   digits and lower, eight long, is moderate
ok   three kinds with digits, ten long, is strong
ok   all four kinds, twelve long, is very strong
ok   boundary: 7 x 7 = 49 is strong

== user initial ================================================
ok   first letter, upper case
ok   admin
ok   a PIN
ok   nothing

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

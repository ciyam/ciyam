
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
ok   the rooms this user declined
ok   someone else's decline is not this user's
ok   a declined invitation is not pending, with no :ignore
ok   another's decline leaves this user's invitation
ok   without 'me', declines are not counted

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
ok   the limit is 2200 bytes
ok   under what the server takes from the longest name

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

== time-outs ===================================================
ok   a time-out is recognised
ok   another error is not one
ok   an answer is not one
ok   nothing is not one
ok   three in a row ends the session
ok   sent after all - newer than the start point
ok   the same words from before the send do not count
ok   someone else's words do not count
ok   not there - not sent
ok   an edit arrived if its message now reads so
ok   an edit that did not take
ok   no start point yet - anything of mine counts

== read markers ================================================
ok   the read point is kept
ok   the session count is still right
ok   a line without it has none
ok   each marker under the last message before the read point
ok   the user's own is left out
ok   a read point equal to a message's unique has read that message
ok   a read point before every message shows nothing
ok   names under one message are sorted

== reading only what is new ====================================
ok   new ones are added after
ok   one already held is not taken twice
ok   same unique, different entry - both kept
ok   nothing held yet
ok   nothing new
ok   an announcement takes the cheap read
ok   nothing came back - read it whole
ok   an invitation - read it whole
ok   an answered one - read it whole
ok   a decline - read it whole
ok   a join - read it whole

== changing who may post =======================================
ok   the server's value is in capitals
ok   anything else is nothing
ok   the owner may change it
ok   admin may
ok   another member may not
ok   nobody for Administration
ok   nobody for the entrance
ok   nothing known, nothing changed

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
ok   an edited public message is public
ok   the message box, to everyone
ok   the message box, to named people
ok   the message box, editing a public message
ok   the message box, editing a private one
ok   an edit decides, whoever is picked
ok   a public message is edited as for=<unique>
ok   a private one as for=!<unique>
ok   an edited private message stays private
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
ok   a stack of five shows the newest two
ok   expanded, all of them
ok   two or fewer are never held back
ok   three is collapsed
ok   preview audience: everyone
ok   preview audience: named people
ok   preview audience: admin's own copy left out
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

== direct messages =============================================
ok   named from the sorted people
ok   the same whoever starts it
ok   a group
ok   names cleaned and not repeated
ok   a valid room name
ok   three short names still fit
ok   too many names to fit: no conversation name
ok   read back: the people
ok   read back: the first form still
ok   a hashed name is not a conversation
ok   one conversation whichever form it is named in
ok   an ordinary room has no key
ok   not a direct message: an ordinary room
ok   not a direct message: one person
ok   not a direct message: a room called Private with other words
ok   not a direct message: not usernames
ok   shown as the other person
ok   shown as the others
ok   an ordinary room keeps its name
ok   finds the conversation - the lowest numbered of two
ok   not one owned by someone it does not name
ok   none yet
ok   found by its people, in the new form too
ok   too big a group finds nothing - not an ordinary room either
ok   a conversation open
ok   one started here, not joined yet
ok   their request
ok   not a request from someone it does not name
ok   nothing yet
ok   a group is its own conversation
ok   waiting for whoever has not joined
ok   nobody left to wait for
ok   trusted when the owner is named
ok   not when someone else owns it
ok   an owner not known yet is trusted until it is
ok   an ordinary room is never a direct message
ok   a member the name leaves out
ok   nobody left out
ok   names listed
ok   a request from one person
ok   a request for a group

== direct messages on the server ===============================
ok   the server's name read
ok   one person twice is not a conversation
ok   nor three, nor a name that is not a username
ok   shown as the other person
ok   trusted as it is - the server built it
ok   the same conversation as the prototype's, by its people
ok   found in the rail
ok   their request found
ok   a request to message you
ok   two people start one as .<the other>
ok   whoever starts it
ok   a group keeps the prototype's room
ok   too big a group, nothing
ok   never signed in
ok   already have one
ok   a group's name taken by another room
ok   anything else as it came

== sign in errors ==============================================
ok   a missing connect status, in plain words
ok   any other error as it is
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

== a tab for each page =========================================
ok   no tab of that name - the browser made an empty one: load it
ok   a tab on this session: switch to it, no reload
ok   a tab signed in as someone else: leave it, open another
ok   a tab signed out: leave it, open another
ok   neither signed in: still another - nothing to share

== an address once a link has ended ============================
ok   source goes, the section stays
ok   other parameters stay
ok   an address with no source is left as it is

== a linked tab opens on the same room =========================
ok   the room goes in the address
ok   a room already asked for is replaced
ok   no room open, no room asked for
ok   the room asked for
ok   only a room number

== remember on this browser ====================================
ok   an account not saved shows Nothing
ok   saved, no password: The PIN only
ok   saved with a password: both
ok   no account yet
ok   Nothing forgets the account and its password
ok   Nothing for the last account removes the key
ok   The PIN only: saved, any password dropped
ok   The PIN and the password
ok   both, but no hash held - the PIN only

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

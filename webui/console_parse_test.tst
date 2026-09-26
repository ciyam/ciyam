
== session handover messages ===================================
ok   announce
ok   owner
ok   credentials
ok   session ended
ok   not ours
ok   JSON is not ours
ok   hyphen after the separator stays credentials
ok   credential fields
ok   harness form without username
ok   too few fields
ok   junk in a fixed field
ok   undecodable username

== resolving commands ==========================================
ok   noun then verb
ok   verb then noun
ok   joined form
ok   synonym verb
ok   singular noun
ok   options keep their spaces
ok   invite
ok   users create secret
ok   users create nominated
ok   unlock keys
ok   status
ok   missing name
ok   view lists without a name
ok   view scripts without a name
ok   view styles without a name
ok   review storages without a name
ok   a list by name
ok   options where none are taken
ok   raw
ok   raw with space
ok   bare tilde
ok   local var
ok   local help
ok   local is case blind
ok   quit
ok   exit means quit
ok   blank
ok   comment
ok   unknown is not sent raw

== building request URLs =======================================
ok   fetch
ok   raw
ok   own name
ok   no device yet

== variable names ==============================================
ok   lower case
ok   mixed
ok   upper case is reserved
ok   upper with digit is allowed
ok   leading digit
ok   punctuation
ok   empty

== scripts =====================================================
ok   steps skip blanks and comments
ok   five scripts
ok   arguments
ok   no arguments
ok   sorted
ok   bad means none
ok   error means none
ok   command
ok   command without arguments
ok   comma refused
ok   a space quotes the whole list
ok   the whole list, not just the value
ok   quote refused
ok   backslash refused
ok   destroy is destructive
ok   backup import is destructive
ok   backup export is not

== output ======================================================
ok   error
ok   bad
ok   okay
ok   head kept
ok   rest held back
ok   short output untouched

== history =====================================================
ok   repeats and blanks dropped
ok   oldest dropped at the limit

== list language ===============================================
ok   ? runs when the value was there
ok   ? skips when it was not
ok   ! runs when it was not
ok   ! skips when it was
ok   ? then ! on one line
ok   a line without a guard is untouched
ok   a lone ? with no space is left alone
ok   raw protocol is not a guard
ok   var name shows it
ok   var name text sets it
ok   var !name sets only if unset
ok   var @name null removes
ok   var @name global reads a script result
ok   var #name substr with a length
ok   var #name substr without one
ok   an unknown function is refused
ok   an all upper case name is reserved
ok   no arguments is a usage error
ok   substr with a length
ok   substr to the end
ok   output starts empty
ok   output adds a line
ok   load script is a javascript line
ok   eval script is one
ok   exec script is one
ok   plain exec is not
ok   view scripts is not
ok   exec resolves to the console
ok   list names, one per line
ok   no lists
ok   an error is no lists

== palette =====================================================
ok   empty query keeps all
ok   every word must match
ok   command matches rank first
ok   description only
ok   a word starting with it ranks first
ok   placeholder
ok   no placeholder

== storage =====================================================
ok   hashed password shortened
ok   other values whole

== preferences =================================================
ok   nothing stored
ok   stored value
ok   not JSON
ok   wrong type
ok   unknown names dropped
ok   not an object

== request log entries =========================================
ok   entry
ok   credentials never kept
ok   request kept
ok   error marked
ok   network failure
ok   posted body kept
ok   long response cut

== log capture =================================================
ok   caller still gets the response
ok   request logged
ok   quiet request not logged
ok   post logged with body
ok   a request that never answers is still logged

All checks passed.

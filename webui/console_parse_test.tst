
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

== the page a linked console belongs to ========================
ok   the accounts page
ok   the chat
ok   anything else is the chat

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
ok   view logs
ok   view log server
ok   review logs script
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

== saved credentials ===========================================
ok   remove creds
ok   remove creds for a PIN
ok   remove creds partial
ok   a PIN and partial, either order
ok   the harness's word orders
ok   retain creds partial
ok   not a PIN: refused
ok   retain takes no PIN
ok   other lines are not creds
ok   resolved as a local command
ok   a bad argument says why
ok   completely: the PIN out of the list, and every key of that account
ok   the last account: the list key goes
ok   partially: only the password hash
ok   partially, with no saved password
ok   nothing saved for that PIN
ok   not in the list, but its keys are still removed
ok   no PIN and not signed in
ok   keys of a longer PIN are not touched
ok   retain: the PIN and its hash
ok   retain partial: the PIN, and any old hash dropped
ok   retain with no hash held: the PIN only, and says so
ok   retain, not signed in

== building request URLs =======================================
ok   fetch
ok   raw
ok   own name
ok   a payload, encoded
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
ok   execute script is one
ok   exec resolves to the console

== server javascript lines =====================================
ok   load with an input
ok   load with none
ok   eval keeps every word
ok   exec is eval
ok   employ and execute too
ok   reload is load
ok   result, named
ok   result without a name is this account's
ok   unload
ok   this account's own
ok   load needs a name
ok   no path in a name
ok   no quotes in a name
ok   not a script line
ok   a global's name
ok   not a global's name
ok   anyone runs a script not named after a PIN
ok   and their own, by name or as ***
ok   another account's is admin's alone
ok   list names, one per line
ok   no lists
ok   this account's own is kept
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
ok   a list's lines shown as they run - off unless asked
ok   not an object
ok   log polling

== request log entries =========================================
ok   entry
ok   credentials never kept
ok   request kept
ok   error marked
ok   network failure
ok   posted body kept
ok   long response cut
ok   the full query, with this tab's credentials, in the harness's order
ok   a sign in's password hash is never put back
ok   only the credentials the request had
ok   an entry from before nothing
ok   a devices list keeps its devices, not their sessions
ok   only a devices list is masked
ok   devices: the list
ok   devices: one removed
ok   devices: removing needs which

== ntfy - subscribing a phone ==================================
ok   ntfy is a local command
ok   a server address, tidied
ok   not a server address
ok   a topic, by ntfy's rule
ok   the apps' subscribe link - plain HTTP says so
ok   and HTTPS does not
ok   no link without a server or a topic
ok   ntfy server <url>
ok   ntfy server, to show it
ok   ntfy qr <topic>, and the node's own
ok   ntfy qr ... web - the web page instead, the node's own or a named one
ok   the web page for a topic
ok   ntfy refusals

== log capture =================================================
ok   caller still gets the response
ok   request logged
ok   quiet request not logged
ok   post logged with body
ok   a request that never answers is still logged

All checks passed.

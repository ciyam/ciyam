// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Exercises "console_parse.js" against fixed inputs and prints a deterministic
// transcript. No server and no browser are needed. The "run_script *" fixture is real
// output from the development container.
//
// Run with:  node console_parse_test.js
// Compare against "console_parse_test.tst" - see "run_chat_tests.sh".

// NOTE: In the browser the console loads "chat_parse.js" first; its saved account list
// helpers are globals there, and made so here.
const chat_parse = require( "./chat_parse.js" );

global.parse_access_list = chat_parse.parse_access_list;
global.format_access_list = chat_parse.format_access_list;

const cp = require( "./console_parse.js" );

var failures = 0;

function check( label, actual, expected )
{
   var ok = ( JSON.stringify( actual ) === JSON.stringify( expected ) );

   if( !ok )
      ++failures;

   console.log( ( ok ? "ok   " : "FAIL " ) + label );

   if( !ok )
   {
      console.log( "       expected: " + JSON.stringify( expected ) );
      console.log( "       actual:   " + JSON.stringify( actual ) );
   }
}

function heading( text )
{
   console.log( "" );
   console.log( "== " + text + " " + "=".repeat( Math.max( 0, 60 - text.length ) ) );
}

function pick( spec, keys )
{
   var out = { };

   keys.forEach( function( k ) { out[ k ] = spec[ k ]; } );

   return out;
}

// --------------------------------------------------------------------
heading( "session handover messages" );

check( "announce", cp.parse_channel_message( "1790000000001:1790000000002" ),
 { kind: "announce", id: "1790000000001", rest: "1790000000002" } );

check( "owner", cp.parse_channel_message( "1790000000002-1790000000001" ),
 { kind: "owner", id: "1790000000002", rest: "1790000000001" } );

check( "credentials", cp.parse_channel_message( "17900=45679,c5588b,8dd2f1,ddd52e,6e29de,admin,1" ).kind, "credentials" );

check( "session ended", cp.parse_channel_message( "1790000000001" ), { kind: "ended", id: "1790000000001", rest: "" } );

check( "not ours", cp.parse_channel_message( "hello" ), null );
check( "JSON is not ours", cp.parse_channel_message( "{\"kind\":\"entry\"}" ), null );

// NOTE: The ISS-016 shape - a hyphen inside a field must not change the kind.
check( "hyphen after the separator stays credentials",
 cp.parse_channel_message( "17900=45679,c5588b,8dd2f1,ddd52e,6e29de,test-1,0" ).kind, "credentials" );

var creds = cp.parse_credentials( "45679,c5588b,8dd2f1,ddd52e,6e29de,test%2D1,1" );

check( "credential fields", creds,
 { access: "45679", device: "c5588b", hashed: "8dd2f1", sessid: "ddd52e", unique: "6e29de", username: "test-1", is_admin: true } );

check( "harness form without username", cp.parse_credentials( "45679,c5588b,8dd2f1,ddd52e,6e29de" ).username, "" );
check( "too few fields", cp.parse_credentials( "45679,c5588b" ), null );
check( "junk in a fixed field", cp.parse_credentials( "45679,c5<b>,8dd2f1,ddd52e,6e29de" ), null );
check( "undecodable username", cp.parse_credentials( "1,2,3,4,5,%E0%A4%A,0" ).username, "" );

// --------------------------------------------------------------------
heading( "the page a linked console belongs to" );

check( "the accounts page", cp.linked_owner( "accounts" ),
 { name: "the accounts page", subject: "The accounts page", source: "account", label: "Accounts" } );
check( "the chat", cp.linked_owner( "" ), { name: "the chat", subject: "The chat", source: "chat", label: "Chat" } );
check( "anything else is the chat", cp.linked_owner( "other" ).source, "chat" );

// --------------------------------------------------------------------
heading( "resolving commands" );

var keys = [ "kind", "method", "path", "options" ];

check( "noun then verb", pick( cp.resolve_command( "messages review 0000001" ), keys ),
 { kind: "cws", method: "GET", path: "/messages/0000001", options: "" } );

check( "verb then noun", pick( cp.resolve_command( "review messages 0000001 from=0" ), keys ),
 { kind: "cws", method: "GET", path: "/messages/0000001", options: "from=0" } );

check( "joined form", pick( cp.resolve_command( "messages|review 0000001 from=0" ), keys ),
 { kind: "cws", method: "GET", path: "/messages/0000001", options: "from=0" } );

check( "synonym verb", cp.resolve_command( "list users" ).key, "users|review" );
check( "singular noun", cp.resolve_command( "user delete 20101" ).path, "/users/20101" );

check( "options keep their spaces", cp.resolve_command( "messages create 0000000 text=Design  Review" ).options,
 "text=Design  Review" );

check( "invite", pick( cp.resolve_command( "messages update 0000004 for=tester-1,tester-2" ), keys ),
 { kind: "cws", method: "PUT", path: "/messages/0000004", options: "for=tester-1,tester-2" } );

check( "users create secret", pick( cp.resolve_command( "users create secret" ), keys ),
 { kind: "cws", method: "POST", path: "/users", options: "secret" } );

check( "users create nominated", cp.resolve_command( "users create nominated=20401:verify-a" ).options,
 "nominated=20401:verify-a" );

check( "unlock keys", cp.resolve_command( "unlock-keys create" ).method, "POST" );
check( "status", cp.resolve_command( "status" ).path, "/status" );

check( "missing name", cp.resolve_command( "messages review" ).reason, "'messages review' needs a name" );

// NOTE: Ian, 2026-09-26 - these list what is available when given no name.
check( "view lists without a name", pick( cp.resolve_command( "view lists" ), keys ), { kind: "cws", method: "GET", path: "/webcmdlists", options: "" } );
check( "view scripts without a name", cp.resolve_command( "view scripts" ).path, "/javascripts" );
check( "view styles without a name", cp.resolve_command( "view styles" ).path, "/stylesheets" );
check( "view logs", pick( cp.resolve_command( "view logs" ), keys ), { kind: "cws", method: "GET", path: "/logs", options: "" } );
check( "view log server", cp.resolve_command( "view log server" ).path, "/logs/server" );
check( "review logs script", cp.resolve_command( "review logs script" ).path, "/logs/script" );
check( "review storages without a name", cp.resolve_command( "review storages" ).path, "/storages" );
check( "a list by name", cp.resolve_command( "view list demo_echo_variables" ).path, "/webcmdlists/demo_echo_variables" );
check( "options where none are taken", cp.resolve_command( "users review extra" ).reason, "'users review' takes no options" );

check( "raw", cp.resolve_command( "~run_script *" ), { kind: "raw", request: "run_script *" } );
check( "raw with space", cp.resolve_command( "~ variable @irc_allow" ), { kind: "raw", request: "variable @irc_allow" } );
check( "bare tilde", cp.resolve_command( "~" ).kind, "unknown" );

check( "local var", cp.resolve_command( "var room 0000004" ), { kind: "local", name: "var", args: "room 0000004" } );
check( "local help", cp.resolve_command( "help" ), { kind: "local", name: "help", args: "" } );
check( "local is case blind", cp.resolve_command( "CLEAR" ).name, "clear" );

check( "quit", cp.resolve_command( "quit" ), { kind: "quit" } );
check( "exit means quit", cp.resolve_command( "exit" ), { kind: "quit" } );

check( "blank", cp.resolve_command( "   " ), { kind: "none" } );
check( "comment", cp.resolve_command( "# a note" ), { kind: "none" } );

// NOTE: Unlike the harness, an unrecognised command is not sent as raw protocol - "~" is
// required, so a typo cannot become a server command.
check( "unknown is not sent raw", cp.resolve_command( "variable @irc_allow" ), { kind: "unknown", word: "variable" } );

// --------------------------------------------------------------------
heading( "saved credentials" );

check( "remove creds", cp.parse_creds_command( "remove creds" ), { verb: "remove", pin: "", partial: false, error: "" } );
check( "remove creds for a PIN", cp.parse_creds_command( "remove creds 20401" ).pin, "20401" );
check( "remove creds partial", cp.parse_creds_command( "remove creds partial" ).partial, true );
check( "a PIN and partial, either order", [ cp.parse_creds_command( "remove creds partial 20401" ).pin, cp.parse_creds_command( "remove creds 20401 partial" ).partial ], [ "20401", true ] );
check( "the harness's word orders", [ "creds remove", "creds|remove", "delete creds", "Remove Credentials" ].map( function( t ) { return cp.parse_creds_command( t ).verb; } ), [ "remove", "remove", "remove", "remove" ] );
check( "retain creds partial", cp.parse_creds_command( "retain creds partial" ), { verb: "retain", pin: "", partial: true, error: "" } );
check( "not a PIN: refused", cp.parse_creds_command( "remove creds 204" ).error !== "", true );
check( "retain takes no PIN", cp.parse_creds_command( "retain creds 20401" ).error, "'retain creds' takes only 'partial'" );
check( "other lines are not creds", [ "remove", "creds", "messages delete 0000004", "delete messages 0000004", "constructor creds" ].map( function( t ) { return cp.parse_creds_command( t ); } ), [ null, null, null, null, null ] );
check( "resolved as a local command", cp.resolve_command( "remove creds 20401" ).kind + " " + cp.resolve_command( "remove creds 20401" ).name, "local creds" );
check( "a bad argument says why", cp.resolve_command( "remove creds bob" ).reason, "'remove creds' takes a 5-digit PIN and 'partial', both optional" );

var keys = [ "cws.access", "cws.device", "cws.hashed_20401", "cws.dismissed_20401", "cws.emoji_recent_20401", "cws.hashed_20405", "cws.prefs", "cws.script_demo" ];

check( "completely: the PIN out of the list, and every key of that account",
 cp.plan_creds_removal( keys, "20362,20401,20405", "20401", false ),
 { list: "20362,20405", remove: [ "cws.hashed_20401", "cws.dismissed_20401", "cws.emoji_recent_20401" ], set: { }, message: "(removed credentials completely for 20401)" } );
check( "the last account: the list key goes", cp.plan_creds_removal( [ "cws.access" ], "20401", "20401", false ).list, null );
check( "partially: only the password hash", cp.plan_creds_removal( keys, "20362,20401", "20401", true ),
 { list: undefined, remove: [ "cws.hashed_20401" ], set: { }, message: "(removed credentials partially for 20401)" } );
check( "partially, with no saved password", cp.plan_creds_removal( keys, "20362", "20362", true ).error, "Error: No saved password for '20362'." );
check( "nothing saved for that PIN", cp.plan_creds_removal( keys, "20362", "20303", false ).error, "Error: Unable to find credentials for '20303'." );
check( "not in the list, but its keys are still removed", cp.plan_creds_removal( keys, "20362", "20405", false ).remove, [ "cws.hashed_20405" ] );
check( "no PIN and not signed in", cp.plan_creds_removal( keys, "20362", "", false ).error, "Error: Not signed in - name the account: remove creds <pin>" );
check( "keys of a longer PIN are not touched", cp.plan_creds_removal( [ "cws.hashed_204011" ], "20401", "20401", false ).remove, [ ] );

check( "retain: the PIN and its hash", cp.plan_creds_retain( "20401", "20362", "abc", false ),
 { list: "20362,20401", remove: [ ], set: { "cws.hashed_20362": "abc" }, message: "(retained credentials completely for 20362)" } );
check( "retain partial: the PIN, and any old hash dropped", cp.plan_creds_retain( null, "20362", "abc", true ),
 { list: "20362", remove: [ "cws.hashed_20362" ], set: { }, message: "(retained credentials partially for 20362)" } );
check( "retain with no hash held: the PIN only, and says so", cp.plan_creds_retain( "20362", "20362", "", false ).message, "(retained credentials partially for 20362 - no password hash is held here)" );
check( "retain, not signed in", cp.plan_creds_retain( "20362", "", "", false ).error, "Error: Not signed in." );

// --------------------------------------------------------------------
heading( "building request URLs" );

var session = { access: "45679", device: "c5588b", sessid: "ddd52e" };

check( "fetch", cp.build_cws_url( "http://h/cws", cp.resolve_command( "messages review 0000001 from=0" ), session ),
 "http://h/cws/messages/0000001?access=45679&device=c5588b&format=text&options=from%3D0&session=ddd52e" );

check( "raw", cp.build_cws_url( "http://h/cws", { path: "", request: "run_script *" }, session ),
 "http://h/cws?access=45679&device=c5588b&format=text&request=run_script%20*&session=ddd52e" );

check( "own name", cp.build_cws_url( "http://h/cws", cp.resolve_command( "users delete ***" ), session ),
 "http://h/cws/users/45679?access=45679&device=c5588b&format=text&session=ddd52e" );

check( "a payload, encoded", cp.build_cws_url( "http://h/cws", { path: "/javascripts", payload: "var a = 1; // b&c" }, session ),
 "http://h/cws/javascripts?access=45679&device=c5588b&format=text&payload=var%20a%20%3D%201%3B%20%2F%2F%20b%26c&session=ddd52e" );

check( "no device yet", cp.build_cws_url( "http://h/cws", { path: "/status" }, { access: "1", device: "", sessid: "s" } ),
 "http://h/cws/status?access=1&format=text&session=s" );

// --------------------------------------------------------------------
heading( "variable names" );

check( "lower case", cp.is_valid_variable_name( "room" ), true );
check( "mixed", cp.is_valid_variable_name( "Room_2" ), true );
check( "upper case is reserved", cp.is_valid_variable_name( "ACCESS" ), false );
check( "upper with digit is allowed", cp.is_valid_variable_name( "ROOM2" ), true );
check( "leading digit", cp.is_valid_variable_name( "2room" ), false );
check( "punctuation", cp.is_valid_variable_name( "ro-om" ), false );
check( "empty", cp.is_valid_variable_name( "" ), false );

// --------------------------------------------------------------------
heading( "scripts" );

check( "steps skip blanks and comments", cp.split_script( "status\n\n# note\n  messages review 0000001  \r\n" ),
 [ { line: 1, text: "status" }, { line: 4, text: "messages review 0000001" } ] );

// --------------------------------------------------------------------
heading( "output" );

check( "error", cp.is_error_output( "Error: Unknown access pin." ), true );
check( "bad", cp.is_error_output( "[bad]" ), true );
check( "okay", cp.is_error_output( "[okay]" ), false );

var long = [ ];

for( var i = 1; i <= 45; i++ )
   long.push( "line " + i );

var cut = cp.truncate_lines( long.join( "\n" ), 40 );

check( "head kept", cut.shown.length, 40 );
check( "rest held back", cut.hidden, [ "line 41", "line 42", "line 43", "line 44", "line 45" ] );
check( "short output untouched", cp.truncate_lines( "a\nb", 40 ), { shown: [ "a", "b" ], hidden: [ ] } );

// --------------------------------------------------------------------
heading( "history" );

var history = [ ];

history = cp.push_history( history, "status" );
history = cp.push_history( history, "status" );
history = cp.push_history( history, "  " );
history = cp.push_history( history, "vars" );

check( "repeats and blanks dropped", history, [ "status", "vars" ] );
check( "oldest dropped at the limit", cp.push_history( [ "a", "b", "c" ], "d", 3 ), [ "b", "c", "d" ] );

// --------------------------------------------------------------------
heading( "list language" );

// NOTE: The guards see the line after substitution - "?{aaa} echo" is "?xxx echo" when aaa
// is set and "? echo" when it is not.
check( "? runs when the value was there", cp.apply_line_guard( "?xxx echo has aaa" ), { run: true, text: "echo has aaa" } );
check( "? skips when it was not", cp.apply_line_guard( "? echo has ccc" ), { run: false, text: "" } );
check( "! runs when it was not", cp.apply_line_guard( "! echo not has ccc" ), { run: true, text: "echo not has ccc" } );
check( "! skips when it was", cp.apply_line_guard( "!xxx echo not has aaa" ), { run: false, text: "" } );
check( "? then ! on one line", cp.apply_line_guard( "?a ! echo both" ), { run: true, text: "echo both" } );
check( "a line without a guard is untouched", cp.apply_line_guard( "echo plain" ), { run: true, text: "echo plain" } );
check( "a lone ? with no space is left alone", cp.apply_line_guard( "?" ), { run: true, text: "?" } );
check( "raw protocol is not a guard", cp.apply_line_guard( "~run_script !irc_join" ), { run: true, text: "~run_script !irc_join" } );

check( "var name shows it", cp.parse_var_command( "aaa" ), { kind: "show", name: "aaa" } );
check( "var name text sets it", cp.parse_var_command( "aaa xxx and more" ), { kind: "set", name: "aaa", value: "xxx and more", only_if_unset: false } );
check( "var !name sets only if unset", cp.parse_var_command( "!entropy de60" ), { kind: "set", name: "entropy", value: "de60", only_if_unset: true } );
check( "var @name null removes", cp.parse_var_command( "@ccc null" ), { kind: "remove", name: "ccc", value: "", only_if_unset: false } );
check( "var @name global reads a script result", cp.parse_var_command( "@entropy_2 ciyam_bip39_result" ), { kind: "from_script", name: "entropy_2", source: "ciyam_bip39_result", only_if_unset: false } );
check( "var #name substr with a length", cp.parse_var_command( "#pwd_hard substr:0,8" ), { kind: "substr", name: "pwd_hard", start: 0, length: 8 } );
check( "var #name substr without one", cp.parse_var_command( "#pwd_hard substr:4" ), { kind: "substr", name: "pwd_hard", start: 4, length: null } );
check( "an unknown function is refused", cp.parse_var_command( "#x upper:1" ).kind, "error" );
check( "an all upper case name is reserved", cp.parse_var_command( "DEVICE x" ).kind, "error" );
check( "no arguments is a usage error", cp.parse_var_command( "" ).kind, "error" );

check( "substr with a length", cp.substr_of( "abcdefghij", 0, 8 ), "abcdefgh" );
check( "substr to the end", cp.substr_of( "abcdefghij", 4, null ), "efghij" );

check( "output starts empty", cp.append_output( "", "one" ), "one" );
check( "output adds a line", cp.append_output( "one", "two" ), "one\ntwo" );

check( "load script is a javascript line", cp.is_javascript_line( "load script bip39 3c6e" ), true );
check( "eval script is one", cp.is_javascript_line( "eval script xor_hex_data {entropy_1}" ), true );
check( "exec script is one", cp.is_javascript_line( "exec script harden {rounds}:30" ), true );
check( "plain exec is not", cp.is_javascript_line( "exec" ), false );
check( "view scripts is not", cp.is_javascript_line( "view scripts" ), false );
check( "execute script is one", cp.is_javascript_line( "execute script harden 10" ), true );
check( "exec resolves to the console", cp.resolve_command( "exec" ), { kind: "local", name: "exec", args: "" } );

// --------------------------------------------------------------------
heading( "server javascript lines" );

// NOTE: The lines of Ian's two demo lists, and the harness's other verbs.
check( "load with an input", cp.parse_script_line( "load script bip39 3c6eede8b0c9717370546cd446427499" ),
 { kind: "script", verb: "load", name: "bip39", arg: "3c6eede8b0c9717370546cd446427499" } );
check( "load with none", cp.parse_script_line( "load script xor_hex_data" ), { kind: "script", verb: "load", name: "xor_hex_data", arg: null } );
check( "eval keeps every word", cp.parse_script_line( "eval script bip39 abandon ability able about" ),
 { kind: "script", verb: "eval", name: "bip39", arg: "abandon ability able about" } );
check( "exec is eval", cp.parse_script_line( "exec script harden 33333:30" ), { kind: "script", verb: "eval", name: "harden", arg: "33333:30" } );
check( "employ and execute too", [ cp.parse_script_line( "employ javascript harden 1" ).verb, cp.parse_script_line( "execute script harden 1" ).verb ],
 [ "eval", "eval" ] );
check( "reload is load", cp.parse_script_line( "reload javascripts harden" ).verb, "load" );
check( "result, named", cp.parse_script_line( "result script bip39" ), { kind: "script", verb: "result", name: "bip39", arg: null } );
check( "result without a name is this account's", cp.parse_script_line( "result script" ), { kind: "script", verb: "result", name: "***", arg: null } );
check( "unload", cp.parse_script_line( "unload script harden" ), { kind: "script", verb: "unload", name: "harden", arg: null } );
check( "this account's own", cp.parse_script_line( "load script ***" ).name, "***" );
check( "load needs a name", cp.parse_script_line( "load script" ), { kind: "error", message: "Name the script - load script <name> [<input>]" } );
check( "no path in a name", cp.parse_script_line( "load script ../secret" ).kind, "error" );
check( "no quotes in a name", cp.parse_script_line( "eval script a\"b 1" ).kind, "error" );
check( "not a script line", [ cp.parse_script_line( "view scripts" ), cp.parse_script_line( "status" ) ], [ null, null ] );

check( "a global's name", [ cp.is_global_name( "ciyam_harden_result" ), cp.is_global_name( "$x" ) ], [ true, true ] );
check( "not a global's name", [ cp.is_global_name( "a.b" ), cp.is_global_name( "1x" ), cp.is_global_name( "" ), cp.is_global_name( "a b" ) ],
 [ false, false, false, false ] );

check( "list names, one per line", cp.parse_name_list( "demo_bip39_entropy\ndemo_echo_variables\ndemo_new_unlock_key\n" ), [ "demo_bip39_entropy", "demo_echo_variables", "demo_new_unlock_key" ] );
check( "no lists", cp.parse_name_list( "[none]" ), [ ] );
check( "this account's own is kept", cp.parse_name_list( "***\nbip39\nharden\n" ), [ "***", "bip39", "harden" ] );
check( "an error is no lists", cp.parse_name_list( "Error: This web session is not valid (or has expired)." ), [ ] );

heading( "palette" );

var items = [
 { command: "messages review 0000000", description: "list rooms" },
 { command: "users review", description: "list every account" },
 { command: "messages create 0000000 text=<name>", description: "create a room" }
];

check( "empty query keeps all", cp.filter_palette( items, "" ).length, 3 );

check( "every word must match", cp.filter_palette( items, "messages create" ).map( function( i ) { return i.command; } ),
 [ "messages create 0000000 text=<name>" ] );

check( "command matches rank first", cp.filter_palette( items, "list" ).map( function( i ) { return i.command; } ),
 [ "messages review 0000000", "users review" ] );

check( "description only", cp.filter_palette( items, "rooms" ).length, 1 );

var views = [
 { command: "messages review 0000000", description: "list rooms" },
 { command: "view lists", description: "command lists on the server" } ];

check( "a word starting with it ranks first", cp.filter_palette( views, "view" ).map( function( i ) { return i.command; } ),
 [ "view lists", "messages review 0000000" ] );

check( "placeholder", cp.first_placeholder( "messages create 0000000 text=<name>" ), { start: 29, end: 35 } );
check( "no placeholder", cp.first_placeholder( "users review" ), null );

// --------------------------------------------------------------------
heading( "storage" );

check( "hashed password shortened", cp.summarise_storage_value( "cws.hashed_45679", "8dd2f1c70a4cf9a1f9be166e604aebdf" ),
 "8dd2f1c70a4cf9a..." );

check( "other values whole", cp.summarise_storage_value( "cws.access", "45679,20101" ), "45679,20101" );

// --------------------------------------------------------------------
heading( "preferences" );

check( "nothing stored", cp.parse_prefs( null ), { log_session_only: false, log_polling: false } );
check( "stored value", cp.parse_prefs( "{\"log_session_only\":true}" ), { log_session_only: true, log_polling: false } );
check( "not JSON", cp.parse_prefs( "yes please" ), { log_session_only: false, log_polling: false } );
check( "wrong type", cp.parse_prefs( "{\"log_session_only\":\"true\"}" ), { log_session_only: false, log_polling: false } );
check( "unknown names dropped", cp.parse_prefs( "{\"log_session_only\":true,\"other\":1}" ), { log_session_only: true, log_polling: false } );
check( "not an object", cp.parse_prefs( "[true]" ), { log_session_only: false, log_polling: false } );
check( "log polling", cp.parse_prefs( "{\"log_polling\":true}" ), { log_session_only: false, log_polling: true } );

// --------------------------------------------------------------------
heading( "request log entries" );

var t0 = new Date( 2026, 8, 25, 23, 14, 4, 0 );
var t1 = new Date( 2026, 8, 25, 23, 14, 4, 38 );

var entry = cp.make_log_entry( "console", "post",
 "http://localhost:13031/cws/messages/0000000?access=45679&device=c5588b&format=text&options=text%3DDesign%20Review&session=ddd52e",
 null, "0000004-8d88bf9c1b679df29aea5a719ca33fe4", t0, t1 );

check( "entry", entry, {
 source: "console", time: "23:14:04", method: "POST", endpoint: "/cws/messages/0000000",
 request: "text=Design Review", response: "0000004-8d88bf9c1b679df29aea5a719ca33fe4", ok: true, ms: 38 } );

var secret = cp.make_log_entry( "chat", "POST",
 "http://h/cws/devices?access=45679&format=text&passwd=YWRtaW46NDg0&request=%40none", null, "c5588b", t0, t1 );

check( "credentials never kept", /45679|passwd|YWRt|device=/.test( JSON.stringify( secret ) ), false );
check( "request kept", secret.request, "@none" );

check( "error marked", cp.make_log_entry( "console", "GET", "http://h/cws?request=x", null, "[bad]", t0, t1 ).ok, false );

var failed = cp.make_log_entry( "console", "GET", "http://h/cws/status", null, null, t0, t1 );

check( "network failure", [ failed.ok, failed.response ], [ false, "(no response - the request failed)" ] );

check( "posted body kept", cp.make_log_entry( "chat", "POST", "http://h/cws/messages/0000004", "hello", "[okay]", t0, t1 ).request,
 "hello" );

check( "long response cut", /truncated at 20000/.test(
 cp.make_log_entry( "chat", "GET", "http://h/cws/status", null, "x".repeat( 20100 ), t0, t1 ).response ), true );

// --------------------------------------------------------------------
heading( "log capture" );

( async function( )
{
   var quiet = false;

   var logged = [ ];

   var stamps = [ t0, t1, t0, t1 ];

   var fake = {
      fetch: async function( url, type, callback ) { callback( "[okay]" ); },
      post: async function( url, text, callback ) { callback( "posted" ); }
   };

   cp.install_log_capture( fake, "chat", function( ) { return quiet; },
    function( e ) { logged.push( e ); }, function( ) { return stamps.shift( ) || t1; } );

   var seen = "";

   await fake.fetch( "http://h/cws/status?access=1&session=2", "GET", function( r ) { seen = r; } );

   check( "caller still gets the response", seen, "[okay]" );
   check( "request logged", logged.map( function( e ) { return e.endpoint; } ), [ "/cws/status" ] );

   quiet = true;

   await fake.fetch( "http://h/cws/messages/0000001?options=from%3D0", "GET", function( ) { } );

   check( "quiet request not logged", logged.length, 1 );

   quiet = false;

   await fake.post( "http://h/cws/messages/0000004", "hi", function( r ) { seen = r; } );

   check( "post logged with body", [ logged.length, logged[ 1 ].request, seen ], [ 2, "hi", "posted" ] );

   var silent = { fetch: async function( ) { }, post: async function( ) { } };

   var dropped = [ ];

   cp.install_log_capture( silent, "console", function( ) { return false; }, function( e ) { dropped.push( e ); } );

   await silent.fetch( "http://h/cws/status", "GET", function( ) { } );

   check( "a request that never answers is still logged", [ dropped.length, dropped[ 0 ].ok ], [ 1, false ] );

   console.log( "" );
   console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

   process.exit( failures === 0 ? 0 : 1 );
} )( );

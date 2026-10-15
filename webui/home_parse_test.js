// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Exercises "home_parse.js" against fixed inputs and prints a deterministic transcript. No
// server and no browser are needed. The "/system" and "/uptime" replies are real ones from the
// development container and three clients, 2026-10-07 and 2026-10-08.
//
// Run with:  node home_parse_test.js
// Compare against "home_parse_test.tst" - see "run_chat_tests.sh".

// NOTE: In the browser the page loads "chat_parse.js" first; the helpers it uses from there are globals,
// and made so here.
const chat_parse = require( "./chat_parse.js" );

global.visible_rooms = chat_parse.visible_rooms;
global.is_dm_name = chat_parse.is_dm_name;
global.dm_request_text = chat_parse.dm_request_text;

const hp = require( "./home_parse.js" );

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

// --------------------------------------------------------------------
heading( "/system - the node's state" );

check( "ready over plain HTTP", hp.parse_system( "CIYAM 0.0.0 (NONE)" ),
 { state: "ready", version: "0.0.0", cipher: "", group: "", security: "none" } );

check( "locked", hp.parse_system( ":CIYAM: 0.0.0 (NONE)" ).state, "locked" );
check( "new - no identity yet", hp.parse_system( "*CIYAM* 0.0.0 (NONE)" ).state, "new" );
check( "a server older than the cipher - no security said", hp.parse_system( "CIYAM 0.0.0" ),
 { state: "ready", version: "0.0.0", cipher: "", group: "", security: "" } );
check( "a trailing line break", hp.parse_system( "CIYAM 0.0.0 (NONE)\n" ).state, "ready" );
check( "not a node", hp.parse_system( "<html>Not Found</html>" ).state, "" );
check( "nothing", hp.parse_system( "" ).state, "" );
check( "another name", hp.parse_system( "CIYAMX 0.0.0" ).state, "" );

// --------------------------------------------------------------------
heading( "/system - the connection's security" );

check( "Chrome 154 - ML-KEM", hp.parse_system( "CIYAM 0.0.0 TLS_AES_128_GCM_SHA256 (X25519MLKEM768)" ),
 { state: "ready", version: "0.0.0", cipher: "TLS_AES_128_GCM_SHA256", group: "X25519MLKEM768", security: "quantum" } );
check( "OpenSSL 3.5's curl - ML-KEM", hp.parse_system( "CIYAM 0.0.0 TLS_AES_256_GCM_SHA384 (X25519MLKEM768)" ).security, "quantum" );
check( "an older curl - X25519 alone", hp.parse_system( "CIYAM 0.0.0 TLS_AES_256_GCM_SHA384 (x25519)" ),
 { state: "ready", version: "0.0.0", cipher: "TLS_AES_256_GCM_SHA384", group: "x25519", security: "encrypted" } );
check( "ML-KEM alone, any case", hp.parse_system( "CIYAM 0.0.0 TLS_AES_256_GCM_SHA384 (mlkem768)" ).security, "quantum" );
check( "a cipher with no group", hp.parse_system( "CIYAM 0.0.0 TLS_AES_256_GCM_SHA384" ).security, "encrypted" );
check( "locked, over HTTPS", hp.parse_system( ":CIYAM: 0.0.0 TLS_AES_128_GCM_SHA256 (X25519MLKEM768)" ).security, "quantum" );

check( "the pill - none", hp.security_text( "none" ), { pill: "Not encrypted", heading: "Connection: not encrypted" } );
check( "the pill - encrypted", hp.security_text( "encrypted" ), { pill: "Encrypted connection", heading: "Connection: encrypted" } );
check( "the pill - quantum", hp.security_text( "quantum" ),
 { pill: "Quantum-resistant connection", heading: "Connection: encrypted, quantum-resistant" } );
check( "the pill - not said", hp.security_text( "" ), { pill: "", heading: "" } );

check( "the master password is warned against over plain HTTP", hp.warns_master_password( "none" ), true );
check( "not over TLS", hp.warns_master_password( "encrypted" ), false );
check( "not over ML-KEM", hp.warns_master_password( "quantum" ), false );
check( "nor when the node did not say", hp.warns_master_password( "" ), false );

// --------------------------------------------------------------------
heading( "arriving" );

check( "ready - the sign in", hp.arriving_screen( "ready" ), "signin" );
check( "locked - Unlock", hp.arriving_screen( "locked" ), "unlock" );
check( "new - Set up", hp.arriving_screen( "new" ), "setup" );
check( "no answer - unreachable", hp.arriving_screen( "" ), "unreachable" );

check( "a locked node's refusal said plainly", /recover it with its twelve words/.test(
 hp.locked_sign_in_text( "Error: Was unable to start a web session with access token '97620'." ) ), true );
check( "any other refusal left as it was", hp.locked_sign_in_text( "Error: Invalid password." ), "" );
check( "any refusal, when the node is known to be locked", /recover it with its twelve words/.test(
 hp.locked_sign_in_text( "Error: This web session is not valid (or has expired).", true ) ), true );
check( "no error", hp.locked_sign_in_text( "" ), "" );

// --------------------------------------------------------------------
heading( "unlock keys" );

check( "as the node gives it", hp.normalise_unlock_key( "NUJ5M-mk4eV-hWNWY" ), "NUJ5M-mk4eV-hWNWY" );
check( "with spaces, as the docs show it", hp.normalise_unlock_key( " NUJ5M mk4eV  hWNWY " ), "NUJ5M-mk4eV-hWNWY" );
check( "with no separators", hp.normalise_unlock_key( "NUJ5Mmk4eVhWNWY" ), "NUJ5M-mk4eV-hWNWY" );
check( "a hyphen inside a group", hp.normalise_unlock_key( "a-b_c-D3f9z-___-x" ), "a-b_c-D3f9z-___-x" );
check( "case is kept", hp.normalise_unlock_key( "nuj5m-MK4Ev-HwnwY" ), "nuj5m-MK4Ev-HwnwY" );
check( "too short", hp.normalise_unlock_key( "NUJ5M-mk4eV-hWNW" ), "" );
check( "a character base64 does not have", hp.normalise_unlock_key( "NUJ5M-mk4eV-hWN+Y" ), "" );
check( "the master password is not a key", hp.normalise_unlock_key( "need_to_use_a_very_good_password" ), "" );
check( "nothing", hp.normalise_unlock_key( "" ), "" );

check( "keys - asked of the node", hp.c_home_keys_request, "system_variable @num_unlock_keys" );
check( "keys - the node's count", [ hp.parse_key_count( "3" ), hp.parse_key_count( "3\n" ), hp.parse_key_count( "0" ) ], [ 3, 3, 0 ] );
check( "keys - no answer is not none", [ hp.parse_key_count( null ), hp.parse_key_count( "" ) ], [ null, null ] );
check( "keys - not a count", [ hp.parse_key_count( "lots" ), hp.parse_key_count( "-2" ), hp.parse_key_count( "Error: Not permitted." ) ],
 [ null, null, null ] );

// --------------------------------------------------------------------
heading( "admin's Overview" );

check( "people - admin not counted", hp.people_summary( [
 { pin: "72237", name: "admin", you: true, status: "admin" },
 { pin: "20401", name: "verify-a", you: false, status: "active" },
 { pin: "20405", name: "verify-e", you: false, status: "active" },
 { pin: "20303", name: "", you: false, status: "unclaimed" } ] ), { active: 2, unclaimed: 1 } );
check( "people - nobody", hp.people_summary( [ ] ), { active: 0, unclaimed: 0 } );

check( "uptime - as the node gives it", hp.parse_uptime( "41m 45s" ), "41m 45s" );
check( "uptime - weeks", hp.parse_uptime( "1w 0d 3h 4m 5s\n" ), "1w 0d 3h 4m 5s" );
check( "uptime - an error", hp.parse_uptime( "Error: something" ), "" );
check( "the node's line - its version and how long it has been up", [ hp.node_line( "0.0.0", "11 minutes" ), hp.node_line( "0.0.0", "" ) ],
 [ "Home node · CIYAM 0.0.0 · up 11 minutes", "Home node · CIYAM 0.0.0" ] );
check( "uptime in words - minutes, no seconds (Ian, 2026-10-11)", hp.uptime_words( "41m 45s" ), "41 minutes" );
check( "uptime in words - an hour and seconds is the hour", hp.uptime_words( "1h 0m 5s" ), "1 hour" );
check( "uptime in words - under a minute, just started", hp.uptime_words( "45s" ), "just started" );
check( "uptime in words - the two largest, a nothing skipped", hp.uptime_words( "1w 0d 3h 4m 5s" ), "1 week 3 hours" );
check( "uptime in words - one of each", hp.uptime_words( "1d 1h 1m 1s" ), "1 day 1 hour" );
check( "uptime in words - just started", hp.uptime_words( "0s" ), "just started" );
check( "uptime in words - an error", hp.uptime_words( "Error: something" ), "" );

// --------------------------------------------------------------------
heading( "logs" );

check( "the log names", hp.parse_log_names( "script\nserver\nupdate\n" ), [ "script", "server", "update" ] );
check( "the log names - blank lines and repeats", hp.parse_log_names( "\r\nserver\r\n\r\nserver\n" ), [ "server" ] );
check( "the log names - an error is not a name", hp.parse_log_names( "Error: Logs can only be viewed by the administrator." ), [ ] );

check( "a log's lines", hp.parse_log_lines( "one\r\ntwo\nthree\n" ), [ "one", "two", "three" ] );
check( "a log with no last break", hp.parse_log_lines( "one\ntwo" ), [ "one", "two" ] );
check( "an empty log", hp.parse_log_lines( "" ), [ ] );
check( "a blank line inside is kept", hp.parse_log_lines( "one\n\ntwo\n" ), [ "one", "", "two" ] );
check( "an empty log - the node's [none]", hp.parse_log_lines( "[none]" ), [ ] );
check( "the last 200 lines asked for", hp.log_options( 200 ), "lines=200" );
check( "every line asked for", hp.log_options( 0 ), "lines=0" );

check( "people - codes waiting", hp.people_note( { active: 6, unclaimed: 2 } ), "2 codes not yet claimed" );
check( "people - one code", hp.people_note( { active: 6, unclaimed: 1 } ), "1 code not yet claimed" );
check( "people - all claimed", hp.people_note( { active: 6, unclaimed: 0 } ), "Every code claimed" );

check( "people waiting - one", hp.people_waiting_text( { active: 4, unclaimed: 1 } ), "1 person hasn't used their code yet." );
check( "people waiting - two", hp.people_waiting_text( { active: 4, unclaimed: 2 } ), "2 people haven't used their code yet." );
check( "people waiting - none", hp.people_waiting_text( { active: 4, unclaimed: 0 } ), "Everyone you added has set up their account." );

// NOTE: "@sid_secure" (Ian, 2026-10-12) - no unlock keys without a secure identity.
check( "secure - asked of the node", hp.c_home_secure_request, "system_variable @sid_secure" );
check( "secure - '1' is held encrypted", [ hp.parse_secure( "1" ), hp.parse_secure( "1\n" ) ], [ true, true ] );
check( "secure - nothing is not - a development or demo node", hp.parse_secure( "" ), false );
check( "secure - '[okay]', how the node answers a variable with nothing in it, is not either", hp.parse_secure( "[okay]\n" ), false );
check( "secure - no answer, or anything else, is not known", [ hp.parse_secure( null ), hp.parse_secure( "Error: Not permitted." ) ], [ null, null ] );
check( "secure - not, said plainly", /no unlock keys/.test( hp.c_home_keys_unavailable ), true );

check( "keys - none left", /^None left\. .*make a few\.$/.test( hp.keys_note( 0 ) ), true );
check( "keys - one left", /^1 unlock key left\./.test( hp.keys_note( 1 ) ), true );
check( "keys - three left", /^3 unlock keys left\./.test( hp.keys_note( 3 ) ), true );
check( "keys - the node not saying", hp.keys_note( null ), "The node did not say how many unlock keys it holds." );

check( "a key too soon after another, said plainly", /a few seconds apart/.test(
 hp.key_error_text( "Error: *** attempt to create another unlock key too quickly ***" ) ), true );
check( "any other refusal as it came", hp.key_error_text( "Error: Only the administrator can do that." ), "Only the administrator can do that." );

check( "an error line", hp.log_line_kind( "[2026-10-07 13:20:10] [000005] [general] :: Error: unable to open file" ), "error" );
check( "a failure", hp.log_line_kind( "backup failed" ), "error" );
check( "a warning", hp.log_line_kind( "Warning: '/home/root/backup.img' was not found." ), "warn" );
check( "an ordinary line", hp.log_line_kind( "[2026-10-07 13:20:10] [000000] [general] :: (responding with 698 bytes)" ), "" );
check( "a word holding 'error' is not one", hp.log_line_kind( "terrors and mirrors" ), "" );

// NOTE: A real server log, the rebuilt container of 2026-10-09 - two starts, the second with errors after it.
var server_log = [
   "[2026-10-08 09:00:00] [000000] [general] server starting (pid :::::::::: 1)",
   "[2026-10-08 09:10:00] [000004] [general] session error: from the run before",
   "[2026-10-09 00:52:06] [000000] [general] server starting (pid :::::::::: 1)",
   "[2026-10-09 00:52:06] [000000] [general] rpc_core listener started on tcp port 11011",
   "[2026-10-09 00:52:14] [000003] [general] session error: system variable not found for '/home//shared' notifier",
   "[2026-10-09 00:53:00] [000003] [general] warning: something to watch",
   "[2026-10-09 00:56:38] [000005] [general] session error: \\ https://ntfy/upJW5R65MZ36PA" ];

check( "a log line's time and words", hp.log_line_parts( server_log[ 4 ] ),
 { time: "2026-10-09 00:52:14", text: "session error: system variable not found for '/home//shared' notifier" } );
check( "a line of another shape - all words", hp.log_line_parts( "just text" ), { time: "", text: "just text" } );
check( "problems since the restart - not before it", hp.log_problems( server_log ),
 { errors: 2, warnings: 1, latest: server_log[ 6 ], since_start: true } );
check( "no start among the lines read", hp.log_problems( server_log.slice( 3 ) ).since_start, false );
check( "no lines", hp.log_problems( [ ] ), { errors: 0, warnings: 0, latest: "", since_start: false } );

// NOTE: "Show them" shows these lines - the same the count is made of (2026-10-12).
var with_failed = server_log.concat( [ "[2026-10-09 01:00:00] [000006] [general] connection failed to peer",
 "[2026-10-09 01:00:01] [000006] [general] an errors_seen counter is not a problem" ] );
check( "the problem lines - since the restart, errors and warnings, 'failed' among them",
 hp.problem_lines( with_failed ), { lines: [ server_log[ 4 ], server_log[ 5 ], server_log[ 6 ], with_failed[ 7 ] ], since_start: true } );
check( "and exactly what is counted", hp.problem_lines( with_failed ).lines.length,
 hp.log_problems( with_failed ).errors + hp.log_problems( with_failed ).warnings );
check( "no start read - every problem line read", hp.problem_lines( server_log.slice( 3 ) ).lines.length, 3 );
check( "in words - both", hp.log_problems_text( hp.log_problems( server_log ) ), "2 errors and 1 warning since the restart" );

// NOTE: An unlock with a key - the line as the rebuilt node wrote it, 2026-10-15.
var unlocked_log = server_log.concat( [ "[2026-10-15 10:14:35] [000001] [general] *** system identity unlock occurred ***" ] );
check( "an unlock with a key since the restart - when, by the node's clock", hp.last_key_unlock( unlocked_log ),
 { time: "2026-10-15 10:14:35", this_run: true } );
check( "- one before the last restart", hp.last_key_unlock( unlocked_log.concat( [ server_log[ 2 ] ] ) ), { time: "2026-10-15 10:14:35", this_run: false } );
check( "- none read", hp.last_key_unlock( server_log ), null );
check( "- in words", [ hp.key_unlock_text( hp.last_key_unlock( unlocked_log ) ),
 hp.key_unlock_text( { time: "2026-10-15 10:14:35", this_run: false } ), hp.key_unlock_text( null ) ],
 [ "Unlocked with a key since the last restart at 2026-10-15 10:14 (the node's clock). Which key is never recorded - if no one here"
 + " used one, someone else has one.", "Last unlocked with a key at 2026-10-15 10:14 (the node's clock), before the last restart.", "" ] );
check( "in words - none", hp.log_problems_text( { errors: 0, warnings: 0, since_start: true } ), "No errors or warnings since the restart" );
check( "in words - one, no start read", hp.log_problems_text( { errors: 1, warnings: 0, since_start: false } ), "1 error in the log's latest lines" );

var listed = [
   { kind: "room", room: "0000001", name: "Administration", total: 120 },
   { kind: "room", room: "0000002", name: "General", total: 7412 },
   { kind: "room", room: "0000003", name: "Busy", total: 9000 },
   { kind: "room", room: "0000004", name: "", total: 7200 },
   { kind: "chat", room: "0000005", name: "not a room", total: 8999 } ];

check( "rooms filling - at 80% or more, fullest first", hp.rooms_filling( listed ).map( function( r ) { return r.name + " " + r.percent; } ),
 [ "Busy 100", "General 82", "#0000004 80" ] );
check( "rooms filling - none", hp.rooms_filling( listed.slice( 0, 1 ) ), [ ] );
check( "rooms filling - in words", [ hp.rooms_filling_text( [ ] ), hp.rooms_filling_text( [ 1 ] ), hp.rooms_filling_text( [ 1, 2 ] ) ],
 [ "No room is filling up", "1 room is filling up", "2 rooms are filling up" ] );
check( "a filling room", hp.room_filling_line( hp.rooms_filling( listed )[ 1 ] ), "General - 7,412 of 9,000 messages (82%)" );
check( "a full room", hp.room_filling_line( hp.rooms_filling( listed )[ 0 ] ), "Busy - full, 9,000 of 9,000 messages - the oldest make way for new ones" );

var log = [ "one alpha", "two beta", "three alpha", "four gamma", "five alpha" ];

check( "the log - the last lines", hp.log_view( log, "", 2 ), { lines: [ "four gamma", "five alpha" ], total: 5 } );
check( "the log - filtered, any case", hp.log_view( log, "ALPHA", 2 ), { lines: [ "three alpha", "five alpha" ], total: 3 } );
check( "the log - fewer than asked", hp.log_view( log, "beta", 10 ), { lines: [ "two beta" ], total: 1 } );
check( "the log - nothing matches", hp.log_view( log, "delta", 10 ), { lines: [ ], total: 0 } );
check( "the log - no count is the default", hp.log_view( log, "", 0 ).lines.length, 5 );

// --------------------------------------------------------------------
heading( "apps and sections" );

function keys( list ) { return list.map( function( item ) { return item.key; } ); }

check( "a member's apps", keys( hp.home_apps( false, false, false ) ), [ "home", "chat", "account" ] );
check( "admin's - the console too", keys( hp.home_apps( true, false, false ) ), [ "home", "chat", "account", "console" ] );
check( "anyone's on a development system", keys( hp.home_apps( false, true, false ) ), [ "home", "chat", "account", "console" ] );
check( "no console on a phone", keys( hp.home_apps( true, true, true ) ), [ "home", "chat", "account" ] );
check( "the pages", hp.home_apps( true, false, false ).map( function( app ) { return app.page; } ),
 [ "home.html", "chat.html", "account.html", "console.html" ] );

check( "admin's accounts app is My account too - People is in Home (2026-10-11)", hp.home_apps( true, false, false )[ 2 ].title, "My account" );
check( "a member's is My account", hp.home_apps( false, false, false )[ 2 ].title, "My account" );
check( "the tabs the apps reuse", hp.home_apps( true, false, false ).map( function( app ) { return app.tab; } ),
 [ "ciyam-home", "ciyam-chat", "ciyam-accounts", "ciyam-console" ] );

check( "admin's sections - People since 2026-10-11, Health and Alerts since 2026-10-12", keys( hp.home_sections( true ) ),
 [ "overview", "health", "people", "keys", "alerts", "logs" ] );
check( "a member has none", hp.home_sections( false ), [ ] );
check( "a copy, not the list itself", hp.home_sections( true ) !== hp.home_sections( true ), true );

check( "admin opens on the Overview", hp.home_section( true, "" ), "overview" );
check( "admin asking for the logs", hp.home_section( true, "#logs" ), "logs" );
check( "admin asking for something not theirs", hp.home_section( true, "#settings" ), "overview" );
check( "a member opens on Home", hp.home_section( false, "" ), "home" );
check( "a member asking for an admin section", hp.home_section( false, "#keys" ), "home" );

check( "the roles", [ hp.role_text( true ), hp.role_text( false ) ], [ "Administrator", "Member" ] );

// --------------------------------------------------------------------
heading( "a member's Home" );

// NOTE: Rooms as "parse_room_entry( )" gives them; Administration ("0000001") is not a member's to count.
var rooms = [
 { room: "0000001", unread: 4, total: 9, name: "Administration" },
 { room: "0000004", unread: 2, total: 30, name: "Book club" },
 { room: "0000007", unread: 1, total: 3, name: "/admin/verify-a" },
 { room: "0000009", unread: 0, total: 12, name: "Garden" } ];

var invitations = [
 { room: "0000011", token: "t1", name: "/bob/verify-a", inviter: "bob", unique: "1759900000002" },
 { room: "0000012", token: "t2", name: "Recipes", inviter: "carol", unique: "1759900000001" } ];

check( "the chat's summary - a member", hp.chat_summary( rooms, invitations, false ),
 { unread: 3, rooms: 2, requests: 1, invitations: 1, badge: 5 } );
check( "the badge is the chat's own count", hp.chat_summary( rooms, invitations, false ).badge,
 chat_parse.unread_elsewhere( rooms, invitations, "", false ) );
check( "admin counts Administration too", hp.chat_summary( rooms, [ ], true ).unread, 7 );
check( "nothing at all", hp.chat_summary( [ ], [ ], false ), { unread: 0, rooms: 0, requests: 0, invitations: 0, badge: 0 } );

check( "the Chat tile's line", hp.chat_summary_text( hp.chat_summary( rooms, invitations, false ) ),
 "3 unread in 2 rooms · 1 message request · 1 room invitation" );
check( "one of each, singular", hp.chat_summary_text( { unread: 1, rooms: 1, requests: 0, invitations: 0 } ), "1 unread in 1 room" );
check( "requests only, plural", hp.chat_summary_text( { unread: 0, rooms: 0, requests: 2, invitations: 0 } ), "2 message requests" );
check( "nothing new", hp.chat_summary_text( { unread: 0, rooms: 0, requests: 0, invitations: 0 } ), "Nothing new" );

check( "needs you - a request and an invitation", hp.needs_you( invitations, "verify-a" ), [
 { kind: "request", text: "bob wants to message you.", detail: "Message request", inviter: "bob", room: "0000011" },
 { kind: "invitation", text: "carol invited you to Recipes", detail: "Room invitation", inviter: "carol", room: "0000012" } ] );
check( "needs you - nothing", hp.needs_you( [ ], "verify-a" ), [ ] );

check( "a device shortened", hp.short_device( "6034a59f4554116a" ), "6034a5…116a" );
check( "a short one kept", hp.short_device( "abc" ), "abc" );
check( "the devices", hp.device_rows( [
 { device: "6034a59f4554116a", session: "s1", active: true, current: true },
 { device: "e5f6a7b8c9d0e1f2b8c9", session: "s2", active: true, current: false },
 { device: "c9d0e1f2a3b4f203", session: "s3", active: false, current: false } ] ), [
 { label: "This browser", title: "6034a59f4554116a", state: "Signed in now", active: true, current: true },
 { label: "e5f6a7…b8c9", title: "e5f6a7b8c9d0e1f2b8c9", state: "Signed in now", active: true, current: false },
 { label: "c9d0e1…f203", title: "c9d0e1f2a3b4f203", state: "Not signed in", active: false, current: false } ] );

function row( label, current, active ) { return { label: label, title: label, state: "", active: active, current: current }; }

var many = [ row( "off-1", false, false ), row( "on-1", false, true ), row( "here", true, true ), row( "off-2", false, false ),
 row( "on-2", false, true ), row( "on-3", false, true ), row( "on-4", false, true ) ];

check( "devices shown - this browser, then signed in, four in all",
 hp.devices_shown( many, 0 ).rows.map( function( r ) { return r.label; } ), [ "here", "on-1", "on-2", "on-3" ] );
check( "and how many more", hp.devices_shown( many, 0 ).more, 3 );
check( "the signed out last, when there is room", hp.devices_shown( many, 10 ).rows.map( function( r ) { return r.label; } ),
 [ "here", "on-1", "on-2", "on-3", "on-4", "off-1", "off-2" ] );
check( "a few - all of them, none more", hp.devices_shown( [ row( "here", true, true ) ], 0 ), { rows: [ row( "here", true, true ) ], more: 0 } );
check( "none", hp.devices_shown( [ ], 0 ), { rows: [ ], more: 0 } );
check( "more - three", hp.more_devices_text( 3 ), "and 3 more devices" );
check( "more - one", hp.more_devices_text( 1 ), "and 1 more device" );
check( "more - none", hp.more_devices_text( 0 ), "" );

// --------------------------------------------------------------------
heading( "health" );

var some_problems = hp.log_problems( server_log );
var some_filling = hp.rooms_filling( [ { room: "0000002", name: "General", kind: "room", total: 8280 },
 { room: "0000009", name: "Family", kind: "room", total: 9000 }, { room: "0000010", name: "Quiet", kind: "room", total: 12 } ] );
var issues = hp.health_issues( { secure: true, keys: 0, problems: some_problems, filling: some_filling } );
check( "what needs you - problems first, then the rest", issues.map( function( i ) { return i.key + " " + i.level; } ),
 [ "log problem", "room:0000009 problem", "room:0000002 warn", "keys warn" ] );
check( "- the log's, counted since the restart, its latest line and time", [ issues[ 0 ].title, issues[ 0 ].detail, issues[ 0 ].target ],
 [ "2 errors since the restart", "00:56:38 session error: \\ https://ntfy/upJW5R65MZ36PA", "problems" ] );
check( "- a full room, and one filling", [ issues[ 1 ].title, issues[ 2 ].title ], [ "Family is full", "General is 92% full" ] );
check( "- no unlock keys left - to make one", [ issues[ 3 ].title, issues[ 3 ].action, issues[ 3 ].target ], [ "No unlock keys left", "Make a key", "keys" ] );
check( "nothing wrong - nothing", hp.health_issues( { secure: true, keys: 3, problems: hp.log_problems( [ ] ), filling: [ ] } ), [ ] );
check( "warnings alone - a warning", hp.health_issues( { secure: true, keys: 2,
 problems: { errors: 0, warnings: 2, latest: "x", since_start: true }, filling: [ ] } )
 .map( function( i ) { return i.title + " " + i.level; } ), [ "2 warnings since the restart warn" ] );
check( "no keys on a node without a secure identity - not a problem", hp.health_issues( { secure: false, keys: 0, problems: null, filling: null } ), [ ] );
check( "what the node did not say raises nothing", hp.health_issues( { secure: null, keys: null, problems: null, filling: null } ), [ ] );
check( "the fullest rooms - filling or not, rooms only, as many as asked", hp.rooms_fullest( [ { room: "0000003", name: "", kind: "room", total: 40 },
 { room: "0000002", name: "General", kind: "room", total: 900 }, { room: "x", name: "bob", kind: "direct", total: 5000 },
 { room: "0000004", name: "Quiet", kind: "room", total: 1 } ], 2 ),
 [ { room: "0000002", name: "General", total: 900, percent: 10 }, { room: "0000003", name: "#0000003", total: 40, percent: 0 } ] );
check( "the heading", [ hp.health_heading( [ 1 ] ), hp.health_heading( [ 1, 2, 3 ] ) ], [ "1 thing needs you", "3 things need you" ] );
check( "the node's kind", [ hp.node_kind_text( true, false ), hp.node_kind_text( false, true ), hp.node_kind_text( false, false ),
 hp.node_kind_text( null, null ) ],
 [ "Development - raw protocol open to admin", "Demo", "Standard", "Not reported" ] );
check( "a flag - '1' or nothing", [ hp.parse_flag( "1" ), hp.parse_flag( "" ), hp.parse_flag( "[okay]" ), hp.parse_flag( null ) ],
 [ true, false, false, null ] );

// --------------------------------------------------------------------
heading( "alerts" );

check( "the ntfy server's example - this page's machine, ntfy's port", hp.ntfy_server_example( "antares.local" ), "http://antares.local:8090" );
check( "- with no machine, a placeholder", hp.ntfy_server_example( "" ), "http://<address>:8090" );
check( "the test - its title quoted whole", hp.c_home_ntfy_test_request.indexOf( '"-title=Test alert"' ) > 0, true );

// --------------------------------------------------------------------
heading( "setting up a new node" );

// NOTE: Real answers from a node left new (2026-10-12) - the first request's, then a second first request's.
check( "refused - the PIN held by another's setup, or gone", [ hp.setup_refused( "Error: This web session is not valid (or has expired)." ),
 hp.setup_refused( "Error: Invalid device identity 'x'." ), hp.setup_refused( "0123abcd" ), hp.setup_refused( null ) ], [ true, false, false, false ] );
check( "Enter in the password - on to typing it again, once long enough", [ hp.setup_password_enter( "new", "lantern-orbit", "", true ),
 hp.setup_password_enter( "new", "short", "", false ) ], [ { focus: "setup_password_confirm", problem: "", make: false },
 { focus: "", problem: "short", make: false } ] );
check( "Enter in typing it again - makes the words, once they match", [ hp.setup_password_enter( "confirm", "lantern-orbit", "lantern-orbit", true ),
 hp.setup_password_enter( "confirm", "lantern-orbit", "lantern-orbiT", true ) ],
 [ { focus: "", problem: "", make: true }, { focus: "", problem: "different", make: false } ] );
// NOTE: As the node gave them, through Home's admin request (2026-10-15).
check( "a node time, in UTC, as a unique", [ hp.node_time_unique( "2026-10-15 04:03:42" ), hp.node_time_unique( "soon" ) ],
 [ String( Date.UTC( 2026, 9, 15, 4, 3, 42 ) / 1000 ) + "000", "" ] );
var web_sessions = hp.parse_web_sessions( [
 "session              pin   device          init_request        last_request        username (ip_addr)",
 "-------------------- ----- --------------- ------------------- ------------------- ------------------",
 "0b9a4a69a7b862a80ad2 78826 3d53733e6a18ddb 2026-10-15 02:53:06 2026-10-15 02:53:36 admin (172.19.0.1)",
 "46c3509973a39671fcaa 20401 98706f1ff38d2dd 2026-10-15 03:49:54 2026-10-15 03:50:44 verify-a (172.19.0.1)",
 "aca181264ee1390b64ea 78826 a072581bd278fd1 2026-10-15 04:03:40 2026-10-15 04:03:42 admin (172.19.0.1)",
 "b93df46259379c5535d9 31324 0123456789abcde 2026-10-15 01:00:00 2026-10-15 01:00:05  (10.0.0.9)" ].join( "\n" ) );
check( "the web sessions - a line each, the heading and rule passed over", web_sessions.length, 4 );
check( "- each read whole", web_sessions[ 1 ], { session: "46c3509973a39671fcaa", pin: "20401", device: "98706f1ff38d2dd", since: "2026-10-15 03:49:54",
 last: "2026-10-15 03:50:44", name: "verify-a", address: "172.19.0.1" } );
check( "- one with no username", [ web_sessions[ 3 ].name, web_sessions[ 3 ].address ], [ "", "10.0.0.9" ] );
var at_four = Date.UTC( 2026, 9, 15, 4, 4, 0 );
var by_person = hp.sessions_summary( web_sessions, at_four, "aca181264ee1390b64ea" );
check( "by person, the most recently seen first - yours marked",
 by_person.people.map( function( p ) { return p.name + ":" + p.count + ( p.yours ? ":you" : "" ); } ), [ "admin:2:you", "verify-a:1", ":1" ] );
check( "- idle an hour or more counted - since 02:53 and 01:00", [ by_person.total, by_person.idle ], [ 4, 2 ] );
check( "- said in a line", [ hp.sessions_line( by_person ), hp.sessions_line( hp.sessions_summary( [ ], at_four, "" ) ) ],
 [ "4 sessions open · 2 idle an hour or more.", "No sessions open." ] );
// NOTE: As the node answers since Ian's "af67621d" - the PIN, and the start of the session and the device, X'd out.
var redacted = hp.parse_web_sessions( [
 "session              pin   device          init_request        last_request        username (ip_addr)",
 "-------------------- ----- --------------- ------------------- ------------------- ------------------",
 "XXXXXXXXXX7b862a80ad XXXXX XXXXXXXXXX18ddb 2026-10-15 02:53:06 2026-10-15 02:53:36 admin (172.19.0.1)",
 "XXXXXXXXXX9973a39671 XXXXX XXXXXXXXXX8d2dd 2026-10-15 03:49:54 2026-10-15 03:50:44 verify-a (172.19.0.1)",
 "XXXXXXXXXX4ee1390b64 XXXXX XXXXXXXXXX278fd 2026-10-15 04:03:40 2026-10-15 04:03:42 admin (172.19.0.1)",
 "XXXXXXXXXX5c00000000 XXXXX XXXXXXXXXXd0001 2026-10-15 04:00:00 2026-10-15 04:00:05 verify-a (10.0.0.4)",
 "XXXXXXXXXX59379c5535 XXXXX XXXXXXXXXXabcde 2026-10-15 01:00:00 2026-10-15 01:00:05  (10.0.0.9)" ].join( "\n" ) );
var redacted_by_person = hp.sessions_summary( redacted, at_four, "4f2a5b11115c00000000" );
check( "redacted - each line still read", [ redacted.length, redacted[ 3 ].pin, redacted[ 3 ].session, redacted[ 3 ].name ],
 [ 5, "XXXXX", "XXXXXXXXXX5c00000000", "verify-a" ] );
check( "this session known by the end of it, the start redacted", [ hp.is_same_session( "XXXXXXXXXX5c00000000", "4f2a5b11115c00000000" ),
 hp.is_same_session( "XXXXXXXXXX5c00000000", "4f2a5b11115c00000001" ), hp.is_same_session( "XXXXXXXXXXXXXXXXXXXX", "4f2a5b11115c00000000" ),
 hp.is_same_session( "0b9a4a69a7b862a80ad2", "0b9a4a69a7b862a80ad2" ), hp.is_same_session( "0b9a4a69a7b862a80ad2", "" ) ],
 [ true, false, false, true, false ] );
check( "redacted - grouped by name, yours marked", redacted_by_person.people.map( function( p )
 { return p.name + ":" + p.count + ( p.yours ? ":you" : "" ); } ), [ "admin:2", "verify-a:2:you", ":1" ] );
check( "the Overview's figure - how many open, how many idle; or not reported", [ hp.sessions_stat( by_person ),
 hp.sessions_stat( hp.sessions_summary( [ web_sessions[ 2 ] ], at_four, "" ) ), hp.sessions_stat( null ) ], [ { value: "4", note: "2 idle an hour or more",
 reported: true }, { value: "1", note: "None idle", reported: true }, { value: "Not reported", note: "Waiting on the node", reported: false } ] );
check( "the schedule - when next, the name, the rest as given", hp.parse_schedule( "2026-10-15 04:03:43 irc_store_or_restore 5s [@irc_alter]\n"
 + "2026-10-15 04:03:46 hub_check 5s [@generate_hub_block]" ), [ { next: hp.node_time_unique( "2026-10-15 04:03:43" ), name: "irc_store_or_restore",
 detail: "5s [@irc_alter]" }, { next: hp.node_time_unique( "2026-10-15 04:03:46" ), name: "hub_check", detail: "5s [@generate_hub_block]" } ] );
check( "what listens - named, the peer port's chains counted", hp.parse_listeners( "11011 rpc_core\n12021 rpc_peer (1461bcf13,31fcb1641,6ba75e11d)\n"
 + "13031 web_rest\n9999 something_new" ), [ { port: "11011", service: "rpc_core", name: "Core protocol", chains: 0 }, { port: "12021", service: "rpc_peer",
 name: "Peers", chains: 3 }, { port: "13031", service: "web_rest", name: "Web", chains: 0 }, { port: "9999", service: "something_new", name: "something_new",
 chains: 0 } ] );
check( "the still-alive token - twelve hex of the hash of the PIN and the suggestion", hp.setup_alive_token( "12345", "abcd",
 function( s ) { return s === "12345abcd" ? "0123456789ABCDEF0123" : "wrong"; } ), "0123456789ab" );
check( "the answer - all X's kept it waiting, an address did not, anything else says nothing", [ hp.setup_alive_answer( "XXXXXXXXXX" ),
 hp.setup_alive_answer( "172.17.0.1\n" ), hp.setup_alive_answer( "::1" ), hp.setup_alive_answer( "" ), hp.setup_alive_answer( "<html>" ) ],
 [ "kept", "gone", "gone", "unknown", "unknown" ] );
check( "the second refused - a PIN the node never gave (it restarted)", hp.setup_pin_gone( "Error: User credentials are either invalid or incorrect." ), true );
check( "the second refused - its PIN gone", [ hp.setup_pin_gone( "Error: System is currently locked for administration." ),
 hp.setup_pin_gone( "Error: This web session is not valid (or has expired)." ), hp.setup_pin_gone( "Error: Invalid device identity 'x'." ) ],
 [ true, true, false ] );
check( "the first request's answer - admin's PIN and the node's suggestion", hp.setup_reply( "32364 4651a055cc23e9ca17aa2582cb586c68\n" ),
 { pin: "32364", suggested: "4651a055cc23e9ca17aa2582cb586c68" } );
check( "a second first request - refused, not an answer", hp.setup_reply( "Error: This web session is not valid (or has expired)." ), null );
check( "nothing - not an answer", [ hp.setup_reply( "" ), hp.setup_reply( null ), hp.setup_reply( "32364" ) ], [ null, null, null ] );

check( "kept for the tab - read back", hp.setup_kept( JSON.stringify( { pin: "32364", suggested: "4651a055cc23e9ca17aa2582cb586c68" } ) ),
 { pin: "32364", suggested: "4651a055cc23e9ca17aa2582cb586c68" } );
check( "kept for the tab - and nothing else from it",
 hp.setup_kept( JSON.stringify( { pin: "32364", suggested: "4651a055cc23e9ca17aa2582cb586c68", words: "x" } ) ),
 { pin: "32364", suggested: "4651a055cc23e9ca17aa2582cb586c68" } );
check( "kept for the tab - anything else is nothing",
 [ hp.setup_kept( null ), hp.setup_kept( "{" ), hp.setup_kept( JSON.stringify( { pin: "1", suggested: "x" } ) ) ],
 [ null, null, null ] );

var crypto = require( "crypto" );
var sha = function( text ) { return crypto.createHash( "sha256" ).update( text ).digest( "hex" ); };
var pops = [ { t: 1012.5, x: 120, y: 40, hit: true }, { t: 1630.25, x: 300, y: 210, hit: false } ];
var mine = hp.randomness_hex( pops, "00ff", sha, 32 );
check( "the person's randomness - hex, as long as asked", /^[0-9a-f]{32}$/.test( mine ), true );
check( "- the same pops and browser numbers, the same", hp.randomness_hex( pops, "00ff", sha, 32 ), mine );
check( "- a pop a millisecond later, different", hp.randomness_hex( [ { t: 1013.5, x: 120, y: 40, hit: true }, pops[ 1 ] ], "00ff", sha, 32 ) !== mine, true );
check( "- other browser numbers, different", hp.randomness_hex( pops, "00fe", sha, 32 ) !== mine, true );
check( "- longer than one hash when asked", /^[0-9a-f]{100}$/.test( hp.randomness_hex( pops, "00ff", sha, 100 ) ), true );
check( "- a pop's colour counts too", hp.randomness_hex( [ { t: 1012.5, x: 120, y: 40, hit: true, c: "f53d3d" }, pops[ 1 ] ], "00ff", sha, 32 ) !== mine,
 true );
check( "a bubble's colour - the hue from two bytes, how vivid and light from the third", [ hp.bubble_colour( [ 0, 0, 255 ] ),
 hp.bubble_colour( [ 0, 120, 0 ] ) ], [ { rgb: "rgb(245, 61, 61)", hex: "f53d3d" }, { rgb: "rgb(52, 178, 52)", hex: "34b234" } ] );
check( "- any three bytes, a colour", [ [ 255, 255, 255 ], [ 1, 103, 17 ], [ 0, 59, 240 ], [ 0, 239, 128 ] ].every( function( b )
 { return /^[0-9a-f]{6}$/.test( hp.bubble_colour( b ).hex ); } ), true );

var setup_words = [ "orbit", "lemon", "canvas", "dutch", "ribbon", "wealth", "glide", "spare", "trophy", "marine", "elbow", "quiz" ];
var setup_list = setup_words.concat( [ "oyster", "fringe", "legal", "thumb", "cancel", "sponsor", "dust", "wagon" ] );
var seq = [ 0.95, 0.10, 0.40, 0.70, 0.20, 0.55, 0.85, 0.30, 0.65, 0.05, 0.45, 0.75 ];
var next = function( ) { var v = seq.shift( ); seq.push( v ); return v; };
var three = hp.word_choices( setup_words, 4, setup_list, next );
check( "checking a word - three choices, the right one among them", three.length === 3 && three.indexOf( "ribbon" ) >= 0, true );
check( "- the others are not the person's other words",
 three.filter( function( w ) { return w !== "ribbon"; } ).every( function( w ) { return setup_words.indexOf( w ) < 0; } ), true );
check( "- no word twice", three[ 0 ] !== three[ 1 ] && three[ 1 ] !== three[ 2 ] && three[ 0 ] !== three[ 2 ], true );

console.log( "" );
console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

process.exit( failures === 0 ? 0 : 1 );

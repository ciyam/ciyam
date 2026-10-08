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

check( "a locked node's refusal said plainly", /only be unlocked on the node itself/.test(
 hp.locked_sign_in_text( "Error: Was unable to start a web session with access token '97620'." ) ), true );
check( "any other refusal left as it was", hp.locked_sign_in_text( "Error: Invalid password." ), "" );
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

check( "keys made - a count", hp.parse_key_count( "3" ), 3 );
check( "keys made - none stored", hp.parse_key_count( null ), 0 );
check( "keys made - not a count", hp.parse_key_count( "lots" ), 0 );
check( "keys made - below none", hp.parse_key_count( "-2" ), 0 );

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
check( "uptime in words - minutes", hp.uptime_words( "41m 45s" ), "41 minutes 45 seconds" );
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

check( "people - codes waiting", hp.people_note( { active: 6, unclaimed: 2 } ), "2 codes not yet claimed" );
check( "people - one code", hp.people_note( { active: 6, unclaimed: 1 } ), "1 code not yet claimed" );
check( "people - all claimed", hp.people_note( { active: 6, unclaimed: 0 } ), "Every code claimed" );

check( "people waiting - one", hp.people_waiting_text( { active: 4, unclaimed: 1 } ), "1 person hasn't used their code yet." );
check( "people waiting - two", hp.people_waiting_text( { active: 4, unclaimed: 2 } ), "2 people haven't used their code yet." );
check( "people waiting - none", hp.people_waiting_text( { active: 4, unclaimed: 0 } ), "Everyone you added has set up their account." );

check( "keys - none made", /^None made here yet/.test( hp.keys_note( 0 ) ), true );
check( "keys - one made", /^1 key made here so far/.test( hp.keys_note( 1 ) ), true );
check( "keys - three made", /^3 keys made here so far/.test( hp.keys_note( 3 ) ), true );

check( "a key too soon after another, said plainly", /a few seconds apart/.test(
 hp.key_error_text( "Error: *** attempt to create another unlock key too quickly ***" ) ), true );
check( "any other refusal as it came", hp.key_error_text( "Error: Only the administrator can do that." ), "Only the administrator can do that." );

check( "an error line", hp.log_line_kind( "[2026-10-07 13:20:10] [000005] [general] :: Error: unable to open file" ), "error" );
check( "a failure", hp.log_line_kind( "backup failed" ), "error" );
check( "a warning", hp.log_line_kind( "Warning: '/home/root/backup.img' was not found." ), "warn" );
check( "an ordinary line", hp.log_line_kind( "[2026-10-07 13:20:10] [000000] [general] :: (responding with 698 bytes)" ), "" );
check( "a word holding 'error' is not one", hp.log_line_kind( "terrors and mirrors" ), "" );

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

check( "admin's accounts app is Accounts", hp.home_apps( true, false, false )[ 2 ].title, "Accounts" );
check( "a member's is My account", hp.home_apps( false, false, false )[ 2 ].title, "My account" );
check( "the tabs the apps reuse", hp.home_apps( true, false, false ).map( function( app ) { return app.tab; } ),
 [ "ciyam-home", "ciyam-chat", "ciyam-accounts", "ciyam-console" ] );

check( "admin's sections - People is the accounts page's", keys( hp.home_sections( true ) ), [ "overview", "keys", "logs" ] );
check( "a member has none", hp.home_sections( false ), [ ] );
check( "a copy, not the list itself", hp.home_sections( true ) !== hp.home_sections( true ), true );

check( "admin opens on the Overview", hp.home_section( true, "" ), "overview" );
check( "admin asking for the logs", hp.home_section( true, "#logs" ), "logs" );
check( "admin asking for something not theirs", hp.home_section( true, "#settings" ), "overview" );
check( "a member opens on Home", hp.home_section( false, "" ), "home" );
check( "a member asking for an admin section", hp.home_section( false, "#keys" ), "home" );

check( "the roles", [ hp.role_text( true ), hp.role_text( false ) ], [ "Administrator", "Member" ] );
check( "an app on Home's session", hp.linked_app_address( "chat.html", "1759999999123" ), "chat.html?source=1759999999123" );

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

console.log( "" );
console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

process.exit( failures === 0 ? 0 : 1 );

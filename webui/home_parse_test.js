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

check( "admin's sections", keys( hp.home_sections( true ) ), [ "overview", "keys", "logs", "people" ] );
check( "a member has none", hp.home_sections( false ), [ ] );
check( "a copy, not the list itself", hp.home_sections( true ) !== hp.home_sections( true ), true );

console.log( "" );
console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

process.exit( failures === 0 ? 0 : 1 );

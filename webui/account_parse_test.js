// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Exercises "account_parse.js" against fixed inputs and prints a deterministic
// transcript. No server and no browser are needed. The replies are real ones from the
// development container, 2026-10-02.
//
// Run with:  node account_parse_test.js
// Compare against "account_parse_test.tst" - see "run_chat_tests.sh".

// NOTE: In the browser the page loads "chat_parse.js" first; the helpers it uses from there are
// globals, and made so here.
const chat_parse = require( "./chat_parse.js" );

global.is_valid_username = chat_parse.is_valid_username;
global.password_strength = chat_parse.password_strength;

const ap = require( "./account_parse.js" );

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
heading( "codes" );

check( "the server's own form", ap.normalise_code( "qiu-ddxb-cht" ), "qiu-ddxb-cht" );
check( "in capitals, with spaces", ap.normalise_code( " QIU DDXB CHT " ), "qiu-ddxb-cht" );
check( "without its hyphens", ap.normalise_code( "qiuddxbcht" ), "qiu-ddxb-cht" );
check( "a letter short", ap.normalise_code( "qiu-ddxb-ch" ), "" );
check( "a digit is not a code", ap.normalise_code( "qiu-ddx8-cht" ), "" );
check( "nothing", ap.normalise_code( "" ), "" );

check( "a PIN", [ ap.is_account_pin( "20401" ), ap.is_account_pin( "2040" ), ap.is_account_pin( "2040a" ) ], [ true, false, false ] );

// --------------------------------------------------------------------
heading( "what the server says to a code or a PIN" );

check( "a code, just used - a new PIN, no username", ap.parse_join_reply( "37689 @none" ),
 { kind: "open", pin: "37689", username: "", fixed: false } );
check( "a PIN with the username admin chose", ap.parse_join_reply( "35887 @none fix-5887" ),
 { kind: "open", pin: "35887", username: "fix-5887", fixed: true } );
check( "a PIN with a suggested username", ap.parse_join_reply( "45887 @none ?sug-5887\n" ),
 { kind: "open", pin: "45887", username: "sug-5887", fixed: false } );
check( "a name that is not a valid username is not offered", ap.parse_join_reply( "76234 @none 55887" ),
 { kind: "open", pin: "76234", username: "", fixed: false } );
check( "a PIN that already has a password - a device token", ap.parse_join_reply( "6ccd313be34ed68" ), { kind: "claimed" } );
check( "an unknown PIN", ap.parse_join_reply( "Error: This web session is not valid (or has expired)." ),
 { kind: "error", error: "This web session is not valid (or has expired)." } );
check( "anything else", ap.parse_join_reply( "37689 something" ).kind, "error" );
check( "nothing", ap.parse_join_reply( "" ).kind, "error" );

// --------------------------------------------------------------------
heading( "people" );

var review = [ "40712 admin", "20101 ", "51093 bob", "20401 alice", "35887 ", "not a line" ].join( "\n" );

check( "admin first, then by name, then unclaimed by PIN", ap.parse_people( review, "40712" ).map( function( r ) { return r.pin + ":" + r.status; } ),
 [ "40712:admin", "20401:active", "51093:active", "20101:unclaimed", "35887:unclaimed" ] );
check( "a row", ap.parse_people( review, "40712" )[ 1 ], { pin: "20401", name: "alice", you: false, status: "active" } );
check( "admin is not in the review - added from the session", ap.parse_people( "20401 alice\n", "40712", "admin" ),
 [ { pin: "40712", name: "admin", you: true, status: "admin" }, { pin: "20401", name: "alice", you: false, status: "active" } ] );
check( "nobody, and no session", ap.parse_people( "", "" ), [ ] );
check( "Windows line ends", ap.parse_people( "20401 alice\r\n35887 \r\n", "" ).map( function( r ) { return r.name; } ), [ "alice", "" ] );

// --------------------------------------------------------------------
heading( "a PIN admin chooses" );

check( "both", ap.nominated_options( "63358", "dave", false ), "nominated=63358:dave" );
check( "a suggestion", ap.nominated_options( "63358", "dave", true ), "nominated=63358:?dave" );
check( "a PIN alone", ap.nominated_options( "63358", "", false ), "nominated=63358:" );
check( "a username alone", ap.nominated_options( "", "dave", true ), "nominated=?dave" );
check( "neither - a random PIN", ap.nominated_options( "", "", false ), "nominated=" );

var people = ap.parse_people( review, "40712" );

check( "fine", ap.nominate_problem( "63358", "dave", people ), "" );
check( "both empty is fine", ap.nominate_problem( "", "", people ), "" );
check( "a short PIN", ap.nominate_problem( "6335", "", people ), "A PIN is 5 digits." );
check( "a PIN in use", ap.nominate_problem( "20101", "", people ), "PIN 20101 is already in use." );
check( "a bad username", ap.nominate_problem( "", "Dave", people ).indexOf( "A username is" ), 0 );
check( "a username in use", ap.nominate_problem( "", "alice", people ), "Someone already has the username alice." );

// --------------------------------------------------------------------
heading( "a new username and password" );

check( "fine", ap.join_problem( "alice", "correct-horse", "correct-horse" ), "" );
check( "no username", ap.join_problem( "", "correct-horse", "correct-horse" ).indexOf( "Choose a username" ), 0 );
check( "too short a password", ap.join_problem( "alice", "short", "short" ), "Choose a password of at least 7 characters." );
check( "not the same twice", ap.join_problem( "alice", "correct-horse", "correct-hors" ), "The two passwords are not the same." );

// --------------------------------------------------------------------
heading( "the address in the QR code" );

check( "the code after #", ap.join_url( "http://localhost:13031", "qiu-ddxb-cht" ), "http://localhost:13031/account.html#code=qiu-ddxb-cht" );
check( "read back", ap.parse_account_hash( "#code=qiu-ddxb-cht" ), { view: "welcome", code: "qiu-ddxb-cht" } );
check( "read back, typed oddly", ap.parse_account_hash( "#code=QIU%20DDXB%20CHT" ), { view: "welcome", code: "qiu-ddxb-cht" } );
check( "a bad code still opens Welcome, empty", ap.parse_account_hash( "#code=nonsense" ), { view: "welcome", code: "" } );
check( "#welcome", ap.parse_account_hash( "#welcome" ), { view: "welcome", code: "" } );
check( "nothing", ap.parse_account_hash( "" ), { view: "", code: "" } );

// --------------------------------------------------------------------
heading( "a device token the node has never seen" );

check( "the server's words", ap.is_unknown_device_error( "Error: Invalid device identity 'c6f7ea5f172a64b'." ), true );
check( "anything else", [ ap.is_unknown_device_error( "Error: User credentials are either invalid or incorrect." ),
 ap.is_unknown_device_error( "" ) ], [ false, false ] );

// --------------------------------------------------------------------
heading( "a code or PIN the node does not know" );

// NOTE: The server's words for a made-up code, a used one and an unknown PIN alike - checked on
// a container built from 44ddc860, 2026-10-03 ("code_check_probe.js").
const c_refused = "Error: This web session is not valid (or has expired).";

check( "a code", ap.refused_join_text( c_refused, "code" ), "That code isn't known, or has already been used." );
check( "a PIN", ap.refused_join_text( c_refused, "pin" ), "That PIN isn't known on this node." );
check( "without the prefix", ap.refused_join_text( "This web session is not valid (or has expired).", "code" ),
 "That code isn't known, or has already been used." );
check( "anything else is passed on", ap.refused_join_text( "Error: Something else went wrong.", "code" ), "Something else went wrong." );
check( "nothing", ap.refused_join_text( "", "pin" ), "" );

// --------------------------------------------------------------------
heading( "the Add yourself banner" );

const banner_people = [ { pin: "23048", name: "admin" }, { pin: "40311", name: "damon" } ];

check( "shown until admin has added themselves", ap.shows_add_yourself( false, "", banner_people ), true );
check( "not once their own account is among the people", ap.shows_add_yourself( false, "40311", banner_people ), false );
check( "shown again if that account was removed", ap.shows_add_yourself( false, "51234", banner_people ), true );
check( "not once admin has hidden it", ap.shows_add_yourself( true, "", banner_people ), false );
check( "not a PIN is no account", ap.shows_add_yourself( false, "dam", banner_people ), true );

// --------------------------------------------------------------------
heading( "this account's devices" );

// NOTE: The shape of "GET /cws/devices", from Ian's code of 2026-10-06 - "<device> <session>", "*" after an active one.
const devices_text = "a1b2c3d4 11111111111111111111*\ne5f6a7b8 22222222222222222222\nc9d0e1f2 33333333333333333333*\n";

check( "each device, this browser first", ap.parse_devices( devices_text, "c9d0e1f2" ), [
 { device: "c9d0e1f2", session: "33333333333333333333", active: true, current: true },
 { device: "a1b2c3d4", session: "11111111111111111111", active: true, current: false },
 { device: "e5f6a7b8", session: "22222222222222222222", active: false, current: false } ] );
check( "with Windows line ends too", ap.parse_devices( "a1b2c3d4 111*\r\n", "x" ).length, 1 );
check( "nothing, or anything else, is none", [ ap.parse_devices( "", "x" ), ap.parse_devices( "Error: something", "x" ) ], [ [ ], [ ] ] );

console.log( "" );
console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

process.exit( failures === 0 ? 0 : 1 );

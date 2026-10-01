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

console.log( "" );
console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

process.exit( failures === 0 ? 0 : 1 );

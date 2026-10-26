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
global.password_refusal = chat_parse.password_refusal;

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
heading( "this account's devices" );

// NOTE: The shape of "GET /cws/devices", from Ian's code of 2026-10-06 - "<device> <session>", "*" after an active one.
const devices_text = "a1b2c3d4 11111111111111111111*\ne5f6a7b8 22222222222222222222\nc9d0e1f2 33333333333333333333*\n";

check( "each device, this browser first", ap.parse_devices( devices_text, "c9d0e1f2" ), [
 { device: "c9d0e1f2", session: "33333333333333333333", active: true, current: true },
 { device: "a1b2c3d4", session: "11111111111111111111", active: true, current: false },
 { device: "e5f6a7b8", session: "22222222222222222222", active: false, current: false } ] );
check( "with Windows line ends too", ap.parse_devices( "a1b2c3d4 111*\r\n", "x" ).length, 1 );
check( "nothing, or anything else, is none", [ ap.parse_devices( "", "x" ), ap.parse_devices( "Error: something", "x" ) ], [ [ ], [ ] ] );


// --------------------------------------------------------------------
heading( "your keys" );

const locked = "UK3:100000:000102030405060708090a0b0c0d0e0f:9a9e:" + "ab".repeat( 32 );

check( "the card's state - from this browser and the node", [ ap.keys_state( false, "[none]" ), ap.keys_state( true, "[none]\n" ),
 ap.keys_state( false, locked ), ap.keys_state( true, locked + "\n" ) ], [ "none", "lost", "elsewhere", "here" ] );
check( "- keys in an older form, as none - set up again", [ ap.keys_state( false, "UK2:" + locked.substring( 11 ) ),
 ap.keys_state( true, "UK1:1000:ab:cd:ef" ) ], [ "none", "lost" ] );
check( "- an error, or not locked keys, unreadable", [ ap.keys_state( true, "Error: Unknown request." ), ap.keys_state( false, "" ),
 ap.keys_state( false, "UK4:" + locked.substring( 4 ) ), ap.keys_state( false, locked.replace( ":9a9e:", ":9a9:" ) ) ],
 [ "unreadable", "unreadable", "unreadable", "unreadable" ] );

var draws = [ 0.99, 0, 0.5 ];

check( "words to check back - three different, in order", [ ap.keys_check_indexes( function( ) { return draws.shift( ); } ),
 ap.keys_check_indexes( function( ) { return 0; }, 12 ).length, ap.keys_check_indexes( Math.random, 20 ).length ], [ [ 0, 6, 11 ], 12, 12 ] );

const verify_a_key = "0028b35f26edb6ffe44d3d2cc597906f4065bd550d5009c4d62c422c789493e0";
const own_key = "bd03d5c91f430fc9bc585d232043d781d31b61616abcf19c7333cea4ea1b6bc9";

check( "a public key as pasted - capitals, spaces, broken over lines", [ ap.normalise_public_key( verify_a_key.toUpperCase( ) ),
 ap.normalise_public_key( " " + verify_a_key.substr( 0, 32 ) + "\n  " + verify_a_key.substr( 32 ) + " " ), ap.normalise_public_key( verify_a_key + "0" ),
 ap.normalise_public_key( verify_a_key.replace( "0", "g" ) ), ap.normalise_public_key( "" ) ], [ verify_a_key, verify_a_key, "", "", "" ] );
check( "adding someone's key - fine", ap.contact_problem( "verify-a", verify_a_key, { }, own_key ), "" );
check( "- not a username, no key, your own, one already kept, or theirs under another name", [
 ap.contact_problem( "Verify A", verify_a_key, { }, own_key ).indexOf( "Type their username" ), ap.contact_problem( "verify-a", "", { }, own_key ),
 ap.contact_problem( "verify-a", own_key, { }, own_key ), ap.contact_problem( "verify-a", verify_a_key, { "verify-a": own_key.replace( "b", "c" ) }, own_key ),
 ap.contact_problem( "verify-b", verify_a_key, { "verify-a": verify_a_key }, own_key ) ], [ 0,
 "A public key is 64 letters and digits, 0-9 and a-f - copy all of it.", "That's your own public key - theirs is shown on their My account.",
 "You have a key for verify-a already - remove it first to change it.", "That key is saved for verify-a already." ] );
check( "the keys kept, by name, shortened", ap.contact_rows( { "verify-b": own_key, "verify-a": verify_a_key } ), [
 { name: "verify-a", key: verify_a_key, short: "0028b35f…789493e0" }, { name: "verify-b", key: own_key, short: "bd03d5c9…ea1b6bc9" } ] );

// NOTE: My account's sections (2026-10-26) - "#mine", as the chat and Home have opened it, is the first.
check( "My account's sections, in order", ap.account_sections( ).map( function( s ) { return s.key; } ),
 [ "profile", "keys", "contacts", "devices", "notify", "password" ] );
check( "- the one an address asks for, else the first", [ ap.account_section( "#devices" ), ap.account_section( "keys" ),
 ap.account_section( "#mine" ), ap.account_section( "" ), ap.account_section( "#keys=00ff" ) ], [ "devices", "keys", "profile", "profile", "profile" ] );
check( "the room keys kept - none, one, or several in order", [ ap.room_keys_text( { } ), ap.room_keys_text( { "0000069": "ab" } ),
 ap.room_keys_text( { "0000072": "cd", "0000069": "ab", "0000070": "ef" } ) ], [
 "None yet. A room's owner makes one in the chat, under Room Details, and sends it to each member.",
 "Kept for room #0000069. Made, shared and removed in the chat, under Room Details.",
 "Kept for rooms #0000069, #0000070 and #0000072. Made, shared and removed in the chat, under Room Details." ] );

console.log( "" );
console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

process.exit( failures === 0 ? 0 : 1 );

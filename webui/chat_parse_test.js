// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Exercises "chat_parse.js" against fixed inputs and prints a deterministic
// transcript. No server and no browser are needed. Fixtures are taken from the real
// wire formats captured in "tests/test_node_1_b.tst" and from the design canvas.
//
// Run with:  node chat_parse_test.js
// Compare against "chat_parse_test.tst" - see "run_chat_tests.sh".

const cp = require( "./chat_parse.js" );

var failures = 0;

function show( label, value )
{
   console.log( label + ": " + JSON.stringify( value ) );
}

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

heading( "member list and presence" );

var members = cp.parse_members( "admin+0 test-1+1 test-2+2" );

check( "three members parsed", members.length, 3 );
check( "offline member", members[ 0 ], { name: "admin", sessions: 0, online: false } );
check( "two sessions", members[ 2 ], { name: "test-2", sessions: 2, online: true } );
check( "hyphenated name kept", members[ 1 ].name, "test-1" );
check( "empty line yields none", cp.parse_members( "" ).length, 0 );

var ordered = cp.apply_presence( members ).map( function( m ) { return m.name; } );

check( "online first then alphabetical", ordered, [ "test-1", "test-2", "admin" ] );

heading( "ordinary chat messages" );

var chat = cp.parse_message_line( "1757520000002 alice  hello everyone" );

check( "kind", chat.kind, "chat" );
check( "unique", chat.unique, "1757520000002" );
check( "sender", chat.sender, "alice" );
check( "leading marker space removed", chat.text, "hello everyone" );
check( "not edited", chat.edited, false );

var spaced = cp.parse_message_line( "1757520000003 jun-w  text with  a double space" );

check( "inner spacing preserved", spaced.text, "text with  a double space" );

heading( "edited messages" );

var edited = cp.parse_message_line( "1757520000123 alice*  corrected text" );

check( "edited flag", edited.edited, true );
check( "marker stripped from sender", edited.sender, "alice" );
check( "text", edited.text, "corrected text" );

heading( "system events" );

var joined = cp.parse_message_line( "1757520000001 test-1 :joined" );

check( "kind", joined.kind, "system" );
check( "verb", joined.event.verb, "joined" );
check( "sender", joined.sender, "test-1" );

// NOTE: The server emits a "room" keyword after the verb - see ISS-003.
var invite = cp.parse_message_line(
 "1757520000009 test-1 :invite room 0000002-9f3c1d8e4b7a2f60c5e3a91b8d7f4e11 Private (test-1 and test-2)" );

check( "invite verb", invite.event.verb, "invite" );
check( "invite room", invite.event.room, "0000002" );
check( "invite token", invite.event.token, "9f3c1d8e4b7a2f60c5e3a91b8d7f4e11" );
check( "invite room name", invite.event.name, "Private (test-1 and test-2)" );

var created = cp.parse_message_line(
 "1757520000007 test-1 :create 0000002-9f3c1d8e4b7a2f60c5e3a91b8d7f4e11 Project Room (draft)" );

check( "create verb", created.event.verb, "create" );
check( "create room", created.event.room, "0000002" );
check( "create name", created.event.name, "Project Room (draft)" );

var renamed = cp.parse_message_line(
 "1757520000140 admin :rename 'Administration' to 'Testing'" );

check( "rename verb", renamed.event.verb, "rename" );
check( "rename from", renamed.event.from_name, "Administration" );
check( "rename to", renamed.event.to_name, "Testing" );

var issued = cp.parse_message_line( "1757520000155 test-1 :issued (message sent to mira)" );

check( "issued verb", issued.event.verb, "issued" );
check( "issued kind", issued.event.issued_kind, "message" );
check( "issued has no room", issued.event.room, "" );
check( "issued recipients", issued.event.recipients, [ "mira" ] );

var issued_invite = cp.parse_message_line(
 "1757520000156 test-1 :issued (invite for 0000002 sent to test-2)" );

check( "issued invite kind", issued_invite.event.issued_kind, "invite" );
check( "issued invite room", issued_invite.event.room, "0000002" );
check( "issued invite recipients", issued_invite.event.recipients, [ "test-2" ] );

var issued_many = cp.parse_message_line(
 "1757520000157 alice :issued (message sent to jun-w,mira)" );

check( "several recipients", issued_many.event.recipients, [ "jun-w", "mira" ] );

heading( "entrance room listing" );

var room = cp.parse_message_line( "0000000000001 admin #0000001 2/7 Administration" );

check( "kind", room.kind, "room" );
check( "owner", room.owner, "admin" );
check( "room number", room.room, "0000001" );
check( "unread", room.unread, 2 );
check( "total", room.total, 7 );
check( "name", room.name, "Administration" );

var spaced_room = cp.parse_message_line( "0000000000001 alice #0000002 0/3 Private (test-1 and test-2)" );

check( "name with spaces", spaced_room.name, "Private (test-1 and test-2)" );

heading( "posting restrictions" );

var open_room = cp.parse_message_line( "0000000000001 admin #0000002 3/47 Ops Room" );
var own_room = cp.parse_message_line( "0000000000001 admin #0000003 3/47! Owner Only" );
var shut_room = cp.parse_message_line( "0000000000001 admin #0000004 0/12~ Locked Room" );

check( "unrestricted", open_room.posts, "any" );
check( "owner only marker", own_room.posts, "own" );
check( "locked marker", shut_room.posts, "none" );

check( "suffix stripped from total (own)", own_room.total, 47 );
check( "suffix stripped from total (none)", shut_room.total, 12 );
check( "name still parsed past a suffix", shut_room.name, "Locked Room" );
check( "locked convenience flag", shut_room.locked, true );
check( "not locked when owner only", own_room.locked, false );

heading( "who may post" );

var open_entry = { owner: "alice", posts: "any" };
var own_entry = { owner: "alice", posts: "own" };
var shut_entry = { owner: "alice", posts: "none" };

check( "entrance room never accepts posts",
 cp.posting_status( "0000000", null, "alice", false ).can_post, false );
check( "starting room refused for a standard user",
 cp.posting_status( "0000001", open_entry, "bob", false ).can_post, false );
check( "starting room allowed for admin",
 cp.posting_status( "0000001", open_entry, "admin", true ).can_post, true );
check( "open room allows anyone",
 cp.posting_status( "0000002", open_entry, "bob", false ).can_post, true );
check( "owner-only refuses a member",
 cp.posting_status( "0000002", own_entry, "bob", false ).can_post, false );
check( "owner-only allows the owner",
 cp.posting_status( "0000002", own_entry, "alice", false ).can_post, true );
check( "owner-only allows admin",
 cp.posting_status( "0000002", own_entry, "bob", true ).can_post, true );
check( "locked refuses the owner too",
 cp.posting_status( "0000002", shut_entry, "alice", false ).can_post, false );
check( "locked is flagged as locked",
 cp.posting_status( "0000002", shut_entry, "alice", false ).locked, true );
check( "a reason is always given when refused",
 cp.posting_status( "0000002", shut_entry, "alice", false ).reason.length > 0, true );

check( "starting room recognised", cp.is_starting_room( "0000001" ), true );
check( "other room is not the starting room", cp.is_starting_room( "0000002" ), false );

heading( "no new messages" );

var none = cp.parse_message_line( "0000000000000 (no new messages)" );

check( "kind", none.kind, "none" );
check( "unique retained", none.unique, "0000000000000" );

heading( "whole responses" );

var fetch = cp.parse_fetch_response(
 "alice+1 jun-w+2 mira+0\n"
 + "1757520000001 alice :joined\n"
 + "1757520000002 alice  hello everyone\n"
 + "1757520000123 alice*  corrected text" );

check( "members", fetch.members.length, 3 );
check( "messages", fetch.messages.length, 3 );
check( "no rooms", fetch.rooms.length, 0 );
check( "has new", fetch.has_new, true );

var entrance = cp.parse_fetch_response(
 "admin+1 alice+0\n"
 + "0000000000001 admin #0000001 2/7 Administration\n"
 + "0000000000001 alice #0000002 0/3 Project Room (draft)" );

check( "entrance rooms", entrance.rooms.length, 2 );
check( "entrance has no messages", entrance.messages.length, 0 );

var empty = cp.parse_fetch_response( "alice+1 jun-w+2\n0000000000000 (no new messages)" );

check( "quiet poll has members", empty.members.length, 2 );
check( "quiet poll has no messages", empty.messages.length, 0 );
check( "quiet poll has_new false", empty.has_new, false );

var failed = cp.parse_fetch_response( "Error: Unknown room '0000009' (or not joined)." );

check( "error detected", failed.error, "Unknown room '0000009' (or not joined)." );
check( "error yields no members", failed.members.length, 0 );

heading( "polling start point" );

check( "one past the highest unique",
 cp.next_start_point( fetch.messages ), "1757520000124" );
check( "padded to 13 digits",
 cp.next_start_point( [ { unique: "0000000000001" } ] ), "0000000000002" );
check( "empty when nothing seen", cp.next_start_point( [ ] ), "" );

heading( "room ordering" );

var listed = cp.derive_room_list( [
 { room: "0000004", unread: 0 },
 { room: "0000001", unread: 2 },
 { room: "0000002", unread: 0 },
 { room: "0000007", unread: 5 } ] ).map( function( r ) { return r.room; } );

check( "unread first, then by number", listed, [ "0000001", "0000007", "0000002", "0000004" ] );

heading( "room name validation" );

check( "accepts a plain name", cp.is_valid_room_name( "Ledger Ops" ), true );
check( "accepts parentheses", cp.is_valid_room_name( "Private (test-1 and test-2)" ), true );
check( "accepts an apostrophe", cp.is_valid_room_name( "Jun's Room" ), true );
check( "rejects underscores", cp.is_valid_room_name( "Name_Not_Valid" ), false );
check( "rejects too short", cp.is_valid_room_name( "Ops" ), false );
check( "rejects a leading digit", cp.is_valid_room_name( "1st Room" ), false );
check( "rejects a trailing space", cp.is_valid_room_name( "Ledger Ops " ), false );
check( "rejects a double space", cp.is_valid_room_name( "Ledger  Ops" ), false );
check( "rejects doubled ampersands", cp.is_valid_room_name( "Dev && Ops" ), false );
check( "rejects empty parentheses", cp.is_valid_room_name( "Ledger () Ops" ), false );
check( "rejects reversed parentheses", cp.is_valid_room_name( "Ledger )x( Ops" ), false );
check( "rejects two open parentheses", cp.is_valid_room_name( "Ledger ((x) Ops" ), false );

heading( "username validation" );

check( "accepts a simple name", cp.is_valid_username( "alice" ), true );
check( "accepts an inner hyphen", cp.is_valid_username( "test-1" ), true );
check( "rejects capitals", cp.is_valid_username( "Alice" ), false );
check( "rejects too short", cp.is_valid_username( "al" ), false );
check( "rejects too long", cp.is_valid_username( "abcdefghijklm" ), false );
check( "rejects a trailing hyphen", cp.is_valid_username( "alice-" ), false );
check( "rejects a doubled hyphen", cp.is_valid_username( "al--ice" ), false );
check( "rejects a leading digit", cp.is_valid_username( "1alice" ), false );

heading( "sender colours" );

check( "stable for a given name",
 cp.sender_colour_index( "alice" ), cp.sender_colour_index( "alice" ) );
check( "within range", ( cp.sender_colour_index( "jun-w" ) >= 1 )
 && ( cp.sender_colour_index( "jun-w" ) <= 6 ), true );

show( "alice", cp.sender_colour_index( "alice" ) );
show( "jun-w", cp.sender_colour_index( "jun-w" ) );
show( "mira", cp.sender_colour_index( "mira" ) );
show( "admin", cp.sender_colour_index( "admin" ) );

heading( "helpers" );

check( "entrance room recognised", cp.is_entrance_room( "0000000" ), true );
check( "other room not entrance", cp.is_entrance_room( "0000001" ), false );

// NOTE: The absolute value depends on the local time zone, so only the shape
// is pinned here.
check( "time shape", /^\d\d:\d\d:\d\d$/.test( cp.unique_to_time( "1757520000002" ) ), true );
check( "time of a bad unique", cp.unique_to_time( "not-a-number" ), "" );

heading( "system events" );

// NOTE: The server emits eight verbs - joined, create, invite, issued, rename, assign,
// allows and remove. Enumerating them in the view meant a new one lost its detail, so
// these pin the two that were being dropped.
var ev = cp.parse_system_event( ":allows set to own" );
check( "allows verb", ev.verb, "allows" );
check( "allows keeps its detail", ev.detail, "set to own" );

ev = cp.parse_system_event( ":assign 'admin' to 'tester-1'" );
check( "assign verb", ev.verb, "assign" );
check( "assign names the old owner", ev.from_name, "admin" );
check( "assign names the new owner", ev.to_name, "tester-1" );

ev = cp.parse_system_event( ":remove" );
check( "a bare verb has no detail", ev.detail, "" );
check( "a bare verb still parses", ev.verb, "remove" );

heading( "invitations already sent" );

var receipts = cp.parse_fetch_response( [
 "admin+1 tester-1+0",
 "1790259222001 admin :issued (invite for 0000005 sent to tester-1)",
 "1790259222002 admin :issued (invite for 0000005 sent to tester-2,tester-3)",
 "1790259222003 admin :issued (invite for 0000006 sent to damon)",
 "1790259222004 admin :issued (message sent to tester-1)",
 "1790259222005 admin hello" ].join( "\n" ) ).messages;

var sent = cp.invited_to_room( receipts, "0000005" );

check( "single recipient found", sent[ "tester-1" ], true );
check( "every name in a list found", ( sent[ "tester-2" ] === true ) && ( sent[ "tester-3" ] === true ), true );
check( "another room ignored", sent[ "damon" ], undefined );
check( "a private message receipt ignored", Object.keys( cp.invited_to_room( receipts, "" ) ).length, 0 );
check( "nothing for an unknown room", Object.keys( cp.invited_to_room( receipts, "0000009" ) ).length, 0 );
check( "no messages is no invitations", Object.keys( cp.invited_to_room( [ ], "0000005" ) ).length, 0 );

heading( "session handover fields" );

// NOTE: Receivers of the handover test for ":" then "-" then "=", so a field carrying "-"
// turned the credentials message into an owner message and it was dropped - ISS-016. The
// api.ciyam.org test accounts are all "test-N", so every one of them hit it.
var enc = cp.encode_channel_field( "tester-1" );

check( "hyphen is encoded", enc.indexOf( "-" ), -1 );
check( "colon is encoded", cp.encode_channel_field( "a:b" ).indexOf( ":" ), -1 );
check( "equals is encoded", cp.encode_channel_field( "a=b" ).indexOf( "=" ), -1 );
check( "comma is encoded", cp.encode_channel_field( "a,b" ).indexOf( "," ), -1 );
check( "plain name unchanged", cp.encode_channel_field( "admin" ), "admin" );
check( "round trip", cp.decode_channel_field( enc ), "tester-1" );
check( "empty encodes empty", cp.encode_channel_field( "" ), "" );
check( "malformed decodes empty", cp.decode_channel_field( "%E0%A4%A" ), "" );

heading( "saved account list" );

// NOTE: "cws.access" is shared with "test_web_session.html", so a malformed value breaks
// the harness as well. Ian hit exactly that - an emptied list stored as "" came back as
// one blank account, and the next PIN appended to it giving ",11111".
check( "no key yet", cp.parse_access_list( null ).length, 0 );
check( "empty string is no accounts", cp.parse_access_list( "" ).length, 0 );
check( "a single account", cp.parse_access_list( "11111" ).join( "," ), "11111" );

check( "leading blank dropped", cp.parse_access_list( ",11111" ).join( "," ), "11111" );
check( "trailing blank dropped", cp.parse_access_list( "11111," ).join( "," ), "11111" );
check( "interior blank dropped", cp.parse_access_list( "11111,,22222" ).join( "," ), "11111,22222" );
check( "surrounding space trimmed", cp.parse_access_list( " 11111 , 22222 " ).join( "," ), "11111,22222" );
check( "duplicates collapsed", cp.parse_access_list( "11111,11111" ).join( "," ), "11111" );

// NOTE: null means "remove the key" - storing "" is what caused the blank entry.
check( "emptied list removes the key", cp.format_access_list( [ ] ), null );
check( "list of blanks removes the key", cp.format_access_list( [ "" ] ), null );
check( "one account formats", cp.format_access_list( [ "11111" ] ), "11111" );
check( "accounts are sorted", cp.format_access_list( [ "22222", "11111" ] ), "11111,22222" );

// NOTE: Reading a corrupted value repairs it, so an already broken browser heals on the
// next write rather than needing the user to clear storage by hand.
check( "round trip repairs corruption",
 cp.format_access_list( cp.parse_access_list( ",11111" ) ), "11111" );

heading( "day labels" );

// NOTE: Fixed "now" of Wednesday 23 September 2026, so the labels below do not change
// with the day the tests are run. Uniques are built the same way the server does it -
// Unix seconds with a three digit counter appended.
var now = new Date( 2026, 8, 23, 12, 0, 0 );

function at( day )
{
   return String( Math.floor( new Date( 2026, 8, day, 10, 30, 0 ).getTime( ) / 1000 ) ) + "000";
}

check( "same day is Today", cp.day_label( at( 23 ), now ), "Today" );
check( "one day back is Yesterday", cp.day_label( at( 22 ), now ), "Yesterday" );
check( "two days back is a weekday", cp.day_label( at( 21 ), now ), "Monday" );
check( "six days back is still a weekday", cp.day_label( at( 17 ), now ), "Thursday" );

// NOTE: Seven days back repeats today's weekday, so the name would be ambiguous.
check( "seven days back gives a date", /September/.test( cp.day_label( at( 16 ), now ) ), true );
check( "seven days back drops the bare weekday",
 cp.day_label( at( 16 ), now ) === "Wednesday", false );

check( "a bad unique has no label", cp.day_label( "not-a-number", now ), "" );

// NOTE: Grouping must use the local calendar day, not UTC, or the divider lands on the
// wrong side of midnight for anyone well away from Greenwich.
check( "day key shape", /^\d{4}-\d\d-\d\d$/.test( cp.day_key( at( 21 ) ) ), true );
check( "day key of a bad unique", cp.day_key( "not-a-number" ), "" );

check( "same day shares a key", cp.day_key( at( 21 ) ) === cp.day_key( at( 21 ) ), true );
check( "different days differ", cp.day_key( at( 21 ) ) === cp.day_key( at( 22 ) ), false );

check( "days between counts calendar days",
 cp.days_between( new Date( 2026, 8, 22, 23, 59, 0 ), new Date( 2026, 8, 23, 0, 1, 0 ) ), 1 );

check( "full stamp mentions the year", /2026/.test( cp.unique_to_full( at( 21 ) ) ), true );
check( "full stamp of a bad unique", cp.unique_to_full( "not-a-number" ), "" );

console.log( "" );
console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

process.exit( failures === 0 ? 0 : 1 );

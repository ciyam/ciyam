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
const ce = require( "./chat_emoji.js" );

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
// NOTE: Ian's "extra=TIME" option (46797f3a) appends ".<time>" to each member - the chat asks
// for it for read markers since 2026-09-28. Names and counts must still read correctly.
check( "extra=TIME suffix tolerated", cp.parse_members( "admin+2.1790313275000 verify-a+0" ).map( function( m ) { return m.name + ":" + m.sessions; } ),
 [ "admin:2", "verify-a:0" ] );

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
check( "reserved room refused for a standard user",
 cp.posting_status( "0000001", open_entry, "bob", false ).can_post, false );
check( "reserved room allowed for admin",
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

check( "reserved room recognised", cp.is_reserved_room( "0000001" ), true );
check( "other room is not the reserved room", cp.is_reserved_room( "0000002" ), false );

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

heading( "pending invitations" );

// NOTE: As tester-1 sees Administration - its own invitations, two of them to the same room
// (the server accepts duplicates, ISS-017), one to a room since joined, and ordinary noise.
var inbox = cp.parse_fetch_response( [
 "admin+1 tester-1+0 damon+0",
 "1790259222001 admin :invite room 0000005-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Design Review",
 "1790259222002 damon :invite room 0000006-bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb Garden (Veg)",
 "1790259222003 admin :invite room 0000005-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Design Review 2",
 "1790259222004 admin :invite room 0000007-cccccccccccccccccccccccccccccccc Old Room",
 "1790259222005 tester-1 :joined",
 "1790259222006 admin hello everyone" ].join( "\n" ) ).messages;

var member_of = [ { room: "0000001" }, { room: "0000007" } ];

// NOTE: As the server now answers once an invitation has been declined - the original
// becomes ":ignore", with an edited marker, and the decline is posted as ":reject".
var answered = cp.parse_fetch_response( [
 "admin+1 verify-a+1",
 "1790370000001 admin* :ignore (invite for 0000002 was processed)",
 "1790370000002 verify-a :reject (invite for 0000002 was rejected)" ].join( "\n" ) ).messages;

check( "ignore names its room", [ answered[ 0 ].event.verb, answered[ 0 ].event.room, answered[ 0 ].sender ], [ "ignore", "0000002", "admin" ] );
check( "reject names its room", [ answered[ 1 ].event.verb, answered[ 1 ].event.room ], [ "reject", "0000002" ] );
check( "an answered invitation is not pending", cp.pending_invitations( answered, [ ] ).length, 0 );

var pending = cp.pending_invitations( inbox, member_of );

check( "one per room, joined rooms left out", pending.map( function( p ) { return p.room; } ), [ "0000005", "0000006" ] );
check( "newest first, latest details win", pending[ 0 ], {
 room: "0000005", token: "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", name: "Design Review 2", inviter: "admin", unique: "1790259222003" } );
check( "inviter is the sender", pending[ 1 ].inviter, "damon" );
check( "a name with brackets kept", pending[ 1 ].name, "Garden (Veg)" );
check( "joining clears it", cp.pending_invitations( inbox, member_of.concat( [ { room: "0000005" }, { room: "0000006" } ] ) ).length, 0 );
check( "no messages, none pending", cp.pending_invitations( [ ], member_of ).length, 0 );

var odd = cp.parse_fetch_response( [
 "admin+1",
 "1790259222010 admin :invite room 0000001-dddddddddddddddddddddddddddddddd Administration",
 "1790259222011 admin :invite room 0000008" ].join( "\n" ) ).messages;

check( "never the reserved room, never without a token", cp.pending_invitations( odd, [ ] ).length, 0 );

// NOTE: Real lines, 2026-09-28 - since Ian's change that day the server no longer rewrites an
// answered invitation as ":ignore" (ISS-028), so the original ":invite" stays beside the decline.
var declined_inbox = cp.parse_fetch_response( [
 "admin+1 verify-a+1",
 "1790595024000 verify-a :invite room 0000010-eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee Admin Declines 3602",
 "1790595028000 admin :reject (invite for 0000010 was rejected)",
 "1790595030000 tester-1 :reject (invite for 0000011 was rejected)",
 "1790595031000 verify-a :invite room 0000011-ffffffffffffffffffffffffffffffff Shared Room" ].join( "\n" ) ).messages;

check( "the rooms this user declined", cp.declined_rooms( declined_inbox, "admin" ), { "0000010": true } );
check( "someone else's decline is not this user's", cp.declined_rooms( declined_inbox, "verify-e" ), { } );
check( "a declined invitation is not pending, with no :ignore", cp.pending_invitations( declined_inbox, [ ], "admin" ).map( function( p ) { return p.room; } ), [ "0000011" ] );
check( "another's decline leaves this user's invitation", cp.pending_invitations( declined_inbox, [ ], "admin" ).some( function( p ) { return p.room === "0000011"; } ), true );
check( "without 'me', declines are not counted", cp.pending_invitations( declined_inbox, [ ] ).length, 2 );

heading( "line breaks and backslashes" );

// NOTE: The lines the container returned on 2026-09-26 for messages sent as "one<break>two",
// "double\\slash" (one backslash stored), "bs then nl\<break>next", and "ends with\\"
// (one backslash stored, so it comes back doubled and must not join the next line).
var multi = cp.parse_fetch_response( [
 "admin+1",
 "1790426390001 admin  one\\",
 "two",
 "1790426391000 admin  double\\\\slash",
 "1790426392000 admin  bs then nl\\\\\\",
 "next",
 "1790426393000 admin  ends with\\\\",
 "1790426394000 admin  after" ].join( "\n" ) ).messages;

check( "five messages, none lost to a break", multi.length, 5 );
check( "a line break comes back as one", multi[ 0 ].text, "one\ntwo" );
check( "a stored backslash comes back single", multi[ 1 ].text, "double\\slash" );
check( "backslash then break", multi[ 2 ].text, "bs then nl\\\nnext" );
check( "an ending backslash does not swallow the next message", [ multi[ 3 ].text, multi[ 4 ].text ], [ "ends with\\", "after" ] );

check( "sending doubles a backslash", cp.escape_message_text( "a\\b \\\\ c" ), "a\\\\b \\\\\\\\ c" );
check( "sending leaves breaks alone", cp.escape_message_text( "one\ntwo" ), "one\ntwo" );
check( "sending keeps a ';' - the server splits on it otherwise", cp.escape_message_text( "a; b;c" ), "a\\; b\\;c" );
check( "after sending, a computer keeps the focus in the message box", cp.keeps_focus_after_send( false ), true );
check( "a phone lets it go, so its keyboard goes away", cp.keeps_focus_after_send( true ), false );
check( "a round trip keeps what was typed", cp.unescape_message_text( cp.escape_message_text( "C:\\temp\\x" ) ), "C:\\temp\\x" );

// NOTE: A carriage return marks a message's kind at its start (Ian, 2026-10-20) - the chat never sends one.
check( "sending turns a CR, or a CR and line feed, into a line feed", cp.escape_message_text( "one\r\ntwo\rthree\nfour" ), "one\ntwo\nthree\nfour" );
check( "- so nothing typed can start 'X\\r'", cp.message_prefix( cp.escape_message_text( "X\r1234" ) ), "" );
check( "a message's kind from its start - a letter and a CR", [ cp.message_prefix( "X\rab12" ), cp.message_prefix( "Q\rzz" ),
 cp.message_prefix( "X ab12" ), cp.message_prefix( "XY\rab" ), cp.message_prefix( "" ) ], [ "X", "Q", "", "", "" ] );
check( "an encrypted message said so, not shown as its ciphertext", [ cp.message_unreadable_label( "X\r9f3c" ),
 cp.message_unreadable_label( "Q\rzz" ), cp.message_unreadable_label( "hello" ) ],
 [ "Encrypted message - this chat cannot read it yet", "A message of a kind this chat cannot show yet", "" ] );
check( "- and in the plain places, the pinned line and Home", [ cp.message_plain_text( "X\r9f3c" ),
 cp.message_plain_text( "\u001b[31mred\u001b[0m words" ) ], [ "Encrypted message - this chat cannot read it yet", "red words" ] );

heading( "message size" );

check( "letters are a byte each", cp.message_bytes( "hello" ), 5 );
check( "an accented letter is two", cp.message_bytes( "é" ), 2 );
check( "an emoji is four", cp.message_bytes( "😀" ), 4 );
check( "a line break is one", cp.message_bytes( "a\nb" ), 3 );
check( "the limit is 2200 bytes", cp.c_max_message_bytes, 2200 );
check( "under what the server takes from the longest name", cp.c_max_message_bytes <= 2233 - ( 12 - 5 ), true );

heading( "room events in plain words" );

function said( line, label ) { return cp.describe_event( cp.parse_system_event( line ), label ); }

function known( room ) { return ( room === "0000005" ) ? "Design Review (#0000005)" : "#" + room; }

check( "joined", said( ":joined" ), "joined" );
check( "left", said( ":remove" ), "left the room" );
check( "created", said( ":create 0000004-abc Design Review" ), "created Design Review (#0000004)" );
check( "invited you", said( ":invite room 0000005-aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa Design Review" ), "invited you to Design Review (#0000005)" );
check( "renamed", said( ":rename 'Old' to 'New'" ), "renamed the room from 'Old' to 'New'" );
check( "handed over", said( ":assign 'admin' to 'tester-1'" ), "handed the room from 'admin' to 'tester-1'" );
check( "posting set", said( ":allows set to own" ), "set who may post to the owner only" );
check( "posting locked", said( ":allows set to none" ), "set who may post to nobody - locked" );
check( "invitation sent", said( ":issued (invite for 0000005 sent to tester-1,tester-2)", known ), "invited tester-1, tester-2 to Design Review (#0000005)" );
check( "private message sent", said( ":issued (message sent to verify-a)" ), "sent a private message to verify-a" );
check( "answered", said( ":ignore (invite for 0000005 was processed)", known ), "invited you to Design Review (#0000005) - already answered" );
check( "declined", said( ":reject (invite for 0000004 was rejected)" ), "declined the invitation to #0000004" );
check( "an unknown verb keeps its words, without the colon", said( ":frobbed the widget" ), "frobbed the widget" );

heading( "rooms shown in the rail" );

var listed = [ { room: "0000001" }, { room: "0000004" }, { room: "0000005" } ];

check( "admin sees Administration", cp.visible_rooms( listed, true ).map( function( r ) { return r.room; } ), [ "0000001", "0000004", "0000005" ] );
check( "others do not", cp.visible_rooms( listed, false ).map( function( r ) { return r.room; } ), [ "0000004", "0000005" ] );
check( "the list itself is untouched", listed.length, 3 );

heading( "time-outs" );

// NOTE: The server's own words, seen on the container 2026-09-27.
check( "a time-out is recognised", cp.is_timeout_response( "Error: Timed out waiting for web session response." ), true );
check( "another error is not one", cp.is_timeout_response( "Error: Unknown room '0000009' (or not joined)." ), false );
check( "an answer is not one", cp.is_timeout_response( "admin+1\n1790520000000 admin  hello" ), false );
check( "nothing is not one", cp.is_timeout_response( null ), false );
check( "three in a row ends the session", cp.c_max_timeouts_in_row, 3 );

var after_rows = [
   { unique: "1790520000000", sender: "admin", text: "hello" },
   { unique: "1790520005000", sender: "admin", text: "did it arrive" },
   { unique: "1790520006000", sender: "verify-a", text: "same words" } ];

check( "sent after all - newer than the start point", cp.arrived_after_timeout( after_rows, { me: "admin", text: "did it arrive", after: "1790520001000" } ), true );
check( "the same words from before the send do not count", cp.arrived_after_timeout( after_rows, { me: "admin", text: "hello", after: "1790520001000" } ), false );
check( "someone else's words do not count", cp.arrived_after_timeout( after_rows, { me: "admin", text: "same words", after: "1790520001000" } ), false );
check( "not there - not sent", cp.arrived_after_timeout( after_rows, { me: "admin", text: "never arrived", after: "1790520001000" } ), false );
check( "an edit arrived if its message now reads so", cp.arrived_after_timeout( after_rows, { me: "admin", text: "hello", edit: "1790520000000" } ), true );
check( "an edit that did not take", cp.arrived_after_timeout( after_rows, { me: "admin", text: "hello again", edit: "1790520000000" } ), false );
check( "no start point yet - anything of mine counts", cp.arrived_after_timeout( after_rows, { me: "admin", text: "did it arrive", after: "" } ), true );

heading( "read markers" );

// NOTE: A real member line with "extra=TIME", 2026-09-28.
var readers = cp.parse_members( "admin+1.1790521522001 tester-1+1.1790521522001 verify-a+0.1790520461001 verify-e+0" );

check( "the read point is kept", readers.map( function( m ) { return m.name + ":" + ( m.read || "-" ); } ), [ "admin:1790521522001", "tester-1:1790521522001", "verify-a:1790520461001", "verify-e:-" ] );
check( "the session count is still right", readers.map( function( m ) { return m.sessions; } ), [ 1, 1, 0, 0 ] );
check( "a line without it has none", cp.parse_members( "admin+1 verify-a+0" )[ 0 ].read, undefined );

var in_room = [ "1790520461000", "1790520462000", "1790521522000" ];

check( "each marker under the last message before the read point",
 cp.seen_by( in_room, readers, "admin" ), { "1790521522000": [ "tester-1" ], "1790520461000": [ "verify-a" ] } );
check( "the user's own is left out", Object.keys( cp.seen_by( in_room, readers, "tester-1" ) ).sort( ).map( function( k ) { return k + ":" + cp.seen_by( in_room, readers, "tester-1" )[ k ].join( "," ); } ), [ "1790520461000:verify-a", "1790521522000:admin" ] );
// NOTE: Since 2026-09-30 a read point is the unique of the last message read, not a later time -
// so a member who has read exactly the last message is shown under it.
check( "a read point equal to a message's unique has read that message",
 cp.seen_by( in_room, [ { name: "bob", read: "1790521522000" } ], "admin" ), { "1790521522000": [ "bob" ] } );
check( "a read point before every message shows nothing", cp.seen_by( in_room, [ { name: "x", read: "1790000000000" } ], "admin" ), { } );
check( "names under one message are sorted", cp.seen_by( in_room, [ { name: "zed", read: "1790530000000" }, { name: "amy", read: "1790530000000" } ], "admin" ), { "1790521522000": [ "amy", "zed" ] } );

heading( "reading only what is new" );

var held = [ { unique: "1", sender: "admin", text: "one" }, { unique: "2", sender: "admin", text: "two" } ];

check( "new ones are added after", cp.merge_new_messages( held, [ { unique: "3", sender: "admin", text: "three" } ] ).map( function( m ) { return m.unique; } ), [ "1", "2", "3" ] );
check( "one already held is not taken twice", cp.merge_new_messages( held, [ { unique: "2", sender: "admin", text: "two" } ] ).length, 2 );
check( "same unique, different entry - both kept", cp.merge_new_messages( held, [ { unique: "2", sender: "admin", text: "(issued)" } ] ).length, 3 );
check( "nothing held yet", cp.merge_new_messages( null, [ { unique: "1", sender: "admin", text: "one" } ] ).length, 1 );
check( "nothing new", cp.merge_new_messages( held, [ ] ), held );

var fresh_lines = function( lines ) { return cp.parse_fetch_response( [ "admin+1 verify-a+1" ].concat( lines ).join( "\n" ) ).messages; };

check( "an announcement takes the cheap read", cp.needs_full_read( fresh_lines( [ "1790520000000 admin  Maintenance on Sunday" ] ) ), false );
check( "nothing came back - read it whole", cp.needs_full_read( [ ] ), true );
check( "an invitation - read it whole", cp.needs_full_read( fresh_lines( [ "1790520000001 admin :invite room 0000006-7e8dc218f33200c07e43ead73911af9a Common room" ] ) ), true );
check( "an answered one - read it whole", cp.needs_full_read( fresh_lines( [ "1790520000002 admin :ignore (invite for 0000006 was processed)" ] ) ), true );
check( "a decline - read it whole", cp.needs_full_read( fresh_lines( [ "1790520000003 verify-a :reject (invite for 0000006 was rejected)" ] ) ), true );
check( "a join - read it whole", cp.needs_full_read( fresh_lines( [ "1790520000004 tester-1 :joined" ] ) ), true );

heading( "changing who may post" );

// NOTE: Verified 2026-09-28 - "posts=own" is refused, "posts=OWN" is "[okay]".
check( "the server's value is in capitals", [ cp.posts_request_value( "any" ), cp.posts_request_value( "own" ), cp.posts_request_value( "none" ) ], [ "ANY", "OWN", "NONE" ] );
check( "anything else is nothing", [ cp.posts_request_value( "all" ), cp.posts_request_value( "" ), cp.posts_request_value( null ) ], [ "", "", "" ] );

var owned = { room: "0000004", owner: "verify-a", posts: "any" };

check( "the owner may change it", cp.can_change_posting( "0000004", owned, "verify-a", false ), true );
check( "admin may", cp.can_change_posting( "0000004", owned, "admin", true ), true );
check( "another member may not", cp.can_change_posting( "0000004", owned, "verify-e", false ), false );
// NOTE: Leaving (Ian, 2026-10-12).
check( "a member may leave a room", cp.can_leave_room( "0000004", { room: "0000004", owner: "admin" }, "verify-a" ), true );
check( "- General too, now", cp.can_leave_room( "0000002", { room: "0000002", owner: "admin" }, "verify-a" ), true );
check( "- not one they own", cp.can_leave_room( "0000004", { room: "0000004", owner: "verify-a" }, "verify-a" ), false );
check( "- never the Admin room, nor the entrance", [ cp.can_leave_room( "0000001", { room: "0000001", owner: "admin" }, "verify-a" ),
 cp.can_leave_room( "0000000", { room: "0000000", owner: "" }, "verify-a" ) ], [ false, false ] );
check( "- not one the listing does not have yet", cp.can_leave_room( "0000004", null, "verify-a" ), false );

check( "nobody for Administration", cp.can_change_posting( "0000001", { room: "0000001", owner: "admin" }, "admin", true ), false );
check( "nobody for the entrance", cp.can_change_posting( "0000000", { room: "0000000", owner: "admin" }, "admin", true ), false );
check( "nothing known, nothing changed", cp.can_change_posting( "0000004", null, "verify-a", false ), false );

heading( "emoji" );

var catalogue = ce.emoji_catalogue( );

var every = [ ].concat.apply( [ ], catalogue.map( function( c ) { return c.items; } ) );

function chars( list ) { return list.map( function( i ) { return i.char; } ); }

check( "eight categories, none empty", [ catalogue.length, catalogue.every( function( c ) { return c.items.length > 0; } ) ], [ 8, true ] );
check( "each entry has a name", every.every( function( i ) { return i.name !== ""; } ), true );
check( "no emoji is listed twice", every.length === new Set( chars( every ) ).size, true );
check( "none is longer than 16 bytes", every.every( function( i ) { return Buffer.byteLength( i.char, "utf8" ) <= 16; } ), true );
check( "at least 500 in all", every.length >= 500, true );
check( "code points become the character", ce.emoji_from_codes( "1F600" ), "😀" );
check( "with a variation selector", ce.emoji_from_codes( "2764 FE0F" ), "❤️" );
check( "a keycap", ce.emoji_from_codes( "0031 FE0F 20E3" ), "1️⃣" );

check( "search by the start of a word", chars( ce.search_emoji( catalogue, "thumb" ) ), [ ce.emoji_from_codes( "1F44D" ), ce.emoji_from_codes( "1F44E" ) ] );
check( "every word must match", chars( ce.search_emoji( catalogue, "heart red" ) ), [ ce.emoji_from_codes( "2764 FE0F" ) ] );
check( "extra words count", chars( ce.search_emoji( catalogue, "lol" ) ).indexOf( ce.emoji_from_codes( "1F602" ) ) >= 0, true );
check( "a name match comes before a keyword match", chars( ce.search_emoji( catalogue, "sun" ) )[ 0 ], ce.emoji_from_codes( "1F31E" ) );
check( "hyphenated names split", chars( ce.search_emoji( catalogue, "eyes heart" ) ), [ ce.emoji_from_codes( "1F60D" ) ] );
check( "nothing typed, nothing found", ce.search_emoji( catalogue, "  " ), [ ] );
check( "no match", ce.search_emoji( catalogue, "xyzzy" ), [ ] );

check( "autocomplete: a colon and two letters", ce.emoji_query_at( "nice :th", 8 ), { start: 5, query: "th" } );
check( "autocomplete: at the very start", ce.emoji_query_at( ":thumbs_up", 10 ), { start: 0, query: "thumbs_up" } );
check( "autocomplete: only up to the cursor", ce.emoji_query_at( "a :thu and more", 6 ), { start: 2, query: "thu" } );
check( "autocomplete: one letter is not enough", ce.emoji_query_at( "ok :p", 5 ), null );
check( "autocomplete: not a time", ce.emoji_query_at( "at 10:30", 8 ), null );
check( "autocomplete: not a link", ce.emoji_query_at( "see http://x", 12 ), null );
check( "autocomplete: not a smiley", ce.emoji_query_at( "hi :)", 5 ), null );
check( "autocomplete: not once a space is typed", ce.emoji_query_at( "a :thumbs up", 12 ), null );
check( "autocomplete: not glued to a word", ce.emoji_query_at( "word:thu", 8 ), null );
check( "autocomplete: after a line break", ce.emoji_query_at( "one\n:tad", 8 ), { start: 4, query: "tad" } );
check( "shortcode from a name", ce.emoji_shortcode( "thumbs up" ), ":thumbs_up:" );
check( "shortcode without the punctuation", ce.emoji_shortcode( "OK hand" ), ":ok_hand:" );
check( "shortcode keeps + and -", ce.emoji_shortcode( "1st place medal" ), ":1st_place_medal:" );
check( "suggestions: _ for a space", chars( ce.suggest_emoji( catalogue, "thumbs_up" ) ), [ ce.emoji_from_codes( "1F44D" ) ] );
check( "suggestions: at most eight", ce.suggest_emoji( catalogue, "heart" ).length, 8 );
check( "suggestions: as many as asked", ce.suggest_emoji( catalogue, "heart", 3 ).length, 3 );

check( "recent: newest first", ce.push_recent_emoji( [ "a", "b" ], "c" ), [ "c", "a", "b" ] );
check( "recent: once each", ce.push_recent_emoji( [ "a", "b", "c" ], "b" ), [ "b", "a", "c" ] );
check( "recent: capped", ce.push_recent_emoji( Array.from( { length: ce.c_emoji_recent_max }, function( _, i ) { return "e" + i; } ), "new" ).length, ce.c_emoji_recent_max );
check( "recent: read back", ce.parse_recent_emoji( "[\"\\ud83d\\ude00\",\"x\"]" ), [ "😀", "x" ] );
check( "recent: damaged is empty", ce.parse_recent_emoji( "{nope" ), [ ] );
check( "recent: stray entries dropped", ce.parse_recent_emoji( "[1,\"\",\"ok\"]" ), [ "ok" ] );

heading( "private messages" );

// NOTE: Real lines from the container, 2026-09-27: admin sent one message to the room, one to
// verify-a, and one to verify-a with admin's own name in "for" to keep a copy.
var sent_side = cp.parse_fetch_response( [
   "admin+1 verify-a+0 verify-e+0",
   "1790514104000 admin  public 3493",
   "1790514104001 admin :issued (message sent to verify-a)",
   "1790514104002 admin !private-to-a-and-me 3493",
   "1790514104002 admin :issued (message sent to verify-a,admin)"
].join( "\n" ) ).messages;

var received_side = cp.parse_fetch_response( [
   "admin+1 verify-a+0 verify-e+0",
   "1790514104000 admin  public 3493",
   "1790514104001 admin !private-to-a 3493",
   "1790514104002 admin !private-to-a-and-me 3493"
].join( "\n" ) ).messages;

check( "a public message is not private", [ received_side[ 0 ].private, received_side[ 0 ].text ], [ false, "public 3493" ] );
check( "a private one is, without its marker", [ received_side[ 1 ].private, received_side[ 1 ].text ], [ true, "private-to-a 3493" ] );
check( "the sender's copy and its receipt become one", sent_side.map( function( m ) { return m.kind + ( m.private ? "!" : "" ); } ), [ "chat", "system", "chat!" ] );
check( "the copy carries who it went to, less the sender", sent_side[ 2 ].recipients, [ "verify-a" ] );
check( "a receipt with no copy stays a notice", sent_side[ 1 ].event.recipients, [ "verify-a" ] );
// NOTE: Real, 2026-09-28 - verify-a sent a public message and edited it; the server now marks
// every edit with "!" (ISS-026). Fixed by Ian on 2026-09-28 - an edit keeps the original's
// prefix, so "!" on an edit is true again. Real lines after that fix:
var edited_public = cp.parse_message_line( "1790594242000 admin*  pub edited 1161" );
var edited_private = cp.parse_message_line( "1790594243000 admin* !priv edited 1161" );

check( "an edited public message is public", [ edited_public.edited, edited_public.private, edited_public.text ], [ true, false, "pub edited 1161" ] );
check( "the message box, to everyone", cp.composer_mode( [ ], false, false ), { is_private: false, label: "", send: "Send", send_short: "Send" } );
check( "the message box, to named people", cp.composer_mode( [ "verify-a" ], false, false ), { is_private: true, label: "Private to", send: "Send privately", send_short: "Send" } );
check( "the message box, editing a public message", cp.composer_mode( [ ], true, false ), { is_private: false, label: "", send: "Save edit", send_short: "Save edit" } );
check( "the message box, editing a private one", cp.composer_mode( [ ], true, true ), { is_private: true, label: "Editing a private message", send: "Save edit", send_short: "Save edit" } );
check( "an edit decides, whoever is picked", cp.composer_mode( [ "verify-a" ], true, false ).is_private, false );
check( "a public message is edited as for=<unique>", cp.edit_for_value( "1790594242000", false ), "1790594242000" );
check( "a private one as for=!<unique>", cp.edit_for_value( "1790594243000", true ), "!1790594243000" );
check( "an edited private message stays private", [ edited_private.edited, edited_private.private, edited_private.text ], [ true, true, "priv edited 1161" ] );
check( "an edited one in the old form is unchanged", cp.parse_message_line( "1790520461000 verify-a*  edit probe" ).text, "edit probe" );
check( "a message starting ! in the old format is not private", cp.parse_message_line( "1790514104000 admin  !not private" ).private, false );
check( "and keeps its !", cp.parse_message_line( "1790514104000 admin  !not private" ).text, "!not private" );

check( "label on a received one", cp.private_label( received_side[ 1 ] ), { text: "private", title: "Private - sent to you, not to the whole room" } );
check( "label on a sent copy", cp.private_label( sent_side[ 2 ] ), { text: "private · to verify-a", title: "Private - only you and verify-a can see this" } );
check( "the sender is added to for", cp.with_sender( [ "verify-a" ], "admin" ), [ "verify-a", "admin" ] );
check( "only once", cp.with_sender( [ "verify-a", "admin" ], "admin" ), [ "verify-a", "admin" ] );
check( "not when unknown", cp.with_sender( [ "verify-a" ], "" ), [ "verify-a" ] );

heading( "announcements" );

// NOTE: As the background read of Administration returns them to a standard account.
var admin_room = cp.parse_fetch_response( [
   "admin+1 verify-e+1",
   "1790435700000 admin  Announcement",
   "1790435793000 admin  Announcement 2\\",
   "Welcome to the new chat!",
   "1790436690000 tester-1 :joined",
   "1790436716001 admin :invite room 0000006-7e8dc218f33200c07e43ead73911af9a Common room",
   "1790436800000 tester-1  not from admin",
   "1790436900000 admin*  Edited announcement"
].join( "\n" ) ).messages;

function ids( list ) { return list.map( function( m ) { return m.unique; } ); }

check( "admin's messages, newest first", ids( cp.pending_announcements( admin_room, [ ] ) ), [ "1790436900000", "1790435793000", "1790435700000" ] );
check( "a multi-line one keeps its lines", cp.pending_announcements( admin_room, [ ] )[ 1 ].text, "Announcement 2\nWelcome to the new chat!" );
check( "an edited one is still admin's", cp.pending_announcements( admin_room, [ ] )[ 0 ].text, "Edited announcement" );
check( "dismissed ones are left out", ids( cp.pending_announcements( admin_room, [ "1790435793000" ] ) ), [ "1790436900000", "1790435700000" ] );
check( "nothing read, nothing shown", cp.pending_announcements( null, null ), [ ] );

var five = [ "e", "d", "c", "b", "a" ].map( function( u ) { return { unique: u }; } );

function uniques( list ) { return list.map( function( m ) { return m.unique; } ); }

check( "a stack of five shows the newest two", [ uniques( cp.announcement_stack( five, false ).shown ), cp.announcement_stack( five, false ).more ], [ [ "e", "d" ], 3 ] );
check( "expanded, all of them", [ cp.announcement_stack( five, true ).shown.length, cp.announcement_stack( five, true ).more ], [ 5, 0 ] );
check( "two or fewer are never held back", [ cp.announcement_stack( five.slice( 0, 2 ), false ).more, cp.announcement_stack( [ ], false ).shown.length ], [ 0, 0 ] );
check( "three is collapsed", cp.announcement_stack( five.slice( 0, 3 ), false ).more, 1 );

check( "preview audience: everyone", cp.announcement_audience( [ ], "admin" ), "Everyone sees this at the top of every room until they dismiss it" );
check( "preview audience: named people", cp.announcement_audience( [ "verify-a", "tester-1" ], "admin" ), "Only verify-a, tester-1 will see this, marked as to them" );
check( "preview audience: admin's own copy left out", cp.announcement_audience( [ "verify-a", "admin" ], "admin" ), "Only verify-a will see this, marked as to them" );

check( "dismissed list read back", cp.parse_dismissed( "[\"1790435700000\",\"1790435793000\"]" ), [ "1790435700000", "1790435793000" ] );
check( "a damaged value is nothing dismissed", cp.parse_dismissed( "{oops" ), [ ] );
check( "nothing stored is nothing dismissed", cp.parse_dismissed( null ), [ ] );
check( "stray entries are dropped", cp.parse_dismissed( "[\"123\",42,\"x\"]" ), [ "123" ] );
check( "dismissing adds the id once", cp.add_dismissed( [ "1", "2" ], "2" ), [ "1", "2" ] );
check( "and keeps the newest", cp.add_dismissed( Array.from( { length: cp.c_max_dismissed }, function( _, i ) { return String( i ); } ), "new" ).slice( -2 ), [ String( cp.c_max_dismissed - 1 ), "new" ] );
check( "the list stays capped", cp.add_dismissed( Array.from( { length: cp.c_max_dismissed }, function( _, i ) { return String( i ); } ), "new" ).length, cp.c_max_dismissed );

heading( "unread while the rail is out of sight" );

var waiting = [ { room: "0000001", unread: 4 }, { room: "0000004", unread: 2 }, { room: "0000005", unread: 3 }, { room: "0000006", unread: 0 } ];
var invited = [ { room: "0000009" } ];

check( "the open room is not counted", cp.unread_elsewhere( waiting, [ ], "0000004", false ), 3 );
check( "each invitation counts one", cp.unread_elsewhere( waiting, invited, "0000004", false ), 4 );
check( "Administration counts for admin only", cp.unread_elsewhere( waiting, invited, "0000004", true ), 8 );
check( "nothing open counts every room", cp.unread_elsewhere( waiting, [ ], "", false ), 5 );
check( "no rooms, no count", cp.unread_elsewhere( null, null, "", false ), 0 );
check( "badge for none", cp.badge_text( 0 ), "" );
check( "badge for some", cp.badge_text( 7 ), "7" );
check( "badge capped", cp.badge_text( 140 ), "99+" );
check( "a page's title - with what waits", cp.page_title( "CIYAM Chat", 3 ), "(3) CIYAM Chat" );
check( "a page's title - nothing waiting, the title alone", cp.page_title( "CIYAM Chat", 0 ), "CIYAM Chat" );
check( "a page's title - capped as the badge is", cp.page_title( "CIYAM Home", 250 ), "(99+) CIYAM Home" );

heading( "signing in - what it is doing" );

// NOTE: Ian, 2026-10-25 - the password is hardened before it is sent, and a refusal tried once more the other way after a gap,
// so the sign in says what it is doing rather than seem stuck.
check( "securing the password - how far, for the bar", [ cp.sign_in_progress( "securing", 0 ), cp.sign_in_progress( "securing", 0.456 ),
 cp.sign_in_progress( "securing", 2 ) ], [ { text: "Securing your password… 0%", fraction: 0 }, { text: "Securing your password… 46%", fraction: 0.456 },
 { text: "Securing your password… 100%", fraction: 1 } ] );
check( "- waiting to try the other way, counted down; updating; and otherwise contacting", [ cp.sign_in_progress( "waiting", 3 ).text,
 cp.sign_in_progress( "waiting", 1 ).text, cp.sign_in_progress( "updating" ).text, cp.sign_in_progress( null ).text,
 cp.sign_in_progress( "waiting", 3 ).fraction ], [ "Checking it another way in 3 seconds…", "Checking it another way in 1 second…",
 "Updating how your password is kept…", "Contacting the server…", null ] );

heading( "password strength" );

// NOTE: The same scores "test_bip39.html" gives - length times a multiplier for variety.
function rated( p ) { return cp.password_strength( p ).text; }

check( "empty says nothing", cp.password_strength( "" ), { level: -1, text: "" } );
check( "under seven is unsatisfactory", cp.password_strength( "abc12!" ), { level: 0, text: "Unsatisfactory" } );
check( "seven digits is weak", rated( "1234567" ), "Weak" );
check( "twelve lower case is moderate", rated( "abcdefghijkl" ), "Moderate" );
check( "digits and lower, eight long, is moderate", rated( "abcd1234" ), "Moderate" );
check( "three kinds with digits, ten long, is strong", rated( "Abcdefg123" ), "Strong" );
check( "all four kinds, twelve long, is very strong", rated( "Abcdef12!@#x" ), "Very Strong" );
check( "boundary: 7 x 7 = 49 is strong", rated( "Ab1!xyz" ), "Strong" );

// NOTE: With "zxcvbn.min.js" read in (Ian, 2026-10-23) - rated by the guesses it would take, banded every hundredfold from
// 10^8, zxcvbn's warning after; the account's own PIN and name counted as easily guessed. Read in here as a page would.
global.zxcvbn = require( "./zxcvbn.min.js" );

check( "by guesses - the predictable refused, however many kinds of character", [ rated( "Password1!" ), rated( "P@ssw0rd123" ),
 rated( "qwertyuiop" ), rated( "Summer2024!" ) ], [ "Too easy to guess - This is similar to a commonly used password",
 "Too easy to guess - This is similar to a commonly used password", "Too easy to guess - This is a top-100 common password",
 "Too easy to guess - This is similar to a commonly used password" ] );
check( "- and the rest by how many", [ rated( "new-pass-2" ), rated( "correct-horse-1" ), rated( "granite-orchid-42" ),
 rated( "taxi-act-describe-attitude-focus" ), cp.password_strength( "Password1!" ).level, cp.password_strength( "new-pass-2" ).level ],
 [ "Weak", "Moderate", "Strong", "Very Strong", 0, 1 ] );
check( "- the account's own PIN or name in it counts for little", [ cp.password_strength( "verify-a-20401" ).level,
 cp.password_strength( "verify-a-20401", [ "20401", "verify-a" ] ).level < cp.password_strength( "verify-a-20401" ).level ], [ 2, true ] );
check( "- under seven still unsatisfactory, before it is asked", cp.password_strength( "abc12!" ), { level: 0, text: "Unsatisfactory" } );

check( "- why one cannot be saved: too short, or too easy to guess", [ cp.password_refusal( cp.password_strength( "abc12!" ) ),
 cp.password_refusal( cp.password_strength( "Password1!" ) ) ], [ "Choose a password of at least 7 characters.",
 "That password is too easy to guess - choose a longer one, or Suggest one." ] );

delete global.zxcvbn;

check( "- not read in: the rules as before", rated( "Password1!" ), "Strong" );

// NOTE: A passphrase offered (Ian, 2026-10-23) - five of the 2048 BIP39 words, eleven bits each of a random 16-bit value.
var word_list = Array.from( { length: 2048 }, function( v, i ) { return "w" + i; } );

check( "a suggested passphrase - five words, eleven bits each", [ cp.suggested_passphrase( word_list, [ 0, 2047, 2048, 65535, 1234 ] ),
 cp.suggested_passphrase( word_list, [ 1, 2, 3 ], 3 ), cp.suggested_passphrase( [ "too", "few" ], [ 0, 1, 2, 3, 4 ] ) ],
 [ "w0-w2047-w0-w2047-w1234", "w1-w2-w3", "" ] );

heading( "user initial" );

check( "first letter, upper case", cp.user_initial( "verify-a" ), "V" );
check( "admin", cp.user_initial( "admin" ), "A" );
check( "a PIN", cp.user_initial( "20401" ), "2" );
check( "nothing", cp.user_initial( "" ), "?" );

heading( "direct messages" );

check( "named from the sorted people", cp.dm_room_name( [ "verify-a", "admin" ] ), "Private (admin + verify-a)" );
check( "the same whoever starts it", cp.dm_room_name( [ "admin", "verify-a" ] ), cp.dm_room_name( [ "verify-a", "admin" ] ) );
check( "a group", cp.dm_room_name( [ "carol", "admin", "bob" ] ), "Private (admin + bob + carol)" );
check( "names cleaned and not repeated", cp.dm_room_name( [ " Bob ", "bob", "admin" ] ), "Private (admin + bob)" );
check( "a valid room name", cp.is_valid_room_name( cp.dm_room_name( [ "twelve-chars", "another-name" ] ) ), true );
check( "three short names still fit", cp.dm_room_name( [ "amy", "bob", "cat" ] ), "Private (amy + bob + cat)" );
// NOTE: A group whose names will not fit is not a direct message - a name made from a hash could
// not be checked, and anyone could make one (found by review, 2026-10-01).
var big = cp.dm_room_name( [ "twelve-chars", "another-name", "third-person", "fourth-one" ] );
check( "too many names to fit: no conversation name", big, "" );
check( "read back: the people", cp.parse_dm_name( "Private (admin + verify-a)" ), { people: [ "admin", "verify-a" ], count: 2, server: false } );
check( "read back: the first form still", cp.parse_dm_name( "DM admin + verify-a" ), { people: [ "admin", "verify-a" ], count: 2, server: false } );
check( "a hashed name is not a conversation", [ cp.is_dm_name( "Private (group of 4 0a1b2c3d)" ), cp.is_dm_name( "DM 0a1b2c3d (4)" ) ], [ false, false ] );
check( "one conversation whichever form it is named in", cp.dm_key( "DM admin + verify-a" ), cp.dm_key( "Private (verify-a + admin)" ) );
check( "an ordinary room has no key", cp.dm_key( "Private (test-1 and test-2)" ), null );
check( "not a direct message: an ordinary room", cp.is_dm_name( "Ledger Ops" ), false );
check( "not a direct message: one person", cp.is_dm_name( "DM admin" ), false );
check( "not a direct message: a room called Private with other words", cp.is_dm_name( "Private (Planning)" ), false );
check( "not a direct message: not usernames", cp.is_dm_name( "DM Planning + Review" ), false );
check( "shown as the other person", cp.dm_title( "Private (admin + verify-a)", "admin" ), "verify-a" );
check( "shown as the others", cp.dm_title( "DM admin + bob + carol", "bob" ), "admin, carol" );
check( "an ordinary room keeps its name", cp.dm_title( "Ledger Ops", "admin" ), "Ledger Ops" );
var dm_rooms = [
 { room: "0000009", name: "DM admin + bob", owner: "bob" },
 { room: "0000004", name: "DM admin + bob", owner: "admin" },
 { room: "0000003", name: "DM admin + bob", owner: "carol" },
 { room: "0000005", name: "DM admin + carol", owner: "admin" },
 { room: "0000006", name: "Ledger Ops", owner: "admin" } ];
check( "finds the conversation - the lowest numbered of two", cp.find_dm_room( dm_rooms, [ "bob", "admin" ] ).room, "0000004" );
check( "not one owned by someone it does not name", cp.find_dm_room( dm_rooms, [ "bob", "admin" ] ).room !== "0000003", true );
check( "none yet", cp.find_dm_room( dm_rooms, [ "admin", "dave" ] ), null );
check( "found by its people, in the new form too",
 cp.find_dm_room( dm_rooms.concat( [ { room: "0000007", name: "Private (admin + dave)", owner: "dave" } ] ), [ "dave", "admin" ] ).room, "0000007" );
check( "too big a group finds nothing - not an ordinary room either",
 cp.find_dm_room( dm_rooms, [ "twelve-chars", "another-name", "third-person", "admin" ] ), null );

// NOTE: The New message list marks what already exists - it does not hide anyone, since the same
// list picks the people for a group conversation (Ian, 2026-09-30: people already in one showed).
var dm_invites = [ { room: "0000011", name: "Private (admin + erin)", inviter: "erin" },
 { room: "0000012", name: "Private (admin + fay)", inviter: "carol" } ];
check( "a conversation open", cp.dm_existing( [ "bob" ], "admin", dm_rooms, dm_invites, [ ] ), { kind: "open", room: "0000004" } );
check( "one started here, not joined yet", cp.dm_existing( [ "bob" ], "admin", dm_rooms, dm_invites, [ "0000004" ] ), { kind: "waiting", room: "0000004" } );
check( "their request", cp.dm_existing( [ "erin" ], "admin", dm_rooms, dm_invites, [ ] ), { kind: "request", room: "0000011" } );
check( "not a request from someone it does not name", cp.dm_existing( [ "fay" ], "admin", dm_rooms, dm_invites, [ ] ), { kind: "", room: "" } );
check( "nothing yet", cp.dm_existing( [ "dave" ], "admin", dm_rooms, dm_invites, [ ] ), { kind: "", room: "" } );
check( "a group is its own conversation", cp.dm_existing( [ "bob", "carol" ], "admin", dm_rooms, dm_invites, [ ] ), { kind: "", room: "" } );
var dm_lines = [ "1790000000000 admin :create 0000004-abc DM admin + bob + carol", "1790000000001 bob :joined" ]
 .map( cp.parse_message_line );
check( "waiting for whoever has not joined", cp.dm_waiting_for( dm_lines, [ "bob", "carol" ] ), [ "carol" ] );
check( "nobody left to wait for",
 cp.dm_waiting_for( dm_lines.concat( [ cp.parse_message_line( "1790000000002 carol :joined" ) ] ), [ "bob", "carol" ] ), [ ] );
check( "a member has joined, though their :joined is gone from the queue",
 cp.dm_waiting_for( [ ], [ "bob", "carol" ], cp.parse_members( "admin+1 carol+0" ) ), [ "bob" ] );

// NOTE: Real lines, 2026-10-05 ("dm_decline_probe.js") - verify-a's reserved room, after starting a
// conversation with verify-e, who declined it. Then a group of admin's, and someone else's receipt.
var outcome_lines = cp.parse_fetch_response( [
 "admin+0 verify-a+1 verify-e+0",
 "1791201084000 verify-a :issued (invite for 0000003 sent to verify-e)",
 "1791204165000 verify-e :reject (invite for 0000003 was rejected)",
 "1791204166000 admin :issued (invite for 0000005 sent to bob,carol)",
 "1791204167000 carol :reject (invite for 0000005 was rejected)",
 "1791204168000 dave :reject (invite for 0000005 was rejected)",
 "1791204169000 admin :issued (invite for 0000005 sent to bob)",
 "1791204170000 admin :issued (message sent to verify-a)" ].join( "\n" ) ).messages;

check( "whom each conversation was for, and who declined", cp.dm_invite_outcomes( outcome_lines, "verify-a" ),
 { "0000003": { invited: [ "verify-e" ], declined: [ "verify-e" ] } } );
check( "a group's, each name once - a decline only from someone invited", cp.dm_invite_outcomes( outcome_lines, "admin" ),
 { "0000005": { invited: [ "bob", "carol" ], declined: [ "carol" ] } } );
check( "nothing for someone who started none", cp.dm_invite_outcomes( outcome_lines, "verify-e" ), { } );

check( "someone who joined, then left, is not waited for",
 cp.dm_waiting_for( [ cp.parse_message_line( "1790000000003 carol :remove" ) ], [ "bob", "carol" ] ), [ "bob" ] );

check( "a decline: still waiting for the rest, held kept", cp.dm_after_declines( [ "bob", "carol" ], { invited: [ "bob", "carol" ], declined: [ "carol" ] } ),
 { waiting: [ "bob" ], held: "keep" } );
check( "the last to answer declines, the other joined: held sent", cp.dm_after_declines( [ "carol" ], { invited: [ "bob", "carol" ], declined: [ "carol" ] } ),
 { waiting: [ ], held: "send" } );
check( "everyone declined: held dropped", cp.dm_after_declines( [ "verify-e" ], { invited: [ "verify-e" ], declined: [ "verify-e" ] } ),
 { waiting: [ ], held: "drop" } );
check( "nobody declined: nothing changes", cp.dm_after_declines( [ "bob" ], { invited: [ "bob" ], declined: [ ] } ), { waiting: [ "bob" ], held: "keep" } );

// NOTE: A room's name is its owner's to choose - "DM admin + bob" owned by carol is her room,
// not a conversation between admin and bob (found by review, 2026-09-30).
check( "trusted when the owner is named", cp.dm_trusted( "DM admin + bob", "bob" ), true );
check( "not when someone else owns it", cp.dm_trusted( "DM admin + bob", "carol" ), false );
check( "an owner not known yet is trusted until it is", cp.dm_trusted( "DM admin + bob", "" ), true );
check( "an ordinary room is never a direct message", cp.dm_trusted( "Ledger Ops", "admin" ), false );
check( "a member the name leaves out", cp.dm_outsiders( "DM admin + bob", [ "admin", "bob", "carol" ] ), [ "carol" ] );
check( "nobody left out", cp.dm_outsiders( "DM admin + bob", [ "admin", "bob" ] ), [ ] );
check( "names listed", [ cp.name_list( [ "bob" ] ), cp.name_list( [ "bob", "carol" ] ), cp.name_list( [ "bob", "carol", "dave" ] ) ],
 [ "bob", "bob and carol", "bob, carol and dave" ] );
check( "a request from one person", cp.dm_request_text( "admin", "DM admin + bob", "bob" ), "admin wants to message you." );
check( "a request for a group", cp.dm_request_text( "admin", "DM admin + bob + carol", "bob" ),
 "admin wants to start a conversation with you and carol." );

heading( "direct messages on the server" );

// NOTE: Ian's server direct messages - made as ".<user>", named "/<a>/<b>" by the server, two people
// only (2026-10-02, "." since 2026-10-05). Checked on the container with "dm_server_probe.js".
check( "the server's name read", cp.parse_dm_name( "/admin/verify-a" ), { people: [ "admin", "verify-a" ], count: 2, server: true } );
check( "one person twice is not a conversation", cp.parse_dm_name( "/admin/admin" ), null );
check( "nor three, nor a name that is not a username", [ cp.parse_dm_name( "/a1b/bob/carol" ), cp.parse_dm_name( "/admin/Bob" ), cp.parse_dm_name( "/admin" ) ],
 [ null, null, null ] );
check( "shown as the other person", cp.dm_title( "/admin/verify-a", "verify-a" ), "admin" );
check( "trusted as it is - the server built it", [ cp.dm_trusted( "/admin/verify-a", "admin" ), cp.dm_trusted( "/admin/verify-a", "carol" ) ], [ true, true ] );
check( "the same conversation as the prototype's, by its people", cp.dm_key( "/admin/verify-a" ), cp.dm_key( "Private (admin + verify-a)" ) );
check( "found in the rail", cp.find_dm_room( [ { room: "0000016", name: "/admin/verify-a", owner: "admin" } ], [ "verify-a", "admin" ] ).room, "0000016" );
check( "their request found", cp.dm_existing( [ "verify-a" ], "admin", [ ], [ { room: "0000016", name: "/admin/verify-a", inviter: "verify-a" } ], [ ] ),
 { kind: "request", room: "0000016" } );
check( "a request to message you", cp.dm_request_text( "admin", "/admin/verify-a", "verify-a" ), "admin wants to message you." );

check( "two people start one as .<the other>", cp.dm_create_text( [ "verify-a", "admin" ], "admin" ), ".verify-a" );
check( "whoever starts it", cp.dm_create_text( [ "verify-a", "admin" ], "verify-a" ), ".admin" );
check( "a group keeps the prototype's room", cp.dm_create_text( [ "carol", "admin", "bob" ], "admin" ), "Private (admin + bob + carol)" );
check( "too big a group, nothing", cp.dm_create_text( [ "twelve-chars", "another-name", "third-person", "admin" ], "admin" ), "" );

check( "never signed in", cp.dm_create_problem( "Error: User 'tester-1' is not known.", [ "tester-1" ] ),
 "tester-1 has not signed in yet, so cannot be messaged." );
check( "already have one", cp.dm_create_problem( "Error: Room '/admin/verify-a' already exists.", [ "verify-a" ] ),
 "You already have a conversation with verify-a - if it is not in the rail, look for their request, or it may have been declined." );
check( "a group's name taken by another room", cp.dm_create_problem( "Error: Room 'Private (a1b + bob + carol)' already exists.", [ "bob", "carol" ] )
 .indexOf( "someone else already has a room called" ) > 0, true );
check( "anything else as it came", cp.dm_create_problem( "Error: Something else.", [ "bob" ] ), "Something else." );

// NOTE: Ian, 2026-10-05 - no private rooms for admin, or with admin.
check( "two people may", cp.dm_allowed( "bob", "carol" ), true );
check( "not with admin", cp.dm_allowed( "bob", "admin" ), false );
check( "nor admin with anyone", [ cp.dm_allowed( "admin", "bob" ), cp.dm_allowed( "admin" ) ], [ false, false ] );
check( "nor with yourself", cp.dm_allowed( "bob", "bob" ), false );
check( "anyone else may message someone", cp.dm_allowed( "bob" ), true );
check( "the server's room names the other person, to make and invite at once", cp.dm_create_options( ".carol", [ "carol" ] ), "for=carol;text=.carol" );
check( "a group's room takes its people in the one request", cp.dm_create_options( "Private (amy + bob + cat)", [ "bob", "cat" ] ),
 "for=bob,cat;text=Private (amy + bob + cat)" );
check( "the server's words for admin, as they come", cp.dm_create_problem( "Error: Private rooms are not permitted with the administrator.", [ "admin" ] ),
 "Private rooms are not permitted with the administrator." );

heading( "sign in errors" );

check( "a missing connect status, in plain words", cp.sign_in_error_text( "invalid or missing connect status" ),
 "The server's answer to signing in was not what was expected. Please try again." );
check( "a locked node, said so", cp.sign_in_error_text( "Error: Was unable to start a web session with access token '10301'." ),
 "This node is locked - it restarted, and nobody can sign in until it is unlocked." );
check( "a locked node's refusal known", [ cp.is_locked_sign_in_error( "Error: Was unable to start a web session with access token '1'." ),
 cp.is_locked_sign_in_error( "Error: Invalid password." ) ], [ true, false ] );
check( "any other error as it is, less its 'Error:'", cp.sign_in_error_text( "Error: Web session is currently busy (try again shortly)." ),
 "Web session is currently busy (try again shortly)." );
check( "nothing", cp.sign_in_error_text( "" ), "" );
check( "unable to start a session on a node known not to be locked - not called locked (2026-10-13)",
 cp.sign_in_error_text( "Error: Was unable to start a web session for 'admin'.", false ),
 "The node couldn't start a session just now - wait a moment, then try again." );
check( "a node known to be locked - said so, whatever it answered", cp.sign_in_error_text( "Error: This web session is not valid (or has expired).", true ),
 "This node is locked - it restarted, and nobody can sign in until it is unlocked." );
check( "recovering onto a node not set up, refused - someone else setting it up, not the words", [
 cp.recovery_error_text( "Error: System is currently locked for administration.", true ),
 cp.recovery_error_text( "Error: System is currently locked for administration.", false ) ],
 [ "Someone else is setting this node up just now - if it isn't you, disconnect it from the network. Otherwise try again in a"
  + " few minutes.", "The node did not accept those words - check each one, and that they are in order." ] );
check( "recovering a locked node - the sign in's words, back to the sign in", [ cp.recovery_texts( false ).heading,
 cp.recovery_texts( false ).back, cp.recovery_texts( false ).caution ], [ "Recover this node", "Back to sign in", "" ] );
check( "recovering onto a node not set up - its own words, the caution, back to setting up", [ cp.recovery_texts( true ).heading,
 cp.recovery_texts( true ).back, /Check them against your paper/.test( cp.recovery_texts( true ).caution ),
 /restored from its backups/.test( cp.recovery_texts( true ).done_note ) ], [ "Recover a node from its twelve words", "Back", true, true ] );
check( "the chat not turned on", [ cp.irc_unavailable( "Error: IRC usage is not currently available." ), cp.irc_unavailable( "Error: Other." ),
 cp.irc_unavailable( null ) ], [ true, false, false ] );

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

heading( "a tab for each page" );

check( "no tab of that name - the browser made an empty one: load it", cp.choose_tab_action( true, "", "abc" ), "load" );
check( "a tab on this session: switch to it, no reload", cp.choose_tab_action( false, "abc", "abc" ), "focus" );
check( "a tab signed in as someone else: leave it, open another", cp.choose_tab_action( false, "xyz", "abc" ), "new" );
check( "a tab signed out: leave it, open another", cp.choose_tab_action( false, "", "abc" ), "new" );
check( "neither signed in: still another - nothing to share", cp.choose_tab_action( false, "", "" ), "new" );

heading( "an address once a link has ended" );

check( "source goes, the section stays", cp.address_without_source( "http://localhost:13031/account.html?source=1790900000000#mine" ),
 "/account.html#mine" );
check( "other parameters stay", cp.address_without_source( "http://h/console.html?embedded=1&source=5" ), "/console.html?embedded=1" );
check( "an address with no source is left as it is", cp.address_without_source( "http://h/chat.html" ), "/chat.html" );

heading( "a linked tab opens on the same room" );

check( "the room goes in the address", cp.linked_tab_address( "http://h/chat.html", "1790900000000", "0000004" ),
 "http://h/chat.html?source=1790900000000#room=0000004" );
check( "a room already asked for is replaced", cp.linked_tab_address( "http://h/chat.html?source=1#room=0000002", "2", "0000005" ),
 "http://h/chat.html?source=2#room=0000005" );
check( "no room open, no room asked for", cp.linked_tab_address( "http://h/chat.html#room=0000002", "3", "" ), "http://h/chat.html?source=3" );
check( "the room asked for", cp.room_from_hash( "#room=0000004" ), "0000004" );
check( "only a room number", [ cp.room_from_hash( "#room=12" ), cp.room_from_hash( "#room=0000004x" ), cp.room_from_hash( "#mine" ), cp.room_from_hash( "" ) ],
 [ "", "", "", "" ] );

heading( "remember on this browser" );

check( "an account not saved shows Nothing", cp.retain_mode_of( "11111", "22222", false ), "none" );
check( "saved, no password: The PIN only", cp.retain_mode_of( "11111,22222", "22222", false ), "access" );
check( "saved with a password: both", cp.retain_mode_of( "11111,22222", "22222", true ), "full" );
check( "no account yet", cp.retain_mode_of( "11111", "", false ), "none" );

check( "Nothing forgets the account and its password", cp.plan_retain_choice( "11111,22222", "22222", "none", "h" ),
 { list: "11111", keep_hash: false } );
check( "Nothing for the last account removes the key", cp.plan_retain_choice( "22222", "22222", "none", "h" ),
 { list: null, keep_hash: false } );
check( "The PIN only: saved, any password dropped", cp.plan_retain_choice( "11111", "22222", "access", "h" ),
 { list: "11111,22222", keep_hash: false } );
check( "The PIN and the password", cp.plan_retain_choice( null, "22222", "full", "h" ), { list: "22222", keep_hash: true } );
check( "both, but no hash held - the PIN only", cp.plan_retain_choice( "22222", "22222", "full", "" ), { list: "22222", keep_hash: false } );

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

// --------------------------------------------------------------------
heading( "the app switcher" );

function app_keys( list ) { return list.map( function( app ) { return app.key + ( app.current ? "*" : "" ); } ); }

check( "admin's apps, the chat marked", app_keys( cp.switcher_apps( true, false, "chat" ) ), [ "home", "chat*", "account", "console" ] );
check( "a member's - no console", app_keys( cp.switcher_apps( false, false, "home" ) ), [ "home*", "chat", "account" ] );
check( "admin on a phone - no console", app_keys( cp.switcher_apps( true, true, "account" ) ), [ "home", "chat", "account*" ] );
check( "the accounts page is My account for everyone - People is in Home (2026-10-11)",
 [ cp.switcher_apps( true, false, "" )[ 2 ].title, cp.switcher_apps( false, false, "" )[ 2 ].title ], [ "My account", "My account" ] );
check( "each app's page and tab", cp.switcher_apps( true, false, "" ).map( function( app ) { return app.page + " " + app.tab; } ),
 [ "home.html ciyam-home", "chat.html ciyam-chat", "account.html ciyam-accounts", "console.html ciyam-console" ] );
check( "the button's name", [ "home", "chat", "account", "console" ].map( function( key ) { return cp.switcher_title( key, false ); } ),
 [ "Home", "Chat", "My account", "Console" ] );
check( "an app not known - no name", cp.switcher_title( "other", true ), "" );

check( "signed out - the message", cp.signed_out_message( "4014a01afdf9" ), { kind: "signed_out", sessid: "4014a01afdf9" } );
check( "signed out - this session", cp.is_signed_out_here( cp.signed_out_message( "4014a01afdf9" ), "4014a01afdf9" ), true );
check( "signed out - another session", cp.is_signed_out_here( cp.signed_out_message( "4014a01afdf9" ), "99" ), false );
check( "signed out - this tab signed in to nothing", cp.is_signed_out_here( cp.signed_out_message( "" ), "" ), false );
check( "signed out - a channel string is not one", cp.is_signed_out_here( "1790900000000", "1790900000000" ), false );

var chat_app = cp.switcher_apps( false, false, "home" )[ 1 ];

check( "a row - opens here, a new tab at its right", cp.switcher_row( chat_app, false ),
 { action: "here", note: "Rooms and messages", button: "new", label: "Open Chat in a new tab" } );
check( "a row - open in another tab, gone to", cp.switcher_row( chat_app, true ),
 { action: "go", note: "Open in another tab", button: "go", label: "Go to the tab Chat is open in" } );
check( "a row - this app, open elsewhere too", cp.switcher_row( cp.switcher_apps( false, false, "chat" )[ 1 ], true ),
 { action: "current", note: "You are here", button: "", label: "" } );

check( "a plain click - this tab", cp.switch_wants_new_tab( { button: 0 } ), false );
check( "ctrl, cmd or shift and click - a new tab", [ { ctrlKey: true }, { metaKey: true }, { shiftKey: true } ].map( cp.switch_wants_new_tab ),
 [ true, true, true ] );
check( "the middle button - a new tab", cp.switch_wants_new_tab( { button: 1 } ), true );
check( "no event - this tab", cp.switch_wants_new_tab( null ), false );

check( "which apps - the question", cp.which_apps_message( "4014a01afdf9" ), { kind: "which_apps", sessid: "4014a01afdf9" } );
check( "which apps - this session", cp.is_which_apps_here( cp.which_apps_message( "4014a01afdf9" ), "4014a01afdf9" ), true );
check( "which apps - another session", cp.is_which_apps_here( cp.which_apps_message( "4014a01afdf9" ), "99" ), false );
check( "which apps - signed in to nothing", cp.is_which_apps_here( cp.which_apps_message( "" ), "" ), false );
check( "an app open - this session, and its page", cp.app_open_here( cp.app_open_message( "4014a01afdf9", "console", 1790900000000 ), "4014a01afdf9" ),
 { app: "console", page: "1790900000000" } );
check( "an app open - another session", cp.app_open_here( cp.app_open_message( "4014a01afdf9", "console", "17" ), "99" ), null );
check( "an app open - not a key", cp.app_open_here( cp.app_open_message( "4014a01afdf9", "<b>x</b>", "17" ), "4014a01afdf9" ), null );
check( "an app open - no page", cp.app_open_here( cp.app_open_message( "4014a01afdf9", "chat" ), "4014a01afdf9" ), null );
check( "an app open - a signed out message is not one", cp.app_open_here( cp.signed_out_message( "4014a01afdf9" ), "4014a01afdf9" ), null );

check( "a page left - the message", cp.page_left_message( "4014a01afdf9", 17 ), { kind: "page_left", sessid: "4014a01afdf9", page: "17" } );
check( "a page left - this session", cp.page_left_here( cp.page_left_message( "4014a01afdf9", "17" ), "4014a01afdf9" ), "17" );
check( "a page left - another session", cp.page_left_here( cp.page_left_message( "4014a01afdf9", "17" ), "99" ), "" );
check( "a page left - not an id", cp.page_left_here( cp.page_left_message( "4014a01afdf9", "x" ), "4014a01afdf9" ), "" );

// --------------------------------------------------------------------
heading( "the session kept for the tab" );

var signed_in = { access: "20401", device: "6034a59f4554116", hashed: "secret-hash", sessid: "4014a01afdf955cf0ef8",
 unique: "1759999999123", username: "verify-a", is_admin: false };

var kept = cp.format_tab_session( signed_in );

check( "never the hashed password", kept.indexOf( "secret-hash" ) < 0 && kept.indexOf( "hashed" ) < 0, true );
check( "read back as it was", cp.parse_tab_session( kept ),
 { access: "20401", device: "6034a59f4554116", sessid: "4014a01afdf955cf0ef8", unique: "1759999999123", username: "verify-a", is_admin: false } );
check( "admin kept as admin", cp.parse_tab_session( cp.format_tab_session(
 { access: "97620", device: "d", sessid: "ab12", unique: "", username: "admin", is_admin: true } ) ).is_admin, true );
check( "nothing kept", cp.parse_tab_session( null ), null );
check( "not JSON", cp.parse_tab_session( "{nope" ), null );
check( "not a PIN", cp.parse_tab_session( JSON.stringify( { access: "abc", device: "d", sessid: "ab12" } ) ), null );
check( "no session", cp.parse_tab_session( JSON.stringify( { access: "20401", device: "d", sessid: "" } ) ), null );
check( "no device", cp.parse_tab_session( JSON.stringify( { access: "20401", device: "", sessid: "ab12" } ) ), null );
check( "admin only when it says so exactly",
 cp.parse_tab_session( JSON.stringify( { access: "20401", device: "d", sessid: "ab12", is_admin: "true" } ) ).is_admin, false );

// --------------------------------------------------------------------
heading( "recovering with the twelve words" );

// NOTE: A few of the 2048 - enough to tell known words from others.
var wordlist = [ "act", "attitude", "describe", "endless", "focus", "govern", "isolate", "note", "outer", "taxi", "ugly", "abandon" ];

check( "typed with spaces", cp.recovery_words( "taxi act describe" ), [ "taxi", "act", "describe" ] );
check( "pasted with numbers, commas and line breaks, any case", cp.recovery_words( "1. Taxi, 2. ACT\n3) describe" ), [ "taxi", "act", "describe" ] );
check( "nothing", cp.recovery_words( "  " ), [ ] );

var twelve = cp.recovery_words( "taxi act describe attitude focus ugly govern outer note isolate endless act" );

check( "twelve known words - nothing wrong", cp.recovery_words_problem( twelve, wordlist ), "" );
check( "none yet", cp.recovery_words_problem( [ ], wordlist ), "Type or paste your twelve words." );
check( "a misspelling said before a count", cp.recovery_words_problem( [ "taxi", "acct" ], wordlist ),
 "\"acct\" isn't one of the words - check its spelling." );
check( "too few", cp.recovery_words_problem( twelve.slice( 0, 11 ), wordlist ), "That's 11 words - there are twelve." );
check( "too many", cp.recovery_words_problem( twelve.concat( [ "act" ] ), wordlist ), "That's 13 words - there are twelve." );

var completing = [ "abandon", "act", "action", "actor", "actress", "actual", "adapt", "fact", "impact", "taxi" ];

check( "completions start with what is typed - not anywhere in a word", cp.word_completions( "act", completing, 6 ),
 [ "act", "action", "actor", "actress", "actual" ] );
check( "any case, spaces trimmed", cp.word_completions( " AC ", completing, 6 ), [ "act", "action", "actor", "actress", "actual" ] );
check( "at most the limit", cp.word_completions( "a", completing, 3 ), [ "abandon", "act", "action" ] );
check( "a whole word that is the only one - none", cp.word_completions( "taxi", completing, 6 ), [ ] );
check( "nothing typed, or not a word - none", [ cp.word_completions( "", completing, 6 ), cp.word_completions( "a1", completing, 6 ) ], [ [ ], [ ] ] );
check( "no word starts so - none", cp.word_completions( "zz", completing, 6 ), [ ] );

// NOTE: After a recovery - admin's PIN 10301, once 31324; 40311 another account, kept with its password.
// NOTE: PINs out of sight (Ian, 2026-10-10) - names in their place.
check( "an account's name on screen - its username", cp.account_display_name( "damon", false ), "damon" );
check( "- admin, with none of its own", [ cp.account_display_name( "", true ), cp.account_display_name( undefined, true ) ], [ "admin", "admin" ] );
check( "- never a PIN", cp.account_display_name( "", false ), "Your account" );
check( "a saved account listed by its name", cp.saved_account_label( "damon", 0 ), "damon" );
check( "- one saved before names were kept, by its place", [ cp.saved_account_label( "", 0 ), cp.saved_account_label( null, 2 ) ],
 [ "Saved account 1", "Saved account 3" ] );
// NOTE: In alphabetical order, the account just signed out of chosen (Ian, 2026-10-11).
var saved_accounts = [ { pin: "20405", name: "verify-e" }, { pin: "31324", name: "" }, { pin: "10301", name: "admin" },
 { pin: "20401", name: "Maya" }, { pin: "40311", name: "" } ];
var ordered_accounts = cp.order_saved_accounts( saved_accounts );
check( "saved accounts - by name, A to Z whatever the case, then those with no name as saved",
 ordered_accounts, [ { pin: "10301", label: "admin" }, { pin: "20401", label: "Maya" }, { pin: "20405", label: "verify-e" },
  { pin: "31324", label: "Saved account 1" }, { pin: "40311", label: "Saved account 2" } ] );
check( "- none saved", cp.order_saved_accounts( [ ] ), [ ] );
check( "the account just signed out of - chosen", cp.chosen_saved_index( ordered_accounts, "20405" ), 2 );
check( "- not saved, or none yet - the first",
 [ cp.chosen_saved_index( ordered_accounts, "55555" ), cp.chosen_saved_index( ordered_accounts, null ) ], [ 0, 0 ] );

check( "names whose PIN is no longer saved - let go", cp.stale_saved_names( [ "cws.name_10301", "cws.name_31324", "cws.access", "cws.name_40311" ],
 "10301,40311" ), [ "cws.name_31324" ] );
check( "- none saved, every name goes", cp.stale_saved_names( [ "cws.name_10301", "cws.device" ], null ), [ "cws.name_10301" ] );

// NOTE: Kept for an account whether or not it is remembered - by its name (2026-10-11).
check( "dismissed announcements kept by name", cp.account_storage_key( cp.c_storage_dismissed_by_name, "damon", false ), "cws.dismissed@damon" );
check( "recent emoji kept by name - admin's as admin", cp.account_storage_key( cp.c_storage_emoji_recent_by_name, "", true ), "cws.emoji_recent@admin" );
check( "the keys kept by PIN before - let go, nothing else",
 cp.pin_keyed_leftovers( [ "cws.dismissed_20401", "cws.emoji_recent_10301", "cws.dismissed@damon", "cws.hashed_10301", "cws.emoji_recent", "cws.access" ] ),
 [ "cws.dismissed_20401", "cws.emoji_recent_10301" ] );

check( "the node's refusals - not its words", cp.recovery_error_text( "Error: System identity mismatch (incorrect mnemonics?)." ),
 "Those aren't this node's twelve words." );
check( "- what it answered for words not its own, before Ian's 'a9d18319'", cp.recovery_error_text( "Error: System is currently locked for administration." ),
 "The node did not accept those words - check each one, and that they are in order." );
check( "- not locked", /isn't locked, so there is nothing to recover/.test(
 cp.recovery_error_text( "Error: System identity is not currently locked." ) ), true );
check( "- anything else as it came", cp.recovery_error_text( "Error: Something else." ), "Something else." );
check( "a refusal's step - the words for words not taken, the password's for the rest, and never the words onto a new node", [
 cp.recovery_error_step( "Error: System identity mismatch (incorrect mnemonics?).", false ),
 cp.recovery_error_step( "Error: System is currently locked for administration.", false ),
 cp.recovery_error_step( "Error: System is currently locked for administration.", true ),
 cp.recovery_error_step( "Error: Something else.", false ) ], [ "words", "words", "password", "password" ] );

heading( "pinned messages" );

// NOTE: As the node sends them, captured by "pin_probe.js" (2026-10-14) - the pinned first, author prefixed "!".
check( "a pinned message - pinned, the sender without its \"!\"", cp.parse_message_line( "1791951506000 !admin  pin probe A" ),
 { kind: "chat", unique: "1791951506000", sender: "admin", edited: false, private: false, text: "pin probe A", pinned: true } );
check( "- pinned and edited - \"!admin*\"", cp.parse_message_line( "1791951506000 !admin*  pin probe A edited" ),
 { kind: "chat", unique: "1791951506000", sender: "admin", edited: true, private: false, text: "pin probe A edited", pinned: true } );
check( "- an ordinary message carries no pinned mark", "pinned" in cp.parse_message_line( "1791951508000 admin  pin probe B" ), false );

var pinned_read = cp.parse_fetch_response( "admin+2 verify-a+0\n1791951506000 !admin  pin probe A\n1791951392000 admin :create 0000004-8812 Suite Posts\n"
 + "1791951397000 verify-a :joined\n1791951508000 admin  pin probe B" );
check( "a read with one pinned - the room's pinned set", pinned_read.pinned.map( function( m ) { return m.unique; } ), [ "1791951506000" ] );
check( "- the messages back in time order, the pinned among them", pinned_read.messages.map( function( m ) { return m.unique; } ),
 [ "1791951392000", "1791951397000", "1791951506000", "1791951508000" ] );
check( "- new because of the others", pinned_read.has_new, true );
var pinned_only = cp.parse_fetch_response( "admin+2 verify-a+0\n1791951506000 !admin  pin probe A" );
check( "a poll with nothing new but the pinned - not new", pinned_only.has_new, false );
check( "- and the start point not taken back to it", cp.later_start_point( "1791951610001", cp.next_start_point( pinned_only.messages ) ),
 "1791951610001" );
check( "- a read whose newest is pinned moves it on", cp.later_start_point( "1791951500001", cp.next_start_point( pinned_only.messages ) ),
 "1791951506001" );
check( "- none yet, or nothing read - as it comes", [ cp.later_start_point( "", "1791951506001" ), cp.later_start_point( "1791951506001", "" ),
 cp.later_start_point( "", "" ) ], [ "1791951506001", "1791951506001", "" ] );
check( "nothing pinned - none, as before", cp.parse_fetch_response( "admin+2 verify-a+0\n1791951508000 admin  pin probe B" ).pinned, [ ] );
check( "in time order - by unique, a shared one as it came",
 cp.in_time_order( [ { unique: "3", n: 1 }, { unique: "1" }, { unique: "3", n: 2 }, { unique: "2" } ] ),
 [ { unique: "1" }, { unique: "2" }, { unique: "3", n: 1 }, { unique: "3", n: 2 } ] );

check( "the Admin room read whole when only its pinned came back - nothing new among them", cp.needs_full_read(
 cp.parse_fetch_response( "admin+2 verify-a+0\n1791951506000 !admin  a pinned announcement" ).messages ), true );
check( "the strip shut for a room, then open again - the others kept", [ cp.with_pinned_shut( [ "0000004" ], "0000002", true ),
 cp.with_pinned_shut( [ "0000004", "0000002" ], "0000004", false ), cp.with_pinned_shut( [ "0000004" ], "0000004", true ),
 cp.parse_dismissed( JSON.stringify( [ "0000004", "x" ] ) ) ], [ [ "0000004", "0000002" ], [ "0000002" ], [ "0000004" ], [ "0000004" ] ] );
check( "Up edits the newest of your own - others' and nothing to edit passed over", [ cp.last_editable( [ { unique: "1", editable: true, text: "first" },
 { unique: "2", editable: true, text: "second" }, { unique: "3", editable: false, text: "theirs" } ] ).unique,
 cp.last_editable( [ { unique: "4", editable: false, text: "theirs" } ] ), cp.last_editable( [ ] ), cp.last_editable( [ { unique: "5", editable: true, text: "  " } ] ) ], [ "2", null, null, null ] );
check( "a message edited to spaces - deleted, public or private (Ian, 2026-10-15)", [ cp.parse_message_line( "1792032681000 admin*    " ),
 cp.parse_message_line( "1792032682000 admin* !  " ).deleted ], [ { kind: "chat", unique: "1792032681000", sender: "admin", edited: true, private: false,
 text: "  ", deleted: true }, true ] );
check( "- an ordinary one carries no deleted mark", "deleted" in cp.parse_message_line( "1792032683000 admin  still here" ), false );
check( "deleting - an edit to two spaces, private with its \"!\"", [ cp.delete_options( "1792032681000", false ), cp.delete_options( "1792032682000", true ) ],
 [ "for=1792032681000;text=  ", "for=!1792032682000;text=  " ] );
check( "- not pinned, nor announced", [ cp.can_pin( cp.parse_message_line( "1792032681000 admin*    " ), "admin" ),
 cp.pending_announcements( [ cp.parse_message_line( "1792032681000 admin*    " ) ], [ ] ).length ], [ false, 0 ] );
check( "pinning - an edit to a single space", cp.pin_options( "1791951506000" ), "for=1791951506000;text= " );
var own = cp.parse_message_line( "1791951508000 admin  mine" );
check( "who may pin - their own ordinary messages only", [ cp.can_pin( own, "admin" ), cp.can_pin( own, "verify-a" ),
 cp.can_pin( cp.parse_message_line( "1791951587000 admin !private" ), "admin" ), cp.can_pin( cp.parse_message_line( "1791951397000 admin :joined" ), "admin" ),
 cp.can_pin( null, "admin" ), cp.can_pin( own, "" ) ], [ true, false, false, false, false, false ] );
check( "the ninth pin refused - said plainly", cp.pin_error_text( "Error: Maximum pinned messages for this room already exist." ),
 "This room already has 8 pinned messages, the most it can hold - unpin one first." );
check( "- a private one", cp.pin_error_text( "Error: Source message must not be modified to or from private." ), "A private message can't be pinned." );
check( "- anything else as it came", cp.pin_error_text( "Error: Something else." ), "Something else." );
check( "- redrawn when what it shows changes", [ cp.pinned_signature( pinned_read.pinned ) === cp.pinned_signature( pinned_only.pinned ),
 cp.pinned_signature( pinned_read.pinned ) === cp.pinned_signature( [ ] ) ], [ true, false ] );

// NOTE: Colour and style - Ian's shorthand typed, real escapes stored and drawn (Ian, 2026-10-15; "Message Formatting Design").
heading( "formatting" );

var esc = String.fromCharCode( 27 );
var shown_escapes = function( text ) { return String( text ).split( esc ).join( "\\e" ); };

check( "his codes as his console sends them - bold red is bold, then red", shown_escapes( cp.shorthand_to_ansi( "%BR%down%N% and %LA%%OK%x%N%" ) ),
 "\\e[1m\\e[31mdown\\e[0m and \\e[94m\\e[40mx\\e[0m" );
check( "- a '%..%' not his left as typed", cp.shorthand_to_ansi( "100%ZZ% %r% %N" ), "100%ZZ% %r% %N" );
check( "- every code of his", Object.keys( cp.c_format_codes ).length, 38 );
check( "the preview only when there is a code", [ cp.has_shorthand_codes( "%I%x%N%" ), cp.has_shorthand_codes( "50% off %ZZ%" ),
 cp.has_shorthand_codes( "" ) ], [ true, false, false ] );

check( "a stored message drawn - a run each style", cp.format_pieces( esc + "[1m" + esc + "[31mdown" + esc + "[0m now " + esc + "[3m" + esc + "[44mx" ),
 [ { text: "down", style: { b: true, fg: "R" } }, { text: " now ", style: { } }, { text: "x", style: { i: true, bg: "OA" } } ] );
check( "- the ends of one style each, as other clients send them", cp.format_pieces( esc + "[1;4;9mabc" + esc + "[22md" + esc + "[24;29;39me" ),
 [ { text: "abc", style: { b: true, s: true, u: true } }, { text: "d", style: { s: true, u: true } }, { text: "e", style: { } } ] );
check( "- the lighter colours and the bright backgrounds", cp.format_pieces( esc + "[97;101mx" ), [ { text: "x", style: { fg: "LW", bg: "OR" } } ] );
check( "- 256 colours and true colour passed over, numbers and all", cp.format_pieces( esc + "[38;5;196;1mx" + esc + "[48;2;1;2;3my" ),
 [ { text: "xy", style: { b: true } } ] );
check( "every other escape dropped - cursor, clearing, a title, a link, a bell", cp.strip_format( "a" + esc + "[2J" + esc + "[10;5Hb" + esc
 + "]0;title" + String.fromCharCode( 7 ) + "c" + esc + "]8;;http://x" + esc + "\\d" + esc + "7e" + String.fromCharCode( 7 ) + "f" ), "abcdef" );
check( "- an escape cut short, and the control characters", cp.strip_format( "a" + esc + "[" + String.fromCharCode( 8 ) + "b" + String.fromCharCode( 155 )
 + "c" + esc ), "abc" );
check( "- line breaks and tabs kept", cp.strip_format( "a\n\tb" ), "a\n\tb" );
check( "- DCS, SOS, PM and APC dropped to their end", cp.strip_format( "a" + esc + "Pq#0;1" + esc + "\\b" + esc + "X1" + esc + "\\c" + esc + "^2" + esc
 + "\\d" + esc + "_3" + esc + "\\e" ), "abcde" );
check( "- one never ended drops only its opening, not the rest", cp.strip_format( "a" + esc + "]0;no end b" ), "a0;no end b" );
check( "- a two-character escape and one with intermediates", [ cp.strip_format( "a" + esc + "7b" + esc + "(Bc" ), cp.strip_format( "a" + esc + "\nb" ) ],
 [ "ab" + "c", "a\nb" ] );
check( "- text turned to run the other way - its characters dropped", cp.strip_format( "a\u202eb\u2066c\u2069d" ), "abcd" );
check( "a long SGR still read", cp.format_pieces( esc + "[" + "1;".repeat( 40 ) + "31mx" ), [ { text: "x", style: { b: true, fg: "R" } } ] );
check( "- details after ':' kept with their number - no italics from a colour", [ cp.format_pieces( esc + "[38:2::1:2:3mx" ),
 cp.format_pieces( esc + "[4:3mx" + esc + "[4:0my" ) ], [ [ { text: "x", style: { } } ], [ { text: "x", style: { u: true } }, { text: "y", style: { } } ] ] );
check( "- one with a private marker is not a style", cp.format_pieces( esc + "[>4;1mx" ), [ { text: "x", style: { } } ] );
check( "a message with no codes - one plain run", cp.format_pieces( "just words" ), [ { text: "just words", style: { } } ] );

check( "to edit - escapes back to his shorthand, plain left alone", [ cp.ansi_to_shorthand( cp.shorthand_to_ansi( "Hi %R%red %B%bold%N%%R% more%N% end" ) ),
 cp.ansi_to_shorthand( "plain 50% text" ), cp.ansi_to_shorthand( esc + "[1m" + esc + "[3mx" + esc + "[0m" ) ],
 [ "Hi %R%red %B%bold%N%%R% more%N% end", "plain 50% text", "%BI%x%N%" ] );

// NOTE: The format bar - the selected words restyled and the codes written again, with "%N%" his one off code. "start" and
// "end" are places in what is typed, codes and all, as the box gives them.
check( "bold a word", cp.format_restyle( "The node is down", 4, 8, { flag: "b" } ), { text: "The %B%node%N% is down", start: 7, end: 11 } );
check( "- then red - his bold-red code", cp.format_restyle( "The %B%node%N% is down", 7, 11, { fg: "R" } ).text, "The %BR%node%N% is down" );
check( "- red again takes it off, then bold again", [ cp.format_restyle( "The %BR%node%N% is down", 8, 12, { fg: "R" } ).text,
 cp.format_restyle( "The %B%node%N% is down", 7, 11, { flag: "b" } ).text ], [ "The %B%node%N% is down", "The node is down" ] );
check( "bold inside red - the red put back after it", cp.format_restyle( "%R%The whole node%N%", 7, 12, { flag: "b" } ),
 { text: "%R%The %B%whole%N%%R% node%N%", start: 10, end: 15 } );
check( "bold off the middle of bold", cp.format_restyle( "%B%The whole node%N%", 7, 12, { flag: "b" } ).text, "%B%The %N%whole%B% node%N%" );
check( "a selection across a code - by the words, not the codes", cp.format_restyle( "%R%The whole%N% node", 7, 20, { flag: "b" } ).text,
 "%R%The %B%whole%N%%B% node%N%" );
check( "clear formatting on part", cp.format_restyle( "%BR%The whole node%N%", 8, 13, { clear: true } ).text, "%BR%The %N%whole%BR% node%N%" );
check( "over several lines", cp.format_restyle( "one\ntwo", 2, 5, { bg: "OO" } ).text, "on%OO%e\nt%N%wo" );
check( "text that would join a code into one of Ian's - nothing done rather than the message changed",
 cp.format_restyle( "x%Ry", 3, 4, { flag: "b" } ), null );
check( "nothing selected, or only codes - nothing done", [ cp.format_restyle( "abc", 1, 1, { flag: "b" } ),
 cp.format_restyle( "%R%abc", 0, 3, { flag: "b" } ) ], [ null, null ] );
check( "the buttons pressed - what every selected character has", [ cp.format_selection_style( "%BR%The%N% x", 4, 7 ),
 cp.format_selection_style( "%BR%The%N% x", 4, 12 ), cp.format_selection_style( "abc", 1, 1 ) ], [ { b: true, fg: "R" }, { }, { } ] );
check( "his own codes typed by hand - drawn the same as the bar's", cp.format_shorthand_pieces( "%B%%R%x%N%y" ),
 [ { text: "x", style: { b: true, fg: "R" } }, { text: "y", style: { } } ] );

// NOTE: The styled box (Ian, 2026-10-18) - with Codes off the box holds the words, their styles kept beside them a character
// each. "sty" writes them shortly - the words and, for each run, its style - so the checks read.
function sty( chars )
{
   return cp.format_shorthand_pieces( cp.format_styled_to_shorthand( chars ) ).map( function( p ) { return p.text + JSON.stringify( p.style ); } ).join( " " );
}

const s_late = cp.format_styled_from_shorthand( "Backup is %LR%late%N% - %B%check%N% it" );

check( "styled - the words alone in the box", cp.format_styled_text( s_late ), "Backup is late - check it" );
check( "- from a stored message, the same", sty( cp.format_styled_from_ansi( cp.shorthand_to_ansi( "Backup is %LR%late%N% - %B%check%N% it" ) ) ),
 sty( s_late ) );
check( "- back to his shorthand, for Codes", cp.format_styled_to_shorthand( s_late ), "Backup is %LR%late%N% - %B%check%N% it" );
check( "- as sent: escapes, the same as his shorthand gives", cp.format_styled_to_ansi( s_late ),
 cp.shorthand_to_ansi( "Backup is %LR%late%N% - %B%check%N% it" ) );
check( "- a '%R%' typed as words stays words when sent", cp.format_styled_to_ansi( cp.format_styled_edit( [ ], "50% %R% off" ) ), "50% %R% off" );
check( "- with Codes on it becomes his code, as it always has", cp.format_styled_to_shorthand( cp.format_styled_edit( [ ], "a %R% b" ) ), "a %R% b" );
check( "- bold then red together - his bold-red code, and off at the end", cp.format_styled_to_ansi(
 cp.format_styled_restyle( cp.format_styled_restyle( cp.format_styled_edit( [ ], "go" ), 0, 2, { flag: "b" } ), 0, 2, { fg: "R" } ) ),
 cp.shorthand_to_ansi( "%BR%go%N%" ) );

check( "typing at the end of a red word - red too", sty( cp.format_styled_edit( s_late, "Backup is lateness - check it" ) ),
 "Backup is {} lateness{\"fg\":\"LR\"}  - {} check{\"b\":true}  it{}" );
check( "typing before it - plain, as the word before", sty( cp.format_styled_edit( s_late, "Backup is so late - check it" ) ),
 "Backup is so {} late{\"fg\":\"LR\"}  - {} check{\"b\":true}  it{}" );
check( "deleting part of a styled word - the rest keeps its style", sty( cp.format_styled_edit( s_late, "Backup is le - check it" ) ),
 "Backup is {} le{\"fg\":\"LR\"}  - {} check{\"b\":true}  it{}" );
check( "a word pasted over a selection - the style before it", sty( cp.format_styled_edit( s_late, "Backup is early - check it" ) ),
 "Backup is early - {} check{\"b\":true}  it{}" );
check( "everything deleted, then typed - plain", sty( cp.format_styled_edit( cp.format_styled_edit( s_late, "" ), "new" ) ), "new{}" );
check( "an emoji - two of the box's places - in one go", cp.format_styled_edit( [ ], "a😀" ).length, 3 );

// NOTE: Where the cursor is says where the change was (found by review, 2026-10-18).
const s_aa = cp.format_styled_from_shorthand( "%R%a%N%a" );
const s_bg = cp.format_styled_from_shorthand( "%OO%word%N% next" );

check( "the first of two like characters deleted - the one left keeps its own style", [ sty( cp.format_styled_edit( s_aa, "a", 0 ) ),
 sty( cp.format_styled_edit( s_aa, "a" ) ) ], [ "a{}", "a{\"fg\":\"R\"}" ] );
check( "a space typed after a word with a background - the word's style, as typed after it", sty( cp.format_styled_edit( s_bg, "word  next", 5 ) ),
 "word {\"bg\":\"OO\"}  next{}" );
check( "an emoji typed before a like one - each keeps one style for both its halves", [ cp.format_styled_edit(
 cp.format_styled_from_shorthand( "%R%😀%N%" ), "😁😀", 2 ).map( c => c.style.fg || "-" ).join( "," ),
 cp.format_styled_edit( cp.format_styled_from_shorthand( "%R%😀%N%" ), "😁😀" ).map( c => c.style.fg || "-" ).join( "," ) ],
 [ "-,-,R,R", "-,-,-,-" ] );
check( "a caret that does not fit what changed - as without one", sty( cp.format_styled_edit( s_late, "Backup is lateness - check it", 2 ) ),
 sty( cp.format_styled_edit( s_late, "Backup is lateness - check it" ) ) );

check( "the bar on the words - bold, and bold again takes it off", [ sty( cp.format_styled_restyle( s_late, 0, 6, { flag: "b" } ) ),
 sty( cp.format_styled_restyle( s_late, 17, 22, { flag: "b" } ) ) ],
 [ "Backup{\"b\":true}  is {} late{\"fg\":\"LR\"}  - {} check{\"b\":true}  it{}", "Backup is {} late{\"fg\":\"LR\"}  - check it{}" ] );
check( "- the same colour again takes it off, clear takes everything", [ sty( cp.format_styled_restyle( s_late, 10, 14, { fg: "LR" } ) ),
 sty( cp.format_styled_restyle( s_late, 0, 25, { clear: true } ) ) ], [ "Backup is late - {} check{\"b\":true}  it{}", "Backup is late - check it{}" ] );
check( "- nothing selected, nothing done; the words left alone", [ cp.format_styled_restyle( s_late, 3, 3, { flag: "b" } ),
 cp.format_styled_text( cp.format_styled_restyle( s_late, 0, 6, { bg: "OA" } ) ) ], [ null, "Backup is late - check it" ] );
check( "- the one it was given is not changed", sty( s_late ), "Backup is {} late{\"fg\":\"LR\"}  - {} check{\"b\":true}  it{}" );
check( "the buttons pressed - what every selected character has", [ cp.format_styled_shared( s_late, 10, 14 ),
 cp.format_styled_shared( s_late, 8, 14 ), cp.format_styled_shared( s_late, 4, 4 ) ], [ { fg: "LR" }, { }, { } ] );
// NOTE: Undo in the styled box - its own history, as the browser's knows only the words. "typed" types one character after
// another, each a moment after the last, as a person would.
function typed( history, text, at_ms )
{
   let h = history;
   let t = at_ms;

   for( const ch of text )
   {
      const now = h.states[ h.at ];
      const words = cp.format_styled_text( now.chars );
      const chars = cp.format_styled_edit( now.chars, words.slice( 0, now.start ) + ch + words.slice( now.start ) );

      h = cp.format_history_record( h, chars, now.start + 1, now.start + 1, "type", t );
      t += 100;
   }

   return h;
}

const h_words = typed( cp.format_history( [ ], 0, 0 ), "the node", 0 );

check( "undo - typing joined into bursts, a new one at the first space after a word", h_words.states.map( s => cp.format_styled_text( s.chars ) ),
 [ "", "the", "the node" ] );
check( "- a pause of a second starts another",
 typed( typed( cp.format_history( [ ], 0, 0 ), "ab", 0 ), "cd", 5000 ).states.map( s => cp.format_styled_text( s.chars ) ), [ "", "ab", "abcd" ] );

const h_bold = cp.format_history_record( cp.format_history_select( h_words, 4, 8 ), cp.format_styled_restyle( h_words.states[ 2 ].chars, 4, 8, { flag: "b" } ),
 4, 8, "format", 9000 );

check( "- a format change is a state of its own", [ h_bold.states.length, sty( h_bold.states[ h_bold.at ].chars ) ], [ 4, "the {} node{\"b\":true}" ] );

const u1 = cp.format_history_undo( h_bold );

check( "- undo takes the bold off, the words left, selected as they were", [ sty( u1.state.chars ), u1.state.start, u1.state.end ], [ "the node{}", 4, 8 ] );

const u2 = cp.format_history_undo( u1.history );

check( "- again takes back the last word typed", cp.format_styled_text( u2.state.chars ), "the" );

const r1 = cp.format_history_redo( cp.format_history_redo( u2.history ).history );

check( "- redo puts both back", sty( r1.state.chars ), "the {} node{\"b\":true}" );
check( "- nothing further either way", [ cp.format_history_redo( r1.history ), cp.format_history_undo( cp.format_history( [ ], 0, 0 ) ) ], [ null, null ] );

const red = cp.format_styled_from_shorthand( "a %R%red%N% word" );
const h_red = cp.format_history( red, 5, 5 );
const h_gone = cp.format_history_record( h_red, cp.format_styled_edit( red, "a  word" ), 2, 2, "other", 0 );

check( "- a red word deleted comes back red", sty( cp.format_history_undo( h_gone ).state.chars ), "a {} red{\"fg\":\"R\"}  word{}" );
check( "- something new after an undo drops what could have been redone", cp.format_history_redo( cp.format_history_record(
 u1.history, cp.format_styled_edit( u1.state.chars, "the node!" ), 9, 9, "other", 9500 ) ), null );
check( "- a change that changes nothing only moves the selection", [ cp.format_history_record( h_red, red, 1, 3, "type", 0 ).states.length,
 cp.format_history_record( h_red, red, 1, 3, "type", 0 ).states[ 0 ].start ], [ 1, 1 ] );

check( "trimmed as the words are - styles kept", [ cp.format_styled_text( cp.format_styled_trim( cp.format_styled_edit( [ ], "  hi there \n" ) ) ),
 cp.format_styled_trim( cp.format_styled_edit( [ ], " \n " ) ).length ], [ "hi there", 0 ] );

// NOTE: Desktop notifications (Damon, 2026-10-20) - the chat only, who and where, never what was said.
const n_rooms = [ { room: "0000004", name: "Suite Posts", unread: 0 }, { room: "0000007", name: "/admin/verify-a", unread: 1 },
 { room: "0000009", name: "Ledger Ops", unread: 0 } ];
const n_first = cp.notifications_due( null, n_rooms, [ ], "0000009", "verify-a", true );

check( "notifications - nothing on the first look, only what it saw", [ n_first.notes, n_first.seen.unread[ "0000007" ] ], [ [ ], 1 ] );

const n_later = cp.notifications_due( n_first.seen, [ { room: "0000004", name: "Suite Posts", unread: 3 },
 { room: "0000007", name: "/admin/verify-a", unread: 2 }, { room: "0000009", name: "Ledger Ops", unread: 5 } ],
 [ { room: "0000011", name: "Planning", inviter: "admin" }, { room: "0000012", name: "/verify-a/carol", inviter: "carol" } ], "0000009", "verify-a", true );

check( "- a room's unread up, by its name; a conversation by its person; the open room left to its messages", n_later.notes.slice( 0, 2 ),
 [ { room: "0000004", tag: "room-0000004", title: "Suite Posts", body: "3 new messages" },
 { room: "0000007", tag: "room-0000007", title: "admin", body: "2 new messages" } ] );
check( "- an invitation to a room, and a request for a conversation", n_later.notes.slice( 2 ),
 [ { room: "0000011", invite: true, tag: "invite-0000011", title: "Planning", body: "Invitation from admin" },
 { room: "0000012", invite: true, tag: "invite-0000012", title: "carol", body: "Wants to start a conversation" } ] );
check( "- nothing again for what was seen, or a count that went down", cp.notifications_due( n_later.seen,
 [ { room: "0000004", name: "Suite Posts", unread: 0 } ], [ { room: "0000011", name: "Planning", inviter: "admin" } ], "", "verify-a", true ).notes, [ ] );
check( "- the open room, the tab hidden (nothing read): by how far its total went up, as its unread need not move", [
 cp.notifications_due( { unread: { "0000009": 1 }, total: { "0000009": 40 }, invited: { }, invites_known: true },
 [ { room: "0000009", name: "Ledger Ops", unread: 1, total: 42 } ], [ ], "0000009", "verify-a", true, true ).notes,
 cp.notifications_due( { unread: { "0000009": 1 }, total: { "0000009": 40 }, invited: { }, invites_known: true },
 [ { room: "0000009", name: "Ledger Ops", unread: 1, total: 42 } ], [ ], "0000009", "verify-a", true, false ).notes ],
 [ [ { room: "0000009", tag: "room-0000009", title: "Ledger Ops", body: "2 new messages" } ], [ ] ] );

// NOTE: Invitations are read after the first room list (found testing - every one waiting notified on signing in).
const n_unread_only = cp.notifications_due( null, n_rooms, [ ], "", "verify-a", false );
const n_waiting = cp.notifications_due( n_unread_only.seen, n_rooms, [ { room: "0000011", name: "Planning", inviter: "admin" } ], "", "verify-a", true );

check( "- invitations not yet read, then read: those waiting are not news", [ n_unread_only.notes, n_waiting.notes ], [ [ ], [ ] ] );
check( "- one that comes after, is", cp.notifications_due( n_waiting.seen, n_rooms, [ { room: "0000011", name: "Planning", inviter: "admin" },
 { room: "0000013", name: "Garden", inviter: "carol" } ], "", "verify-a", true ).notes.map( n => n.title ), [ "Garden" ] );

check( "the open room's new messages - who, never what", [ cp.messages_notification( [ { kind: "chat", sender: "admin", text: "secret plans" } ],
 "verify-a", "0000004", "Suite Posts" ), cp.messages_notification( [ { kind: "chat", sender: "admin", text: "a" }, { kind: "chat", sender: "carol", text: "b" },
 { kind: "chat", sender: "admin", text: "c" } ], "verify-a", "0000007", "/admin/verify-a" ) ],
 [ { room: "0000004", tag: "room-0000004", title: "Suite Posts", body: "New message from admin" },
 { room: "0000007", tag: "room-0000007", title: "admin", body: "3 new messages from admin, carol" } ] );
check( "- not for one's own, a system line, or a deleted one", cp.messages_notification( [ { kind: "chat", sender: "verify-a", text: "mine" },
 { kind: "system", sender: "admin", text: "joined" }, { kind: "chat", sender: "admin", text: "  ", deleted: true } ],
 "verify-a", "0000004", "Suite Posts" ), null );
// NOTE: Every read brings the room's pinned messages again (found by review) - not news; nor a line not after where the read began.
check( "- nor a pinned message, brought by every read, nor one not after the read's start", cp.messages_notification( [
 { kind: "chat", sender: "admin", text: "pinned", pinned: true, unique: "20261020120000000" },
 { kind: "chat", sender: "admin", text: "older", unique: "20261020110000000" } ], "verify-a", "0000004", "Suite Posts", "20261020115900000" ), null );
check( "- a new one after the read's start, still", cp.messages_notification( [ { kind: "chat", sender: "admin", text: "new", unique: "20261020120100000" } ],
 "verify-a", "0000004", "Suite Posts", "20261020115900000" ).body, "New message from admin" );

check( "the switch's line, as the browser allows", [ cp.notify_status_text( "on", "ok" ), cp.notify_status_text( "off", "ok" ),
 cp.notify_status_text( "", "ok" ), cp.notify_status_text( "on", "insecure" ), cp.notify_status_text( "on", "blocked" ),
 cp.notify_status_text( "", "unsupported" ), cp.notify_status_text( "off", "dismissed" ) ],
 [ "On - who and where, never what was said.", "Off", "Off", "Not available here - notifications need the node's https address.",
 "Blocked for this site - allow notifications in the browser's site settings, then turn them on here.", "This browser does not offer notifications.",
 "Not turned on - the browser's question was closed without an answer. Turn them on again to be asked." ] );
check( "- and its word in the account menu", [ cp.notify_menu_word( true, "ok" ), cp.notify_menu_word( false, "ok" ),
 cp.notify_menu_word( false, "insecure" ) ], [ "On", "Off", "Unavailable" ] );
check( "- offered once: not asked yet, and the browser can", [ cp.notify_offer_bar( "", "ok" ), cp.notify_offer_bar( "off", "ok" ),
 cp.notify_offer_bar( "", "insecure" ), cp.notify_offer_bar( "", "blocked" ) ], [ true, false, false, false ] );
check( "kept per account, by name", cp.account_storage_key( cp.c_storage_notify_by_name, "verify-a", false ), "cws.notify@verify-a" );

console.log( "" );
console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

process.exit( failures === 0 ? 0 : 1 );

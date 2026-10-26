// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Pure functions for the accounts page - reading the server's replies about accounts,
// and checking what a person types before anything is sent. Loads in the browser as plain
// globals and in Node through "module.exports", so all of it is covered by
// "account_parse_test.js" without a server or a browser. Relies on "is_valid_username( )"
// and "password_strength( )" from "chat_parse.js", which the page loads first.

const c_account_pin_pattern = /^[0-9]{5}$/;

// NOTE: A code - Ian's "secret" - is 12 characters: three, four and three lower case letters
// with a hyphen between each ("qiu-ddxb-cht"), from "create_access_token( )" in
// "ciyam_web_session.cpp".
const c_account_code_pattern = /^[a-z]{3}-[a-z]{4}-[a-z]{3}$/;

const c_account_code_letters = 10;

const c_account_no_seed = "@none";

const c_account_suggestion_prefix = "?";

const c_account_page = "account.html";

const c_account_code_param = "code=";

const c_account_welcome_hash = "welcome";

// NOTE: A code as typed or pasted - in capitals, with spaces, or without its hyphens - in the
// server's own form, or "" if it cannot be one.
function normalise_code( text )
{
   var value = String( text || "" ).toLowerCase( ).replace( /[\s-]+/g, "" );

   if( !/^[a-z]+$/.test( value ) || ( value.length !== c_account_code_letters ) )
      return "";

   return value.substr( 0, 3 ) + "-" + value.substr( 3, 4 ) + "-" + value.substr( 7 );
}

function is_account_pin( text )
{
   return c_account_pin_pattern.test( String( text || "" ) );
}

// NOTE: What "POST /cws/devices?access=<code or PIN>" answered, before any password:
//
//   "<pin> @none"             an account with no username yet - the PIN a code has just made,
//                             or a nominated PIN with no username
//   "<pin> @none <name>"      a nominated PIN whose username admin chose
//   "<pin> @none ?<name>"     a nominated PIN whose username admin only suggested
//   "<device token>"          the PIN already has a password - nothing to set up
//   "Error: ..."              an unknown PIN
//
// The server answers an unknown nominated name that is not a valid username - "55887" - as
// if it were one; such a name is not offered.
function parse_join_reply( reply )
{
   var text = String( reply || "" ).trim( );

   if( text.indexOf( "Error: " ) === 0 )
      return { kind: "error", error: text.substring( 7 ) };

   var parts = text.split( /\s+/ );

   if( ( parts.length === 1 ) && ( parts[ 0 ] !== "" ) )
      return { kind: "claimed" };

   if( ( parts.length < 2 ) || !is_account_pin( parts[ 0 ] ) || ( parts[ 1 ] !== c_account_no_seed ) )
      return { kind: "error", error: "The server's answer was not understood." };

   var result = { kind: "open", pin: parts[ 0 ], username: "", fixed: false };

   var name = ( parts.length > 2 ) ? parts[ 2 ] : "";

   var suggested = ( name.indexOf( c_account_suggestion_prefix ) === 0 );

   if( suggested )
      name = name.substring( 1 );

   if( ( name !== "" ) && is_valid_username( name ) )
   {
      result.username = name;
      result.fixed = !suggested;
   }

   return result;
}

// NOTE: The users review, as text - a line per account, "<pin> <username>", with nothing after
// the space for an account nobody has claimed. admin's own PIN is not in it - admin is not an
// account in the system ODS - so it is added, as "own_name". admin comes first, then everyone
// by name, then the unclaimed by PIN.
function parse_people( text, own_pin, own_name )
{
   var rows = [ ];

   String( text || "" ).split( "\n" ).forEach( function( line )
   {
      var match = line.replace( /\r$/, "" ).match( /^([0-9]{5})(?: (.*))?$/ );

      if( match === null )
         return;

      var name = ( match[ 2 ] || "" ).trim( );

      var row = { pin: match[ 1 ], name: name, you: ( match[ 1 ] === own_pin ) };

      row.status = row.you ? "admin" : ( ( name === "" ) ? "unclaimed" : "active" );

      rows.push( row );
   } );

   if( ( ( own_pin || "" ) !== "" ) && !rows.some( function( row ) { return row.you; } ) )
      rows.push( { pin: own_pin, name: own_name || "", you: true, status: "admin" } );

   var rank = { admin: 0, active: 1, unclaimed: 2 };

   rows.sort( function( a, b )
   {
      if( rank[ a.status ] !== rank[ b.status ] )
         return rank[ a.status ] - rank[ b.status ];

      if( a.name !== b.name )
         return ( a.name < b.name ) ? -1 : 1;

      return ( a.pin < b.pin ) ? -1 : ( ( a.pin > b.pin ) ? 1 : 0 );
   } );

   return rows;
}

// NOTE: The options for "create user" with a PIN admin chooses. Both parts are optional:
// "nominated=" alone gives a random PIN and no username. A "?" makes the username only a
// suggestion.
function nominated_options( pin, username, suggestion )
{
   var value = "";

   if( pin !== "" )
      value += pin + ":";

   if( username !== "" )
      value += ( suggestion ? c_account_suggestion_prefix : "" ) + username;

   return "nominated=" + value;
}

// NOTE: What is wrong with a PIN and username admin is about to nominate, or "". The server
// takes a username already in use, or one that is not valid, without complaint, so they are
// checked here against the people already listed.
function nominate_problem( pin, username, people )
{
   if( ( pin !== "" ) && !is_account_pin( pin ) )
      return "A PIN is 5 digits.";

   if( ( pin !== "" ) && people.some( function( row ) { return row.pin === pin; } ) )
      return "PIN " + pin + " is already in use.";

   if( ( username !== "" ) && !is_valid_username( username ) )
      return "A username is 3 to 12 characters: a-z, 0-9 and single hyphens, starting with a letter.";

   if( ( username !== "" ) && people.some( function( row ) { return row.name === username; } ) )
      return "Someone already has the username " + username + ".";

   return "";
}

// NOTE: What is wrong with a new username and password, or "" - the same rules as the chat's
// change of password: at least "Weak", and typed the same twice.
function join_problem( username, password, confirm, pin )
{
   if( !is_valid_username( username ) )
      return "Choose a username of 3 to 12 characters: a-z, 0-9 and single hyphens, starting with a letter.";

   // NOTE: Rated as the meter rates it, the PIN and username counting for little (review, 2026-10-25).
   var strength = password_strength( password, [ pin, username ].filter( Boolean ) );

   if( strength.level < 1 )
      return password_refusal( strength );

   if( password !== confirm )
      return "The two passwords are not the same.";

   return "";
}

// NOTE: The address in the QR code admin shows: the page, with the code after "#" - a fragment
// never goes to the server, so it is in no log, and a phone's camera opens it straight into
// the Welcome screen with the code filled in. Scanning in the page itself would need the
// camera, which a browser allows only over HTTPS or on "localhost".
function join_url( origin, code )
{
   return String( origin ) + "/" + c_account_page + "#" + c_account_code_param + code;
}

// NOTE: Whether signing in failed because the node has never seen this browser's device token -
// one issued by a node since rebuilt, or by another node at the same address.
function is_unknown_device_error( error )
{
   return /Invalid device identity/.test( String( error || "" ) );
}

// NOTE: The server answers a code or a PIN it does not know - made up, mistyped, or a code already
// used (since ISS-034's fix) - as an invalid session, which reads as something wrong with the
// browser. So that one is said in the page's own words; any other error is passed on as it came.
function refused_join_text( error, mode )
{
   var text = String( error || "" ).replace( /^Error: /, "" );

   if( !/not valid/.test( text ) )
      return text;

   return ( mode === "code" ) ? "That code isn't known, or has already been used." : "That PIN isn't known on this node.";
}

// NOTE: What the address asks for: "#code=<code>" or "#welcome" open the Welcome screen.
function parse_account_hash( hash )
{
   var text = String( hash || "" ).replace( /^#/, "" );

   if( text.indexOf( c_account_code_param ) === 0 )
      return { view: "welcome", code: normalise_code( decodeURIComponent( text.substring( c_account_code_param.length ) ) ) };

   if( text === c_account_welcome_hash )
      return { view: "welcome", code: "" };

   return { view: "", code: "" };
}

// NOTE: "GET /cws/devices" (Ian, 2026-10-06) - this PIN's devices, one per line, "<device> <session>",
// with "*" after a session the server treats as active. "current" is this browser's own device, which
// cannot be removed from here. Anything not of that shape is left out; this browser comes first.
//
// Returns [ { device, session, active, current } ].
function parse_devices( text, current )
{
   var devices = [ ];

   String( text || "" ).split( /\r?\n/ ).forEach( function( line )
   {
      var match = /^([A-Za-z0-9]+)\s+([A-Za-z0-9]+)(\*?)$/.exec( line.trim( ) );

      if( match === null )
         return;

      // NOTE: A device that has signed in as this PIN before but has no session now is listed with twenty zeros for its session
      // (Ian, 2026-10-26 - "known devices", kept in ".devices") - "signed_out".
      devices.push( { device: match[ 1 ], session: match[ 2 ], active: ( match[ 3 ] === "*" ), current: ( match[ 1 ] === current ),
       signed_out: /^0+$/.test( match[ 2 ] ) } );
   } );

   return devices.filter( function( d ) { return d.current; } ).concat( devices.filter( function( d ) { return !d.current; } ) );
}

// NOTE: What a device's row says, and what removing it does (Ian, 2026-10-26): removing one with a session signs it out - it is
// listed as signed out then; removing one signed out blocks it, for this account, for good - its sign in refused as an invalid
// device, and it leaves the list. So two steps, said as two: Sign out, then Block. This browser is neither.
function device_choice( entry )
{
   var name = short_device( entry.device );

   if( entry.current )
      return { state: "This browser", action: "", label: "", ask: "", yes: "", done: "" };

   if( entry.signed_out )
      return { state: "Signed out", action: "block", label: "Block", ask: "Block it? It can never sign in as you again.", yes: "Block it",
       done: "The device " + name + " is blocked - it can't sign in as you again. If it may have your password saved, change your password too." };

   return { state: entry.active ? "Signed in now" : "Not signed in", action: "sign_out", label: "Sign out",
    ask: "Sign it out? It can sign in again with your password.", yes: "Sign it out",
    done: "The device " + name + " was signed out. To stop it signing in again, Block it." };
}

// NOTE: A device token shortened, as the console's log shortens long values.
function short_device( device )
{
   var text = String( device || "" );

   return ( text.length > 12 ) ? text.substr( 0, 6 ) + "…" + text.substr( -4 ) : text;
}

// NOTE: Your keys (Ian, 2026-10-25: locked under a hash of the twelve words, which each browser keeps) - which of the card's
// states, from whether this browser keeps that ("here") and what "GET /cws/user-keys" answered: "none" - nothing anywhere, to
// set up; "lost" - kept here but gone from the node, where the keys were (what a browser keeps cannot make them again - the
// words can); "elsewhere" - on the node but not here; "here"; or "unreadable" - an error, or something that is not locked keys.
const c_keys_locked_pattern = /^UK3:[1-9][0-9]{0,8}:[0-9a-f]{32}:([0-9a-f]{2})+:[0-9a-f]{64}$/;

function keys_state( here, node_text )
{
   var text = String( node_text || "" ).trim( );

   // NOTE: Keys in a form from before 2026-10-25's change ("UK1", "UK2") cannot be opened any more - as none, to set up again.
   if( /^UK[12]:/.test( text ) )
      text = "[none]";

   if( text === "[none]" )
      return here ? "lost" : "none";

   if( !c_keys_locked_pattern.test( text ) )
      return "unreadable";

   return here ? "here" : "elsewhere";
}

// NOTE: Which of the twelve words to check back after setting up - "count" of them, different, in order, "random( )" (0 to 1)
// choosing. Some, not all twelve as setting up a node does (Ian, 2026-10-23: "some checked back").
function keys_check_indexes( random, count )
{
   var all = [ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 ];
   var picked = [ ];

   while( ( picked.length < ( count || 3 ) ) && ( all.length > 0 ) )
      picked.push( all.splice( Math.floor( random( ) * all.length ), 1 )[ 0 ] );

   return picked.sort( function( a, b ) { return a - b; } );
}

// NOTE: Others' public keys, kept with your keys by username, as the console keeps them (".irc_<me>.public.<them>"). A key as
// pasted - in capitals, broken over lines or spaced - as 64 lower-case hex digits, or "" if it cannot be one.
function normalise_public_key( text )
{
   var key = String( text || "" ).replace( /\s+/g, "" ).toLowerCase( );

   return /^[0-9a-f]{64}$/.test( key ) ? key : "";
}

// NOTE: What is wrong with adding "name" and "key" - "" if nothing. "contacts" - those kept; "own_key" - this account's own.
function contact_problem( name, key, contacts, own_key )
{
   var kept = contacts || { };

   if( !is_valid_username( name ) )
      return "Type their username - 3 to 12 characters: a-z, 0-9 and single hyphens, starting with a letter.";

   if( key === "" )
      return "A public key is 64 letters and digits, 0-9 and a-f - copy all of it.";

   if( key === own_key )
      return "That's your own public key - theirs is shown on their My account.";

   if( kept.hasOwnProperty( name ) )
      return "You have a key for " + name + " already - remove it first to change it.";

   var other = Object.keys( kept ).filter( function( kept_name ) { return kept[ kept_name ] === key; } )[ 0 ];

   if( other !== undefined )
      return "That key is saved for " + other + " already.";

   return "";
}

// NOTE: The keys kept, as the card lists them - by name, each key shortened ("short_public_key( )").
function contact_rows( contacts )
{
   var kept = contacts || { };

   return Object.keys( kept ).sort( ).map( function( name ) { return { name: name, key: kept[ name ], short: short_public_key( kept[ name ] ) }; } );
}

function short_public_key( key )
{
   var text = String( key || "" );

   return ( text.length > 16 ) ? text.substr( 0, 8 ) + "…" + text.substr( -8 ) : text;
}

// NOTE: My account's sections, in the side menu's order (Damon, 2026-10-26 - one card each had made the page long, and the
// profile's fields to come have a place now). Each is "#<key>" in the address.
const c_account_sections = [
   { key: "profile", title: "Profile" },
   { key: "keys", title: "Your keys" },
   { key: "contacts", title: "Contacts" },
   { key: "devices", title: "Devices" },
   { key: "notify", title: "Notifications" },
   { key: "password", title: "Password" }
];

function account_sections( )
{
   return c_account_sections.map( function( section ) { return { key: section.key, title: section.title }; } );
}

// NOTE: The section an address asks for - "#mine", as the chat and Home have opened the page since before the sections, or
// anything else, is the first.
function account_section( hash )
{
   var wanted = String( hash || "" ).replace( /^#/, "" );

   return c_account_sections.some( function( section ) { return section.key === wanted; } ) ? wanted : c_account_sections[ 0 ].key;
}

// NOTE: The room keys kept with the keys ("rooms", by room number) - which rooms, for Your keys; each is made, shared and
// removed in the chat.
function room_keys_text( rooms )
{
   var numbers = Object.keys( rooms || { } ).filter( function( room ) { return /^[0-9]{7}$/.test( room ); } ).sort( );

   if( numbers.length === 0 )
      return "None yet. A room's owner makes one in the chat, under Room Details, and sends it to each member.";

   var named = numbers.map( function( room ) { return "#" + room; } );
   var listed = ( named.length === 1 ) ? named[ 0 ] : named.slice( 0, -1 ).join( ", " ) + " and " + named[ named.length - 1 ];

   return "Kept for " + ( ( named.length === 1 ) ? "room " : "rooms " ) + listed + ". Made, shared and removed in the chat,"
    + " under Room Details.";
}

if( typeof module !== "undefined" )
{
   module.exports = {
      account_sections: account_sections,
      account_section: account_section,
      room_keys_text: room_keys_text,
      parse_devices: parse_devices,
      device_choice: device_choice,
      short_device: short_device,
      normalise_code: normalise_code,
      is_account_pin: is_account_pin,
      parse_join_reply: parse_join_reply,
      parse_people: parse_people,
      nominated_options: nominated_options,
      nominate_problem: nominate_problem,
      join_problem: join_problem,
      join_url: join_url,
      is_unknown_device_error: is_unknown_device_error,
      refused_join_text: refused_join_text,
      parse_account_hash: parse_account_hash,
      keys_state: keys_state,
      keys_check_indexes: keys_check_indexes,
      normalise_public_key: normalise_public_key,
      contact_problem: contact_problem,
      contact_rows: contact_rows,
      short_public_key: short_public_key
   };
}

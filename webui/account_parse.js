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
function join_problem( username, password, confirm )
{
   if( !is_valid_username( username ) )
      return "Choose a username of 3 to 12 characters: a-z, 0-9 and single hyphens, starting with a letter.";

   if( password_strength( password ).level < 1 )
      return "Choose a password of at least 7 characters.";

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

if( typeof module !== "undefined" )
{
   module.exports = {
      normalise_code: normalise_code,
      is_account_pin: is_account_pin,
      parse_join_reply: parse_join_reply,
      parse_people: parse_people,
      nominated_options: nominated_options,
      nominate_problem: nominate_problem,
      join_problem: join_problem,
      join_url: join_url,
      is_unknown_device_error: is_unknown_device_error,
      parse_account_hash: parse_account_hash
   };
}

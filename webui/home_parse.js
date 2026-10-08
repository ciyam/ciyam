// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Pure functions for the Home app - the node's state before anyone signs in, the connection's
// security, unlock keys, admin's Overview and Logs, and which apps and sections a person sees. Loads
// in the browser as plain globals and in Node through "module.exports", so all of it is covered by
// "home_parse_test.js" without a server or a browser. What a member's Home shows of the chat - the
// invitations, requests and announcements waiting - comes from "chat_parse.js" as it is.

const c_home_page = "home.html";

const c_home_state_ready = "ready";
const c_home_state_locked = "locked";
const c_home_state_new = "new";

const c_home_security_none = "none";
const c_home_security_encrypted = "encrypted";
const c_home_security_quantum = "quantum";

// NOTE: How "/system" names the node before its version (Ian, "c1e553a5") - "*CIYAM*" has no
// identity yet, ":CIYAM:" has one but is locked, "CIYAM" is ready.
const c_home_system_names = { "CIYAM": c_home_state_ready, ":CIYAM:": c_home_state_locked, "*CIYAM*": c_home_state_new };

// NOTE: The key exchange that makes a connection quantum-resistant - ML-KEM, alone or in a hybrid
// such as "X25519MLKEM768".
const c_home_quantum_group = /MLKEM/i;

const c_home_no_cipher = "(NONE)";

// NOTE: An unlock key is 11 random characters encoded as 15 base64 URL characters, shown as three
// groups of five - "NUJ5M-mk4eV-hWNWY" ("docs/system_identity.md"). The server finds the hyphens at
// fixed places, since "-" is itself a base64 URL character and may be part of a group.
const c_home_key_group = "[A-Za-z0-9_-]{5}";

const c_home_key_pattern = new RegExp( "^(" + c_home_key_group + ")-(" + c_home_key_group + ")-(" + c_home_key_group + ")$" );

const c_home_key_spaced = new RegExp( "^(" + c_home_key_group + ")\\s+(" + c_home_key_group + ")\\s+(" + c_home_key_group + ")$" );

const c_home_key_bare = new RegExp( "^(" + c_home_key_group + ")(" + c_home_key_group + ")(" + c_home_key_group + ")$" );

const c_home_log_lines = 200;

const c_home_admin_sections = [
   { key: "overview", title: "Overview" },
   { key: "keys", title: "Unlock keys" },
   { key: "logs", title: "Logs" },
   { key: "people", title: "People" }
];

// NOTE: "/system" as text - "<name> <version>", then since "441099e0" how this connection is
// encrypted: "(NONE)" over plain HTTP, else "<cipher suite> (<key exchange>)":
//
//   "CIYAM 0.0.0 (NONE)"
//   ":CIYAM: 0.0.0 TLS_AES_256_GCM_SHA384 (x25519)"
//   "CIYAM 0.0.0 TLS_AES_128_GCM_SHA256 (X25519MLKEM768)"
//
// Returns { state, version, cipher, group, security }. "state" is "" for anything that is not a
// node's answer; "security" is "" when the server does not say (one older than "441099e0").
function parse_system( text )
{
   var result = { state: "", version: "", cipher: "", group: "", security: "" };

   var match = String( text || "" ).trim( ).match( /^(\*CIYAM\*|:CIYAM:|CIYAM) +(\S+)(?: +(.*))?$/ );

   if( match === null )
      return result;

   result.state = c_home_system_names[ match[ 1 ] ];
   result.version = match[ 2 ];

   var rest = ( match[ 3 ] || "" ).trim( );

   if( rest === "" )
      return result;

   if( rest.toUpperCase( ) === c_home_no_cipher )
   {
      result.security = c_home_security_none;

      return result;
   }

   var cipher = rest.match( /^(\S+)(?: +\(([^)]*)\))?$/ );

   if( cipher === null )
      return result;

   result.cipher = cipher[ 1 ];
   result.group = cipher[ 2 ] || "";
   result.security = c_home_quantum_group.test( result.group ) ? c_home_security_quantum : c_home_security_encrypted;

   return result;
}

// NOTE: How the connection is described - "pill" in a header, "heading" on admin's Overview. Both say
// how the node sees it: behind a proxy it sees the proxy's connection, not the browser's. Empty when
// the node did not say.
function security_text( security )
{
   if( security === c_home_security_none )
      return { pill: "Not encrypted", heading: "Connection: not encrypted" };

   if( security === c_home_security_encrypted )
      return { pill: "Encrypted connection", heading: "Connection: encrypted" };

   if( security === c_home_security_quantum )
      return { pill: "Quantum-resistant connection", heading: "Connection: encrypted, quantum-resistant" };

   return { pill: "", heading: "" };
}

// NOTE: The master password is a rescue for someone on the node's own network (Ian, 2026-10-03), so an
// unencrypted connection is said plainly before it is typed. An unknown one is not called unencrypted.
function warns_master_password( security )
{
   return ( security === c_home_security_none );
}

// NOTE: Which screen a visitor arrives at, from the node's state - the sign in, Unlock, or Set up; and
// "unreachable" when "/system" did not answer as a node.
function arriving_screen( state )
{
   if( state === c_home_state_ready )
      return "signin";

   if( state === c_home_state_locked )
      return "unlock";

   if( state === c_home_state_new )
      return "setup";

   return "unreachable";
}

// NOTE: A sign in refused while the node is locked, said plainly. A locked node should let people sign in, so
// that one of them can use a key (Ian, 2026-10-03); today it cannot start the session - "Was unable to start a
// web session" (2026-10-07, for Ian). Any other refusal is left as it was - "" here.
function locked_sign_in_text( error )
{
   if( !/unable to start a web session/i.test( String( error || "" ) ) )
      return "";

   return "The node is not letting anyone sign in while it is locked - a fault being fixed. Until then it can only be"
    + " unlocked on the node itself, from its terminal.";
}

// NOTE: An unlock key as typed or pasted - with spaces between the groups, as the docs show it, or
// none at all - in the server's own form, or "" if it cannot be one. Case is kept: the characters are
// base64, so "a" and "A" differ.
function normalise_unlock_key( text )
{
   var value = String( text || "" ).trim( );

   var match = value.match( c_home_key_pattern ) || value.match( c_home_key_spaced ) || value.match( c_home_key_bare );

   if( match === null )
      return "";

   return match[ 1 ] + "-" + match[ 2 ] + "-" + match[ 3 ];
}

// NOTE: How many unlock keys this browser has made on this node - kept by Home, since the node cannot
// list them (each is a file named by its own hash). Anything that is not a count is none.
function parse_key_count( stored )
{
   var count = parseInt( String( stored || "" ), 10 );

   return ( isNaN( count ) || ( count < 0 ) ) ? 0 : count;
}

// NOTE: The people figure on admin's Overview, from "parse_people( )" in "account_parse.js" - admin is
// not counted.
function people_summary( rows )
{
   var summary = { active: 0, unclaimed: 0 };

   ( rows || [ ] ).forEach( function( row )
   {
      if( row.status === "active" )
         ++summary.active;
      else if( row.status === "unclaimed" )
         ++summary.unclaimed;
   } );

   return summary;
}

// NOTE: "/uptime" - the time since the server started, already in words ("41m 45s",
// "1w 2d 3h 4m 5s"). "" for anything else, such as an error.
function parse_uptime( text )
{
   var value = String( text || "" ).trim( );

   return /^([0-9]+[wdhms] ?)+$/.test( value ) ? value : "";
}

// NOTE: The uptime as the Overview says it - the two largest parts that are not nothing, in words:
// "1w 0d 3h 4m 5s" is "1 week 3 hours", "41m 45s" is "41 minutes 45 seconds". "" for anything else.
function uptime_words( text )
{
   var names = { w: "week", d: "day", h: "hour", m: "minute", s: "second" };

   var parts = parse_uptime( text ).split( " " ).filter( function( part )
   {
      return ( part !== "" ) && ( parseInt( part, 10 ) > 0 );
   } );

   if( ( parts.length === 0 ) && ( parse_uptime( text ) !== "" ) )
      return "just started";

   return parts.slice( 0, 2 ).map( function( part )
   {
      var count = parseInt( part, 10 );

      return count + " " + names[ part.charAt( part.length - 1 ) ] + ( ( count === 1 ) ? "" : "s" );
   } ).join( " " );
}

// NOTE: "GET /cws/logs" - the logs present, one name a line ("script", "server", "update").
function parse_log_names( text )
{
   var names = [ ];

   String( text || "" ).split( "\n" ).forEach( function( line )
   {
      var name = line.trim( );

      if( /^[a-z0-9_]+$/.test( name ) && ( names.indexOf( name ) < 0 ) )
         names.push( name );
   } );

   return names;
}

// NOTE: Tints a log line - "error" or "warn" - so they stand out in a long log; "" for the rest.
function log_line_kind( line )
{
   var text = String( line || "" );

   if( /\b(error|failed|failure|exception|fatal)\b/i.test( text ) )
      return "error";

   if( /\b(warning|warn)\b/i.test( text ) )
      return "warn";

   return "";
}

// NOTE: What the Logs section shows - the lines holding "filter" (any case), the last "count" of them,
// newest at the end as in the file. Returns { lines, total } - "total" being how many matched.
function log_view( lines, filter, count )
{
   var wanted = String( filter || "" ).trim( ).toLowerCase( );

   var matched = ( lines || [ ] ).filter( function( line )
   {
      return ( wanted === "" ) || ( String( line ).toLowerCase( ).indexOf( wanted ) >= 0 );
   } );

   var limit = ( count > 0 ) ? count : c_home_log_lines;

   return { lines: matched.slice( Math.max( 0, matched.length - limit ) ), total: matched.length };
}

// NOTE: The apps in the switcher. The console is admin's - and anyone's on a development system - and
// never on a phone (parked 2026-10-05).
function home_apps( is_admin, is_development, is_phone )
{
   var apps = [
      { key: "home", title: "Home", page: c_home_page },
      { key: "chat", title: "Chat", page: "chat.html" },
      { key: "account", title: "My account", page: "account.html" }
   ];

   if( ( is_admin || is_development ) && !is_phone )
      apps.push( { key: "console", title: "Console", page: "console.html" } );

   return apps;
}

// NOTE: The node's own sections, for admin only - a member's Home is one page.
function home_sections( is_admin )
{
   return is_admin ? c_home_admin_sections.slice( ) : [ ];
}

if( typeof module !== "undefined" )
{
   module.exports = {
      parse_system: parse_system,
      security_text: security_text,
      warns_master_password: warns_master_password,
      arriving_screen: arriving_screen,
      locked_sign_in_text: locked_sign_in_text,
      normalise_unlock_key: normalise_unlock_key,
      parse_key_count: parse_key_count,
      people_summary: people_summary,
      parse_uptime: parse_uptime,
      uptime_words: uptime_words,
      parse_log_names: parse_log_names,
      log_line_kind: log_line_kind,
      log_view: log_view,
      home_apps: home_apps,
      home_sections: home_sections
   };
}

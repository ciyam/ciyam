// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Pure functions for the Home app - the node's state before anyone signs in, the connection's
// security, unlock keys, admin's Overview and Logs, and which apps and sections a person sees. Loads
// in the browser as plain globals and in Node through "module.exports", so all of it is covered by
// "home_parse_test.js" without a server or a browser. What a member's Home shows of the chat - the
// invitations, requests and announcements waiting - is read by "chat_parse.js", which the page loads first.

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
   { key: "health", title: "Health" },
   { key: "people", title: "People" },
   { key: "keys", title: "Unlock keys" },
   { key: "alerts", title: "Alerts" },
   { key: "logs", title: "Logs" }
];

// NOTE: Home's own tab, which the other apps can find and reuse - as "c_tab_chat" and the rest in
// "chat_parse.js".
const c_tab_home = "ciyam-home";

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

// NOTE: A sign in refused while the node is locked, said plainly - for any refusal when the node is known to be locked
// ("is_locked"; it answers in more than one way, 2026-10-10). A locked node should let people sign in, so
// that one of them can use a key (Ian, 2026-10-03); today it cannot start the session - "Was unable to start a
// web session" (2026-10-07, for Ian). Any other refusal is left as it was - "" here.
function locked_sign_in_text( error, is_locked )
{
   if( ( is_locked !== true ) && !/unable to start a web session/i.test( String( error || "" ) ) )
      return "";

   return "The node is not letting anyone sign in while it is locked - a fault being fixed. Until then, recover it with"
    + " its twelve words, below, or unlock it on the node itself.";
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

// NOTE: How many unlock keys the node holds - "system_variable @num_unlock_keys", which admin may ask on any node (Ian,
// "889b160f", 2026-10-10). Counted by the node, so it is the same in every browser and goes down as keys are used - it
// replaced a count this browser kept of the keys it had made. null for anything that is not a count.
const c_home_keys_request = "system_variable @num_unlock_keys";

function parse_key_count( reply )
{
   var value = String( reply === null || reply === undefined ? "" : reply ).trim( );

   return /^[0-9]+$/.test( value ) ? parseInt( value, 10 ) : null;
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

// NOTE: The uptime as the Overview says it - the two largest parts that are not nothing, in words, to the minute: seconds
// are left out, as Home refreshes once a minute (Ian, 2026-10-11). "1w 0d 3h 4m 5s" is "1 week 3 hours", "41m 45s" is
// "41 minutes", under a minute "just started". "" for anything else.
function uptime_words( text )
{
   var names = { w: "week", d: "day", h: "hour", m: "minute" };

   var parts = parse_uptime( text ).split( " " ).filter( function( part )
   {
      return ( part !== "" ) && ( parseInt( part, 10 ) > 0 ) && ( part.charAt( part.length - 1 ) !== "s" );
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

// NOTE: "GET /cws/logs/<name>" as text - its last lines ("lines=<n>", ten unless asked; "lines=0" all of it - Ian,
// "3d6a5820"), a line each; the last line's break leaves no line. An empty log answers "[none]".
function parse_log_lines( text )
{
   if( String( text || "" ).trim( ) === "[none]" )
      return [ ];

   var lines = String( text || "" ).split( "\n" ).map( function( line ) { return line.replace( /\r$/, "" ); } );

   if( ( lines.length > 0 ) && ( lines[ lines.length - 1 ] === "" ) )
      lines.pop( );

   return lines;
}

// NOTE: The options asking a log for its last "count" lines - every line for none ("lines=0").
function log_options( count )
{
   return "lines=" + ( ( count > 0 ) ? Math.floor( count ) : 0 );
}

// NOTE: The People figure's line on admin's Overview.
function people_note( summary )
{
   if( summary.unclaimed > 0 )
      return counted( summary.unclaimed, "code", "codes" ) + " not yet claimed";

   return "Every code claimed";
}

// NOTE: The Overview's People card - who has not used the code admin gave them.
function people_waiting_text( summary )
{
   if( summary.unclaimed === 0 )
      return "Everyone you added has set up their account.";

   return ( ( summary.unclaimed === 1 ) ? "1 person hasn't" : summary.unclaimed + " people haven't" ) + " used their code yet.";
}

// NOTE: Making an unlock key refused, said plainly - the node makes one every three seconds at most
// ("unlock_create_allowed( )" in "ciyam_base.cpp"). Anything else is passed on as it came.
function key_error_text( error )
{
   var text = String( error || "" ).replace( /^Error: /, "" );

   if( /too quickly/i.test( text ) )
      return "Keys are made one at a time, a few seconds apart - wait a moment, then make another.";

   return text;
}

// NOTE: Whether the node's identity is held encrypted - "system_variable @sid_secure", which admin may ask on any node (Ian,
// "e50db383", 2026-10-12): "1" if it is, nothing if not. Without it there are no unlock keys - a pure development or
// demo-only node. null when the node did not answer, which is not taken as either.
const c_home_secure_request = "system_variable @sid_secure";

const c_home_keys_unavailable = "This node's identity isn't held encrypted - a development or demo node - so it has no unlock"
 + " keys: there is nothing to unlock.";

// NOTE: A variable with nothing in it is answered "[okay]", not an empty reply - so on a node without a secure identity
// "[okay]" is the "no" (Ian, 2026-10-13: unlock keys were offered there, and failed).
function parse_secure( reply )
{
   if( ( reply === null ) || ( reply === undefined ) )
      return null;

   var value = String( reply ).trim( );

   if( value === "1" )
      return true;

   return ( ( value === "" ) || ( value === "[okay]" ) ) ? false : null;
}

// NOTE: The unlock keys the node holds, as admin's Overview says it - each is used up when it unlocks the node.
function keys_note( count )
{
   if( count === null )
      return "The node did not say how many unlock keys it holds.";

   if( count === 0 )
      return "None left. Without one, a restart leaves the node locked until you reach it - make a few.";

   return counted( count, "unlock key", "unlock keys" ) + " left. Each works once - make more before they run out.";
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

// NOTE: A server log line's time and its words - "[2026-10-09 00:52:14] [000003] [general] session error: ..." - or the
// whole line as its words when it is not of that shape.
function log_line_parts( line )
{
   var text = String( line || "" );

   var match = text.match( /^\[(\d{4}-\d\d-\d\d \d\d:\d\d:\d\d)\]\s+\[\d+\]\s+\[[^\]]*\]\s*(.*)$/ );

   return match ? { time: match[ 1 ], text: match[ 2 ] } : { time: "", text: text };
}

// NOTE: The errors and warnings in a log since the server last started - its last "server starting" line - for the
// Overview (Damon, 2026-10-09: what can be shown of R2's warnings before Ian's combined request). "since_start" is
// false when no start is among the lines read, so the count is only of those. "latest" is the newest of them.
function log_problems( lines )
{
   var found = problem_lines( lines );

   var result = { errors: 0, warnings: 0, latest: "", since_start: found.since_start };

   found.lines.forEach( function( line )
   {
      if( log_line_kind( line ) === "error" )
         ++result.errors;
      else
         ++result.warnings;

      result.latest = line;
   } );

   return result;
}

// NOTE: The problem lines themselves - those "log_line_kind( )" calls an error or a warning, after the last "server
// starting", or among every line read when no start is. The Overview counts these and "Show them" shows these, so the
// two always agree (2026-10-12: it filtered by one word, "error" or "warn", which missed "failed" and the like, hid the
// warnings when there were errors, and showed lines from before the restart).
function problem_lines( lines )
{
   var all = lines || [ ];
   var start = -1;

   all.forEach( function( line, i )
   {
      if( /\]\s*server starting\b/.test( line ) )
         start = i;
   } );

   return { lines: all.slice( start + 1 ).filter( function( line ) { return log_line_kind( line ) !== ""; } ), since_start: ( start >= 0 ) };
}

function log_problems_text( problems )
{
   var when = problems.since_start ? " since the restart" : " in the log's latest lines";

   if( ( problems.errors === 0 ) && ( problems.warnings === 0 ) )
      return "No errors or warnings" + when;

   var parts = [ ];

   if( problems.errors > 0 )
      parts.push( counted( problems.errors, "error", "errors" ) );

   if( problems.warnings > 0 )
      parts.push( counted( problems.warnings, "warning", "warnings" ) );

   return parts.join( " and " ) + when;
}

// NOTE: How many messages a room keeps - its queue's limit (9,000 since 2026-10-01, Ian). A full room drops its oldest
// for each new one since "@qs_pf_irc" (2026-10-11, "IRC Domain Model"). The node does not report the limit to a page,
// so it is named here.
const c_home_room_limit = 9000;

const c_home_room_filling = 0.8;

// NOTE: The rooms in a listing ("derive_room_list( )" in "chat_parse.js") at or past "c_home_room_filling" of the
// limit, the fullest first - only rooms the one asking is in, as the listing has no others.
function rooms_filling( rooms )
{
   return rooms_fullest( rooms, Infinity ).filter( function( room )
   {
      return room.total >= c_home_room_limit * c_home_room_filling;
   } );
}

// NOTE: The fullest "count" rooms in a listing, filling or not - Health's bars - in "rooms_filling( )"'s shape.
function rooms_fullest( rooms, count )
{
   return ( rooms || [ ] ).filter( function( entry )
   {
      return entry.kind === "room";
   } ).map( function( entry )
   {
      return { room: entry.room, name: entry.name || ( "#" + entry.room ), total: entry.total,
       percent: Math.min( 100, Math.floor( 100 * entry.total / c_home_room_limit ) ) };
   } ).sort( function( a, b ) { return b.total - a.total; } ).slice( 0, count );
}

function rooms_filling_text( filling )
{
   if( filling.length === 0 )
      return "No room is filling up";

   return ( filling.length === 1 ? "1 room is" : filling.length + " rooms are" ) + " filling up";
}

// NOTE: One filling room in words - "General - 7,412 of 9,000 messages (82%)", or full.
function room_filling_line( room )
{
   var of = room.total.toLocaleString( "en-AU" ) + " of " + c_home_room_limit.toLocaleString( "en-AU" ) + " messages";

   // NOTE: A full room drops its oldest message for each new one since Ian's "@qs_pf_irc" (2026-10-11) - it refused them.
   return room.name + " - " + ( room.total >= c_home_room_limit ? "full, " + of + " - the oldest make way for new ones" : of + " (" + room.percent + "%)" );
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

// NOTE: The apps in Home's rail, each with the tab "open_app_tab( )" finds it by when one is asked for. The
// accounts page is "My account" for everyone - admin's People is one of Home's sections since 2026-10-11. The
// console is admin's - and anyone's on a development system - and never on a phone (parked 2026-10-05).
function home_apps( is_admin, is_development, is_phone )
{
   var apps = [
      { key: "home", title: "Home", page: c_home_page, tab: c_tab_home },
      { key: "chat", title: "Chat", page: "chat.html", tab: "ciyam-chat" },
      { key: "account", title: "My account", page: "account.html", tab: "ciyam-accounts" }
   ];

   if( ( is_admin || is_development ) && !is_phone )
      apps.push( { key: "console", title: "Console", page: "console.html", tab: "ciyam-console" } );

   return apps;
}

// NOTE: The node's own sections, for admin only - a member's Home is one page.
function home_sections( is_admin )
{
   return is_admin ? c_home_admin_sections.slice( ) : [ ];
}

// NOTE: Where Home opens - admin on the node's Overview, which carries what needs them too ("admin is a
// member too", the design); a member on their Home. "asked" is the address's "#...", kept when it is one of
// the person's own sections.
function home_section( is_admin, asked )
{
   var wanted = String( asked || "" ).replace( /^#/, "" );

   var own = home_sections( is_admin ).map( function( section ) { return section.key; } );

   if( own.indexOf( wanted ) >= 0 )
      return wanted;

   return is_admin ? own[ 0 ] : "home";
}

function role_text( is_admin )
{
   return is_admin ? "Administrator" : "Member";
}

// ====================================================================
// A member's Home - build step 5. These use the chat's own reading of the node ("chat_parse.js":
// "visible_rooms( )", "is_dm_name( )", "dm_request_text( )"), so Home and the chat never disagree.
// ====================================================================

// NOTE: What the Chat tile and the menu's badge say, from the lobby listing's rooms and the invitations still
// waiting ("pending_invitations( )"). A message request is an invitation to a direct message room. The badge
// is what the chat's own counts - "unread_elsewhere( )" with no room open.
function chat_summary( rooms, invitations, is_admin )
{
   var summary = { unread: 0, rooms: 0, requests: 0, invitations: 0, badge: 0 };

   visible_rooms( rooms, is_admin ).forEach( function( entry )
   {
      if( entry.unread > 0 )
      {
         summary.unread += entry.unread;
         ++summary.rooms;
      }
   } );

   ( invitations || [ ] ).forEach( function( invite )
   {
      if( is_dm_name( invite.name ) )
         ++summary.requests;
      else
         ++summary.invitations;
   } );

   summary.badge = summary.unread + summary.requests + summary.invitations;

   return summary;
}

function counted( count, one, many )
{
   return count + " " + ( ( count === 1 ) ? one : many );
}

// NOTE: The Chat tile's line - "3 unread in 2 rooms · 1 message request", or that nothing is new.
function chat_summary_text( summary )
{
   var parts = [ ];

   if( summary.unread > 0 )
      parts.push( summary.unread + " unread in " + counted( summary.rooms, "room", "rooms" ) );

   if( summary.requests > 0 )
      parts.push( counted( summary.requests, "message request", "message requests" ) );

   if( summary.invitations > 0 )
      parts.push( counted( summary.invitations, "room invitation", "room invitations" ) );

   return ( parts.length > 0 ) ? parts.join( " · " ) : "Nothing new";
}

// NOTE: "Needs you" - each invitation waiting, said as the chat says it, a message request first by
// kind. They are answered in the chat.
function needs_you( invitations, me )
{
   return ( invitations || [ ] ).map( function( invite )
   {
      var request = is_dm_name( invite.name );

      return {
         kind: request ? "request" : "invitation",
         text: request ? dm_request_text( invite.inviter, invite.name, me ) : ( invite.inviter + " invited you to " + invite.name ),
         detail: request ? "Message request" : "Room invitation",
         inviter: invite.inviter,
         room: invite.room
      };
   } );
}

// NOTE: A device token shortened for a list - the first six and last four characters, as the accounts page
// shows them; the whole token is its title.
function short_device( device )
{
   var text = String( device || "" );

   return ( text.length > 12 ) ? text.substr( 0, 6 ) + "…" + text.substr( -4 ) : text;
}

// NOTE: "Your account"'s devices, from "parse_devices( )" in "account_parse.js" - this browser first.
function device_rows( devices )
{
   return ( devices || [ ] ).map( function( entry )
   {
      return {
         label: entry.current ? "This browser" : short_device( entry.device ),
         title: entry.device,
         state: ( entry.current || entry.active ) ? "Signed in now" : "Not signed in",
         active: !!( entry.current || entry.active ),
         current: !!entry.current
      };
   } );
}

// NOTE: How many devices My account lists - this browser and three more; the rest are a count, the whole list
// a link away (Damon, 2026-10-08: test runs had left fifteen).
const c_home_devices_shown = 4;

// NOTE: The devices My account lists, from "device_rows( )" - this browser first, then those signed in now, then
// the rest, as many as "limit" (or the default) in all. Returns { rows, more } - "more" being how many are left.
function devices_shown( rows, limit )
{
   var count = ( limit > 0 ) ? limit : c_home_devices_shown;

   var ordered = ( rows || [ ] ).filter( function( row ) { return row.current; } )
    .concat( ( rows || [ ] ).filter( function( row ) { return !row.current && row.active; } ) )
    .concat( ( rows || [ ] ).filter( function( row ) { return !row.current && !row.active; } ) );

   return { rows: ordered.slice( 0, count ), more: Math.max( 0, ordered.length - count ) };
}

// NOTE: The line beneath a shortened list.
function more_devices_text( more )
{
   return ( more > 0 ) ? "and " + counted( more, "more device", "more devices" ) : "";
}

// ====================================================================
// Health (Damon, 2026-10-12 - "Home App Scope", Health): the Overview shows only what needs you, Health the detail
// ====================================================================

// NOTE: Whether the node says it is for development or demo - "system_variable @system_is_for_devt" and "..._demo", which
// admin may ask on any node (Ian, "fe66af9d"): "1" or nothing. null when not answered.
const c_home_devt_request = "system_variable @system_is_for_devt";
const c_home_demo_request = "system_variable @system_is_for_demo";

function parse_flag( reply )
{
   return parse_secure( reply );
}

// NOTE: The node's kind, in words, for Health.
function node_kind_text( devt, demo )
{
   if( devt === true )
      return "Development - raw protocol open to admin";

   if( demo === true )
      return "Demo";

   if( ( devt === false ) && ( demo === false ) )
      return "Standard";

   return "Not reported";
}

// NOTE: What needs you, from what the node said - for the Overview's list and Health's count alike, so the two always agree.
// "facts": { secure, keys, problems (from "log_problems( )"), filling (from "rooms_filling( )") }; anything the node did not
// say (null) raises nothing. Each: { key, level ("problem" or "warn"), title, detail, action, target } - problems first.
// A plain HTTP connection is not among them: on one's own network it is normal - Health says so ("worth knowing").
function health_issues( facts )
{
   var issues = [ ];
   var problems = facts.problems;

   if( problems && ( ( problems.errors + problems.warnings ) > 0 ) )
   {
      var when = problems.since_start ? " since the restart" : " in the log's latest lines";
      var latest = log_line_parts( problems.latest );
      var time = latest.time.replace( /^\d{4}-\d\d-\d\d /, "" );

      issues.push( { key: "log", level: ( problems.errors > 0 ) ? "problem" : "warn",
       title: ( ( problems.errors > 0 ) ? counted( problems.errors, "error", "errors" ) : counted( problems.warnings, "warning", "warnings" ) ) + when,
       detail: ( time !== "" ? time + " " : "" ) + latest.text, action: "Show them", target: "problems" } );
   }

   ( facts.filling || [ ] ).slice( 0, 3 ).forEach( function( room )
   {
      var full = ( room.total >= c_home_room_limit );

      issues.push( { key: "room:" + room.room, level: full ? "problem" : "warn",
       title: room.name + ( full ? " is full" : " is " + room.percent + "% full" ),
       detail: room.total.toLocaleString( "en-AU" ) + " of " + c_home_room_limit.toLocaleString( "en-AU" ) + " messages - the oldest are dropped.",
       action: "Rooms", target: "health" } );
   } );

   if( ( facts.secure !== false ) && ( facts.keys === 0 ) )
      issues.push( { key: "keys", level: "warn", title: "No unlock keys left",
       detail: "After a restart, only the master password unlocks the node.", action: "Make a key", target: "keys" } );

   return issues.filter( function( issue ) { return issue.level === "problem"; } )
    .concat( issues.filter( function( issue ) { return issue.level !== "problem"; } ) );
}

function health_heading( issues )
{
   return ( issues.length === 1 ) ? "1 thing needs you" : issues.length + " things need you";
}

// ====================================================================
// Alerts (Damon, 2026-10-12 - "Home App Scope", Notifications): ntfy on admin's phone
// ====================================================================

// NOTE: The node's own ntfy topic - where it sends its alerts - and a test sent to it. Both are raw protocol (Ian's
// "ntfy_topic" and "ntfy_send"), so a development node only until he lets admin ask for them on any node. The title is
// quoted whole - quoted after the "=" it is cut short and the request times out (2026-10-07).
const c_home_ntfy_topic_request = "ntfy_topic";
const c_home_ntfy_test_request = "ntfy_send \"-title=Test alert\" -tags=white_check_mark \"From Home - this phone gets the node alerts.\"";

// NOTE: How the ntfy server's address looks, for the box it is typed into - the machine this page came from, on ntfy's
// port beside it. Only an example: where ntfy runs is the owner's to say.
function ntfy_server_example( hostname )
{
   return "http://" + ( String( hostname || "" ).trim( ) || "<address>" ) + ":8090";
}

// ====================================================================
// Setting up a new node (Damon and Ian, 2026-10-12 - "Home App Scope", Setting up a new node)
// ====================================================================

// NOTE: What Enter does in setting up's two password fields (Damon, 2026-10-13): from the first, on to "Type it again" once
// the password is long enough; from the second, on to "Make my twelve words" once the two match - focus only, nothing sent,
// as that button is the node's first request. Otherwise what is wrong: "short" or "different".
function setup_password_enter( field, password, again, long_enough )
{
   if( !long_enough )
      return { focus: "", problem: "short" };

   if( field === "new" )
      return { focus: "setup_password_confirm", problem: "" };

   if( password !== again )
      return { focus: "", problem: "different" };

   return { focus: "setup_make_words", problem: "" };
}

// NOTE: The still-alive token (Ian, 2026-10-13 - "637f4e60", "944901f7"): the first twelve hex of the SHA-256 of admin's
// PIN and the entropy the node suggested with it - nobody else could make it, none of the entropy is in the URL, and it
// is as long as a DDNS token. "sha256" is the page's own ("hex_sha256").
function setup_alive_token( pin, suggested, sha256 )
{
   return String( sha256( String( pin || "" ) + String( suggested || "" ) ) ).toLowerCase( ).substr( 0, 12 );
}

// NOTE: The node's answer to it - this browser's address with every character an "X" when it was in time and kept the node
// waiting ("kept"); the address as it is when not - admin's PIN gone ("gone"). Anything else - no answer, an error page -
// says nothing ("unknown").
function setup_alive_answer( text )
{
   var value = String( text || "" ).trim( );

   if( /^X+$/.test( value ) )
      return "kept";

   return /^[0-9A-Fa-f.:]+$/.test( value ) ? "gone" : "unknown";
}

// NOTE: The node refusing the first set up request - "This web session is not valid (or has expired)." - while someone
// else's setup holds admin's PIN.
function setup_refused( text )
{
   return /not valid|has expired/i.test( String( text || "" ) );
}

// NOTE: The node refusing the second, its PIN gone - it waits five minutes from its last word with the tab that has it (Ian,
// 2026-10-12). Then it answers "System is currently locked for administration." - as it also does, for a moment, while
// another admin command runs, so the page asks again before believing it. A node that has restarted since - or been made
// new again - never gave that PIN: "User credentials are either invalid or incorrect." (Damon, 2026-10-13, carrying on
// in a tab kept from before). The password is the page's own, so only the PIN can be wrong.
function setup_pin_gone( text )
{
   return setup_refused( text ) || /locked for administration|credentials are either invalid or incorrect/i.test( String( text || "" ) );
}

// NOTE: The node's answer to the first set up request ("POST /cws/devices?access=admin") - admin's PIN and the entropy the
// node suggests, "32364 4651a055cc23e9ca17aa2582cb586c68". null for anything else - an error is said apart, by the page.
function setup_reply( text )
{
   var match = String( text || "" ).trim( ).match( /^([0-9]{5})\s+([0-9a-f]{32,})$/ );

   return ( match === null ) ? null : { pin: match[ 1 ], suggested: match[ 2 ] };
}

// NOTE: What is kept for the tab between the two requests, so a reload can still finish - the PIN and the node's suggestion,
// never the words or the person's randomness (those are made again). null for anything else.
function setup_kept( stored )
{
   try
   {
      var value = JSON.parse( stored );

      if( value && /^[0-9]{5}$/.test( value.pin ) && /^[0-9a-f]{32,}$/.test( value.suggested ) )
         return { pin: value.pin, suggested: value.suggested };
   }
   catch( e )
   {
   }

   return null;
}

// NOTE: The person's randomness as hex, "length" characters - every pop's time and place (and the misses' times) with the
// browser's own random numbers, hashed by "sha256" (hex in, hex out: "hex_sha256( )" in the page). Long enough to mix
// with the node's suggestion by "xor_hex_data" - several hashes chained when one is too short.
function randomness_hex( samples, browser_hex, sha256, length )
{
   var text = ( samples || [ ] ).map( function( sample )
   {
      return sample.t.toFixed( 3 ) + "," + Math.round( sample.x || 0 ) + "," + Math.round( sample.y || 0 ) + ( sample.hit ? "+" : "-" );
   } ).join( ";" ) + "|" + String( browser_hex || "" );

   var hex = "";

   for( var n = 0; hex.length < length; n++ )
      hex += sha256( n + ":" + text );

   return hex.substr( 0, length );
}

// NOTE: The three words to pick from when checking word "index" - the right one and two others from the list, in an order
// "random( )" (0 to 1) decides. Never one of the person's other words, so a slip on the paper is caught.
function word_choices( words, index, wordlist, random )
{
   var right = words[ index ];
   var choices = [ right ];

   for( var tries = 0; ( choices.length < 3 ) && ( tries < 200 ); tries++ )
   {
      var other = wordlist[ Math.floor( random( ) * wordlist.length ) ];

      if( other && ( choices.indexOf( other ) < 0 ) && ( words.indexOf( other ) < 0 ) )
         choices.push( other );
   }

   for( var i = choices.length - 1; i > 0; i-- )
   {
      var j = Math.floor( random( ) * ( i + 1 ) );
      var swap = choices[ i ];

      choices[ i ] = choices[ j ];
      choices[ j ] = swap;
   }

   return choices;
}

if( typeof module !== "undefined" )
{
   module.exports = {
      parse_system: parse_system,
      setup_reply: setup_reply,
      setup_refused: setup_refused,
      setup_password_enter: setup_password_enter,
      c_home_ntfy_topic_request: c_home_ntfy_topic_request,
      c_home_ntfy_test_request: c_home_ntfy_test_request,
      ntfy_server_example: ntfy_server_example,
      setup_pin_gone: setup_pin_gone,
      setup_alive_token: setup_alive_token,
      setup_alive_answer: setup_alive_answer,
      c_home_devt_request: c_home_devt_request,
      c_home_demo_request: c_home_demo_request,
      parse_flag: parse_flag,
      node_kind_text: node_kind_text,
      health_issues: health_issues,
      health_heading: health_heading,
      setup_kept: setup_kept,
      randomness_hex: randomness_hex,
      word_choices: word_choices,
      security_text: security_text,
      warns_master_password: warns_master_password,
      arriving_screen: arriving_screen,
      locked_sign_in_text: locked_sign_in_text,
      normalise_unlock_key: normalise_unlock_key,
      c_home_keys_request: c_home_keys_request,
      c_home_secure_request: c_home_secure_request,
      c_home_keys_unavailable: c_home_keys_unavailable,
      parse_secure: parse_secure,
      parse_key_count: parse_key_count,
      people_summary: people_summary,
      parse_uptime: parse_uptime,
      uptime_words: uptime_words,
      parse_log_names: parse_log_names,
      parse_log_lines: parse_log_lines,
      log_options: log_options,
      people_note: people_note,
      keys_note: keys_note,
      key_error_text: key_error_text,
      people_waiting_text: people_waiting_text,
      log_line_kind: log_line_kind,
      log_view: log_view,
      log_line_parts: log_line_parts,
      log_problems: log_problems,
      problem_lines: problem_lines,
      log_problems_text: log_problems_text,
      rooms_filling: rooms_filling,
      rooms_fullest: rooms_fullest,
      rooms_filling_text: rooms_filling_text,
      room_filling_line: room_filling_line,
      home_apps: home_apps,
      home_sections: home_sections,
      home_section: home_section,
      role_text: role_text,
      chat_summary: chat_summary,
      chat_summary_text: chat_summary_text,
      needs_you: needs_you,
      short_device: short_device,
      device_rows: device_rows,
      devices_shown: devices_shown,
      more_devices_text: more_devices_text
   };
}

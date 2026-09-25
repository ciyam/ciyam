// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Pure parsing and derivation for the chat client. Nothing here touches the DOM
// or the network, so the whole module can be exercised from Node.js by the regression
// suite. Keep it that way - anything needing a document belongs in "chat.js".

const c_entrance_room = "0000000";

const c_unique_length = 13;

const c_error_prefix = "Error: ";

const c_no_new_messages = "(no new messages)";

const c_kind_chat = "chat";
const c_kind_room = "room";
const c_kind_none = "none";
const c_kind_system = "system";

const c_edited_marker = "*";

// NOTE: Who may post in a room, and the markers used to signal it in the entrance listing.
const c_posts_any = "any";
const c_posts_own = "own";
const c_posts_none = "none";

const c_posts_own_marker = "!";
const c_posts_none_marker = "~";

const c_starting_room = "0000001";

const c_num_sender_colours = 6;

// NOTE: The verbs the server can place after a ":" in a system message.
const c_system_verbs = [ "joined", "create", "invite", "rename", "issued" ];

function is_error_response( response )
{
   return ( typeof response === "string" ) && ( response.indexOf( c_error_prefix ) === 0 );
}

function error_text( response )
{
   if( !is_error_response( response ) )
      return "";

   return response.substring( c_error_prefix.length );
}

// NOTE: The member list is the first line of every fetch, in the form
// "<name>+<sessions>" separated by spaces. A count of zero is a member
// who is known to the room but has no live session.
function parse_members( line )
{
   var members = [ ];

   if( !line )
      return members;

   var entries = line.split( " " );

   for( var i = 0; i < entries.length; i++ )
   {
      var entry = entries[ i ];

      if( entry === "" )
         continue;

      var pos = entry.lastIndexOf( "+" );

      if( pos <= 0 )
         continue;

      var name = entry.substring( 0, pos );
      var count = parseInt( entry.substring( pos + 1 ), 10 );

      if( isNaN( count ) )
         count = 0;

      members.push( { name: name, sessions: count, online: ( count > 0 ) } );
   }

   return members;
}

// NOTE: A ":rename" carries both names in single quotes, a ":create" and an
// ":invite" carry "<room>-<token> <name>", and an ":issued" carries its detail
// inside parentheses. Anything unrecognised is returned with just its verb so
// that the caller can still render the raw text.
function parse_system_event( text )
{
   var event = { verb: "", detail: text };

   if( !text || ( text.charAt( 0 ) !== ":" ) )
      return event;

   var body = text.substring( 1 );

   var pos = body.indexOf( " " );

   if( pos < 0 )
   {
      event.verb = body;
      event.detail = "";

      return event;
   }

   event.verb = body.substring( 0, pos );
   event.detail = body.substring( pos + 1 );

   if( ( event.verb === "create" ) || ( event.verb === "invite" ) )
   {
      var rest = event.detail;

      // NOTE: An invitation reads ":invite room <room>-<token> <name>" - the "room"
      // keyword was added server side and is not part of the target.
      if( ( event.verb === "invite" ) && ( rest.indexOf( "room " ) === 0 ) )
         rest = rest.substring( 5 );

      var spos = rest.indexOf( " " );

      var target = ( spos < 0 ) ? rest : rest.substring( 0, spos );

      event.name = ( spos < 0 ) ? "" : rest.substring( spos + 1 );

      var tpos = target.indexOf( "-" );

      if( tpos > 0 )
      {
         event.room = target.substring( 0, tpos );
         event.token = target.substring( tpos + 1 );
      }
      else
         event.room = target;
   }
   // NOTE: ":assign" carries the same "'old' to 'new'" shape as ":rename" - it reports a
   // change of room ownership. Both are parsed here so the notice can name the two sides.
   else if( ( event.verb === "rename" ) || ( event.verb === "assign" ) )
   {
      var quoted = event.detail.match( /'([^']*)'\s+to\s+'([^']*)'/ );

      if( quoted )
      {
         event.from_name = quoted[ 1 ];
         event.to_name = quoted[ 2 ];
      }
   }
   else if( event.verb === "issued" )
   {
      var inner = event.detail.match( /^\(([^)]*)\)/ );

      if( inner )
      {
         var info = inner[ 1 ];

         var parts = info.match( /^(\S+)(?:\s+for\s+(\S+))?\s+sent to\s+(.*)$/ );

         if( parts )
         {
            event.issued_kind = parts[ 1 ];
            event.room = parts[ 2 ] || "";
            event.recipients = parts[ 3 ].split( "," ).map( function( s ) { return s.trim( ); } );
         }
      }
   }

   return event;
}

// NOTE: Every non-member line is "<unique> <sender> <remainder>". The first
// character of the remainder says what kind of line it is - a space for
// ordinary chat (the stripped "_" marker), a colon for a system event and a
// hash for an entrance room listing.
function parse_message_line( line )
{
   if( !line )
      return null;

   var pos = line.indexOf( " " );

   if( pos < 0 )
      return null;

   var unique = line.substring( 0, pos );

   var rest = line.substring( pos + 1 );

   if( rest === c_no_new_messages )
      return { kind: c_kind_none, unique: unique };

   pos = rest.indexOf( " " );

   if( pos < 0 )
      return null;

   var sender = rest.substring( 0, pos );

   var remainder = rest.substring( pos + 1 );

   var edited = false;

   if( sender.slice( -1 ) === c_edited_marker )
   {
      edited = true;
      sender = sender.substring( 0, sender.length - 1 );
   }

   var first = remainder.charAt( 0 );

   if( first === "#" )
      return parse_room_entry( unique, sender, remainder );

   if( first === ":" )
   {
      var event = parse_system_event( remainder );

      return {
         kind: c_kind_system,
         unique: unique,
         sender: sender,
         edited: edited,
         text: remainder,
         event: event
      };
   }

   // NOTE: A leading space is the placeholder left by the stripped "_" prefix
   // and is not part of what the user typed.
   var text = ( first === " " ) ? remainder.substring( 1 ) : remainder;

   return {
      kind: c_kind_chat,
      unique: unique,
      sender: sender,
      edited: edited,
      text: text
   };
}

// NOTE: An entrance room line is "<unique> <owner> #<room> <new>/<total> <name>".
function parse_room_entry( unique, owner, remainder )
{
   var entry = { kind: c_kind_room, unique: unique, owner: owner };

   var parts = remainder.split( " " );

   entry.room = parts[ 0 ].substring( 1 );

   entry.unread = 0;
   entry.total = 0;

   // NOTE: Posting restrictions are signalled by a suffix on the total - "!" when only
   // the owner may post and "~" when the room is locked. Absent means anyone may post.
   entry.posts = c_posts_any;

   if( parts.length > 1 )
   {
      var counts = parts[ 1 ].split( "/" );

      entry.unread = parseInt( counts[ 0 ], 10 ) || 0;

      var total = ( counts.length > 1 ) ? counts[ 1 ] : "";

      var suffix = total.slice( -1 );

      if( suffix === c_posts_own_marker )
         entry.posts = c_posts_own;
      else if( suffix === c_posts_none_marker )
         entry.posts = c_posts_none;

      entry.total = parseInt( total, 10 ) || 0;
   }

   entry.locked = ( entry.posts === c_posts_none );

   entry.name = ( parts.length > 2 ) ? parts.slice( 2 ).join( " " ) : "";

   return entry;
}

// NOTE: Splits a complete "format=text" fetch into its member list and its
// rows. The caller does not need to know whether it asked for the entrance
// room - the row kinds say what came back.
function parse_fetch_response( response )
{
   var result = { error: "", members: [ ], messages: [ ], rooms: [ ], has_new: false };

   if( is_error_response( response ) )
   {
      result.error = error_text( response );

      return result;
   }

   if( !response )
      return result;

   var lines = response.split( "\n" );

   result.members = parse_members( lines[ 0 ] );

   for( var i = 1; i < lines.length; i++ )
   {
      var line = lines[ i ];

      if( line === "" )
         continue;

      var entry = parse_message_line( line );

      if( entry === null )
         continue;

      if( entry.kind === c_kind_none )
         continue;
      else if( entry.kind === c_kind_room )
         result.rooms.push( entry );
      else
      {
         result.messages.push( entry );
         result.has_new = true;
      }
   }

   return result;
}

// NOTE: Rooms arrive in allocation order. Sort unread first so that anything
// wanting attention rises, then by room number for a stable ordering.
function derive_room_list( rooms )
{
   var sorted = rooms.slice( );

   sorted.sort( function( lhs, rhs )
   {
      if( ( lhs.unread > 0 ) !== ( rhs.unread > 0 ) )
         return ( lhs.unread > 0 ) ? -1 : 1;

      if( lhs.room < rhs.room )
         return -1;
      else if( lhs.room > rhs.room )
         return 1;

      return 0;
   } );

   return sorted;
}

// NOTE: Members are returned in the order the server holds them. Show the
// connected ones first, then alphabetically, so the list does not jump about
// as session counts change.
function apply_presence( members )
{
   var sorted = members.slice( );

   sorted.sort( function( lhs, rhs )
   {
      if( lhs.online !== rhs.online )
         return lhs.online ? -1 : 1;

      if( lhs.name < rhs.name )
         return -1;
      else if( lhs.name > rhs.name )
         return 1;

      return 0;
   } );

   return sorted;
}

// NOTE: Polling reads forward from one past the last unique seen. The value is
// kept as a zero padded string of the same width the server uses.
function next_start_point( messages )
{
   var last = "";

   for( var i = 0; i < messages.length; i++ )
   {
      var unique = messages[ i ].unique;

      if( unique && ( unique > last ) )
         last = unique;
   }

   if( last === "" )
      return "";

   var next = String( parseInt( last, 10 ) + 1 );

   while( next.length < c_unique_length )
      next = "0" + next;

   return next;
}

// NOTE: A 13 digit millisecond key. Kept as a helper so that callers do not
// scatter the conversion, and so a test can pin the formatting.
function unique_to_time( unique )
{
   var value = parseInt( unique, 10 );

   if( isNaN( value ) )
      return "";

   var when = new Date( value );

   function pad( n )
   {
      return ( n < 10 ) ? ( "0" + n ) : String( n );
   }

   return pad( when.getHours( ) ) + ":" + pad( when.getMinutes( ) ) + ":" + pad( when.getSeconds( ) );
}

// NOTE: The 13 digit unique is Unix seconds with a three digit sequence counter, so it
// lands in the millisecond place and "new Date" reads it correctly. The counter is not a
// real millisecond value, which is why nothing here ever displays sub-second precision.
function unique_to_date( unique )
{
   var value = parseInt( unique, 10 );

   if( isNaN( value ) )
      return null;

   var when = new Date( value );

   return isNaN( when.getTime( ) ) ? null : when;
}

// NOTE: Local calendar day, used to decide where a day divider belongs. Built from the
// local getters rather than toISOString, which would group by UTC and put the divider in
// the wrong place for anyone far from Greenwich - us included.
function day_key( unique )
{
   var when = unique_to_date( unique );

   if( when === null )
      return "";

   function pad( n )
   {
      return ( n < 10 ) ? ( "0" + n ) : String( n );
   }

   return when.getFullYear( ) + "-" + pad( when.getMonth( ) + 1 ) + "-" + pad( when.getDate( ) );
}

// NOTE: Whole days between two dates, comparing calendar days rather than elapsed time -
// 23:59 to 00:01 is one day apart, not zero.
function days_between( from, to )
{
   var a = new Date( from.getFullYear( ), from.getMonth( ), from.getDate( ) );
   var b = new Date( to.getFullYear( ), to.getMonth( ), to.getDate( ) );

   return Math.round( ( b - a ) / 86400000 );
}

// NOTE: Follows the convention Element uses - "Today", "Yesterday", then the weekday name
// while it is still unambiguous. Seven days back is the same weekday as today, so the
// name stops being a useful label at that point and the full date is given instead. The
// year is added only when it differs, which keeps the common case short.
//
// "p_now" is injectable so this can be tested against a fixed day.
function day_label( unique, p_now )
{
   var when = unique_to_date( unique );

   if( when === null )
      return "";

   var now = ( p_now == null ) ? new Date( ) : p_now;

   var diff = days_between( when, now );

   if( diff === 0 )
      return "Today";

   if( diff === 1 )
      return "Yesterday";

   if( ( diff > 1 ) && ( diff < 7 ) )
      return when.toLocaleDateString( undefined, { weekday: "long" } );

   var options = { weekday: "long", day: "numeric", month: "long" };

   if( when.getFullYear( ) !== now.getFullYear( ) )
      options.year = "numeric";

   return when.toLocaleDateString( undefined, options );
}

// NOTE: The full stamp shown on hover. Seconds are included because the sequence counter
// makes same-second ordering visible, and without them two adjacent entries can look like
// they arrived out of order.
function unique_to_full( unique )
{
   var when = unique_to_date( unique );

   if( when === null )
      return "";

   return when.toLocaleString( undefined,
    { weekday: "short", year: "numeric", month: "short", day: "numeric",
      hour: "2-digit", minute: "2-digit", second: "2-digit" } );
}

// NOTE: A stable colour index so that a participant keeps the same colour
// between sessions and between clients.
function sender_colour_index( name )
{
   if( !name )
      return 1;

   var hash = 0;

   for( var i = 0; i < name.length; i++ )
      hash = ( ( hash * 31 ) + name.charCodeAt( i ) ) % 100003;

   return ( hash % c_num_sender_colours ) + 1;
}

function is_entrance_room( room )
{
   return ( room === c_entrance_room );
}

function is_starting_room( room )
{
   return ( room === c_starting_room );
}

// NOTE: Decides whether the composer should accept input, and why not when it should not.
// Kept here rather than in the view so the rules are covered by the regression tests.
//
// Three separate restrictions apply:
//  - the entrance room is a lobby and never accepts messages
//  - the starting room is reserved for invitations and admin messages
//  - a room may be set to owner-only or locked with the "posts" option
function posting_status( room, entry, username, is_admin )
{
   if( is_entrance_room( room ) )
      return { can_post: false, reason: "The entrance room is a lobby - pick a room to chat in." };

   if( is_starting_room( room ) && !is_admin )
      return { can_post: false, reason: "Room 0000001 is reserved for invitations and admin messages." };

   var posts = ( entry && entry.posts ) ? entry.posts : c_posts_any;

   if( posts === c_posts_none )
      return { can_post: false, locked: true, reason: "This room is locked - nobody can post." };

   if( posts === c_posts_own )
   {
      var owner = ( entry && entry.owner ) ? entry.owner : "";

      if( ( owner !== username ) && !is_admin )
         return { can_post: false, locked: true, reason: "Only the room owner can post here." };
   }

   return { can_post: true, reason: "" };
}

// NOTE: Who this user has already invited to a room, from the ":issued" receipts the
// server posts to their Administration room. The server accepts a duplicate invitation
// without complaint, and the invitee just gets a second notice carrying the same token,
// so the picker uses this to stop it being sent again.
//
// Only receipts addressed to the viewer are visible, so this knows about invitations this
// user sent - not ones another owner or the administrator sent to the same person.
function invited_to_room( messages, room )
{
   var names = { };

   for( var i = 0; i < ( messages || [ ] ).length; i++ )
   {
      var event = messages[ i ].event;

      if( !event || ( event.verb !== "issued" ) || ( event.issued_kind !== "invite" ) )
         continue;

      if( event.room !== room )
         continue;

      ( event.recipients || [ ] ).forEach( function( name )
      {
         if( name )
            names[ name ] = true;
      } );
   }

   return names;
}

// NOTE: The session handover on the "test_web_channel" BroadcastChannel is a string whose
// receivers - ours and the harness's - test for ":" first, then "-", then "=". The
// credentials message is "<viewer>=<field>,<field>,...", so a field containing "-" or ":"
// is read as a different message type and silently dropped. A username like "tester-1"
// did exactly that, and the console showed nothing but its own device (ISS-016).
//
// Any field that can hold free text is therefore encoded so that none of ":", "-", "="
// or "," survive. encodeURIComponent covers the last three but leaves "-" alone, hence the
// extra replace.
// NOTE: The invitations still waiting to be taken up, from the messages in the starting room.
// An invitation is a private ":invite room <room>-<token> <name>" message to the invitee, sent
// by the inviter. One is pending until its room appears among the user's own rooms - which
// is also what clears it, with no state to keep. The server accepts the same invitation
// twice (ISS-017), so there is one entry per room, and the latest supplies its details.
// Newest first.
function pending_invitations( messages, rooms )
{
   var joined = { };

   ( rooms || [ ] ).forEach( function( entry ) { joined[ entry.room ] = true; } );

   var latest = { };

   ( messages || [ ] ).forEach( function( message )
   {
      if( !message || ( message.kind !== c_kind_system ) || !message.event )
         return;

      var event = message.event;

      if( ( event.verb !== "invite" ) || !event.room || !event.token )
         return;

      if( joined[ event.room ] || is_entrance_room( event.room ) || is_starting_room( event.room ) )
         return;

      latest[ event.room ] = {
         room: event.room,
         token: event.token,
         name: event.name || event.room,
         inviter: message.sender,
         unique: message.unique
      };
   } );

   var pending = Object.keys( latest ).map( function( room ) { return latest[ room ]; } );

   pending.sort( function( lhs, rhs )
   {
      if( lhs.unique !== rhs.unique )
         return ( lhs.unique > rhs.unique ) ? -1 : 1;

      return ( lhs.room < rhs.room ) ? -1 : ( lhs.room > rhs.room ) ? 1 : 0;
   } );

   return pending;
}

// NOTE: The rooms shown in the rail. The starting room - Administration - is shown to admin
// only; it still exists for everyone, and is read in the background for invitations.
function visible_rooms( rooms, is_admin )
{
   if( is_admin )
      return ( rooms || [ ] ).slice( );

   return ( rooms || [ ] ).filter( function( entry ) { return !is_starting_room( entry.room ); } );
}

// NOTE: The password strength rules from "test_bip39.html", so both pages rate a password
// the same way. Under seven characters is unsatisfactory; otherwise the length is scaled by
// how many kinds of character are used - digits alone count least - and the score banded.
// Level 0 cannot be saved; the rest are allowed, with the rating shown.
const c_password_min_length = 7;

const c_password_ratings = [
   { level: 1, min: 0, text: "Weak" },
   { level: 2, min: 24, text: "Moderate" },
   { level: 3, min: 48, text: "Strong" },
   { level: 4, min: 80, text: "Very Strong" }
];

function password_strength( password )
{
   var text = String( password || "" );

   if( text.length === 0 )
      return { level: -1, text: "" };

   if( text.length < c_password_min_length )
      return { level: 0, text: "Unsatisfactory" };

   var has_digits = /[0-9]/.test( text );
   var has_lower = /[a-z]/.test( text );
   var has_upper = /[A-Z]/.test( text );
   var has_special = /[^A-Za-z0-9]/.test( text );

   var kinds = ( has_digits ? 1 : 0 ) + ( has_lower ? 1 : 0 ) + ( has_upper ? 1 : 0 ) + ( has_special ? 1 : 0 );

   var multiplier = 7;

   if( kinds === 1 )
      multiplier = has_digits ? 1 : 2;
   else if( kinds === 2 )
      multiplier = has_digits ? 3 : 4;
   else if( kinds === 3 )
      multiplier = has_digits ? 5 : 6;

   var score = text.length * multiplier;

   var rating = c_password_ratings[ 0 ];

   for( var i = 1; i < c_password_ratings.length; i++ )
   {
      if( score >= c_password_ratings[ i ].min )
         rating = c_password_ratings[ i ];
   }

   return { level: rating.level, text: rating.text };
}

// NOTE: The letter shown in place of an avatar - the first character of the username, or of
// the PIN for an account that has no username.
function user_initial( name )
{
   var text = String( name || "" ).trim( );

   return ( text === "" ) ? "?" : text.charAt( 0 ).toUpperCase( );
}

function encode_channel_field( value )
{
   return encodeURIComponent( String( value || "" ) ).replace( /-/g, "%2D" );
}

function decode_channel_field( value )
{
   try
   {
      return decodeURIComponent( String( value || "" ) );
   }
   catch( e )
   {
      return "";
   }
}

// NOTE: The saved account list is a comma separated string in localStorage, shared with
// "test_web_session.html" - both read the same "cws.access" key. Parsing and formatting
// live here, as one pair, because doing it inline in three places is what allowed a blank
// entry to get in: "".split( "," ) is [ "" ], not [ ], so an emptied list came back as a
// single nameless account and the next PIN appended to it, giving ",11111". That then
// broke the harness PIN list as well as ours.
function parse_access_list( stored )
{
   if( ( stored === null ) || ( stored === undefined ) )
      return [ ];

   var entries = String( stored ).split( "," );

   var valid = [ ];

   for( var i = 0; i < entries.length; i++ )
   {
      var entry = entries[ i ].trim( );

      if( ( entry !== "" ) && ( valid.indexOf( entry ) < 0 ) )
         valid.push( entry );
   }

   return valid;
}

// NOTE: Returns null when nothing is left, so the caller removes the key rather than
// storing an empty string. Ian: "if intending to wipe the access it should be deleted
// (rather than set to an empty string)".
function format_access_list( entries )
{
   var valid = parse_access_list( ( entries || [ ] ).join( "," ) );

   return ( valid.length === 0 ) ? null : valid.sort( ).join( "," );
}

// NOTE: Room names are validated server side by "irc_add_room". Checking the
// same rules here lets the form report a problem before a round trip.
function is_valid_room_name( name )
{
   if( !name || ( name.length < 5 ) || ( name.length > 50 ) )
      return false;

   if( !/^[a-zA-Z][-+&()' a-zA-Z0-9]+$/.test( name ) )
      return false;

   if( !/[)a-zA-Z0-9]$/.test( name ) )
      return false;

   if( ( name.indexOf( "&&" ) >= 0 ) || ( name.indexOf( "()" ) >= 0 )
    || ( name.indexOf( "''" ) >= 0 ) || ( name.indexOf( "  " ) >= 0 ) )
      return false;

   var opens = ( name.match( /\(/g ) || [ ] ).length;
   var closes = ( name.match( /\)/g ) || [ ] ).length;

   if( ( opens > 1 ) || ( closes > 1 ) || ( opens !== closes ) )
      return false;

   if( ( opens === 1 ) && ( name.indexOf( ")" ) < name.indexOf( "(" ) ) )
      return false;

   return true;
}

// NOTE: Usernames are validated by "irc_join" using the same shape.
function is_valid_username( name )
{
   if( !name || ( name.length < 3 ) || ( name.length > 12 ) )
      return false;

   if( !/^[a-z][-a-z0-9]+$/.test( name ) )
      return false;

   if( !/[a-z0-9]$/.test( name ) )
      return false;

   return ( name.indexOf( "--" ) < 0 );
}

// NOTE: Exported for Node.js; in the browser these are plain globals.
if( typeof module !== "undefined" )
{
   module.exports = {
      is_error_response: is_error_response,
      error_text: error_text,
      parse_members: parse_members,
      parse_system_event: parse_system_event,
      parse_message_line: parse_message_line,
      parse_fetch_response: parse_fetch_response,
      derive_room_list: derive_room_list,
      apply_presence: apply_presence,
      next_start_point: next_start_point,
      unique_to_time: unique_to_time,
      unique_to_date: unique_to_date,
      unique_to_full: unique_to_full,
      day_key: day_key,
      day_label: day_label,
      days_between: days_between,
      sender_colour_index: sender_colour_index,
      is_entrance_room: is_entrance_room,
      is_starting_room: is_starting_room,
      posting_status: posting_status,
      invited_to_room: invited_to_room,
      pending_invitations: pending_invitations,
      password_strength: password_strength,
      user_initial: user_initial,
      visible_rooms: visible_rooms,
      encode_channel_field: encode_channel_field,
      decode_channel_field: decode_channel_field,
      parse_access_list: parse_access_list,
      format_access_list: format_access_list,
      is_valid_room_name: is_valid_room_name,
      is_valid_username: is_valid_username
   };
}

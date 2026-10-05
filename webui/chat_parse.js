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

const c_private_marker = "!";

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

      var member = { name: name, sessions: count, online: ( count > 0 ) };

      // NOTE: With "extra=TIME" the count is followed by ".<unique>" - the last message this
      // user's reads have returned, so everything up to and including it has reached them.
      var dot = entry.indexOf( ".", pos );

      if( dot > 0 )
      {
         var read = entry.substring( dot + 1 );

         if( /^\d+$/.test( read ) )
            member.read = read;
      }

      members.push( member );
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
   // NOTE: Once an invitation is joined or declined the server rewrites it as ":ignore (invite
   // for <room> was processed)", and a decline also posts ":reject (invite for <room> was
   // rejected)" - Ian, 2026-09-26. Both keep the room number, so it is picked out here.
   else if( ( event.verb === "ignore" ) || ( event.verb === "reject" ) )
   {
      var about = event.detail.match( /invite for (\d{7})/ );

      if( about )
         event.room = about[ 1 ];
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
// ordinary chat (the stripped "_" marker), "!" for a private message (Ian,
// 2026-09-27 - before then a private message had a space too), a colon for a
// system event and a hash for an entrance room listing.
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
   // and is not part of what the user typed; "!" is the same place, marking it private.
   // NOTE: Edits included. Until 2026-09-28 the server marked every edit private (ISS-026) and
   // the mark was ignored on an edit; an edit now keeps the original's prefix, so it is true.
   var is_private = ( first === c_private_marker );

   var text = ( ( first === " " ) || is_private ) ? remainder.substring( 1 ) : remainder;

   return {
      kind: c_kind_chat,
      unique: unique,
      sender: sender,
      edited: edited,
      private: is_private,
      text: unescape_message_text( text )
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
// NOTE: How the server carries a message's backslashes and line breaks - verified against
// the container, 2026-09-26:
//
//   on the way in   "\" escapes the next character and is dropped: "a\b" is stored "ab",
//                   and "\\" is stored as one "\"
//   on the way out  a stored "\" comes back as "\\", and a line break as "\" at the end of
//                   the line, the message carrying on on the next - the response is one
//                   message per line, so a raw break would split it
//
// So a backslash the user typed is doubled before sending, and on receipt the continued
// lines are joined and the escapes undone.
function escape_message_text( text )
{
   return String( text || "" ).replace( /\\/g, "\\\\" );
}

// NOTE: The server keeps each message as a queue item of at most 3000 characters since
// 2026-10-01 ("c_default_max_deque_item_size" in "ciyam_variables.cpp", 8000 before), which
// holds the text encoded with the sender's name - measured at 2233 bytes of text for "admin"
// and 2230 for "verify-a", a byte less for each letter of the name, so 2226 for the longest.
// Ian lowered it so a thousand rooms fit in memory, keeping his reserve in the figure itself,
// so the chat stops just under it (QST-004). It is bytes, not characters: "é" is two and most
// emoji four.
const c_max_message_bytes = 2200;

function message_bytes( text )
{
   var value = String( text || "" );

   if( typeof TextEncoder !== "undefined" )
      return new TextEncoder( ).encode( value ).length;

   return unescape( encodeURIComponent( value ) ).length;
}

function unescape_message_text( text )
{
   return String( text || "" ).replace( /\\([\s\S])/g, "$1" );
}

// NOTE: A line ends in a continuation when it ends in an odd number of backslashes - an even
// number is escaped backslashes, and the message genuinely ends there.
function join_continued_lines( lines )
{
   var joined = [ ];

   for( var i = 0; i < lines.length; i++ )
   {
      var line = lines[ i ];

      while( ( /(?:^|[^\\])(?:\\\\)*\\$/.test( line ) ) && ( i + 1 < lines.length ) )
         line = line + "\n" + lines[ ++i ];

      joined.push( line );
   }

   return joined;
}

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

   // NOTE: A message with a line break arrives over several lines - see
   // "escape_message_text( )" above.
   var lines = join_continued_lines( response.split( "\n" ) );

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

   result.messages = pair_private_copies( result.messages );

   return result;
}

// NOTE: A sender who puts their own name in "for" gets a copy of their private message - and
// the ":issued (message sent to ...)" receipt for it carries the same unique. The two are one
// thing: the receipt's recipients go onto the copy, and the receipt is dropped. Left as two,
// the chat would draw whichever came first and skip the other as already drawn.
function pair_private_copies( messages )
{
   var copies = { };

   ( messages || [ ] ).forEach( function( entry )
   {
      if( entry && ( entry.kind === c_kind_chat ) && entry.private )
         copies[ entry.unique + " " + entry.sender ] = entry;
   } );

   return ( messages || [ ] ).filter( function( entry )
   {
      var event = entry && entry.event;

      if( !event || ( event.verb !== "issued" ) || ( event.issued_kind !== "message" ) )
         return true;

      var copy = copies[ entry.unique + " " + entry.sender ];

      if( !copy )
         return true;

      copy.recipients = ( event.recipients || [ ] ).filter( function( name ) { return name !== entry.sender; } );

      return false;
   } );
}

// NOTE: The small label on a private message. A copy the user sent says who it went to;
// one they received can only say it was sent to them - the server does not say who else.
function private_label( entry )
{
   if( entry.recipients && entry.recipients.length )
      return { text: "private · to " + entry.recipients.join( ", " ),
       title: "Private - only you and " + entry.recipients.join( ", " ) + " can see this" };

   return { text: "private", title: "Private - sent to you, not to the whole room" };
}

// NOTE: What the message box says about who will see what is typed - Damon, 2026-09-28: a
// private mode that stays on after sending is only safe if it cannot be missed. Private is
// shown by colour and by words together - "Private to" and "Send privately" - never colour
// alone. Editing a private message is private too, and keeps "Save edit".
//
// "send_short" is for a phone, where "Send privately" would squeeze the box - Damon,
// 2026-09-28: there it is just "Send", the amber and the heading still saying private.
function composer_mode( recipients, editing, edit_private )
{
   var to_people = ( recipients || [ ] ).length > 0;

   if( editing )
      return { is_private: !!edit_private, label: edit_private ? "Editing a private message" : "", send: "Save edit", send_short: "Save edit" };

   return { is_private: to_people, label: to_people ? "Private to" : "", send: to_people ? "Send privately" : "Send", send_short: "Send" };
}

// NOTE: Time-outs (ISS-020). The server gives up on a request after 5 seconds - "Error: Timed out
// waiting for web session response." - but may still finish it, and its late answer could
// reach the next request. The chat then absorbs any late answer, re-reads what is on screen,
// and after this many in a row gives up on the session.
const c_max_timeouts_in_row = 3;

function is_timeout_response( response )
{
   return is_error_response( response ) && ( error_text( response ).indexOf( "Timed out waiting for web session response" ) === 0 );
}

// NOTE: Whether a message whose send timed out reached the room after all, judged from the
// room re-read whole. "pending" is { me, text, after, edit }: "after" is the start point the
// chat held before sending - a server unique, so the browser's clock plays no part - and
// "edit" the unique of a message being edited. Rows are { unique, sender, text }.
function arrived_after_timeout( rows, pending )
{
   return ( rows || [ ] ).some( function( row )
   {
      if( ( row.sender !== pending.me ) || ( row.text !== pending.text ) )
         return false;

      if( pending.edit )
         return row.unique === pending.edit;

      return Number( row.unique ) >= Number( pending.after || 0 );
   } );
}

// NOTE: The "for" value that edits a message: its unique, with "!" in front for a private one
// (Ian, 2026-09-28). The server keeps public and private apart - an edit must say which, and
// the wrong one is refused ("Source message must not be modified to or from private.").
function edit_for_value( unique, is_private )
{
   return ( is_private ? c_private_marker : "" ) + unique;
}

// NOTE: Adding the sender to "for" is how they keep a copy of what they sent (Ian,
// 2026-09-27) - otherwise they get only the ":issued" receipt.
function with_sender( recipients, sender )
{
   var list = ( recipients || [ ] ).slice( );

   if( sender && ( list.indexOf( sender ) < 0 ) )
      list.push( sender );

   return list;
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

// NOTE: Read markers - which message each member has read up to, from "extra=TIME". Since Ian's
// change of 2026-09-30 a read point is the unique of the last message that member's read returned
// (it was the time of the read), so they have read everything up to and including it, and their
// marker goes under the last message (or notice) at or before it. The user's own is left out, as
// are members with no read point or none reaching the first message. Returns { <unique>: [ names ] },
// names in order.
function seen_by( uniques, members, me )
{
   var marks = { };

   ( members || [ ] ).forEach( function( member )
   {
      if( !member.read || ( member.name === me ) )
         return;

      var read = Number( member.read );

      var last = "";

      ( uniques || [ ] ).forEach( function( unique )
      {
         if( Number( unique ) <= read )
            last = unique;
      } );

      if( last === "" )
         return;

      ( marks[ last ] = marks[ last ] || [ ] ).push( member.name );
   } );

   Object.keys( marks ).forEach( function( unique ) { marks[ unique ].sort( ); } );

   return marks;
}

// NOTE: New messages added to those already held, as a read without "from" returns only what
// is new. Keyed on the unique with the sender and what was said, since a private copy and its
// receipt share a unique - and an entry already held is not taken twice.
function merge_new_messages( held, fresh )
{
   function key( m ) { return m.unique + " " + ( m.sender || "" ) + " " + ( m.text || "" ); }

   var seen = { };

   ( held || [ ] ).forEach( function( m ) { seen[ key( m ) ] = true; } );

   return ( held || [ ] ).concat( ( fresh || [ ] ).filter( function( m ) { return !seen[ key( m ) ]; } ) );
}

// NOTE: When a read of only what is new is not enough, and Administration must be read whole
// ("from=0") - Ian's suggestion, 2026-09-27. Two cases:
//
//   - nothing new came back, though its message count moved - something else on this device
//     (the console, another tab) read it first and moved the server's "new" point past it
//   - anything about invitations came back - an answered invitation is rewritten in place,
//     as ":ignore", which a read of only what is new never returns
//
// Announcements, the everyday case, are new lines and take the cheap read.
const c_invitation_verbs = [ "invite", "ignore", "reject", "joined", "remove" ];

function needs_full_read( fresh )
{
   if( !fresh || ( fresh.length === 0 ) )
      return true;

   return fresh.some( function( m ) { return m.event && ( c_invitation_verbs.indexOf( m.event.verb ) >= 0 ); } );
}

// NOTE: The "posts" option for a room's messages PUT - who may post. The server takes only
// "ANY", "OWN" or "NONE", in capitals ("Unknown room posts value 'own'" otherwise), from
// the owner or admin, and never for Administration. "" for anything else.
function posts_request_value( posts )
{
   var value = String( posts || "" ).toLowerCase( );

   if( [ c_posts_any, c_posts_own, c_posts_none ].indexOf( value ) < 0 )
      return "";

   return value.toUpperCase( );
}

// NOTE: Whether this user may change who posts in a room.
function can_change_posting( room, entry, username, is_admin )
{
   if( !entry || is_entrance_room( room ) || is_starting_room( room ) )
      return false;

   return is_admin || ( ( entry.owner || "" ) === username );
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
// NOTE: The rooms whose invitation this user has declined - from their own ":reject (invite
// for <room> was rejected)". Only their own: someone else's decline of the same room must not
// answer this user's invitation. A decline is final - the server refuses to invite them again.
function declined_rooms( messages, me )
{
   var declined = { };

   ( messages || [ ] ).forEach( function( message )
   {
      if( message && message.event && ( message.event.verb === "reject" ) && message.event.room
       && me && ( message.sender === me ) )
         declined[ message.event.room ] = true;
   } );

   return declined;
}

// NOTE: "me" lets a declined invitation drop out even when the server has not rewritten it as
// ":ignore" - which it stopped doing on 2026-09-28 (ISS-028). Joined rooms drop out as before.
function pending_invitations( messages, rooms, me )
{
   var joined = declined_rooms( messages, me );

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

// NOTE: A room event in plain words, after the name of whoever it is about - "joined",
// "left the room" - rather than the server's ":joined" and ":remove". "label" turns a room
// number into what the reader knows it as. An unknown verb still shows its detail, without
// the colon, so a new one from the server is never silently dropped.
function describe_event( event, label )
{
   var name_of = label || function( room ) { return "#" + room; };

   var posting = { any: "anyone", own: "the owner only", none: "nobody - locked" };

   switch( event.verb )
   {
      case "joined":
         return "joined";

      case "remove":
         return "left the room";

      case "create":
         return "created " + ( event.name ? event.name + " (#" + event.room + ")" : name_of( event.room ) );

      case "invite":
         return "invited you to " + ( event.name ? event.name + " (#" + event.room + ")" : name_of( event.room ) );

      case "rename":
         return "renamed the room from '" + ( event.from_name || "" ) + "' to '" + ( event.to_name || "" ) + "'";

      case "assign":
         return "handed the room from '" + ( event.from_name || "" ) + "' to '" + ( event.to_name || "" ) + "'";

      case "allows":
      {
         var set = String( event.detail || "" ).match( /^set to (\S+)/ );

         if( set )
            return "set who may post to " + ( posting[ set[ 1 ].toLowerCase( ) ] || set[ 1 ] );

         break;
      }

      case "issued":
         if( event.recipients && event.recipients.length )
         {
            if( event.issued_kind === "invite" )
               return "invited " + event.recipients.join( ", " ) + ( event.room ? " to " + name_of( event.room ) : "" );

            if( event.issued_kind === "message" )
               return "sent a private message to " + event.recipients.join( ", " );
         }

         break;

      // NOTE: Only the invitee sees these, and the sender is whoever invited them.
      case "ignore":
         if( event.room )
            return "invited you to " + name_of( event.room ) + " - already answered";

         break;

      case "reject":
         if( event.room )
            return "declined the invitation to " + name_of( event.room );

         break;
   }

   return event.verb + ( event.detail ? " " + event.detail : "" );
}

// NOTE: The rooms shown in the rail. The starting room - Administration - is shown to admin
// only; it still exists for everyone, and is read in the background for invitations.
function visible_rooms( rooms, is_admin )
{
   if( is_admin )
      return ( rooms || [ ] ).slice( );

   return ( rooms || [ ] ).filter( function( entry ) { return !is_starting_room( entry.room ); } );
}

// NOTE: Direct messages - a prototype, 2026-09-29. A direct message is a room whose members are
// exactly the people in the conversation, one room for each set of people. It is named from
// them - "Private (" and the sorted usernames joined by " + " - so both sides arrive at the same
// name and the chat can find an existing conversation, and show it by the other people's names,
// without reading it. Usernames are 3 to 12 of "a-z", "0-9" and "-", all allowed in a room name.
//
// A group whose names will not fit the 50 character limit cannot be a direct message. It was
// named by a hash of its people, until review found (2026-10-01) that such a name cannot be
// checked - no people in it to find the owner among - and the hash can be worked out by anyone,
// so anyone could make a room that passes for another group's conversation. Two people of any
// length fit, three with short names; a bigger group is an ordinary room until the server knows
// what a direct message is.
//
// "Private (...)" since 2026-10-01, at Ian's suggestion - it reads better where a room name is
// shown as it is, in the terminal client. The first names, "DM ...", are still read, and a
// conversation is matched by its people, not its exact name, so either form is found.
//
// Since 2026-10-05 a conversation between two people is Ian's server direct message instead: made
// as "." and the other's name, it is named "/<a>/<b>" by the server - the two names in order - and
// only those two can be in it, nor can it be renamed. A group, which the server does not have yet,
// keeps the prototype's "Private (...)" room. Both forms are read, matched by their people.
const c_dm_prefix = "Private (";
const c_dm_suffix = ")";
const c_dm_prefix_old = "DM ";
const c_dm_separator = " + ";
const c_dm_server_prefix = "/";
const c_dm_create_prefix = ".";
const c_max_room_name = 50;

const c_username_pattern = /^[a-z][-a-z0-9]{1,10}[a-z0-9]$/;

function dm_people( usernames )
{
   var people = [ ];

   ( usernames || [ ] ).forEach( function( name )
   {
      var clean = String( name || "" ).trim( ).toLowerCase( );

      if( ( clean !== "" ) && ( people.indexOf( clean ) < 0 ) )
         people.push( clean );
   } );

   return people.sort( );
}

// NOTE: The room name for a conversation between "usernames" - which include the user. Empty
// when their names will not fit a room name - too big a group to be a direct message.
function dm_room_name( usernames )
{
   var people = dm_people( usernames );

   var name = c_dm_prefix + people.join( c_dm_separator ) + c_dm_suffix;

   return ( name.length <= c_max_room_name ) ? name : "";
}

// NOTE: Whether a room name is a direct message's, and who is in it. Reads the server's "/<a>/<b>"
// ("server" true - the server named it, so it is to be trusted) and the prototype's two forms,
// "Private (...)" and the first, "DM ...". A server name of one person twice is not a conversation.
function parse_dm_name( name )
{
   var text = String( name || "" );

   if( text.indexOf( c_dm_server_prefix ) === 0 )
   {
      var pair = text.substring( c_dm_server_prefix.length ).split( c_dm_server_prefix );

      var server_valid = ( pair.length === 2 ) && ( pair[ 0 ] !== pair[ 1 ] )
       && c_username_pattern.test( pair[ 0 ] ) && c_username_pattern.test( pair[ 1 ] );

      return server_valid ? { people: pair, count: 2, server: true } : null;
   }

   var rest = null;

   if( ( text.indexOf( c_dm_prefix ) === 0 ) && ( text.slice( -1 ) === c_dm_suffix ) )
      rest = text.substring( c_dm_prefix.length, text.length - c_dm_suffix.length );
   else if( text.indexOf( c_dm_prefix_old ) === 0 )
      rest = text.substring( c_dm_prefix_old.length );

   if( rest === null )
      return null;

   var people = rest.split( c_dm_separator );

   var valid = ( people.length >= 2 ) && people.every( function( person ) { return c_username_pattern.test( person ); } );

   return valid ? { people: people, count: people.length, server: false } : null;
}

// NOTE: What starting a conversation with "usernames" - the user among them - sends as the room's
// name: for two people "." and the other's name, which the server makes "/<a>/<b>"; for a group the
// prototype's "Private (...)" name, or "" when it is too big to fit.
function dm_create_text( usernames, me )
{
   var people = dm_people( usernames );

   if( ( people.length === 2 ) && ( people.indexOf( me ) >= 0 ) )
      return c_dm_create_prefix + people.filter( function( person ) { return person !== me; } )[ 0 ];

   return dm_room_name( people );
}

// NOTE: Whether "me" may have a conversation with "person" - Ian, 2026-10-05: admin may neither start
// a private room nor be the other person in one, admin being for administration; and a conversation
// with yourself is none. With "person" left out, whether "me" may message anyone at all.
const c_admin_username = "admin";

function dm_allowed( me, person )
{
   if( me === c_admin_username )
      return false;

   if( ( person === undefined ) || ( person === null ) )
      return true;

   return ( person !== c_admin_username ) && ( person !== me );
}

// NOTE: What starting a conversation sends, from "dm_create_text( )"'s name. The server's own room
// ignores who is named with it (Ian, 2026-10-05), so it is made alone and the other person invited
// after ("invite" - "" for a group's room, which takes its people in the one request).
function dm_create_options( name, others )
{
   var is_server_room = ( String( name ).indexOf( c_dm_create_prefix ) === 0 );

   return {
      options: ( is_server_room ? "" : "for=" + ( others || [ ] ).join( "," ) + ";" ) + "text=" + name,
      invite: is_server_room ? ( others || [ ] )[ 0 ] : ""
   };
}

// NOTE: The server's refusals when a conversation is started, in plain words - "others" are the
// people it was for. Anything else is passed on as it came.
function dm_create_problem( reply, others )
{
   var text = String( reply || "" ).trim( ).replace( /^Error: /, "" );

   var unknown = /^User '([^']+)' is not known\.$/.exec( text );

   if( unknown !== null )
      return unknown[ 1 ] + " has not signed in yet, so cannot be messaged.";

   var exists = /^Room '([^']+)' already exists\.$/.exec( text );

   if( ( exists !== null ) && ( exists[ 1 ].indexOf( c_dm_server_prefix ) === 0 ) )
      return "You already have a conversation with " + name_list( others || [ ] )
       + " - if it is not in the rail, look for their request, or it may have been declined.";

   // NOTE: A group's "Private (...)" name is shared by everyone on the server, so another room can hold it first.
   if( exists !== null )
      return "This conversation cannot be started - someone else already has a room called “" + exists[ 1 ]
       + "”. Room names are shared by everyone on the server, so a conversation's name can be taken first.";

   return text;
}

function is_dm_name( name )
{
   return parse_dm_name( name ) !== null;
}

// NOTE: What identifies a conversation whatever form its name takes - its people. Null for a room
// that is not a direct message.
function dm_key( name )
{
   var dm = parse_dm_name( name );

   if( dm === null )
      return null;

   return "people:" + dm.people.slice( ).sort( ).join( "," );
}

// NOTE: How a conversation is shown - the other people, never the room name.
function dm_title( name, me )
{
   var dm = parse_dm_name( name );

   if( dm === null )
      return String( name || "" );

   var others = dm.people.filter( function( person ) { return person !== me; } );

   return ( others.length > 0 ) ? others.join( ", " ) : "Only you";
}

// NOTE: The conversation with exactly these people, if the user already has one - and only if
// its owner is one of them (see "dm_trusted( )"). Room names are unique across the server, so
// there is at most one; the lowest number is taken should that ever change.
function find_dm_room( rooms, usernames )
{
   var key = dm_key( dm_room_name( usernames ) );

   if( key === null )
      return null;

   var found = null;

   ( rooms || [ ] ).forEach( function( entry )
   {
      if( ( dm_key( entry.name ) === key ) && dm_trusted( entry.name, entry.owner ) && ( ( found === null ) || ( entry.room < found.room ) ) )
         found = entry;
   } );

   return found;
}

// NOTE: What already exists for a conversation with "people" - for the New message list and
// its button. "open" - a conversation; "waiting" - one started here that they have not joined
// ("waiting" names such rooms); "request" - their request to the user; "" - nothing yet.
function dm_existing( people, me, rooms, invitations, waiting )
{
   var everyone = dm_people( ( people || [ ] ).concat( [ me ] ) );

   if( everyone.length < 2 )
      return { kind: "", room: "" };

   var room = find_dm_room( rooms, everyone );

   if( room !== null )
      return { kind: ( ( waiting || [ ] ).indexOf( room.room ) >= 0 ) ? "waiting" : "open", room: room.room };

   var key = dm_key( dm_room_name( everyone ) );

   if( key === null )
      return { kind: "", room: "" };

   var request = ( invitations || [ ] ).filter( function( invite )
   {
      return ( dm_key( invite.name ) === key ) && dm_trusted( invite.name, invite.inviter );
   } )[ 0 ];

   if( request )
      return { kind: "request", room: request.room };

   return { kind: "", room: "" };
}

// NOTE: Who has not joined yet. A member sees nothing posted before they joined, so the first
// message of a new conversation waits until everyone it is for has - their ":joined" notices.
function dm_waiting_for( messages, recipients )
{
   var joined = { };

   ( messages || [ ] ).forEach( function( message )
   {
      if( message && ( message.kind === c_kind_system ) && message.event && ( message.event.verb === "joined" ) )
         joined[ message.sender ] = true;
   } );

   return ( recipients || [ ] ).filter( function( name ) { return !joined[ name ]; } );
}

// NOTE: A room's name is set by its owner, so a name alone proves nothing - anyone can call a
// room "DM admin + bob" and invite both, then read what they say (found by review, 2026-09-30).
// So it is taken as a direct message only when "who" - its owner, or the sender of a request -
// is one of the people it names. An owner not yet known is trusted until the listing says who
// it is - joining from a request, whose sender was checked. A server name is trusted as it is: the
// server builds it from the two people, and lets nobody else in.
function dm_trusted( name, who )
{
   var dm = parse_dm_name( name );

   if( dm === null )
      return false;

   if( dm.server || !who )
      return true;

   return dm.people.indexOf( who ) >= 0;
}

// NOTE: Members of a conversation its name leaves out - anyone in the room can read it.
function dm_outsiders( name, members )
{
   var dm = parse_dm_name( name );

   if( dm === null )
      return [ ];

   return ( members || [ ] ).filter( function( member ) { return dm.people.indexOf( member ) < 0; } );
}

// NOTE: "Bob", "Bob and Carol", "Bob, Carol and Dave".
function name_list( names )
{
   var list = names || [ ];

   if( list.length < 2 )
      return list.join( "" );

   return list.slice( 0, -1 ).join( ", " ) + " and " + list[ list.length - 1 ];
}

// NOTE: What a request says - who wants to talk, and who else would be in the conversation.
function dm_request_text( inviter, name, me )
{
   var dm = parse_dm_name( name );

   if( dm === null )
      return inviter + " wants to start a conversation with you.";

   var others = dm.people.filter( function( person ) { return ( person !== me ) && ( person !== inviter ); } );

   if( others.length === 0 )
      return inviter + " wants to message you.";

   return inviter + " wants to start a conversation with you and " + name_list( others ) + ".";
}

// NOTE: Announcements - a prototype, 2026-09-27. Administration is hidden from everyone but
// admin, yet every account is a member of it and the chat already reads it in the background
// for invitations - so admin's ordinary messages there reach every browser, and are shown as
// announcements at the top of each room. The server returns only what was posted after the
// account joined, and only what was meant for it: a message admin sends to named people
// reaches just those people. Room notices (":invite", ":joined") are never announcements.
const c_announcer = "admin";

// NOTE: Dismissed ids are kept per account in "localStorage". Only the newest are kept -
// an old announcement the server no longer returns needs no entry.
const c_max_dismissed = 200;

function pending_announcements( messages, dismissed )
{
   var gone = dismissed || [ ];

   return ( messages || [ ] ).filter( function( message )
   {
      return message && ( message.kind === c_kind_chat ) && ( message.sender === c_announcer )
       && ( gone.indexOf( message.unique ) < 0 );
   } ).sort( function( a, b ) { return Number( b.unique ) - Number( a.unique ); } );
}

// NOTE: A stack of announcements is kept small: past two, only the newest two show and the
// rest wait behind "Show N more". Returns which to show and how many are held back.
const c_announcements_collapsed = 2;

function announcement_stack( list, expanded )
{
   var all = list || [ ];

   if( expanded || ( all.length <= c_announcements_collapsed ) )
      return { shown: all.slice( ), more: 0 };

   return { shown: all.slice( 0, c_announcements_collapsed ), more: all.length - c_announcements_collapsed };
}

// NOTE: Who will see an announcement admin is about to post - for the preview. Admin's own
// name in "for" is only there to keep a copy (ISS-022), so it is left out.
function announcement_audience( recipients, sender )
{
   var others = ( recipients || [ ] ).filter( function( name ) { return name !== sender; } );

   if( others.length === 0 )
      return "Everyone sees this at the top of every room until they dismiss it";

   return "Only " + others.join( ", " ) + " will see this, marked as to them";
}

// NOTE: Anything that is not a list of message ids is treated as nothing dismissed - a
// damaged value then shows announcements again, rather than hiding them for good.
function parse_dismissed( stored )
{
   try
   {
      var value = JSON.parse( stored );

      if( !Array.isArray( value ) )
         return [ ];

      return value.filter( function( id ) { return ( typeof id === "string" ) && /^\d+$/.test( id ); } );
   }
   catch( e )
   {
      return [ ];
   }
}

function add_dismissed( dismissed, unique )
{
   var list = ( dismissed || [ ] ).filter( function( id ) { return id !== unique; } );

   list.push( unique );

   return list.slice( -c_max_dismissed );
}

// NOTE: What the room rail would show as waiting when it is out of sight - on a narrow screen
// it slides away, and this is the count on the button that brings it back. The unread
// messages in the rooms shown there, apart from the one open, and each open invitation.
function unread_elsewhere( rooms, invitations, current_room, is_admin )
{
   var total = ( invitations || [ ] ).length;

   visible_rooms( rooms, is_admin ).forEach( function( entry )
   {
      if( ( entry.room !== current_room ) && ( entry.unread > 0 ) )
         total += entry.unread;
   } );

   return total;
}

// NOTE: A count for a small badge - nothing for none, and capped so it stays small.
function badge_text( count )
{
   if( !( count > 0 ) )
      return "";

   return ( count > 99 ) ? "99+" : String( count );
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

// NOTE: A sign in error in plain words. Ian's "connect" says "invalid or missing connect status"
// when the server's answer to signing in was not a session status (ISS-029); every other error
// is already a sentence and is shown as it is.
const c_connect_status_error = "invalid or missing connect status";

function sign_in_error_text( error )
{
   var text = String( error || "" );

   if( text === c_connect_status_error )
      return "The server's answer to signing in was not what was expected. Please try again.";

   return text;
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

// NOTE: Each page of the node keeps to one browser tab, named for it, so moving between them
// reuses a tab rather than opening another (Damon, 2026-10-02). Lower case and hyphens - names
// starting "_" are reserved by browsers.
const c_tab_chat = "ciyam-chat";
const c_tab_accounts = "ciyam-accounts";
const c_tab_console = "ciyam-console";

// NOTE: What to do with the tab found by a name: "load" an empty one (there was none - the
// browser has just made it), "focus" one already on this session (no reload - it keeps its
// place), or open a "new" one when the tab found is signed in some other way or not at all,
// rather than reload over someone's session.
function choose_tab_action( is_blank, their_sessid, our_sessid )
{
   if( is_blank )
      return "load";

   if( ( ( their_sessid || "" ) !== "" ) && ( their_sessid === our_sessid ) )
      return "focus";

   return "new";
}

// NOTE: The one function here that touches the browser: opens "url" in the tab named "name", as
// "choose_tab_action( )" decides. The pages share an origin, so the tab found can be looked into
// without reloading it. A browser finds a named tab only among tabs related to this one - opened
// from it or from each other - so one opened by typing its address may still get a second tab.
// "on_focus" runs in a tab that is reused, to take it to the right place.
function open_app_tab( name, url, our_sessid, on_focus )
{
   var found = null;

   try
   {
      found = window.open( "", name );
   }
   catch( e )
   {
   }

   if( !found )
   {
      window.open( url, "_blank" );

      return;
   }

   var blank = false;
   var theirs = "";

   try
   {
      blank = ( found.location.href === "about:blank" );
      theirs = ( found.ciyam && found.ciyam.sessid ) ? found.ciyam.sessid : "";
   }
   catch( e )
   {
   }

   var action = choose_tab_action( blank, theirs, our_sessid );

   if( action === "load" )
      found.location.href = url;
   else if( action === "focus" )
   {
      if( on_focus )
         on_focus( found );

      found.focus( );
   }
   else
      window.open( url, "_blank" );
}

// NOTE: A page's address without "source=" - the page it was linked to - keeping everything else
// and the "#...". While linked it stays, so a reload links straight back; once the link has ended
// it goes, so a reload does not try to link again (Damon, 2026-10-02).
function address_without_source( href )
{
   var url = new URL( href );

   url.searchParams.delete( "source" );

   return url.pathname + url.search + url.hash;
}

// NOTE: "Open a linked tab" opens on the room the user is in - "#room=<number>" in its address (Ian,
// 2026-10-02: in a linked tab opened on another room, a message typed in the first did not appear).
const c_room_hash_prefix = "#room=";

function linked_tab_address( href, source, room )
{
   var url = new URL( href );

   url.searchParams.set( "source", source );

   url.hash = /^\d{7}$/.test( String( room || "" ) ) ? c_room_hash_prefix + room : "";

   return url.toString( );
}

// NOTE: The room an address asks for, or "" - only a seven digit room number.
function room_from_hash( hash )
{
   var text = String( hash || "" );

   if( text.indexOf( c_room_hash_prefix ) !== 0 )
      return "";

   var room = text.substring( c_room_hash_prefix.length );

   return /^\d{7}$/.test( room ) ? room : "";
}

// NOTE: In the browser - drops "source=" from the address bar without loading the page again or
// adding to its history.
function forget_source( )
{
   try
   {
      history.replaceState( null, "", address_without_source( window.location.href ) );
   }
   catch( e )
   {
   }
}

// NOTE: "Remember on this browser" - the chat's and the accounts page's sign in. "none" forgets
// the account, "access" keeps its PIN, "full" its PIN and password hash. Three outcomes, not two:
// the harness has had "creds retain partial" all along - keep the PIN, drop the password - which a
// single checkbox could not express, as Ian raised.
const c_retain_none = "none";
const c_retain_access = "access";
const c_retain_full = "full";

// NOTE: What the box shows for an account already used here - its current state, rather than a
// default that would silently forget it at the next sign in.
function retain_mode_of( stored, access, has_hash )
{
   if( ( access === "" ) || ( parse_access_list( stored ).indexOf( access ) < 0 ) )
      return c_retain_none;

   return has_hash ? c_retain_full : c_retain_access;
}

// NOTE: What a sign in does with the choice: the saved list to store ("list", null to remove the
// key) and whether to keep the password hash ("keep_hash" - otherwise it is removed). A hash is
// kept only when there is one to keep.
function plan_retain_choice( stored, access, mode, hashed )
{
   var entries = parse_access_list( stored );

   var pos = entries.indexOf( access );

   if( mode === c_retain_none )
   {
      if( pos >= 0 )
         entries.splice( pos, 1 );

      return { list: format_access_list( entries ), keep_hash: false };
   }

   if( pos < 0 )
      entries.push( access );

   return { list: format_access_list( entries ), keep_hash: ( ( mode === c_retain_full ) && ( hashed !== "" ) ) };
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
      escape_message_text: escape_message_text,
      message_bytes: message_bytes,
      c_max_message_bytes: c_max_message_bytes,
      unescape_message_text: unescape_message_text,
      join_continued_lines: join_continued_lines,
      describe_event: describe_event,
      password_strength: password_strength,
      user_initial: user_initial,
      sign_in_error_text: sign_in_error_text,
      visible_rooms: visible_rooms,
      dm_room_name: dm_room_name,
      dm_create_text: dm_create_text,
      dm_create_problem: dm_create_problem,
      dm_allowed: dm_allowed,
      dm_create_options: dm_create_options,
      parse_dm_name: parse_dm_name,
      is_dm_name: is_dm_name,
      dm_title: dm_title,
      find_dm_room: find_dm_room,
      dm_waiting_for: dm_waiting_for,
      dm_trusted: dm_trusted,
      dm_key: dm_key,
      dm_existing: dm_existing,
      dm_outsiders: dm_outsiders,
      name_list: name_list,
      dm_request_text: dm_request_text,
      unread_elsewhere: unread_elsewhere,
      declined_rooms: declined_rooms,
      pending_announcements: pending_announcements,
      announcement_stack: announcement_stack,
      announcement_audience: announcement_audience,
      c_announcements_collapsed: c_announcements_collapsed,
      seen_by: seen_by,
      merge_new_messages: merge_new_messages,
      needs_full_read: needs_full_read,
      posts_request_value: posts_request_value,
      can_change_posting: can_change_posting,
      pair_private_copies: pair_private_copies,
      private_label: private_label,
      with_sender: with_sender,
      edit_for_value: edit_for_value,
      is_timeout_response: is_timeout_response,
      arrived_after_timeout: arrived_after_timeout,
      c_max_timeouts_in_row: c_max_timeouts_in_row,
      composer_mode: composer_mode,
      parse_dismissed: parse_dismissed,
      add_dismissed: add_dismissed,
      c_max_dismissed: c_max_dismissed,
      badge_text: badge_text,
      encode_channel_field: encode_channel_field,
      decode_channel_field: decode_channel_field,
      parse_access_list: parse_access_list,
      format_access_list: format_access_list,
      retain_mode_of: retain_mode_of,
      choose_tab_action: choose_tab_action,
      address_without_source: address_without_source,
      linked_tab_address: linked_tab_address,
      room_from_hash: room_from_hash,
      plan_retain_choice: plan_retain_choice,
      is_valid_room_name: is_valid_room_name,
      is_valid_username: is_valid_username
   };
}

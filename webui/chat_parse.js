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

// NOTE: Pinned messages (Ian, 2026-10-14) - the sender with "!" in front; at most eight in a room.
const c_pinned_marker = "!";
const c_most_pinned = 8;

// NOTE: Who may post in a room, and the markers used to signal it in the entrance listing.
const c_posts_any = "any";
const c_posts_own = "own";
const c_posts_none = "none";

const c_posts_own_marker = "!";
const c_posts_none_marker = "~";

// NOTE: Room 0000001 - Ian's RESERVED_ROOM, named "Admin" (until 2026-10-11 "Administration", and here the "starting
// room"): invitations and admin's messages; only admin sees it as a room. Ian's STARTING_ROOM is now General, 0000002,
// which everyone joins - an ordinary room to the client.
const c_reserved_room = "0000001";

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

   // NOTE: Pinned (Ian, 2026-10-14) - "!" before the sender, and "*" after it as well when it was edited too ("!name*").
   var pinned = false;

   if( sender.charAt( 0 ) === c_pinned_marker )
   {
      pinned = true;
      sender = sender.substring( 1 );
   }

   var edited = false;

   if( sender.slice( -1 ) === c_edited_marker )
   {
      edited = true;
      sender = sender.substring( 0, sender.length - 1 );
   }

   // NOTE: Reactions after a tab - a tab in the text comes as "\t", so one as it is can only be theirs (2026-10-29, with Ian);
   // then the text's escapes undone ("unescape_message_text( )").
   var tab = remainder.indexOf( "\t" );

   var reactions = ( tab < 0 ) ? null : parse_reactions( remainder.substring( tab + 1 ) );

   remainder = unescape_message_text( ( tab < 0 ) ? remainder : remainder.substring( 0, tab ) );

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

   var entry = {
      kind: c_kind_chat,
      unique: unique,
      sender: sender,
      edited: edited,
      private: is_private,
      text: text
   };

   // NOTE: Only when pinned, or with reactions, so every other message reads as it always has.
   if( pinned )
      entry.pinned = true;

   if( reactions )
      entry.reactions = reactions;

   // NOTE: Only spaces - deleted (Ian, 2026-10-15): there is no deleting yet, so a message is edited to spaces, which the
   // node takes (a new one of only spaces it refuses).
   if( entry.text.trim( ) === "" )
      entry.deleted = true;

   return entry;
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
// NOTE: How the server carries a message's backslashes and line breaks:
//
//   on the way in   "\" escapes the next character and is dropped: "a\b" is stored "ab",
//                   and "\\" is stored as one "\" (verified against the container, 2026-09-26)
//   on the way out  one line a message, its text escaped - see "unescape_message_text( )"
//
// So a backslash the user typed is doubled before sending, and on receipt the escapes undone.
//
// A ";" too (found 2026-10-15): the server splits a request's options on it, so "a; b" was
// stored as "a" - every message with a semicolon was cut short there. After a backslash it is
// kept as typed ("semicolon_probe.js").
// A carriage return never goes: a line break goes as "\n" alone, as a CR at a message's start marks its kind (Ian,
// 2026-10-20 - "message_prefix( )"). Browsers already give a text box's breaks as "\n"; this makes it certain.
function escape_message_text( text )
{
   return String( text || "" ).replace( /\r\n?/g, "\n" ).replace( /[\\;]/g, "\\$&" );
}

// NOTE: A message's kind from its start - a letter and a carriage return (Ian, 2026-10-20): "X\r" an X25519 encrypted message.
// A CR has no other use in a message, so more letters can follow. "" for an ordinary message.
function message_prefix( text )
{
   var m = /^([A-Za-z])\r/.exec( String( text || "" ) );

   return m ? m[ 1 ] : "";
}

// NOTE: What is shown for a message the chat cannot read - an encrypted one, until it can decrypt, and later one whose key it
// does not have; "" for one it can.
function message_unreadable_label( text )
{
   var prefix = message_prefix( text );

   if( prefix === "" )
      return "";

   return ( prefix === "X" ) ? "Encrypted message - this chat cannot read it yet" : "A message of a kind this chat cannot show yet";
}

// NOTE: A message's words for the places that show plain text - the pinned line, Home - or what is shown for one it cannot read.
function message_plain_text( text )
{
   return message_unreadable_label( text ) || strip_format( text );
}

// NOTE: Whether a message is marked as not encrypted - a warning sign before it, as "irc.cin" marks one (Ian, 2026-10-26: as
// nearly every message should be encrypted, only those that aren't are marked, as Element does). Only where it could have been -
// "keyed", a room with a key here, or a direct message with one for the other person: a room with none says so once, under its
// name (Damon, 2026-10-28: a sign before every message was clutter). Not in the entrance room, nor a message of only spaces; an
// encrypted one ("X\r") or one of another kind is never marked.
function is_unencrypted_message( text, room, keyed )
{
   var value = String( text || "" );

   if( !keyed || ( message_prefix( value ) !== "" ) || is_entrance_room( room ) )
      return false;

   return value.trim( ) !== "";
}

// NOTE: Whether the message box keeps the focus once a message is sent - so a follow-up can be typed
// straight away, on a computer. On a phone the focus keeps the on-screen keyboard up, over half the
// screen and the message just sent (Ian, 2026-10-07), so there it lets go. "coarse_pointer" - a touch
// screen as the main pointer, the browser's "(pointer: coarse)" - stands in for an on-screen keyboard.
function keeps_focus_after_send( coarse_pointer )
{
   return !coarse_pointer;
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

// NOTE: A message comes as one line, its text escaped by the web session (2026-10-29, agreed with Ian - the text and JSON
// formats alike): "\\" a backslash, "\n" a line break, "\r" a carriage return and "\t" a tab. Nothing else follows a backslash;
// were it to, it is kept as it came.
const c_message_escapes = { "\\": "\\", "n": "\n", "r": "\r", "t": "\t" };

function unescape_message_text( text )
{
   return String( text || "" ).replace( /\\([\s\S])/g, function( all, c )
   {
      return c_message_escapes.hasOwnProperty( c ) ? c_message_escapes[ c ] : all;
   } );
}

function parse_fetch_response( response )
{
   var result = { error: "", members: [ ], messages: [ ], rooms: [ ], pinned: [ ], has_new: false };

   if( is_error_response( response ) )
   {
      result.error = error_text( response );

      return result;
   }

   if( !response )
      return result;

   // NOTE: One line a message - see "unescape_message_text( )".
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

         // NOTE: Every read of a room returns its pinned messages first (Ian, 2026-10-14) - the room's whole pinned set,
         // each time, so they are not news.
         if( entry.pinned )
            result.pinned.push( entry );
         else
            result.has_new = true;
      }
   }

   result.messages = pair_private_copies( result.messages );

   // NOTE: Pinned ones come first, out of time - put back in their place, so a whole read draws them where they were said.
   if( result.pinned.length > 0 )
      result.messages = in_time_order( result.messages );

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
// NOTE: Who it is to, beside the crossed-out eye - the amber and the eye say private (Damon, 2026-10-28: "private ·" said it twice).
function private_label( entry )
{
   if( entry.recipients && entry.recipients.length )
      return { text: "to " + entry.recipients.join( ", " ),
       title: "Private - only you and " + entry.recipients.join( ", " ) + " can see this" };

   return { text: "to you", title: "Private - sent to you, not to the whole room" };
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

// NOTE: The message Up edits from an empty message box - the newest of the person's own the conversation shows, as
// Element does (Ian, 2026-10-15). "rows" are { unique, editable, text, private }, oldest first; null when there is none.
function last_editable( rows )
{
   for( var i = ( rows || [ ] ).length - 1; i >= 0; i-- )
   {
      if( rows[ i ] && rows[ i ].editable && ( String( rows[ i ].text || "" ).trim( ) !== "" ) )
         return rows[ i ];
   }

   return null;
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

function is_reserved_room( room )
{
   return ( room === c_reserved_room );
}

// NOTE: Decides whether the composer should accept input, and why not when it should not.
// Kept here rather than in the view so the rules are covered by the regression tests.
//
// Three separate restrictions apply:
//  - the entrance room is a lobby and never accepts messages
//  - the reserved room is kept for invitations and admin messages
//  - a room may be set to owner-only or locked with the "posts" option
function posting_status( room, entry, username, is_admin )
{
   if( is_entrance_room( room ) )
      return { can_post: false, reason: "The entrance room is a lobby - pick a room to chat in." };

   if( is_reserved_room( room ) && !is_admin )
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

// ---- A quieter conversation (Damon, 2026-10-28: centred notices among left-aligned messages, every message with its own time and
// name, "Seen by" lines between them - cluttered)

// NOTE: How long apart two of one person's messages may be and still be drawn as one run - the name and time on the first only.
const c_message_run_ms = 5 * 60 * 1000;

// NOTE: Whether a message from "sender" at "unique" goes on the run of the one before it, from "prev_sender" at "prev_unique" -
// the same person, in order, within five minutes. The caller sees to the rest: nothing else between them, and the same day.
function continues_run( prev_sender, prev_unique, sender, unique )
{
   var gap = Number( unique ) - Number( prev_unique );

   return ( sender !== "" ) && ( sender === prev_sender ) && !isNaN( gap ) && ( gap >= 0 ) && ( gap <= c_message_run_ms );
}

// NOTE: A run of notices of one kind ("joined", "left the room") as one line - "tester-1, tester-2 and verify-a joined", or the
// first "most" and how many others: "claim-1, keep-1, test-99 and 25 others joined". Each person once, in order.
function notice_run( names, verb, most )
{
   var seen = { };
   var people = ( names || [ ] ).filter( function( name ) { return seen.hasOwnProperty( name ) ? false : ( seen[ name ] = true ); } );
   var limit = most || 3;
   var shown = ( people.length > limit ) ? people.slice( 0, limit ) : people;
   var others = people.length - shown.length;

   var text = ( others > 0 ) ? shown.join( ", " ) + " and " + others + ( ( others === 1 ) ? " other " : " others " ) + verb
    : name_list( shown ) + " " + verb;

   return { shown: shown, others: others, text: text };
}

// NOTE: "Seen by" as small initials at the end of the row read up to (Damon, 2026-10-28: a line of its own between messages) - the
// first "most", then how many more; the whole said in "label".
function seen_chips( names, most )
{
   var list = names || [ ];
   var limit = most || 3;
   var shown = list.slice( 0, limit );

   return { shown: shown, more: list.length - shown.length, label: ( list.length > 0 ) ? "Seen by " + name_list( list ) : "" };
}

// NOTE: Reactions (Ian, 2026-10-28 and 29) - one of ten, one each on someone else's message: the same again takes it off, another
// replaces it. A web session reads them as the node keeps them: a reaction's two digits, then each person who gave it as six hex of
// the SHA-256 of their name from the eleventh character, an "x" after each, and an "o" between reactions - "014b573fx159e43xo02…".
// Hex has no "o" and no "x", so they cannot be mistaken.
const c_reaction_emojis = [ "👌", "👍", "👎", "🙂", "🙁", "😀", "😲", "🤓", "🤦‍♂️", "❤️" ];
const c_reaction_names = [ "OK", "Thumbs up", "Thumbs down", "Smile", "Frown", "Grin", "Surprised", "Nerd", "Facepalm", "Heart" ];

const c_reactions_pattern = /^0\d(?:[0-9a-f]{6}x)+(?:o0\d(?:[0-9a-f]{6}x)+)*$/;

// NOTE: A message's reactions - [ { code: "01", people: [ "4b573f", ... ] } ], in the node's order - or null if "text" is not
// exactly their form.
function parse_reactions( text )
{
   var value = String( text || "" );

   if( !c_reactions_pattern.test( value ) )
      return null;

   return value.split( "o" ).map( function( part )
   {
      return { code: part.substring( 0, 2 ), people: part.substring( 2 ).split( "x" ).filter( function( s ) { return s !== ""; } ) };
   } );
}

// NOTE: Reactions as shown - each one's emoji, how many gave it, whether one was "mine", and who: "short_names" maps each member's
// six hex to their name; one who is not a member now is "someone".
function reaction_chips( reactions, my_short, short_names )
{
   return ( reactions || [ ] ).map( function( reaction )
   {
      var n = parseInt( reaction.code, 10 );

      var names = reaction.people.map( function( short )
      {
         return ( short === my_short ) ? "you" : ( ( short_names || { } )[ short ] || "someone" );
      } );

      return { code: reaction.code, emoji: c_reaction_emojis[ n ] || "?", count: reaction.people.length,
       mine: reaction.people.indexOf( my_short ) >= 0, label: ( c_reaction_names[ n ] || "Reaction" ) + " - " + name_list( names ) };
   } );
}

// NOTE: What a reaction does, as the node does it - so it is shown at once, before the room is next read whole: one's own taken off
// wherever it was, then given unless it was this one; in the order of their digits.
function apply_reaction( reactions, code, my_short )
{
   var had = "";

   var kept = [ ];

   ( reactions || [ ] ).forEach( function( reaction )
   {
      var people = reaction.people.filter( function( short ) { return short !== my_short; } );

      if( people.length < reaction.people.length )
         had = reaction.code;

      if( people.length > 0 )
         kept.push( { code: reaction.code, people: people } );
   } );

   if( had !== code )
   {
      var found = kept.filter( function( reaction ) { return reaction.code === code; } );

      if( found.length > 0 )
         found[ 0 ].people.push( my_short );
      else
         kept.push( { code: code, people: [ my_short ] } );
   }

   return kept.sort( function( a, b ) { return ( a.code < b.code ) ? -1 : ( ( a.code > b.code ) ? 1 : 0 ); } );
}

// NOTE: A reaction is an edit of someone else's message to a space and its two digits (Ian, 2026-10-28).
function reaction_options( unique, code )
{
   return "for=" + unique + ";text= " + code;
}

// NOTE: The node refusing a message while it handles another (Ian's "@irc_lock", "a658ef5b", 2026-10-30) - one at a time, the next
// refused rather than kept waiting; the chat tries it once more ("watch_timeouts( )").
function is_irc_lock_refusal( response )
{
   return /Unable to obtain the system IRC lock/.test( String( response || "" ) );
}

// NOTE: How a message is drawn, but for its reactions (drawn on their own) - so a room read whole again redraws only a message
// someone has edited or deleted since (Ian, 2026-10-29: a poll brings only new messages).
function message_state( entry )
{
   return ( entry.edited ? "*" : "" ) + ( entry.deleted ? "-" : "" ) + ( entry.pinned ? "!" : "" ) + " " + ( entry.text || "" );
}

// NOTE: Only someone else's ordinary message, not deleted - not one for some only, for now, nor a system line.
function can_react( entry, me )
{
   return !!entry && ( entry.kind === c_kind_chat ) && ( me !== "" ) && ( entry.sender !== me ) && !entry.deleted && !entry.private;
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

// NOTE: When a read of only what is new is not enough, and the Admin room must be read whole
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
   // NOTE: Not the pinned - they come with every read, so say nothing of what is new (found by review, 2026-10-14: one
   // pinned announcement and nothing new never looked empty again).
   fresh = ( fresh || [ ] ).filter( function( m ) { return !m.pinned; } );

   if( fresh.length === 0 )
      return true;

   return fresh.some( function( m ) { return m.event && ( c_invitation_verbs.indexOf( m.event.verb ) >= 0 ); } );
}

// NOTE: The "posts" option for a room's messages PUT - who may post. The server takes only
// "ANY", "OWN" or "NONE", in capitals ("Unknown room posts value 'own'" otherwise), from
// the owner or admin, and never for the Admin room. "" for anything else.
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
   if( !entry || is_entrance_room( room ) || is_reserved_room( room ) )
      return false;

   return is_admin || ( ( entry.owner || "" ) === username );
}

// NOTE: Whether this user may leave a room (Ian, 2026-10-12: "I can't seem to find any UI to leave a room"). Not the
// entrance, nor the Admin room - it carries every invitation - nor a room they own: Ian thinks leaving your own room is
// probably wrong, and it would be left with nobody to look after it. General can be left since his change of 2026-10-12.
function can_leave_room( room, entry, username )
{
   if( !entry || is_entrance_room( room ) || is_reserved_room( room ) )
      return false;

   return ( entry.owner || "" ) !== username;
}

// NOTE: Who this user has already invited to a room, from the ":issued" receipts the
// server posts to their Admin room. The server accepts a duplicate invitation
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
// NOTE: The invitations still waiting to be taken up, from the messages in the reserved room.
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

      if( joined[ event.room ] || is_entrance_room( event.room ) || is_reserved_room( event.room ) )
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

// NOTE: The rooms shown in the rail. The reserved room - the Admin room - is shown to admin
// only; it still exists for everyone, and is read in the background for invitations.
function visible_rooms( rooms, is_admin )
{
   if( is_admin )
      return ( rooms || [ ] ).slice( );

   return ( rooms || [ ] ).filter( function( entry ) { return !is_reserved_room( entry.room ); } );
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

// NOTE: What starting a conversation sends, from "dm_create_text( )"'s name - its people named with it,
// so the one request makes the room and invites them. The server's own room takes a name only when it
// is the one after the dot (Ian, 2026-10-05, "7c7cc6e7"), and that is the one "dm_create_text( )" gives.
function dm_create_options( name, others )
{
   return "for=" + ( others || [ ] ).join( "," ) + ";text=" + name;
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

// NOTE: Encrypted direct messages (2026-10-25), as the console has them (Ian, 2026-10-24: "irc.cin" encrypts every message in
// a direct message whose other person's public key it has). The other person in a direct message the server named ("/a/b",
// which the console takes as one), or "" - for any other room, a group, or one "me" is not in.
function dm_other_person( name, me )
{
   var dm = parse_dm_name( name );

   if( ( dm === null ) || !dm.server || ( dm.people.indexOf( me ) < 0 ) )
      return "";

   return dm.people[ 0 ] === me ? dm.people[ 1 ] : dm.people[ 0 ];
}

// NOTE: What the line under a direct message adds about encryption - "keys": whether this browser has your keys (null while
// they are still being read, "unread" if the node's answer could not be read), "theirs": whether you have the other person's
// public key.
function dm_encryption_note( other, keys, theirs )
{
   if( ( other === "" ) || ( keys === null ) )
      return "";

   if( keys === "unread" )
      return "not encrypted - your keys could not be read just now; reload to try again";

   if( !keys )
      return "not encrypted - set up your keys on My account";

   if( !theirs )
      return "not encrypted - add " + other + "'s public key on My account";

   return "encrypted";
}

// NOTE: The same for a group's direct message (Damon, 2026-10-27: it said nothing, the warning signs the only sign) - with no one
// other person to share a key with, it takes a room key, sent to each of them in an encrypted direct message. "keyed": whether
// this browser has one for it.
function group_encryption_note( keys, keyed )
{
   if( keys === null )
      return "";

   if( keys === "unread" )
      return "not encrypted - your keys could not be read just now; reload to try again";

   if( !keys )
      return "not encrypted - set up your keys on My account";

   return keyed ? "encrypted with the room key" : "not encrypted - encrypt it in Chat Details";
}

// NOTE: Who a room's key is sent to - each other person once, in order; not "me".
function room_key_recipients( names, me )
{
   return dm_people( names ).filter( function( name ) { return name !== me; } );
}

// NOTE: What sending a room's key to its members came to. Each of "outcomes" is { name, how }: "sent"; "held" - in a direct
// message started for it, sent once they accept; "no_key" - no public key for them, so it could not be sealed; "request" - their
// direct message is to be accepted first; "failed".
function room_key_sent_text( outcomes )
{
   var by = { sent: [ ], held: [ ], no_key: [ ], request: [ ], failed: [ ] };

   ( outcomes || [ ] ).forEach( function( outcome )
   {
      if( by.hasOwnProperty( outcome.how ) )
         by[ outcome.how ].push( outcome.name );
   } );

   var parts = [ ];

   if( by.sent.length > 0 )
      parts.push( "Sent to " + name_list( by.sent ) + "." );

   if( by.held.length > 0 )
      parts.push( "Sent to " + name_list( by.held ) + " once " + ( ( by.held.length === 1 ) ? "they accept" : "each accepts" )
       + " your direct message - keep this page open until then." );

   if( by.no_key.length > 0 )
      parts.push( "Not sent to " + name_list( by.no_key ) + " - add " + ( ( by.no_key.length === 1 ) ? "their public key" : "their public keys" )
       + " on My account first." );

   if( by.request.length > 0 )
      parts.push( "Not sent to " + name_list( by.request ) + " - accept " + ( ( by.request.length === 1 ) ? "their direct message" : "their direct messages" )
       + " first." );

   if( by.failed.length > 0 )
      parts.push( "Could not send it to " + name_list( by.failed ) + " - try again." );

   return ( parts.length > 0 ) ? parts.join( " " ) : "There is no one else here to send it to.";
}

// NOTE: Room keys (Ian, 2026-10-25) - "irc.cin"'s "k" makes "<room>#<32 hex digits>" to send each member in an encrypted direct
// message, and "r" with it saves it; every message in that room is then sealed with the 32 digits as they are. Not for the
// entrance or reserved rooms, nor a direct message (named "/..."), which uses the other person's public key.
const c_room_key_pattern = /^([0-9]{7})#([0-9a-f]{32}|[0-9a-f]{64})$/;

// NOTE: "k" makes 32 hex digits; the console takes any length as the key, and one of Ian's had 64 (2026-10-26) - so 64 too.
const c_room_key_digits = /^([0-9a-f]{32}|[0-9a-f]{64})$/;

// NOTE: A key at the start of a message, anything after it (Ian, 2026-10-26: "Text: [room#secret] *").
const c_room_key_leading = /^([0-9]{7})#([0-9a-fA-F]{64}|[0-9a-fA-F]{32})(?![0-9a-fA-F])/;

function can_have_room_key( room, name )
{
   return /^[0-9]{7}$/.test( String( room || "" ) ) && !is_entrance_room( room ) && !is_reserved_room( room )
    && ( String( name || "" ).charAt( 0 ) !== "/" );
}

// NOTE: As the console shows it, to be sent and pasted.
function room_key_text( room, key )
{
   return room + "#" + key;
}

// NOTE: A room key given for "room" - as the console shows it ("<room>#<key>") or the key alone, spaces and case aside -
// { key } or { problem } to say.
function parse_room_key( text, room )
{
   // NOTE: The whole message pasted, words after the key - the key taken from its start (2026-10-26).
   var lead = room_key_in_message( text );

   if( lead )
      text = room_key_text( lead.room, lead.key );

   var given = String( text || "" ).replace( /\s+/g, "" ).toLowerCase( );
   var whole = c_room_key_pattern.exec( given );

   if( whole && ( whole[ 1 ] !== room ) )
      return { key: "", problem: "That key is for room #" + whole[ 1 ] + ", not this one (#" + room + ")." };

   var key = whole ? whole[ 2 ] : given;

   if( !c_room_key_digits.test( key ) )
      return { key: "", problem: "A room key is " + room + "# followed by 32 or 64 letters and digits (0-9, a-f) - copy all of it." };

   return { key: key, problem: "" };
}

// NOTE: A message that starts with a room key - one shared in an encrypted direct message, perhaps with words after it (Ian,
// 2026-10-26) - as { room, key }, or null.
function room_key_in_message( text )
{
   var lead = c_room_key_leading.exec( String( text || "" ).trim( ) );

   return lead ? { room: lead[ 1 ], key: lead[ 2 ].toLowerCase( ) } : null;
}

// NOTE: A message that starts with a room key, the key cut short - its room, and its first and last four digits (Ian, 2026-10-27:
// not left on the screen for anyone looking over a shoulder). Any other message as it is.
function room_key_masked( text )
{
   var value = String( text || "" ).replace( /^\s+/, "" );
   var lead = c_room_key_leading.exec( value );

   if( !lead )
      return String( text || "" );

   var key = lead[ 2 ].toLowerCase( );

   return lead[ 1 ] + "#" + key.substr( 0, 4 ) + "…" + key.substr( -4 ) + value.substring( lead[ 0 ].length );
}

// NOTE: The people picker ("picker.js", Damon, 2026-10-27) - whether a person is shown for what is typed in its search: their
// name holding it, in any case.
function picker_matches( name, query )
{
   var typed = String( query || "" ).trim( ).toLowerCase( );

   return ( typed === "" ) || ( String( name || "" ).toLowerCase( ).indexOf( typed ) >= 0 );
}

// NOTE: Enter in the search, the list narrowed to one person, chooses them (Damon, 2026-10-27). "shown" - those the search
// shows, each { name, available }; "" unless exactly one, and one who can be chosen.
function picker_enter_choice( shown )
{
   var list = shown || [ ];

   return ( ( list.length === 1 ) && ( list[ 0 ].available !== false ) ) ? String( list[ 0 ].name ) : "";
}

// NOTE: Above the picker's list - how many there are, or how many of them are chosen.
function picker_count_text( chosen, total )
{
   if( chosen === 0 )
      return total + ( ( total === 1 ) ? " person" : " people" );

   return chosen + " chosen of " + total;
}

// NOTE: Whether a message can be sealed - at most 1,000 bytes of it ("ciyam_secure_message.js", as "irc_extra.cin"'s
// "MAX_TO_ENCRYPT" - 500 until Ian's "21ae2f5f", 2026-10-29).
const c_sealed_most_bytes = 1000;

function sealed_too_long( text )
{
   return new TextEncoder( ).encode( String( text || "" ) ).length > c_sealed_most_bytes;
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
// message of a new conversation waits until everyone it is for has - their ":joined" notices, or
// their name in the room's member line ("members", from "parse_members( )"), which still has them
// once a long conversation's ":joined" has gone from the queue. Someone only invited is not in it.
function dm_waiting_for( messages, recipients, members )
{
   var joined = { };

   ( messages || [ ] ).forEach( function( message )
   {
      if( message && ( message.kind === c_kind_system ) && message.event && ( message.event.verb === "joined" ) )
         joined[ message.sender ] = true;
   } );

   ( members || [ ] ).forEach( function( member )
   {
      if( member && member.name )
         joined[ member.name ] = true;
   } );

   // NOTE: Someone who joined and has since left ("_:remove") is not waited for either - otherwise,
   // once their ":joined" has gone from the queue, a reload would hold everything for them for good.
   ( messages || [ ] ).forEach( function( message )
   {
      if( message && ( message.kind === c_kind_system ) && message.event && ( message.event.verb === "remove" ) )
         joined[ message.sender ] = true;
   } );

   return ( recipients || [ ] ).filter( function( name ) { return !joined[ name ]; } );
}

// NOTE: What a decline does to a conversation still waiting - "waiting" who it waits for, "outcome"
// from "dm_invite_outcomes( )". Whoever declined is waited for no longer; while anyone else is, what is
// held stays ("keep"). Once nobody is, it is sent if anyone it was for is left - they joined - and
// dropped only when everyone declined (found by review: in a group, one decline after the other had
// joined dropped what was held for the one who joined).
//
// Returns { waiting: [ names ], held: "keep" | "send" | "drop" }.
function dm_after_declines( waiting, outcome )
{
   var declined = ( outcome && outcome.declined ) || [ ];

   var left = ( waiting || [ ] ).filter( function( name ) { return declined.indexOf( name ) < 0; } );

   if( left.length > 0 )
      return { waiting: left, held: "keep" };

   var still = ( ( outcome && outcome.invited ) || [ ] ).filter( function( name ) { return declined.indexOf( name ) < 0; } );

   return { waiting: [ ], held: ( still.length > 0 ) ? "send" : "drop" };
}

// NOTE: What became of the conversations this user started, from the reserved room - kept by the
// server, so it outlasts a reload, as what the chat holds does not. Their own ":issued (invite for
// <room> sent to <names>)" says whom each was for, and an invitee's ":reject (invite for <room> was
// rejected)" that they declined (verified 2026-10-05, "dm_decline_probe.js"). Nothing here says who
// has joined - that is the room's own member line ("dm_waiting_for( )").
//
// Returns { <room>: { invited: [ names ], declined: [ names ] } }, each name once.
function dm_invite_outcomes( messages, me )
{
   var outcomes = { };

   ( messages || [ ] ).forEach( function( message )
   {
      var event = message && message.event;

      if( !event || !event.room || ( event.verb !== "issued" ) || ( event.issued_kind !== "invite" ) || !me || ( message.sender !== me ) )
         return;

      var outcome = outcomes[ event.room ] || ( outcomes[ event.room ] = { invited: [ ], declined: [ ] } );

      ( event.recipients || [ ] ).forEach( function( name )
      {
         if( name && ( name !== me ) && ( outcome.invited.indexOf( name ) < 0 ) )
            outcome.invited.push( name );
      } );
   } );

   // NOTE: Only a decline by someone this user invited counts - it is that invitation it answers.
   ( messages || [ ] ).forEach( function( message )
   {
      var event = message && message.event;

      var outcome = event && ( event.verb === "reject" ) && event.room ? outcomes[ event.room ] : null;

      if( outcome && ( outcome.invited.indexOf( message.sender ) >= 0 ) && ( outcome.declined.indexOf( message.sender ) < 0 ) )
         outcome.declined.push( message.sender );
   } );

   return outcomes;
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

// NOTE: Announcements - a prototype, 2026-09-27. The Admin room is hidden from everyone but
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
      return message && ( message.kind === c_kind_chat ) && ( message.sender === c_announcer ) && !message.deleted
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

// NOTE: A page's title with what waits in front of it - "(3) CIYAM Chat" - so a tab in the background shows it, as other
// chat apps do (Damon, 2026-10-12). The badge's own cap; nothing waiting, the title alone.
function page_title( title, count )
{
   var waiting = badge_text( count );

   return ( waiting !== "" ) ? "(" + waiting + ") " + title : title;
}

// NOTE: What signing in is doing, as "ciyam.js"'s "report_progress( )" says it (Ian, 2026-10-25: a progress whilst it is
// hashing, or waiting) - the words, and how far along it is (0 to 1) when that is known, for a bar; null for none.
function sign_in_progress( stage, value )
{
   if( stage === "securing" )
   {
      var fraction = Math.max( 0, Math.min( 1, Number( value ) || 0 ) );

      return { text: "Securing your password… " + Math.round( fraction * 100 ) + "%", fraction: fraction };
   }

   if( stage === "waiting" )
   {
      var seconds = Math.max( 1, Math.round( Number( value ) || 1 ) );

      return { text: "Checking it another way in " + seconds + ( seconds === 1 ? " second…" : " seconds…" ), fraction: null };
   }

   if( stage === "updating" )
      return { text: "Updating how your password is kept…", fraction: null };

   return { text: "Contacting the server…", fraction: null };
}

// NOTE: The password strength rules from "test_bip39.html", so both pages rate a password
// the same way. Under seven characters is unsatisfactory; otherwise the length is scaled by
// how many kinds of character are used - digits alone count least - and the score banded.
// Level 0 cannot be saved; the rest are allowed, with the rating shown.
//
// Where "zxcvbn.min.js" has been read in (only where a password is chosen - "signin.js"), the rating is instead by how many
// guesses it would take (Ian, 2026-10-23: "zxcvbn seems like a good option to add to the mix") - what decides how long it
// would last, as every guess costs the same hardening. Under 10^8 is too easy to guess and cannot be saved (at 100,000 rounds
// one graphics card makes about 10,000 guesses a second, so hours); each band after a hundred times harder, to Very Strong
// past 10^14; with zxcvbn's own warning ("This is a top-100 common password") where it has one. These rules stay as they
// were until it is read in, and for "test_bip39.html", which keeps them.
const c_password_min_length = 7;

const c_password_guess_ratings = [
   { level: 0, min: 0, text: "Too easy to guess" },
   { level: 1, min: 8, text: "Weak" },
   { level: 2, min: 10, text: "Moderate" },
   { level: 3, min: 12, text: "Strong" },
   { level: 4, min: 14, text: "Very Strong" }
];

const c_password_ratings = [
   { level: 1, min: 0, text: "Weak" },
   { level: 2, min: 24, text: "Moderate" },
   { level: 3, min: 48, text: "Strong" },
   { level: 4, min: 80, text: "Very Strong" }
];

// NOTE: "inputs" - what zxcvbn should count as easily guessed for this account: its PIN, its username.
function password_strength( password, inputs )
{
   var text = String( password || "" );

   if( text.length === 0 )
      return { level: -1, text: "" };

   if( text.length < c_password_min_length )
      return { level: 0, text: "Unsatisfactory" };

   if( typeof zxcvbn === "function" )
      return password_strength_by_guesses( zxcvbn( text, ( inputs || [ ] ).map( String ) ) );

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

// NOTE: A rating from zxcvbn's estimate - its "guesses_log10", banded - and its warning, if it has one, after it.
function password_strength_by_guesses( estimate )
{
   var log10 = Number( ( estimate || { } ).guesses_log10 ) || 0;
   var warning = String( ( ( estimate || { } ).feedback || { } ).warning || "" );

   var rating = c_password_guess_ratings[ 0 ];

   for( var i = 1; i < c_password_guess_ratings.length; i++ )
   {
      if( log10 >= c_password_guess_ratings[ i ].min )
         rating = c_password_guess_ratings[ i ];
   }

   if( warning !== "" )
      warning = warning.charAt( 0 ).toLowerCase( ) + warning.slice( 1 );

   return { level: rating.level, text: rating.text + ( warning !== "" ? " - " + warning : "" ) };
}

// NOTE: Why a password cannot be saved, from its rating - too short, or (rated by zxcvbn) too easy to guess.
function password_refusal( strength )
{
   if( ( strength || { } ).text === "Unsatisfactory" )
      return "Choose a password of at least 7 characters.";

   return "That password is too easy to guess - choose a longer one, or Suggest one.";
}

// NOTE: A passphrase to offer (Ian, 2026-10-23: "a suggested passphrase of five words seems like a good idea") - "count"
// words from the BIP39 list, each picked by eleven bits of a random 16-bit value ("random"), as the list is 2048 words, so
// none is likelier than another: five are 55 bits. Joined by "-", which reads well and types anywhere.
function suggested_passphrase( words, random, count )
{
   var list = words || [ ];
   var picked = [ ];

   if( list.length !== 2048 )
      return "";

   for( var i = 0; i < ( count || 5 ); i++ )
      picked.push( list[ Number( random[ i ] ) & 0x7ff ] );

   return picked.join( "-" );
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

// NOTE: "locked", when known - whether "/system" says the node is locked. "Unable to start a web session" is how a locked
// node refuses a sign in, but not only a locked one: a node waiting on its chat (IRC) that never starts refuses it too
// (Ian's "web_session.cin", 2026-10-13) - so a node known not to be locked is not called locked.
function sign_in_error_text( error, locked )
{
   var text = String( error || "" );

   if( text === c_connect_status_error )
      return "The server's answer to signing in was not what was expected. Please try again.";

   if( is_blocked_device_error( text ) )
      return c_sign_in_blocked;

   if( ( locked === true ) && ( text !== "" ) )
      return c_sign_in_locked;

   if( is_locked_sign_in_error( text ) )
      return ( locked === false ) ? c_sign_in_no_session : c_sign_in_locked;

   // NOTE: The node's own words, without its "Error: " - the panel says it is one (2026-10-10).
   return text.replace( /^Error:\s*/, "" );
}

// NOTE: Recovering a node with the twelve words written down when it was set up (Ian, 2026-10-09: how the UI must do
// it when a PIN and password cannot be given) - offered by the shared sign in in every part of the node ("signin.js",
// 2026-10-10; Home's alone before). The words become the node's entropy in the page - Ian's BIP39 library, which also
// says whether they fit together (their checksum); these say what else is wrong with them, in words.
const c_recovery_words = 12;

const c_recovery_words_mismatched = "These twelve words don't fit together - check each word, and that they are in order.";

// NOTE: The words as typed or pasted - into one box or across them, with spaces, commas, line breaks or numbers
// ("1. taxi 2. act") between - in lower case, anything else dropped.
function recovery_words( text )
{
   return String( text || "" ).toLowerCase( ).split( /[^a-z]+/ ).filter( function( word ) { return word !== ""; } );
}

// NOTE: "" when there are twelve and each is one of the words; else what is wrong. An unknown word is said before a
// count, since a misspelling is the likelier slip.
function recovery_words_problem( words, wordlist )
{
   var list = words || [ ];

   if( list.length === 0 )
      return "Type or paste your twelve words.";

   var unknown = list.filter( function( word ) { return ( wordlist || [ ] ).indexOf( word ) < 0; } );

   if( unknown.length > 0 )
      return "\"" + unknown[ 0 ] + "\" isn't one of the words - check its spelling.";

   if( list.length !== c_recovery_words )
      return "That's " + list.length + ( list.length === 1 ? " word" : " words" ) + " - there are twelve.";

   return "";
}

// NOTE: The words a box offers as it is typed in - those starting with what is typed, as an autocomplete does (Damon,
// 2026-10-10: the browser's own list matched anywhere in a word), at most "limit". None once the word is whole and the
// only one, or for nothing typed.
function word_completions( typed, wordlist, limit )
{
   var prefix = String( typed || "" ).trim( ).toLowerCase( );

   if( ( prefix === "" ) || /[^a-z]/.test( prefix ) )
      return [ ];

   var matches = ( wordlist || [ ] ).filter( function( word ) { return word.indexOf( prefix ) === 0; } );

   if( ( matches.length === 1 ) && ( matches[ 0 ] === prefix ) )
      return [ ];

   return matches.slice( 0, ( limit > 0 ) ? limit : 6 );
}

// NOTE: The recovery cards' words - for a locked node (the sign in's "Can't sign in?"), or, "fresh", for a node not set up
// that takes the place of one lost, failed or moved (Damon, 2026-10-13: setting up's "Recover a node from its twelve
// words"). There nothing can check the words - any twelve that fit together make a node - hence "caution"; and the
// identity comes back, not the data.
function recovery_texts( fresh )
{
   if( !fresh )
   {
      return { heading: "Recover this node",
       lede: "With the twelve words written down when it was set up. The node checks them and takes a new master password, then"
        + " shows admin's PIN.",
       caution: "", back: "Back to sign in", done_heading: "Recovered", done_lede: "The node is unlocked, with its new master password.",
       done_note: "Write it down - it may be a new one, and only this one works now. Then make a few unlock keys, in Home, so a"
        + " restart can be undone without the words." };
   }

   return { heading: "Recover a node from its twelve words",
    lede: "This machine takes the place of one that was lost, failed or moved. With the words written down when that node was"
     + " set up, it becomes that node again - and the nodes holding its backups know it.",
    caution: "Check them against your paper. This machine has nothing to compare them with - twelve words that fit together"
     + " but aren't your node's would make a different node, one your backups don't know.",
    back: "Back", done_heading: "Recovered - it's your node again",
    done_lede: "Its identity is back from its words, with a new master password - so the nodes holding its backups know it.",
    done_note: "Write admin's PIN down - only this one works now. The node starts without its people, rooms and messages: they"
     + " come back when its data is restored from its backups, which isn't offered here yet." };
}

// NOTE: The start point only moves on (2026-10-14) - a room's pinned messages come with every read, however old, and a
// read of nothing new but them would otherwise take it back to the oldest. "" is no start point yet.
function later_start_point( current, next )
{
   if( ( next === "" ) || ( ( current !== "" ) && ( current >= next ) ) )
      return current;

   return next;
}

// NOTE: Messages in the order they were said - by unique, those sharing one (a private copy and its receipt) as they came.
function in_time_order( messages )
{
   return ( messages || [ ] ).map( function( entry, i ) { return { entry: entry, i: i }; } ).sort( function( a, b )
   {
      var x = String( a.entry.unique || "" );
      var y = String( b.entry.unique || "" );

      return ( x === y ) ? ( a.i - b.i ) : ( ( x < y ) ? -1 : 1 );
   } ).map( function( item ) { return item.entry; } );
}

// NOTE: Pinning is an edit to a single space, which pins or unpins (Ian, 2026-10-14). Only the sender's own, and only
// ordinary messages - not private ones, nor system events ("can_pin( )").
function pin_options( unique )
{
   return "for=" + unique + ";text= ";
}

function can_pin( entry, me )
{
   return !!entry && ( entry.kind === c_kind_chat ) && ( entry.sender === me ) && !entry.private && !entry.deleted && ( me !== "" );
}

// NOTE: Deleting one's own message - an edit to two spaces, public or private ("edit_for_value( )"); a single space would
// pin it instead (Ian, 2026-10-15).
function delete_options( unique, is_private )
{
   return "for=" + edit_for_value( unique, is_private ) + ";text=  ";
}

// NOTE: The node's refusals of a pin, said plainly - anything else as it came.
function pin_error_text( error )
{
   var text = String( error || "" ).replace( /^Error:\s*/, "" ).trim( );

   if( /Maximum pinned messages/i.test( text ) )
      return "This room already has " + c_most_pinned + " pinned messages, the most it can hold - unpin one first.";

   if( /modified to or from private/i.test( text ) )
      return "A private message can't be pinned.";

   return text;
}

// NOTE: What the pinned strip shows, to redraw it only when that changes.
function pinned_signature( list )
{
   // NOTE: Its reactions too - shown in the strip (Damon, 2026-10-29).
   return ( list || [ ] ).map( function( m )
   {
      return m.unique + ( m.edited ? "*" : "" ) + m.sender + " " + m.text + "\t" + JSON.stringify( m.reactions || [ ] );
   } ).join( "\n" );
}

// NOTE: The node's refusals, said plainly - anything else as it came. Words that are not the node's are answered
// "locked for administration" today - the mismatch is set, then a later check for an administration lock replaces it
// (seen 2026-10-09, for Ian). It is also the answer while someone else is signing in as admin, which is rare.
// "fresh" - recovering onto a node not set up: no words can be wrong there, and it is not locked, so a refusal means
// someone else is setting it up just now (found by review, 2026-10-13).
function recovery_error_text( error, fresh )
{
   var text = String( error || "" ).replace( /^Error:\s*/, "" ).trim( );

   if( fresh && /locked for administration|not valid|has expired/i.test( text ) )
      return "Someone else is setting this node up just now - if it isn't you, disconnect it from the network. Otherwise try"
       + " again in a few minutes.";

   if( /identity mismatch/i.test( text ) )
      return "Those aren't this node's twelve words.";

   if( /not currently locked/i.test( text ) )
      return "This node isn't locked, so there is nothing to recover - sign in as usual.";

   if( /locked for administration/i.test( text ) )
      return "The node did not accept those words - check each one, and that they are in order.";

   return text;
}

// NOTE: Which step a refusal belongs on, the recovery being two (Damon, 2026-10-13): "words" for words the node would not
// take, to be put right there - not its words, or the "locked for administration" it answered for them before; "password"
// for anything else. Onto a node not set up no words can be wrong, so never "words" ("recovery_error_text( )").
function recovery_error_step( error, fresh )
{
   if( fresh )
      return "password";

   return /identity mismatch|locked for administration/i.test( String( error || "" ) ) ? "words" : "password";
}

// NOTE: How a locked node answers a sign in today - it cannot start the session (2026-10-07, for Ian).
function is_locked_sign_in_error( error )
{
   return /unable to start a web session/i.test( String( error || "" ) );
}

const c_sign_in_locked = "This node is locked - it restarted, and nobody can sign in until it is unlocked.";

const c_sign_in_no_session = "The node couldn't start a session just now - wait a moment, then try again.";

// NOTE: The node's refusal of a blocked device (Ian, "d370affc", ISS-053) - its own error since, where it had "Invalid device
// identity", which a sign in with a typed password takes as a node set up again and tries once more as a new device.
const c_sign_in_blocked = "This browser has been blocked from signing in as this account, from My account on one of its devices.";

function is_blocked_device_error( error )
{
   return /This device is no longer available/.test( String( error || "" ) );
}

// NOTE: The node's chat (IRC) not turned on - every room and message request answered "IRC usage is not currently
// available." (Ian, 2026-10-13: the default on his development node). The session itself is good; the chat says why.
function irc_unavailable( response )
{
   return /IRC usage is not currently available/i.test( String( response || "" ) );
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

// NOTE: The app switcher - the same in every app (Damon, 2026-10-08: no "Back to", one switcher, its button the
// CIYAM mark and the app's name). Every app, this one marked: Home, Chat, the accounts page - "My account" for
// everyone since People moved into Home (2026-10-11), "Accounts" for admin before - and the console, admin's and never
// on a phone. Each with its page and its tab; Home's tab is "c_tab_home" in "home_parse.js", named here as it is,
// since the Home page loads both.
function switcher_apps( is_admin, is_phone, current )
{
   var apps = [
      { key: "home", title: "Home", note: "The node, and what needs you", page: "home.html", tab: "ciyam-home" },
      { key: "chat", title: "Chat", note: "Rooms and messages", page: "chat.html", tab: c_tab_chat },
      { key: "account", title: switcher_title( "account", is_admin ), note: "Your password and devices",
       page: "account.html", tab: c_tab_accounts }
   ];

   if( is_admin && !is_phone )
      apps.push( { key: "console", title: "Console", note: "Admin · developer tools", page: "console.html", tab: c_tab_console } );

   return apps.map( function( app )
   {
      app.current = ( app.key === current );

      return app;
   } );
}

// NOTE: "Sign out" from any of them - the session is shared, not owned, so whichever app signs out
// tells every tab on the same session, on a channel of its own ("ciyam_session"), whoever opened whom. Any
// other message, or one for another session, is not for this tab.
const c_session_channel_name = "ciyam_session";

function signed_out_message( sessid )
{
   return { kind: "signed_out", sessid: String( sessid || "" ) };
}

function is_signed_out_here( data, sessid )
{
   return !!data && ( typeof data === "object" ) && ( data.kind === "signed_out" )
    && ( typeof data.sessid === "string" ) && ( data.sessid !== "" ) && ( data.sessid === sessid );
}

// NOTE: The app's name beside the CIYAM mark on the switcher's button.
function switcher_title( key, is_admin )
{
   var titles = { home: "Home", chat: "Chat", account: "My account", console: "Console" };

   return titles[ key ] || "";
}

// NOTE: The session this tab is on, kept for the tab, so an app switched to in it - or a reload - carries on without a
// second sign in (Damon, 2026-10-09: the switcher opens apps in this tab). In "sessionStorage" - one for each tab,
// gone when it closes, and shared by every app the tab shows. Only what requests need once signed in: never the
// hashed password, which is used only to work out the session at the sign in. Read back, anything not of that shape
// is nothing. Home kept its own from 2026-10-08.
const c_tab_session_key = "ciyam.session";

function format_tab_session( ciyam_like )
{
   return JSON.stringify( { access: ciyam_like.access, device: ciyam_like.device, sessid: ciyam_like.sessid,
    unique: ciyam_like.unique, username: ciyam_like.username, is_admin: !!ciyam_like.is_admin } );
}

function parse_tab_session( stored )
{
   try
   {
      var value = JSON.parse( stored );

      if( !value || ( typeof value !== "object" ) )
         return null;

      var text = function( name ) { return ( typeof value[ name ] === "string" ) ? value[ name ] : ""; };

      if( !/^[0-9]{5}$/.test( text( "access" ) ) || !/^[0-9a-f]+$/i.test( text( "sessid" ) ) || ( text( "device" ) === "" ) )
         return null;

      return { access: text( "access" ), device: text( "device" ), sessid: text( "sessid" ), unique: text( "unique" ),
       username: text( "username" ), is_admin: ( value.is_admin === true ) };
   }
   catch( e )
   {
      return null;
   }
}

// NOTE: Choosing an app opens it in this tab, unless a new one is asked for - by the button at the row's right, or
// the browser's own ways of asking: ctrl or cmd and click, shift and click, the middle button.
function switch_wants_new_tab( event_like )
{
   var event = event_like || { };

   return !!( event.ctrlKey || event.metaKey || event.shiftKey || ( event.button === 1 ) );
}

// NOTE: One row of the switcher - what choosing it does ("current", "here", or "go" to the tab it is open in), the
// line under its name, and the button at its right ("new" for a new tab, "go", or none). An app open in another tab
// on this session is gone to rather than opened again, so there are never two of one.
function switcher_row( app, open_elsewhere )
{
   if( app.current )
      return { action: "current", note: "You are here", button: "", label: "" };

   if( open_elsewhere )
      return { action: "go", note: "Open in another tab", button: "go", label: "Go to the tab " + app.title + " is open in" };

   return { action: "here", note: app.note, button: "new", label: "Open " + app.title + " in a new tab" };
}

// NOTE: Which apps are open in other tabs on this session - asked on the session's channel when an app signs in and
// each time the switcher opens; every app on the same session answers with its own key and its page's id ("g_self"),
// and says so unasked when it signs in. A page leaving - another app in its tab, or the tab closed - says that too, so
// its app is no longer counted, and a console linked to it carries on by itself (found by review, 2026-10-09).
// Anything else is not an answer.
function which_apps_message( sessid )
{
   return { kind: "which_apps", sessid: String( sessid || "" ) };
}

function app_open_message( sessid, app, page )
{
   return { kind: "app_open", sessid: String( sessid || "" ), app: String( app || "" ), page: String( page || "" ) };
}

function page_left_message( sessid, page )
{
   return { kind: "page_left", sessid: String( sessid || "" ), page: String( page || "" ) };
}

function is_which_apps_here( data, sessid )
{
   return !!data && ( typeof data === "object" ) && ( data.kind === "which_apps" )
    && ( typeof data.sessid === "string" ) && ( data.sessid !== "" ) && ( data.sessid === sessid );
}

// NOTE: { app, page } for an app open on this session, or null.
function app_open_here( data, sessid )
{
   if( !data || ( typeof data !== "object" ) || ( data.kind !== "app_open" ) || ( typeof data.app !== "string" ) )
      return null;

   if( ( typeof data.sessid !== "string" ) || ( data.sessid === "" ) || ( data.sessid !== sessid ) )
      return null;

   if( !/^[a-z]+$/.test( data.app ) || ( typeof data.page !== "string" ) || !/^\d+$/.test( data.page ) )
      return null;

   return { app: data.app, page: data.page };
}

// NOTE: The id of a page on this session that has left, or "".
function page_left_here( data, sessid )
{
   if( !data || ( typeof data !== "object" ) || ( data.kind !== "page_left" ) || ( typeof data.page !== "string" ) )
      return "";

   if( ( typeof data.sessid !== "string" ) || ( data.sessid === "" ) || ( data.sessid !== sessid ) )
      return "";

   return /^\d+$/.test( data.page ) ? data.page : "";
}

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
// NOTE: A PIN is not shown once its account is made - it is meant to be known only by its user and the administrator, and
// on screen anyone looking over a shoulder could read it (Ian, 2026-10-10). Each PIN saved on this browser keeps its
// account's name beside it ("cws.name_<pin>"), and the sign in's list shows that in place of the PIN.
const c_saved_name_prefix = "cws.name_";

// NOTE: The name an account goes by on screen - never its PIN. Admin has no username of its own.
function account_display_name( username, is_admin )
{
   var name = String( username || "" ).trim( );

   if( name !== "" )
      return name;

   return is_admin ? "admin" : "Your account";
}

// NOTE: A saved account in the sign in's list - its name, or for one saved before names were kept, its place in the list
// ("Saved account 2") until it next signs in.
function saved_account_label( name, n )
{
   var text = String( name || "" ).trim( );

   return ( text !== "" ) ? text : "Saved account " + ( n + 1 );
}

// NOTE: The saved accounts as the sign in lists them - by name, A to Z whatever the case, then any saved before names were
// kept, in the order saved (Ian, 2026-10-11: "the ordering should probably be alphabetic"). Takes { pin, name } and gives
// each its "label" too.
function order_saved_accounts( accounts )
{
   var named = ( accounts || [ ] ).filter( function( account ) { return String( account.name || "" ).trim( ) !== ""; } );
   var unnamed = ( accounts || [ ] ).filter( function( account ) { return String( account.name || "" ).trim( ) === ""; } );

   named.sort( function( a, b )
   {
      var x = a.name.trim( ).toLowerCase( );
      var y = b.name.trim( ).toLowerCase( );

      if( x !== y )
         return ( x < y ) ? -1 : 1;

      return ( a.pin < b.pin ) ? -1 : ( ( a.pin > b.pin ) ? 1 : 0 );
   } );

   return named.map( function( account ) { return { pin: account.pin, label: account.name.trim( ) }; } )
    .concat( unnamed.map( function( account, n ) { return { pin: account.pin, label: saved_account_label( "", n ) }; } ) );
}

// NOTE: Which saved account the sign in starts on - the one this tab was last signed in as, when it is saved, so signing
// out and back in needs no choosing (Ian, 2026-10-11); otherwise the first.
function chosen_saved_index( ordered, last_pin )
{
   for( var i = 0; i < ( ordered || [ ] ).length; i++ )
   {
      if( ordered[ i ].pin === last_pin )
         return i;
   }

   return 0;
}

// NOTE: The stored keys of names whose PIN is no longer saved - left when a PIN is forgotten elsewhere (the console's
// "creds", a recovery), so the sign in lets them go as it lists the rest.
function stale_saved_names( keys, stored )
{
   var saved = parse_access_list( stored );

   return ( keys || [ ] ).filter( function( key )
   {
      return ( String( key ).indexOf( c_saved_name_prefix ) === 0 ) && ( saved.indexOf( key.substr( c_saved_name_prefix.length ) ) < 0 );
   } );
}

// NOTE: What is kept for one account on this browser whether or not it is remembered - its recent emoji, the
// announcements it dismissed - is keyed by the account's name, not its PIN, so "Remember: Nothing" leaves no PIN behind
// (Damon, 2026-10-11). The "@" sets these keys apart from the ones kept by PIN before.
const c_storage_dismissed_by_name = "cws.dismissed@";
const c_storage_emoji_recent_by_name = "cws.emoji_recent@";

// NOTE: The rooms whose pinned strip this account shut, on this browser - open is the default (Ian, 2026-10-14: they
// should stand out - a to-do list, room guidelines). A list of room numbers, read by "parse_dismissed( )".
const c_storage_pinned_shut_by_name = "cws.pinned_shut@";

function with_pinned_shut( rooms, room, shut )
{
   var others = ( rooms || [ ] ).filter( function( entry ) { return entry !== room; } );

   return shut ? others.concat( [ room ] ) : others;
}

function account_storage_key( prefix, username, is_admin )
{
   return prefix + account_display_name( username, is_admin );
}

// NOTE: Desktop notifications (Damon, 2026-10-20) - from the chat only, while it is open but not in front; who and where,
// never what was said; switched in the chat's account menu and on My account ("notify.js"). Kept per account on this
// browser: "on", "off", or nothing - not asked yet.
const c_storage_notify_by_name = "cws.notify@";

// NOTE: A room as a notification names it - a conversation by its other people, any other room by its name.
function notify_room_title( name, me )
{
   return is_dm_name( name ) ? dm_title( name, me ) : String( name || "" );
}

// NOTE: What has come since the last look at the room list - "before" what was seen then (null on the first look, which
// notifies of nothing), the open room left out (its own messages say who - "messages_notification( )"). A room whose unread
// count went up, and an invitation not there before - a request for a conversation, or to a room. Returns the
// notifications and what is seen now, for next time. "invitations_known" - whether they have been read yet: they are read after
// the first room list, so until then an empty list is not news, and the first one read is what was waiting, not new (found
// testing - every invitation waiting notified on signing in).
// "open_hidden" - the tab is hidden, so the open room is not being read and is told of too: by how far its total has gone up,
// as its unread count need not move while it is the open room (found testing).
function notifications_due( before, rooms, invitations, open_room, me, invitations_known, open_hidden )
{
   var seen = { unread: { }, total: { }, invited: { }, invites_known: !!invitations_known };
   var notes = [ ];

   ( rooms || [ ] ).forEach( function( entry )
   {
      seen.unread[ entry.room ] = entry.unread || 0;
      seen.total[ entry.room ] = entry.total || 0;
   } );

   ( invitations || [ ] ).forEach( function( invite ) { seen.invited[ invite.room ] = true; } );

   if( !before )
      return { notes: notes, seen: seen };

   ( rooms || [ ] ).forEach( function( entry )
   {
      var now = entry.unread || 0;

      if( entry.room === open_room )
      {
         var was = ( before.total && ( before.total[ entry.room ] !== undefined ) ) ? before.total[ entry.room ] : ( entry.total || 0 );
         var more = ( entry.total || 0 ) - was;

         if( open_hidden && ( more > 0 ) )
            notes.push( { room: entry.room, tag: "room-" + entry.room, title: notify_room_title( entry.name, me ),
             body: ( more === 1 ) ? "New message" : more + " new messages" } );

         return;
      }

      if( now <= ( before.unread[ entry.room ] || 0 ) )
         return;

      notes.push( { room: entry.room, tag: "room-" + entry.room, title: notify_room_title( entry.name, me ),
       body: ( now === 1 ) ? "New message" : now + " new messages" } );
   } );

   ( invitations || [ ] ).forEach( function( invite )
   {
      if( !before.invites_known || !invitations_known || before.invited[ invite.room ] )
         return;

      var conversation = is_dm_name( invite.name );

      notes.push( { room: invite.room, invite: true, tag: "invite-" + invite.room,
       title: conversation ? dm_title( invite.name, me ) : String( invite.name || invite.room ),
       body: conversation ? "Wants to start a conversation" : "Invitation from " + invite.inviter } );
   } );

   return { notes: notes, seen: seen };
}

// NOTE: New messages in the open room, while the chat is not in front - from others, not system lines; who, never what. A
// read brings the room's pinned messages every time, so they are left out, as is anything not after "after", where the read
// began (found by review - a pinned message from someone else notified on every poll).
function messages_notification( messages, me, room, room_name, after )
{
   var from_others = ( messages || [ ] ).filter( function( message )
   {
      return message && ( message.kind === c_kind_chat ) && ( message.sender !== me ) && !message.deleted && !message.pinned
       && ( !after || ( String( message.unique || "" ) > after ) );
   } );

   if( from_others.length === 0 )
      return null;

   var senders = [ ];

   from_others.forEach( function( message ) { if( senders.indexOf( message.sender ) < 0 ) senders.push( message.sender ); } );

   return { room: room, tag: "room-" + room, title: notify_room_title( room_name, me ),
    body: ( from_others.length === 1 ) ? "New message from " + senders[ 0 ] : from_others.length + " new messages from " + senders.join( ", " ) };
}

// NOTE: The switch's line - "setting" as kept, "state" what the browser allows ("notify_state( )" in "notify.js").
function notify_status_text( setting, state )
{
   if( state === "insecure" )
      return "Not available here - notifications need the node's https address.";

   if( state === "unsupported" )
      return "This browser does not offer notifications.";

   if( state === "blocked" )
      return "Blocked for this site - allow notifications in the browser's site settings, then turn them on here.";

   // NOTE: The browser's question closed without an answer - not refused, so asking again will work (found by review).
   if( state === "dismissed" )
      return "Not turned on - the browser's question was closed without an answer. Turn them on again to be asked.";

   return ( setting === "on" ) ? "On - who and where, never what was said." : "Off";
}

// NOTE: The switch's word in the chat's account menu.
function notify_menu_word( on, state )
{
   return ( state === "ok" ) ? ( on ? "On" : "Off" ) : "Unavailable";
}

// NOTE: Whether the chat offers them, once - not asked yet, and the browser can give them.
function notify_offer_bar( setting, state )
{
   return ( setting === "" ) && ( state === "ok" );
}

// NOTE: The keys kept by PIN before then - let go by the sign in, as it lists the saved accounts.
function pin_keyed_leftovers( keys )
{
   return ( keys || [ ] ).filter( function( key ) { return /^cws\.(dismissed|emoji_recent)_[0-9]{5}$/.test( String( key ) ); } );
}

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

// ====================================================================
// Formatting - colour and style in messages (Ian, 2026-10-15; "Message Formatting Design")
// ====================================================================

// NOTE: Ian's shorthand, from his console IRC's "ansi.cin": "%R%" and the like typed in a message, each a change to the
// style in force, and "%N%" ending everything. A letter a text colour, "L" and a letter lighter, "O" and a letter a
// background, "B" bold - with a letter, bold in that colour. The letters in ANSI order - 30 is "K", 31 "R" and so on.
const c_format_colours = [ "K", "R", "G", "O", "A", "P", "C", "W" ];

const c_format_codes = ( function( )
{
   var codes = { B: { b: true }, I: { i: true }, S: { s: true }, U: { u: true }, BI: { b: true, i: true }, N: { reset: true } };

   c_format_colours.forEach( function( c )
   {
      codes[ c ] = { fg: c };
      codes[ "L" + c ] = { fg: "L" + c };
      codes[ "O" + c ] = { bg: "O" + c };
      codes[ "B" + c ] = { b: true, fg: c };
   } );

   return codes;
} )( );

const c_format_keys = [ "b", "i", "s", "u", "fg", "bg" ];

const c_format_escape = "\u001b";

function format_token_pattern( )
{
   return /%([A-Z]{1,2})%/g;
}

// NOTE: The escapes for one of Ian's codes, as "ansi.cin" writes them - "BR" is bold, then red.
function format_code_escapes( code )
{
   var change = c_format_codes[ code ];
   var csi = c_format_escape + "[";

   if( !change )
      return "";

   if( change.reset )
      return csi + "0m";

   var out = "";

   if( change.b )
      out += csi + "1m";

   if( change.i )
      out += csi + "3m";

   if( change.s )
      out += csi + "9m";

   if( change.u )
      out += csi + "4m";

   if( change.fg )
      out += csi + ( ( ( change.fg.length === 2 ) ? 90 : 30 ) + c_format_colours.indexOf( change.fg.slice( -1 ) ) ) + "m";

   if( change.bg )
      out += csi + ( 40 + c_format_colours.indexOf( change.bg.slice( -1 ) ) ) + "m";

   return out;
}

// NOTE: What is typed, as it is sent - each of Ian's codes as its escapes; anything else between "%" left as typed.
function shorthand_to_ansi( text )
{
   return String( text || "" ).replace( format_token_pattern( ), function( whole, code )
   {
      return c_format_codes[ code ] ? format_code_escapes( code ) : whole;
   } );
}

// NOTE: Whether what is typed holds any of Ian's codes - the preview shows only then.
function has_shorthand_codes( text )
{
   var pattern = format_token_pattern( );
   var m;

   while( ( m = pattern.exec( String( text || "" ) ) ) !== null )
   {
      if( c_format_codes[ m[ 1 ] ] )
         return true;
   }

   return false;
}

// NOTE: One SGR sequence's numbers applied to a style - a new style, the old one left alone. Only what can be drawn: bold,
// italics, underline, strikethrough, the 16 colours and 8 backgrounds, and their ends. 256 colours and true colour are
// passed over, numbers and all; blink, reverse and the like are not drawn. A number can carry its own details after ":" -
// "4:3" a curly underline, "38:2::1:2:3" a colour - and they stay with it (found by review).
function format_apply_sgr( style, params )
{
   var next = Object.assign( { }, style );
   var list = ( params === "" ) ? [ "0" ] : params.split( ";" );

   for( var k = 0; k < list.length; k++ )
   {
      var parts = list[ k ].split( ":" );
      var n = ( parts[ 0 ] === "" ) ? 0 : parseInt( parts[ 0 ], 10 );

      if( n === 0 )
         next = { };
      else if( n === 1 )
         next.b = true;
      else if( n === 3 )
         next.i = true;
      else if( ( n === 4 ) && ( parts[ 1 ] === "0" ) )
         delete next.u;
      else if( n === 4 )
         next.u = true;
      else if( n === 9 )
         next.s = true;
      else if( n === 22 )
         delete next.b;
      else if( n === 23 )
         delete next.i;
      else if( n === 24 )
         delete next.u;
      else if( n === 29 )
         delete next.s;
      else if( ( n >= 30 ) && ( n <= 37 ) )
         next.fg = c_format_colours[ n - 30 ];
      else if( ( n >= 90 ) && ( n <= 97 ) )
         next.fg = "L" + c_format_colours[ n - 90 ];
      else if( n === 39 )
         delete next.fg;
      else if( ( n >= 40 ) && ( n <= 47 ) )
         next.bg = "O" + c_format_colours[ n - 40 ];
      else if( ( n >= 100 ) && ( n <= 107 ) )
         next.bg = "O" + c_format_colours[ n - 100 ];
      else if( n === 49 )
         delete next.bg;
      else if( ( ( n === 38 ) || ( n === 48 ) || ( n === 58 ) ) && ( parts.length === 1 ) )
         k += ( list[ k + 1 ] === "5" ) ? 2 : ( ( list[ k + 1 ] === "2" ) ? 4 : 0 );
   }

   return next;
}

// NOTE: A message as it is stored - with real escapes, from Ian's console or from here - as characters, each with its style.
// Only the colour and style sequences count; **every other escape is dropped, never drawn** - moving the cursor, clearing
// the screen, a window's title, a link - and so is every other control character but a line break and a tab, and the
// characters that turn text to run the other way (found by review).
function format_ansi_chars( text )
{
   var value = String( text || "" );
   var chars = [ ];
   var style = { };
   var i = 0;

   while( i < value.length )
   {
      var ch = value[ i ];

      if( ch === c_format_escape )
      {
         var next = value[ i + 1 ];

         if( next === "[" )
         {
            var csi = /^\u001b\[([0-9;:?<=>]*)([ -\/]*)([@-~])/.exec( value.slice( i ) );

            if( !csi )
            {
               i += 2;

               continue;
            }

            // NOTE: Only a plain SGR is a style - none with a private marker ("?", "<", "=", ">") or an intermediate.
            if( ( csi[ 3 ] === "m" ) && ( csi[ 2 ] === "" ) && /^[0-9;:]*$/.test( csi[ 1 ] ) )
               style = format_apply_sgr( style, csi[ 1 ] );

            i += csi[ 0 ].length;

            continue;
         }

         // NOTE: The strings - OSC (a title, a link), DCS, SOS, PM and APC - up to their end, ST, or a bell for an OSC. One
         // never ended drops only its opening, so it cannot hide the rest of the message.
         if( ( next !== undefined ) && ( "]PX^_".indexOf( next ) >= 0 ) )
         {
            var st = value.indexOf( c_format_escape + "\\", i + 2 );
            var bell = ( next === "]" ) ? value.indexOf( "\u0007", i + 2 ) : -1;

            if( ( bell >= 0 ) && ( ( st < 0 ) || ( bell < st ) ) )
               i = bell + 1;
            else if( st >= 0 )
               i = st + 2;
            else
               i += 2;

            continue;
         }

         // NOTE: Any other escape - its intermediates (" " to "/") and its final character ("0" to "~"), as "ESC 7" or
         // "ESC ( B"; an escape before anything else is dropped alone.
         var other = /^\u001b[ -\/]*[0-~]/.exec( value.slice( i, i + 16 ) );

         i += other ? other[ 0 ].length : 1;

         continue;
      }

      var code = ch.charCodeAt( 0 );

      if( ( ( code < 32 ) && ( ch !== "\n" ) && ( ch !== "\t" ) ) || ( code === 127 ) || ( ( code >= 0x80 ) && ( code <= 0x9f ) )
       || ( ( code >= 0x202a ) && ( code <= 0x202e ) ) || ( ( code >= 0x2066 ) && ( code <= 0x2069 ) ) )
      {
         ++i;

         continue;
      }

      chars.push( { ch: ch, style: style } );

      ++i;
   }

   return chars;
}

// NOTE: What is typed - Ian's shorthand - as characters, each with its style and where it sits in the text. A "%..%" that is
// not one of his codes stays as text.
function format_shorthand_chars( text )
{
   var value = String( text || "" );
   var chars = [ ];
   var style = { };
   var at = 0;
   var pattern = format_token_pattern( );
   var m;

   function take( from, to )
   {
      for( var k = from; k < to; k++ )
         chars.push( { ch: value[ k ], raw: k, style: style } );
   }

   while( ( m = pattern.exec( value ) ) !== null )
   {
      var change = c_format_codes[ m[ 1 ] ];

      if( !change )
         continue;

      take( at, m.index );

      if( change.reset )
         style = { };
      else
      {
         style = Object.assign( { }, style, change );

         delete style.reset;
      }

      at = pattern.lastIndex;
   }

   take( at, value.length );

   return chars;
}

function format_same_style( a, b )
{
   return c_format_keys.every( function( k ) { return ( a[ k ] || "" ) === ( b[ k ] || "" ); } );
}

// NOTE: Runs of characters with one style - what is drawn, a span each. A style is { b, i, s, u, fg, bg }, only what is on.
function format_pieces_of( chars )
{
   var pieces = [ ];

   chars.forEach( function( c )
   {
      var last = pieces[ pieces.length - 1 ];

      if( last && format_same_style( last.style, c.style ) )
         last.text += c.ch;
      else
         pieces.push( { text: c.ch, style: c.style } );
   } );

   return pieces.map( function( piece )
   {
      var style = { };

      c_format_keys.forEach( function( k ) { if( piece.style[ k ] ) style[ k ] = piece.style[ k ]; } );

      return { text: piece.text, style: style };
   } );
}

// NOTE: A stored message, to draw.
function format_pieces( text )
{
   return format_pieces_of( format_ansi_chars( text ) );
}

// NOTE: What is typed, to preview.
function format_shorthand_pieces( text )
{
   return format_pieces_of( format_shorthand_chars( text ) );
}

// NOTE: A stored message's words alone - for the places that show plain text: the pinned line, announcements, Home.
function strip_format( text )
{
   return format_ansi_chars( text ).map( function( c ) { return c.ch; } ).join( "" );
}

// NOTE: Whether going from one style to another only adds - nothing stops, so no "%N%" is needed.
function format_only_adds( from, to )
{
   return c_format_keys.every( function( k ) { return !from[ k ] || ( from[ k ] === to[ k ] ); } );
}

// NOTE: The codes that take "base" to "style", with Ian's combined codes where they fit - bold in a colour, bold italics.
function format_codes_for( style, base )
{
   var want = { };
   var out = [ ];

   c_format_keys.forEach( function( k ) { if( style[ k ] && ( style[ k ] !== base[ k ] ) ) want[ k ] = style[ k ]; } );

   if( want.b && want.fg && ( want.fg.length === 1 ) )
   {
      out.push( "B" + want.fg );

      delete want.b;
      delete want.fg;
   }
   else if( want.b && want.i )
   {
      out.push( "BI" );

      delete want.b;
      delete want.i;
   }

   [ [ "b", "B" ], [ "fg", null ], [ "i", "I" ], [ "s", "S" ], [ "u", "U" ], [ "bg", null ] ].forEach( function( pair )
   {
      if( want[ pair[ 0 ] ] )
         out.push( pair[ 1 ] || want[ pair[ 0 ] ] );
   } );

   return out.map( function( code ) { return "%" + code + "%"; } ).join( "" );
}

// NOTE: Characters written out as Ian's shorthand - codes only where the style changes, and where anything stops "%N%" and
// the codes still wanted (his one off code - Ian, 2026-10-15); "%N%" at the end if anything is still on. "raw" is where each
// character landed - for keeping the same words selected.
function format_serialise( chars )
{
   var text = "";
   var current = { };
   var raw = [ ];

   chars.forEach( function( c )
   {
      if( !format_same_style( current, c.style ) )
      {
         text += format_only_adds( current, c.style ) ? format_codes_for( c.style, current ) : "%N%" + format_codes_for( c.style, { } );

         current = c.style;
      }

      raw.push( text.length );

      text += c.ch;
   } );

   if( c_format_keys.some( function( k ) { return current[ k ]; } ) )
      text += "%N%";

   return { text: text, raw: raw };
}

// NOTE: A stored message as Ian's shorthand, to edit - the box never holds an escape.
function ansi_to_shorthand( text )
{
   return format_serialise( format_ansi_chars( text ) ).text;
}

// NOTE: A format button on the picked characters - each given a style of its own. A style every one has already comes off.
function format_restyle_picked( picked, change )
{
   if( change.clear )
      picked.forEach( function( c ) { c.style = { }; } );
   else
   {
      var key = change.flag || ( change.fg ? "fg" : "bg" );
      var value = change.flag ? true : ( change.fg || change.bg );
      var all = picked.every( function( c ) { return c.style[ key ] === value; } );

      picked.forEach( function( c )
      {
         c.style = Object.assign( { }, c.style );

         if( all )
            delete c.style[ key ];
         else
            c.style[ key ] = value;
      } );
   }
}

// NOTE: Each style every picked character has; for a colour, the one they all share.
function format_shared_style( picked )
{
   var shared = { };

   if( picked.length === 0 )
      return shared;

   c_format_keys.forEach( function( k )
   {
      var first = picked[ 0 ].style[ k ];

      if( first && picked.every( function( c ) { return c.style[ k ] === first; } ) )
         shared[ k ] = first;
   } );

   return shared;
}

function format_plain_index( chars, raw )
{
   var n = 0;

   while( ( n < chars.length ) && ( chars[ n ].raw < raw ) )
      ++n;

   return n;
}

// NOTE: A format button on the selected words ("start" to "end" in what is typed) - "change" is { flag: "b" }, { fg: "R" },
// { bg: "OR" } or { clear: true }. A style every selected character has already comes off. The whole text is written out
// again, the same words selected; null when no words are selected, or the codes would not read back as meant.
function format_restyle( text, start, end, change )
{
   var chars = format_shorthand_chars( text );
   var from = format_plain_index( chars, start );
   var to = format_plain_index( chars, end );

   if( from >= to )
      return null;

   format_restyle_picked( chars.slice( from, to ), change );

   var out = format_serialise( chars );

   // NOTE: Typed text ending in "%" and capitals can join a code written after it into one of Ian's - "x%R" then "%B%" reads
   // "%R%B%" - and the shorthand has no way round that, so the bar does nothing rather than change the message (found by
   // review).
   var reread = format_shorthand_chars( out.text );

   var changed = ( reread.length !== chars.length ) || reread.some( function( c, k )
   {
      return ( c.ch !== chars[ k ].ch ) || !format_same_style( c.style, chars[ k ].style );
   } );

   if( changed )
      return null;

   return { text: out.text, start: out.raw[ from ], end: out.raw[ to - 1 ] + 1 };
}

// NOTE: Which buttons show pressed - each style every selected character has; for a colour, the one they all share.
function format_selection_style( text, start, end )
{
   var chars = format_shorthand_chars( text );

   return format_shared_style( chars.slice( format_plain_index( chars, start ), format_plain_index( chars, end ) ) );
}

// NOTE: The styled message box (Ian, 2026-10-18) - with Codes off the box holds the words alone, and their styles are kept
// beside them, a character each ({ ch, style }, as "format_ansi_chars( )" gives). These go between that and what is sent,
// what Codes shows, and what a change to the box does to it. A place in the box is a place in these characters.

function format_styled_of( chars )
{
   return chars.map( function( c ) { return { ch: c.ch, style: c.style }; } );
}

// NOTE: A stored message - one being edited - as the box's words and styles.
function format_styled_from_ansi( text )
{
   return format_styled_of( format_ansi_chars( text ) );
}

// NOTE: What was typed with Codes on, as the box's words and styles once it is turned off.
function format_styled_from_shorthand( text )
{
   return format_styled_of( format_shorthand_chars( text ) );
}

function format_styled_text( chars )
{
   return chars.map( function( c ) { return c.ch; } ).join( "" );
}

// NOTE: As "trim( )" does the words - the spaces and line breaks at either end dropped, with their styles.
function format_styled_trim( chars )
{
   var from = 0;
   var to = chars.length;

   while( ( from < to ) && /\s/.test( chars[ from ].ch ) )
      ++from;

   while( ( to > from ) && /\s/.test( chars[ to - 1 ].ch ) )
      --to;

   return chars.slice( from, to );
}

// NOTE: As it is sent - the escapes written from the styles, with the same codes and the same one off code as Ian's shorthand
// ("format_serialise( )"). Never through the shorthand, so words typed with a "%" stay words: a typed "%R%" is not red.
function format_styled_to_ansi( chars )
{
   var text = "";
   var current = { };

   chars.forEach( function( c )
   {
      if( !format_same_style( current, c.style ) )
      {
         text += shorthand_to_ansi( format_only_adds( current, c.style )
          ? format_codes_for( c.style, current ) : "%N%" + format_codes_for( c.style, { } ) );

         current = c.style;
      }

      text += c.ch;
   } );

   if( c_format_keys.some( function( k ) { return current[ k ]; } ) )
      text += shorthand_to_ansi( "%N%" );

   return text;
}

// NOTE: What Codes shows - Ian's shorthand. A "%R%" typed as words becomes his code here, as it always has (his shorthand has
// no way to write one as words).
function format_styled_to_shorthand( chars )
{
   return format_serialise( chars ).text;
}

// NOTE: The box's words changed - typed, deleted, pasted, an emoji put in - and the styles follow. What is the same before and
// after keeps its style; what came in between takes the style of the character before it, as a word processor does. "caret",
// where the cursor is after the change, says where the change ended - without it, deleting the first of two like characters
// took the second's style away (found by review). Neither end falls inside an emoji's pair of characters.
function format_styled_edit( chars, text, caret )
{
   var old = format_styled_text( chars );
   var now = String( text || "" );
   var max = Math.min( old.length, now.length );
   var same_before = 0;
   var same_after = 0;

   var after_caret = ( typeof caret === "number" ) ? now.length - caret : -1;

   if( ( after_caret >= 0 ) && ( after_caret <= old.length ) && ( old.slice( old.length - after_caret ) === now.slice( caret ) ) )
   {
      same_after = after_caret;

      while( ( same_before < max - same_after ) && ( old[ same_before ] === now[ same_before ] ) )
         ++same_before;
   }
   else
   {
      while( ( same_before < max ) && ( old[ same_before ] === now[ same_before ] ) )
         ++same_before;

      while( ( same_after < max - same_before ) && ( old[ old.length - 1 - same_after ] === now[ now.length - 1 - same_after ] ) )
         ++same_after;
   }

   if( ( same_before > 0 ) && /[\ud800-\udbff]/.test( now[ same_before - 1 ] ) )
      --same_before;

   if( ( same_after > 0 ) && /[\udc00-\udfff]/.test( now[ now.length - same_after ] ) )
      --same_after;

   var style = ( same_before > 0 ) ? chars[ same_before - 1 ].style : { };
   var added = [ ];

   for( var k = same_before; k < now.length - same_after; k++ )
      added.push( { ch: now[ k ], style: style } );

   return chars.slice( 0, same_before ).concat( added, chars.slice( old.length - same_after ) );
}

// NOTE: A format button on the selected words in the styled box - "change" as for "format_restyle( )", "start" and "end"
// places in the words. New characters, the same words; null when nothing is selected.
function format_styled_restyle( chars, start, end, change )
{
   if( start >= end )
      return null;

   var out = format_styled_of( chars );

   format_restyle_picked( out.slice( start, end ), change );

   return out;
}

function format_styled_shared( chars, start, end )
{
   return format_shared_style( chars.slice( start, end ) );
}

// NOTE: Undo in the styled box (Damon, 2026-10-18) - the browser's own undo knows only the words, so a format change was not
// in it, and a deleted red word came back plain. The box keeps its own: each state the words, their styles and the selection,
// "at" the one showing. Typing joins the state before it while it carries on - within a second, next to where the last
// typing was, and not the first space or line after a word - so one undo takes back a burst, as a browser's does.
const c_format_history_join_ms = 1000;
const c_format_history_most = 200;

function format_history( chars, start, end )
{
   return { states: [ { chars: chars, start: start, end: end } ], at: 0, kind: "", time: 0 };
}

function format_styled_same( a, b )
{
   return ( a.length === b.length ) && a.every( function( c, k ) { return ( c.ch === b[ k ].ch ) && format_same_style( c.style, b[ k ].style ); } );
}

// NOTE: A change to the box - "kind" is "type" for typing, "format" for the bar, anything else for the rest (a paste, an emoji,
// a message put in to edit). Returns the history with it; a change that changes nothing only moves the selection.
function format_history_record( history, chars, start, end, kind, time )
{
   var current = history.states[ history.at ];

   if( format_styled_same( current.chars, chars ) )
      return format_history_select( history, start, end );

   // NOTE: What was typed, if anything - the first space or line typed after a word starts a state of its own.
   var added = ( start > current.start ) ? format_styled_text( chars ).slice( current.start, start ) : "";
   var after_word = ( current.start > 0 ) && !/\s/.test( current.chars[ current.start - 1 ] ? current.chars[ current.start - 1 ].ch : " " );

   var joins = ( kind === "type" ) && ( history.kind === "type" ) && ( time - history.time < c_format_history_join_ms )
    && ( history.at === history.states.length - 1 ) && ( Math.abs( start - current.start ) <= 2 ) && !( /\s/.test( added ) && after_word );

   var states = history.states.slice( 0, history.at + ( joins ? 0 : 1 ) );

   states.push( { chars: chars, start: start, end: end } );

   if( states.length > c_format_history_most )
      states = states.slice( states.length - c_format_history_most );

   return { states: states, at: states.length - 1, kind: kind, time: time };
}

// NOTE: The selection moved, nothing else - kept, so an undo puts the cursor back where the typing began.
function format_history_select( history, start, end )
{
   var states = history.states.slice( );

   states[ history.at ] = { chars: states[ history.at ].chars, start: start, end: end };

   return { states: states, at: history.at, kind: history.kind, time: history.time };
}

// NOTE: One back, or one forward - the history and the state to show; null when there is none.
function format_history_undo( history )
{
   if( history.at === 0 )
      return null;

   return { history: { states: history.states, at: history.at - 1, kind: "", time: 0 }, state: history.states[ history.at - 1 ] };
}

function format_history_redo( history )
{
   if( history.at >= history.states.length - 1 )
      return null;

   return { history: { states: history.states, at: history.at + 1, kind: "", time: 0 }, state: history.states[ history.at + 1 ] };
}

// NOTE: Exported for Node.js; in the browser these are plain globals.
if( typeof module !== "undefined" )
{
   module.exports = {
      c_format_colours: c_format_colours,
      c_format_codes: c_format_codes,
      shorthand_to_ansi: shorthand_to_ansi,
      has_shorthand_codes: has_shorthand_codes,
      format_pieces: format_pieces,
      format_shorthand_pieces: format_shorthand_pieces,
      strip_format: strip_format,
      ansi_to_shorthand: ansi_to_shorthand,
      format_restyle: format_restyle,
      format_selection_style: format_selection_style,
      format_styled_from_ansi: format_styled_from_ansi,
      format_styled_from_shorthand: format_styled_from_shorthand,
      format_styled_text: format_styled_text,
      format_styled_trim: format_styled_trim,
      format_styled_to_ansi: format_styled_to_ansi,
      format_styled_to_shorthand: format_styled_to_shorthand,
      format_styled_edit: format_styled_edit,
      format_styled_restyle: format_styled_restyle,
      format_styled_shared: format_styled_shared,
      format_history: format_history,
      format_history_record: format_history_record,
      format_history_select: format_history_select,
      format_history_undo: format_history_undo,
      format_history_redo: format_history_redo,
      switcher_apps: switcher_apps,
      switcher_title: switcher_title,
      signed_out_message: signed_out_message,
      is_signed_out_here: is_signed_out_here,
      format_tab_session: format_tab_session,
      parse_tab_session: parse_tab_session,
      switch_wants_new_tab: switch_wants_new_tab,
      switcher_row: switcher_row,
      which_apps_message: which_apps_message,
      app_open_message: app_open_message,
      is_which_apps_here: is_which_apps_here,
      app_open_here: app_open_here,
      page_left_message: page_left_message,
      page_left_here: page_left_here,
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
      is_reserved_room: is_reserved_room,
      posting_status: posting_status,
      invited_to_room: invited_to_room,
      pending_invitations: pending_invitations,
      escape_message_text: escape_message_text,
      message_prefix: message_prefix,
      message_unreadable_label: message_unreadable_label,
      message_plain_text: message_plain_text,
      is_unencrypted_message: is_unencrypted_message,
      c_reaction_emojis: c_reaction_emojis,
      c_reaction_names: c_reaction_names,
      parse_reactions: parse_reactions,
      is_irc_lock_refusal: is_irc_lock_refusal,
      message_state: message_state,
      reaction_chips: reaction_chips,
      apply_reaction: apply_reaction,
      reaction_options: reaction_options,
      can_react: can_react,
      keeps_focus_after_send: keeps_focus_after_send,
      message_bytes: message_bytes,
      c_max_message_bytes: c_max_message_bytes,
      unescape_message_text: unescape_message_text,
      describe_event: describe_event,
      password_strength: password_strength,
      sign_in_progress: sign_in_progress,
      password_strength_by_guesses: password_strength_by_guesses,
      suggested_passphrase: suggested_passphrase,
      password_refusal: password_refusal,
      user_initial: user_initial,
      sign_in_error_text: sign_in_error_text,
      is_blocked_device_error: is_blocked_device_error,
      irc_unavailable: irc_unavailable,
      is_locked_sign_in_error: is_locked_sign_in_error,
      recovery_words: recovery_words,
      recovery_words_problem: recovery_words_problem,
      word_completions: word_completions,
      recovery_error_text: recovery_error_text,
      recovery_error_step: recovery_error_step,
      in_time_order: in_time_order,
      last_editable: last_editable,
      later_start_point: later_start_point,
      pin_options: pin_options,
      delete_options: delete_options,
      can_pin: can_pin,
      pin_error_text: pin_error_text,
      pinned_signature: pinned_signature,
      recovery_texts: recovery_texts,
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
      dm_invite_outcomes: dm_invite_outcomes,
      dm_after_declines: dm_after_declines,
      dm_trusted: dm_trusted,
      dm_key: dm_key,
      dm_other_person: dm_other_person,
      dm_encryption_note: dm_encryption_note,
      group_encryption_note: group_encryption_note,
      room_key_recipients: room_key_recipients,
      room_key_sent_text: room_key_sent_text,
      sealed_too_long: sealed_too_long,
      can_have_room_key: can_have_room_key,
      room_key_text: room_key_text,
      parse_room_key: parse_room_key,
      room_key_in_message: room_key_in_message,
      room_key_masked: room_key_masked,
      picker_matches: picker_matches,
      picker_enter_choice: picker_enter_choice,
      picker_count_text: picker_count_text,
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
      continues_run: continues_run,
      notice_run: notice_run,
      seen_chips: seen_chips,
      merge_new_messages: merge_new_messages,
      needs_full_read: needs_full_read,
      posts_request_value: posts_request_value,
      can_change_posting: can_change_posting,
      can_leave_room: can_leave_room,
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
      page_title: page_title,
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
      c_saved_name_prefix: c_saved_name_prefix,
      account_display_name: account_display_name,
      saved_account_label: saved_account_label,
      stale_saved_names: stale_saved_names,
      order_saved_accounts: order_saved_accounts,
      chosen_saved_index: chosen_saved_index,
      c_storage_dismissed_by_name: c_storage_dismissed_by_name,
      c_storage_emoji_recent_by_name: c_storage_emoji_recent_by_name,
      with_pinned_shut: with_pinned_shut,
      account_storage_key: account_storage_key,
      c_storage_notify_by_name: c_storage_notify_by_name,
      notify_room_title: notify_room_title,
      notifications_due: notifications_due,
      messages_notification: messages_notification,
      notify_status_text: notify_status_text,
      notify_offer_bar: notify_offer_bar,
      notify_menu_word: notify_menu_word,
      pin_keyed_leftovers: pin_keyed_leftovers,
      is_valid_room_name: is_valid_room_name,
      is_valid_username: is_valid_username
   };
}

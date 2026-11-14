// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Calls (Ian, 2026-11-13: "the P2P style initially ... tied to a DM room") - one to one, browser to browser, in an encrypted
// direct message. The call is set up with messages in the conversation, sealed with the secret the two share: an offer, its answer,
// an end or a decline ("call_messages( )" in "chat_parse.js"). Sealed, the node can neither read them nor change the fingerprints of
// the call's own encryption (DTLS-SRTP) in them, so it cannot sit in the middle of the call either.
//
// No STUN or TURN server - nothing is asked of any host but the node (no external requests, ever). So for now a call connects only on
// the same machine or network; between networks it waits for the node's own TURN (LiveKit's - Ian, 2026-10-30). The camera and the
// microphone need a secure page: "localhost", or the node over HTTPS.

const c_call_ice_servers = [ ];

// NOTE: How often the call's conversation is read while it is being set up - the poll's four seconds would be slow to ring and answer.
const c_call_watch_ms = 1000;

// NOTE: How long a call rings before it is given up.
const c_call_ring_ms = 45000;

// NOTE: How long the set-up waits for the browser to gather its ways to be reached - all are sent at once, in the offer or answer.
const c_call_gather_ms = 5000;

// NOTE: The call there is, or null - { id, room, other, role ("caller" or "callee"), state ("starting", "calling", "ringing",
// "connecting", "in-call"), offer, pc, local, started, answered }.
var g_call = null;

// NOTE: Calls already rung, made or ended here - not rung again ("call_apply( )").
var g_call_handled = { };

// NOTE: Each direct message's "total/unread" when last looked at for calls - looked at again only once it changes.
var g_call_seen = { };

// NOTE: The other person's call messages in each direct message not opened since - { <room>: { uniques: { <unique>: true }, raw } } -
// taken off its count of what is new ("call_unread_adjust( )"): a call is not three new messages.
var g_call_unread = { };

var g_call_ring_sound = null;

var g_call_looking = false;
var g_call_watch_timer = null;
var g_call_ring_timer = null;
var g_call_clock_timer = null;

function call_new_id( )
{
   var bytes = new Uint8Array( 4 );

   crypto.getRandomValues( bytes );

   return Array.prototype.map.call( bytes, function( b ) { return ( b < 16 ? "0" : "" ) + b.toString( 16 ); } ).join( "" );
}

// NOTE: A call's description compressed, in base64 - six kilobytes of offer with video become about two (measured, 2026-11-13).
async function call_pack( text )
{
   var stream = new Blob( [ text ] ).stream( ).pipeThrough( new CompressionStream( "deflate-raw" ) );
   var bytes = new Uint8Array( await new Response( stream ).arrayBuffer( ) );
   var binary = "";

   for( var i = 0; i < bytes.length; i++ )
      binary += String.fromCharCode( bytes[ i ] );

   return btoa( binary );
}

async function call_unpack( data )
{
   var binary = atob( data );
   var bytes = new Uint8Array( binary.length );

   for( var i = 0; i < binary.length; i++ )
      bytes[ i ] = binary.charCodeAt( i );

   var stream = new Blob( [ bytes ] ).stream( ).pipeThrough( new DecompressionStream( "deflate-raw" ) );

   return await new Response( stream ).text( );
}

function call_now( )
{
   return Math.floor( Date.now( ) / 1000 );
}

// NOTE: Whether the open room can be called from - an encrypted direct message.
function call_possible_here( )
{
   var other = chat_dm_other( );

   return ( other !== "" ) && ( chat_secret_with( other ) !== "" );
}

// NOTE: The Call button, in a conversation that can be called from (from "show_encryption( )", as the room or the keys change).
function call_show_button( )
{
   var button = document.getElementById( "thread_call" );

   if( button !== null )
      button.hidden = ( g_room === "" ) || !call_possible_here( );

   call_show_open( );
}

// NOTE: The call's conversation offered to open, from anywhere else.
function call_show_open( )
{
   var open = document.getElementById( "call_open" );

   if( open !== null )
      open.hidden = ( g_call === null ) || ( g_call.room === g_room );
}

function do_open_call_room( )
{
   if( ( g_call !== null ) && ( g_call.room !== g_room ) )
      select_room( g_call.room, "" );
}

// NOTE: From each listing of the rooms - a direct message's count of what is new without the other person's call messages, known from
// looking at it for calls. Once it has been read (its count has gone down, or it is open), they are forgotten.
function call_unread_adjust( entry )
{
   var kept = g_call_unread[ entry.room ];

   // NOTE: The node's own count kept, for knowing which messages are the new ones ("call_look( )").
   entry.unread_raw = entry.unread;

   if( !kept )
      return;

   if( ( entry.room === g_room ) || ( entry.unread < kept.raw ) )
      kept.uniques = { };

   kept.raw = entry.unread;

   entry.unread = Math.max( 0, entry.unread - Object.keys( kept.uniques ).length );
}

// NOTE: Each of a call's messages sealed and sent in turn - false if one was not taken (or had no answer).
async function call_send( room, other, id, kind, data )
{
   var secret = chat_secret_with( other );

   if( secret === "" )
      return false;

   var texts = call_messages( id, call_now( ), ciyam.username, kind, data );
   var sealer = ciyam_module_named( "secure_message" );

   for( var i = 0; i < texts.length; i++ )
   {
      // NOTE: Not once a sign out has begun - a part queued then went alongside it (found by review, 2026-11-14).
      if( g_signing_out )
         return false;

      var sealed = await sealer.seal( secret, texts[ i ] );
      var taken = false;

      if( g_signing_out )
         return false;

      await serialised( function( )
      {
         return ciyam.create_message( room, "text=" + sealed, function( response )
         {
            taken = !is_error_response( response );
         } );
      } );

      if( !taken )
         return false;
   }

   return true;
}

// NOTE: A room's messages since "since" (Unix seconds), read quietly - or null if it could not be read. Read without moving the
// person's read point (Ian's "extra=PEEK", 2026-11-14): otherwise looking for a call marked the conversation read before they saw it
// (found by review, 2026-11-14).
async function call_read_recent( room, since )
{
   var response = null;

   g_in_poll = true;

   var reading = serialised( function( )
   {
      return ciyam.fetch_messages( room, "from=" + since + ";extra=PEEK", function( text ) { response = String( text ); } );
   } );

   g_in_poll = false;

   await reading;

   if( response === null )
      return null;

   var result = parse_fetch_response( response );

   if( is_timeout_response( response ) || ( result.error !== "" ) || ( result.rooms.length > 0 ) )
      return null;

   return result.messages;
}

// NOTE: Of "messages", the call messages sealed between this person and "other", opened, oldest first.
async function call_opened( messages, other )
{
   var secret = chat_secret_with( other );
   var sealer = ciyam_module_named( "secure_message" );
   var opened = [ ];

   for( var i = 0; i < messages.length; i++ )
   {
      var message = messages[ i ];

      if( ( message.kind !== c_kind_chat ) || ( message_prefix( message.text ) !== "X" )
       || ( ( message.sender !== other ) && ( message.sender !== ciyam.username ) ) )
         continue;

      var plain = null;

      try
      {
         plain = await sealer.open( secret, message.text );
      }
      catch( e )
      {
         plain = null;
      }

      if( ( plain !== null ) && is_call_message( plain ) )
         opened.push( { sender: message.sender, text: plain, unique: message.unique } );
   }

   return opened;
}

// NOTE: A direct message read for its calls, and what they say acted on - false if it could not be read. The last
// "c_call_fresh_seconds", or since the call going there began, however long ago (found by review, 2026-11-14: a hang up after 90
// seconds went unseen).
async function call_look( room, other, unread )
{
   var going = ( ( g_call !== null ) && ( g_call.room === room ) ) ? g_call : null;
   var since = call_now( ) - c_call_fresh_seconds;

   if( ( going !== null ) && ( going.at > 0 ) )
      since = Math.min( since, going.at - 5 );

   var messages = await call_read_recent( room, since );

   if( messages === null )
      return false;

   if( g_signing_out )
      return true;

   var opened = await call_opened( messages, other );

   // NOTE: Only the call messages among those still new to this person - the newest "unread" of what the other said - are taken off
   // its count (found by review, 2026-11-14: call messages already read, counted again, hid a message sent after them).
   if( ( room !== g_room ) && ( unread > 0 ) )
   {
      var kept = g_call_unread[ room ] || ( g_call_unread[ room ] = { uniques: { }, raw: 0 } );
      var said = messages.filter( function( message ) { return ( message.kind === c_kind_chat ) && ( message.sender === other ); } );
      var fresh = { };

      said.slice( -unread ).forEach( function( message ) { fresh[ message.unique ] = true; } );

      opened.forEach( function( message )
      {
         if( ( message.sender === other ) && fresh[ message.unique ] )
            kept.uniques[ message.unique ] = true;
      } );
   }

   call_apply( room, other, call_states( opened, ciyam.username, other, call_now( ), ( going !== null ) ? going.id : "" ) );

   return true;
}

// NOTE: From the poll - each encrypted direct message whose counts have changed is looked at for calls, and the call's own while one
// is going (its end may come at any time).
async function call_watch( )
{
   if( g_call_looking || g_signing_out )
      return;

   g_call_looking = true;

   try
   {
      var rooms = g_rooms.slice( );

      for( var i = 0; i < rooms.length; i++ )
      {
         var entry = rooms[ i ];
         var other = dm_trusted( entry.name, entry.owner ) ? dm_other_person( entry.name, ciyam.username ) : "";

         if( ( other === "" ) || ( chat_secret_with( other ) === "" ) )
            continue;

         var seen = entry.total + "/" + entry.unread;
         var first = !g_call_seen.hasOwnProperty( entry.room );

         if( ( g_call_seen[ entry.room ] === seen ) && !( g_call && ( g_call.room === entry.room ) ) )
            continue;

         // NOTE: Seen the first time with nothing new - no call can be ringing there (found 2026-11-14: each was read at sign in).
         if( first && ( entry.unread === 0 ) )
         {
            g_call_seen[ entry.room ] = seen;

            continue;
         }

         // NOTE: Taken as seen only once read - one that could not be read is looked at again (found by review, 2026-11-14).
         if( await call_look( entry.room, other, entry.hasOwnProperty( "unread_raw" ) ? entry.unread_raw : entry.unread ) )
            g_call_seen[ entry.room ] = seen;

         if( g_signing_out )
            return;
      }
   }
   finally
   {
      g_call_looking = false;
   }
}

// NOTE: While a call is set up, its conversation read every second.
async function call_watch_now( )
{
   if( ( g_call === null ) || g_call_looking || g_signing_out )
      return;

   g_call_looking = true;

   try
   {
      await call_look( g_call.room, g_call.other );
   }
   finally
   {
      g_call_looking = false;
   }
}

function call_watch_fast( on )
{
   if( g_call_watch_timer !== null )
   {
      window.clearInterval( g_call_watch_timer );

      g_call_watch_timer = null;
   }

   if( on )
      g_call_watch_timer = window.setInterval( call_watch_now, c_call_watch_ms );
}

function call_apply( room, other, calls )
{
   Object.keys( calls ).map( function( id ) { return calls[ id ]; } ).sort( function( a, b ) { return a.at - b.at; } ).forEach( function( call )
   {
      if( g_call && ( call.id === g_call.id ) )
      {
         if( call.declined )
            call_end_local( other + " declined the call." );
         else if( call.busy )
            call_end_local( other + " is on another call." );
         else if( call.ended )
            call_end_local( ( g_call.state === "ringing" ) ? "Missed call from " + other + "." : "The call ended." );
         else if( ( g_call.role === "caller" ) && ( call.answer !== "" ) && !g_call.answered )
            call_take_answer( call.answer );
         else if( ( g_call.role === "callee" ) && ( g_call.state === "ringing" ) && ( call.answer !== "" ) )
            call_end_local( "Answered on another device." );

         return;
      }

      // NOTE: Rung once - not if answered or ended already (on another device of this person's too); while in another call, said
      // to be busy.
      if( ( call.caller === other ) && ( call.offer !== "" ) && !call.ended && !call.declined && !call.busy && ( call.answer === "" )
       && !g_call_handled[ call.id ] )
      {
         if( g_call === null )
            call_ring( room, other, call );
         else
         {
            g_call_handled[ call.id ] = true;

            call_send( room, other, call.id, "busy", "" ).catch( function( ) { } );
         }
      }
   } );
}

// ---- Media and the connection

// NOTE: The camera and the microphone - the microphone alone if there is no camera.
async function call_media( )
{
   try
   {
      return await navigator.mediaDevices.getUserMedia( { audio: true, video: true } );
   }
   catch( e )
   {
      return await navigator.mediaDevices.getUserMedia( { audio: true } );
   }
}

function call_connection( )
{
   var pc = new RTCPeerConnection( { iceServers: c_call_ice_servers } );

   pc.addEventListener( "track", function( event )
   {
      var remote = document.getElementById( "call_remote" );

      if( event.streams && event.streams[ 0 ] && ( remote.srcObject !== event.streams[ 0 ] ) )
         remote.srcObject = event.streams[ 0 ];
   } );

   pc.addEventListener( "connectionstatechange", function( )
   {
      if( ( g_call === null ) || ( g_call.pc !== pc ) )
         return;

      if( pc.connectionState === "connected" )
         call_connected( );
      else if( pc.connectionState === "failed" )
      {
         // NOTE: One that never connected is most likely two networks, with no relay as yet; one that had, lost (found by review: the
         // other closing their page was put down to the network).
         var connected = g_call.started !== 0;

         // NOTE: Both sides see it fail - the caller alone says so, so the conversation has one line for it, not two (found by review).
         if( g_call.role === "caller" )
            call_send_quietly( "end", true );

         call_end_local( connected ? "The call was lost." : "The call couldn't connect - for now, calls only work between people on the"
          + " same network." );
      }
   } );

   return pc;
}

// NOTE: Once every way to be reached is known - sent whole, not one at a time - or after "c_call_gather_ms" with what there is.
function call_gathered( pc )
{
   return new Promise( function( done )
   {
      if( pc.iceGatheringState === "complete" )
         return done( );

      var timer = window.setTimeout( done, c_call_gather_ms );

      pc.addEventListener( "icegatheringstatechange", function( )
      {
         if( pc.iceGatheringState === "complete" )
         {
            window.clearTimeout( timer );

            done( );
         }
      } );
   } );
}

function call_add_local( call, media )
{
   call.local = media;

   media.getTracks( ).forEach( function( track ) { call.pc.addTrack( track, media ); } );

   document.getElementById( "call_local" ).srcObject = media;
   document.getElementById( "call_camera" ).hidden = ( media.getVideoTracks( ).length === 0 );
}

// ---- Making, answering and ending

async function do_start_call( )
{
   var other = chat_dm_other( );
   var room = g_room;

   if( g_call !== null )
   {
      show_alert( "You're already in a call.", "is-error" );

      return;
   }

   if( !call_possible_here( ) )
   {
      show_alert( "Calls are only in an encrypted direct message - exchange contact codes with them first.", "is-error" );

      return;
   }

   if( !window.RTCPeerConnection || !window.CompressionStream )
   {
      show_alert( "This browser can't make calls.", "is-error" );

      return;
   }

   if( !navigator.mediaDevices )
   {
      show_alert( "This page can't make calls - the camera and microphone need the node over HTTPS (or localhost).", "is-error" );

      return;
   }

   var call = g_call = { id: call_new_id( ), room: room, other: other, role: "caller", state: "starting", offer: "", pc: null, local: null,
    started: 0, answered: false, at: call_now( ), offered: false };

   g_call_handled[ call.id ] = true;

   call_show( "Calling " + other + "…" );

   try
   {
      var media = await call_media( );

      if( g_call !== call )
      {
         media.getTracks( ).forEach( function( track ) { track.stop( ); } );

         return;
      }

      call.pc = call_connection( );

      call_add_local( call, media );

      await call.pc.setLocalDescription( await call.pc.createOffer( ) );
      await call_gathered( call.pc );

      if( g_call !== call )
         return;

      var data = await call_pack( call.pc.localDescription.sdp );

      call.offered = true;

      if( !await call_send( room, other, call.id, "offer", data ) )
      {
         call_end_local( "The call couldn't be started - the node didn't take it. Try again." );

         return;
      }

      if( g_call !== call )
         return;

      call.state = "calling";

      call_show( "Calling " + other + "…" );

      call_watch_fast( true );

      g_call_ring_timer = window.setTimeout( function( )
      {
         if( ( g_call === call ) && ( call.state === "calling" ) )
         {
            call_send_quietly( "end" );

            call_end_local( other + " didn't answer." );
         }
      }, c_call_ring_ms );
   }
   catch( e )
   {
      if( g_call === call )
         call_end_local( "The call couldn't be started: " + ( e && e.message ? e.message : "the camera or microphone was refused" ) + "." );
   }
}

function call_ring( room, other, call )
{
   g_call_handled[ call.id ] = true;

   g_call = { id: call.id, room: room, other: other, role: "callee", state: "ringing", offer: call.offer, pc: null, local: null, started: 0,
    answered: false, at: call.at, offered: true };

   call_show( other + " is calling" );

   call_watch_fast( true );

   call_ring_sound( true );

   if( notify_is_on( ) && !chat_in_front( ) )
      send_notification( { title: other + " is calling", body: "Open the chat to answer.", tag: room, room: room } );

   var ringing = g_call;

   g_call_ring_timer = window.setTimeout( function( )
   {
      if( ( g_call === ringing ) && ( ringing.state === "ringing" ) )
         call_end_local( "Missed call from " + other + "." );
   }, c_call_ring_ms );
}

async function do_accept_call( )
{
   var call = g_call;

   if( ( call === null ) || ( call.state !== "ringing" ) )
      return;

   call_ring_sound( false );

   call.state = "connecting";

   call_show( "Connecting to " + call.other + "…" );

   try
   {
      var media = await call_media( );

      if( g_call !== call )
      {
         media.getTracks( ).forEach( function( track ) { track.stop( ); } );

         return;
      }

      call.pc = call_connection( );

      call_add_local( call, media );

      await call.pc.setRemoteDescription( { type: "offer", sdp: await call_unpack( call.offer ) } );
      await call.pc.setLocalDescription( await call.pc.createAnswer( ) );
      await call_gathered( call.pc );

      if( g_call !== call )
         return;

      if( !await call_send( call.room, call.other, call.id, "answer", await call_pack( call.pc.localDescription.sdp ) ) )
         call_end_local( "The call couldn't be answered - the node didn't take it." );
   }
   catch( e )
   {
      if( g_call === call )
      {
         call_send_quietly( "end", true );

         call_end_local( "The call couldn't be answered: " + ( e && e.message ? e.message : "the camera or microphone was refused" ) + "." );
      }
   }
}

async function call_take_answer( answer )
{
   var call = g_call;

   call.answered = true;
   call.state = "connecting";

   call_show( "Connecting to " + call.other + "…" );

   try
   {
      await call.pc.setRemoteDescription( { type: "answer", sdp: await call_unpack( answer ) } );
   }
   catch( e )
   {
      if( g_call === call )
      {
         call_send_quietly( "end", true );

         call_end_local( "The call couldn't connect: " + ( e && e.message ? e.message : "its answer could not be read" ) + "." );
      }
   }
}

function call_connected( )
{
   if( ( g_call === null ) || ( g_call.state === "in-call" ) )
      return;

   g_call.state = "in-call";
   g_call.started = Date.now( );

   call_watch_fast( false );

   if( g_call_ring_timer !== null )
   {
      window.clearTimeout( g_call_ring_timer );

      g_call_ring_timer = null;
   }

   call_show( "In a call with " + g_call.other );

   g_call_clock_timer = window.setInterval( call_show_time, 1000 );

   call_show_time( );
}

function do_decline_call( )
{
   if( ( g_call === null ) || ( g_call.state !== "ringing" ) )
      return;

   call_send_quietly( "decline" );

   call_end_local( "" );
}

function do_hang_up_call( )
{
   if( g_call === null )
      return;

   // NOTE: Not an end for a call whose offer never went - it was drawn as a call not answered, with no call (found by review).
   if( g_call.offered )
      call_send_quietly( ( g_call.state === "ringing" ) ? "decline" : "end" );

   call_end_local( "" );
}

// NOTE: Sent for the call as it is now, nothing waited for - the other side ends its own if it never arrives, when the connection fails.
// "failed" - an end for a call that never connected ("c_call_end_failed").
function call_send_quietly( kind, failed )
{
   if( ( g_call === null ) || g_signing_out )
      return;

   // NOTE: An end says how long the call lasted - its line in the conversation ("call_line_text( )").
   var lasted = ( ( kind === "end" ) && ( g_call.started !== 0 ) ) ? String( Math.round( ( Date.now( ) - g_call.started ) / 1000 ) ) : "";

   if( ( kind === "end" ) && ( lasted === "" ) && failed )
      lasted = c_call_end_failed;

   call_send( g_call.room, g_call.other, g_call.id, kind, lasted ).catch( function( ) { } );
}

// NOTE: The call taken down here - the connection closed, the camera and microphone let go - and "said", if anything, said.
function call_end_local( said )
{
   var call = g_call;

   g_call = null;

   call_watch_fast( false );

   call_ring_sound( false );

   call_show_open( );

   if( g_call_ring_timer !== null )
   {
      window.clearTimeout( g_call_ring_timer );

      g_call_ring_timer = null;
   }

   if( g_call_clock_timer !== null )
   {
      window.clearInterval( g_call_clock_timer );

      g_call_clock_timer = null;
   }

   if( call !== null )
   {
      if( call.pc !== null )
         call.pc.close( );

      if( call.local !== null )
         call.local.getTracks( ).forEach( function( track ) { track.stop( ); } );
   }

   document.getElementById( "call_remote" ).srcObject = null;
   document.getElementById( "call_local" ).srcObject = null;
   document.getElementById( "call_panel" ).hidden = true;

   // NOTE: The next call starts with its microphone and camera on - so do the buttons (found by review: a caller's stayed "Unmute").
   call_reset_controls( );

   if( said )
      show_alert( said, "is-info" );
}

// NOTE: Signing out - the call ended here, nothing sent (the sign out's own request goes alone); and what was looked at forgotten.
function call_sign_out( )
{
   if( g_call !== null )
      call_end_local( "" );

   g_call_handled = { };
   g_call_seen = { };
   g_call_unread = { };
}

// NOTE: Ringing - two short tones every three seconds, made here (no sound file, and nothing fetched). A browser may hold sound back
// until the page has been used; the panel and the notification still say it.
function call_ring_sound( on )
{
   if( g_call_ring_sound !== null )
   {
      window.clearInterval( g_call_ring_sound.timer );

      g_call_ring_sound.context.close( ).catch( function( ) { } );

      g_call_ring_sound = null;
   }

   if( !on || !window.AudioContext )
      return;

   var context;

   try
   {
      context = new AudioContext( );
   }
   catch( e )
   {
      return;
   }

   var ring = function( )
   {
      [ 0, 0.5 ].forEach( function( start )
      {
         var tone = context.createOscillator( );
         var level = context.createGain( );
         var at = context.currentTime + start;

         tone.frequency.value = 440;
         level.gain.setValueAtTime( 0, at );
         level.gain.linearRampToValueAtTime( 0.15, at + 0.02 );
         level.gain.setValueAtTime( 0.15, at + 0.35 );
         level.gain.linearRampToValueAtTime( 0, at + 0.4 );

         tone.connect( level );
         level.connect( context.destination );
         tone.start( at );
         tone.stop( at + 0.45 );
      } );
   };

   ring( );

   g_call_ring_sound = { context: context, timer: window.setInterval( ring, 3000 ) };
}

function do_mute_call( )
{
   if( ( g_call === null ) || ( g_call.local === null ) )
      return;

   var button = document.getElementById( "call_mute" );
   var muted = button.getAttribute( "aria-pressed" ) !== "true";

   g_call.local.getAudioTracks( ).forEach( function( track ) { track.enabled = !muted; } );

   button.setAttribute( "aria-pressed", muted ? "true" : "false" );
   button.textContent = muted ? "Unmute" : "Mute";
}

function do_camera_call( )
{
   if( ( g_call === null ) || ( g_call.local === null ) )
      return;

   var button = document.getElementById( "call_camera" );
   var off = button.getAttribute( "aria-pressed" ) !== "true";

   g_call.local.getVideoTracks( ).forEach( function( track ) { track.enabled = !off; } );

   button.setAttribute( "aria-pressed", off ? "true" : "false" );
   button.textContent = off ? "Camera on" : "Camera off";
}

// ---- The panel

function call_show( status )
{
   var panel = document.getElementById( "call_panel" );
   var state = ( g_call === null ) ? "" : g_call.state;

   panel.hidden = false;
   panel.dataset.state = state;

   document.getElementById( "call_status" ).textContent = status;

   call_show_open( );
   document.getElementById( "call_accept" ).hidden = ( state !== "ringing" );
   document.getElementById( "call_decline" ).hidden = ( state !== "ringing" );
   document.getElementById( "call_hang_up" ).hidden = ( state === "ringing" );
   document.getElementById( "call_mute" ).hidden = ( state === "ringing" ) || ( state === "starting" );

   if( state !== "in-call" )
      document.getElementById( "call_time" ).textContent = "";

   if( state === "ringing" )
      call_reset_controls( );
}

function call_reset_controls( )
{
   var mute = document.getElementById( "call_mute" );
   var camera = document.getElementById( "call_camera" );

   mute.setAttribute( "aria-pressed", "false" );
   mute.textContent = "Mute";
   camera.setAttribute( "aria-pressed", "false" );
   camera.textContent = "Camera off";
   camera.hidden = true;
}

function call_show_time( )
{
   if( ( g_call === null ) || ( g_call.started === 0 ) )
      return;

   document.getElementById( "call_time" ).textContent = call_elapsed_text( ( Date.now( ) - g_call.started ) / 1000 );
}

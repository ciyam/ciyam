// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Encrypted direct messages in the chat (2026-10-25), as the console has them (Ian, 2026-10-24: in a direct message whose
// other person's public key it has, "irc.cin" encrypts every message, and decrypts theirs). The keys are those set up on My
// account ("account_keys.js"): kept on the node with others' public keys, locked under the access key this browser keeps - a
// hash of the twelve words, never the words. Read once a sign in. In a direct message the server named ("dm_other_person( )") with a key for the other person,
// a message is sealed ("ciyam_secure_message.js") under the secret between the two - the same from either side - before it is
// sent, and theirs opened as they are drawn. Elsewhere nothing changes.
const c_chat_keys_scripts = [ "ed25519.min.js", "ciyam_chacha20.js", "ciyam_ed25519.js", "ciyam_secure_message.js", "ciyam_user_keys.js" ];

// NOTE: null while being read (or signed out), false when this browser has no keys for the account, else what they are.
var g_chat_keys = null;
var g_chat_keys_unread = false;
var g_chat_keys_scripts = null;
var g_chat_keys_read = 0;

// NOTE: What has been opened, by its sealed text - a message is drawn again on every change to its row.
var g_chat_opened = { };
var g_chat_secrets = { };

function chat_keys_load_script( src )
{
   return new Promise( function( resolve )
   {
      var script = document.createElement( "script" );

      script.src = src;
      script.onload = function( ) { resolve( true ); };
      script.onerror = function( ) { resolve( false ); };

      document.body.appendChild( script );
   } );
}

function chat_keys_load_scripts( )
{
   if( g_chat_keys_scripts === null )
   {
      g_chat_keys_scripts = c_chat_keys_scripts.reduce( function( chain, src )
      {
         return chain.then( function( ok ) { return ok && chat_keys_load_script( src ); } );
      }, Promise.resolve( true ) ).then( function( ok )
      {
         if( !ok )
            g_chat_keys_scripts = null;

         return ok;
      } );
   }

   return g_chat_keys_scripts;
}

function chat_keys_kept( )
{
   var name = c_signin_storage_keys_prefix + ciyam.access;

   try
   {
      return localStorage.getItem( name ) || sessionStorage.getItem( name ) || "";
   }
   catch( e )
   {
      return "";
   }
}

function chat_keys_url( extra )
{
   return ciyam.get_cws_url( ) + "/user-keys?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text" + ( extra || "" )
    + "&session=" + ciyam.sessid;
}

// NOTE: The node's copy of the keys, as text - "" if the request failed.
async function chat_keys_fetch( )
{
   var response = "";

   await serialised( function( )
   {
      return ciyam.fetch( chat_keys_url( "" ), "GET", function( r ) { response = String( r || "" ).trim( ); } );
   } );

   return response;
}

// NOTE: After a sign in. An access key that does not open the node's keys - or none there - is as none; My account says why.
async function chat_keys_read( )
{
   var read = ++g_chat_keys_read;
   var asked_for = ciyam.access;

   g_chat_keys = null;
   g_chat_opened = { };
   g_chat_secrets = { };

   var access = chat_keys_kept( );
   var found = false;
   var unread = false;

   // NOTE: Kept here but not read - the scripts, or the node's answer an error or a time-out (found by review, 2026-10-26) - is
   // said as that, not as no keys.
   if( ( access !== "" ) && !await chat_keys_load_scripts( ) )
      unread = true;
   else if( access !== "" )
   {
      var response = await chat_keys_fetch( );

      try
      {
         found = chat_keys_from( access, await ciyam_module_named( "user_keys" ).unlock( access, response ) );
      }
      catch( e )
      {
         found = false;
         unread = ( response !== "[none]" ) && !/^UK[0-9]+:/.test( response );
      }
   }

   // NOTE: Signed out, or signed in again, while they were read - not this sign in's.
   if( ( read !== g_chat_keys_read ) || ( ciyam.access !== asked_for ) )
      return;

   g_chat_keys = found;
   g_chat_keys_unread = unread;

   show_thread_title( );
   chat_render_room_key( );
   chat_keys_redraw( );
}

// NOTE: What the chat keeps of them - all of it, as a room key's change locks them again ("chat_keys_save_room( )").
function chat_keys_from( access, opened )
{
   return { access: access, rounds: opened.rounds, primary: opened.primary, secondary: opened.secondary, contacts: opened.contacts,
    rooms: opened.rooms || { } };
}

function chat_keys_forget( )
{
   ++g_chat_keys_read;

   g_chat_keys = null;
   g_chat_keys_unread = false;
   g_chat_opened = { };
   g_chat_secrets = { };
}

// NOTE: The open room's other person, if it is a direct message the console would encrypt too.
function chat_dm_other( )
{
   return room_is_dm( ) ? dm_other_person( g_room_name, ciyam.username ) : "";
}

// NOTE: The secret with "other", or "" - no one, no keys here, or no key for them.
function chat_secret_with( other )
{
   if( ( other === "" ) || !g_chat_keys || !g_chat_keys.contacts.hasOwnProperty( other ) )
      return "";

   if( !g_chat_secrets.hasOwnProperty( other ) )
      g_chat_secrets[ other ] = ciyam_module_named( "secure_message" ).shared_secret_of_seed( g_chat_keys.primary, g_chat_keys.contacts[ other ] );

   return g_chat_secrets[ other ];
}

// NOTE: A room's key, or "" - none kept, no keys here, or a room that does not take one ("can_have_room_key( )").
function chat_room_key( room )
{
   var entry = ( room === g_room ) ? { name: g_room_name } : find_room( room );

   if( !g_chat_keys || !entry || !can_have_room_key( room, entry.name ) || !g_chat_keys.rooms.hasOwnProperty( room ) )
      return "";

   return g_chat_keys.rooms[ room ];
}

// NOTE: The open room's - the other person's in a direct message, else the room key - or "" if it cannot be encrypted.
function chat_room_secret( )
{
   var other = chat_dm_other( );

   return ( other !== "" ) ? chat_secret_with( other ) : chat_room_key( g_room );
}

// NOTE: For the line under a direct message ("dm_encryption_note( )").
function chat_encryption_note( )
{
   var other = chat_dm_other( );

   if( g_chat_keys === null )
      return dm_encryption_note( other, null, false );

   if( g_chat_keys_unread )
      return dm_encryption_note( other, "unread", false );

   return dm_encryption_note( other, !!g_chat_keys, !!g_chat_keys && g_chat_keys.contacts.hasOwnProperty( other ) );
}

function chat_draw_opened( element, opened )
{
   element.classList.toggle( "chat-unreadable", !opened.ok );
   element.classList.toggle( "chat-encrypted", opened.ok );

   if( opened.ok )
      draw_formatted( element, format_pieces( opened.text ) );
   else
      element.textContent = "Encrypted message - not readable with the keys you have";

   // NOTE: A room key is shared in an encrypted direct message, as the console's owner sends it.
   if( opened.ok && ( chat_dm_other( ) !== "" ) )
      chat_offer_room_key( element, opened.text );
}

// NOTE: Drawn opened, when the open room has a secret - true if it is being handled here, false to show it can't be read.
function chat_open_message( element, text )
{
   var secret = chat_room_secret( );

   if( secret === "" )
      return false;

   var key = secret + "|" + text;

   if( g_chat_opened.hasOwnProperty( key ) )
   {
      chat_draw_opened( element, g_chat_opened[ key ] );

      return true;
   }

   element.classList.add( "chat-unreadable" );
   element.textContent = "Opening…";

   ciyam_module_named( "secure_message" ).open( secret, text ).then( function( plain )
   {
      return { ok: true, text: plain };
   }, function( )
   {
      return { ok: false, text: "" };
   } ).then( function( opened )
   {
      g_chat_opened[ key ] = opened;

      // NOTE: A message's row keeps its text ("build_message( )"); a pinned line or an announcement does not.
      if( element.isConnected && ( ( element.dataset.text === undefined ) || ( element.dataset.text === text ) ) )
         chat_draw_opened( element, opened );
   } );

   return true;
}

// NOTE: Once the keys are read, the encrypted messages already drawn are drawn again.
function chat_keys_redraw( )
{
   document.querySelectorAll( "#message_list .chat-message-body" ).forEach( function( body )
   {
      if( message_prefix( body.dataset.text || "" ) === "X" )
         draw_message_text( body, body.dataset.text );
   } );
}

// NOTE: What is sent for "text" to "other" - sealed when there is a secret with them, or the text as it is. "sealed" with no text
// when it is too long to seal, said by the caller.
async function chat_seal_for( other, text )
{
   return chat_seal_with( chat_secret_with( other ), text );
}

// NOTE: For the open room - with the other person in a direct message, or the room key.
async function chat_seal_here( text )
{
   return chat_seal_with( chat_room_secret( ), text );
}

async function chat_seal_with( secret, text )
{
   if( secret === "" )
      return { sealed: false, text: text };

   if( sealed_too_long( text ) )
      return { sealed: true, text: "" };

   return { sealed: true, text: await ciyam_module_named( "secure_message" ).seal( secret, text ) };
}

// ---- Room keys (Ian, 2026-10-25) - made or added under Room Details, or from one shared in an encrypted direct message

// NOTE: The room key for "room" set - or taken away, with "" - on the node's latest copy of the keys (another browser, or My
// account, may have changed them since they were read), locked again and sent. "" when done, else what to say.
async function chat_keys_save_room( room, key )
{
   if( !g_chat_keys )
      return "Set up your keys on My account first.";

   var uk = ciyam_module_named( "user_keys" );
   var access = g_chat_keys.access;
   var asked_for = ciyam.access;
   var latest = null;

   try
   {
      latest = await uk.unlock( access, await chat_keys_fetch( ) );
   }
   catch( e )
   {
      return "Your keys could not be read from the node - reload the page to try again.";
   }

   var rooms = Object.assign( { }, latest.rooms );

   if( key === "" )
      delete rooms[ room ];
   else
      rooms[ room ] = key;

   var locked = "";

   try
   {
      locked = await uk.lock( access, { primary: latest.primary, secondary: latest.secondary, contacts: latest.contacts, rooms: rooms },
       { rounds: latest.rounds } );
   }
   catch( e )
   {
      return e.message;
   }

   var answer = "";

   await serialised( function( )
   {
      return ciyam.fetch( chat_keys_url( "&payload=" + encodeURIComponent( locked ) ), "PUT", function( r ) { answer = String( r || "" ); } );
   } );

   if( is_timeout_response( answer ) )
      return "The server didn't answer in time - try again.";

   if( is_error_response( answer ) )
      return "The room key could not be saved: " + error_text( answer );

   // NOTE: Signed out, or in again, meanwhile - saved, but not this sign in's to keep.
   if( ciyam.access !== asked_for )
      return "";

   latest.rooms = rooms;

   g_chat_keys = chat_keys_from( access, latest );
   g_chat_secrets = { };

   show_thread_title( );
   chat_render_room_key( );
   chat_keys_redraw( );

   return "";
}

function chat_random_room_key( )
{
   return Array.from( crypto.getRandomValues( new Uint8Array( 16 ) ), function( b ) { return b.toString( 16 ).padStart( 2, "0" ); } ).join( "" );
}

// NOTE: The room the Room key part was last drawn for - a change of room puts away what was open there.
var g_room_key_drawn_for = "";
var g_room_key_busy = false;

// NOTE: The Room key part of Room Details - what this browser can do in the open room.
function chat_render_room_key( )
{
   var part = document.getElementById( "room_key_part" );

   if( part === null )
      return;

   if( g_room_key_drawn_for !== g_room )
   {
      g_room_key_drawn_for = g_room;

      chat_room_key_reset( );
   }

   var key = chat_room_key( g_room );
   var state = "";

   if( !can_have_room_key( g_room, g_room_name ) || ( g_chat_keys === null ) )
      state = "";
   else if( g_chat_keys_unread )
      state = "Your keys could not be read just now - reload the page to use a room key.";
   else if( !g_chat_keys )
      state = "Not encrypted. To use a room key, set up your keys on My account.";
   else if( key === "" )
      state = "Not encrypted - messages here are sent as they are.";
   else
      state = "Encrypted with the room key - members without it see your messages as unreadable.";

   part.hidden = ( state === "" );

   document.getElementById( "room_key_state" ).textContent = state;
   document.getElementById( "room_key_none_actions" ).hidden = !g_chat_keys || ( key !== "" )
    || !document.getElementById( "room_key_add_part" ).hidden;
   document.getElementById( "room_key_have_actions" ).hidden = !g_chat_keys || ( key === "" );

   if( key === "" )
      chat_hide_room_key_share( );

   [ "room_key_make", "room_key_add_open", "room_key_add_go", "room_key_share", "room_key_remove" ].forEach( function( id )
   {
      document.getElementById( id ).disabled = g_room_key_busy;
   } );
}

function chat_room_key_reset( )
{
   document.getElementById( "room_key_add_part" ).hidden = true;
   document.getElementById( "room_key_add_input" ).value = "";
   document.getElementById( "room_key_error" ).textContent = "";

   chat_hide_room_key_share( );
}

function chat_hide_room_key_share( )
{
   document.getElementById( "room_key_share_part" ).hidden = true;
   document.getElementById( "room_key_share_text" ).textContent = "";
}

// NOTE: Saved, said if it was not - the part busy meanwhile, so it is not asked twice.
async function chat_room_key_change( room, key )
{
   g_room_key_busy = true;

   document.getElementById( "room_key_error" ).textContent = "";

   chat_render_room_key( );

   var problem = await chat_keys_save_room( room, key );

   g_room_key_busy = false;

   if( g_room === room )
      document.getElementById( "room_key_error" ).textContent = problem;

   chat_render_room_key( );

   return problem === "";
}

async function do_make_room_key( )
{
   if( !confirm( "Make a key for this room? From now on your messages here are encrypted - members can read them once you've sent"
    + " them the key in an encrypted direct message." ) )
      return;

   var room = g_room;

   if( await chat_room_key_change( room, chat_random_room_key( ) ) && ( g_room === room ) )
      do_share_room_key( );
}

function do_open_add_room_key( )
{
   document.getElementById( "room_key_add_part" ).hidden = false;
   document.getElementById( "room_key_error" ).textContent = "";

   chat_render_room_key( );

   document.getElementById( "room_key_add_input" ).focus( );
}

function do_cancel_add_room_key( )
{
   chat_room_key_reset( );
   chat_render_room_key( );
}

async function do_add_room_key( )
{
   var given = parse_room_key( document.getElementById( "room_key_add_input" ).value, g_room );

   if( given.problem !== "" )
   {
      document.getElementById( "room_key_error" ).textContent = given.problem;

      return;
   }

   if( await chat_room_key_change( g_room, given.key ) )
      chat_room_key_reset( );

   chat_render_room_key( );
}

function do_share_room_key( )
{
   var key = chat_room_key( g_room );

   if( key === "" )
      return;

   document.getElementById( "room_key_share_text" ).textContent = room_key_text( g_room, key );
   document.getElementById( "room_key_share_part" ).hidden = false;
}

function do_copy_room_key( )
{
   var field = document.getElementById( "room_key_share_text" );
   var range = document.createRange( );

   range.selectNodeContents( field );

   window.getSelection( ).removeAllRanges( );
   window.getSelection( ).addRange( range );

   var copied = navigator.clipboard ? navigator.clipboard.writeText( field.textContent ).then( function( ) { return true; },
    function( ) { return document.execCommand( "copy" ); } ) : Promise.resolve( document.execCommand( "copy" ) );

   copied.then( function( done )
   {
      document.getElementById( "room_key_copy" ).textContent = done ? "Copied" : "Copy";

      setTimeout( function( ) { document.getElementById( "room_key_copy" ).textContent = "Copy"; }, 2000 );
   } );
}

async function do_remove_room_key( )
{
   if( !confirm( "Remove this room's key from your keys? Messages sealed with it can't be read here any more, and yours are sent"
    + " unencrypted - unless it's added again." ) )
      return;

   await chat_room_key_change( g_room, "" );
}

// NOTE: Under a message in a direct message that is just a room key ("room_key_in_message( )") for a room this person is in,
// that this browser has not got - added with one press.
function chat_offer_room_key( element, text )
{
   var shared = room_key_in_message( text );
   var entry = shared ? find_room( shared.room ) : null;

   if( !entry || !g_chat_keys || !can_have_room_key( shared.room, entry.name ) )
      return;

   var button = document.createElement( "button" );

   button.type = "button";
   button.className = "chat-btn chat-btn--small chat-room-key-offer";

   if( g_chat_keys.rooms[ shared.room ] === shared.key )
   {
      button.textContent = "Room key for " + entry.name + " added";
      button.disabled = true;
   }
   else
   {
      button.textContent = "Add this room key for " + entry.name;

      button.addEventListener( "click", function( )
      {
         button.disabled = true;

         chat_keys_save_room( shared.room, shared.key ).then( function( problem )
         {
            if( problem !== "" )
            {
               button.disabled = false;

               show_alert( problem, "is-error" );
            }
         } );
      } );
   }

   element.appendChild( button );
}

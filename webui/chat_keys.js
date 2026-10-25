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
      var response = "";

      await serialised( function( )
      {
         return ciyam.fetch( ciyam.get_cws_url( ) + "/user-keys?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text&session="
          + ciyam.sessid, "GET", function( r ) { response = String( r || "" ).trim( ); } );
      } );

      try
      {
         var opened = await ciyam_module_named( "user_keys" ).unlock( access, response );

         found = { primary: opened.primary, contacts: opened.contacts };
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
   chat_keys_redraw( );
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

// NOTE: The open room's - "" if it is not a direct message, or it cannot be encrypted.
function chat_room_secret( )
{
   return chat_secret_with( chat_dm_other( ) );
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
   var secret = chat_secret_with( other );

   if( secret === "" )
      return { sealed: false, text: text };

   if( sealed_too_long( text ) )
      return { sealed: true, text: "" };

   return { sealed: true, text: await ciyam_module_named( "secure_message" ).seal( secret, text ) };
}

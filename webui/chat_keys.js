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

// NOTE: An encrypted message that cannot be read here, as a QR code (Ian, 2026-10-30: for the offline device, where it may be).
// "qrcode.min.js" read only the first time one is shown.
var g_chat_qr_library = null;

function chat_qr_library( )
{
   if( g_chat_qr_library === null )
   {
      g_chat_qr_library = ( typeof QRCode !== "undefined" ) ? Promise.resolve( true ) : chat_keys_load_script( "qrcode.min.js" ).then( function( ok )
      {
         if( !ok )
            g_chat_qr_library = null;

         return ok;
      } );
   }

   return g_chat_qr_library;
}

// NOTE: "text" as a QR code in alphanumeric mode - "qrcode.min.js" makes byte mode only, so its model is given the text as
// alphanumeric data (mode 2: two characters in 11 bits, a last one in 6), and the smallest version that holds it is found.
// Null if none does. The model is made from the library's own (its prototype is an object written out, so "constructor" is
// not it), with what its constructor would set.
function chat_alphanumeric_qr( text )
{
   var model_kind = Object.getPrototypeOf( new QRCode( document.createElement( "div" ), { text: "0", correctLevel: QRCode.CorrectLevel.M } )._oQRCode );

   var data = {
      mode: 2,
      getLength: function( ) { return text.length; },
      write: function( buffer )
      {
         for( var i = 0; i < text.length; i += 2 )
         {
            if( i + 1 < text.length )
               buffer.put( ( c_base45_alphabet.indexOf( text[ i ] ) * 45 ) + c_base45_alphabet.indexOf( text[ i + 1 ] ), 11 );
            else
               buffer.put( c_base45_alphabet.indexOf( text[ i ] ), 6 );
         }
      } };

   for( var version = 1; version <= 40; version++ )
   {
      try
      {
         var model = Object.assign( Object.create( model_kind ), { typeNumber: version, errorCorrectLevel: QRCode.CorrectLevel.M, modules: null,
          moduleCount: 0, dataCache: null, dataList: [ data ] } );

         model.make( );

         return model;
      }
      catch( e )
      {
      }
   }

   return null;
}

// NOTE: The code drawn as an SVG, four modules of quiet space around it - a path of its dark modules.
function chat_qr_svg( model )
{
   var count = model.getModuleCount( );
   var size = count + 8;
   var path = "";

   for( var row = 0; row < count; row++ )
   {
      for( var col = 0; col < count; col++ )
      {
         if( model.isDark( row, col ) )
            path += "M" + ( col + 4 ) + " " + ( row + 4 ) + "h1v1h-1z";
      }
   }

   var svg = document.createElementNS( "http://www.w3.org/2000/svg", "svg" );
   var dark = document.createElementNS( "http://www.w3.org/2000/svg", "path" );

   svg.setAttribute( "viewBox", "0 0 " + size + " " + size );
   svg.setAttribute( "shape-rendering", "crispEdges" );
   svg.setAttribute( "aria-hidden", "true" );
   svg.dataset.modules = String( count );

   dark.setAttribute( "d", path );
   dark.setAttribute( "fill", "#000000" );

   svg.appendChild( dark );

   return svg;
}

async function chat_show_qr( sealed )
{
   var box = document.getElementById( "qr_dialog_code" );
   var error = document.getElementById( "qr_dialog_error" );

   box.replaceChildren( );
   error.textContent = "";

   document.getElementById( "qr_dialog" ).hidden = false;
   document.getElementById( "qr_dialog_close" ).focus( );

   var text = sealed_qr_text( sealed );

   if( text === "" )
   {
      error.textContent = "That message can't be shown as a QR code.";

      return;
   }

   if( !await chat_qr_library( ) )
   {
      error.textContent = "What draws QR codes could not be read - reload the page to try again.";

      return;
   }

   var model = chat_alphanumeric_qr( text );

   if( model === null )
   {
      error.textContent = "That message is too long for a QR code.";

      return;
   }

   box.appendChild( chat_qr_svg( model ) );
}

function do_close_qr_dialog( )
{
   document.getElementById( "qr_dialog" ).hidden = true;
   document.getElementById( "qr_dialog_code" ).replaceChildren( );
}

// NOTE: The other way (Ian, 2026-11-01: "we will need to be able to scan back an encrypted message if one has been created using
// an offline device") - its QR code read by the camera, or from a picture of it where there is no camera or no https, and posted
// in the open room just as it was encrypted ("scanned_message( )"). The reading is "qr_scan.js"'s.
var g_chat_scanner = null;
var g_chat_scanned = "";
var g_chat_scan_room = "";
var g_chat_scan_opened = 0;

function chat_scan_status( text, problem )
{
   document.getElementById( "scan_dialog_status" ).textContent = text;
   document.getElementById( "scan_dialog_error" ).textContent = problem || "";
}

function chat_scan_stop( )
{
   var scanner = g_chat_scanner;

   g_chat_scanner = null;

   return qr_scan_stop( scanner );
}

async function do_open_scan_dialog( )
{
   var opened = ++g_chat_scan_opened;

   g_chat_scanned = "";
   g_chat_scan_room = g_room;

   document.getElementById( "scan_dialog_reader" ).replaceChildren( );
   document.getElementById( "scan_dialog_text" ).textContent = "";
   document.getElementById( "scan_dialog_text" ).hidden = true;
   document.getElementById( "scan_dialog_post" ).disabled = true;
   document.getElementById( "scan_dialog" ).hidden = false;
   document.getElementById( "scan_dialog_picture" ).focus( );

   chat_scan_status( "Starting the camera…" );

   if( !await qr_scan_library( ) )
   {
      if( opened === g_chat_scan_opened )
         chat_scan_status( "", "The QR code reader could not be loaded - reload the page to try again." );

      return;
   }

   if( opened !== g_chat_scan_opened )
      return;

   if( !qr_scan_can_use_camera( ) )
   {
      chat_scan_status( "The camera needs the chat opened as https - choose a picture of the code instead." );

      return;
   }

   var scanner = qr_scan_reader( "scan_dialog_reader" );

   g_chat_scanner = scanner;

   try
   {
      await scanner.start( { facingMode: "environment" }, { fps: 10 }, chat_scan_read, function( ) { } );
   }
   catch( e )
   {
      if( g_chat_scanner === scanner )
         g_chat_scanner = null;

      if( opened === g_chat_scan_opened )
         chat_scan_status( "No camera could be used - choose a picture of the code instead." );

      return;
   }

   // NOTE: Closed while the camera was starting - let go of it now.
   if( ( opened !== g_chat_scan_opened ) || ( g_chat_scanner !== scanner ) )
   {
      g_chat_scanner = scanner;

      chat_scan_stop( );

      return;
   }

   chat_scan_status( "Hold the QR code from your offline device up to the camera." );
}

async function do_scan_picture( input )
{
   var file = ( input.files && input.files.length ) ? input.files[ 0 ] : null;
   var opened = g_chat_scan_opened;

   input.value = "";

   if( file === null )
      return;

   if( !await qr_scan_library( ) )
   {
      chat_scan_status( "", "The QR code reader could not be loaded - reload the page to try again." );

      return;
   }

   await chat_scan_stop( );

   if( opened !== g_chat_scan_opened )
      return;

   g_chat_scanned = "";

   document.getElementById( "scan_dialog_reader" ).replaceChildren( );
   document.getElementById( "scan_dialog_text" ).hidden = true;
   document.getElementById( "scan_dialog_post" ).disabled = true;

   chat_scan_status( "Reading the picture…" );

   var text = await qr_scan_picture( "scan_dialog_reader", file, function( ) { return opened === g_chat_scan_opened; } );

   if( opened !== g_chat_scan_opened )
      return;

   if( text === null )
      chat_scan_status( "", "No QR code could be read in that picture - try one closer up, or straighter." );
   else
      chat_scan_read( text );
}

// NOTE: What was read. The camera reads a code many times a second, so once one is taken the rest are not. Opened first where
// this browser has the room's secret: what it says shown, and one sealed for somewhere else not posted, as nobody here could read
// it. Without the secret it is posted as it is, for those with the keys.
async function chat_scan_read( text )
{
   if( g_chat_scanned !== "" )
      return;

   var opened = g_chat_scan_opened;
   var scanned = scanned_message( text );

   if( scanned.problem )
   {
      chat_scan_status( "", scanned.problem );

      return;
   }

   g_chat_scanned = scanned.sealed;

   await chat_scan_stop( );

   if( ( opened !== g_chat_scan_opened ) || ( g_room !== g_chat_scan_room ) )
      return;

   var secret = chat_room_secret( );
   var shown = document.getElementById( "scan_dialog_text" );

   if( secret === "" )
   {
      chat_scan_status( "An encrypted message - it can't be read here, so it is posted just as it is, for those with the keys." );

      document.getElementById( "scan_dialog_post" ).disabled = false;

      return;
   }

   try
   {
      var plain = await ciyam_module_named( "secure_message" ).open( secret, scanned.sealed );

      if( opened !== g_chat_scan_opened )
         return;

      shown.textContent = room_key_masked( plain );
      shown.hidden = false;

      chat_scan_status( "An encrypted message for this " + chat_scan_place( ) + ". It reads:" );

      document.getElementById( "scan_dialog_post" ).disabled = false;
   }
   catch( e )
   {
      if( opened === g_chat_scan_opened )
         chat_scan_status( "", "That message was not encrypted for this " + chat_scan_place( ) + " - nobody here could read it. Open the "
          + chat_scan_place( ) + " it was meant for, or choose a picture of another code." );
   }
}

// NOTE: What the open room is called, in what the dialog says - a direct message is a conversation (found by review).
function chat_scan_place( )
{
   return ( chat_dm_other( ) !== "" ) ? "conversation" : "room";
}

async function do_post_scanned( )
{
   var sealed = g_chat_scanned;
   var opened = g_chat_scan_opened;
   var post = document.getElementById( "scan_dialog_post" );

   if( ( sealed === "" ) || ( g_room !== g_chat_scan_room ) )
      return;

   // NOTE: As "do_send( )" - nobody would read it where the only other person declined, and a new conversation's first messages
   // wait until the others have joined. A scanned message is not held with them, so it is posted once they have (found by review).
   var pending = g_dm_pending[ g_chat_scan_room ];

   if( !pending && g_dm_shut[ g_chat_scan_room ] )
   {
      chat_scan_status( "", name_list( g_dm_declined[ g_chat_scan_room ] ) + " declined this conversation - nothing posted here would reach them." );

      return;
   }

   if( pending )
   {
      chat_scan_status( "", "This conversation is waiting for " + name_list( pending.waiting ) + " to join - post it once they have." );

      return;
   }

   post.disabled = true;

   chat_scan_status( "Posting…" );

   var answer = null;

   ciyam.error = "";

   await serialised( function( )
   {
      return ciyam.create_message( g_chat_scan_room, "text=" + sealed, function( response ) { answer = response; } );
   } );

   if( opened !== g_chat_scan_opened )
      return;

   var problem = ( ciyam.error !== "" ) ? ciyam.error : ( answer === null ) ? "The server did not answer." : "";

   if( ( problem === "" ) && is_timeout_response( answer ) )
      problem = "The server did not answer in time - check whether it arrived before posting it again.";
   else if( ( problem === "" ) && is_error_response( answer ) )
      problem = error_text( answer );

   if( problem !== "" )
   {
      chat_scan_status( "", problem );

      post.disabled = false;

      return;
   }

   do_close_scan_dialog( );

   if( g_room !== g_chat_scan_room )
      return;

   var result = parse_fetch_response( answer );

   g_members = apply_presence( result.members );

   render_members( );
   append_messages( result.messages );
   apply_pinned( result.pinned );

   g_start_point = later_start_point( g_start_point, next_start_point( result.messages ) );
}

function do_close_scan_dialog( )
{
   g_chat_scan_opened++;
   g_chat_scanned = "";

   // NOTE: The camera's picture is taken away by the library as it stops ("clear( )").
   chat_scan_stop( );

   document.getElementById( "scan_dialog" ).hidden = true;
   document.getElementById( "scan_dialog_text" ).textContent = "";
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
   g_owner_key_looked = { };
   g_room_key_later = { };
   g_room_key_waiting = { };
   g_owner_key_unopened = { };
   g_owner_keys_checked = { };
   g_owner_key_states = { };
   g_owner_away_checked = { };
   g_owner_away_members = { };
   g_owner_codes_told = { };
   g_keyless_deleted = { };

   show_thread_title( );
   chat_render_room_key( );
   chat_keys_redraw( );

   chat_look_for_owner_key( );
   chat_owner_send_keys_away( );
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
   g_room_keys_tried = { };
   g_owner_key_looked = { };
   g_room_key_later = { };
   g_room_key_waiting = { };
   g_owner_key_unopened = { };
   g_owner_keys_checked = { };
   g_owner_key_states = { };
   g_owner_away_checked = { };
   g_owner_away_members = { };
   g_owner_codes_told = { };
   g_keyless_deleted = { };
}

// NOTE: The open room's other person, if it is a direct message the console would encrypt too - by its name, whoever else is in
// it, as the console does (Damon, 2026-11-02): someone let in besides the two sees only what is sealed for them. It had stopped
// encrypting then, and posted in the clear to that someone too (found by review, 2026-11-01).
function chat_dm_other( )
{
   return dm_trusted( g_room_name, g_room_owner ) ? dm_other_person( g_room_name, ciyam.username ) : "";
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

// NOTE: The secret for "room", open or not - with the other person in a direct message the server named, else its room key.
function chat_secret_for_room( room )
{
   var entry = ( room === g_room ) ? { name: g_room_name } : find_room( room );
   var other = entry ? dm_other_person( entry.name, ciyam.username ) : "";

   return ( other !== "" ) ? chat_secret_with( other ) : chat_room_key( room );
}

// NOTE: For the line under a direct message ("dm_encryption_note( )"), or a group's ("group_encryption_note( )").
function chat_encryption_note( )
{
   var other = chat_dm_other( );
   var keys = ( g_chat_keys === null ) ? null : ( g_chat_keys_unread ? "unread" : !!g_chat_keys );

   if( ( other === "" ) && can_have_room_key( g_room, g_room_name ) )
      return group_encryption_note( keys, chat_room_key( g_room ) !== "" );

   return dm_encryption_note( other, keys, !!g_chat_keys && g_chat_keys.contacts.hasOwnProperty( other ) );
}

// NOTE: The open room's badge and details row ("encryption_badge( )"), or null for none.
function chat_encryption_badge( )
{
   var dm = parse_dm_name( g_room_name );
   var others = ( dm && dm.people ) ? dm.people.filter( function( person ) { return person !== ciyam.username; } ) : [ ];
   var kind = ( chat_dm_other( ) !== "" ) ? "dm" : ( dm !== null ) ? "group" : "room";

   return encryption_badge( chat_encryption_note( ), others, kind, chat_is_room_owner( ), g_room_owner, chat_has_owner_key( ) );
}

function chat_draw_opened( element, opened )
{
   element.classList.toggle( "chat-unreadable", !opened.ok );
   element.classList.toggle( "chat-locked", !opened.ok );
   element.classList.toggle( "chat-encrypted", opened.ok );

   // NOTE: A room's key note, or its receipt, in words (Damon, 2026-11-05) - not in a direct message, where a key sent by hand is
   // offered to add as before.
   var note = ( opened.ok && ( chat_dm_other( ) === "" ) ) ? key_note_text( opened.text, element.dataset.sender || "", ciyam.username,
    chat_note_other( element ) ) : "";

   // NOTE: A room key in it cut short - the button under it says whether it was added ("room_key_masked( )", Ian, 2026-10-27).
   if( note !== "" )
      element.textContent = note;
   else if( opened.ok )
      draw_formatted( element, format_pieces( room_key_masked( opened.text ) ), links_here( element ) );
   else
      element.textContent = "Encrypted message - not readable with the keys you have";

   chat_offer_sealed_actions( element, opened.ok );

   // NOTE: A poll in a room with a key is sealed like any message, and drawn as one once opened (Ian, 2026-10-31).
   mark_poll( element, opened.ok ? opened.text : "" );

   // NOTE: A room key is shared in an encrypted direct message, as the console's owner sends it.
   if( opened.ok && ( chat_dm_other( ) !== "" ) )
      chat_offer_room_key( element, opened.text );
}

// NOTE: One's own encrypted message offered to edit once opened here, and not otherwise - nor taken by Up (found by review,
// 2026-10-30). Someone else's offered to react to only then too (Ian, 2026-10-30: not to a message you cannot read), its chips
// not given by a click either ("is-unopened", "chat.css"). "element" the message's words; a pinned line or an announcement has
// no row, and is left alone.
function chat_offer_sealed_actions( element, opened )
{
   var row = element.closest( ".chat-message" );

   if( row === null )
      return;

   row.classList.toggle( "is-unopened", !opened );

   var edit = row.querySelector( ".chat-message-edit" );
   var react = row.querySelector( ".chat-message-react" );

   if( ( edit !== null ) && ( edit.dataset.sealed === "1" ) )
      edit.hidden = !opened;

   if( ( react !== null ) && ( react.dataset.sealed === "1" ) )
      react.hidden = !opened;

   // NOTE: As a QR code only while it cannot be read here.
   var qr = row.querySelector( ".chat-message-qr" );

   if( ( qr !== null ) && qr.dataset.text )
      qr.hidden = opened;
}

// NOTE: A member of a room that is encrypted, without its key here (Damon, 2026-11-05: nothing to be posted there in the clear) -
// known by the owner's private notes to them: the key's, sealed, or the plain one saying it is encrypted. Not by any encrypted message
// in it - one scanned in from an offline device is sealed in a room with no key (found by the suites).
function chat_member_keyless( )
{
   return chat_member_keyless_kind( ) !== "";
}

// NOTE: Whether the open room shows a message sealed with a room key - not a private one, which a secret between two seals.
function chat_room_shows_sealed( )
{
   return Array.prototype.some.call( document.querySelectorAll( "#message_list .chat-message-body" ), function( body )
   {
      return ( body.dataset.private !== "1" ) && ( message_prefix( body.dataset.text || "" ) === "X" );
   } );
}

// NOTE: How a member without the key gets it - "accept": the owner's note with it is here; "codes": not, and each needs the other's
// contact code (the owner's plain note says so, even when the owner's is here - the owner lacks theirs: found by review, 2026-11-06);
// "" for a room that is not encrypted, or one they have the key for.
function chat_member_keyless_kind( )
{
   var entry = ( g_room !== "" ) ? find_room( g_room ) : null;

   if( !entry || chat_is_room_owner( ) || ( chat_dm_other( ) !== "" ) || !can_have_room_key( g_room, g_room_name )
    || ( chat_room_key( g_room ) !== "" ) )
      return "";

   var notice = keyless_note_text( g_room_owner );
   var sealed = false;
   var told = false;

   document.querySelectorAll( "#message_list .chat-message-body" ).forEach( function( body )
   {
      if( ( body.dataset.private !== "1" ) || ( body.dataset.sender !== g_room_owner ) )
         return;

      if( message_prefix( body.dataset.text || "" ) === "X" )
         sealed = true;
      else if( body.dataset.text === notice )
         told = true;
   } );

   // NOTE: The owner's note with the key is in the reserved room since 2026-11-08 ("c_key_notes_room") - found there for this room; or,
   // without the owner's code, a sealed note from them there that cannot be opened, so for which room is not known. Taken as this
   // one, so nothing is posted in the clear - and once the codes are exchanged, that no longer holds.
   if( g_room_key_waiting[ g_room ] && chat_has_owner_key( ) )
      return "accept";

   if( !chat_has_owner_key( ) && chat_owner_note_in_notes( g_room_owner ) )
      sealed = true;

   if( sealed && chat_has_owner_key( ) )
      return "accept";

   return ( sealed || told ) ? "codes" : "";
}

// NOTE: For a private message between two - a room key's note, or its receipt (Damon, 2026-11-05) - the secret those two share, or
// "": the other of the two is its sender, or when this person sent it, the one it is to.
function chat_note_other( element )
{
   if( element.dataset.private !== "1" )
      return "";

   var sender = element.dataset.sender || "";

   // NOTE: Posted "for=~<other>" (Ian, 2026-11-06) - the node names both.
   if( element.dataset.pair )
      return ( sender === ciyam.username ) ? element.dataset.pair : sender;

   var others = ( element.dataset.recipients || "" ).split( "," ).filter( function( name ) { return ( name !== "" ) && ( name !== ciyam.username ); } );

   if( sender !== ciyam.username )
      return sender;

   return ( others.length === 1 ) ? others[ 0 ] : "";
}

// NOTE: Drawn opened, when the open room has a secret - true if it is being handled here, false to show it can't be read. A private
// message the room's secret does not open is tried with the secret its two people share.
function chat_open_message( element, text )
{
   var secret = chat_room_secret( );
   var pair = chat_secret_with( chat_note_other( element ) );

   if( ( secret === "" ) && ( pair === "" ) )
      return false;

   var key = ( ( secret !== "" ) ? secret : pair ) + "|" + text;

   if( g_chat_opened.hasOwnProperty( key ) )
   {
      chat_draw_opened( element, g_chat_opened[ key ] );

      return true;
   }

   element.classList.add( "chat-unreadable" );
   element.textContent = "Opening…";

   var read = g_chat_keys_read;

   var sealer = ciyam_module_named( "secure_message" );
   var first = ( secret !== "" ) ? sealer.open( secret, text ) : Promise.reject( );

   first.catch( function( )
   {
      if( ( pair === "" ) || ( pair === secret ) )
         throw new Error( "not opened" );

      return sealer.open( pair, text );
   } ).then( function( plain )
   {
      return { ok: true, text: plain };
   }, function( )
   {
      return { ok: false, text: "" };
   } ).then( function( opened )
   {
      // NOTE: Signed out while it was opened - its words are not kept, nor drawn (found by review, 2026-11-01).
      if( read !== g_chat_keys_read )
         return;

      g_chat_opened[ key ] = opened;

      // NOTE: A message's row keeps its text ("build_message( )"); a pinned line or an announcement does not.
      if( element.isConnected && ( ( element.dataset.text === undefined ) || ( element.dataset.text === text ) ) )
         chat_draw_opened( element, opened );
   } );

   return true;
}

// NOTE: An encrypted message's words as this chat opened them - to edit it (Ian, 2026-10-29: encrypted messages could not be edited)
// - or null if it was not opened here.
function chat_opened_text( text )
{
   var opened = g_chat_opened[ chat_room_secret( ) + "|" + text ];

   return ( opened && opened.ok ) ? opened.text : null;
}

// NOTE: Once the keys are read, the encrypted messages already drawn are drawn again - and the others marked, or not, as not
// encrypted, as there is now a key for the room or not ("is_unencrypted_message( )").
function chat_keys_redraw( )
{
   var keyed = ( chat_secret_for_room( g_room ) !== "" );

   document.querySelectorAll( "#message_list .chat-message-body" ).forEach( function( body )
   {
      if( message_prefix( body.dataset.text || "" ) === "X" )
         draw_message_text( body, body.dataset.text, g_room );
      else
         body.classList.toggle( "chat-unencrypted", is_unencrypted_message( body.dataset.text || "", g_room, keyed ) );
   } );

   // NOTE: The pinned strip too - its lines keep no text to be drawn again from, so they stayed sealed until the room was read
   // again (Ian, 2026-11-03: a room key added, the pinned ones not opened).
   if( g_pinned.length > 0 )
      render_pinned( );
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

// NOTE: Whether this account's keys are not known here yet - still being read after a sign in, or not read (an error or a time-out:
// "g_chat_keys_unread"). Either way there is no telling whether a text should have been sealed.
function chat_keys_unknown( )
{
   return ( g_chat_keys === null ) || ( ( g_chat_keys === false ) && g_chat_keys_unread );
}

// NOTE: Whether a text may go unsealed (found by review, 2026-11-06: while the keys were still being read, or could not be, one went
// in the clear even where a room key or a contact was kept) - not while they are read, and only once asked if they could not be.
async function chat_may_send_unsealed( )
{
   if( g_chat_keys === null )
   {
      show_alert( "Your keys are still being read - send it again in a moment.", "is-error" );

      return false;
   }

   if( chat_keys_unknown( ) )
      return await ask_dialog( { title: "Send it unencrypted?", text: "Your keys could not be read from the node, so it can't be"
       + " encrypted here - whoever runs the node could read it.", yes: "Send unencrypted", danger: true } );

   return true;
}

// ---- Room keys (Ian, 2026-10-25) - made with a new room (Ian, 2026-10-28), in the Room key dialog, or added from one shared in an
// encrypted direct message

// NOTE: The room key for "room" set - or taken away, with "" - on the node's latest copy of the keys (another browser, or My
// account, may have changed them since they were read), locked again and sent. "" when done, else what to say.
// NOTE: One change at a time - each re-reads the node's copy before locking it again, so two at once would lose the first (found
// by review: the button under a shared key while Room Details was saving).
var g_room_key_saving = Promise.resolve( );

// NOTE: "only_if_none" - added as it arrived: not saved if a key for the room has been kept meanwhile ("kept" said for it), so of
// two arriving together the first stays (found by review, 2026-10-27).
function chat_keys_save_room( room, key, only_if_none )
{
   var saved = g_room_key_saving.then( function( )
   {
      var kept = g_chat_keys ? ( g_chat_keys.rooms[ room ] || "" ) : "";

      if( only_if_none && ( kept !== "" ) && ( kept !== key ) )
         return "kept";

      return chat_keys_save_room_now( room, key );
   } );

   g_room_key_saving = saved.catch( function( ) { } );

   return saved;
}

async function chat_keys_save_room_now( room, key )
{
   return chat_keys_save_now( function( latest )
   {
      if( key === "" )
         delete latest.rooms[ room ];
      else
         latest.rooms[ room ] = key;
   }, "The room key" );
}

// NOTE: "person"'s contact code ("parse_contact_code( )") added in the chat (Damon, 2026-11-08) - on the node's latest copy, checked
// again there ("exchange_contact_problem( )"), one change at a time as a room key's is ("chat_keys_save_room( )"). "" when done, else
// what to say.
function chat_keys_save_contact( contact, person, own_key, own_name )
{
   var saved = g_room_key_saving.then( function( )
   {
      return chat_keys_save_now( function( latest )
      {
         var problem = exchange_contact_problem( contact, person, latest.contacts, own_key, own_name );

         if( problem === "" )
            latest.contacts[ contact.name ] = contact.key;

         return problem;
      }, "Their contact code" );
   } );

   g_room_key_saving = saved.catch( function( ) { } );

   return saved;
}

// NOTE: "change" made to copies of the node's latest rooms and contacts - what it says is wrong returned, if anything - then locked
// again and sent; "what" names it if not saved.
async function chat_keys_save_now( change, what )
{
   if( !g_chat_keys )
      return "Set up your keys on My account first.";

   var uk = ciyam_module_named( "user_keys" );
   var access = g_chat_keys.access;
   var asked_for = ciyam.access;
   var read = g_chat_keys_read;
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
   var contacts = Object.assign( { }, latest.contacts );

   var problem = change( { rooms: rooms, contacts: contacts } ) || "";

   if( problem !== "" )
      return problem;

   var locked = "";

   try
   {
      // NOTE: With the guard's line the node has, which only the words could change (Ian, 2026-10-27).
      locked = uk.kept_text( latest.guard, await uk.lock( access, { primary: latest.primary, secondary: latest.secondary, contacts: contacts,
       rooms: rooms, people: latest.people }, { rounds: latest.rounds } ) );
   }
   catch( e )
   {
      return e.message;
   }

   // NOTE: Lost on the way, it is said as not saved, not taken as saved ("answer_of( )", found by review).
   var answer = String( await serialised( function( )
   {
      return answer_of( function( take ) { return ciyam.fetch_in_body( chat_keys_url( "&payload=" + encodeURIComponent( locked ) ), "PUT", take ); } );
   } ) || "" );

   if( is_timeout_response( answer ) )
      return "The server didn't answer in time - try again.";

   if( is_error_response( answer ) )
      return what + " could not be saved: " + error_text( answer );

   // NOTE: Signed out, or in again, meanwhile - saved, but not this sign in's to keep. Signing out leaves "ciyam.access" as it
   // was, so it is the keys read's count that says so - else the keys came back after signing out (found by review).
   if( ( read !== g_chat_keys_read ) || ( ciyam.access !== asked_for ) )
      return "";

   latest.rooms = rooms;
   latest.contacts = contacts;

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

// NOTE: The room the Room key part was last drawn for - a change of room puts away what was open there, the dialog too.
var g_room_key_drawn_for = "";
var g_room_key_busy = false;

// NOTE: Whether the open room's owner's public key is here - which a room key from them needs ("member_key_hint( )").
function chat_has_owner_key( )
{
   return !!g_chat_keys && ( g_room_owner !== "" ) && g_chat_keys.contacts.hasOwnProperty( g_room_owner );
}

// NOTE: Only the owner makes, sends and shares a room's key (Damon, 2026-10-27: a member given the key was offered Make) - only
// the owner's is added by itself as it arrives, so a member's could split the room between two keys. Members add or remove one.
function chat_is_room_owner( )
{
   return ( g_room_owner !== "" ) && ( g_room_owner === ciyam.username );
}

// NOTE: The Room key part of Room Details - what it is - and the Room key dialog, with what this browser can do in the open room.
function chat_render_room_key( )
{
   var part = document.getElementById( "room_key_part" );

   if( part === null )
      return;

   if( g_room_key_drawn_for !== g_room )
   {
      g_room_key_drawn_for = g_room;

      chat_room_key_reset( );

      document.getElementById( "room_key_dialog" ).hidden = true;
   }

   var key = chat_room_key( g_room );
   var owner = chat_is_room_owner( );
   var state = "";

   if( !can_have_room_key( g_room, g_room_name ) || ( g_chat_keys === null ) )
      state = "";
   else if( g_chat_keys_unread )
      state = "Your keys could not be read just now - reload the page to use a room key.";
   else if( !g_chat_keys )
      state = "Not encrypted. To use a room key, set up your keys on My account.";
   else if( key !== "" )
      state = "Encrypted with the room key - members without it see your messages as unreadable.";
   else if( owner )
      state = "Not encrypted - messages here are sent as they are.";
   else
      state = "Not encrypted - messages here are sent as they are. " + member_key_hint( g_room_owner, chat_has_owner_key( ) ) + ".";

   part.hidden = ( state === "" );

   document.getElementById( "room_key_state" ).textContent = state;
   document.getElementById( "room_key_dialog_state" ).textContent = state;
   // NOTE: The owner's (Damon, 2026-11-05) - a member is given the key, and asked to accept it ("chat_look_for_owner_key( )"); but
   // a member without one, in a room that is encrypted, may add it by hand (Damon, 2026-11-07, as Ian asked: a poll's voters were
   // never passed it) - only then, so a member's key can neither replace the room's nor encrypt a room that has none.
   var member_may_add = !owner && ( key === "" ) && ( chat_member_keyless( ) || chat_room_shows_sealed( ) );

   document.getElementById( "room_key_open" ).hidden = !g_chat_keys || ( !owner && !member_may_add );

   // NOTE: Nothing to do there without keys - the dialog put away if they are forgotten meanwhile.
   if( !g_chat_keys || ( state === "" ) )
      document.getElementById( "room_key_dialog" ).hidden = true;

   document.getElementById( "room_key_none_actions" ).hidden = !g_chat_keys || ( key !== "" )
    || !document.getElementById( "room_key_add_part" ).hidden;
   document.getElementById( "room_key_have_actions" ).hidden = !g_chat_keys || ( key === "" );

   document.getElementById( "room_key_make" ).hidden = !owner;
   document.getElementById( "room_key_send" ).hidden = !owner;
   document.getElementById( "room_key_share" ).hidden = !owner;

   // NOTE: A group's direct message is encrypted in one go ("do_make_room_key( )").
   document.getElementById( "room_key_make" ).textContent = room_is_dm( ) ? "Encrypt this conversation" : "Make a room key";

   if( ( key === "" ) || !owner )
      chat_hide_room_key_share( );

   chat_render_member_key_states( ( key !== "" ) && owner );

   [ "room_key_make", "room_key_add_open", "room_key_add_go", "room_key_send", "room_key_share", "room_key_remove" ].forEach( function( id )
   {
      document.getElementById( id ).disabled = g_room_key_busy;
   } );
}

// NOTE: For the owner of a room with a key - each member as their chat was last found: has the key, passed and not yet accepted, or
// waiting for contact codes (Damon, 2026-11-05: setting up an encrypted room said nothing of where it stood).
function chat_render_member_key_states( shown )
{
   var list = document.getElementById( "room_key_members" );

   if( list === null )
      return;

   var states = shown ? ( g_owner_key_states[ g_room ] || { } ) : { };
   var names = Object.keys( states ).sort( );

   list.replaceChildren( );
   list.hidden = ( names.length === 0 );

   names.forEach( function( name )
   {
      var item = document.createElement( "li" );
      var who = document.createElement( "span" );
      var what = document.createElement( "span" );

      item.className = "chat-room-key-member is-" + states[ name ];
      who.className = "chat-room-key-member-name";
      what.className = "chat-room-key-member-state";

      who.textContent = name;
      what.textContent = member_key_state_text( states[ name ] );

      item.appendChild( who );
      item.appendChild( what );

      // NOTE: The way to their codes, from their row (Damon, 2026-11-08).
      if( states[ name ] === "needs_codes" )
      {
         var go = document.createElement( "button" );

         go.type = "button";
         go.className = "chat-btn chat-btn--small chat-room-key-member-go";
         go.textContent = "Exchange codes";

         go.addEventListener( "click", function( )
         {
            do_close_room_key_dialog( );
            do_open_contact_dialog( name );
         } );

         item.appendChild( go );
      }

      list.appendChild( item );
   } );
}

function do_open_room_key_dialog( )
{
   chat_room_key_reset( );
   chat_render_room_key( );

   var dialog = document.getElementById( "room_key_dialog" );

   dialog.hidden = false;

   var first = Array.prototype.slice.call( dialog.querySelectorAll( "button" ) ).filter( function( button )
   {
      return button.offsetParent !== null;
   } )[ 0 ];

   ( first || document.getElementById( "room_key_dialog_close" ) ).focus( );
}

function do_close_room_key_dialog( )
{
   document.getElementById( "room_key_dialog" ).hidden = true;

   chat_room_key_reset( );
   chat_render_room_key( );

   // NOTE: A member who has added the key is offered the dialog no longer - the message box instead.
   var opener = document.getElementById( "room_key_open" );

   ( opener.hidden ? document.getElementById( "composer_input" ) : opener ).focus( );
}

function chat_room_key_reset( )
{
   document.getElementById( "room_key_add_part" ).hidden = true;
   document.getElementById( "room_key_add_input" ).value = "";
   document.getElementById( "room_key_error" ).textContent = "";
   document.getElementById( "room_key_sent" ).textContent = "";

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
   var room = g_room;

   // NOTE: A group's direct message is encrypted in one go - the key made and sent to each of them (Damon, 2026-10-27).
   if( room_is_dm( ) )
   {
      var people = chat_room_people( );

      if( await ask_dialog( { title: "Encrypt this conversation?", text: "A key is made for it and passed to each of its people here,"
       + " who are asked to accept it.", yes: "Encrypt it" } ) && ( g_room === room ) )
         chat_encrypt_group( room, people );

      return;
   }

   if( !await ask_dialog( { title: "Make a key for this room?", text: "From now on your messages here are encrypted - each member is"
    + " passed the key and asked to accept it.", yes: "Make the key" } ) || ( g_room !== room ) )
      return;

   // NOTE: Passed to the members straight away (Damon, 2026-11-05) - as a new room's is to each who joins.
   if( await chat_room_key_change( room, chat_random_room_key( ) ) && ( g_room === room ) )
   {
      do_share_room_key( );

      chat_owner_send_keys( );
   }
}

// ---- Sending a room key to its people (Damon, 2026-10-27: sharing it with each by hand in a group, one was missed - they had no
// direct message with its owner, the only way a key is added)

// NOTE: The open room's people - its members, and a group's people from its name, those who have not joined yet among them.
function chat_room_people( )
{
   var dm = room_is_dm( ) ? parse_dm_name( g_room_name ) : null;

   return g_members.map( function( member ) { return member.name; } ).concat( dm ? dm.people : [ ] );
}

// NOTE: Whether each of "people" can be sent a key - a public key here for each, and the keys read.
function chat_can_encrypt_for( people )
{
   return !!g_chat_keys && !g_chat_keys_unread && ( people.length > 0 ) && people.every( function( person )
   {
      return g_chat_keys.contacts.hasOwnProperty( person );
   } );
}

// NOTE: A key made for a group's "room", if it has none, and sent to "people" - what came of it said.
// NOTE: Busy meanwhile, as "do_send_room_key( )" is - Send to members pressed while it ran sent the key twice (found by review).
async function chat_encrypt_group( room, people )
{
   // NOTE: Busy from before the key is made - once it is kept, the room's members would be sent it by "chat_owner_send_keys( )" too
   // (found by testing, 2026-11-03: the two sends together, one failed).
   g_room_key_busy = true;

   if( ( chat_room_key( room ) === "" ) && !( await chat_room_key_change( room, chat_random_room_key( ) ) ) )
   {
      g_room_key_busy = false;

      return;
   }

   // NOTE: Made busy again - "chat_room_key_change( )" ends it.
   g_room_key_busy = true;

   chat_render_room_key( );

   var text = await chat_send_room_key( room, people );

   g_room_key_busy = false;

   chat_render_room_key( );

   if( text !== "" )
      chat_room_key_sent( room, text );
   else if( room_key_recipients( people, ciyam.username ).length > 0 )
      chat_room_key_sent( room, "Each member is passed it - and asked to accept it." );
}

// NOTE: Said in the Room key dialog, and in the alert after "lead" (what was done, an invitation say) - "Room key:" there, as the
// alert is seen with the dialog shut.
function chat_room_key_sent( room, text, lead )
{
   if( g_room === room )
      document.getElementById( "room_key_sent" ).textContent = text;

   show_alert( ( lead ? lead + " " : "" ) + "Room key: " + text, "is-info" );
}

// NOTE: Every new room keyed (Ian, 2026-10-28) - a key made as it is created and passed to each invited, as a group's is
// ("chat_encrypt_group( )"): as they join (Damon, 2026-11-05), and since 2026-11-08 at once, before they have (Ian). Without keys of
// one's own the room is made as it was, Room Details saying why it is not encrypted.
async function chat_key_new_room( room, invitees )
{
   var entry = find_room( room );

   // NOTE: Made without keys of one's own (Damon, 2026-11-05) - said, and kept here, so opening it once they are set up asks to
   // encrypt it ("chat_offer_encrypt_room( )").
   if( ( g_chat_keys === false ) && !g_chat_keys_unread && entry && can_have_room_key( room, entry.name ) )
   {
      chat_unkeyed_rooms_change( room, true );

      show_alert( entry.name + " is made, but not encrypted - your keys aren't set up yet. Set them up on My account, and you're asked"
       + " to encrypt it the next time you open it.", "is-info" );

      return;
   }

   if( !g_chat_keys || g_chat_keys_unread || !entry || !can_have_room_key( room, entry.name ) || ( chat_room_key( room ) !== "" ) )
      return;

   if( invitees.length > 0 )
      await chat_encrypt_group( room, invitees );
   else
      await chat_room_key_change( room, chat_random_room_key( ) );
}

// NOTE: The rooms this account made before it had keys (Damon, 2026-11-05) - kept on this browser by number, as nothing on the node
// says how a room was made. "cws.unkeyed_rooms_<name>".
const c_unkeyed_rooms_prefix = "cws.unkeyed_rooms_";

function chat_unkeyed_rooms( )
{
   try
   {
      return JSON.parse( localStorage.getItem( c_unkeyed_rooms_prefix + ciyam.username ) || "[]" );
   }
   catch( e )
   {
      return [ ];
   }
}

function chat_unkeyed_rooms_change( room, add )
{
   var rooms = chat_unkeyed_rooms( ).filter( function( number ) { return number !== room; } );

   if( add )
      rooms.push( room );

   try
   {
      localStorage.setItem( c_unkeyed_rooms_prefix + ciyam.username, JSON.stringify( rooms ) );
   }
   catch( e )
   {
   }
}

// NOTE: A room made before the owner had keys, opened now they have them - asked to encrypt it, once each time it is opened; "Not
// now" asks again the next time (Damon, 2026-11-05). Encrypted, each member is passed the key as any new room's are.
var g_encrypt_offered = "";

async function chat_offer_encrypt_room( room, entry )
{
   if( !g_chat_keys || g_chat_keys_unread || g_room_key_busy || ( g_encrypt_offered === room ) || !can_have_room_key( room, entry.name )
    || ( chat_unkeyed_rooms( ).indexOf( room ) < 0 ) )
      return;

   g_encrypt_offered = room;

   var yes = await ask_dialog( { title: "Encrypt " + entry.name + " now?", text: "Your keys are set up now, but this room isn't encrypted."
    + " Encrypt it, and each member is passed its key here and asked to accept it. What is already posted stays as it is.",
    yes: "Encrypt it", no: "Not now" } );

   if( !yes || ( g_room !== room ) || ( chat_room_key( room ) !== "" ) )
      return;

   await chat_encrypt_group( room, g_members.map( function( member ) { return member.name; } ) );

   // NOTE: Only once it has a key - asked again the next time otherwise (found by review, 2026-11-06).
   if( chat_room_key( room ) !== "" )
      chat_unkeyed_rooms_change( room, false );
}

// NOTE: Anyone invited later is sent the key too (Ian, 2026-10-28) - by the owner, as only the owner's is added by itself. What came
// of it, or "" if nothing was sent.
async function chat_send_key_to_invited( room, names )
{
   var entry = find_room( room );

   if( !entry || ( entry.owner !== ciyam.username ) || ( chat_room_key( room ) === "" ) )
      return "";

   g_room_key_busy = true;

   chat_render_room_key( );

   var text = await chat_send_room_key( room, names );

   g_room_key_busy = false;

   chat_render_room_key( );

   return text;
}

async function do_send_room_key( )
{
   var room = g_room;

   if( !await ask_dialog( { title: "Send the key to each member?", text: "Each member is passed it again, in a note only they can"
    + " open, and asked to accept it.", yes: "Send it" } ) || ( g_room !== room ) )
      return;

   g_room_key_busy = true;

   chat_render_room_key( );

   var text = await chat_send_room_key( room, chat_room_people( ) );

   g_room_key_busy = false;

   chat_render_room_key( );

   if( text !== "" )
      chat_room_key_sent( room, text );
}

// NOTE: Passed to each as a note, not in a direct message (Damon, 2026-11-05) - since 2026-11-08 in the reserved room (Ian), so to
// "people" whether or not they have joined, the members too ("chat_owner_send_keys( )"). "people" looked at afresh. What came of it,
// or "" for nothing to say.
async function chat_send_room_key( room, people )
{
   if( ( chat_room_key( room ) === "" ) || ( room !== g_room ) )
      return "";

   ( people || [ ] ).forEach( function( person ) { delete g_owner_keys_checked[ room + "|" + person ]; } );

   return await chat_owner_send_keys( true, people );
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
   {
      chat_room_key_removed( g_room + "#" + given.key, false );

      chat_room_key_reset( );
   }

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
   chat_copy_text( "room_key_share_text", "room_key_copy" );
}

// NOTE: The text of "field_id" selected and copied, its button "button_id" saying so for a moment.
function chat_copy_text( field_id, button_id )
{
   var field = document.getElementById( field_id );
   var range = document.createRange( );

   range.selectNodeContents( field );

   window.getSelection( ).removeAllRanges( );
   window.getSelection( ).addRange( range );

   var copied = navigator.clipboard ? navigator.clipboard.writeText( field.textContent ).then( function( ) { return true; },
    function( ) { return document.execCommand( "copy" ); } ) : Promise.resolve( document.execCommand( "copy" ) );

   copied.then( function( done )
   {
      document.getElementById( button_id ).textContent = done ? "Copied" : "Copy";

      setTimeout( function( ) { document.getElementById( button_id ).textContent = "Copy"; }, 2000 );
   } );
}

// ---- Contact codes exchanged in the chat (Damon, 2026-11-08: members far apart, the room's owner among them, could not meet to scan
// each other's, and the room's key waited on it) - yours to copy, or to show as a QR code, and theirs pasted. Sent between the two
// another way they trust, never by the node (Ian, 2026-11-03: no public keys kept on the node).

const c_contact_qr_size = 160;

var g_contact_person = "";
var g_contact_opened = 0;

// NOTE: Owners told once a sign in, for each room and member, that they need each other's codes ("chat_owner_send_keys_in( )").
var g_owner_codes_told = { };

// NOTE: The plain notes saying codes were needed already deleted - not asked to go again as the room is looked at before it is redrawn.
var g_keyless_deleted = { };

// NOTE: Whether "person"'s contact code is wanted here - this account's keys read, and no code kept for them.
function chat_contact_missing( person )
{
   return !!g_chat_keys && !g_chat_keys_unread && ( String( person || "" ) !== "" ) && ( person !== ciyam.username )
    && !g_chat_keys.contacts.hasOwnProperty( person );
}

function chat_own_public_key( )
{
   return g_chat_keys ? ciyam_module_named( "secure_message" ).public_key_of_seed( g_chat_keys.primary ) : "";
}

// NOTE: Opened for "person" - your code in the page only now, and taken out again as it is closed (Ian, 2026-10-27).
async function do_open_contact_dialog( person )
{
   if( !g_chat_keys || g_chat_keys_unread || ( String( person || "" ) === "" ) || ( person === ciyam.username ) )
      return;

   var padding = Array.from( crypto.getRandomValues( new Uint8Array( 13 ) ), function( b ) { return b.toString( 16 ).padStart( 2, "0" ); } ).join( "" );
   var code = contact_code( account_display_name( ciyam.username, ciyam.is_admin ), chat_own_public_key( ), padding );

   if( code === "" )
      return;

   var opened = ++g_contact_opened;
   var have = g_chat_keys.contacts.hasOwnProperty( person );
   var dialog = document.getElementById( "contact_dialog" );
   var box = document.getElementById( "contact_dialog_qr" );

   g_contact_person = person;

   document.getElementById( "contact_dialog_title" ).textContent = "Exchange contact codes with " + person;
   document.getElementById( "contact_dialog_note" ).textContent = contact_exchange_text( person, have, false );
   document.getElementById( "contact_dialog_own" ).textContent = code;
   document.getElementById( "contact_dialog_input_label" ).textContent = person + "'s contact code";
   document.getElementById( "contact_dialog_input" ).value = "";
   document.getElementById( "contact_dialog_error" ).textContent = "";
   document.getElementById( "contact_dialog_theirs_part" ).hidden = have;
   document.getElementById( "contact_dialog_add" ).hidden = have;
   document.getElementById( "contact_dialog_add" ).disabled = false;
   document.getElementById( "contact_dialog_close" ).textContent = have ? "Done" : "Not now";
   document.getElementById( "contact_dialog_copy" ).textContent = "Copy";

   box.replaceChildren( );
   dialog.hidden = false;

   document.getElementById( have ? "contact_dialog_copy" : "contact_dialog_input" ).focus( );

   // NOTE: Drawn in the page - "qrcode.min.js" - so the code goes nowhere else; the library's own title taken off, as My account's.
   if( !await chat_qr_library( ) || ( opened !== g_contact_opened ) )
      return;

   new QRCode( box, { text: code, width: c_contact_qr_size, height: c_contact_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
    correctLevel: QRCode.CorrectLevel.M } );

   box.removeAttribute( "title" );
   box.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );
}

function do_close_contact_dialog( )
{
   ++g_contact_opened;

   g_contact_person = "";

   document.getElementById( "contact_dialog" ).hidden = true;
   document.getElementById( "contact_dialog_own" ).textContent = "";
   document.getElementById( "contact_dialog_qr" ).replaceChildren( );
   document.getElementById( "contact_dialog_input" ).value = "";
}

function do_copy_contact_code( )
{
   chat_copy_text( "contact_dialog_own", "contact_dialog_copy" );
}

// NOTE: Their code as pasted - checked as for this person, then kept as My account keeps one. Kept even if the dialog was closed
// meanwhile, and the keys looked at again for them at once ("chat_contact_added( )").
async function do_add_contact_code( )
{
   var person = g_contact_person;
   var opened = g_contact_opened;
   var error = document.getElementById( "contact_dialog_error" );
   var add = document.getElementById( "contact_dialog_add" );

   if( ( person === "" ) || !g_chat_keys || add.disabled )
      return;

   var contact = parse_contact_code( document.getElementById( "contact_dialog_input" ).value );
   var own_key = chat_own_public_key( );
   var own_name = account_display_name( ciyam.username, ciyam.is_admin );
   var problem = exchange_contact_problem( contact, person, g_chat_keys.contacts, own_key, own_name );

   error.textContent = problem;

   if( problem !== "" )
      return;

   add.disabled = true;

   var said = await chat_keys_save_contact( contact, person, own_key, own_name );

   if( said === "" )
      chat_contact_added( person );

   if( opened !== g_contact_opened )
      return;

   add.disabled = false;

   if( said !== "" )
   {
      error.textContent = said;

      return;
   }

   document.getElementById( "contact_dialog_note" ).textContent = contact_exchange_text( person, false, true );
   document.getElementById( "contact_dialog_theirs_part" ).hidden = true;
   document.getElementById( "contact_dialog_add" ).hidden = true;
   document.getElementById( "contact_dialog_close" ).textContent = "Done";
   document.getElementById( "contact_dialog_close" ).focus( );
}

// NOTE: "person"'s code here now - the room keys looked at again for them at once, not at the next change: the owner's rooms passing
// theirs to them, and the open room's taken from its owner.
function chat_contact_added( person )
{
   Object.keys( g_owner_keys_checked ).forEach( function( checked )
   {
      if( checked.slice( -( person.length + 1 ) ) === "|" + person )
         delete g_owner_keys_checked[ checked ];
   } );

   g_owner_away_checked = { };

   show_encryption_action( );

   chat_look_for_owner_key( false );
   chat_owner_send_keys_away( );
}

async function do_remove_room_key( )
{
   var room = g_room;

   if( !await ask_dialog( { title: "Remove this room's key?", text: "Messages sealed with it can't be read here any more, and yours"
    + " are sent unencrypted - unless it's added again.", yes: "Remove it", danger: true } ) || ( g_room !== room ) )
      return;

   var removed = chat_room_key( room );

   if( await chat_room_key_change( room, "" ) && ( removed !== "" ) )
      chat_room_key_removed( room + "#" + removed, true );
}

// NOTE: The room keys removed on this browser - not added again by themselves when the direct message that brought one is seen
// again (found by review, 2026-10-27); added by hand, one is forgotten from here. Kept for the account on this browser, as the
// notifications switch is: only a mark of each "<room>#<key>" (found by review, 2026-11-06: the keys themselves stayed here after
// sign out, for the next to use the browser to read). Those kept whole before are made marks as they are next read.
function chat_room_keys_removed_name( )
{
   return "cws.room_key_marks_removed_" + ciyam.access;
}

function chat_room_keys_removed_old_name( )
{
   return "cws.room_keys_removed_" + ciyam.access;
}

function chat_room_key_removed_mark( shared )
{
   return hex_sha256( "room_key_removed:" + shared );
}

function chat_room_keys_read( name )
{
   try
   {
      var kept = JSON.parse( localStorage.getItem( name ) || "[]" );

      return Array.isArray( kept ) ? kept : [ ];
   }
   catch( e )
   {
      return [ ];
   }
}

function chat_room_keys_write( marks )
{
   try
   {
      localStorage.setItem( chat_room_keys_removed_name( ), JSON.stringify( marks ) );
   }
   catch( e )
   {
   }
}

// NOTE: The marks ("chat_room_key_removed_mark( )").
function chat_room_keys_removed( )
{
   var marks = chat_room_keys_read( chat_room_keys_removed_name( ) );
   var old = chat_room_keys_read( chat_room_keys_removed_old_name( ) );

   if( old.length > 0 )
   {
      old.forEach( function( shared )
      {
         var mark = chat_room_key_removed_mark( String( shared ) );

         if( marks.indexOf( mark ) < 0 )
            marks.push( mark );
      } );

      chat_room_keys_write( marks );
   }

   try
   {
      localStorage.removeItem( chat_room_keys_removed_old_name( ) );
   }
   catch( e )
   {
   }

   return marks;
}

function chat_room_key_was_removed( shared )
{
   return chat_room_keys_removed( ).indexOf( chat_room_key_removed_mark( shared ) ) >= 0;
}

function chat_room_key_removed( shared, removed )
{
   var mark = chat_room_key_removed_mark( shared );
   var kept = chat_room_keys_removed( ).filter( function( entry ) { return entry !== mark; } );

   if( removed )
      kept.push( mark );

   chat_room_keys_write( kept );
}

// NOTE: Under a message in a direct message that starts with a room key ("room_key_in_message( )") for a room this person is in.
// Added as it arrives (Ian, 2026-10-26: no button to press, copying and pasting being painful) when it comes from the room's owner,
// as the console's owner sends it, none is kept for the room, and it has not been removed here; otherwise offered. One in place of a
// key already kept is asked first: whoever sent it could read what is sent there with it, and members with the old key could not
// (found by review). Each is added by itself only once a sign in, so a save that fails is not tried again at every redraw.
var g_room_keys_tried = { };

// ---- A room's key looked for in the direct message with its owner (Damon, 2026-11-02: a member who joined and stayed in the room
// had it unencrypted - the owner's key was added only as their direct message was drawn, and they never opened it)

// NOTE: For each room, its owner's direct message as it was last looked through - "<room>/<total>" from the listing - so opening
// the room again reads it only once something new is there.
var g_owner_key_looked = { };

// NOTE: Rooms whose key was put off with "Not now" - asked again the next time the room is opened ("chat_look_for_owner_key( )").
var g_room_key_later = { };

// NOTE: Rooms whose key from the owner was found waiting in the reserved room, and not yet added - so the room is known to be encrypted
// ("chat_member_keyless_kind( )"), the note not being in it.
var g_room_key_waiting = { };

// NOTE: Rooms whose owner's messages would not open, said once a sign in ("chat_look_for_owner_key( )").
var g_owner_key_unopened = { };

// NOTE: The sealed private messages "sender" posted in room "dm_room" ("sender" a name, or a list of them) - a room key's notes and
// their receipts (Damon, 2026-11-05) - read quietly from the start and opened with "secret": as { opened: [ { sender, text, unique } ],
// failed: <how many would not open>, newest_failed: <the newest would not> }, or null if it could not be read (looked for again
// later). Not the room's own messages, sealed with its key, which the two's secret never opens (found by review, 2026-11-06).
async function chat_dm_opened( dm_room, sender, secret )
{
   var result = await chat_room_read_quietly( dm_room );

   return ( result === null ) ? null : await chat_sealed_opened( result.messages, sender, secret );
}

// NOTE: A room read quietly from the start, as "parse_fetch_response( )" gives it - or null if it could not be read.
async function chat_room_read_quietly( room )
{
   var response = null;

   // NOTE: Quiet, like the poll it comes from ("watch_held_rooms( )").
   g_in_poll = true;

   var reading = serialised( function( )
   {
      return ciyam.fetch_messages( room, "from=0", function( text ) { response = String( text ); } );
   } );

   g_in_poll = false;

   await reading;

   var result = parse_fetch_response( response );

   if( ( response === null ) || is_timeout_response( response ) || ( result.error !== "" ) || ( result.rooms.length > 0 ) )
      return null;

   return result;
}

// NOTE: Of "messages" read, the sealed private ones "sender" posted, opened with "secret" ("chat_dm_opened( )").
async function chat_sealed_opened( messages, sender, secret )
{
   var senders = [ ].concat( sender );
   var opened = [ ];
   var failed = 0;
   var newest_failed = false;
   var sealer = ciyam_module_named( "secure_message" );

   for( var i = 0; i < messages.length; i++ )
   {
      var message = messages[ i ];

      if( ( message.kind !== c_kind_chat ) || !message.private || ( senders.indexOf( message.sender ) < 0 )
       || ( message_prefix( message.text ) !== "X" ) )
         continue;

      var cached = g_chat_opened[ secret + "|" + message.text ];
      var plain = cached ? ( cached.ok ? cached.text : null ) : undefined;

      if( plain === undefined )
      {
         try
         {
            plain = await sealer.open( secret, message.text );
         }
         catch( e )
         {
            plain = null;
         }
      }

      if( plain !== null )
         opened.push( { sender: message.sender, text: plain, unique: message.unique } );
      else
         failed++;

      newest_failed = ( plain === null );
   }

   return { opened: opened, failed: failed, newest_failed: newest_failed };
}

// NOTE: The open room, if it has no key here and is someone else's - their direct message read quietly, their sealed messages
// opened, and the newest key for the room added as opening it would ("chat_offer_room_key( )"). Called as the room is first read
// and once the keys are read - not at each poll: a read marks the direct message read and seen by this person, so one sent while
// they stay in the room waits for it to be opened again (Damon, 2026-11-03, after review). Since 2026-11-04 at each poll too, but
// read only once the direct message has something new - the key often comes a moment after the room is joined (Damon accepting
// that a message arriving there meanwhile is marked read). The owner's own room has its members sent the key instead
// ("chat_owner_send_keys( )") - not at a poll ("member_only"). The notes are in the reserved room since 2026-11-08 ("c_key_notes_room").
async function chat_look_for_owner_key( member_only )
{
   var room = g_room;
   var entry = ( room !== "" ) ? find_room( room ) : null;

   if( entry && ( entry.owner === ciyam.username ) )
   {
      if( member_only )
         return;

      if( chat_room_key( room ) === "" )
         chat_offer_encrypt_room( room, entry );
      else
         chat_owner_send_keys( );

      return;
   }

   if( !entry || !g_chat_keys || g_chat_keys_unread || !can_have_room_key( room, entry.name ) )
      return;

   // NOTE: Asked again each time the room is opened, not at every poll of it (Damon, 2026-11-05) - the room read again for it.
   if( !member_only )
   {
      delete g_room_key_later[ room ];
      delete g_owner_key_looked[ room ];
   }

   if( chat_room_key( room ) !== "" )
   {
      await chat_answer_held_key( room, entry );

      return;
   }

   if( g_room_key_later[ room ] )
      return;

   var owner = entry.owner;
   var secret = chat_secret_with( owner );

   // NOTE: The owner's note with the key is in the reserved room, to this member only (Ian, 2026-11-08 - "c_key_notes_room") - read
   // again once that has something new.
   var looked = room + "/" + chat_key_notes_total( );

   if( ( secret === "" ) || ( g_owner_key_looked[ room ] === looked ) )
      return;

   g_owner_key_looked[ room ] = looked;

   var read = g_chat_keys_read;
   var found = await chat_dm_opened( c_key_notes_room, owner, secret );

   // NOTE: Not read - looked for again the next time the room is opened.
   if( found === null )
   {
      delete g_owner_key_looked[ room ];

      return;
   }

   var key = owner_room_key( found.opened, owner, room, chat_room_keys_removed( ), chat_room_key_removed_mark );
   var whole = room_key_text( room, key );

   if( read !== g_chat_keys_read )
      return;

   // NOTE: Their note there would not open (Damon, 2026-11-03: a key sent, and never added) - the public key each keeps for the
   // other is not the other's now, one of them having made new keys since. Said once a sign in for the room, and only if the newest
   // would not - older ones sealed before new keys were exchanged never will (found by review, 2026-11-04).
   if( ( key === "" ) && found.newest_failed && ( g_room === room ) && !g_owner_key_unopened[ room ] )
   {
      g_owner_key_unopened[ room ] = true;

      show_alert( "The room key " + owner + " passed you for " + entry.name + " can't be opened - one of you has new keys since you added"
       + " each other. Add each other's contact code again, under Contacts on My account.", "is-error" );

      return;
   }

   // NOTE: Signed out, or the key added meanwhile - in another tab, say.
   if( ( key === "" ) || ( chat_room_key( room ) !== "" ) )
      return;

   // NOTE: Known to be encrypted now, its note being elsewhere - the bar over the message box says so ("chat_member_keyless_kind( )").
   g_room_key_waiting[ room ] = true;

   if( g_room === room )
      show_encryption_action( );

   if( g_room_keys_tried.hasOwnProperty( whole ) )
      return;

   g_room_keys_tried[ whole ] = true;

   // NOTE: Asked, not added by itself (Damon, 2026-11-05) - "Not now" asks again the next time the room is opened.
   var accepted = await ask_dialog( { title: "Accept the room key from " + owner + "?", text: "Messages in " + entry.name + " are"
    + " encrypted with a room key. Accept it to read them - and what you send here is encrypted too.", yes: "Accept", no: "Not now" } );

   if( ( read !== g_chat_keys_read ) || ( chat_room_key( room ) !== "" ) )
      return;

   if( !accepted )
   {
      g_room_key_later[ room ] = true;

      delete g_room_keys_tried[ whole ];

      if( g_room === room )
         show_encryption_action( );

      return;
   }

   var problem = "";

   // NOTE: Called without waiting for it as well, so nothing it meets is left unhandled (found by review).
   try
   {
      problem = await chat_keys_save_room( room, key, true );
   }
   catch( e )
   {
      problem = "The room key could not be saved - reload the page to try again.";
   }

   if( read !== g_chat_keys_read )
      return;

   if( problem === "" )
   {
      show_alert( "Room key for " + entry.name + " added - " + owner + "'s messages here, and yours, are encrypted with it.", "is-info" );

      // NOTE: Not looked for again until it is sent ("g_room_receipt_busy").
      g_room_receipt_busy[ room ] = true;

      try
      {
         await chat_send_room_key_receipt( owner, room, key );
      }
      finally
      {
         delete g_room_receipt_busy[ room ];
      }
   }
   else if( problem !== "kept" )
      show_alert( problem, "is-error" );
}

// NOTE: The rooms whose receipt is being looked for or sent (Ian, 2026-11-09: each member's receipt came back to the owner twice,
// about a second apart) - a sign in looks for it twice at once, as the room is first read and as the keys are, and each read the
// reserved room before the other's receipt was there. One at a time for a room, so a look begun after it ends finds the receipt.
var g_room_receipt_busy = { };

// NOTE: The room's key already here, and the owner's note for it there all the same - answered with a receipt if it has none
// ("member_receipt_due( )", Ian, 2026-11-07: after a restore from scratch the owner passes it again, and would otherwise count
// this member as without it for good). The reserved room read only when it holds a sealed private message from the owner, and only
// once it has something new.
async function chat_answer_held_key( room, entry )
{
   var owner = entry.owner;
   var secret = chat_secret_with( owner );
   var key = chat_room_key( room );
   var looked = room + "/" + chat_key_notes_total( );

   if( ( secret === "" ) || g_room_receipt_busy[ room ] || ( g_owner_key_looked[ room ] === looked ) || !chat_owner_note_waiting( owner ) )
      return;

   g_owner_key_looked[ room ] = looked;
   g_room_receipt_busy[ room ] = true;

   try
   {
      var read = g_chat_keys_read;
      var found = await chat_dm_opened( c_key_notes_room, [ owner, ciyam.username ], secret );

      if( found === null )
      {
         delete g_owner_key_looked[ room ];

         return;
      }

      if( ( read !== g_chat_keys_read ) || ( chat_room_key( room ) !== key ) )
         return;

      if( member_receipt_due( found.opened, owner, ciyam.username, room, key, room_key_mark( key ) ) )
         await chat_send_room_key_receipt( owner, room, key );
   }
   finally
   {
      delete g_room_receipt_busy[ room ];
   }
}

// NOTE: Whether the reserved room, as the invitations' poll last read it, holds a sealed private message from "owner" - a note with a
// key, perhaps. Not known for admin, whose chat does not read it for invitations, nor before its first read - looked at as if it might.
function chat_owner_note_waiting( owner )
{
   return ciyam.is_admin || !g_invitations_known || chat_owner_note_in_notes( owner );
}

// NOTE: The same, only as far as it is known.
function chat_owner_note_in_notes( owner )
{
   return g_invite_messages.some( function( message )
   {
      return ( message.kind === c_kind_chat ) && message.private && ( message.sender === owner ) && ( message_prefix( message.text ) === "X" );
   } );
}

// NOTE: The fingerprint a receipt names its key by ("room_key_receipt_text( )") - not the key, which is in no receipt.
function room_key_mark( key )
{
   return hex_sha256( "room_key:" + key ).substr( 0, 8 );
}

// NOTE: The key the owner sent added - a receipt sent back to them alone where the key came, sealed as the key was, so their chat can
// delete the note that carried it ("chat_owner_send_keys( )"). Not said if it could not be sent: the key is still in the note.
async function chat_send_room_key_receipt( owner, room, key )
{
   try
   {
      await chat_send_key_note( c_key_notes_room, owner, room_key_receipt_text( room, room_key_mark( key ) ) );
   }
   catch( e )
   {
   }
}

// NOTE: Whether the open room shows this person's plain note to "person" saying the room is encrypted ("keyless_note_text( )").
function chat_keyless_note_shown( person )
{
   return chat_keyless_note_uniques( person ).length > 0;
}

// NOTE: Those notes' uniques.
function chat_keyless_note_uniques( person )
{
   var text = keyless_note_text( ciyam.username );
   var uniques = [ ];

   document.querySelectorAll( "#message_list .chat-message-body" ).forEach( function( body )
   {
      var row = body.closest( ".chat-message" );

      if( ( body.dataset.sender === ciyam.username ) && ( body.dataset.private === "1" ) && ( body.dataset.text === text )
       && ( ( body.dataset.recipients || "" ).split( "," ).indexOf( person ) >= 0 ) && row && row.dataset.unique )
         uniques.push( row.dataset.unique );
   } );

   return uniques;
}

// NOTE: A private message in "room" to "person" alone, as it is - nothing to seal it with.
async function chat_send_plain_note( room, person, text )
{
   await serialised( function( )
   {
      return ciyam.create_message( room, "for=" + with_sender( [ person ], ciyam.username ).join( "," ) + ";text=" + escape_message_text( text ),
       function( ) { } );
   } );
}

// NOTE: A private message in "room" to "person" alone (and the sender), sealed with the secret the two share - how a room key is
// passed to a member, and its receipt sent back (Damon, 2026-11-05: as a folded note, rather than in a direct message that has to be
// accepted first; since 2026-11-08 in the reserved room, "c_key_notes_room"). Posted "for=~<person>" (Ian, 2026-11-06), so the node
// names both ("_<sender>_<person>_") and the console opens it as a direct message's; the sender gets a copy, and only they may
// delete it. "sent", "no_key" (nothing to seal it with) or "failed".
async function chat_send_key_note( room, person, text )
{
   var sealing = await chat_seal_for( person, text );

   if( !sealing.sealed || ( sealing.text === "" ) )
      return "no_key";

   var answer = null;

   await serialised( function( )
   {
      return ciyam.create_message( room, "for=" + paired_for( person ) + ";text=" + sealing.text,
       function( response ) { answer = String( response ); } );
   } );

   return ( ( answer === null ) || is_timeout_response( answer ) || is_error_response( answer ) ) ? "failed" : "sent";
}

// ---- The owner's side (Damon, 2026-11-03: a key held for a new direct message waited in the owner's page until it was accepted -
// "keep this page open" - and was lost when the owner signed out first, so those invited had the room unencrypted)

// NOTE: For each room and member, their direct message as it was last looked through - "<room>#<key>|<dm>/<total>" - so it is
// read again only once something new is there, or the key has changed.
var g_owner_keys_checked = { };

// NOTE: Each owned room's members as the owner's chat last found them (Damon, 2026-11-05) - "has" the key, the key "passed" and not
// yet accepted, "needs_codes" (no contact code to seal it with), "failed" - said in Room Details ("member_key_state_text( )").
var g_owner_key_states = { };
var g_owner_keys_sending = false;

// NOTE: Where a room key's notes and their receipts go (Ian, 2026-11-08: a sealed key is nearly the largest message, so a new room's
// invitation could not carry one for each invited) - each its own "~" note in the reserved room, which everyone is in, as the key
// names its room. So it is passed as soon as the owner has someone's contact code, joined or not.
const c_key_notes_room = c_reserved_room;

// NOTE: The reserved room's total in the room list - what moves as a note or a receipt arrives there.
function chat_key_notes_total( )
{
   var entry = find_room( c_key_notes_room );

   return entry ? String( entry.total ) : "";
}

// NOTE: The owner's open room, if it has a key here - each member, and each of "invited" (not yet joined, perhaps), sent the key
// unless the owner's own sealed notes already have it. Nothing kept anywhere, so whichever of the owner's browsers opens the room does
// it. As the room is first read, once the keys are read, as someone joins it, and as people are invited. Said only when something was
// sent.
async function chat_owner_send_keys( quiet, invited )
{
   var room = g_room;
   var entry = ( room !== "" ) ? find_room( room ) : null;
   var key = chat_room_key( room );

   if( !entry || ( entry.owner !== ciyam.username ) || !g_chat_keys || g_chat_keys_unread || ( key === "" )
    || ( g_room_key_busy && !quiet ) || g_owner_keys_sending )
      return "";

   var read = g_chat_keys_read;
   var whole = room_key_text( room, key );
   var members = g_members.map( function( member ) { return member.name; } );
   var people = room_key_recipients( members.concat( invited || [ ] ), ciyam.username );
   var notes = null;
   var outcomes = [ ];

   if( !g_owner_key_states.hasOwnProperty( room ) )
      g_owner_key_states[ room ] = { };

   var states = g_owner_key_states[ room ];

   g_owner_keys_sending = true;

   try
   {
      for( var i = 0; i < people.length; i++ )
      {
         var person = people[ i ];
         var secret = chat_secret_with( person );

         // NOTE: Without their contact code there is nothing to seal it with - said, and tried again as the room changes.
         if( secret === "" )
         {
            // NOTE: Looked at again as an invited person joins - the note below is only for those in it.
            var joined = ( members.indexOf( person ) >= 0 );
            var no_key = joined ? "no_key" : "no_key_invited";

            if( g_owner_keys_checked[ room + "|" + person ] !== no_key )
            {
               outcomes.push( { name: person, how: "no_key" } );

               // NOTE: Told, in a plain note to them alone, that the room is encrypted and what the key needs (Damon, 2026-11-05: a
               // member who joins after the last encrypted message sees none, so nothing else would say so) - once, if not there.
               if( joined && !chat_keyless_note_shown( person ) )
                  await chat_send_plain_note( room, person, keyless_note_text( ciyam.username ) );
            }

            g_owner_keys_checked[ room + "|" + person ] = no_key;
            states[ person ] = "needs_codes";

            continue;
         }

         // NOTE: Their contact code here now - the plain note that said it was needed deleted, as no longer so (Ian, 2026-11-07:
         // members with the code still read it).
         var stale = chat_keyless_note_uniques( person ).filter( function( unique ) { return !g_keyless_deleted[ unique ]; } );

         stale.forEach( function( unique ) { g_keyless_deleted[ unique ] = true; } );

         if( stale.length > 0 )
            await chat_delete_key_messages( room, stale );

         var marker = whole + "|" + room + "/" + entry.total + "|" + chat_key_notes_total( );

         if( g_owner_keys_checked[ room + "|" + person ] === marker )
            continue;

         // NOTE: The reserved room read for the notes to them and their receipt - once for everyone ("c_key_notes_room").
         if( notes === null )
            notes = await chat_room_read_quietly( c_key_notes_room );

         if( ( read !== g_chat_keys_read ) || ( g_room !== room ) || ( chat_room_key( room ) !== key ) )
            return "";

         if( notes === null )
            continue;

         var found = await chat_sealed_opened( notes.messages, [ ciyam.username, person ], secret );

         g_owner_keys_checked[ room + "|" + person ] = marker;

         var delivery = owner_key_delivery( found.opened, ciyam.username, person, room, key, room_key_mark( key ) );

         // NOTE: Their receipt for it there (Ian's idea, Damon 2026-11-05) - the messages that carried a key for the room deleted, so
         // it is not left in the conversation; not sent again, as the receipt stays. Looked at again once the conversation changes.
         if( delivery.state === "received" )
         {
            states[ person ] = "has";

            await chat_delete_key_messages( c_key_notes_room, delivery.remove );

            continue;
         }

         if( delivery.state === "sent" )
         {
            states[ person ] = "passed";

            continue;
         }

         var how = await chat_send_key_note( c_key_notes_room, person, whole );

         if( read !== g_chat_keys_read )
            return "";

         states[ person ] = ( how === "sent" ) ? "passed" : ( ( how === "no_key" ) ? "needs_codes" : "failed" );

         // NOTE: Not sent - tried again the next time this is called, as if never looked at (found by review, 2026-11-04).
         if( how === "failed" )
            delete g_owner_keys_checked[ room + "|" + person ];

         outcomes.push( { name: person, how: how } );
      }
   }
   finally
   {
      g_owner_keys_sending = false;
   }

   // NOTE: Who has gone - out of the room's states, as no longer members.
   Object.keys( states ).forEach( function( person ) { if( people.indexOf( person ) < 0 ) delete states[ person ]; } );

   if( g_room === room )
      chat_render_room_key( );

   // NOTE: Forced - the rail's own signature does not hold the members' states.
   render_rooms( true );

   var text = ( outcomes.length > 0 ) ? room_key_sent_text( outcomes ) : "";

   if( !quiet && ( text !== "" ) )
      chat_room_key_sent( room, text );

   return text;
}

// NOTE: For each owned room with a key here and not open, as "chat_owner_away_marker( )" was when last looked through
// ("chat_owner_send_keys_away( )").
var g_owner_away_checked = { };
var g_owner_away_running = false;

// NOTE: For each of those rooms, its members as last read - { total, names } - so a room is read again only once it has changed, not
// each time a note or receipt arrives in the reserved room.
var g_owner_away_members = { };

// NOTE: A room as last looked through - its own total, and the reserved room's, where its notes and receipts are.
function chat_owner_away_marker( entry )
{
   return entry.room + "/" + entry.total + "|" + chat_key_notes_total( );
}

// NOTE: The owner's other rooms with a key here - each looked through once it or the reserved room has changed, and those who have
// joined passed the key as "chat_owner_send_keys( )" does for the open room (Damon, 2026-11-07: Ian, in one browser, made a poll and
// signed out, and its voters, joining later, were never given its key - the owner never opened the room again). Damon accepting that
// a room looked through is marked read. A member without a contact code here is left to the open room's look, which tells them
// plainly. Called as the room list is read; quietly, one room at a time.
async function chat_owner_send_keys_away( )
{
   if( !g_chat_keys || g_chat_keys_unread || g_owner_away_running || ( ciyam.username === "" ) )
      return;

   var read = g_chat_keys_read;
   var due = g_rooms.filter( function( entry )
   {
      return ( entry.owner === ciyam.username ) && ( entry.room !== g_room ) && ( chat_room_key( entry.room ) !== "" )
       && can_have_room_key( entry.room, entry.name ) && ( g_owner_away_checked[ entry.room ] !== chat_owner_away_marker( entry ) );
   } );

   if( due.length === 0 )
      return;

   g_owner_away_running = true;

   try
   {
      // NOTE: The notes and receipts for all of them in one read.
      var notes = await chat_room_read_quietly( c_key_notes_room );

      if( notes === null )
         return;

      // NOTE: Those without a contact code here, said together once the rooms are looked through.
      var needed = [ ];

      for( var i = 0; i < due.length; i++ )
      {
         if( read !== g_chat_keys_read )
            return;

         await chat_owner_send_keys_in( due[ i ], notes, needed );
      }

      if( needed.length > 0 )
         show_alert( owner_codes_needed_text( needed ), "is-info" );
   }
   finally
   {
      g_owner_away_running = false;
   }

   // NOTE: Forced - the rail's own signature does not hold the members' states.
   render_rooms( true );
}

async function chat_owner_send_keys_in( entry, notes, needed )
{
   var room = entry.room;
   var key = chat_room_key( room );
   var whole = room_key_text( room, key );
   var read = g_chat_keys_read;
   var known = g_owner_away_members[ room ];
   var names = ( known && ( known.total === entry.total ) ) ? known.names : null;

   g_owner_away_checked[ room ] = chat_owner_away_marker( entry );

   if( names === null )
   {
      var result = await chat_room_read_quietly( room );

      if( result === null )
      {
         delete g_owner_away_checked[ room ];

         return;
      }

      names = result.members.map( function( member ) { return member.name; } );

      g_owner_away_members[ room ] = { total: entry.total, names: names };
   }

   if( ( read !== g_chat_keys_read ) || ( chat_room_key( room ) !== key ) || ( g_room === room ) )
      return;

   var people = room_key_recipients( names, ciyam.username );

   if( !g_owner_key_states.hasOwnProperty( room ) )
      g_owner_key_states[ room ] = { };

   var states = g_owner_key_states[ room ];

   for( var i = 0; i < people.length; i++ )
   {
      var person = people[ i ];
      var secret = chat_secret_with( person );

      if( secret === "" )
      {
         states[ person ] = "needs_codes";

         // NOTE: Said once a sign in for each (Damon, 2026-11-08: the owner had no prompt to exchange them as someone joined).
         if( !g_owner_codes_told[ room + "|" + person ] )
         {
            g_owner_codes_told[ room + "|" + person ] = true;

            needed.push( { person: person, room_name: entry.name } );
         }

         continue;
      }

      var found = await chat_sealed_opened( notes.messages, [ ciyam.username, person ], secret );

      if( ( read !== g_chat_keys_read ) || ( chat_room_key( room ) !== key ) )
         return;

      var delivery = owner_key_delivery( found.opened, ciyam.username, person, room, key, room_key_mark( key ) );

      if( delivery.state === "received" )
      {
         states[ person ] = "has";

         await chat_delete_key_messages( c_key_notes_room, delivery.remove );
      }
      else if( delivery.state === "sent" )
         states[ person ] = "passed";
      else
      {
         var how = await chat_send_key_note( c_key_notes_room, person, whole );

         states[ person ] = ( how === "sent" ) ? "passed" : ( ( how === "no_key" ) ? "needs_codes" : "failed" );

         // NOTE: Not sent - the room looked through again at the next reading of the room list.
         if( how === "failed" )
            delete g_owner_away_checked[ room ];
      }
   }

   Object.keys( states ).forEach( function( person ) { if( people.indexOf( person ) < 0 ) delete states[ person ]; } );
}

// NOTE: The owner's own messages that carried a room key, deleted from a member's direct message once they have the key - one at a
// time, as a message's delete is. One that fails is tried again when the conversation is next looked at.
async function chat_delete_key_messages( dm_room, uniques )
{
   for( var i = 0; i < uniques.length; i++ )
   {
      var unique = uniques[ i ];

      await serialised( function( )
      {
         // NOTE: Private - the notes are to the one member alone (Damon, 2026-11-05), so deleted as a private message is edited.
         return ciyam.create_message( dm_room, delete_options( unique, true ), function( ) { } );
      } );
   }
}

// NOTE: A room this person is invited to and has not joined yet, as a room - whoever invited them taken as its owner. A group's key
// is sent as it is started, so it usually arrives before the group is joined; only rooms already joined counted, and it was never
// added (Damon, 2026-10-27).
function chat_invited_room( room )
{
   var invite = g_invitations.filter( function( entry ) { return entry.room === room; } )[ 0 ];

   return invite ? { room: invite.room, name: invite.name, owner: invite.inviter } : null;
}

function chat_offer_room_key( element, text )
{
   var shared = room_key_in_message( text );
   var entry = shared ? ( find_room( shared.room ) || chat_invited_room( shared.room ) ) : null;

   if( !entry || !g_chat_keys || !can_have_room_key( shared.room, entry.name ) )
      return;

   var button = document.createElement( "button" );

   button.type = "button";
   button.className = "chat-btn chat-btn--small chat-room-key-offer";

   element.appendChild( button );

   var kept = g_chat_keys.rooms[ shared.room ] || "";

   if( kept === shared.key )
   {
      button.textContent = "Room key for " + entry.name + " added";
      button.disabled = true;

      return;
   }

   var offered = ( kept !== "" ) ? "Use this room key for " + entry.name : "Add this room key for " + entry.name;
   var whole = shared.room + "#" + shared.key;
   // NOTE: The owner's by the conversation it was opened in, not by the sender the server names (found by review, 2026-11-06: the
   // node could name the owner as the sender of a key another sealed, and have it added unasked).
   var from_owner = ( ( element.dataset.sender || "" ) === entry.owner ) && ( chat_dm_other( ) === entry.owner );

   var save = function( only_if_none )
   {
      button.disabled = true;
      button.textContent = "Adding the room key for " + entry.name + "…";

      chat_keys_save_room( shared.room, shared.key, only_if_none ).then( function( problem )
      {
         if( problem === "kept" )
         {
            button.disabled = false;
            button.textContent = "Use this room key for " + entry.name;
         }
         else if( problem !== "" )
         {
            button.disabled = false;
            button.textContent = offered;

            show_alert( problem, "is-error" );
         }
         else
         {
            chat_room_key_removed( whole, false );

            if( from_owner )
               chat_send_room_key_receipt( entry.owner, shared.room, shared.key );
         }
      } );
   };

   button.textContent = offered;

   button.addEventListener( "click", async function( )
   {
      var now = g_chat_keys ? ( g_chat_keys.rooms[ shared.room ] || "" ) : "";

      if( ( now !== "" ) && ( now !== shared.key ) && !await ask_dialog( { title: "Use this key for " + entry.name + "?",
       text: "You already have a key for it. Your messages there will be encrypted with this one - only those who have it can read"
       + " them.", yes: "Use this key", danger: true } ) )
         return;

      save( false );
   } );

   if( ( kept === "" ) && from_owner && !g_room_keys_tried.hasOwnProperty( whole ) && !chat_room_key_was_removed( whole ) )
   {
      g_room_keys_tried[ whole ] = true;

      save( true );
   }
}

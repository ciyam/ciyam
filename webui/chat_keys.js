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
   g_room_keys_tried = { };
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

function chat_draw_opened( element, opened )
{
   element.classList.toggle( "chat-unreadable", !opened.ok );
   element.classList.toggle( "chat-locked", !opened.ok );
   element.classList.toggle( "chat-encrypted", opened.ok );

   // NOTE: A room key in it cut short - the button under it says whether it was added ("room_key_masked( )", Ian, 2026-10-27).
   if( opened.ok )
      draw_formatted( element, format_pieces( room_key_masked( opened.text ) ) );
   else
      element.textContent = "Encrypted message - not readable with the keys you have";

   // NOTE: One's own is offered to edit once opened here, and not otherwise - nor taken by Up (found by review, 2026-10-30).
   var row = element.closest( ".chat-message" );
   var edit = row ? row.querySelector( ".chat-message-edit" ) : null;

   if( ( edit !== null ) && ( edit.dataset.sealed === "1" ) )
      edit.hidden = !opened.ok;

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

// NOTE: The room the Room key part was last drawn for - a change of room puts away what was open there, the dialog too.
var g_room_key_drawn_for = "";
var g_room_key_busy = false;

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
      state = "Not encrypted - messages here are sent as they are. If the owner has a room key, they can send it to you.";

   part.hidden = ( state === "" );

   document.getElementById( "room_key_state" ).textContent = state;
   document.getElementById( "room_key_dialog_state" ).textContent = state;
   document.getElementById( "room_key_open" ).hidden = !g_chat_keys;

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

   [ "room_key_make", "room_key_add_open", "room_key_add_go", "room_key_send", "room_key_share", "room_key_remove" ].forEach( function( id )
   {
      document.getElementById( id ).disabled = g_room_key_busy;
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

   document.getElementById( "room_key_open" ).focus( );
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

      if( await ask_dialog( { title: "Encrypt this conversation?", text: "A key is made for it and sent to each of its people in an"
       + " encrypted direct message - one is started with anyone you have none with yet.", yes: "Encrypt it" } ) && ( g_room === room ) )
         chat_encrypt_group( room, people );

      return;
   }

   if( !await ask_dialog( { title: "Make a key for this room?", text: "From now on your messages here are encrypted - members can"
    + " read them once you've sent them the key with Send to members.", yes: "Make the key" } ) || ( g_room !== room ) )
      return;

   if( await chat_room_key_change( room, chat_random_room_key( ) ) && ( g_room === room ) )
      do_share_room_key( );
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
   if( ( chat_room_key( room ) === "" ) && !( await chat_room_key_change( room, chat_random_room_key( ) ) ) )
      return;

   g_room_key_busy = true;

   chat_render_room_key( );

   var text = await chat_send_room_key( room, people );

   g_room_key_busy = false;

   chat_render_room_key( );

   if( text !== "" )
      chat_room_key_sent( room, text );
}

// NOTE: Said in the Room key dialog, and in the alert after "lead" (what was done, an invitation say) - "Room key:" there, as the
// alert is seen with the dialog shut.
function chat_room_key_sent( room, text, lead )
{
   if( g_room === room )
      document.getElementById( "room_key_sent" ).textContent = text;

   show_alert( ( lead ? lead + " " : "" ) + "Room key: " + text, "is-info" );
}

// NOTE: Every new room keyed (Ian, 2026-10-28) - a key made as it is created and sent to each invited in an encrypted direct message,
// as a group's is ("chat_encrypt_group( )"). Without keys of one's own the room is made as it was, Room Details saying why it is
// not encrypted.
async function chat_key_new_room( room, invitees )
{
   var entry = find_room( room );

   if( !g_chat_keys || g_chat_keys_unread || !entry || !can_have_room_key( room, entry.name ) || ( chat_room_key( room ) !== "" ) )
      return;

   if( invitees.length > 0 )
      await chat_encrypt_group( room, invitees );
   else
      await chat_room_key_change( room, chat_random_room_key( ) );
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

   if( !await ask_dialog( { title: "Send the key to each member?", text: "In an encrypted direct message - one is started with anyone"
    + " you have none with yet.", yes: "Send it" } ) || ( g_room !== room ) )
      return;

   g_room_key_busy = true;

   chat_render_room_key( );

   var text = await chat_send_room_key( room, chat_room_people( ) );

   g_room_key_busy = false;

   chat_render_room_key( );

   if( text !== "" )
      chat_room_key_sent( room, text );
}

// NOTE: "room"'s key sent to each of "people" in an encrypted direct message - the one with them, or one started for it and sent
// once they accept. Never in the clear: anyone without a public key here is left out. What came of it ("room_key_sent_text( )").
async function chat_send_room_key( room, people )
{
   var key = chat_room_key( room );

   if( key === "" )
      return "";

   var text = room_key_text( room, key );
   var outcomes = [ ];
   var started = false;

   // NOTE: Signed out (or in again) meanwhile - nothing more sent, and nothing of it said over the next sign in (found by review).
   var read = g_chat_keys_read;

   var recipients = room_key_recipients( people, ciyam.username ).filter( function( person ) { return dm_allowed( dm_me( ), person ); } );

   for( var i = 0; i < recipients.length; i++ )
   {
      var how = await chat_send_room_key_to( recipients[ i ], text );

      if( read !== g_chat_keys_read )
         return "";

      if( how === "started" )
      {
         started = true;
         how = "held";
      }

      outcomes.push( { name: recipients[ i ], how: how } );
   }

   // NOTE: The rail knows the conversations started for it - a held one is watched only once it is listed ("watch_held_rooms( )").
   if( started )
      await load_rooms( );

   return room_key_sent_text( outcomes );
}

async function chat_send_room_key_to( person, text )
{
   if( chat_secret_with( person ) === "" )
      return "no_key";

   var existing = dm_existing( [ person ], ciyam.username, g_rooms, g_invitations, dm_waiting_rooms( ) );

   if( existing.kind === "request" )
      return "request";

   if( existing.kind === "waiting" )
   {
      g_dm_pending[ existing.room ].texts.push( text );

      return "held";
   }

   if( existing.kind === "" )
   {
      var dm = await start_dm( [ person ] );

      // NOTE: Not started, or signed out meanwhile (no room) - "chat_send_room_key( )" then says nothing.
      if( ( dm.problem !== "" ) || ( dm.room === "" ) )
         return "failed";

      g_dm_pending[ dm.room ].texts.push( text );

      return "started";
   }

   // NOTE: They declined it - nothing sent there reaches them.
   if( g_dm_shut[ existing.room ] )
      return "failed";

   var sealing = await chat_seal_for( person, text );

   if( !sealing.sealed || ( sealing.text === "" ) )
      return "failed";

   var answer = null;

   await serialised( function( )
   {
      return ciyam.create_message( existing.room, "text=" + sealing.text, function( response ) { answer = String( response ); } );
   } );

   return ( ( answer === null ) || is_timeout_response( answer ) || is_error_response( answer ) ) ? "failed" : "sent";
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
   var room = g_room;

   if( !await ask_dialog( { title: "Remove this room's key?", text: "Messages sealed with it can't be read here any more, and yours"
    + " are sent unencrypted - unless it's added again.", yes: "Remove it", danger: true } ) || ( g_room !== room ) )
      return;

   var removed = chat_room_key( room );

   if( await chat_room_key_change( room, "" ) && ( removed !== "" ) )
      chat_room_key_removed( room + "#" + removed, true );
}

// NOTE: The room keys removed on this browser, "<room>#<key>" - not added again by themselves when the direct message that
// brought one is seen again (found by review, 2026-10-27); added by hand, one is forgotten from here. Kept for the account on
// this browser, as the notifications switch is.
function chat_room_keys_removed_name( )
{
   return "cws.room_keys_removed_" + ciyam.access;
}

function chat_room_keys_removed( )
{
   try
   {
      var kept = JSON.parse( localStorage.getItem( chat_room_keys_removed_name( ) ) || "[]" );

      return Array.isArray( kept ) ? kept : [ ];
   }
   catch( e )
   {
      return [ ];
   }
}

function chat_room_key_removed( shared, removed )
{
   var kept = chat_room_keys_removed( ).filter( function( entry ) { return entry !== shared; } );

   if( removed )
      kept.push( shared );

   try
   {
      localStorage.setItem( chat_room_keys_removed_name( ), JSON.stringify( kept ) );
   }
   catch( e )
   {
   }
}

// NOTE: Under a message in a direct message that starts with a room key ("room_key_in_message( )") for a room this person is in.
// Added as it arrives (Ian, 2026-10-26: no button to press, copying and pasting being painful) when it comes from the room's owner,
// as the console's owner sends it, none is kept for the room, and it has not been removed here; otherwise offered. One in place of a
// key already kept is asked first: whoever sent it could read what is sent there with it, and members with the old key could not
// (found by review). Each is added by itself only once a sign in, so a save that fails is not tried again at every redraw.
var g_room_keys_tried = { };

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
            chat_room_key_removed( whole, false );
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

   var from_owner = ( element.dataset.sender || "" ) === entry.owner;

   if( ( kept === "" ) && from_owner && !g_room_keys_tried.hasOwnProperty( whole ) && ( chat_room_keys_removed( ).indexOf( whole ) < 0 ) )
   {
      g_room_keys_tried[ whole ] = true;

      save( true );
   }
}

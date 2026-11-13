// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Your keys, on My account (Ian, 2026-10-23 and 2026-10-25). An account's keys come from twelve words of its own - made
// from popped bubbles and the browser's randomness, as a node's are, written down and some checked back. The words go nowhere:
// from them come the keys, kept on the node ("user-keys") with others' public keys, locked under an access key - many rounds of
// a hash of the words - which is what each browser keeps ("ciyam_user_keys.js"; Ian: a hash "won't help anyone trying to crack
// the actual twelve words"). Another browser gets the access key from this one by Ian's exchange of two codes ("keys_ask_start( )",
// "keys_device_answer( )"), or makes it from the words typed.
// It is kept while the account is remembered here, for the tab only when it is not, and goes with "Reset this browser" or the
// account forgotten at a sign in.
//
// What the card needs is read in the first time it is shown - "bip39.min.js" only when words are made or typed
// ("recover_load_bip39( )" in "signin.js"), being large.
const c_keys_scripts = [ "ed25519.min.js", "ciyam_harden.js", "ciyam_chacha20.js", "ciyam_ed25519.js", "ciyam_secure_message.js",
 "ciyam_user_keys.js", "home_parse.js", "bubbles.js", "qrcode.min.js",
 "qr_scan.js" ];

const c_keys_parts = [ "keys_loading", "keys_none", "keys_random", "keys_words", "keys_check", "keys_enter", "keys_ask", "keys_here" ];

const c_keys_pops = 12;
const c_keys_most_bubbles = 16;
const c_keys_entropy_length = 32;
const c_keys_qr_size = 120;
const c_keys_device_qr_size = 168;

// NOTE: An access key brought by another browser - opened from its answer ("keys_device_link_taken( )") - held for the tab until this
// account's keys are known.
const c_keys_pending_name = c_signin_pending_keys_name;

// NOTE: The address that carried the access key itself, before Ian's exchange (2026-11-05) - taken out of the address, not used.
const c_keys_old_link_prefix = "#keys=";

// NOTE: Another device's code, or the answer to this one's, brought by its address ("#keys-ask=" or "#keys-reply=" - the part after
// "#" never goes to the server) - held for the tab until the keys are read.
const c_keys_device_link_name = "cws.device_link";

// NOTE: This device's half of the exchange - its one-off private key, kept until the answer is opened (perhaps in another tab, as the
// camera opens an address there) and for a quarter of an hour at most. In memory too, for a browser that keeps nothing.
const c_keys_ask_name = "cws.device_ask";
const c_keys_ask_life = 15 * 60 * 1000;
var g_keys_ask = null;

// NOTE: What browsers kept before 2026-10-25's change - the entropy itself. Removed wherever it is found.
const c_keys_old_prefix = "cws.user_entropy_";

var g_keys = null;
var g_keys_bubbles = null;
var g_keys_scripts = null;
var g_keys_installed = false;

// NOTE: The twelve words field's typing help ("words_entry.js"), once installed.
var g_keys_words_entry = null;

// NOTE: Each showing of the card counted - My account can be shown again before the last has finished, and only the latest acts.
var g_keys_render = 0;

function fresh_keys( )
{
   return { samples: [ ], pops: 0, entropy: "", words: [ ], checking: [ ], right: 0, node: "", access: "", rounds: 0,
    primary: "", secondary: "", contacts: { }, rooms: { }, people: "", own: "", entering: "", asked_from: "" };
}

function keys_load_script( src )
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

// NOTE: In order, once - each may use those before it. Tried again next time if one could not be read.
function keys_load_scripts( )
{
   if( g_keys_scripts === null )
   {
      g_keys_scripts = c_keys_scripts.reduce( function( chain, src )
      {
         return chain.then( function( ok ) { return ok && keys_load_script( src ); } );
      }, Promise.resolve( true ) ).then( function( ok )
      {
         if( !ok )
            g_keys_scripts = null;

         return ok;
      } );
   }

   return g_keys_scripts;
}

function keys_module( name )
{
   return ciyam_module_named( name );
}

// ---- What this browser keeps

function keys_storage_name( )
{
   return c_signin_storage_keys_prefix + ciyam.access;
}

function keys_kept_here( )
{
   try
   {
      localStorage.removeItem( c_keys_old_prefix + ciyam.access );
      sessionStorage.removeItem( c_keys_old_prefix + ciyam.access );

      return localStorage.getItem( keys_storage_name( ) ) || sessionStorage.getItem( keys_storage_name( ) ) || "";
   }
   catch( e )
   {
      return "";
   }
}

// NOTE: For as long as the account is remembered on this browser - otherwise for the tab only.
function keys_keep_here( access )
{
   try
   {
      var remembered = ( parse_access_list( localStorage.getItem( c_storage_access ) ).indexOf( ciyam.access ) >= 0 );

      ( remembered ? localStorage : sessionStorage ).setItem( keys_storage_name( ), access );
      ( remembered ? sessionStorage : localStorage ).removeItem( keys_storage_name( ) );
   }
   catch( e )
   {
   }
}

function keys_forget_here( )
{
   try
   {
      localStorage.removeItem( keys_storage_name( ) );
      sessionStorage.removeItem( keys_storage_name( ) );
   }
   catch( e )
   {
   }
}

// NOTE: From "account_main( )", before anything else - an access key in the address taken out of it at once, so it is not left
// in the address bar, and held for the tab until the keys are shown.
function keys_take_link( )
{
   // NOTE: Give keys, from People - who to is held for the tab ("keys_give_taken( )"); the address is Your keys'.
   if( location.hash === "#give" )
   {
      history.replaceState( null, "", location.pathname + location.search + "#keys" );

      return true;
   }

   var device = parse_device_link( location.hash );

   if( ( device === null ) && ( location.hash.indexOf( c_keys_old_link_prefix ) !== 0 ) )
      return false;

   try
   {
      if( device !== null )
         sessionStorage.setItem( c_keys_device_link_name, location.hash.substring( 1 ) );
   }
   catch( e )
   {
   }

   // NOTE: Another device's code is answered under Devices, where Add another device is; an answer opens Your keys.
   history.replaceState( null, "", location.pathname + location.search + ( ( device && ( device.kind === "ask" ) ) ? "#devices" : "#keys" ) );

   return true;
}

// NOTE: The device address held for the tab, taken - { kind, ... } as "parse_device_link( )" reads it, or null.
function keys_device_link_taken( )
{
   try
   {
      var held = sessionStorage.getItem( c_keys_device_link_name ) || "";

      sessionStorage.removeItem( c_keys_device_link_name );

      return parse_device_link( held );
   }
   catch( e )
   {
      return null;
   }
}

// NOTE: Taken only once the node's answer could be read (found by review, 2026-10-26) - after a time-out or an error it is
// kept for the next showing of the card, rather than the QR code needing scanning again.
function keys_pending( take )
{
   try
   {
      var access = sessionStorage.getItem( c_keys_pending_name ) || "";

      if( take )
         sessionStorage.removeItem( c_keys_pending_name );

      return access;
   }
   catch( e )
   {
      return "";
   }
}

// ---- The node's copy

function keys_url( extra )
{
   return ciyam.get_cws_url( ) + "/user-keys?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text" + ( extra || "" )
    + "&session=" + ciyam.sessid;
}

// NOTE: The guard's line of the keys the node has (Ian, 2026-10-27) - "" for keys without one.
function keys_guard_now( )
{
   try
   {
      return keys_module( "user_keys" ).parts_of( g_keys.node ).guard;
   }
   catch( e )
   {
      return "";
   }
}

// NOTE: Locked under the access key and sent, with the guard's line before them ("guard" - made from the words, or the one the
// node has) - false, with the error said, if the node did not take them.
async function keys_send( access, rounds, contents, error_id, what, guard )
{
   var response = "";
   var locked = "";

   try
   {
      var uk = keys_module( "user_keys" );

      locked = uk.kept_text( guard, await uk.lock( access, contents, { rounds: rounds } ) );

      response = await request( function( done )
      {
         return ciyam.fetch_in_body( keys_url( "&payload=" + encodeURIComponent( locked ) ), "PUT", done );
      } );
   }
   catch( e )
   {
      response = "Error: " + ( e && e.message ? e.message : "they could not be locked." );
   }

   if( problem_text( response ) !== "" )
   {
      set_error( error_id, what + " could not be saved: " + problem_text( response ) );

      return false;
   }

   g_keys.node = locked;

   return true;
}

// ---- Showing it

function keys_show( part )
{
   c_keys_parts.forEach( function( id ) { document.getElementById( id ).hidden = ( id !== part ); } );

   // NOTE: Contacts and Add another device have sections of their own (2026-10-26) - each needs the keys on this browser, and
   // says where to put them there until they are; nothing while they are still being looked for.
   var here = ( part === "keys_here" );
   var looking = ( part === "keys_loading" );

   document.getElementById( "keys_rooms_part" ).hidden = !here;
   document.getElementById( "contacts_part" ).hidden = !here;
   document.getElementById( "contacts_add_part" ).hidden = !here;
   document.getElementById( "contacts_need_keys" ).hidden = here || looking;
   document.getElementById( "device_add_part" ).hidden = !here;
   document.getElementById( "device_need_keys" ).hidden = here || looking;
   document.getElementById( "give_part" ).hidden = !here || ( g_keys_give === null );

   // NOTE: The last step of joining, and Profile's reminder ("account.js").
   if( typeof onboard_draw === "function" )
      onboard_draw( part );
}

// NOTE: Words shown, and perhaps written down, but not yet saved - for the switcher's warning on leaving ("unsent_work( )").
function keys_unsent( )
{
   return [ "keys_words", "keys_check" ].some( function( id )
   {
      var shown = document.getElementById( id );

      return ( shown !== null ) && !shown.hidden;
   } );
}

// NOTE: While the keys are made or opened, nothing that would start it again can be pressed, and the words stay as typed (Ian,
// 2026-10-31: they seemed lost meanwhile).
function keys_busy( text )
{
   document.getElementById( "keys_busy" ).hidden = !text;
   document.getElementById( "keys_busy_text" ).textContent = text || "";

   [ "keys_enter_words", "keys_enter_go", "keys_enter_new", "keys_enter_cancel", "keys_check_back" ].forEach( function( id )
   {
      document.getElementById( id ).disabled = !!text;
   } );

   document.getElementById( "keys_save" ).disabled = !!text || ( g_keys.right < g_keys.checking.length );
}

// NOTE: A frame drawn before the hardening - the page's SHA-256 runs its rounds without pausing, so nothing said before it would
// show until it ends (on a phone, some seconds).
function keys_drawn( )
{
   return new Promise( function( resolve )
   {
      requestAnimationFrame( function( ) { setTimeout( resolve, 0 ); } );

      setTimeout( resolve, 200 );
   } );
}

// NOTE: A change to Contacts, said under it - its section's own (2026-10-26).
function keys_contacts_busy( text )
{
   document.getElementById( "contacts_busy" ).hidden = !text;
   document.getElementById( "contacts_busy_text" ).textContent = text || "";
}

// NOTE: Words and keys gone from the page - after saving, cancelling or signing out.
function keys_clear( )
{
   if( g_keys_bubbles !== null )
      g_keys_bubbles.clear( );

   g_keys = fresh_keys( );

   [ "keys_word_list", "keys_check_rows", "keys_qr", "keys_contact_list", "keys_device_qr", "keys_ask_qr" ].forEach( function( id )
   {
      document.getElementById( id ).replaceChildren( );
   } );

   [ "keys_enter_words", "keys_add_key", "keys_device_link", "keys_device_ask", "keys_ask_link", "keys_ask_reply" ].forEach( function( id )
   {
      document.getElementById( id ).value = "";
   } );

   keys_scan_stop( );

   document.getElementById( "keys_written" ).checked = false;
   keys_hide_public( );
   document.getElementById( "keys_device_part" ).hidden = true;
   document.getElementById( "keys_device_row" ).hidden = false;
   document.getElementById( "keys_rooms" ).textContent = "";

   set_error( "device_error", "" );
   document.getElementById( "keys_room_list" ).replaceChildren( );

   g_keys_give = null;
   keys_give_reset( );
   document.getElementById( "give_part" ).hidden = true;

   g_keys_room_names = null;
   g_keys_room_names_reading = false;
   g_keys_room_names_reads++;

   keys_busy( "" );
   keys_contacts_busy( "" );
   set_error( "keys_error", "" );
   set_error( "keys_add_error", "" );
}

// NOTE: From My account - which part, from this browser and the node ("keys_state( )" in "account_parse.js").
async function render_keys( )
{
   install_keys( );
   keys_clear( );

   // NOTE: Said plainly (Damon and Ian, 2026-11-11) - the node's twelve words make everyone's keys here.
   document.getElementById( "keys_words_lede" ).textContent = keys_words_text( ciyam.is_admin );
   document.getElementById( "keys_words_lede" ).hidden = ( keys_words_text( ciyam.is_admin ) === "" );
   keys_show( "keys_loading" );

   var render = ++g_keys_render;
   var asked_for = ciyam.access;

   if( !await keys_load_scripts( ) )
   {
      if( render !== g_keys_render )
         return;

      keys_show( null );
      set_error( "keys_error", "What your keys need could not be read - reload the page to try again." );

      return;
   }

   var response = await request( function( done ) { return ciyam.fetch( keys_url( "" ), "GET", done ); } );

   // NOTE: Signed out, another account in, or shown again since, while it was asked - not this one's to show.
   if( ( ciyam.access === "" ) || ( ciyam.access !== asked_for ) || ( render !== g_keys_render ) )
      return;

   // NOTE: Another device's code to answer, once the keys are shown here - or the answer to this one's, opened into an access key.
   var device = keys_device_link_taken( );
   var unopened = false;

   // NOTE: The administrator's answer, opened into this person's entropy - its keys made or opened once the node's are known.
   var given = "";

   if( device && ( device.kind === "given" ) )
   {
      given = await keys_given_open( device );

      if( ( ciyam.access !== asked_for ) || ( render !== g_keys_render ) )
         return;

      unopened = ( given === "" );
   }

   if( device && ( device.kind === "reply" ) )
   {
      var brought = await keys_ask_open( device );

      if( ( ciyam.access !== asked_for ) || ( render !== g_keys_render ) )
         return;

      if( brought !== "" )
      {
         try
         {
            sessionStorage.setItem( c_keys_pending_name, brought );
         }
         catch( e )
         {
         }
      }
      else
         unopened = true;
   }

   var pending = keys_pending( false );
   var here = keys_kept_here( );
   var state = keys_state( ( here !== "" ) || ( pending !== "" ), response );

   if( state !== "unreadable" )
      keys_pending( true );

   g_keys.node = String( response ).trim( );

   if( state === "unreadable" )
   {
      keys_show( null );
      set_error( "keys_error", "Your keys could not be read: " + ( problem_text( response ) || "the node keeps something that is not locked keys." ) );
   }
   else if( ( state === "none" ) || ( state === "lost" ) )
   {
      keys_forget_here( );

      // NOTE: Admin is the node (Ian, 2026-11-11) - its keys come only from the node's twelve words; anyone wanting words of their
      // own uses an account of their own.
      var node_only = ciyam.is_admin;

      if( node_only )
         document.getElementById( "keys_none_lede" ).textContent = ( state === "lost" )
          ? "Your keys are no longer on the node. Type the node's twelve words to make them again."
          : "As admin, your keys come from the node's twelve words - admin is the node. Anyone wanting words of their own uses"
           + " their own account.";
      else
         document.getElementById( "keys_none_lede" ).textContent = ( state === "lost" )
          ? "Your keys are no longer on the node, and this browser can't make them again by itself." : "";

      // NOTE: Everyone else - their administrator's, then words of their own; not the node's twelve words, which make everyone's keys
      // here and are admin's to keep (Damon, 2026-11-14).
      document.getElementById( "keys_none_admin" ).hidden = !node_only;
      document.getElementById( "keys_none_person" ).hidden = node_only;
      document.getElementById( "keys_none_lede" ).hidden = ( document.getElementById( "keys_none_lede" ).textContent === "" );

      keys_show( "keys_none" );
   }
   else if( pending !== "" )
      await keys_show_here( pending, "Your keys are now on this browser too.", true );
   else if( state === "elsewhere" )
      keys_show_enter( "elsewhere" );
   else
      await keys_show_here( here, "Your keys are on this browser.", false );

   if( render !== g_keys_render )
      return;

   if( given !== "" )
      await keys_take_given( given );
   else if( unopened && device && ( device.kind === "given" ) )
      set_error( "keys_error", "That code isn't your administrator's answer to this device's - it is for another device or someone"
       + " else, or was used already, or is more than a quarter of an hour old. Get them from your administrator again." );
   else if( unopened )
      set_error( "keys_error", "That code isn't the answer to this device's - it is for another device, or was used already, or is more"
       + " than a quarter of an hour old. Get them from another device again." );

   if( render !== g_keys_render )
      return;

   // NOTE: Someone's code for Get them from your administrator, opened here by its address (found by review, 2026-11-11: nothing was
   // said) - answered only from Give keys beside their name in People.
   if( device && ( device.kind === "give" ) )
      set_error( "keys_error", ciyam.is_admin ? "That's someone's code asking for their keys - choose Give keys beside their name in"
       + " People, then scan it there." : "That code is for your administrator to scan - in People, with Give keys." );

   // NOTE: Give keys, from People - only admin's, once admin's own keys are here (the people key is kept with them).
   var give = keys_give_taken( );

   if( ( give !== null ) && ciyam.is_admin )
   {
      if( !document.getElementById( "keys_here" ).hidden )
         keys_show_give( give );
      else
         set_error( "keys_error", "To give " + give.name + " their keys, your own need to be on this browser first - use the node's"
          + " twelve words, or your own, here." );
   }

   // NOTE: Another device's code opened here is put in Add another device and answered only on "Use it", under its warning (found by
   // review, 2026-11-06: answered at once, anyone who got this browser to open their code's address, and to show them what came back,
   // would have had the keys).
   if( device && ( device.kind === "ask" ) )
   {
      if( !document.getElementById( "keys_here" ).hidden )
      {
         keys_show_device( );

         document.getElementById( "keys_device_ask" ).value = device_ask_link( location.origin + location.pathname, device.key );
         document.getElementById( "keys_device_use" ).focus( );
      }
      else
         set_error( "keys_error", "Another device asked for your keys, but they aren't on this browser to give - type your twelve words"
          + " here first." );
   }
}

// NOTE: The words asked for - "elsewhere": on a browser new to the keys; "other": what this browser kept does not open the
// node's (set up again from another browser, or a new access key, say); "restore": nothing on the node, the keys made again
// from the words; "change": a new access key (Ian, 2026-10-27: a round fewer each time); "node": nothing on the node, the keys made
// from the node's own twelve words and the PIN (Ian, 2026-11-11). Guarded keys (Ian, 2026-10-27) the node replaces only from the
// words they were set up with, so new words are not offered for them - the admin clears them. Words typed to open keys or change
// the access key may be the person's own or the node's - whichever the keys were made from.
function keys_show_enter( mode )
{
   var made = ( mode === "restore" ) || ( mode === "node" );
   var guarded = ( keys_guard_now( ) !== "" ) && !made;
   var lost = guarded ? " Lost them? Ask the administrator to clear your keys, then set up new ones." : "";

   g_keys.entering = mode;

   document.getElementById( "keys_enter_lede" ).textContent = {
      elsewhere: "Your keys are set up. To use them on this browser, type your twelve words - or get them from another device"
       + " that has them." + lost,
      other: "The keys on the node were locked by another browser since. Type the twelve words they were set up with." + lost,
      // NOTE: One's own - not offered to admin, whose keys come from the node's (Ian, 2026-11-11); and only admin is offered those
      // (Damon, 2026-11-14).
      restore: "Type the twelve words you set your keys up with, and they're made again from them.",
      change: "Type your twelve words for a new access key. Use it if a browser or phone with your keys is lost: any other browser"
       + " that has them will then need your words, or to get them from this one, again.",
      node: "Type the twelve words the node was set up with. Your keys are made from them and your PIN, so whoever has the node's"
       + " words can make them again for you." }[ mode ];

   document.getElementById( "keys_enter_label" ).textContent = ( mode === "node" ) ? "The node's twelve words" : "Your twelve words";
   document.getElementById( "keys_enter_new" ).hidden = made || ( mode === "change" ) || guarded || ciyam.is_admin;
   document.getElementById( "keys_ask_start" ).hidden = ( mode !== "elsewhere" ) && ( mode !== "other" );
   document.getElementById( "keys_enter_admin" ).hidden = ( ( mode !== "elsewhere" ) && ( mode !== "other" ) ) || ciyam.is_admin;
   document.getElementById( "keys_enter_cancel" ).hidden = !made && ( mode !== "change" );
   document.getElementById( "keys_enter_go" ).textContent = ( mode === "change" ) ? "Change the access key" : "Use my keys here";
   document.getElementById( "keys_enter_words" ).value = "";

   keys_show( "keys_enter" );

   // NOTE: The word list for the help in typing them - drawn again once it is in, and the field the right size now it shows.
   recover_load_bip39( ).then( function( ) { if( g_keys_words_entry ) g_keys_words_entry.draw( ); } );

   if( g_keys_words_entry )
      g_keys_words_entry.draw( );
}

// NOTE: The keys opened with "access" and shown - or, if it does not open them, the words asked for. "from_link" - brought by
// another browser's QR code, and kept only once it has opened them.
async function keys_show_here( access, lede, from_link )
{
   var opened = null;

   try
   {
      opened = await keys_module( "user_keys" ).unlock( access, g_keys.node );
   }
   catch( e )
   {
      if( from_link )
      {
         keys_show_enter( "elsewhere" );
         set_error( "keys_error", "Those keys aren't this account's - was the other device signed in as someone else?" );
      }
      else
      {
         keys_forget_here( );
         keys_show_enter( "other" );
      }

      return;
   }

   if( from_link )
      keys_keep_here( access );

   var sm = keys_module( "secure_message" );
   var key = sm.public_key_of_seed( opened.primary );

   keys_hide_public( );

   g_keys.access = access;
   g_keys.rounds = opened.rounds;
   g_keys.primary = opened.primary;
   g_keys.secondary = opened.secondary;
   g_keys.own = key;
   g_keys.contacts = opened.contacts;
   g_keys.rooms = opened.rooms;
   g_keys.people = opened.people;

   document.getElementById( "keys_here_lede" ).textContent = lede;

   keys_draw_contacts( );
   keys_draw_rooms( );

   keys_show( "keys_here" );
}

// ---- Setting them up

function keys_bubbles( )
{
   if( g_keys_bubbles === null )
   {
      g_keys_bubbles = bubble_field( document.getElementById( "keys_bubbles" ), { state: function( ) { return g_keys; },
       pops: c_keys_pops, most: c_keys_most_bubbles, on_count: keys_count_pops,
       on_done: function( ) { document.getElementById( "keys_random_next" ).focus( ); } } );
   }

   return g_keys_bubbles;
}

function keys_count_pops( )
{
   var pops = g_keys ? g_keys.pops : 0;

   document.getElementById( "keys_popped" ).textContent = pops + " of " + c_keys_pops + " popped" + ( pops >= c_keys_pops ? " - thank you" : "" );
   document.getElementById( "keys_popped_bar" ).style.width = Math.round( pops / c_keys_pops * 100 ) + "%";
   document.getElementById( "keys_random_next" ).disabled = ( pops < c_keys_pops );
}

function keys_start( )
{
   var node = g_keys.node;

   keys_clear( );

   g_keys.node = node;

   keys_count_pops( );
   keys_show( "keys_random" );

   keys_bubbles( ).start( );
}

function keys_cancel( )
{
   render_keys( );
}

async function keys_make_words( )
{
   set_error( "keys_error", "" );

   if( !await recover_load_bip39( ) )
   {
      set_error( "keys_error", "The word list could not be read - reload the page to try again." );

      return;
   }

   var bytes = new Uint8Array( 32 );

   window.crypto.getRandomValues( bytes );

   var browser_hex = Array.from( bytes, function( b ) { return b.toString( 16 ).padStart( 2, "0" ); } ).join( "" );

   keys_bubbles( ).stop( );

   g_keys.entropy = randomness_hex( g_keys.samples, browser_hex, hex_sha256, c_keys_entropy_length );
   g_keys.words = keys_module( "user_keys" ).words_of_entropy( g_keys.entropy ).split( " " );

   var list = document.getElementById( "keys_word_list" );

   list.replaceChildren( );

   g_keys.words.forEach( function( word, i )
   {
      var item = document.createElement( "li" );
      var number = document.createElement( "span" );
      var text = document.createElement( "b" );

      number.textContent = String( i + 1 );
      text.textContent = word;

      item.appendChild( number );
      item.appendChild( text );
      list.appendChild( item );
   } );

   document.getElementById( "keys_written" ).checked = false;
   document.getElementById( "keys_words_next" ).disabled = true;

   keys_show( "keys_words" );
}

// NOTE: Three of the twelve, each from three choices (Home's "word_choices( )") - never another of the person's words.
function keys_draw_check( )
{
   var rows = document.getElementById( "keys_check_rows" );
   var list = recover_wordlist( );

   rows.replaceChildren( );

   g_keys.checking = keys_check_indexes( Math.random, 3 );
   g_keys.right = 0;

   g_keys.checking.forEach( function( i )
   {
      var word = g_keys.words[ i ];
      var row = document.createElement( "div" );
      var label = document.createElement( "span" );
      var note = document.createElement( "p" );

      row.className = "home-setup-check-row";
      label.className = "home-setup-check-label";
      label.textContent = "Word " + ( i + 1 );
      note.className = "home-setup-check-note";
      note.hidden = true;

      row.appendChild( label );

      word_choices( g_keys.words, i, list, Math.random ).forEach( function( choice )
      {
         var button = document.createElement( "button" );

         button.type = "button";
         button.className = "home-setup-choice";
         button.textContent = choice;

         button.addEventListener( "click", function( ) { keys_pick( row, note, button, choice === word, i ); } );

         row.appendChild( button );
      } );

      rows.appendChild( row );
      rows.appendChild( note );
   } );

   document.getElementById( "keys_save" ).disabled = true;

   keys_show( "keys_check" );
}

function keys_pick( row, note, button, is_right, index )
{
   row.querySelectorAll( ".home-setup-choice" ).forEach( function( other ) { other.classList.remove( "is-wrong" ); } );

   if( is_right )
   {
      button.classList.add( "is-right" );

      row.querySelectorAll( ".home-setup-choice" ).forEach( function( other ) { other.disabled = true; } );

      note.hidden = true;

      g_keys.right++;

      document.getElementById( "keys_save" ).disabled = ( g_keys.right < g_keys.checking.length );
   }
   else
   {
      button.classList.add( "is-wrong" );

      note.textContent = "Not that one - look at word " + ( index + 1 ) + " on your paper again.";
      note.hidden = false;
   }
}

// NOTE: The keys made from "entropy", locked under its access key and sent - kept here once the node has them. No one else's
// public keys yet: new keys, or made again when the node had lost them.
async function keys_make_and_save( entropy, lede )
{
   set_error( "keys_error", "" );
   keys_busy( "Securing your keys…" );

   await keys_drawn( );

   var uk = keys_module( "user_keys" );
   var rounds = uk.c_default_rounds;
   var access = "";
   var guard = "";
   var seeds = null;

   try
   {
      access = await uk.access_key( entropy, rounds );
      guard = await uk.guard( entropy, rounds );
      seeds = uk.seeds_of( entropy );
   }
   catch( e )
   {
      keys_busy( "" );
      set_error( "keys_error", "Your keys could not be made: " + ( e && e.message ? e.message : "reload the page to try again." ) );

      return;
   }

   keys_busy( "Saving your keys…" );

   var saved = await keys_send( access, rounds, { primary: seeds.primary, secondary: seeds.secondary, contacts: { } }, "keys_error", "Your keys",
    guard );

   keys_busy( "" );

   if( !saved )
      return;

   keys_keep_here( access );

   var node = g_keys.node;

   keys_clear( );

   g_keys.node = node;

   await keys_show_here( access, lede, false );
}

// ---- Adding another device

// NOTE: Ian's exchange (2026-10-26; Damon, 2026-11-05 - in place of an address that carried the access key itself): the new device
// shows the public key of a pair it makes for this once; this one, which has the keys, makes a pair of its own and shows its public
// key with the access key sealed under the X25519 secret between the two ("device_reply_link( )"). Both private keys are thrown away
// once used, so a photo of either code opens nothing; nothing goes to the node. Sealed as a one-time pad - an HMAC of the secret,
// which is never used again - with a check, so a wrong or damaged answer is said rather than taken.
const c_keys_device_pad = "ciyam device access key";
const c_keys_device_check = "ciyam device access check ";

function keys_draw_qr( id, text )
{
   var box = document.getElementById( id );

   box.replaceChildren( );

   new QRCode( box, { text: text, width: c_keys_device_qr_size, height: c_keys_device_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
    correctLevel: QRCode.CorrectLevel.M } );

   box.removeAttribute( "title" );
   box.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );
}

// NOTE: 32 random bytes as hex - a one-off private key's seed.
function keys_random_seed( )
{
   var bytes = new Uint8Array( 32 );

   crypto.getRandomValues( bytes );

   return Array.prototype.map.call( bytes, function( b ) { return ( "0" + b.toString( 16 ) ).slice( -2 ); } ).join( "" );
}

async function keys_device_seal( secret, access )
{
   var sm = keys_module( "secure_message" );

   return { sealed: xor_hex( access, await sm.hmac_sha256( secret, c_keys_device_pad ) ),
    check: ( await sm.hmac_sha256( secret, c_keys_device_check + access ) ).substr( 0, 16 ) };
}

// ---- The device that has the keys - Add another device, under Devices

function keys_show_device( )
{
   keys_hide_device( );

   document.getElementById( "keys_device_part" ).hidden = false;
   document.getElementById( "keys_device_row" ).hidden = true;
   document.getElementById( "keys_device_ask" ).focus( );
}

function keys_hide_device( )
{
   if( g_keys_scan_target === "device" )
      keys_scan_stop( );

   document.getElementById( "keys_device_qr" ).replaceChildren( );
   document.getElementById( "keys_device_link" ).value = "";
   document.getElementById( "keys_device_ask" ).value = "";
   document.getElementById( "keys_device_ask_part" ).hidden = false;
   document.getElementById( "keys_device_reply_part" ).hidden = true;
   document.getElementById( "keys_device_part" ).hidden = true;
   document.getElementById( "keys_device_row" ).hidden = false;
   document.getElementById( "device_scan_status" ).textContent = "";

   set_error( "device_error", "" );
}

// NOTE: The other device's code, read - its address, scanned or pasted ("parse_device_link( )"); its answer shown for it alone.
async function keys_device_answer( device )
{
   set_error( "device_error", "" );

   if( !device || ( device.kind !== "ask" ) )
   {
      set_error( "device_error", "That isn't another device's code - on it, choose Get them from another device under Your keys." );

      return;
   }

   if( g_keys.access === "" )
      return;

   var sm = keys_module( "secure_message" );
   var seed = keys_random_seed( );
   var secret = "";

   try
   {
      secret = sm.shared_secret_of_seed( seed, device.key );
   }
   catch( e )
   {
      set_error( "device_error", "That code could not be read - scan it again, or paste its address." );

      return;
   }

   var sealing = await keys_device_seal( secret, g_keys.access );
   var link = device_reply_link( location.origin + location.pathname, sm.public_key_of_seed( seed ), sealing.sealed, sealing.check );

   seed = "";
   secret = "";

   keys_draw_qr( "keys_device_qr", link );

   document.getElementById( "keys_device_link" ).value = link;
   document.getElementById( "keys_device_ask_part" ).hidden = true;
   document.getElementById( "keys_device_reply_part" ).hidden = false;
}

function keys_device_use( )
{
   keys_device_answer( parse_device_link( document.getElementById( "keys_device_ask" ).value ) );
}

// ---- The new device - Get them from another device, under Your keys

function keys_ask_kept( )
{
   if( ( g_keys_ask !== null ) && ( Date.now( ) - g_keys_ask.at < c_keys_ask_life ) )
      return g_keys_ask;

   try
   {
      var kept = JSON.parse( localStorage.getItem( c_keys_ask_name ) || "null" );

      if( kept && /^[0-9a-f]{64}$/.test( kept.seed ) && ( Date.now( ) - kept.at < c_keys_ask_life ) )
         return kept;

      // NOTE: Too old, or not one - not left behind (found by review, 2026-11-06).
      if( kept !== null )
         localStorage.removeItem( c_keys_ask_name );
   }
   catch( e )
   {
   }

   return null;
}

function keys_ask_forget( )
{
   g_keys_ask = null;

   try
   {
      localStorage.removeItem( c_keys_ask_name );
   }
   catch( e )
   {
   }
}

// NOTE: "kind" "give" asks the administrator (Give keys in People), anything else - a click - another device of one's own.
function keys_ask_start( kind )
{
   var sm = keys_module( "secure_message" );
   var giving = ( kind === "give" );

   g_keys_ask = { seed: keys_random_seed( ), at: Date.now( ), kind: giving ? "give" : "device" };

   try
   {
      localStorage.setItem( c_keys_ask_name, JSON.stringify( g_keys_ask ) );
   }
   catch( e )
   {
   }

   var page = location.origin + location.pathname;
   var link = giving ? device_give_link( page, sm.public_key_of_seed( g_keys_ask.seed ) ) : device_ask_link( page, sm.public_key_of_seed( g_keys_ask.seed ) );

   keys_draw_qr( "keys_ask_qr", link );

   document.getElementById( "keys_ask_lede" ).textContent = giving ? "Show this code to your administrator. In People, they choose Give"
    + " keys beside your name and scan it. They show a code back: open that with this device's camera, scan it here, or paste its"
    + " address." : "On a device that has your keys, open this code with its camera - or open the address there, signed in as you. It"
    + " shows a code back: open that with this device's camera, scan it here, or paste its address.";
   document.getElementById( "keys_ask_reply_label" ).textContent = giving ? "The address your administrator shows back" : "The address it shows back";

   document.getElementById( "keys_ask_link" ).value = link;
   document.getElementById( "keys_ask_reply" ).value = "";
   document.getElementById( "ask_scan_status" ).textContent = "";

   set_error( "keys_error", "" );
   keys_show( "keys_ask" );
}

function keys_ask_cancel( )
{
   keys_scan_stop( );
   keys_ask_forget( );

   document.getElementById( "keys_ask_qr" ).replaceChildren( );
   document.getElementById( "keys_ask_link" ).value = "";

   set_error( "keys_error", "" );

   if( g_keys.asked_from === "keys_none" )
      keys_show( "keys_none" );
   else
      keys_show_enter( g_keys.entering || "elsewhere" );
}

// NOTE: The answer opened with this device's one-off key - the access key, or "" if it is not the answer to this device's code (or
// the code has gone: used, or too old). The one-off key is thrown away once it has opened one.
async function keys_ask_open( device )
{
   var kept = keys_ask_kept( );

   if( ( kept === null ) || ( kept.kind === "give" ) || !device || ( device.kind !== "reply" ) || !await keys_load_scripts( ) )
      return "";

   var sm = keys_module( "secure_message" );
   var access = "";

   try
   {
      var secret = sm.shared_secret_of_seed( kept.seed, device.key );

      access = xor_hex( device.sealed, await sm.hmac_sha256( secret, c_keys_device_pad ) );

      // NOTE: Sealed again as the other device sealed it - the same check only if this is what it sent, to this device.
      if( ( ( await keys_device_seal( secret, access ) ).check !== device.check ) || !/^[0-9a-f]{64}$/.test( access ) )
         access = "";
   }
   catch( e )
   {
      access = "";
   }

   if( access !== "" )
      keys_ask_forget( );

   return access;
}

// NOTE: The answer scanned or pasted here - opened, and the keys shown as from another device's address.
async function keys_ask_use( text )
{
   var device = parse_device_link( ( typeof text === "string" ) ? text : document.getElementById( "keys_ask_reply" ).value );
   var kept = keys_ask_kept( );

   set_error( "keys_error", "" );

   if( kept && ( kept.kind === "give" ) )
   {
      if( !device || ( device.kind !== "given" ) )
      {
         set_error( "keys_error", "That isn't your administrator's answer - they show one once they have read this device's code." );

         return;
      }

      var entropy = await keys_given_open( device );

      if( entropy === "" )
      {
         set_error( "keys_error", "That answer isn't for this device's code, or is for someone else - ask your administrator to scan the"
          + " code shown here again." );

         return;
      }

      keys_scan_stop( );

      await keys_take_given( entropy );

      return;
   }

   if( !device || ( device.kind !== "reply" ) )
   {
      set_error( "keys_error", "That isn't the other device's answer - it shows one once it has read this device's code." );

      return;
   }

   var access = await keys_ask_open( device );

   if( access === "" )
   {
      set_error( "keys_error", "That answer isn't for this device's code - scan the code shown here on the other device again." );

      return;
   }

   keys_scan_stop( );

   await keys_show_here( access, "Your keys are now on this browser too.", true );
}

// ---- Keys from the administrator - Give keys, from People

// NOTE: The same exchange (Damon, 2026-11-11), the person's entropy sealed in place of an access key - 32 hex digits, the pad cut to
// that - with the check made with their username too, so an answer for someone else is refused on their device, not taken.
const c_keys_give_pad = "ciyam give person entropy";
const c_keys_give_check = "ciyam give person check ";

// NOTE: Who admin is giving keys to - { pin, name } - while Give keys shows.
var g_keys_give = null;

async function keys_give_seal( secret, entropy, name )
{
   var sm = keys_module( "secure_message" );

   return { sealed: xor_hex( entropy, ( await sm.hmac_sha256( secret, c_keys_give_pad ) ).substr( 0, 32 ) ),
    check: ( await sm.hmac_sha256( secret, c_keys_give_check + entropy + ":" + name ) ).substr( 0, 16 ) };
}

// NOTE: The administrator's answer opened with this device's one-off key - this person's entropy, or "" if it is not the answer to
// this device's code from Get them from your administrator, or is for someone else.
async function keys_given_open( device )
{
   var kept = keys_ask_kept( );

   if( ( kept === null ) || ( kept.kind !== "give" ) || !device || ( device.kind !== "given" ) || !await keys_load_scripts( ) )
      return "";

   var sm = keys_module( "secure_message" );
   var entropy = "";

   try
   {
      var secret = sm.shared_secret_of_seed( kept.seed, device.key );

      entropy = xor_hex( device.sealed, ( await sm.hmac_sha256( secret, c_keys_give_pad ) ).substr( 0, 32 ) );

      if( ( ( await keys_give_seal( secret, entropy, ciyam.username ) ).check !== device.check ) || !/^[0-9a-f]{32}$/.test( entropy ) )
         entropy = "";
   }
   catch( e )
   {
      entropy = "";
   }

   if( entropy !== "" )
      keys_ask_forget( );

   return entropy;
}

// NOTE: The entropy the administrator gave - the keys made from it and saved if the node has none; opened if it has them and they
// were made from it (from the node's words, say); refused if they came from twelve words of the person's own, which no one else
// can give.
async function keys_take_given( entropy )
{
   var uk = keys_module( "user_keys" );
   var lede = "Your keys are from your administrator, and on this browser.";
   var rounds = 0;

   try
   {
      rounds = uk.rounds_of( g_keys.node );
   }
   catch( e )
   {
      rounds = 0;
   }

   if( rounds === 0 )
   {
      await keys_make_and_save( entropy, lede );

      return;
   }

   keys_busy( "Opening your keys…" );

   await keys_drawn( );

   var access = "";

   try
   {
      access = await uk.access_key( entropy, rounds );

      await uk.unlock( access, g_keys.node );
   }
   catch( e )
   {
      access = "";
   }

   keys_busy( "" );

   if( access === "" )
   {
      keys_show_enter( "elsewhere" );
      set_error( "keys_error", "Your keys weren't made from the node's words - from twelve words of your own, it seems - so your"
       + " administrator can't give them. Type your own words here." );

      return;
   }

   keys_keep_here( access );

   await keys_show_here( access, lede, false );
}

// NOTE: Who admin chose Give keys for in People ("do_give_keys( )" in "home.js") - { pin, name }, taken once; null if none, or if
// it was for another account's session.
function keys_give_taken( )
{
   try
   {
      var held = JSON.parse( sessionStorage.getItem( c_signin_give_keys_name ) || "null" );

      sessionStorage.removeItem( c_signin_give_keys_name );

      if( held && ( held.for === ciyam.access ) && /^[0-9]{5}$/.test( String( held.pin ) ) && ( String( held.name || "" ) !== "" ) )
         return { pin: String( held.pin ), name: String( held.name ) };
   }
   catch( e )
   {
   }

   return null;
}

// NOTE: Give keys' parts and where each says what it lacks ("give_field_problems( )") - the poll dialog's pattern (2026-11-04).
const c_keys_give_parts = [
   { key: "password", error: "give_password_error", field: "give_password" },
   { key: "words", error: "give_words_error", field: "give_words" },
   { key: "ask", error: "give_ask_error", field: "give_ask" } ];

function keys_give_problem( key, said )
{
   c_keys_give_parts.forEach( function( part )
   {
      if( part.key !== key )
         return;

      document.getElementById( part.error ).textContent = said;

      if( said )
         document.getElementById( part.field ).setAttribute( "aria-invalid", "true" );
      else
         document.getElementById( part.field ).removeAttribute( "aria-invalid" );
   } );
}

// NOTE: Each part's problem said under it, and the first focused - true when there are none; null clears them.
function keys_give_problems( problems )
{
   var first = null;

   c_keys_give_parts.forEach( function( part )
   {
      var said = problems ? ( problems[ part.key ] || "" ) : "";

      keys_give_problem( part.key, said );

      if( said && ( first === null ) )
         first = part.field;
   } );

   if( first !== null )
      document.getElementById( first ).focus( );

   return first === null;
}

function keys_give_busy( text )
{
   document.getElementById( "give_busy" ).hidden = !text;
   document.getElementById( "give_busy_text" ).textContent = text || "";

   [ "give_password", "give_words", "give_ask", "give_go", "give_scan", "give_cancel" ].forEach( function( id )
   {
      document.getElementById( id ).disabled = !!text;
   } );
}

// NOTE: The password, the words and both codes gone from the page - after giving, cancelling or signing out.
function keys_give_reset( )
{
   if( g_keys_scan_target === "give" )
      keys_scan_stop( );

   [ "give_password", "give_words", "give_ask", "give_link" ].forEach( function( id ) { document.getElementById( id ).value = ""; } );

   document.getElementById( "give_qr" ).replaceChildren( );
   document.getElementById( "give_scan_status" ).textContent = "";
   document.getElementById( "give_ask_part" ).hidden = false;
   document.getElementById( "give_reply_part" ).hidden = true;

   keys_give_problems( null );
   set_error( "give_error", "" );
   keys_give_busy( "" );
}

function keys_show_give( give )
{
   keys_give_reset( );

   g_keys_give = give;

   document.getElementById( "give_heading" ).textContent = "Give " + give.name + " their keys";
   document.getElementById( "give_lede" ).textContent = "On " + give.name + "'s device, signed in as them, they choose Get them from your"
    + " administrator under Your keys. Scan the code it shows, or paste its address. Their keys are made from the node's twelve words"
    + " and their PIN.";

   // NOTE: The node's words only until the people key is kept with admin's keys - then the password opens it.
   document.getElementById( "give_words_part" ).hidden = ( g_keys.people !== "" );
   // NOTE: Locked under the password typed - which the page cannot check against the sign in's (found by review, 2026-11-11), so a
   // mistyped one is only found next time, when the words are asked again.
   document.getElementById( "give_words_note" ).textContent = "Asked only this once. What they make is then kept with your keys,"
    + " locked under the password typed here - so type the one you sign in with.";
   document.getElementById( "give_part" ).hidden = false;
   document.getElementById( "give_password" ).focus( );
}

// NOTE: Back to People, where Give keys was chosen.
function keys_give_close( )
{
   keys_give_reset( );

   g_keys_give = null;
   document.getElementById( "give_part" ).hidden = true;

   if( typeof apps_go === "function" )
      apps_go( "home", "#people", null );
}

// NOTE: The people key - opened with the password from admin's keys, or made from the node's words typed (and then kept, sealed
// under the password) - or "" with the problem said. The words are taken as the node's only if they make admin's own keys, so
// wrong ones are said rather than used to make keys no one else could make (admin's keys come from "Use the node's twelve words").
async function keys_give_people_key( password, latest )
{
   var uk = keys_module( "user_keys" );
   var words_part = document.getElementById( "give_words_part" );

   if( latest.people !== "" )
   {
      try
      {
         return await uk.open_people_key( latest.people, ciyam.access, password );
      }
      catch( e )
      {
         // NOTE: A password changed since, say - the words make it again, and it is kept under the new password.
         if( document.getElementById( "give_words" ).value.trim( ) === "" )
         {
            words_part.hidden = false;
            keys_give_problems( { password: "That password doesn't open what's kept with your keys. If you've changed your password since,"
             + " type the node's twelve words too.", words: "", ask: "" } );

            return "";
         }
      }
   }

   // NOTE: Nothing kept after all (gone since the card was shown, say) - the words asked for, where they can be typed.
   if( document.getElementById( "give_words" ).value.trim( ) === "" )
   {
      words_part.hidden = false;
      keys_give_problems( { password: "", words: "Type the node's twelve words.", ask: "" } );

      return "";
   }

   if( !await recover_load_bip39( ) )
   {
      set_error( "give_error", "The word list could not be read - reload the page to try again." );

      return "";
   }

   var people = "";

   try
   {
      people = await uk.people_key( uk.entropy_of_words( document.getElementById( "give_words" ).value ) );
   }
   catch( e )
   {
      keys_give_problems( { password: "", words: e.message, ask: "" } );

      return "";
   }

   if( uk.seeds_of( await uk.person_entropy( people, uk.c_admin_person ) ).primary !== latest.primary )
   {
      keys_give_problems( { password: "", words: "Those aren't the words your own keys were made from. Yours need to come from the"
       + " node's twelve words - Use the node's twelve words, under Your keys.", ask: "" } );

      return "";
   }

   var sealed = await uk.seal_people_key( people, ciyam.access, password );

   if( !await keys_send( g_keys.access, g_keys.rounds, { primary: latest.primary, secondary: latest.secondary, contacts: latest.contacts,
    rooms: latest.rooms, people: sealed }, "give_error", "What the node's words make", latest.guard ) )
      return "";

   g_keys.people = sealed;
   words_part.hidden = true;

   return people;
}

// NOTE: Their device's code read - the people key opened, their entropy made from it and their PIN, and sealed for that device alone
// as the answer. Nothing of it goes to the node.
async function keys_give_go( )
{
   var give = g_keys_give;

   set_error( "give_error", "" );

   if( ( give === null ) || ( g_keys.access === "" ) )
      return;

   var password = document.getElementById( "give_password" ).value;
   // NOTE: The words needed only while nothing is kept - with it kept, only if the password does not open it.
   var asking_words = ( g_keys.people === "" );
   var device = parse_device_link( document.getElementById( "give_ask" ).value );

   if( !keys_give_problems( give_field_problems( password, asking_words ? document.getElementById( "give_words" ).value : null, device ) ) )
      return;

   keys_give_busy( "Opening what gives keys…" );

   await keys_drawn( );

   var latest = await keys_latest( );
   var people = "";

   if( latest === null )
      set_error( "give_error", "Your keys could not be read again from the node - reload the page to try again." );
   else
      people = await keys_give_people_key( password, latest );

   if( ( people === "" ) || ( g_keys_give !== give ) )
   {
      keys_give_busy( "" );

      return;
   }

   var uk = keys_module( "user_keys" );
   var sm = keys_module( "secure_message" );
   var seed = keys_random_seed( );
   var link = "";

   try
   {
      var secret = sm.shared_secret_of_seed( seed, device.key );
      var sealing = await keys_give_seal( secret, await uk.person_entropy( people, give.pin ), give.name );

      link = device_given_link( location.origin + location.pathname, sm.public_key_of_seed( seed ), sealing.sealed, sealing.check );
   }
   catch( e )
   {
      link = "";
   }

   seed = "";
   people = "";

   keys_give_busy( "" );

   if( link === "" )
   {
      keys_give_problems( { password: "", words: "", ask: "That code could not be read - scan it again, or paste its address." } );

      return;
   }

   document.getElementById( "give_password" ).value = "";
   document.getElementById( "give_words" ).value = "";

   keys_draw_qr( "give_qr", link );

   document.getElementById( "give_link" ).value = link;
   document.getElementById( "give_reply_lede" ).textContent = "Now open this with " + give.name + "'s camera, or scan or paste it there."
    + " It works only on that device, for " + give.name + ", and only once.";
   document.getElementById( "give_ask_part" ).hidden = true;
   document.getElementById( "give_reply_part" ).hidden = false;
   document.getElementById( "give_done" ).focus( );
}

// ---- Others' public keys

function keys_draw_contacts( )
{
   var list = document.getElementById( "keys_contact_list" );
   var rows = contact_rows( g_keys.contacts );

   list.replaceChildren( );

   rows.forEach( function( row )
   {
      var item = document.createElement( "li" );
      var name = document.createElement( "b" );
      var key = document.createElement( "code" );
      var remove = document.createElement( "button" );

      name.textContent = row.name;
      key.textContent = row.short;
      key.title = row.key;

      remove.type = "button";
      remove.className = "chat-btn chat-btn--small";
      remove.textContent = "Remove";
      remove.setAttribute( "aria-label", "Remove " + row.name + "'s public key" );
      remove.addEventListener( "click", function( ) { keys_remove_contact( row.name ); } );

      item.appendChild( name );
      item.appendChild( key );
      item.appendChild( remove );
      list.appendChild( item );
   } );

   document.getElementById( "keys_contacts" ).textContent = ( rows.length === 0 )
    ? "None yet. Add someone's to write to them encrypted - they'll find theirs on their My account." : "";
   document.getElementById( "keys_contacts" ).hidden = ( rows.length > 0 );
}

// NOTE: Your contact code - your username and public key ("contact_code( )") - as text and a QR code, only when asked (Ian,
// 2026-10-27: with quantum attacks to come, shown only when it is known to be safe; 2026-11-03: shown on adding a contact, for
// each to scan the other's) - and taken out of the page again on Done, or on leaving Contacts.
function keys_show_public( )
{
   if( !g_keys || !g_keys.own )
      return;

   var bytes = new Uint8Array( 13 );

   crypto.getRandomValues( bytes );

   var padding = Array.from( bytes ).map( function( b ) { return b.toString( 16 ).padStart( 2, "0" ); } ).join( "" );
   var code = contact_code( account_display_name( ciyam.username, ciyam.is_admin ), g_keys.own, padding );

   if( code === "" )
      return;

   var box = document.getElementById( "keys_qr" );

   document.getElementById( "keys_public" ).textContent = code;

   // NOTE: Drawn in the page - "qrcode.min.js" - so the key goes nowhere else; the library's own title taken off, as Home's.
   box.replaceChildren( );

   new QRCode( box, { text: code, width: c_keys_qr_size, height: c_keys_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
    correctLevel: QRCode.CorrectLevel.M } );

   box.removeAttribute( "title" );
   box.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );

   set_error( "keys_add_error", "" );
   keys_scan_status( "" );

   document.getElementById( "keys_public_part" ).hidden = false;
   document.getElementById( "contacts_start_row" ).hidden = true;
   document.getElementById( "keys_add_key" ).focus( );
}

function keys_hide_public( )
{
   keys_scan_stop( );

   document.getElementById( "keys_qr" ).replaceChildren( );
   document.getElementById( "keys_public" ).textContent = "";
   document.getElementById( "keys_add_key" ).value = "";
   document.getElementById( "keys_public_part" ).hidden = true;
   document.getElementById( "contacts_start_row" ).hidden = false;

   keys_scan_status( "" );
   set_error( "keys_add_error", "" );
}

// NOTE: Their contact code read by the camera, or from a picture of it where there is no camera or no https ("qr_scan.js") -
// added once read. Each start counted, as Done or leaving can come while the camera is still starting.
var g_keys_scanner = null;
var g_keys_scan_opened = 0;

// NOTE: Where the camera reads for - a contact's code, the other device's code (Add another device), or its answer (Get them from
// another device) - one at a time; its reader, status line and button by id, and what to say while it looks.
const c_keys_scan_targets = {
   contacts: { reader: "contacts_scan_reader", status: "contacts_scan_status", button: "contacts_scan", error: "keys_add_error",
    hint: "Hold their contact code up to the camera." },
   device: { reader: "device_scan_reader", status: "device_scan_status", button: "device_scan", error: "device_error",
    hint: "Hold the other device's code up to the camera." },
   ask: { reader: "ask_scan_reader", status: "ask_scan_status", button: "ask_scan", error: "keys_error",
    hint: "Hold the code the other device shows up to the camera." },
   give: { reader: "give_scan_reader", status: "give_scan_status", button: "give_scan", error: "give_ask_error",
    hint: "Hold their device's code up to the camera." } };

var g_keys_scan_target = "contacts";

function keys_scan_status( text )
{
   document.getElementById( c_keys_scan_targets[ g_keys_scan_target ].status ).textContent = text;
}

function keys_scan_stop( )
{
   var scanner = g_keys_scanner;
   var target = c_keys_scan_targets[ g_keys_scan_target ];

   g_keys_scan_opened++;
   g_keys_scanner = null;

   document.getElementById( target.button ).disabled = false;

   // NOTE: Not "qr_scan_stop( )" without a camera - "qr_scan.js" is read with the keys' scripts, and this can come before them.
   if( scanner === null )
   {
      document.getElementById( target.reader ).replaceChildren( );

      return Promise.resolve( );
   }

   return qr_scan_stop( scanner ).then( function( ) { document.getElementById( target.reader ).replaceChildren( ); } );
}

async function keys_scan_start( which )
{
   await keys_scan_stop( );

   g_keys_scan_target = ( typeof which === "string" ) ? which : "contacts";

   var target = c_keys_scan_targets[ g_keys_scan_target ];
   var opened = g_keys_scan_opened;

   set_error( target.error, "" );
   keys_scan_status( "Starting the camera…" );

   if( !await qr_scan_library( ) )
   {
      if( opened === g_keys_scan_opened )
         keys_scan_status( "The QR code reader could not be loaded - reload the page to try again." );

      return;
   }

   if( opened !== g_keys_scan_opened )
      return;

   if( !qr_scan_can_use_camera( ) )
   {
      keys_scan_status( ( g_keys_scan_target === "contacts" ) ? "The camera needs this page opened as https - choose a picture of their"
       + " code instead." : "The camera needs this page opened as https - paste the address instead." );

      return;
   }

   var scanner = qr_scan_reader( target.reader );

   g_keys_scanner = scanner;
   document.getElementById( target.button ).disabled = true;

   try
   {
      await scanner.start( { facingMode: "environment" }, { fps: 10 }, keys_scan_read, function( ) { } );
   }
   catch( e )
   {
      if( opened === g_keys_scan_opened )
      {
         g_keys_scanner = null;
         document.getElementById( target.button ).disabled = false;

         keys_scan_status( ( g_keys_scan_target === "contacts" ) ? "No camera could be used - choose a picture of their code instead."
          : "No camera could be used - paste the address instead." );
      }

      return;
   }

   // NOTE: Stopped while the camera was starting - let go of it now.
   if( opened !== g_keys_scan_opened )
   {
      qr_scan_stop( scanner );

      return;
   }

   keys_scan_status( ( ( g_keys_scan_target === "ask" ) && g_keys_ask && ( g_keys_ask.kind === "give" ) )
    ? "Hold the code your administrator shows up to the camera." : target.hint );
}

// NOTE: The camera let go on leaving the section it reads for ("show_section( )" in "account.js").
function keys_leave_section( name )
{
   var section = { contacts: "contacts", device: "devices", ask: "keys", give: "keys" }[ g_keys_scan_target ];

   if( ( g_keys_scanner !== null ) && ( section !== name ) )
      keys_scan_stop( );
}

// NOTE: The camera reads a code many times a second - the first taken, then the camera let go.
async function keys_scan_read( text )
{
   if( g_keys_scanner === null )
      return;

   await keys_scan_stop( );

   keys_scan_status( "" );

   if( g_keys_scan_target === "device" )
   {
      document.getElementById( "keys_device_ask" ).value = text;

      await keys_device_answer( parse_device_link( text ) );
   }
   else if( g_keys_scan_target === "ask" )
   {
      document.getElementById( "keys_ask_reply" ).value = text;

      await keys_ask_use( text );
   }
   else if( g_keys_scan_target === "give" )
   {
      document.getElementById( "give_ask" ).value = text;

      await keys_give_go( );
   }
   else
   {
      document.getElementById( "keys_add_key" ).value = text;

      await keys_add_contact( );
   }
}

async function keys_scan_picture( input )
{
   var file = ( input.files && input.files.length ) ? input.files[ 0 ] : null;

   input.value = "";

   if( file === null )
      return;

   await keys_scan_stop( );

   g_keys_scan_target = "contacts";

   var opened = g_keys_scan_opened;

   set_error( "keys_add_error", "" );

   if( !await qr_scan_library( ) )
   {
      keys_scan_status( "The QR code reader could not be loaded - reload the page to try again." );

      return;
   }

   keys_scan_status( "Reading the picture…" );

   var text = await qr_scan_picture( "contacts_scan_reader", file, function( ) { return opened === g_keys_scan_opened; } );

   if( opened !== g_keys_scan_opened )
      return;

   document.getElementById( "contacts_scan_reader" ).replaceChildren( );

   if( text === null )
   {
      keys_scan_status( "No QR code could be read in that picture - try one closer up, or straighter." );

      return;
   }

   keys_scan_status( "" );
   document.getElementById( "keys_add_key" ).value = text;

   await keys_add_contact( );
}

// NOTE: The rooms' names by number, from the listing the chat reads ("keys_room_names( )") - null until read, false if it could not
// be read. A read answered after the page was cleared (signed out, say) is not kept - its names may be the last account's (found by
// review, 2026-11-04).
var g_keys_room_names = null;
var g_keys_room_names_reads = 0;

// NOTE: Which rooms have a key kept with these - each by its name, its number under it ("room_key_rows( )"), drawn with the numbers
// alone at once and again once the names are read. Made and removed in the chat ("room_keys_text( )").
function keys_draw_rooms( )
{
   var list = document.getElementById( "keys_room_list" );

   list.replaceChildren( );

   room_key_rows( g_keys.rooms, g_keys_room_names || { } ).forEach( function( row )
   {
      var item = document.createElement( "li" );
      var name = document.createElement( "span" );
      var number = document.createElement( "span" );

      name.className = "account-room-name";
      number.className = "account-room-number";

      if( row.name !== "" )
         name.textContent = row.name;
      else if( g_keys_room_names === null )
         name.textContent = "…";
      else
         name.textContent = ( g_keys_room_names === false ) ? "Its name could not be read" : "Not one of your rooms now";
      number.textContent = "#" + row.room;

      item.appendChild( name );
      item.appendChild( number );
      list.appendChild( item );
   } );

   document.getElementById( "keys_rooms" ).textContent = room_keys_text( g_keys.rooms );

   if( ( g_keys_room_names === null ) && ( list.children.length > 0 ) )
      keys_room_names( );
}

// NOTE: The rooms' names read from the lobby's listing, as the chat reads it - once a showing of the page; drawn again once known.
var g_keys_room_names_reading = false;

async function keys_room_names( )
{
   if( g_keys_room_names_reading )
      return;

   g_keys_room_names_reading = true;

   var reads = g_keys_room_names_reads;
   var response = await request( function( done ) { return ciyam.fetch_messages( "0000000", "", done ); } );

   if( reads !== g_keys_room_names_reads )
      return;

   g_keys_room_names_reading = false;

   var result = parse_fetch_response( response );

   if( ( result.error !== "" ) || is_error_response( response ) )
      g_keys_room_names = false;
   else
   {
      var names = { };

      result.rooms.forEach( function( entry ) { names[ entry.room ] = entry.name || ""; } );

      g_keys_room_names = names;
   }

   if( g_keys !== null )
      keys_draw_rooms( );
}

// NOTE: The keys as the node has them now, or null - another browser, or the chat with a room key, may have changed them since
// this page read them (found by review, 2026-10-26), and a change made to this page's copy would undo that one.
async function keys_latest( )
{
   var response = await request( function( done ) { return ciyam.fetch( keys_url( "" ), "GET", done ); } );

   try
   {
      var opened = await keys_module( "user_keys" ).unlock( g_keys.access, String( response ).trim( ) );

      g_keys.node = String( response ).trim( );
      g_keys.rooms = opened.rooms;
      g_keys.people = opened.people;

      keys_draw_rooms( );

      return opened;
   }
   catch( e )
   {
      return null;
   }
}

// NOTE: "change" made to the node's latest - it gives a problem to say, or "" - then locked again and sent; the card changes
// once the node has them.
async function keys_store_contacts( change, busy_text )
{
   set_error( "keys_add_error", "" );
   keys_contacts_busy( busy_text );

   var latest = await keys_latest( );

   if( latest === null )
   {
      keys_contacts_busy( "" );
      set_error( "keys_add_error", "Your keys could not be read again from the node - reload the page to try again." );

      return false;
   }

   var contacts = Object.assign( { }, latest.contacts );
   var problem = change( contacts );

   if( problem !== "" )
   {
      keys_contacts_busy( "" );

      g_keys.contacts = latest.contacts;

      keys_draw_contacts( );
      set_error( "keys_add_error", problem );

      return false;
   }

   var saved = await keys_send( g_keys.access, g_keys.rounds, { primary: g_keys.primary, secondary: g_keys.secondary, contacts: contacts,
    rooms: latest.rooms, people: latest.people }, "keys_add_error", "That", latest.guard );

   keys_contacts_busy( "" );

   if( !saved )
      return false;

   g_keys.contacts = contacts;

   keys_draw_contacts( );

   return true;
}

// NOTE: Their contact code as pasted or scanned - your code stays shown, as they may still be reading it.
async function keys_add_contact( )
{
   var contact = parse_contact_code( document.getElementById( "keys_add_key" ).value );
   var own_name = account_display_name( ciyam.username, ciyam.is_admin );
   var problem = contact_problem( contact, g_keys.contacts, g_keys.own, own_name );

   keys_scan_status( "" );

   if( problem !== "" )
   {
      set_error( "keys_add_error", problem );

      return;
   }

   var add = function( contacts )
   {
      var again = contact_problem( contact, contacts, g_keys.own, own_name );

      if( again === "" )
         contacts[ contact.name ] = contact.key;

      return again;
   };

   if( await keys_store_contacts( add, "Saving " + contact.name + "'s key…" ) )
   {
      document.getElementById( "keys_add_key" ).value = "";

      keys_scan_status( contact.name + " added - once they have added you too, your direct messages are encrypted." );
   }
}

async function keys_remove_contact( name )
{
   if( !await ask_dialog( { title: "Remove " + name + "'s public key?", text: "You can add it again later.", yes: "Remove",
    danger: true } ) )
      return;

   await keys_store_contacts( function( contacts ) { delete contacts[ name ]; return ""; }, "Removing " + name + "'s key…" );
}

// ---- The words typed

async function keys_enter_go( )
{
   set_error( "keys_error", "" );

   if( !await recover_load_bip39( ) )
   {
      set_error( "keys_error", "The word list could not be read - reload the page to try again." );

      return;
   }

   var uk = keys_module( "user_keys" );
   var entropy = "";

   try
   {
      entropy = uk.entropy_of_words( document.getElementById( "keys_enter_words" ).value );
   }
   catch( e )
   {
      set_error( "keys_error", e.message );

      return;
   }

   // NOTE: The words are left in the field until the keys are open - wrong ones are put right, not typed again.
   if( g_keys.entering === "restore" )
   {
      await keys_make_and_save( entropy, "Your keys are made again from your words, and on this browser." );

      return;
   }

   // NOTE: The node's words (Ian, 2026-11-11) - this person's keys made from them and their PIN.
   if( g_keys.entering === "node" )
   {
      try
      {
         entropy = await keys_person_entropy( entropy );
      }
      catch( e )
      {
         set_error( "keys_error", "Your keys could not be made: " + ( e && e.message ? e.message : "reload the page to try again." ) );

         return;
      }

      await keys_make_and_save( entropy, "Your keys are made from the node's words, and on this browser." );

      return;
   }

   // NOTE: Made with the rounds the node's were locked with, so it is the access key they were locked under - from words of their
   // own, or from the node's.
   keys_busy( "Securing your keys…" );

   await keys_drawn( );

   var changing = ( g_keys.entering === "change" );

   // NOTE: No round left to take off - said by "keys_change_access( )".
   if( changing && ( g_keys.rounds <= 1 ) )
   {
      keys_busy( "" );

      await keys_change_access( entropy, "" );

      return;
   }

   var found = null;

   try
   {
      // NOTE: For a new access key, the one a round fewer - made once, and passed on (found by review, 2026-11-11).
      if( changing )
         found = await keys_entropy_typed( entropy, g_keys.rounds - 1, async function( access, typed )
         {
            return ( await uk.next_access_key( typed, access ) ) === g_keys.access;
         } );
      else
         found = await keys_entropy_typed( entropy, uk.rounds_of( g_keys.node ), async function( access )
         {
            return uk.unlock( access, g_keys.node ).then( function( ) { return true; }, function( ) { return false; } );
         } );
   }
   catch( e )
   {
      keys_busy( "" );
      set_error( "keys_error", ( changing ? "Your access key could not be changed: " : "Your keys could not be opened: " )
       + ( e && e.message ? e.message : "reload the page to try again." ) );

      return;
   }

   keys_busy( "" );

   if( changing && ( found !== null ) )
   {
      await keys_change_access( found.entropy, found.access );

      return;
   }

   if( found === null )
   {
      set_error( "keys_error", "Those are not the words your keys were set up with." );

      return;
   }

   entropy = found.entropy;

   var access = found.access;

   // NOTE: Keys from before the guard (2026-10-31) given one, now the words are here to make it.
   if( keys_guard_now( ) === "" )
      await keys_add_guard( entropy );

   keys_keep_here( access );

   document.getElementById( "keys_enter_words" ).value = "";

   await keys_show_here( access, "Your keys are on this browser.", false );
}

// NOTE: This person's entropy from the entropy of the node's twelve words (Ian, 2026-11-11 - one set of words for the node and
// everyone on it): the people key, and from it and their PIN theirs - admin's from it and "admin", as a recovery changes admin's
// PIN (Ian, 2026-11-12) ("ciyam_user_keys.js").
async function keys_person_entropy( node_entropy )
{
   var uk = keys_module( "user_keys" );

   return uk.person_entropy( await uk.people_key( node_entropy ), ciyam.is_admin ? uk.c_admin_person : ciyam.access );
}

// NOTE: The words typed as their own, then as the node's - { entropy, access } for the first whose access key (with "rounds") "opens"
// says is the one ("opens( access, entropy )"), or null for neither.
async function keys_entropy_typed( entropy, rounds, opens )
{
   var uk = keys_module( "user_keys" );
   var access = await uk.access_key( entropy, rounds );

   if( await opens( access, entropy ) )
      return { entropy: entropy, access: access };

   var person = await keys_person_entropy( entropy );

   access = await uk.access_key( person, rounds );

   if( await opens( access, person ) )
      return { entropy: person, access: access };

   return null;
}

// NOTE: The guard's line put before keys without one, as they are - nothing said if the node does not take it, as the keys are
// still as they were.
async function keys_add_guard( entropy )
{
   var uk = keys_module( "user_keys" );

   keys_busy( "Securing your keys…" );

   await keys_drawn( );

   try
   {
      var rounds = uk.rounds_of( g_keys.node );
      var guard = await uk.guard( entropy, rounds );

      // NOTE: The keys as the node has them now, just before - a room key or contact saved meanwhile (by the chat, or another
      // browser, while the words were typed) is kept, not overwritten (found by review). Guarded since, or changed - left alone.
      var latest = String( await request( function( done ) { return ciyam.fetch( keys_url( "" ), "GET", done ); } ) ).trim( );

      if( ( uk.parts_of( latest ).guard !== "" ) || ( uk.rounds_of( latest ) !== rounds ) )
         g_keys.node = latest;
      else
      {
         var kept = uk.kept_text( guard, uk.parts_of( latest ).locked );

         var response = await request( function( done )
         {
            return ciyam.fetch_in_body( keys_url( "&payload=" + encodeURIComponent( kept ) ), "PUT", done );
         } );

         g_keys.node = ( problem_text( response ) === "" ) ? kept : latest;
      }
   }
   catch( e )
   {
   }

   keys_busy( "" );
}

// NOTE: A new access key, a round fewer (Ian, 2026-10-27) - the words seen to be these keys' (the new key a round on is the one
// this browser has), the keys locked again under it with a guard the node hashes on to its own, and kept here. Any other browser
// with the old key can no longer open them, and cannot make the new one without the words.
// NOTE: "made" - the access key a round fewer, if already made from these words ("" to make it here).
async function keys_change_access( entropy, made )
{
   var uk = keys_module( "user_keys" );
   var rounds = g_keys.rounds - 1;

   if( rounds < 1 )
   {
      set_error( "keys_error", "The access key can't be changed again - set up new keys." );

      return;
   }

   keys_busy( "Securing your keys…" );

   await keys_drawn( );

   var access = "";
   var guard = "";

   try
   {
      access = made || await uk.access_key( entropy, rounds );

      if( await uk.next_access_key( entropy, access ) === g_keys.access )
         guard = await uk.guard( entropy, rounds );
   }
   catch( e )
   {
      keys_busy( "" );
      set_error( "keys_error", "Your access key could not be changed: " + ( e && e.message ? e.message : "reload the page to try again." ) );

      return;
   }

   if( guard === "" )
   {
      keys_busy( "" );
      set_error( "keys_error", "Those are not the words your keys were set up with." );

      return;
   }

   keys_busy( "Saving your keys…" );

   // NOTE: What the node has now - another browser, or the chat with a room key, may have changed them since.
   var latest = await keys_latest( );

   if( latest === null )
   {
      keys_busy( "" );
      set_error( "keys_error", "Your keys could not be read again from the node - reload the page to try again." );

      return;
   }

   var saved = await keys_send( access, rounds, { primary: latest.primary, secondary: latest.secondary, contacts: latest.contacts,
    rooms: latest.rooms, people: latest.people }, "keys_error", "Your keys", guard );

   keys_busy( "" );

   if( !saved )
      return;

   keys_keep_here( access );

   var node = g_keys.node;

   keys_clear( );

   g_keys.node = node;

   await keys_show_here( access, "Your access key is changed. Any other browser that had your keys needs your twelve words, or this"
    + " browser's QR code, to use them again.", false );
}

async function keys_start_again( )
{
   if( !await ask_dialog( { title: "Set up new keys?", text: "From new twelve words. The keys on the node are replaced, the public"
    + " keys you've added for others and your room keys are removed, and anyone with your old public key will need your new one.",
    yes: "Set up new keys", danger: true } ) )
      return;

   keys_start( );
}

async function keys_forget( )
{
   if( !await ask_dialog( { title: "Forget your keys on this browser?", text: "To use them here again you'll need your twelve words,"
    + " or another browser's QR code.", yes: "Forget them", danger: true } ) )
      return;

   keys_forget_here( );

   render_keys( );
}

function install_keys( )
{
   if( g_keys_installed )
      return;

   g_keys_installed = true;

   // NOTE: The twelve words typed with help, as "test_bip39.html" (Ian, 2026-10-31) - Enter as "Use my keys here".
   g_keys_words_entry = words_entry( document.getElementById( "keys_enter_words" ), { wordlist: recover_wordlist, on_enter: keys_enter_go } );

   document.getElementById( "keys_start" ).addEventListener( "click", keys_start );
   document.getElementById( "keys_restore" ).addEventListener( "click", function( ) { keys_show_enter( "restore" ); } );
   document.getElementById( "keys_node" ).addEventListener( "click", function( ) { keys_show_enter( "node" ); } );
   document.getElementById( "keys_admin" ).addEventListener( "click", function( ) { g_keys.asked_from = "keys_none"; keys_ask_start( "give" ); } );
   document.getElementById( "keys_enter_admin" ).addEventListener( "click", function( ) { g_keys.asked_from = ""; keys_ask_start( "give" ); } );
   document.getElementById( "keys_random_cancel" ).addEventListener( "click", keys_cancel );
   document.getElementById( "keys_random_next" ).addEventListener( "click", keys_make_words );
   document.getElementById( "keys_words_cancel" ).addEventListener( "click", keys_cancel );
   document.getElementById( "keys_words_next" ).addEventListener( "click", keys_draw_check );
   document.getElementById( "keys_check_back" ).addEventListener( "click", function( ) { keys_show( "keys_words" ); } );
   document.getElementById( "keys_save" ).addEventListener( "click", function( )
   {
      keys_make_and_save( g_keys.entropy, "Your keys are on this browser." );
   } );
   document.getElementById( "keys_enter_go" ).addEventListener( "click", keys_enter_go );
   document.getElementById( "keys_enter_new" ).addEventListener( "click", keys_start_again );
   document.getElementById( "keys_change" ).addEventListener( "click", function( ) { keys_show_enter( "change" ); } );
   document.getElementById( "keys_enter_cancel" ).addEventListener( "click", keys_cancel );
   document.getElementById( "keys_forget" ).addEventListener( "click", keys_forget );
   document.getElementById( "keys_public_show" ).addEventListener( "click", keys_show_public );
   document.getElementById( "keys_public_hide" ).addEventListener( "click", keys_hide_public );
   document.getElementById( "keys_device" ).addEventListener( "click", keys_show_device );
   document.getElementById( "keys_device_hide" ).addEventListener( "click", keys_hide_device );
   document.getElementById( "keys_device_done" ).addEventListener( "click", keys_hide_device );
   document.getElementById( "keys_device_use" ).addEventListener( "click", keys_device_use );
   document.getElementById( "device_scan" ).addEventListener( "click", function( ) { keys_scan_start( "device" ); } );
   document.getElementById( "keys_ask_start" ).addEventListener( "click", function( ) { g_keys.asked_from = ""; keys_ask_start( ); } );
   document.getElementById( "keys_ask_cancel" ).addEventListener( "click", keys_ask_cancel );
   document.getElementById( "keys_ask_use" ).addEventListener( "click", function( ) { keys_ask_use( ); } );
   document.getElementById( "ask_scan" ).addEventListener( "click", function( ) { keys_scan_start( "ask" ); } );
   document.getElementById( "keys_add" ).addEventListener( "click", keys_add_contact );
   document.getElementById( "give_go" ).addEventListener( "click", keys_give_go );
   document.getElementById( "give_cancel" ).addEventListener( "click", keys_give_close );
   document.getElementById( "give_done" ).addEventListener( "click", keys_give_close );
   document.getElementById( "give_scan" ).addEventListener( "click", function( ) { keys_scan_start( "give" ); } );

   // NOTE: A field's problem cleared as it is changed.
   c_keys_give_parts.forEach( function( part )
   {
      document.getElementById( part.field ).addEventListener( "input", function( ) { keys_give_problem( part.key, "" ); } );
   } );
   document.getElementById( "contacts_scan" ).addEventListener( "click", function( ) { keys_scan_start( "contacts" ); } );
   document.getElementById( "contacts_picture" ).addEventListener( "click", function( ) { document.getElementById( "contacts_scan_file" ).click( ); } );
   document.getElementById( "contacts_scan_file" ).addEventListener( "change", function( ) { keys_scan_picture( this ); } );

   document.getElementById( "keys_written" ).addEventListener( "change", function( event )
   {
      document.getElementById( "keys_words_next" ).disabled = !event.target.checked;
   } );
}

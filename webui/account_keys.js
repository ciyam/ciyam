// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Your keys, on My account (Ian, 2026-10-23 and 2026-10-25). An account's keys come from twelve words of its own - made
// from popped bubbles and the browser's randomness, as a node's are, written down and some checked back. The words go nowhere:
// from them come the keys, kept on the node ("user-keys") with others' public keys, locked under an access key - many rounds of
// a hash of the words - which is what each browser keeps ("ciyam_user_keys.js"; Ian: a hash "won't help anyone trying to crack
// the actual twelve words"). Another browser gets the access key from this one by a QR code, or makes it from the words typed.
// It is kept while the account is remembered here, for the tab only when it is not, and goes with "Reset this browser" or the
// account forgotten at a sign in.
//
// What the card needs is read in the first time it is shown - "bip39.min.js" only when words are made or typed
// ("recover_load_bip39( )" in "signin.js"), being large.
const c_keys_scripts = [ "ed25519.min.js", "ciyam_harden.js", "ciyam_chacha20.js", "ciyam_ed25519.js", "ciyam_secure_message.js",
 "ciyam_user_keys.js", "home_parse.js", "bubbles.js", "qrcode.min.js",
 "qr_scan.js" ];

const c_keys_parts = [ "keys_loading", "keys_none", "keys_random", "keys_words", "keys_check", "keys_enter", "keys_here" ];

const c_keys_pops = 12;
const c_keys_most_bubbles = 16;
const c_keys_entropy_length = 32;
const c_keys_qr_size = 120;
const c_keys_device_qr_size = 168;

// NOTE: An access key brought by another browser's QR code ("#keys=" - the part after "#" never goes to the server), held for the
// tab until this account's keys are known.
const c_keys_link_prefix = "#keys=";
const c_keys_pending_name = c_signin_pending_keys_name;

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
    primary: "", secondary: "", contacts: { }, rooms: { }, own: "", entering: "" };
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
   if( location.hash.indexOf( c_keys_link_prefix ) !== 0 )
      return false;

   var access = location.hash.substring( c_keys_link_prefix.length ).toLowerCase( );

   try
   {
      if( /^[0-9a-f]{64}$/.test( access ) )
         sessionStorage.setItem( c_keys_pending_name, access );
   }
   catch( e )
   {
   }

   history.replaceState( null, "", location.pathname + location.search + "#keys" );

   return true;
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
         return ciyam.fetch( keys_url( "&payload=" + encodeURIComponent( locked ) ), "PUT", done );
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

   [ "keys_word_list", "keys_check_rows", "keys_qr", "keys_contact_list", "keys_device_qr" ].forEach( function( id )
   {
      document.getElementById( id ).replaceChildren( );
   } );

   [ "keys_enter_words", "keys_add_key", "keys_device_link" ].forEach( function( id ) { document.getElementById( id ).value = ""; } );

   document.getElementById( "keys_written" ).checked = false;
   keys_hide_public( );
   document.getElementById( "keys_device_part" ).hidden = true;
   document.getElementById( "keys_device_row" ).hidden = false;
   document.getElementById( "keys_rooms" ).textContent = "";

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

      document.getElementById( "keys_none_lede" ).textContent = ( state === "lost" )
       ? "Your keys are no longer on the node, and this browser can't make them again by itself. Type your twelve words to put"
        + " them back, or set up new ones."
       : "Your keys come from twelve words of your own, which you write down. The node never sees them - it keeps your keys"
        + " locked so that only your browsers can open them.";

      keys_show( "keys_none" );
   }
   else if( pending !== "" )
      await keys_show_here( pending, "Your keys are now on this browser too.", true );
   else if( state === "elsewhere" )
      keys_show_enter( "elsewhere" );
   else
      await keys_show_here( here, "Your keys are on this browser.", false );
}

// NOTE: The words asked for - "elsewhere": on a browser new to the keys; "other": what this browser kept does not open the
// node's (set up again from another browser, or a new access key, say); "restore": nothing on the node, the keys made again
// from the words; "change": a new access key (Ian, 2026-10-27: a round fewer each time). Guarded keys (Ian, 2026-10-27) the node
// replaces only from the words they were set up with, so new words are not offered for them - the admin clears them.
function keys_show_enter( mode )
{
   var guarded = ( keys_guard_now( ) !== "" ) && ( mode !== "restore" );
   var lost = guarded ? " Lost them? Ask the administrator to clear your keys, then set up new ones." : "";

   g_keys.entering = mode;

   document.getElementById( "keys_enter_lede" ).textContent = {
      elsewhere: "Your keys are set up. To use them on this browser, type your twelve words - or open the QR code from"
       + " \"Add another device\" on a browser that has them." + lost,
      other: "The keys on the node were locked by another browser since. Type the twelve words they were set up with." + lost,
      restore: "Type your twelve words, and your keys are made again from them.",
      change: "Type your twelve words for a new access key. Use it if a browser or phone with your keys is lost: any other browser"
       + " that has them will then need your words, or this browser's QR code, again." }[ mode ];

   document.getElementById( "keys_enter_new" ).hidden = ( mode === "restore" ) || ( mode === "change" ) || guarded;
   document.getElementById( "keys_enter_cancel" ).hidden = ( mode !== "restore" ) && ( mode !== "change" );
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
         set_error( "keys_error", "That QR code is not for this account's keys - is it from another account?" );
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

// NOTE: The access key as a QR code that opens this page on another device ("#keys=", "keys_take_link( )") - shown only when
// asked, as anyone who scans it can open the keys.
function keys_show_device( )
{
   var link = location.origin + location.pathname + c_keys_link_prefix + g_keys.access;
   var box = document.getElementById( "keys_device_qr" );

   box.replaceChildren( );

   new QRCode( box, { text: link, width: c_keys_device_qr_size, height: c_keys_device_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
    correctLevel: QRCode.CorrectLevel.M } );

   box.removeAttribute( "title" );
   box.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );

   document.getElementById( "keys_device_link" ).value = link;
   document.getElementById( "keys_device_part" ).hidden = false;
   document.getElementById( "keys_device_row" ).hidden = true;
}

function keys_hide_device( )
{
   document.getElementById( "keys_device_qr" ).replaceChildren( );
   document.getElementById( "keys_device_link" ).value = "";
   document.getElementById( "keys_device_part" ).hidden = true;
   document.getElementById( "keys_device_row" ).hidden = false;
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

function keys_scan_status( text )
{
   document.getElementById( "contacts_scan_status" ).textContent = text;
}

function keys_scan_stop( )
{
   var scanner = g_keys_scanner;

   g_keys_scan_opened++;
   g_keys_scanner = null;

   document.getElementById( "contacts_scan" ).disabled = false;

   // NOTE: Not "qr_scan_stop( )" without a camera - "qr_scan.js" is read with the keys' scripts, and this can come before them.
   if( scanner === null )
   {
      document.getElementById( "contacts_scan_reader" ).replaceChildren( );

      return Promise.resolve( );
   }

   return qr_scan_stop( scanner ).then( function( ) { document.getElementById( "contacts_scan_reader" ).replaceChildren( ); } );
}

async function keys_scan_start( )
{
   await keys_scan_stop( );

   var opened = g_keys_scan_opened;

   set_error( "keys_add_error", "" );
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
      keys_scan_status( "The camera needs this page opened as https - choose a picture of their code instead." );

      return;
   }

   var scanner = qr_scan_reader( "contacts_scan_reader" );

   g_keys_scanner = scanner;
   document.getElementById( "contacts_scan" ).disabled = true;

   try
   {
      await scanner.start( { facingMode: "environment" }, { fps: 10 }, keys_scan_read, function( ) { } );
   }
   catch( e )
   {
      if( opened === g_keys_scan_opened )
      {
         g_keys_scanner = null;
         document.getElementById( "contacts_scan" ).disabled = false;

         keys_scan_status( "No camera could be used - choose a picture of their code instead." );
      }

      return;
   }

   // NOTE: Stopped while the camera was starting - let go of it now.
   if( opened !== g_keys_scan_opened )
   {
      qr_scan_stop( scanner );

      return;
   }

   keys_scan_status( "Hold their contact code up to the camera." );
}

// NOTE: The camera reads a code many times a second - the first taken, then the camera let go.
async function keys_scan_read( text )
{
   if( g_keys_scanner === null )
      return;

   await keys_scan_stop( );

   keys_scan_status( "" );
   document.getElementById( "keys_add_key" ).value = text;

   await keys_add_contact( );
}

async function keys_scan_picture( input )
{
   var file = ( input.files && input.files.length ) ? input.files[ 0 ] : null;

   input.value = "";

   if( file === null )
      return;

   await keys_scan_stop( );

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

// NOTE: Which rooms have a key kept with these ("room_keys_text( )") - made and removed in the chat.
function keys_draw_rooms( )
{
   document.getElementById( "keys_rooms" ).textContent = room_keys_text( g_keys.rooms );
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
    rooms: latest.rooms }, "keys_add_error", "That", latest.guard );

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

      keys_scan_status( contact.name + " added - their messages to you and yours to them are encrypted now." );
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

   if( g_keys.entering === "change" )
   {
      await keys_change_access( entropy );

      return;
   }

   // NOTE: Made with the rounds the node's were locked with, so it is the access key they were locked under.
   keys_busy( "Securing your keys…" );

   await keys_drawn( );

   var access = "";

   try
   {
      access = await uk.access_key( entropy, uk.rounds_of( g_keys.node ) );
   }
   catch( e )
   {
      keys_busy( "" );
      set_error( "keys_error", "Your keys could not be opened: " + ( e && e.message ? e.message : "reload the page to try again." ) );

      return;
   }

   keys_busy( "" );

   try
   {
      await uk.unlock( access, g_keys.node );
   }
   catch( e )
   {
      set_error( "keys_error", "Those are not the words your keys were set up with." );

      return;
   }

   // NOTE: Keys from before the guard (2026-10-31) given one, now the words are here to make it.
   if( keys_guard_now( ) === "" )
      await keys_add_guard( entropy );

   keys_keep_here( access );

   document.getElementById( "keys_enter_words" ).value = "";

   await keys_show_here( access, "Your keys are on this browser.", false );
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
            return ciyam.fetch( keys_url( "&payload=" + encodeURIComponent( kept ) ), "PUT", done );
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
async function keys_change_access( entropy )
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
      access = await uk.access_key( entropy, rounds );

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
    rooms: latest.rooms }, "keys_error", "Your keys", guard );

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
   document.getElementById( "keys_add" ).addEventListener( "click", keys_add_contact );
   document.getElementById( "contacts_scan" ).addEventListener( "click", keys_scan_start );
   document.getElementById( "contacts_picture" ).addEventListener( "click", function( ) { document.getElementById( "contacts_scan_file" ).click( ); } );
   document.getElementById( "contacts_scan_file" ).addEventListener( "change", function( ) { keys_scan_picture( this ); } );

   document.getElementById( "keys_written" ).addEventListener( "change", function( event )
   {
      document.getElementById( "keys_words_next" ).disabled = !event.target.checked;
   } );
}

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
 "ciyam_user_keys.js", "home_parse.js", "bubbles.js", "qrcode.min.js" ];

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

// NOTE: Locked under the access key and sent - false, with the error said, if the node did not take them.
async function keys_send( access, rounds, contents, error_id, what )
{
   var response = "";
   var locked = "";

   try
   {
      locked = await keys_module( "user_keys" ).lock( access, contents, { rounds: rounds } );

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

function keys_busy( text )
{
   document.getElementById( "keys_busy" ).hidden = !text;
   document.getElementById( "keys_busy_text" ).textContent = text || "";
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

   [ "keys_enter_words", "keys_add_name", "keys_add_key", "keys_device_link" ].forEach( function( id ) { document.getElementById( id ).value = ""; } );

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
// node's (set up again from another browser, say); "restore": nothing on the node, the keys made again from the words.
function keys_show_enter( mode )
{
   g_keys.entering = mode;

   document.getElementById( "keys_enter_lede" ).textContent = {
      elsewhere: "Your keys are set up. To use them on this browser, type your twelve words - or open the QR code from"
       + " \"Add another device\" on a browser that has them.",
      other: "The keys on the node were locked by another browser since. Type the twelve words they were set up with.",
      restore: "Type your twelve words, and your keys are made again from them." }[ mode ];

   document.getElementById( "keys_enter_new" ).hidden = ( mode === "restore" );
   document.getElementById( "keys_enter_cancel" ).hidden = ( mode !== "restore" );
   document.getElementById( "keys_enter_words" ).value = "";

   keys_show( "keys_enter" );
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

   var uk = keys_module( "user_keys" );
   var rounds = uk.c_default_rounds;
   var access = "";
   var seeds = null;

   try
   {
      access = await uk.access_key( entropy, rounds );
      seeds = uk.seeds_of( entropy );
   }
   catch( e )
   {
      keys_busy( "" );
      set_error( "keys_error", "Your keys could not be made: " + ( e && e.message ? e.message : "reload the page to try again." ) );

      return;
   }

   keys_busy( "Saving your keys…" );

   var saved = await keys_send( access, rounds, { primary: seeds.primary, secondary: seeds.secondary, contacts: { } }, "keys_error", "Your keys" );

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

// NOTE: The public key, as text and a QR code, only when asked (Ian, 2026-10-27: with quantum attacks to come, shown only when it
// is known to be safe) - and taken out of the page again when hidden, or on leaving Your keys.
function keys_show_public( )
{
   if( !g_keys || !g_keys.own )
      return;

   var box = document.getElementById( "keys_qr" );

   document.getElementById( "keys_public" ).textContent = g_keys.own;

   // NOTE: Drawn in the page - "qrcode.min.js" - so the key goes nowhere else; the library's own title taken off, as Home's.
   box.replaceChildren( );

   new QRCode( box, { text: g_keys.own, width: c_keys_qr_size, height: c_keys_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
    correctLevel: QRCode.CorrectLevel.M } );

   box.removeAttribute( "title" );
   box.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );

   document.getElementById( "keys_public_part" ).hidden = false;
   document.getElementById( "keys_public_show" ).hidden = true;
   document.getElementById( "keys_public_hide" ).hidden = false;
}

function keys_hide_public( )
{
   document.getElementById( "keys_qr" ).replaceChildren( );
   document.getElementById( "keys_public" ).textContent = "";
   document.getElementById( "keys_public_part" ).hidden = true;
   document.getElementById( "keys_public_show" ).hidden = false;
   document.getElementById( "keys_public_hide" ).hidden = true;
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
    rooms: latest.rooms }, "keys_add_error", "That" );

   keys_contacts_busy( "" );

   if( !saved )
      return false;

   g_keys.contacts = contacts;

   keys_draw_contacts( );

   return true;
}

async function keys_add_contact( )
{
   var name = document.getElementById( "keys_add_name" ).value.trim( ).toLowerCase( );
   var key = normalise_public_key( document.getElementById( "keys_add_key" ).value );
   var problem = contact_problem( name, key, g_keys.contacts, g_keys.own );

   if( problem !== "" )
   {
      set_error( "keys_add_error", problem );

      return;
   }

   var add = function( contacts )
   {
      var again = contact_problem( name, key, contacts, g_keys.own );

      if( again === "" )
         contacts[ name ] = key;

      return again;
   };

   if( await keys_store_contacts( add, "Saving " + name + "'s key…" ) )
   {
      document.getElementById( "keys_add_name" ).value = "";
      document.getElementById( "keys_add_key" ).value = "";
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

   document.getElementById( "keys_enter_words" ).value = "";

   if( g_keys.entering === "restore" )
   {
      await keys_make_and_save( entropy, "Your keys are made again from your words, and on this browser." );

      return;
   }

   // NOTE: Made with the rounds the node's were locked with, so it is the access key they were locked under.
   keys_busy( "Securing your keys…" );

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

   keys_keep_here( access );

   await keys_show_here( access, "Your keys are on this browser.", false );
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
   document.getElementById( "keys_enter_cancel" ).addEventListener( "click", keys_cancel );
   document.getElementById( "keys_forget" ).addEventListener( "click", keys_forget );
   document.getElementById( "keys_public_show" ).addEventListener( "click", keys_show_public );
   document.getElementById( "keys_public_hide" ).addEventListener( "click", keys_hide_public );
   document.getElementById( "keys_device" ).addEventListener( "click", keys_show_device );
   document.getElementById( "keys_device_hide" ).addEventListener( "click", keys_hide_device );
   document.getElementById( "keys_add" ).addEventListener( "click", keys_add_contact );

   document.getElementById( "keys_written" ).addEventListener( "change", function( event )
   {
      document.getElementById( "keys_words_next" ).disabled = !event.target.checked;
   } );
}

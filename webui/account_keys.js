// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Your keys, on My account (Ian, 2026-10-23, and 2026-10-25: "just use the twelve words to encrypt them"). An account's
// keys come from twelve words of its own - made from popped bubbles and the browser's randomness, as a node's are, written
// down and some checked back. The words never go to the node: what it keeps ("user-keys") is what they cannot make again -
// others' public keys, scanned face to face - locked under them ("ciyam_user_keys.js"). This browser keeps the words, so
// they are typed once a browser: kept while the account is remembered here, for the tab only when it is not, and gone with
// "Reset this browser" or the account forgotten at a sign in.
//
// What the card needs is read in the first time it is shown - "bip39.min.js" only when words are made or typed
// ("recover_load_bip39( )" in "signin.js"), being large.
const c_keys_scripts = [ "ed25519.min.js", "ciyam_chacha20.js", "ciyam_ed25519.js", "ciyam_secure_message.js", "ciyam_user_keys.js",
 "home_parse.js", "bubbles.js", "qrcode.min.js" ];

const c_keys_parts = [ "keys_loading", "keys_none", "keys_random", "keys_words", "keys_check", "keys_enter", "keys_here" ];

const c_keys_pops = 12;
const c_keys_most_bubbles = 16;
const c_keys_entropy_length = 32;
const c_keys_qr_size = 120;

var g_keys = null;
var g_keys_bubbles = null;
var g_keys_scripts = null;
var g_keys_installed = false;

function fresh_keys( )
{
   return { samples: [ ], pops: 0, entropy: "", words: [ ], checking: [ ], right: 0, node: "" };
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
      return localStorage.getItem( keys_storage_name( ) ) || sessionStorage.getItem( keys_storage_name( ) ) || "";
   }
   catch( e )
   {
      return "";
   }
}

// NOTE: For as long as the account is remembered on this browser - otherwise for the tab only.
function keys_keep_here( entropy )
{
   try
   {
      var remembered = ( parse_access_list( localStorage.getItem( c_storage_access ) ).indexOf( ciyam.access ) >= 0 );

      ( remembered ? localStorage : sessionStorage ).setItem( keys_storage_name( ), entropy );
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

// ---- The node's copy

function keys_url( extra )
{
   return ciyam.get_cws_url( ) + "/user-keys?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text" + ( extra || "" )
    + "&session=" + ciyam.sessid;
}

// ---- Showing it

function keys_show( part )
{
   c_keys_parts.forEach( function( id ) { document.getElementById( id ).hidden = ( id !== part ); } );
}

function keys_busy( text )
{
   document.getElementById( "keys_busy" ).hidden = !text;
   document.getElementById( "keys_busy_text" ).textContent = text || "";
}

// NOTE: Words gone from the page - after saving, cancelling or signing out.
function keys_clear( )
{
   if( g_keys_bubbles !== null )
      g_keys_bubbles.clear( );

   g_keys = fresh_keys( );

   [ "keys_word_list", "keys_check_rows", "keys_qr" ].forEach( function( id ) { document.getElementById( id ).replaceChildren( ); } );

   document.getElementById( "keys_enter_words" ).value = "";
   document.getElementById( "keys_written" ).checked = false;
   document.getElementById( "keys_public" ).textContent = "";

   keys_busy( "" );
   set_error( "keys_error", "" );
}

// NOTE: From My account - which part, from this browser and the node ("keys_state( )" in "account_parse.js").
async function render_keys( )
{
   install_keys( );
   keys_clear( );
   keys_show( "keys_loading" );

   var asked_for = ciyam.access;

   if( !await keys_load_scripts( ) )
   {
      keys_show( null );
      set_error( "keys_error", "What your keys need could not be read - reload the page to try again." );

      return;
   }

   var response = await request( function( done ) { return ciyam.fetch( keys_url( "" ), "GET", done ); } );

   // NOTE: Signed out, or another account in, while it was asked - not theirs to show.
   if( ( ciyam.access === "" ) || ( ciyam.access !== asked_for ) )
      return;

   var here = keys_kept_here( );
   var state = keys_state( here !== "", response );

   g_keys.node = String( response ).trim( );

   if( state === "unreadable" )
   {
      keys_show( null );
      set_error( "keys_error", "Your keys could not be read: " + ( problem_text( response ) || "the node keeps something that is not locked keys." ) );
   }
   else if( state === "none" )
      keys_show( "keys_none" );
   else if( state === "elsewhere" )
      keys_show_enter( false );
   else
      await keys_show_here( here, state === "unsaved" );
}

// NOTE: The words asked for - on a browser new to the keys, or kept here but not the ones the node's were locked with (set up
// again from another browser, say).
function keys_show_enter( other_words )
{
   document.getElementById( "keys_enter_lede" ).textContent = other_words
    ? "The keys on the node were locked with other words than this browser had. Type the twelve words they were set up with."
    : "Your keys are set up. To use them on this browser, type your twelve words.";

   document.getElementById( "keys_enter_words" ).value = "";

   keys_show( "keys_enter" );
}

async function keys_show_here( entropy, unsaved )
{
   var contacts = { };

   if( !unsaved )
   {
      try
      {
         contacts = ( await keys_module( "user_keys" ).unlock( entropy, g_keys.node ) ).contacts;
      }
      catch( e )
      {
         keys_forget_here( );
         keys_show_enter( true );

         return;
      }
   }

   var key = keys_module( "secure_message" ).public_key( entropy );
   var box = document.getElementById( "keys_qr" );

   document.getElementById( "keys_public" ).textContent = key;

   // NOTE: Drawn in the page - "qrcode.min.js" - so the key goes nowhere else; the library's own title taken off, as Home's.
   box.replaceChildren( );

   new QRCode( box, { text: key, width: c_keys_qr_size, height: c_keys_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
    correctLevel: QRCode.CorrectLevel.M } );

   box.removeAttribute( "title" );

   var count = Object.keys( contacts ).length;

   document.getElementById( "keys_here_lede" ).textContent = unsaved
    ? "Your keys are on this browser, but not on the node - save them there so your other browsers can use them."
    : "Your keys are on this browser.";

   document.getElementById( "keys_contacts" ).textContent = ( count === 0 ) ? "No one's public key saved yet."
    : ( count + ( count === 1 ? " person's public key saved." : " people's public keys saved." ) );

   document.getElementById( "keys_save_again" ).hidden = !unsaved;

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
   keys_clear( );
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

// NOTE: Locked under the words and sent; kept here once the node has them. "entropy" - kept here already (saving again).
async function keys_save( entropy )
{
   var words_entropy = entropy || g_keys.entropy;

   set_error( "keys_error", "" );
   keys_busy( "Saving your keys…" );

   var response = "";

   try
   {
      var locked = await keys_module( "user_keys" ).lock( words_entropy, { contacts: { } } );

      response = await request( function( done )
      {
         return ciyam.fetch( keys_url( "&payload=" + encodeURIComponent( locked ) ), "PUT", done );
      } );

      g_keys.node = locked;
   }
   catch( e )
   {
      response = "Error: " + ( e && e.message ? e.message : "they could not be locked." );
   }

   keys_busy( "" );

   if( problem_text( response ) !== "" )
   {
      set_error( "keys_error", "Your keys could not be saved: " + problem_text( response ) );

      return;
   }

   keys_keep_here( words_entropy );

   var node = g_keys.node;

   keys_clear( );

   g_keys.node = node;

   await keys_show_here( words_entropy, false );
}

// ---- Using them on this browser

async function keys_enter_go( )
{
   set_error( "keys_error", "" );

   if( !await recover_load_bip39( ) )
   {
      set_error( "keys_error", "The word list could not be read - reload the page to try again." );

      return;
   }

   var entropy = "";

   try
   {
      entropy = keys_module( "user_keys" ).entropy_of_words( document.getElementById( "keys_enter_words" ).value );
   }
   catch( e )
   {
      set_error( "keys_error", e.message );

      return;
   }

   try
   {
      await keys_module( "user_keys" ).unlock( entropy, g_keys.node );
   }
   catch( e )
   {
      set_error( "keys_error", "Those are not the words your keys were set up with." );

      return;
   }

   keys_keep_here( entropy );

   document.getElementById( "keys_enter_words" ).value = "";

   await keys_show_here( entropy, false );
}

function keys_start_again( )
{
   if( !confirm( "Set up new keys from new twelve words? The keys on the node are replaced, and anyone with your old public key"
    + " will need your new one." ) )
      return;

   keys_start( );
}

function keys_forget( )
{
   if( !confirm( "Forget your keys on this browser? To use them here again you'll need your twelve words." ) )
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
   document.getElementById( "keys_random_cancel" ).addEventListener( "click", keys_cancel );
   document.getElementById( "keys_random_next" ).addEventListener( "click", keys_make_words );
   document.getElementById( "keys_words_cancel" ).addEventListener( "click", keys_cancel );
   document.getElementById( "keys_words_next" ).addEventListener( "click", keys_draw_check );
   document.getElementById( "keys_check_back" ).addEventListener( "click", function( ) { keys_show( "keys_words" ); } );
   document.getElementById( "keys_save" ).addEventListener( "click", function( ) { keys_save( "" ); } );
   document.getElementById( "keys_enter_go" ).addEventListener( "click", keys_enter_go );
   document.getElementById( "keys_enter_new" ).addEventListener( "click", keys_start_again );
   document.getElementById( "keys_forget" ).addEventListener( "click", keys_forget );
   document.getElementById( "keys_save_again" ).addEventListener( "click", function( ) { keys_save( keys_kept_here( ) ); } );

   document.getElementById( "keys_written" ).addEventListener( "change", function( event )
   {
      document.getElementById( "keys_words_next" ).disabled = !event.target.checked;
   } );
}

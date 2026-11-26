// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Moving to a new phone (2026-11-23: "an option to 'transfer' a device ... suitable for when you've bought a phone to
// replace your current one ... via QR code and not requiring a password to be entered"). Every account saved on this browser with
// its password moves - the PIN, the saved password, the keys' access key, the settings - and the device token with them, which the
// saved passwords were hashed with: to the node the new phone is the same device, in new hands.
//
// The old phone, signed in, shows one code (Devices, "Move this device") - this page's address and a secret after "#", which the
// node never sees. The new phone's camera opens it here, and it answers through a place on the node made from the secret
// ("move_request( )" in "ciyam_web_session.cpp") with a one-off public key. The old phone asks "Move these accounts to it?"; once
// told yes it seals what moves under the X25519 secret between the two phones' one-off keys and the code's own secret, leaves it on
// the node and signs out - the new phone's signing in would end its session anyway. The new phone signs in with each account, keeps
// those that did, and says which through the node - only then does the old phone offer to clear them ("Not now" leaves a reminder on
// its sign in, "signin_show_moved( )").
//
// Nothing moves until the old phone's owner says so; a second phone answering the same code (one that photographed it) stops the
// move; the code lapses in two minutes unanswered. The node can't open what passes through it, nor stand in for either phone: the
// key is made with the code's secret too.

const c_move_watch_ms = 2000;

// NOTE: How long each phone waits on the other - the node keeps the place ten minutes once answered.
const c_move_answer_wait_ms = 2 * 60 * 1000;
const c_move_finish_wait_ms = 10 * 60 * 1000;

const c_move_qr_size = 200;

const c_move_id_label = "ciyam move id";
const c_move_key_label = "ciyam move key ";

// NOTE: The move under way - "side" "old" (showing the code) or "new" (arrived by it), and what each keeps: "secret" (the code's),
// "id", "owner" (the old phone's own secret), "seed" and "proof" (the new phone's one-off key and secret), "key" (once both are
// known), "pins" and "names" (the accounts), "until" (when it gives up waiting). "turn" counts each start or cancel, so an answer
// to an earlier one isn't drawn into a later.
var g_move = null;
var g_move_turn = 0;

// NOTE: An address with a code's secret, taken out of the address before anything else reads it - and the move begun once the page
// is ready ("move_take_link( )" from "account_main( )").
var g_move_arrived = "";

function move_showing( )
{
   return ( g_move !== null ) || ( g_move_arrived !== "" );
}

// NOTE: True if the address was a new phone's way here - the secret kept, the address cleared.
function move_take_link( )
{
   var secret = parse_move_link( window.location.hash );

   if( secret === "" )
      return false;

   g_move_arrived = secret;

   history.replaceState( null, "", window.location.pathname + window.location.search );

   return true;
}

// NOTE: The one card, drawn for a step - "title", "lede", "qr" (an address shown as a code), "names" (a list), "note", "error",
// "busy" (text), and the buttons "go" and "other" as "{ text, act }".
function move_draw( step )
{
   var names = document.getElementById( "move_names" );
   var qr = document.getElementById( "move_qr" );

   document.getElementById( "move_title" ).textContent = step.title || "";
   document.getElementById( "move_lede" ).textContent = step.lede || "";

   qr.replaceChildren( );
   qr.hidden = !step.qr;

   if( step.qr )
   {
      new QRCode( qr, { text: step.qr, width: c_move_qr_size, height: c_move_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
       correctLevel: QRCode.CorrectLevel.M } );

      qr.removeAttribute( "title" );
      qr.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );
   }

   names.replaceChildren( );
   names.hidden = !step.names || ( step.names.length === 0 );

   ( step.names || [ ] ).forEach( function( name )
   {
      var item = document.createElement( "li" );

      item.textContent = name;
      names.appendChild( item );
   } );

   document.getElementById( "move_note" ).textContent = step.note || "";
   document.getElementById( "move_note" ).hidden = !step.note;

   set_error( "move_error", step.error || "" );

   document.getElementById( "move_busy_text" ).textContent = step.busy || "";
   document.getElementById( "move_busy" ).hidden = !step.busy;

   [ "go", "other" ].forEach( function( which )
   {
      var button = document.getElementById( "move_" + which );

      button.hidden = !step[ which ];
      button.onclick = step[ which ] ? step[ which ].act : null;
      button.textContent = step[ which ] ? step[ which ].text : "";
      button.disabled = false;
   } );

   show_view( "move_view" );

   var focus = step.go ? document.getElementById( "move_go" ) : document.getElementById( "move_title" );

   focus.focus( { preventScroll: true } );
}

// NOTE: Its buttons off while a press is acted on, so it isn't acted on twice.
function move_hold( )
{
   document.getElementById( "move_go" ).disabled = true;
   document.getElementById( "move_other" ).disabled = true;
}

// ---- What both phones use

async function move_secure( )
{
   return await keys_load_scripts( ) ? keys_module( "secure_message" ) : null;
}

async function move_sha256( sm, text )
{
   return sm.hex_of( await sm.sha256_bytes( new TextEncoder( ).encode( text ) ) );
}

// NOTE: Sealed as "ciyam_secure_message.js" seals a message (ChaCha20, then an HMAC of what it made) but without its padding to a
// thousand bytes, so more fits - "<salt>.<sealed>.<check>". Opened, "" if it isn't what was sealed under "key".
async function move_seal( sm, key, text )
{
   var salt = sm.hex_of( sm.random_bytes( 16 ) );
   var sealed = sm.hex_of( await sm.chacha20( ).crypt( key + salt, new TextEncoder( ).encode( text ) ) );

   return salt + "." + sealed + "." + ( await sm.hmac_sha256( "mac" + key + salt, sealed ) ).substring( 0, 32 );
}

async function move_open( sm, key, text )
{
   var parts = String( text || "" ).split( "." );

   if( ( parts.length !== 3 ) || !/^[0-9a-f]{32}$/.test( parts[ 0 ] ) || !/^([0-9a-f]{2})+$/.test( parts[ 1 ] )
    || !/^[0-9a-f]{32}$/.test( parts[ 2 ] ) )
      return "";

   var expected = ( await sm.hmac_sha256( "mac" + key + parts[ 0 ], parts[ 1 ] ) ).substring( 0, 32 );
   var differ = 0;

   for( var i = 0; i < 32; i++ )
      differ |= expected.charCodeAt( i ) ^ parts[ 2 ].charCodeAt( i );

   if( differ )
      return "";

   var bytes = new Uint8Array( parts[ 1 ].match( /../g ).map( function( pair ) { return parseInt( pair, 16 ); } ) );

   try
   {
      return new TextDecoder( "utf-8", { fatal: true } ).decode( await sm.chacha20( ).crypt( key + parts[ 0 ], bytes ) );
   }
   catch( e )
   {
      return "";
   }
}

// NOTE: The key both phones make - from the X25519 secret between their one-off keys, and the code's secret, so that the node,
// which passes the public keys on, can't put one of its own in their place.
async function move_key( sm, seed, public_key, secret )
{
   return await sm.hmac_sha256( sm.shared_secret_of_seed( seed, public_key ), c_move_key_label + secret );
}

// NOTE: Asked of the place on the node without a session - "options" as "move_request( )" takes them. Not logged unless polling is.
function move_ask( method, options, quiet )
{
   return request( function( done )
   {
      g_request_quiet = !!quiet;

      var pending = ciyam.fetch( ciyam.get_cws_url( ) + "/move?options=" + encodeURIComponent( options ) + "&format=text", method, done );

      g_request_quiet = false;

      return pending;
   } );
}

function move_wait( ms )
{
   return new Promise( function( resolve ) { window.setTimeout( resolve, ms ); } );
}

// NOTE: What the node said, as a sentence - its own words for a refusal, else "fallback".
function move_problem( response, fallback )
{
   return ( is_error_response( response ) && ( response !== c_no_answer ) ) ? error_text( response ) : fallback;
}

// ---- The old phone - Devices, "Move this device"

function move_saved_values( )
{
   var values = { };

   try
   {
      for( var i = 0; i < localStorage.length; i++ )
      {
         var key = localStorage.key( i );

         values[ key ] = localStorage.getItem( key );
      }
   }
   catch( e )
   {
   }

   return values;
}

async function move_start( )
{
   var turn = ++g_move_turn;
   var hashes = { };

   set_error( "move_start_error", "" );

   signin_saved_pins( ).forEach( function( pin ) { hashes[ pin ] = signin_saved_hash( pin ); } );

   var pins = move_pins( signin_storage_get( c_signin_storage_access ), hashes );

   if( pins.length === 0 )
   {
      set_error( "move_start_error", "Sign in with the PIN and the password remembered first - only a saved account can move." );

      return;
   }

   var sm = await move_secure( );

   if( sm === null )
   {
      set_error( "move_start_error", "This browser couldn't load what moving needs - reload and try again." );

      return;
   }

   var secret = keys_random_seed( );
   var owner = keys_random_seed( );
   var id = await sm.hmac_sha256( secret, c_move_id_label );
   var owner_hash = await move_sha256( sm, owner );

   var response = await request( function( done )
   {
      return ciyam.fetch( ciyam.get_cws_url( ) + "/move?options=" + encodeURIComponent( "id=" + id + ";owner=" + owner_hash ) + "&access="
       + ciyam.access + "&device=" + ciyam.device + "&format=text&session=" + ciyam.sessid, "POST", done );
   } );

   if( turn !== g_move_turn )
      return;

   if( problem_text( response ) !== "" )
   {
      set_error( "move_start_error", problem_text( response ) );

      return;
   }

   g_move = { side: "old", turn: turn, secret: secret, id: id, owner: owner, pins: pins, names: pins.map( function( pin )
   {
      return signin_saved_name( pin ) || pin;
   } ), until: Date.now( ) + c_move_answer_wait_ms };

   var link = move_link( window.location.origin + window.location.pathname, secret );

   move_draw( { title: "Move this device", lede: "Point your new device's camera at this code - or copy the link, and paste it on"
    + " its sign in.", qr: link, busy: "Waiting for your new device…", go: { text: "Copy link", act: function( ) { move_copy_link( link ); } },
    other: { text: "Cancel", act: move_cancel } } );

   move_watch_old( turn );
}

// NOTE: The code's link by the clipboard, for a new device with no camera (2026-11-26: "I can't easily scan the QR code (without
// using a phone ...) so the ability to optionally also use the clipboard would be helpful"). It holds what the code holds; the new
// device pastes it on its sign in, at whatever address it uses for the node ("signin_move_go( )"). The check code still guards it.
// Where the browser won't copy (plain http on a LAN address), the link is shown to copy by hand.
async function move_copy_link( link )
{
   var note = document.getElementById( "move_note" );
   var copied = false;

   try
   {
      await navigator.clipboard.writeText( link );

      copied = true;
   }
   catch( e )
   {
      copied = false;
   }

   if( ( g_move === null ) || ( g_move.side !== "old" ) || g_move.asked )
      return;

   note.textContent = copied ? "Copied - paste it on your new device, under \"Moving from another device?\" on its sign in."
    : "This browser won't copy it - copy it from here: " + link;
   note.hidden = false;
}

// NOTE: Back where it started - the place on the node left to lapse.
function move_cancel( )
{
   ++g_move_turn;

   g_move = null;
   g_move_arrived = "";

   route( );
}

// NOTE: Asked until the new phone answers - then the question, still asked while it shows - or the code lapses, or another phone
// answers too. Not once the accounts are on their way ("sending").
async function move_watch_old( turn )
{
   while( ( turn === g_move_turn ) && !g_move.sending )
   {
      await move_wait( c_move_watch_ms );

      if( ( turn !== g_move_turn ) || g_move.sending )
         return;

      var response = await move_ask( "GET", "id=" + g_move.id + ";owner=" + g_move.owner, true );

      if( ( turn !== g_move_turn ) || g_move.sending )
         return;

      var shown = parse_move_state( response );

      if( shown.state === "answered" )
      {
         if( !g_move.asked )
            await move_ask_old( shown.key );

         if( ( turn !== g_move_turn ) || g_move.sending )
            return;

         g_move.asked = true;
         g_move.until = Date.now( ) + c_move_finish_wait_ms;

         continue;
      }

      if( shown.state === "contested" )
      {
         move_draw_again( "Another device answered this code too, so nothing was moved." );

         return;
      }

      if( ( ( shown.state === "error" ) && ( response !== c_no_answer ) ) || ( Date.now( ) > g_move.until ) )
      {
         move_draw_again( g_move.asked ? "The code lapsed." : "The code lapsed before a new device answered it." );

         return;
      }
   }
}

function move_draw_again( lede )
{
   ++g_move_turn;

   g_move = null;

   move_draw( { title: "Start again", lede: lede, go: { text: "Show a new code", act: move_start }, other: { text: "Cancel",
    act: move_cancel } } );
}

// NOTE: Its check code first ("move_check_code( )"), so the phone answering is the one in hand.
async function move_ask_old( key )
{
   var turn = g_move_turn;
   var one = ( g_move.names.length === 1 );
   var sm = await move_secure( );
   var code = ( sm === null ) ? "" : move_check_code( await move_sha256( sm, g_move.secret + "." + key ) );

   if( turn !== g_move_turn )
      return;

   if( code === "" )
   {
      move_draw_again( "This browser couldn't load what moving needs - reload and try again." );

      return;
   }

   move_draw( { title: "Your new device is ready", lede: "Check it shows " + code + ", then move " + ( one ? "this account" : "these accounts" )
    + " to it.",
    names: g_move.names, go: { text: one ? "Move it" : "Move them", act: function( ) { move_send( key ); } },
    other: { text: "Cancel", act: move_cancel } } );
}

// NOTE: Sealed for the new phone and left on the node - then signed out here, as the new phone's signing in would end this session.
async function move_send( new_key )
{
   var turn = g_move_turn;

   g_move.sending = true;

   move_hold( );

   var sm = await move_secure( );

   var seed = keys_random_seed( );
   var text = move_bundle_text( ciyam.device, g_move.pins, g_move.names, move_saved_values( ) );

   if( ( sm === null ) || ( text === "" ) )
   {
      move_draw_again( ( sm === null ) ? "This browser couldn't load what moving needs - reload and try again."
       : "Too much is kept for these accounts to move at once." );

      return;
   }

   var key = "";

   try
   {
      key = await move_key( sm, seed, new_key, g_move.secret );
   }
   catch( e )
   {
      move_draw_again( "What your new device sent couldn't be read." );

      return;
   }

   var bundle = sm.public_key_of_seed( seed ) + "." + await move_seal( sm, key, text );

   seed = "";
   text = "";

   move_draw( { title: "Moving…", lede: "Keep this open until your new device is signed in.", busy: "Finishing on your new device…" } );

   // NOTE: Signed out first, so that ending this session can't cross the new phone's sign in on the same device.
   await request( function( done ) { return ciyam.disconnect( done ); } );

   clear_session( );

   var response = await move_ask( "POST", "id=" + g_move.id + ";owner=" + g_move.owner + ";bundle=" + bundle );

   if( turn !== g_move_turn )
      return;

   if( problem_text( response ) !== "" )
   {
      move_finish_old( { title: "Nothing moved", lede: "Sign in and try again.", error: move_problem( response, "The node didn't take it." ) } );

      return;
   }

   g_move.key = key;
   g_move.until = Date.now( ) + c_move_finish_wait_ms;

   move_watch_sent( turn );
}

// NOTE: Asked until the new phone says how it went - or it gives up, and nothing here is cleared.
async function move_watch_sent( turn )
{
   while( turn === g_move_turn )
   {
      await move_wait( c_move_watch_ms );

      if( turn !== g_move_turn )
         return;

      var response = await move_ask( "GET", "id=" + g_move.id + ";owner=" + g_move.owner, true );

      if( turn !== g_move_turn )
         return;

      var shown = parse_move_state( response );

      if( shown.state === "done" )
      {
         await move_told( shown.report );

         return;
      }

      if( ( shown.state === "contested" ) || ( ( shown.state === "error" ) && ( response !== c_no_answer ) )
       || ( Date.now( ) > g_move.until ) )
      {
         move_finish_old( { title: "Your new device didn't finish", lede: "Nothing was cleared here - sign in and try again." } );

         return;
      }
   }
}

function move_finish_old( step )
{
   ++g_move_turn;

   g_move = null;

   step.go = { text: "Done", act: move_cancel };

   move_draw( step );
}

async function move_told( sealed )
{
   var sm = await move_secure( );
   var report = ( sm === null ) ? null : parse_move_report( await move_open( sm, g_move.key, sealed ), g_move.pins );

   if( report === null )
   {
      move_finish_old( { title: "Your new device didn't finish", lede: "Nothing was cleared here - sign in and try again." } );

      return;
   }

   if( report.done.length === 0 )
   {
      move_finish_old( { title: "Nothing moved", lede: "Your accounts wouldn't sign in on your new device, so they're still here." } );

      return;
   }

   var names_of = function( pins ) { return pins.map( function( pin ) { return g_move.names[ g_move.pins.indexOf( pin ) ]; } ); };
   var moved = { pins: report.done, names: names_of( report.done ) };
   var stayed = names_of( report.failed );

   move_draw( { title: "Your new device is signed in", lede: "Clear " + move_names_text( moved.names ) + " from this device?",
    note: ( stayed.length === 0 ) ? "" : move_names_text( stayed ) + " didn't sign in there, so " + ( ( stayed.length === 1 ) ? "stays"
    : "stay" ) + " here.", go: { text: "Clear this device", act: function( ) { move_clear( moved ); } }, other: { text: "Not now",
    act: function( ) { move_later( moved ); } } } );
}

function move_keep_moved( moved )
{
   try
   {
      localStorage.setItem( c_signin_moved_name, JSON.stringify( moved ) );
   }
   catch( e )
   {
   }
}

function move_clear( moved )
{
   move_keep_moved( moved );
   signin_clear_moved( );

   move_finish_old( { title: "This device is cleared", lede: move_names_text( moved.names ) + ( ( moved.names.length === 1 ) ? " is" : " are" )
    + " on your new device now." } );
}

// NOTE: Kept here for now, said on the sign in until cleared ("signin_show_moved( )").
function move_later( moved )
{
   move_keep_moved( moved );
   signin_fill_saved( );

   move_cancel( );
}

// ---- The new phone - arrived by the old phone's code

async function move_arrive( )
{
   var secret = g_move_arrived;
   var turn = ++g_move_turn;

   g_move_arrived = "";

   if( secret === "" )
      return;

   g_move = { side: "new", turn: turn, secret: secret };

   move_draw( { title: "Moving to this device", busy: "Getting ready…" } );

   if( signin_saved_pins( ).length > 0 )
   {
      move_finish_new( { title: "This browser already has accounts", lede: "Use a browser with none, or clear this one with Reset this browser." } );

      return;
   }

   var sm = await move_secure( );

   if( sm === null )
   {
      move_finish_new( { title: "That didn't work", lede: "This browser couldn't load what moving needs - reload and try again." } );

      return;
   }

   g_move.seed = keys_random_seed( );
   g_move.proof = keys_random_seed( );
   g_move.id = await sm.hmac_sha256( secret, c_move_id_label );
   g_move.until = Date.now( ) + c_move_finish_wait_ms;

   var response = await move_ask( "POST", "id=" + g_move.id + ";key=" + sm.public_key_of_seed( g_move.seed ) + ";proof="
    + await move_sha256( sm, g_move.proof ) );

   if( turn !== g_move_turn )
      return;

   if( problem_text( response ) !== "" )
   {
      move_finish_new( { title: "That didn't work", lede: move_problem( response, "The node didn't answer - start again on your old"
       + " device." ) } );

      return;
   }

   var code = move_check_code( await move_sha256( sm, secret + "." + sm.public_key_of_seed( g_move.seed ) ) );

   if( turn !== g_move_turn )
      return;

   move_draw( { title: "Moving to this device", lede: "Check your old device shows " + code + ", then confirm there.", busy: "Waiting for your old device…",
    other: { text: "Cancel", act: move_cancel } } );

   move_watch_new( turn, sm );
}

function move_finish_new( step )
{
   ++g_move_turn;

   g_move = null;

   step.go = step.go || { text: "Go to sign in", act: move_cancel };

   move_draw( step );
}

// NOTE: Asked until what the old phone sealed arrives - handed over once - or the place lapses.
async function move_watch_new( turn, sm )
{
   while( turn === g_move_turn )
   {
      await move_wait( c_move_watch_ms );

      if( turn !== g_move_turn )
         return;

      var response = ( await move_ask( "GET", "id=" + g_move.id + ";proof=" + g_move.proof, true ) ).trim( );

      if( turn !== g_move_turn )
         return;

      if( /^[0-9a-f]{64}\.[0-9a-f.]+$/.test( response ) )
      {
         await move_take( turn, sm, response );

         return;
      }

      if( ( is_error_response( response ) && ( response !== c_no_answer ) ) || ( Date.now( ) > g_move.until ) )
      {
         move_finish_new( { title: "That didn't work", lede: move_problem( response, "The code lapsed - start again on your old device." ) } );

         return;
      }
   }
}

// NOTE: Opened, and each account signed in with - the first that does stays signed in here, the rest checked by a sign in of their
// own, then signed out - and those that did kept, as the old phone kept them. The old phone told which, then this one's done.
async function move_take( turn, sm, sealed )
{
   var at = sealed.indexOf( "." );
   var bundle = null;

   try
   {
      var key = await move_key( sm, g_move.seed, sealed.substring( 0, at ), g_move.secret );

      g_move.key = key;
      bundle = parse_move_bundle( await move_open( sm, key, sealed.substring( at + 1 ) ) );
   }
   catch( e )
   {
      bundle = null;
   }

   g_move.seed = "";

   if( bundle === null )
   {
      move_finish_new( { title: "That didn't work", lede: "What arrived couldn't be opened - start again on your old device." } );

      return;
   }

   move_draw( { title: "Moving to this device", busy: "Signing in…" } );

   if( ciyam.sessid !== "" )
   {
      await request( function( done ) { return ciyam.disconnect( done ); } );

      clear_session( );
   }

   ciyam.device = bundle.device;

   var done = [ ];
   var failed = [ ];

   for( var i = 0; i < bundle.pins.length; i++ )
   {
      var pin = bundle.pins[ i ];
      var hashed = bundle.store[ "cws.hashed_" + pin ];

      if( done.length === 0 )
      {
         await connect_as( pin, hashed, "" );

         ( ( ciyam.sessid !== "" ) ? done : failed ).push( pin );

         continue;
      }

      var other = new CIYAM( window.location.protocol + "//" + window.location.host, false, true );

      await other.connect( pin, bundle.device, hashed, "", function( ) { } );

      if( other.sessid === "" )
         failed.push( pin );
      else
      {
         done.push( pin );

         await other.disconnect( function( ) { } );
      }
   }

   if( turn !== g_move_turn )
      return;

   // NOTE: The session's hash version read again once kept - it was signed in before the bundle's was stored.
   if( done.length > 0 )
   {
      move_keep_here( bundle, done );

      ciyam.hash_version = ciyam.hash_version_for( ciyam.access );
   }

   var report = await move_seal( sm, g_move.key, JSON.stringify( { done: done, failed: failed } ) );

   await move_ask( "POST", "id=" + g_move.id + ";proof=" + g_move.proof + ";report=" + report );

   if( turn !== g_move_turn )
      return;

   if( done.length === 0 )
   {
      move_finish_new( { title: "Nothing moved", lede: "Your accounts wouldn't sign in here, so they're still on your old device." } );

      return;
   }

   signin_keep_session( );

   var names = done.map( function( pin ) { return bundle.store[ "cws.name_" + pin ] || pin; } );
   var stayed = failed.map( function( pin ) { return bundle.store[ "cws.name_" + pin ] || pin; } );

   move_finish_new( { title: "You're signed in", lede: move_names_text( names ) + ( ( names.length === 1 ) ? " is" : " are" )
    + " on this device now.", note: ( ( stayed.length === 0 ) ? "" : move_names_text( stayed ) + " stayed on your old device. " )
    + "Calls ring through the ntfy app - set it up again under Notifications.", go: { text: "Go to Home", act: function( )
    {
       window.location.href = "home.html";
    } }, other: { text: "Set up call alerts", act: function( )
    {
       history.replaceState( null, "", window.location.pathname + window.location.search + "#notify" );

       enter_app( );
    } } } );
}

// NOTE: What moved kept on this browser for the accounts that signed in here ("done") - remembered with their passwords, as on the
// old phone.
function move_keep_here( bundle, done )
{
   var names = done.map( function( pin ) { return bundle.store[ "cws.name_" + pin ] || ""; } );
   var keys = move_keys_for( Object.keys( bundle.store ), done, names );

   try
   {
      localStorage.setItem( c_signin_storage_device, bundle.device );

      keys.needed.concat( keys.extra ).forEach( function( key ) { localStorage.setItem( key, bundle.store[ key ] ); } );

      localStorage.setItem( c_signin_storage_access, format_access_list( done ) );
   }
   catch( e )
   {
   }
}

// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The Home app - the node's starting point (Ian, 2026-10-03; "Home App Scope" in the vault). Before
// anyone signs in, "/system" decides the screen: the sign in, Unlock, or Set up. The logic is in
// "home_parse.js", with its tests; the sign in is the shared one in "signin.js".

// NOTE: How long, after an unlock, to wait for the node to say it is ready - its services start first.
const c_ready_wait_ms = 30000;

const c_ready_poll_ms = 1500;

const c_no_answer = "Error: The server did not answer - check the connection and try again.";

const c_views = [ "checking_view", "unreachable_view", "setup_view", "signin_view", "unlock_view", "app_view" ];

const c_pill_classes = [ "is-none", "is-encrypted", "is-quantum", "is-ready", "is-locked", "is-new" ];

var g_queue = Promise.resolve( );

var g_system = parse_system( "" );

// NOTE: One request at a time - "ciyam.js" keeps a single callback per instance, so a second request in
// flight would take the first one's answer (ISS-005). As the accounts page's "request( )": resolves once
// the call has finished, with the last answer its callback was given; no answer at all is an error.
function request( issue )
{
   return new Promise( function( resolve )
   {
      g_queue = g_queue.then( function( )
      {
         var answered = false;

         var last = "";

         return Promise.resolve( issue( function( response )
         {
            answered = true;

            last = String( response );
         } ) ).then( function( ) { resolve( answered ? last : c_no_answer ); } );
      } ).catch( function( e )
      {
         resolve( "Error: " + ( e && e.message ? e.message : "the request failed." ) );
      } );
   } );
}

function home_main( )
{
   // NOTE: The eye in each new password's field, to see it - one suggested above all (2026-10-25).
   attach_reveals( document );

   signin_build( document.getElementById( "signin_host" ), {
      note: "home",
      setup_href: "account.html#welcome",
      request: request,
      on_signed_in: on_signed_in,
      error_text: function( error ) { return ( g_system.state === "locked" ) ? locked_sign_in_text( error, true ) : ""; }
   } );

   document.getElementById( "unreachable_retry" ).addEventListener( "click", check_node );
   document.getElementById( "setup_retry" ).addEventListener( "click", check_node );
   document.getElementById( "unlock_form" ).addEventListener( "submit", do_unlock );
   document.getElementById( "rescue_form" ).addEventListener( "submit", do_rescue );
   document.getElementById( "unlock_sign_out" ).addEventListener( "click", do_sign_out );
   // NOTE: "Sign out" - here and in the switcher alike, every tab on the session told (2026-10-08).
   document.getElementById( "app_sign_out" ).addEventListener( "click", apps_sign_out_everywhere );

   install_setup( );
   install_shell( );
   install_member_home( );
   install_admin( );
   install_console( );

   var source = linking_source( );

   if( source !== "" )
      start_linking( source );
   else
      check_node( );
}

function show_view( id )
{
   c_views.forEach( function( view )
   {
      var element = document.getElementById( view );

      if( element )
         element.hidden = ( view !== id );
   } );

   // NOTE: The arriving screens carry the pills above them; signed in, the top bar does.
   document.getElementById( "arrive_bar" ).hidden = ( id === "app_view" ) || ( id === "checking_view" );

   // NOTE: Leaving setting up by another way than its own views - its bubbles and its still-alive timer stop (found by review).
   if( id !== "setup_view" )
   {
      if( g_setup_bubbles !== null )
         g_setup_bubbles.stop( );
      setup_alive_stop( );
   }
}

function set_text( id, text )
{
   document.getElementById( id ).textContent = text;
}

function set_pill( id, kind, text )
{
   var pill = document.getElementById( id );

   c_pill_classes.forEach( function( name ) { pill.classList.remove( name ); } );

   if( kind !== "" )
      pill.classList.add( "is-" + kind );

   pill.textContent = text;
   pill.hidden = ( text === "" );
}

async function fetch_text( path )
{
   try
   {
      var response = await fetch( path, { cache: "no-store" } );

      return await response.text( );
   }
   catch( e )
   {
      return "";
   }
}

async function read_system( )
{
   g_system = parse_system( await fetch_text( "/system" ) );

   var security = security_text( g_system.security );

   set_pill( "security_pill", g_system.security, security.pill );
   set_pill( "app_security_pill", g_system.security, security.pill );

   var states = { ready: "Node ready", locked: "Locked", new: "Not set up" };

   set_pill( "state_pill", g_system.state, states[ g_system.state ] || "" );

   return g_system;
}

// NOTE: What the visitor arrives at, from the node's state.
async function check_node( )
{
   await read_system( );

   var screen = arriving_screen( g_system.state );

   if( ( ciyam.sessid !== "" ) && ( ( screen === "signin" ) || ( screen === "unlock" ) ) )
   {
      after_sign_in( );

      return;
   }

   // NOTE: A reload, or an app switched to Home in this tab - the session kept for the tab, if the node still knows it.
   if( ( screen === "signin" ) && await signin_resume( ) )
   {
      after_sign_in( );

      return;
   }

   if( screen === "signin" )
   {
      signin_describe( c_signin_title, c_signin_lede, true );
      signin_show( );
      show_view( "signin_view" );
   }
   else if( screen === "unlock" )
   {
      signin_describe( "This node is locked", "It restarted. Sign in first - with your own account; it does not have to be"
       + " admin's - then use one of the unlock keys you kept.", false );
      signin_show( );
      show_view( "signin_view" );
   }
   else if( screen === "setup" )
      setup_open( );
   else
      show_view( "unreachable_view" );
}

function on_signed_in( )
{
   after_sign_in( );
}

function clear_ciyam( )
{
   ciyam.sessid = "";
   ciyam.access = "";
   ciyam.hashed = "";
   ciyam.unique = "";
   ciyam.username = "";
   ciyam.is_admin = false;
}

async function after_sign_in( )
{
   await read_system( );

   if( g_system.state === "locked" )
   {
      set_text( "unlock_error", "" );
      set_text( "unlock_done", "" );
      set_text( "rescue_error", "" );

      document.getElementById( "unlock_key" ).value = "";
      document.getElementById( "rescue_password" ).value = "";
      document.getElementById( "rescue_warning" ).hidden = !warns_master_password( g_system.security );

      show_view( "unlock_view" );

      return;
   }

   enter_app( );
}

// ====================================================================
// Setting up a new node (Damon and Ian, 2026-10-12 - "Home App Scope", Setting up a new node)
// ====================================================================

// NOTE: The node takes two requests: "access=admin" answers admin's PIN and the entropy it suggests, then the PIN with the
// master password's hash and the entropy chosen answers admin's first device. A second first request is refused, so between
// the two the page must finish - the password is asked before the first is sent, and the PIN and the suggestion are kept for
// the tab ("setup_kept( )") so a reload can still finish, with new words. The words are the node's suggestion mixed with the
// person's randomness by Ian's "xor_hex_data" - never words they type: the twelfth is a checksum, and people choose poor
// words (Ian, 2026-10-12) - typing them is for restoring.
const c_setup_views = [ "setup_intro", "setup_new", "setup_random", "setup_password", "setup_words", "setup_check", "setup_done", "setup_resume",
 "setup_elsewhere", "setup_expired" ];

// NOTE: "Step N of 5" - the views that are steps.
const c_setup_steps = { setup_random: 1, setup_password: 2, setup_words: 3, setup_check: 4, setup_done: 5 };

const c_setup_pops = 12;

const c_setup_kept_key = "ciyam.setup";

// NOTE: Twelve words are 128 bits - 32 hex characters, the length the node suggests.
const c_setup_entropy_length = 32;

const c_setup_most_bubbles = 16;

var g_setup = null;

// NOTE: The node waits five minutes from its last word with this tab, then lets admin's PIN go so setting up can start
// over ("@utm_web_reset", Ian, 2026-10-12). "/ip-addr?token=<PIN>", from the address the PIN was given to, gives it another
// five ("2a99baee") - sent when the PIN comes, at each step, every minute and on coming back to the tab, so an open tab
// never runs out. It answers this browser's address either way: whether it was in time is known only at the end.
const c_setup_alive_ms = 60 * 1000;

var g_setup_alive_timer = null;
var g_setup_bubbles = null;

function setup_fresh( )
{
   return { samples: [ ], pops: 0, password: "", pin: "", suggested: "", entropy: "", words: [ ], right: 0 };
}

function setup_kept_read( )
{
   try
   {
      return setup_kept( sessionStorage.getItem( c_setup_kept_key ) );
   }
   catch( e )
   {
      return null;
   }
}

function setup_kept_write( )
{
   try
   {
      sessionStorage.setItem( c_setup_kept_key, JSON.stringify( { pin: g_setup.pin, suggested: g_setup.suggested } ) );
   }
   catch( e )
   {
   }
}

function setup_kept_forget( )
{
   try
   {
      sessionStorage.removeItem( c_setup_kept_key );
   }
   catch( e )
   {
   }
}

async function setup_keep_alive( )
{
   if( ( g_setup === null ) || ( g_setup.pin === "" ) )
      return;

   var reply = await fetch_text( "/ip-addr?token=" + setup_alive_token( g_setup.pin, g_setup.suggested, hex_sha256 ) );

   // NOTE: The node no longer waiting - said now, not at Finish (Ian's answer, 2026-10-13): a tab back after too long, or a
   // node restarted. Only while still being kept alive - not once Finish is under way, when the token is let go.
   if( ( setup_alive_answer( reply ) === "gone" ) && ( g_setup_alive_timer !== null ) )
   {
      setup_kept_forget( );
      setup_show( "setup_expired" );
   }
}

function setup_alive_start( )
{
   setup_alive_stop( );

   setup_keep_alive( );

   g_setup_alive_timer = window.setInterval( setup_keep_alive, c_setup_alive_ms );
}

function setup_alive_stop( )
{
   if( g_setup_alive_timer !== null )
      window.clearInterval( g_setup_alive_timer );

   g_setup_alive_timer = null;
}

// NOTE: A new node - from the start, or where a reload left it.
function setup_open( )
{
   setup_alive_stop( );

   if( g_setup_bubbles !== null )
      g_setup_bubbles.clear( );

   g_setup = setup_fresh( );

   setup_clear_fields( );

   show_view( "setup_view" );

   setup_show( setup_kept_read( ) !== null ? "setup_resume" : "setup_intro" );

   // NOTE: The word list is read in now, while the person reads - it is needed for the words and for checking them.
   recover_load_bip39( );
}

function setup_show( view )
{
   c_setup_views.forEach( function( name ) { document.getElementById( name ).hidden = ( name !== view ); } );

   var step = c_setup_steps[ view ] || 0;

   document.getElementById( "setup_progress" ).hidden = ( step === 0 );

   set_text( "setup_step", step > 0 ? "Step " + step + " of 5" : "" );

   document.querySelectorAll( "#setup_progress .home-setup-bars span" ).forEach( function( bar, i )
   {
      bar.classList.toggle( "is-done", i < step );
   } );

   if( view === "setup_random" )
      setup_bubbles( ).start( );
   else if( g_setup_bubbles !== null )
      g_setup_bubbles.stop( );

   // NOTE: Each step tells the node this tab is still here; done, or let go, it stops.
   if( ( view === "setup_done" ) || ( view === "setup_expired" ) || ( view === "setup_elsewhere" ) )
      setup_alive_stop( );
   else
      setup_keep_alive( );

   // NOTE: Over plain HTTP the master password is said to be for your own network only.
   if( view === "setup_password" )
      document.getElementById( "setup_http_warning" ).hidden = !warns_master_password( g_system.security );

   // NOTE: The master password's own field, for typing straight away (Damon, 2026-10-13); elsewhere the view's heading.
   var first = ( view === "setup_password" ) ? document.getElementById( "setup_password_new" ) : document.querySelector( "#" + view + " h2" );

   if( first !== null )
      first.focus( { preventScroll: true } );
}

// NOTE: Recovering a node onto this one, from its twelve words (Damon, 2026-10-13) - the shared recovery, worded for a
// node not set up; recovered, on into Home signed in as admin, as after a sign in. Back comes back here.
function setup_recover( )
{
   document.getElementById( "setup_view" ).hidden = true;

   recover_show( { fresh: true, on_back: function( )
   {
      show_view( "setup_view" );
      setup_show( "setup_intro" );
   } } );
}

function setup_clear_fields( )
{
   [ "setup_password_new", "setup_password_confirm" ].forEach( function( id ) { document.getElementById( id ).value = ""; } );

   [ "setup_written", "setup_pin_written" ].forEach( function( id ) { document.getElementById( id ).checked = false; } );

   document.getElementById( "setup_retain" ).value = c_retain_access;

   [ "setup_password_error", "setup_password_status", "setup_finish_error", "setup_finish_status", "setup_pin" ]
    .forEach( function( id ) { set_text( id, "" ); } );

   [ "setup_word_list", "setup_check_rows" ].forEach( function( id ) { document.getElementById( id ).replaceChildren( ); } );

   document.getElementById( "setup_strength" ).hidden = true;
   document.getElementById( "setup_random_next" ).disabled = true;
   document.getElementById( "setup_words_next" ).disabled = true;
   document.getElementById( "setup_finish" ).disabled = true;
   document.getElementById( "setup_continue" ).disabled = true;

   setup_count_pops( );
}

// ---- The bubbles

// NOTE: "bubbles.js" (2026-10-25), shared with setting up an account's keys - made once, the first time they are shown, so
// nothing else on Home depends on it.
function setup_bubbles( )
{
   if( g_setup_bubbles === null )
   {
      g_setup_bubbles = bubble_field( document.getElementById( "setup_bubbles" ), { state: function( ) { return g_setup; },
       pops: c_setup_pops, most: c_setup_most_bubbles, on_count: setup_count_pops,
       on_done: function( ) { document.getElementById( "setup_random_next" ).focus( ); } } );
   }

   return g_setup_bubbles;
}

function setup_count_pops( )
{
   var pops = g_setup ? g_setup.pops : 0;

   set_text( "setup_popped", pops + " of " + c_setup_pops + " popped" + ( pops >= c_setup_pops ? " - thank you" : "" ) );

   document.getElementById( "setup_popped_bar" ).style.width = Math.round( pops / c_setup_pops * 100 ) + "%";
   document.getElementById( "setup_random_next" ).disabled = ( pops < c_setup_pops );
}

// ---- The master password, then the words

// NOTE: Enter moves on through the two fields ("setup_password_enter( )"), saying what is wrong where it cannot.
function setup_password_key( event )
{
   if( event.key !== "Enter" )
      return;

   event.preventDefault( );

   var password = document.getElementById( "setup_password_new" ).value;

   var next = setup_password_enter( ( event.target.id === "setup_password_new" ) ? "new" : "confirm", password,
    document.getElementById( "setup_password_confirm" ).value, password_strength( password, [ g_setup.pin ] ).level >= 1 );

   set_text( "setup_password_error", ( next.problem === "short" ) ? "The master password needs at least " + c_password_min_length + " characters."
    : ( next.problem === "different" ? "The two passwords are not the same." : "" ) );

   if( next.focus !== "" )
      document.getElementById( next.focus ).focus( );

   if( next.make )
      document.getElementById( "setup_make_words" ).click( );
}

function setup_update_strength( )
{
   // NOTE: Rated by guesses once "zxcvbn.min.js" is in - read in now, and rated again then.
   load_strength_rules( setup_update_strength );

   var strength = password_strength( document.getElementById( "setup_password_new" ).value, [ g_setup.pin ] );
   var meter = document.getElementById( "setup_strength" );

   meter.hidden = ( strength.level < 0 );
   meter.dataset.level = String( strength.level );

   set_text( "setup_strength_label", strength.text );
}

// NOTE: Ian's "xor_hex_data" - the node's suggestion with the person's randomness, through the module's own "xor( )"
// ("ciyam_module_named( )") rather than setting its global and calling into it.
function setup_mix( suggested, mine )
{
   return ciyam_module_named( "xor_hex_data" ).xor( suggested, mine );
}

function setup_browser_hex( )
{
   var bytes = new Uint8Array( 32 );

   window.crypto.getRandomValues( bytes );

   return Array.from( bytes ).map( function( b ) { return ( b < 16 ? "0" : "" ) + b.toString( 16 ); } ).join( "" );
}

async function setup_make_words( )
{
   var password = document.getElementById( "setup_password_new" ).value;

   set_text( "setup_password_error", "" );

   // NOTE: The rules by guesses are waited for, so a password is never let through by the old ones for being sent before
   // "zxcvbn.min.js" was in (review, 2026-10-25). If it cannot be read, the old ones are what there is.
   await load_strength_rules( );

   var strength = password_strength( password, [ g_setup.pin ] );

   if( strength.level < 1 )
   {
      set_text( "setup_password_error", ( strength.text === "Unsatisfactory" )
       ? "The master password needs at least " + c_password_min_length + " characters." : password_refusal( strength ) );

      return;
   }

   if( password !== document.getElementById( "setup_password_confirm" ).value )
   {
      set_text( "setup_password_error", "The two passwords are not the same." );

      return;
   }

   var button = document.getElementById( "setup_make_words" );

   button.disabled = true;

   try
   {
      if( !await recover_load_bip39( ) )
      {
         set_text( "setup_password_error", "The word list could not be read - reload the page to try again." );

         return;
      }

      // NOTE: The node's first request - not again after a reload, which kept what it answered.
      if( g_setup.pin === "" )
      {
         set_text( "setup_password_status", "Asking the node to start..." );

         var reply = await request( function( done )
         {
            return ciyam.fetch( ciyam.get_cws_url( ) + "/devices?access=admin&format=text", "POST", done );
         } );

         set_text( "setup_password_status", "" );

         var answer = setup_reply( reply );

         if( answer === null )
         {
            if( setup_refused( reply ) )
               setup_show( "setup_elsewhere" );
            else
               set_text( "setup_password_error", "The node did not start setting up: " + error_text( reply ) );

            return;
         }

         g_setup.pin = answer.pin;
         g_setup.suggested = answer.suggested;

         setup_kept_write( );

         setup_alive_start( );
      }

      g_setup.password = password;

      var mine = randomness_hex( g_setup.samples, setup_browser_hex( ), hex_sha256, g_setup.suggested.length );

      g_setup.entropy = setup_mix( g_setup.suggested, mine ).substr( 0, c_setup_entropy_length );

      g_setup.words = BIP39.entropyToMnemonic( g_setup.entropy ).split( " " );

      document.getElementById( "setup_password_new" ).value = "";
      document.getElementById( "setup_password_confirm" ).value = "";
      document.getElementById( "setup_strength" ).hidden = true;

      setup_draw_words( );
      setup_show( "setup_words" );
   }
   finally
   {
      button.disabled = false;
   }
}

function setup_draw_words( )
{
   var list = document.getElementById( "setup_word_list" );

   list.replaceChildren( );

   g_setup.words.forEach( function( word, i )
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

   // NOTE: No countdown (Damon, 2026-10-12) - an open tab keeps the node waiting by itself, so a clock would only add pressure.
   set_text( "setup_words_note", "Made from your bubbles and the node's own suggestion together. The node waits for you while this tab"
    + " is open - take your time, and finish here." );

   document.getElementById( "setup_written" ).checked = false;
   document.getElementById( "setup_words_next" ).disabled = true;
}

// ---- Checking them

function setup_draw_check( )
{
   var rows = document.getElementById( "setup_check_rows" );
   var list = recover_wordlist( );

   rows.replaceChildren( );

   g_setup.right = 0;

   g_setup.words.forEach( function( word, i )
   {
      var row = document.createElement( "div" );
      var label = document.createElement( "span" );
      var note = document.createElement( "p" );

      row.className = "home-setup-check-row";
      label.className = "home-setup-check-label";
      label.textContent = "Word " + ( i + 1 );
      note.className = "home-setup-check-note";
      note.hidden = true;

      row.appendChild( label );

      word_choices( g_setup.words, i, list, Math.random ).forEach( function( choice )
      {
         var button = document.createElement( "button" );

         button.type = "button";
         button.className = "home-setup-choice";
         button.textContent = choice;

         button.addEventListener( "click", function( ) { setup_pick( row, note, button, choice === word, i ); } );

         row.appendChild( button );
      } );

      rows.appendChild( row );
      rows.appendChild( note );
   } );

   setup_count_right( );
}

function setup_pick( row, note, button, is_right, index )
{
   row.querySelectorAll( ".home-setup-choice" ).forEach( function( other ) { other.classList.remove( "is-wrong" ); } );

   if( is_right )
   {
      button.classList.add( "is-right" );

      row.querySelectorAll( ".home-setup-choice" ).forEach( function( other ) { other.disabled = true; } );

      note.hidden = true;

      g_setup.right++;

      setup_count_right( );
   }
   else
   {
      button.classList.add( "is-wrong" );

      note.textContent = "Not that one - look at word " + ( index + 1 ) + " on your paper again.";
      note.hidden = false;
   }
}

function setup_count_right( )
{
   var right = g_setup.right;
   var total = g_setup.words.length;

   set_text( "setup_check_count", right + " of " + total + " right" );

   document.getElementById( "setup_check_bar" ).style.width = ( total > 0 ? Math.round( right / total * 100 ) : 0 ) + "%";
   document.getElementById( "setup_finish" ).disabled = ( right < g_setup.words.length );
}

// ---- Finishing - the node's second request, then signed in as admin

async function setup_finish( )
{
   var button = document.getElementById( "setup_finish" );

   button.disabled = true;

   set_text( "setup_finish_error", "" );
   set_text( "setup_finish_status", "Setting the node up..." );

   // NOTE: No still-alive while the node takes its identity - it lets the token go as it does (Ian, "637f4e60").
   setup_alive_stop( );

   // NOTE: The password is hardened before it is sent, a second or two - said as it goes (Ian, 2026-10-25).
   ciyam.on_progress = function( stage, value ) { set_text( "setup_finish_status", sign_in_progress( stage, value ).text ); };

   var combined;

   try
   {
      combined = await ciyam.hash_combined( g_setup.password, g_setup.pin );
   }
   finally
   {
      ciyam.on_progress = null;

      set_text( "setup_finish_status", "Setting the node up..." );
   }

   var credentials = CIYAM.encode_base64_url( "admin:" + combined );

   // NOTE: Set up with the current hash, so the sign in after uses it at once.
   ciyam.keep_hash_version( g_setup.pin, c_password_hash_version );

   var send = async function( )
   {
      return String( await request( function( done )
      {
         return ciyam.fetch( ciyam.get_cws_url( ) + "/devices?access=" + g_setup.pin + "&format=text&passwd=" + credentials
          + "&request=" + g_setup.entropy, "POST", done );
      } ) ).trim( );
   };

   var device = await send( );

   // NOTE: Refused as if the PIN had gone - asked once more, as the same words also mean the node was busy for a moment.
   if( setup_pin_gone( device ) )
   {
      await new Promise( function( resolve ) { window.setTimeout( resolve, 2000 ); } );

      device = await send( );
   }

   if( is_error_response( device ) || !/^[0-9a-f]{8,}$/.test( device ) )
   {
      set_text( "setup_finish_status", "" );

      button.disabled = false;

      // NOTE: The PIN went while this tab was away - the node is new again, and the words are no good.
      if( setup_pin_gone( device ) )
      {
         setup_kept_forget( );
         setup_show( "setup_expired" );
      }
      else
      {
         set_text( "setup_finish_error", "The node was not set up: " + error_text( device ) );

         setup_alive_start( );
      }

      return;
   }

   setup_alive_stop( );

   setup_kept_forget( );

   // NOTE: The pills say so - the node is ready now.
   await read_system( );

   set_text( "setup_finish_status", "Set up - signing in as admin..." );

   // NOTE: A node that has just taken its identity is busy starting up, and its first requests can miss the five seconds it
   // gives them ("Timed out waiting for web session response.", seen 2026-10-13). Signing in is safe to ask again - so it is,
   // twice more, a moment apart, for that answer only.
   for( var tries = 0; tries < 3; tries++ )
   {
      if( tries > 0 )
         await new Promise( function( resolve ) { window.setTimeout( resolve, 2000 ); } );

      ciyam.error = "";
      ciyam.device = device;

      await request( function( done )
      {
         return ciyam.connect( g_setup.pin, device, "", g_setup.password, done );
      } );

      if( !/Timed out waiting for web session response/.test( ciyam.error ) )
         break;
   }

   set_text( "setup_finish_status", "" );

   if( ( ciyam.error === "" ) && ( ciyam.sessid !== "" ) )
   {
      signin_remember_device( );
      signin_keep_session( );
   }

   set_text( "setup_pin", g_setup.pin );

   // NOTE: The words, the entropy and the password go - only the PIN is shown, once.
   g_setup.words = [ ];
   g_setup.entropy = "";
   g_setup.password = "";
   g_setup.samples = [ ];

   document.getElementById( "setup_word_list" ).replaceChildren( );
   document.getElementById( "setup_check_rows" ).replaceChildren( );
   // NOTE: Signed in, or not - said where it is seen, on the done view (found by review); Continue then goes to the sign in.
   var signed_in = ( ciyam.sessid !== "" );

   set_text( "setup_done_lede", signed_in ? "You're signed in as admin. One more thing to write down - admin's PIN. It's shown only now;"
    + " you sign in with it and the master password." : "Write down admin's PIN - it's shown only now. You sign in with it and the master"
    + " password." );
   set_text( "setup_done_error", signed_in ? "" : "The node is set up, but signing in failed - sign in with admin's PIN and the master password." );

   document.getElementById( "setup_done_error" ).hidden = signed_in;

   // NOTE: Nothing to remember without a sign in - the sign in it goes on to has its own.
   document.getElementById( "setup_retain_box" ).hidden = !signed_in;

   setup_show( "setup_done" );
}

function install_setup( )
{
   // NOTE: New, or recover - each answer goes straight on.
   document.getElementById( "setup_choose_new" ).addEventListener( "click", function( ) { setup_show( "setup_new" ); } );
   document.getElementById( "setup_choose_recover" ).addEventListener( "click", setup_recover );
   document.getElementById( "setup_new_back" ).addEventListener( "click", function( ) { setup_show( "setup_intro" ); } );
   document.getElementById( "setup_start" ).addEventListener( "click", function( ) { setup_show( "setup_random" ); } );

   document.getElementById( "setup_resume_go" ).addEventListener( "click", function( )
   {
      var kept = setup_kept_read( );

      if( kept !== null )
      {
         g_setup.pin = kept.pin;
         g_setup.suggested = kept.suggested;

         setup_alive_start( );
      }

      setup_show( "setup_random" );
   } );

   document.getElementById( "setup_random_next" ).addEventListener( "click", function( ) { setup_show( "setup_password" ); } );

   document.getElementById( "setup_password_new" ).addEventListener( "input", setup_update_strength );

   [ "setup_password_new", "setup_password_confirm" ].forEach( function( id )
   {
      document.getElementById( id ).addEventListener( "keydown", setup_password_key );
   } );

   document.getElementById( "setup_password_back" ).addEventListener( "click", function( ) { setup_show( "setup_random" ); } );

   document.getElementById( "setup_make_words" ).addEventListener( "click", setup_make_words );

   document.getElementById( "setup_written" ).addEventListener( "change", function( event )
   {
      document.getElementById( "setup_words_next" ).disabled = !event.target.checked;
   } );

   document.getElementById( "setup_words_next" ).addEventListener( "click", function( )
   {
      setup_draw_check( );
      setup_show( "setup_check" );
   } );

   document.getElementById( "setup_check_back" ).addEventListener( "click", function( ) { setup_show( "setup_words" ); } );
   document.getElementById( "setup_finish" ).addEventListener( "click", setup_finish );

   document.getElementById( "setup_again" ).addEventListener( "click", function( )
   {
      setup_kept_forget( );

      check_node( );
   } );

   // NOTE: Back to a tab that was set aside - the timer may have been held back meanwhile.
   document.addEventListener( "visibilitychange", function( )
   {
      if( !document.hidden && ( g_setup_alive_timer !== null ) )
         setup_keep_alive( );
   } );

   document.getElementById( "setup_pin_written" ).addEventListener( "change", function( event )
   {
      document.getElementById( "setup_continue" ).disabled = !event.target.checked;
   } );

   document.getElementById( "setup_continue" ).addEventListener( "click", function( )
   {
      set_text( "setup_pin", "" );

      // NOTE: Not signed in - the setting up's sign in failed - to the sign in, the node now ready.
      if( ciyam.sessid === "" )
         check_node( );
      else
      {
         signin_apply_retain( document.getElementById( "setup_retain" ).value );

         after_sign_in( );
      }
   } );

   g_setup = setup_fresh( );
}

// NOTE: "employ_unlock_key( )" takes an unlock key - or, the server finding no key's shape, the master
// password ("ciyam_base.cpp"). It answers nothing but an error on failure.
async function employ( secret, error_id, button_id )
{
   var button = document.getElementById( button_id );

   button.disabled = true;

   set_text( error_id, "" );
   set_text( "unlock_done", "" );

   var reply = await request( function( done )
   {
      return ciyam.employ_unlock_key( secret, done );
   } );

   button.disabled = false;

   if( is_error_response( reply ) )
   {
      set_text( error_id, error_text( reply ) );

      return;
   }

   set_text( "unlock_done", "Unlocked - the node is starting its services." );

   await wait_until_ready( );

   if( g_system.state === "ready" )
      enter_app( );
   else
      set_text( error_id, "The node has not said it is ready yet - wait a moment, then reload." );
}

async function do_unlock( event )
{
   event.preventDefault( );

   var key = normalise_unlock_key( document.getElementById( "unlock_key" ).value );

   if( key === "" )
   {
      set_text( "unlock_error", "That is not an unlock key - three groups of five letters and digits, as XXXXX-xxxxx-XXXXX." );

      return;
   }

   await employ( key, "unlock_error", "unlock_submit" );
}

async function do_rescue( event )
{
   event.preventDefault( );

   var password = document.getElementById( "rescue_password" ).value;

   if( password === "" )
   {
      set_text( "rescue_error", "Enter the master password." );

      return;
   }

   // NOTE: It goes in the request's path, so it is encoded.
   await employ( encodeURIComponent( password ), "rescue_error", "rescue_submit" );

   document.getElementById( "rescue_password" ).value = "";
}

async function wait_until_ready( )
{
   var waited = 0;

   while( waited < c_ready_wait_ms )
   {
      await read_system( );

      if( g_system.state === "ready" )
         return;

      await new Promise( function( resolve ) { window.setTimeout( resolve, c_ready_poll_ms ); } );

      waited += c_ready_poll_ms;
   }
}

// ====================================================================
// The shell - build step 4
// ====================================================================

// NOTE: As wide as the design's phone layouts - below it the side menu becomes a drawer.
const c_phone_query = "(max-width: 760px)";

const c_channel_name = "test_web_channel";

// NOTE: Home's id on the channel - unique to this tab, so the apps it opens ask it, and only it, for the session.
var g_self = String( Date.now( ) ) + String( Math.floor( Math.random( ) * 1000 ) ).padStart( 3, "0" );

var g_channel = null;

var g_section = "";

function is_phone( )
{
   return window.matchMedia( c_phone_query ).matches;
}

// NOTE: The channel's handshake, as the chat speaks it ("chat.html"): an app opened with "?source=<g_self>"
// asks "<g_self>:<viewer>", and is answered with Home's id and then the session's fields. When Home signs
// out it sends its own id, and every app it linked lets the session go.
function open_channel( )
{
   if( ( g_channel !== null ) || ( typeof BroadcastChannel === "undefined" ) )
      return;

   g_channel = new BroadcastChannel( c_channel_name );

   g_channel.addEventListener( "message", function( event )
   {
      var message = parse_channel_message( event.data );

      if( message === null )
         return;

      // NOTE: An app opened from here asks for the session - it is handed over.
      if( ( message.kind === "announce" ) && ( message.id === g_self ) && ( ciyam.sessid !== "" ) )
      {
         g_channel.postMessage( message.rest + "-" + g_self );

         g_channel.postMessage( message.rest + "=" + ciyam.access + "," + ciyam.device + "," + ciyam.hashed + ","
          + ciyam.sessid + "," + ciyam.unique + "," + encode_channel_field( ciyam.username ) + "," + ( ciyam.is_admin ? "1" : "0" ) );
      }
      // NOTE: Home opened from another app ("?source=", the switcher) - that app's session, taken up here.
      else if( ( message.kind === "credentials" ) && ( message.id === g_self ) && ( g_link_timer !== null ) )
      {
         var credentials = parse_credentials( message.rest );

         if( credentials !== null )
            adopt_session( credentials );
      }
   } );
}

// ---- Home opened on another app's session - the switcher's "?source=" (2026-10-08)

const c_link_interval = 500;
const c_link_attempts = 10;

var g_link_timer = null;
var g_link_attempts = 0;

function linking_source( )
{
   var source = new URL( window.location.href ).searchParams.get( "source" );

   return ( ( source !== null ) && /^[0-9]+$/.test( source ) ) ? source : "";
}

// NOTE: Asks the app that opened Home for its session, a few times; none answering, Home arrives as usual.
function start_linking( source )
{
   open_channel( );

   g_link_attempts = 0;

   var ask = function( )
   {
      if( ++g_link_attempts > c_link_attempts )
      {
         stop_linking( );

         check_node( );

         return;
      }

      g_channel.postMessage( source + ":" + g_self );
   };

   g_link_timer = window.setInterval( ask, c_link_interval );

   ask( );
}

function stop_linking( )
{
   if( g_link_timer !== null )
      window.clearInterval( g_link_timer );

   g_link_timer = null;

   // NOTE: The "?source=" goes once used, so a reload is Home's own, with its kept session.
   var url = new URL( window.location.href );

   if( url.searchParams.has( "source" ) )
   {
      url.searchParams.delete( "source" );

      history.replaceState( null, "", url.pathname + url.search + url.hash );
   }
}

async function adopt_session( credentials )
{
   stop_linking( );

   ciyam.access = credentials.access;
   ciyam.device = credentials.device;
   ciyam.hashed = credentials.hashed;
   // NOTE: The version this browser keeps for the PIN - the hash taken up was made that way (found by review, 2026-10-23).
   ciyam.hash_version = ciyam.hash_version_for( ciyam.access );
   ciyam.sessid = credentials.sessid;
   ciyam.unique = credentials.unique;
   ciyam.username = credentials.username;
   ciyam.is_admin = credentials.is_admin;

   await read_system( );

   after_sign_in( );
}

function unlink_apps( )
{
   if( g_channel !== null )
      g_channel.postMessage( g_self );
}

// NOTE: The shell's item ("shell.js") with Home's badges - what waits in the chat, and Health's count for admin.
function rail_item( key, title, is_app, on_click )
{
   var item = shell_rail_item( key, title, is_app, on_click );

   if( key === "chat" )
      item.appendChild( chat_badge( ) );

   if( key === "health" )
      item.appendChild( health_badge( ) );

   return item;
}

// NOTE: A link to another app carries the address a new tab needs - on this page's session - so the browser's own
// "Open in new tab" works too (found by review, 2026-10-09). Home's own stays "#home".
function app_address( item, app, hash )
{
   if( app.key !== "home" )
      item.href = app.page + "?source=" + encodeURIComponent( g_self ) + ( hash || "" );

   return item;
}

function bottom_item( key, title, on_click )
{
   var item = shell_bottom_item( key, title, on_click );

   if( key === "chat" )
      item.appendChild( chat_badge( ) );

   return item;
}

// NOTE: Whether the node's chat (IRC) is turned on, as its lobby last answered.
var g_chat_off = false;

// NOTE: The tab's title carries what Home's badges count - the chat's, and Health's for admin - for a tab in the
// background (2026-10-12).
const c_home_title = "CIYAM Home";

var g_title_counts = { chat: 0, health: 0 };

function update_title( which, count )
{
   g_title_counts[ which ] = count;

   document.title = page_title( c_home_title, g_title_counts.chat + g_title_counts.health );
}

function forget_title( )
{
   g_title_counts = { chat: 0, health: 0 };

   document.title = c_home_title;
}

// NOTE: What waits in the chat - unread messages, requests and invitations, as the chat's own badge counts them.
function chat_badge( )
{
   var badge = document.createElement( "span" );

   badge.className = "shell-badge";
   badge.dataset.badge = "chat";
   badge.hidden = true;

   return badge;
}

// NOTE: How many things need admin - Health's count, the same as the Overview's list (2026-10-12).
function health_badge( )
{
   var badge = document.createElement( "span" );

   badge.className = "shell-badge";
   badge.dataset.badge = "health";
   badge.hidden = true;

   return badge;
}

// NOTE: In this tab, as the switcher opens it (Damon, 2026-10-09) - a new one when the event asks.
function open_app( app, event )
{
   shell_close_rail( );

   if( app.key === "home" )
   {
      go( home_section( ciyam.is_admin, "" ) );

      return;
   }

   apps_go( app.key, "", event );
}

// NOTE: Built afresh at each sign in - who is signed in decides the apps and the sections.
function build_shell( )
{
   var apps = home_apps( ciyam.is_admin, false, false );
   var phone_apps = home_apps( ciyam.is_admin, false, true );
   var sections = home_sections( ciyam.is_admin );

   var rail_apps = document.getElementById( "rail_apps" );
   var rail_sections = document.getElementById( "rail_sections" );
   var chips = document.getElementById( "shell_chips" );
   var bottom = document.getElementById( "shell_bottombar" );

   [ rail_apps, rail_sections, chips, bottom ].forEach( function( element ) { element.replaceChildren( ); } );

   apps.forEach( function( app )
   {
      rail_apps.appendChild( app_address( rail_item( app.key, app.title, true, function( event ) { open_app( app, event ); } ), app ) );
   } );

   phone_apps.forEach( function( app )
   {
      bottom.appendChild( app_address( bottom_item( app.key, ( app.key === "account" ) ? "Account" : app.title,
       function( event ) { open_app( app, event ); } ), app ) );
   } );

   sections.forEach( function( section )
   {
      rail_sections.appendChild( rail_item( section.key, section.title, false, function( ) { shell_close_rail( ); go( section.key ); } ) );

      var chip = shell_chip( section.key, section.title, function( ) { go( section.key ); } );

      if( section.key === "health" )
         chip.appendChild( health_badge( ) );

      chips.appendChild( chip );
   } );

   document.getElementById( "rail_node" ).hidden = ( sections.length === 0 );
   chips.hidden = ( sections.length === 0 );

   var name = account_display_name( ciyam.username, ciyam.is_admin );

   // NOTE: The rail's only - a phone's top bar had one too, looking like the chat's account menu but not one, so it went (Ian,
   // 2026-10-16); Home's menu there is the hamburger.
   shell_show_user( name, role_text( ciyam.is_admin ) );
   set_text( "top_sub", name + " · " + role_text( ciyam.is_admin ) );
}

// NOTE: One of Home's own sections - the address's "#..." follows, so a reload or Back comes back to it.
function go( section )
{
   g_section = section;

   [ "home", "overview", "health", "people", "keys", "alerts", "logs" ].forEach( function( key )
   {
      document.getElementById( "section_" + key ).hidden = ( key !== section );
   } );

   var titles = { home: "Home", overview: "Overview", health: "Health", people: "People", keys: "Unlock keys", alerts: "Alerts", logs: "Logs" };

   set_text( "top_heading", titles[ section ] || "Home" );

   shell_mark( [ section ] );

   // NOTE: Home's own item stands for admin's sections too, on the phone's bottom bar.
   if( ciyam.is_admin )
   {
      document.querySelectorAll( ".shell-bottom-item[data-key='home']" ).forEach( function( item )
      {
         item.classList.add( "is-current" );
      } );
   }

   if( window.location.hash !== "#" + section )
      history.replaceState( null, "", "#" + section );

   // NOTE: A new unlock key is shown once - leaving the section lets it go. So is a code or a new PIN, in People.
   if( section !== "keys" )
      forget_shown_key( );

   if( section !== "people" )
   {
      forget_handover( );
      forget_ready( );

      // NOTE: Every PIN hidden again - one shown stays shown only while People is (found by review).
      g_people_shown = { };
   }

   // NOTE: The problems alone are for the visit "Show them" opened - Logs chosen again shows the whole log.
   if( section !== "logs" )
      g_log_problems_only = false;

   on_section_shown( section );
}

function install_shell( )
{
   window.name = c_tab_home;

   open_channel( );

   // NOTE: The switcher at the top of the side menu - the CIYAM mark and "Home", as in every app (2026-10-08).
   apps_build( document.getElementById( "apps_host" ), {
      current: "home",
      self: function( ) { return g_self; },
      sign_out: do_sign_out,
      signed_out: signed_out_elsewhere,
      from: "home"
   } );

   shell_install( );

   // NOTE: Only once in Home - not behind Unlock, where a section would load unseen (found by review).
   window.addEventListener( "hashchange", function( )
   {
      if( ( ciyam.sessid !== "" ) && !document.getElementById( "app_view" ).hidden )
         go( home_section( ciyam.is_admin, window.location.hash ) );
   } );
}

async function enter_app( )
{
   signin_keep_session( );

   build_shell( );

   apps_refresh( );

   // NOTE: The console drawer, as in the chat and the accounts page - admin's, and not on a phone (Damon, 2026-10-08).
   document.getElementById( "console_toggle" ).hidden = !ciyam.is_admin;

   var name = account_display_name( ciyam.username, ciyam.is_admin );

   set_text( "app_greeting", "Hello, " + name );

   await refresh_node_line( );

   go( home_section( ciyam.is_admin, window.location.hash ) );

   show_view( "app_view" );

   g_rooms = [ ];
   g_starting_messages = [ ];
   g_starting_total = -1;
   g_devices = [ ];
   g_no_keys = false;
   g_keys_known = false;

   render_member_home( );

   await load_member_home( );

   start_refresh( );
}

// ====================================================================
// A member's Home - build step 5
// ====================================================================

const c_lobby_room = "0000000";
const c_administration_room = "0000001";

// NOTE: How often Home looks again while it is open - and at once when its tab is shown again.
const c_refresh_ms = 60000;

var g_rooms = [ ];
var g_starting_messages = [ ];
var g_starting_total = -1;
var g_devices = [ ];
var g_refresh_timer = null;

// NOTE: Whether the account's keys are not set up - asked at each look until they are (Damon, 2026-10-28: a reminder in Needs
// you, keys being skippable as the last step of joining).
var g_no_keys = false;
var g_keys_known = false;

function devices_url( )
{
   return ciyam.get_cws_url( ) + "/devices?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text&session=" + ciyam.sessid;
}

// NOTE: The key the chat keeps a person's dismissed announcements under, so dismissing on Home dismisses in the
// chat too (Damon, 2026-10-08: one dismissal for both) - the chat hears it through the "storage" event. By the
// account's name, not its PIN ("account_storage_key( )" in "chat_parse.js").
function dismissed_key( )
{
   return account_storage_key( c_storage_dismissed_by_name, ciyam.username, ciyam.is_admin );
}

function read_dismissed( )
{
   try
   {
      return parse_dismissed( localStorage.getItem( dismissed_key( ) ) );
   }
   catch( e )
   {
      return [ ];
   }
}

function dismiss_announcement( unique )
{
   try
   {
      localStorage.setItem( dismissed_key( ), JSON.stringify( add_dismissed( read_dismissed( ), unique ) ) );
   }
   catch( e )
   {
   }

   render_member_home( );
}

// NOTE: The lobby listing gives every room and its unread count without moving what the session has read.
// The Admin room - invitations, requests, announcements - is read whole, and only when its total changes, as
// the chat reads it: a tab of the chat opened from Home shares this session.
async function load_member_home( )
{
   var asked_for = ciyam.access;

   var answer = await request( function( done )
   {
      return ciyam.fetch_messages( c_lobby_room, "", done );
   } );

   var listing = parse_fetch_response( answer );

   if( ciyam.access !== asked_for )
      return;

   // NOTE: The chat not turned on - the Chat tile says so, rather than "Nothing new" (Ian, 2026-10-13).
   g_chat_off = irc_unavailable( answer );

   if( listing.error === "" )
      g_rooms = derive_room_list( listing.rooms );

   var starting = g_rooms.filter( function( entry ) { return entry.room === c_administration_room; } )[ 0 ];

   if( !ciyam.is_admin && starting && ( starting.total !== g_starting_total ) )
   {
      var read = parse_fetch_response( await request( function( done )
      {
         return ciyam.fetch_messages( c_administration_room, "from=0", done );
      } ) );

      if( ciyam.access !== asked_for )
         return;

      if( read.error === "" )
      {
         g_starting_messages = read.messages;
         g_starting_total = starting.total;
      }
   }

   var devices = await request( function( done )
   {
      return ciyam.fetch( devices_url( ), "GET", done );
   } );

   if( ciyam.access !== asked_for )
      return;

   g_devices = is_error_response( devices ) ? [ ] : parse_devices( devices, ciyam.device );

   if( !g_keys_known )
   {
      var keys = await request( function( done )
      {
         return ciyam.fetch( ciyam.get_cws_url( ) + "/user-keys?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text&session="
          + ciyam.sessid, "GET", done );
      } );

      if( ciyam.access !== asked_for )
         return;

      // NOTE: Only "none" is said - an error, or keys this browser cannot read, is My account's to explain.
      var state = keys_state( false, keys );

      g_no_keys = ( state === "none" );
      g_keys_known = ( state === "elsewhere" );
   }

   render_member_home( );
}

function invitations_waiting( )
{
   return ciyam.is_admin ? [ ] : pending_invitations( g_starting_messages, g_rooms, ciyam.username );
}

function render_member_home( )
{
   var invitations = invitations_waiting( );
   var summary = chat_summary( g_rooms, invitations, ciyam.is_admin );

   set_text( "tile_chat_text", g_chat_off ? "The chat isn't turned on for this node" : chat_summary_text( summary ) );

   document.querySelectorAll( "[data-badge='chat']" ).forEach( function( badge )
   {
      badge.textContent = badge_text( summary.badge );
      badge.hidden = ( summary.badge === 0 );
   } );

   update_title( "chat", summary.badge );

   // ---- Needs you
   var list = document.getElementById( "needs_list" );
   var items = needs_you( invitations, ciyam.username, g_no_keys );

   list.replaceChildren( );

   items.forEach( function( item )
   {
      var row = document.createElement( "li" );

      row.className = "home-need";

      var avatar = document.createElement( "span" );

      avatar.className = "shell-avatar" + ( ( item.kind !== "request" ) ? " is-room" : "" );
      avatar.textContent = { invitation: "#", keys: "🔑" }[ item.kind ] || user_initial( item.inviter );

      if( item.kind === "request" )
         avatar.style.background = "var(--color-sender-" + sender_colour_index( item.inviter ) + ")";

      var text = document.createElement( "span" );

      text.className = "home-need-text";

      var what = document.createElement( "span" );

      what.className = "home-need-what";
      what.textContent = item.text;

      var detail = document.createElement( "span" );

      detail.className = "home-need-detail";
      detail.textContent = item.detail;

      text.appendChild( what );
      text.appendChild( detail );

      var answer = document.createElement( "a" );

      answer.className = "chat-btn home-need-answer";
      answer.href = "#";

      if( item.kind === "keys" )
      {
         answer.textContent = "Set them up";
         shell_bind_link( answer, function( event ) { open_app_by_key( "account", "#keys", event ); } );
      }
      else
      {
         answer.textContent = "Answer in Chat";
         shell_bind_link( answer, function( event ) { open_app_by_key( "chat", "", event ); } );
      }

      row.appendChild( avatar );
      row.appendChild( text );
      row.appendChild( answer );

      list.appendChild( row );
   } );

   document.getElementById( "needs_empty" ).hidden = ( items.length > 0 );
   document.getElementById( "needs_count" ).hidden = ( items.length === 0 );

   set_text( "needs_count", String( items.length ) );

   // ---- Announcements - admin's, so admin sees none, as in the chat
   var announcements = ciyam.is_admin ? [ ] : pending_announcements( g_starting_messages, read_dismissed( ) );
   var holder = document.getElementById( "announce_list" );

   holder.replaceChildren( );

   announcements.forEach( function( message )
   {
      var card = document.createElement( "section" );

      card.className = "home-announcement";
      card.dataset.unique = message.unique;

      var label = document.createElement( "span" );

      label.className = "home-announcement-label";
      label.textContent = "Announcement · from admin";

      var text = document.createElement( "p" );

      text.className = "home-announcement-text";
      text.textContent = message_plain_text( message.text );

      var ok = document.createElement( "button" );

      ok.type = "button";
      ok.className = "chat-btn chat-btn--primary";
      ok.textContent = "OK";
      ok.addEventListener( "click", function( ) { dismiss_announcement( message.unique ); } );

      card.appendChild( label );
      card.appendChild( text );
      card.appendChild( ok );

      holder.appendChild( card );
   } );

   // ---- Your account
   var name = account_display_name( ciyam.username, ciyam.is_admin );
   var avatar_large = document.getElementById( "account_avatar" );

   avatar_large.textContent = user_initial( name );
   avatar_large.style.background = "var(--color-sender-" + sender_colour_index( name ) + ")";

   set_text( "account_name", name );
   set_text( "account_role", role_text( ciyam.is_admin ) );

   var devices = document.getElementById( "device_list" );

   devices.replaceChildren( );

   var shown = devices_shown( device_rows( g_devices ), 0 );

   set_text( "device_more", more_devices_text( shown.more ) );

   shown.rows.forEach( function( row )
   {
      var item = document.createElement( "li" );

      item.className = "home-device" + ( row.active ? " is-active" : "" );

      var dot = document.createElement( "span" );

      dot.className = "home-device-dot";

      var label = document.createElement( "span" );

      label.className = "home-device-label" + ( row.current ? "" : " home-mono" );
      label.textContent = row.label;
      label.title = row.title;

      var state = document.createElement( "span" );

      state.className = "home-device-state";
      state.textContent = row.state;

      item.appendChild( dot );
      item.appendChild( label );
      item.appendChild( state );

      devices.appendChild( item );
   } );
}

function open_app_by_key( key, hash, event )
{
   shell_close_rail( );

   apps_go( key, hash || "", event );
}

// NOTE: "Home node · CIYAM 0.0.0 · up 11 minutes" - kept up to date with the minute, as Uptime is (Ian, 2026-10-13: it stayed
// as it was at sign in).
async function refresh_node_line( )
{
   var line = node_line( g_system.version, uptime_words( await fetch_text( "/uptime" ) ) );

   set_text( "app_node", line );
   set_text( "overview_node", line );
}

function start_refresh( )
{
   stop_refresh( );

   g_refresh_timer = window.setInterval( function( )
   {
      if( ( ciyam.sessid === "" ) || document.hidden )
         return;

      load_member_home( );
      refresh_node_line( );

      // NOTE: Admin's Overview or Health too, while shown - a page left open is a way to watch the node (Ian, 2026-10-11).
      if( ciyam.is_admin && ( ( g_section === "overview" ) || ( g_section === "health" ) ) )
         load_overview( );
   }, c_refresh_ms );
}

function stop_refresh( )
{
   if( g_refresh_timer !== null )
      window.clearInterval( g_refresh_timer );

   g_refresh_timer = null;
}

function install_member_home( )
{
   document.getElementById( "tile_chat_icon" ).appendChild( shell_icon( "chat", 22 ) );

   shell_bind_link( document.getElementById( "tile_chat" ), function( event ) { open_app_by_key( "chat", "", event ); } );

   app_address( document.getElementById( "tile_chat" ), { key: "chat", page: "chat.html" } );

   // NOTE: The accounts page - in this tab now, as every app opens (Damon, 2026-10-09; in its own tab until then).
   shell_bind_link( document.getElementById( "manage_account" ), function( event ) { open_app_by_key( "account", "#mine", event ); } );

   app_address( document.getElementById( "manage_account" ), { key: "account", page: "account.html" }, "#mine" );

   document.addEventListener( "visibilitychange", function( )
   {
      if( !document.hidden && ( ciyam.sessid !== "" ) )
         load_member_home( );
   } );

   // NOTE: The chat dismissing an announcement - or another Home - shows here at once.
   window.addEventListener( "storage", function( event )
   {
      if( ( ciyam.access !== "" ) && ( event.key === dismissed_key( ) ) )
         render_member_home( );
   } );
}

// ====================================================================
// Admin's sections - build step 6
// ====================================================================

const c_qr_size = 168;

const c_overview_log_lines = 5;

// NOTE: How much of the server log the Overview reads to count its problems since the restart (2026-10-09), and how
// many filling rooms it names.
const c_overview_problem_lines = 1000;

const c_overview_filling_shown = 3;

var g_log_names = [ ];
var g_log_name = "server";
var g_log_lines = [ ];

function admin_url( path )
{
   return ciyam.get_cws_url( ) + path + "?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text&session=" + ciyam.sessid;
}

// NOTE: How many unlock keys the node holds - asked of the node, so every browser says the same (Ian, 2026-10-10). The
// count this browser used to keep is let go.
async function read_key_count( )
{
   try
   {
      localStorage.removeItem( "home.unlock_keys_made" );
   }
   catch( e )
   {
   }

   var url = admin_url( "" ) + "&request=" + encodeURIComponent( c_home_keys_request );

   var response = await request( function( done )
   {
      return ciyam.fetch( url, "GET", done );
   } );

   return is_error_response( response ) ? null : parse_key_count( response );
}

// NOTE: One of the requests admin may make of any node ("is_admin_sanctioned") - its answer, or null when refused. A request
// not on the node's list is answered "[bad]" - refused too, not an answer (found 2026-10-15, trying Health with "@no_devt").
async function ask_node( text )
{
   var url = admin_url( "" ) + "&request=" + encodeURIComponent( text );

   var response = await request( function( done )
   {
      return ciyam.fetch( url, "GET", done );
   } );

   return ( is_error_response( response ) || ( String( response ).trim( ) === "[bad]" ) ) ? null : response;
}

// NOTE: Whether the node's identity is held encrypted - without it, no unlock keys (Ian, 2026-10-12).
async function read_secure( )
{
   return parse_secure( await ask_node( c_home_secure_request ) );
}

// NOTE: On a node without a secure identity the keys are not offered - Health and Unlock keys say why.
function show_keys_available( secure )
{
   var none = ( secure === false );

   [ "overview_make_key", "health_make_key", "key_make" ].forEach( function( id ) { document.getElementById( id ).hidden = none; } );
}

// NOTE: A Health card's word on itself - "ok", "warn", "problem", or "quiet" for what was not said or is only worth knowing.
function set_health_pill( id, level, text )
{
   var pill = document.getElementById( id );

   pill.className = "home-health-pill is-" + level;
   pill.textContent = text;
}

// NOTE: A log's last "count" lines - every line for none. The node gives ten unless asked (Ian, "3d6a5820"), so
// Home always says how many it wants.
async function read_log( name, count )
{
   var url = admin_url( "/logs/" + encodeURIComponent( name ) ) + "&options=" + encodeURIComponent( log_options( count ) );

   var response = await request( function( done )
   {
      return ciyam.fetch( url, "GET", done );
   } );

   return is_error_response( response ) ? null : parse_log_lines( response );
}

// NOTE: A log's lines drawn read only, each tinted by what it says - text only, never markup.
function draw_log( element, lines )
{
   element.replaceChildren( );

   lines.forEach( function( line )
   {
      var row = document.createElement( "span" );

      var kind = log_line_kind( line );

      row.className = "home-log-line" + ( kind !== "" ? " is-" + kind : "" );
      row.textContent = line + "\n";

      element.appendChild( row );
   } );
}

async function load_overview( )
{
   if( !ciyam.is_admin )
      return;

   var asked_for = ciyam.access;

   await read_system( );

   var uptime = uptime_words( await fetch_text( "/uptime" ) ) || "-";

   show_connection( );

   var secure = await read_secure( );
   var key_count = await read_key_count( );
   var devt = parse_flag( await ask_node( c_home_devt_request ) );
   var demo = parse_flag( await ask_node( c_home_demo_request ) );

   var users = await request( function( done ) { return ciyam.fetch_users( done ); } );

   if( ciyam.access !== asked_for )
      return;

   show_node_facts( uptime, secure, devt, demo );
   show_identity( secure, key_count );

   if( !is_error_response( users ) )
   {
      var summary = people_summary( parse_people( users, ciyam.access, ciyam.username || "admin" ) );

      set_text( "stat_people", String( summary.active ) );
      set_text( "stat_people_note", people_note( summary ) );
      set_text( "overview_people", people_waiting_text( summary ) );
   }

   // NOTE: Enough of the log to reach back to the last start, usually - the latest lines are drawn from the same read.
   var server = await read_log( "server", c_overview_problem_lines );

   if( ciyam.access !== asked_for )
      return;

   draw_log( document.getElementById( "overview_log" ),
    server ? log_view( server, "", c_overview_log_lines ).lines : [ "(the server log could not be read)" ] );

   var problems = show_problems( server );

   show_key_unlock( server ? last_key_unlock( server ) : null );

   // NOTE: Who is connected, what runs by itself, and where the node listens - allowed admin on any node (Ian, "136d4815").
   var sessions = await ask_node( c_home_sessions_request );
   var schedule = await ask_node( c_home_schedule_request );
   var listeners = await ask_node( c_home_listeners_request );

   if( ciyam.access !== asked_for )
      return;

   show_sessions( sessions );
   show_schedule( schedule );
   show_listening( listeners );

   var listing = await request( function( done ) { return ciyam.fetch_messages( c_lobby_room, "", done ); } );

   if( ciyam.access !== asked_for )
      return;

   var filling = show_filling( is_error_response( listing ) ? null : derive_room_list( parse_fetch_response( listing ).rooms ),
    irc_unavailable( listing ) );

   show_attention( health_issues( { secure: secure, keys: key_count, problems: problems, filling: filling } ) );
}

// NOTE: A time for Health, from a unique - "Today 15:03", as the chat's days say it.
function health_time( unique )
{
   return ( unique === "" ) ? "-" : day_label( unique ) + " " + unique_to_time( unique ).substring( 0, 5 );
}

// NOTE: Health's sessions card - by person, yours marked; "null", the node did not answer (one before Ian's "136d4815").
function show_sessions( reply )
{
   var rows = document.getElementById( "sessions_rows" );

   rows.replaceChildren( );

   if( reply === null )
   {
      show_sessions_stat( null );

      set_text( "sessions_line", "This node doesn't report its sessions." );
      set_health_pill( "health_sessions_pill", "quiet", "Not reported" );

      document.getElementById( "sessions_wrap" ).hidden = true;

      return;
   }

   var summary = sessions_summary( parse_web_sessions( reply ), Date.now( ), ciyam.sessid );

   show_sessions_stat( summary );

   set_text( "sessions_line", sessions_line( summary ) );
   set_health_pill( "health_sessions_pill", "quiet", summary.total + " open" );

   summary.people.forEach( function( person )
   {
      var row = document.createElement( "tr" );

      var who = document.createElement( "td" );

      who.textContent = ( person.name || "No name" ) + ( person.yours ? " (you)" : "" );
      who.classList.toggle( "is-you", person.yours );

      row.appendChild( who );

      [ String( person.count ), health_time( person.last ), person.addresses.join( ", " ) || "-" ].forEach( function( text )
      {
         var cell = document.createElement( "td" );

         cell.textContent = text;

         row.appendChild( cell );
      } );

      rows.appendChild( row );
   } );

   document.getElementById( "sessions_wrap" ).hidden = ( summary.people.length === 0 );
}

// NOTE: The Overview's sessions figure, from the same read as Health's card ("sessions_stat( )").
function show_sessions_stat( summary )
{
   var stat = sessions_stat( summary );

   set_text( "stat_sessions", stat.value );
   set_text( "stat_sessions_note", stat.note );

   document.getElementById( "stat_sessions_unit" ).hidden = !stat.reported;
   document.getElementById( "stat_sessions_value" ).classList.toggle( "home-stat-value--quiet", !stat.reported );
}

// NOTE: Health's schedule card - each autoscript and its next run, and what else the node says of it.
function show_schedule( reply )
{
   var list = document.getElementById( "schedule_list" );

   list.replaceChildren( );

   if( reply === null )
   {
      set_health_pill( "health_schedule_pill", "quiet", "Not reported" );

      return;
   }

   var entries = parse_schedule( reply );

   set_health_pill( "health_schedule_pill", entries.length > 0 ? "ok" : "quiet", entries.length > 0 ? entries.length + " running" : "None" );

   entries.forEach( function( entry )
   {
      var item = document.createElement( "li" );

      var name = document.createElement( "b" );

      name.textContent = entry.name;

      var next = document.createElement( "span" );

      next.textContent = "next " + health_time( entry.next );

      item.appendChild( name );
      item.appendChild( next );

      if( entry.detail !== "" )
      {
         var detail = document.createElement( "span" );

         detail.className = "home-mono";
         detail.textContent = entry.detail;

         item.appendChild( detail );
      }

      list.appendChild( item );
   } );
}

// NOTE: Health's listening card - each port the node takes connections on, the peer port's chains counted.
function show_listening( reply )
{
   var facts = document.getElementById( "listening_facts" );

   facts.replaceChildren( );

   if( reply === null )
   {
      set_health_pill( "health_listening_pill", "quiet", "Not reported" );

      return;
   }

   var listeners = parse_listeners( reply );

   set_health_pill( "health_listening_pill", listeners.length > 0 ? "ok" : "quiet", listeners.length > 0 ? "OK" : "None" );

   listeners.forEach( function( listener )
   {
      var term = document.createElement( "dt" );
      var value = document.createElement( "dd" );

      term.textContent = listener.name;
      value.textContent = listener.port + ( listener.chains > 0 ? " · " + listener.chains + ( listener.chains === 1 ? " chain" : " chains" ) : "" );

      facts.appendChild( term );
      facts.appendChild( value );
   } );
}

// NOTE: The node at a glance (the Overview's first stat) and in Health's node card.
function show_node_facts( uptime, secure, devt, demo )
{
   var ready = ( g_system.state === "ready" );

   set_text( "stat_node", ready ? "Ready" : "Not ready" );
   set_text( "stat_node_note", "Unlocked" + ( secure === true ? " · encrypted identity" : "" ) );
   set_text( "stat_uptime", uptime );

   set_text( "health_state", ready ? "Ready, unlocked" : "Not ready" );
   set_text( "health_uptime", uptime );
   set_text( "health_version", g_system.version || "-" );
   set_text( "health_kind", node_kind_text( devt, demo ) );

   set_health_pill( "health_node_pill", ready ? "ok" : "problem", ready ? "OK" : "Not ready" );
}

// NOTE: How this browser's connection is encrypted, as the node sees it. Plain HTTP is normal on the node's own network
// (Ian, 2026-10-03) - worth knowing, not a problem, so it is not among what needs you.
function show_connection( )
{
   var security = security_text( g_system.security );

   set_text( "connection_heading", security.heading || "Connection: not reported" );
   set_text( "connection_cipher", ( g_system.cipher !== "" ) ? g_system.cipher + ( g_system.group !== "" ? " (" + g_system.group + ")" : "" )
    : ( g_system.security === "none" ? "Plain HTTP - (NONE)" : "" ) );

   if( g_system.security === "none" )
      set_health_pill( "health_connection_pill", "quiet", "Worth knowing" );
   else if( security.heading !== "" )
      set_health_pill( "health_connection_pill", "ok", "OK" );
   else
      set_health_pill( "health_connection_pill", "quiet", "Not reported" );
}

// NOTE: The last unlock with a key, from the server log - Health's identity card and the Overview's keys ("key_unlock_text( )").
function show_key_unlock( unlock )
{
   var text = key_unlock_text( unlock );

   [ "health_unlock_line", "overview_unlock_line" ].forEach( function( id )
   {
      set_text( id, text );

      document.getElementById( id ).hidden = ( text === "" );
   } );
}

// NOTE: The identity and its unlock keys - Health's card, and the Overview's quick action.
function show_identity( secure, key_count )
{
   var none = ( secure === false );

   set_text( "health_secure_line", ( secure === true ) ? "Held encrypted. After a restart, needs an unlock key or the master password."
    : ( none ? "Not held encrypted." : "Not reported." ) );

   set_text( "overview_keys_note", none ? c_home_keys_unavailable : keys_note( key_count ) );

   set_text( "overview_keys_line", none ? "Not used on this node."
    : ( key_count === null ? "Not reported." : counted( key_count, "key", "keys" ) + " left." ) );

   if( none )
      set_health_pill( "health_identity_pill", "quiet", "No keys needed" );
   else if( key_count === 0 )
      set_health_pill( "health_identity_pill", "warn", "No keys left" );
   else if( key_count === null )
      set_health_pill( "health_identity_pill", "quiet", "Not reported" );
   else
      set_health_pill( "health_identity_pill", "ok", "OK" );

   show_keys_available( secure );
}

// NOTE: What needs admin - on the Overview, each with its way to act, or "Nothing needs you"; Health's badge counts them.
function show_attention( issues )
{
   var list = document.getElementById( "attention_list" );

   list.replaceChildren( );

   issues.forEach( function( issue )
   {
      var item = document.createElement( "li" );

      item.className = "home-attention-item is-" + issue.level;

      var dot = document.createElement( "span" );

      dot.className = "home-attention-dot";
      dot.setAttribute( "aria-hidden", "true" );

      var body = document.createElement( "span" );

      body.className = "home-attention-body";

      var title = document.createElement( "b" );

      title.textContent = issue.title;

      var detail = document.createElement( "span" );

      detail.className = "home-attention-detail";
      detail.textContent = issue.detail;

      body.append( title, detail );

      var action = document.createElement( "button" );

      action.type = "button";
      action.className = "chat-btn" + ( issue.level === "problem" ? " chat-btn--primary" : "" );
      action.textContent = issue.action;
      action.dataset.target = issue.target;

      action.addEventListener( "click", function( event )
      {
         if( issue.target === "problems" )
            show_problems_in_logs( event );
         else
            go( issue.target );
      } );

      item.append( dot, body, action );

      list.appendChild( item );
   } );

   var any = ( issues.length > 0 );

   set_text( "attention_heading", any ? health_heading( issues ) : "" );

   document.getElementById( "attention_card" ).hidden = !any;
   document.getElementById( "attention_rest" ).hidden = !any;
   document.getElementById( "well_card" ).hidden = any;

   var problem = issues.some( function( issue ) { return issue.level === "problem"; } );

   document.querySelectorAll( "[data-badge='health']" ).forEach( function( badge )
   {
      badge.textContent = badge_text( issues.length );
      badge.hidden = !any;
      badge.classList.toggle( "is-problem", problem );
   } );

   update_title( "health", issues.length );
}

// NOTE: The errors and warnings in the server log since the restart - the latest in words, and Logs showing just
// those lines ("problem_lines( )") when "Show them" is chosen, until the whole log is asked for again.
var g_log_problems_only = false;

function show_problems( lines )
{
   var show = document.getElementById( "problems_show" );

   if( lines === null )
   {
      set_text( "problems_heading", "The server log could not be read" );
      set_text( "problems_latest", "" );
      set_health_pill( "health_log_pill", "quiet", "Not read" );

      show.hidden = true;

      return null;
   }

   var problems = log_problems( lines );
   var any = ( problems.errors + problems.warnings ) > 0;

   set_text( "problems_heading", log_problems_text( problems ) );

   var latest = log_line_parts( problems.latest );

   set_text( "problems_latest", any ? "Latest" + ( latest.time !== "" ? ", " + latest.time : "" ) + ": " + latest.text : "" );

   if( problems.errors > 0 )
      set_health_pill( "health_log_pill", "problem", counted( problems.errors, "error", "errors" ) );
   else if( problems.warnings > 0 )
      set_health_pill( "health_log_pill", "warn", counted( problems.warnings, "warning", "warnings" ) );
   else
      set_health_pill( "health_log_pill", "ok", "OK" );

   show.hidden = !any;

   return problems;
}

function show_problems_in_logs( event )
{
   event.preventDefault( );

   g_log_name = "server";
   g_log_problems_only = true;

   document.getElementById( "log_filter" ).value = "";
   document.getElementById( "log_count" ).value = String( c_overview_problem_lines );

   go( "logs" );
}

// NOTE: The rooms near the number of messages a room keeps - a full one drops its oldest (Ian's "@qs_pf_irc",
// 2026-10-11) - and Health's bars for the fullest, filling or not. Only the rooms admin is in: the listing has no others.
// Returns the rooms filling up, or null when the listing could not be read.
function show_filling( rooms, chat_off )
{
   var list = document.getElementById( "rooms_filling" );

   list.replaceChildren( );

   if( rooms === null )
   {
      set_text( "rooms_heading", chat_off ? "The chat isn't turned on for this node" : "The rooms could not be read" );
      set_health_pill( "health_rooms_pill", "quiet", chat_off ? "Off" : "Not read" );

      return null;
   }

   var filling = rooms_filling( rooms );

   set_text( "rooms_heading", rooms_filling_text( filling ) );

   rooms_fullest( rooms, c_overview_filling_shown ).forEach( function( room )
   {
      var item = document.createElement( "li" );

      if( room.total >= c_home_room_limit )
         item.className = "is-full";
      else if( room.total >= c_home_room_limit * c_home_room_filling )
         item.className = "is-filling";

      var name = document.createElement( "span" );

      name.className = "home-room-bar-name";
      name.textContent = room.name;

      var bar = document.createElement( "span" );

      bar.className = "home-room-bar";
      bar.setAttribute( "role", "img" );
      bar.setAttribute( "aria-label", room.percent + "% full" );

      var fill = document.createElement( "span" );

      fill.style.width = Math.max( room.percent, 1 ) + "%";

      bar.appendChild( fill );

      var count = document.createElement( "span" );

      count.className = "home-room-bar-count";
      count.textContent = room.total.toLocaleString( "en-AU" ) + " of " + c_home_room_limit.toLocaleString( "en-AU" );

      item.append( name, bar, count );

      list.appendChild( item );
   } );

   var full = filling.some( function( room ) { return room.total >= c_home_room_limit; } );

   if( full )
      set_health_pill( "health_rooms_pill", "problem", "Full" );
   else if( filling.length > 0 )
      set_health_pill( "health_rooms_pill", "warn", "Filling up" );
   else
      set_health_pill( "health_rooms_pill", "ok", "OK" );

   return filling;
}

// ---- Unlock keys

async function show_keys( )
{
   var asked_for = ciyam.access;

   var secure = await read_secure( );
   var count = ( secure === false ) ? null : await read_key_count( );

   // NOTE: An answer arriving after a sign out is let go (found by review).
   if( ciyam.access !== asked_for )
      return;

   set_text( "keys_note", ( secure === false ) ? c_home_keys_unavailable : keys_note( count ) );

   show_keys_available( secure );
}

async function do_make_key( )
{
   var button = document.getElementById( "key_make" );

   button.disabled = true;

   set_text( "key_error", "" );

   var reply = await request( function( done ) { return ciyam.create_unlock_key( "", done ); } );

   button.disabled = false;

   var key = normalise_unlock_key( String( reply ).trim( ) );

   if( is_error_response( reply ) || ( key === "" ) )
   {
      set_text( "key_error", is_error_response( reply ) ? key_error_text( reply ) : "The node did not answer with a key." );

      return;
   }

   set_text( "key_text", key );

   var box = document.getElementById( "key_qr" );

   box.replaceChildren( );

   // NOTE: Drawn in the page - "qrcode.min.js" - so the key goes nowhere else. The library titles its image
   // with the text it holds; that tooltip is taken off, so the key is not shown on hover.
   new QRCode( box, { text: key, width: c_qr_size, height: c_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
    correctLevel: QRCode.CorrectLevel.M } );

   box.removeAttribute( "title" );
   box.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );

   set_text( "key_copy", "Copy" );

   document.getElementById( "key_shown" ).hidden = false;
   document.getElementById( "key_make" ).hidden = true;

   show_keys( );

   // NOTE: Health's "no unlock keys left" goes at once, its count and the Overview with it (Ian, 2026-10-13).
   load_overview( );
}

async function do_copy_key( )
{
   try
   {
      await navigator.clipboard.writeText( document.getElementById( "key_text" ).textContent );

      set_text( "key_copy", "Copied" );
   }
   catch( e )
   {
      set_text( "key_copy", "Copy it by hand" );
   }
}

// NOTE: Once stored, the key leaves the page - it is shown once.
function forget_shown_key( )
{
   set_text( "key_text", "" );

   document.getElementById( "key_qr" ).replaceChildren( );
   document.getElementById( "key_shown" ).hidden = true;
   document.getElementById( "key_make" ).hidden = false;
}

// ---- Alerts (Damon, 2026-10-12)

// NOTE: The node's own topic, as it last said - "" where it would not say.
var g_alerts_topic = "";

// NOTE: The code opens the ntfy app ("ntfy://") or ntfy's web page for the topic - an iPhone's camera opens only the
// second ("No usable data" for the first, 2026-10-06); the page shows the topic, to copy into the app.
var g_alerts_web = false;

function alerts_saved_server( )
{
   try
   {
      return localStorage.getItem( c_ntfy_server_key ) || "";
   }
   catch( e )
   {
      return "";
   }
}

async function show_alerts( )
{
   var field = document.getElementById( "alerts_server" );

   field.value = alerts_saved_server( );
   field.placeholder = ntfy_server_example( window.location.hostname );

   set_text( "alerts_server_error", "" );
   set_text( "alerts_test_status", "" );

   var asked_for = ciyam.access;

   var reply = await ask_node( c_home_ntfy_topic_request );

   if( ciyam.access !== asked_for )
      return;

   var topic = ( reply === null ) ? "" : String( reply ).trim( );

   g_alerts_topic = is_ntfy_topic( topic ) ? topic : "";

   set_text( "alerts_copy", "Copy topic" );

   draw_alerts( );
}

function draw_alerts( )
{
   var server = alerts_saved_server( );
   var box = document.getElementById( "alerts_subscribe" );
   var qr = document.getElementById( "alerts_qr" );

   qr.replaceChildren( );

   document.getElementById( "alerts_admin_today" ).hidden = ( g_alerts_topic === "" );

   if( g_alerts_topic === "" )
   {
      set_health_pill( "alerts_admin_pill", "quiet", "Not available" );
      set_text( "alerts_admin_line", "This node doesn't let a page read its alert topic yet - only a development node does, through"
       + " the raw protocol." );

      box.hidden = true;

      return;
   }

   set_text( "alerts_topic", g_alerts_topic );

   if( server === "" )
   {
      set_health_pill( "alerts_admin_pill", "warn", "Server needed" );
      set_text( "alerts_admin_line", "Save the ntfy server above, and a phone can subscribe with a code." );

      box.hidden = true;

      return;
   }

   set_health_pill( "alerts_admin_pill", "ok", "Ready" );
   set_text( "alerts_admin_line", "The node's own topic - subscribe admin's phone to it." );

   var link = g_alerts_web ? ntfy_web_link( server, g_alerts_topic ) : ntfy_subscribe_link( server, g_alerts_topic );

   // NOTE: Drawn in the page - "qrcode.min.js" - so the topic goes nowhere else.
   new QRCode( qr, { text: link, width: c_qr_size, height: c_qr_size, colorDark: "#1b1f2a", colorLight: "#ffffff",
    correctLevel: QRCode.CorrectLevel.M } );

   // NOTE: The library titles its image with the text - the topic is a secret, so no tooltip gives it away.
   qr.removeAttribute( "title" );
   qr.querySelectorAll( "[title]" ).forEach( function( element ) { element.removeAttribute( "title" ); } );

   set_text( "alerts_code_note", g_alerts_web ? "Opens ntfy's page for the topic - copy the topic from it into the app, with "
    + server + " as the server." : "Opens the ntfy app on the subscription - on Android, from the camera's Scan." );

   [ [ "alerts_code_app", !g_alerts_web ], [ "alerts_code_web", g_alerts_web ] ].forEach( function( pair )
   {
      var button = document.getElementById( pair[ 0 ] );

      button.classList.toggle( "is-current", pair[ 1 ] );
      button.setAttribute( "aria-pressed", pair[ 1 ] ? "true" : "false" );
   } );

   box.hidden = false;
}

function save_alerts_server( event )
{
   event.preventDefault( );

   var server = normalise_ntfy_server( document.getElementById( "alerts_server" ).value );

   if( server === "" )
   {
      set_text( "alerts_server_error", "Not a server address - like " + ntfy_server_example( window.location.hostname ) + "." );

      return;
   }

   try
   {
      localStorage.setItem( c_ntfy_server_key, server );
   }
   catch( e )
   {
      set_text( "alerts_server_error", "This browser would not keep it." );

      return;
   }

   document.getElementById( "alerts_server" ).value = server;

   set_text( "alerts_server_error", "" );

   draw_alerts( );
}

async function send_test_alert( )
{
   var button = document.getElementById( "alerts_test" );

   button.disabled = true;

   set_text( "alerts_test_status", "Sending..." );

   var asked_for = ciyam.access;

   var reply = await ask_node( c_home_ntfy_test_request );

   if( ciyam.access !== asked_for )
   {
      button.disabled = false;

      return;
   }

   set_text( "alerts_test_status", ( reply === null ) ? "The node could not send it." : "Sent - it should be on the phone in a moment." );

   button.disabled = false;
}

async function copy_alerts_topic( )
{
   try
   {
      await navigator.clipboard.writeText( g_alerts_topic );

      set_text( "alerts_copy", "Copied" );
   }
   catch( e )
   {
      set_text( "alerts_copy", "Copy it by hand" );
   }
}

// ---- Logs

async function load_logs( )
{
   if( !ciyam.is_admin )
      return;

   var asked_for = ciyam.access;

   set_text( "log_status", "Reading…" );

   var names = await request( function( done ) { return ciyam.fetch( admin_url( "/logs" ), "GET", done ); } );

   if( ciyam.access !== asked_for )
      return;

   g_log_names = is_error_response( names ) ? [ ] : parse_log_names( names );

   if( g_log_names.indexOf( g_log_name ) < 0 )
      g_log_name = g_log_names.length > 0 ? g_log_names[ 0 ] : "";

   var tabs = document.getElementById( "log_tabs" );

   tabs.replaceChildren( );

   g_log_names.forEach( function( name )
   {
      var tab = document.createElement( "button" );

      tab.type = "button";
      tab.className = "home-log-tab";
      tab.textContent = name;
      tab.setAttribute( "role", "tab" );
      tab.setAttribute( "aria-selected", String( name === g_log_name ) );
      tab.addEventListener( "click", function( ) { g_log_name = name; g_log_problems_only = false; load_logs( ); } );

      tabs.appendChild( tab );
   } );

   if( g_log_name === "" )
   {
      g_log_lines = [ ];

      set_text( "log_status", is_error_response( names ) ? error_text( names ) : "There are no logs." );

      draw_log( document.getElementById( "log_lines" ), [ ] );

      return;
   }

   var lines = await read_log( g_log_name, parseInt( document.getElementById( "log_count" ).value, 10 ) );

   if( ciyam.access !== asked_for )
      return;

   g_log_lines = lines || [ ];

   show_log( );
}

function show_log( )
{
   var count = parseInt( document.getElementById( "log_count" ).value, 10 );

   var found = g_log_problems_only ? problem_lines( g_log_lines ) : null;

   var view = log_view( found ? found.lines : g_log_lines, document.getElementById( "log_filter" ).value,
    ( count > 0 ) ? count : g_log_lines.length );

   document.getElementById( "log_problems_only" ).hidden = !found;

   if( found )
      set_text( "log_problems_label", "Errors and warnings " + ( found.since_start ? "since the restart" : "in the lines read" )
       + " - " + found.lines.length + ", as the Overview counts them" );

   draw_log( document.getElementById( "log_lines" ), view.lines );

   set_text( "log_status", "'" + g_log_name + "' - " + ( ( view.lines.length < view.total ) ? "the last " + view.lines.length + " of " : "" )
    + view.total + " line" + ( view.total === 1 ? "" : "s" ) + ( found ? " - problems only" : "" )
    + ( document.getElementById( "log_filter" ).value.trim( ) !== "" ? " matching" : "" ) );

   var pre = document.getElementById( "log_lines" );

   pre.scrollTop = pre.scrollHeight;
}

// NOTE: A section's data is read when it is shown, not before - Logs can be large.
function on_section_shown( section )
{
   if( ( section === "overview" ) || ( section === "health" ) )
      load_overview( );
   else if( section === "people" )
   {
      show_people( g_people_opening );

      g_people_opening = "people";
   }
   else if( section === "keys" )
      show_keys( );
   else if( section === "alerts" )
      show_alerts( );
   else if( section === "logs" )
      load_logs( );
}

function install_admin( )
{
   document.getElementById( "problems_show" ).addEventListener( "click", show_problems_in_logs );
   document.getElementById( "log_problems_clear" ).addEventListener( "click", function( ) { g_log_problems_only = false; show_log( ); } );

   document.getElementById( "overview_refresh" ).addEventListener( "click", load_overview );
   document.getElementById( "health_refresh" ).addEventListener( "click", load_overview );

   document.getElementById( "alerts_server_form" ).addEventListener( "submit", save_alerts_server );
   document.getElementById( "alerts_test" ).addEventListener( "click", send_test_alert );
   document.getElementById( "alerts_copy" ).addEventListener( "click", copy_alerts_topic );

   [ [ "alerts_code_app", false ], [ "alerts_code_web", true ] ].forEach( function( pair )
   {
      document.getElementById( pair[ 0 ] ).addEventListener( "click", function( ) { g_alerts_web = pair[ 1 ]; draw_alerts( ); } );
   } );
   document.getElementById( "logs_refresh" ).addEventListener( "click", load_logs );
   document.getElementById( "key_make" ).addEventListener( "click", do_make_key );
   document.getElementById( "key_copy" ).addEventListener( "click", do_copy_key );
   document.getElementById( "key_stored" ).addEventListener( "click", forget_shown_key );
   document.getElementById( "log_filter" ).addEventListener( "input", show_log );
   // NOTE: More lines are read from the node, not cut from what was read.
   document.getElementById( "log_count" ).addEventListener( "change", load_logs );

   [ [ "overview_make_key", "keys" ], [ "overview_open_logs", "logs" ], [ "health_make_key", "keys" ], [ "health_open_logs", "logs" ],
    [ "attention_all", "health" ], [ "attention_rest_link", "health" ], [ "well_health", "health" ] ].forEach( function( pair )
   {
      document.getElementById( pair[ 0 ] ).addEventListener( "click", function( event ) { event.preventDefault( ); go( pair[ 1 ] ); } );
   } );

   // NOTE: People is one of Home's own sections since 2026-10-11 - it was the accounts page's.
   document.getElementById( "overview_people_link" ).addEventListener( "click", function( event ) { event.preventDefault( ); go( "people" ); } );

   document.getElementById( "overview_add_person" ).addEventListener( "click", function( event )
   {
      event.preventDefault( );

      g_people_opening = "add";

      go( "people" );
   } );

   install_people( );
}

// NOTE: Another tab on this session signed out of every app - the session has gone, so nothing is asked of the node.
function signed_out_elsewhere( )
{
   stop_refresh( );
   forget_shown_key( );
   close_console( );
   unlink_apps( );
   signin_forget_session( );
   clear_ciyam( );
   forget_admin( );
   shell_close_rail( );

   history.replaceState( null, "", window.location.pathname + window.location.search );

   check_node( ).then( function( )
   {
      signin_set_text( "signin_error", "You signed out in another tab." );
   } );
}

// ====================================================================
// The console drawer - as in the chat and the accounts page (2026-10-08)
// ====================================================================

const c_log_channel_name = "ciyam_console_log";

var g_log_channel = null;
var g_console_loaded = false;
var g_request_log = [ ];
var g_request_log_id = 0;
var g_request_quiet = false;

// NOTE: Home's requests are logged for a console, as the chat's and the accounts page's are - the quiet ones (a
// console's own, passed through here) only while the console's "Log polling" is ticked.
function install_console( )
{
   install_log_capture( ciyam, "home", function( ) { return g_request_quiet && !is_logging_polling( ); }, record_request );

   if( typeof BroadcastChannel !== "undefined" )
   {
      g_log_channel = new BroadcastChannel( c_log_channel_name );
      g_log_channel.addEventListener( "message", on_log_message );
   }

   document.getElementById( "console_toggle" ).addEventListener( "click", toggle_console );
   document.getElementById( "console_popout" ).addEventListener( "click", popout_console );
   document.getElementById( "console_close" ).addEventListener( "click", toggle_console );
}

function record_request( entry )
{
   entry.id = ++g_request_log_id;

   g_request_log.push( entry );

   if( g_request_log.length > c_console_log_capacity )
      g_request_log.shift( );

   if( g_log_channel !== null )
      g_log_channel.postMessage( { kind: "entry", owner: g_self, entry: entry } );
}

// NOTE: A linked console sends its requests here, so Home's queue keeps them one at a time (ISS-005, ISS-020).
function on_log_message( event )
{
   var data = event.data;

   if( ( data === null ) || ( typeof data !== "object" ) || ( data.owner !== g_self ) )
      return;

   if( data.kind === "replay" )
      g_log_channel.postMessage( { kind: "entries", owner: g_self, viewer: data.viewer, entries: g_request_log } );
   else if( data.kind === "request" )
      proxy_console_request( data );
}

function proxy_console_request( data )
{
   function reply( response )
   {
      g_log_channel.postMessage( { kind: "response", owner: g_self, viewer: data.viewer, id: data.id, response: response } );
   }

   var methods = [ "GET", "POST", "PUT", "DELETE" ];

   if( ( ciyam.sessid === "" ) || ( typeof data.url !== "string" )
    || ( data.url.indexOf( ciyam.get_cws_url( ) ) !== 0 ) || ( methods.indexOf( data.method ) < 0 ) )
   {
      reply( null );

      return;
   }

   request( function( done )
   {
      // NOTE: Not logged here - the console logs its own request.
      g_request_quiet = true;

      var pending = ciyam.fetch( data.url, data.method, done );

      g_request_quiet = false;

      return pending;
   } ).then( function( response )
   {
      reply( ( response === c_no_answer ) ? null : response );
   } );
}

function toggle_console( )
{
   var drawer = document.getElementById( "console_drawer" );

   drawer.hidden = !drawer.hidden;

   set_text( "console_session", "inherits session " + ciyam.sessid );

   if( !drawer.hidden && !g_console_loaded )
   {
      g_console_loaded = true;

      // NOTE: "from=home" so the console names Home, and filters its log by it.
      apps_console_frame_load( document.getElementById( "console_drawer" ), "console.html?embedded=1&from=home&source=" + encodeURIComponent( g_self ) );
   }
}

function popout_console( )
{
   open_app_tab( "ciyam-console", "console.html?from=home&source=" + encodeURIComponent( g_self ), ciyam.sessid );

   if( !document.getElementById( "console_drawer" ).hidden )
      toggle_console( );
}

// NOTE: A console opened for one session must not carry on under the next.
function close_console( )
{
   g_console_loaded = false;
   g_request_log = [ ];

   document.getElementById( "console_drawer" ).hidden = true;
   apps_console_frame_unload( );
}

// ====================================================================
// People - adding, handing over and removing accounts, moved here from the accounts page (Damon, 2026-10-11)
// ====================================================================

// NOTE: As the accounts page had them: four views in the one section - the list, Add a person, the code to hand over
// and the PIN ready. A code and a new PIN are shown once - leaving the view, or the section, lets them go.
const c_people_views = [ "people", "add", "handover", "ready" ];

const c_people_alert_ms = 6000;

const c_people_copied_ms = 2000;

const c_people_qr_size = 176;

// NOTE: A PIN is for its user and the administrator (Ian, 2026-10-10), and Home may be left open to watch the node
// (2026-10-11) - so People hides each PIN until "Show" is chosen for its row.
const c_hidden_pin = "•••••";

var g_people = [ ];
var g_people_armed = "";
var g_people_shown = { };

// NOTE: People's search and the page shown (2026-10-28) - kept while Home is open, the page held within those there are.
var g_people_query = "";
var g_people_page = 1;
var g_handover_code = "";
var g_ready = null;
var g_people_alert_timer = null;

// NOTE: The view People opens on - the list, unless the Overview's "Add a person" asked for its form.
var g_people_opening = "people";

// NOTE: What went wrong with a request, in words - "" if nothing did.
function people_problem( response )
{
   if( is_timeout_response( response ) )
      return "The server didn't answer in time - try again.";

   if( is_error_response( response ) )
      return error_text( response );

   return "";
}

function show_people( view )
{
   c_people_views.forEach( function( name )
   {
      document.getElementById( name + "_section" ).hidden = ( name !== view );
   } );

   if( view !== "handover" )
      forget_handover( );

   if( view !== "ready" )
      forget_ready( );

   if( view === "people" )
      load_people( );
   else if( view === "add" )
      reset_add_form( );

   var heading = document.querySelector( "#" + view + "_section h1" );

   if( heading !== null )
      heading.focus( { preventScroll: true } );
}

function people_alert( text, is_error )
{
   var alert = document.getElementById( "people_alert" );

   alert.textContent = text;
   alert.classList.toggle( "is-error", !!is_error );
   alert.hidden = false;

   if( g_people_alert_timer !== null )
      window.clearTimeout( g_people_alert_timer );

   g_people_alert_timer = window.setTimeout( function( ) { alert.hidden = true; }, c_people_alert_ms );
}

async function load_people( )
{
   var asked_for = ciyam.access;

   var response = await request( function( done )
   {
      return ciyam.fetch_users( done );
   } );

   // NOTE: Signed out, or in again as someone else, while it was asked - the answer is not theirs (found by review).
   if( ciyam.access !== asked_for )
      return;

   if( people_problem( response ) !== "" )
   {
      g_people = [ ];

      document.getElementById( "people_rows" ).replaceChildren( );

      set_text( "people_empty", "The list could not be read: " + people_problem( response ) );

      return;
   }

   g_people = parse_people( response, ciyam.access, ciyam.username || "admin" );

   render_people( );
}

// NOTE: A page of those the search matches - the search, the count and the pages only once there is more than a page.
function render_people( )
{
   var body = document.getElementById( "people_rows" );

   body.replaceChildren( );

   var many = ( g_people.length > c_people_page_size );
   var query = many ? g_people_query.trim( ) : "";
   var matched = people_matching( g_people, query );
   var shown = people_page( matched.length, g_people_page, c_people_page_size );

   g_people_page = shown.page;

   document.getElementById( "people_tools" ).hidden = !many;
   document.getElementById( "people_pager" ).hidden = ( shown.pages <= 1 );

   set_text( "people_count", people_count_text( g_people.length, matched.length, query !== "", shown ) );
   set_text( "people_page", "Page " + shown.page + " of " + shown.pages );

   document.getElementById( "people_prev" ).disabled = ( shown.page <= 1 );
   document.getElementById( "people_next" ).disabled = ( shown.page >= shown.pages );

   set_text( "people_empty", ( g_people.length === 0 ) ? "Nobody yet." : ( ( matched.length === 0 ) ? "No one matches that search." : "" ) );

   matched.slice( shown.from, shown.to ).forEach( function( person )
   {
      var row = document.getElementById( "tpl_person" ).content.firstElementChild.cloneNode( true );

      row.dataset.pin = person.pin;

      var avatar = row.querySelector( ".account-avatar" );
      var name = row.querySelector( ".account-name" );

      avatar.textContent = user_initial( person.name );

      if( person.name !== "" )
      {
         avatar.style.background = "var(--color-sender-" + sender_colour_index( person.name ) + ")";

         name.textContent = person.name;
      }
      else
      {
         avatar.classList.add( "is-unknown" );

         name.textContent = "No username yet";
         name.classList.add( "is-unknown" );

         row.querySelector( ".account-sub" ).textContent = "Waiting for its first sign in";
      }

      var shown = !!g_people_shown[ person.pin ];
      var show = row.querySelector( ".account-pin-show" );

      row.querySelector( ".account-pin" ).textContent = shown ? person.pin : c_hidden_pin;

      show.textContent = shown ? "Hide" : "Show";
      show.setAttribute( "aria-label", ( shown ? "Hide the PIN of " : "Show the PIN of " ) + ( person.name || "this account" ) );
      show.onclick = function( ) { toggle_pin( person.pin ); };

      var pill = row.querySelector( ".account-pill" );

      pill.textContent = { admin: "Administrator", active: "Active", unclaimed: "Not yet claimed" }[ person.status ];
      pill.classList.add( "account-pill--" + person.status );

      var remove = row.querySelector( ".account-remove" );
      var confirm = row.querySelector( ".account-confirm" );

      // NOTE: Named, not by PIN - the PIN may be hidden.
      var called = person.name || "this unclaimed account";

      if( person.you )
      {
         row.querySelector( ".account-you" ).hidden = false;

         remove.hidden = true;
      }
      else if( g_people_armed === person.pin )
      {
         remove.hidden = true;
         confirm.hidden = false;

         row.querySelector( ".account-confirm-text" ).textContent = "Remove " + called + "? They can no longer sign in.";

         row.querySelector( ".account-confirm-yes" ).onclick = function( ) { do_remove( person.pin ); };
         row.querySelector( ".account-confirm-no" ).onclick = function( ) { arm_remove( "" ); };
      }
      else
      {
         remove.setAttribute( "aria-label", "Remove " + called );
         remove.onclick = function( ) { arm_remove( person.pin ); };
      }

      body.appendChild( row );
   } );

   if( g_people_armed !== "" )
   {
      var keep = body.querySelector( ".account-confirm:not([hidden]) .account-confirm-no" );

      if( keep !== null )
         keep.focus( );
   }
}

function do_people_search( )
{
   g_people_query = document.getElementById( "people_search" ).value;
   g_people_page = 1;
   g_people_armed = "";

   render_people( );
}

// NOTE: Another page - the list's top kept in view, as the pager is under it.
function do_people_page( step )
{
   g_people_page += step;
   g_people_armed = "";

   render_people( );

   var table = document.querySelector( "#people_section .account-table-wrap" );

   if( table.getBoundingClientRect( ).top < 0 )
      table.scrollIntoView( { block: "start" } );
}

function toggle_pin( pin )
{
   if( g_people_shown[ pin ] )
      delete g_people_shown[ pin ];
   else
      g_people_shown[ pin ] = true;

   render_people( );
}

// NOTE: Removing asks once more in the row itself, as the chat's "Block for good" does.
function arm_remove( pin )
{
   g_people_armed = pin;

   render_people( );
}

async function do_remove( pin )
{
   var person = g_people.filter( function( row ) { return row.pin === pin; } )[ 0 ];

   var asked_for = ciyam.access;

   var response = await request( function( done )
   {
      return ciyam.delete_user( pin, done );
   } );

   if( ciyam.access !== asked_for )
      return;

   g_people_armed = "";

   if( people_problem( response ) !== "" )
      people_alert( "Not removed: " + people_problem( response ), true );
   else
      people_alert( ( person && person.name ? person.name : "The unclaimed account" ) + " was removed." );

   load_people( );
}

function add_how( )
{
   return document.querySelector( "input[name='add_how']:checked" ).value;
}

function reset_add_form( )
{
   document.querySelector( "input[name='add_how'][value='code']" ).checked = true;

   document.getElementById( "add_pin" ).value = "";
   document.getElementById( "add_username" ).value = "";
   document.getElementById( "add_suggestion" ).checked = true;

   update_add_form( );

   // NOTE: The list is checked against when a PIN or username is chosen.
   if( g_people.length === 0 )
      load_people_quietly( );
}

async function load_people_quietly( )
{
   var asked_for = ciyam.access;

   var response = await request( function( done ) { return ciyam.fetch_users( done ); } );

   if( ( ciyam.access === asked_for ) && ( people_problem( response ) === "" ) )
      g_people = parse_people( response, ciyam.access, ciyam.username || "admin" );
}

function update_add_form( )
{
   var by_pin = ( add_how( ) === "pin" );

   document.getElementById( "add_pin_fields" ).hidden = !by_pin;
   document.getElementById( "add_submit" ).textContent = by_pin ? "Create the PIN" : "Create a code";

   set_text( "add_error", "" );
}

async function do_add( event )
{
   event.preventDefault( );

   var by_pin = ( add_how( ) === "pin" );

   var pin = document.getElementById( "add_pin" ).value.trim( );
   var username = document.getElementById( "add_username" ).value.trim( );
   var suggestion = document.getElementById( "add_suggestion" ).checked;

   if( by_pin )
   {
      var problem = nominate_problem( pin, username, g_people );

      if( problem !== "" )
      {
         set_text( "add_error", problem );

         return;
      }
   }

   set_text( "add_error", "" );

   var submit = document.getElementById( "add_submit" );

   submit.disabled = true;

   var options = by_pin ? nominated_options( pin, username, suggestion ) : "secret";

   var response = await request( function( done )
   {
      return ciyam.create_user( options, done );
   } );

   submit.disabled = false;

   if( people_problem( response ) !== "" )
   {
      set_text( "add_error", people_problem( response ) );

      return;
   }

   var answer = response.trim( );

   if( by_pin )
   {
      if( !is_account_pin( answer ) )
      {
         set_text( "add_error", "The server's answer was not a PIN: " + answer );

         return;
      }

      show_people( "ready" );

      g_ready = { pin: answer, username: username, suggestion: suggestion };

      render_ready( );
   }
   else
   {
      if( normalise_code( answer ) === "" )
      {
         set_text( "add_error", "The server's answer was not a code: " + answer );

         return;
      }

      show_people( "handover" );

      g_handover_code = answer;

      render_handover( );
   }
}

// NOTE: Where a new person goes - the accounts page, whose Welcome takes a code or a PIN.
function account_page_address( )
{
   return window.location.protocol + "//" + window.location.host + "/" + c_account_page;
}

function render_handover( )
{
   set_text( "handover_code", g_handover_code );
   set_text( "handover_url", account_page_address( ) );
   set_text( "handover_copy", "Copy" );

   var box = document.getElementById( "handover_qr" );

   box.replaceChildren( );

   // NOTE: Drawn in the page - "qrcode.min.js" - so the code goes nowhere else. It opens the accounts page's Welcome.
   new QRCode( box, {
      text: join_url( window.location.protocol + "//" + window.location.host, g_handover_code ),
      width: c_people_qr_size,
      height: c_people_qr_size,
      colorDark: "#1b1f2a",
      colorLight: "#ffffff",
      correctLevel: QRCode.CorrectLevel.M
   } );

   // NOTE: The library gives its image a "title" of the address, code and all - a tooltip anyone looking over admin's
   // shoulder could read.
   box.removeAttribute( "title" );
   box.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );
}

// NOTE: A new PIN is shown once - not left in the page when its view goes (found by review).
function forget_ready( )
{
   g_ready = null;

   [ "ready_pin", "ready_url", "ready_username", "ready_username_note" ].forEach( function( id ) { set_text( id, "" ); } );
}

function forget_handover( )
{
   g_handover_code = "";

   set_text( "handover_code", "" );

   document.getElementById( "handover_qr" ).replaceChildren( );
}

async function do_copy_code( )
{
   var button = document.getElementById( "handover_copy" );

   try
   {
      await navigator.clipboard.writeText( g_handover_code );

      button.textContent = "Copied";
   }
   catch( e )
   {
      // NOTE: The clipboard needs HTTPS or "localhost"; elsewhere the code is selected instead, ready to copy by hand.
      var range = document.createRange( );

      range.selectNodeContents( document.getElementById( "handover_code" ) );

      window.getSelection( ).removeAllRanges( );
      window.getSelection( ).addRange( range );

      button.textContent = "Selected - copy it";
   }

   window.setTimeout( function( ) { button.textContent = "Copy"; }, c_people_copied_ms );
}

function render_ready( )
{
   set_text( "ready_pin", g_ready.pin );
   set_text( "ready_url", account_page_address( ) );

   if( g_ready.username === "" )
   {
      set_text( "ready_username", "Their choice" );
      set_text( "ready_username_note", "They choose one when they set their password" );
   }
   else
   {
      set_text( "ready_username", g_ready.username );
      set_text( "ready_username_note", g_ready.suggestion ? "A suggestion - they can change it" : "Chosen by you" );
   }
}

// NOTE: What People holds is not left for whoever signs in next in this tab.
function forget_people( )
{
   g_people = [ ];
   g_people_armed = "";
   g_people_shown = { };
   g_people_query = "";
   g_people_page = 1;

   document.getElementById( "people_search" ).value = "";

   forget_ready( );
   forget_handover( );

   document.getElementById( "people_rows" ).replaceChildren( );
   document.getElementById( "people_alert" ).hidden = true;

   set_text( "people_empty", "Loading..." );
}

function install_people( )
{
   document.getElementById( "people_add" ).addEventListener( "click", function( ) { show_people( "add" ); } );

   var search = document.getElementById( "people_search" );

   search.addEventListener( "input", do_people_search );

   // NOTE: Escape clears it, as a search box does.
   search.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Escape" ) && ( search.value !== "" ) )
      {
         event.preventDefault( );

         search.value = "";

         do_people_search( );
      }
   } );

   document.getElementById( "people_prev" ).addEventListener( "click", function( ) { do_people_page( -1 ); } );
   document.getElementById( "people_next" ).addEventListener( "click", function( ) { do_people_page( 1 ); } );
   document.getElementById( "add_form" ).addEventListener( "submit", do_add );
   document.getElementById( "handover_copy" ).addEventListener( "click", do_copy_code );

   document.querySelectorAll( "input[name='add_how']" ).forEach( function( input )
   {
      input.addEventListener( "change", update_add_form );
   } );

   document.querySelectorAll( "#section_people [data-people='list']" ).forEach( function( button )
   {
      button.addEventListener( "click", function( ) { show_people( "people" ); } );
   } );
}

// NOTE: Admin's figures, logs and the log chosen are not left for whoever signs in next in this tab (found by review).
function forget_admin( )
{
   forget_people( );

   forget_title( );

   g_log_names = [ ];
   g_log_name = "server";
   g_log_lines = [ ];
   g_log_problems_only = false;

   document.getElementById( "log_filter" ).value = "";

   [ "log_tabs", "log_lines", "overview_log", "attention_list", "rooms_filling", "sessions_rows", "schedule_list", "listening_facts" ].forEach( function( id )
   {
      document.getElementById( id ).replaceChildren( );
   } );

   set_text( "alerts_copy", "Copy topic" );

   [ "log_status", "stat_people_note", "overview_people", "overview_keys_note", "connection_heading", "connection_cipher", "attention_heading",
    "overview_keys_line", "health_secure_line", "health_state", "health_uptime", "health_version", "health_kind", "problems_latest",
    "health_identity_pill", "health_connection_pill", "health_log_pill", "health_rooms_pill", "health_node_pill", "sessions_line",
    "health_sessions_pill", "health_schedule_pill", "health_listening_pill", "stat_sessions_note" ]
    .forEach( function( id ) { set_text( id, "" ); } );

   show_key_unlock( null );

   // NOTE: The Overview's sessions figure back as the form has it - waiting, not "Not reported" - and the table shut.
   document.getElementById( "stat_sessions_unit" ).hidden = false;
   document.getElementById( "stat_sessions_value" ).classList.remove( "home-stat-value--quiet" );
   document.getElementById( "sessions_wrap" ).hidden = true;

   [ "attention_card", "attention_rest", "well_card" ].forEach( function( id ) { document.getElementById( id ).hidden = true; } );

   g_alerts_topic = "";
   g_alerts_web = false;

   document.getElementById( "alerts_qr" ).replaceChildren( );
   document.getElementById( "alerts_subscribe" ).hidden = true;

   [ "alerts_topic", "alerts_test_status", "alerts_admin_line", "alerts_admin_pill" ].forEach( function( id ) { set_text( id, "" ); } );

   set_text( "stat_people", "…" );
   set_text( "stat_uptime", "…" );
   set_text( "stat_sessions", "…" );
}

// NOTE: Signing out of Home signs every app it opened out too - they were on its session (the design's
// "Sign out").
async function do_sign_out( )
{
   stop_refresh( );

   forget_shown_key( );

   close_console( );

   unlink_apps( );

   signin_forget_session( );

   await request( function( done )
   {
      return ciyam.disconnect( done );
   } );

   // NOTE: "disconnect( )" ends the session on the server but leaves the instance as it was.
   clear_ciyam( );

   forget_admin( );

   shell_close_rail( );

   history.replaceState( null, "", window.location.pathname + window.location.search );

   check_node( );
}


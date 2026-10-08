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
   signin_build( document.getElementById( "signin_host" ), {
      note: "home",
      title: "Home",
      lede: "Sign in to your node - your apps, and what needs you.",
      setup_href: "account.html#welcome",
      request: request,
      on_signed_in: on_signed_in,
      error_text: function( error ) { return ( g_system.state === "locked" ) ? locked_sign_in_text( error ) : ""; }
   } );

   document.getElementById( "unreachable_retry" ).addEventListener( "click", check_node );
   document.getElementById( "setup_retry" ).addEventListener( "click", check_node );
   document.getElementById( "unlock_form" ).addEventListener( "submit", do_unlock );
   document.getElementById( "rescue_form" ).addEventListener( "submit", do_rescue );
   document.getElementById( "unlock_sign_out" ).addEventListener( "click", do_sign_out );
   document.getElementById( "app_sign_out" ).addEventListener( "click", do_sign_out );

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

   if( screen === "signin" )
   {
      signin_describe( "Home", "Sign in to your node - your apps, and what needs you.", true );
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
   else
      show_view( screen === "setup" ? "setup_view" : "unreachable_view" );
}

function on_signed_in( )
{
   after_sign_in( );
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

async function enter_app( )
{
   var name = ciyam.username || ciyam.access;

   set_text( "app_user", name + ( ( ciyam.is_admin && ( name !== "admin" ) ) ? " - admin" : "" ) );
   set_text( "app_greeting", "Hello, " + name );

   var uptime = uptime_words( await fetch_text( "/uptime" ) );

   set_text( "app_node", "Home node - CIYAM " + g_system.version + ( uptime !== "" ? " - up " + uptime : "" ) );

   show_view( "app_view" );
}

async function do_sign_out( )
{
   await request( function( done )
   {
      return ciyam.disconnect( done );
   } );

   // NOTE: "disconnect( )" ends the session on the server but leaves the instance as it was.
   ciyam.sessid = "";
   ciyam.access = "";
   ciyam.hashed = "";
   ciyam.unique = "";
   ciyam.username = "";
   ciyam.is_admin = false;

   check_node( );
}

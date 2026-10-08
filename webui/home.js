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

   install_shell( );

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

// ====================================================================
// The shell - build step 4
// ====================================================================

// NOTE: The icons, as the design's side menu draws them - a path each, in a 24 unit box.
const c_icons = {
   home: "M3 10.5 12 3l9 7.5V20a1 1 0 0 1-1 1h-5v-6h-6v6H4a1 1 0 0 1-1-1z",
   chat: "M4 5h16v11H8l-4 4z",
   account: "M16 19v-1a4 4 0 0 0-4-4H7a4 4 0 0 0-4 4v1M10 11a3 3 0 1 0 0-6 3 3 0 0 0 0 6M21 19v-1a4 4 0 0 0-3-3.9M16 5.1a3 3 0 0 1 0 5.8",
   console: "M4 6l5 5-5 5M12 18h8",
   overview: "M12 14l4-4M3.5 18a9 9 0 1 1 17 0",
   keys: "M15 7a4 4 0 1 1-3.5 6L4 20.5H2v-3l7.5-7.5A4 4 0 0 1 15 7z",
   logs: "M5 4h14v16H5zM8 8h8M8 12h8M8 16h5",
   external: "M7 17 17 7M9 7h8v8"
};

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

function icon( name, size )
{
   var svg = document.createElementNS( "http://www.w3.org/2000/svg", "svg" );

   svg.setAttribute( "width", String( size || 18 ) );
   svg.setAttribute( "height", String( size || 18 ) );
   svg.setAttribute( "viewBox", "0 0 24 24" );
   svg.setAttribute( "fill", "none" );
   svg.setAttribute( "stroke", "currentColor" );
   svg.setAttribute( "stroke-width", "1.8" );
   svg.setAttribute( "stroke-linecap", "round" );
   svg.setAttribute( "stroke-linejoin", "round" );
   svg.setAttribute( "aria-hidden", "true" );

   var path = document.createElementNS( "http://www.w3.org/2000/svg", "path" );

   path.setAttribute( "d", c_icons[ name ] || "" );

   svg.appendChild( path );

   return svg;
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
      var data = String( event.data );

      var pos = data.indexOf( ":" );

      if( pos < 0 )
         return;

      var target = data.substr( 0, pos );
      var viewer = data.substring( pos + 1 );

      if( ( target !== g_self ) || ( ciyam.sessid === "" ) || !/^[0-9]+$/.test( viewer ) )
         return;

      g_channel.postMessage( viewer + "-" + g_self );

      g_channel.postMessage( viewer + "=" + ciyam.access + "," + ciyam.device + "," + ciyam.hashed + "," + ciyam.sessid + ","
       + ciyam.unique + "," + encode_channel_field( ciyam.username ) + "," + ( ciyam.is_admin ? "1" : "0" ) );
   } );
}

function unlink_apps( )
{
   if( g_channel !== null )
      g_channel.postMessage( g_self );
}

function rail_item( key, title, opens_tab, on_click )
{
   var item = document.createElement( "a" );

   item.className = "home-rail-item";
   item.href = opens_tab ? "#" : "#" + key;
   item.dataset.key = key;

   item.appendChild( icon( key ) );

   var label = document.createElement( "span" );

   label.className = "home-rail-label";
   label.textContent = title;

   item.appendChild( label );

   if( opens_tab )
   {
      item.title = "Opens in its own tab, on this session";

      var mark = icon( "external", 14 );

      mark.classList.add( "home-rail-external" );

      item.appendChild( mark );
   }

   item.addEventListener( "click", function( event )
   {
      event.preventDefault( );

      on_click( );
   } );

   return item;
}

function bottom_item( key, title, on_click )
{
   var item = document.createElement( "a" );

   item.className = "home-bottom-item";
   item.href = "#";
   item.dataset.key = key;

   item.appendChild( icon( key, 22 ) );

   var label = document.createElement( "span" );

   label.textContent = title;

   item.appendChild( label );

   item.addEventListener( "click", function( event )
   {
      event.preventDefault( );

      on_click( );
   } );

   return item;
}

function open_app( app )
{
   close_rail( );

   if( app.key === "home" )
   {
      go( home_section( ciyam.is_admin, "" ) );

      return;
   }

   open_app_tab( app.tab, linked_app_address( app.page, g_self ), ciyam.sessid );
}

// NOTE: Built afresh at each sign in - who is signed in decides the apps and the sections.
function build_shell( )
{
   var apps = home_apps( ciyam.is_admin, false, false );
   var phone_apps = home_apps( ciyam.is_admin, false, true );
   var sections = home_sections( ciyam.is_admin );

   var rail_apps = document.getElementById( "rail_apps" );
   var rail_sections = document.getElementById( "rail_sections" );
   var chips = document.getElementById( "home_chips" );
   var bottom = document.getElementById( "home_bottombar" );

   [ rail_apps, rail_sections, chips, bottom ].forEach( function( element ) { element.replaceChildren( ); } );

   apps.forEach( function( app )
   {
      rail_apps.appendChild( rail_item( app.key, app.title, ( app.key !== "home" ), function( ) { open_app( app ); } ) );
   } );

   phone_apps.forEach( function( app )
   {
      bottom.appendChild( bottom_item( app.key, ( app.key === "account" ) ? "Account" : app.title, function( ) { open_app( app ); } ) );
   } );

   sections.forEach( function( section )
   {
      rail_sections.appendChild( rail_item( section.key, section.title, false, function( ) { close_rail( ); go( section.key ); } ) );

      var chip = document.createElement( "a" );

      chip.className = "home-chip";
      chip.href = "#" + section.key;
      chip.dataset.key = section.key;
      chip.textContent = section.title;

      chip.addEventListener( "click", function( event ) { event.preventDefault( ); go( section.key ); } );

      chips.appendChild( chip );
   } );

   document.getElementById( "rail_node" ).hidden = ( sections.length === 0 );
   chips.hidden = ( sections.length === 0 );

   var name = ciyam.username || ciyam.access;

   [ "rail_avatar", "top_avatar" ].forEach( function( id )
   {
      var avatar = document.getElementById( id );

      avatar.textContent = user_initial( name );
      avatar.style.background = "var(--color-sender-" + sender_colour_index( name ) + ")";
   } );

   set_text( "rail_name", name );
   set_text( "rail_role", role_text( ciyam.is_admin ) );
   set_text( "rail_pin", ciyam.access );
   set_text( "top_sub", name + " · " + role_text( ciyam.is_admin ) );
}

// NOTE: One of Home's own sections - the address's "#..." follows, so a reload or Back comes back to it.
function go( section )
{
   g_section = section;

   [ "home", "overview", "keys", "logs" ].forEach( function( key )
   {
      document.getElementById( "section_" + key ).hidden = ( key !== section );
   } );

   var titles = { home: "Home", overview: "Overview", keys: "Unlock keys", logs: "Logs" };

   set_text( "top_heading", titles[ section ] || "Home" );

   document.querySelectorAll( ".home-rail-item, .home-chip, .home-bottom-item" ).forEach( function( item )
   {
      var current = ( item.dataset.key === section ) || ( ( item.dataset.key === "home" ) && ( section === "home" ) );

      item.classList.toggle( "is-current", current );

      if( current )
         item.setAttribute( "aria-current", "page" );
      else
         item.removeAttribute( "aria-current" );
   } );

   // NOTE: Home's own item stands for admin's sections too, on the phone's bottom bar.
   if( ciyam.is_admin )
   {
      document.querySelectorAll( ".home-bottom-item[data-key='home']" ).forEach( function( item )
      {
         item.classList.add( "is-current" );
      } );
   }

   if( window.location.hash !== "#" + section )
      history.replaceState( null, "", "#" + section );
}

function open_rail( )
{
   document.getElementById( "home_rail" ).classList.add( "is-open" );
   document.getElementById( "rail_scrim" ).hidden = false;
   document.getElementById( "rail_open" ).setAttribute( "aria-expanded", "true" );
}

function close_rail( )
{
   document.getElementById( "home_rail" ).classList.remove( "is-open" );
   document.getElementById( "rail_scrim" ).hidden = true;
   document.getElementById( "rail_open" ).setAttribute( "aria-expanded", "false" );
}

function install_shell( )
{
   window.name = c_tab_home;

   open_channel( );

   document.getElementById( "rail_open" ).addEventListener( "click", open_rail );
   document.getElementById( "rail_scrim" ).addEventListener( "click", close_rail );

   document.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Escape" ) && document.getElementById( "home_rail" ).classList.contains( "is-open" ) )
         close_rail( );
   } );

   window.addEventListener( "hashchange", function( )
   {
      if( ciyam.sessid !== "" )
         go( home_section( ciyam.is_admin, window.location.hash ) );
   } );
}

async function enter_app( )
{
   build_shell( );

   var name = ciyam.username || ciyam.access;

   set_text( "app_greeting", "Hello, " + name );

   var uptime = uptime_words( await fetch_text( "/uptime" ) );

   var node = "Home node · CIYAM " + g_system.version + ( uptime !== "" ? " · up " + uptime : "" );

   set_text( "app_node", node );
   set_text( "overview_node", node );

   go( home_section( ciyam.is_admin, window.location.hash ) );

   show_view( "app_view" );
}

// NOTE: Signing out of Home signs every app it opened out too - they were on its session (the design's
// "Sign out of every app").
async function do_sign_out( )
{
   unlink_apps( );

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

   close_rail( );

   history.replaceState( null, "", window.location.pathname + window.location.search );

   check_node( );
}


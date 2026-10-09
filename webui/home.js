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
   // NOTE: "Sign out of every app" - here and in the switcher alike, every tab on the session told (2026-10-08).
   document.getElementById( "app_sign_out" ).addEventListener( "click", apps_sign_out_everywhere );

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
   shield: "M12 3l8 3v6c0 4.5-3.4 8.3-8 9-4.6-.7-8-4.5-8-9V6zM9 12l2 2 4-4"
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

function rail_item( key, title, is_app, on_click )
{
   var item = document.createElement( "a" );

   item.className = "home-rail-item";
   item.href = "#" + key;
   item.dataset.key = key;

   item.appendChild( icon( key ) );

   var label = document.createElement( "span" );

   label.className = "home-rail-label";
   label.textContent = title;

   item.appendChild( label );

   if( key === "chat" )
      item.appendChild( chat_badge( ) );

   if( is_app )
      bind_app_link( item, on_click );
   else
   {
      item.addEventListener( "click", function( event )
      {
         event.preventDefault( );

         on_click( event );
      } );
   }

   return item;
}

// NOTE: An app opens in this tab, or a new one with ctrl or cmd and click or the middle button ("apps_go( )").
function bind_app_link( item, on_click )
{
   item.addEventListener( "click", function( event )
   {
      event.preventDefault( );

      on_click( event );
   } );

   item.addEventListener( "auxclick", function( event )
   {
      if( event.button !== 1 )
         return;

      event.preventDefault( );

      on_click( event );
   } );
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
   var item = document.createElement( "a" );

   item.className = "home-bottom-item";
   item.href = "#";
   item.dataset.key = key;

   item.appendChild( icon( key, 22 ) );

   var label = document.createElement( "span" );

   label.textContent = title;

   item.appendChild( label );

   if( key === "chat" )
      item.appendChild( chat_badge( ) );

   bind_app_link( item, on_click );

   return item;
}

// NOTE: What waits in the chat - unread messages, requests and invitations, as the chat's own badge counts them.
function chat_badge( )
{
   var badge = document.createElement( "span" );

   badge.className = "home-badge";
   badge.dataset.badge = "chat";
   badge.hidden = true;

   return badge;
}

// NOTE: In this tab, as the switcher opens it (Damon, 2026-10-09) - a new one when the event asks.
function open_app( app, event )
{
   close_rail( );

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
   var chips = document.getElementById( "home_chips" );
   var bottom = document.getElementById( "home_bottombar" );

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

   // NOTE: A new unlock key is shown once - leaving the section lets it go.
   if( section !== "keys" )
      forget_shown_key( );

   on_section_shown( section );
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

   // NOTE: The switcher at the top of the side menu - the CIYAM mark and "Home", as in every app (2026-10-08).
   apps_build( document.getElementById( "apps_host" ), {
      current: "home",
      self: function( ) { return g_self; },
      sign_out: do_sign_out,
      signed_out: signed_out_elsewhere,
      from: "home"
   } );

   document.getElementById( "rail_open" ).addEventListener( "click", open_rail );
   document.getElementById( "rail_scrim" ).addEventListener( "click", close_rail );

   document.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Escape" ) && document.getElementById( "home_rail" ).classList.contains( "is-open" ) )
         close_rail( );
   } );

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

   var name = ciyam.username || ciyam.access;

   set_text( "app_greeting", "Hello, " + name );

   var uptime = uptime_words( await fetch_text( "/uptime" ) );

   var node = "Home node · CIYAM " + g_system.version + ( uptime !== "" ? " · up " + uptime : "" );

   set_text( "app_node", node );
   set_text( "overview_node", node );

   go( home_section( ciyam.is_admin, window.location.hash ) );

   show_view( "app_view" );

   g_rooms = [ ];
   g_starting_messages = [ ];
   g_starting_total = -1;
   g_devices = [ ];

   render_member_home( );

   await load_member_home( );

   start_refresh( );
}

// ====================================================================
// A member's Home - build step 5
// ====================================================================

const c_lobby_room = "0000000";
const c_administration_room = "0000001";

// NOTE: The key the chat keeps a person's dismissed announcements under, so dismissing on Home dismisses in the
// chat too (Damon, 2026-10-08: one dismissal for both) - the chat hears it through the "storage" event.
const c_storage_dismissed = "cws.dismissed_";

// NOTE: How often Home looks again while it is open - and at once when its tab is shown again.
const c_refresh_ms = 60000;

var g_rooms = [ ];
var g_starting_messages = [ ];
var g_starting_total = -1;
var g_devices = [ ];
var g_refresh_timer = null;

function devices_url( )
{
   return ciyam.get_cws_url( ) + "/devices?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text&session=" + ciyam.sessid;
}

function read_dismissed( )
{
   try
   {
      return parse_dismissed( localStorage.getItem( c_storage_dismissed + ciyam.access ) );
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
      localStorage.setItem( c_storage_dismissed + ciyam.access, JSON.stringify( add_dismissed( read_dismissed( ), unique ) ) );
   }
   catch( e )
   {
   }

   render_member_home( );
}

// NOTE: The lobby listing gives every room and its unread count without moving what the session has read.
// Administration - invitations, requests, announcements - is read whole, and only when its total changes, as
// the chat reads it: a tab of the chat opened from Home shares this session.
async function load_member_home( )
{
   var asked_for = ciyam.access;

   var listing = parse_fetch_response( await request( function( done )
   {
      return ciyam.fetch_messages( c_lobby_room, "", done );
   } ) );

   if( ciyam.access !== asked_for )
      return;

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

   set_text( "tile_chat_text", chat_summary_text( summary ) );

   document.querySelectorAll( "[data-badge='chat']" ).forEach( function( badge )
   {
      badge.textContent = badge_text( summary.badge );
      badge.hidden = ( summary.badge === 0 );
   } );

   // ---- Needs you
   var list = document.getElementById( "needs_list" );
   var items = needs_you( invitations, ciyam.username );

   list.replaceChildren( );

   items.forEach( function( item )
   {
      var row = document.createElement( "li" );

      row.className = "home-need";

      var avatar = document.createElement( "span" );

      avatar.className = "home-avatar" + ( ( item.kind === "invitation" ) ? " is-room" : "" );
      avatar.textContent = ( item.kind === "invitation" ) ? "#" : user_initial( item.inviter );

      if( item.kind !== "invitation" )
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
      answer.textContent = "Answer in Chat";
      bind_app_link( answer, function( event ) { open_app_by_key( "chat", "", event ); } );

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
      text.textContent = message.text;

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
   var name = ciyam.username || ciyam.access;
   var avatar_large = document.getElementById( "account_avatar" );

   avatar_large.textContent = user_initial( name );
   avatar_large.style.background = "var(--color-sender-" + sender_colour_index( name ) + ")";

   set_text( "account_name", name );
   set_text( "account_role", role_text( ciyam.is_admin ) );
   set_text( "account_pin", ciyam.access );

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
   close_rail( );

   apps_go( key, hash || "", event );
}

function start_refresh( )
{
   stop_refresh( );

   g_refresh_timer = window.setInterval( function( )
   {
      if( ( ciyam.sessid !== "" ) && !document.hidden )
         load_member_home( );
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
   document.getElementById( "tile_chat_icon" ).appendChild( icon( "chat", 22 ) );

   bind_app_link( document.getElementById( "tile_chat" ), function( event ) { open_app_by_key( "chat", "", event ); } );

   app_address( document.getElementById( "tile_chat" ), { key: "chat", page: "chat.html" } );

   // NOTE: The accounts page - in this tab now, as every app opens (Damon, 2026-10-09; in its own tab until then).
   bind_app_link( document.getElementById( "manage_account" ), function( event ) { open_app_by_key( "account", "#mine", event ); } );

   app_address( document.getElementById( "manage_account" ), { key: "account", page: "account.html" }, "#mine" );

   document.addEventListener( "visibilitychange", function( )
   {
      if( !document.hidden && ( ciyam.sessid !== "" ) )
         load_member_home( );
   } );

   // NOTE: The chat dismissing an announcement - or another Home - shows here at once.
   window.addEventListener( "storage", function( event )
   {
      if( ( ciyam.access !== "" ) && ( event.key === c_storage_dismissed + ciyam.access ) )
         render_member_home( );
   } );
}

// ====================================================================
// Admin's sections - build step 6
// ====================================================================

// NOTE: How many unlock keys this browser has made - the node cannot list them. Kept per browser, and the
// browser keeps it per node, by its address.
const c_storage_keys_made = "home.unlock_keys_made";

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

function keys_made( )
{
   try
   {
      return parse_key_count( localStorage.getItem( c_storage_keys_made ) );
   }
   catch( e )
   {
      return 0;
   }
}

function count_key_made( )
{
   try
   {
      localStorage.setItem( c_storage_keys_made, String( keys_made( ) + 1 ) );
   }
   catch( e )
   {
   }
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

   set_text( "stat_node", ( g_system.state === "ready" ) ? "Ready" : "Not ready" );
   set_text( "stat_uptime", uptime_words( await fetch_text( "/uptime" ) ) || "-" );

   var security = security_text( g_system.security );

   set_text( "connection_heading", security.heading || "Connection: not reported" );
   set_text( "connection_cipher", ( g_system.cipher !== "" ) ? g_system.cipher + ( g_system.group !== "" ? " (" + g_system.group + ")" : "" )
    : ( g_system.security === "none" ? "Plain HTTP - (NONE)" : "" ) );

   document.getElementById( "connection_icon" ).classList.toggle( "is-warn", g_system.security === "none" );

   set_text( "overview_keys_note", keys_note( keys_made( ) ) );

   var users = await request( function( done ) { return ciyam.fetch_users( done ); } );

   if( ciyam.access !== asked_for )
      return;

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

   show_problems( server );

   var listing = await request( function( done ) { return ciyam.fetch_messages( c_lobby_room, "", done ); } );

   if( ciyam.access !== asked_for )
      return;

   show_filling( is_error_response( listing ) ? null : derive_room_list( parse_fetch_response( listing ).rooms ) );
}

// NOTE: The errors and warnings in the server log since the restart - the latest in words, and Logs filtered to them.
var g_problems_filter = "";

function show_problems( lines )
{
   var card = document.getElementById( "problems_card" );
   var show = document.getElementById( "problems_show" );

   if( lines === null )
   {
      set_text( "problems_heading", "The server log could not be read" );
      set_text( "problems_latest", "" );

      show.hidden = true;

      return;
   }

   var problems = log_problems( lines );
   var any = ( problems.errors + problems.warnings ) > 0;

   g_problems_filter = ( problems.errors > 0 ) ? "error" : "warn";

   set_text( "problems_heading", log_problems_text( problems ) );

   var latest = log_line_parts( problems.latest );

   set_text( "problems_latest", any ? "Latest" + ( latest.time !== "" ? ", " + latest.time : "" ) + ": " + latest.text : "" );

   card.classList.toggle( "home-card--warn", problems.errors > 0 );
   document.getElementById( "problems_icon" ).classList.toggle( "is-warn", problems.errors > 0 );

   show.hidden = !any;
}

function show_problems_in_logs( event )
{
   event.preventDefault( );

   g_log_name = "server";

   document.getElementById( "log_filter" ).value = g_problems_filter;
   document.getElementById( "log_count" ).value = String( c_overview_problem_lines );

   go( "logs" );
}

// NOTE: The rooms near the number of messages a room keeps - a full one refuses new messages. Only the rooms admin
// is in: the listing has no others.
function show_filling( rooms )
{
   var list = document.getElementById( "rooms_filling" );

   list.replaceChildren( );

   if( rooms === null )
   {
      set_text( "rooms_heading", "The rooms could not be read" );

      return;
   }

   var filling = rooms_filling( rooms );

   set_text( "rooms_heading", rooms_filling_text( filling ) );

   filling.slice( 0, c_overview_filling_shown ).forEach( function( room )
   {
      var item = document.createElement( "li" );

      item.textContent = room_filling_line( room );

      if( room.total >= c_home_room_limit )
         item.className = "is-full";

      list.appendChild( item );
   } );

   var full = filling.some( function( room ) { return room.total >= c_home_room_limit; } );

   document.getElementById( "rooms_card" ).classList.toggle( "home-card--warn", filling.length > 0 );
   document.getElementById( "rooms_icon" ).classList.toggle( "is-warn", filling.length > 0 );
   document.getElementById( "rooms_icon" ).classList.toggle( "is-full", full );
}

// ---- Unlock keys

function show_keys( )
{
   set_text( "keys_note", keys_note( keys_made( ) ) );
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

   count_key_made( );

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
      tab.addEventListener( "click", function( ) { g_log_name = name; load_logs( ); } );

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

   var view = log_view( g_log_lines, document.getElementById( "log_filter" ).value, ( count > 0 ) ? count : g_log_lines.length );

   draw_log( document.getElementById( "log_lines" ), view.lines );

   set_text( "log_status", "'" + g_log_name + "' - " + ( ( view.lines.length < view.total ) ? "the last " + view.lines.length + " of " : "" )
    + view.total + " line" + ( view.total === 1 ? "" : "s" ) + ( document.getElementById( "log_filter" ).value.trim( ) !== "" ? " matching" : "" ) );

   var pre = document.getElementById( "log_lines" );

   pre.scrollTop = pre.scrollHeight;
}

// NOTE: A section's data is read when it is shown, not before - Logs can be large.
function on_section_shown( section )
{
   if( section === "overview" )
      load_overview( );
   else if( section === "keys" )
      show_keys( );
   else if( section === "logs" )
      load_logs( );
}

function install_admin( )
{
   document.getElementById( "connection_icon" ).appendChild( icon( "shield", 22 ) );
   document.getElementById( "keys_icon" ).appendChild( icon( "keys", 22 ) );
   document.getElementById( "problems_icon" ).appendChild( icon( "logs", 22 ) );
   document.getElementById( "rooms_icon" ).appendChild( icon( "chat", 22 ) );

   document.getElementById( "problems_show" ).addEventListener( "click", show_problems_in_logs );

   document.getElementById( "overview_refresh" ).addEventListener( "click", load_overview );
   document.getElementById( "logs_refresh" ).addEventListener( "click", load_logs );
   document.getElementById( "key_make" ).addEventListener( "click", do_make_key );
   document.getElementById( "key_copy" ).addEventListener( "click", do_copy_key );
   document.getElementById( "key_stored" ).addEventListener( "click", forget_shown_key );
   document.getElementById( "log_filter" ).addEventListener( "input", show_log );
   // NOTE: More lines are read from the node, not cut from what was read.
   document.getElementById( "log_count" ).addEventListener( "change", load_logs );

   [ [ "overview_make_key", "keys" ], [ "overview_open_logs", "logs" ] ].forEach( function( pair )
   {
      document.getElementById( pair[ 0 ] ).addEventListener( "click", function( event ) { event.preventDefault( ); go( pair[ 1 ] ); } );
   } );

   bind_app_link( document.getElementById( "overview_add_person" ), function( event ) { open_app_by_key( "account", "#add", event ); } );

   bind_app_link( document.getElementById( "overview_people_link" ), function( event ) { open_app_by_key( "account", "#people", event ); } );
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
   close_rail( );

   history.replaceState( null, "", window.location.pathname + window.location.search );

   check_node( ).then( function( )
   {
      signin_set_text( "signin_error", "You signed out in another app." );
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
      document.getElementById( "console_frame" ).src = "console.html?embedded=1&from=home&source=" + encodeURIComponent( g_self );
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
   document.getElementById( "console_frame" ).src = "about:blank";
}

// NOTE: Admin's figures, logs and the log chosen are not left for whoever signs in next in this tab (found by review).
function forget_admin( )
{
   g_log_names = [ ];
   g_log_name = "server";
   g_log_lines = [ ];

   document.getElementById( "log_filter" ).value = "";

   [ "log_tabs", "log_lines", "overview_log" ].forEach( function( id ) { document.getElementById( id ).replaceChildren( ); } );

   [ "log_status", "stat_people_note", "overview_people", "overview_keys_note", "connection_heading", "connection_cipher" ]
    .forEach( function( id ) { set_text( id, "" ); } );

   set_text( "stat_people", "…" );
   set_text( "stat_uptime", "…" );
}

// NOTE: Signing out of Home signs every app it opened out too - they were on its session (the design's
// "Sign out of every app").
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

   close_rail( );

   history.replaceState( null, "", window.location.pathname + window.location.search );

   check_node( );
}


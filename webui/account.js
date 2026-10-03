// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The accounts page - admin's People (adding, handing over, removing) and everyone's own
// account, plus the Welcome screen a new person uses before they have one. Only what the server
// supports today; see the vault's "Account Management Design". Its pure logic is in
// "account_parse.js", with tests.

// NOTE: The same keys as the chat, so the two share this browser's device and saved accounts.
const c_storage_device = "cws.device";
const c_storage_access = "cws.access";
const c_storage_hashed_prefix = "cws.hashed_";
const c_storage_banner = "cws.accounts_banner_hidden";

const c_alert_time = 6000;

const c_copied_time = 2000;

const c_qr_size = 176;

const c_admin_views = [ "people", "add", "handover", "ready", "mine" ];

var g_queue = Promise.resolve( );

var g_people = [ ];
var g_armed = "";

var g_handover_code = "";
var g_ready = null;

var g_join_mode = "code";
var g_join_pin = "";
var g_join_code = "";

var g_alert_timer = null;

// NOTE: Sharing a session with the chat and the console, over the channel the chat and Ian's
// harness already speak - see "chat.html" and "parse_channel_message( )" in "console_parse.js":
//
//   "<owner>:<viewer>"   a viewer asks the page that opened it for its session
//   "<viewer>-<owner>"   the owner names itself
//   "<viewer>=<fields>"  the owner hands over its session
//   "<id>"               that page's session has ended - its viewers let go
//
// This page is a viewer when the chat opens it ("?source=<chat>"), and an owner for the chat or
// console it opens. Ids are fresh for each load, never from "sessionStorage", which a page and
// its iframe share.
const c_channel_name = "test_web_channel";
const c_log_channel_name = "ciyam_console_log";

const c_announce_interval = 500;
const c_max_announces = 10;

var g_self = String( Date.now( ) );
var g_source = "";
var g_owner = "";

var g_channel = null;
var g_log_channel = null;

var g_announces = 0;
var g_announce_timer = null;

// NOTE: The pages this one has handed its session to - a chat it opened signs the session out
// for everyone when it signs out, and says so with its id.
var g_viewers = { };

var g_console_loaded = false;

var g_request_log = [ ];
var g_request_log_id = 0;
var g_request_quiet = false;

// ====================================================================
// Entry point
// ====================================================================

function account_main( )
{
   // NOTE: The accounts page's tab, which the chat reuses rather than opening another.
   window.name = c_tab_accounts;

   try
   {
      if( localStorage.getItem( c_storage_device ) !== null )
         ciyam.device = localStorage.getItem( c_storage_device );
   }
   catch( e )
   {
   }

   fill_saved_pins( );

   // NOTE: Every request is logged for a console, as the chat's are.
   install_log_capture( ciyam, "account", function( ) { return g_request_quiet; }, record_request );

   g_channel = new BroadcastChannel( c_channel_name );
   g_channel.addEventListener( "message", on_channel_message );

   g_log_channel = new BroadcastChannel( c_log_channel_name );
   g_log_channel.addEventListener( "message", on_log_message );

   var source = new URL( window.location.href ).searchParams.get( "source" );

   if( ( source !== null ) && /^[0-9]+$/.test( source ) )
   {
      g_source = source;

      g_announce_timer = window.setInterval( announce, c_announce_interval );

      announce( );
   }

   window.addEventListener( "hashchange", route );

   route( );
}

// ====================================================================
// A shared session
// ====================================================================

function announce( )
{
   if( ciyam.sessid !== "" )
      return;

   if( ++g_announces > c_max_announces )
   {
      stop_announcing( );

      forget_source( );

      set_error( "linking_text", "The chat didn't share its session - is it still signed in?" );

      document.getElementById( "linking_actions" ).hidden = false;

      return;
   }

   g_channel.postMessage( g_source + ":" + g_self );
}

function stop_announcing( )
{
   if( g_announce_timer !== null )
   {
      window.clearInterval( g_announce_timer );

      g_announce_timer = null;
   }
}

function is_waiting_for_link( )
{
   return ( g_source !== "" ) && ( ciyam.sessid === "" ) && ( g_announce_timer !== null );
}

function do_sign_in_instead( )
{
   stop_announcing( );

   forget_source( );

   g_source = "";

   route( );
}

function on_channel_message( event )
{
   var message = parse_channel_message( event.data );

   if( message === null )
      return;

   if( ( message.kind === "announce" ) && ( message.id === g_self ) && ( ciyam.sessid !== "" ) )
   {
      g_viewers[ message.rest ] = true;

      g_channel.postMessage( message.rest + "-" + g_self );

      g_channel.postMessage( message.rest + "=" + ciyam.access + "," + ciyam.device + "," + ciyam.hashed + ","
       + ciyam.sessid + "," + ciyam.unique + "," + encode_channel_field( ciyam.username ) + "," + ( ciyam.is_admin ? "1" : "0" ) );
   }
   else if( ( message.kind === "owner" ) && ( message.id === g_self ) )
      g_owner = message.rest;
   else if( ( message.kind === "credentials" ) && ( message.id === g_self ) && is_waiting_for_link( ) )
   {
      var credentials = parse_credentials( message.rest );

      if( credentials !== null )
         adopt_session( credentials );
   }
   else if( ( message.kind === "ended" ) && ( g_source !== "" ) && ( ciyam.sessid !== "" )
    && ( ( message.id === g_source ) || ( message.id === g_owner ) ) )
      end_linked_session( );
   else if( ( message.kind === "ended" ) && ( ciyam.sessid !== "" ) && g_viewers[ message.id ] )
      end_linked_session( );
}

function adopt_session( credentials )
{
   stop_announcing( );

   ciyam.access = credentials.access;
   ciyam.device = credentials.device;
   ciyam.hashed = credentials.hashed;
   ciyam.sessid = credentials.sessid;
   ciyam.unique = credentials.unique;
   ciyam.username = credentials.username;
   ciyam.is_admin = credentials.is_admin;

   enter_app( );
}

// NOTE: The chat signed out, so the session this page borrowed is gone. Nothing of it is kept.
function end_linked_session( )
{
   clear_session( );

   forget_source( );

   g_source = "";
   g_owner = "";

   route( );

   set_error( "signin_error", "The chat signed out, so this page did too." );
}

// NOTE: Requests and the log for a console linked to this page - the drawer's, or one in its
// own window. It hands its requests here, so this page's queue keeps them one at a time.
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

function record_request( entry )
{
   entry.id = ++g_request_log_id;

   g_request_log.push( entry );

   if( g_request_log.length > c_console_log_capacity )
      g_request_log.shift( );

   g_log_channel.postMessage( { kind: "entry", owner: g_self, entry: entry } );
}

function do_toggle_console( )
{
   var drawer = document.getElementById( "console_drawer" );

   drawer.hidden = !drawer.hidden;

   if( !drawer.hidden && !g_console_loaded )
   {
      g_console_loaded = true;

      document.getElementById( "console_frame" ).src = "console.html?embedded=1&source=" + encodeURIComponent( g_self );
   }
}

function do_popout_console( )
{
   open_app_tab( c_tab_console, "console.html?source=" + encodeURIComponent( g_self ), ciyam.sessid );

   if( !document.getElementById( "console_drawer" ).hidden )
      do_toggle_console( );
}

// NOTE: The chat's tab, reused - switched to when it is on this session, as it is when this page
// was opened from it; otherwise the chat opens sharing this session. This page stays open, so the
// chat's Account settings comes back to it without reloading.
function do_open_chat_linked( )
{
   open_app_tab( c_tab_chat, "chat.html?source=" + encodeURIComponent( g_self ), ciyam.sessid );
}

// NOTE: A console or chat opened from this session must not carry on under the next.
function unlink_others( )
{
   g_channel.postMessage( g_self );

   g_console_loaded = false;

   document.getElementById( "console_drawer" ).hidden = true;
   document.getElementById( "console_frame" ).src = "about:blank";

   g_request_log = [ ];
}

// NOTE: One request at a time - "ciyam.js" keeps a single callback per instance, so a second
// request in flight would take the first one's answer (ISS-005). Resolves once the call has
// finished - "connect( )" makes several requests - with the last answer its callback was given.
// A request lost on the way is only logged by "ciyam.fetch", never answered, so no answer at all
// is an error - not an empty answer, which would read as success.
const c_no_answer = "Error: The server did not answer - check the connection and try again.";

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

// NOTE: "POST /cws/devices" with the given query - how a code is used and a PIN looked at or
// claimed. These are the requests "CIYAM.connect" would issue, made directly because it cannot
// use a code: "register_account( )" in "chat.js" does the same (ISS-008).
function post_devices( query )
{
   return request( function( done )
   {
      return ciyam.fetch( ciyam.get_cws_url( ) + "/devices?" + query + "&format=" + ciyam.format_type, "POST", done );
   } );
}

function problem_text( response )
{
   if( is_timeout_response( response ) )
      return "The server didn't answer in time - try again.";

   if( is_error_response( response ) )
      return error_text( response );

   return "";
}

// ====================================================================
// Which view - from the address
// ====================================================================

function route( )
{
   var asked = parse_account_hash( window.location.hash );

   if( ciyam.sessid === "" )
   {
      if( is_waiting_for_link( ) )
         show_view( "linking_view" );
      else if( asked.view === "welcome" )
         open_welcome( asked.code );
      else
         show_view( "signin_view" );

      return;
   }

   var name = window.location.hash.replace( /^#/, "" );

   if( !ciyam.is_admin || ( c_admin_views.indexOf( name ) < 0 ) )
      name = ciyam.is_admin ? "people" : "mine";

   // NOTE: A code is shown once, and a new PIN only straight after it is made - going back to
   // either finds People instead.
   if( ( ( name === "handover" ) && ( g_handover_code === "" ) ) || ( ( name === "ready" ) && ( g_ready === null ) ) )
      name = "people";

   if( name !== "handover" )
      forget_handover( );

   if( name !== "ready" )
      g_ready = null;

   show_section( name );
}

function show_view( id )
{
   [ "linking_view", "signin_view", "welcome_view", "written_view", "app_view" ].forEach( function( view )
   {
      document.getElementById( view ).hidden = ( view !== id );
   } );
}

function show_section( name )
{
   show_view( "app_view" );

   c_admin_views.forEach( function( section )
   {
      document.getElementById( section + "_section" ).hidden = ( section !== name );
   } );

   // NOTE: A notice lines up with the section under it - People is the wide one.
   document.querySelector( ".account-main" ).classList.toggle( "is-wide", name === "people" );

   var current = ( name === "mine" ) ? "nav_mine" : "nav_people";

   [ "nav_people", "nav_mine" ].forEach( function( id )
   {
      var link = document.getElementById( id );

      if( id === current )
         link.setAttribute( "aria-current", "page" );
      else
         link.removeAttribute( "aria-current" );
   } );

   if( name === "people" )
      load_people( );
   else if( name === "add" )
      reset_add_form( );
   else if( name === "mine" )
      render_mine( );

   var heading = document.querySelector( "#" + name + "_section h1" );

   if( heading !== null )
      heading.focus( { preventScroll: true } );
}

function go( name )
{
   if( window.location.hash === "#" + name )
      route( );
   else
      window.location.hash = name;
}

// NOTE: Drops the address's "#..." without a new history entry - so a used code does not sit
// in the address bar or the back button.
function clear_hash( )
{
   history.replaceState( null, "", window.location.pathname + window.location.search );
}

function set_error( id, text )
{
   document.getElementById( id ).textContent = text;
}

function show_alert( text, is_error )
{
   var alert = document.getElementById( "account_alert" );

   alert.textContent = text;
   alert.classList.toggle( "is-error", !!is_error );
   alert.hidden = false;

   if( g_alert_timer !== null )
      window.clearTimeout( g_alert_timer );

   g_alert_timer = window.setTimeout( function( ) { alert.hidden = true; }, c_alert_time );
}

// NOTE: The strength bar under a new password - "join" or "mine".
function update_strength( prefix )
{
   var strength = password_strength( document.getElementById( prefix + "_password" ).value );

   var box = document.getElementById( prefix + "_strength" );

   box.hidden = ( strength.level < 0 );
   box.dataset.level = String( strength.level );

   document.getElementById( prefix + "_strength_label" ).textContent = strength.text;
}

// ====================================================================
// Sign in and out
// ====================================================================

function saved_pins( )
{
   try
   {
      return parse_access_list( localStorage.getItem( c_storage_access ) ).filter( is_account_pin );
   }
   catch( e )
   {
      return [ ];
   }
}

// NOTE: Read each time it is needed rather than once - the chat may have replaced it since this
// page loaded ("Reset this browser").
function stored_device( )
{
   try
   {
      return localStorage.getItem( c_storage_device ) || "";
   }
   catch( e )
   {
      return "";
   }
}

function saved_hash( pin )
{
   try
   {
      return localStorage.getItem( c_storage_hashed_prefix + pin );
   }
   catch( e )
   {
      return null;
   }
}

// NOTE: The accounts saved on this browser - by this page or the chat, which share them - ahead
// of "Enter a PIN...", the first chosen when there is one, as the chat does.
function fill_saved_pins( )
{
   var select = document.getElementById( "signin_access" );

   for( var i = select.options.length - 1; i >= 0; i-- )
   {
      if( select.options[ i ].value !== "" )
         select.remove( i );
   }

   saved_pins( ).forEach( function( pin, n )
   {
      var label = pin + ( ( saved_hash( pin ) !== null ) ? "  \u00b7  saved password" : "" );

      select.options.add( new Option( label, pin, false ), n );
   } );

   select.selectedIndex = 0;

   on_signin_access( );
}

// NOTE: As the chat's - "do_reset_browser( )" in "chat.js". Everything this browser keeps for the
// node goes: the device token, which a rebuilt node no longer knows, and every saved account.
function do_reset_browser( )
{
   if( ciyam.sessid !== "" )
      return;

   if( !confirm( "Forget the device token and every saved account on this browser?" ) )
      return;

   try
   {
      localStorage.clear( );
   }
   catch( e )
   {
   }

   location.reload( );
}

function signin_pin( )
{
   var chosen = document.getElementById( "signin_access" ).value;

   return ( chosen !== "" ) ? chosen : document.getElementById( "signin_pin" ).value.trim( );
}

// NOTE: The PIN field only for a PIN not saved here; the Remember box shows what is saved for the
// account now, so signing in never silently forgets it. A saved password can be used by leaving
// the field empty - it stays open, so a password that no longer works can be typed instead.
function on_signin_access( )
{
   var typed = ( document.getElementById( "signin_access" ).value === "" );

   document.getElementById( "signin_pin_label" ).hidden = !typed;
   document.getElementById( "signin_pin" ).hidden = !typed;

   var pin = signin_pin( );

   var has_hash = is_account_pin( pin ) && ( saved_hash( pin ) !== null );

   set_error( "signin_hint", has_hash ? "The password is saved on this browser - leave it empty to use it." : "" );

   var stored = null;

   try
   {
      stored = localStorage.getItem( c_storage_access );
   }
   catch( e )
   {
   }

   document.getElementById( "signin_retain" ).value = retain_mode_of( stored, is_account_pin( pin ) ? pin : "", has_hash );
}

// NOTE: After a sign in, what the Remember box asked for - "plan_retain_choice( )" in
// "chat_parse.js", the same as the chat's.
function apply_retain_choice( )
{
   try
   {
      var plan = plan_retain_choice( localStorage.getItem( c_storage_access ), ciyam.access,
       document.getElementById( "signin_retain" ).value, ciyam.hashed );

      if( plan.keep_hash )
         localStorage.setItem( c_storage_hashed_prefix + ciyam.access, ciyam.hashed );
      else
         localStorage.removeItem( c_storage_hashed_prefix + ciyam.access );

      if( plan.list === null )
         localStorage.removeItem( c_storage_access );
      else
         localStorage.setItem( c_storage_access, plan.list );
   }
   catch( e )
   {
   }
}

async function do_sign_in( event )
{
   event.preventDefault( );

   var pin = signin_pin( );
   var password = document.getElementById( "signin_password" ).value;

   if( !is_account_pin( pin ) )
   {
      set_error( "signin_error", "Enter your PIN - 5 digits." );

      return;
   }

   var hashed = ( password === "" ) ? saved_hash( pin ) : "";

   if( ( password === "" ) && ( hashed === null ) )
   {
      set_error( "signin_error", "Enter your password." );

      return;
   }

   set_error( "signin_error", "" );

   var submit = document.getElementById( "signin_submit" );

   submit.disabled = true;

   ciyam.device = stored_device( );

   await connect_as( pin, hashed, password );

   // NOTE: The node has never seen this browser's device token - it was issued before the node
   // was rebuilt, or by another node at this address. A typed password does not depend on it,
   // so the server is asked for a new one and the sign in tried again. A saved password cannot
   // be used: it was hashed with the old token. The chat asks for "Reset this browser" instead.
   if( is_unknown_device_error( ciyam.error ) && ( password !== "" ) )
   {
      ciyam.device = "";

      await connect_as( pin, "", password );
   }

   submit.disabled = false;

   if( ciyam.error === "" )
      remember_device( );

   if( ciyam.error !== "" )
   {
      set_error( "signin_error", is_unknown_device_error( ciyam.error )
       ? "This browser's saved sign in is from before the node was set up again - type your password."
       : sign_in_error_text( ciyam.error ) );

      return;
   }

   if( ciyam.sessid === "" )
   {
      set_error( "signin_error", "No session was established." );

      return;
   }

   document.getElementById( "signin_password" ).value = "";
   document.getElementById( "signin_pin" ).value = "";

   apply_retain_choice( );

   enter_app( );
}

async function connect_as( pin, hashed, password )
{
   ciyam.error = "";
   ciyam.unique = "";

   await request( function( done )
   {
      return ciyam.connect( pin, ciyam.device, hashed, password, done );
   } );
}

function remember_device( )
{
   try
   {
      if( ciyam.device !== "" )
         localStorage.setItem( c_storage_device, ciyam.device );
   }
   catch( e )
   {
   }
}

function enter_app( )
{
   var linked = ( g_source !== "" );

   document.getElementById( "nav_people" ).hidden = !ciyam.is_admin;
   document.getElementById( "topbar_user" ).textContent = ( ciyam.username || ciyam.access ) + ( linked ? " - through the chat" : "" );
   document.getElementById( "console_session" ).textContent = "inherits session " + ciyam.sessid;

   // NOTE: The session is the chat's - signing out belongs there, and "Chat" goes back to it.
   document.getElementById( "sign_out_button" ).hidden = linked;
   document.getElementById( "chat_button" ).textContent = linked ? "Back to the chat" : "Chat";

   var wanted = window.location.hash.replace( /^#/, "" );

   go( ( ciyam.is_admin && ( c_admin_views.indexOf( wanted ) >= 0 ) ) ? wanted : ( ciyam.is_admin ? "people" : "mine" ) );
}

async function do_sign_out( )
{
   await request( function( done )
   {
      return ciyam.disconnect( done );
   } );

   clear_session( );

   show_view( "signin_view" );
}

// NOTE: Signed out here whatever the server answered - a session left half open would take the
// next change of address back into the page. A console or chat sharing it lets go.
function clear_session( )
{
   unlink_others( );

   g_viewers = { };

   ciyam.sessid = "";
   ciyam.access = "";
   ciyam.hashed = "";
   ciyam.unique = "";
   ciyam.username = "";
   ciyam.is_admin = false;

   g_people = [ ];
   g_armed = "";
   g_ready = null;

   forget_handover( );

   clear_hash( );

   fill_saved_pins( );
}

// ====================================================================
// People
// ====================================================================

async function load_people( )
{
   var banner_hidden = false;

   try
   {
      banner_hidden = ( localStorage.getItem( c_storage_banner ) !== null );
   }
   catch( e )
   {
   }

   document.getElementById( "people_banner" ).hidden = banner_hidden;

   var response = await request( function( done )
   {
      return ciyam.fetch_users( done );
   } );

   if( problem_text( response ) !== "" )
   {
      g_people = [ ];

      document.getElementById( "people_rows" ).replaceChildren( );

      set_error( "people_empty", "The list could not be read: " + problem_text( response ) );

      return;
   }

   g_people = parse_people( response, ciyam.access, ciyam.username || "admin" );

   render_people( );
}

function render_people( )
{
   var body = document.getElementById( "people_rows" );

   body.replaceChildren( );

   set_error( "people_empty", ( g_people.length === 0 ) ? "Nobody yet." : "" );

   g_people.forEach( function( person )
   {
      var row = document.getElementById( "tpl_person" ).content.firstElementChild.cloneNode( true );

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

      row.querySelector( ".account-pin" ).textContent = person.pin;

      var pill = row.querySelector( ".account-pill" );

      pill.textContent = { admin: "Administrator", active: "Active", unclaimed: "Not yet claimed" }[ person.status ];
      pill.classList.add( "account-pill--" + person.status );

      var remove = row.querySelector( ".account-remove" );
      var confirm = row.querySelector( ".account-confirm" );

      if( person.you )
      {
         row.querySelector( ".account-you" ).hidden = false;

         remove.hidden = true;
      }
      else if( g_armed === person.pin )
      {
         remove.hidden = true;
         confirm.hidden = false;

         row.querySelector( ".account-confirm-text" ).textContent = "Remove " + ( person.name || "PIN " + person.pin )
          + "? They can no longer sign in.";

         row.querySelector( ".account-confirm-yes" ).onclick = function( ) { do_remove( person.pin ); };
         row.querySelector( ".account-confirm-no" ).onclick = function( ) { arm_remove( "" ); };
      }
      else
      {
         remove.setAttribute( "aria-label", "Remove " + ( person.name || "PIN " + person.pin ) );
         remove.onclick = function( ) { arm_remove( person.pin ); };
      }

      body.appendChild( row );
   } );

   if( g_armed !== "" )
   {
      var keep = body.querySelector( ".account-confirm:not([hidden]) .account-confirm-no" );

      if( keep !== null )
         keep.focus( );
   }
}

// NOTE: Removing asks once more in the row itself, as the chat's "Block for good" does.
function arm_remove( pin )
{
   g_armed = pin;

   render_people( );
}

async function do_remove( pin )
{
   var person = g_people.filter( function( row ) { return row.pin === pin; } )[ 0 ];

   var response = await request( function( done )
   {
      return ciyam.delete_user( pin, done );
   } );

   g_armed = "";

   if( problem_text( response ) !== "" )
      show_alert( "Not removed: " + problem_text( response ), true );
   else
      show_alert( ( person && person.name ? person.name : "PIN " + pin ) + " was removed." );

   load_people( );
}

function do_hide_banner( )
{
   try
   {
      localStorage.setItem( c_storage_banner, "1" );
   }
   catch( e )
   {
   }

   document.getElementById( "people_banner" ).hidden = true;
}

// ====================================================================
// Add a person
// ====================================================================

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
   var response = await request( function( done ) { return ciyam.fetch_users( done ); } );

   if( problem_text( response ) === "" )
      g_people = parse_people( response, ciyam.access, ciyam.username || "admin" );
}

function update_add_form( )
{
   var by_pin = ( add_how( ) === "pin" );

   document.getElementById( "add_pin_fields" ).hidden = !by_pin;
   document.getElementById( "add_submit" ).textContent = by_pin ? "Create the PIN" : "Create a code";

   set_error( "add_error", "" );
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
         set_error( "add_error", problem );

         return;
      }
   }

   set_error( "add_error", "" );

   var submit = document.getElementById( "add_submit" );

   submit.disabled = true;

   var options = by_pin ? nominated_options( pin, username, suggestion ) : "secret";

   var response = await request( function( done )
   {
      return ciyam.create_user( options, done );
   } );

   submit.disabled = false;

   if( problem_text( response ) !== "" )
   {
      set_error( "add_error", problem_text( response ) );

      return;
   }

   var answer = response.trim( );

   if( by_pin )
   {
      if( !is_account_pin( answer ) )
      {
         set_error( "add_error", "The server's answer was not a PIN: " + answer );

         return;
      }

      g_ready = { pin: answer, username: username, suggestion: suggestion };

      render_ready( );

      go( "ready" );
   }
   else
   {
      if( normalise_code( answer ) === "" )
      {
         set_error( "add_error", "The server's answer was not a code: " + answer );

         return;
      }

      g_handover_code = answer;

      render_handover( );

      go( "handover" );
   }
}

// ====================================================================
// Hand over the code, and the PIN is ready
// ====================================================================

function page_address( )
{
   return window.location.protocol + "//" + window.location.host + "/" + c_account_page;
}

function render_handover( )
{
   document.getElementById( "handover_code" ).textContent = g_handover_code;
   document.getElementById( "handover_url" ).textContent = page_address( );
   document.getElementById( "handover_copy" ).textContent = "Copy";

   var box = document.getElementById( "handover_qr" );

   box.replaceChildren( );

   // NOTE: Drawn in the page - "qrcode.min.js" - so the code goes nowhere else.
   new QRCode( box, {
      text: join_url( window.location.protocol + "//" + window.location.host, g_handover_code ),
      width: c_qr_size,
      height: c_qr_size,
      colorDark: "#1b1f2a",
      colorLight: "#ffffff",
      correctLevel: QRCode.CorrectLevel.M
   } );

   // NOTE: The library gives its image a "title" of the address, code and all - a tooltip
   // anyone looking over admin's shoulder could read.
   box.removeAttribute( "title" );
   box.querySelectorAll( "[title]" ).forEach( function( node ) { node.removeAttribute( "title" ); } );
}

function forget_handover( )
{
   g_handover_code = "";

   document.getElementById( "handover_code" ).textContent = "";
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
      // NOTE: The clipboard needs HTTPS or "localhost"; elsewhere the code is selected instead,
      // ready to copy by hand.
      var range = document.createRange( );

      range.selectNodeContents( document.getElementById( "handover_code" ) );

      window.getSelection( ).removeAllRanges( );
      window.getSelection( ).addRange( range );

      button.textContent = "Selected - copy it";
   }

   window.setTimeout( function( ) { button.textContent = "Copy"; }, c_copied_time );
}

function render_ready( )
{
   document.getElementById( "ready_pin" ).textContent = g_ready.pin;
   document.getElementById( "ready_url" ).textContent = page_address( );

   var name = document.getElementById( "ready_username" );
   var note = document.getElementById( "ready_username_note" );

   if( g_ready.username === "" )
   {
      name.textContent = "Their choice";
      note.textContent = "They choose one when they set their password";
   }
   else
   {
      name.textContent = g_ready.username;
      note.textContent = g_ready.suggestion ? "A suggestion - they can change it" : "Chosen by you";
   }
}

// ====================================================================
// My account
// ====================================================================

function render_mine( )
{
   var name = ciyam.username || "";

   var avatar = document.getElementById( "mine_avatar" );

   avatar.textContent = user_initial( name || ciyam.access );
   avatar.style.background = "var(--color-sender-" + sender_colour_index( name || ciyam.access ) + ")";

   document.getElementById( "mine_name" ).textContent = name || "No username";
   document.getElementById( "mine_pin" ).textContent = ciyam.access;
   document.getElementById( "mine_type" ).textContent = ciyam.is_admin ? "Administrator" : "Standard";

   // NOTE: For a password manager - which account the password belongs to.
   document.getElementById( "mine_username" ).value = name || ciyam.access;

   // NOTE: admin's password is the node's master password, which also unlocks the node after a
   // restart - not something to change from a web page (an open question for Ian).
   document.getElementById( "mine_password_form" ).hidden = ciyam.is_admin;
   document.getElementById( "mine_admin_note" ).hidden = !ciyam.is_admin;

   [ "mine_current", "mine_password", "mine_confirm" ].forEach( function( id )
   {
      document.getElementById( id ).value = "";
   } );

   update_strength( "mine" );

   set_error( "mine_error", "" );
}

// NOTE: The hash this session was opened with, for a given password - "determine_hashed( )"
// in "ciyam.js", worked out without touching the session's own - as "chat.js" does.
function session_hash_for( password )
{
   return hex_sha256( hex_sha256( ciyam.hash_combined( password ) ) + ciyam.device );
}

async function do_change_password( event )
{
   event.preventDefault( );

   var current = document.getElementById( "mine_current" ).value;
   var fresh = document.getElementById( "mine_password" ).value;
   var confirm = document.getElementById( "mine_confirm" ).value;

   // NOTE: The server only checks the session, so the current password is checked here,
   // against the hash the session was opened with - no request, and nothing sent.
   if( session_hash_for( current ) !== ciyam.hashed )
   {
      set_error( "mine_error", "The current password is not right." );

      return;
   }

   if( password_strength( fresh ).level < 1 )
   {
      set_error( "mine_error", "Choose a password of at least 7 characters." );

      return;
   }

   if( fresh !== confirm )
   {
      set_error( "mine_error", "The two new passwords are not the same." );

      return;
   }

   if( fresh === current )
   {
      set_error( "mine_error", "The new password is the same as the current one." );

      return;
   }

   set_error( "mine_error", "" );

   var submit = document.getElementById( "mine_submit" );

   submit.disabled = true;

   // NOTE: The account's own PIN - "***" is not substituted by "update_user( )" and the server
   // refuses it. The password is hashed with the PIN before it is sent.
   var response = await request( function( done )
   {
      return ciyam.update_user( ciyam.access, "password=" + fresh, done );
   } );

   submit.disabled = false;

   if( problem_text( response ) !== "" )
   {
      set_error( "mine_error", problem_text( response ) );

      return;
   }

   // NOTE: The session stays open. A hash the chat saved for this account was made from the old
   // password and is now refused, so it is replaced.
   var hashed = session_hash_for( fresh );

   ciyam.hashed = hashed;

   try
   {
      if( localStorage.getItem( c_storage_hashed_prefix + ciyam.access ) !== null )
         localStorage.setItem( c_storage_hashed_prefix + ciyam.access, hashed );
   }
   catch( e )
   {
   }

   render_mine( );

   show_alert( "Password changed." );
}

// ====================================================================
// Welcome - a code, or a PIN
// ====================================================================

function open_welcome( code )
{
   show_view( "welcome_view" );

   set_join_mode( "code" );

   if( code !== "" )
   {
      document.getElementById( "join_code" ).value = code;

      document.getElementById( "join_username" ).focus( );
   }
   else
      document.querySelector( "#welcome_view h2" ).focus( { preventScroll: true } );
}

function set_join_mode( mode )
{
   g_join_mode = mode;

   g_join_pin = "";
   g_join_code = "";

   var by_code = ( mode === "code" );

   document.getElementById( "join_tab_code" ).setAttribute( "aria-selected", by_code ? "true" : "false" );
   document.getElementById( "join_tab_pin" ).setAttribute( "aria-selected", by_code ? "false" : "true" );

   document.getElementById( "join_code_row" ).hidden = !by_code;
   document.getElementById( "join_pin_row" ).hidden = by_code;

   // NOTE: With a PIN, the username and password come once the PIN is known - the server says
   // then whether admin chose a username.
   document.getElementById( "join_details" ).hidden = !by_code;
   document.getElementById( "join_submit" ).hidden = !by_code;
   document.getElementById( "join_submit" ).textContent = by_code ? "Create my account" : "Set my password";

   set_username_field( "", false, false );

   set_error( "join_error", "" );
}

function set_username_field( value, fixed, suggested )
{
   var field = document.getElementById( "join_username" );

   field.value = value;
   field.readOnly = fixed;

   document.querySelector( "label[for='join_username']" ).textContent = fixed ? "Your username" : "Choose a username";

   set_error( "join_username_hint", fixed ? "Chosen for you by the person who added you."
    : ( suggested ? "Suggested for you - keep it or choose another." : "3 to 12 characters: a-z, 0-9 and -. Others see it." ) );
}

// NOTE: A different code from the one already used here is a fresh start.
function on_join_code_input( )
{
   if( normalise_code( document.getElementById( "join_code" ).value ) !== g_join_code )
      g_join_pin = "";
}

function on_join_pin_input( )
{
   if( document.getElementById( "join_pin" ).value.trim( ) !== g_join_pin )
   {
      g_join_pin = "";

      document.getElementById( "join_details" ).hidden = true;
      document.getElementById( "join_submit" ).hidden = true;
   }
}

// NOTE: What the server says about a PIN: whether it is waiting for a password, and whether
// admin chose its username. Asking registers this browser as one of the PIN's devices, which
// signing in would do anyway.
async function do_check_pin( )
{
   var pin = document.getElementById( "join_pin" ).value.trim( );

   if( !is_account_pin( pin ) )
   {
      set_error( "join_error", "A PIN is 5 digits." );

      return;
   }

   set_error( "join_error", "" );

   var button = document.getElementById( "join_pin_check" );

   button.disabled = true;

   var response = await post_devices( "access=" + pin );

   button.disabled = false;

   if( is_timeout_response( response ) )
   {
      set_error( "join_error", problem_text( response ) );

      return;
   }

   var reply = parse_join_reply( response );

   if( reply.kind === "error" )
   {
      set_error( "join_error", refused_join_text( reply.error, "pin" ) );

      return;
   }

   if( reply.kind === "claimed" )
   {
      set_error( "join_error", "PIN " + pin + " already has a password - sign in instead." );

      return;
   }

   g_join_pin = reply.pin;

   set_username_field( reply.username, reply.fixed, ( reply.username !== "" ) && !reply.fixed );

   document.getElementById( "join_details" ).hidden = false;
   document.getElementById( "join_submit" ).hidden = false;

   document.getElementById( reply.fixed ? "join_password" : "join_username" ).focus( );
}

async function do_join( event )
{
   event.preventDefault( );

   if( ( g_join_mode === "pin" ) && ( g_join_pin === "" ) )
   {
      do_check_pin( );

      return;
   }

   var code = normalise_code( document.getElementById( "join_code" ).value );

   if( ( g_join_mode === "code" ) && ( code === "" ) )
   {
      set_error( "join_error", "A code is 10 letters, like abc-defg-hij." );

      return;
   }

   var username = document.getElementById( "join_username" ).value.trim( );
   var password = document.getElementById( "join_password" ).value;
   var confirm = document.getElementById( "join_confirm" ).value;

   var problem = join_problem( username, password, confirm );

   if( problem !== "" )
   {
      set_error( "join_error", problem );

      return;
   }

   set_error( "join_error", "" );

   var submit = document.getElementById( "join_submit" );

   submit.disabled = true;

   try
   {
      // NOTE: A code makes the account's PIN as soon as it is used, so the PIN is kept: if the
      // username is then refused, another is tried on the same PIN without using the code again.
      if( ( g_join_mode === "code" ) && ( g_join_pin === "" ) )
      {
         var made = await post_devices( "access=" + encodeURIComponent( code ) );

         var reply = parse_join_reply( made );

         if( reply.kind !== "open" )
         {
            var refused = is_timeout_response( made ) ? problem_text( made ) : "";

            if( ( refused === "" ) && ( reply.kind === "error" ) )
               refused = refused_join_text( reply.error, "code" );

            set_error( "join_error", refused || "That code wasn't accepted." );

            return;
         }

         g_join_pin = reply.pin;
         g_join_code = code;
      }

      var credentials = CIYAM.encode_base64_url( username + ":" + ciyam.hash_combined( password, g_join_pin ) );

      var token = await post_devices( "access=" + g_join_pin + "&passwd=" + credentials );

      if( problem_text( token ) !== "" )
      {
         set_error( "join_error", /already been (used|taken)/i.test( token )
          ? "Someone already has the username " + username + " - choose another." : problem_text( token ) );

         return;
      }

      // NOTE: The browser keeps the device it already has - a password the chat saved is hashed
      // with it, as "register_account( )" in "chat.js" explains. A new one only when there is none.
      if( ( ciyam.device === "" ) && ( token.trim( ) !== "" ) )
      {
         ciyam.device = token.trim( );

         remember_device( );
      }

      remember_pin( g_join_pin );

      show_written( g_join_pin, username );
   }
   finally
   {
      submit.disabled = false;
   }
}

// NOTE: The chat lists the accounts this browser has used, so the new PIN is added - the PIN
// only, never the password.
function remember_pin( pin )
{
   try
   {
      var entries = parse_access_list( localStorage.getItem( c_storage_access ) );

      if( entries.indexOf( pin ) < 0 )
      {
         entries.push( pin );

         localStorage.setItem( c_storage_access, format_access_list( entries ) );
      }
   }
   catch( e )
   {
   }
}

function show_written( pin, username )
{
   [ "join_code", "join_pin", "join_username", "join_password", "join_confirm" ].forEach( function( id )
   {
      document.getElementById( id ).value = "";
   } );

   g_join_pin = "";
   g_join_code = "";

   update_strength( "join" );

   clear_hash( );

   document.getElementById( "written_pin" ).textContent = pin;
   document.getElementById( "written_username" ).textContent = username;
   document.getElementById( "written_check" ).checked = false;

   update_written( );

   show_view( "written_view" );

   document.querySelector( "#written_view h2" ).focus( { preventScroll: true } );
}

function update_written( )
{
   document.getElementById( "written_open" ).disabled = !document.getElementById( "written_check" ).checked;
}

function do_open_chat( )
{
   window.location.href = "chat.html";
}

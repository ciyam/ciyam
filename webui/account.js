// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The accounts page - everyone's own account, My account, plus the Welcome screen a new person uses before
// they have one. Admin's People (adding, handing over, removing) moved into Home on 2026-10-11; an old address for it
// goes there. Only what the server supports today; see the vault's "Account Management Design". Its pure logic is in
// "account_parse.js", with tests.

// NOTE: The same keys as the chat, so the two share this browser's device and saved accounts.
const c_storage_device = "cws.device";
const c_storage_access = "cws.access";
const c_storage_hashed_prefix = "cws.hashed_";

const c_alert_time = 6000;

// NOTE: People's views, when it was here - an address for one now opens People in Home.
const c_people_views = [ "people", "add", "handover", "ready" ];

var g_queue = Promise.resolve( );

// NOTE: This account's devices on My account, and the one whose Remove has been clicked once.
var g_devices = [ ];
var g_device_armed = "";

// NOTE: The session My account was last drawn for - a section chosen after that only shows, so what is being typed in another
// stays (2026-10-26).
var g_shown_for = "";

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
   // NOTE: Another browser's QR code for your keys ("account_keys.js") - out of the address before anything reads it.
   if( typeof keys_take_link === "function" )
      keys_take_link( );

   // NOTE: The eye in each new password's field, to see it - one suggested above all (2026-10-25).
   attach_reveals( document );

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

   // NOTE: The node's sign in, shared by every app ("signin.js", 2026-10-08) - this page's was its first form.
   signin_build( document.getElementById( "signin_host" ), {
      note: "accounts",
      setup_href: "#welcome",
      request: request,
      on_signed_in: enter_app
   } );

   signin_fill_saved( );

   // NOTE: The app switcher - the same in every app, its button the CIYAM mark and this page's name (2026-10-08).
   apps_build( document.getElementById( "apps_host" ), {
      current: "account",
      self: function( ) { return g_self; },
      sign_out: do_sign_out,
      signed_out: function( )
      {
         clear_session( );

         // NOTE: As when the page that opened this one signs out - a reload must not try to link to it again.
         forget_source( );

         g_source = "";
         g_owner = "";

         route( );

         set_error( "signin_error", "You signed out in another tab." );
      },
      from: "accounts",
      unsent: unsent_work
   } );

   // NOTE: Every request is logged for a console, as the chat's are - the quiet ones only while the
   // console's "Log polling" is ticked.
   install_log_capture( ciyam, "account", function( ) { return g_request_quiet && !is_logging_polling( ); }, record_request );

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

   // NOTE: The shell - the side menu's button, its scrim and Escape ("shell.js"); Sign out, as in Home, of every app.
   shell_install( );

   document.getElementById( "app_sign_out" ).addEventListener( "click", apps_sign_out_everywhere );

   window.addEventListener( "hashchange", function( )
   {
      // NOTE: A QR code's address opened in a tab already on this page changes only the part after "#" - taken up the same,
      // then Your keys drawn again to take it.
      var linked = ( typeof keys_take_link === "function" ) && keys_take_link( );

      route( );

      if( linked && ( ciyam.sessid !== "" ) )
         render_keys( );
   } );

   // NOTE: Switched to from another app in this tab, or reloaded - the session kept for the tab, if the node still
   // knows it (2026-10-09). Nothing is shown until it is known, so the sign in does not flash past.
   // NOTE: Not for a Welcome link - it is for someone new, whoever this tab was signed in as (found by review).
   if( ( g_source === "" ) && ( signin_kept_session( ) !== null ) && ( parse_account_hash( window.location.hash ).view !== "welcome" ) )
   {
      signin_resume( ).then( function( resumed )
      {
         if( resumed )
            enter_app( );
         else
            route( );
      } );
   }
   else
      route( );
}

// NOTE: What leaving for another app in this tab would lose - a form part filled in. The switcher asks first.
function unsent_work( )
{
   if( ciyam.sessid === "" )
      return "";

   // NOTE: Not the current password alone - a password manager fills that in by itself, and nothing is lost with it (Damon,
   // 2026-10-25: warned on leaving after setting up keys).
   var filled = [ "mine_password", "mine_confirm" ].some( function( id )
   {
      var field = document.getElementById( id );

      return ( field !== null ) && ( field.value !== "" );
   } );

   if( filled )
      return "A form here is part filled in.";

   return ( ( typeof keys_unsent === "function" ) && keys_unsent( ) ) ? "Your new keys aren't saved yet - their twelve words would be lost." : "";
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
   // NOTE: The version this browser keeps for the PIN - the hash taken up was made that way (found by review, 2026-10-23).
   ciyam.hash_version = ciyam.hash_version_for( ciyam.access );
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

      // NOTE: "from=accounts" so the console names this page, not the chat, and filters its log by it.
      apps_console_frame_load( document.getElementById( "console_drawer" ), "console.html?embedded=1&from=accounts&source=" + encodeURIComponent( g_self ) );
   }
}

function do_popout_console( )
{
   open_app_tab( c_tab_console, "console.html?from=accounts&source=" + encodeURIComponent( g_self ), ciyam.sessid );

   if( !document.getElementById( "console_drawer" ).hidden )
      do_toggle_console( );
}

// NOTE: A console or chat opened from this session must not carry on under the next.
function unlink_others( )
{
   g_channel.postMessage( g_self );

   g_console_loaded = false;

   document.getElementById( "console_drawer" ).hidden = true;
   apps_console_frame_unload( );

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
      {
         show_view( "signin_view" );

         // NOTE: As the shared sign in does - a node not set up says so, and points to Home (Ian, 2026-10-13).
         signin_check_locked( );
      }

      return;
   }

   // NOTE: People is Home's now - admin is taken there, on this tab's session ("signin_keep_session( )").
   if( ciyam.is_admin && ( c_people_views.indexOf( window.location.hash.replace( /^#/, "" ) ) >= 0 ) )
   {
      window.location.replace( "home.html#people" );

      return;
   }

   show_section( account_section( window.location.hash ) );
}

function show_view( id )
{
   [ "linking_view", "signin_view", "welcome_view", "written_view", "keys_step_view", "app_view" ].forEach( function( view )
   {
      document.getElementById( view ).hidden = ( view !== id );
   } );
}

// NOTE: One of My account's sections ("account_sections( )", 2026-10-26) - drawn afresh only for a session not drawn yet; the
// address's "#..." follows, so a reload comes back to it.
function show_section( name )
{
   var fresh = document.getElementById( "app_view" ).hidden || ( g_shown_for !== ciyam.sessid );

   show_view( "app_view" );

   if( fresh )
   {
      g_shown_for = ciyam.sessid;

      build_shell( );
      render_mine( );
   }

   var title = "";

   account_sections( ).forEach( function( section )
   {
      document.getElementById( "section_" + section.key ).hidden = ( section.key !== name );

      if( section.key === name )
         title = section.title;
   } );

   document.getElementById( "top_heading" ).textContent = title;

   shell_mark( [ "account", name ] );

   // NOTE: What is shown only when asked is hidden again on leaving its section (Ian, 2026-10-27) - the public key, and the QR
   // code that carries the access key to another device.
   if( ( name !== "keys" ) && ( typeof keys_hide_public === "function" ) )
      keys_hide_public( );

   if( ( name !== "devices" ) && ( typeof keys_hide_device === "function" ) )
      keys_hide_device( );
   shell_close_rail( );

   if( window.location.hash !== "#" + name )
      history.replaceState( null, "", window.location.pathname + window.location.search + "#" + name );

   window.scrollTo( 0, 0 );

   var heading = document.querySelector( "#section_" + name + " h1" );

   if( heading !== null )
      heading.focus( { preventScroll: true } );
}

// NOTE: The side menu, the chips and the bottom bar - built at each sign in, as Home's are ("build_shell( )" in "home.js"): the
// apps as the switcher lists them, My account's sections, and who is signed in.
function build_shell( )
{
   var rail_apps = document.getElementById( "rail_apps" );
   var rail_sections = document.getElementById( "rail_sections" );
   var chips = document.getElementById( "shell_chips" );
   var bottom = document.getElementById( "shell_bottombar" );

   [ rail_apps, rail_sections, chips, bottom ].forEach( function( element ) { element.replaceChildren( ); } );

   var open = function( app )
   {
      return function( event )
      {
         if( app.current )
            show_section( account_sections( )[ 0 ].key );
         else
            apps_go( app.key, "", event );
      };
   };

   switcher_apps( ciyam.is_admin, false, "account" ).forEach( function( app )
   {
      var item = shell_rail_item( app.key, app.title, true, open( app ) );

      if( !app.current )
         item.href = apps_address( app, "" );

      rail_apps.appendChild( item );
   } );

   switcher_apps( ciyam.is_admin, true, "account" ).forEach( function( app )
   {
      var item = shell_bottom_item( app.key, app.current ? "Account" : app.title, open( app ) );

      if( !app.current )
         item.href = apps_address( app, "" );

      bottom.appendChild( item );
   } );

   account_sections( ).forEach( function( section )
   {
      var go = function( ) { show_section( section.key ); };

      var item = shell_rail_item( section.key, section.title, false, go );
      var chip = shell_chip( section.key, section.title, go );

      // NOTE: Your keys' badge - 1 while there are none ("onboard_draw( )"), as Home counts what needs admin beside Health.
      if( section.key === "keys" )
      {
         [ item, chip ].forEach( function( element )
         {
            var badge = document.createElement( "span" );

            badge.className = "shell-badge";
            badge.dataset.badge = "keys";
            badge.hidden = true;

            element.appendChild( badge );
         } );
      }

      rail_sections.appendChild( item );
      chips.appendChild( chip );
   } );

   var name = account_display_name( ciyam.username, ciyam.is_admin );

   // NOTE: As Home says it ("role_text( )").
   var role = ciyam.is_admin ? "Administrator" : "Member";

   shell_show_user( name, role );

   document.getElementById( "top_sub" ).textContent = name + " · " + role;
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
   // NOTE: Rated by guesses once "zxcvbn.min.js" is in - read in now, and rated again then; the PIN and username count for
   // little in it.
   load_strength_rules( function( ) { update_strength( prefix ); } );

   var inputs = ( prefix === "join" ) ? [ g_join_pin, document.getElementById( "join_username" ).value ] : [ ciyam.access, ciyam.username ];

   var strength = password_strength( document.getElementById( prefix + "_password" ).value, inputs.filter( Boolean ) );

   var box = document.getElementById( prefix + "_strength" );

   box.hidden = ( strength.level < 0 );
   box.dataset.level = String( strength.level );

   document.getElementById( prefix + "_strength_label" ).textContent = strength.text;
}

// ====================================================================
// Sign in and out
// ====================================================================

// NOTE: After a claim, what Welcome's Remember box asked for - "plan_retain_choice( )" in
// "chat_parse.js", as the shared sign in ("signin.js") does after a sign in.
function apply_retain_choice( select_id )
{
   try
   {
      var plan = plan_retain_choice( localStorage.getItem( c_storage_access ), ciyam.access,
       document.getElementById( select_id || "signin_retain" ).value, ciyam.hashed );

      if( plan.keep_hash )
         localStorage.setItem( c_storage_hashed_prefix + ciyam.access, ciyam.hashed );
      else
         localStorage.removeItem( c_storage_hashed_prefix + ciyam.access );

      if( parse_access_list( plan.list ).indexOf( ciyam.access ) < 0 )
         localStorage.removeItem( c_signin_storage_keys_prefix + ciyam.access );

      if( plan.list === null )
         localStorage.removeItem( c_storage_access );
      else
         localStorage.setItem( c_storage_access, plan.list );
   }
   catch( e )
   {
   }

   signin_remember_name( ciyam.access, account_display_name( ciyam.username, ciyam.is_admin ) );
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
   signin_keep_session( );

   document.getElementById( "console_session" ).textContent = "inherits session " + ciyam.sessid;

   // NOTE: The console drawer is admin's, as in Home.
   document.getElementById( "console_toggle" ).hidden = !ciyam.is_admin;

   // NOTE: Moving between apps, and signing out of every one, is the switcher's - whoever opened this page.
   apps_refresh( );

   route( );
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

   g_onboarding = false;

   keys_card_return( );

   if( typeof keys_clear === "function" )
      keys_clear( );

   signin_forget_session( );

   g_viewers = { };

   ciyam.sessid = "";
   ciyam.access = "";
   ciyam.hashed = "";
   ciyam.unique = "";
   ciyam.username = "";
   ciyam.is_admin = false;

   // NOTE: The next account on this page must not see this one's devices.
   g_devices = [ ];
   g_device_armed = "";
   g_shown_for = "";

   document.getElementById( "mine_devices_list" ).replaceChildren( );

   set_error( "mine_devices_error", "" );

   clear_hash( );

   signin_fill_saved( );
}

// ====================================================================
// My account
// ====================================================================

function render_mine( )
{
   var name = ciyam.username || "";

   var shown = account_display_name( name, ciyam.is_admin );

   var avatar = document.getElementById( "mine_avatar" );

   avatar.textContent = user_initial( shown );
   avatar.style.background = "var(--color-sender-" + sender_colour_index( shown ) + ")";

   document.getElementById( "mine_name" ).textContent = shown;
   document.getElementById( "mine_type" ).textContent = ciyam.is_admin ? "Administrator" : "Member";

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

   show_mine_notify( );

   load_devices( );

   // NOTE: Your keys - "account_keys.js" (2026-10-25).
   if( typeof render_keys === "function" )
      render_keys( );
}

// NOTE: Desktop notifications from the chat (Damon, 2026-10-20) - the switch here and in the chat's account menu are one
// ("notify.js"). Where the browser cannot give them, the switch is off and says why.
function show_mine_notify( )
{
   var state = notify_state( );
   var on = notify_is_on( );
   var box = document.getElementById( "mine_notify_switch" );

   box.checked = on;
   box.disabled = ( state !== "ok" ) && !on;

   document.getElementById( "mine_notify_status" ).textContent = notify_status_text( on ? "on" : "off", state );
}

async function do_mine_notify( on )
{
   var outcome = await notify_switch( on );

   show_mine_notify( );

   // NOTE: Asked to turn on and the browser did not - refused, or its question closed unanswered - the line says which.
   if( on && ( outcome !== "on" ) )
      document.getElementById( "mine_notify_status" ).textContent = notify_status_text( "off", outcome );
}

// ====================================================================
// My devices - Ian, 2026-10-06
// ====================================================================

// NOTE: "GET /cws/devices" with this session - this PIN's devices, an active session marked.
function devices_url( suffix )
{
   return ciyam.get_cws_url( ) + "/devices" + suffix + "?access=" + ciyam.access + "&device=" + ciyam.device
    + "&format=text&session=" + ciyam.sessid;
}

async function load_devices( )
{
   g_device_armed = "";

   var asked_for = ciyam.access;

   var response = await request( function( done )
   {
      return ciyam.fetch( devices_url( "" ), "GET", done );
   } );

   // NOTE: Signed out, or another account in, while it was asked - not theirs to show (found by review).
   if( ( ciyam.access === "" ) || ( ciyam.access !== asked_for ) )
      return;

   if( problem_text( response ) !== "" )
   {
      g_devices = [ ];

      render_devices( );

      set_error( "mine_devices_error", "The devices could not be read: " + problem_text( response ) );

      return;
   }

   g_devices = parse_devices( response, ciyam.device );

   set_error( "mine_devices_error", "" );

   render_devices( );
}

// NOTE: Device tokens and sessions are the server's, so only "textContent" - and a token is shortened, as
// the console's log does with long values. This browser cannot be removed from here (the server refuses).
function render_devices( )
{
   var list = document.getElementById( "mine_devices_list" );

   list.replaceChildren( );

   g_devices.forEach( function( entry )
   {
      var row = document.createElement( "li" );

      row.className = "account-device";

      var label = document.createElement( "div" );

      label.className = "account-device-label";

      var token = document.createElement( "span" );

      token.className = "chat-mono";
      token.textContent = short_device( entry.device );
      token.title = entry.device;

      label.appendChild( token );

      var state = document.createElement( "span" );

      state.className = "account-device-state";
      var choice = device_choice( entry );

      state.textContent = choice.state;

      label.appendChild( state );

      row.appendChild( label );

      // NOTE: The one asked about, in place of the row's buttons ("arm_remove_device( )").
      var armed = choice.actions.filter( function( action ) { return g_device_armed === entry.device + " " + action.action; } )[ 0 ];

      if( armed )
      {
         var ask = document.createElement( "span" );

         ask.className = "account-confirm-text";
         ask.textContent = armed.ask;

         var yes = document.createElement( "button" );

         yes.type = "button";
         yes.className = "account-btn-danger";
         yes.textContent = armed.yes;
         yes.onclick = function( ) { do_remove_device( entry, armed ); };

         var no = document.createElement( "button" );

         no.type = "button";
         no.className = "chat-btn";
         no.textContent = "Keep";
         no.onclick = function( ) { arm_remove_device( "", "" ); };

         row.appendChild( ask );
         row.appendChild( yes );
         row.appendChild( no );
      }
      else
      {
         choice.actions.forEach( function( action )
         {
            var remove = document.createElement( "button" );

            remove.type = "button";
            remove.className = "chat-btn";
            remove.textContent = action.label;
            remove.setAttribute( "aria-label", action.label + " device " + short_device( entry.device ) );
            remove.onclick = function( ) { arm_remove_device( entry.device, action.action ); };

            row.appendChild( remove );
         } );
      }

      list.appendChild( row );
   } );

   if( g_devices.length === 0 )
   {
      var none = document.createElement( "li" );

      none.className = "account-device account-device--none";
      none.textContent = "None listed.";

      list.appendChild( none );
   }
}

// NOTE: Signing out or blocking asks once more in the row itself, as removing a person does.
function arm_remove_device( device, action )
{
   g_device_armed = ( device !== "" ) ? device + " " + action : "";

   render_devices( );
}

// NOTE: One request for both (Ian, 2026-10-26) - a Block says so ("options=block", "d370affc"), and blocks whatever the device's
// state. Without it, the node decides by whether the device has a session - "action" one of "device_choice( )"'s.
// NOTE: A DELETE blocks only with the "block" option (Ian's "54d1f99b", 2026-10-28) - without it, it only ever signs out, so a
// Sign out no longer reads the list again first.
async function do_remove_device( entry, action )
{
   var response = await request( function( done )
   {
      return ciyam.fetch( devices_url( "/" + encodeURIComponent( entry.device ) )
       + ( ( action.options !== "" ) ? "&options=" + encodeURIComponent( action.options ) : "" ), "DELETE", done );
   } );

   g_device_armed = "";

   if( problem_text( response ) !== "" )
      show_alert( ( ( action.action === "block" ) ? "Not blocked: " : "Not signed out: " ) + problem_text( response ), true );
   else
      show_alert( action.done );

   load_devices( );
}

// NOTE: What hardening a password (and signing in) is doing while "work" runs, under the form's button - "join" or "mine"
// ("sign_in_progress( )" in "chat_parse.js"; Ian, 2026-10-25: a progress whilst it is hashing).
async function with_progress( prefix, work )
{
   var box = document.getElementById( prefix + "_busy" );
   var bar = document.getElementById( prefix + "_busy_bar" );

   var show = function( stage, value )
   {
      var shown = sign_in_progress( stage, value );

      document.getElementById( prefix + "_busy_text" ).textContent = shown.text;

      bar.hidden = ( shown.fraction === null );

      if( shown.fraction !== null )
         bar.value = shown.fraction;
   };

   show( "securing", 0 );

   box.hidden = false;
   ciyam.on_progress = show;

   try
   {
      return await work( );
   }
   finally
   {
      ciyam.on_progress = null;

      box.hidden = true;
      bar.hidden = true;
   }
}

// NOTE: The hash this session was opened with, for a given password - "determine_hashed( )"
// in "ciyam.js", worked out without touching the session's own - as "chat.js" does. Hashed
// the way the session's password was ("ciyam.hash_version"), unless another version is given.
async function session_hash_for( password, version )
{
   return ciyam.session_hash( await ciyam.hash_combined( password, null, ( version == null ) ? ciyam.hash_version : version ) );
}

async function do_change_password( event )
{
   event.preventDefault( );

   var current = document.getElementById( "mine_current" ).value;
   var fresh = document.getElementById( "mine_password" ).value;
   var confirm = document.getElementById( "mine_confirm" ).value;

   // NOTE: A session handed over without its hash - Home's after a reload, its password not saved on this browser -
   // cannot check the current password here, so it says so rather than calling it wrong (found by review).
   if( ciyam.hashed === "" )
   {
      set_error( "mine_error", "Your current password can't be checked on this session - sign out, then sign in here to change it." );

      return;
   }

   // NOTE: The server only checks the session, so the current password is checked here,
   // against the hash the session was opened with - no request, and nothing sent. Hashing
   // it hardens it as a sign in does, a second or two, so the button is held meanwhile.
   var holding = document.getElementById( "mine_submit" );

   holding.disabled = true;

   var current_right = false;

   try
   {
      current_right = await with_progress( "mine", async function( ) { return await session_hash_for( current ) === ciyam.hashed; } );
   }
   finally
   {
      holding.disabled = false;
   }

   if( !current_right )
   {
      set_error( "mine_error", "The current password is not right." );

      return;
   }

   // NOTE: The rules by guesses are waited for, so a password is never let through by the old ones for being sent before
   // "zxcvbn.min.js" was in (review, 2026-10-25). If it cannot be read, the old ones are what there is.
   await load_strength_rules( );

   var fresh_strength = password_strength( fresh, [ ciyam.access, ciyam.username ].filter( Boolean ) );

   if( fresh_strength.level < 1 )
   {
      set_error( "mine_error", password_refusal( fresh_strength ) );

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
   // refuses it. The password is hashed with the PIN before it is sent - once, here, the current
   // way, and the session's hash made from the same (it had been hardened twice).
   var combined = "";

   try
   {
      combined = await with_progress( "mine", function( ) { return ciyam.hash_combined( fresh, ciyam.access, c_password_hash_version ); } );
   }
   catch( e )
   {
      submit.disabled = false;

      set_error( "mine_error", "The new password could not be secured - " + String( e && e.message || e ) );

      return;
   }

   var response = await request( function( done )
   {
      return ciyam.update_user( ciyam.access, "password=" + combined, done, true );
   } );

   submit.disabled = false;

   if( problem_text( response ) !== "" )
   {
      set_error( "mine_error", problem_text( response ) );

      return;
   }

   // NOTE: The session stays open. A hash the chat saved for this account was made from the old
   // password and is now refused, so it is replaced.
   // NOTE: Hashed the current way, so the session's hash and version are now that way too.
   var hashed = ciyam.session_hash( combined );

   ciyam.hashed = hashed;
   ciyam.hash_version = c_password_hash_version;

   ciyam.keep_hash_version( ciyam.access, c_password_hash_version );

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

   // NOTE: The rules by guesses are waited for, so a password is never let through by the old ones for being sent before
   // "zxcvbn.min.js" was in (review, 2026-10-25). If it cannot be read, the old ones are what there is.
   await load_strength_rules( );

   var problem = join_problem( username, password, confirm, g_join_pin );

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

      var combined = await with_progress( "join", function( ) { return ciyam.hash_combined( password, g_join_pin ); } );

      var credentials = CIYAM.encode_base64_url( username + ":" + combined );

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

      var pin = g_join_pin;

      // NOTE: Claimed with the current hash, so the sign in after uses it at once rather than trying an older one first.
      ciyam.keep_hash_version( pin, c_password_hash_version );

      // NOTE: Signed in at once with what was just chosen, then the Remember choice saved, as on the
      // sign in (Damon, 2026-10-05) - Ian found the chat's sign in waiting, on another PIN. A device
      // token from before the node was set up again is replaced, as the sign in does ("signin.js"). Should the
      // sign in fail, the PIN is still remembered, so the account can be found.
      await with_progress( "join", function( ) { return connect_as( pin, "", password ); } );

      if( is_unknown_device_error( ciyam.error ) )
      {
         ciyam.device = "";

         await with_progress( "join", function( ) { return connect_as( pin, "", password ); } );
      }

      if( ( ciyam.error === "" ) && ( ciyam.sessid !== "" ) )
      {
         remember_device( );

         apply_retain_choice( "join_retain" );
      }
      else
      {
         remember_pin( pin );

         signin_remember_name( pin, username );
      }

      show_written( pin, username );
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

// NOTE: Keys as the last step of joining (Damon, 2026-10-28) - set after "Write these down", until the step is ended (Skip, or Open
// the chat), or the account signs out. Your keys' card is brought into the step meanwhile - "g_keys_card_home" where it goes back.
var g_onboarding = false;
var g_keys_card_home = null;

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

   // NOTE: Not signed in by the claim - no keys to set up here, so two steps, and on to the chat's own sign in.
   var signed_in = ( ciyam.sessid !== "" );

   document.getElementById( "written_step" ).textContent = signed_in ? "Step 2 of 3" : "Step 2 of 2";
   document.getElementById( "written_open" ).textContent = signed_in ? "Next: your keys" : "Open the chat";

   update_written( );

   show_view( "written_view" );

   document.querySelector( "#written_view h2" ).focus( { preventScroll: true } );
}

function update_written( )
{
   document.getElementById( "written_open" ).disabled = !document.getElementById( "written_check" ).checked;
}

// NOTE: On to Your keys, the last step - the band at its top says why, and offers the chat for later.
function do_written_next( )
{
   if( ciyam.sessid === "" )
   {
      do_open_chat( );

      return;
   }

   g_onboarding = true;

   var card = document.getElementById( "mine_keys" );

   g_keys_card_home = { parent: card.parentNode, next: card.nextSibling };

   document.getElementById( "keys_step_host" ).appendChild( card );

   onboard_draw( "keys_loading" );

   show_view( "keys_step_view" );

   document.getElementById( "keys_step_title" ).focus( { preventScroll: true } );

   render_keys( );
}

// NOTE: Your keys' card back where it belongs, once the step has ended.
function keys_card_return( )
{
   if( g_keys_card_home === null )
      return;

   g_keys_card_home.parent.insertBefore( document.getElementById( "mine_keys" ), g_keys_card_home.next );

   g_keys_card_home = null;
}

// NOTE: Skip, or Open the chat once they are set up - on to the chat, and this page to My account, which says what needs you.
function do_keys_step_end( )
{
   g_onboarding = false;

   keys_card_return( );

   do_open_chat( );
}

// NOTE: As the keys card changes - "part" the part shown ("keys_show( )"): the last step of joining, while it is open, and what
// needs you on My account - Profile's card and Your keys' badge - while there are no keys.
function onboard_draw( part )
{
   var none = ( part === "keys_none" );

   document.getElementById( "keys_reminder" ).hidden = !none;

   document.querySelectorAll( "[data-badge='keys']" ).forEach( function( badge )
   {
      badge.textContent = "1";
      badge.hidden = !none;
   } );

   if( !g_onboarding )
      return;

   var here = ( part === "keys_here" );
   var said = keys_step_text( here );

   document.getElementById( "keys_step_title" ).textContent = said.title;
   document.getElementById( "keys_step_lede" ).textContent = said.text;

   // NOTE: Create keys and Skip until the words are being made - the card's own buttons then - and Open the chat once saved.
   document.getElementById( "keys_step_choose" ).hidden = !none && ( part !== "keys_loading" );
   document.getElementById( "keys_step_create" ).disabled = !none;
   document.getElementById( "keys_step_done" ).hidden = !here;
}

// NOTE: Signed in by the claim, the chat opens on this session - no second sign in - and this page
// goes on to the account. Not signed in, the chat's own sign in.
function do_open_chat( )
{
   if( ciyam.sessid === "" )
   {
      window.location.href = "chat.html";

      return;
   }

   open_app_tab( c_tab_chat, "chat.html?source=" + encodeURIComponent( g_self ), ciyam.sessid );

   enter_app( );
}

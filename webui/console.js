// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The developer console. Runs standalone, signing in itself, or linked to a chat tab
// - iframed in the chat's drawer or opened in its own window - when it inherits the chat's
// session over the "test_web_channel" BroadcastChannel. Pure logic is in
// "console_parse.js"; this file is the view.

const c_channel_name = "test_web_channel";
const c_log_channel_name = "ciyam_console_log";

const c_storage_device = "cws.device";
const c_storage_access = "cws.access";
const c_storage_hashed_prefix = "cws.hashed_";
const c_storage_script_prefix = "cws.script_";

const c_retain_none = "none";
const c_retain_access = "access";
const c_retain_full = "full";

const c_announce_interval = 2000;
const c_max_announces = 30;

const c_linked_request_timeout = 30000;

const c_output_line_limit = 40;

const c_log_limit = 500;
const c_log_summary_chars = 80;

const c_max_wait_ms = 60000;
const c_max_script_depth = 5;
const c_max_script_name = 60;

const c_delete_confirm_ms = 5000;

const c_raw_probe = "run_script *";

// NOTE: Only commands verified against the container are offered. A palette entry that
// fails teaches the user to distrust the palette.
const c_palette_commands = [
   { command: "status", description: "this session's status" },
   { command: "help", description: "console and server commands" },
   { command: "messages review 0000000", description: "list rooms and unread counts" },
   { command: "messages review 0000001 from=0", description: "the Administration room" },
   { command: "messages review <room> from=0", description: "every message in a room" },
   { command: "messages create 0000000 text=<room name>", description: "create a room" },
   { command: "messages create 0000000 for=<names>;text=<room name>", description: "create a room and invite" },
   { command: "messages create <room> text=<message>", description: "post to a room" },
   { command: "messages update <room> for=<names>", description: "invite to an existing room" },
   { command: "users review", description: "list accounts (admin)", admin: true },
   { command: "users create secret", description: "issue an account-creation token (admin)", admin: true },
   { command: "users create nominated=<pin>:<username>", description: "reserve a PIN and username (admin)", admin: true },
   { command: "vars", description: "show the variables" },
   { command: "var <name> <text>", description: "set a variable" },
   { command: "history", description: "commands entered this session" },
   { command: "clear", description: "clear the scrollback" },
   { command: "~run_script *", description: "list the server scripts (admin, dev)", raw: true }
];

var g_self = String( Date.now( ) ) + String( Math.floor( Math.random( ) * 1000 ) ).padStart( 3, "0" );

var g_source = "";
var g_owner = "";

var g_linked = false;
var g_embedded = false;
var g_connected = false;

var g_announces = 0;
var g_announce_timer = null;

var g_raw_available = null;
var g_server_scripts = [ ];

var g_history = [ ];
var g_history_at = 0;
var g_history_draft = "";

var g_log = [ ];
var g_log_keys = { };
var g_log_counter = 0;
var g_log_filter = "all";

var g_quiet = false;
var g_queue = Promise.resolve( );

var g_linked_request_id = 0;
var g_linked_pending = { };

var g_script_depth = 0;
var g_stop_requested = false;

var g_current_script = "";
var g_current_server_script = null;

var g_delete_armed = 0;

var g_palette_items = [ ];
var g_palette_index = 0;

var g_channel = null;
var g_log_channel = null;

// ====================================================================
// Entry point
// ====================================================================

function console_main( )
{
   var href = new URL( window.location.href );

   g_source = href.searchParams.get( "source" ) || "";
   g_embedded = ( href.searchParams.get( "embedded" ) === "1" );

   g_linked = ( g_source !== "" );

   if( g_embedded )
      document.getElementById( "console_app" ).classList.add( "is-embedded" );

   document.getElementById( "title_host" ).textContent = window.location.host;

   install_log_capture( ciyam, "console", function( ) { return g_quiet; }, add_console_entry );

   try
   {
      if( localStorage.getItem( c_storage_device ) !== null )
         ciyam.device = localStorage.getItem( c_storage_device );
   }
   catch( e )
   {
   }

   bind_tabs( );
   bind_prompt( );
   bind_palette( );
   bind_log_filter( );
   bind_server_filter( );

   window.addEventListener( "storage", function( )
   {
      render_storage( );
      render_saved_scripts( );
   } );

   g_channel = new BroadcastChannel( c_channel_name );
   g_channel.addEventListener( "message", on_channel_message );

   g_log_channel = new BroadcastChannel( c_log_channel_name );
   g_log_channel.addEventListener( "message", on_log_message );

   document.getElementById( "filter_chat" ).hidden = !g_linked;

   update_status( );

   if( g_linked )
   {
      document.getElementById( "waiting_view" ).hidden = false;

      announce( );

      g_announce_timer = window.setInterval( announce, c_announce_interval );
   }
   else
   {
      document.getElementById( "signin_view" ).hidden = false;

      populate_accounts( );
   }
}

// ====================================================================
// Linked session
// ====================================================================

function announce( )
{
   if( g_connected )
      return;

   if( ++g_announces > c_max_announces )
   {
      window.clearInterval( g_announce_timer );

      g_announce_timer = null;

      document.getElementById( "waiting_text" ).textContent =
       "The chat did not share a session. Is it signed in? Reload this console to try again.";

      return;
   }

   g_channel.postMessage( g_source + ":" + g_self );
}

function on_channel_message( event )
{
   var message = parse_channel_message( event.data );

   if( ( message === null ) || !g_linked )
      return;

   if( ( message.kind === "owner" ) && ( message.id === g_self ) )
      g_owner = message.rest;
   else if( ( message.kind === "credentials" ) && ( message.id === g_self ) && !g_connected )
   {
      var credentials = parse_credentials( message.rest );

      if( credentials !== null )
         adopt_session( credentials );
   }
   else if( ( message.kind === "ended" ) && g_connected
    && ( ( message.id === g_source ) || ( message.id === g_owner ) ) )
      end_linked_session( );
}

function adopt_session( credentials )
{
   ciyam.access = credentials.access;
   ciyam.device = credentials.device;
   ciyam.hashed = credentials.hashed;
   ciyam.sessid = credentials.sessid;
   ciyam.unique = credentials.unique;
   ciyam.username = credentials.username;
   ciyam.is_admin = credentials.is_admin;

   if( g_announce_timer !== null )
   {
      window.clearInterval( g_announce_timer );

      g_announce_timer = null;
   }

   // NOTE: Asks the chat for what it logged before this console existed - sign in and the
   // first loads happen before anyone opens the drawer.
   g_log_channel.postMessage( { kind: "replay", owner: g_source, viewer: g_self } );

   enter_console( );
}

function end_linked_session( )
{
   g_connected = false;

   ciyam.sessid = "";
   ciyam.unique = "";
   ciyam.hashed = "";

   print_line( "The chat session ended. This console is no longer connected.", "is-warn" );

   document.getElementById( "prompt_input" ).disabled = true;

   update_title( );
   update_status( );
}

function on_log_message( event )
{
   var data = event.data;

   if( !g_linked || ( data === null ) || ( typeof data !== "object" ) || ( data.owner !== g_source ) )
      return;

   if( ( data.kind === "response" ) && ( data.viewer === g_self ) && g_linked_pending[ data.id ] )
      g_linked_pending[ data.id ]( ( data.response === null ) ? null : String( data.response ) );
   else if( data.kind === "entry" )
      add_log_entry( data.entry );
   else if( ( data.kind === "entries" ) && ( data.viewer === g_self ) && Array.isArray( data.entries ) )
      data.entries.forEach( add_log_entry );
}

// ====================================================================
// Standalone sign in
// ====================================================================

function stored( key )
{
   try
   {
      return localStorage.getItem( key );
   }
   catch( e )
   {
      return null;
   }
}

function populate_accounts( )
{
   var select = document.getElementById( "signin_account" );

   while( select.options.length > 1 )
      select.remove( 1 );

   var entries = parse_access_list( stored( c_storage_access ) );

   entries.forEach( function( access )
   {
      var label = access;

      if( stored( c_storage_hashed_prefix + access ) !== null )
         label += "  ·  saved password";

      select.options.add( new Option( label, access, false ) );
   } );

   select.selectedIndex = ( entries.length > 0 ) ? 1 : 0;

   do_select_account( );
}

function do_select_account( )
{
   var access = document.getElementById( "signin_account" ).value;

   var pin = document.getElementById( "signin_pin" );
   var password = document.getElementById( "signin_password" );
   var retain = document.getElementById( "signin_retain" );

   var has_saved = ( access !== "" ) && ( stored( c_storage_hashed_prefix + access ) !== null );

   pin.value = access;
   pin.readOnly = ( access !== "" );

   password.value = "";
   password.placeholder = has_saved ? "saved - leave blank" : "";

   retain.value = has_saved ? c_retain_full : ( access !== "" ? c_retain_access : c_retain_none );

   set_error( "signin_error", "" );

   if( access === "" )
      pin.focus( );
   else
      password.focus( );
}

async function do_connect( )
{
   var access = document.getElementById( "signin_pin" ).value.trim( );
   var password = document.getElementById( "signin_password" ).value;

   set_error( "signin_error", "" );

   if( access === "" )
   {
      set_error( "signin_error", "Enter an account PIN." );

      return;
   }

   var hashed = "";

   if( password === "" )
      hashed = stored( c_storage_hashed_prefix + access ) || "";

   if( ( password === "" ) && ( hashed === "" ) )
   {
      set_error( "signin_error", "Enter the password." );

      return;
   }

   var button = document.getElementById( "signin_connect" );

   button.disabled = true;

   ciyam.error = "";

   // NOTE: "CIYAM.connect" prefers a hash it is handed over the password, so a typed
   // password must not arrive alongside an old hash.
   ciyam.hashed = "";

   await ciyam.connect( access, ciyam.device, hashed, password, function( ) { } );

   button.disabled = false;

   if( ciyam.device !== "" )
   {
      try
      {
         localStorage.setItem( c_storage_device, ciyam.device );
      }
      catch( e )
      {
      }
   }

   if( ciyam.error !== "" )
   {
      set_error( "signin_error", ciyam.error );

      return;
   }

   if( ciyam.sessid === "" )
   {
      set_error( "signin_error", "No session was established." );

      return;
   }

   apply_retain( document.getElementById( "signin_retain" ).value );

   document.getElementById( "signin_password" ).value = "";

   enter_console( );
}

// NOTE: The same keys and the same rules as the chat's "apply_retain_choice( )", so an
// account saved in either is offered by both.
function apply_retain( mode )
{
   try
   {
      var entries = parse_access_list( localStorage.getItem( c_storage_access ) );

      var pos = entries.indexOf( ciyam.access );

      if( mode === c_retain_none )
      {
         if( pos >= 0 )
            entries.splice( pos, 1 );

         localStorage.removeItem( c_storage_hashed_prefix + ciyam.access );
      }
      else
      {
         if( pos < 0 )
            entries.push( ciyam.access );

         if( ( mode === c_retain_full ) && ( ciyam.hashed !== "" ) )
            localStorage.setItem( c_storage_hashed_prefix + ciyam.access, ciyam.hashed );
         else
            localStorage.removeItem( c_storage_hashed_prefix + ciyam.access );
      }

      var value = format_access_list( entries );

      if( value === null )
         localStorage.removeItem( c_storage_access );
      else
         localStorage.setItem( c_storage_access, value );
   }
   catch( e )
   {
   }
}

async function do_disconnect( )
{
   if( g_linked )
   {
      print_line( "This console shares the chat's session - sign out from the chat.", "is-err" );

      return;
   }

   g_stop_requested = true;

   await ciyam.disconnect( function( ) { } );

   g_connected = false;
   g_raw_available = null;
   g_server_scripts = [ ];

   ciyam.remove_all_variables( );

   document.getElementById( "main_view" ).hidden = true;
   document.getElementById( "signin_view" ).hidden = false;
   document.getElementById( "scrollback" ).textContent = "";

   update_title( );
   update_status( );

   populate_accounts( );
}

// ====================================================================
// The console proper
// ====================================================================

function enter_console( )
{
   g_connected = true;

   document.getElementById( "signin_view" ).hidden = true;
   document.getElementById( "waiting_view" ).hidden = true;
   document.getElementById( "main_view" ).hidden = false;

   var prompt = document.getElementById( "prompt_input" );

   prompt.disabled = false;

   update_title( );
   update_status( );

   print_line( "CIYAM console · " + ( g_linked ? "linked to the chat" : "standalone" ) + " · signed in as "
    + ( ciyam.username || ciyam.access ) + ( ciyam.is_admin ? " [adm]" : " [std]" ), "is-dim" );

   print_line( "Type help for the commands, or press ctrl+k for the palette.", "is-dim" );

   render_saved_scripts( );
   render_vars( );
   render_storage( );
   render_log( );

   probe_raw( );

   select_tab( "console" );

   prompt.focus( );
}

function update_title( )
{
   var dot = document.getElementById( "title_dot" );
   var role = document.getElementById( "title_role" );

   dot.classList.toggle( "is-idle", !g_connected );
   role.classList.toggle( "is-idle", !g_connected );

   document.getElementById( "title_session" ).hidden = !g_connected;
   document.getElementById( "title_session_sep" ).hidden = !g_connected;
   document.getElementById( "title_disconnect" ).hidden = !g_connected || g_linked;

   if( g_connected )
   {
      document.getElementById( "title_host" ).textContent = ciyam.access + "@" + window.location.host;
      document.getElementById( "title_session" ).textContent = "session " + ciyam.sessid;

      role.textContent = ( ciyam.is_admin ? "[adm] " : "[std] " ) + ( ciyam.username || ciyam.access );
   }
   else
   {
      document.getElementById( "title_host" ).textContent = window.location.host;

      role.textContent = "not connected";
   }
}

function update_status( )
{
   document.getElementById( "status_mode" ).textContent = g_linked ? "linked to chat" : "standalone";
   document.getElementById( "status_device" ).textContent = ciyam.device || "none";
   document.getElementById( "status_requests" ).textContent = String( g_log_counter );

   var raw = "unknown";

   if( g_raw_available === true )
      raw = "available";
   else if( g_raw_available === false )
      raw = ciyam.is_admin ? "refused here" : "admin only";

   document.getElementById( "status_raw" ).textContent = raw;

   document.getElementById( "prompt_hint" ).textContent = "↑↓ history · ctrl+k palette" + ( g_raw_available ? " · ~ raw" : "" );
}

// ====================================================================
// Tabs
// ====================================================================

function tab_names( )
{
   return [ "console", "log", "scripts", "vars", "storage" ];
}

function bind_tabs( )
{
   tab_names( ).forEach( function( name, index )
   {
      var tab = document.getElementById( "tab_" + name );

      tab.addEventListener( "click", function( ) { select_tab( name ); } );

      tab.addEventListener( "keydown", function( event )
      {
         var names = tab_names( );

         var next = -1;

         if( event.key === "ArrowRight" )
            next = ( index + 1 ) % names.length;
         else if( event.key === "ArrowLeft" )
            next = ( index - 1 + names.length ) % names.length;

         if( next >= 0 )
         {
            event.preventDefault( );

            select_tab( names[ next ] );

            document.getElementById( "tab_" + names[ next ] ).focus( );
         }
      } );
   } );
}

function select_tab( name )
{
   tab_names( ).forEach( function( other )
   {
      var on = ( other === name );

      var tab = document.getElementById( "tab_" + other );

      tab.setAttribute( "aria-selected", on ? "true" : "false" );
      tab.tabIndex = on ? 0 : -1;

      document.getElementById( "panel_" + other ).hidden = !on;
   } );

   if( name === "vars" )
      render_vars( );
   else if( name === "storage" )
      render_storage( );
   else if( name === "console" )
      scroll_to_end( );
}

// ====================================================================
// Scrollback
// ====================================================================

function scroll_to_end( )
{
   var list = document.getElementById( "scrollback" );

   list.scrollTop = list.scrollHeight;
}

function print_line( text, kind )
{
   var line = document.createElement( "div" );

   line.className = "console-line" + ( kind ? " " + kind : "" );
   line.textContent = text;

   document.getElementById( "scrollback" ).appendChild( line );

   scroll_to_end( );

   return line;
}

function print_command( text )
{
   print_line( text, "is-cmd" );
}

// NOTE: Long output shows its head and holds the rest behind an expander, so one command
// cannot bury everything before it.
function print_output( text )
{
   var value = String( text );

   var kind = "";

   if( is_error_output( value ) )
      kind = "is-err";
   else if( value.trim( ) === "[okay]" )
      kind = "is-ok";

   var cut = truncate_lines( value, c_output_line_limit );

   cut.shown.forEach( function( line ) { print_line( line, kind ); } );

   if( cut.hidden.length === 0 )
      return;

   var more = document.createElement( "button" );

   more.type = "button";
   more.className = "console-expander";
   more.textContent = "show " + cut.hidden.length + " more line" + ( cut.hidden.length === 1 ? "" : "s" );

   more.addEventListener( "click", function( )
   {
      var fragment = document.createDocumentFragment( );

      cut.hidden.forEach( function( line )
      {
         var node = document.createElement( "div" );

         node.className = "console-line" + ( kind ? " " + kind : "" );
         node.textContent = line;

         fragment.appendChild( node );
      } );

      more.replaceWith( fragment );
   } );

   document.getElementById( "scrollback" ).appendChild( more );

   scroll_to_end( );
}

// ====================================================================
// Prompt and history
// ====================================================================

function bind_prompt( )
{
   var input = document.getElementById( "prompt_input" );

   input.addEventListener( "keydown", function( event )
   {
      if( event.key === "ArrowUp" )
      {
         event.preventDefault( );

         if( g_history_at === g_history.length )
            g_history_draft = input.value;

         if( g_history_at > 0 )
         {
            --g_history_at;

            input.value = g_history[ g_history_at ];
         }
      }
      else if( event.key === "ArrowDown" )
      {
         event.preventDefault( );

         if( g_history_at < g_history.length - 1 )
         {
            ++g_history_at;

            input.value = g_history[ g_history_at ];
         }
         else
         {
            g_history_at = g_history.length;

            input.value = g_history_draft;
         }
      }
   } );
}

// NOTE: History is kept in memory only. A typed line can hold a password ("users update
// <pin> password=..."), and like the scrollback it does not belong on disk.
function do_submit_prompt( )
{
   var input = document.getElementById( "prompt_input" );

   var line = input.value;

   if( line.trim( ) === "" )
      return;

   g_history = push_history( g_history, line );
   g_history_at = g_history.length;
   g_history_draft = "";

   input.value = "";

   run_line( line );
}

// ====================================================================
// Running commands
// ====================================================================

function delay( ms )
{
   return new Promise( function( resolve ) { window.setTimeout( resolve, ms ); } );
}

// NOTE: One request at a time, in the order they were asked for. "quiet" is set just as
// the request starts, which is when the log capture asks whether to record it.
//
// Linked, the request goes to the chat to send instead: the server keeps one command slot
// per access and device, so this console and the chat must never have requests in flight
// on the shared session at the same time (ISS-020). The chat queues them with its own.
function send_request( method, url, quiet )
{
   if( g_linked )
      return send_linked_request( method, url, quiet );

   var job = g_queue.then( function( )
   {
      return new Promise( function( resolve )
      {
         var answered = false;

         g_quiet = !!quiet;

         var pending = ciyam.fetch( url, method, function( response )
         {
            answered = true;

            resolve( String( response ) );
         } );

         g_quiet = false;

         pending.then( function( ) { if( !answered ) resolve( null ); } );
      } );
   } );

   g_queue = job.catch( function( ) { } );

   return job;
}

function send_linked_request( method, url, quiet )
{
   var id = ++g_linked_request_id;

   var started = new Date( );

   return new Promise( function( resolve )
   {
      var timer = window.setTimeout( function( ) { finish( null ); }, c_linked_request_timeout );

      function finish( response )
      {
         if( !g_linked_pending[ id ] )
            return;

         delete g_linked_pending[ id ];

         window.clearTimeout( timer );

         if( !quiet )
            add_console_entry( make_log_entry( "console", method, url, null, response, started, new Date( ) ) );

         resolve( response );
      }

      g_linked_pending[ id ] = finish;

      g_log_channel.postMessage( { kind: "request", owner: g_source, viewer: g_self, id: id, method: method, url: url } );
   } );
}

function session_info( )
{
   return { access: ciyam.access, device: ciyam.device, sessid: ciyam.sessid };
}

// NOTE: Resolves to { ok } so a script knows whether to carry on.
async function run_line( line, from_script )
{
   print_command( line );

   // NOTE: "{name}" substitution is Ian's, from "ciyam.js", so a script written for the
   // harness runs unchanged here - including "." at the start to switch it off.
   var text = ciyam.replace_variables( String( line ).trim( ) );

   var spec = resolve_command( text );

   if( spec.kind === "none" )
      return { ok: true };

   if( spec.kind === "unknown" )
   {
      if( spec.reason )
         print_line( "Error: " + spec.reason + ".", "is-err" );
      else
         print_line( "Error: Unknown command '" + spec.word + "'. Type help for the commands, or prefix it with ~ to send it as raw protocol.", "is-err" );

      return { ok: false };
   }

   if( spec.kind === "local" )
      return run_local( spec, from_script );

   if( spec.kind === "quit" )
   {
      if( g_linked )
      {
         print_line( "Error: This console shares the chat's session - sign out from the chat.", "is-err" );

         return { ok: false };
      }

      await do_disconnect( );

      return { ok: true };
   }

   if( !g_connected )
   {
      print_line( "Error: Not connected.", "is-err" );

      return { ok: false };
   }

   var url;

   if( spec.kind === "raw" )
      url = build_cws_url( ciyam.get_cws_url( ), { path: "", request: spec.request }, session_info( ) );
   else
   {
      // NOTE: A password in "users update <pin> password=<text>" is hashed with the PIN, as
      // the harness does, so it never travels in the clear.
      if( ( spec.key === "users|update" ) && /^password=/.test( spec.options ) )
         spec.options = "password=" + ciyam.hash_combined( spec.options.substring( 9 ), spec.name );

      url = build_cws_url( ciyam.get_cws_url( ), spec, session_info( ) );
   }

   var response = await send_request( spec.kind === "raw" ? "GET" : spec.method, url, false );

   if( response === null )
   {
      print_line( "Error: No response - the request failed. Is the server reachable?", "is-err" );

      return { ok: false };
   }

   print_output( response );

   if( ( spec.kind === "raw" ) && ( response.trim( ) === "[bad]" ) )
      print_line( "Raw protocol needs the admin PIN on a development system, and a command starting a-z.", "is-dim" );

   return { ok: !is_error_output( response ) };
}

async function run_local( spec, from_script )
{
   var args = spec.args;

   switch( spec.name )
   {
      case "help":
         print_help( );

         if( g_connected )
         {
            var response = await send_request( "GET",
             build_cws_url( ciyam.get_cws_url( ), { path: "/help" }, session_info( ) ), false );

            if( response !== null )
            {
               print_line( "Server commands", "is-dim" );
               print_output( response );
            }
         }

         return { ok: true };

      case "clear":
         document.getElementById( "scrollback" ).textContent = "";

         return { ok: true };

      case "vars":
      {
         var all = ciyam.get_all_variables( );

         print_output( all === "" ? "(no variables)" : all );

         return { ok: true };
      }

      case "var":
      {
         var space = args.search( /\s/ );

         var name = ( space < 0 ) ? args : args.substr( 0, space );
         var value = ( space < 0 ) ? null : args.substring( space ).trim( );

         if( name === "" )
         {
            print_line( "Error: Usage is var <name> [<text>].", "is-err" );

            return { ok: false };
         }

         if( value === null )
         {
            print_output( ciyam.has_variable( name ) ? ciyam.get_variable( name ) : "(not set)" );

            return { ok: true };
         }

         if( !is_valid_variable_name( name ) )
         {
            print_line( "Error: Invalid variable name '" + name + "' - start with a letter, use letters, digits and _, and not all upper case.", "is-err" );

            return { ok: false };
         }

         ciyam.set_variable( name, value );

         render_vars( );

         return { ok: true };
      }

      case "unset":
         if( ciyam.has_variable( args ) )
         {
            ciyam.remove_variable( args );

            render_vars( );
         }

         return { ok: true };

      case "echo":
         print_output( args );

         return { ok: true };

      case "seed":
      {
         var count = parseInt( args, 10 );

         print_output( CIYAM.generate_base64_key( ( count > 0 && count <= 256 ) ? count : undefined ) );

         return { ok: true };
      }

      case "history":
         if( g_history.length === 0 )
            print_line( "(no history)", "is-dim" );
         else
            print_output( g_history.map( function( h, i ) { return String( i + 1 ).padStart( 3, " " ) + "  " + h; } ).join( "\n" ) );

         return { ok: true };

      case "wait":
      {
         var ms = Math.min( Math.max( parseInt( args, 10 ) || 0, 0 ), c_max_wait_ms );

         await delay( ms );

         return { ok: true };
      }

      case "run":
      {
         var body = stored( c_storage_script_prefix + args );

         if( body === null )
         {
            print_line( "Error: No saved script called '" + args + "'.", "is-err" );

            return { ok: false };
         }

         return run_script_body( body, args );
      }
   }

   return { ok: false };
}

function print_help( )
{
   var lines = [
    "Console commands",
    "  help                         this list, then the server's",
    "  status                       this session's status",
    "  <noun> <verb> [name] [opts]  a CWS request - e.g. messages review 0000001 from=0",
    "  ~<command>                   raw CIYAM protocol (admin, development system)",
    "  var <name> [<text>]          show or set a variable; use it as {name}",
    "  unset <name>                 remove a variable",
    "  vars                         list the variables",
    "  echo <text>                  print text, after substitution",
    "  seed [<count>]               print random characters",
    "  wait <ms>                    pause - useful in scripts",
    "  run <script>                 run a saved script",
    "  history                      commands entered this session",
    "  clear                        clear the scrollback"
   ];

   // NOTE: Linked, the session is the chat's - ending it here would sign the chat out.
   if( !g_linked )
      lines.push( "  quit                         sign out" );

   lines.push( "" );
   lines.push( "Keys: up and down recall history, ctrl+k opens the palette." );

   print_output( lines.join( "\n" ) );
}

// ====================================================================
// Raw protocol probe and server scripts
// ====================================================================

// NOTE: Raw protocol needs the admin PIN on a development system. Probing once, quietly,
// means the "~" affordance and the server scripts are only offered when they will work -
// and "run_script *" never executes anything, so the probe doubles as the script list.
async function probe_raw( )
{
   g_server_scripts = [ ];

   document.getElementById( "server_filter" ).value = "";

   if( !ciyam.is_admin )
   {
      g_raw_available = false;

      update_status( );
      render_server_scripts( );

      return;
   }

   var url = build_cws_url( ciyam.get_cws_url( ), { path: "", request: c_raw_probe }, session_info( ) );

   var response = await send_request( "GET", url, true );

   if( ( response === null ) || ( response.trim( ) === "[bad]" ) || ( response.indexOf( "Error: " ) === 0 ) )
      g_raw_available = false;
   else
   {
      g_raw_available = true;

      g_server_scripts = parse_script_list( response );
   }

   update_status( );
   render_server_scripts( );
}

// NOTE: The same matching as the palette - every word typed must appear in the script's
// name or its arguments, and name matches come first.
function bind_server_filter( )
{
   var filter = document.getElementById( "server_filter" );

   filter.addEventListener( "input", render_server_scripts );

   filter.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Escape" ) && ( filter.value !== "" ) )
      {
         event.preventDefault( );

         event.stopPropagation( );

         filter.value = "";

         render_server_scripts( );

         // NOTE: Back to the full list with the script that was picked still in view.
         var current = document.querySelector( "#server_scripts [aria-current=true]" );

         if( current !== null )
            current.scrollIntoView( { block: "nearest" } );
      }
   } );
}

function render_server_scripts( )
{
   var holder = document.getElementById( "server_scripts" );
   var note = document.getElementById( "server_scripts_note" );
   var filter = document.getElementById( "server_filter" );

   holder.textContent = "";

   filter.hidden = ( g_server_scripts.length === 0 );

   var items = g_server_scripts.map( function( script )
   {
      return { command: script.name, description: script.args.join( " " ), script: script };
   } );

   var shown = filter_palette( items, filter.value );

   if( !ciyam.is_admin )
      note.textContent = "Server scripts need the admin PIN on a development system.";
   else if( g_raw_available === false )
      note.textContent = "Raw protocol is refused here, so server scripts are unavailable - it needs a development system.";
   else if( g_raw_available === null )
      note.textContent = "Checking…";
   else if( shown.length === g_server_scripts.length )
      note.textContent = g_server_scripts.length + " from run_script *";
   else if( shown.length === 0 )
      note.textContent = "No scripts match - Esc clears the filter.";
   else
      note.textContent = shown.length + " of " + g_server_scripts.length + " from run_script *";

   shown.forEach( function( item )
   {
      var script = item.script;

      var button = document.createElement( "button" );

      button.type = "button";
      button.className = "console-item";
      button.textContent = script.name;

      if( ( g_current_server_script !== null ) && ( g_current_server_script.name === script.name ) )
         button.setAttribute( "aria-current", "true" );

      var sub = document.createElement( "span" );

      sub.className = "console-sub";
      sub.textContent = ( script.args.length > 0 ) ? script.args.join( " " ) : "no arguments";

      button.appendChild( sub );

      button.addEventListener( "click", function( ) { show_server_script( script, button ); } );

      holder.appendChild( button );
   } );
}

function mark_current_item( button )
{
   document.querySelectorAll( "#panel_scripts .console-item" ).forEach( function( item )
   {
      item.removeAttribute( "aria-current" );
   } );

   if( button )
      button.setAttribute( "aria-current", "true" );
}

function show_server_script( script, button )
{
   g_current_server_script = script;

   mark_current_item( button );

   document.getElementById( "args_title" ).textContent = script.name;

   var form = document.getElementById( "args_form" );

   form.textContent = "";

   if( script.args.length === 0 )
   {
      var none = document.createElement( "span" );

      none.className = "console-slabel";
      none.textContent = "No arguments";

      form.appendChild( none );
      form.appendChild( document.createElement( "span" ) );
   }

   script.args.forEach( function( arg, index )
   {
      var id = "arg_" + index;

      var label = document.createElement( "label" );

      label.className = "console-slabel";
      label.htmlFor = id;
      label.textContent = arg;

      var field = document.createElement( "input" );

      field.className = "console-sfield";
      field.id = id;
      field.type = "text";
      field.spellcheck = false;
      field.dataset.arg = arg;
      field.placeholder = "required";

      field.addEventListener( "input", update_args_preview );

      form.appendChild( label );
      form.appendChild( field );
   } );

   var destructive = is_destructive_script( script.name );

   document.getElementById( "args_confirm_row" ).hidden = !destructive;
   document.getElementById( "args_confirm" ).checked = false;

   set_error( "args_error", "" );

   document.getElementById( "script_editor" ).hidden = true;
   document.getElementById( "script_args" ).hidden = false;

   update_args_preview( );
}

function args_values( )
{
   var values = { };

   document.querySelectorAll( "#args_form input" ).forEach( function( field )
   {
      values[ field.dataset.arg ] = field.value.trim( );
   } );

   return values;
}

function update_args_preview( )
{
   if( g_current_server_script === null )
      return;

   var values = args_values( );

   var shown = { };

   Object.keys( values ).forEach( function( key ) { shown[ key ] = values[ key ] || "?"; } );

   var built = build_script_command( g_current_server_script.name, shown );

   document.getElementById( "args_preview" ).textContent = built.error ? built.error : "~" + built.command;

   var missing = Object.keys( values ).some( function( key ) { return values[ key ] === ""; } );

   var unconfirmed = !document.getElementById( "args_confirm_row" ).hidden && !document.getElementById( "args_confirm" ).checked;

   document.getElementById( "args_run" ).disabled = missing || unconfirmed || !!built.error;
}

function do_run_server_script( )
{
   if( g_current_server_script === null )
      return;

   var built = build_script_command( g_current_server_script.name, args_values( ) );

   if( built.error )
   {
      set_error( "args_error", built.error );

      return;
   }

   select_tab( "console" );

   run_line( "~" + built.command );
}

// ====================================================================
// Saved scripts
// ====================================================================

function saved_script_names( )
{
   var names = [ ];

   try
   {
      for( var i = 0; i < localStorage.length; i++ )
      {
         var key = localStorage.key( i );

         if( key.indexOf( c_storage_script_prefix ) === 0 )
            names.push( key.substring( c_storage_script_prefix.length ) );
      }
   }
   catch( e )
   {
   }

   return names.sort( );
}

function render_saved_scripts( )
{
   var holder = document.getElementById( "saved_scripts" );

   holder.textContent = "";

   saved_script_names( ).forEach( function( name )
   {
      var body = stored( c_storage_script_prefix + name ) || "";

      var steps = split_script( body ).length;

      var button = document.createElement( "button" );

      button.type = "button";
      button.className = "console-item";
      button.textContent = name;

      var sub = document.createElement( "span" );

      sub.className = "console-sub";
      sub.textContent = steps + " command" + ( steps === 1 ? "" : "s" );

      button.appendChild( sub );

      if( name === g_current_script )
         button.setAttribute( "aria-current", "true" );

      button.addEventListener( "click", function( ) { open_saved_script( name, button ); } );

      holder.appendChild( button );
   } );
}

function show_script_editor( )
{
   g_current_server_script = null;

   document.getElementById( "script_args" ).hidden = true;
   document.getElementById( "script_editor" ).hidden = false;

   render_saved_scripts( );
}

function open_saved_script( name, button )
{
   show_script_editor( );

   g_current_script = name;

   document.getElementById( "script_name" ).value = name;
   document.getElementById( "script_body" ).value = stored( c_storage_script_prefix + name ) || "";
   document.getElementById( "script_delete" ).hidden = false;

   disarm_delete( );

   set_error( "script_error", "" );

   render_saved_scripts( );
}

function do_new_script( )
{
   show_script_editor( );

   g_current_script = "";

   document.getElementById( "script_name" ).value = "";
   document.getElementById( "script_body" ).value = "";
   document.getElementById( "script_delete" ).hidden = true;

   set_error( "script_error", "" );

   render_saved_scripts( );

   document.getElementById( "script_name" ).focus( );
}

function do_save_script( )
{
   var name = document.getElementById( "script_name" ).value.trim( );
   var body = document.getElementById( "script_body" ).value;

   if( name === "" )
   {
      set_error( "script_error", "Give the script a name first." );

      return;
   }

   if( name.length > c_max_script_name )
   {
      set_error( "script_error", "Keep the name to " + c_max_script_name + " characters or fewer." );

      return;
   }

   try
   {
      localStorage.setItem( c_storage_script_prefix + name, body );
   }
   catch( e )
   {
      set_error( "script_error", "This browser would not save it." );

      return;
   }

   // NOTE: Saving under a new name keeps the old script - it is "save as", not a rename.
   g_current_script = name;

   document.getElementById( "script_delete" ).hidden = false;

   set_error( "script_error", "" );

   render_saved_scripts( );
   render_storage( );
}

function disarm_delete( )
{
   g_delete_armed = 0;

   document.getElementById( "script_delete" ).textContent = "Delete";
}

// NOTE: Two clicks rather than a dialog - the first arms it for a few seconds.
function do_delete_script( )
{
   if( g_current_script === "" )
      return;

   if( ( g_delete_armed === 0 ) || ( Date.now( ) - g_delete_armed > c_delete_confirm_ms ) )
   {
      g_delete_armed = Date.now( );

      document.getElementById( "script_delete" ).textContent = "Click again to delete";

      window.setTimeout( function( )
      {
         if( ( g_delete_armed !== 0 ) && ( Date.now( ) - g_delete_armed >= c_delete_confirm_ms ) )
            disarm_delete( );
      }, c_delete_confirm_ms );

      return;
   }

   try
   {
      localStorage.removeItem( c_storage_script_prefix + g_current_script );
   }
   catch( e )
   {
   }

   disarm_delete( );

   do_new_script( );

   render_storage( );
}

function do_run_editor( )
{
   var body = document.getElementById( "script_body" ).value;

   var name = document.getElementById( "script_name" ).value.trim( ) || "unsaved script";

   if( split_script( body ).length === 0 )
   {
      set_error( "script_error", "There is nothing to run." );

      return;
   }

   set_error( "script_error", "" );

   run_script_body( body, name );
}

function do_stop_script( )
{
   g_stop_requested = true;
}

function set_running( running )
{
   document.getElementById( "script_run" ).disabled = running;
   document.getElementById( "script_stop" ).hidden = !running;
}

// NOTE: Running anything switches to the Console, so the command and its response are
// watched rather than guessed at. A run stops at the first error: a script is usually a
// sequence where each step depends on the last.
async function run_script_body( body, name )
{
   if( g_script_depth >= c_max_script_depth )
   {
      print_line( "Error: Scripts are nested too deeply - does '" + name + "' run itself?", "is-err" );

      return { ok: false };
   }

   var steps = split_script( body );

   if( g_script_depth === 0 )
   {
      g_stop_requested = false;

      select_tab( "console" );

      set_running( true );
   }

   ++g_script_depth;

   print_line( "Running '" + name + "' - " + steps.length + " step" + ( steps.length === 1 ? "" : "s" ), "is-dim" );

   var ok = true;

   for( var i = 0; i < steps.length; i++ )
   {
      if( g_stop_requested )
      {
         print_line( "Stopped before line " + steps[ i ].line + " of '" + name + "'.", "is-warn" );

         ok = false;

         break;
      }

      var result = await run_line( steps[ i ].text, true );

      if( !result.ok )
      {
         print_line( "Stopped at line " + steps[ i ].line + " of '" + name + "'.", "is-err" );

         ok = false;

         break;
      }
   }

   --g_script_depth;

   if( g_script_depth === 0 )
   {
      set_running( false );

      if( ok )
         print_line( "Finished '" + name + "'.", "is-ok" );
   }

   return { ok: ok };
}

// ====================================================================
// Variables
// ====================================================================

function render_vars( )
{
   var body = document.getElementById( "vars_body" );

   body.textContent = "";

   var session = [
      [ "ACCESS", ciyam.access ],
      [ "DEVICE", ciyam.device ],
      [ "HASHED", ciyam.hashed ? ciyam.hashed.substr( 0, 15 ) + "..." : "" ],
      [ "SESSID", ciyam.sessid ],
      [ "UNIQUE", ciyam.unique ]
   ];

   session.forEach( function( pair )
   {
      if( pair[ 1 ] !== "" )
         body.appendChild( kv_row( pair[ 0 ], pair[ 1 ], "session" ) );
   } );

   // NOTE: The add row sits above the user variables so it is always in the same place.
   var add = document.createElement( "tr" );

   var name_cell = document.createElement( "td" );
   var value_cell = document.createElement( "td" );
   var by_cell = document.createElement( "td" );

   var name_input = make_vedit( "", "new name", "New variable name" );
   var value_input = make_vedit( "", "value - Enter to add", "New variable value" );

   value_input.addEventListener( "keydown", function( event )
   {
      if( event.key !== "Enter" )
         return;

      event.preventDefault( );

      var name = name_input.value.trim( );

      if( !is_valid_variable_name( name ) )
      {
         set_error( "vars_error", "Invalid variable name '" + name + "' - start with a letter, use letters, digits and _, and not all upper case." );

         return;
      }

      set_error( "vars_error", "" );

      if( value_input.value === "" )
         ciyam.remove_variable( name );
      else
         ciyam.set_variable( name, value_input.value );

      render_vars( );
   } );

   name_cell.className = "console-k";
   name_cell.appendChild( name_input );
   value_cell.appendChild( value_input );
   by_cell.className = "console-by";
   by_cell.textContent = "-";

   add.appendChild( name_cell );
   add.appendChild( value_cell );
   add.appendChild( by_cell );

   body.appendChild( add );

   var names = Array.from( ciyam.var_map.keys( ) ).sort( );

   names.forEach( function( name )
   {
      var row = document.createElement( "tr" );

      var key = document.createElement( "td" );

      key.className = "console-k";
      key.textContent = name;

      var cell = document.createElement( "td" );

      var edit = make_vedit( ciyam.get_variable( name ), "", name );

      // NOTE: Emptying a value removes the variable, as the harness's "var" does.
      edit.addEventListener( "change", function( )
      {
         if( edit.value === "" )
         {
            ciyam.remove_variable( name );

            render_vars( );
         }
         else
            ciyam.set_variable( name, edit.value );
      } );

      cell.appendChild( edit );

      var by = document.createElement( "td" );

      by.className = "console-by";
      by.textContent = "var";

      row.appendChild( key );
      row.appendChild( cell );
      row.appendChild( by );

      body.appendChild( row );
   } );

   document.getElementById( "count_vars" ).textContent = String( names.length );
}

function make_vedit( value, placeholder, label )
{
   var input = document.createElement( "input" );

   input.type = "text";
   input.className = "console-vedit";
   input.value = value;
   input.placeholder = placeholder;
   input.spellcheck = false;
   input.setAttribute( "aria-label", label );

   return input;
}

function kv_row( key, value, by, by_is_num )
{
   var row = document.createElement( "tr" );

   var k = document.createElement( "td" );
   var v = document.createElement( "td" );
   var b = document.createElement( "td" );

   k.className = "console-k";
   k.textContent = key;

   v.className = "console-v";
   v.textContent = value;

   b.className = "console-by" + ( by_is_num ? " is-num" : "" );
   b.textContent = by;

   row.appendChild( k );
   row.appendChild( v );
   row.appendChild( b );

   return row;
}

// ====================================================================
// Storage
// ====================================================================

function render_storage( )
{
   var body = document.getElementById( "storage_body" );

   body.textContent = "";

   var keys = [ ];

   try
   {
      for( var i = 0; i < localStorage.length; i++ )
         keys.push( localStorage.key( i ) );
   }
   catch( e )
   {
   }

   keys.sort( ).forEach( function( key )
   {
      var value = stored( key );

      body.appendChild( kv_row( key, summarise_storage_value( key, value ), String( ( value || "" ).length ), true ) );
   } );

   document.getElementById( "count_storage" ).textContent = String( keys.length );
}

// ====================================================================
// Request log
// ====================================================================

function bind_log_filter( )
{
   document.querySelectorAll( "#panel_log [data-source]" ).forEach( function( chip )
   {
      chip.addEventListener( "click", function( )
      {
         g_log_filter = chip.dataset.source;

         document.querySelectorAll( "#panel_log [data-source]" ).forEach( function( other )
         {
            other.setAttribute( "aria-pressed", ( other === chip ) ? "true" : "false" );
         } );

         render_log( );
      } );
   } );
}

function add_console_entry( entry )
{
   entry.id = ++g_log_counter;

   add_log_entry( entry );

   update_status( );
}

// NOTE: Chat entries arrive both live and in the replay asked for on linking, so each is
// keyed by its source and id and kept once.
function add_log_entry( entry )
{
   if( ( entry === null ) || ( typeof entry !== "object" ) )
      return;

   var key = entry.source + ":" + entry.id;

   if( g_log_keys[ key ] )
      return;

   g_log_keys[ key ] = true;

   g_log.push( entry );

   if( g_log.length > c_log_limit )
   {
      var dropped = g_log.shift( );

      delete g_log_keys[ dropped.source + ":" + dropped.id ];

      render_log( );
   }
   else if( ( g_log_filter === "all" ) || ( g_log_filter === entry.source ) )
      append_log_rows( entry );

   update_log_count( );
}

function update_log_count( )
{
   document.getElementById( "count_log" ).textContent = String( g_log.length );
   document.getElementById( "log_empty" ).hidden = ( g_log.length > 0 );
}

function do_clear_log( )
{
   g_log = [ ];
   g_log_keys = { };

   render_log( );
}

function render_log( )
{
   document.getElementById( "log_body" ).textContent = "";

   g_log.forEach( function( entry )
   {
      if( ( g_log_filter === "all" ) || ( g_log_filter === entry.source ) )
         append_log_rows( entry );
   } );

   update_log_count( );
}

function cell( text, class_name )
{
   var td = document.createElement( "td" );

   if( class_name )
      td.className = class_name;

   td.textContent = text;

   return td;
}

function append_log_rows( entry )
{
   var body = document.getElementById( "log_body" );

   var row = document.createElement( "tr" );

   row.className = "console-logrow";
   row.tabIndex = 0;
   row.setAttribute( "aria-expanded", "false" );

   var twist = cell( "›", "console-twist" );

   var source = document.createElement( "td" );
   var tag = document.createElement( "span" );

   tag.className = "console-src";
   tag.textContent = String( entry.source );

   source.appendChild( tag );

   var result = document.createElement( "td" );
   var badge = document.createElement( "span" );

   badge.className = "console-result" + ( entry.ok ? "" : " is-err" );
   badge.textContent = entry.ok ? "ok" : "error";

   result.appendChild( badge );

   row.appendChild( twist );
   row.appendChild( cell( String( entry.time ), "console-when" ) );
   row.appendChild( source );
   row.appendChild( cell( String( entry.method ), "console-verb" ) );

   // NOTE: The options, or the raw command, beside the endpoint - every raw request is
   // "/cws", so without this the rows cannot be told apart without opening each one.
   var path = cell( String( entry.endpoint ), "console-path" );

   if( entry.request )
   {
      var summary = String( entry.request ).split( "\n" )[ 0 ];

      if( summary.length > c_log_summary_chars )
         summary = summary.substr( 0, c_log_summary_chars ) + "…";

      path.appendChild( make_span( "  " + summary, "console-reqsum" ) );
   }

   row.appendChild( path );
   row.appendChild( result );
   row.appendChild( cell( String( entry.ms ), "console-ms is-num" ) );

   var detail = document.createElement( "tr" );

   detail.className = "console-detail";
   detail.hidden = true;

   var detail_cell = document.createElement( "td" );

   detail_cell.colSpan = 7;

   var pair = document.createElement( "div" );

   pair.className = "console-pair";

   // NOTE: The whole request - method and endpoint, then the options or raw command sent
   // with it. A bare "(none)" read as though nothing had been sent at all.
   var sent = String( entry.method ) + " " + String( entry.endpoint );

   if( entry.request )
      sent += "\n" + entry.request;

   pair.appendChild( make_span( "Request", "console-k" ) );
   pair.appendChild( make_span( sent, "console-v" ) );
   pair.appendChild( make_span( "Response", "console-k" ) );
   pair.appendChild( make_span( String( entry.response ), "console-v " + ( entry.ok ? "is-ok" : "is-err" ) ) );

   detail_cell.appendChild( pair );
   detail.appendChild( detail_cell );

   function toggle( )
   {
      detail.hidden = !detail.hidden;

      twist.textContent = detail.hidden ? "›" : "⌄";

      row.setAttribute( "aria-expanded", detail.hidden ? "false" : "true" );
   }

   row.addEventListener( "click", toggle );

   row.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Enter" ) || ( event.key === " " ) )
      {
         event.preventDefault( );

         toggle( );
      }
   } );

   body.appendChild( row );
   body.appendChild( detail );
}

function make_span( text, class_name )
{
   var span = document.createElement( "span" );

   span.className = class_name;
   span.textContent = text;

   return span;
}

// ====================================================================
// Palette
// ====================================================================

function bind_palette( )
{
   document.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "k" || event.key === "K" ) && ( event.ctrlKey || event.metaKey ) )
      {
         if( !document.getElementById( "main_view" ).hidden )
         {
            event.preventDefault( );

            open_palette( );
         }
      }
      else if( ( event.key === "Escape" ) && !document.getElementById( "palette" ).hidden )
      {
         event.preventDefault( );

         close_palette( );
      }
   } );

   var input = document.getElementById( "palette_input" );

   input.addEventListener( "input", function( )
   {
      g_palette_index = 0;

      render_palette( );
   } );

   input.addEventListener( "keydown", function( event )
   {
      var shown = filter_palette( g_palette_items, input.value );

      if( event.key === "ArrowDown" )
      {
         event.preventDefault( );

         g_palette_index = Math.min( g_palette_index + 1, shown.length - 1 );

         render_palette( );
      }
      else if( event.key === "ArrowUp" )
      {
         event.preventDefault( );

         g_palette_index = Math.max( g_palette_index - 1, 0 );

         render_palette( );
      }
      else if( event.key === "Enter" )
      {
         event.preventDefault( );

         if( shown.length > 0 )
            insert_palette_item( shown[ Math.max( 0, g_palette_index ) ] );
      }
   } );

   document.getElementById( "palette" ).addEventListener( "click", function( event )
   {
      if( event.target.id === "palette" )
         close_palette( );
   } );
}

function palette_items( )
{
   var items = c_palette_commands.filter( function( item )
   {
      if( item.admin && !ciyam.is_admin )
         return false;

      if( item.raw && !g_raw_available )
         return false;

      return true;
   } );

   saved_script_names( ).forEach( function( name )
   {
      items.push( { command: "run " + name, description: "saved script" } );
   } );

   return items;
}

function open_palette( )
{
   g_palette_items = palette_items( );
   g_palette_index = 0;

   var input = document.getElementById( "palette_input" );

   input.value = "";

   document.getElementById( "palette" ).hidden = false;

   render_palette( );

   input.focus( );
}

function close_palette( )
{
   document.getElementById( "palette" ).hidden = true;

   document.getElementById( "prompt_input" ).focus( );
}

function render_palette( )
{
   var list = document.getElementById( "palette_list" );

   list.textContent = "";

   var shown = filter_palette( g_palette_items, document.getElementById( "palette_input" ).value );

   if( g_palette_index >= shown.length )
      g_palette_index = shown.length - 1;

   shown.forEach( function( item, index )
   {
      var li = document.createElement( "li" );

      li.setAttribute( "role", "option" );
      li.setAttribute( "aria-selected", ( index === g_palette_index ) ? "true" : "false" );

      li.appendChild( make_span( item.command, "console-palette-cmd" ) );
      li.appendChild( make_span( item.description, "console-palette-desc" ) );

      li.addEventListener( "mousedown", function( event )
      {
         event.preventDefault( );

         insert_palette_item( item );
      } );

      list.appendChild( li );

      if( index === g_palette_index )
         li.scrollIntoView( { block: "nearest" } );
   } );
}

// NOTE: Inserts rather than runs, with the first "<placeholder>" selected so it can be
// typed straight over - a palette entry is a template, not a finished command.
function insert_palette_item( item )
{
   document.getElementById( "palette" ).hidden = true;

   select_tab( "console" );

   var input = document.getElementById( "prompt_input" );

   input.value = item.command;

   input.focus( );

   var place = first_placeholder( item.command );

   if( place !== null )
      input.setSelectionRange( place.start, place.end );
   else
      input.setSelectionRange( item.command.length, item.command.length );
}

// ====================================================================
// Helpers
// ====================================================================

function set_error( id, text )
{
   var node = document.getElementById( id );

   if( node !== null )
      node.textContent = text;
}

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

// NOTE: c_retain_none, c_retain_access and c_retain_full come from "chat_parse.js", loaded first.

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
   { command: "messages delete <room>", description: "leave a room, or decline an invitation (a decline cannot be undone)" },
   { command: "view lists", description: "command lists on the server" },
   { command: "view list <name>", description: "show a command list" },
   { command: "view list ***", description: "your own list on the server" },
   { command: "delete webcmdlist", description: "delete your own list on the server" },
   { command: "view scripts", description: "JavaScripts on the server" },
   { command: "load script <name>", description: "load a server JavaScript and run its _at_load - another account's, admin only" },
   { command: "load script ***", description: "load and run your own JavaScript on the server" },
   { command: "eval script <name> <input>", description: "run a loaded JavaScript's _execute" },
   { command: "result script <name>", description: "a loaded JavaScript's _result, into the output" },
   { command: "unload script <name>", description: "take a JavaScript out of the page" },
   { command: "delete javascript", description: "delete your own JavaScript on the server" },
   { command: "view logs", description: "the server's log files (admin)", admin: true },
   { command: "view log <name>", description: "one whole log - script, server or update (admin)", admin: true },
   { command: "view devices", description: "this account's devices - * after one with an active session" },
   { command: "delete device <ident>", description: "remove one of this account's devices and end its session - not this one" },
   { command: "users review", description: "list accounts (admin)", admin: true },
   { command: "users create secret", description: "issue an account-creation token (admin)", admin: true },
   { command: "users create nominated=<pin>:<username>", description: "reserve a PIN and username (admin)", admin: true },
   { command: "vars", description: "show the variables" },
   { command: "var <name> <text>", description: "set a variable" },
   { command: "history", description: "commands entered this session" },
   { command: "clear", description: "empty the output and clear the screen - in a list too" },
   { command: "remove creds", description: "forget this account's saved PIN and password on this browser" },
   { command: "remove creds <pin>", description: "forget another saved account on this browser (admin)", admin: true },
   { command: "remove creds partial", description: "forget this account's saved password - keep its PIN" },
   { command: "retain creds", description: "save this account's PIN and password on this browser" },
   { command: "retain creds partial", description: "save this account's PIN only - not its password" },
   { command: "~run_script *", description: "list the server scripts (admin, dev)", raw: true },
   // NOTE: Ian's commands for watching the server work (2026-09-27). A trace level stays set, for every
   // session, until it is set back.
   { command: "~trace", description: "the server's trace level now (admin, dev)", raw: true },
   { command: "~trace 70008", description: "trace sessions in detail - stays on until set back (admin, dev)", raw: true },
   { command: "~trace 10000", description: "tracing back to the usual level (admin, dev)", raw: true },
   { command: "~log_tail server", description: "the last 10 lines of the server's log (admin, dev)", raw: true },
   { command: "~log_tail -n=50 server", description: "the last 50 lines - -n=<lines> takes any number (admin, dev)", raw: true },
   { command: "~wait -no_progress <ms> @<word>", description: "the server answers <word> after <ms> - over 5000 times out (admin, dev)", raw: true },
   // NOTE: ntfy (the proof of concept, 2026-10-06) - a topic as a QR code to subscribe a phone, and the server's
   // own commands. "~ntfy_send" takes the message as one value, so it is quoted.
   { command: "ntfy server <url>", description: "the ntfy server as the phones reach it - kept in this browser" },
   { command: "ntfy qr <topic>", description: "a topic as a QR code, to subscribe a phone - the app's link" },
   { command: "ntfy qr <topic> web", description: "a topic as a QR code of ntfy's web page - every phone camera opens it" },
   { command: "ntfy qr", description: "the node's own topic as a QR code (admin, dev)", raw: true },
   { command: "~ntfy_topic <uid>", description: "a user's ntfy topic - with no uid, the node's own (admin, dev)", raw: true },
   { command: "~ntfy_send \"<message>\"", description: "send to the node's own topic (admin, dev)", raw: true },
   { command: "~ntfy_send -uid=<uid> \"<message>\"", description: "send to a user's topic (admin, dev)", raw: true }
];

var g_self = String( Date.now( ) ) + String( Math.floor( Math.random( ) * 1000 ) ).padStart( 3, "0" );

var g_source = "";
var g_owner = "";

// NOTE: The page whose session a linked console shares - "linked_owner( )". The chat unless the
// accounts page opened it.
var g_owner_page = linked_owner( "" );

var g_linked = false;
var g_embedded = false;
var g_connected = false;

var g_announces = 0;
var g_announce_timer = null;

var g_raw_available = null;

var g_server_lists = [ ];
var g_server_javascripts = [ ];

// NOTE: Whether each listing has been read - until it has, the account may have one saved already.
var g_server_lists_known = false;
var g_server_javascripts_known = false;

// NOTE: What the Scripts tab's editor holds - a "list" (the harness's list language, run here) or a
// server "javascript", which is saved but not run from the editor - and whether it is this account's
// own from the server, which saving replaces without asking.
var g_editor_kind = "list";
var g_editor_from_own = false;
var g_replace_armed = 0;

var g_history = [ ];
var g_history_at = 0;
var g_history_draft = "";

var g_log = [ ];

// NOTE: The last output - what "{@1}" reads and "exec" runs. See "append_output( )".
var g_output = "";
var g_log_keys = { };
var g_log_counter = 0;
var g_log_filter = "all";

var g_prefs = parse_prefs( null );

// NOTE: Counts sessions ended in this page, so each log entry can say which it belongs to.
var g_session_seq = 0;

var g_quiet = false;
var g_queue = Promise.resolve( );

var g_linked_request_id = 0;
var g_linked_pending = { };

var g_script_depth = 0;
var g_stop_requested = false;

var g_current_script = "";

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

   g_owner_page = linked_owner( href.searchParams.get( "from" ) || "" );

   if( g_embedded )
      document.getElementById( "console_app" ).classList.add( "is-embedded" );
   else
   {
      // NOTE: Its own tab, which the chat and the accounts page reuse - never in a drawer, whose
      // frame would then be found by the name instead.
      window.name = c_tab_console;
   }

   document.getElementById( "title_host" ).textContent = window.location.host;

   // NOTE: The node's sign in, shared by every app ("signin.js", 2026-10-08) - the console's own ids and words
   // kept, so its suites and its "Open chat instead" carry on. It signs in at once, as it always did.
   signin_build( document.getElementById( "signin_host" ), {
      note: "console",
      ids: { access: "signin_account", submit: "signin_connect" },
      setup_href: "account.html#welcome",
      on_signed_in: enter_console
   } );

   // NOTE: The app switcher - the same in every app, its button the CIYAM mark and "Console" (2026-10-08). Not in a
   // drawer, whose title bar is hidden and whose page has its own - nor listening there for a sign out, which that
   // page answers by closing the drawer (found by review). Shown once signed in, as in the other apps.
   if( !g_embedded )
   {
      apps_build( document.getElementById( "apps_host" ), {
         current: "console",
         self: function( ) { return g_self; },
         sign_out: do_disconnect,
         signed_out: async function( )
         {
            await do_disconnect( true );

            set_error( "signin_error", "You signed out in another app." );
         },
         unsent: unsent_work,
         page_left: on_page_left
      } );
   }

   document.getElementById( "apps_host" ).hidden = true;

   // NOTE: A quiet request is logged too while "Log polling" is ticked.
   install_log_capture( ciyam, "console", function( ) { return g_quiet && !g_prefs.log_polling; }, add_console_entry );

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

   load_prefs( );

   // NOTE: Another tab - the chat, or another console - may change a preference.
   window.addEventListener( "storage", function( )
   {
      load_prefs( );

      render_storage( );
      render_saved_scripts( );
   } );

   g_channel = new BroadcastChannel( c_channel_name );
   g_channel.addEventListener( "message", on_channel_message );

   g_log_channel = new BroadcastChannel( c_log_channel_name );
   g_log_channel.addEventListener( "message", on_log_message );

   var owner_filter = document.getElementById( "filter_chat" );

   owner_filter.hidden = !g_linked;
   owner_filter.dataset.source = g_owner_page.source;
   owner_filter.textContent = g_owner_page.label;

   update_status( );

   if( g_linked )
   {
      document.getElementById( "waiting_text" ).textContent = "Waiting for " + g_owner_page.name + " to share its session…";
      document.getElementById( "waiting_view" ).hidden = false;

      announce( );

      g_announce_timer = window.setInterval( announce, c_announce_interval );
   }
   else if( !g_embedded )
   {
      // NOTE: Switched to from another app in this tab, or reloaded - the session kept for the tab, if the node still
      // knows it (2026-10-09).
      signin_resume( ).then( function( resumed )
      {
         if( resumed )
            enter_console( );
         else
            signin_show( );
      } );
   }
   else
      signin_show( );
}

// NOTE: What leaving for another app in this tab would lose - a script still running. The switcher asks first.
function unsent_work( )
{
   if( !g_connected )
      return "";

   return document.getElementById( "script_stop" ).hidden ? "" : "A script is still running.";
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

      document.getElementById( "waiting_text" ).textContent = g_owner_page.subject
       + " did not share a session. Is it signed in? Reload this console to try again.";

      return;
   }

   g_channel.postMessage( g_source + ":" + g_self );
}

function on_channel_message( event )
{
   var message = parse_channel_message( event.data );

   if( message === null )
      return;

   // NOTE: An app opened from this console's switcher asks for the session - it is handed over, as the other
   // apps hand theirs (2026-10-08).
   if( ( message.kind === "announce" ) && ( message.id === g_self ) && g_connected && ( ciyam.sessid !== "" ) )
   {
      g_channel.postMessage( message.rest + "-" + g_self );

      g_channel.postMessage( message.rest + "=" + ciyam.access + "," + ciyam.device + "," + ciyam.hashed + ","
       + ciyam.sessid + "," + ciyam.unique + "," + encode_channel_field( ciyam.username ) + "," + ( ciyam.is_admin ? "1" : "0" ) );

      return;
   }

   if( !g_linked )
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

// NOTE: How long a page that left has to come back - a chat reloaded keeps its id and announces itself again once it
// has taken up its session - before this console stops waiting for it.
const c_owner_return_ms = 3000;

// NOTE: The page this console is linked to has left - switched to another app in its tab, or closed (found by review,
// 2026-10-09). Its requests went through that page, so they would wait in vain: unless it comes back, the console
// carries on by itself, on the same session, which is still the node's.
function on_page_left( page )
{
   if( !g_linked || !g_connected || ( ( page !== g_source ) && ( page !== g_owner ) ) )
      return;

   window.setTimeout( function( )
   {
      if( g_linked && g_connected && !g_apps_open_pages[ g_source ] && !g_apps_open_pages[ g_owner ] )
         carry_on_alone( );
   }, c_owner_return_ms );
}

function carry_on_alone( )
{
   var owner = g_owner_page.subject;

   g_linked = false;
   g_source = "";
   g_owner = "";

   // NOTE: Anything still waiting on the page that left is answered now, as no answer, rather than after its time-out.
   Object.keys( g_linked_pending ).forEach( function( id ) { g_linked_pending[ id ]( null ); } );

   forget_source( );

   document.getElementById( "filter_chat" ).hidden = true;

   signin_keep_session( );

   print_line( owner + " has gone - this console carries on by itself, on the same session.", "is-dim" );

   update_title( );
   update_status( );
}

function end_linked_session( )
{
   g_connected = false;

   if( !g_embedded )
      signin_forget_session( );

   ciyam.sessid = "";
   ciyam.unique = "";
   ciyam.hashed = "";

   print_line( g_owner_page.subject + "'s session ended. This console is no longer connected.", "is-warn" );

   end_log_session( );

   unload_server_scripts( );

   // NOTE: As the chat and the accounts page do - so a reload starts a console of its own rather
   // than waiting for a session that has gone.
   forget_source( );

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

// NOTE: Linked or not - the session is shared, not owned, so this console may end it too (2026-10-08); every other
// tab on it has been told by the switcher. Linked, the request goes through the page that shares the session, as
// all its requests do (ISS-020), and it then becomes a console of its own. "already_ended" - another tab signed
// out of every app - asks nothing of the node: the session has gone (found by review).
async function do_disconnect( already_ended )
{
   g_stop_requested = true;

   if( !g_embedded )
      signin_forget_session( );

   if( already_ended !== true )
   {
      if( g_linked )
         await send_request( "DELETE", ciyam.get_cws_url( ) + "/sessions/" + ciyam.sessid + "?access=" + ciyam.access
          + "&device=" + ciyam.device + "&format=" + ciyam.format_type );
      else
         await ciyam.disconnect( function( ) { } );
   }

   ciyam.sessid = "";
   ciyam.unique = "";

   if( g_linked )
   {
      g_linked = false;
      g_source = "";

      forget_source( );

      document.getElementById( "filter_chat" ).hidden = true;
   }

   g_connected = false;
   g_raw_available = null;
   g_server_lists = [ ];
   g_server_javascripts = [ ];

   g_output = "";

   ciyam.remove_all_variables( );

   end_log_session( );

   unload_server_scripts( );

   reset_script_editor( );

   document.getElementById( "main_view" ).hidden = true;
   document.getElementById( "scrollback" ).textContent = "";

   update_title( );
   update_status( );

   document.getElementById( "apps_host" ).hidden = true;

   signin_show( );
}

// ====================================================================
// The console proper
// ====================================================================

function enter_console( )
{
   g_connected = true;

   if( !g_embedded )
      signin_keep_session( );

   document.getElementById( "signin_view" ).hidden = true;
   document.getElementById( "waiting_view" ).hidden = true;
   document.getElementById( "main_view" ).hidden = false;

   var prompt = document.getElementById( "prompt_input" );

   prompt.disabled = false;

   document.getElementById( "apps_host" ).hidden = g_embedded;

   apps_refresh( );

   update_title( );
   update_status( );

   print_line( "CIYAM console · " + ( g_linked ? "linked to " + g_owner_page.name : "standalone" ) + " · signed in as "
    + ( ciyam.username || ciyam.access ) + ( ciyam.is_admin ? " [adm]" : " [std]" ), "is-dim" );

   print_line( "Type help for the commands, or press ctrl+k for the palette.", "is-dim" );

   render_saved_scripts( );
   render_vars( );
   render_storage( );
   render_log( );

   reset_script_editor( );

   probe_raw( );
   load_server_lists( );
   load_server_javascripts( );

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
   document.getElementById( "status_mode" ).textContent = g_linked ? "linked to " + g_owner_page.label.toLowerCase( ) : "standalone";
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
   // NOTE: A list's own lines are not printed unless asked - only what they answer, as in the harness
   // ("echo_list_lines", the Scripts tab's "Show each line"; Ian, 2026-10-07).
   if( !from_script || g_prefs.echo_list_lines )
      print_command( line );

   // NOTE: "{name}" substitution is Ian's, from "ciyam.js", so a script written for the
   // harness runs unchanged here - including "." at the start to switch it off, and
   // "{@1}", "{@}" for the lines of the last output.
   var text = ciyam.replace_variables( String( line ).trim( ), g_output );

   // NOTE: "@" makes the line out of variables - "@{cmd}" runs the command "cmd" holds,
   // substituted again so the variables inside it are filled in too.
   if( text.charAt( 0 ) === "@" )
      text = ciyam.replace_variables( text.substring( 1 ), g_output );

   var guard = apply_line_guard( text );

   if( !guard.run )
   {
      // NOTE: Only beside a line that was shown - unshown, a list's skipped lines print nothing, as in the harness (Ian, 2026-10-08).
      if( !from_script || g_prefs.echo_list_lines )
         print_line( "(skipped)", "is-dim" );

      return { ok: true };
   }

   text = guard.text;

   if( is_javascript_line( text ) )
   {
      var script = parse_script_line( text );

      if( ( script === null ) || ( script.kind === "error" ) )
      {
         print_line( "Error: " + ( script ? script.message : "Not a server javascript line" ) + ".", "is-err" );

         return { ok: false };
      }

      return run_script_line( script, from_script );
   }

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
      // NOTE: In a drawer the console is part of the page around it, so signing out is that page's switcher's. In
      // its own tab it signs out of every app, as its switcher does (2026-10-08).
      if( g_embedded )
      {
         print_line( "Error: This console is part of " + g_owner_page.name + " - sign out from " + g_owner_page.name + "'s switcher.", "is-err" );

         return { ok: false };
      }

      await apps_sign_out_everywhere( );

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

   // NOTE: A response replaces the output, as it replaces the harness's text box.
   g_output = response.replace( /\s+$/, "" );

   if( ( spec.kind === "raw" ) && ( response.trim( ) === "[bad]" ) )
      print_line( "Raw protocol needs the admin PIN on a development system, and a command starting a-z.", "is-dim" );

   return { ok: !is_error_output( response ) };
}

// ====================================================================
// Server javascripts
// ====================================================================

// NOTE: Ian's server javascripts - "ciyam_<name>.js" - run in this page, as the harness runs them,
// with this account's session. Anyone runs those not named after a PIN, and their own; another
// account's is admin's alone (Ian, 2026-10-05 - "script_allowed( )"). A fingerprint against tampering
// is to come.
//
// The scripts read two page globals, as they do in the harness. "init_script_value" is the input
// given with "load script <name> <input>", which "_at_load" passes on to "_execute";
// "include_script_usage_hints" adds a usage line to what a script answers - when typed, not when
// run from a list.
var init_script_value = null;
var include_script_usage_hints = false;

// NOTE: The harness's figures - fifty milliseconds between looks for "_at_load", a hundred looks.
const c_script_load_attempts = 100;
const c_script_load_delay = 50;

// NOTE: How long a line waits for a script to answer before the next one runs. One that takes
// longer - "exec script harden 33333:30" - still answers into the output when it finishes, and a
// list waits for it with "wait <variable>", as in the harness.
const c_script_answer_wait = 2000;

// NOTE: "wait <variable>" - the harness's hundred looks, a tenth of a second apart.
const c_wait_variable_repeats = 100;
const c_wait_variable_delay = 100;

var g_script_libraries = null;
var g_loaded_scripts = { };

// NOTE: Counts the sessions whose scripts have been unloaded, so a script still working when its
// session ended - "exec script harden 33333:30" - cannot answer into the next one.
var g_script_generation = 0;

function add_page_script( src, id )
{
   return new Promise( function( resolve )
   {
      var script = document.createElement( "script" );

      script.src = src;

      if( id )
         script.id = id;

      script.onload = function( ) { resolve( true ); };
      script.onerror = function( ) { resolve( false ); };

      document.head.appendChild( script );
   } );
}

// NOTE: What the scripts use besides "sha2.min.js" - "BIP39" and "QRCode". Loaded on first use
// only, since "bip39.min.js" is nearly 700 KB; both are in "webui/" with everything else.
//
// Resolves false if either did not load, and forgets the attempt so the next "load script" tries again.
async function load_script_libraries( )
{
   if( g_script_libraries === null )
      g_script_libraries = Promise.all( [ add_page_script( "bip39.min.js" ), add_page_script( "qrcode.min.js" ) ] );

   var loaded = await g_script_libraries;

   if( loaded.indexOf( false ) < 0 )
      return true;

   g_script_libraries = null;

   return false;
}

// NOTE: The harness's progress functions, by the names its scripts call - "ciyam_harden.js" shows
// how far through its rounds it is.
function show_progress( )
{
   document.getElementById( "progress_bar" ).style.width = "0%";
   document.getElementById( "progress_track" ).hidden = false;
}

function hide_progress( )
{
   document.getElementById( "progress_track" ).hidden = true;
}

function update_progress( fraction )
{
   document.getElementById( "progress_bar" ).style.width = ( Math.min( Math.max( Number( fraction ) || 0, 0 ), 1 ) * 100 ) + "%";
}

// NOTE: "ciyam_rpc_unlock.js" draws a QR code into "test_image", as in the harness; the panel
// shows while there is one.
function refresh_qr( )
{
   var panel = document.getElementById( "script_qr" );

   panel.hidden = ( document.getElementById( "test_image" ).childElementCount === 0 );

   // NOTE: The panel takes room from the scrollback, so what the script printed is brought back into view.
   if( !panel.hidden )
      scroll_to_end( );
}

function do_hide_qr( )
{
   document.getElementById( "test_image" ).textContent = "";

   refresh_qr( );
}

// ====================================================================
// ntfy - subscribing a phone (the ntfy proof of concept, 2026-10-06)
// ====================================================================

var g_qr_library = null;

// NOTE: Just "qrcode.min.js" - "load_script_libraries( )" brings "bip39.min.js" too, nearly 700 KB a QR
// code does not need.
async function load_qr_library( )
{
   if( typeof QRCode === "function" )
      return true;

   if( g_qr_library === null )
      g_qr_library = add_page_script( "qrcode.min.js" );

   var loaded = await g_qr_library;

   if( !loaded )
      g_qr_library = null;

   return loaded;
}

function saved_ntfy_server( )
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

// NOTE: "ntfy server [<url>]" and "ntfy qr [<topic>]" ("parse_ntfy_command( )"). The QR code is the apps'
// own subscribe link, drawn here - the console sends nothing to ntfy. With no topic, the node's own, which
// only raw protocol can ask the server for ("~ntfy_topic").
async function run_ntfy( args )
{
   var command = parse_ntfy_command( args );

   if( command.verb === "error" )
   {
      print_line( "Error: " + command.message, "is-err" );

      return { ok: false };
   }

   if( command.verb === "server" )
   {
      if( command.server === "" )
      {
         var current = saved_ntfy_server( );

         print_line( current ? "The phones reach ntfy at " + current + "." : "No ntfy server set - ntfy server http://<address>:<port>", "is-dim" );

         g_output = current;

         return { ok: true };
      }

      try
      {
         localStorage.setItem( c_ntfy_server_key, command.server );
      }
      catch( e )
      {
         print_line( "Error: This browser would not keep it.", "is-err" );

         return { ok: false };
      }

      print_line( "The phones reach ntfy at " + command.server + " - kept in this browser.", "is-dim" );

      g_output = command.server;

      return { ok: true };
   }

   var server = saved_ntfy_server( );

   if( server === "" )
   {
      print_line( "Error: Set the server the phones use first - ntfy server http://<address>:<port>", "is-err" );

      return { ok: false };
   }

   var topic = command.topic;

   if( topic === "" )
   {
      if( !g_connected || !g_raw_available )
      {
         print_line( "Error: Name the topic - ntfy qr <topic>. The node's own needs raw protocol: admin on a development system.", "is-err" );

         return { ok: false };
      }

      var response = await send_request( "GET", build_cws_url( ciyam.get_cws_url( ), { path: "", request: "ntfy_topic" }, session_info( ) ), true );

      topic = ( response === null ) ? "" : response.trim( );

      if( !is_ntfy_topic( topic ) )
      {
         print_line( "Error: The server gave no topic" + ( response ? " - " + response.trim( ) : "." ), "is-err" );

         return { ok: false };
      }
   }

   if( !await load_qr_library( ) )
   {
      print_line( "Error: qrcode.min.js did not load - try again.", "is-err" );

      return { ok: false };
   }

   // NOTE: The apps' own link by default - a GrapheneOS camera's Scan mode can open it; with "web", ntfy's page
   // for the topic, which every camera opens (the iPhone's showed "No usable data" for the other, 2026-10-06).
   var app_link = ntfy_subscribe_link( server, topic );
   var web_link = ntfy_web_link( server, topic );

   var holder = document.getElementById( "test_image" );

   holder.textContent = "";

   new QRCode( holder, { text: command.web ? web_link : app_link, width: 220, height: 220 } );

   refresh_qr( );

   print_output( [ "Subscribe on a phone - scan the code with its camera, or add these in the ntfy app:",
    "  server    " + server, "  topic     " + topic,
    "  app link  " + app_link + ( command.web ? "" : "   (in the code)" ),
    "  web page  " + web_link + ( command.web ? "   (in the code)"
     : "   - ntfy qr " + ( command.topic ? command.topic + " " : "" ) + "web for this one" ) ].join( "\n" ) );

   g_output = topic;

   return { ok: true };
}

function unload_server_script( prefix )
{
   var old = document.getElementById( prefix );

   delete g_loaded_scripts[ prefix ];

   if( old === null )
      return false;

   old.remove( );

   // NOTE: As the harness cleans up - a script's "var"s cannot be deleted, so they are emptied too.
   [ "_result", "_at_load", "_execute" ].forEach( function( suffix )
   {
      try
      {
         delete window[ prefix + suffix ];
      }
      catch( e )
      {
      }

      window[ prefix + suffix ] = undefined;
   } );

   // NOTE: And so is every other value the script keeps - "ciyam_harden_string_to_hash" holds what
   // it was given. Its functions are left, so a loop still running ends quietly rather than failing.
   Object.keys( window ).forEach( function( name )
   {
      if( ( name.indexOf( prefix + "_" ) === 0 ) && ( typeof window[ name ] !== "function" ) )
         window[ name ] = undefined;
   } );

   return true;
}

// NOTE: When the session ends - what a script worked out, an unlock key's hashes say, must not
// outlast the account that ran it.
function unload_server_scripts( )
{
   Object.keys( g_loaded_scripts ).forEach( unload_server_script );

   ++g_script_generation;

   init_script_value = null;

   hide_progress( );
   do_hide_qr( );
}

// NOTE: What a script answers, through the callback it is handed, goes into the output as a
// server's response does - so "{@1}" reads it on the next line.
function script_answered( value )
{
   var text = ( ( value === null ) || ( value === undefined ) ) ? "" : String( value );

   print_output( text );

   g_output = text.replace( /\s+$/, "" );

   refresh_qr( );
}

// NOTE: Calls a script's function with a callback, as the harness does, and the input as data -
// never spliced into code to be evaluated. Resolves when the script answers, or after
// "c_script_answer_wait" if it is still working.
function call_script( prefix, fn, args )
{
   var generation = g_script_generation;

   return new Promise( function( resolve )
   {
      var timer = window.setTimeout( function( ) { resolve( { ok: true } ); }, c_script_answer_wait );

      var failed = function( e )
      {
         window.clearTimeout( timer );

         if( generation === g_script_generation )
            print_line( "Error: " + prefix + ".js failed - " + ( ( e && e.message ) ? e.message : String( e ) ), "is-err" );

         resolve( { ok: false } );
      };

      var callback = function( value )
      {
         // NOTE: From a session that has since ended - see "g_script_generation".
         if( generation !== g_script_generation )
            return;

         script_answered( value );

         window.clearTimeout( timer );

         resolve( { ok: true } );
      };

      try
      {
         var result = fn.apply( window, [ callback ].concat( args ) );

         if( result && ( typeof result.catch === "function" ) )
            result.catch( failed );
      }
      catch( e )
      {
         failed( e );
      }
   } );
}

async function find_script_function( name )
{
   for( var i = 0; i < c_script_load_attempts; i++ )
   {
      if( typeof window[ name ] === "function" )
         return window[ name ];

      await delay( c_script_load_delay );
   }

   return null;
}

function refuse_unless_allowed( name )
{
   if( script_allowed( name, ciyam.access, ciyam.is_admin ) )
      return false;

   print_line( "Error: The javascript '" + name + "' is another account's - only admin runs it.", "is-err" );

   return true;
}

async function run_script_line( spec, from_script )
{
   if( !g_connected )
   {
      print_line( "Error: Not connected.", "is-err" );

      return { ok: false };
   }

   if( refuse_unless_allowed( spec.name ) )
      return { ok: false };

   var name = ( spec.name === c_console_own_name ) ? ciyam.access : spec.name;

   var prefix = "ciyam_" + name;

   include_script_usage_hints = !from_script;

   switch( spec.verb )
   {
      case "load":
      {
         if( !await load_script_libraries( ) )
         {
            print_line( "Error: bip39.min.js or qrcode.min.js did not load - the scripts need them. Try again.", "is-err" );

            return { ok: false };
         }

         unload_server_script( prefix );

         init_script_value = spec.arg;

         if( !await add_page_script( prefix + ".js?" + Date.now( ), prefix ) )
         {
            print_line( "Error: There is no server javascript called '" + name + "' - " + prefix + ".js did not load.", "is-err" );

            return { ok: false };
         }

         g_loaded_scripts[ prefix ] = true;

         var at_load = await find_script_function( prefix + "_at_load" );

         if( at_load === null )
         {
            print_line( "Error: " + prefix + ".js has no " + prefix + "_at_load( ) - (failed to load script).", "is-err" );

            return { ok: false };
         }

         return call_script( prefix, at_load, [ ] );
      }

      case "eval":
      {
         var execute = window[ prefix + "_execute" ];

         if( typeof execute !== "function" )
         {
            print_line( "Error: Load it first - load script " + spec.name + ".", "is-err" );

            return { ok: false };
         }

         return call_script( prefix, execute, ( spec.arg === null ) ? [ ] : [ spec.arg ] );
      }

      case "result":
      {
         var result = window[ prefix + "_result" ];

         if( ( result === null ) || ( result === undefined ) )
            print_line( "(no result)", "is-dim" );
         else
            script_answered( result );

         return { ok: true };
      }

      case "unload":
         print_line( unload_server_script( prefix ) ? "(" + name + " unloaded)" : "(" + name + " was not loaded)", "is-dim" );

         return { ok: true };
   }

   return { ok: false };
}

// NOTE: "wait <variable>" - until a server javascript sets the global to something other than
// nothing, empty or false, as in the harness. A stop asked for ends the wait.
async function wait_for_global( name )
{
   if( !is_global_name( name ) )
   {
      print_line( "Error: wait takes milliseconds, or the name of a global a server javascript sets.", "is-err" );

      return { ok: false };
   }

   for( var i = 0; i < c_wait_variable_repeats; i++ )
   {
      var value = window[ name ];

      if( ( value !== null ) && ( value !== undefined ) && ( value !== "" ) && ( value !== false ) )
         return { ok: true };

      if( g_stop_requested )
         return { ok: false };

      await delay( c_wait_variable_delay );
   }

   print_line( "(timed out waiting for '" + name + "')", "is-err" );

   return { ok: false };
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

      case "creds":
         return run_creds( spec.creds );

      // NOTE: "clear" empties the output and clears the screen - in a list too, as in the harness,
      // where the output box is what is shown. Ian's demo lists clear to take hashes and a password
      // off the screen (Ian, 2026-10-04).
      case "clear":
         g_output = "";

         document.getElementById( "scrollback" ).textContent = "";

         return { ok: true };

      case "vars":
      {
         var all = ciyam.get_all_variables( );

         print_output( all === "" ? "(no variables)" : all );

         g_output = all;

         return { ok: true };
      }

      case "var":
         return run_var( args );

      case "unset":
         if( ciyam.has_variable( args ) )
         {
            ciyam.remove_variable( args );

            render_vars( );
         }

         return { ok: true };

      case "echo":
         print_output( args );

         g_output = append_output( g_output, args );

         return { ok: true };

      // NOTE: Eleven characters unless told otherwise, as in the harness.
      case "seed":
      {
         var count = parseInt( args, 10 );

         var seed = CIYAM.generate_base64_key( ( count > 0 && count <= 256 ) ? count : 11 );

         print_output( seed );

         g_output = append_output( g_output, seed );

         return { ok: true };
      }

      case "ntfy":
         return run_ntfy( args );

      case "history":
         if( g_history.length === 0 )
            print_line( "(no history)", "is-dim" );
         else
            print_output( g_history.map( function( h, i ) { return String( i + 1 ).padStart( 3, " " ) + "  " + h; } ).join( "\n" ) );

         return { ok: true };

      // NOTE: "wait <variable>" waits for a server javascript to set a global - "wait_for_global( )".
      case "wait":
      {
         if( /^[A-Za-z_$]/.test( args ) )
            return wait_for_global( args );

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

      // NOTE: Runs the output as a list - the harness's way to run a server list is
      // "view list <name>" and then "exec". The output is emptied first, as there.
      case "exec":
      {
         if( g_output.trim( ) === "" )
         {
            print_line( "Error: Nothing to run - the output is empty. Try view list <name> first.", "is-err" );

            return { ok: false };
         }

         var list = g_output;

         g_output = "";

         return run_script_body( list, "the output" );
      }
   }

   return { ok: false };
}

// NOTE: The forms are parsed by "parse_var_command( )". Showing a variable, or part of it,
// also adds it to the output - that is how a list passes a value on through "{@1}".
function run_var( args )
{
   var command = parse_var_command( args );

   switch( command.kind )
   {
      case "error":
         print_line( "Error: " + command.message + ".", "is-err" );

         return { ok: false };

      case "show":
         if( !ciyam.has_variable( command.name ) )
            print_line( "(not set)", "is-dim" );
         else
         {
            var value = ciyam.get_variable( command.name );

            print_output( value );

            g_output = append_output( g_output, value );
         }

         return { ok: true };

      case "substr":
         if( ciyam.has_variable( command.name ) )
         {
            var part = substr_of( ciyam.get_variable( command.name ), command.start, command.length );

            print_output( part );

            g_output = append_output( g_output, part );
         }

         return { ok: true };

      // NOTE: "var @<name> <global>" - from a global a server javascript set, as in the harness. One
      // not set, or empty, leaves the variable unset.
      case "from_script":
      {
         if( !is_valid_variable_name( command.name ) || !is_global_name( command.source ) )
         {
            print_line( "Error: Usage is var @<name> <global> - a variable's name, then the global a script sets.", "is-err" );

            return { ok: false };
         }

         if( !command.only_if_unset || !ciyam.has_variable( command.name ) )
         {
            var global = window[ command.source ];

            global = ( ( global === null ) || ( global === undefined ) ) ? "" : String( global );

            if( global !== "" )
               ciyam.set_variable( command.name, global );
            else if( ciyam.has_variable( command.name ) )
               ciyam.remove_variable( command.name );
         }

         render_vars( );

         return { ok: true };
      }

      case "remove":
         if( ciyam.has_variable( command.name ) )
            ciyam.remove_variable( command.name );

         render_vars( );

         return { ok: true };

      case "set":
         if( !command.only_if_unset || !ciyam.has_variable( command.name ) )
            ciyam.set_variable( command.name, command.value );

         render_vars( );

         return { ok: true };
   }

   return { ok: false };
}

// NOTE: "remove creds" and "retain creds", as in the harness - they change what this browser
// has saved, never the account. "remove creds <pin>" forgets one other account without the
// reset that forgets them all. The sign in list is rebuilt when signing out, so it shows
// the change then. See "plan_creds_removal( )".
function run_creds( creds )
{
   var keys = [ ];

   try
   {
      for( var i = 0; i < localStorage.length; i++ )
         keys.push( localStorage.key( i ) );
   }
   catch( e )
   {
   }

   var list = stored( c_storage_access );

   var plan = ( creds.verb === "remove" )
    ? plan_creds_removal( keys, list, creds.pin || ciyam.access, creds.partial )
    : plan_creds_retain( list, ciyam.access, ciyam.hashed || "", creds.partial );

   if( plan.error )
   {
      print_line( plan.error, "is-err" );

      return { ok: false };
   }

   try
   {
      if( plan.list === null )
         localStorage.removeItem( c_storage_access );
      else if( plan.list !== undefined )
         localStorage.setItem( c_storage_access, plan.list );

      plan.remove.forEach( function( key ) { localStorage.removeItem( key ); } );

      Object.keys( plan.set ).forEach( function( key ) { localStorage.setItem( key, plan.set[ key ] ); } );
   }
   catch( e )
   {
      print_line( "Error: This browser would not change its saved data.", "is-err" );

      return { ok: false };
   }

   print_line( plan.message, "is-dim" );

   return { ok: true };
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
    "  var !<name> <text>           set it only if it is not set",
    "  var #<name> substr:<a>[,<n>] part of it, into the output",
    "  var @<name> null             remove a variable - or unset <name>",
    "  unset <name>                 remove a variable",
    "  vars                         list the variables",
    "  echo <text>                  print text, after substitution",
    "  seed [<count>]               print random characters",
    "  wait <ms>                    pause - useful in scripts",
    "  run <script>                 run a saved script",
    "  exec                         run the output as a list - after view list <name>",
    "  history                      commands entered this session",
    "  ntfy server [<url>]          the ntfy server as the phones reach it - kept in this browser",
    "  ntfy qr [<topic>] [web]      a topic as a QR code, to subscribe a phone - the node's own if none; web: ntfy's page",
    "  clear                        empty the output and the scrollback",
    // NOTE: As the harness, naming another account's PIN is offered to admin only.
    ciyam.is_admin
     ? "  remove creds [<pin>]         forget a saved account here - partial keeps the PIN"
     : "  remove creds                 forget this account here - partial keeps the PIN",
    "  retain creds [partial]       save this account here - partial leaves out the password",
    "",
    "In a line - the harness's list language",
    "  {name}  {@1}  {@}            a variable; a line of the last output; all of it",
    "  .<line>                      no substitution",
    "  ?{name} <line>               run only if name is set; !{name} only if not",
    "  @{name}                      run the command a variable holds"
   ];

   // NOTE: With the console's commands, ahead of the blank line before the list language. The session is shared,
   // so quitting signs out of every app, linked or not (2026-10-08) - but not from a drawer, which is its page's.
   if( !g_embedded )
      lines.splice( lines.indexOf( "" ), 0, "  quit                         sign out of every app" );

   lines.push( "" );
   lines.push( ciyam.is_admin ? "Server javascripts - as in the harness; admin runs any"
    : "Server javascripts - as in the harness; any not named after a PIN, and your own (***)" );
   lines.push( "  load script <name> [<input>] load ciyam_<name>.js; its _at_load gets the input" );
   lines.push( "  eval script <name> [<input>] run its _execute - exec and employ too" );
   lines.push( "  result script [<name>]       its _result, into the output" );
   lines.push( "  unload script [<name>]       take it out of the page" );
   lines.push( "  wait <global>                wait for a script to set a global" );
   lines.push( "  var @<name> <global>         set a variable from a script's global" );

   // NOTE: Raw protocol - only where it works, admin on a development system.
   if( g_raw_available )
   {
      lines.push( "" );
      lines.push( "Tracing - raw protocol, admin on a development system" );
      lines.push( "  ~trace                       the server's trace level now - 10000 is the usual" );
      lines.push( "  ~trace 70008                 trace sessions in detail; stays on, for everyone, until set back" );
      lines.push( "  ~log_tail [-n=<lines>] server  the last lines of the server's log, 10 unless -n says - script and update too" );
      lines.push( "  ~wait -no_progress <ms> @<word>  the server answers <word> after <ms>; over 5000 times out" );
      lines.push( "" );
      lines.push( "Notifications - ntfy, raw protocol" );
      lines.push( "  ~ntfy_topic [<uid>]          a topic - the node's own with no uid; admin has none" );
      lines.push( "  ~ntfy_send [-uid=<uid>] \"<message>\"  send to it - the message quoted, or it is several words" );
   }

   lines.push( "" );
   lines.push( "Keys: up and down recall history, ctrl+k opens the palette." );

   print_output( lines.join( "\n" ) );
}

// ====================================================================
// Raw protocol probe and server scripts
// ====================================================================

// NOTE: Raw protocol needs the admin PIN on a development system. Probing once, quietly,
// means the "~" affordance is only offered when it will work - and "run_script *" never
// executes anything. What it lists is not shown: many of those scripts are only for the server
// itself to run (Ian, 2026-10-04), and "~run_script *" lists them at the prompt.
async function probe_raw( )
{
   if( !ciyam.is_admin )
   {
      g_raw_available = false;

      update_status( );

      return;
   }

   var url = build_cws_url( ciyam.get_cws_url( ), { path: "", request: c_raw_probe }, session_info( ) );

   var response = await send_request( "GET", url, true );

   g_raw_available = ( response !== null ) && ( response.trim( ) !== "[bad]" ) && ( response.indexOf( "Error: " ) !== 0 );

   update_status( );
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

// NOTE: A list runs here and can be saved here or on the server; a javascript is only saved on the
// server - it runs with "load script".
function show_script_editor( kind )
{
   g_editor_kind = kind || "list";

   var is_list = ( g_editor_kind === "list" );

   document.getElementById( "script_editor" ).hidden = false;
   document.getElementById( "script_origin" ).hidden = true;

   document.getElementById( "script_note_list" ).hidden = !is_list;
   document.getElementById( "script_note_javascript" ).hidden = is_list;

   document.getElementById( "script_run" ).hidden = !is_list;
   document.getElementById( "echo_list_lines_row" ).hidden = !is_list;
   document.getElementById( "script_save" ).hidden = !is_list;

   document.getElementById( "script_name" ).readOnly = !is_list;
   document.getElementById( "script_name_label" ).textContent = is_list ? "Name, for Save here" : "Opened from";

   if( !is_list )
      document.getElementById( "script_delete" ).hidden = true;

   // NOTE: Set again by whatever opens this account's own "***" - see "do_save_to_server( )".
   g_editor_from_own = false;

   disarm_replace( );

   document.getElementById( "script_server_hint" ).textContent = is_list
    ? "On the server each account keeps one list - ciyam_" + ciyam.access + ".list, shown as ***. "
     + "The name above is not kept, and saving replaces the list saved before."
    : "On the server each account keeps one JavaScript - ciyam_" + ciyam.access + ".js, shown as ***. Saving replaces the one saved before.";

   render_saved_scripts( );
}

// NOTE: Whenever a session starts or ends - what the editor held, and whose it was, belongs to the
// account before. Left, the next account's one click could save it as their own (found by review).
function reset_script_editor( )
{
   g_current_script = "";

   show_script_editor( "list" );

   document.getElementById( "script_name" ).value = "";
   document.getElementById( "script_body" ).value = "";
   document.getElementById( "script_delete" ).hidden = true;

   mark_current_item( null );

   set_error( "script_error", "" );
}

function server_save_label( )
{
   return ( g_editor_kind === "javascript" ) ? "Save as my server JavaScript" : "Save as my server list";
}

function disarm_replace( )
{
   g_replace_armed = 0;

   document.getElementById( "script_save_server" ).textContent = server_save_label( );
}

// NOTE: The server lists this account's own list or javascript as "***".
function server_item_label( name )
{
   return ( name === c_console_own_name ) ? c_console_own_name + " (yours)" : name;
}

function server_item_path( prefix, name )
{
   return prefix + ( ( name === c_console_own_name ) ? ciyam.access : encodeURIComponent( name ) );
}

// NOTE: "retain webcmdlist" or "retain javascript" - one of each per account, which the server names
// after the PIN, so it replaces only this account's own (Ian, 2026-10-04: a button, not a command).
// Replacing one already there takes a second click, unless it is that one being edited.
async function do_save_to_server( )
{
   var body = document.getElementById( "script_body" ).value;

   if( body.trim( ) === "" )
   {
      set_error( "script_error", "There is nothing to save." );

      return;
   }

   var is_javascript = ( g_editor_kind === "javascript" );

   var button = document.getElementById( "script_save_server" );

   // NOTE: Not knowing - the listing still loading, or failed - is treated as having one, so it asks (found by review).
   var known = is_javascript ? g_server_javascripts_known : g_server_lists_known;

   var has_own = !known || ( ( is_javascript ? g_server_javascripts : g_server_lists ).indexOf( c_console_own_name ) >= 0 );

   if( has_own && !g_editor_from_own && ( ( g_replace_armed === 0 ) || ( Date.now( ) - g_replace_armed > c_delete_confirm_ms ) ) )
   {
      g_replace_armed = Date.now( );

      button.textContent = is_javascript ? "Replace my server JavaScript?" : "Replace my server list?";

      window.setTimeout( function( )
      {
         if( ( g_replace_armed !== 0 ) && ( Date.now( ) - g_replace_armed >= c_delete_confirm_ms ) )
            disarm_replace( );
      }, c_delete_confirm_ms );

      return;
   }

   disarm_replace( );

   button.disabled = true;

   var response = await send_request( "PUT", build_cws_url( ciyam.get_cws_url( ),
    { path: is_javascript ? "/javascripts" : "/webcmdlists", payload: body }, session_info( ) ), false );

   button.disabled = false;

   if( ( response === null ) || is_error_output( response ) )
   {
      set_error( "script_error", ( response === null ) ? "The server did not answer - nothing was saved." : response.trim( ) );

      return;
   }

   set_error( "script_error", "" );

   // NOTE: The editor now holds this account's own, so saving it again needs no second click.
   g_editor_from_own = true;

   var origin = document.getElementById( "script_origin" );

   origin.textContent = is_javascript
    ? "Saved on the server as this account's own, ciyam_" + ciyam.access + ".js - load script *** runs it."
    : "Saved on the server as this account's own list, ciyam_" + ciyam.access + ".list - listed as ***.";
   origin.hidden = false;

   if( is_javascript )
      load_server_javascripts( );
   else
      load_server_lists( );
}

// ====================================================================
// Lists on the server - Ian's ".list" files
// ====================================================================

async function load_server_lists( )
{
   g_server_lists = [ ];
   g_server_lists_known = false;

   render_server_lists( null );

   var response = await send_request( "GET",
    build_cws_url( ciyam.get_cws_url( ), { path: "/webcmdlists" }, session_info( ) ), true );

   g_server_lists = parse_name_list( response );
   g_server_lists_known = ( response !== null ) && !is_error_output( response );

   render_server_lists( response );
}

function render_server_lists( response )
{
   var holder = document.getElementById( "server_lists" );
   var note = document.getElementById( "server_lists_note" );

   holder.textContent = "";

   if( response === null )
      note.textContent = g_connected ? "Checking…" : "";
   else if( g_server_lists.length === 0 )
      note.textContent = ( String( response ).indexOf( "Error: " ) === 0 ) ? response.trim( ) : "No lists on this server.";
   else
      note.textContent = g_server_lists.length + " from view lists";

   g_server_lists.forEach( function( name )
   {
      var button = document.createElement( "button" );

      button.type = "button";
      button.className = "console-item";
      button.textContent = server_item_label( name );

      button.addEventListener( "click", function( ) { open_server_list( name, button ); } );

      holder.appendChild( button );
   } );
}

// NOTE: Opens a copy in the editor. It is not saved anywhere until Save here - in this browser,
// under whatever name is in the box - or Save to server, as this account's own list.
async function open_server_list( name, button )
{
   var response = await send_request( "GET",
    build_cws_url( ciyam.get_cws_url( ), { path: server_item_path( "/webcmdlists/", name ) }, session_info( ) ), false );

   // NOTE: The editor is left as it was - switched first, what it held would be relabelled (found by review).
   if( ( response === null ) || ( response.indexOf( "Error: " ) === 0 ) )
   {
      set_error( "script_error", ( response === null ) ? "The list could not be fetched." : response.trim( ) );

      return;
   }

   show_script_editor( );

   g_current_script = "";

   g_editor_from_own = ( name === c_console_own_name );

   mark_current_item( button );

   document.getElementById( "script_name" ).value = name;
   document.getElementById( "script_body" ).value = response.replace( /\s+$/, "" ) + "\n";
   document.getElementById( "script_delete" ).hidden = true;

   var origin = document.getElementById( "script_origin" );

   origin.textContent = "From the server's ciyam_" + ( ( name === c_console_own_name ) ? ciyam.access : name )
    + ".list - Save here keeps a copy in this browser.";
   origin.hidden = false;

   set_error( "script_error", "" );
}

// ====================================================================
// JavaScripts on the server - "review javascripts"
// ====================================================================

// NOTE: For everyone - the server lists another account's own to admin alone (Ian, 2026-10-05).
async function load_server_javascripts( )
{
   g_server_javascripts = [ ];
   g_server_javascripts_known = false;

   render_server_javascripts( null );

   var response = await send_request( "GET",
    build_cws_url( ciyam.get_cws_url( ), { path: "/javascripts" }, session_info( ) ), true );

   g_server_javascripts = parse_name_list( response );
   g_server_javascripts_known = ( response !== null ) && !is_error_output( response );

   render_server_javascripts( response );
}

function render_server_javascripts( response )
{
   var holder = document.getElementById( "server_javascripts" );
   var note = document.getElementById( "server_javascripts_note" );

   holder.textContent = "";

   if( response === null )
      note.textContent = g_connected ? "Checking…" : "";
   else if( g_server_javascripts.length === 0 )
      note.textContent = ( String( response ).indexOf( "Error: " ) === 0 ) ? response.trim( ) : "No JavaScripts on this server.";
   else
      note.textContent = g_server_javascripts.length + " from review javascripts";

   g_server_javascripts.forEach( function( name )
   {
      var button = document.createElement( "button" );

      button.type = "button";
      button.className = "console-item";
      button.textContent = server_item_label( name );

      button.addEventListener( "click", function( ) { open_server_javascript( name, button ); } );

      holder.appendChild( button );
   } );
}

// NOTE: The server hands a javascript back with its "ciyam_<name>" renamed for this account
// ("ciyam_<pin>"), ready to be saved as this account's own.
async function open_server_javascript( name, button )
{
   var response = await send_request( "GET",
    build_cws_url( ciyam.get_cws_url( ), { path: server_item_path( "/javascripts/", name ) }, session_info( ) ), false );

   // NOTE: As for a list - the editor is only switched once there is a JavaScript to put in it.
   if( ( response === null ) || ( response.indexOf( "Error: " ) === 0 ) )
   {
      set_error( "script_error", ( response === null ) ? "The JavaScript could not be fetched." : response.trim( ) );

      return;
   }

   show_script_editor( "javascript" );

   g_current_script = "";

   g_editor_from_own = ( name === c_console_own_name );

   mark_current_item( button );

   document.getElementById( "script_name" ).value = "ciyam_" + ( ( name === c_console_own_name ) ? ciyam.access : name ) + ".js";
   document.getElementById( "script_body" ).value = response.replace( /\s+$/, "" ) + "\n";

   var origin = document.getElementById( "script_origin" );

   origin.textContent = ( name === c_console_own_name ) ? "This account's own JavaScript."
    : "From the server's ciyam_" + name + ".js - its functions are named for this account, ciyam_" + ciyam.access + "_...";
   origin.hidden = false;

   set_error( "script_error", "" );
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
         // NOTE: Unless each line is shown the failing one was not printed, so the stop names it.
         print_line( "Stopped at line " + steps[ i ].line + " of '" + name + "'"
          + ( g_prefs.echo_list_lines ? "." : ": " + steps[ i ].text ), "is-err" );

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

function load_prefs( )
{
   g_prefs = parse_prefs( stored( c_console_prefs_key ) );

   document.getElementById( "log_session_only" ).checked = g_prefs.log_session_only;
   document.getElementById( "log_polling" ).checked = g_prefs.log_polling;
   document.getElementById( "echo_list_lines" ).checked = g_prefs.echo_list_lines;
}

function save_prefs( )
{
   try
   {
      localStorage.setItem( c_console_prefs_key, JSON.stringify( g_prefs ) );
   }
   catch( e )
   {
   }

   render_storage( );
}

function do_set_log_session_only( )
{
   g_prefs.log_session_only = document.getElementById( "log_session_only" ).checked;

   if( g_prefs.log_session_only )
      forget_earlier_sessions( );

   save_prefs( );
}

// NOTE: Read by the chat and the accounts page as well, on every request, so ticking it here
// starts logging their polling too - from the next poll, with no reload.
function do_set_log_polling( )
{
   g_prefs.log_polling = document.getElementById( "log_polling" ).checked;

   save_prefs( );
}

function do_set_echo_list_lines( )
{
   g_prefs.echo_list_lines = document.getElementById( "echo_list_lines" ).checked;

   save_prefs( );
}

function set_log( entries )
{
   g_log = entries;
   g_log_keys = { };

   g_log.forEach( function( entry ) { g_log_keys[ entry.source + ":" + entry.id ] = true; } );

   render_log( );
}

// NOTE: Ticked mid-session, what is already showing from an earlier session goes at once.
// Linked, this console only ever lived in the current session, but its owner's entries came
// from its replay - so those are dropped and asked for again, and the owner now sends only
// the current session's.
function forget_earlier_sessions( )
{
   if( g_linked )
   {
      set_log( g_log.filter( function( entry ) { return entry.source !== g_owner_page.source; } ) );

      if( g_connected )
         g_log_channel.postMessage( { kind: "replay", owner: g_source, viewer: g_self } );
   }
   else
      set_log( g_log.filter( function( entry ) { return entry.session === g_session_seq; } ) );
}

// NOTE: When the session ends. The log is only ever in memory, so a reload clears it
// anyway - this is for signing out and a different account signing in on the same page.
function end_log_session( )
{
   ++g_session_seq;

   if( g_prefs.log_session_only )
      do_clear_log( );
}

function add_console_entry( entry )
{
   entry.id = ++g_log_counter;
   entry.session = g_session_seq;

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

const c_copy_query_label = "Copy full query";

const c_copy_query_ms = 1500;

// NOTE: An entry keeps no credentials, so they are put back from this tab's session ("full_query( )") -
// the chat's own, when linked. Signed out, there are none to put back.
async function copy_full_query( entry, button )
{
   var text = g_connected ? full_query( ciyam.host_info, entry, session_info( ) ) : "";

   var said = !g_connected ? "Sign in first" : ( ( text !== "" ) && await copy_text( text ) ) ? "Copied" : "Could not copy";

   button.textContent = said;

   window.setTimeout( function( ) { button.textContent = c_copy_query_label; }, c_copy_query_ms );
}

// NOTE: The clipboard API needs a secure page - "localhost" is one, a plain "http://" node is not - so a
// hidden text area and "copy" stand in for it there.
async function copy_text( text )
{
   try
   {
      if( navigator.clipboard && window.isSecureContext )
      {
         await navigator.clipboard.writeText( text );

         return true;
      }
   }
   catch( e )
   {
   }

   var area = document.createElement( "textarea" );

   area.value = text;
   area.setAttribute( "readonly", "" );
   area.style.position = "fixed";
   area.style.opacity = "0";

   document.body.appendChild( area );

   area.select( );

   var copied = false;

   try
   {
      copied = document.execCommand( "copy" );
   }
   catch( e )
   {
   }

   document.body.removeChild( area );

   return copied;
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

   // NOTE: The list keeps its pared-down look (Ian, 2026-10-06) - the whole query is copied from here, the
   // opened entry, to try again elsewhere. An entry from before this was kept has nothing to copy.
   if( entry.sent )
   {
      var copy_row = document.createElement( "div" );

      copy_row.className = "console-copyrow";

      var copy = document.createElement( "button" );

      copy.type = "button";
      copy.className = "console-chip";
      copy.textContent = c_copy_query_label;

      copy.addEventListener( "click", function( event )
      {
         event.stopPropagation( );

         copy_full_query( entry, copy );
      } );

      copy_row.appendChild( copy );
      detail_cell.appendChild( copy_row );
   }

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

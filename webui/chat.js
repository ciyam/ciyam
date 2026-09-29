// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: View logic for the chat prototype. Parsing lives in "chat_parse.js" and all
// API access goes through the CIYAM class in "ciyam.js" - nothing here builds a URL.
//
// Everything rendered from a response is written with textContent. Message text,
// usernames and room names are supplied by other users and must never reach innerHTML.

const c_lobby_room = "0000000";
const c_starting_room_no = "0000001";

const c_poll_interval = 4000;

const c_max_sender_colours = 6;

// NOTE: The fixed entry in the account selector, as distinct from a saved PIN.
//
// There is deliberately no "admin" entry. On a bootstrapped server the literal access
// "admin" is refused - "This web session is not valid (or has expired)" - because the
// access token is the PIN the bootstrap issued, which ".web_access_admin" merely points
// at. Admin signs in with that PIN like any other account.
const c_access_create = "create";

// NOTE: Three outcomes, not two. The harness has had this all along as
// "creds retain partial" - keep the access PIN, drop the hashed password. A single
// checkbox could not express it, which is what Ian raised.
const c_retain_none = "none";
const c_retain_access = "access";
const c_retain_full = "full";

const c_storage_device = "cws.device";
const c_storage_access = "cws.access";
const c_storage_hashed_prefix = "cws.hashed_";

// NOTE: The announcements this account has dismissed on this browser, by message id.
const c_storage_dismissed_prefix = "cws.dismissed_";

var g_room = "";
var g_rooms = [ ];

// NOTE: Invitations waiting to be taken up, worked out from Administration's messages. The
// room total last seen for Administration says when to read it again; -1 means never read.
var g_invite_messages = [ ];
var g_invitations = [ ];

var g_announcements = [ ];
var g_announcements_drawn = "";
var g_invite_total = -1;
var g_selected_invite = "";
var g_members = [ ];
var g_room_name = "";
var g_room_owner = "";

var g_start_point = "";
var g_poll_timer = null;
var g_last_poll = 0;

var g_edit_unique = "";
var g_edit_private = false;
var g_recipients = [ ];

var g_dialog_mode = "create";

var g_known_users = [ ];

// NOTE: Set for the one connect that follows a registration, so the issued PIN can be
// kept and shown. Cleared once used.
var g_registered_pin = "";

const c_first_load_limit = 15000;

var g_first_load = false;
var g_first_load_timer = null;

var g_console_open = false;
var g_console_loaded = false;

// NOTE: The request log the developer console shows. Kept here, in memory only, so that a
// console opened later can still see how this session began - sign in and the first loads
// happen before anyone opens the drawer. Request URLs carry credentials; the entries do
// not, because "make_log_entry( )" drops them.
const c_log_channel_name = "ciyam_console_log";

var g_request_log = [ ];
var g_request_log_id = 0;

// NOTE: Counts sessions ended in this page. Starting a count at sign out, not sign in,
// means the sign in requests belong to the session they start.
var g_session_seq = 0;

var g_in_poll = false;
var g_request_quiet = false;

var g_log_channel = null;

// ====================================================================
// Entry point
// ====================================================================

function chat( )
{
   install_request_log( );

   watch_timeouts( );

   if( localStorage.getItem( c_storage_device ) !== null )
      ciyam.device = localStorage.getItem( c_storage_device );

   populate_accounts( );

   window.setInterval( update_poll_label, 1000 );

   // NOTE: Typing, and a change of width that re-wraps the text, both resize the composer.
   document.getElementById( "composer_input" ).addEventListener( "input", on_composer_input );

   window.addEventListener( "resize", resize_composer );

   watch_drawer_widths( );

   // NOTE: An announcement dismissed in another tab of this browser goes here too.
   window.addEventListener( "storage", function( event )
   {
      if( ( ciyam.access !== "" ) && ( event.key === dismissed_key( ) ) )
         refresh_announcements( );
   } );

   // NOTE: Escape cancels whichever dialog is open. Bound on the document because the
   // dialogs are not focus traps, so the key would otherwise be missed depending on
   // what happens to have focus.
   document.addEventListener( "keydown", function( event )
   {
      if( event.key !== "Escape" )
         return;

      if( !document.getElementById( "pin_dialog" ).hidden )
      {
         do_close_pin_dialog( );

         event.preventDefault( );
      }
      else if( !document.getElementById( "room_dialog" ).hidden )
      {
         do_close_room_dialog( );

         event.preventDefault( );
      }
      else if( !document.getElementById( "password_dialog" ).hidden )
      {
         do_close_password_dialog( );

         event.preventDefault( );
      }
      else if( !document.getElementById( "emoji_panel" ).hidden )
      {
         close_emoji_panel( true );

         event.preventDefault( );
      }
      else if( !document.getElementById( "user_menu" ).hidden )
      {
         close_user_menu( true );

         event.preventDefault( );
      }
      else if( open_drawer( ) !== "" )
      {
         close_drawers( true );

         event.preventDefault( );
      }
   } );

   // NOTE: A click anywhere outside the account menu closes it.
   document.addEventListener( "click", function( event )
   {
      var menu = document.getElementById( "user_menu" );

      if( !menu.hidden && !event.target.closest( ".chat-user" ) )
         close_user_menu( false );

      // NOTE: And outside the emoji panel closes that - its own button toggles it instead.
      if( !document.getElementById( "emoji_panel" ).hidden
       && !event.target.closest( "#emoji_panel" ) && !event.target.closest( "#emoji_toggle" ) )
         close_emoji_panel( false );
   } );

   bind_emoji_search( );
   bind_emoji_suggest( );
}

function populate_accounts( )
{
   var select = document.getElementById( "signin_access" );

   // NOTE: Saved accounts are inserted *ahead* of the fixed entries, so they cannot be
   // cleared by trimming from the end - doing that removed "+ Register new account" and
   // left the saved ones in place, so every rebuild listed each PIN once more than the
   // last. Removing by value is stable however many have been inserted.
   for( var i = select.options.length - 1; i >= 0; i-- )
   {
      var value = select.options[ i ].value;

      if( ( value !== "" ) && ( value !== c_access_create ) )
         select.remove( i );
   }

   var entries = parse_access_list( localStorage.getItem( c_storage_access ) );

   for( var n = 0; n < entries.length; n++ )
   {
      var access = entries[ n ];

      if( ( access === "" ) || ( access === c_access_create ) )
         continue;

      var label = access;

      if( localStorage.getItem( c_storage_hashed_prefix + access ) !== null )
         label += "  ·  saved password";

      // NOTE: Inserted after the entries already added rather than always at index 1,
      // which reversed the list - the stored value is sorted, so the display was not.
      select.options.add( new Option( label, access, false ), 1 + n );
   }

   // NOTE: Only preselect a saved account when there is one - otherwise the first
   // option stands and the PIN field is what the user needs.
   if( entries.length > 0 )
      select.selectedIndex = 1;

   // NOTE: Always run this, even with no saved accounts. It is what decides which
   // fields are visible, and skipping it left the PIN field hidden until the user
   // changed the selection and changed it back.
   do_select_access( );
}

function do_select_access( )
{
   // NOTE: This writes to "ciyam.access" and "ciyam.hashed", so it must never run while a
   // session is open - it would repoint the session at a different account.
   if( ciyam.sessid !== "" )
      return;

   var access = document.getElementById( "signin_access" ).value;

   var pin_row = document.getElementById( "signin_pin_row" );
   var name_row = document.getElementById( "signin_name_row" );
   var password_row = document.getElementById( "signin_password_row" );
   var hint = document.getElementById( "signin_password_hint" );

   name_row.hidden = true;

   set_error( "signin_error", "" );

   if( access === c_access_create )
   {
      pin_row.hidden = true;
      password_row.hidden = false;

      // NOTE: Registering is the one case that needs a username, and this row was never
      // being shown - so there was no way to supply one and the form could not be used.
      name_row.hidden = false;

      ciyam.access = "";
      ciyam.hashed = "";

      var new_password = document.getElementById( "signin_password" );

      new_password.value = "";

      // NOTE: Re-enabled because selecting a saved account disables it.
      new_password.disabled = false;

      hint.textContent = "A PIN will be issued by the server.";

      document.getElementById( "signin_connect" ).textContent = "Register";
   }
   else
   {
      pin_row.hidden = ( access !== "" );
      password_row.hidden = false;

      ciyam.access = access;

      var hashed = localStorage.getItem( c_storage_hashed_prefix + access );

      ciyam.hashed = ( hashed === null ) ? "" : hashed;

      var password = document.getElementById( "signin_password" );

      password.value = "";
      password.disabled = ( ciyam.hashed !== "" );

      hint.textContent = ( ciyam.hashed !== "" )
       ? "Using saved credentials for this account." : "";

      document.getElementById( "signin_connect" ).textContent = "Connect";
   }

   refresh_retain_choice( ( access === c_access_create ) ? "" : access );
}

// ====================================================================
// Session
// ====================================================================

// NOTE: Registration is done here rather than through "CIYAM.connect", which cannot do
// it for any username longer than a PIN - it treats a longer "access" as admin seed
// entropy and sends admin credentials instead, so anything from six characters up fails
// with "User credentials are either invalid or incorrect". See ISS-008. These are the
// same two requests it would otherwise issue.
//
// The first allocates a PIN and answers "<pin> <seed>"; the second claims it by sending
// "<username>:<hash>" and answers the device token. An ordinary PIN sign in follows.
async function register_account( username, password )
{
   var issued = "";

   await ciyam.fetch( ciyam.get_cws_url( ) + "/devices?access=" + encodeURIComponent( username )
    + "&format=" + ciyam.format_type, "POST", function( response ) { issued = String( response ).trim( ); } );

   if( issued.indexOf( "Error: " ) === 0 )
   {
      ciyam.error = issued;

      return "";
   }

   var pos = issued.indexOf( " " );

   var pin = ( pos > 0 ) ? issued.substr( 0, pos ) : issued;

   if( !/^[0-9]{5}$/.test( pin ) )
   {
      ciyam.error = "Error: The server did not issue a PIN (answered '" + issued + "').";

      return "";
   }

   var credentials = username + ":" + ciyam.hash_combined( password, pin );

   var token = "";

   await ciyam.fetch( ciyam.get_cws_url( ) + "/devices?access=" + pin
    + "&format=" + ciyam.format_type + "&passwd=" + CIYAM.encode_base64_url( credentials ),
    "POST", function( response ) { token = String( response ).trim( ); } );

   if( token.indexOf( "Error: " ) === 0 )
   {
      ciyam.error = token;

      return "";
   }

   // NOTE: The browser keeps the device it already has. A saved password is hashed with
   // the device - sha256( sha256( access + password ) + device ) - so adopting the new
   // account's token here silently invalidated every password saved before it, and the
   // other accounts could not sign in until the browser was reset. Device tokens are not
   // tied to an account for signing in, so the new one is only needed when there is none.
   if( ciyam.device === "" )
      ciyam.device = token;

   return pin;
}

async function do_connect( )
{
   var select = document.getElementById( "signin_access" );

   var is_register = ( select.value === c_access_create );

   var access = select.value;

   if( is_register )
      access = "";
   else if( access === "" )
      access = document.getElementById( "signin_pin" ).value.trim( );

   var password = document.getElementById( "signin_password" ).value;
   var username = document.getElementById( "signin_name" ).value.trim( );

   if( is_register && ( username === "" ) )
   {
      set_error( "signin_error", "Choose a username to register." );

      return;
   }

   if( ( username !== "" ) && !is_valid_username( username ) )
   {
      set_error( "signin_error", "Username must be 3-12 lowercase characters, no repeated hyphens." );

      return;
   }

   if( !is_register && ( access === "" ) )
   {
      set_error( "signin_error", "Enter the account PIN." );

      return;
   }

   set_error( "signin_error", "" );

   document.getElementById( "signin_connect" ).disabled = true;

   ciyam.error = "";

   begin_busy( );

   var signin_note = document.getElementById( "signin_busy" );

   if( signin_note !== null )
      signin_note.hidden = false;

   try
   {
      if( is_register )
      {
         access = await register_account( username, password );

         g_registered_pin = access;
      }

      if( ( ciyam.error === "" ) && ( access !== "" ) )
      {
         // NOTE: "CIYAM.connect" uses a hash it is handed in preference to the password, so
         // a typed password has to clear any hash still held from a previous sign in.
         if( password !== "" )
            ciyam.hashed = "";

         await ciyam.connect( access, ciyam.device, ciyam.hashed, password, function( ) { } );
      }
   }
   finally
   {
      end_busy( );

      if( signin_note !== null )
         signin_note.hidden = true;
   }

   document.getElementById( "signin_connect" ).disabled = false;

   if( ciyam.device !== "" )
      localStorage.setItem( c_storage_device, ciyam.device );

   if( ciyam.error !== "" )
   {
      set_error( "signin_error", sign_in_error_text( ciyam.error ) );

      return;
   }

   if( ciyam.sessid === "" )
   {
      set_error( "signin_error", "No session was established." );

      return;
   }

   apply_retain_choice( );

   if( g_registered_pin !== "" )
   {
      show_new_pin( g_registered_pin, ciyam.username || username );

      g_registered_pin = "";
   }

   // NOTE: The account list is deliberately *not* rebuilt here. "populate_accounts"
   // reselects the first saved entry and "do_select_access" then overwrites
   // "ciyam.access" with it - which, once a second account had been saved, pointed the
   // live session at the wrong PIN and every later call failed with "This web session is
   // not valid (or has expired)". Signing out rebuilds the list, which is the only time
   // the sign-in view is seen again.
   enter_chat( );
}

function enter_chat( )
{
   document.getElementById( "signin_view" ).hidden = true;
   document.getElementById( "chat_view" ).hidden = false;

   document.getElementById( "topbar_user" ).textContent = ciyam.username || ciyam.access;
   document.getElementById( "user_menu_session" ).textContent = ciyam.sessid;
   document.getElementById( "console_session" ).textContent = "inherits chat session " + ciyam.sessid;

   render_user_badge( );

   begin_first_load( );

   load_rooms( );

   start_polling( );
}

// ====================================================================
// Account menu
// ====================================================================

// NOTE: The user's initial in their sender colour - the colour their name has in messages -
// in place of an avatar image, top right and larger in the menu it opens.
function render_user_badge( )
{
   var name = ciyam.username || ciyam.access;

   var colour = "var(--color-sender-" + sender_colour_index( name ) + ")";

   [ "user_avatar", "user_menu_initial" ].forEach( function( id )
   {
      var node = document.getElementById( id );

      node.textContent = user_initial( name );
      node.style.background = colour;
   } );

   var avatar = document.getElementById( "user_avatar" );

   avatar.title = name + " - account menu";
   avatar.setAttribute( "aria-label", "Account menu for " + name );

   document.getElementById( "user_menu_name" ).textContent = name;
   document.getElementById( "user_menu_pin" ).textContent = ciyam.access;
   document.getElementById( "user_menu_device" ).textContent = ciyam.device;
   document.getElementById( "user_menu_type" ).textContent = ciyam.is_admin ? "admin" : "standard";
}

function do_toggle_user_menu( )
{
   var menu = document.getElementById( "user_menu" );

   if( !menu.hidden )
   {
      close_user_menu( false );

      return;
   }

   menu.hidden = false;

   document.getElementById( "user_avatar" ).setAttribute( "aria-expanded", "true" );

   menu.querySelector( ".chat-user-menu-item" ).focus( );
}

function close_user_menu( restore_focus )
{
   document.getElementById( "user_menu" ).hidden = true;

   var avatar = document.getElementById( "user_avatar" );

   avatar.setAttribute( "aria-expanded", "false" );

   if( restore_focus )
      avatar.focus( );
}

function do_menu_linked_tab( )
{
   close_user_menu( false );

   do_open_linked_tab( );
}

function do_menu_sign_out( )
{
   close_user_menu( false );

   do_disconnect( );
}

// ====================================================================
// Emoji panel
// ====================================================================

// NOTE: The curated list is in "chat_emoji.js", read in the first time either the panel or the
// ":name" suggestions want it. The panel's buttons are built the first time it opens.
var g_emoji_catalogue = null;
var g_emoji_panel_built = false;

// NOTE: Per account, like the other saved settings. The first version kept one list for the
// whole browser under "cws.emoji_recent"; that is simply dropped.
const c_storage_emoji_recent_prefix = "cws.emoji_recent_";
const c_storage_emoji_recent_old = "cws.emoji_recent";

const c_emoji_suggest_max = 8;

var g_emoji_suggestions = [ ];
var g_emoji_suggest_index = 0;
var g_emoji_suggest_query = null;

function emoji_list( )
{
   if( g_emoji_catalogue === null )
      g_emoji_catalogue = emoji_catalogue( );

   return g_emoji_catalogue;
}

function emoji_recent_key( )
{
   return c_storage_emoji_recent_prefix + ciyam.access;
}

// NOTE: A phone has emoji on its own keyboard, and focusing a field there brings the keyboard
// up over the panel - so on a touch screen nothing is focused for the user.
function has_fine_pointer( )
{
   return window.matchMedia( "(pointer: fine)" ).matches;
}

function do_toggle_emoji( )
{
   if( document.getElementById( "emoji_panel" ).hidden )
      open_emoji_panel( );
   else
      close_emoji_panel( true );
}

function open_emoji_panel( )
{
   if( document.getElementById( "emoji_toggle" ).disabled )
      return;

   if( !g_emoji_panel_built )
   {
      emoji_list( );

      build_emoji_panel( );

      g_emoji_panel_built = true;
   }

   close_emoji_suggest( );

   var search = document.getElementById( "emoji_search" );

   search.value = "";

   render_recent_emoji( );
   show_emoji_results( "" );

   document.getElementById( "emoji_scroll" ).scrollTop = 0;
   document.getElementById( "emoji_panel" ).hidden = false;
   document.getElementById( "emoji_toggle" ).setAttribute( "aria-expanded", "true" );

   if( has_fine_pointer( ) )
      search.focus( );
}

function close_emoji_panel( restore_focus )
{
   var panel = document.getElementById( "emoji_panel" );

   if( panel.hidden )
      return;

   panel.hidden = true;

   document.getElementById( "emoji_toggle" ).setAttribute( "aria-expanded", "false" );

   if( restore_focus && has_fine_pointer( ) )
      document.getElementById( "composer_input" ).focus( );
}

function emoji_button( item )
{
   var button = document.createElement( "button" );

   button.type = "button";
   button.className = "chat-emoji";
   button.textContent = item.char;
   button.title = item.name;
   button.setAttribute( "aria-label", item.name );

   button.addEventListener( "click", function( ) { insert_emoji( item.char ); } );

   return button;
}

function emoji_section( id, label, items )
{
   var section = document.createElement( "section" );

   section.className = "chat-emoji-section";
   section.dataset.section = id;

   var head = document.createElement( "h3" );

   head.className = "chat-emoji-section-head";
   head.textContent = label;

   var grid = document.createElement( "div" );

   grid.className = "chat-emoji-grid";

   items.forEach( function( item ) { grid.appendChild( emoji_button( item ) ); } );

   section.appendChild( head );
   section.appendChild( grid );

   return section;
}

// NOTE: One section per category, then two that change - Recent at the top, and the search
// results, shown in place of everything else while there is something typed.
function build_emoji_panel( )
{
   var scroll = document.getElementById( "emoji_scroll" );
   var tabs = document.getElementById( "emoji_tabs" );

   scroll.appendChild( emoji_section( "recent", "Recent", [ ] ) );
   scroll.appendChild( emoji_section( "results", "Results", [ ] ) );

   g_emoji_catalogue.forEach( function( category )
   {
      scroll.appendChild( emoji_section( category.id, category.label, category.items ) );

      var tab = document.createElement( "button" );

      tab.type = "button";
      tab.className = "chat-emoji-tab";
      tab.textContent = category.icon;
      tab.title = category.label;
      tab.setAttribute( "aria-label", category.label );

      tab.addEventListener( "click", function( )
      {
         var search = document.getElementById( "emoji_search" );

         if( search.value !== "" )
         {
            search.value = "";

            show_emoji_results( "" );
         }

         var target = scroll.querySelector( "[data-section=\"" + category.id + "\"]" );

         scroll.scrollTop = target.offsetTop - scroll.offsetTop;
      } );

      tabs.appendChild( tab );
   } );
}

function read_recent_emoji( )
{
   try
   {
      localStorage.removeItem( c_storage_emoji_recent_old );

      return parse_recent_emoji( localStorage.getItem( emoji_recent_key( ) ) );
   }
   catch( e )
   {
      return [ ];
   }
}

function remember_recent_emoji( char )
{
   try
   {
      localStorage.setItem( emoji_recent_key( ), JSON.stringify( push_recent_emoji( read_recent_emoji( ), char ) ) );
   }
   catch( e )
   {
   }
}

// NOTE: Recent is drawn when the panel opens, not as emoji are picked - the row moving under
// the pointer while several are clicked in turn would put the wrong one in.
function render_recent_emoji( )
{
   var section = document.querySelector( "#emoji_scroll [data-section=\"recent\"]" );

   var grid = section.querySelector( ".chat-emoji-grid" );

   grid.textContent = "";

   var names = { };

   g_emoji_catalogue.forEach( function( category )
   {
      category.items.forEach( function( item ) { names[ item.char ] = item.name; } );
   } );

   var recent = read_recent_emoji( ).filter( function( char ) { return names[ char ] !== undefined; } );

   recent.forEach( function( char ) { grid.appendChild( emoji_button( { char: char, name: names[ char ] } ) ); } );

   section.hidden = ( recent.length === 0 );
}

function show_emoji_results( query )
{
   var scroll = document.getElementById( "emoji_scroll" );

   var searching = ( query.trim( ) !== "" );

   scroll.querySelectorAll( ".chat-emoji-section" ).forEach( function( section )
   {
      var id = section.dataset.section;

      if( id === "results" )
         section.hidden = !searching;
      else if( id === "recent" )
         section.hidden = searching || ( section.querySelector( ".chat-emoji-grid" ).children.length === 0 );
      else
         section.hidden = searching;
   } );

   if( !searching )
      return;

   var results = scroll.querySelector( "[data-section=\"results\"]" );

   var grid = results.querySelector( ".chat-emoji-grid" );

   grid.textContent = "";

   var found = search_emoji( g_emoji_catalogue, query );

   found.forEach( function( item ) { grid.appendChild( emoji_button( item ) ); } );

   results.querySelector( ".chat-emoji-section-head" ).textContent = ( found.length === 0 ) ? "No emoji match" : "Results";

   scroll.scrollTop = 0;
}

function bind_emoji_search( )
{
   var search = document.getElementById( "emoji_search" );

   search.addEventListener( "input", function( ) { show_emoji_results( search.value ); } );

   // NOTE: Enter takes the first result, as a search box usually does.
   search.addEventListener( "keydown", function( event )
   {
      if( event.key !== "Enter" )
         return;

      event.preventDefault( );

      var first = document.querySelector( "#emoji_scroll [data-section=\"results\"] .chat-emoji" );

      if( first !== null )
         insert_emoji( first.textContent );
   } );
}

// NOTE: Goes in where the cursor was in the message box - its selection is kept while the
// panel has the focus - and the cursor moves past it, so several in a row land in order.
function insert_emoji( char )
{
   var input = document.getElementById( "composer_input" );

   if( input.disabled )
      return;

   var start = input.selectionStart;
   var end = input.selectionEnd;

   input.value = input.value.substring( 0, start ) + char + input.value.substring( end );

   var after = start + char.length;

   input.setSelectionRange( after, after );

   // NOTE: The same path as typing, so the box grows and the byte count follows.
   input.dispatchEvent( new Event( "input" ) );

   remember_recent_emoji( char );
}

// ====================================================================
// ":name" suggestions - the emoji autocomplete
// ====================================================================

// NOTE: Typing ":" and two letters - ":thu" - offers the emoji whose names match, above the
// message box. Up and Down move through them, Tab or Enter takes one in place of the ":thu",
// Escape puts them away. "emoji_query_at( )" decides when, so "10:30" and ":)" never do.
function update_emoji_suggest( )
{
   var input = document.getElementById( "composer_input" );

   var found = null;

   if( !input.disabled && ( input.selectionStart === input.selectionEnd ) )
      found = emoji_query_at( input.value, input.selectionStart );

   var list = found ? suggest_emoji( emoji_list( ), found.query, c_emoji_suggest_max ) : [ ];

   if( list.length === 0 )
   {
      close_emoji_suggest( );

      return;
   }

   if( ( g_emoji_suggest_query === null ) || ( g_emoji_suggest_query.query !== found.query ) )
      g_emoji_suggest_index = 0;

   g_emoji_suggest_query = found;
   g_emoji_suggestions = list;

   close_emoji_panel( false );

   render_emoji_suggest( );
}

function emoji_suggest_open( )
{
   return !document.getElementById( "emoji_suggest" ).hidden;
}

function render_emoji_suggest( )
{
   var host = document.getElementById( "emoji_suggest" );
   var input = document.getElementById( "composer_input" );

   host.textContent = "";

   g_emoji_suggestions.forEach( function( item, index )
   {
      var option = document.createElement( "li" );

      option.id = "emoji_suggest_" + index;
      option.className = "chat-emoji-option";
      option.setAttribute( "role", "option" );
      option.setAttribute( "aria-selected", ( index === g_emoji_suggest_index ) ? "true" : "false" );

      var char = document.createElement( "span" );

      char.className = "chat-emoji-option-char";
      char.textContent = item.char;

      var code = document.createElement( "span" );

      code.className = "chat-emoji-option-code";
      code.textContent = emoji_shortcode( item.name );

      option.appendChild( char );
      option.appendChild( code );

      // NOTE: On mousedown, so the message box keeps the focus and its cursor.
      option.addEventListener( "mousedown", function( event )
      {
         event.preventDefault( );

         take_emoji_suggestion( index );
      } );

      host.appendChild( option );
   } );

   host.hidden = false;

   input.setAttribute( "aria-expanded", "true" );
   input.setAttribute( "aria-activedescendant", "emoji_suggest_" + g_emoji_suggest_index );

   var current = document.getElementById( "emoji_suggest_" + g_emoji_suggest_index );

   if( current !== null )
      current.scrollIntoView( { block: "nearest" } );
}

function close_emoji_suggest( )
{
   var host = document.getElementById( "emoji_suggest" );

   if( host.hidden )
      return;

   host.hidden = true;
   host.textContent = "";

   g_emoji_suggestions = [ ];
   g_emoji_suggest_query = null;

   var input = document.getElementById( "composer_input" );

   input.setAttribute( "aria-expanded", "false" );
   input.removeAttribute( "aria-activedescendant" );
}

function take_emoji_suggestion( index )
{
   var item = g_emoji_suggestions[ index ];
   var query = g_emoji_suggest_query;

   if( !item || !query )
      return;

   var input = document.getElementById( "composer_input" );

   var caret = input.selectionStart;

   input.value = input.value.substring( 0, query.start ) + item.char + input.value.substring( caret );

   var after = query.start + item.char.length;

   input.setSelectionRange( after, after );

   close_emoji_suggest( );

   input.dispatchEvent( new Event( "input" ) );

   remember_recent_emoji( item.char );
}

// NOTE: The keys the suggestions take while they are showing - returns true when it took one.
// Escape is kept from the page's own handler, which would otherwise close a panel as well.
function emoji_suggest_key( event )
{
   if( !emoji_suggest_open( ) )
      return false;

   var count = g_emoji_suggestions.length;

   if( ( event.key === "ArrowDown" ) || ( event.key === "ArrowUp" ) )
   {
      g_emoji_suggest_index = ( g_emoji_suggest_index + ( ( event.key === "ArrowDown" ) ? 1 : count - 1 ) ) % count;

      render_emoji_suggest( );
   }
   else if( ( ( event.key === "Enter" ) && !event.shiftKey ) || ( event.key === "Tab" ) )
      take_emoji_suggestion( g_emoji_suggest_index );
   else if( event.key === "Escape" )
   {
      close_emoji_suggest( );

      event.stopPropagation( );
   }
   else
      return false;

   event.preventDefault( );

   return true;
}

function bind_emoji_suggest( )
{
   var input = document.getElementById( "composer_input" );

   // NOTE: Moving the cursor can move into or out of a ":name".
   input.addEventListener( "click", update_emoji_suggest );

   input.addEventListener( "keyup", function( event )
   {
      if( [ "ArrowLeft", "ArrowRight", "Home", "End" ].indexOf( event.key ) >= 0 )
         update_emoji_suggest( );
   } );

   input.addEventListener( "blur", close_emoji_suggest );
}

// ====================================================================
// Narrow screens - the room rail and room details slide over the thread
// ====================================================================

// NOTE: These match the widths in "chat.css" at which each panel leaves the page. One panel
// is open at a time. Room details stay until closed; the rail also closes on picking a room.
const c_rail_width_query = "(max-width: 820px)";
const c_details_width_query = "(max-width: 1100px)";

function open_drawer( )
{
   var app = document.getElementById( "chat_view" );

   if( app.classList.contains( "is-rail-open" ) )
      return "rail";

   if( app.classList.contains( "is-details-open" ) )
      return "details";

   return "";
}

function set_drawer( which, restore_focus )
{
   var was = open_drawer( );

   var app = document.getElementById( "chat_view" );

   app.classList.toggle( "is-rail-open", ( which === "rail" ) );
   app.classList.toggle( "is-details-open", ( which === "details" ) );

   update_drawer_toggles( );

   // NOTE: Focus goes into a panel as it opens - it comes before or after the thread, so the
   // keyboard would otherwise have to go the long way round to reach it.
   if( which === "rail" )
   {
      var rail = document.getElementById( "room_rail" );

      var target = rail.querySelector( ".chat-room.is-selected" ) || rail.querySelector( "button" );

      if( target )
         target.focus( { preventScroll: true } );
   }
   else if( which === "details" )
      document.querySelector( "#room_details .chat-drawer-close" ).focus( { preventScroll: true } );
   else if( restore_focus && ( was !== "" ) )
      document.getElementById( ( was === "rail" ) ? "rail_toggle" : "details_toggle" ).focus( );
}

function close_drawers( restore_focus )
{
   if( open_drawer( ) !== "" )
      set_drawer( "", restore_focus );
}

function do_toggle_rail( )
{
   set_drawer( ( open_drawer( ) === "rail" ) ? "" : "rail", true );
}

function do_toggle_details( )
{
   set_drawer( ( open_drawer( ) === "details" ) ? "" : "details", true );
}

// NOTE: A panel open when the window widens past its width is part of the page again, and
// would come back as a drawer if the window narrowed later - so it is closed.
function watch_drawer_widths( )
{
   [ [ c_rail_width_query, "rail" ], [ c_details_width_query, "details" ] ].forEach( function( pair )
   {
      window.matchMedia( pair[ 0 ] ).addEventListener( "change", function( event )
      {
         if( !event.matches && ( open_drawer( ) === pair[ 1 ] ) )
            close_drawers( false );
      } );
   } );
}

// NOTE: The rail's button carries a count of what is waiting elsewhere, since the rail and its
// counts are out of sight.
function update_drawer_toggles( )
{
   var drawer = open_drawer( );

   var waiting = badge_text( unread_elsewhere( g_rooms, g_invitations, g_room, ciyam.is_admin ) );

   var badge = document.getElementById( "rail_badge" );

   set_text( badge, waiting );

   badge.hidden = ( waiting === "" );

   var rail = document.getElementById( "rail_toggle" );

   rail.setAttribute( "aria-expanded", ( drawer === "rail" ) ? "true" : "false" );
   rail.setAttribute( "aria-label", ( ( drawer === "rail" ) ? "Hide rooms" : "Show rooms" )
    + ( ( waiting !== "" ) ? " (" + waiting + " unread)" : "" ) );

   var details = document.getElementById( "details_toggle" );

   details.setAttribute( "aria-expanded", ( drawer === "details" ) ? "true" : "false" );
   details.setAttribute( "aria-label", ( drawer === "details" ) ? "Hide room details" : "Show room details" );
}

// ====================================================================
// Change password
// ====================================================================

function do_open_password_dialog( )
{
   close_user_menu( false );

   [ "password_current", "password_new", "password_confirm" ].forEach( function( id )
   {
      document.getElementById( id ).value = "";
   } );

   set_error( "password_error", "" );

   update_password_dialog( );

   document.getElementById( "password_dialog" ).hidden = false;

   document.getElementById( "password_current" ).focus( );
}

function do_close_password_dialog( )
{
   document.getElementById( "password_dialog" ).hidden = true;
}

// NOTE: The strength bar follows the new password; Change is enabled once there is a current
// password, the new one is at least weak, and the two new ones match.
function update_password_dialog( )
{
   var current = document.getElementById( "password_current" ).value;
   var fresh = document.getElementById( "password_new" ).value;
   var confirm = document.getElementById( "password_confirm" ).value;

   var strength = password_strength( fresh );

   var box = document.getElementById( "password_strength" );

   box.hidden = ( strength.level < 0 );
   box.dataset.level = String( strength.level );

   document.getElementById( "password_strength_label" ).textContent = strength.text;

   document.getElementById( "password_submit" ).disabled =
    ( current === "" ) || ( strength.level < 1 ) || ( fresh !== confirm );

   if( ( confirm !== "" ) && ( fresh !== confirm ) && ( confirm.length >= fresh.length ) )
      set_error( "password_error", "The new passwords do not match." );
   else if( strength.level === 0 )
      set_error( "password_error", "Use at least 7 characters." );
   else
      set_error( "password_error", "" );
}

// NOTE: The hash this session was opened with, for a given password - "determine_hashed( )"
// in "ciyam.js", worked out without touching the session's own.
function session_hash_for( password )
{
   return hex_sha256( hex_sha256( ciyam.hash_combined( password ) ) + ciyam.device );
}

async function do_submit_password( )
{
   var current = document.getElementById( "password_current" ).value;
   var fresh = document.getElementById( "password_new" ).value;
   var confirm = document.getElementById( "password_confirm" ).value;

   // NOTE: The server only checks the session, so the current password is checked here,
   // against the hash the session was opened with - no request, and nothing sent.
   if( session_hash_for( current ) !== ciyam.hashed )
   {
      set_error( "password_error", "The current password is not right." );

      return;
   }

   if( ( password_strength( fresh ).level < 1 ) || ( fresh !== confirm ) )
      return;

   if( fresh === current )
   {
      set_error( "password_error", "The new password is the same as the current one." );

      return;
   }

   var submit = document.getElementById( "password_submit" );

   submit.disabled = true;

   // NOTE: The account's own PIN - "***" is not substituted by "update_user( )" and the
   // server refuses it. The password is hashed with the PIN before it is sent.
   var response = await new Promise( function( resolve )
   {
      serialised( function( )
      {
         return ciyam.update_user( ciyam.access, "password=" + fresh, function( r ) { resolve( String( r ) ); } );
      } );
   } );

   submit.disabled = false;

   if( is_error_response( response ) )
   {
      set_error( "password_error", error_text( response ) );

      return;
   }

   // NOTE: The session stays open. A hash remembered for this account was made from the old
   // password and the server now refuses it, so it is replaced; nothing stored, nothing to do.
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

   do_close_password_dialog( );

   show_alert( "Password changed.", "is-info" );
}

// NOTE: Between sign in and the first room's messages the thread showed "No room selected"
// - untrue, just not loaded yet - or, after signing out and back in, the previous room's
// header. A spinner holds the thread until the first room is ready or there is none.
function begin_first_load( )
{
   g_first_load = true;

   if( g_first_load_timer !== null )
      window.clearTimeout( g_first_load_timer );

   // NOTE: A lost response must not leave the spinner up for good.
   g_first_load_timer = window.setTimeout( end_first_load, c_first_load_limit );

   show_thread_view( );
}

function end_first_load( )
{
   if( g_first_load_timer !== null )
   {
      window.clearTimeout( g_first_load_timer );

      g_first_load_timer = null;
   }

   if( !g_first_load )
      return;

   g_first_load = false;

   show_thread_view( );

   var list = document.getElementById( "message_list" );

   // NOTE: Messages rendered while the list was hidden could not scroll it.
   list.scrollTop = list.scrollHeight;
}

// NOTE: The one place that decides which of the thread's views is showing.
function show_thread_view( )
{
   var loading = g_first_load;
   var has_room = ( g_room !== "" );
   var inviting = !loading && !has_room && ( g_selected_invite !== "" );

   document.getElementById( "thread_loading" ).hidden = !loading;
   document.getElementById( "thread_invite" ).hidden = !inviting;
   document.getElementById( "thread_empty" ).hidden = loading || has_room || inviting;
   // NOTE: Only a room has its head at every width; for an invitation or an empty thread it
   // holds the narrow screen's panel buttons, so the title says where the user is.
   var head = document.getElementById( "thread_head" );

   head.hidden = loading;
   head.dataset.view = has_room ? "room" : ( inviting ? "invite" : "empty" );

   if( !has_room )
      document.getElementById( "thread_name" ).textContent = inviting ? "Invitation" : "Rooms";

   document.getElementById( "message_list" ).hidden = loading || !has_room;
   document.getElementById( "composer" ).hidden = loading || !has_room;
   document.getElementById( "room_panel" ).hidden = loading || ( !has_room && !inviting );
   // NOTE: Announcements show whatever the thread holds - someone with no rooms yet is the
   // very person a welcome is for.
   document.getElementById( "announcements" ).hidden = loading || ( g_announcements.length === 0 );
}

async function do_disconnect( )
{
   stop_polling( );

   await ciyam.disconnect( function( ) { } );

   g_room = "";
   g_rooms = [ ];
   g_members = [ ];
   g_start_point = "";

   g_invite_messages = [ ];
   g_invitations = [ ];
   g_invite_total = -1;
   g_selected_invite = "";

   g_timeouts_in_row = 0;
   g_resyncing = false;
   g_uncertain_send = null;

   g_announcements = [ ];
   g_announcements_drawn = "";

   document.getElementById( "announcements" ).textContent = "";

   // NOTE: Nothing from this session may be left on screen for the next account - an alert
   // such as "Password changed." was still showing after someone else signed in.
   do_dismiss_alert( );

   g_last_poll = 0;

   document.getElementById( "rail_poll" ).textContent = "";

   close_drawers( false );

   close_emoji_panel( false );
   close_emoji_suggest( );

   end_first_load( );

   if( g_console_open )
      do_toggle_console( );

   unlink_consoles( );

   end_log_session( );

   document.getElementById( "chat_view" ).hidden = true;
   document.getElementById( "signin_view" ).hidden = false;

   populate_accounts( );
}

// NOTE: Applied after a successful connect rather than on demand - there is nothing to
// remember until the session has produced a hashed password, which is why this used to be
// a button that refused to do anything until you had connected.
function apply_retain_choice( )
{
   var mode = document.getElementById( "signin_retain" ).value;

   if( ciyam.access === "" )
      return;

   // NOTE: A PIN the server has just issued is the account's only identifier and the user
   // has no other copy, so it is kept even when "forget" was chosen - see ISS-010. The
   // password still follows the choice.
   if( ( g_registered_pin === ciyam.access ) && ( mode === c_retain_none ) )
      mode = c_retain_access;

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

   // NOTE: Removing the key rather than storing an empty string. Storing "" left a blank
   // entry that both this client and the harness then read as a nameless account.
   var value = format_access_list( entries );

   if( value === null )
      localStorage.removeItem( c_storage_access );
   else
      localStorage.setItem( c_storage_access, value );
}

// NOTE: Reflects whether the selected account is already saved, so the box shows the
// current state rather than a default that would silently forget it on the next connect.
function refresh_retain_choice( access )
{
   var entries = parse_access_list( localStorage.getItem( c_storage_access ) );

   var mode = c_retain_none;

   if( ( access !== "" ) && ( entries.indexOf( access ) >= 0 ) )
   {
      mode = ( localStorage.getItem( c_storage_hashed_prefix + access ) !== null )
       ? c_retain_full : c_retain_access;
   }

   document.getElementById( "signin_retain" ).value = mode;
}

function do_reset_browser( )
{
   if( ciyam.sessid !== "" )
      return;

   if( !confirm( "Forget the device token and every saved account on this browser?" ) )
      return;

   localStorage.clear( );

   location.reload( );
}

function do_open_linked_tab( )
{
   var url = new URL( window.location.href );

   url.searchParams.set( "source", g_self );

   window.open( url.toString( ), "_blank" );
}


// ====================================================================
// Request serialisation
// ====================================================================

// NOTE: The CIYAM class keeps one "user_callback" per instance, so two requests in
// flight at once will deliver the second caller's callback the first caller's
// response. That is what made the member list churn: a poll issued the entrance
// request and the room request together, the callbacks crossed, and the members of
// whichever room answered last were written over the other.
//
// Every call through the class is therefore queued, so only one is ever in flight.
var g_request_chain = Promise.resolve( );

// NOTE: Requests are usually quick enough that a spinner would only flash, so one is
// shown only once a request has been outstanding for longer than this.
const c_busy_delay = 1000;

var g_busy_count = 0;
var g_busy_timer = null;

function begin_busy( )
{
   if( ++g_busy_count === 1 )
   {
      g_busy_timer = window.setTimeout( function( )
      {
         var node = document.getElementById( "chat_busy" );

         if( node !== null )
            node.hidden = false;
      }, c_busy_delay );
   }
}

function end_busy( )
{
   if( --g_busy_count > 0 )
      return;

   g_busy_count = 0;

   if( g_busy_timer !== null )
   {
      window.clearTimeout( g_busy_timer );

      g_busy_timer = null;
   }

   var node = document.getElementById( "chat_busy" );

   if( node !== null )
      node.hidden = true;
}

function serialised( fn )
{
   // NOTE: Whether this was queued by the poll is decided now, when it is queued, and
   // applied only while "fn" issues its request - which it does synchronously - so the
   // request log can leave polling out without guessing from the URL.
   var quiet = g_in_poll;

   var run = function( )
   {
      g_request_quiet = quiet;

      try
      {
         return fn( );
      }
      finally
      {
         g_request_quiet = false;
      }
   };

   var wrapped = function( )
   {
      begin_busy( );

      return Promise.resolve( ).then( run ).then(
       function( value ) { end_busy( ); return value; },
       function( error ) { end_busy( ); throw error; } );
   };

   var next = g_request_chain.then( wrapped, wrapped );

   // NOTE: Keep the chain alive even if one request rejects.
   g_request_chain = next.catch( function( ) { } );

   return next;
}

// ====================================================================
// Rooms
// ====================================================================

function load_rooms( )
{
   return serialised( function( )
   {
      return ciyam.fetch_messages( c_lobby_room, "", on_rooms_response );
   } );
}

function on_rooms_response( response )
{
   var result = parse_fetch_response( response );

   // NOTE: A failure on the first load keeps the spinner up - the next poll tries again, and
   // the first-load limit still ends it. Ending it here showed "No room selected" to someone
   // who does have rooms, until the next poll found them.
   // NOTE: A time-out has its own notice, from 'note_timeout'.
   if( result.error !== "" )
   {
      if( !is_timeout_response( response ) )
         show_alert( result.error, "is-error", true );

      return;
   }

   // NOTE: Defensive - an entrance listing carries room rows. Anything else is a
   // response meant for a different caller and must not be acted on here.
   if( ( result.rooms.length === 0 ) && ( result.messages.length > 0 ) )
      return;

   clear_loading_alert( );

   // NOTE: The room list is polled too - someone with no room open still sees when.
   g_last_poll = Date.now( );

   // NOTE: The entrance listing returns every user, so the invitee picker can be
   // populated from it with no request of its own.
   if( result.members.length > 0 )
      g_known_users = apply_presence( result.members );

   {
      g_rooms = derive_room_list( result.rooms );

      // NOTE: A new invitation arrives as a message in Administration, which moves its total
      // in this listing - so it is only read again when that changes, not on every poll.
      var starting = find_room( c_starting_room_no );

      if( !ciyam.is_admin && ( starting !== null ) && ( starting.total !== g_invite_total ) )
      {
         g_invite_total = starting.total;

         check_invitations( );
      }

      refresh_invitations( );

      render_rooms( );

      if( ( g_room !== "" ) || ( g_selected_invite !== "" ) )
         update_thread_meta( );

      // NOTE: The owner can change who may post while others are in the room - so the message
      // box follows the listing, not only a change of room.
      sync_posting_rules( );

      var shown = visible_rooms( g_rooms, ciyam.is_admin );

      // NOTE: Open the first room with anything unread, else the first listed - of the rooms
      // this user can see, and not over an invitation they are looking at.
      if( ( g_room === "" ) && ( g_selected_invite === "" ) && ( shown.length > 0 ) )
         select_room( shown[ 0 ].room, "" );

      // NOTE: With no room to open there is nothing more to wait for - and now "No room
      // selected" is true rather than premature.
      if( g_room === "" )
         end_first_load( );
   }
}

// NOTE: The poll runs every few seconds and these lists are usually identical between
// polls. Rebuilding regardless made both panels visibly flicker, so each render is
// skipped when the data it would draw has not changed.
var g_rooms_drawn = "";
var g_members_drawn = "";

function rooms_signature( )
{
   var parts = [ g_room, g_selected_invite, ciyam.is_admin ? "adm" : "std" ];

   for( var n = 0; n < g_invitations.length; n++ )
      parts.push( "invite:" + g_invitations[ n ].room + ":" + g_invitations[ n ].name + ":" + g_invitations[ n ].inviter );

   for( var i = 0; i < g_rooms.length; i++ )
   {
      var r = g_rooms[ i ];

      parts.push( r.room + ":" + r.name + ":" + r.owner + ":" + r.unread + "/" + r.total + ":" + r.posts );
   }

   return parts.join( "|" );
}

function members_signature( )
{
   var parts = [ ];

   for( var i = 0; i < g_members.length; i++ )
      parts.push( g_members[ i ].name + "+" + g_members[ i ].sessions );

   return parts.join( "|" );
}

// NOTE: Updates a list in place rather than emptying and rebuilding it. Clearing a list
// on every poll made the panels visibly blank and repaint, which is distracting when the
// underlying data has barely changed.
//
// Rows already present are kept and only their changed parts are written; rows are moved
// with insertBefore, which relocates an existing element without recreating it, so click
// handlers bound at creation survive and nothing flashes.
function reconcile_list( host, items, key_of, make, update )
{
   var existing = { };

   var child = host.firstElementChild;

   while( child )
   {
      existing[ child.dataset.key ] = child;

      child = child.nextElementSibling;
   }

   var previous = null;

   for( var i = 0; i < items.length; i++ )
   {
      var key = key_of( items[ i ] );

      var node = existing[ key ];

      if( node === undefined )
      {
         node = make( items[ i ] );

         node.dataset.key = key;
      }
      else
         delete existing[ key ];

      update( node, items[ i ] );

      var wanted = ( previous === null ) ? host.firstElementChild : previous.nextElementSibling;

      if( node !== wanted )
         host.insertBefore( node, wanted );

      previous = node;
   }

   for( var gone in existing )
      host.removeChild( existing[ gone ] );
}

function render_rooms( force )
{
   update_drawer_toggles( );

   var signature = rooms_signature( );

   if( !force && ( signature === g_rooms_drawn ) )
      return;

   g_rooms_drawn = signature;

   var list = document.getElementById( "room_list" );

   var template = document.getElementById( "tpl_room" );

   // NOTE: Invitations first, newest first, then the rooms themselves. They are keyed apart,
   // so when an invitation is taken up its entry is replaced by the room's own.
   var items = g_invitations.map( function( invite ) { return { key: "invite:" + invite.room, invite: invite }; } )
    .concat( visible_rooms( g_rooms, ciyam.is_admin ).map( function( entry ) { return { key: entry.room, room: entry }; } ) );

   reconcile_list( list, items,
    function( item )
    {
       return item.key;
    },
    function( item )
    {
       var node = template.content.cloneNode( true ).querySelector( ".chat-room" );

       if( item.invite )
       {
          node.classList.add( "is-invitation" );

          node.dataset.invite = item.invite.room;

          node.addEventListener( "click", function( event )
          {
             select_invitation( event.currentTarget.dataset.invite );
          } );
       }
       else
       {
          node.dataset.room = item.room.room;

          node.addEventListener( "click", function( event )
          {
             select_room( event.currentTarget.dataset.room, "" );
          } );
       }

       return node;
    },
    function( node, item )
    {
       if( item.invite )
       {
          set_text( node.querySelector( ".chat-room-name" ), item.invite.name );

          var sub = node.querySelector( ".chat-room-sub" );

          set_text( sub, "invited by " + item.invite.inviter );

          sub.hidden = false;

          node.querySelector( ".chat-room-count" ).hidden = true;
          node.querySelector( ".chat-room-invite" ).hidden = false;

          node.title = item.invite.inviter + " invited you to this room";

          node.classList.toggle( "is-selected", ( item.invite.room === g_selected_invite ) );

          return;
       }

       var entry = item.room;

       set_text( node.querySelector( ".chat-room-name" ), entry.name );

       var count = node.querySelector( ".chat-room-count" );

       set_text( count, entry.unread + "/" + entry.total );

       count.classList.toggle( "has-unread", ( entry.unread > 0 ) );

       var mark = node.querySelector( ".chat-room-lock" );

       if( entry.posts === "any" )
          mark.hidden = true;
       else
       {
          mark.hidden = false;

          set_text( mark, ( entry.posts === "none" ) ? "\uD83D\uDD12" : "\uD83D\uDD13" );

          mark.title = ( entry.posts === "none" ) ? "Locked - nobody can post"
           : "Only the room owner can post";
       }

       node.classList.toggle( "is-selected", ( entry.room === g_room ) );
    } );

   refresh_invite_actions( );
}

function set_text( node, text )
{
   if( node.textContent !== text )
      node.textContent = text;
}

function select_room( room, token )
{
   if( room === "" )
      return;

   g_room = room;
   g_start_point = "";
   g_edit_unique = "";
   g_edit_private = false;
   g_recipients = [ ];

   g_selected_invite = "";

   var entry = find_room( room );

   // NOTE: Just joined from an invitation, the room is not in the listing yet - the
   // invitation has its name until the listing catches up.
   var invite = entry ? null : find_invitation( room );

   g_room_name = entry ? entry.name : ( invite ? invite.name : room );
   g_room_owner = entry ? entry.owner : "";

   if( open_drawer( ) === "rail" )
      close_drawers( false );

   close_emoji_panel( false );
   close_emoji_suggest( );

   show_thread_view( );

   document.getElementById( "thread_name" ).textContent = g_room_name;

   update_thread_meta( );

   document.getElementById( "message_list" ).textContent = "";

   render_rooms( true );
   render_composer( );
   apply_posting_rules( );

   // NOTE: A token is only supplied when joining from an invitation. It goes
   // in "from", which is how the server recognises a join token.
   load_messages( ( token !== "" ) ? ( "from=" + token ) : "from=0", true );
}

function find_room( room )
{
   for( var i = 0; i < g_rooms.length; i++ )
   {
      if( g_rooms[ i ].room === room )
         return g_rooms[ i ];
   }

   return null;
}

// ====================================================================
// Invitations
// ====================================================================

function find_invitation( room )
{
   for( var i = 0; i < g_invitations.length; i++ )
   {
      if( g_invitations[ i ].room === room )
         return g_invitations[ i ];
   }

   return null;
}

// NOTE: A selected invitation shows a join prompt rather than the room - the server refuses
// a non-member's read, so nothing is fetched until the user joins.
function select_invitation( room )
{
   var invite = find_invitation( room );

   if( invite === null )
      return;

   g_room = "";
   g_start_point = "";
   g_edit_unique = "";
   g_edit_private = false;
   g_recipients = [ ];
   g_members = [ ];

   g_selected_invite = room;

   disarm_decline( );

   document.getElementById( "invite_title" ).textContent = invite.name;
   document.getElementById( "invite_text" ).textContent = invite.inviter + " invited you to join this room.";

   if( open_drawer( ) === "rail" )
      close_drawers( false );

   show_thread_view( );
   update_thread_meta( );

   render_members( true );
   render_rooms( true );
}

// NOTE: A decline cannot be taken back - the server refuses any later invitation to that
// room for this user (Ian, 2026-09-26) - so the first click only arms it, and says so.
const c_decline_confirm_ms = 5000;

var g_decline_armed = 0;

function disarm_decline( )
{
   g_decline_armed = 0;

   document.getElementById( "invite_decline" ).textContent = "Decline";
   document.getElementById( "invite_decline_note" ).hidden = true;
}

async function do_decline_invitation( )
{
   var invite = find_invitation( g_selected_invite );

   if( invite === null )
      return;

   if( ( g_decline_armed === 0 ) || ( Date.now( ) - g_decline_armed > c_decline_confirm_ms ) )
   {
      g_decline_armed = Date.now( );

      document.getElementById( "invite_decline" ).textContent = "Decline for good";
      document.getElementById( "invite_decline_note" ).hidden = false;

      window.setTimeout( function( )
      {
         if( ( g_decline_armed !== 0 ) && ( Date.now( ) - g_decline_armed >= c_decline_confirm_ms ) )
            disarm_decline( );
      }, c_decline_confirm_ms );

      return;
   }

   disarm_decline( );

   // NOTE: "messages delete <room>" - for someone only invited it declines; for a member
   // it would leave the room.
   var response = await new Promise( function( resolve )
   {
      serialised( function( )
      {
         return ciyam.delete_message_room( invite.room, function( r ) { resolve( String( r ) ); } );
      } );
   } );

   if( is_error_response( response ) )
   {
      show_alert( error_text( response ), "is-error" );

      return;
   }

   // NOTE: Drop it here at once. The server now answers it as ":ignore", so the next read of
   // Administration keeps it gone; until then the old ":invite" must not bring it back.
   g_invite_messages = g_invite_messages.filter( function( message )
   {
      return !( message.event && ( message.event.verb === "invite" ) && ( message.event.room === invite.room ) );
   } );

   g_invite_total = -1;
   g_selected_invite = "";

   refresh_invitations( );
   show_thread_view( );
   render_rooms( true );

   show_alert( "Declined the invitation to " + invite.name + ".", "is-info" );

   load_rooms( );
}

function do_join_invitation( )
{
   var invite = find_invitation( g_selected_invite );

   if( invite === null )
      return;

   // NOTE: The token goes in "from", which is how the server recognises a join. Once the
   // listing includes the room, the invitation drops out of the rail by itself.
   select_room( invite.room, invite.token );

   load_rooms( );
}

// NOTE: Only for someone who cannot see Administration - admin reads it as a room, and a
// background read would mark its messages read behind their back. Quiet, like polling, so
// it stays out of the console's log.
//
// NOTE: Whole ("from=0") the first time; after that only what is new - no "from" - added to
// what is held, and whole again only when "needs_full_read( )" says a read of what is new
// cannot be trusted (Ian, 2026-09-27: reading it whole every time was wasteful).
var g_invite_read_whole = false;

function check_invitations( whole )
{
   g_invite_read_whole = !!whole || ( g_invite_messages.length === 0 );

   var options = g_invite_read_whole ? "from=0" : "";

   g_in_poll = true;

   serialised( function( )
   {
      return ciyam.fetch_messages( c_starting_room_no, options, on_invitations_response );
   } );

   g_in_poll = false;
}

function on_invitations_response( response )
{
   var result = parse_fetch_response( response );

   // NOTE: Forget the total, so the next poll tries again.
   if( result.error !== "" )
   {
      g_invite_total = -1;

      return;
   }

   if( g_invite_read_whole )
      g_invite_messages = result.messages;
   else if( needs_full_read( result.messages ) )
   {
      check_invitations( true );

      return;
   }
   else
      g_invite_messages = merge_new_messages( g_invite_messages, result.messages );

   refresh_invitations( );
}

function refresh_invitations( )
{
   refresh_announcements( );

   g_invitations = pending_invitations( g_invite_messages, g_rooms, ciyam.username );

   // NOTE: The invitation on screen can be taken up elsewhere - another tab, say.
   if( ( g_selected_invite !== "" ) && ( find_invitation( g_selected_invite ) === null ) )
   {
      g_selected_invite = "";

      show_thread_view( );
   }

   render_rooms( );

   // NOTE: A decline can arrive without the room list changing, so the Join links on screen
   // are brought up to date here too.
   refresh_invite_actions( );
}

// NOTE: Deliberately synchronous, and the dialog is shown before the invitee list is
// populated. Previously this was an async handler that built the list first - so any
// failure in that step rejected the promise, which an inline onclick discards silently,
// and the dialog simply never appeared.
function do_open_create_room( )
{
   g_dialog_mode = "create";

   document.getElementById( "room_dialog_title" ).textContent = "Create a room";
   document.getElementById( "room_dialog_submit" ).textContent = "Create";
   document.getElementById( "room_dialog_name" ).value = "";
   document.getElementById( "room_dialog_name_row" ).hidden = false;
   document.getElementById( "room_dialog_invitees_row" ).hidden = false;

   set_error( "room_dialog_error", "" );

   // NOTE: Emptied before the list is drawn so ticks from a previous room are not
   // carried over - render_invitees deliberately preserves whatever is already ticked
   // when it refreshes underneath the user.
   document.getElementById( "room_dialog_invitees" ).textContent = "";

   document.getElementById( "room_dialog" ).hidden = false;
   document.getElementById( "room_dialog_name" ).focus( );

   load_invitees( );
}

// NOTE: Rendered from the user list the poll already collects, so the picker appears
// immediately. Previously this issued its own request, which queued behind whatever the
// poll had in flight and took seconds to appear.
function load_invitees( )
{
   render_invitees( ( g_known_users.length > 0 ) ? g_known_users : g_members );
}

function do_open_rename_room( )
{
   g_dialog_mode = "rename";

   document.getElementById( "room_dialog_title" ).textContent = "Rename this room";
   document.getElementById( "room_dialog_submit" ).textContent = "Rename";
   document.getElementById( "room_dialog_name" ).value = g_room_name;
   document.getElementById( "room_dialog_name_row" ).hidden = false;
   document.getElementById( "room_dialog_invitees_row" ).hidden = true;

   set_error( "room_dialog_error", "" );

   document.getElementById( "room_dialog" ).hidden = false;
   document.getElementById( "room_dialog_name" ).focus( );
}

// ====================================================================
// Announcements - a prototype
// ====================================================================

// NOTE: Admin's messages in Administration, from the same background read as invitations.
// Admin reads Administration as a room, so sees none of this.
function refresh_announcements( )
{
   var list = ciyam.is_admin ? [ ] : pending_announcements( g_invite_messages, read_dismissed( ) );

   var signature = announcements_signature( list );

   if( signature === g_announcements_drawn )
      return;

   g_announcements = list;
   g_announcements_drawn = signature;

   render_announcements( );
}

function announcements_signature( list )
{
   return list.map( function( m ) { return m.unique + ( m.edited ? "*" : "" ) + m.text; } ).join( "\n" );
}

function dismissed_key( )
{
   return c_storage_dismissed_prefix + ciyam.access;
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

// NOTE: One card, as the strip shows it and as admin's preview shows it. "when" is the text
// for its time; "private" marks one sent to named people.
function build_announcement_card( text, when, when_title, is_private )
{
   var node = document.getElementById( "tpl_announcement" ).content.cloneNode( true ).querySelector( ".chat-announcement" );

   var stamp = node.querySelector( ".chat-announcement-when" );

   stamp.textContent = when;
   stamp.title = when_title || "";

   node.querySelector( ".chat-announcement-text" ).textContent = text;

   // NOTE: One admin sent to named people, rather than to everyone.
   if( is_private )
      node.querySelector( ".chat-announcement-label" ).textContent = "Announcement · to you";

   return node;
}

// NOTE: Past two, the stack is collapsed to the newest two with "Show N more" - and with two
// or more there is a heading with the count and "Dismiss all". "announcement_stack( )" decides.
var g_announcements_expanded = false;

var g_dismiss_all_timer = null;

function render_announcements( )
{
   var host = document.getElementById( "announcements" );

   host.textContent = "";

   disarm_dismiss_all( );

   if( g_announcements.length <= c_announcements_collapsed )
      g_announcements_expanded = false;

   if( g_announcements.length >= 2 )
   {
      var head = document.createElement( "div" );

      head.className = "chat-announcements-head";

      var count = document.createElement( "span" );

      count.className = "chat-announcements-count";
      count.textContent = "Announcements · " + g_announcements.length;

      var all = document.createElement( "button" );

      all.type = "button";
      all.id = "announcements_dismiss_all";
      all.className = "chat-btn chat-btn--small chat-announcements-all";
      all.textContent = "Dismiss all";
      all.title = "Dismiss every announcement - they will not be shown again on this browser";

      all.addEventListener( "click", do_dismiss_all_announcements );

      head.appendChild( count );
      head.appendChild( all );

      host.appendChild( head );
   }

   var stack = announcement_stack( g_announcements, g_announcements_expanded );

   stack.shown.forEach( function( message )
   {
      var node = build_announcement_card( message.text,
       day_label( message.unique ) + " " + unique_to_time( message.unique ).substring( 0, 5 ),
       unique_to_full( message.unique ), message.private );

      var ok = node.querySelector( ".chat-announcement-ok" );

      ok.title = "Dismiss - it will not be shown again on this browser";

      ok.addEventListener( "click", function( ) { do_dismiss_announcement( message.unique ); } );

      host.appendChild( node );
   } );

   if( ( stack.more > 0 ) || g_announcements_expanded )
   {
      var toggle = document.createElement( "button" );

      toggle.type = "button";
      toggle.id = "announcements_more";
      toggle.className = "chat-announcements-more";
      toggle.textContent = g_announcements_expanded ? "Show fewer" : ( "Show " + stack.more + " more" );
      toggle.setAttribute( "aria-expanded", g_announcements_expanded ? "true" : "false" );

      toggle.addEventListener( "click", function( )
      {
         g_announcements_expanded = !g_announcements_expanded;

         render_announcements( );

         document.getElementById( "announcements_more" ).focus( );
      } );

      host.appendChild( toggle );
   }

   show_thread_view( );
}

// NOTE: Two clicks, as Decline is - they go for good on this browser, and a slip of the pointer
// should not lose ones not yet read.
function do_dismiss_all_announcements( )
{
   var button = document.getElementById( "announcements_dismiss_all" );

   if( g_dismiss_all_timer === null )
   {
      button.textContent = "Dismiss all " + g_announcements.length + "?";
      button.classList.add( "is-armed" );

      g_dismiss_all_timer = window.setTimeout( disarm_dismiss_all, 5000 );

      return;
   }

   disarm_dismiss_all( );

   var dismissed = read_dismissed( );

   g_announcements.forEach( function( message ) { dismissed = add_dismissed( dismissed, message.unique ); } );

   try
   {
      localStorage.setItem( dismissed_key( ), JSON.stringify( dismissed ) );
   }
   catch( e )
   {
   }

   g_announcements = [ ];
   g_announcements_drawn = announcements_signature( g_announcements );

   render_announcements( );
}

function disarm_dismiss_all( )
{
   if( g_dismiss_all_timer !== null )
   {
      window.clearTimeout( g_dismiss_all_timer );

      g_dismiss_all_timer = null;
   }

   var button = document.getElementById( "announcements_dismiss_all" );

   if( button !== null )
   {
      button.textContent = "Dismiss all";
      button.classList.remove( "is-armed" );
   }
}

// NOTE: What admin is about to post in Administration, shown as everyone else will see it -
// the card, and who will see it. Only for admin, in Administration, with something typed.
function update_announcement_preview( )
{
   var host = document.getElementById( "announcement_preview" );
   var input = document.getElementById( "composer_input" );

   var text = input.value.trim( );

   var showing = ciyam.is_admin && is_starting_room( g_room ) && ( text !== "" ) && !input.disabled;

   host.hidden = !showing;

   var holder = document.getElementById( "announcement_preview_card" );

   holder.textContent = "";

   if( !showing )
      return;

   var others = g_recipients.filter( function( name ) { return name !== ciyam.username; } );

   var card = build_announcement_card( text, "now", "", others.length > 0 );

   card.querySelector( ".chat-announcement-ok" ).hidden = true;

   holder.appendChild( card );

   set_text( document.getElementById( "announcement_preview_audience" ), announcement_audience( g_recipients, ciyam.username ) );
}

// NOTE: Remembered in this browser only - on another device it shows again.
function do_dismiss_announcement( unique )
{
   try
   {
      localStorage.setItem( dismissed_key( ), JSON.stringify( add_dismissed( read_dismissed( ), unique ) ) );
   }
   catch( e )
   {
   }

   // NOTE: Hidden at once even if the browser would not save it - it then returns next time.
   g_announcements = g_announcements.filter( function( m ) { return m.unique !== unique; } );
   g_announcements_drawn = announcements_signature( g_announcements );

   render_announcements( );
}

// NOTE: Invites to the room already open, using the "for" option on a messages PUT that
// Ian added on 2026-09-24. Reuses the create dialog without its name field, and offers
// only people who are not already members - inviting a member again would just issue a
// token they have no use for.
function do_open_invite( )
{
   if( ( g_room === "" ) || is_starting_room( g_room ) )
      return;

   g_dialog_mode = "invite";

   document.getElementById( "room_dialog_title" ).textContent = "Invite to " + g_room_name;
   document.getElementById( "room_dialog_submit" ).textContent = "Invite";
   document.getElementById( "room_dialog_name_row" ).hidden = true;
   document.getElementById( "room_dialog_invitees_row" ).hidden = false;

   set_error( "room_dialog_error", "" );

   document.getElementById( "room_dialog_invitees" ).textContent = "";

   document.getElementById( "room_dialog" ).hidden = false;

   var members = { };

   for( var i = 0; i < g_members.length; i++ )
      members[ g_members[ i ].name ] = true;

   var candidates = ( g_known_users.length > 0 ) ? g_known_users : [ ];

   render_invitees( candidates.filter( function( user ) { return !members[ user.name ]; } ) );

   if( candidates.length > 0 )
   {
      var first = document.querySelector( "#room_dialog_invitees .chat-invitee-check" );

      if( first !== null )
         first.focus( );
   }

   mark_already_invited( g_room );
}

// NOTE: The server accepts the same invitation twice without complaint, so the picker
// stops it instead. This user's Administration room holds an ":issued" receipt for every
// invitation they sent; anyone already invited to this room is shown but cannot be ticked.
// The dialog is drawn first and marked when the receipts arrive, so a slow fetch never
// delays it. Invitations sent by someone else are not visible here - see ISS-017.
function mark_already_invited( room )
{
   serialised( function( )
   {
      return ciyam.fetch_messages( c_starting_room_no, "from=0", function( response )
      {
         // NOTE: The dialog may have been closed or switched to another room meanwhile.
         if( is_error_response( response ) || ( g_dialog_mode !== "invite" ) || ( g_room !== room )
          || document.getElementById( "room_dialog" ).hidden )
            return;

         var invited = invited_to_room( parse_fetch_response( response ).messages, room );

         var checks = document.querySelectorAll( "#room_dialog_invitees .chat-invitee-check" );

         for( var i = 0; i < checks.length; i++ )
         {
            var box = checks[ i ];

            if( !invited[ box.value ] )
               continue;

            box.checked = false;
            box.disabled = true;

            var row = box.parentNode;

            row.classList.add( "is-invited" );
            row.title = "Already invited to this room";

            var tag = document.createElement( "span" );

            tag.className = "chat-invitee-tag";
            tag.textContent = "invited";

            row.appendChild( tag );
         }
      } );
   } );
}

function ticked_invitees( )
{
   var checks = document.querySelectorAll( "#room_dialog_invitees .chat-invitee-check" );

   var names = [ ];

   for( var i = 0; i < checks.length; i++ )
   {
      if( checks[ i ].checked )
         names.push( checks[ i ].value );
   }

   return names;
}

async function submit_invite( )
{
   var names = ticked_invitees( );

   if( names.length === 0 )
   {
      set_error( "room_dialog_error", "Choose at least one person to invite." );

      return;
   }

   ciyam.error = "";

   var room = g_room;

   await ciyam.update_message_room( room, "for=" + names.join( "," ), function( response )
   {
      if( is_error_response( response ) )
      {
         set_error( "room_dialog_error", error_text( response ) );

         return;
      }

      do_close_room_dialog( );

      show_alert( "Invited " + names.join( ", " ) + " to " + g_room_name + ".", "is-info" );

      // NOTE: The invitation is posted as a system event, so a reload shows the
      // receipt without waiting for the next poll.
      if( room === g_room )
         load_messages( "from=0", true );
   } );

   if( ciyam.error !== "" )
      set_error( "room_dialog_error", ciyam.error );
}

function render_invitees( members )
{
   var host = document.getElementById( "room_dialog_invitees" );

   if( host === null )
      return;

   // NOTE: Keep anything already ticked when the list is refreshed underneath.
   var ticked = { };

   var existing = host.querySelectorAll( ".chat-invitee-check" );

   for( var t = 0; t < existing.length; t++ )
   {
      if( existing[ t ].checked )
         ticked[ existing[ t ].value ] = true;
   }

   host.textContent = "";

   var template = document.getElementById( "tpl_invitee" );

   if( ( members === null ) || ( members === undefined ) )
      members = [ ];

   if( members.length === 0 )
   {
      host.textContent = "No other users to invite.";

      return;
   }

   for( var i = 0; i < members.length; i++ )
   {
      var member = members[ i ];

      if( member.name === ciyam.username )
         continue;

      var node = template.content.cloneNode( true );

      node.querySelector( ".chat-invitee-name" ).textContent = member.name;

      var box = node.querySelector( ".chat-invitee-check" );

      box.value = member.name;
      box.checked = ( ticked[ member.name ] === true );

      host.appendChild( node );
   }
}

// NOTE: Bound to "blur" rather than "input". Validating on every keystroke meant the
// error appeared while the name was still half typed - a space between words fails the
// rules until the next word is started. Submitting validates as well, so nothing gets
// through unchecked. The value is trimmed first because that is what is submitted.
function do_validate_room_name( )
{
   var name = document.getElementById( "room_dialog_name" ).value.trim( );

   if( ( name !== "" ) && !is_valid_room_name( name ) )
      set_error( "room_dialog_error", "That name does not satisfy the room name rules." );
   else
      set_error( "room_dialog_error", "" );
}

// NOTE: Typing clears a standing error so the message goes away as it is corrected,
// without a new one appearing mid-word.
function do_clear_room_name_error( )
{
   set_error( "room_dialog_error", "" );
}

function do_close_room_dialog( )
{
   document.getElementById( "room_dialog" ).hidden = true;
}

async function do_submit_room_dialog( )
{
   // NOTE: Invite has no name field, so it is handled before the name is validated.
   if( g_dialog_mode === "invite" )
      return submit_invite( );

   var name = document.getElementById( "room_dialog_name" ).value.trim( );

   if( !is_valid_room_name( name ) )
   {
      set_error( "room_dialog_error", "That name does not satisfy the room name rules." );

      return;
   }

   // NOTE: Renaming moved to PUT with a "name" option. The POST form still works but is
   // expected to be withdrawn.
   if( g_dialog_mode === "rename" )
   {
      ciyam.error = "";

      await ciyam.update_message_room( g_room, "name=" + name, function( response )
      {
         if( is_error_response( response ) )
            set_error( "room_dialog_error", error_text( response ) );
         else
         {
            do_close_room_dialog( );

            g_room_name = name;

            document.getElementById( "thread_name" ).textContent = name;

            load_rooms( );
         }
      } );

      if( ciyam.error !== "" )
         set_error( "room_dialog_error", ciyam.error );

      return;
   }

   var options = "";

   {
      var checks = document.querySelectorAll( "#room_dialog_invitees .chat-invitee-check" );

      var invitees = [ ];

      for( var i = 0; i < checks.length; i++ )
      {
         if( checks[ i ].checked )
            invitees.push( checks[ i ].value );
      }

      options = ( invitees.length > 0 ) ? ( "for=" + invitees.join( "," ) + ";text=" + name )
       : ( "text=" + name );
   }

   ciyam.error = "";

   await ciyam.create_message( c_lobby_room, options, function( response )
   {
      if( is_error_response( response ) )
      {
         set_error( "room_dialog_error", error_text( response ) );

         return;
      }

      do_close_room_dialog( );

      if( g_dialog_mode === "create" )
         open_new_room( response );
      else
      {
         g_room_name = name;

         document.getElementById( "thread_name" ).textContent = name;

         load_rooms( );
      }
   } );

   if( ciyam.error !== "" )
      set_error( "room_dialog_error", ciyam.error );
}

// NOTE: Switches to the room just created, so the user ends up in it rather than having
// to find it in the rail. The create response is "<room>-<join token>" in text format.
//
// There is deliberately no "room created" dialog any more. It showed the token and a QR
// code and said anyone holding it could enter - but tokens are now derived per user, so
// that token was the owner's own and should never have been shared. Invitations are how
// people get in. Removed at Ian's suggestion, 2026-09-24.
async function open_new_room( response )
{
   var value = String( response ).trim( );

   if( value.charAt( 0 ) === ":" )
      value = value.substring( 1 );

   var pos = value.indexOf( "-" );

   if( pos < 0 )
      return;

   var room = value.substring( 0, pos );
   var token = value.substring( pos + 1 );

   // NOTE: The rail has to know about the room before it can be shown as selected.
   await load_rooms( );

   select_room( room, token );
}

// NOTE: A registered account's PIN is issued by the server and shown nowhere else. Miss
// it and the account is unreachable - there is no recovery short of an administrator
// listing the users. So it is shown on a dialog that has to be dismissed, and the PIN is
// also added to the saved list regardless of the "remember" choice, see
// "apply_retain_choice".
function show_new_pin( pin, username )
{
   document.getElementById( "pin_value" ).textContent = pin;

   set_text( document.getElementById( "pin_username" ),
    "Signed in as " + username + ". Keep the PIN with the password you just chose." );

   document.getElementById( "pin_dialog" ).hidden = false;
}

function do_copy_pin( )
{
   var value = document.getElementById( "pin_value" ).textContent;

   if( navigator.clipboard )
      navigator.clipboard.writeText( value );
}

function do_close_pin_dialog( )
{
   document.getElementById( "pin_dialog" ).hidden = true;
}

// ====================================================================
// Messages
// ====================================================================

function load_messages( options, replace )
{
   if( g_room === "" )
      return Promise.resolve( );

   // NOTE: Captured so a response arriving after the user has moved on is discarded
   // rather than written into whatever room is now open.
   var asked_for = g_room;

   // NOTE: "extra=TIME" puts each member's read point on the member line - the read markers.
   var asked = ( options ? options + ";" : "" ) + "extra=TIME";

   return serialised( function( )
   {
      return ciyam.fetch_messages( asked_for, asked, function( response )
      {
         on_messages_response( response, asked_for, replace );
      } );
   } );
}

function on_messages_response( response, asked_for, replace )
{
   {
      var result = parse_fetch_response( response );

      // NOTE: The first room's messages are what the loading state was waiting for -
      // including when they fail, so an error is shown rather than a spinner forever.
      if( g_first_load && replace )
         end_first_load( );

      g_last_poll = Date.now( );

      // NOTE: A time-out has its own notice, from 'note_timeout'.
      if( result.error !== "" )
      {
         if( !is_timeout_response( response ) )
            show_alert( result.error, "is-error", true );

         return;
      }

      // NOTE: Defensive - room rows mean this is an entrance listing, not ours.
      if( result.rooms.length > 0 )
         return;

      clear_loading_alert( );

      // NOTE: The user changed room while this was in flight.
      if( asked_for !== g_room )
         return;

      g_members = apply_presence( result.members );

      render_members( );

      if( replace )
         document.getElementById( "message_list" ).textContent = "";

      append_messages( result.messages );

      // NOTE: Admin sees Administration as a room, so their invitations come from here
      // rather than from a background read.
      if( is_starting_room( asked_for ) )
      {
         g_invite_messages = replace ? result.messages : g_invite_messages.concat( result.messages );

         refresh_invitations( );
      }

      var next = next_start_point( result.messages );

      if( next !== "" )
         g_start_point = next;

      render_seen_markers( );

      update_thread_meta( );
   }
}

// NOTE: "Seen by ..." under the last message each other member has read, from their read
// points ("seen_by( )" in "chat_parse.js"). Redrawn on every read of the room, as read points
// only move forward and a marker moves down with them.
function render_seen_markers( )
{
   var list = document.getElementById( "message_list" );

   var was_at_end = ( list.scrollTop + list.clientHeight >= list.scrollHeight - 40 );

   list.querySelectorAll( ".chat-seen" ).forEach( function( marker ) { marker.remove( ); } );

   // NOTE: The list's own rows only - an own message's Edit button carries its unique too, and
   // counting it put a second marker under every message the user had sent.
   var rows = Array.from( list.querySelectorAll( ":scope > [data-unique]" ) );

   var marks = seen_by( rows.map( function( row ) { return row.dataset.unique; } ), g_members, ciyam.username );

   rows.forEach( function( row )
   {
      var names = marks[ row.dataset.unique ];

      if( !names )
         return;

      var marker = document.createElement( "div" );

      marker.className = "chat-seen";
      marker.textContent = "Seen by " + names.join( ", " );
      marker.title = names.join( ", " ) + ( ( names.length === 1 ) ? " has" : " have" ) + " read up to here";

      // NOTE: The day of the row it follows - "last_rendered_day( )" reads the last element.
      marker.dataset.day = row.dataset.day || "";

      row.after( marker );
   } );

   if( was_at_end )
      list.scrollTop = list.scrollHeight;
}

// NOTE: Every row carries its day, so the last one in the list says which day the list
// has reached. That works for a full redraw and for polling appending a handful of new
// rows, without having to keep the day in a variable that the two paths could disagree
// about.
function build_day_divider( unique )
{
   var node = document.getElementById( "tpl_day" ).content.cloneNode( true );

   set_text( node.querySelector( ".chat-day-label" ), day_label( unique ) );

   return node;
}

function last_rendered_day( list )
{
   var last = list.lastElementChild;

   return ( last === null ) ? "" : ( last.dataset.day || "" );
}

// NOTE: Sending returns the new message, and a poll already in flight was issued with an
// earlier "from" - so it returns that same message and it gets appended twice. Rather
// than trying to order the two, appending is made idempotent: a unique already on screen
// is skipped. The DOM is asked directly so this cannot drift out of step with a separate
// record of what has been drawn. Reported by Ian, 2026-09-23.
function already_rendered( list, unique )
{
   if( !unique )
      return false;

   return ( list.querySelector( "[data-unique=\"" + unique + "\"]" ) !== null );
}

function append_messages( messages )
{
   var list = document.getElementById( "message_list" );

   var was_at_end = ( list.scrollTop + list.clientHeight >= list.scrollHeight - 40 );

   for( var i = 0; i < messages.length; i++ )
   {
      var entry = messages[ i ];

      if( already_rendered( list, entry.unique ) )
         continue;

      var key = day_key( entry.unique );

      // NOTE: A divider goes in whenever the day changes, including before the first row.
      // An entry with an unreadable unique has no day, so it simply joins whatever day is
      // current rather than forcing a blank divider.
      if( ( key !== "" ) && ( key !== last_rendered_day( list ) ) )
      {
         var divider = build_day_divider( entry.unique );

         divider.querySelector( ".chat-day" ).dataset.day = key;

         list.appendChild( divider );
      }

      var row = ( entry.kind === "system" ) ? build_notice( entry ) : build_message( entry );

      var element = row.firstElementChild;

      element.dataset.day = ( key !== "" ) ? key : last_rendered_day( list );

      if( entry.unique )
         element.dataset.unique = entry.unique;

      list.appendChild( row );
   }

   if( was_at_end )
      list.scrollTop = list.scrollHeight;
}

function build_message( entry )
{
   var node = document.getElementById( "tpl_message" ).content.cloneNode( true );

   var row = node.querySelector( ".chat-message" );

   var time = row.querySelector( ".chat-message-time" );

   time.textContent = unique_to_time( entry.unique );

   // NOTE: The row shows the time only - the divider above it carries the day. The full
   // stamp is on hover, for when a message is far from its divider.
   time.title = unique_to_full( entry.unique );

   var name = row.querySelector( ".chat-message-sender-name" );

   name.textContent = entry.sender;
   name.style.color = "var(--color-sender-" + sender_colour_index( entry.sender ) + ")";

   row.querySelector( ".chat-message-body" ).textContent = entry.text;

   if( entry.edited )
   {
      row.querySelector( ".chat-edit-mark" ).hidden = false;
      row.querySelector( ".chat-edit-tag" ).hidden = false;
   }

   if( entry.private )
   {
      var label = private_label( entry );

      var mark = row.querySelector( ".chat-private" );

      mark.hidden = false;
      mark.title = label.title;

      mark.querySelector( ".chat-private-label" ).textContent = label.text;

      row.classList.add( "is-private" );
   }

   row.classList.toggle( "is-own", ( entry.sender === ciyam.username ) );

   // NOTE: Private messages too, since 2026-09-28 - the edit says it is private
   // ("edit_for_value( )"), and the server keeps it so.
   if( entry.sender === ciyam.username )
   {
      var edit = row.querySelector( ".chat-message-edit" );

      edit.hidden = false;
      edit.dataset.unique = entry.unique;
      edit.dataset.text = entry.text;
      edit.dataset.private = entry.private ? "1" : "";

      edit.addEventListener( "click", function( event )
      {
         var data = event.currentTarget.dataset;

         begin_edit( data.unique, data.text, data.private === "1" );
      } );
   }

   return node;
}

function room_label( room )
{
   var entry = find_room( room );

   return ( entry && entry.name ) ? ( entry.name + " (#" + room + ")" ) : ( "#" + room );
}

function build_notice( entry )
{
   var node = document.getElementById( "tpl_notice" ).content.cloneNode( true );

   var event = entry.event;

   // NOTE: System events are timed like any other message. They were previously the one
   // kind of row with no time at all, which made the Administration room - almost entirely
   // joins and invitations - read as undated.
   var time = node.querySelector( ".chat-notice-time" );

   time.textContent = unique_to_time( entry.unique );
   time.title = unique_to_full( entry.unique );

   // NOTE: Plain words - "verify-a joined", "admin sent a private message to verify-a" -
   // from "describe_event( )" in "chat_parse.js", where every verb's wording is tested.
   // Rooms are named as this user knows them.
   node.querySelector( ".chat-notice-who" ).textContent = entry.sender + " ";
   node.querySelector( ".chat-notice-verb" ).textContent = describe_event( event, room_label );
   node.querySelector( ".chat-notice-detail" ).textContent = "";

   // NOTE: An invitation carries a join token, so it can be acted on directly.
   if( ( event.verb === "invite" ) && event.room && event.token )
   {
      var action = node.querySelector( ".chat-notice-action" );

      action.hidden = false;
      action.dataset.room = event.room;
      action.dataset.token = event.token;

      action.addEventListener( "click", function( ev )
      {
         var room = ev.currentTarget.dataset.room;

         // NOTE: Once the room has been joined the token is no longer needed, and
         // passing it again would be a second join attempt.
         if( is_joined( room ) )
            select_room( room, "" );
         else
         {
            select_room( room, ev.currentTarget.dataset.token );

            // NOTE: Joining changes what rooms are listed, so refresh rather than
            // waiting up to a full poll for the rail and this button to catch up.
            load_rooms( );
         }
      } );

      apply_invite_state( action );
   }

   return node;
}

function is_joined( room )
{
   return ( find_room( room ) !== null );
}

// NOTE: Whether an invitation has been taken up can change after the notice was drawn,
// so the state is applied from the current room list rather than fixed at render time.
function apply_invite_state( action )
{
   var joined = is_joined( action.dataset.room );

   // NOTE: Declined by this user - the server no longer rewrites such an invitation as
   // ":ignore" (ISS-028), so the chat reads it from the user's own ":reject" instead.
   var declined = !joined && !!declined_rooms( g_invite_messages, ciyam.username )[ action.dataset.room ];

   set_text( action, joined ? "Joined" : ( declined ? "Declined" : "Join" ) );

   action.classList.toggle( "is-joined", joined );
   action.classList.toggle( "is-declined", declined );

   action.disabled = declined;

   action.title = joined ? "Already a member - open this room"
    : ( declined ? "You declined this invitation" : "Accept this invitation and join the room" );
}

// NOTE: Called whenever the room list changes, so invitations already on screen pick up
// the fact that their room has since been joined.
function refresh_invite_actions( )
{
   var actions = document.querySelectorAll( ".chat-notice-action[data-room]" );

   for( var i = 0; i < actions.length; i++ )
      apply_invite_state( actions[ i ] );
}

function render_members( force )
{
   var signature = members_signature( );

   if( !force && ( signature === g_members_drawn ) )
      return;

   g_members_drawn = signature;

   var list = document.getElementById( "member_list" );

   var template = document.getElementById( "tpl_member" );

   reconcile_list( list, g_members,
    function( member )
    {
       return member.name;
    },
    function( member )
    {
       var node = template.content.cloneNode( true ).querySelector( ".chat-member" );

       node.querySelector( ".chat-member-name" ).textContent = member.name;

       node.dataset.name = member.name;

       node.addEventListener( "click", function( event )
       {
          add_recipient( event.currentTarget.dataset.name );
       } );

       return node;
    },
    function( node, member )
    {
       var count = node.querySelector( ".chat-member-count" );

       var text = "+" + member.sessions;

       // NOTE: Only touch the DOM when the value actually differs - assigning the same
       // text still counts as a mutation for the browser.
       if( count.textContent !== text )
          count.textContent = text;

       node.classList.toggle( "is-offline", !member.online );
       node.classList.toggle( "is-self", ( member.name === ciyam.username ) );

       node.querySelector( ".chat-dot" ).classList.toggle( "is-offline", !member.online );
    } );

   var head = document.getElementById( "presence_head" );

   var heading = "Members · " + g_members.length;

   if( head.textContent !== heading )
      head.textContent = heading;
}

// NOTE: What was last applied to the message box, so a listing that changes nothing about
// posting leaves it alone - applying a refusal empties the box.
var g_posting_applied = "";

function posting_key( status )
{
   return g_room + " " + status.can_post + " " + status.reason;
}

function sync_posting_rules( )
{
   if( g_room === "" )
      return;

   var status = posting_status( g_room, find_room( g_room ), ciyam.username, ciyam.is_admin );

   if( posting_key( status ) !== g_posting_applied )
      apply_posting_rules( );
}

// NOTE: The rules themselves live in "chat_parse.js" so they are covered by the regression
// tests - this only reflects the answer in the UI.
function apply_posting_rules( )
{
   var entry = find_room( g_room );

   var status = posting_status( g_room, entry, ciyam.username, ciyam.is_admin );

   g_posting_applied = posting_key( status );

   var input = document.getElementById( "composer_input" );
   var send = document.getElementById( "composer_send" );
   var scope = document.getElementById( "composer_scope" );
   var note = document.getElementById( "composer_note" );

   input.disabled = !status.can_post;
   send.disabled = !status.can_post;
   scope.disabled = !status.can_post;

   document.getElementById( "emoji_toggle" ).disabled = !status.can_post;

   if( !status.can_post )
   {
      close_emoji_panel( false );
      close_emoji_suggest( );
   }

   // NOTE: The standard note gives way on a phone; a reason the user cannot post does not.
   note.classList.toggle( "is-reason", !status.can_post );

   if( status.can_post )
   {
      note.textContent = "Ordinary chat only — system and slash messages are server-side.";

      // NOTE: Posting allowed again - the reason it was not goes, and the usual placeholder
      // comes back.
      if( input.dataset.refused === "1" )
      {
         input.dataset.refused = "";

         render_composer( );
      }
   }
   else
   {
      note.textContent = status.reason;

      input.dataset.refused = "1";

      input.value = "";
      input.placeholder = status.locked ? "🔒 " + status.reason : status.reason;

      resize_composer( );
   }
}

// NOTE: Fills the Room section of the right panel - the room's number, owner, who may post
// and how many messages, with the owner's Rename and Invite. For a selected invitation only
// the number and who sent it are known.
function update_thread_meta( )
{
   var entry = find_room( g_room );

   var invite = ( g_room === "" ) ? find_invitation( g_selected_invite ) : null;

   // NOTE: A room joined from an invitation is not in the listing at first, so its owner
   // is unknown until the listing catches up - and a rename or a change of owner reaches
   // here the same way.
   if( entry )
   {
      g_room_owner = entry.owner;

      if( entry.name && ( entry.name !== g_room_name ) )
      {
         g_room_name = entry.name;

         document.getElementById( "thread_name" ).textContent = g_room_name;
      }
   }

   var posting = { any: "Anyone", own: "Owner only", none: "Locked" };

   set_text( document.getElementById( "room_fact_name" ), invite ? invite.name : g_room_name );
   set_text( document.getElementById( "room_fact_number" ), "#" + ( invite ? invite.room : g_room ) );
   set_text( document.getElementById( "room_fact_owner" ), g_room_owner || "-" );
   set_text( document.getElementById( "room_fact_posts" ), entry ? ( posting[ entry.posts ] || entry.posts ) : "-" );
   set_text( document.getElementById( "room_fact_messages" ), entry ? String( entry.total ) : "-" );
   set_text( document.getElementById( "room_fact_inviter" ), invite ? invite.inviter : "" );

   document.getElementById( "room_fact_owner_row" ).hidden = !!invite;
   document.getElementById( "room_fact_posts_row" ).hidden = !!invite;
   document.getElementById( "room_fact_messages_row" ).hidden = !!invite;
   document.getElementById( "room_fact_inviter_row" ).hidden = !invite;

   var is_owner = !invite && ( ( g_room_owner === ciyam.username ) || ciyam.is_admin );

   document.getElementById( "owner_actions" ).hidden = !is_owner;

   // NOTE: The owner's choice replaces the plain text - left alone while a change is on its
   // way, so the listing catching up does not flick it back.
   var changeable = !invite && can_change_posting( g_room, entry, ciyam.username, ciyam.is_admin );

   var select = document.getElementById( "room_posts_select" );

   select.hidden = !changeable;

   document.getElementById( "room_fact_posts" ).hidden = changeable;

   if( changeable && !select.disabled )
      select.value = entry.posts || "any";

   // NOTE: Nobody can be invited to or removed from the Administration room - it is
   // joined automatically - so the control is absent there rather than present and
   // failing. Ian raised this against 0000001 specifically.
   document.getElementById( "owner_invite" ).hidden = is_starting_room( g_room );

   document.getElementById( "presence_foot" ).textContent =
    "poll every " + ( c_poll_interval / 1000 ) + "s"
    + ( g_start_point ? ( "\nfrom=" + g_start_point ) : "" )
    + "\nno push · no typing state";
}

// NOTE: Who may post, changed by the owner or admin from Room Details. The server posts an
// ":allows set to ..." notice into the room, and the listing follows on the next refresh.
async function do_change_posting( )
{
   var select = document.getElementById( "room_posts_select" );

   var room = g_room;
   var entry = find_room( room );
   var value = posts_request_value( select.value );

   if( !entry || ( value === "" ) || ( select.value === entry.posts ) )
      return;

   select.disabled = true;

   ciyam.error = "";

   var answer = await serialised( function( )
   {
      return new Promise( function( resolve )
      {
         ciyam.update_message_room( room, "posts=" + value, resolve );
      } );
   } );

   select.disabled = false;

   if( is_error_response( answer ) || ( ciyam.error !== "" ) )
   {
      show_alert( error_text( is_error_response( answer ) ? answer : ciyam.error ), "is-error" );

      select.value = entry.posts || "any";

      return;
   }

   // NOTE: Straight away rather than on the next listing, so the message box follows at once.
   entry.posts = select.value;

   apply_posting_rules( );
   update_thread_meta( );

   load_rooms( );
}

// ====================================================================
// Composing
// ====================================================================

function do_composer_key( event )
{
   if( emoji_suggest_key( event ) )
      return false;

   if( ( event.key === "Enter" ) && !event.shiftKey )
   {
      event.preventDefault( );

      do_send( );

      return false;
   }

   if( event.key === "Escape" && ( g_edit_unique !== "" ) )
   {
      do_cancel_edit( );

      return false;
   }

   // NOTE: Escape in an empty box leaves private mode - a quick way back to everyone, and
   // only when nothing typed could be lost or sent to the wrong people.
   if( ( event.key === "Escape" ) && ( g_recipients.length > 0 ) && ( event.target.value === "" ) )
   {
      g_recipients = [ ];

      render_composer( );

      event.stopPropagation( );

      return false;
   }

   return true;
}

async function do_send( )
{
   var input = document.getElementById( "composer_input" );

   var text = input.value.trim( );

   if( ( text === "" ) || ( g_room === "" ) )
      return;

   close_emoji_panel( false );
   close_emoji_suggest( );

   // NOTE: Enter sends as well as the button, so the length is checked here too.
   if( message_too_long( ) )
   {
      update_composer_count( );

      return;
   }

   var options = "";

   // NOTE: The server drops a lone backslash as an escape, so each is doubled to arrive as
   // typed - see "escape_message_text( )" in "chat_parse.js". Line breaks go as they are.
   var sent = escape_message_text( text );

   if( g_edit_unique !== "" )
      options = "for=" + edit_for_value( g_edit_unique, g_edit_private ) + ";text=" + sent;
   else if( g_recipients.length > 0 )
      options = "for=" + with_sender( g_recipients, ciyam.username ).join( "," ) + ";text=" + sent;
   else
      options = "text=" + sent;

   input.value = "";
   input.disabled = true;

   resize_composer( );
   update_announcement_preview( );

   ciyam.error = "";

   var was_edit = ( g_edit_unique !== "" );

   var failed = false;

   // NOTE: What is needed to tell, after a time-out, whether this arrived after all - see
   // "arrived_after_timeout( )". The start point is the server's, not the browser's clock.
   var pending = { me: ciyam.username, text: text, after: g_start_point, edit: g_edit_unique, room: g_room };

   var timed_out = false;

   await ciyam.create_message( g_room, options, function( response )
   {
      if( is_timeout_response( response ) )
      {
         failed = true;
         timed_out = true;

         return;
      }

      if( is_error_response( response ) )
      {
         failed = true;

         show_alert( error_text( response ), "is-error" );

         return;
      }

      var result = parse_fetch_response( response );

      g_members = apply_presence( result.members );

      render_members( );

      // NOTE: An edit rewrites an existing line, so the whole thread is
      // reloaded rather than appended to.
      if( was_edit )
         load_messages( "from=0", true );
      else
      {
         append_messages( result.messages );

         var next = next_start_point( result.messages );

         if( next !== "" )
            g_start_point = next;
      }
   } );

   input.disabled = false;
   input.focus( );

   if( ciyam.error !== "" )
   {
      failed = true;

      show_alert( ciyam.error, "is-error" );
   }

   // NOTE: A message the server refused goes back into the box - it was cleared on sending,
   // and was otherwise simply lost. An edit stays an edit, to be tried again. One that timed
   // out goes back too, until the re-read after the time-out says whether it was sent.
   if( failed )
   {
      input.value = text;

      on_composer_input( );

      if( timed_out )
      {
         g_uncertain_send = pending;

         show_alert( "The server did not answer in time - your message may have been sent. Checking…", "is-info" );

         document.getElementById( "chat_alert" ).dataset.resync = "1";
      }

      return;
   }

   update_composer_count( );

   do_cancel_edit( );
}

// NOTE: The composer grows with what is typed - wrapped or broken over lines - up to five
// lines, and scrolls beyond that. The row keeps the Send button at the bottom, so the box
// grows upwards as the thread gives way; a thread that was at its latest message stays there.
const c_composer_max_lines = 5;

function resize_composer( )
{
   var input = document.getElementById( "composer_input" );
   var list = document.getElementById( "message_list" );

   var at_end = ( list.scrollTop + list.clientHeight >= list.scrollHeight - 40 );

   input.style.height = "auto";

   var style = getComputedStyle( input );

   var borders = parseFloat( style.borderTopWidth ) + parseFloat( style.borderBottomWidth );
   var padding = parseFloat( style.paddingTop ) + parseFloat( style.paddingBottom );

   var tallest = Math.ceil( ( parseFloat( style.lineHeight ) * c_composer_max_lines ) + padding + borders );

   var wanted = input.scrollHeight + borders;

   input.style.height = Math.min( wanted, tallest ) + "px";
   input.style.overflowY = ( wanted > tallest ) ? "auto" : "hidden";

   if( at_end )
      list.scrollTop = list.scrollHeight;
}

// NOTE: The count appears as a message nears the server's limit, and past it turns red and
// holds the Send button - better than the server's "Maximum size for 'irc_...' items" after
// the fact. Counted in bytes, as the server counts - see "message_bytes( )".
const c_count_from_bytes = 3600;

function message_too_long( )
{
   return message_bytes( document.getElementById( "composer_input" ).value.trim( ) ) > c_max_message_bytes;
}

function update_composer_count( )
{
   var input = document.getElementById( "composer_input" );
   var count = document.getElementById( "composer_count" );

   var bytes = message_bytes( input.value.trim( ) );

   var over = ( bytes > c_max_message_bytes );

   count.hidden = ( bytes < c_count_from_bytes );
   count.classList.toggle( "is-over", over );
   count.textContent = bytes + " / " + c_max_message_bytes + ( over ? " - too long" : "" );

   document.getElementById( "composer_send" ).disabled = over || input.disabled;
}

function on_composer_input( )
{
   resize_composer( );
   update_composer_count( );
   update_emoji_suggest( );
   update_announcement_preview( );
}

function begin_edit( unique, text, is_private )
{
   g_edit_unique = unique;
   g_edit_private = !!is_private;

   var input = document.getElementById( "composer_input" );

   input.value = text;
   input.focus( );

   on_composer_input( );

   document.getElementById( "composer_cancel_edit" ).hidden = false;

   render_composer( );
}

function do_cancel_edit( )
{
   g_edit_unique = "";
   g_edit_private = false;

   document.getElementById( "composer_cancel_edit" ).hidden = true;

   render_composer( );
}

// NOTE: The hint goes once it has been acted on - and only that hint, not whatever else the
// alert bar might be saying by then.
function show_scope_hint( text )
{
   show_alert( text, "is-info" );

   document.getElementById( "chat_alert" ).dataset.scopeHint = "1";
}

function clear_scope_hint( )
{
   var alert = document.getElementById( "chat_alert" );

   if( !alert.hidden && ( alert.dataset.scopeHint === "1" ) )
      do_dismiss_alert( );

   alert.dataset.scopeHint = "";
}

function add_recipient( name )
{
   if( ( name === "" ) || ( name === ciyam.username ) )
      return;

   clear_scope_hint( );

   if( g_recipients.indexOf( name ) < 0 )
      g_recipients.push( name );

   render_composer( );
}

function remove_recipient( name )
{
   var pos = g_recipients.indexOf( name );

   if( pos >= 0 )
      g_recipients.splice( pos, 1 );

   render_composer( );

   // NOTE: The name's remove button has just gone with the redraw - the focus would be lost.
   document.getElementById( "composer_input" ).focus( );
}

function do_toggle_scope( )
{
   // NOTE: Back to everyone - the next thing is typing, so the message box has the focus.
   if( g_recipients.length > 0 )
   {
      g_recipients = [ ];

      render_composer( );

      document.getElementById( "composer_input" ).focus( );

      return;
   }

   if( window.matchMedia( c_details_width_query ).matches )
   {
      // NOTE: On a narrow screen the members are in Room Details, out of sight - open it.
      set_drawer( "details", false );

      show_scope_hint( "Pick members to send to privately." );
   }
   else
      show_scope_hint( "Pick members from the list on the right to send privately." );

   render_composer( );
}

function render_composer( )
{
   var row = document.getElementById( "recipient_row" );

   row.textContent = "";

   row.hidden = ( g_recipients.length === 0 );

   // NOTE: The whole message area says when what is typed is private - amber, "Private to",
   // "Send privately" - and says nothing extra when it is not. See "composer_mode( )".
   var mode = composer_mode( g_recipients, g_edit_unique !== "", g_edit_private );

   document.getElementById( "composer" ).classList.toggle( "is-private", mode.is_private );
   document.getElementById( "composer_private" ).hidden = !mode.is_private;
   document.getElementById( "composer_private_label" ).textContent = mode.label;
   // NOTE: Both labels, the screen width choosing - "Send privately" would squeeze the box on a
   // phone, so there it is "Send". The accessible name is always the full one.
   var send = document.getElementById( "composer_send" );

   send.textContent = "";

   var full = document.createElement( "span" );

   full.className = "chat-send-full";
   full.textContent = mode.send;

   var short = document.createElement( "span" );

   short.className = "chat-send-short";
   short.textContent = mode.send_short;

   send.appendChild( full );
   send.appendChild( short );

   send.setAttribute( "aria-label", mode.send );

   var template = document.getElementById( "tpl_recipient" );

   for( var i = 0; i < g_recipients.length; i++ )
   {
      var name = g_recipients[ i ];

      var node = template.content.cloneNode( true );

      node.querySelector( ".chat-recipient-name" ).textContent = name;

      var button = node.querySelector( "button" );

      button.dataset.name = name;

      button.addEventListener( "click", function( event )
      {
         remove_recipient( event.currentTarget.dataset.name );
      } );

      row.appendChild( node );
   }

   var input = document.getElementById( "composer_input" );

   if( g_recipients.length > 0 )
      input.placeholder = "Private message to " + g_recipients.join( ", " );
   else
      input.placeholder = "Message " + ( g_room_name || ( "#" + g_room ) );

   document.getElementById( "composer_scope" ).textContent =
    ( g_recipients.length > 0 ) ? "Send to everyone" : "Send to selected…";

   // NOTE: The recipients, or the room, may have changed who a preview is for.
   update_announcement_preview( );
}

// ====================================================================
// Polling
// ====================================================================

function start_polling( )
{
   stop_polling( );

   g_poll_timer = window.setInterval( poll, c_poll_interval );
}

function stop_polling( )
{
   if( g_poll_timer !== null )
   {
      window.clearInterval( g_poll_timer );

      g_poll_timer = null;
   }
}

async function poll( )
{
   if( ( ciyam.sessid === "" ) || document.hidden )
      return;

   // NOTE: Polling while a dialog is open puts requests in front of whatever the user
   // is actually waiting for.
   if( is_dialog_open( ) )
      return;

   // NOTE: Awaited in turn rather than issued together - see the serialiser above. The
   // flag is only up while each request is queued, so nothing the user does in between is
   // mistaken for polling.
   g_in_poll = true;

   var rooms = load_rooms( );

   g_in_poll = false;

   await rooms;

   if( g_room !== "" )
   {
      g_in_poll = true;

      var messages = load_messages( g_start_point ? ( "from=" + g_start_point ) : "", false );

      g_in_poll = false;

      await messages;
   }
}

// NOTE: The button turns its icon while the refresh runs, and cannot be pressed again until
// it is done.
async function do_refresh( )
{
   var button = document.getElementById( "rail_refresh" );

   button.disabled = true;

   try
   {
      await load_rooms( );

      if( g_room !== "" )
         await load_messages( "from=0", true );
   }
   finally
   {
      button.disabled = false;
   }
}

function update_poll_label( )
{
   var label = document.getElementById( "rail_poll" );

   if( ( label === null ) || ( g_last_poll === 0 ) )
      return;

   var seconds = Math.round( ( Date.now( ) - g_last_poll ) / 1000 );

   label.textContent = "polled " + seconds + "s ago";
}

// ====================================================================
// Console drawer
// ====================================================================

function do_toggle_console( )
{
   g_console_open = !g_console_open;

   var drawer = document.getElementById( "console_drawer" );

   drawer.hidden = !g_console_open;

   if( g_console_open && !g_console_loaded )
   {
      g_console_loaded = true;

      // NOTE: The console announces itself on the shared channel and is then handed this
      // session - see the handler in "chat.html".
      document.getElementById( "console_frame" ).src = "console.html?embedded=1&source=" + encodeURIComponent( g_self );
   }
}

// NOTE: The same console in its own window, linked to this session the same way. More room
// than the drawer, and the conversation stays fully visible beside it.
function do_popout_console( )
{
   window.open( "console.html?source=" + encodeURIComponent( g_self ), "_blank", "noopener" );

   if( g_console_open )
      do_toggle_console( );
}

// NOTE: A console opened for one session must not carry on under the next, so signing out
// unloads the drawer's console and tells any others that the session has ended.
function unlink_consoles( )
{
   chat_channel.postMessage( String( g_self ) );

   g_console_loaded = false;

   document.getElementById( "console_frame" ).src = "about:blank";
}

function install_request_log( )
{
   install_log_capture( ciyam, "chat", function( ) { return g_request_quiet; }, record_request );

   g_log_channel = new BroadcastChannel( c_log_channel_name );

   g_log_channel.addEventListener( "message", function( event )
   {
      var data = event.data;

      if( ( data === null ) || ( typeof data !== "object" ) || ( data.owner !== String( g_self ) ) )
         return;

      if( data.kind === "replay" )
         g_log_channel.postMessage( { kind: "entries", owner: String( g_self ), viewer: data.viewer, entries: replay_entries( ) } );
      else if( data.kind === "request" )
         proxy_console_request( data );
   } );
}

// ====================================================================
// Time-outs (ISS-020)
// ====================================================================

// NOTE: The server gives up on a request after 5 seconds, but may still finish it - and Ian
// warns its late answer could reach the next request on the session. So after a time-out the
// chat sends one request only to absorb a late answer, then re-reads the room list and the
// open room whole, so nothing wrongly added stays on screen. A send that timed out may have
// been sent: the re-read says whether it was. Three time-outs in a row and the session ends -
// the server or the connection is in trouble. Agreed with Damon, 2026-09-28.
var g_timeouts_in_row = 0;
var g_resyncing = false;
var g_uncertain_send = null;
var g_losing_contact = false;

// NOTE: Every response passes here first, whatever asked for it - polls, sends, the linked
// console's requests. The callbacks are otherwise untouched.
function watch_timeouts( )
{
   [ "fetch", "post" ].forEach( function( name )
   {
      var original = ciyam[ name ].bind( ciyam );

      ciyam[ name ] = function( url, arg, callback )
      {
         return original( url, arg, function( response )
         {
            note_response( response );

            if( callback )
               callback( response );
         } );
      };
   } );
}

function note_response( response )
{
   if( is_timeout_response( response ) )
      note_timeout( );
   else if( typeof response === "string" )
      g_timeouts_in_row = 0;
}

function note_timeout( )
{
   // NOTE: Not while signing in or out - signing out makes a request of its own, and its
   // time-out must not start the sign out again.
   if( g_losing_contact || ( ciyam.sessid === "" ) )
      return;

   ++g_timeouts_in_row;

   if( g_timeouts_in_row >= c_max_timeouts_in_row )
   {
      lose_contact( );

      return;
   }

   if( g_resyncing )
      return;

   g_resyncing = true;

   show_alert( "The server was slow to answer - catching up…", "is-info" );

   document.getElementById( "chat_alert" ).dataset.resync = "1";

   // NOTE: After the request in hand has finished, so the resync joins the queue behind it.
   window.setTimeout( resync_after_timeout, 0 );
}

function resync_after_timeout( )
{
   if( !g_resyncing )
      return;

   // NOTE: Its answer is thrown away - if a late one is coming, this is where it lands.
   g_in_poll = true;

   var drain = serialised( function( )
   {
      return ciyam.fetch_messages( c_lobby_room, "", function( ) { } );
   } );

   g_in_poll = false;

   drain
    .then( function( ) { return load_rooms( ); } )
    .then( function( ) { if( g_room !== "" ) return load_messages( "from=0", true ); } )
    .then( function( )
    {
       // NOTE: Administration read whole again, for the invitations and announcements.
       if( !ciyam.is_admin && ( g_invite_messages.length > 0 ) )
          check_invitations( true );
    } )
    .then( finish_resync, finish_resync );
}

function finish_resync( )
{
   if( !g_resyncing )
      return;

   g_resyncing = false;

   if( g_uncertain_send !== null )
   {
      resolve_uncertain_send( );

      return;
   }

   var alert = document.getElementById( "chat_alert" );

   if( !alert.hidden && ( alert.dataset.resync === "1" ) )
      do_dismiss_alert( );
}

// NOTE: Whether a send that timed out reached the room, from the room just re-read whole.
function resolve_uncertain_send( )
{
   var pending = g_uncertain_send;

   g_uncertain_send = null;

   if( pending.room !== g_room )
   {
      show_alert( "The server did not answer in time - check whether your message arrived before sending it again.", "is-info" );

      return;
   }

   var rows = Array.from( document.querySelectorAll( "#message_list > .chat-message" ) ).map( function( row )
   {
      return {
         unique: row.dataset.unique,
         sender: row.querySelector( ".chat-message-sender-name" ).textContent,
         text: row.querySelector( ".chat-message-body" ).textContent
      };
   } );

   if( arrived_after_timeout( rows, pending ) )
   {
      var input = document.getElementById( "composer_input" );

      if( input.value.trim( ) === pending.text )
      {
         input.value = "";

         on_composer_input( );
      }

      if( pending.edit )
         do_cancel_edit( );

      show_alert( "Your message was sent after all - no need to send it again.", "is-info" );
   }
   else
      show_alert( "Your message was not sent - it is back in the box to send again.", "is-info" );
}

async function lose_contact( )
{
   g_losing_contact = true;
   g_resyncing = false;
   g_uncertain_send = null;

   try
   {
      await do_disconnect( );
   }
   finally
   {
      g_losing_contact = false;
   }

   set_error( "signin_error", "Lost contact with the server - it did not answer " + c_max_timeouts_in_row
    + " times in a row. Please sign in again." );
}

// NOTE: A linked console shares this session, and the server keeps one command slot and one
// output file per access and device - so two requests in flight on the session at once can
// each receive the other's response (ISS-020). The console therefore sends its requests
// here, and they join this tab's queue like any other. Not logged as "chat": the console
// logs them itself, as its own.
function proxy_console_request( data )
{
   function reply( response )
   {
      g_log_channel.postMessage( { kind: "response", owner: String( g_self ), viewer: data.viewer, id: data.id, response: response } );
   }

   var methods = [ "GET", "POST", "PUT", "DELETE" ];

   if( ( ciyam.sessid === "" ) || ( typeof data.url !== "string" )
    || ( data.url.indexOf( ciyam.get_cws_url( ) ) !== 0 ) || ( methods.indexOf( data.method ) < 0 ) )
   {
      reply( null );

      return;
   }

   serialised( function( )
   {
      return new Promise( function( resolve )
      {
         var answered = false;

         g_request_quiet = true;

         var pending = ciyam.fetch( data.url, data.method, function( response )
         {
            answered = true;

            reply( String( response ) );

            resolve( );
         } );

         g_request_quiet = false;

         pending.then( function( )
         {
            if( !answered )
            {
               reply( null );

               resolve( );
            }
         } );
      } );
   } );
}

function log_session_only( )
{
   try
   {
      return parse_prefs( localStorage.getItem( c_console_prefs_key ) ).log_session_only;
   }
   catch( e )
   {
      return false;
   }
}

// NOTE: The "Log current session only" preference is set in the console. With it on, a
// console that links is sent only this session's requests - never an earlier account's.
function replay_entries( )
{
   if( !log_session_only( ) )
      return g_request_log;

   return g_request_log.filter( function( entry ) { return entry.session === g_session_seq; } );
}

function end_log_session( )
{
   ++g_session_seq;

   if( log_session_only( ) )
      g_request_log = [ ];
}

function record_request( entry )
{
   entry.id = ++g_request_log_id;
   entry.session = g_session_seq;

   g_request_log.push( entry );

   if( g_request_log.length > c_console_log_capacity )
      g_request_log.shift( );

   if( g_log_channel !== null )
      g_log_channel.postMessage( { kind: "entry", owner: String( g_self ), entry: entry } );
}

// ====================================================================
// Status
// ====================================================================

function is_dialog_open( )
{
   var dialogs = document.querySelectorAll( ".chat-scrim" );

   for( var i = 0; i < dialogs.length; i++ )
   {
      if( !dialogs[ i ].hidden )
         return true;
   }

   return false;
}

// NOTE: "from_loading" marks an error raised by loading rooms or messages. The next load
// that succeeds clears it - Ian saw "IRC not available" stay up after IRC had come back and
// the rooms had appeared. Errors from something the user did stay until dismissed.
function show_alert( text, kind, from_loading )
{
   var alert = document.getElementById( "chat_alert" );

   document.getElementById( "chat_alert_text" ).textContent = text;

   alert.className = "chat-alert " + ( kind || "is-error" );
   alert.dataset.fromLoading = from_loading ? "1" : "";
   alert.dataset.scopeHint = "";
   alert.dataset.resync = "";
   alert.hidden = false;
}

function clear_loading_alert( )
{
   var alert = document.getElementById( "chat_alert" );

   if( !alert.hidden && ( alert.dataset.fromLoading === "1" ) )
      alert.hidden = true;
}

function do_dismiss_alert( )
{
   document.getElementById( "chat_alert" ).hidden = true;
}

function set_error( id, text )
{
   var node = document.getElementById( id );

   if( node !== null )
      node.textContent = text;
}

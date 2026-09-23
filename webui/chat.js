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

var g_room = "";
var g_rooms = [ ];
var g_members = [ ];
var g_room_name = "";
var g_room_owner = "";

var g_start_point = "";
var g_poll_timer = null;
var g_last_poll = 0;

var g_edit_unique = "";
var g_recipients = [ ];

var g_dialog_mode = "create";

var g_known_users = [ ];

// NOTE: Set for the one connect that follows a registration, so the issued PIN can be
// kept and shown. Cleared once used.
var g_registered_pin = "";

var g_console_open = false;
var g_console_loaded = false;

// ====================================================================
// Entry point
// ====================================================================

function chat( )
{
   install_claim_workaround( );

   if( localStorage.getItem( c_storage_device ) !== null )
      ciyam.device = localStorage.getItem( c_storage_device );

   populate_accounts( );
   update_connection_details( );

   window.setInterval( update_poll_label, 1000 );

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
      else if( !document.getElementById( "token_dialog" ).hidden )
      {
         do_close_token_dialog( );

         event.preventDefault( );
      }
      else if( !document.getElementById( "room_dialog" ).hidden )
      {
         do_close_room_dialog( );

         event.preventDefault( );
      }
   } );
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

      select.options.add( new Option( label, access, false ), 1 );
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

// NOTE: Registering a device, or opening a session, for an account whose username has
// not yet been claimed returns "<pin> <seed>" rather than a device token or a session
// unique. "CIYAM.at_connect" recognises that in its JSON branch, through "obj.pin", but
// the text branch has no equivalent and stores the whole string as the device or unique.
// The account is then never claimed, the session carries no username, and every later
// call answers "<pin> @none" - which is why creating a room appeared to do nothing.
//
// Patched on the instance rather than in "ciyam.js", which is shared with the harness.
// Reported to Ian - remove this once the text branch handles it.
function install_claim_workaround( )
{
   var original = ciyam.at_connect.bind( ciyam );

   ciyam.at_connect = function( response )
   {
      var text = String( response );

      var sep = text.indexOf( " " );

      if( ( sep > 0 ) && ( text.indexOf( "[" ) !== 0 ) && ( text.indexOf( "Error: " ) !== 0 ) )
      {
         ciyam.access = text.substr( 0, sep );

         if( ciyam.seed === "" )
            ciyam.seed = text.substring( sep + 1 );

         if( ciyam.user_callback != null )
            ciyam.user_callback( response );

         return;
      }

      original( response );
   };
}

// NOTE: The claim only happens on the code path taken when no device token is held, so a
// browser that already has one (from signing in as somebody else) would open a session
// with no username instead. Detected by the connect ending with no session and no error,
// which is what the seed response leaves behind, and retried without the device.
function needs_claiming( )
{
   return ( ( ciyam.sessid === "" ) && ( ciyam.error === "" ) && ( ciyam.seed !== "" ) );
}

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
   set_signin_state( "connecting…", "is-idle" );

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
         await ciyam.connect( access, ciyam.device, ciyam.hashed, password, function( ) { } );

         if( needs_claiming( ) )
         {
            // NOTE: A fresh device registration is what carries the credentials that
            // claim the account, so the stored token is deliberately not reused here.
            ciyam.device = "";

            await ciyam.connect( access, "", "", password, function( ) { } );
         }
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

   update_connection_details( );

   if( ciyam.error !== "" )
   {
      set_error( "signin_error", ciyam.error );
      set_signin_state( "not connected", "is-idle" );

      return;
   }

   if( ciyam.sessid === "" )
   {
      set_error( "signin_error", "No session was established." );
      set_signin_state( "not connected", "is-idle" );

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
   set_signin_state( "connected", "" );

   enter_chat( );
}

function enter_chat( )
{
   document.getElementById( "signin_view" ).hidden = true;
   document.getElementById( "chat_view" ).hidden = false;

   document.getElementById( "topbar_user" ).textContent = ciyam.username || ciyam.access;
   document.getElementById( "topbar_role" ).textContent = ciyam.is_admin ? "· admin" : "· standard";
   document.getElementById( "topbar_session" ).textContent = "session " + ciyam.sessid;
   document.getElementById( "console_session" ).textContent = "inherits chat session " + ciyam.sessid;

   load_rooms( );

   start_polling( );
}

async function do_disconnect( )
{
   stop_polling( );

   await ciyam.disconnect( function( ) { } );

   g_room = "";
   g_rooms = [ ];
   g_members = [ ];
   g_start_point = "";

   if( g_console_open )
      do_toggle_console( );

   document.getElementById( "chat_view" ).hidden = true;
   document.getElementById( "signin_view" ).hidden = false;

   set_signin_state( "not connected · no session id yet", "is-idle" );

   update_connection_details( );
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

function set_signin_state( text, dot_class )
{
   document.getElementById( "signin_state" ).textContent = text;

   var dot = document.getElementById( "signin_dot" );

   dot.className = "chat-dot" + ( dot_class ? ( " " + dot_class ) : "" );
}

function update_connection_details( )
{
   set_detail( "detail_device", ciyam.device );
   set_detail( "detail_unique", ciyam.unique );
   set_detail( "detail_session", ciyam.sessid );
}

function set_detail( id, value )
{
   var node = document.getElementById( id );

   node.textContent = ( value === "" ) ? "—" : value;

   if( value === "" )
      node.classList.add( "is-empty" );
   else
      node.classList.remove( "is-empty" );
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
   var wrapped = function( )
   {
      begin_busy( );

      return Promise.resolve( ).then( fn ).then(
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

   if( result.error !== "" )
   {
      show_alert( result.error, "is-error" );

      return;
   }

   // NOTE: Defensive - an entrance listing carries room rows. Anything else is a
   // response meant for a different caller and must not be acted on here.
   if( ( result.rooms.length === 0 ) && ( result.messages.length > 0 ) )
      return;

   // NOTE: The entrance listing returns every user, so the invitee picker can be
   // populated from it with no request of its own.
   if( result.members.length > 0 )
      g_known_users = apply_presence( result.members );

   {
      g_rooms = derive_room_list( result.rooms );

      render_rooms( );

      // NOTE: Open the first room with anything unread, else the first listed.
      if( ( g_room === "" ) && ( g_rooms.length > 0 ) )
         select_room( g_rooms[ 0 ].room, "" );
   }
}

// NOTE: The poll runs every few seconds and these lists are usually identical between
// polls. Rebuilding regardless made both panels visibly flicker, so each render is
// skipped when the data it would draw has not changed.
var g_rooms_drawn = "";
var g_members_drawn = "";

function rooms_signature( )
{
   var parts = [ g_room ];

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
   var signature = rooms_signature( );

   if( !force && ( signature === g_rooms_drawn ) )
      return;

   g_rooms_drawn = signature;

   var list = document.getElementById( "room_list" );

   var template = document.getElementById( "tpl_room" );

   reconcile_list( list, g_rooms,
    function( entry )
    {
       return entry.room;
    },
    function( entry )
    {
       var node = template.content.cloneNode( true ).querySelector( ".chat-room" );

       node.querySelector( ".chat-room-number" ).textContent = "#" + entry.room;

       node.dataset.room = entry.room;

       node.addEventListener( "click", function( event )
       {
          select_room( event.currentTarget.dataset.room, "" );
       } );

       return node;
    },
    function( node, entry )
    {
       set_text( node.querySelector( ".chat-room-name" ), entry.name );
       set_text( node.querySelector( ".chat-room-owner" ), "owner " + entry.owner );

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
   g_recipients = [ ];

   var entry = find_room( room );

   g_room_name = entry ? entry.name : room;
   g_room_owner = entry ? entry.owner : "";

   document.getElementById( "thread_head" ).hidden = false;
   document.getElementById( "thread_empty" ).hidden = true;
   document.getElementById( "message_list" ).hidden = false;
   document.getElementById( "composer" ).hidden = false;

   document.getElementById( "thread_name" ).textContent = g_room_name;
   document.getElementById( "thread_number" ).textContent = "#" + room;

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
   document.getElementById( "room_dialog_invitees_row" ).hidden = true;

   set_error( "room_dialog_error", "" );

   document.getElementById( "room_dialog" ).hidden = false;
   document.getElementById( "room_dialog_name" ).focus( );
}

function do_open_invite( )
{
   show_alert( "Inviting to an existing room needs a server route that does not exist yet - "
    + "create a room with invitees instead.", "is-warning" );
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

// NOTE: Shows the join token and switches to the room just created, so the user ends up
// in it rather than having to find it in the rail.
async function open_new_room( response )
{
   show_new_room( response );

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

// NOTE: The create response is "<room>-<join token>" in text format.
function show_new_room( response )
{
   var value = String( response ).trim( );

   if( value.charAt( 0 ) === ":" )
      value = value.substring( 1 );

   var pos = value.indexOf( "-" );

   if( pos < 0 )
      return;

   document.getElementById( "token_value" ).textContent = value;

   var qr = document.getElementById( "token_qr" );

   qr.textContent = "";

   if( typeof QRCode !== "undefined" )
   {
      try
      {
         new QRCode( qr, { text: value, width: 160, height: 160 } );
      }
      catch( e )
      {
         qr.textContent = "(QR unavailable)";
      }
   }

   document.getElementById( "token_dialog" ).hidden = false;
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

function do_copy_token( )
{
   var value = document.getElementById( "token_value" ).textContent;

   if( navigator.clipboard )
      navigator.clipboard.writeText( value );
}

function do_close_token_dialog( )
{
   document.getElementById( "token_dialog" ).hidden = true;
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

   return serialised( function( )
   {
      return ciyam.fetch_messages( asked_for, options, function( response )
      {
         on_messages_response( response, asked_for, replace );
      } );
   } );
}

function on_messages_response( response, asked_for, replace )
{
   {
      var result = parse_fetch_response( response );

      g_last_poll = Date.now( );

      if( result.error !== "" )
      {
         show_alert( result.error, "is-error" );

         return;
      }

      // NOTE: Defensive - room rows mean this is an entrance listing, not ours.
      if( result.rooms.length > 0 )
         return;

      // NOTE: The user changed room while this was in flight.
      if( asked_for !== g_room )
         return;

      g_members = apply_presence( result.members );

      render_members( );

      if( replace )
         document.getElementById( "message_list" ).textContent = "";

      append_messages( result.messages );

      var next = next_start_point( result.messages );

      if( next !== "" )
         g_start_point = next;

      // NOTE: On a full load mark where reading had got to, so that a long
      // history has a visible boundary.
      if( replace && ( g_start_point !== "" ) )
         append_marker( "last read " + g_start_point );

      update_thread_meta( );
   }
}

function append_marker( text )
{
   var node = document.getElementById( "tpl_marker" ).content.cloneNode( true );

   node.querySelector( "span" ).textContent = text;

   var list = document.getElementById( "message_list" );

   list.appendChild( node );

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

function append_messages( messages )
{
   var list = document.getElementById( "message_list" );

   var was_at_end = ( list.scrollTop + list.clientHeight >= list.scrollHeight - 40 );

   for( var i = 0; i < messages.length; i++ )
   {
      var entry = messages[ i ];

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

      row.firstElementChild.dataset.day = ( key !== "" ) ? key : last_rendered_day( list );

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

   if( entry.sender === ciyam.username )
   {
      row.classList.add( "is-own" );

      var edit = row.querySelector( ".chat-message-edit" );

      edit.hidden = false;
      edit.dataset.unique = entry.unique;
      edit.dataset.text = entry.text;

      edit.addEventListener( "click", function( event )
      {
         begin_edit( event.currentTarget.dataset.unique, event.currentTarget.dataset.text );
      } );
   }

   return node;
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

   node.querySelector( ".chat-notice-who" ).textContent = entry.sender + " ";
   node.querySelector( ".chat-notice-verb" ).textContent = ":" + event.verb;

   var detail = "";

   if( event.verb === "rename" )
      detail = " '" + ( event.from_name || "" ) + "' to '" + ( event.to_name || "" ) + "'";
   else if( ( event.verb === "invite" ) || ( event.verb === "create" ) )
      detail = " " + ( event.name || "" ) + " (#" + ( event.room || "" ) + ")";
   else if( event.verb === "issued" )
      detail = " " + ( event.detail || "" );

   node.querySelector( ".chat-notice-detail" ).textContent = detail;

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

   set_text( action, joined ? "Joined" : "Join" );

   action.classList.toggle( "is-joined", joined );

   action.title = joined ? "Already a member - open this room"
    : "Accept this invitation and join the room";
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

// NOTE: The rules themselves live in "chat_parse.js" so they are covered by the regression
// tests - this only reflects the answer in the UI.
function apply_posting_rules( )
{
   var entry = find_room( g_room );

   var status = posting_status( g_room, entry, ciyam.username, ciyam.is_admin );

   var input = document.getElementById( "composer_input" );
   var send = document.getElementById( "composer_send" );
   var scope = document.getElementById( "composer_scope" );
   var note = document.getElementById( "composer_note" );

   input.disabled = !status.can_post;
   send.disabled = !status.can_post;
   scope.disabled = !status.can_post;

   if( status.can_post )
      note.textContent = "Ordinary chat only — system and slash messages are server-side.";
   else
   {
      note.textContent = status.reason;

      input.value = "";
      input.placeholder = status.locked ? "🔒 " + status.reason : status.reason;
   }
}

function update_thread_meta( )
{
   var entry = find_room( g_room );

   var parts = [ ];

   if( g_room_owner !== "" )
      parts.push( "owner " + g_room_owner );

   parts.push( g_members.length + " member" + ( g_members.length === 1 ? "" : "s" ) );

   if( entry )
      parts.push( entry.total + " message" + ( entry.total === 1 ? "" : "s" ) );

   document.getElementById( "thread_meta" ).textContent = parts.join( " · " );

   var is_owner = ( g_room_owner === ciyam.username ) || ciyam.is_admin;

   document.getElementById( "owner_actions" ).hidden = !is_owner;

   document.getElementById( "presence_foot" ).textContent =
    "poll every " + ( c_poll_interval / 1000 ) + "s"
    + ( g_start_point ? ( "\nfrom=" + g_start_point ) : "" )
    + "\nno push · no typing state";
}

// ====================================================================
// Composing
// ====================================================================

function do_composer_key( event )
{
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

   return true;
}

async function do_send( )
{
   var input = document.getElementById( "composer_input" );

   var text = input.value.trim( );

   if( ( text === "" ) || ( g_room === "" ) )
      return;

   var options = "";

   if( g_edit_unique !== "" )
      options = "for=" + g_edit_unique + ";text=" + text;
   else if( g_recipients.length > 0 )
      options = "for=" + g_recipients.join( "," ) + ";text=" + text;
   else
      options = "text=" + text;

   input.value = "";
   input.disabled = true;

   ciyam.error = "";

   var was_edit = ( g_edit_unique !== "" );

   await ciyam.create_message( g_room, options, function( response )
   {
      if( is_error_response( response ) )
      {
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
      show_alert( ciyam.error, "is-error" );

   do_cancel_edit( );
}

function begin_edit( unique, text )
{
   g_edit_unique = unique;

   var input = document.getElementById( "composer_input" );

   input.value = text;
   input.focus( );

   document.getElementById( "composer_cancel_edit" ).hidden = false;
   document.getElementById( "composer_send" ).textContent = "Save edit";
}

function do_cancel_edit( )
{
   g_edit_unique = "";

   document.getElementById( "composer_cancel_edit" ).hidden = true;
   document.getElementById( "composer_send" ).textContent = "Send";
}

function add_recipient( name )
{
   if( ( name === "" ) || ( name === ciyam.username ) )
      return;

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
}

function do_toggle_scope( )
{
   if( g_recipients.length > 0 )
      g_recipients = [ ];
   else
      show_alert( "Pick members from the list on the right to send privately.", "is-info" );

   render_composer( );
}

function render_composer( )
{
   var row = document.getElementById( "recipient_row" );

   row.textContent = "";

   row.hidden = ( g_recipients.length === 0 );

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
      input.placeholder = "Message #" + g_room + " — plain chat";

   document.getElementById( "composer_scope" ).textContent =
    ( g_recipients.length > 0 ) ? "Send to everyone" : "Send to selected…";
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

   // NOTE: Awaited in turn rather than issued together - see the serialiser above.
   await load_rooms( );

   if( g_room !== "" )
      await load_messages( g_start_point ? ( "from=" + g_start_point ) : "", false );
}

async function do_refresh( )
{
   await load_rooms( );

   if( g_room !== "" )
      await load_messages( "from=0", true );
}

function update_poll_label( )
{
   var label = document.getElementById( "topbar_poll" );

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

      // NOTE: The harness announces itself on the shared channel and is then
      // handed this session - see the handler in "chat.html".
      document.getElementById( "console_frame" ).src = "test_web_session.html?source=" + g_self;
   }
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

function show_alert( text, kind )
{
   var alert = document.getElementById( "chat_alert" );

   document.getElementById( "chat_alert_text" ).textContent = text;

   alert.className = "chat-alert " + ( kind || "is-error" );
   alert.hidden = false;
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

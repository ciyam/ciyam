// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The node's sign in, shared - one copy for every app, so the sign in is the same everywhere
// (Damon, 2026-10-08) - Home, the chat, the accounts page and the console, each of which had its own copy.
// It is the accounts page's, the newest of the three: the saved accounts, "Remember on this browser", a typed
// password tried again with a new device token after the node was set up again, "Reset this browser"; with
// the chat's "Contacting the server..." while it signs in.
//
// The page builds it with "signin_build( host, options )", shows it with "signin_show( )" and may reword it
// with "signin_describe( title, lede )":
//
//   options.note           the word beside the wordmark ("home")
//   options.title          the heading ("Home")
//   options.lede           the line beneath it
//   options.setup_href     where "New here?" goes - the accounts page's Welcome
//   options.request        the page's one-at-a-time request wrapper (ISS-005), resolving to the answer
//   options.on_signed_in   called once "ciyam" has a session
//   options.error_text     optional - a refusal in the page's own words, or "" to leave it as it is
//   options.ids            optional - { access, submit }: the ids a page's suites know its list and button by
//   options.submit_text    optional - the button's word ("Sign in"; the chat and the console "Connect")
//   options.extra_link     optional - { text, href }: a second action beside the button (the console's chat)
//   options.on_busy        optional - called with true as the sign in starts and false as it ends
//
// "request" may be left out - the sign in is then made at once, as the console made it.
//
// The element ids are the ones every page's sign in has used ("signin_access", "signin_pin", ...), as
// the browser suites expect. Needs "chat_parse.js" ("parse_access_list( )", "retain_mode_of( )",
// "plan_retain_choice( )", "sign_in_error_text( )") and "account_parse.js" ("is_account_pin( )",
// "is_unknown_device_error( )"), and the page's "ciyam". The look is "chat.css"'s ".chat-signin".
//
// NOTE: Its own names throughout - the pages declare "c_storage_device" and the rest as "const", and a
// second declaration of one would stop the page. The keys stored are the same, so the apps share them.

const c_signin_storage_device = "cws.device";
const c_signin_storage_access = "cws.access";
const c_signin_storage_hashed_prefix = "cws.hashed_";

const c_signin_saved_mark = "  ·  saved password";

var g_signin_options = { };

var g_signin_ids = { access: "signin_access", submit: "signin_submit" };

function signin_element( tag, attributes, text )
{
   var element = document.createElement( tag );

   Object.keys( attributes || { } ).forEach( function( name )
   {
      if( name === "class" )
         element.className = attributes[ name ];
      else
         element.setAttribute( name, attributes[ name ] );
   } );

   if( text !== undefined )
      element.textContent = text;

   return element;
}

function signin_build( host, options )
{
   g_signin_options = options || { };

   g_signin_ids = { access: ( g_signin_options.ids && g_signin_options.ids.access ) || "signin_access",
    submit: ( g_signin_options.ids && g_signin_options.ids.submit ) || "signin_submit" };

   var view = signin_element( "div", { class: "chat-signin", id: "signin_view", hidden: "" } );
   var form = signin_element( "form", { class: "chat-signin-form", id: "signin_form", novalidate: "" } );

   form.addEventListener( "submit", signin_submit );

   var brand = signin_element( "div", { class: "chat-brand" } );

   brand.appendChild( signin_element( "span", { class: "chat-logo" } ) );
   brand.appendChild( signin_element( "span", { class: "chat-wordmark" }, "CIYAM" ) );
   brand.appendChild( signin_element( "span", { class: "chat-brand-note" }, g_signin_options.note || "" ) );

   form.appendChild( brand );
   form.appendChild( signin_element( "h2", { tabindex: "-1" }, g_signin_options.title || "Sign in" ) );
   form.appendChild( signin_element( "p", { class: "chat-signin-lede" }, g_signin_options.lede || "" ) );

   form.appendChild( signin_element( "label", { class: "chat-label", for: g_signin_ids.access }, "Account" ) );

   var access = signin_element( "select", { id: g_signin_ids.access, class: "chat-field chat-field--mono" } );

   access.appendChild( new Option( "Enter a PIN…", "" ) );
   access.addEventListener( "change", signin_on_access );

   form.appendChild( access );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_pin", id: "signin_pin_label" }, "PIN" ) );

   var pin = signin_element( "input", { type: "text", id: "signin_pin", class: "chat-field chat-field--mono",
    inputmode: "numeric", maxlength: "5", autocomplete: "username" } );

   pin.addEventListener( "input", signin_on_access );

   form.appendChild( pin );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_password" }, "Password" ) );
   form.appendChild( signin_element( "input", { type: "password", id: "signin_password", class: "chat-field",
    autocomplete: "current-password" } ) );
   form.appendChild( signin_element( "div", { class: "signin-hint", id: "signin_hint" } ) );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_retain" }, "Remember on this browser" ) );

   var retain = signin_element( "select", { id: "signin_retain", class: "chat-field" } );

   retain.appendChild( new Option( "Nothing — forget this account", "none" ) );
   retain.appendChild( new Option( "The PIN only", "access" ) );
   retain.appendChild( new Option( "The PIN and the password", "full" ) );

   form.appendChild( retain );
   form.appendChild( signin_element( "div", { class: "signin-hint" },
    "Shared by every app on this browser. Saving the password as well signs you in without typing it." ) );

   form.appendChild( signin_element( "div", { class: "chat-error-text", id: "signin_error", role: "alert" } ) );

   var actions = signin_element( "div", { class: "chat-signin-actions" } );

   actions.appendChild( signin_element( "button", { type: "submit", class: "chat-btn chat-btn--primary", id: g_signin_ids.submit },
    g_signin_options.submit_text || "Sign in" ) );

   if( g_signin_options.extra_link )
      actions.appendChild( signin_element( "a", { class: "chat-btn chat-signin-setup", id: "signin_extra", href: g_signin_options.extra_link.href },
       g_signin_options.extra_link.text ) );

   if( g_signin_options.setup_href )
      actions.appendChild( signin_element( "a", { class: "chat-btn chat-signin-setup", id: "signin_setup",
       href: g_signin_options.setup_href }, "New here? Set up your account" ) );

   form.appendChild( actions );

   // NOTE: Shown while the sign in is under way - it can take a few seconds (the chat's, 2026-09).
   var busy = signin_element( "div", { class: "chat-signin-busy", id: "signin_busy", hidden: "" } );

   busy.appendChild( signin_element( "span", { class: "chat-spinner" } ) );
   busy.appendChild( signin_element( "span", { }, "Contacting the server…" ) );

   form.appendChild( busy );

   if( g_signin_options.setup_href )
      form.appendChild( signin_element( "div", { class: "chat-hint chat-signin-centred", id: "signin_setup_hint" },
       "With the code or PIN from the person who added you." ) );

   var foot = signin_element( "div", { class: "chat-signin-foot" } );
   var reset = signin_element( "button", { type: "button", class: "chat-btn--link", id: "signin_reset" }, "Reset this browser" );

   reset.addEventListener( "click", signin_reset_browser );

   foot.appendChild( reset );
   foot.appendChild( signin_element( "div", { class: "chat-signin-foot-note" },
    "Forgets the device token, every saved account and this browser's other settings for the node." ) );

   form.appendChild( foot );
   view.appendChild( form );
   host.appendChild( view );

   return view;
}

function signin_show( )
{
   document.getElementById( "signin_password" ).value = "";
   document.getElementById( "signin_pin" ).value = "";

   signin_set_text( "signin_error", "" );

   signin_fill_saved( );

   document.getElementById( "signin_view" ).hidden = false;
}

// NOTE: A different heading and line for the same sign in - Home's for a locked node, say - and whether it
// offers "New here?", which a locked node cannot use.
function signin_describe( title, lede, offers_setup )
{
   document.querySelector( "#signin_view h2" ).textContent = title;
   document.querySelector( "#signin_view .chat-signin-lede" ).textContent = lede;

   document.querySelectorAll( "#signin_setup, #signin_setup_hint" ).forEach( function( element )
   {
      element.hidden = ( offers_setup === false );
   } );
}

function signin_hide( )
{
   document.getElementById( "signin_view" ).hidden = true;
}

function signin_set_text( id, text )
{
   document.getElementById( id ).textContent = text;
}

function signin_storage_get( key )
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

function signin_saved_pins( )
{
   return parse_access_list( signin_storage_get( c_signin_storage_access ) ).filter( is_account_pin );
}

function signin_saved_hash( pin )
{
   return signin_storage_get( c_signin_storage_hashed_prefix + pin );
}

// NOTE: Read each time - another app may have replaced it since this page loaded ("Reset this browser").
function signin_stored_device( )
{
   return signin_storage_get( c_signin_storage_device ) || "";
}

function signin_remember_device( )
{
   try
   {
      if( ciyam.device !== "" )
         localStorage.setItem( c_signin_storage_device, ciyam.device );
   }
   catch( e )
   {
   }
}

// NOTE: The accounts saved on this browser, by any app, ahead of "Enter a PIN...", the first chosen.
function signin_fill_saved( )
{
   var select = document.getElementById( g_signin_ids.access );

   for( var i = select.options.length - 1; i >= 0; i-- )
   {
      if( select.options[ i ].value !== "" )
         select.remove( i );
   }

   signin_saved_pins( ).forEach( function( pin, n )
   {
      select.options.add( new Option( pin + ( ( signin_saved_hash( pin ) !== null ) ? c_signin_saved_mark : "" ), pin, false ), n );
   } );

   select.selectedIndex = 0;

   signin_on_access( );
}

function signin_pin( )
{
   var chosen = document.getElementById( g_signin_ids.access ).value;

   return ( chosen !== "" ) ? chosen : document.getElementById( "signin_pin" ).value.trim( );
}

// NOTE: The PIN field only for a PIN not saved here; the Remember box shows what is saved for the
// account now, so signing in never silently forgets it. A saved password is used by leaving the field
// empty - it stays open, so a password that no longer works can be typed instead.
function signin_on_access( )
{
   var typed = ( document.getElementById( g_signin_ids.access ).value === "" );

   document.getElementById( "signin_pin_label" ).hidden = !typed;
   document.getElementById( "signin_pin" ).hidden = !typed;

   var pin = signin_pin( );

   var has_hash = is_account_pin( pin ) && ( signin_saved_hash( pin ) !== null );

   signin_set_text( "signin_hint", has_hash ? "The password is saved on this browser - leave it empty to use it." : "" );

   document.getElementById( "signin_retain" ).value =
    retain_mode_of( signin_storage_get( c_signin_storage_access ), is_account_pin( pin ) ? pin : "", has_hash );
}

// NOTE: After a sign in, what the Remember box asked for - "plan_retain_choice( )" in "chat_parse.js".
function signin_apply_retain( )
{
   try
   {
      var plan = plan_retain_choice( localStorage.getItem( c_signin_storage_access ), ciyam.access,
       document.getElementById( "signin_retain" ).value, ciyam.hashed );

      if( plan.keep_hash )
         localStorage.setItem( c_signin_storage_hashed_prefix + ciyam.access, ciyam.hashed );
      else
         localStorage.removeItem( c_signin_storage_hashed_prefix + ciyam.access );

      if( plan.list === null )
         localStorage.removeItem( c_signin_storage_access );
      else
         localStorage.setItem( c_signin_storage_access, plan.list );
   }
   catch( e )
   {
   }
}

// NOTE: Everything this browser keeps for the node goes: the device token, which a rebuilt node no
// longer knows, every saved account - for every app, as they share them - and the rest: dismissed
// announcements, recent emoji, Home's count of keys made. Said so on the page (found by review).
function signin_reset_browser( )
{
   if( ciyam.sessid !== "" )
      return;

   if( !confirm( "Forget the device token, every saved account and this browser's other settings for the node?" ) )
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

async function signin_connect( pin, hashed, password )
{
   ciyam.error = "";
   ciyam.unique = "";

   var issue = function( done )
   {
      return ciyam.connect( pin, ciyam.device, hashed, password, done );
   };

   if( g_signin_options.request )
      await g_signin_options.request( issue );
   else
      await issue( function( ) { } );
}

async function signin_submit( event )
{
   event.preventDefault( );

   var pin = signin_pin( );
   var password = document.getElementById( "signin_password" ).value;

   if( !is_account_pin( pin ) )
   {
      signin_set_text( "signin_error", "Enter your PIN - 5 digits." );

      return;
   }

   var hashed = ( password === "" ) ? signin_saved_hash( pin ) : "";

   if( ( password === "" ) && ( hashed === null ) )
   {
      signin_set_text( "signin_error", "Enter your password." );

      return;
   }

   signin_set_text( "signin_error", "" );

   var submit = document.getElementById( g_signin_ids.submit );

   submit.disabled = true;

   document.getElementById( "signin_busy" ).hidden = false;

   if( g_signin_options.on_busy )
      g_signin_options.on_busy( true );

   ciyam.device = signin_stored_device( );

   await signin_connect( pin, hashed, password );

   // NOTE: The node has never seen this browser's device token - issued before the node was set up
   // again, or by another node at this address. A typed password does not depend on it, so a new one
   // is asked for and the sign in tried again. A saved password cannot be: it was hashed with the old.
   if( is_unknown_device_error( ciyam.error ) && ( password !== "" ) )
   {
      ciyam.device = "";

      await signin_connect( pin, "", password );
   }

   submit.disabled = false;

   document.getElementById( "signin_busy" ).hidden = true;

   if( g_signin_options.on_busy )
      g_signin_options.on_busy( false );

   if( ciyam.error !== "" )
   {
      var own = g_signin_options.error_text ? g_signin_options.error_text( ciyam.error ) : "";

      signin_set_text( "signin_error", ( own !== "" ) ? own : ( is_unknown_device_error( ciyam.error )
       ? "This browser's saved sign in is from before the node was set up again - type your password."
       : sign_in_error_text( ciyam.error ) ) );

      return;
   }

   if( ciyam.sessid === "" )
   {
      signin_set_text( "signin_error", "No session was established." );

      return;
   }

   signin_remember_device( );

   document.getElementById( "signin_password" ).value = "";
   document.getElementById( "signin_pin" ).value = "";

   signin_apply_retain( );

   signin_hide( );

   if( g_signin_options.on_signed_in )
      g_signin_options.on_signed_in( );
}

// ---- The session kept for this tab - "format_tab_session( )" in "chat_parse.js" (2026-10-09)

const c_signin_lobby_room = "0000000";

function signin_keep_session( )
{
   try
   {
      if( ciyam.sessid !== "" )
         sessionStorage.setItem( c_tab_session_key, format_tab_session( ciyam ) );
   }
   catch( e )
   {
   }
}

function signin_forget_session( )
{
   try
   {
      sessionStorage.removeItem( c_tab_session_key );
   }
   catch( e )
   {
   }
}

function signin_kept_session( )
{
   try
   {
      return parse_tab_session( sessionStorage.getItem( c_tab_session_key ) );
   }
   catch( e )
   {
      return null;
   }
}

// NOTE: Takes up the session kept for this tab - an app switched to in it, or a reload - if the node still knows it,
// tried with one request: the lobby listing, which moves nothing the session has read. Refused - it timed out, or
// the node was set up again - it is forgotten, and the page signs in as usual. A session the node does not know is
// an error ("This web session is not valid", "No current session exists" - seen 2026-10-09), so only an error or no
// answer counts: a listing with no rooms in it is still a session (found by review). The hashed password is not kept for
// the tab, but where the person chose to save it on this browser it is here already - the accounts page needs it
// to check a password change.
async function signin_resume( )
{
   var kept = signin_kept_session( );

   if( kept === null )
      return false;

   ciyam.access = kept.access;
   ciyam.device = kept.device;
   ciyam.sessid = kept.sessid;
   ciyam.unique = kept.unique;
   ciyam.username = kept.username;
   ciyam.is_admin = kept.is_admin;
   ciyam.hashed = signin_saved_hash( kept.access ) || "";

   var reply = "";

   var issue = function( done )
   {
      return ciyam.fetch_messages( c_signin_lobby_room, "", done );
   };

   try
   {
      if( g_signin_options.request )
         reply = String( await g_signin_options.request( issue ) );
      else
         await issue( function( response ) { reply = String( response ); } );
   }
   catch( e )
   {
      reply = "";
   }

   if( ( reply.trim( ) === "" ) || is_error_response( reply ) )
   {
      ciyam.sessid = "";
      ciyam.unique = "";
      ciyam.hashed = "";
      ciyam.username = "";
      ciyam.is_admin = false;

      signin_forget_session( );

      return false;
   }

   return true;
}

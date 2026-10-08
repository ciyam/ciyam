// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The node's sign in, shared - one copy for every app, so the sign in is the same everywhere
// (Damon, 2026-10-08). Home uses it first; the chat, the accounts page and the console each have their
// own copy still, to move over one at a time with their browser suites as the check. It is the
// accounts page's, the newest of the three: the saved accounts, "Remember on this browser", a typed
// password tried again with a new device token after the node was set up again, "Reset this browser".
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

   var view = signin_element( "div", { class: "chat-signin", id: "signin_view", hidden: "" } );
   var form = signin_element( "form", { class: "chat-signin-form", novalidate: "" } );

   form.addEventListener( "submit", signin_submit );

   var brand = signin_element( "div", { class: "chat-brand" } );

   brand.appendChild( signin_element( "span", { class: "chat-logo" } ) );
   brand.appendChild( signin_element( "span", { class: "chat-wordmark" }, "CIYAM" ) );
   brand.appendChild( signin_element( "span", { class: "chat-brand-note" }, g_signin_options.note || "" ) );

   form.appendChild( brand );
   form.appendChild( signin_element( "h2", { tabindex: "-1" }, g_signin_options.title || "Sign in" ) );
   form.appendChild( signin_element( "p", { class: "chat-signin-lede" }, g_signin_options.lede || "" ) );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_access" }, "Account" ) );

   var access = signin_element( "select", { id: "signin_access", class: "chat-field chat-field--mono" } );

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

   actions.appendChild( signin_element( "button", { type: "submit", class: "chat-btn chat-btn--primary", id: "signin_submit" }, "Sign in" ) );

   if( g_signin_options.setup_href )
      actions.appendChild( signin_element( "a", { class: "chat-btn chat-signin-setup", id: "signin_setup",
       href: g_signin_options.setup_href }, "New here? Set up your account" ) );

   form.appendChild( actions );

   if( g_signin_options.setup_href )
      form.appendChild( signin_element( "div", { class: "chat-hint chat-signin-centred", id: "signin_setup_hint" },
       "With the code or PIN from the person who added you." ) );

   var foot = signin_element( "div", { class: "chat-signin-foot" } );
   var reset = signin_element( "button", { type: "button", class: "chat-btn--link", id: "signin_reset" }, "Reset this browser" );

   reset.addEventListener( "click", signin_reset_browser );

   foot.appendChild( reset );
   foot.appendChild( signin_element( "div", { class: "chat-signin-foot-note" }, "Forgets the device token and every saved account." ) );

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
   var select = document.getElementById( "signin_access" );

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
   var chosen = document.getElementById( "signin_access" ).value;

   return ( chosen !== "" ) ? chosen : document.getElementById( "signin_pin" ).value.trim( );
}

// NOTE: The PIN field only for a PIN not saved here; the Remember box shows what is saved for the
// account now, so signing in never silently forgets it. A saved password is used by leaving the field
// empty - it stays open, so a password that no longer works can be typed instead.
function signin_on_access( )
{
   var typed = ( document.getElementById( "signin_access" ).value === "" );

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
// longer knows, and every saved account - for every app, as they share them.
function signin_reset_browser( )
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

async function signin_connect( pin, hashed, password )
{
   ciyam.error = "";
   ciyam.unique = "";

   await g_signin_options.request( function( done )
   {
      return ciyam.connect( pin, ciyam.device, hashed, password, done );
   } );
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

   var submit = document.getElementById( "signin_submit" );

   submit.disabled = true;

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

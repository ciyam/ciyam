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
//   options.note           the word beside the wordmark - which part of the node this is ("home", "chat")
//   options.setup_href     where "New here?" goes - the accounts page's Welcome
//   options.request        the page's one-at-a-time request wrapper (ISS-005), resolving to the answer
//   options.on_signed_in   called once "ciyam" has a session
//   options.error_text     optional - a refusal in the page's own words, or "" to leave it as it is
//   options.ids            optional - { access, submit }: the ids a page's suites know its list and button by
//   options.on_busy        optional - called with true as the sign in starts and false as it ends
//
// The heading, its line and the button are the same everywhere (Damon, 2026-10-10: "the login screen should be the same
// for all" - the parts are one app, Ian's point). Only Home rewords it, for a locked node ("signin_describe( )").
//
// On a locked node it offers recovering with the twelve words, below its buttons - in place, whichever part this is,
// then on to "options.on_signed_in" as after a sign in (Damon, 2026-10-10; Home's alone before). Needs Ian's
// "bip39.min.js" beside it, read in only then, and "recovery_words( )" and the rest from "chat_parse.js".
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

const c_signin_title = "Sign in";

const c_signin_lede = "One PIN and password for every part of your node. The password is hashed in this browser and never sent"
 + " in the clear.";

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
   form.appendChild( signin_element( "h2", { tabindex: "-1" }, c_signin_title ) );
   form.appendChild( signin_element( "p", { class: "chat-signin-lede" }, c_signin_lede ) );

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

   // NOTE: A refusal stands out - a panel of its own (Damon, 2026-10-10: it was small grey-red type).
   form.appendChild( signin_element( "div", { class: "chat-error-text signin-error", id: "signin_error", role: "alert" } ) );

   var actions = signin_element( "div", { class: "chat-signin-actions" } );

   actions.appendChild( signin_element( "button", { type: "submit", class: "chat-btn chat-btn--primary", id: g_signin_ids.submit }, "Sign in" ) );

   if( g_signin_options.setup_href )
      actions.appendChild( signin_element( "a", { class: "chat-btn chat-signin-setup", id: "signin_setup",
       href: g_signin_options.setup_href }, "New here? Set up your account" ) );

   form.appendChild( actions );

   // NOTE: Recovering with the twelve words - only on a locked node ("signin_check_locked( )"), and below the buttons:
   // an edge case, rarely needed (Damon, 2026-10-10).
   var offer = signin_element( "button", { type: "button", class: "chat-btn chat-signin-setup recover-offer", id: "recover_offer", hidden: "" },
    "Can't sign in? Recover the node with its twelve words" );

   offer.addEventListener( "click", recover_show );

   form.appendChild( offer );

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

   host.appendChild( recover_build( ) );

   return view;
}

function signin_show( )
{
   document.getElementById( "signin_password" ).value = "";
   document.getElementById( "signin_pin" ).value = "";

   signin_set_text( "signin_error", "" );

   signin_fill_saved( );

   document.getElementById( "recover_view" ).hidden = true;
   document.getElementById( "signin_view" ).hidden = false;

   signin_check_locked( );
}

var g_signin_plain = false;

// NOTE: Whether the node is locked - "/system" names a locked one ":CIYAM:". A locked node offers recovery and not
// "New here?", which cannot work on it. Answers whether it is; "(NONE)" there is a connection not encrypted.
async function signin_check_locked( )
{
   var locked = false;

   try
   {
      var system = ( await ( await fetch( "/system", { cache: "no-store" } ) ).text( ) ).trim( );

      locked = ( system.indexOf( ":CIYAM:" ) === 0 );

      g_signin_plain = / \(NONE\)$/.test( system );
   }
   catch( e )
   {
   }

   document.getElementById( "recover_offer" ).hidden = !locked;

   if( locked )
   {
      document.querySelectorAll( "#signin_setup, #signin_setup_hint" ).forEach( function( element ) { element.hidden = true; } );
   }

   return locked;
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

// NOTE: The refusal, in words. A locked node answers a sign in in more than one way ("unable to start a web session",
// "not valid (or has expired)" - both seen 2026-10-10), so the node is asked, and a locked one said so - with its
// recovery offered beneath - unless the page has words of its own for it (Home's).
async function signin_show_refusal( error )
{
   var own = g_signin_options.error_text ? g_signin_options.error_text( error ) : "";

   signin_set_text( "signin_error", ( own !== "" ) ? own : ( is_unknown_device_error( error )
    ? "This browser's saved sign in is from before the node was set up again - type your password."
    : sign_in_error_text( error ) ) );

   var locked = await signin_check_locked( );

   if( locked && ( own === "" ) && ( document.getElementById( "signin_error" ).textContent !== "" ) )
      signin_set_text( "signin_error", sign_in_error_text( "Error: Was unable to start a web session." ) );
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
      signin_show_refusal( ciyam.error );

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

// ====================================================================
// Recovering the node with its twelve words (Ian, 2026-10-09; in every part, 2026-10-10)
// ====================================================================

// NOTE: The two cards - the words and a new master password, then admin's PIN - built here, as the sign in is, so
// every part of the node has the same. The ids are the ones "home_recover" knows them by.
function recover_build( )
{
   var view = signin_element( "div", { class: "chat-signin", id: "recover_view", hidden: "" } );
   var box = signin_element( "div", { class: "recover-box" } );

   var form = signin_element( "form", { class: "chat-signin-form recover-form", id: "recover_form", novalidate: "" } );

   form.addEventListener( "submit", recover_submit );

   var brand = signin_element( "div", { class: "chat-brand" } );

   brand.appendChild( signin_element( "span", { class: "chat-logo" } ) );
   brand.appendChild( signin_element( "span", { class: "chat-wordmark" }, "CIYAM" ) );
   brand.appendChild( signin_element( "span", { class: "chat-brand-note" }, g_signin_options.note || "" ) );

   form.appendChild( brand );
   form.appendChild( signin_element( "h2", { tabindex: "-1" }, "Recover this node" ) );
   form.appendChild( signin_element( "p", { class: "chat-signin-lede" },
    "With the twelve words written down when it was set up. The node checks them and takes a new master password, then"
    + " shows admin's PIN." ) );

   var set = signin_element( "fieldset", { class: "recover-words-set" } );

   set.appendChild( signin_element( "legend", { class: "chat-label" }, "Your twelve words" ) );

   var grid = signin_element( "div", { class: "recover-words", id: "recover_words" } );

   for( var i = 0; i < c_recovery_words; i++ )
   {
      var cell = signin_element( "label", { class: "recover-word" } );

      cell.appendChild( signin_element( "span", { class: "recover-word-number" }, String( i + 1 ) ) );

      var input = signin_element( "input", { type: "text", class: "chat-field", autocomplete: "off", autocapitalize: "none",
       spellcheck: "false", "aria-autocomplete": "list", "aria-controls": "recover_suggest", "aria-label": "Word " + ( i + 1 ),
       "data-index": String( i ) } );

      input.addEventListener( "input", recover_on_input );
      input.addEventListener( "keydown", recover_on_keydown );
      input.addEventListener( "blur", function( ) { recover_hide_suggestions( ); recover_words_status( ); } );

      cell.appendChild( input );
      grid.appendChild( cell );
   }

   set.appendChild( grid );
   set.appendChild( signin_element( "ul", { class: "recover-word-suggest", id: "recover_suggest", role: "listbox", "aria-label": "Words",
    hidden: "" } ) );

   form.appendChild( set );
   form.appendChild( signin_element( "div", { class: "signin-hint", id: "recover_words_status", role: "status" },
    "Type them in order, or paste all twelve into the first box." ) );

   var warning = signin_element( "div", { class: "recover-warning", id: "recover_warning", role: "alert", hidden: "" } );

   warning.appendChild( signin_element( "strong", { }, "This connection isn't encrypted." ) );
   warning.appendChild( document.createTextNode( " The node sees it as plain text - only recover from a device on the node's"
    + " own network." ) );

   form.appendChild( warning );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "recover_password" }, "New master password" ) );

   var password = signin_element( "input", { type: "password", id: "recover_password", class: "chat-field", autocomplete: "new-password" } );

   password.addEventListener( "input", function( )
   {
      var strength = password_strength( password.value );

      var meter = document.getElementById( "recover_strength" );

      meter.hidden = ( strength.level < 0 );
      meter.dataset.level = String( strength.level );

      signin_set_text( "recover_strength_label", strength.text );
   } );

   // NOTE: Enter goes on to the password again, as from each word - it is Enter there that sends the form.
   password.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Enter" ) && !event.shiftKey )
      {
         event.preventDefault( );

         document.getElementById( "recover_confirm" ).focus( );
      }
   } );

   form.appendChild( password );

   var meter = signin_element( "div", { class: "chat-strength", id: "recover_strength", hidden: "" } );
   var track = signin_element( "div", { class: "chat-strength-track" } );

   track.appendChild( signin_element( "div", { class: "chat-strength-bar" } ) );

   meter.appendChild( track );
   meter.appendChild( signin_element( "span", { class: "chat-strength-label", id: "recover_strength_label" } ) );

   form.appendChild( meter );
   form.appendChild( signin_element( "label", { class: "chat-label", for: "recover_confirm" }, "The same again" ) );
   form.appendChild( signin_element( "input", { type: "password", id: "recover_confirm", class: "chat-field", autocomplete: "new-password" } ) );
   form.appendChild( signin_element( "div", { class: "chat-error-text signin-error", id: "recover_error", role: "alert" } ) );
   form.appendChild( signin_element( "div", { class: "recover-status", id: "recover_status", role: "status" } ) );

   var actions = signin_element( "div", { class: "chat-signin-actions" } );

   actions.appendChild( signin_element( "button", { type: "submit", class: "chat-btn chat-btn--primary", id: "recover_submit" }, "Recover the node" ) );

   var back = signin_element( "button", { type: "button", class: "chat-btn chat-signin-setup", id: "recover_back" }, "Back to sign in" );

   back.addEventListener( "click", function( ) { signin_show( ); } );

   actions.appendChild( back );

   form.appendChild( actions );

   var done = signin_element( "section", { class: "chat-signin-form recover-form", id: "recover_done", "aria-labelledby": "recover_done_heading",
    hidden: "" } );

   done.appendChild( brand.cloneNode( true ) );
   done.appendChild( signin_element( "h2", { id: "recover_done_heading" }, "Recovered" ) );
   done.appendChild( signin_element( "p", { class: "chat-signin-lede" }, "The node is unlocked, with its new master password." ) );
   done.appendChild( signin_element( "span", { class: "chat-label" }, "Admin's PIN" ) );
   done.appendChild( signin_element( "span", { class: "recover-mono recover-pin", id: "recover_pin" } ) );
   done.appendChild( signin_element( "p", { class: "recover-note" }, "Write it down - it may be a new one, and only this one works"
    + " now. Then make a few unlock keys, in Home, so a restart can be undone without the words." ) );

   var done_actions = signin_element( "div", { class: "chat-signin-actions" } );

   var go_on = signin_element( "button", { type: "button", class: "chat-btn chat-btn--primary", id: "recover_continue" }, "Continue" );

   // NOTE: On as after a sign in - into whichever part of the node this is.
   go_on.addEventListener( "click", function( )
   {
      view.hidden = true;

      if( g_signin_options.on_signed_in )
         g_signin_options.on_signed_in( );
   } );

   done_actions.appendChild( go_on );
   done.appendChild( done_actions );

   box.appendChild( form );
   box.appendChild( done );
   view.appendChild( box );

   return view;
}

var g_recover_bip39 = null;

// NOTE: Ian's BIP39 library - large, so read in only when recovery opens, and once.
function recover_load_bip39( )
{
   if( g_recover_bip39 === null )
   {
      g_recover_bip39 = new Promise( function( resolve )
      {
         if( typeof BIP39 !== "undefined" )
         {
            resolve( true );

            return;
         }

         var script = document.createElement( "script" );

         script.src = "bip39.min.js";
         script.onload = function( ) { resolve( typeof BIP39 !== "undefined" ); };
         script.onerror = function( ) { g_recover_bip39 = null; resolve( false ); };

         document.body.appendChild( script );
      } );
   }

   return g_recover_bip39;
}

function recover_wordlist( )
{
   return ( typeof BIP39 !== "undefined" ) ? BIP39.DEFAULT_WORDLIST : [ ];
}

function recover_inputs( )
{
   return Array.from( document.querySelectorAll( "#recover_words input" ) );
}

async function recover_show( )
{
   recover_inputs( ).forEach( function( input ) { input.value = ""; input.classList.remove( "is-unknown" ); } );

   [ "recover_password", "recover_confirm" ].forEach( function( id ) { document.getElementById( id ).value = ""; } );

   document.getElementById( "recover_strength" ).hidden = true;
   document.getElementById( "recover_warning" ).hidden = !g_signin_plain;
   document.getElementById( "recover_form" ).hidden = false;
   document.getElementById( "recover_done" ).hidden = true;

   signin_set_text( "recover_error", "" );
   signin_set_text( "recover_status", "" );

   document.getElementById( "signin_view" ).hidden = true;
   document.getElementById( "recover_view" ).hidden = false;

   recover_inputs( )[ 0 ].focus( );

   if( !await recover_load_bip39( ) )
   {
      signin_set_text( "recover_error", "The word list could not be read - reload the page to try again." );

      return;
   }

   recover_words_status( );
}

// NOTE: Words pasted into a box - all twelve, say - go on across the boxes from it.
function recover_on_input( event )
{
   var input = event.target;

   // NOTE: A refusal was about the words as they were - it goes once they change.
   signin_set_text( "recover_error", "" );

   var words = recovery_words( input.value );

   if( ( words.length > 1 ) || /[\s,]/.test( input.value.trim( ) ) )
   {
      var inputs = recover_inputs( );
      var start = parseInt( input.dataset.index, 10 );

      words.slice( 0, inputs.length - start ).forEach( function( word, i ) { inputs[ start + i ].value = word; } );

      recover_hide_suggestions( );

      inputs[ Math.min( inputs.length - 1, start + words.length ) ].focus( );
   }
   else
      recover_show_suggestions( input );

   recover_words_status( );
}

// NOTE: What is wrong with the words, as they are typed - each box not one of the words marked, once it has a whole word,
// and the word still being typed left alone while it is the start of a real one.
function recover_words_status( )
{
   var wordlist = recover_wordlist( );

   if( wordlist.length === 0 )
      return "";

   recover_inputs( ).forEach( function( input )
   {
      var word = input.value.trim( ).toLowerCase( );

      input.classList.toggle( "is-unknown", ( word !== "" ) && ( wordlist.indexOf( word ) < 0 ) && ( document.activeElement !== input ) );
   } );

   var words = recover_inputs( ).filter( function( input )
   {
      var word = input.value.trim( ).toLowerCase( );

      return !( ( input === document.activeElement ) && ( wordlist.indexOf( word ) < 0 ) && ( word_completions( word, wordlist, 1 ).length > 0 ) );
   } ).map( function( input ) { return input.value.trim( ).toLowerCase( ); } ).filter( function( word ) { return word !== ""; } );

   var problem = ( words.length === 0 ) ? "" : recovery_words_problem( words, wordlist );

   if( ( problem === "" ) && ( words.length === c_recovery_words ) && !BIP39.validateMnemonic( words.join( " " ) ) )
      problem = c_recovery_words_mismatched;

   signin_set_text( "recover_words_status", ( problem !== "" ) ? problem : ( words.length === c_recovery_words
    ? "All twelve words are here, and they fit together." : "Type them in order, or paste all twelve into the first box." ) );

   return problem;
}

// ---- The words a box offers as it is typed in - from their start, as an autocomplete does (Damon, 2026-10-10: the
// browser's own list matched anywhere in a word). Up and down choose, Enter or Tab takes one and goes on to the next
// box, Escape closes, a click takes one.

const c_recover_suggestions = 6;

var g_recover_input = null;
var g_recover_words = [ ];
var g_recover_index = 0;

function recover_show_suggestions( input )
{
   var list = document.getElementById( "recover_suggest" );

   g_recover_words = word_completions( input.value, recover_wordlist( ), c_recover_suggestions );
   g_recover_index = 0;

   if( g_recover_words.length === 0 )
   {
      recover_hide_suggestions( );

      return;
   }

   g_recover_input = input;

   recover_draw_suggestions( );

   // NOTE: Beneath the box being typed in - the list sits in the words' fieldset, which it is placed against.
   var set = list.parentElement.getBoundingClientRect( );
   var box = input.getBoundingClientRect( );

   list.style.left = Math.round( box.left - set.left ) + "px";
   list.style.top = Math.round( box.bottom - set.top + 2 ) + "px";
   list.style.minWidth = Math.round( box.width ) + "px";
   list.hidden = false;
}

function recover_draw_suggestions( )
{
   var list = document.getElementById( "recover_suggest" );

   list.replaceChildren( );

   g_recover_words.forEach( function( word, i )
   {
      var option = signin_element( "li", { id: "recover_suggest_" + i, class: "recover-word-option" + ( ( i === g_recover_index ) ? " is-active" : "" ),
       role: "option", "aria-selected": String( i === g_recover_index ) }, word );

      // NOTE: On the press, before the box loses its focus.
      option.addEventListener( "mousedown", function( event )
      {
         event.preventDefault( );

         recover_take_suggestion( word );
      } );

      list.appendChild( option );
   } );

   if( g_recover_input !== null )
      g_recover_input.setAttribute( "aria-activedescendant", "recover_suggest_" + g_recover_index );
}

function recover_hide_suggestions( )
{
   document.getElementById( "recover_suggest" ).hidden = true;

   if( g_recover_input !== null )
      g_recover_input.removeAttribute( "aria-activedescendant" );

   g_recover_input = null;
   g_recover_words = [ ];
}

function recover_take_suggestion( word )
{
   var input = g_recover_input;

   recover_hide_suggestions( );

   if( input === null )
      return;

   input.value = word;

   recover_next_field( input );
}

// NOTE: On from a word's box - to the next, or after the last to the new password.
function recover_next_field( input )
{
   var next = recover_inputs( )[ parseInt( input.dataset.index, 10 ) + 1 ];

   ( next || document.getElementById( "recover_password" ) ).focus( );

   recover_words_status( );
}

// NOTE: Enter moves on from a box whether a word was chosen from the list or typed in full - the last box to the new
// password - and never sends the form from a word (Damon, 2026-10-10: Enter on a whole word sent it, and the count of
// words then stood as an error).
function recover_on_keydown( event )
{
   if( ( g_recover_input !== event.target ) || ( g_recover_words.length === 0 ) )
   {
      if( ( event.key === "Enter" ) && !event.shiftKey )
      {
         event.preventDefault( );

         recover_hide_suggestions( );

         recover_next_field( event.target );
      }

      return;
   }

   if( ( event.key === "ArrowDown" ) || ( event.key === "ArrowUp" ) )
   {
      event.preventDefault( );

      var count = g_recover_words.length;

      g_recover_index = ( g_recover_index + ( event.key === "ArrowDown" ? 1 : count - 1 ) ) % count;

      recover_draw_suggestions( );
   }
   else if( ( ( event.key === "Enter" ) || ( event.key === "Tab" ) ) && !event.shiftKey )
   {
      event.preventDefault( );

      recover_take_suggestion( g_recover_words[ g_recover_index ] );
   }
   else if( event.key === "Escape" )
   {
      event.preventDefault( );

      recover_hide_suggestions( );
   }
}

async function recover_submit( event )
{
   event.preventDefault( );

   signin_set_text( "recover_error", "" );

   if( !await recover_load_bip39( ) )
   {
      signin_set_text( "recover_error", "The word list could not be read - reload the page to try again." );

      return;
   }

   var words = recover_inputs( ).map( function( input ) { return input.value.trim( ).toLowerCase( ); } ).filter( function( word ) { return word !== ""; } );

   var problem = recovery_words_problem( words, recover_wordlist( ) );

   if( ( problem === "" ) && !BIP39.validateMnemonic( words.join( " " ) ) )
      problem = c_recovery_words_mismatched;

   if( problem !== "" )
   {
      signin_set_text( "recover_error", problem );

      return;
   }

   var password = document.getElementById( "recover_password" ).value;

   if( password_strength( password ).level < 1 )
   {
      signin_set_text( "recover_error", "The master password needs at least " + c_password_min_length + " characters." );

      return;
   }

   if( password !== document.getElementById( "recover_confirm" ).value )
   {
      signin_set_text( "recover_error", "The two passwords are not the same." );

      return;
   }

   var button = document.getElementById( "recover_submit" );

   button.disabled = true;

   signin_set_text( "recover_status", "Checking the words with the node..." );

   // NOTE: The words' entropy as "access", and the new password - "connect( )" in "ciyam.js" then does as Ian's curl
   // does: the node checks the entropy, answers with admin's PIN, and takes the password with the entropy, then the
   // session is made as for any sign in. A fresh device is registered, as from the terminal - kept with this tab's
   // session only, not as the browser's token: the passwords saved here for other accounts are hashed with that.
   var device = ciyam.device;

   recover_clear_ciyam( );

   ciyam.device = "";

   var reply = "";

   var issue = function( done )
   {
      return ciyam.connect( BIP39.mnemonicToEntropy( words.join( " " ) ), "", "", password, done );
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
      reply = "Error: " + ( ( e && e.message ) ? e.message : "the request failed." );
   }

   button.disabled = false;

   signin_set_text( "recover_status", "" );

   if( ( ciyam.error !== "" ) || ( ciyam.sessid === "" ) )
   {
      var error = ciyam.error || ( is_error_response( reply ) ? reply : "The node did not recover." );

      recover_clear_ciyam( );

      ciyam.device = device;

      signin_set_text( "recover_error", recovery_error_text( error ) );

      return;
   }

   document.getElementById( "recover_password" ).value = "";
   document.getElementById( "recover_confirm" ).value = "";

   signin_keep_session( );

   signin_set_text( "recover_pin", ciyam.access );

   document.getElementById( "recover_form" ).hidden = true;
   document.getElementById( "recover_done" ).hidden = false;

   // NOTE: The way on has the focus - not the heading, which looked strange outlined (Damon, 2026-10-10).
   document.getElementById( "recover_continue" ).focus( );
}

function recover_clear_ciyam( )
{
   ciyam.sessid = "";
   ciyam.access = "";
   ciyam.hashed = "";
   ciyam.unique = "";
   ciyam.username = "";
   ciyam.is_admin = false;
}

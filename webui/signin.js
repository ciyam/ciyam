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
// "plan_retain_choice( )", "sign_in_error_text( )", "account_display_name( )", "saved_account_label( )",
// "stale_saved_names( )", "pin_keyed_leftovers( )", "order_saved_accounts( )", "chosen_saved_index( )") and
// "account_parse.js" ("is_account_pin( )", "is_unknown_device_error( )"), and the page's "ciyam". The look is "chat.css"'s ".chat-signin".
//
// NOTE: Its own names throughout - the pages declare "c_storage_device" and the rest as "const", and a
// second declaration of one would stop the page. The keys stored are the same, so the apps share them.

const c_signin_storage_device = "cws.device";
const c_signin_storage_access = "cws.access";
const c_signin_storage_hashed_prefix = "cws.hashed_";

// NOTE: What opens an account's keys, kept on this browser ("account_keys.js", 2026-10-25) - a hash of its twelve words, not
// the words (Ian). Forgotten with the account, and the tab's copy on signing out ("signin_forget_tab_keys( )").
const c_signin_storage_keys_prefix = "cws.user_keys_";

// NOTE: One brought by another browser's QR code, held for the tab until the keys are shown ("account_keys.js").
const c_signin_pending_keys_name = "cws.pending_keys";

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

   form.appendChild( signin_element( "label", { class: "chat-label", for: g_signin_ids.access, id: "signin_access_label" }, "Account" ) );

   var access = signin_element( "select", { id: g_signin_ids.access, class: "chat-field chat-field--mono" } );

   access.appendChild( new Option( "Enter a PIN…", "" ) );
   access.addEventListener( "change", signin_choose_access );

   form.appendChild( access );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_pin", id: "signin_pin_label" }, "PIN" ) );

   // NOTE: Typed out of sight, like the password (Ian, 2026-10-10).
   var pin = signin_element( "input", { type: "password", id: "signin_pin", class: "chat-field chat-field--mono",
    inputmode: "numeric", maxlength: "5", autocomplete: "off" } );

   pin.addEventListener( "input", signin_choose_access );

   form.appendChild( pin );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_password", id: "signin_password_label" }, "Password" ) );
   form.appendChild( signin_element( "input", { type: "password", id: "signin_password", class: "chat-field",
    autocomplete: "current-password" } ) );

   // NOTE: A saved password is used without its field (Ian, 2026-10-29: an empty field looks as if a password must be typed) -
   // typed instead if it no longer works.
   var saved = signin_element( "div", { class: "signin-hint", id: "signin_hint", hidden: "" } );
   var type_it = signin_element( "button", { type: "button", class: "chat-btn--link", id: "signin_type_password" }, "Type it instead" );

   type_it.addEventListener( "click", function( ) { signin_type_password( true ); } );

   saved.appendChild( signin_element( "span", { }, "The password is saved on this browser." ) );
   saved.appendChild( document.createTextNode( " " ) );
   saved.appendChild( type_it );

   form.appendChild( saved );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_retain" }, "Remember on this browser" ) );

   var retain = signin_element( "select", { id: "signin_retain", class: "chat-field" } );

   retain.appendChild( new Option( "Nothing — forget this account", "none" ) );
   retain.appendChild( new Option( "The PIN only", "access" ) );
   retain.appendChild( new Option( "The PIN and the password", "full" ) );

   form.appendChild( retain );
   form.appendChild( signin_element( "div", { class: "signin-hint" }, "Saving the password as well signs you in without typing it." ) );

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
   busy.appendChild( signin_element( "span", { id: "signin_busy_text", role: "status" }, "Contacting the server…" ) );

   form.appendChild( busy );

   // NOTE: How far securing the password has got - it is hardened before it is sent, a second or two (Ian, 2026-10-25).
   form.appendChild( signin_element( "progress", { class: "chat-signin-progress", id: "signin_busy_bar", max: "1", value: "0", hidden: "" } ) );

   if( g_signin_options.setup_href )
      form.appendChild( signin_element( "div", { class: "chat-hint chat-signin-centred", id: "signin_setup_hint" },
       "With the code or PIN from the person who added you." ) );

   // NOTE: One line (Damon, 2026-10-28: the sign in scrolled) - what it forgets is said by the question it asks first.
   var foot = signin_element( "div", { class: "chat-signin-foot" } );
   var reset = signin_element( "button", { type: "button", class: "chat-btn--link", id: "signin_reset" }, "Reset this browser" );

   reset.addEventListener( "click", signin_reset_browser );

   foot.appendChild( signin_element( "span", { }, "Trouble signing in?" ) );
   foot.appendChild( document.createTextNode( " " ) );
   foot.appendChild( reset );

   form.appendChild( foot );
   view.appendChild( form );

   // NOTE: A node not set up yet - nobody can sign in; it is set up from Home (Ian, 2026-10-13: a Back from setting up
   // landed him on a part's sign in).
   var fresh = signin_element( "div", { class: "chat-signin-form", id: "signin_new_node", hidden: "" } );
   var fresh_brand = signin_element( "div", { class: "chat-brand" } );

   fresh_brand.appendChild( signin_element( "span", { class: "chat-logo" } ) );
   fresh_brand.appendChild( signin_element( "span", { class: "chat-wordmark" }, "CIYAM" ) );
   fresh_brand.appendChild( signin_element( "span", { class: "chat-brand-note" }, g_signin_options.note || "" ) );

   fresh.appendChild( fresh_brand );
   fresh.appendChild( signin_element( "h2", { tabindex: "-1" }, "This node isn't set up yet" ) );
   fresh.appendChild( signin_element( "p", { class: "chat-signin-lede" },
    "It has no identity yet, so nobody can sign in. It's set up from Home - and if you were part way, Home carries on." ) );

   var fresh_actions = signin_element( "div", { class: "chat-signin-actions" } );

   fresh_actions.appendChild( signin_element( "a", { class: "chat-btn chat-btn--primary", id: "signin_new_node_home", href: "home.html" },
    "Set it up in Home" ) );

   fresh.appendChild( fresh_actions );
   view.appendChild( fresh );

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

   recover_clear_words( );

   signin_check_locked( );
}

var g_signin_plain = false;

var g_signin_node_known = false;

// NOTE: Whether the node is locked - "/system" names a locked one ":CIYAM:". A locked node offers recovery and not
// "New here?", which cannot work on it. Answers whether it is; "(NONE)" there is a connection not encrypted.
async function signin_check_locked( )
{
   var locked = false;
   var fresh = false;
   var known = false;

   try
   {
      var system = ( await ( await fetch( "/system", { cache: "no-store" } ) ).text( ) ).trim( );

      locked = ( system.indexOf( ":CIYAM:" ) === 0 );
      fresh = ( system.indexOf( "*CIYAM*" ) === 0 );
      known = ( system.indexOf( "CIYAM" ) >= 0 );

      g_signin_plain = / \(NONE\)$/.test( system );
   }
   catch( e )
   {
   }

   // NOTE: Not set up - the sign in gives way to the way to Home ("signin_new_node"). Whether the node answered at all is
   // kept, so a refusal is not said to be a lock unless the node said so ("g_signin_node_known").
   document.getElementById( "signin_form" ).hidden = fresh;
   document.getElementById( "signin_new_node" ).hidden = !fresh;

   g_signin_node_known = known;

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

   var locked = await signin_check_locked( );

   // NOTE: Locked only when the node says so - "unable to start a web session" from one that is not is said as that
   // (2026-10-13: a node waiting on its chat refuses so too).
   var state = g_signin_node_known ? locked : undefined;

   signin_set_text( "signin_error", ( own !== "" ) ? own : ( ( is_unknown_device_error( error ) && !locked )
    ? "This browser's saved sign in is from before the node was set up again - type your password."
    : sign_in_error_text( error, state ) ) );
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

function signin_saved_name( pin )
{
   return signin_storage_get( c_saved_name_prefix + pin ) || "";
}

// NOTE: The name shown for a saved PIN - kept while the PIN is saved, let go when it is not. The accounts page calls it
// too, after a claim.
function signin_remember_name( pin, name )
{
   try
   {
      if( parse_access_list( localStorage.getItem( c_signin_storage_access ) ).indexOf( pin ) >= 0 )
         localStorage.setItem( c_saved_name_prefix + pin, name );
      else
         localStorage.removeItem( c_saved_name_prefix + pin );
   }
   catch( e )
   {
   }
}

function signin_forget_stale_names( )
{
   try
   {
      var keys = [ ];

      for( var i = 0; i < localStorage.length; i++ )
         keys.push( localStorage.key( i ) );

      stale_saved_names( keys, localStorage.getItem( c_signin_storage_access ) ).concat( pin_keyed_leftovers( keys ) )
       .forEach( function( key ) { localStorage.removeItem( key ); } );
   }
   catch( e )
   {
   }
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

// NOTE: The accounts saved on this browser, by any app, ahead of "Enter a PIN...", the first chosen - by name, never by
// PIN (Ian, 2026-10-10).
function signin_fill_saved( )
{
   signin_forget_stale_names( );

   var select = document.getElementById( g_signin_ids.access );

   for( var i = select.options.length - 1; i >= 0; i-- )
   {
      if( select.options[ i ].value !== "" )
         select.remove( i );
   }

   var ordered = order_saved_accounts( signin_saved_pins( ).map( function( pin ) { return { pin: pin, name: signin_saved_name( pin ) }; } ) );

   ordered.forEach( function( account, n )
   {
      select.options.add( new Option( account.label + ( ( signin_saved_hash( account.pin ) !== null ) ? c_signin_saved_mark : "" ),
       account.pin, false ), n );
   } );

   var last = null;

   try
   {
      last = sessionStorage.getItem( c_signin_last_account_key );
   }
   catch( e )
   {
   }

   select.selectedIndex = chosen_saved_index( ordered, last );

   // NOTE: The list only once there is an account saved here - with none, its one choice is "Enter a PIN…" (Damon, 2026-10-28:
   // the sign in scrolled).
   document.getElementById( "signin_access_label" ).hidden = ( ordered.length === 0 );
   select.hidden = ( ordered.length === 0 );

   signin_choose_access( );
}

function signin_pin( )
{
   var chosen = document.getElementById( g_signin_ids.access ).value;

   return ( chosen !== "" ) ? chosen : document.getElementById( "signin_pin" ).value.trim( );
}

// NOTE: Whether the password is being typed rather than the saved one used - "Type it instead", or the saved one refused;
// forgotten as another account is chosen.
var g_signin_typing = false;

function signin_choose_access( )
{
   g_signin_typing = false;

   signin_on_access( );
}

// NOTE: The PIN field only for a PIN not saved here; the Remember box shows what is saved for the account now, so signing in
// never silently forgets it. With a saved password, no password field - the saved one is used - but a line saying so, and
// "Type it instead" for one that no longer works.
function signin_on_access( )
{
   var typed = ( document.getElementById( g_signin_ids.access ).value === "" );

   document.getElementById( "signin_pin_label" ).hidden = !typed;
   document.getElementById( "signin_pin" ).hidden = !typed;

   var pin = signin_pin( );

   var has_hash = is_account_pin( pin ) && ( signin_saved_hash( pin ) !== null );

   var use_saved = has_hash && !g_signin_typing;

   document.getElementById( "signin_password_label" ).hidden = use_saved;
   document.getElementById( "signin_password" ).hidden = use_saved;
   document.getElementById( "signin_hint" ).hidden = !use_saved;

   if( use_saved )
      document.getElementById( "signin_password" ).value = "";

   document.getElementById( "signin_retain" ).value =
    retain_mode_of( signin_storage_get( c_signin_storage_access ), is_account_pin( pin ) ? pin : "", has_hash );
}

// NOTE: The password field back for the account chosen - asked for, or as the saved password was refused.
function signin_type_password( focus )
{
   g_signin_typing = true;

   signin_on_access( );

   if( focus )
      document.getElementById( "signin_password" ).focus( );
}

// NOTE: After a sign in, what the Remember box asked for - "plan_retain_choice( )" in "chat_parse.js". "mode" - another
// page's own Remember box (the end of setting up, 2026-10-13); otherwise the sign in's. "hashed" - the password's hash to
// keep, when not this session's (after a recovery, made for the browser's own device).
function signin_apply_retain( mode, hashed )
{
   try
   {
      var plan = plan_retain_choice( localStorage.getItem( c_signin_storage_access ), ciyam.access,
       mode || document.getElementById( "signin_retain" ).value, hashed || ciyam.hashed );

      if( plan.keep_hash )
         localStorage.setItem( c_signin_storage_hashed_prefix + ciyam.access, hashed || ciyam.hashed );
      else
         localStorage.removeItem( c_signin_storage_hashed_prefix + ciyam.access );

      if( parse_access_list( plan.list ).indexOf( ciyam.access ) < 0 )
         localStorage.removeItem( c_signin_storage_keys_prefix + ciyam.access );

      if( plan.list === null )
         localStorage.removeItem( c_signin_storage_access );
      else
         localStorage.setItem( c_signin_storage_access, plan.list );
   }
   catch( e )
   {
   }

   signin_remember_name( ciyam.access, account_display_name( ciyam.username, ciyam.is_admin ) );
}

// NOTE: Everything this browser keeps for the node goes: the device token, which a rebuilt node no
// longer knows, every saved account - for every app, as they share them - and the rest: dismissed
// announcements and recent emoji. Said so on the page (found by review).
async function signin_reset_browser( )
{
   if( ciyam.sessid !== "" )
      return;

   if( !await ask_dialog( { title: "Reset this browser?", text: "The device token, every saved account and this browser's other"
    + " settings for the node are forgotten.", yes: "Reset", danger: true } ) )
      return;

   try
   {
      localStorage.clear( );
      sessionStorage.removeItem( c_signin_last_account_key );
   }
   catch( e )
   {
   }

   signin_forget_tab_keys( "" );

   location.reload( );
}

// NOTE: What the sign in is doing, under the spinner ("sign_in_progress( )" in "chat_parse.js").
function signin_show_progress( stage, value )
{
   var shown = sign_in_progress( stage, value );
   var bar = document.getElementById( "signin_busy_bar" );

   signin_set_text( "signin_busy_text", shown.text );

   if( bar )
   {
      bar.hidden = ( shown.fraction === null );

      if( shown.fraction !== null )
         bar.value = shown.fraction;
   }
}

// NOTE: "on_progress" shows what it is doing somewhere else - the console's own sign in, from its prompt - the card by default.
async function signin_connect( pin, hashed, password, on_progress )
{
   ciyam.error = "";
   ciyam.unique = "";

   ciyam.on_progress = on_progress || signin_show_progress;

   var issue = function( done )
   {
      return ciyam.connect( pin, ciyam.device, hashed, password, done );
   };

   try
   {
      if( g_signin_options.request )
         await g_signin_options.request( issue );
      else
         await issue( function( ) { } );
   }
   finally
   {
      ciyam.on_progress = null;

      ( on_progress || signin_show_progress )( null, null );
   }
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
      // NOTE: The saved password refused - changed since, or saved before the node was set up again - so the field is offered.
      if( ( password === "" ) && is_saved_password_refused( ciyam.error ) )
         signin_type_password( true );

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

// NOTE: The account this tab is signed in as is kept apart too, and outlasts signing out - the sign in starts on it
// ("chosen_saved_index( )").
const c_signin_last_account_key = "ciyam.last_account";

function signin_keep_session( )
{
   try
   {
      if( ciyam.sessid !== "" )
      {
         sessionStorage.setItem( c_tab_session_key, format_tab_session( ciyam ) );

         signin_forget_tab_keys( ciyam.access );

         // NOTE: Only an account saved on this browser - "Remember: Nothing" leaves no PIN behind (found by review).
         if( signin_saved_pins( ).indexOf( ciyam.access ) >= 0 )
            sessionStorage.setItem( c_signin_last_account_key, ciyam.access );
         else
            sessionStorage.removeItem( c_signin_last_account_key );
      }
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

   signin_forget_tab_keys( "" );
}

// NOTE: The access keys this tab keeps - an account not remembered keeps its in "sessionStorage", which outlived signing out,
// a reload and clearing "localStorage" (Ian, 2026-10-27). All but "kept_pin"'s go: none on signing out, the account's own
// on signing in, so a reload while signed in still has it.
function signin_forget_tab_keys( kept_pin )
{
   try
   {
      var names = [ ];

      for( var i = 0; i < sessionStorage.length; i++ )
      {
         var name = sessionStorage.key( i );

         if( ( name.indexOf( c_signin_storage_keys_prefix ) === 0 ) && ( name !== c_signin_storage_keys_prefix + kept_pin ) )
            names.push( name );
      }

      if( kept_pin === "" )
         names.push( c_signin_pending_keys_name );

      names.forEach( function( name ) { sessionStorage.removeItem( name ); } );
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
// answer counts. An empty reply is no answer: the listing always begins with the entrance's line of members, so even
// someone in no rooms gets one (seen 2026-10-12 - tester-2, out of every room it could leave). The hashed password is
// not kept for the tab, but where the person chose to save it on this browser it is here already - the accounts page
// needs it to check a password change.
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
   // NOTE: The version this browser keeps for the PIN - the hash taken up was made that way (found by review, 2026-10-23).
   ciyam.hash_version = ciyam.hash_version_for( kept.access );

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

   // NOTE: The chat not turned on refuses the lobby, though the session is good (Ian, 2026-10-13) - so it is kept, and
   // the part says why it cannot go on.
   if( ( reply.trim( ) === "" ) || ( is_error_response( reply ) && !irc_unavailable( reply ) ) )
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

   var words = recovery_texts( false );

   form.appendChild( brand );

   // NOTE: Two steps, as setting up has (Damon, 2026-10-13): the twelve words, then the new master password - all on one
   // card it was long, and scrolled on a phone. "recover_show_step( )" shows one.
   var step_words = signin_element( "div", { class: "recover-step", id: "recover_step_words" } );
   var step_password = signin_element( "div", { class: "recover-step", id: "recover_step_password", hidden: "" } );

   form.appendChild( step_words );
   form.appendChild( step_password );

   step_words.appendChild( signin_element( "h2", { tabindex: "-1", id: "recover_heading" }, words.heading ) );
   step_words.appendChild( signin_element( "p", { class: "chat-signin-lede", id: "recover_lede" }, words.lede ) );

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

   step_words.appendChild( set );
   step_words.appendChild( signin_element( "div", { class: "signin-hint", id: "recover_words_status", role: "status" },
    "Type them in order, or paste all twelve into the first box." ) );

   // NOTE: Recovering onto a node not set up - nothing there can check the words ("recovery_texts( true )").
   step_words.appendChild( signin_element( "div", { class: "recover-warning", id: "recover_caution", hidden: "" } ) );

   var warning = signin_element( "div", { class: "recover-warning", id: "recover_warning", role: "alert", hidden: "" } );

   warning.appendChild( signin_element( "strong", { }, "This connection isn't encrypted." ) );
   warning.appendChild( document.createTextNode( " The node sees it as plain text - only recover from a device on the node's"
    + " own network." ) );

   step_words.appendChild( warning );
   step_words.appendChild( signin_element( "div", { class: "chat-error-text signin-error", id: "recover_words_error", role: "alert" } ) );

   var words_actions = signin_element( "div", { class: "chat-signin-actions" } );

   var next = signin_element( "button", { type: "button", class: "chat-btn chat-btn--primary", id: "recover_words_next" }, "Next" );

   next.addEventListener( "click", recover_words_go );

   words_actions.appendChild( next );
   step_words.appendChild( words_actions );

   step_password.appendChild( signin_element( "h2", { tabindex: "-1", id: "recover_password_heading" }, "A new master password" ) );
   step_password.appendChild( signin_element( "p", { class: "chat-signin-lede" },
    "It takes the place of the old one - admin's password, and what unlocks the node after a restart." ) );
   step_password.appendChild( signin_element( "label", { class: "chat-label", for: "recover_password" }, "New master password" ) );

   var password = signin_element( "input", { type: "password", id: "recover_password", class: "chat-field", autocomplete: "new-password" } );

   var rate = function( )
   {
      var strength = password_strength( password.value );

      var meter = document.getElementById( "recover_strength" );

      meter.hidden = ( strength.level < 0 );
      meter.dataset.level = String( strength.level );

      signin_set_text( "recover_strength_label", strength.text );
   };

   // NOTE: Rated by guesses once "zxcvbn.min.js" is in - read in at the first key, and rated again then.
   password.addEventListener( "input", function( ) { load_strength_rules( rate ); rate( ); } );

   // NOTE: Enter goes on to the password again, as from each word - it is Enter there that sends the form.
   password.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Enter" ) && !event.shiftKey )
      {
         event.preventDefault( );

         document.getElementById( "recover_confirm" ).focus( );
      }
   } );

   step_password.appendChild( password );

   var meter = signin_element( "div", { class: "chat-strength", id: "recover_strength", hidden: "" } );
   var track = signin_element( "div", { class: "chat-strength-track" } );

   track.appendChild( signin_element( "div", { class: "chat-strength-bar" } ) );

   meter.appendChild( track );
   meter.appendChild( signin_element( "span", { class: "chat-strength-label", id: "recover_strength_label" } ) );

   step_password.appendChild( meter );

   var suggest = signin_element( "button", { type: "button", class: "chat-btn chat-btn--link chat-suggest", id: "recover_suggest" }, "Suggest one" );

   suggest.addEventListener( "click", function( ) { offer_passphrase( "recover_password", "recover_confirm", "recover_suggested" ); } );

   step_password.appendChild( suggest );
   step_password.appendChild( signin_element( "p", { class: "chat-hint chat-suggested", id: "recover_suggested", role: "status", hidden: "" } ) );
   step_password.appendChild( signin_element( "label", { class: "chat-label", for: "recover_confirm" }, "The same again" ) );
   step_password.appendChild( signin_element( "input", { type: "password", id: "recover_confirm", class: "chat-field", autocomplete: "new-password" } ) );

   attach_reveals( step_password );
   step_password.appendChild( signin_element( "div", { class: "chat-error-text signin-error", id: "recover_error", role: "alert" } ) );
   step_password.appendChild( signin_element( "div", { class: "recover-status", id: "recover_status", role: "status" } ) );

   var actions = signin_element( "div", { class: "chat-signin-actions" } );

   actions.appendChild( signin_element( "button", { type: "submit", class: "chat-btn chat-btn--primary", id: "recover_submit" }, "Recover the node" ) );

   var back = signin_element( "button", { type: "button", class: "chat-btn chat-signin-setup", id: "recover_back" }, words.back );

   // NOTE: Back to the sign in - or, recovering onto a node not set up, to wherever it was opened from.
   back.addEventListener( "click", function( )
   {
      if( g_recover_fresh !== null )
      {
         var on_back = g_recover_fresh.on_back;

         g_recover_fresh = null;

         recover_clear_words( );
         recover_clear_passwords( );

         view.hidden = true;

         on_back( );
      }
      else
      {
         recover_clear_passwords( );

         signin_show( );
      }
   } );

   words_actions.appendChild( back );

   var to_words = signin_element( "button", { type: "button", class: "chat-btn chat-signin-setup", id: "recover_password_back" }, "Back to the words" );

   to_words.addEventListener( "click", function( ) { recover_show_step( "words" ); } );

   actions.appendChild( to_words );

   step_password.appendChild( actions );

   var done = signin_element( "section", { class: "chat-signin-form recover-form", id: "recover_done", "aria-labelledby": "recover_done_heading",
    hidden: "" } );

   done.appendChild( brand.cloneNode( true ) );
   done.appendChild( signin_element( "h2", { id: "recover_done_heading" }, words.done_heading ) );
   done.appendChild( signin_element( "p", { class: "chat-signin-lede", id: "recover_done_lede" }, words.done_lede ) );
   done.appendChild( signin_element( "span", { class: "chat-label" }, "Admin's PIN" ) );
   done.appendChild( signin_element( "span", { class: "recover-mono recover-pin", id: "recover_pin" } ) );
   done.appendChild( signin_element( "p", { class: "recover-note", id: "recover_done_note" }, words.done_note ) );

   // NOTE: Admin's PIN, and the password, remembered on this browser as setting up's end offers (2026-10-13) - the browser
   // started afresh by the recovery, with this session's device as its own (Ian, 2026-10-14).
   done.appendChild( signin_element( "label", { class: "chat-label", for: "recover_retain" }, "Remember on this browser" ) );

   var retain = signin_element( "select", { id: "recover_retain", class: "chat-field" } );

   retain.appendChild( new Option( "Nothing — forget this account", "none" ) );
   retain.appendChild( new Option( "The PIN only", "access" ) );
   retain.appendChild( new Option( "The PIN and the password", "full" ) );

   done.appendChild( retain );
   done.appendChild( signin_element( "div", { class: "signin-hint" }, "This browser starts afresh - what it kept for the node from before"
    + " was cleared. Saving the password as well signs you in without typing it." ) );

   var done_actions = signin_element( "div", { class: "chat-signin-actions" } );

   var go_on = signin_element( "button", { type: "button", class: "chat-btn chat-btn--primary", id: "recover_continue" }, "Continue" );

   // NOTE: On as after a sign in - into whichever part of the node this is.
   go_on.addEventListener( "click", function( )
   {
      signin_apply_retain( document.getElementById( "recover_retain" ).value, g_recover_hash );

      g_recover_hash = "";

      view.hidden = true;

      g_recover_fresh = null;

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

// NOTE: Recovering onto a node not set up - { on_back } - or null for a locked node's recovery.
var g_recover_fresh = null;

// NOTE: The new password's hash, for Remember - "" until a recovery succeeds.
var g_recover_hash = "";

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

// NOTE: "zxcvbn.min.js" - read in the first time a password is chosen (it is over 800 KB), and then the rating is by guesses
// ("password_strength( )" in "chat_parse.js"); "then" is run once it is in, to rate again what is typed.
var g_strength_rules = null;

function load_strength_rules( then )
{
   if( g_strength_rules === null )
   {
      g_strength_rules = new Promise( function( resolve )
      {
         if( typeof zxcvbn === "function" )
         {
            resolve( true );

            return;
         }

         var script = document.createElement( "script" );

         script.src = "zxcvbn.min.js";
         script.onload = function( ) { resolve( typeof zxcvbn === "function" ); };
         script.onerror = function( ) { g_strength_rules = null; resolve( false ); };

         document.body.appendChild( script );
      } );
   }

   // NOTE: Only when it is still to come - "then" usually rates again, and asks again as it does, so with it already in that
   // would never end (found by the suites, 2026-10-25: the page hung).
   if( then && ( typeof zxcvbn !== "function" ) )
      g_strength_rules.then( function( loaded ) { if( loaded ) then( ); } );

   return g_strength_rules;
}

// NOTE: A reveal button in a new password's field - the eye - showing the password as typed, or hiding it again (Damon,
// 2026-10-25: "if we get a suggested password, we have no way of seeing what was suggested"). Each field once; the field is
// wrapped so the button sits inside it. "attach_reveals( )" finds them all - "autocomplete" of "new-password".
function attach_reveal( input )
{
   if( !input || ( input.dataset.reveal === "1" ) )
      return;

   input.dataset.reveal = "1";

   var wrap = signin_element( "span", { class: "chat-reveal-wrap" } );

   input.parentNode.insertBefore( wrap, input );
   wrap.appendChild( input );

   // NOTE: Not a tab stop (Ian, 2026-10-27) - Password to its "type it again" is one tab; from the keys, the checkbox after both
   // shows them ("reveal_check( )").
   var button = signin_element( "button", { type: "button", class: "chat-reveal", id: input.id + "_reveal", tabindex: "-1" } );
   var ns = "http://www.w3.org/2000/svg";
   var icon = document.createElementNS( ns, "svg" );

   icon.setAttribute( "viewBox", "0 0 24 24" );
   icon.setAttribute( "aria-hidden", "true" );
   icon.setAttribute( "focusable", "false" );

   [ [ "path", { d: "M1.5 12S5.5 5 12 5s10.5 7 10.5 7-4 7-10.5 7S1.5 12 1.5 12z" } ], [ "circle", { cx: "12", cy: "12", r: "3" } ],
    [ "line", { x1: "3", y1: "21", x2: "21", y2: "3", class: "chat-reveal-slash" } ] ].forEach( function( part )
   {
      var shape = document.createElementNS( ns, part[ 0 ] );

      Object.keys( part[ 1 ] ).forEach( function( name ) { shape.setAttribute( name, part[ 1 ][ name ] ); } );

      icon.appendChild( shape );
   } );

   button.appendChild( icon );
   button.addEventListener( "click", function( )
   {
      var shown = ( input.type === "password" );

      reveal_partners( input ).forEach( function( field ) { set_revealed( field, shown ); } );
   } );

   wrap.appendChild( button );

   set_revealed( input, false );
}

function set_revealed( input, shown )
{
   var button = document.getElementById( input.id + "_reveal" );

   input.type = shown ? "text" : "password";

   if( button )
   {
      var words = shown ? "Hide password" : "Show password";

      button.setAttribute( "aria-label", words );
      button.setAttribute( "aria-pressed", shown ? "true" : "false" );
      button.title = words;
      button.classList.toggle( "is-shown", !!shown );
   }

   var partners = reveal_partners( input );
   var check = ( partners.length > 0 ) ? document.getElementById( partners[ partners.length - 1 ].id + "_show" ) : null;

   if( check )
      check.checked = !!shown;
}

// NOTE: A new password and its "type it again" are shown or hidden together (Damon, 2026-10-25) - the new password fields
// with a reveal in the same form, or the same section of Home's set-up or step of recovery.
function reveal_partners( input )
{
   var group = input.closest( "form, section, .recover-step" );

   if( !group )
      return [ input ];

   return Array.from( group.querySelectorAll( "input[autocomplete='new-password']" ) ).filter( function( field )
   {
      return field.dataset.reveal === "1";
   } );
}

function attach_reveals( root )
{
   var fields = Array.from( ( root || document ).querySelectorAll( "input[autocomplete='new-password']" ) );

   fields.forEach( attach_reveal );

   // NOTE: Each new password and its "type it again" one tab apart (Ian, 2026-10-27, as the legacy page): "Suggest one" on the
   // first's label row - a back-tab from it - and a checkbox after the last to show them both, the eyes being no tab stops.
   fields.forEach( function( field )
   {
      var partners = reveal_partners( field );

      if( partners[ 0 ] === field )
         suggest_on_label( field );

      if( partners[ partners.length - 1 ] === field )
         reveal_check( field );
   } );
}

function suggest_on_label( field )
{
   var group = field.closest( "form, section, .recover-step" ) || field.parentNode;
   var suggest = group.querySelector( ".chat-suggest" );
   var label = group.querySelector( "label[for='" + field.id + "']" );

   if( !suggest || !label || label.parentNode.classList.contains( "chat-label-row" ) )
      return;

   var row = signin_element( "div", { class: "chat-label-row" } );

   label.parentNode.insertBefore( row, label );
   row.appendChild( label );
   row.appendChild( suggest );
}

function reveal_check( field )
{
   if( document.getElementById( field.id + "_show" ) )
      return;

   var partners = reveal_partners( field );
   var after = field.closest( ".chat-reveal-wrap" ) || field;
   var label = signin_element( "label", { class: "chat-reveal-check" } );
   var check = signin_element( "input", { type: "checkbox", id: field.id + "_show" } );

   label.appendChild( check );
   label.appendChild( document.createTextNode( ( partners.length > 1 ) ? "Show the passwords" : "Show the password" ) );

   after.parentNode.insertBefore( label, after.nextSibling );

   check.addEventListener( "change", function( )
   {
      reveal_partners( field ).forEach( function( partner ) { set_revealed( partner, check.checked ); } );
   } );

   // NOTE: What "Suggest one" offers, to write down - under both now, after the checkbox.
   var group = field.closest( "form, section, .recover-step" ) || field.parentNode;
   var note = group.querySelector( ".chat-suggested" );

   if( note )
      label.parentNode.insertBefore( note, label.nextSibling );
}

// NOTE: "Suggest one" (Ian, 2026-10-23: a suggested passphrase of five words) - five BIP39 words fill the new password and
// its confirmation, each told it changed, shown in both - as a browser's own password manager does - and in "note_id", to be
// written down.
async function offer_passphrase( password_id, confirm_id, note_id )
{
   if( !await recover_load_bip39( ) )
   {
      signin_set_text( note_id, "A passphrase could not be made - the word list did not load. Try again." );
      document.getElementById( note_id ).hidden = false;

      return;
   }

   var phrase = suggested_passphrase( BIP39.DEFAULT_WORDLIST, crypto.getRandomValues( new Uint16Array( 5 ) ) );

   [ password_id, confirm_id ].forEach( function( id )
   {
      var field = document.getElementById( id );

      field.value = phrase;
      field.dispatchEvent( new Event( "input", { bubbles: true } ) );

      attach_reveal( field );
      set_revealed( field, true );
   } );

   // NOTE: One line - it is in both fields too (Damon, 2026-10-28: Welcome pushed past the bottom of the screen).
   signin_set_text( note_id, "Write this down: " + phrase );
   document.getElementById( note_id ).hidden = false;
}

function recover_wordlist( )
{
   return ( typeof BIP39 !== "undefined" ) ? BIP39.DEFAULT_WORDLIST : [ ];
}

function recover_inputs( )
{
   return Array.from( document.querySelectorAll( "#recover_words input" ) );
}

// NOTE: "options" - { fresh: true, on_back } to recover onto a node not set up (setting up's choice, Damon, 2026-10-13);
// anything else - the sign in's click - is a locked node's recovery.
async function recover_show( options )
{
   var fresh = !!( options && options.fresh );

   g_recover_fresh = fresh ? { on_back: options.on_back } : null;

   var words = recovery_texts( fresh );

   signin_set_text( "recover_heading", words.heading );
   signin_set_text( "recover_lede", words.lede );
   signin_set_text( "recover_caution", words.caution );
   signin_set_text( "recover_back", words.back );
   signin_set_text( "recover_done_heading", words.done_heading );
   signin_set_text( "recover_done_lede", words.done_lede );
   signin_set_text( "recover_done_note", words.done_note );

   document.getElementById( "recover_caution" ).hidden = !fresh;

   // NOTE: Over plain HTTP the warning shows - known from "/system", asked here when no sign in has asked yet.
   if( fresh )
      await signin_check_locked( );

   recover_inputs( ).forEach( function( input ) { input.value = ""; input.classList.remove( "is-unknown" ); } );

   [ "recover_password", "recover_confirm" ].forEach( function( id ) { document.getElementById( id ).value = ""; } );

   document.getElementById( "recover_strength" ).hidden = true;
   document.getElementById( "recover_warning" ).hidden = !g_signin_plain;
   document.getElementById( "recover_form" ).hidden = false;
   document.getElementById( "recover_done" ).hidden = true;
   document.getElementById( "recover_step_words" ).hidden = false;
   document.getElementById( "recover_step_password" ).hidden = true;

   g_recover_hash = "";

   signin_set_text( "recover_words_error", "" );
   signin_set_text( "recover_error", "" );
   signin_set_text( "recover_status", "" );

   document.getElementById( "signin_view" ).hidden = true;
   document.getElementById( "recover_view" ).hidden = false;

   recover_inputs( )[ 0 ].focus( );

   // NOTE: Said on the words' step, which is the one showing (found by review).
   if( !await recover_load_bip39( ) )
   {
      signin_set_text( "recover_words_error", "The word list could not be read - reload the page to try again." );

      return;
   }

   recover_words_status( );
}

// NOTE: Words pasted into a box - all twelve, say - go on across the boxes from it.
function recover_on_input( event )
{
   var input = event.target;

   // NOTE: A refusal was about the words as they were - it goes once they change.
   signin_set_text( "recover_words_error", "" );
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

// NOTE: On from a word's box - to the next, or after the last on to the new master password, as Next.
function recover_next_field( input )
{
   var next = recover_inputs( )[ parseInt( input.dataset.index, 10 ) + 1 ];

   if( next )
      next.focus( );
   else
      recover_words_go( );

   recover_words_status( );
}

// NOTE: The two steps - "words" or "password" - with the focus where typing goes on.
function recover_show_step( step )
{
   document.getElementById( "recover_step_words" ).hidden = ( step !== "words" );
   document.getElementById( "recover_step_password" ).hidden = ( step !== "password" );

   if( step === "password" )
      document.getElementById( "recover_password" ).focus( );
   else
      recover_inputs( )[ 0 ].focus( );
}

function recover_typed_words( )
{
   return recover_inputs( ).map( function( input ) { return input.value.trim( ).toLowerCase( ); } ).filter( function( word ) { return word !== ""; } );
}

// NOTE: What is wrong with the words as they stand - "" when all twelve are words that fit together.
function recover_words_problem_now( )
{
   var words = recover_typed_words( );

   var problem = recovery_words_problem( words, recover_wordlist( ) );

   if( ( problem === "" ) && !BIP39.validateMnemonic( words.join( " " ) ) )
      problem = c_recovery_words_mismatched;

   return problem;
}

// NOTE: On from the words to the new master password, once they are twelve that fit together - what is wrong said on the
// words' own step. Nothing is sent yet: the node sees the words with the password.
async function recover_words_go( )
{
   // NOTE: A refusal from the password's step goes too - back to the words and on again is a fresh try (found by review).
   signin_set_text( "recover_words_error", "" );
   signin_set_text( "recover_error", "" );

   if( !await recover_load_bip39( ) )
   {
      signin_set_text( "recover_words_error", "The word list could not be read - reload the page to try again." );

      return;
   }

   var problem = recover_words_problem_now( );

   if( problem !== "" )
   {
      signin_set_text( "recover_words_error", problem );

      return;
   }

   recover_hide_suggestions( );
   recover_show_step( "password" );
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

   var words = recover_typed_words( );

   var problem = recover_words_problem_now( );

   if( problem !== "" )
   {
      recover_show_step( "words" );

      signin_set_text( "recover_words_error", problem );

      return;
   }

   var password = document.getElementById( "recover_password" ).value;

   // NOTE: The rules by guesses are waited for, so a password is never let through by the old ones for being sent before
   // "zxcvbn.min.js" was in (review, 2026-10-25). If it cannot be read, the old ones are what there is.
   await load_strength_rules( );

   var strength = password_strength( password );

   if( strength.level < 1 )
   {
      signin_set_text( "recover_error", ( strength.text === "Unsatisfactory" )
       ? "The master password needs at least " + c_password_min_length + " characters." : password_refusal( strength ) );

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
   // session is made as for any sign in. A fresh device is registered, as from the terminal - kept as the browser's own
   // once it succeeds, the browser started afresh (Ian, 2026-10-14; below).
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

      // NOTE: Words the node would not take are said with the words, to be put right there; anything else here.
      if( recovery_error_step( error, g_recover_fresh !== null ) === "words" )
      {
         recover_show_step( "words" );

         signin_set_text( "recover_words_error", recovery_error_text( error, false ) );
      }
      else
         signin_set_text( "recover_error", recovery_error_text( error, g_recover_fresh !== null ) );

      return;
   }

   // NOTE: A recovery starts this browser afresh (Ian, 2026-10-14) - most likely a new device, its storage empty, and
   // anything kept here from before suspect: its device token may be a node's first, random one, which no other node
   // knows (later ones carry a checksum from the identity). So everything kept for the node goes, as "Reset this browser"
   // does - the session itself is the tab's - and this session's device becomes the browser's, its hash for Remember.
   try
   {
      localStorage.clear( );
   }
   catch( e )
   {
   }

   signin_forget_tab_keys( "" );

   signin_remember_device( );

   g_recover_hash = ciyam.hashed;

   document.getElementById( "recover_password" ).value = "";
   document.getElementById( "recover_confirm" ).value = "";

   recover_clear_words( );

   signin_keep_session( );

   signin_set_text( "recover_pin", ciyam.access );

   document.getElementById( "recover_retain" ).value = c_retain_access;

   document.getElementById( "recover_form" ).hidden = true;
   document.getElementById( "recover_done" ).hidden = false;

   // NOTE: The way on has the focus - not the heading, which looked strange outlined (Damon, 2026-10-10).
   document.getElementById( "recover_continue" ).focus( );
}

// NOTE: The words are the node's secret - not left in the page once they have been used, or the sign in is shown again
// (found by review, 2026-10-10).
function recover_clear_words( )
{
   recover_inputs( ).forEach( function( input ) { input.value = ""; input.classList.remove( "is-unknown" ); } );

   recover_hide_suggestions( );
}

// NOTE: Left by Back, the new master password typed so far goes too (found by review, 2026-10-13).
function recover_clear_passwords( )
{
   [ "recover_password", "recover_confirm" ].forEach( function( id ) { document.getElementById( id ).value = ""; } );

   document.getElementById( "recover_strength" ).hidden = true;
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

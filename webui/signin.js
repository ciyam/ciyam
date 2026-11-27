// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The node's sign in, shared - one copy for every app, so the sign in is the same everywhere
// (2026-10-08) - Home, the chat, the accounts page and the console, each of which had its own copy.
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
//   options.ids            optional - { access, submit }: the ids a page's suites know its list and button by
//   options.on_busy        optional - called with true as the sign in starts and false as it ends
//
// The heading, its line and the button are the same everywhere (2026-10-10: "the login screen should be the same
// for all" - the parts are one app). Only Home rewords it, for a locked node ("signin_describe( )").
//
// On a locked node it offers recovering with the twelve words, below its buttons - in place, whichever part this is,
// then on to "options.on_signed_in" as after a sign in (2026-10-10; Home's alone before). Needs the
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
// the words. Forgotten with the account, and the tab's copy on signing out ("signin_forget_tab_keys( )").
const c_signin_storage_keys_prefix = "cws.user_keys_";

// NOTE: One brought by another browser's QR code, held for the tab until the keys are shown ("account_keys.js").
const c_signin_pending_keys_name = "cws.pending_keys";

// NOTE: Who admin is giving keys to - from People to Your keys ("do_give_keys( )" in "home.js", 2026-11-11), held for the tab until
// Your keys is shown.
const c_signin_give_keys_name = "cws.give_keys";

// NOTE: The accounts moved to a new phone and not cleared from here yet ("signin_moved( )").
const c_signin_moved_name = "cws.moved";

const c_signin_saved_mark = "  ·  saved password";

const c_signin_title = "Sign in";

const c_signin_lede = "One sign in for every part of your node.";

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

   // NOTE: Accounts moved to a new phone but not cleared from here ("Not now" - "account_move.js", 2026-11-23), offered again.
   var moved = signin_element( "div", { class: "signin-moved", id: "signin_moved", role: "status", hidden: "" } );
   var clear_moved = signin_element( "button", { type: "button", class: "chat-btn--link", id: "signin_moved_clear" }, "Clear them from here" );

   clear_moved.addEventListener( "click", signin_clear_moved );

   moved.appendChild( signin_element( "span", { id: "signin_moved_text" } ) );
   moved.appendChild( document.createTextNode( " " ) );
   moved.appendChild( clear_moved );

   form.appendChild( moved );

   form.appendChild( signin_element( "label", { class: "chat-label", for: g_signin_ids.access, id: "signin_access_label" }, "Account" ) );

   var access = signin_element( "select", { id: g_signin_ids.access, class: "chat-field chat-field--mono" } );

   access.appendChild( new Option( "Enter a PIN…", "" ) );
   access.addEventListener( "change", signin_choose_access );

   form.appendChild( access );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_pin", id: "signin_pin_label" }, "PIN" ) );

   // NOTE: Typed out of sight, like the password (2026-10-10).
   var pin = signin_element( "input", { type: "password", id: "signin_pin", class: "chat-field chat-field--mono",
    inputmode: "numeric", maxlength: "5", autocomplete: "off" } );

   pin.addEventListener( "input", signin_choose_access );

   form.appendChild( pin );

   form.appendChild( signin_element( "label", { class: "chat-label", for: "signin_password", id: "signin_password_label" }, "Password" ) );
   form.appendChild( signin_element( "input", { type: "password", id: "signin_password", class: "chat-field",
    autocomplete: "current-password" } ) );

   // NOTE: Whenever the password is asked for (2026-11-27) - what can be done only once it is asked for, as the sign in was too wordy
   // with it said throughout. A forgotten password is reset by admin, the account then set up again with its PIN (2026-10-29) - or,
   // since 2026-11-22, by the person with the twelve words their keys were made from ("recover_show( { own } )"); not on a locked
   // node, which can't ("signin_check_locked( )").
   var forgot_line = signin_element( "div", { class: "signin-hint", id: "signin_forgot_line" } );
   var forgot_offer = signin_element( "button", { type: "button", class: "chat-btn--link signin-link", id: "signin_forgot_offer",
    "aria-expanded": "false", "aria-controls": "signin_forgot" }, "Forgot your password?" );

   forgot_offer.addEventListener( "click", function( ) { signin_show_forgot( document.getElementById( "signin_forgot" ).hidden ); } );

   forgot_line.appendChild( forgot_offer );

   form.appendChild( forgot_line );

   var forgot = signin_element( "div", { class: "signin-hint", id: "signin_forgot", hidden: "" } );
   var own_reset = signin_element( "button", { type: "button", class: "chat-btn--link signin-link", id: "signin_forgot_words" },
    "Use your twelve words" );

   own_reset.addEventListener( "click", function( ) { recover_show( { own: true, pin: signin_pin( ) } ); } );

   forgot.appendChild( own_reset );
   forgot.appendChild( signin_element( "span", { id: "signin_forgot_admin" }, ", or ask your admin to reset it." ) );

   form.appendChild( forgot );

   // NOTE: A saved password is used without its field (2026-10-29: an empty field looks as if a password must be typed) -
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

   // NOTE: A refusal stands out - a panel of its own (2026-10-10: it was small grey-red type).
   form.appendChild( signin_element( "div", { class: "chat-error-text signin-error", id: "signin_error", role: "alert" } ) );

   var actions = signin_element( "div", { class: "chat-signin-actions" } );

   actions.appendChild( signin_element( "button", { type: "submit", class: "chat-btn chat-btn--primary", id: g_signin_ids.submit }, "Sign in" ) );

   form.appendChild( actions );

   // NOTE: Recovering with the twelve words - only on a locked node ("signin_check_locked( )"), and below the buttons:
   // an edge case, rarely needed (2026-10-10).
   var offer = signin_element( "button", { type: "button", class: "chat-btn chat-signin-setup recover-offer", id: "recover_offer", hidden: "" },
    "Can't sign in? Recover the node with its twelve words" );

   offer.addEventListener( "click", recover_show );

   form.appendChild( offer );

   // NOTE: Shown while the sign in is under way - it can take a few seconds (the chat's, 2026-09).
   var busy = signin_element( "div", { class: "chat-signin-busy", id: "signin_busy", hidden: "" } );

   busy.appendChild( signin_element( "span", { class: "chat-spinner" } ) );
   busy.appendChild( signin_element( "span", { id: "signin_busy_text", role: "status" }, "Contacting the server…" ) );

   form.appendChild( busy );

   // NOTE: How far securing the password has got - it is hardened before it is sent, a second or two (2026-10-25).
   form.appendChild( signin_element( "progress", { class: "chat-signin-progress", id: "signin_busy_bar", max: "1", value: "0", hidden: "" } ) );

   // NOTE: All but signing in, out of the way under "More options" (2026-11-27: the sign in was cluttered) - open by itself on a
   // browser with no account saved, where "New here?" or a move is what is wanted ("signin_fill_saved( )").
   var more_line = signin_element( "div", { class: "chat-signin-centred signin-more-line" } );
   var more = signin_element( "button", { type: "button", class: "chat-btn--link signin-link signin-more", id: "signin_more",
    "aria-expanded": "false", "aria-controls": "signin_options" }, "More options" );

   more.addEventListener( "click", function( ) { signin_show_options( document.getElementById( "signin_options" ).hidden ); } );

   more_line.appendChild( more );

   form.appendChild( more_line );

   var options = signin_element( "div", { class: "signin-options", id: "signin_options", hidden: "" } );

   if( g_signin_options.setup_href )
   {
      options.appendChild( signin_element( "a", { class: "chat-btn chat-signin-setup", id: "signin_setup",
       href: g_signin_options.setup_href }, "New here? Set up your account" ) );

      options.appendChild( signin_element( "div", { class: "chat-hint chat-signin-centred", id: "signin_setup_hint" },
       "With the code or PIN from the person who added you." ) );
   }

   // NOTE: The other end of a move ("account_move.js") - the old device's code scanned here (2026-11-27), or its link pasted
   // ("move_copy_link( )", 2026-11-26), opened at this page's own address, which may not be the one it was made at. Scanned here, it
   // stays in this browser - the camera's own app opens a link in the phone's default browser.
   var move = signin_element( "div", { class: "signin-option chat-signin-centred", id: "signin_move" } );
   var move_scan = signin_element( "button", { type: "button", class: "chat-btn--link signin-link", id: "signin_move_scan" }, "Scan its code" );
   var move_offer = signin_element( "button", { type: "button", class: "chat-btn--link signin-link", id: "signin_move_offer" }, "Paste its link" );

   move_scan.addEventListener( "click", function( ) { signin_move_show( true ); } );
   move_offer.addEventListener( "click", function( ) { signin_move_show( false ); } );

   move.appendChild( document.createTextNode( "Moving from another device? " ) );
   move.appendChild( move_scan );
   move.appendChild( signin_element( "span", { "aria-hidden": "true" }, " · " ) );
   move.appendChild( move_offer );

   options.appendChild( move );

   var paste = signin_element( "div", { class: "signin-move", id: "signin_move_paste", hidden: "" } );

   // NOTE: The camera's picture ("qr_scan.js"), or a picture of the code where there is no camera or no https.
   var scan = signin_element( "div", { class: "signin-move-scan", id: "signin_move_scanning", hidden: "" } );
   var picture = signin_element( "input", { type: "file", accept: "image/*", id: "signin_move_picture", hidden: "" } );
   var choose = signin_element( "button", { type: "button", class: "chat-btn--link signin-link", id: "signin_move_choose" },
    "Use a picture of it" );

   picture.addEventListener( "change", function( ) { signin_move_picture( picture ); } );
   choose.addEventListener( "click", function( ) { picture.click( ); } );

   scan.appendChild( signin_element( "div", { class: "chat-scan-reader", id: "signin_move_reader" } ) );
   scan.appendChild( signin_element( "div", { class: "signin-hint", id: "signin_move_status", role: "status" } ) );
   scan.appendChild( choose );
   scan.appendChild( picture );

   paste.appendChild( scan );

   // NOTE: The link's field - pasted, or a scan's that would not open.
   var pasting = signin_element( "div", { id: "signin_move_pasting" } );

   var paste_go = signin_element( "button", { type: "button", class: "chat-btn chat-signin-setup", id: "signin_move_go" }, "Move to this device" );

   paste_go.addEventListener( "click", signin_move_go );

   pasting.appendChild( signin_element( "label", { class: "chat-label", for: "signin_move_link", id: "signin_move_link_label" },
    "The link from your old device" ) );

   var paste_field = signin_element( "input", { type: "text", id: "signin_move_link", class: "chat-field chat-field--mono",
    autocomplete: "off", spellcheck: "false" } );

   paste_field.addEventListener( "keydown", function( event )
   {
      if( event.key === "Enter" )
      {
         event.preventDefault( );
         signin_move_go( );
      }
   } );

   pasting.appendChild( paste_field );
   pasting.appendChild( signin_element( "div", { class: "chat-error-text signin-error", id: "signin_move_error", role: "alert" } ) );

   var paste_actions = signin_element( "div", { class: "chat-signin-actions" } );

   paste_actions.appendChild( paste_go );
   pasting.appendChild( paste_actions );
   paste.appendChild( pasting );

   options.appendChild( paste );

   // NOTE: What it forgets is said by the question it asks first. Out of the way here - it forgets every account saved.
   var reset_line = signin_element( "div", { class: "signin-option chat-signin-centred" } );
   var reset = signin_element( "button", { type: "button", class: "chat-btn--link", id: "signin_reset" }, "Reset this browser" );

   reset.addEventListener( "click", signin_reset_browser );

   reset_line.appendChild( reset );
   options.appendChild( reset_line );

   form.appendChild( options );
   view.appendChild( form );

   // NOTE: A node not set up yet - nobody can sign in; it is set up from Home (2026-10-13: a Back from setting up
   // landed them on a part's sign in).
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

   signin_show_forgot( false );
   signin_fill_saved( );

   document.getElementById( "recover_view" ).hidden = true;
   document.getElementById( "signin_view" ).hidden = false;

   recover_clear_words( );

   signin_check_locked( );
}

var g_signin_plain = false;

// NOTE: Whether the node is locked - "/system" names a locked one ":CIYAM:". A locked node offers recovery and not
// "New here?", which cannot work on it. Answers whether it is; "(NONE)" there is a connection not encrypted.
async function signin_check_locked( )
{
   var locked = false;
   var fresh = false;

   try
   {
      var system = ( await ( await fetch( "/system", { cache: "no-store" } ) ).text( ) ).trim( );

      locked = ( system.indexOf( ":CIYAM:" ) === 0 );
      fresh = ( system.indexOf( "*CIYAM*" ) === 0 );

      g_signin_plain = / \(NONE\)$/.test( system );
   }
   catch( e )
   {
   }

   // NOTE: Not set up - the sign in gives way to the way to Home ("signin_new_node").
   document.getElementById( "signin_form" ).hidden = fresh;
   document.getElementById( "signin_new_node" ).hidden = !fresh;

   document.getElementById( "recover_offer" ).hidden = !locked;

   // NOTE: A locked node takes no password, so a reset with words is not offered there.
   document.getElementById( "signin_forgot_words" ).hidden = locked;
   signin_set_text( "signin_forgot_admin", locked ? "Ask your admin to reset it." : ", or ask your admin to reset it." );

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
   signin_move_close( );

   document.getElementById( "signin_view" ).hidden = true;
}

function signin_set_text( id, text )
{
   document.getElementById( id ).textContent = text;
}

// NOTE: The refusal, in words - the node's own, locked or not (ISS-056). The node is asked whether it is locked, so that a
// locked one offers its recovery beneath.
async function signin_show_refusal( error )
{
   var locked = await signin_check_locked( );

   signin_set_text( "signin_error", ( is_unknown_device_error( error ) && !locked )
    ? "This browser's saved sign in is from before the node was set up again - type your password."
    : sign_in_error_text( error ) );
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

// NOTE: The accounts moved to a new phone and not yet cleared from here - "{ pins, names }" - or null ("account_move.js").
function signin_moved( )
{
   try
   {
      var moved = JSON.parse( localStorage.getItem( c_signin_moved_name ) || "null" );

      if( moved && Array.isArray( moved.pins ) && Array.isArray( moved.names ) )
         return { pins: moved.pins.map( String ).filter( is_account_pin ), names: moved.names.map( String ) };
   }
   catch( e )
   {
   }

   return null;
}

function signin_show_moved( )
{
   var moved = signin_moved( );
   var notice = document.getElementById( "signin_moved" );

   if( notice === null )
      return;

   notice.hidden = ( moved === null );

   if( moved !== null )
      signin_set_text( "signin_moved_text", move_names_text( moved.names ) + ( ( moved.names.length === 1 ) ? " is" : " are" )
       + " on your new device now." );
}

// NOTE: "More options" opened or closed - a move under way closed with it.
function signin_show_options( open )
{
   document.getElementById( "signin_options" ).hidden = !open;
   document.getElementById( "signin_more" ).setAttribute( "aria-expanded", open ? "true" : "false" );

   if( !open )
      signin_move_close( );
}

function signin_show_forgot( open )
{
   document.getElementById( "signin_forgot" ).hidden = !open;
   document.getElementById( "signin_forgot_offer" ).setAttribute( "aria-expanded", open ? "true" : "false" );
}

// NOTE: The move's panel - "scan" with the camera, or the link's field. The same one asked for again closes it.
var g_signin_move_shown = "";

function signin_move_show( scan )
{
   var wanted = scan ? "scan" : "paste";
   var paste = document.getElementById( "signin_move_paste" );

   if( !paste.hidden && ( g_signin_move_shown === wanted ) )
   {
      signin_move_close( );

      return;
   }

   g_signin_move_shown = wanted;

   paste.hidden = false;

   document.getElementById( "signin_move_pasting" ).hidden = scan;

   signin_set_text( "signin_move_error", "" );

   if( scan )
      signin_move_scan( );
   else
   {
      signin_move_stop_scan( );

      document.getElementById( "signin_move_link" ).focus( );
   }
}

function signin_move_close( )
{
   g_signin_move_shown = "";

   signin_move_stop_scan( );

   document.getElementById( "signin_move_paste" ).hidden = true;
}

// NOTE: Scanning the old device's code - "qr_scan.js", read in only now, as it reads "html5-qrcode.min.js" (375 KB) only when asked.
// "g_signin_move_opened" tells a scan still starting that it has been closed since.
var g_signin_move_scanner = null;
var g_signin_move_opened = 0;

function signin_move_status( text )
{
   signin_set_text( "signin_move_status", text );
}

function signin_move_stop_scan( )
{
   var scanner = g_signin_move_scanner;

   g_signin_move_opened++;
   g_signin_move_scanner = null;

   document.getElementById( "signin_move_scanning" ).hidden = true;

   var done = ( typeof qr_scan_stop === "function" ) ? qr_scan_stop( scanner ) : Promise.resolve( );

   return done.then( function( ) { document.getElementById( "signin_move_reader" ).replaceChildren( ); } );
}

async function signin_move_scan( )
{
   await signin_move_stop_scan( );

   var opened = g_signin_move_opened;

   document.getElementById( "signin_move_scanning" ).hidden = false;

   signin_move_status( "Starting the camera…" );

   if( !await signin_load_script( "qr_scan.js" ) || !await qr_scan_library( ) )
   {
      if( opened === g_signin_move_opened )
         signin_move_status( "The code reader could not be loaded - paste its link instead." );

      return;
   }

   if( opened !== g_signin_move_opened )
      return;

   if( !qr_scan_can_use_camera( ) )
   {
      signin_move_status( "The camera needs this page opened as https - use a picture of the code, or paste its link." );

      return;
   }

   var scanner = qr_scan_reader( "signin_move_reader" );

   g_signin_move_scanner = scanner;

   try
   {
      await scanner.start( { facingMode: "environment" }, { fps: 10 }, signin_move_read, function( ) { } );
   }
   catch( e )
   {
      if( g_signin_move_scanner === scanner )
         g_signin_move_scanner = null;

      if( opened === g_signin_move_opened )
         signin_move_status( "No camera could be used - use a picture of the code, or paste its link." );

      return;
   }

   // NOTE: Closed while the camera was starting - let go of it now.
   if( ( opened !== g_signin_move_opened ) || ( g_signin_move_scanner !== scanner ) )
   {
      qr_scan_stop( scanner );

      return;
   }

   signin_move_status( "Point the camera at the code on your old device." );
}

async function signin_move_picture( input )
{
   var file = ( input.files && input.files.length ) ? input.files[ 0 ] : null;

   input.value = "";

   if( file === null )
      return;

   await signin_move_stop_scan( );

   var opened = g_signin_move_opened;

   document.getElementById( "signin_move_scanning" ).hidden = false;

   if( !await signin_load_script( "qr_scan.js" ) || !await qr_scan_library( ) )
   {
      signin_move_status( "The code reader could not be loaded - paste its link instead." );

      return;
   }

   signin_move_status( "Reading the picture…" );

   var text = await qr_scan_picture( "signin_move_reader", file, function( ) { return opened === g_signin_move_opened; } );

   if( opened !== g_signin_move_opened )
      return;

   if( text === null )
      signin_move_status( "No code could be read in that picture - try one closer up, or straighter." );
   else
      signin_move_read( text );
}

// NOTE: What the camera read - it reads a code many times a second, so only the first is taken. Anything but a move's code is said
// so and the camera left on; a move's is opened as a pasted link is.
function signin_move_read( text )
{
   if( document.getElementById( "signin_move_scanning" ).hidden )
      return;

   if( parse_move_link( text ) === "" )
   {
      signin_move_status( "That isn't a move code - show \"Move this device\" on your old device." );

      return;
   }

   document.getElementById( "signin_move_link" ).value = text;

   signin_move_stop_scan( ).then( signin_move_go );
}

// NOTE: Only the secret is taken from the link ("parse_move_link( )") - the move then opens here, on My account, at this page's own
// address, just as the code's camera would have opened it.
function signin_move_go( )
{
   var secret = parse_move_link( document.getElementById( "signin_move_link" ).value );

   if( secret === "" )
   {
      signin_set_text( "signin_move_error", "That isn't a move link - copy it again from your old device." );

      return;
   }

   var page = window.location.pathname.replace( /[^/]*$/, "account.html" );
   var here = ( page === window.location.pathname );

   window.location.assign( move_link( window.location.origin + page, secret ) );

   // NOTE: Only the part after "#" changes on My account itself, which a browser doesn't load again by itself.
   if( here )
      window.location.reload( );
}

// NOTE: The accounts moved forgotten here - all the browser keeps for the node, its device token too (the new phone's now), once no
// account is left.
function signin_clear_moved( )
{
   var moved = signin_moved( );

   try
   {
      if( moved !== null )
      {
         var keys = [ ];

         for( var i = 0; i < localStorage.length; i++ )
            keys.push( localStorage.key( i ) );

         var left = parse_access_list( localStorage.getItem( c_signin_storage_access ) ).filter( function( pin )
         {
            return moved.pins.indexOf( pin ) < 0;
         } );

         move_forget_keys( keys, moved.pins, moved.names, left.length === 0 ).forEach( function( key ) { localStorage.removeItem( key ); } );

         if( left.length > 0 )
            localStorage.setItem( c_signin_storage_access, format_access_list( left ) );
      }

      localStorage.removeItem( c_signin_moved_name );
   }
   catch( e )
   {
   }

   signin_forget_session( );
   signin_fill_saved( );
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
// PIN (2026-10-10).
function signin_fill_saved( )
{
   signin_forget_stale_names( );
   signin_show_moved( );

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

   // NOTE: The list only once there is an account saved here - with none, its one choice is "Enter a PIN…" (2026-10-28:
   // the sign in scrolled).
   document.getElementById( "signin_access_label" ).hidden = ( ordered.length === 0 );
   select.hidden = ( ordered.length === 0 );

   signin_show_options( ordered.length === 0 );

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
   document.getElementById( "signin_forgot_line" ).hidden = use_saved;

   if( use_saved )
      signin_show_forgot( false );

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
      {
         // NOTE: Kept for the tab, not lost (found by review, 2026-11-12: a stale entry for the PIN had them kept with the account,
         // and choosing not to remember it took them away while the page said they were here).
         if( localStorage.getItem( c_signin_storage_keys_prefix + ciyam.access ) !== null )
            sessionStorage.setItem( c_signin_storage_keys_prefix + ciyam.access, localStorage.getItem( c_signin_storage_keys_prefix + ciyam.access ) );

         localStorage.removeItem( c_signin_storage_keys_prefix + ciyam.access );
      }
      else if( sessionStorage.getItem( c_signin_storage_keys_prefix + ciyam.access ) !== null )
      {
         // NOTE: Keys opened for the tab before the account was remembered - made as setting up or recovering signed in
         // ("signin_admin_keys( )") - kept with it now, as My account keeps them ("keys_keep_here( )").
         localStorage.setItem( c_signin_storage_keys_prefix + ciyam.access, sessionStorage.getItem( c_signin_storage_keys_prefix + ciyam.access ) );
         sessionStorage.removeItem( c_signin_storage_keys_prefix + ciyam.access );
      }

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

   await signin_forget_kept_files( );

   location.reload( );
}

// NOTE: The apps' files kept in this browser let go, and what keeps them ("app_worker.js") - so the next load is from the node itself.
// The worker is taken on again as that page opens ("apps_keep_files( )"), starting empty; another of the apps still open in a tab is
// left with the copy it has until it is opened again.
async function signin_forget_kept_files( )
{
   try
   {
      if( navigator.serviceWorker )
      {
         var held = await navigator.serviceWorker.getRegistrations( );

         await Promise.all( held.map( function( registration ) { return registration.unregister( ); } ) );
      }

      if( window.caches )
      {
         var names = await caches.keys( );

         await Promise.all( names.filter( function( name ) { return name.indexOf( "ciyam-app-" ) === 0; } )
          .map( function( name ) { return caches.delete( name ); } ) );
      }
   }
   catch( e )
   {
   }
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
   g_signin_resumed = null;

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
// a reload and clearing "localStorage" (2026-10-27). All but "kept_pin"'s go: none on signing out, the account's own
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
      {
         names.push( c_signin_pending_keys_name );
         names.push( c_signin_give_keys_name );
      }

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

// NOTE: The page going away - its requests are cut off, which is no answer from the node ("signin_resume( )").
var g_signin_leaving = false;

window.addEventListener( "pagehide", function( ) { g_signin_leaving = true; } );
window.addEventListener( "pageshow", function( ) { g_signin_leaving = false; } );

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

   // NOTE: Left while the node was being asked - a reload, or another app opened, before a slow connection answered - and the request
   // was cut off: the session is kept for the next page, not forgotten as refused (found 2026-11-16 - a reload during a reload's
   // resume signed the tab out).
   if( g_signin_leaving )
      return false;

   // NOTE: The chat not turned on refuses the lobby, though the session is good (2026-10-13) - so it is kept, and
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

   g_signin_resumed = is_error_response( reply ) ? null : { at: Date.now( ), sessid: ciyam.sessid, reply: reply };

   return true;
}

// NOTE: The listing a resume was answered with - for the app's own first look at the lobby, which had asked again at once: a round
// trip at every switch, on a phone (2026-11-15). Taken once, and only a moment after, for this session.
const c_signin_resumed_ms = 5000;

var g_signin_resumed = null;

// NOTE: The resume's listing, or null - the app asks the node itself.
function signin_resumed_listing( )
{
   var resumed = g_signin_resumed;

   g_signin_resumed = null;

   if( ( resumed === null ) || ( resumed.sessid !== ciyam.sessid ) || ( Date.now( ) - resumed.at > c_signin_resumed_ms ) )
      return null;

   return resumed.reply;
}

// ====================================================================
// Recovering the node with its twelve words (2026-10-09; in every part, 2026-10-10)
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

   // NOTE: Two steps, as setting up has (2026-10-13): the twelve words, then the new master password - all on one
   // card it was long, and scrolled on a phone. "recover_show_step( )" shows one.
   var step_words = signin_element( "div", { class: "recover-step", id: "recover_step_words" } );
   var step_password = signin_element( "div", { class: "recover-step", id: "recover_step_password", hidden: "" } );

   form.appendChild( step_words );
   form.appendChild( step_password );

   step_words.appendChild( signin_element( "h2", { tabindex: "-1", id: "recover_heading" }, words.heading ) );
   step_words.appendChild( signin_element( "p", { class: "chat-signin-lede", id: "recover_lede" }, words.lede ) );

   // NOTE: A person's own reset asks whose account too (2026-11-22) - the PIN, from the sign in if typed there.
   var own_pin = signin_element( "div", { id: "recover_own_pin_row", hidden: "" } );

   own_pin.appendChild( signin_element( "label", { class: "chat-label", for: "recover_own_pin" }, "Your PIN" ) );
   own_pin.appendChild( signin_element( "input", { type: "password", id: "recover_own_pin", class: "chat-field chat-field--mono",
    inputmode: "numeric", maxlength: "5", autocomplete: "off" } ) );

   step_words.appendChild( own_pin );

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

   step_password.appendChild( signin_element( "h2", { tabindex: "-1", id: "recover_password_heading" }, words.password_heading ) );
   step_password.appendChild( signin_element( "p", { class: "chat-signin-lede", id: "recover_password_lede" }, words.password_lede ) );
   step_password.appendChild( signin_element( "label", { class: "chat-label", for: "recover_password", id: "recover_password_label" },
    words.password_label ) );

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

   actions.appendChild( signin_element( "button", { type: "submit", class: "chat-btn chat-btn--primary", id: "recover_submit" }, words.submit ) );

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

         g_recover_own = false;

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
   done.appendChild( signin_element( "p", { class: "recover-note", id: "recover_keys_note" } ) );

   // NOTE: Admin's PIN, and the password, remembered on this browser as setting up's end offers (2026-10-13) - the browser
   // started afresh by the recovery, with this session's device as its own (2026-10-14).
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

// NOTE: A person's own password reset with their words (2026-11-22) rather than the node's recovery.
var g_recover_own = false;

// NOTE: The new password's hash, for Remember - "" until a recovery succeeds.
var g_recover_hash = "";

// NOTE: The BIP39 library - large, so read in only when recovery opens, and once.
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

// NOTE: A reveal button in a new password's field - the eye - showing the password as typed, or hiding it again
// (2026-10-25: "if we get a suggested password, we have no way of seeing what was suggested"). Each field once; the field is
// wrapped so the button sits inside it. "attach_reveals( )" finds them all - "autocomplete" of "new-password".
function attach_reveal( input )
{
   if( !input || ( input.dataset.reveal === "1" ) )
      return;

   input.dataset.reveal = "1";

   var wrap = signin_element( "span", { class: "chat-reveal-wrap" } );

   input.parentNode.insertBefore( wrap, input );
   wrap.appendChild( input );

   // NOTE: Not a tab stop (2026-10-27) - Password to its "type it again" is one tab; from the keys, the checkbox after both
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

// NOTE: A new password and its "type it again" are shown or hidden together (2026-10-25) - the new password fields
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

   // NOTE: Each new password and its "type it again" one tab apart (2026-10-27, as the legacy page): "Suggest one" on the
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

// NOTE: "Suggest one" (2026-10-23: a suggested passphrase of five words) - five BIP39 words fill the new password and
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

   // NOTE: One line - it is in both fields too (2026-10-28: Welcome pushed past the bottom of the screen).
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

// NOTE: "options" - { fresh: true, on_back } to recover onto a node not set up (setting up's choice, 2026-10-13);
// anything else - the sign in's click - is a locked node's recovery.
async function recover_show( options )
{
   var fresh = !!( options && options.fresh );

   g_recover_fresh = fresh ? { on_back: options.on_back } : null;
   g_recover_own = !fresh && !!( options && options.own );

   var words = recovery_texts( fresh, g_recover_own );

   signin_set_text( "recover_heading", words.heading );
   signin_set_text( "recover_lede", words.lede );
   signin_set_text( "recover_caution", words.caution );
   signin_set_text( "recover_back", words.back );
   signin_set_text( "recover_done_heading", words.done_heading );
   signin_set_text( "recover_done_lede", words.done_lede );
   signin_set_text( "recover_done_note", words.done_note );
   signin_set_text( "recover_password_heading", words.password_heading );
   signin_set_text( "recover_password_lede", words.password_lede );
   signin_set_text( "recover_password_label", words.password_label );
   signin_set_text( "recover_submit", words.submit );

   document.getElementById( "recover_caution" ).hidden = !fresh;
   document.getElementById( "recover_own_pin_row" ).hidden = !g_recover_own;
   document.getElementById( "recover_own_pin" ).value = g_recover_own ? String( options.pin || "" ) : "";
   document.getElementById( "recover_own_pin" ).removeAttribute( "aria-invalid" );

   // NOTE: Over plain HTTP the warning shows - known from "/system", asked here when no sign in has asked yet.
   if( fresh )
      await signin_check_locked( );

   recover_inputs( ).forEach( function( input ) { input.value = ""; input.classList.remove( "is-unknown" ); } );

   [ "recover_password", "recover_confirm" ].forEach( function( id ) { document.getElementById( id ).value = ""; } );

   document.getElementById( "recover_strength" ).hidden = true;

   // NOTE: For a reset with words too (found by review, 2026-11-22) - only a hash of them leaves the browser, but over plain HTTP
   // the page itself could be changed on its way to take them as they are typed.
   var warning = document.getElementById( "recover_warning" );

   warning.hidden = !g_signin_plain;
   warning.lastChild.textContent = g_recover_own ? " Anyone between you and the node could change this page to take your words - only"
    + " reset from a device on the node's own network." : " The node sees it as plain text - only recover from a device on the node's"
    + " own network.";
   document.getElementById( "recover_form" ).hidden = false;
   document.getElementById( "recover_done" ).hidden = true;
   document.getElementById( "recover_step_words" ).hidden = false;

   signin_set_text( "recover_keys_note", "" );
   document.getElementById( "recover_step_password" ).hidden = true;

   g_recover_hash = "";

   signin_set_text( "recover_words_error", "" );
   signin_set_text( "recover_error", "" );
   signin_set_text( "recover_status", "" );

   document.getElementById( "signin_view" ).hidden = true;
   document.getElementById( "recover_view" ).hidden = false;

   // NOTE: The PIN first, for a reset with words, when it wasn't typed at the sign in.
   if( g_recover_own && ( document.getElementById( "recover_own_pin" ).value === "" ) )
      document.getElementById( "recover_own_pin" ).focus( );
   else
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

// ---- The words a box offers as it is typed in - from their start, as an autocomplete does (2026-10-10: the
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

   // NOTE: A reset with words says what the PIN lacks first, then what the words do - both at once.
   var problems = [ recover_own_pin_problem( ), recover_words_problem_now( ) ].filter( function( text ) { return text !== ""; } );

   if( problems.length > 0 )
   {
      signin_set_text( "recover_words_error", problems.join( " " ) );

      return;
   }

   recover_hide_suggestions( );
   recover_show_step( "password" );
}

// NOTE: What the PIN lacks for a reset with words - "" when it is five digits, or this isn't one. Marked on the field.
function recover_own_pin_problem( )
{
   var field = document.getElementById( "recover_own_pin" );

   var problem = ( !g_recover_own || /^[0-9]{5}$/.test( field.value.trim( ) ) ) ? "" : "Enter your PIN - the five digits you sign in with.";

   if( problem !== "" )
      field.setAttribute( "aria-invalid", "true" );
   else
      field.removeAttribute( "aria-invalid" );

   return problem;
}

// NOTE: Enter moves on from a box whether a word was chosen from the list or typed in full - the last box to the new
// password - and never sends the form from a word (2026-10-10: Enter on a whole word sent it, and the count of
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

// NOTE: The scripts admin's keys need ("signin_admin_keys( )") - read in only then, as My account and the chat read theirs.
const c_signin_keys_scripts = [ "ed25519.min.js", "ciyam_harden.js", "ciyam_chacha20.js", "ciyam_ed25519.js", "ciyam_secure_message.js",
 "ciyam_user_keys.js" ];

function signin_load_script( src )
{
   return new Promise( function( resolve )
   {
      if( document.querySelector( "script[src='" + src + "']" ) !== null )
      {
         resolve( true );

         return;
      }

      var script = document.createElement( "script" );

      script.src = src;
      script.onload = function( ) { resolve( true ); };
      script.onerror = function( ) { resolve( false ); };

      document.head.appendChild( script );
   } );
}

// NOTE: A request as the page makes them ("g_signin_options.request" - in turn, ISS-005), its answer as text - asked twice more, a moment
// apart, if it timed out: a node that has just taken its identity is busy starting up (as "setup_finish( )" in "home.js" asks its
// sign in again; found by review, 2026-11-12).
async function signin_issue( issue )
{
   var answer = "";

   for( var tries = 0; tries < 3; tries++ )
   {
      if( tries > 0 )
         await new Promise( function( resolve ) { window.setTimeout( resolve, 2000 ); } );

      ciyam.error = "";

      try
      {
         if( g_signin_options.request )
            answer = String( await g_signin_options.request( issue ) );
         else
            answer = await new Promise( function( resolve ) { issue( function( response ) { resolve( String( response ) ); } ); } );
      }
      catch( e )
      {
         answer = "Error: " + ( ( e && e.message ) ? e.message : "the request failed." );
      }

      if( !/Timed out waiting for web session response/.test( answer + ciyam.error ) )
         break;
   }

   return answer;
}

// NOTE: An access key kept on this browser as My account keeps one ("keys_keep_here( )") - for as long as the account is remembered
// here, otherwise for the tab.
function signin_keep_keys( access )
{
   try
   {
      var remembered = ( parse_access_list( localStorage.getItem( c_signin_storage_access ) ).indexOf( ciyam.access ) >= 0 );

      ( remembered ? localStorage : sessionStorage ).setItem( c_signin_storage_keys_prefix + ciyam.access, access );
      ( remembered ? sessionStorage : localStorage ).removeItem( c_signin_storage_keys_prefix + ciyam.access );
   }
   catch( e )
   {
   }
}

// NOTE: Admin's keys from the node's twelve words (2026-11-12: admin is the node) - made as setting up or recovering the node
// signs in, so Give keys in People works at once. From the people key and "admin" ("person_entropy( )" in "ciyam_user_keys.js"), not
// admin's PIN, which a recovery changes. Keys the node has that the words open are kept as they are - contacts and room keys with
// them - and only opened here; keys from other words are left alone. "made", "opened", "other" or "failed" ("admin_keys_text( )").
// NOTE: The people key is kept with them too, as Give keys keeps it - so Give keys asks for nothing, as the words are here now
// (2026-11-16: "this has to be a smooth and simple process"; 2026-11-16: not the password either).
async function signin_admin_keys( entropy )
{
   try
   {
      for( var i = 0; i < c_signin_keys_scripts.length; i++ )
      {
         if( !await signin_load_script( c_signin_keys_scripts[ i ] ) )
            return "failed";
      }

      var uk = ciyam_module_named( "user_keys" );

      if( !uk )
         return "failed";

      var people = await uk.people_key( entropy );
      var own = await uk.person_entropy( people, uk.c_admin_person );

      var url = ciyam.get_cws_url( ) + "/user-keys?access=" + ciyam.access + "&device=" + ciyam.device + "&format=text";

      var kept = ( await signin_issue( function( done )
      {
         return ciyam.fetch( url + "&session=" + ciyam.sessid, "GET", done );
      } ) ).trim( );

      if( is_error_response( kept ) )
         return "failed";

      // NOTE: Keys there already - opened if the words make them, else left as they are.
      if( ( kept !== "[none]" ) && !/^UK[12]:/.test( kept ) )
      {
         var access = await uk.access_key( own, uk.rounds_of( kept ) );
         var opened = null;

         try
         {
            opened = await uk.unlock( access, kept );
         }
         catch( e )
         {
            return "other";
         }

         signin_keep_keys( access );

         // NOTE: The people key kept with them, unless it is there already - not when none is, or one sealed under a password from
         // before. Only then are the keys sent again (as "keys_send( )" sends them, whole), so the gap in which another device's change
         // could be lost is only when needed (found by review, 2026-11-16). Not taken - the lock or the node refused - and they are
         // still opened: Give keys asks for the words once, as before.
         var keeping = ( opened.people !== people ) ? people : "";

         people = "";

         if( keeping !== "" )
         {
            try
            {
               var relocked = uk.kept_text( opened.guard, await uk.lock( access, { primary: opened.primary, secondary: opened.secondary,
                contacts: opened.contacts, rooms: opened.rooms, people: keeping }, { rounds: opened.rounds } ) );

               keeping = "";

               await signin_issue( function( done )
               {
                  return ciyam.fetch_in_body( url + "&payload=" + encodeURIComponent( relocked ) + "&session=" + ciyam.sessid, "PUT", done );
               } );
            }
            catch( e )
            {
            }
         }

         return "opened";
      }

      var made = await uk.kept_keys( own, undefined, people );

      people = "";

      var saved = ( await signin_issue( function( done )
      {
         return ciyam.fetch_in_body( url + "&payload=" + encodeURIComponent( made.kept ) + "&session=" + ciyam.sessid, "PUT", done );
      } ) ).trim( );

      if( is_error_response( saved ) )
         return "failed";

      signin_keep_keys( made.access );

      return "made";
   }
   catch( e )
   {
      return "failed";
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
      signin_set_text( "recover_error", ( strength.text === "Unsatisfactory" ) ? ( g_recover_own ? "The password" : "The master password" )
       + " needs at least " + c_password_min_length + " characters." : password_refusal( strength ) );

      return;
   }

   if( password !== document.getElementById( "recover_confirm" ).value )
   {
      signin_set_text( "recover_error", "The two passwords are not the same." );

      return;
   }

   var button = document.getElementById( "recover_submit" );

   if( g_recover_own )
   {
      await own_reset_submit( words, password, button );

      return;
   }

   button.disabled = true;

   signin_set_text( "recover_status", "Checking the words with the node..." );

   // NOTE: The words' entropy as "access", and the new password - "connect( )" in "ciyam.js" then does as the server's curl
   // does: the node checks the entropy, answers with admin's PIN, and takes the password with the entropy, then the
   // session is made as for any sign in. A fresh device is registered, as from the terminal - kept as the browser's own
   // once it succeeds, the browser started afresh (2026-10-14; below). The entropy's hash rather than the entropy, as
   // at set up ("setup_finish( )" in "home.js") - the node takes either for the same identity.
   var device = ciyam.device;

   recover_clear_ciyam( );

   ciyam.device = "";

   var reply = "";

   var entropy = BIP39.mnemonicToEntropy( words.join( " " ) );

   var issue = function( done )
   {
      return ciyam.connect( hex_sha256( entropy ), "", "", password, done );
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

   // NOTE: A recovery starts this browser afresh (2026-10-14) - most likely a new device, its storage empty, and
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

   // NOTE: Admin's keys from the words, so Give keys works at once - after the browser was started afresh, above.
   signin_set_text( "recover_status", "Making admin's keys from the words..." );
   button.disabled = true;

   signin_set_text( "recover_keys_note", admin_keys_text( await signin_admin_keys( entropy ) ) );

   entropy = "";
   button.disabled = false;
   signin_set_text( "recover_status", "" );

   signin_set_text( "recover_pin", ciyam.access );

   document.getElementById( "recover_retain" ).value = c_retain_access;

   document.getElementById( "recover_form" ).hidden = true;
   document.getElementById( "recover_done" ).hidden = false;

   // NOTE: The way on has the focus - not the heading, which looked strange outlined (2026-10-10).
   document.getElementById( "recover_continue" ).focus( );
}

// NOTE: A person's own password reset with the words their keys were made from (2026-11-22) - the node checks them against the guard
// on their keys ("reset_password_with_words( )" in "ciyam_web_session.cpp"), takes the new password and signs them out everywhere;
// then the sign in goes on with it as if typed, Remember as chosen there. What is refused is said on the step it belongs to.
async function own_reset_submit( words, password, button )
{
   var pin = document.getElementById( "recover_own_pin" ).value.trim( );

   if( recover_own_pin_problem( ) !== "" )
   {
      recover_show_step( "words" );

      signin_set_text( "recover_words_error", recover_own_pin_problem( ) );

      return;
   }

   button.disabled = true;

   signin_set_text( "recover_status", "Checking the words with the node..." );

   var error = await own_reset_send( pin, words, password );

   button.disabled = false;

   signin_set_text( "recover_status", "" );

   if( error !== "" )
   {
      var refusal = own_reset_error( error );

      if( refusal.step === "words" )
      {
         recover_show_step( "words" );

         signin_set_text( "recover_words_error", refusal.text );
      }
      else
         signin_set_text( "recover_error", refusal.text );

      return;
   }

   // NOTE: What this browser kept for the old password goes - its hash, and the version it was hashed with, the new one hashed as
   // now ("hash_version_for( )" in "ciyam.js").
   try
   {
      localStorage.removeItem( c_signin_storage_hashed_prefix + pin );
      localStorage.setItem( c_storage_hash_version_prefix + pin, String( c_password_hash_version ) );
   }
   catch( e )
   {
   }

   recover_clear_words( );
   recover_clear_passwords( );

   g_recover_own = false;

   document.getElementById( "recover_view" ).hidden = true;
   document.getElementById( "signin_view" ).hidden = false;

   document.getElementById( g_signin_ids.access ).value = "";

   var pin_field = document.getElementById( "signin_pin" );

   pin_field.value = pin;
   pin_field.dispatchEvent( new Event( "input", { bubbles: true } ) );

   signin_type_password( false );

   document.getElementById( "signin_password" ).value = password;

   password = "";

   document.getElementById( "signin_form" ).requestSubmit( );
}

// NOTE: The reset sent as a claim's credentials are ("own_reset_credentials( )") - the check a round under the guard's own rounds,
// taken to be the default until the node says otherwise (keys whose access key was changed, or reset before, have fewer), then
// once more under the rounds it said. "" once the node took it, else its refusal.
async function own_reset_send( pin, words, password )
{
   var entropy = "";

   try
   {
      entropy = BIP39.mnemonicToEntropy( words.join( " " ) );

      for( var i = 0; i < c_signin_keys_scripts.length; i++ )
      {
         if( !await signin_load_script( c_signin_keys_scripts[ i ] ) )
            return "Error: The page's keys could not be read - reload the page to try again.";
      }

      var uk = ciyam_module_named( "user_keys" );
      var hash = await ciyam.hash_combined( password, pin );
      var name = signin_saved_name( pin ) || "";
      var rounds = uk.c_default_rounds - 1;
      var error = "";

      for( var tries = 0; tries < 2; tries++ )
      {
         var credentials = own_reset_credentials( name, hash, await uk.guard( entropy, rounds ) );
         var url = ciyam.get_cws_url( ) + "/devices?access=" + pin + "&format=text&passwd=" + CIYAM.encode_base64_url( credentials );

         var reply = ( await signin_issue( function( done ) { return ciyam.fetch( url, "POST", done ); } ) ).trim( );

         error = is_error_response( reply ) ? reply : ( ciyam.error || "" );

         ciyam.error = "";

         if( error === "" )
            return "";

         var asked = own_reset_rounds_asked( error );

         if( asked < 2 )
            break;

         rounds = asked - 1;
      }

      return error;
   }
   catch( e )
   {
      return "Error: " + ( ( e && e.message ) ? e.message : "the request failed." );
   }
   finally
   {
      entropy = "";
   }
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

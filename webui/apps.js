// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The app switcher, shared - the same in every app (Damon, 2026-10-08). Its button is the CIYAM mark and
// the app's name ("CIYAM · Chat"); open, it lists every app, this one marked, and choosing another switches to
// its tab if it is open on this session, else opens one on it - never "Back to", since any app may have opened
// any other. At its foot, who is signed in and "Sign out of every app", which reaches every tab on the session
// whichever app it is chosen in. On a phone it is a sheet from the bottom. Which apps, and the sign out message,
// are "switcher_apps( )" and "signed_out_message( )" in "chat_parse.js", with their tests.
//
// The page builds it with "apps_build( host, options )" and calls "apps_refresh( )" once signed in:
//
//   options.current        the app this is - "home", "chat", "account" or "console"
//   options.self           the page's id on the channel ("g_self"), for "?source=" to hand on its session
//   options.sign_out       the page's own sign out - it ends the session on the node and clears the page
//   options.signed_out     the page's own clearing, when another tab on the session signed out of every app
//   options.from           optional - how the console names this page ("home", "accounts"; the chat by default)
//
// Needs "chat_parse.js" ("switcher_apps( )", "switcher_title( )", "signed_out_message( )", "open_app_tab( )",
// "user_initial( )", "sender_colour_index( )") and the page's "ciyam". Its look is "apps.css".

const c_apps_phone_query = "(max-width: 760px)";

var g_apps_options = { };

var g_apps_session_channel = null;

function apps_element( tag, class_name, text )
{
   var element = document.createElement( tag );

   if( class_name )
      element.className = class_name;

   if( text !== undefined )
      element.textContent = text;

   return element;
}

function apps_svg( path, size, fill )
{
   var svg = document.createElementNS( "http://www.w3.org/2000/svg", "svg" );

   svg.setAttribute( "width", String( size ) );
   svg.setAttribute( "height", String( size ) );
   svg.setAttribute( "viewBox", "0 0 24 24" );
   svg.setAttribute( "aria-hidden", "true" );

   if( fill )
      svg.setAttribute( "fill", "currentColor" );
   else
   {
      svg.setAttribute( "fill", "none" );
      svg.setAttribute( "stroke", "currentColor" );
      svg.setAttribute( "stroke-width", "2" );
      svg.setAttribute( "stroke-linecap", "round" );
      svg.setAttribute( "stroke-linejoin", "round" );
   }

   var shape = document.createElementNS( "http://www.w3.org/2000/svg", "path" );

   shape.setAttribute( "d", path );

   svg.appendChild( shape );

   return svg;
}

const c_apps_icons = {
   home: "M3 10.5 12 3l9 7.5V20a1 1 0 0 1-1 1h-5v-6h-6v6H4a1 1 0 0 1-1-1z",
   chat: "M4 5h16v11H8l-4 4z",
   account: "M16 19v-1a4 4 0 0 0-4-4H7a4 4 0 0 0-4 4v1M10 11a3 3 0 1 0 0-6 3 3 0 0 0 0 6M21 19v-1a4 4 0 0 0-3-3.9M16 5.1a3 3 0 0 1 0 5.8",
   console: "M4 6l5 5-5 5M12 18h8",
   chevron: "M6 9l6 6 6-6",
   tick: "M5 12l5 5L20 7",
   external: "M7 17 17 7M9 7h8v8"
};

function apps_build( host, options )
{
   g_apps_options = options || { };

   var wrap = apps_element( "div", "apps" );

   var trigger = apps_element( "button", "apps-trigger" );

   trigger.type = "button";
   trigger.id = "apps_trigger";
   trigger.setAttribute( "aria-haspopup", "true" );
   trigger.setAttribute( "aria-expanded", "false" );
   trigger.setAttribute( "aria-controls", "apps_panel" );
   trigger.title = "Switch apps";

   trigger.appendChild( apps_element( "span", "chat-logo" ) );
   trigger.appendChild( apps_element( "span", "chat-wordmark apps-wordmark", "CIYAM" ) );
   trigger.appendChild( apps_element( "span", "apps-dot", "·" ) );

   var current = apps_element( "span", "apps-current", switcher_title( g_apps_options.current, false ) );

   current.id = "apps_current";

   trigger.appendChild( current );
   trigger.appendChild( apps_svg( c_apps_icons.chevron, 14 ) );

   trigger.addEventListener( "click", function( event )
   {
      event.stopPropagation( );

      if( document.getElementById( "apps_panel" ).hidden )
         apps_open( );
      else
         apps_close( );
   } );

   var scrim = apps_element( "div", "apps-scrim" );

   scrim.id = "apps_scrim";
   scrim.hidden = true;
   scrim.addEventListener( "click", apps_close );

   var panel = apps_element( "nav", "apps-panel" );

   panel.id = "apps_panel";
   panel.hidden = true;
   panel.setAttribute( "aria-label", "Apps" );

   panel.appendChild( apps_element( "span", "apps-heading", "Apps on this node" ) );

   var list = apps_element( "div", "apps-list" );

   list.id = "apps_list";

   panel.appendChild( list );

   var foot = apps_element( "div", "apps-foot" );

   var avatar = apps_element( "span", "apps-avatar" );

   avatar.id = "apps_avatar";

   var who = apps_element( "span", "apps-who" );
   var name = apps_element( "span", "apps-name" );

   name.id = "apps_name";

   who.appendChild( name );
   who.appendChild( apps_element( "span", "apps-note", "One session, every app" ) );

   var sign_out = apps_element( "button", "chat-btn apps-sign-out", "Sign out of every app" );

   sign_out.type = "button";
   sign_out.id = "apps_sign_out";
   sign_out.addEventListener( "click", apps_sign_out_everywhere );

   foot.appendChild( avatar );
   foot.appendChild( who );
   foot.appendChild( sign_out );

   panel.appendChild( foot );

   wrap.appendChild( trigger );
   wrap.appendChild( scrim );
   wrap.appendChild( panel );

   host.replaceChildren( wrap );

   panel.addEventListener( "click", function( event ) { event.stopPropagation( ); } );

   document.addEventListener( "click", function( ) { apps_close( ); } );

   document.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Escape" ) && !document.getElementById( "apps_panel" ).hidden )
      {
         apps_close( );

         document.getElementById( "apps_trigger" ).focus( );
      }
   } );

   // NOTE: Another tab on this session signed out of every app - this one lets the session go too.
   if( ( g_apps_session_channel === null ) && ( typeof BroadcastChannel !== "undefined" ) )
   {
      g_apps_session_channel = new BroadcastChannel( c_session_channel_name );

      g_apps_session_channel.addEventListener( "message", function( event )
      {
         if( is_signed_out_here( event.data, ciyam.sessid ) && g_apps_options.signed_out )
         {
            apps_close( );

            g_apps_options.signed_out( );
         }
      } );
   }

   apps_refresh( );
}

function apps_is_phone( )
{
   return window.matchMedia( c_apps_phone_query ).matches;
}

// NOTE: Who is signed in decides the accounts page's name ("Accounts" or "My account") and the console's place.
function apps_refresh( )
{
   var current = document.getElementById( "apps_current" );

   if( current === null )
      return;

   current.textContent = switcher_title( g_apps_options.current, ciyam.is_admin );

   var name = ciyam.username || ciyam.access;

   var avatar = document.getElementById( "apps_avatar" );

   avatar.textContent = user_initial( name );
   avatar.style.background = "var(--color-sender-" + sender_colour_index( name ) + ")";

   document.getElementById( "apps_name" ).textContent = name;
}

function apps_open( )
{
   var list = document.getElementById( "apps_list" );

   list.replaceChildren( );

   switcher_apps( ciyam.is_admin, apps_is_phone( ), g_apps_options.current ).forEach( function( app )
   {
      var item = apps_element( "a", "apps-item" + ( app.current ? " is-current" : "" ) );

      item.href = app.page;
      item.dataset.key = app.key;

      var icon = apps_element( "span", "apps-icon apps-icon--" + app.key );

      icon.appendChild( apps_svg( c_apps_icons[ app.key ], 18 ) );

      var words = apps_element( "span", "apps-words" );

      words.appendChild( apps_element( "span", "apps-title", app.title ) );
      words.appendChild( apps_element( "span", "apps-item-note", app.current ? "You are here" : app.note ) );

      item.appendChild( icon );
      item.appendChild( words );

      if( app.current )
      {
         item.setAttribute( "aria-current", "page" );
         item.appendChild( apps_svg( c_apps_icons.tick, 16 ) );
      }
      else
      {
         item.title = "Opens in its own tab, on this session";
         item.appendChild( apps_svg( c_apps_icons.external, 14 ) );
      }

      item.addEventListener( "click", function( event )
      {
         event.preventDefault( );

         apps_close( );

         if( !app.current )
            apps_switch_to( app );
      } );

      list.appendChild( item );
   } );

   apps_refresh( );

   document.getElementById( "apps_panel" ).hidden = false;
   document.getElementById( "apps_scrim" ).hidden = !apps_is_phone( );
   document.getElementById( "apps_trigger" ).setAttribute( "aria-expanded", "true" );
}

function apps_close( )
{
   var panel = document.getElementById( "apps_panel" );

   if( ( panel === null ) || panel.hidden )
      return;

   panel.hidden = true;

   document.getElementById( "apps_scrim" ).hidden = true;
   document.getElementById( "apps_trigger" ).setAttribute( "aria-expanded", "false" );
}

// NOTE: Its tab if open on this session, else one opened on it - "?source=" asks this page for the session.
function apps_switch_to( app )
{
   var self = g_apps_options.self ? String( g_apps_options.self( ) ) : "";

   var address = app.page + "?source=" + encodeURIComponent( self );

   if( ( app.key === "console" ) && g_apps_options.from )
      address += "&from=" + encodeURIComponent( g_apps_options.from );

   open_app_tab( app.tab, address, ciyam.sessid );
}

// NOTE: Every tab on the session is told before the session ends, so none is left with one the node no longer has.
async function apps_sign_out_everywhere( )
{
   apps_close( );

   if( ( g_apps_session_channel !== null ) && ( ciyam.sessid !== "" ) )
      g_apps_session_channel.postMessage( signed_out_message( ciyam.sessid ) );

   if( g_apps_options.sign_out )
      await g_apps_options.sign_out( );
}

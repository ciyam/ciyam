// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Desktop notifications, shared by the chat (which sends them) and My account (Damon, 2026-10-20) - the setting, kept
// per account on this browser ("c_storage_notify_by_name" in "chat_parse.js"), what the browser allows, and turning them
// on, which has to come from a click, as only then will a browser ask. Needs "chat_parse.js" and the page's "ciyam".

function notify_key( )
{
   return account_storage_key( c_storage_notify_by_name, ciyam.username, ciyam.is_admin );
}

// NOTE: "on", "off", or "" - not asked yet.
function notify_setting( )
{
   try
   {
      return localStorage.getItem( notify_key( ) ) || "";
   }
   catch( e )
   {
      return "";
   }
}

function notify_store( value )
{
   try
   {
      localStorage.setItem( notify_key( ), value );
   }
   catch( e )
   {
   }
}

// NOTE: What this browser allows - "ok"; "insecure", as notifications need https (or localhost); "unsupported"; or
// "blocked", refused for this site.
function notify_state( )
{
   if( window.isSecureContext === false )
      return "insecure";

   if( typeof Notification === "undefined" )
      return "unsupported";

   if( Notification.permission === "denied" )
      return "blocked";

   return "ok";
}

function notify_is_on( )
{
   return ( notify_setting( ) === "on" ) && ( notify_state( ) === "ok" ) && ( Notification.permission === "granted" );
}

// NOTE: Turned on or off, from a click - asking the browser's permission the first time. True if on after.
async function notify_switch( on )
{
   if( !on )
   {
      notify_store( "off" );

      return false;
   }

   if( notify_state( ) !== "ok" )
      return false;

   var permission = Notification.permission;

   if( permission !== "granted" )
   {
      try
      {
         permission = await Notification.requestPermission( );
      }
      catch( e )
      {
         permission = "denied";
      }
   }

   notify_store( ( permission === "granted" ) ? "on" : "off" );

   return permission === "granted";
}

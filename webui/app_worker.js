// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The apps' own files from a copy kept in this browser (Damon, 2026-11-17: switching apps over a slow 4G connection took 4.3 s,
// nearly all of it asking the node whether each of 27 unchanged files had changed - it sends "Cache-Control: no-cache", so that a
// page doesn't run old scripts with new markup, ISS-021). An app page and the files it loads come from a set kept here, so opening one
// asks the node nothing for them. Each time an app page opens, the set is checked against the node in the background - at most once a
// minute ("c_check_every_ms"); if anything changed, a new set is made from the node's copies, and used by pages opened from then on. A
// page keeps the set it opened with, so the files it has already had once come again the same.
//
// What it can't promise, any more than the browser could before it: a file a page loads for the first time later on (a script loaded as
// it is needed) comes from the node as it is then; and a check made while files are still being deployed takes what it finds, which
// the next check, a minute on, puts right. A file that can't be read during a check (the node down, an error) leaves the set as it was
// until the next one. Anything the browser's storage refuses, and the page is served from the node instead, as if this were not here.
//
// Only the apps' own pages ("c_app_pages") and the files they load, and only files: never the node's requests ("/cws/..."), nothing
// with a query, nothing from another origin. Any other page - Ian's harness - is left to the network, as before. A browser runs this
// only for a secure page - HTTPS, or "localhost", so the development container too. A hard reload passes it by, as it does any cache;
// "Reset this browser" throws away what it kept, and it starts again empty ("signin_forget_kept_files( )"); "cws.app_worker" set to
// "off" in a browser's storage turns it off there ("apps_keep_files( )").

const c_app_pages = [ "chat.html", "home.html", "account.html", "console.html" ];

// NOTE: The layout of what is kept - a worker that keeps it differently changes it, and what the old one kept is let go ("activate").
const c_kept_prefix = "ciyam-app-";
const c_set_prefix = "ciyam-app-2-";
const c_set_pattern = /^ciyam-app-2-[0-9]+$/;

// NOTE: Which set each page opened with, and when, kept where it outlasts this worker - a browser stops an idle one within a minute,
// and a page open longer must still get its files from its own set (found by "app_worker", 2026-11-17).
const c_pins = "ciyam-app-2-pins";

// NOTE: How long a page's set is kept for it whatever else - a page still opening isn't yet among a browser's open pages (found by
// review, 2026-11-17).
const c_pin_grace_ms = 5 * 60 * 1000;

// NOTE: Kept in each set once it is whole - a set without it is still being made, and is not used.
const c_set_whole = "/app_worker/whole";

const c_check_every_ms = 60000;

const c_file_pattern = /^\/[A-Za-z0-9_.\/-]+\.(html|form|js|css|list|woff2|woff|ttf|svg|png|ico|txt)$/;

const c_worker_path = "/app_worker.js";

// NOTE: Each page, by its client id, and the set it opened with - as kept in "c_pins", read from there when not known here.
var g_pinned = new Map( );

var g_checking = null;
var g_checked_at = 0;

self.addEventListener( "install", function( )
{
   self.skipWaiting( );
} );

// NOTE: What a worker of another layout kept, let go.
self.addEventListener( "activate", function( event )
{
   event.waitUntil( ( async function( )
   {
      var names = await caches.keys( );

      await Promise.all( names.filter( function( name )
      {
         return ( name.indexOf( c_kept_prefix ) === 0 ) && !c_set_pattern.test( name ) && ( name !== c_pins );
      } ).map( function( name ) { return caches.delete( name ); } ) );

      await self.clients.claim( );
   } )( ) );
} );

function is_app_page( address )
{
   var url = new URL( address );

   return ( url.origin === self.location.origin ) && c_app_pages.includes( url.pathname.replace( /^\//, "" ) );
}

function set_number( name )
{
   return parseInt( name.substring( c_set_prefix.length ), 10 ) || 0;
}

async function set_names( )
{
   return ( await caches.keys( ) ).filter( function( name ) { return c_set_pattern.test( name ); } );
}

function pin_key( client_id )
{
   return "/pins/" + encodeURIComponent( client_id );
}

async function pin( client_id, name )
{
   g_pinned.set( client_id, name );

   await ( await caches.open( c_pins ) ).put( pin_key( client_id ), new Response( JSON.stringify( { name: name, at: Date.now( ) } ) ) );
}

// NOTE: A pin as kept - { name, at }, or null.
async function read_pin( response )
{
   try
   {
      var kept = JSON.parse( await response.text( ) );

      return ( kept && ( typeof kept.name === "string" ) ) ? kept : null;
   }
   catch( e )
   {
      return null;
   }
}

async function pinned( client_id )
{
   if( g_pinned.has( client_id ) )
      return g_pinned.get( client_id );

   var kept = await ( await caches.open( c_pins ) ).match( pin_key( client_id ) );
   var read = kept ? await read_pin( kept ) : null;

   if( read === null )
      return "";

   g_pinned.set( client_id, read.name );

   return read.name;
}

// NOTE: The newest whole set - made, empty, if there is none (it fills as pages ask for their files).
async function current_set( )
{
   var names = ( await set_names( ) ).sort( function( a, b ) { return set_number( b ) - set_number( a ); } );

   for( var i = 0; i < names.length; i++ )
   {
      if( await ( await caches.open( names[ i ] ) ).match( c_set_whole ) )
         return names[ i ];
   }

   var first = c_set_prefix + ( names.length ? set_number( names[ 0 ] ) + 1 : 1 );

   await ( await caches.open( first ) ).put( c_set_whole, new Response( "" ) );

   return first;
}

// NOTE: From the network, and kept in "cache" if it came whole - a page under its address without the query, so "?source=..." is
// the same page.
async function fetch_into( cache, request, page )
{
   var response = await fetch( request );

   if( response.ok && ( response.type === "basic" ) )
   {
      var key = page ? new Request( request.url.split( "?" )[ 0 ] ) : request;

      await cache.put( key, response.clone( ) );
   }

   return response;
}

async function serve_page_kept( event )
{
   var name = await current_set( );

   if( event.resultingClientId )
      await pin( event.resultingClientId, name );

   event.waitUntil( check_soon( ) );

   var cache = await caches.open( name );
   var kept = await cache.match( event.request, { ignoreSearch: true } );

   return kept || fetch_into( cache, event.request, true );
}

async function serve_file_kept( event )
{
   var client = event.clientId ? await self.clients.get( event.clientId ) : null;

   if( !client || !is_app_page( client.url ) )
      return fetch( event.request );

   var name = await pinned( event.clientId );

   if( !name || !await caches.has( name ) )
      name = await current_set( );

   var cache = await caches.open( name );
   var kept = await cache.match( event.request );

   return kept || fetch_into( cache, event.request, false );
}

// NOTE: Anything the browser's storage refuses (full, say) - served from the node, as if this were not here (found by review).
async function serve( event, kept_way )
{
   try
   {
      return await kept_way( event );
   }
   catch( e )
   {
      return fetch( event.request );
   }
}

self.addEventListener( "fetch", function( event )
{
   var request = event.request;

   if( request.method !== "GET" )
      return;

   var url = new URL( request.url );

   if( url.origin !== self.location.origin )
      return;

   if( request.mode === "navigate" )
   {
      if( is_app_page( request.url ) )
         event.respondWith( serve( event, serve_page_kept ) );

      return;
   }

   if( ( url.search !== "" ) || ( url.pathname === c_worker_path ) || !c_file_pattern.test( url.pathname ) )
      return;

   // NOTE: Only for an app page. Another page that says it asked (its referrer - Ian's harness) is left alone at once, never taken
   // by the worker (found by "app_worker", 2026-11-17); one that says nothing - the chat sends no referrer, so a link in a message
   // doesn't give its address away - is told apart by its client ("serve_file_kept( )").
   if( request.referrer && !is_app_page( request.referrer ) )
      return;

   event.respondWith( serve( event, serve_file_kept ) );
} );

// NOTE: Whether two copies of a file are the same - by their contents, not when they were changed (found by review: an edit within
// the same second, the same length, would be missed). An unchanged one comes from the browser's own copy, so costs nothing more.
async function same_contents( kept, fresh )
{
   if( !kept )
      return false;

   var a = new Uint8Array( await crypto.subtle.digest( "SHA-256", await kept.clone( ).arrayBuffer( ) ) );
   var b = new Uint8Array( await crypto.subtle.digest( "SHA-256", await fresh.clone( ).arrayBuffer( ) ) );

   return a.every( function( byte, i ) { return byte === b[ i ]; } );
}

function check_soon( )
{
   if( g_checking !== null )
      return g_checking;

   if( Date.now( ) - g_checked_at < c_check_every_ms )
      return Promise.resolve( );

   g_checked_at = Date.now( );

   g_checking = check( ).catch( function( ) { } ).then( function( ) { g_checking = null; } );

   return g_checking;
}

// NOTE: Every file kept asked of the node again ("no-cache" - the browser's own copy revalidated, so an unchanged one costs a "not
// modified"). If any has changed, the next set is made from the node's copies of them all, and marked whole only once it all is. One
// the node no longer has is left out; one it can't give just now (down, an error) leaves the set as it is until the next check (found
// by review, 2026-11-17: its old copy, kept in a new set, could have mixed two versions for as long as a page was open).
async function check( )
{
   var name = await current_set( );
   var cache = await caches.open( name );
   var requests = ( await cache.keys( ) ).filter( function( request ) { return new URL( request.url ).pathname !== c_set_whole; } );
   var fresh = [ ];
   var changed = false;
   var unread = false;

   await Promise.all( requests.map( async function( request )
   {
      var response = null;

      try
      {
         response = await fetch( request.url, { cache: "no-cache" } );
      }
      catch( e )
      {
         response = null;
      }

      if( ( response !== null ) && ( response.status === 404 ) )
      {
         changed = true;

         return;
      }

      if( ( response === null ) || !response.ok )
      {
         unread = true;

         return;
      }

      if( !await same_contents( await cache.match( request ), response ) )
         changed = true;

      fresh.push( [ request, response ] );
   } ) );

   if( changed && !unread )
   {
      var next = c_set_prefix + ( set_number( name ) + 1 );
      var made = await caches.open( next );

      await Promise.all( fresh.map( function( pair ) { return made.put( pair[ 0 ], pair[ 1 ] ); } ) );
      await made.put( c_set_whole, new Response( "" ) );

      name = next;
   }

   await tidy( name );
}

// NOTE: After each check - pages gone let go of, and the sets no page uses: all but "keep" (the newest) and those of a page still open,
// or opened within "c_pin_grace_ms" (found by review: pages' pins were only let go of when something had changed).
async function tidy( keep )
{
   var open = new Set( ( await self.clients.matchAll( { type: "window", includeUncontrolled: true } ) ).map( function( c ) { return c.id; } ) );
   var used = new Set( [ keep ] );
   var pins = await caches.open( c_pins );

   for( var request of await pins.keys( ) )
   {
      var id = decodeURIComponent( new URL( request.url ).pathname.replace( /^\/pins\//, "" ) );
      var kept = await read_pin( await pins.match( request ) );

      if( ( kept !== null ) && ( open.has( id ) || ( Date.now( ) - ( kept.at || 0 ) < c_pin_grace_ms ) ) )
         used.add( kept.name );
      else
      {
         g_pinned.delete( id );

         await pins.delete( request );
      }
   }

   var names = await set_names( );

   await Promise.all( names.filter( function( name ) { return !used.has( name ); } ).map( function( name ) { return caches.delete( name ); } ) );
}

// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The apps' own files from a copy kept in this browser (Damon, 2026-11-17: switching apps over a slow 4G connection took 4.3 s,
// nearly all of it asking the node whether each of 27 unchanged files had changed - it sends "Cache-Control: no-cache", so that a
// page never runs old scripts with new markup, ISS-021). An app page and every file it loads come from one set kept here, so opening
// one asks the node nothing for them. Each time an app page opens, the set is checked against the node in the background - at most
// once a minute ("c_check_every_ms") - and anything changed is fetched; a new set is made whole before any page uses it, and only
// pages opened from then on do. A page keeps the set it opened with, so a file it loads later (a script loaded as it is needed) is
// never from another version.
//
// Only the apps' own pages ("c_app_pages") and the files they load, and only files: never the node's requests ("/cws/..."), nothing
// with a query, nothing from another origin. Any other page - Ian's harness - is left to the network, as before. A browser runs
// this only for a secure page (HTTPS, or "localhost"); elsewhere the apps load as they always have. A hard reload passes it by, as
// it does any cache; "Reset this browser" removes it and what it kept ("signin_reset_browser( )").

const c_app_pages = [ "chat.html", "home.html", "account.html", "console.html" ];

const c_set_prefix = "ciyam-app-";
const c_set_pattern = /^ciyam-app-[0-9]+$/;

// NOTE: Which set each page opened with, kept where it outlasts this worker - a browser stops an idle one within a minute, and a
// page open longer must still get its later files from its own set (found by "app_worker", 2026-11-17).
const c_pins = "ciyam-app-pins";

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

self.addEventListener( "activate", function( event )
{
   event.waitUntil( self.clients.claim( ) );
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

async function pin( client_id, name )
{
   g_pinned.set( client_id, name );

   await ( await caches.open( c_pins ) ).put( "/pins/" + encodeURIComponent( client_id ), new Response( name ) );
}

async function pinned( client_id )
{
   if( g_pinned.has( client_id ) )
      return g_pinned.get( client_id );

   var kept = await ( await caches.open( c_pins ) ).match( "/pins/" + encodeURIComponent( client_id ) );

   if( !kept )
      return "";

   var name = await kept.text( );

   g_pinned.set( client_id, name );

   return name;
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

async function serve_page( event )
{
   var name = await current_set( );

   if( event.resultingClientId )
      await pin( event.resultingClientId, name );

   event.waitUntil( check_soon( ) );

   var cache = await caches.open( name );
   var kept = await cache.match( event.request, { ignoreSearch: true } );

   return kept || fetch_into( cache, event.request, true );
}

async function serve_file( event )
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
         event.respondWith( serve_page( event ) );

      return;
   }

   if( ( url.search !== "" ) || ( url.pathname === c_worker_path ) || !c_file_pattern.test( url.pathname ) )
      return;

   // NOTE: Only for an app page. Another page that says it asked (its referrer - Ian's harness) is left alone at once, never taken
   // by the worker (found by "app_worker", 2026-11-17); one that says nothing - the chat sends no referrer, so a link in a message
   // doesn't give its address away - is told apart by its client ("serve_file( )").
   if( request.referrer && !is_app_page( request.referrer ) )
      return;

   event.respondWith( serve_file( event ) );
} );

// NOTE: Whether the node's copy differs from the one kept - by when it was last changed, and its length.
function differs( kept, fresh )
{
   if( !kept )
      return true;

   return ( kept.headers.get( "Last-Modified" ) !== fresh.headers.get( "Last-Modified" ) )
    || ( kept.headers.get( "Content-Length" ) !== fresh.headers.get( "Content-Length" ) );
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
// modified"). If any has changed, the next set is made from the lot - changed and unchanged - and marked whole only once it all is.
// One that could not be read keeps its old copy.
async function check( )
{
   var name = await current_set( );
   var cache = await caches.open( name );
   var requests = ( await cache.keys( ) ).filter( function( request ) { return new URL( request.url ).pathname !== c_set_whole; } );
   var fresh = [ ];
   var changed = false;

   await Promise.all( requests.map( async function( request )
   {
      var kept = await cache.match( request );
      var response = null;

      try
      {
         response = await fetch( request.url, { cache: "no-cache" } );
      }
      catch( e )
      {
         response = null;
      }

      if( ( response === null ) || !response.ok )
      {
         fresh.push( [ request, kept ] );

         return;
      }

      if( differs( kept, response ) )
         changed = true;

      fresh.push( [ request, response ] );
   } ) );

   if( !changed )
      return;

   var next = c_set_prefix + ( set_number( name ) + 1 );
   var made = await caches.open( next );

   await Promise.all( fresh.map( function( pair ) { return made.put( pair[ 0 ], pair[ 1 ] ); } ) );
   await made.put( c_set_whole, new Response( "" ) );

   await tidy( next );
}

// NOTE: Sets no open page uses let go - all but "keep" (the newest) and those pinned by a page still open.
async function tidy( keep )
{
   var open = new Set( ( await self.clients.matchAll( { type: "window", includeUncontrolled: true } ) ).map( function( c ) { return c.id; } ) );
   var used = new Set( [ keep ] );
   var pins = await caches.open( c_pins );

   for( var request of await pins.keys( ) )
   {
      var id = decodeURIComponent( new URL( request.url ).pathname.replace( /^\/pins\//, "" ) );

      if( open.has( id ) )
         used.add( await ( await pins.match( request ) ).text( ) );
      else
      {
         g_pinned.delete( id );

         await pins.delete( request );
      }
   }

   var names = await set_names( );

   await Promise.all( names.filter( function( name ) { return !used.has( name ); } ).map( function( name ) { return caches.delete( name ); } ) );
}

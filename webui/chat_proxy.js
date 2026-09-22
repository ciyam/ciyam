// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: A development proxy so the chat client can be exercised against a remote CIYAM
// server. Static files are served from this directory and every API path is forwarded
// upstream, so the browser sees a single origin.
//
// This exists because "ciyam.js" builds every URL from "window.location" and the server
// sends no CORS headers - the client cannot talk to a different origin. It is a testing
// aid only and is not part of the deployed system.
//
//   node chat_proxy.js [<upstream>] [<port>]
//
//   node chat_proxy.js                              https://api.ciyam.org on port 8099
//   node chat_proxy.js http://localhost:13031       the local container
//   node chat_proxy.js https://api.ciyam.org 9000   a different local port
//
// Then open http://localhost:8099/chat.html

const fs = require( "fs" );
const url = require( "url" );
const path = require( "path" );
const http = require( "http" );
const https = require( "https" );

const c_default_upstream = "https://api.ciyam.org";

const c_default_port = 8099;

// NOTE: Anything not matching a local file is forwarded, but these are listed so an
// obvious typo in an API path fails as a proxy error rather than a silent 404.
const c_api_prefixes = [ "/cws", "/echo", "/system", "/upload",
 "/ip-addr", "/unix-now", "/post-limit" ];

const c_content_types = {
   ".html": "text/html; charset=UTF-8",
   ".js": "text/javascript; charset=UTF-8",
   ".css": "text/css; charset=UTF-8",
   ".form": "text/html; charset=UTF-8",
   ".json": "application/json",
   ".png": "image/png",
   ".woff2": "font/woff2",
   ".svg": "image/svg+xml"
};

const upstream = url.parse( process.argv[ 2 ] || c_default_upstream );

const port = parseInt( process.argv[ 3 ] || c_default_port, 10 );

function is_api_path( pathname )
{
   for( var i = 0; i < c_api_prefixes.length; i++ )
   {
      if( ( pathname === c_api_prefixes[ i ] )
       || ( pathname.indexOf( c_api_prefixes[ i ] + "/" ) === 0 ) )
         return true;
   }

   return false;
}

function serve_file( pathname, res )
{
   // NOTE: Refuse anything that climbs out of this directory.
   var name = path.normalize( pathname ).replace( /^(\.\.[/\\])+/, "" );

   var file = path.join( __dirname, name );

   if( file.indexOf( __dirname ) !== 0 )
   {
      res.writeHead( 403 );
      res.end( "forbidden" );

      return;
   }

   fs.readFile( file, function( err, data )
   {
      if( err )
      {
         res.writeHead( 404, { "Content-Type": "text/plain" } );
         res.end( "not found: " + name );

         return;
      }

      var type = c_content_types[ path.extname( file ).toLowerCase( ) ] || "application/octet-stream";

      res.writeHead( 200, { "Content-Type": type, "Cache-Control": "no-store" } );
      res.end( data );
   } );
}

function forward( req, res, pathname, search )
{
   var transport = ( upstream.protocol === "https:" ) ? https : http;

   var options = {
      host: upstream.hostname,
      port: upstream.port || ( ( upstream.protocol === "https:" ) ? 443 : 80 ),
      path: pathname + ( search || "" ),
      method: req.method,
      headers: { "host": upstream.host }
   };

   var proxied = transport.request( options, function( upstream_res )
   {
      console.log( "  " + req.method + " " + pathname + " -> " + upstream_res.statusCode );

      res.writeHead( upstream_res.statusCode, {
         "Content-Type": upstream_res.headers[ "content-type" ] || "text/plain" } );

      upstream_res.pipe( res );
   } );

   proxied.on( "error", function( err )
   {
      console.log( "  " + req.method + " " + pathname + " -> proxy error: " + err.message );

      res.writeHead( 502, { "Content-Type": "text/plain" } );
      res.end( "upstream error: " + err.message );
   } );

   req.pipe( proxied );
}

http.createServer( function( req, res )
{
   var parsed = url.parse( req.url );

   var pathname = parsed.pathname;

   if( pathname === "/" )
      pathname = "/chat.html";

   if( is_api_path( pathname ) )
      forward( req, res, pathname, parsed.search );
   else
      serve_file( pathname, res );
} ).listen( port, function( )
{
   console.log( "" );
   console.log( "  chat proxy" );
   console.log( "" );
   console.log( "    serving   " + __dirname );
   console.log( "    upstream  " + upstream.href );
   console.log( "" );
   console.log( "    open      http://localhost:" + port + "/chat.html" );
   console.log( "" );
} );

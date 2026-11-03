// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Reading a QR code - by the camera, or from a picture of one where there is no camera or no https - for the chat's
// scanned message ("chat_keys.js") and My account's contact code ("account_keys.js"). "html5-qrcode.min.js" read only the
// first time; it draws the camera's picture in the element named.
var g_qr_scan_library = null;

function qr_scan_library( )
{
   if( g_qr_scan_library === null )
   {
      g_qr_scan_library = ( typeof Html5Qrcode !== "undefined" ) ? Promise.resolve( true ) : new Promise( function( resolve )
      {
         var script = document.createElement( "script" );

         script.src = "html5-qrcode.min.js";
         script.onload = function( ) { resolve( true ); };
         script.onerror = function( ) { resolve( false ); };

         document.body.appendChild( script );
      } ).then( function( ok )
      {
         if( !ok )
            g_qr_scan_library = null;

         return ok;
      } );
   }

   return g_qr_scan_library;
}

function qr_scan_reader( id )
{
   return new Html5Qrcode( id, { verbose: false, formatsToSupport: [ Html5QrcodeSupportedFormats.QR_CODE ],
    experimentalFeatures: { useBarCodeDetectorIfSupported: true } } );
}

// NOTE: Whether a camera may be asked for at all - only on https (or this machine).
function qr_scan_can_use_camera( )
{
   return !!window.isSecureContext && !!navigator.mediaDevices;
}

// NOTE: The camera let go - "stop( )" throws if it never started, so it is asked in a promise and the error ignored.
function qr_scan_stop( scanner )
{
   if( !scanner )
      return Promise.resolve( );

   return Promise.resolve( ).then( function( ) { return scanner.stop( ); } ).then( function( ) { scanner.clear( ); } ).catch( function( ) { } );
}

async function qr_scan_read_file( id, file )
{
   try
   {
      return await qr_scan_reader( id ).scanFile( file, false );
   }
   catch( e )
   {
      return null;
   }
}

// NOTE: The reader misses about one sharp picture of a valid code in ten - where the browser has no "BarcodeDetector" (as on
// Windows) it binarises in blocks of 8 px, so large crisp modules can leave a block all one shade. Drawn again smaller, from a
// half down to a twelfth while still 100 px across, it read every one of them (found by testing, 2026-11-01). "still( )" says
// whether it is still wanted; null if no code could be read.
async function qr_scan_picture( id, file, still )
{
   var text = await qr_scan_read_file( id, file );

   if( text !== null )
      return text;

   var bitmap = null;

   try
   {
      bitmap = await createImageBitmap( file );
   }
   catch( e )
   {
      return null;
   }

   var parts = [ 2, 3, 4, 5, 6, 8, 10, 12 ];

   for( var i = 0; ( i < parts.length ) && ( text === null ) && still( ); i++ )
   {
      var width = Math.round( bitmap.width / parts[ i ] );
      var height = Math.round( bitmap.height / parts[ i ] );

      if( Math.min( width, height ) < 100 )
         break;

      var canvas = document.createElement( "canvas" );

      canvas.width = width;
      canvas.height = height;

      var context = canvas.getContext( "2d" );

      context.fillStyle = "#ffffff";
      context.fillRect( 0, 0, width, height );
      context.drawImage( bitmap, 0, 0, width, height );

      var blob = await new Promise( function( resolve ) { canvas.toBlob( resolve, "image/png" ); } );

      if( blob !== null )
         text = await qr_scan_read_file( id, new File( [ blob ], "picture.png", { type: "image/png" } ) );
   }

   bitmap.close( );

   return text;
}

// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: A password hashed with the time - "<unix time>-<sha256( time + sha256( password ) )>" - for "ciyam_client -rpc_unlock",
// said and drawn as a QR code through the page ("show_qr" in its context), a module ("Modules" in "ciyam.js"). The usage
// hint, the first time, is the command with the answer in it.
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_rpc_unlock",
      version: "2",
      description: "a password hashed with the time, said and drawn as a QR code - for ciyam_client -rpc_unlock",
      example: "test",
      hint: function( result ) { return "./ciyam_client -tls -rpc_unlock=" + result + " localhost:6666"; },
      needs: [ "sha256", "qrcode" ]
   };

   async run( input )
   {
      var sha256 = this.context.sha256 || ciyam_sha256_hex;

      var unix_now = Math.floor( Date.now( ) / 1000 );
      var password_hash = await sha256( ( input === null ) ? "" : input );
      var answer = unix_now + "-" + await sha256( unix_now + password_hash );

      if( this.context.show_qr )
         this.context.show_qr( answer );

      return answer;
   }
} );

// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: XORs hex with the result before it - a module ("Modules" in "ciyam.js"). "xor( a, b )" is its core, for a page to use
// directly (Home's setting up mixes the node's suggestion with the person's randomness); "run( input )" keeps the running
// result in the instance, where it was a global.
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_xor_hex_data",
      version: "2",
      description: "XORs hex with the result before it",
      example: "01020304",
      hint: "(i.e. XORs the prior result with hex input)",
      needs: [ ]
   };

   // NOTE: Two hex strings XORed a byte at a time - each of odd length given a trailing "0", the shorter padded at its start
   // with zeros, an empty one taken as "00" - in lower case.
   static xor( a, b )
   {
      var current = ( ( a === null ) || ( a === undefined ) || ( a === "" ) ) ? "00" : String( a );
      var implant = ( ( b === null ) || ( b === undefined ) || ( b === "" ) ) ? "00" : String( b );

      if( ( current.length % 2 ) !== 0 )
         current += "0";

      if( ( implant.length % 2 ) !== 0 )
         implant += "0";

      if( current.length < implant.length )
         current = current.padStart( implant.length, "0" );
      else if( implant.length < current.length )
         implant = implant.padStart( current.length, "0" );

      var result = "";

      for( var i = 0; i < current.length; i += 2 )
      {
         var value = parseInt( current.substr( i, 2 ), 16 ) ^ parseInt( implant.substr( i, 2 ), 16 );

         result += ( value < 16 ? "0" : "" ) + value.toString( 16 );
      }

      return result;
   }

   constructor( context )
   {
      super( context );

      this.result = null;
   }

   async run( input )
   {
      this.result = this.constructor.xor( this.result, input );

      return this.result;
   }
} );

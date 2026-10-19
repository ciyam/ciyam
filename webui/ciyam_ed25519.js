// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Ed25519 and X25519 - a module ("Modules" in "ciyam.js"), the same keys and shared secrets as the console's "@secret:ed",
// "@public" and "@shared" ("crypto_keys.cpp" with "ed25519.cpp"). The curve arithmetic is "@noble/curves" ("ed25519.min.js",
// the global "ED25519"). A secret is read as "private_key" reads one - so the same words give the same key in both - and a
// key pair is made from the 32 bytes it gives as an Ed25519 seed. The shared secret is X25519: one's clamped scalar with the
// other's public key turned to its X25519 form, refused if it comes to nothing (a key of small order).
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_ed25519",
      version: "1",
      description: "Ed25519 and X25519 - a public key, a shared secret, or a signature, from a secret as the console's @secret:ed reads it",
      example: "public <secret> | shared <secret> <public key> | sign <secret> <message> | verify <public key> <signature> <message>",
      hint: "(i.e. the same keys and shared secrets as the console's @secret:ed, @public and @shared)",
      needs: [ "ed25519" ]
   };

   static library( )
   {
      if( typeof ED25519 === "undefined" )
         throw new Error( "ed25519 needs ed25519.min.js" );

      return ED25519;
   }

   static bytes_of( hex )
   {
      var text = String( hex || "" );

      if( !/^([0-9a-fA-F]{2})*$/.test( text ) )
         throw new Error( "Not hex - an even number of the digits 0 to 9 and a to f." );

      var bytes = new Uint8Array( text.length / 2 );

      for( var i = 0; i < bytes.length; i++ )
         bytes[ i ] = parseInt( text.substr( i * 2, 2 ), 16 );

      return bytes;
   }

   static hex_of( bytes )
   {
      return Array.from( bytes, function( b ) { return b.toString( 16 ).padStart( 2, "0" ); } ).join( "" );
   }

   // NOTE: Base64 as "base64::decode( )" reads it, into "out" - the same letters, the same leniency, the same refusals.
   static base64_into( text, out )
   {
      var size = Math.floor( text.length / 4 ) * 3;

      for( var end = text.length; ( end > 1 ) && ( text[ end - 1 ] === "=" ); end-- )
         --size;

      // NOTE: More "=" than letters underflows "decode_size( )" there, which then refuses - as it does here (found by review).
      if( ( size < 0 ) || ( size > out.length ) )
         throw new Error( "A secret in base64 is at most 32 bytes." );

      // NOTE: There a character is a signed "char", so anything past ASCII is below '0' and refused (found by review).
      if( /[^\x00-\x7f]/.test( text ) )
         throw new Error( "Not base64 - only its letters, digits, '+', '/' and '='." );

      function value( ch )
      {
         if( ch === "+" )
            return 62;

         if( ch === "/" )
            return 63;

         var code = ( ch === undefined ) ? 0 : ch.charCodeAt( 0 );

         if( code >= 97 )
            return code - 97 + 26;

         if( code >= 65 )
            return code - 65;

         if( code >= 48 )
            return code - 48 + 52;

         throw new Error( "Not base64 - '" + ( ch === undefined ? "" : ch ) + "' is not one of its letters." );
      }

      var o = 0;

      for( var i = 0; i < text.length; ++i )
      {
         var c = value( text[ i ] );
         var c1 = value( text[ ++i ] );

         out[ o++ ] = ( ( c << 2 ) | ( ( c1 >> 4 ) & 0x03 ) ) & 0xff;

         if( ++i < text.length )
         {
            if( text[ i ] === "=" )
               break;

            c = value( text[ i ] );
            out[ o++ ] = ( ( ( c1 << 4 ) & 0xf0 ) | ( ( c >> 2 ) & 0x0f ) ) & 0xff;
         }

         if( ++i < text.length )
         {
            if( text[ i ] === "=" )
               break;

            c1 = value( text[ i ] );
            out[ o++ ] = ( ( ( c << 6 ) & 0xc0 ) | c1 ) & 0xff;
         }
      }
   }

   // NOTE: A secret as "private_key" reads one - under 30 bytes the bytes themselves (for simple tests only), over 50 hex,
   // otherwise base64 - into 32 bytes, the rest zeros. All zeros is refused, as the console refuses it.
   static seed_of( secret )
   {
      var text = String( secret || "" );
      var raw = new TextEncoder( ).encode( text );
      var seed = new Uint8Array( 32 );

      if( raw.length < 30 )
         seed.set( raw.subarray( 0, 32 ) );
      else if( raw.length > 50 )
      {
         if( !/^[0-9a-fA-F]*$/.test( text ) )
            throw new Error( "A secret of over 50 letters is hex - the digits 0 to 9 and a to f." );

         if( ( text.length % 2 ) !== 0 )
            throw new Error( "A secret in hex has an even number of digits." );

         if( text.length > 64 )
            throw new Error( "A secret in hex is at most 64 digits." );

         seed.set( this.bytes_of( text ) );
      }
      else
         this.base64_into( text, seed );

      if( seed.every( function( b ) { return b === 0; } ) )
         throw new Error( "That secret gives no key - it comes to nothing." );

      return seed;
   }

   static public_key( seed )
   {
      return this.hex_of( this.library( ).ed25519.getPublicKey( seed ) );
   }

   // NOTE: One's scalar with the other's public key in its X25519 form - the same from either side, and as the console's for
   // any real public key. Stricter than "ed25519.cpp": a key that is not a point on the curve is refused here, where the
   // console works a number out from it anyway (found by review).
   static shared_secret( seed, public_key_hex )
   {
      var lib = this.library( );
      var other = this.bytes_of( public_key_hex );

      if( other.length !== 32 )
         throw new Error( "A public key is 64 hex digits." );

      var secret;

      try
      {
         secret = lib.x25519.getSharedSecret( lib.ed25519.utils.toMontgomerySecret( seed ), lib.ed25519.utils.toMontgomery( other ) );
      }
      catch( e )
      {
         throw new Error( "That public key cannot give a shared secret." );
      }

      if( secret.every( function( b ) { return b === 0; } ) )
         throw new Error( "That public key cannot give a shared secret." );

      return this.hex_of( secret );
   }

   static sign( seed, message )
   {
      return this.hex_of( this.library( ).ed25519.sign( new TextEncoder( ).encode( String( message ) ), seed ) );
   }

   // NOTE: Strictly, as RFC 8032 - a signature's S less than the group order, its points as encoded. Stricter than "ed25519.cpp"
   // in one way: a public key of small order is refused, where orlp's check can take a made-up signature for one (found by review).
   static verify( public_key_hex, signature_hex, message )
   {
      var signature = this.bytes_of( signature_hex );
      var key = this.bytes_of( public_key_hex );

      if( ( signature.length !== 64 ) || ( key.length !== 32 ) )
         return false;

      try
      {
         return this.library( ).ed25519.verify( signature, new TextEncoder( ).encode( String( message ) ), key, { zip215: false } );
      }
      catch( e )
      {
         return false;
      }
   }

   // NOTE: "public <secret>", "shared <secret> <public key>", "sign <secret> <message>" or "verify <public key> <signature>
   // <message>" - a secret one word, as the console's are.
   async run( input )
   {
      var words = String( input || "" ).trim( ).split( /\s+/ );
      var what = ( words.shift( ) || "" ).toLowerCase( );
      var it = this.constructor;

      if( ( what === "public" ) && ( words.length === 1 ) )
         return it.public_key( it.seed_of( words[ 0 ] ) );

      if( ( what === "shared" ) && ( words.length === 2 ) )
         return it.shared_secret( it.seed_of( words[ 0 ] ), words[ 1 ] );

      var rest = String( input || "" ).trim( ).replace( /^\S+\s+\S+\s*/, "" );

      if( ( what === "sign" ) && ( words.length >= 1 ) )
         return it.sign( it.seed_of( words[ 0 ] ), rest );

      if( ( what === "verify" ) && ( words.length >= 2 ) )
         return it.verify( words[ 0 ], words[ 1 ], rest.replace( /^\S+\s*/, "" ) ) ? "valid" : "not valid";

      throw new Error( "Use one of - " + it.about.example );
   }
} );

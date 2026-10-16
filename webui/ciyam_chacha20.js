// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: ChaCha20 (RFC 8439) - a module ("Modules" in "ciyam.js"), as "chacha20.cpp": a 32-byte key, a 12-byte nonce and a
// block counter, the keystream XORed with the data, so the same call encrypts and decrypts. "xor( key, nonce, counter, data )"
// is its core; "crypt( password, data )" is "crypt -cc" - the key the password's SHA-256, the nonce zero, the counter from
// nought. One key and nonce must never encrypt two messages - the keystream would cancel out between them - so a message
// needs a key of its own, or a nonce of its own (the secure messages' salt, Ian 2026-10-15).
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_chacha20",
      version: "1",
      description: "ChaCha20 - the password's SHA-256 as the key, as crypt -cc; the same call encrypts and decrypts",
      example: "<password> <hex>",
      hint: "(i.e. XORs the hex with the keystream - run it again to decrypt)",
      needs: [ "sha256" ]
   };

   static word( bytes, at )
   {
      return ( bytes[ at ] | ( bytes[ at + 1 ] << 8 ) | ( bytes[ at + 2 ] << 16 ) | ( bytes[ at + 3 ] << 24 ) ) >>> 0;
   }

   // NOTE: The data XORed with the keystream - "key" 32 bytes, "nonce" 12, "counter" the first block's number. The counter
   // carries into the nonce's first word, as "chacha20.cpp" does.
   static xor( key, nonce, counter, data )
   {
      if( ( key.length !== 32 ) || ( nonce.length !== 12 ) )
         throw new Error( "ChaCha20 takes a 32-byte key and a 12-byte nonce." );

      var state = new Uint32Array( 16 );
      var block = new Uint32Array( 16 );
      var out = new Uint8Array( data.length );

      state[ 0 ] = 0x61707865;
      state[ 1 ] = 0x3320646e;
      state[ 2 ] = 0x79622d32;
      state[ 3 ] = 0x6b206574;

      for( var k = 0; k < 8; k++ )
         state[ 4 + k ] = this.word( key, k * 4 );

      state[ 12 ] = counter >>> 0;
      state[ 13 ] = ( this.word( nonce, 0 ) + Math.floor( counter / 0x100000000 ) ) >>> 0;
      state[ 14 ] = this.word( nonce, 4 );
      state[ 15 ] = this.word( nonce, 8 );

      function quarter( a, b, c, d )
      {
         block[ a ] = ( block[ a ] + block[ b ] ) >>> 0; block[ d ] ^= block[ a ]; block[ d ] = ( block[ d ] << 16 ) | ( block[ d ] >>> 16 );
         block[ c ] = ( block[ c ] + block[ d ] ) >>> 0; block[ b ] ^= block[ c ]; block[ b ] = ( block[ b ] << 12 ) | ( block[ b ] >>> 20 );
         block[ a ] = ( block[ a ] + block[ b ] ) >>> 0; block[ d ] ^= block[ a ]; block[ d ] = ( block[ d ] << 8 ) | ( block[ d ] >>> 24 );
         block[ c ] = ( block[ c ] + block[ d ] ) >>> 0; block[ b ] ^= block[ c ]; block[ b ] = ( block[ b ] << 7 ) | ( block[ b ] >>> 25 );
      }

      for( var start = 0; start < data.length; start += 64 )
      {
         block.set( state );

         for( var round = 0; round < 10; round++ )
         {
            quarter( 0, 4, 8, 12 );
            quarter( 1, 5, 9, 13 );
            quarter( 2, 6, 10, 14 );
            quarter( 3, 7, 11, 15 );
            quarter( 0, 5, 10, 15 );
            quarter( 1, 6, 11, 12 );
            quarter( 2, 7, 8, 13 );
            quarter( 3, 4, 9, 14 );
         }

         for( var i = 0; i < 64 && ( start + i ) < data.length; i++ )
         {
            var value = ( block[ i >> 2 ] + state[ i >> 2 ] ) >>> 0;

            out[ start + i ] = data[ start + i ] ^ ( ( value >>> ( ( i & 3 ) * 8 ) ) & 0xff );
         }

         state[ 12 ] = ( state[ 12 ] + 1 ) >>> 0;

         if( state[ 12 ] === 0 )
            state[ 13 ] = ( state[ 13 ] + 1 ) >>> 0;
      }

      return out;
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

   // NOTE: As "crypt -cc" - the password's SHA-256 the key, the nonce zero, the counter from nought.
   static async crypt( password, data, sha256 )
   {
      var key = this.bytes_of( await ( sha256 || ciyam_sha256_hex )( password ) );

      return this.xor( key, new Uint8Array( 12 ), 0, data );
   }

   // NOTE: "<password> <hex>" - the password a word, the data hex - answers with the hex XORed; the same again decrypts.
   async run( input )
   {
      var text = String( input || "" ).trim( );
      var space = text.indexOf( " " );

      if( space <= 0 )
         throw new Error( "Give a password and then hex - " + this.constructor.about.example );

      var data = this.constructor.bytes_of( text.substring( space + 1 ).replace( /\s+/g, "" ) );

      return this.constructor.hex_of( await this.constructor.crypt( text.substring( 0, space ), data, this.context.sha256 ) );
   }
} );

// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: An encrypted message as the console's "x" makes one ("irc.cin", Ian 2026-10-21) - a module ("Modules" in "ciyam.js").
// It is sealed under a secret: one's "own secret" - the X25519 secret between the key from one's secret entropy and the key
// from that entropy reversed - or the secret with another person, between the key from one's entropy and their public key
// (Ian, 2026-10-22). A message is sealed as "X", CR, a salt (16 random bytes in hex), CR, the ciphertext in hex, CR, and an
// HMAC - "crypt -cc" (ChaCha20) under the secret with the salt after it, of: a few random letters and digits, CR, the message,
// CR, more random letters and digits to a fixed size. The HMAC is HMAC-SHA256 of the ciphertext's hex, keyed with "mac", the
// secret and the salt, its first 32 hex digits; opening checks it before decrypting. A prototype, as Ian's is: the sizes are
// his test ones.
//
// Needs "ciyam_ed25519.js" (with "ed25519.min.js") and "ciyam_chacha20.js" loaded too.
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_secure_message",
      version: "1",
      description: "an encrypted message as the console's x makes one - sealed and opened under one's own secret, or one with another person",
      example: "own <entropy> | public <entropy> | shared <entropy> <their public key> | seal <secret> <message> | open <secret> <sealed>",
      hint: "(i.e. a secret, then a message sealed with it and opened again - a sealed one shows its CRs as \\r)",
      needs: [ "sha256", "ed25519", "chacha20" ]
   };

   static c_prefix = "X\r";
   static c_salt_bytes = 16;
   static c_hmac_digits = 32;

   // NOTE: As "irc.cin"'s "MAX_TO_ENCRYPT" (Ian, 2026-10-24) - the message padded with random letters to 500 bytes, so every
   // message is the same size. Padding (Ian, 2026-10-25): none for a message of 500 bytes; up to 10 left over all before it;
   // more than that split at random, some before and some after.
   static c_max_length = 500;
   static c_padded_length = 500;
   static c_most_before_only = 10;

   static c_alpha_numeric = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

   static ed25519( )
   {
      var ed = ciyam_module_named( "ed25519" );

      if( !ed )
         throw new Error( "secure_message needs ciyam_ed25519.js loaded first." );

      return ed;
   }

   static chacha20( )
   {
      var chacha = ciyam_module_named( "chacha20" );

      if( !chacha )
         throw new Error( "secure_message needs ciyam_chacha20.js loaded first." );

      return chacha;
   }

   static hex_of( bytes )
   {
      return Array.from( bytes, function( b ) { return b.toString( 16 ).padStart( 2, "0" ); } ).join( "" );
   }

   static random_bytes( count )
   {
      var bytes = new Uint8Array( count );

      crypto.getRandomValues( bytes );

      return bytes;
   }

   // NOTE: Letters and digits, each from a random byte below 248 (four times 62) so that none comes up more often than another.
   static random_alpha_numeric( count )
   {
      var text = "";

      while( text.length < count )
      {
         var bytes = this.random_bytes( count - text.length );

         for( var i = 0; i < bytes.length; i++ )
            if( bytes[ i ] < 248 )
               text += this.c_alpha_numeric[ bytes[ i ] % 62 ];
      }

      return text;
   }

   // NOTE: From 0 to one less than "count" (at most 65,536), each as likely - two random bytes, those past the last whole
   // multiple of "count" drawn again.
   static random_below( count )
   {
      var limit = Math.floor( 65536 / count ) * count;

      while( true )
      {
         var bytes = this.random_bytes( 2 );
         var value = ( bytes[ 0 ] << 8 ) | bytes[ 1 ];

         if( value < limit )
            return value % count;
      }
   }

   // NOTE: How many of the bytes left over go before the message (Ian, 2026-10-25) - all of them up to 10, otherwise from one
   // to one less than all, the rest after it.
   static padding_before( left_over )
   {
      if( left_over <= this.c_most_before_only )
         return left_over;

      return 1 + this.random_below( left_over - 1 );
   }

   // NOTE: SHA-256 of bytes - the browser's own where it has one (a secure context), "sha2.min.js" otherwise, given the bytes
   // as a string of character codes, which it hashes as they are.
   static async sha256_bytes( bytes )
   {
      if( ( typeof crypto !== "undefined" ) && crypto.subtle && ( typeof crypto.subtle.digest === "function" ) )
         return new Uint8Array( await crypto.subtle.digest( "SHA-256", bytes ) );

      if( typeof hex_sha256 !== "function" )
         throw new Error( "SHA-256 is not available - this page needs sha2.min.js, or https." );

      var text = "";

      for( var i = 0; i < bytes.length; i++ )
         text += String.fromCharCode( bytes[ i ] );

      var hex = hex_sha256( text );
      var digest = new Uint8Array( hex.length / 2 );

      for( var j = 0; j < digest.length; j++ )
         digest[ j ] = parseInt( hex.substr( j * 2, 2 ), 16 );

      return digest;
   }

   static concat( a, b )
   {
      var out = new Uint8Array( a.length + b.length );

      out.set( a );
      out.set( b, a.length );

      return out;
   }

   // NOTE: HMAC-SHA256 (RFC 2104) of bytes with a key in bytes - a key longer than a block is hashed first.
   static async hmac_sha256_bytes( key, message )
   {
      var block = new Uint8Array( 64 );

      block.set( ( key.length > 64 ) ? await this.sha256_bytes( key ) : key );

      var inner_pad = block.map( function( b ) { return b ^ 0x36; } );
      var outer_pad = block.map( function( b ) { return b ^ 0x5c; } );

      var inner = await this.sha256_bytes( this.concat( inner_pad, message ) );

      return await this.sha256_bytes( this.concat( outer_pad, inner ) );
   }

   // NOTE: As "@hmac256:<key>:<message>" - the key and the message text, the answer lower-case hex.
   static async hmac_sha256( key, message )
   {
      var encoder = new TextEncoder( );

      return this.hex_of( await this.hmac_sha256_bytes( encoder.encode( String( key ) ), encoder.encode( String( message ) ) ) );
   }

   // NOTE: As "irc.cin" - the key from the entropy ("@secret"), the key from the entropy reversed, and the X25519 secret
   // between the first and the second's public key ("@shared").
   static own_secret( entropy )
   {
      var text = this.check_entropy( entropy );
      var ed = this.ed25519( );

      return ed.shared_secret( ed.seed_of( text ), ed.public_key( ed.seed_of( text.split( "" ).reverse( ).join( "" ) ) ) );
   }

   static check_entropy( entropy )
   {
      var text = String( entropy || "" ).trim( );

      if( !/^[0-9a-fA-F]{32,64}$/.test( text ) )
         throw new Error( "Secret entropy is 32 to 64 hex digits." );

      return text;
   }

   // NOTE: The public key from the entropy - the one "irc.cin" shows as a QR code for others to scan.
   static public_key( entropy )
   {
      var ed = this.ed25519( );

      return ed.public_key( ed.seed_of( this.check_entropy( entropy ) ) );
   }

   // NOTE: The secret for messages with another person, as "x" makes it once they are picked - "@shared" between the key from
   // one's entropy and their public key, the same from either side.
   static shared_secret( entropy, public_key )
   {
      var ed = this.ed25519( );

      return ed.shared_secret( ed.seed_of( this.check_entropy( entropy ) ), String( public_key || "" ).trim( ).toLowerCase( ) );
   }

   // NOTE: The same from a key kept as its seed ("seeds_of( )" in "ciyam_user_keys.js") - the browser keeps the keys, not the
   // entropy (Ian, 2026-10-25).
   static check_seed( seed )
   {
      var text = String( seed || "" ).toLowerCase( );

      if( !/^[0-9a-f]{64}$/.test( text ) )
         throw new Error( "A key's seed is 64 hex digits." );

      return this.ed25519( ).bytes_of( text );
   }

   static public_key_of_seed( seed )
   {
      return this.ed25519( ).public_key( this.check_seed( seed ) );
   }

   static shared_secret_of_seed( seed, public_key )
   {
      return this.ed25519( ).shared_secret( this.check_seed( seed ), String( public_key || "" ).trim( ).toLowerCase( ) );
   }

   static check_secret( secret )
   {
      if( !/^[0-9a-f]{64}$/.test( String( secret || "" ) ) )
         throw new Error( "A secret is 64 hex digits - the own secret, or one with another person." );
   }

   // NOTE: Under "secret" - the own secret, or one with another person. "options" fixes the salt, the padding before and the
   // padding after, for a known answer; each is random otherwise. The chat never sends a CR, so one in the message is taken as
   // a line break, as when posting.
   static async seal( secret, message, options )
   {
      this.check_secret( secret );

      var opts = options || { };
      var text = String( message ).replace( /\r\n?/g, "\n" );

      var length = new TextEncoder( ).encode( text ).length;

      if( length > this.c_max_length )
         throw new Error( "That message is too long to seal - at most " + this.c_max_length + " bytes." );

      var left_over = this.c_padded_length - length;
      var before_length = this.padding_before( left_over );

      var before = ( opts.before !== undefined ) ? opts.before : this.random_alpha_numeric( before_length );
      var after = ( opts.after !== undefined ) ? opts.after : this.random_alpha_numeric( left_over - before_length );
      var salt = ( opts.salt !== undefined ) ? opts.salt : this.hex_of( this.random_bytes( this.c_salt_bytes ) );

      // NOTE: A salt or padding given must be what "open( )" takes - or what is sealed could not be opened.
      if( !/^[0-9a-f]{32}$/.test( salt ) || /\r/.test( before + after ) )
         throw new Error( "A salt is 32 lower-case hex digits, and the padding has no CR." );

      var padded = new TextEncoder( ).encode( before + "\r" + text + "\r" + after );
      var ciphertext = this.hex_of( await this.chacha20( ).crypt( secret + salt, padded ) );
      var hmac = ( await this.hmac_sha256( "mac" + secret + salt, ciphertext ) ).substring( 0, this.c_hmac_digits );

      return this.c_prefix + salt + "\r" + ciphertext + "\r" + hmac;
   }

   // NOTE: The message, or an error: not sealed this way, or its HMAC not matching (changed, or sealed under another secret).
   static async open( secret, sealed )
   {
      this.check_secret( secret );

      var text = String( sealed || "" );
      var parts = text.startsWith( this.c_prefix ) ? text.substring( this.c_prefix.length ).split( "\r" ) : [ ];

      if( ( parts.length !== 3 ) || !/^[0-9a-f]{32}$/.test( parts[ 0 ] ) || !/^([0-9a-f]{2})+$/.test( parts[ 1 ] )
       || !/^[0-9a-f]{32}$/.test( parts[ 2 ] ) )
         throw new Error( "That is not a sealed message." );

      var salt = parts[ 0 ];
      var ciphertext = parts[ 1 ];

      var expected = ( await this.hmac_sha256( "mac" + secret + salt, ciphertext ) ).substring( 0, this.c_hmac_digits );

      // NOTE: Every digit compared, so the time taken says nothing of where they differ.
      var differ = 0;

      for( var i = 0; i < this.c_hmac_digits; i++ )
         differ |= expected.charCodeAt( i ) ^ parts[ 2 ].charCodeAt( i );

      if( differ )
         throw new Error( "That message has been changed, or was sealed with another secret." );

      var chacha = this.chacha20( );
      var padded = new TextDecoder( ).decode( await chacha.crypt( secret + salt, chacha.bytes_of( ciphertext ) ) );
      var first = padded.indexOf( "\r" );
      var last = padded.indexOf( "\r", first + 1 );

      if( ( first < 0 ) || ( last < 0 ) )
         throw new Error( "That message has no padding - it was not sealed this way." );

      return padded.substring( first + 1, last );
   }

   // NOTE: "own <entropy>", "public <entropy>", "shared <entropy> <their public key>", "seal <secret> <message>" or "open
   // <secret> <sealed>" - a sealed message shown, and given, with each CR as "\r", since a console line cannot hold one.
   async run( input )
   {
      var it = this.constructor;
      var text = String( input || "" ).trim( );
      var words = text.split( /\s+/ );
      var what = ( words[ 0 ] || "" ).toLowerCase( );

      if( ( what === "own" ) && ( words.length === 2 ) )
         return it.own_secret( words[ 1 ] );

      if( ( what === "public" ) && ( words.length === 2 ) )
         return it.public_key( words[ 1 ] );

      if( ( what === "shared" ) && ( words.length === 3 ) )
         return it.shared_secret( words[ 1 ], words[ 2 ] );

      var rest = text.replace( /^\S+\s+\S+\s?/, "" );

      if( ( what === "seal" ) && ( words.length >= 3 ) )
         return ( await it.seal( words[ 1 ], rest ) ).replace( /\r/g, "\\r" );

      if( ( what === "open" ) && ( words.length === 3 ) )
         return it.open( words[ 1 ], rest.replace( /\\r/g, "\r" ) );

      throw new Error( "Use one of - " + it.about.example );
   }
} );

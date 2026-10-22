// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: An account's own keys as the node keeps them ("user-keys", Ian 2026-10-22) - a module ("Modules" in "ciyam.js"). What is
// kept is the secret entropy (from twelve words, as a node's is) and the public keys of others, scanned face to face. It is
// locked before it leaves the page, under a key hardened from a password with a salt of its own - "harden"'s rounds of the
// salt and the password - so the node cannot open it without that password. Not the account's own password: the node keeps
// one SHA-256 of the PIN and that, so whoever runs it could guess it at that speed, round the hardening (found by review,
// 2026-10-23) - a passphrase of its own. Kept as:
//
//    UK1:<rounds>:<salt>:<ciphertext>:<hmac>
//
// the rounds in decimal, the salt (16 random bytes) and the ciphertext in hex - "crypt -cc" (ChaCha20) under the hardened key -
// and HMAC-SHA256 of the ciphertext's hex keyed with "mac" and the hardened key, in full. Locking again picks a new salt, so no
// two lockings share a key. The rounds are kept so they can be raised later without losing what was locked before.
//
// Needs "ciyam_harden.js", "ciyam_chacha20.js" and "ciyam_secure_message.js" (for its HMAC) loaded too, and "bip39.min.js" for
// the words.
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_user_keys",
      version: "1",
      description: "an account's own keys - locked under a password before they go to the node, and opened again",
      example: "lock <password> <entropy> | unlock <password> <locked> | words <entropy> | entropy <twelve words>",
      hint: "(i.e. the secret entropy and others' public keys, locked so the node cannot read them)",
      needs: [ "sha256", "harden", "chacha20", "secure_message", "bip39" ]
   };

   static c_format = "UK1";
   static c_salt_bytes = 16;

   // NOTE: About two thirds of a second in Chrome on a laptop (2026-10-23), so a few seconds on a phone, once a session. Keys
   // come back from the node, so it cannot ask for more than ten times as many - or it could keep a page hashing for hours.
   static c_default_rounds = 100000;
   static c_most_rounds = 1000000;

   static module_named( name )
   {
      var found = ciyam_module_named( name );

      if( !found )
         throw new Error( "user_keys needs ciyam_" + name + ".js loaded first." );

      return found;
   }

   static hex_of( bytes )
   {
      return Array.from( bytes, function( b ) { return b.toString( 16 ).padStart( 2, "0" ); } ).join( "" );
   }

   // NOTE: The twelve words for entropy, and back - BIP39, as a node's are made and recovered.
   static words_of_entropy( entropy )
   {
      if( !/^([0-9a-fA-F]{2}){16,32}$/.test( String( entropy || "" ) ) )
         throw new Error( "Secret entropy is 32 to 64 hex digits - whole bytes." );

      return BIP39.entropyToMnemonic( String( entropy ).toLowerCase( ) );
   }

   static entropy_of_words( words )
   {
      var text = String( words || "" ).trim( ).toLowerCase( ).split( /\s+/ ).join( " " );

      if( !BIP39.validateMnemonic( text ) )
         throw new Error( "Those are not the words - check each is spelt right and in its place." );

      return BIP39.mnemonicToEntropy( text );
   }

   // NOTE: The key from a password and a salt - "harden"'s rounds of the salt and the password, with the page's faster SHA-256.
   // The password in its composed form (NFC), so one typed with "ä" as one character or as two gives the same key.
   static async hardened_key( password, salt, rounds )
   {
      var harden = this.module_named( "harden" );
      var hasher = ( typeof ciyam_sha256_fastest === "function" ) ? await ciyam_sha256_fastest( ) : { native: true, hash: ciyam_sha256_hex };

      if( !Number.isInteger( rounds ) || ( rounds < 1 ) || ( rounds > this.c_most_rounds ) )
         throw new Error( "The rounds are a whole number from 1 to " + this.c_most_rounds + "." );

      return harden.rounds( hasher, rounds, salt + String( password ).normalize( "NFC" ), null );
   }

   // NOTE: What is kept - checked, so nothing that could not be used is locked: the entropy, and others' public keys by name.
   static contents_text( contents )
   {
      var entropy = String( ( contents || { } ).entropy || "" ).toLowerCase( );
      var given = ( contents || { } ).contacts || { };
      var contacts = { };

      if( !/^([0-9a-f]{2}){16,32}$/.test( entropy ) )
         throw new Error( "Secret entropy is 32 to 64 hex digits - whole bytes." );

      Object.keys( given ).sort( ).forEach( function( name )
      {
         var key = String( given[ name ] || "" ).toLowerCase( );

         if( ( name === "" ) || !/^[0-9a-f]{64}$/.test( key ) )
            throw new Error( "A public key is 64 hex digits, kept by a name." );

         contacts[ name ] = key;
      } );

      return JSON.stringify( { entropy: entropy, contacts: contacts } );
   }

   // NOTE: Locked under the password - "options" fixes the salt and the rounds, for a known answer; a new salt otherwise.
   static async lock( password, contents, options )
   {
      var opts = options || { };

      if( String( password || "" ) === "" )
         throw new Error( "A password is needed to lock the keys." );

      var text = this.contents_text( contents );
      var rounds = ( opts.rounds !== undefined ) ? Number( opts.rounds ) : this.c_default_rounds;

      var salt = ( opts.salt !== undefined ) ? String( opts.salt )
       : this.hex_of( crypto.getRandomValues( new Uint8Array( this.c_salt_bytes ) ) );

      if( !/^[0-9a-f]{32}$/.test( salt ) || !Number.isInteger( rounds ) || ( rounds < 1 ) )
         throw new Error( "A salt is 32 lower-case hex digits, and the rounds a whole number over nought." );

      var key = await this.hardened_key( password, salt, rounds );
      var chacha = this.module_named( "chacha20" );

      var ciphertext = this.hex_of( await chacha.crypt( key, new TextEncoder( ).encode( text ) ) );
      var hmac = await this.module_named( "secure_message" ).hmac_sha256( "mac" + key, ciphertext );

      return [ this.c_format, String( rounds ), salt, ciphertext, hmac ].join( ":" );
   }

   // NOTE: The contents, or an error: not locked keys, or the password not the one they were locked with (or they were changed).
   static async unlock( password, locked )
   {
      var parts = String( locked || "" ).trim( ).split( ":" );

      // NOTE: Refused before any hashing - the rounds too, as the keys come back from the node (found by review, 2026-10-23).
      if( ( parts.length !== 5 ) || ( parts[ 0 ] !== this.c_format ) || !/^[1-9][0-9]{0,8}$/.test( parts[ 1 ] )
       || ( Number( parts[ 1 ] ) > this.c_most_rounds ) || !/^[0-9a-f]{32}$/.test( parts[ 2 ] )
       || !/^([0-9a-f]{2})+$/.test( parts[ 3 ] ) || !/^[0-9a-f]{64}$/.test( parts[ 4 ] ) )
         throw new Error( "Those are not locked keys." );

      var key = await this.hardened_key( password, parts[ 2 ], Number( parts[ 1 ] ) );
      var expected = await this.module_named( "secure_message" ).hmac_sha256( "mac" + key, parts[ 3 ] );

      // NOTE: Every digit compared, so the time taken says nothing of where they differ.
      var differ = 0;

      for( var i = 0; i < expected.length; i++ )
         differ |= expected.charCodeAt( i ) ^ parts[ 4 ].charCodeAt( i );

      if( differ )
         throw new Error( "That password does not open these keys." );

      var chacha = this.module_named( "chacha20" );
      var contents = JSON.parse( new TextDecoder( ).decode( await chacha.crypt( key, chacha.bytes_of( parts[ 3 ] ) ) ) );

      return { entropy: contents.entropy, contacts: contents.contacts || { } };
   }

   // NOTE: "lock <password> <entropy>", "unlock <password> <locked>", "words <entropy>" or "entropy <twelve words>" - a password
   // one word here; what is unlocked is shown as its JSON.
   async run( input )
   {
      var it = this.constructor;
      var words = String( input || "" ).trim( ).split( /\s+/ );
      var what = ( words.shift( ) || "" ).toLowerCase( );

      if( ( what === "lock" ) && ( words.length === 2 ) )
         return it.lock( words[ 0 ], { entropy: words[ 1 ] } );

      if( ( what === "unlock" ) && ( words.length === 2 ) )
         return JSON.stringify( await it.unlock( words[ 0 ], words[ 1 ] ) );

      if( ( what === "words" ) && ( words.length === 1 ) )
         return it.words_of_entropy( words[ 0 ] );

      if( ( what === "entropy" ) && ( words.length >= 12 ) )
         return it.entropy_of_words( words.join( " " ) );

      throw new Error( "Use one of - " + it.about.example );
   }
} );

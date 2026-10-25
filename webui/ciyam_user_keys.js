// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: An account's own keys as the node keeps them ("user-keys", Ian 2026-10-22) - a module ("Modules" in "ciyam.js"). The
// keys come from the account's secret entropy - its twelve words, made as a node's are - which is never kept on the node: it
// is typed (or scanned from a device already signed in) on each device. What the node keeps is what the words cannot make
// again - the public keys of others, scanned face to face - locked under the words, so the node cannot open it (Ian,
// 2026-10-25: "perhaps it's reasonable enough to just use the twelve words to encrypt them"). The words are 128 bits or more,
// so there is nothing to harden, and a password changed or forgotten leaves them as they are. Kept as:
//
//    UK2:<salt>:<ciphertext>:<hmac>
//
// the salt (16 random bytes) and the ciphertext in hex. The key is the SHA-256 of the salt, "user_keys" and the entropy, in
// hex; the ciphertext "crypt -cc" (ChaCha20) under it, and the HMAC HMAC-SHA256 of the ciphertext's hex keyed with "mac" and
// the key, in full. Locking again picks a new salt, so no two lockings share a key. ("UK1", locked under a passphrase, was
// never used.)
//
// Needs "ciyam_chacha20.js" and "ciyam_secure_message.js" (for its HMAC) loaded too, and "bip39.min.js" for the words.
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_user_keys",
      version: "2",
      description: "an account's own keys - locked under its twelve words before they go to the node, and opened again",
      example: "lock <entropy> | unlock <entropy> <locked> | words <entropy> | entropy <twelve words>",
      hint: "(i.e. others' public keys, locked under the secret entropy so the node cannot read them)",
      needs: [ "sha256", "chacha20", "secure_message", "bip39" ]
   };

   static c_format = "UK2";
   static c_salt_bytes = 16;
   static c_label = "user_keys";

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

   static checked_entropy( entropy )
   {
      var text = String( entropy || "" ).toLowerCase( );

      if( !/^([0-9a-f]{2}){16,32}$/.test( text ) )
         throw new Error( "Secret entropy is 32 to 64 hex digits - whole bytes." );

      return text;
   }

   // NOTE: The twelve words for entropy, and back - BIP39, as a node's are made and recovered.
   static words_of_entropy( entropy )
   {
      return BIP39.entropyToMnemonic( this.checked_entropy( entropy ) );
   }

   static entropy_of_words( words )
   {
      var text = String( words || "" ).trim( ).toLowerCase( ).split( /\s+/ ).join( " " );

      if( !BIP39.validateMnemonic( text ) )
         throw new Error( "Those are not the words - check each is spelt right and in its place." );

      return BIP39.mnemonicToEntropy( text );
   }

   // NOTE: The key for a salt - one SHA-256, as the entropy needs no hardening.
   static async key_for( entropy, salt )
   {
      return ciyam_sha256_hex( salt + this.c_label + this.checked_entropy( entropy ) );
   }

   // NOTE: What is kept - checked, so nothing that could not be used is locked: others' public keys by name.
   static contents_text( contents )
   {
      var given = ( contents || { } ).contacts || { };
      var contacts = { };

      Object.keys( given ).sort( ).forEach( function( name )
      {
         var key = String( given[ name ] || "" ).toLowerCase( );

         if( ( name === "" ) || !/^[0-9a-f]{64}$/.test( key ) )
            throw new Error( "A public key is 64 hex digits, kept by a name." );

         contacts[ name ] = key;
      } );

      return JSON.stringify( { contacts: contacts } );
   }

   // NOTE: Locked under the entropy - "options" fixes the salt, for a known answer; a new salt otherwise.
   static async lock( entropy, contents, options )
   {
      var opts = options || { };

      var text = this.contents_text( contents );

      var salt = ( opts.salt !== undefined ) ? String( opts.salt )
       : this.hex_of( crypto.getRandomValues( new Uint8Array( this.c_salt_bytes ) ) );

      if( !/^[0-9a-f]{32}$/.test( salt ) )
         throw new Error( "A salt is 32 lower-case hex digits." );

      var key = await this.key_for( entropy, salt );
      var chacha = this.module_named( "chacha20" );

      var ciphertext = this.hex_of( await chacha.crypt( key, new TextEncoder( ).encode( text ) ) );
      var hmac = await this.module_named( "secure_message" ).hmac_sha256( "mac" + key, ciphertext );

      return [ this.c_format, salt, ciphertext, hmac ].join( ":" );
   }

   // NOTE: The contents, or an error: not locked keys, or the words not the ones they were locked with (or they were changed).
   static async unlock( entropy, locked )
   {
      var parts = String( locked || "" ).trim( ).split( ":" );

      if( ( parts.length !== 4 ) || ( parts[ 0 ] !== this.c_format ) || !/^[0-9a-f]{32}$/.test( parts[ 1 ] )
       || !/^([0-9a-f]{2})+$/.test( parts[ 2 ] ) || !/^[0-9a-f]{64}$/.test( parts[ 3 ] ) )
         throw new Error( "Those are not locked keys." );

      var key = await this.key_for( entropy, parts[ 1 ] );
      var expected = await this.module_named( "secure_message" ).hmac_sha256( "mac" + key, parts[ 2 ] );

      // NOTE: Every digit compared, so the time taken says nothing of where they differ.
      var differ = 0;

      for( var i = 0; i < expected.length; i++ )
         differ |= expected.charCodeAt( i ) ^ parts[ 3 ].charCodeAt( i );

      if( differ )
         throw new Error( "Those words do not open these keys." );

      var chacha = this.module_named( "chacha20" );
      var contents = JSON.parse( new TextDecoder( ).decode( await chacha.crypt( key, chacha.bytes_of( parts[ 2 ] ) ) ) );

      return { contacts: contents.contacts || { } };
   }

   // NOTE: "lock <entropy>" (nothing kept yet), "unlock <entropy> <locked>", "words <entropy>" or "entropy <twelve words>" - what
   // is unlocked is shown as its JSON.
   async run( input )
   {
      var it = this.constructor;
      var words = String( input || "" ).trim( ).split( /\s+/ );
      var what = ( words.shift( ) || "" ).toLowerCase( );

      if( ( what === "lock" ) && ( words.length === 1 ) )
         return it.lock( words[ 0 ], { contacts: { } } );

      if( ( what === "unlock" ) && ( words.length === 2 ) )
         return JSON.stringify( await it.unlock( words[ 0 ], words[ 1 ] ) );

      if( ( what === "words" ) && ( words.length === 1 ) )
         return it.words_of_entropy( words[ 0 ] );

      if( ( what === "entropy" ) && ( words.length >= 12 ) )
         return it.entropy_of_words( words.join( " " ) );

      throw new Error( "Use one of - " + it.about.example );
   }
} );

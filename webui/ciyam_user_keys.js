// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: An account's own keys as the node keeps them ("user-keys", Ian 2026-10-22) - a module ("Modules" in "ciyam.js"). The
// keys come from the account's secret entropy - its twelve words, made as a node's are - which is never kept anywhere but on
// paper (Ian, 2026-10-25: "encrypt them with a hash (or X hash rounds) of the user entropy (which can be safely stored in
// localStorage as it won't help anyone trying to crack the actual twelve words)"). From the entropy:
//
//    the access key - "harden"'s rounds of "user_keys" and the entropy - kept by each browser, and passed from one to another
//    the keys - the seeds "@secret" makes from the entropy and from it reversed, as "irc.cin" does - kept on the node, with
//    others' public keys, locked under the access key
//
// So a browser holds what opens the keys but not the words, and the words make both again. Kept as:
//
//    UK3:<rounds>:<salt>:<ciphertext>:<hmac>
//
// the rounds the access key was made with, in decimal (so typing the words on another browser makes the same); the salt (16
// random bytes) and the ciphertext in hex. The key for them is the SHA-256 of the salt, "user_keys" and the access key; the
// ciphertext "crypt -cc" (ChaCha20) under it, and the HMAC HMAC-SHA256 of the ciphertext's hex keyed with "mac" and the key, in
// full. Locking again picks a new salt. ("UK1", under a passphrase, and "UK2", under the entropy itself, were never used.)
//
// Needs "ciyam_harden.js", "ciyam_chacha20.js", "ciyam_ed25519.js" and "ciyam_secure_message.js" loaded too, and "bip39.min.js"
// for the words.
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_user_keys",
      version: "3",
      description: "an account's own keys - locked under a hash of its twelve words before they go to the node, and opened again",
      example: "access <entropy> | lock <entropy> | unlock <access key> <locked> | words <entropy> | entropy <twelve words>",
      hint: "(i.e. the keys and others' public keys, locked so the node cannot read them, opened by what a browser keeps)",
      needs: [ "sha256", "harden", "chacha20", "ed25519", "secure_message", "bip39" ]
   };

   static c_format = "UK3";
   static c_salt_bytes = 16;
   static c_label = "user_keys";

   // NOTE: About two thirds of a second in Chrome on a laptop, a few seconds on a phone - only when the words are typed or the
   // keys set up. Keys come back from the node, so it cannot ask for more than ten times as many - or it could keep a page
   // hashing for hours.
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

   static checked_entropy( entropy )
   {
      var text = String( entropy || "" ).toLowerCase( );

      if( !/^([0-9a-f]{2}){16,32}$/.test( text ) )
         throw new Error( "Secret entropy is 32 to 64 hex digits - whole bytes." );

      return text;
   }

   static checked_access( access_key )
   {
      var text = String( access_key || "" ).toLowerCase( );

      if( !/^[0-9a-f]{64}$/.test( text ) )
         throw new Error( "An access key is 64 hex digits." );

      return text;
   }

   static checked_rounds( rounds )
   {
      if( !Number.isInteger( rounds ) || ( rounds < 1 ) || ( rounds > this.c_most_rounds ) )
         throw new Error( "The rounds are a whole number from 1 to " + this.c_most_rounds + "." );

      return rounds;
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

   // NOTE: What a browser keeps - "harden"'s rounds of "user_keys" and the entropy, with the page's faster SHA-256.
   static async access_key( entropy, rounds )
   {
      var harden = this.module_named( "harden" );
      var hasher = ( typeof ciyam_sha256_fastest === "function" ) ? await ciyam_sha256_fastest( ) : { native: true, hash: ciyam_sha256_hex };
      var count = this.checked_rounds( ( rounds !== undefined ) ? rounds : this.c_default_rounds );

      return harden.rounds( hasher, count, this.c_label + this.checked_entropy( entropy ), null );
   }

   // NOTE: The keys from the entropy - the seed from it and from it reversed, in hex ("@secret", as "irc.cin" makes them).
   static seeds_of( entropy )
   {
      var ed = this.module_named( "ed25519" );
      var text = this.checked_entropy( entropy );

      return { primary: this.hex_of( ed.seed_of( text ) ), secondary: this.hex_of( ed.seed_of( text.split( "" ).reverse( ).join( "" ) ) ) };
   }

   // NOTE: What is kept - checked, so nothing that could not be used is locked: the two keys, and others' public keys by name.
   static contents_text( contents )
   {
      var given = ( contents || { } ).contacts || { };
      var contacts = { };

      [ "primary", "secondary" ].forEach( function( which )
      {
         if( !/^[0-9a-f]{64}$/.test( String( ( contents || { } )[ which ] || "" ) ) )
            throw new Error( "The keys are two of 64 hex digits." );
      } );

      Object.keys( given ).sort( ).forEach( function( name )
      {
         var key = String( given[ name ] || "" ).toLowerCase( );

         if( ( name === "" ) || !/^[0-9a-f]{64}$/.test( key ) )
            throw new Error( "A public key is 64 hex digits, kept by a name." );

         contacts[ name ] = key;
      } );

      return JSON.stringify( { primary: contents.primary, secondary: contents.secondary, contacts: contacts } );
   }

   static async key_for( access_key, salt )
   {
      return ciyam_sha256_hex( salt + this.c_label + this.checked_access( access_key ) );
   }

   // NOTE: Locked under the access key - made with "options.rounds" (kept with them). "options" fixes the salt too, for a known
   // answer; a new salt otherwise.
   static async lock( access_key, contents, options )
   {
      var opts = options || { };

      var text = this.contents_text( contents );
      var rounds = this.checked_rounds( ( opts.rounds !== undefined ) ? Number( opts.rounds ) : this.c_default_rounds );

      var salt = ( opts.salt !== undefined ) ? String( opts.salt )
       : this.hex_of( crypto.getRandomValues( new Uint8Array( this.c_salt_bytes ) ) );

      if( !/^[0-9a-f]{32}$/.test( salt ) )
         throw new Error( "A salt is 32 lower-case hex digits." );

      var key = await this.key_for( access_key, salt );
      var chacha = this.module_named( "chacha20" );

      var ciphertext = this.hex_of( await chacha.crypt( key, new TextEncoder( ).encode( text ) ) );
      var hmac = await this.module_named( "secure_message" ).hmac_sha256( "mac" + key, ciphertext );

      return [ this.c_format, String( rounds ), salt, ciphertext, hmac ].join( ":" );
   }

   // NOTE: The rounds locked keys were made with - so typing the words makes the same access key - or an error.
   static rounds_of( locked )
   {
      var parts = String( locked || "" ).trim( ).split( ":" );

      if( ( parts.length !== 5 ) || ( parts[ 0 ] !== this.c_format ) || !/^[1-9][0-9]{0,8}$/.test( parts[ 1 ] )
       || ( Number( parts[ 1 ] ) > this.c_most_rounds ) || !/^[0-9a-f]{32}$/.test( parts[ 2 ] )
       || !/^([0-9a-f]{2})+$/.test( parts[ 3 ] ) || !/^[0-9a-f]{64}$/.test( parts[ 4 ] ) )
         throw new Error( "Those are not locked keys." );

      return Number( parts[ 1 ] );
   }

   // NOTE: The contents, or an error: not locked keys, or not the access key they were locked with (or they were changed).
   static async unlock( access_key, locked )
   {
      var rounds = this.rounds_of( locked );
      var parts = String( locked ).trim( ).split( ":" );

      var key = await this.key_for( access_key, parts[ 2 ] );
      var expected = await this.module_named( "secure_message" ).hmac_sha256( "mac" + key, parts[ 3 ] );

      // NOTE: Every digit compared, so the time taken says nothing of where they differ.
      var differ = 0;

      for( var i = 0; i < expected.length; i++ )
         differ |= expected.charCodeAt( i ) ^ parts[ 4 ].charCodeAt( i );

      if( differ )
         throw new Error( "That does not open these keys." );

      var chacha = this.module_named( "chacha20" );
      var contents = JSON.parse( new TextDecoder( ).decode( await chacha.crypt( key, chacha.bytes_of( parts[ 3 ] ) ) ) );

      return { primary: contents.primary, secondary: contents.secondary, contacts: contents.contacts || { }, rounds: rounds };
   }

   // NOTE: "access <entropy>", "lock <entropy>" (the keys from it, no one else's yet - the access key shown before them),
   // "unlock <access key> <locked>", "words <entropy>" or "entropy <twelve words>" - what is unlocked shown as its JSON.
   async run( input )
   {
      var it = this.constructor;
      var words = String( input || "" ).trim( ).split( /\s+/ );
      var what = ( words.shift( ) || "" ).toLowerCase( );

      if( ( what === "access" ) && ( words.length === 1 ) )
         return it.access_key( words[ 0 ] );

      if( ( what === "lock" ) && ( words.length === 1 ) )
      {
         var access = await it.access_key( words[ 0 ] );
         var seeds = it.seeds_of( words[ 0 ] );

         return access + " " + await it.lock( access, { primary: seeds.primary, secondary: seeds.secondary, contacts: { } } );
      }

      if( ( what === "unlock" ) && ( words.length === 2 ) )
         return JSON.stringify( await it.unlock( words[ 0 ], words[ 1 ] ) );

      if( ( what === "words" ) && ( words.length === 1 ) )
         return it.words_of_entropy( words[ 0 ] );

      if( ( what === "entropy" ) && ( words.length >= 12 ) )
         return it.entropy_of_words( words.join( " " ) );

      throw new Error( "Use one of - " + it.about.example );
   }
} );

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
// Guarded (Ian's suggestion, 2026-10-27) with a line before them, the node keeping both:
//
//    UK#:<rounds>:<check>
//
// the check the SHA-256 of "user_keys_check" and the entropy, hashed again once a round. The node hashes a new check on to the
// rounds it has, so it takes keys with fewer rounds - a new access key, which no browser that had the old one can make (a round
// of "harden" needs the entropy) - only from the words; and only the same guard otherwise, or the same check to erase them.
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
      example: "access <entropy> | lock <entropy> | unlock <access key> <locked> | guard <entropy> [<rounds>] | words <entropy>"
       + " | entropy <twelve words> | people <node entropy> | person <people key> <pin>",
      hint: "(i.e. the keys and others' public keys, locked so the node cannot read them, opened by what a browser keeps)",
      needs: [ "sha256", "harden", "chacha20", "ed25519", "secure_message", "bip39" ]
   };

   static c_format = "UK3";
   static c_salt_bytes = 16;
   static c_label = "user_keys";

   static c_guard_format = "UK#";
   static c_check_label = "user_keys_check";

   static c_people_label = "people_key:";
   static c_person_label = "person_entropy:";

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

   // NOTE: One set of twelve words for a node and everyone on it (Ian, 2026-11-09 and 2026-11-11) - the "people key", from the
   // node's entropy by a hash of its own: the node is given the entropy's SHA-256 (the same identity as the entropy, "cdd681cc"),
   // so it cannot make this.
   static async people_key( node_entropy )
   {
      return ciyam_sha256_hex( this.c_people_label + this.checked_entropy( node_entropy ) );
   }

   // NOTE: A person's entropy from the people key and their PIN (never given out twice - someone is disabled, not removed) - 32
   // hex digits, which then make their keys just as twelve words of their own would.
   static async person_entropy( people_key, pin )
   {
      var key = String( people_key || "" ).toLowerCase( );
      var text = String( pin || "" );

      if( !/^[0-9a-f]{64}$/.test( key ) )
         throw new Error( "A people key is 64 hex digits." );

      if( !/^[0-9]{5}$/.test( text ) )
         throw new Error( "A PIN is five digits." );

      return ( await ciyam_sha256_hex( this.c_person_label + key + ":" + text ) ).substr( 0, 32 );
   }

   // NOTE: What a browser keeps - "harden"'s rounds of "user_keys" and the entropy, with the page's faster SHA-256.
   static async access_key( entropy, rounds )
   {
      var harden = this.module_named( "harden" );
      var hasher = ( typeof ciyam_sha256_fastest === "function" ) ? await ciyam_sha256_fastest( ) : { native: true, hash: ciyam_sha256_hex };
      var count = this.checked_rounds( ( rounds !== undefined ) ? rounds : this.c_default_rounds );

      return harden.rounds( hasher, count, this.c_label + this.checked_entropy( entropy ), null );
   }

   // NOTE: The access key one round on from "access_key" - so one made with a round fewer (Ian, 2026-10-27: a new access key, a
   // round fewer each time) is seen to come from the same words as the one it replaces, without hashing them all again.
   static async next_access_key( entropy, access_key )
   {
      var harden = this.module_named( "harden" );
      var hasher = ( typeof ciyam_sha256_fastest === "function" ) ? await ciyam_sha256_fastest( ) : { native: true, hash: ciyam_sha256_hex };

      return harden.rounds( hasher, 1, this.c_label + this.checked_entropy( entropy ), this.checked_access( access_key ) );
   }

   // NOTE: The guard's line for keys locked with "rounds" - only the words make it.
   static async guard( entropy, rounds )
   {
      var hasher = ( typeof ciyam_sha256_fastest === "function" ) ? await ciyam_sha256_fastest( ) : { native: true, hash: ciyam_sha256_hex };
      var count = this.checked_rounds( ( rounds !== undefined ) ? Number( rounds ) : this.c_default_rounds );

      var check = await hasher.hash( this.c_check_label + this.checked_entropy( entropy ) );

      for( var i = 0; i < count; i++ )
         check = hasher.native ? await hasher.hash( check ) : hasher.hash( check );

      return this.c_guard_format + ":" + count + ":" + check;
   }

   // NOTE: What the node keeps, in its two parts - the guard's line ("" for keys without one) and the locked keys.
   static parts_of( kept )
   {
      var lines = String( kept || "" ).trim( ).split( /\r?\n/ );

      if( ( lines.length === 2 ) && ( lines[ 0 ].indexOf( this.c_guard_format + ":" ) === 0 ) )
      {
         if( !/^UK#:[1-9][0-9]{0,6}:[0-9a-f]{64}$/.test( lines[ 0 ] ) )
            throw new Error( "Those are not locked keys." );

         return { guard: lines[ 0 ], locked: lines[ 1 ] };
      }

      if( lines.length !== 1 )
         throw new Error( "Those are not locked keys." );

      return { guard: "", locked: lines[ 0 ] };
   }

   // NOTE: Keys to send - with their guard's line before them, when they have one.
   static kept_text( guard, locked )
   {
      return guard ? ( guard + "\n" + locked ) : locked;
   }

   // NOTE: The keys from the entropy - the seed from it and from it reversed, in hex ("@secret", as "irc.cin" makes them).
   static seeds_of( entropy )
   {
      var ed = this.module_named( "ed25519" );
      var text = this.checked_entropy( entropy );

      return { primary: this.hex_of( ed.seed_of( text ) ), secondary: this.hex_of( ed.seed_of( text.split( "" ).reverse( ).join( "" ) ) ) };
   }

   // NOTE: What is kept - checked, so nothing that could not be used is locked: the two keys, others' public keys by name, and
   // room keys by room (Ian, 2026-10-25: 32 hex digits from "irc.cin"'s "k", used as they are).
   static contents_text( contents )
   {
      var given = ( contents || { } ).contacts || { };
      var given_rooms = ( contents || { } ).rooms || { };
      var contacts = { };
      var rooms = { };

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

      Object.keys( given_rooms ).sort( ).forEach( function( room )
      {
         var key = String( given_rooms[ room ] || "" ).toLowerCase( );

         if( !/^[0-9]{7}$/.test( room ) || !/^([0-9a-f]{32}|[0-9a-f]{64})$/.test( key ) )
            throw new Error( "A room key is 32 or 64 hex digits, kept by its room's number." );

         rooms[ room ] = key;
      } );

      var text = { primary: contents.primary, secondary: contents.secondary, contacts: contacts };

      // NOTE: Only when there are some - so keys with none lock to just what they did before room keys.
      if( Object.keys( rooms ).length > 0 )
         text.rooms = rooms;

      return JSON.stringify( text );
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

   // NOTE: The rounds locked keys were made with - so typing the words makes the same access key - or an error. With or without
   // the guard's line, whose rounds must be theirs.
   static rounds_of( kept )
   {
      var both = this.parts_of( kept );
      var parts = both.locked.split( ":" );

      if( ( parts.length !== 5 ) || ( parts[ 0 ] !== this.c_format ) || !/^[1-9][0-9]{0,8}$/.test( parts[ 1 ] )
       || ( Number( parts[ 1 ] ) > this.c_most_rounds ) || !/^[0-9a-f]{32}$/.test( parts[ 2 ] )
       || !/^([0-9a-f]{2})+$/.test( parts[ 3 ] ) || !/^[0-9a-f]{64}$/.test( parts[ 4 ] )
       || ( ( both.guard !== "" ) && ( both.guard.split( ":" )[ 1 ] !== parts[ 1 ] ) ) )
         throw new Error( "Those are not locked keys." );

      return Number( parts[ 1 ] );
   }

   // NOTE: The contents, or an error: not locked keys, or not the access key they were locked with (or they were changed). The
   // guard's line comes back with them, to send again with any change.
   static async unlock( access_key, kept )
   {
      var rounds = this.rounds_of( kept );
      var guard = this.parts_of( kept ).guard;
      var parts = this.parts_of( kept ).locked.split( ":" );

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

      return { primary: contents.primary, secondary: contents.secondary, contacts: contents.contacts || { }, rooms: contents.rooms || { },
       rounds: rounds, guard: guard };
   }

   // NOTE: "access <entropy>", "lock <entropy>" (the keys from it, no one else's yet - the access key shown before them),
   // "unlock <access key> <locked>", "words <entropy>", "entropy <twelve words>", "people <node entropy>" or "person <people key>
   // <pin>" - what is unlocked shown as its JSON.
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

      if( ( what === "guard" ) && ( ( words.length === 1 ) || ( words.length === 2 ) ) )
         return it.guard( words[ 0 ], ( words.length === 2 ) ? Number( words[ 1 ] ) : undefined );

      if( ( what === "words" ) && ( words.length === 1 ) )
         return it.words_of_entropy( words[ 0 ] );

      if( ( what === "entropy" ) && ( words.length >= 12 ) )
         return it.entropy_of_words( words.join( " " ) );

      if( ( what === "people" ) && ( words.length === 1 ) )
         return it.people_key( words[ 0 ] );

      if( ( what === "person" ) && ( words.length === 2 ) )
         return it.person_entropy( words[ 0 ], words[ 1 ] );

      throw new Error( "Use one of - " + it.about.example );
   }
} );

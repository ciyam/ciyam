// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Twelve words from 32 hex digits of entropy, and back - or new random words - a module ("Modules" in "ciyam.js"), with
// "bip39.min.js". "convert( input )" is its core. A wrong input answers with itself and what was wrong, on the next line, as
// before - "(incorrect mnemonics)" or "(invalid entropy)" - so a list carries on.
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_bip39",
      version: "2",
      description: "twelve words from entropy and back, or new random words",
      example: "{@1}",
      hint_once: true,
      needs: [ "bip39" ]
   };

   static library( )
   {
      if( typeof BIP39 === "undefined" )
         throw new Error( "bip39 needs bip39.min.js" );

      return BIP39;
   }

   // NOTE: Nothing - new words; words (a space after the first) - their entropy; otherwise entropy - its words.
   static convert( input )
   {
      var words = this.library( );

      if( ( input === null ) || ( input === undefined ) || ( input === "" ) )
         return words.generateMnemonic( );

      if( input.indexOf( " " ) > 0 )
         return words.validateMnemonic( input ) ? words.mnemonicToEntropy( input ) : input + "\n(incorrect mnemonics)";

      return /^[0-9a-f]{32}$/i.test( input ) ? words.entropyToMnemonic( input ) : input + "\n(invalid entropy)";
   }

   // NOTE: Loading with nothing to convert answers with new words, and keeps them.
   loaded( )
   {
      var words = this.constructor.convert( null );

      return { result: words, output: words };
   }

   async run( input )
   {
      return this.constructor.convert( input );
   }
} );

var ciyam_script_bip39_result = null;

var ciyam_script_bip39_has_shown_hints = false;

async function ciyam_script_bip39_at_load( callback )
{
   console.log( "ciyam_script_bip39_at_load" );

   ciyam_script_bip39_result = BIP39.generateMnemonic( );

   var output = ciyam_script_bip39_result;

   if( include_script_usage_hints )
      output += "\nemploy javascript bip39 {@1}";

   callback( output );
}

async function ciyam_script_bip39_execute( callback, input )
{
   console.log( "ciyam_script_bip39_execute" );

   ciyam_script_bip39_result = input;

   if( ( input == null ) || ( input == "" ) )
      ciyam_script_bip39_result = BIP39.generateMnemonic( );
   else
   {
      var pos = input.indexOf( " " );

      if( pos > 0 )
      {
         if( !BIP39.validateMnemonic( input ) )
            ciyam_script_bip39_result = input + "\n(incorrect mnemonics)";
         else
            ciyam_script_bip39_result = BIP39.mnemonicToEntropy( input );
      }
      else
         ciyam_script_bip39_result = BIP39.entropyToMnemonic( input );
   }

   callback( ciyam_script_bip39_result );
}

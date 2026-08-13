var ciyam_script_bip39_result = null;

var ciyam_script_bip39_has_shown_hints = false;

async function ciyam_script_bip39_at_load( callback )
{
   console.log( "ciyam_script_bip39_at_load" );

   if( init_script_value != null )
      ciyam_script_bip39_execute( callback, init_script_value );
   else
   {
      ciyam_script_bip39_result = BIP39.generateMnemonic( );

      var output = ciyam_script_bip39_result;

      if( include_script_usage_hints )
      {
         ciyam_script_bip39_has_shown_hints = true;

         output += "\nemploy javascript bip39 {@1}";
      }

      callback( output );
   }
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
      {
         var is_valid_entropy = true;

         if( input.length != 32 )
            is_valid_entropy = false;
         else
         {
            for( i = 0; i < input.length; i++ )
            {
               var next = input.substr( i, 1 ).toLowerCase( );

               if( ( next < 0 ) || ( next > 9 ) )
               {
                  if( ( next < 'a' ) || ( next > 'f' ) )
                  {
                     is_valid_entropy = false;
                     break;
                  }
               }
            }
         }

         if( !is_valid_entropy )
            ciyam_script_bip39_result = input + "\n(invalid entropy)";
         else
            ciyam_script_bip39_result = BIP39.entropyToMnemonic( input );
      }
   }

   callback( ciyam_script_bip39_result );
}

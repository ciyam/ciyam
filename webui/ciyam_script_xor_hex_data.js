var ciyam_script_xor_hex_data_result = null;

var ciyam_script_xor_hex_data_has_shown_hints = false;

function ciyam_script_xor_hex_data_xor_next( input )
{
   var hex_result = '';

   var hex_current = ciyam_script_xor_hex_data_result;
   var hex_implant = input;

   if( ( hex_current == null ) || ( hex_current == "" ) )
      hex_current = "00";
   else if( ( hex_current.length % 2 ) != 0  )
      hex_current += "0";

   if( ( hex_implant == null ) || ( hex_implant == "" ) )
      hex_implant = "00";
   else if( ( hex_implant.length % 2 ) != 0  )
      hex_implant += "0";

   if( hex_current.length < hex_implant.length )
      hex_current = hex_current.padStart( hex_implant.length, "0" );
   else if( hex_implant.length < hex_current.length )
      hex_implant = hex_implant.padStart( hex_current.length, "0" );

   for( i = 0; i < hex_current.length; i += 2 )
   {
      var new_hex = hex_implant.charAt( i );
      new_hex += hex_implant.charAt( i + 1 );

      var new_val = parseInt( new_hex, 16 );

      var old_hex = hex_current.charAt( i );
      old_hex += hex_current.charAt( i + 1 );

      var old_val = parseInt( old_hex, 16 );

      var new_val = ( old_val ^ new_val );

      if( new_val < 16 )
         hex_result += '0';

      hex_result += new_val.toString( 16 );
   }

   return hex_result;
}

async function ciyam_script_xor_hex_data_at_load( callback )
{
   console.log( "ciyam_script_xor_hex_data_at_load" );

   if( init_script_value != null )
      ciyam_script_xor_hex_data_execute( callback, init_script_value );
   else
   {
      var output = "(xor_hex_data loaded)";

      if( include_script_usage_hints )
         output += "\nemploy javascript xor_hex_data 01020304";

      callback( output );
   }
}

async function ciyam_script_xor_hex_data_execute( callback, input )
{
   console.log( "ciyam_script_xor_hex_data_execute" );

   var output = ciyam_script_xor_hex_data_xor_next( input );

   ciyam_script_xor_hex_data_result = output;

   if( include_script_usage_hints && !ciyam_script_xor_hex_data_has_shown_hints )
   {
      ciyam_script_xor_hex_data_has_shown_hints = true;

      output += "\n(i.e. XORs the prior result with hex input)"
   }

   callback( output );
}

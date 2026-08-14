var ciyam_script_rpc_unlock_result = null;

var ciyam_script_rpc_unlock_has_shown_hints = false;

async function ciyam_script_rpc_unlock_at_load( callback )
{
   console.log( "ciyam_script_rpc_unlock_at_load" );

   if( init_script_value != null )
   {
      ciyam_script_rpc_unlock_result = null;

      ciyam_script_rpc_unlock_execute( callback, init_script_value );
   }
   else
   {
      var output = "(rpc_unlock loaded)";

      if( include_script_usage_hints )
         output += "\nemploy javascript rpc_unlock test";

      callback( output );
   }
}

async function ciyam_script_rpc_unlock_execute( callback, input )
{
   console.log( "ciyam_script_rpc_unlock_execute" );

   if( input == null )
      input = "";

   var unix_now = Math.floor( Date.now( ) / 1000 );
   var password_hash = hex_sha256( input ).toLowerCase( );
   var salted_password_hash = hex_sha256( unix_now + password_hash );

   var output = unix_now + "-" + salted_password_hash;

   if( document.getElementById( "test_image" ).innerHTML != null )
      document.getElementById( "test_image" ).innerHTML = "";

   new QRCode( document.getElementById( "test_image" ), output );

   if( include_script_usage_hints && !ciyam_script_rpc_unlock_has_shown_hints )
   {
      ciyam_script_rpc_unlock_has_shown_hints = true;

      output += "\n./ciyam_client -tls -rpc_unlock=" + output + " localhost:6666";
   }

   callback( output );
}

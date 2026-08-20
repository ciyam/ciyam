var ciyam_xxxxx_result = null;

var ciyam_xxxxx_has_shown_hints = false;

async function ciyam_xxxxx_at_load( callback )
{
   console.log( "ciyam_xxxxx_at_load" );

   ciyam_xxxxx_result = null;

   if( init_script_value != null )
      ciyam_xxxxx_execute( callback, init_script_value );
   else
   {
      var output = "(ciyam_xxxxx loaded)";

      // NOTE: This should be simplified.
      if( include_script_usage_hints )
         output += "\nemploy javascript xxxxx test";

      output = output.replaceAll( "ciyam_", "" );

      callback( output );
   }
}

async function ciyam_xxxxx_execute( callback, input )
{
   console.log( "ciyam_xxxxx_execute" );

   if( input == null )
      input = "";

   ciyam_xxxxx_result = input;

   var output = ciyam_xxxxx_result;

   if( include_script_usage_hints && !ciyam_xxxxx_has_shown_hints )
   {
      ciyam_xxxxx_has_shown_hints = true;

      output += "\n(i.e. will echo the provided argument)"
   }

   callback( output );
}

var ciyam_script_template_result = null;

var ciyam_script_template_has_shown_hints = false;

async function ciyam_script_template_at_load( callback )
{
   console.log( "ciyam_script_template_at_load" );

   if( init_script_value != null )
   {
      ciyam_script_template_result = null;

      ciyam_script_template_execute( callback, init_script_value );
   }
   else
   {
      var output = "(ciyam_script_template loaded)";

      // NOTE: This should be simplified.
      if( include_script_usage_hints )
         output += "\nemploy javascript ciyam_script_template test";

      output = output.replaceAll( "ciyam_script_", "" );

      callback( output );
   }
}

async function ciyam_script_template_execute( callback, input )
{
   console.log( "ciyam_script_template_execute" );

   if( input == null )
      input = "";

   ciyam_script_template_result = input;

   var output = ciyam_script_template_result;

   if( include_script_usage_hints && !ciyam_script_template_has_shown_hints )
   {
      ciyam_script_template_has_shown_hints = true;

      output += "\n(i.e. will echo the provided argument)"
   }

   callback( output );
}

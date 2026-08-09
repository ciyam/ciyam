var ciyam_script_template_result = null;

var ciyam_script_template_has_shown_hints = false;

async function ciyam_script_template_at_load( callback )
{
   console.log( "ciyam_script_template_at_load" );

   ciyam_script_template_result = "(ciyam_script_template loaded)";

   ciyam_script_template_result = ciyam_script_template_result.replace( "ciyam_script_", "" );

   var output = ciyam_script_template_result;

   if( include_script_usage_hints )
   {
      var extra = output;

      extra = extra.substr( 1, extra.length - 2 );

      extra = extra.replace( "loaded", "test" );

      output += "\nemploy javascript " + extra;
   }

   callback( output );
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

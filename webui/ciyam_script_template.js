var ciyam_script_template_result = null;

async function ciyam_script_template_at_load( callback )
{
   console.log( "ciyam_script_template_at_load" );

   ciyam_script_template_result = "ciyam_script_template loaded";
   ciyam_script_template_result = ciyam_script_template_result.substring( String( "ciyam_script_" ).length );

   callback( ciyam_script_template_result );
}

async function ciyam_script_template_execute( callback, input )
{
   console.log( "ciyam_script_template_execute" );

   ciyam_script_template_result = input;

   callback( ciyam_script_template_result );
}

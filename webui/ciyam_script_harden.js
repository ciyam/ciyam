var ciyam_script_harden_result = null;

var ciyam_script_harden_use_async = false;

var ciyam_script_harden_string_to_hash = "";

async function ciyam_script_harden_hash_string( message )
{
   const msg_buffer = new TextEncoder( ).encode( message );

   const hash_buffer = await crypto.subtle.digest( "SHA-256", msg_buffer );

   const hash_array = Array.from( new Uint8Array( hash_buffer ) );

   const hash_hex = hash_array.map( b => ( "00" + b.toString( 16 ) ).slice( -2 ) ).join( "" );

   return hash_hex.toLowerCase( );
}

async function ciyam_script_harden_hash_rounds( num, use_async, input, initial )
{
   var s = input;

   if( initial != null )
      s = initial;

   if( num == 0 )
   {
      if( !use_async )
         s = hex_sha256( input ).toLowerCase( );
      else
         s = await ciyam_script_harden_hash_string( input );
   }

   // NOTE: Perform "num" additional rounds.
   for( var i = 0; i < num; i++ )
   {
      if( !use_async )
         s = hex_sha256( s + input ).toLowerCase( );
      else
         s = await ciyam_script_harden_hash_string( s + input );
   }

   return s;
}

async function ciyam_script_harden_at_load( callback )
{
   console.log( "ciyam_script_harden_at_load" );

   var start_time = performance.now( );

   var initial_hash = hex_sha256( "test" ).toLowerCase( );

   var async_result = await ciyam_script_harden_hash_rounds( 10000, true, "test", initial_hash );

   var finish_time = performance.now( );

   var async_time = ( finish_time - start_time );

   start_time = performance.now( );

   var non_async_result = await ciyam_script_harden_hash_rounds( 10000, false, "test", initial_hash );

   finish_time = performance.now( );

   var non_async_time = ( finish_time - start_time );

   var result = async_result;

   if( result != non_async_result )
      result += " and " + non_async_result;

   if( async_time < non_async_time )
      ciyam_script_harden_use_async = true;

   console.log( "async_time = " + async_time
    + ", non_async_time = " + non_async_time + "\n./ciyam_command crypto_hash -x=10000 test\n" + result );

   if( ciyam_script_harden_use_async )
      callback( "(using async version)" );
   else
      callback( "(using non-async version)" );
}

async function ciyam_script_harden_execute( callback, input )
{
   console.log( "ciyam_script_harden_execute" );

   if( ( input == null ) || ( input == "" ) )
      ciyam_script_harden_result = "(ready)";
   else if( ciyam_script_harden_result == "(ready)" )
   {
      ciyam_script_harden_string_to_hash = input;

      ciyam_script_harden_result = await ciyam_script_harden_hash_rounds(
       0, ciyam_script_harden_use_async, ciyam_script_harden_string_to_hash );
   }
   else
      ciyam_script_harden_result = await ciyam_script_harden_hash_rounds( input,
       ciyam_script_harden_use_async, ciyam_script_harden_string_to_hash, ciyam_script_harden_result );

   callback( ciyam_script_harden_result );
}

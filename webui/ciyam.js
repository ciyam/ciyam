// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

const c_def_key_len = 11;

const c_var_name_access = "ACCESS";
const c_var_name_device = "DEVICE";
const c_var_name_hashed = "HASHED";
const c_var_name_sessid = "SESSID";
const c_var_name_unique = "UNIQUE";

const c_visible_ascii_chars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

class CIYAM
{
   constructor( )
   {
      console.log( "CIYAM (ctor)" );

      this.access = "";
      this.device = "";
      this.hashed = "";
      this.sessid = "";
      this.unique = "";

      this.var_map = new Map( );
   }

   static encode_base64( str )
   {
      const bytes = new TextEncoder( ).encode( str );

      var binary = '';

      for( var i = 0; i < bytes.length; i++ )
         binary += String.fromCharCode( bytes[ i ] );

      return btoa( binary );
   }

   static encode_base64_url( str )
   {
      return CIYAM.encode_base64( str ).replace( /\+/g, "-" ).replace( /\//g, "_" ).replace( /=+$/, "" );
   }

   static generate_base64_key( num )
   {
      if( num == null )
         num = c_def_key_len;

      var len = c_visible_ascii_chars.length - 1;

      var output = "";

      for( var i = 0; i < num; i++ )
         output += c_visible_ascii_chars.substr( Math.floor( Math.random( ) * len ), 1 );

      // NOTE: Swap some specials with control
      // characters to ensure that '-' and '_'
      // are not included in the output value.
      output = output.replaceAll( ">", "\r" );
      output = output.replaceAll( "?", "\t" );
      output = output.replaceAll( "~", "\v" );

      output = CIYAM.encode_base64_url( output );

      const regex = new RegExp( `(.{${5}})`, "g" );

      const char = "-";

      // NOTE: Put a hypen separator between every
      // group of five characters (and removes the
      // final character if it is a hyphen).
      output = output.replace( regex, `$1${char}` );

      if( output.slice( -1 ) == "-" )
         output = output.slice( 0, -1 );

      return output;
   }

   has_variable( name )
   {
      return this.var_map.has( name );
   }

   get_variable( name )
   {
      return this.var_map.get( name );
   }

   set_variable( name, value )
   {
      this.var_map.set( name, value );
   }

   remove_variable( name )
   {
      this.var_map.delete( name );
   }

   replace_variables( input, arbitrary )
   {
      var skip = false;
      var output = input;

      if( output == null )
         output = "";

      // NOTE: If is in the form of ".<string>" then will remove the
      // "." and skip performing any string replacement processing.
      if( ( output.length > 0 ) && ( output.substr( 0, 1 ) == "." ) )
      {
         skip = true;
         output = output.substring( 1 );
      }

      if( output == "" )
         skip = true;

      if( !skip && ( output.length > 0 ) )
      {
         this.var_map.forEach( function( value, key )
         {
            value = value.replaceAll( "{", "\x01" );
            value = value.replaceAll( "}", "\x02" );

            output = output.replaceAll( "{" + key + "}", value );
         } )

         output = output.replaceAll( "{" + c_var_name_access + "}", this.access );
         output = output.replaceAll( "{" + c_var_name_device + "}", this.device );
         output = output.replaceAll( "{" + c_var_name_hashed + "}", this.hashed );
         output = output.replaceAll( "{" + c_var_name_sessid + "}", this.sessid );
         output = output.replaceAll( "{" + c_var_name_unique + "}", this.unique );

         if( ( arbitrary != null ) && ( output.indexOf( "{@" ) >= 0 ) )
         {
            var value = arbitrary;

            var value_array = value.split( "\n" );

            output.replaceAll( "{@0}", "{@}" );

            if( output.indexOf( "{@}" ) >= 0 )
            {
               value = value.replaceAll( "{", "\x01" );
               value = value.replaceAll( "}", "\x02" );

               output = output.replaceAll( "{@}", value );
            }

            for( var i = 0; i < value_array.length; i++ )
            {
               var value = value_array[ i ];

               value = value.replaceAll( "{", "\x01" );
               value = value.replaceAll( "}", "\x02" );

               output = output.replaceAll( "{@" + ( i + 1 ) + "}", value );
            }
         }

         // NOTE: Any "{var}" expressions that had not been
         // replaced are now removed.
         output = output.replaceAll( / *\{[^\}]*\}*/g, "" );

         // NOTE: Any "{var}" expressions that were provided
         // by the variable values are now restored (so that
         // they can used in a later pass).
         output = output.replaceAll( "\x01", "{" );
         output = output.replaceAll( "\x02", "}" );
      }

      return output;
   }

   get_all_variables( )
   {
      var output = "";

      // NOTE: Copy then sort for output.
      const array = Array.from( this.var_map );

      const sorted = array.sort( ( a, b ) => a[ 0 ] > b[ 0 ] );

      const sorted_map = new Map( sorted );

      if( this.access != "" )
         output = c_var_name_access + " " + this.access;

      if( this.device != "" )
      {
         if( output != "" )
            output += "\n";

         output += c_var_name_device + " " + this.device;
      }

      if( this.hashed != "" )
      {
         if( output != "" )
            output += "\n";

         output += c_var_name_hashed + " " + this.hashed;
      }

      if( this.sessid != "" )
      {
         if( output != "" )
            output += "\n";

         output += c_var_name_sessid + " " + this.sessid;
      }

      if( this.unique != "" )
      {
         if( output != "" )
            output += "\n";

         output += c_var_name_unique + " " + this.unique;
      }

      sorted_map.forEach( function( value, key )
      {
         if( output != "" )
            output += "\n";

         output += key + " " + value;
      } )

      return output;
   }

   async fetch( url, request_type, callback )
   {
      console.log( request_type + " " + url );

      fetch( url, { method: request_type } )
      .then( response => response.text( ) )
      .then( data => callback( data ) )
      .catch( error => console.error( "Error fetching data: ", error ) );
   }
}

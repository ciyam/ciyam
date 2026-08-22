// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

const c_def_key_len = 11;
const c_sess_id_len = 20;

const c_var_name_access = "ACCESS";
const c_var_name_device = "DEVICE";
const c_var_name_hashed = "HASHED";
const c_var_name_sessid = "SESSID";
const c_var_name_unique = "UNIQUE";

const c_format_type_json = "json";
const c_format_type_text = "text";

const c_visible_ascii_chars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

class CIYAM
{
   constructor( host_info, use_json )
   {
      console.log( "CIYAM [" + host_info + "]" );

      this.access = "";
      this.device = "";
      this.hashed = "";
      this.sessid = "";
      this.unique = "";

      this.var_map = new Map( );

      this.host_info = host_info;

      if( use_json == true )
         this.format_type = c_format_type_json;
      else
         this.format_type = c_format_type_text;

      this.user_callback = null;
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

   get_cws_url( )
   {
      return this.host_info + "/cws";
   }

   get_echo_url( )
   {
      return this.host_info + "/echo";
   }

   get_upload_url( )
   {
      return this.host_info + "/upload";
   }

   get_ip_addr_url( )
   {
      return this.host_info + "/ip-addr";
   }

   get_storage_url( )
   {
      return this.host_info + "/storage";
   }

   get_version_url( )
   {
      return this.host_info + "/version";
   }

   get_unix_now_url( )
   {
      return this.host_info + "/unix-now";
   }

   get_post_limit_url( )
   {
      return this.host_info + "/post-limit";
   }

   get_query_parameters( )
   {
      return "?format=" + this.format_type;
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

   hash_combined( password )
   {
      if( typeof hex_sha256 !== "undefined" )
         return hex_sha256( this.access + password );
      else
      {
         const crypto = require( "crypto" );

         const hash = crypto.createHash( "sha256" );

         hash.update( this.access + password );

         return hash.digest( "hex" );
      }
   }

   determine_hashed( password )
   {
      const combined = this.hash_combined( password );

      if( typeof hex_sha256 !== "undefined" )
         this.hashed = hex_sha256( hex_sha256( combined ) + this.device );
      else
      {
         const crypto = require( "crypto" );

         const hash_1 = crypto.createHash( "sha256" );

         hash_1.update( combined );

         const hash_2 = crypto.createHash( "sha256" );

         hash_2.update( hash_1.digest( "hex" ) + this.device );

         this.hashed = hash_2.digest( "hex" );
      }
   }

   determine_sess_id( )
   {
      if( typeof hex_sha256 !== "undefined" )
         this.sessid = hex_sha256( this.hashed + this.unique ).substr( 0, c_sess_id_len );
      else
      {
         const crypto = require( "crypto" );

         const hash = crypto.createHash( "sha256" );

         hash.update( this.hashed + this.unique );

         this.sessid = hash.digest( "hex" ).substr( 0, c_sess_id_len );
      }
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

   remove_all_variables( )
   {
      this.var_map.clear( );
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

         output += c_var_name_hashed + " " + this.hashed.substr( 0, 15 ) + "...";
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

   async post( url, text, callback )
   {
      console.log( "POST " + url + " ==> " + text );

      if( this.format_type != c_format_type_json )
         await fetch( url, { method: "POST", headers: { "Content-Type": "text/plain" }, body: text } )
          .then( response => response.text( ) )
          .then( data => callback( data ) )
          .catch( error => console.error( "Error fetching data: ", error ) );
      else
         await fetch( url, { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify( text ) } )
          .then( response => response.text( ) )
          .then( data => callback( data ) )
          .catch( error => console.error( "Error fetching data: ", error ) );
   }

   async fetch( url, request_type, callback )
   {
      console.log( request_type + " " + url );

      await fetch( url, { method: request_type } )
       .then( response => response.text( ) )
       .then( data => callback( data ) )
       .catch( error => console.error( "Error fetching data: ", error ) );
   }

   at_connect( response )
   {
      const obj = JSON.parse( response );

      if( obj.error == null )
      {
         if( this.device == "" )
            this.device = obj.new_device;
         else if( this.unique == "" )
            this.unique = obj.unique;
      }

      if( this.user_callback != null )
         this.user_callback( response );
   }

   async connect( access, password, callback )
   {
      if( this.sessid != "" )
         callback( "Error: Current session still exists." );
      else
      {
         this.access = access;

         this.user_callback = callback;

         var url = "";

         if( this.device == "" )
         {
            url = this.get_cws_url( )
             + "/devices?access=" + this.access + "&format=" + this.format_type;

            await this.fetch( url, "POST", this.at_connect.bind( this ) );
         }

         if( this.unique == "" )
         {
            url = this.get_cws_url( )
             + "/sessions?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

            await this.fetch( url, "POST", this.at_connect.bind( this ) );
         }

         if( this.unique != "" )
         {
            this.determine_hashed( password );

            this.determine_sess_id( );

            url = this.get_cws_url( )
             + "/status?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type + "&session=" + this.sessid;

           await this.fetch( url, "GET", callback );
         }
      }
   }

   at_disconnect( response )
   {
      if( this.user_callback != null )
         this.user_callback( response );

      if( response.indexOf( "Error:" ) != 0 )
      {
         this.sessid = "";
         this.unique = "";

         this.remove_all_variables( );
      }
   }

   async disconnect( callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         this.user_callback = callback;

         var url = this.get_cws_url( )
          + "/sessions/" + this.sessid + "?access=" + this.access
          + "&device=" + this.device + "&format=" + this.format_type;

         await this.fetch( url, "DELETE", this.at_disconnect.bind( this ) );
      }
   }
}

async function ciyam_test( params )
{
   console.log( "ciyam_test" );

   var ciyam = new CIYAM( "http://localhost:13031", true );

   console.log( ciyam.get_cws_url( ) );

   await ciyam.fetch( ciyam.get_version_url( ) + ciyam.get_query_parameters( ), "GET", console.log );

   await ciyam.post( ciyam.get_echo_url( ) + ciyam.get_query_parameters( ), "testing...", console.log );

   await ciyam.connect( "11111", "none", console.log );

   await ciyam.disconnect( console.log );
}

if( typeof process !== "undefined" )
{

   if( process.argv[ 2 ] === "test" )
      ciyam_test( "" );
}

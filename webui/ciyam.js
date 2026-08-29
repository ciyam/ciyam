// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

const c_admin = "admin";

const c_def_key_len = 11;
const c_sess_id_len = 20;

const c_node_cmd_users = "users";

const c_cmd_verb_create = "create";
const c_cmd_verb_delete = "delete";
const c_cmd_verb_update = "update";

const c_var_name_access = "ACCESS";
const c_var_name_device = "DEVICE";
const c_var_name_hashed = "HASHED";
const c_var_name_sessid = "SESSID";
const c_var_name_unique = "UNIQUE";

const c_format_type_json = "json";
const c_format_type_text = "text";

const c_session_type_admin = "[adm]";
const c_session_type_standard = "[std]";

const c_visible_ascii_chars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

class CIYAM
{
   constructor( host_info, use_json, is_quiet )
   {
      if( is_quiet != true )
         console.log( "CIYAM [" + host_info + "]" );

      this.seed = "";

      this.debug = false;

      this.error = "";

      this.access = "";
      this.device = "";
      this.hashed = "";
      this.sessid = "";
      this.unique = "";

      this.var_map = new Map( );

      this.username = "";

      this.host_info = host_info;

      if( use_json == true )
         this.format_type = c_format_type_json;
      else
         this.format_type = c_format_type_text;

      this.is_admin = false;
      this.is_locked = false;

      this.lock_source = 0;

      this.node_crypto = null;

      this.user_callback = null;

      this.connect_status = "";

      this.users = [ ];
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

   get_system_url( )
   {
      return this.host_info + "/system";
   }

   get_upload_url( )
   {
      return this.host_info + "/upload";
   }

   get_ip_addr_url( )
   {
      return this.host_info + "/ip-addr";
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

   hash_combined( password, access )
   {
      if( access == null )
         access = this.access;

      if( typeof hex_sha256 !== "undefined" )
         return hex_sha256( access + password );
      else
      {
         const hash = this.node_crypto.createHash( "sha256" );

         hash.update( access + password );

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
         const hash_1 = this.node_crypto.createHash( "sha256" );

         hash_1.update( combined );

         const hash_2 = this.node_crypto.createHash( "sha256" );

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
         const hash = this.node_crypto.createHash( "sha256" );

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
      if( this.debug )
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
      if( this.debug )
         console.log( request_type + " " + url );

      await fetch( url, { method: request_type } )
       .then( response => response.text( ) )
       .then( data => callback( data ) )
       .catch( error => console.error( "Error fetching data: ", error ) );
   }

   at_connect( response )
   {
      if( this.debug && ( this.node_crypto == null ) )
         console.log( response );

      if( this.format_type == c_format_type_text )
      {
         if( response.indexOf( "[" ) == 0 )
            this.connect_status = response;
         else if( response.indexOf( "Error: " ) == 0 )
            this.error = response;
         else
         {
            if( this.device == "" )
               this.device = response;
            else if( this.unique == "" )
               this.unique = response;
         }
      }
      else
      {
         const obj = JSON.parse( response );

         if( obj.error == null )
         {
            if( obj.pin != null )
            {
               this.access = obj.pin;

               if( this.seed == "" )
                  this.seed = obj.seed;
            }
            else if( this.device == "" )
               this.device = obj.device;
            else if( this.unique == "" )
               this.unique = obj.unique;
            else if( obj.status != null )
               this.connect_status = obj.status;
         }
         else
            this.error = obj.error;
      }

      if( this.user_callback != null )
         this.user_callback( response );
   }

   async connect( access, device, hashed, passwd, callback, all_callbacks )
   {
      if( this.sessid != "" )
         callback( "Error: Current session still exists." );
      else
      {
         var extra = "";

         this.error = "";

         var pos = access.indexOf( ":" );

         if( pos > 0 )
         {
            extra = access.substr( pos + 1 );
            access = access.substr( 0, pos );
         }

         this.access = access;

         // NOTE: (see NOTE below)
         if( device.length >= 32 )
            this.seed = device;
         else
            this.device = device;

         this.hashed = hashed;

         if( all_callbacks == null )
            all_callbacks = false;

         if( all_callbacks )
            this.user_callback = callback;

         var url = "";

         if( this.device == "" )
         {
            url = this.get_cws_url( )
             + "/devices?access=" + this.access + "&format=" + this.format_type;

            await this.fetch( url, "POST", this.at_connect.bind( this ) );

            // NOTE: If "admin" or a seed is provided as "access" then
            // assumes that this is either a new system or the "admin"
            // password is being reset so provides "admin" credentials
            // along with the (optionally external) "seed" entropy.
            if( ( this.error == "" )
             && ( ( this.seed != "" ) || ( access != this.access ) ) )
            {
               if( access.length > 5 )
                  access = c_admin;
               else if( ( access != c_admin ) && ( this.seed != "" ) )
               {
                  var seed = this.seed;

                  // NOTE: For "non-admin" users a
                  // seed value is used to provide
                  // a "nominated" username (or is
                  // just "@none" for no nominated
                  // value). If no nominated value
                  // is supplied then "access" can
                  // provide a "username" by using
                  // "<pin>:<username>".
                  var pos = seed.indexOf( " " );

                  if( pos > 0 )
                     access = seed.substring( pos + 1 );
                  else if( extra != "" )
                     access = extra;

                  this.seed = "";
               }

               // NOTE: If nominated "username"
               // is prefixed with "?" then set
               // as an error (which the caller
               // could check so the nominated
               // username can be then handled
               // as a suggestion).
               if( access.indexOf( "?" ) == 0 )
                  this.error = access;
               else
               {
                  var credentials = access + ":" + this.hash_combined( passwd );

                  url = this.get_cws_url( ) + "/devices?access=" + this.access
                   + "&format=" + this.format_type + "&passwd=" + CIYAM.encode_base64_url( credentials );

                  if( this.seed != "" )
                     url += "&request=" + this.seed;

                  this.seed = "";

                  await this.fetch( url, "POST", this.at_connect.bind( this ) );
               }
            }
         }

         if( ( this.error == "" ) && ( this.unique == "" ) )
         {
            url = this.get_cws_url( )
             + "/sessions?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

            await this.fetch( url, "POST", this.at_connect.bind( this ) );
         }

         if( ( this.error == "" ) && ( this.unique != "" ) )
         {
            if( this.hashed == "" )
               this.determine_hashed( passwd );

            this.determine_sess_id( );

            url = this.get_cws_url( )
             + "/status?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type + "&session=" + this.sessid;

            await this.fetch( url, "GET", this.at_connect.bind( this ) );

            // NOTE: If the supplied callback has not been called (and this function was
            // not called via Node.js) then will call it now with the "connect_status".
            if( !all_callbacks && ( callback != null ) && ( this.node_crypto == null ) )
               callback( this.connect_status );

            if( this.connect_status != "" )
            {
               var session_info = this.connect_status;

               var pos = session_info.indexOf( "]" );

               if( pos > 0 )
               {
                  var session_type = session_info.substr( 0, pos + 1 );

                  if( session_type == c_session_type_admin )
                     this.is_admin = true;
                  else
                     this.is_admin = false;

                  session_info = session_info.substring( pos + 1 );

                  this.is_locked = false;
                  this.lock_source = 0;

                  if( session_info.length )
                  {
                     if( session_info.substr( 0, 1 ) == "-" )
                     {
                        this.is_locked = true;
                        this.lock_source = session_info.substr( 1, 1 );
                     }

                     pos = session_info.indexOf( "@" );

                     this.username = "";

                     if( pos >= 0 )
                        this.username = session_info.substring( pos + 1 );
                  }
               }
            }

            if( this.error != "" )
            {
               this.hashed = "";
               this.sessid = "";
            }
         }
      }
   }

   at_disconnect( response )
   {
      if( this.user_callback != null )
         this.user_callback( response );

      this.sessid = "";
      this.unique = "";

      this.connect_status = "";

      this.remove_all_variables( );
   }

   async disconnect( callback )
   {
      if( this.sessid == "" )
      {
         if( callback != null )
            callback( "Error: No current session exists." );
      }
      else
      {
         this.user_callback = callback;

         var url = this.get_cws_url( )
          + "/sessions/" + this.sessid + "?access=" + this.access
          + "&device=" + this.device + "&format=" + this.format_type;

         await this.fetch( url, "DELETE", this.at_disconnect.bind( this ) );
      }
   }

   at_fetch_users( response )
   {
      if( this.format_type == c_format_type_text )
      {
         if( this.user_callback != null )
            this.user_callback( response );
      }
      else
      {
         const obj = JSON.parse( response );

         if( obj.error == null )
         {
            this.users.length = 0;

            // NOTE: Expands the simple array of arrays response into
            // a "users" array with explicit "pin" and "name" values.
            if( obj.all_users != null )
            {
               for( var i = 0; i < obj.all_users.length; i++ )
                  this.users.push( { pin: obj.all_users[ i ][ 0 ], name: obj.all_users[ i ][ 1 ] } );
            }

            if( this.user_callback != null )
               this.user_callback( JSON.stringify( this.users, null, 2 ) );
         }
         else
            this.error = obj.error;
      }
   }

   async fetch_users( callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         this.user_callback = callback;

         var url = this.get_cws_url( )
          + "/users?access=" + this.access + "&device=" + this.device
          + "&format=" + this.format_type + "&session=" + this.sessid;

         await this.fetch( url, "GET", this.at_fetch_users.bind( this ) )
      }
   }

   async create_user( options, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         var url = this.get_cws_url( ) + "/users?access=" + this.access
          + "&device=" + this.device + "&format=" + this.format_type;

         if( ( options != null ) && ( options != "" ) )
            url += "&options=" + encodeURIComponent( options );

         url += "&session=" + this.sessid;

         await this.fetch( url, "POST", callback )
      }
   }

   async delete_user( access_pin, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         var url = this.get_cws_url( ) + "/users/" + access_pin
          + "?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

         url += "&session=" + this.sessid;

         await this.fetch( url, "DELETE", callback )
      }
   }

   async update_user( access_pin, options, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         var url = this.get_cws_url( ) + "/users/" + access_pin
          + "?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

         if( ( options != null ) && ( options != "" ) )
         {
            var pos = options.indexOf( "=" );

            if( pos > 0 )
            {
               var passwd = options.substring( pos + 1 );

               options = options.substr( 0, pos + 1 );

               options += this.hash_combined( passwd, access_pin );
            }

            url += "&options=" + encodeURIComponent( options );
         }

         url += "&session=" + this.sessid;

         await this.fetch( url, "PUT", callback )
      }
   }
}

async function ciyam_node( host, access, device, hashed, passwd, debug, quiet )
{
   if( debug )
      console.log( "ciyam_node" );

   var ciyam = new CIYAM( host, true, quiet );

   ciyam.node_crypto = require( "crypto" );

   if( debug )
   {
      ciyam.debug = debug;

      console.log( ciyam.get_cws_url( ) );
   }

   if( !quiet )
      await ciyam.fetch( ciyam.get_system_url( ) + ciyam.get_query_parameters( ), "GET", console.log );

   if( access == "" )
      await ciyam.post( ciyam.get_echo_url( ) + ciyam.get_query_parameters( ), "testing...", console.log );

   if( ( access != "" ) && ( ( hashed != "" ) || ( passwd != "" ) ) )
   {
      await ciyam.connect( access, device, hashed, passwd, console.log, debug );

      if( !quiet && ( ciyam.connect_status != "" ) )
      {
         console.log( ciyam.connect_status );

         console.log( "ciyam.username = " + ciyam.username );
         console.log( "ciyam.is_admin = " + ciyam.is_admin );
         console.log( "ciyam.is_locked = " + ciyam.is_locked );

         if( ciyam.is_locked )
            console.log( "ciyam.lock_source => " + ciyam.lock_source );
      }

      if( ciyam.error != "" )
         console.log( "Error: " + ciyam.error );
      else
      {
         var command = process.env.CIYAM_NODE_COMMAND;

         if( ( typeof command !== "undefined" ) )
         {
            if( command != "" )
            {
               if( debug )
                  console.log( "cmd: " + command );

               if( command == c_node_cmd_users )
                  await ciyam.fetch_users( console.log );
               else if( command.indexOf( c_node_cmd_users + " " ) == 0 )
               {
                  var record = "";
                  var cmd_args = "";

                  command = command.replace( c_node_cmd_users + " ", "" );

                  var pos = command.indexOf( " " );

                  if( pos > 0 )
                  {
                     cmd_args = command.substring( pos + 1 );
                     command = command.substr( 0, pos );

                     pos = cmd_args.indexOf( " " );

                     if( pos > 0 )
                     {
                        record = cmd_args.substr( 0, pos );
                        cmd_args = cmd_args.substring( pos + 1 );
                     }
                  }

                  if( command == c_cmd_verb_create )
                     await ciyam.create_user( cmd_args, console.log );
                  else if( command == c_cmd_verb_delete )
                     await ciyam.delete_user( cmd_args, console.log );
                  else if( command == c_cmd_verb_update )
                     await ciyam.update_user( record, cmd_args, console.log );
               }
            }
         }

         if( quiet )
            await ciyam.disconnect( );
         else
            await ciyam.disconnect( console.log );
      }
   }
}

if( typeof process !== "undefined" )
{
   var offset = 2;

   if( process.argv[ offset ] != null )
   {
      var host = "http://localhost:13031";

      var debug = false;

      if( process.argv[ offset ] == "-debug" )
      {
         ++offset;
         debug = true;
      }

      var quiet = false;

      if( process.argv[ offset ] == "-quiet" )
      {
         ++offset;
         quiet = true;
      }

      if( process.argv[ offset ] != "" )
         host = process.argv[ offset ];

      var access = "";
      var device = "";
      var hashed = "";
      var passwd = "";

      if( process.argv[ offset + 1 ] != null )
         access = process.argv[ offset + 1 ];

      if( process.argv[ offset + 2 ] != null )
         device = process.argv[ offset + 2 ];

      if( process.argv[ offset + 3 ] != null )
         hashed = process.argv[ offset + 3 ];

      if( process.argv[ offset + 4 ] != null )
         passwd = process.argv[ offset + 4 ];

      ciyam_node( host, access, device, hashed, passwd, debug, quiet );
   }
}

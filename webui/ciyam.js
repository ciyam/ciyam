// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

const c_admin = "admin";

const c_sess_id_len = 20;

const c_pin_access_len = 5;

const c_unlock_key_len = 11;
const c_secret_access_len = 12;

const c_home_room = "0000000";

const c_own_access = "***";

const c_response_okay = "[okay]";

const c_node_cmd_echo = "echo";
const c_node_cmd_users = "users";
const c_node_cmd_messages = "messages";
const c_node_cmd_unlock_keys = "unlock-keys";

const c_cmd_verb_create = "create";
const c_cmd_verb_delete = "delete";
const c_cmd_verb_employ = "employ";
const c_cmd_verb_review = "review";
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

// NOTE: How the PIN and password are hashed before they are sent (Ian, 2026-10-23: "hardening should actually take place
// _before_ sending it to the server"). Version 0 is one SHA-256, as sent until then - which the node keeps, so whoever runs
// one could guess a password at that speed. Version 1 hardens it here first: rounds of SHA-256 of what there is with the PIN
// and password after it, as "harden" does - a floor good on its own, and more for a shorter password (Ian: "only multiply for
// even more rounds for weaker (length in particular) passwords"). What a version does is fixed once used - a change is a new
// version - so a hash already kept goes on matching the one it was made with.
const c_password_hash_version = 1;
const c_password_hash_floor = 100000;

// NOTE: In a page, a pause every so many rounds, so it can redraw while hardening.
const c_password_hash_pause = 5000;

// NOTE: The version a PIN's password was last hashed with in this browser (Ian, 2026-10-23: a version in "localStorage").
const c_storage_hash_version_prefix = "cws.hash_version@";

// NOTE: A sign in refused with this is taken as a password hashed the other way, and tried once more - after the gap the
// server keeps between sign ins for a PIN.
const c_wrong_password_error = "not valid";
const c_sign_in_gap_ms = 3500;

// NOTE: How long switching an account over waits for the node before keeping the older way.
const c_switch_over_wait_ms = 20000;

const c_visible_ascii_chars = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

class CIYAM
{
   constructor( host_info, use_json, is_quiet )
   {
      if( is_quiet != true )
         console.log( "CIYAM [" + host_info + "]" );

      this.room = "";
      this.seed = "";

      this.test = false;
      this.debug = false;

      this.error = "";

      this.access = "";
      this.device = "";
      this.hashed = "";
      this.sessid = "";
      this.unique = "";

      this.var_map = new Map( );

      this.new_room = "";
      this.username = "";

      this.host_info = host_info;

      if( use_json == true )
         this.format_type = c_format_type_json;
      else
         this.format_type = c_format_type_text;

      this.is_admin = false;
      this.is_locked = false;

      this.user_token = "";

      this.lock_source = 0;

      this.node_crypto = null;

      // NOTE: The version of the hash this session's password was sent with ("connect( )").
      this.hash_version = c_password_hash_version;

      this.user_callback = null;

      this.connect_status = "";

      this.users = [ ];
      this.messages = [ ];
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
         num = c_unlock_key_len;

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

   // NOTE: Hashed as UTF-8, as Node's is - "sha2.min.js" given the text itself took each character's low byte, so a password
   // not plain ASCII set in a browser did not sign in from the command line (Ian, 2026-10-22: "please change the sign-in hash to
   // use UTF-8"). One set that way before needs setting again.
   // NOTE: The PIN and password as they are sent - hashed as "version", the current one if none is given ("password_hash( )").
   // Both kinds of hashing keep the UTF-8 of the text, as the server and Node do (Ian, 2026-10-22).
   async hash_combined( password, access, version )
   {
      if( access == null )
         access = this.access;

      if( version == null )
         version = c_password_hash_version;

      return password_hash( access, password, version, ( typeof hex_sha256 !== "undefined" ) ? null : this.node_crypto );
   }

   // NOTE: The hash a session is opened with, from the PIN and password as sent - one SHA-256 of its SHA-256 with the device.
   session_hash( combined )
   {
      if( typeof hex_sha256 !== "undefined" )
         return hex_sha256( hex_sha256( combined ) + this.device );
      else
      {
         const hash_1 = this.node_crypto.createHash( "sha256" );

         hash_1.update( combined );

         const hash_2 = this.node_crypto.createHash( "sha256" );

         hash_2.update( hash_1.digest( "hex" ) + this.device );

         return hash_2.digest( "hex" );
      }
   }

   async determine_hashed( password, version )
   {
      this.hashed = this.session_hash( await this.hash_combined( password, null, version ) );
   }

   // NOTE: The version a PIN's password was last hashed with in this browser. None kept for it: a browser keeping versions
   // for other PINs has signed in since there were versions, so the current one; one keeping things for the node but no
   // versions at all signed in before them, so the first (Ian, 2026-10-23) - and on the command line, the current one. A
   // wrong guess costs one more try ("connect( )").
   hash_version_for( access )
   {
      var version = c_password_hash_version;

      try
      {
         if( typeof localStorage !== "undefined" )
         {
            var kept = localStorage.getItem( c_storage_hash_version_prefix + String( access ).split( ":" )[ 0 ] );

            if( kept !== null )
            {
               if( /^[0-9]+$/.test( kept ) && ( Number( kept ) <= c_password_hash_version ) )
                  version = Number( kept );
            }
            else
            {
               var any_version = false;
               var any_kept = false;

               for( var i = 0; i < localStorage.length; i++ )
               {
                  var key = String( localStorage.key( i ) );

                  if( key.indexOf( c_storage_hash_version_prefix ) === 0 )
                     any_version = true;
                  else if( key.indexOf( "cws." ) === 0 )
                     any_kept = true;
               }

               if( any_kept && !any_version )
                  version = 0;
            }
         }
      }
      catch( e )
      {
      }

      return version;
   }

   keep_hash_version( access, version )
   {
      try
      {
         if( ( typeof localStorage !== "undefined" ) && ( String( access ).length == c_pin_access_len ) )
            localStorage.setItem( c_storage_hash_version_prefix + access, String( version ) );
      }
      catch( e )
      {
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
            var sep = response.indexOf( " " );

            if( sep > 0 )
            {
               this.access = response.substr( 0, sep );

               if( this.seed == "" )
                  this.seed = response.substring( sep + 1 );
            }
            else if( this.device == "" )
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

   // NOTE: Signing in, the password hashed as this browser last hashed it for the PIN ("hash_version_for( )"). Typed, and
   // refused as if wrong, it is tried once more the other way - for an account a browser has not seen, or one whose password
   // admin set - and if it was the older way, the password is hashed the current way and set at once, the session staying
   // open as a change of password leaves it (Ian, 2026-10-23: "could automatically change this after signing in"). A saved
   // hash ("hashed") is used as it is, and changes nothing - there is no password to hash again.
   //
   // Only a PIN signing in is guessed at: anything else - twelve words' entropy recovering a node, a secret, a PIN with a
   // username to claim it - is setting a password, so the current way. Nor is admin switched over: its hash is kept with its
   // access, not as a user's, so "update_user( )" cannot change it - recovering from the words sets it the current way
   // (found by review, 2026-10-23).
   async connect( access, device, hashed, passwd, callback, all_callbacks )
   {
      var typed = ( ( hashed == null ) || ( hashed === "" ) ) && ( passwd != null ) && ( passwd !== "" );
      var pin = String( access || "" );

      var guessed = typed && /^[0-9]+$/.test( pin ) && ( pin.length == c_pin_access_len );

      if( !typed )
         this.hash_version = this.hash_version_for( pin );

      if( !guessed || ( this.sessid != "" ) )
      {
         if( typed )
            this.hash_version = c_password_hash_version;

         return this.connect_once( access, device, hashed, passwd, callback, all_callbacks, c_password_hash_version );
      }

      var version = this.hash_version_for( pin );

      var heard = { called: false, response: null };

      var hold = function( response )
      {
         heard.called = true;
         heard.response = response;
      };

      await this.connect_once( access, device, hashed, passwd, all_callbacks ? callback : hold, all_callbacks, version );

      if( ( this.sessid == "" ) && ( this.error.indexOf( c_wrong_password_error ) >= 0 ) )
      {
         version = ( version == c_password_hash_version ) ? 0 : c_password_hash_version;

         this.error = "";
         this.unique = "";
         this.seed = "";

         heard.called = false;

         await new Promise( function( resolve ) { setTimeout( resolve, c_sign_in_gap_ms ); } );

         await this.connect_once( access, device, hashed, passwd, all_callbacks ? callback : hold, all_callbacks, version );
      }

      if( this.sessid != "" )
      {
         if( ( version < c_password_hash_version ) && !this.is_admin )
         {
            // NOTE: Hashed once, and set as it is - its version kept as soon as the node has it, and the session's hash
            // following. A reply that never comes ends the wait, the older way kept, rather than holding the sign in.
            var combined = await this.hash_combined( passwd, this.access, c_password_hash_version );
            var self = this;

            var answer = await Promise.race( [ new Promise( function( resolve )
            {
               self.update_user( self.access, "password=" + combined, function( response ) { resolve( String( response || "" ) ); }, true );
            } ), new Promise( function( resolve ) { setTimeout( function( ) { resolve( "Error: no answer" ); }, c_switch_over_wait_ms ); } ) ] );

            if( is_okay_response( answer ) )
            {
               version = c_password_hash_version;

               this.keep_hash_version( this.access, version );

               this.hashed = this.session_hash( combined );
            }
         }

         this.hash_version = version;

         this.keep_hash_version( this.access, version );
      }

      if( !all_callbacks && heard.called && ( callback != null ) )
         callback( heard.response );
   }

   async connect_once( access, device, hashed, passwd, callback, all_callbacks, version )
   {
      if( this.sessid != "" )
         callback( "Error: Current session still exists." );
      else
      {
         var extra = "";

         this.error = "";
         this.unique = "";

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

         var self = this;
         var combined = { access: null, value: "" };

         var combined_for = async function( )
         {
            if( combined.access !== self.access )
            {
               combined.value = await self.hash_combined( passwd, self.access, version );
               combined.access = self.access;
            }

            return combined.value;
         };

         if( all_callbacks == null )
            all_callbacks = false;

         if( all_callbacks )
            this.user_callback = callback;

         var url = "";

         var secret_access = false;

         if( access.length == c_secret_access_len )
            secret_access = true;

         if( !secret_access && ( this.seed == "" )
          && ( this.device != "" ) && ( access.length == c_pin_access_len ) )
         {
            url = this.get_cws_url( )
             + "/sessions?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

            await this.fetch( url, "POST", this.at_connect.bind( this ) );

            var seed = this.seed;

            var pos = seed.indexOf( " " );

            if( pos > 0 )
               extra = seed.substring( pos + 1 );
         }

         if( ( this.device == "" ) || ( extra != "" ) || ( access.length > c_pin_access_len ) )
         {
            if( secret_access || ( this.device == "" ) || ( extra == "" ) )
            {
               url = this.get_cws_url( ) + "/devices?access=" + this.access;

               if( this.device != "" )
                  url += "&device=" + this.device;

               url += "&format=" + this.format_type;

               await this.fetch( url, "POST", this.at_connect.bind( this ) );

               if( secret_access )
                  access = this.access;
            }

            // NOTE: If "admin" or a seed is provided as "access" then
            // assumes that this is either a new system or the "admin"
            // password is being reset so provides "admin" credentials
            // along with the (optionally external) "seed" entropy.
            if( ( this.error == "" )
             && ( ( extra != "" ) || ( this.seed != "" ) || ( access != this.access ) ) )
            {
               if( access.length > c_pin_access_len )
                  access = c_admin;
               else if( ( access != c_admin )
                && ( ( this.seed != "" ) || ( extra != "" ) ) )
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

                  if( ( extra != "" )
                   && ( access.substr( 0, 1 ) != "?" ) )
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
                  var credentials = access + ":" + await combined_for( );

                  if( this.device == "" )
                     url = this.get_cws_url( ) + "/devices?access=" + this.access;
                  else
                     url = this.get_cws_url( ) + "/sessions?access=" + this.access + "&device=" + this.device;

                  url +="&format=" + this.format_type + "&passwd=" + CIYAM.encode_base64_url( credentials );

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
               this.hashed = this.session_hash( await combined_for( ) );

            this.determine_sess_id( );

            url = this.get_cws_url( )
             + "/status?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type + "&session=" + this.sessid;

            await this.fetch( url, "GET", this.at_connect.bind( this ) );

            // NOTE: If the supplied callback has not been called (and this function was
            // not called via Node.js) then will call it now with the "connect_status".
            if( !all_callbacks && ( callback != null ) && ( this.node_crypto == null ) )
               callback( this.connect_status );

            if( this.connect_status == "" )
            {
               if( this.error == "" )
                  this.error = "invalid or missing connect status";
            }
            else
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

   at_create_user( response )
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
            if( obj.token != null )
               this.user_token = obj.token;
         }
         else
            this.error = obj.error;
      }
   }

   async create_user( options, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         this.user_callback = callback;

         this.user_token = "";

         var url = this.get_cws_url( ) + "/users?access=" + this.access
          + "&device=" + this.device + "&format=" + this.format_type;

         if( ( options != null ) && ( options != "" ) )
            url += "&options=" + encodeURIComponent( options );

         url += "&session=" + this.sessid;

         await this.fetch( url, "POST", this.at_create_user.bind( this ) )
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

   // NOTE: A password in "options" is hashed with the PIN before it is sent - unless "is_hashed", as "connect( )" switching an
   // account over gives it.
   async update_user( access_pin, options, callback, is_hashed )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         if( access_pin == c_own_access )
            access_pin = this.access;

         var url = this.get_cws_url( ) + "/users/" + access_pin
          + "?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

         if( ( options != null ) && ( options != "" ) )
         {
            var pos = options.indexOf( "=" );

            if( pos > 0 )
            {
               var passwd = options.substring( pos + 1 );

               options = options.substr( 0, pos + 1 );

               options += is_hashed ? passwd : await this.hash_combined( passwd, access_pin );
            }

            url += "&options=" + encodeURIComponent( options );
         }

         url += "&session=" + this.sessid;

         await this.fetch( url, "PUT", callback )
      }
   }

   at_fetch_messages( response )
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
            if( obj.new_room != null )
               this.new_room = obj.new_room;
            else if( obj.all_messages != null )
            {
               this.users.length = 0;
               this.messages.length = 0;

               for( var i = 0; i < obj.all_messages.length; i++ )
               {
                  if( i == 0 )
                  {
                     var all_users = obj.all_messages[ i ];

                     var user_entries = all_users.split( " " );

                     for( var j = 0; j < user_entries.length; j++ )
                     {
                        var user_info = user_entries[ j ];

                        var pos = user_info.indexOf( "+" );

                        if( pos > 0 )
                        {
                           var name = user_info.substr( 0, pos );
                           var count = user_info.substr( pos + 1 );

                           this.users.push( { name: name, count: count } );
                        }
                     }
                  }
                  else
                  {
                     var message = obj.all_messages[ i ];

                     var pos = message.indexOf( " " );

                     if( pos > 0 )
                     {
                        var when = message.substr( 0, pos );
                        var payload = message.substr( pos + 1 );

                        // NOTE: For test purposes just pad the array offset
                        // (so that output can be used in regression tests).
                        if( this.test )
                           when = String( i ).padStart( 13, "0" );

                        this.messages.push( { when: when, payload: payload } );
                     }
                  }
               }

               if( this.user_callback != null )
               {
                  this.user_callback( "{\"room\":\"" + this.room + "\"}" );
                  this.user_callback( JSON.stringify( this.users, null, 2 ) );
                  this.user_callback( JSON.stringify( this.messages, null, 2 ) );
               }
            }
         }
         else
            this.error = obj.error;
      }
   }

   async fetch_messages( room, options, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         this.user_callback = callback;

         var url = this.get_cws_url( ) + "/messages/" + room
          + "?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

         if( ( options != null ) && ( options != "" ) )
            url += "&options=" + encodeURIComponent( options );

         url += "&session=" + this.sessid;

         var old_room = this.room;

         this.room = room;

         await this.fetch( url, "GET", this.at_fetch_messages.bind( this ) );

         if( this.error != null )
            this.room = old_room;
      }
   }

   async create_message( room, options, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         this.user_callback = callback;

         var url = this.get_cws_url( ) + "/messages/" + room
          + "?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

         if( ( options != null ) && ( options != "" ) )
            url += "&options=" + encodeURIComponent( options );

         url += "&session=" + this.sessid;

         this.new_room = "";

         var old_room = this.room;

         this.room = room;

         await this.fetch( url, "POST", this.at_fetch_messages.bind( this ) );

         if( this.error != null )
            this.room = old_room;
      }
   }

   at_delete_message_room( response )
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
            if( ( this.user_callback != null )
             && ( obj.response != c_response_okay ) )
               this.user_callback( obj.response );
         }
         else
            this.error = obj.error;
      }
   }

   async delete_message_room( room, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         this.user_callback = callback;

         var url = this.get_cws_url( ) + "/messages/" + room
          + "?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

         url += "&session=" + this.sessid;

         await this.fetch( url, "DELETE", this.at_delete_message_room.bind( this ) );
      }
   }

   at_update_message_room( response )
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
            if( ( this.user_callback != null )
             && ( obj.response != c_response_okay ) )
               this.user_callback( obj.response );
         }
         else
            this.error = obj.error;
      }
   }

   async update_message_room( room, options, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         this.user_callback = callback;

         var url = this.get_cws_url( ) + "/messages/" + room
          + "?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

         if( ( options != null ) && ( options != "" ) )
            url += "&options=" + encodeURIComponent( options );

         url += "&session=" + this.sessid;

         await this.fetch( url, "PUT", this.at_update_message_room.bind( this ) );
      }
   }

   at_create_unlock_key( response )
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
            if( this.user_callback != null )
               this.user_callback( obj.unlock_key );
         }
         else
            this.error = obj.error;
      }
   }

   async create_unlock_key( options, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         this.user_callback = callback;

         var url = this.get_cws_url( ) + "/unlock-keys"
          + "?access=" + this.access + "&device=" + this.device + "&format=" + this.format_type;

         if( ( options != null ) && ( options != "" ) )
            url += "&options=" + encodeURIComponent( options );

         url += "&session=" + this.sessid;

         await this.fetch( url, "POST", this.at_create_unlock_key.bind( this ) );
      }
   }

   async employ_unlock_key( key, callback )
   {
      if( this.sessid == "" )
         callback( "Error: No current session exists." );
      else
      {
         var url = this.get_cws_url( ) + "/unlock-keys/" + key
          + "?access=" + this.access + "&device=" + this.device
          + "&format=" + this.format_type + "&session=" + this.sessid;

         await this.fetch( url, "POST", callback );
      }
   }
}

function ciyam_node_parse_command( info, command, cmd_info )
{
   info = info.replace( command + " ", "" );

   var pos = info.indexOf( " " );

   if( pos <= 0 )
      cmd_info.cmd = info;
   else
   {
      cmd_info.cmd = info.substr( 0, pos );
      cmd_info.args = info.substring( pos + 1 );

      pos = cmd_info.args.indexOf( " " );

      if( pos > 0 )
      {
         cmd_info.key = cmd_info.args.substr( 0, pos );
         cmd_info.args = cmd_info.args.substring( pos + 1 );
      }
      else if( cmd_info.args.indexOf( "=" ) <= 0 )
      {
         cmd_info.key = cmd_info.args;

         cmd_info.args = "";
      }
   }
}

async function ciyam_node( host, access, device, hashed, passwd, test, debug, quiet )
{
   if( debug )
      console.log( "ciyam_node" );

   var ciyam = new CIYAM( host, true, ( test || quiet ) );

   ciyam.node_crypto = require( "crypto" );

   if( test )
      ciyam.test = true;

   if( debug )
   {
      ciyam.debug = debug;

      console.log( ciyam.get_cws_url( ) );
   }

   if( !test && !quiet )
      await ciyam.fetch( ciyam.get_system_url( ) + ciyam.get_query_parameters( ), "GET", console.log );

   if( access == "" )
      await ciyam.post( ciyam.get_echo_url( ) + ciyam.get_query_parameters( ), "testing...", console.log );

   if( ( access != "" ) && ( ( hashed != "" ) || ( passwd != "" ) ) )
   {
      await ciyam.connect( access, device, hashed, passwd, console.log, debug );

      var command = "";

      if( typeof process.env.CIYAM_NODE_COMMAND !== "undefined" )
         command = process.env.CIYAM_NODE_COMMAND;

      if( test && ( device != ciyam.device ) )
      {
         if( command == "" )
            console.log( ciyam.device );
         else if( command == "credentials" )
         {
            command = "";

            console.log( ciyam.access + " " + ciyam.device + " " + ciyam.hashed );
         }
      }

      if( !test && !quiet && ( ciyam.connect_status != "" ) )
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
         if( command != "" )
         {
            var commands = [ ];

            var first = command.substr( 0, 1 );

            // NOTE: Multiple commands can be provided using the command
            // format "@<name>" which is split into lines with each line
            // being processed as a separate command.
            if( first == "@" )
            {
               var fs = require( 'fs' );

               try
               {
                  var data = fs.readFileSync( command.substring( 1 ), "utf8" );

                  var lines = data.split( "\n" );

                  for( let i = 0; i < lines.length; i++ )
                  {
                     if( lines[ i ] != "" )
                        commands.push( lines[ i ] );
                  }
               }
               catch( e )
               {
                  console.log( "Error: ", e.stack );
               }
            }
            else
               commands.push( command );

            for( let i = 0; i < commands.length; i++ )
            {
               command = commands[ i ];

               if( debug )
                  console.log( "cmd: " + command );

               var cmd_info = { cmd: command, key: "", args: "" };

               if( command == c_node_cmd_echo )
                  console.log( "" );
               else if( command.indexOf( c_node_cmd_echo + " " ) == 0 )
                  console.log( command.substring( 5 ) );
               else if( command == c_node_cmd_users )
                  await ciyam.fetch_users( console.log );
               else if( command.indexOf( c_node_cmd_users + " " ) == 0 )
               {
                  ciyam_node_parse_command( command, c_node_cmd_users, cmd_info );

                  if( cmd_info.cmd == c_cmd_verb_create )
                  {
                     await ciyam.create_user( cmd_info.args, console.log );

                     if( test && ( ciyam.user_token != "" ) )
                        console.log( ciyam.user_token );
                  }
                  else if( cmd_info.cmd == c_cmd_verb_delete )
                     await ciyam.delete_user( cmd_info.key, console.log );
                  else if( cmd_info.cmd == c_cmd_verb_update )
                     await ciyam.update_user( cmd_info.key, cmd_info.args, console.log );
               }
               else if( command == c_node_cmd_messages )
                  await ciyam.fetch_messages( c_home_room, "", console.log );
               else if( command.indexOf( c_node_cmd_messages + " " ) == 0 )
               {
                  ciyam_node_parse_command( command, c_node_cmd_messages, cmd_info );

                  if( cmd_info.cmd == c_cmd_verb_create )
                  {
                     await ciyam.create_message( cmd_info.key, cmd_info.args, console.log );

                     if( test && ( ciyam.new_room != "" ) )
                        console.log( ciyam.new_room );
                  }
                  else if( cmd_info.cmd == c_cmd_verb_delete )
                     await ciyam.delete_message_room( cmd_info.key, console.log );
                  else if( cmd_info.cmd == c_cmd_verb_review )
                     await ciyam.fetch_messages( cmd_info.key, cmd_info.args, console.log );
                  else if( cmd_info.cmd == c_cmd_verb_update )
                     await ciyam.update_message_room( cmd_info.key, cmd_info.args, console.log );
               }
               else if( command.indexOf( c_node_cmd_unlock_keys + " " ) == 0 )
               {
                  ciyam_node_parse_command( command, c_node_cmd_unlock_keys, cmd_info );

                  if( cmd_info.cmd == c_cmd_verb_create )
                     await ciyam.create_unlock_key( cmd_info.args, console.log );
                  else if( cmd_info.cmd == c_cmd_verb_employ )
                     await ciyam.employ_unlock_key( cmd_info.key, console.log );
               }

               if( ciyam.error != "" )
               {
                  console.log( "Error: " + ciyam.error );
                  ciyam.error = "";
               }
            }
         }
      }

      if( ciyam.sessid != "" )
      {
         if( test || quiet )
            await ciyam.disconnect( );
         else
            await ciyam.disconnect( console.log );
      }
   }
}

// ====================================================================
// Modules - "ciyam_<name>.js" plug-in algorithms (Damon, 2026-10-16; agreed with Ian - "JavaScript Modules Design")
// ====================================================================
//
// A module is a class - "about" says what it is, "run( input )" answers - registered with "ciyam_register_module( )". Its
// file is that one call, holding a class expression, so loading it again does not fail as a second "class" would:
//
//    ciyam_register_module( class extends ciyam_module
//    {
//       static about = { name: "ciyam_echo", version: "1", description: "echoes its input", example: "test",
//        hint: "(i.e. will echo the provided argument)", needs: [ ] };
//
//       async run( input )
//       {
//          return input;
//       }
//    } );
//
// "about.name" is written as the file is named - "ciyam_echo" for "ciyam_echo.js" - so the server's renaming of a module
// opened to become an account's own ("ciyam_<name>" to "ciyam_<PIN>") renames it too; the registry knows it as "echo".
//
// A page uses one through "ciyam_module_named( name )" - its class, for its own functions and "new" - and the console and Ian's
// harness through the "ciyam_<name>_at_load", "_execute" and "_result" globals registering makes ("ciyam_module_legacy( )"),
// so the lists that "wait ciyam_<name>_result" keep working. What a module needs from the page - progress, a QR code drawn,
// hashing - comes in its "context" ("ciyam_module_page_context( )"), not from page globals.

var ciyam_registered_modules = { };

// NOTE: Each module run through the old globals, by its short name - what stops it ("ciyam_module_stop( )").
var ciyam_module_stoppers = { };

// NOTE: The base every module extends. "run( input )" resolves to the result, or to { result, output } when what is said
// differs from what is kept (the harness's "(hash primed)"); an error is thrown. "loaded( )" is what loading with no input
// says.
class ciyam_module
{
   constructor( context )
   {
      this.context = context || { };
   }

   loaded( )
   {
      return "(" + ciyam_module_short_name( this.constructor ) + " loaded)";
   }

   async run( input )
   {
      throw new Error( ciyam_module_short_name( this.constructor ) + " does not run" );
   }
}

// NOTE: "ciyam_xor_hex_data" as "xor_hex_data" - the name the console, the lists and "ciyam_module_named( )" use.
function ciyam_module_short_name( module_class )
{
   return String( module_class.about.name ).substring( "ciyam_".length );
}

function ciyam_register_module( module_class )
{
   var about = ( module_class && module_class.about ) || { };

   if( !/^ciyam_[A-Za-z0-9_]+$/.test( String( about.name || "" ) ) )
      throw new Error( "A module needs a name in its 'about', as its file is named - 'ciyam_' and letters, digits or '_'." );

   ciyam_registered_modules[ ciyam_module_short_name( module_class ) ] = module_class;

   if( typeof window !== "undefined" )
      ciyam_module_legacy( module_class, window );

   return module_class;
}

// NOTE: Stops a module run through the old globals - the page's Stop: what it is doing is let go ("stopped( )"), keeping
// nothing, and the next call starts afresh, as after loading it. False when there was nothing to stop.
function ciyam_module_stop( name )
{
   var stopper = ciyam_module_stoppers[ name ];

   return stopper ? stopper( ) : false;
}

// NOTE: A registered module's class, or null.
function ciyam_module_named( name )
{
   return ciyam_registered_modules[ name ] || null;
}

// NOTE: Out of the registry, and what it was doing let go - its last instance, with whatever it held (a primed "harden"'s
// password), dropped rather than kept until the module is loaded again (found by review, 2026-10-16).
function ciyam_unregister_module( name )
{
   var known = ( ciyam_registered_modules[ name ] !== undefined );

   ciyam_module_stop( name );

   delete ciyam_registered_modules[ name ];
   delete ciyam_module_stoppers[ name ];

   return known;
}

// NOTE: What a module says of itself, as lines - for the console's help.
function ciyam_module_about( name )
{
   var module_class = ciyam_module_named( name );

   if( module_class === null )
      return null;

   var about = module_class.about;
   var lines = [ name + ( about.version ? " " + about.version : "" ) + ( about.description ? " - " + about.description : "" ) ];

   if( about.example !== undefined )
      lines.push( "employ javascript " + name + " " + about.example );

   if( about.needs && about.needs.length )
      lines.push( "needs: " + about.needs.join( ", " ) );

   return lines;
}

// NOTE: Text as the character codes of its UTF-8 bytes - what "sha2.min.js" hashes, a code to a byte, as they are - so the
// library hashes text as the browser's own SHA-256 and the server do. Given the text itself, it took only the low byte of
// each character: "café" gave another hash over plain http than over https (found 2026-10-21).
function ciyam_utf8_binary( text )
{
   var s = String( text );

   if( /^[\x00-\x7f]*$/.test( s ) )
      return s;

   var bytes = new TextEncoder( ).encode( s );
   var out = "";

   for( var i = 0; i < bytes.length; i++ )
      out += String.fromCharCode( bytes[ i ] );

   return out;
}

// NOTE: Whether a reply says it was done - "[okay]" as text, or as JSON ("{"response":"[okay]"}", Node's default); an error in
// either, or anything else, is not (found by review, 2026-10-23 - a JSON error had been taken for success).
function is_okay_response( response )
{
   var text = String( response || "" ).trim( );

   if( text === c_response_okay )
      return true;

   if( text.charAt( 0 ) === "{" )
   {
      try
      {
         var obj = JSON.parse( text );

         return ( obj !== null ) && ( obj.error == null ) && ( obj.response === c_response_okay );
      }
      catch( e )
      {
         return false;
      }
   }

   return false;
}

// NOTE: Version 1's rounds for a password, by its length in UTF-8 bytes - the floor for twelve or more, twice it for nine to
// eleven, three times for fewer. Bytes, as a console script measures it ("@strlen"), so "init_web_access" can work the same
// out for admin (Ian, 2026-10-23).
function password_hash_rounds( password )
{
   var length = new TextEncoder( ).encode( String( password || "" ) ).length;

   if( length >= 12 )
      return c_password_hash_floor;
   else if( length >= 9 )
      return c_password_hash_floor * 2;
   else
      return c_password_hash_floor * 3;
}

// NOTE: The PIN and password hashed as "version", in lower-case hex - version 0 one SHA-256 of the two; version 1 that many
// rounds ("password_hash_rounds( )") - one SHA-256 of the two, then each round after hashing what there is with the two after
// it - as the server's "crypto_hash -x=<rounds - 1>" does, so a console script can make it too ("init_web_access" for admin,
// Ian 2026-10-23). In a page through "sha2.min.js", in step, pausing now and then so the page can redraw; with "node_crypto",
// Node's own SHA-256.
async function password_hash( access, password, version, node_crypto )
{
   var input = String( access ) + String( password );

   var sha = function( text )
   {
      if( node_crypto )
         return node_crypto.createHash( "sha256" ).update( text, "utf8" ).digest( "hex" );

      return hex_sha256( text ).toLowerCase( );
   };

   // NOTE: For "sha2.min.js" the text as its UTF-8 bytes, once - each round adds only hex to it.
   var text = node_crypto ? input : ciyam_utf8_binary( input );

   if( Number( version ) === 0 )
      return sha( text );

   if( Number( version ) !== 1 )
      throw new Error( "There is no password hash version " + version + "." );

   var rounds = password_hash_rounds( password );
   var s = sha( text );

   for( var i = 1; i < rounds; i++ )
   {
      s = sha( s + text );

      if( !node_crypto && ( ( i % c_password_hash_pause ) === ( c_password_hash_pause - 1 ) ) )
         await new Promise( function( resolve ) { setTimeout( resolve, 0 ); } );
   }

   return s;
}

// NOTE: SHA-256 of text, as lower-case hex - the browser's own where it has one (a secure context: https, or localhost),
// "sha2.min.js" otherwise. Decided once a page, for every module that hashes (Ian, 2026-10-09).
var ciyam_sha256_native = null;

async function ciyam_sha256_hex( text )
{
   if( ciyam_sha256_native === null )
      ciyam_sha256_native = ( typeof crypto !== "undefined" ) && !!crypto.subtle && ( typeof crypto.subtle.digest === "function" );

   if( ciyam_sha256_native )
   {
      var digest = await crypto.subtle.digest( "SHA-256", new TextEncoder( ).encode( String( text ) ) );

      return Array.from( new Uint8Array( digest ), function( b ) { return b.toString( 16 ).padStart( 2, "0" ); } ).join( "" );
   }

   if( typeof hex_sha256 === "function" )
      return hex_sha256( ciyam_utf8_binary( text ) ).toLowerCase( );

   throw new Error( "SHA-256 is not available - this page needs sha2.min.js, or https." );
}

// NOTE: The faster SHA-256 for many rounds - the browser's own, awaited each time, or "sha2.min.js", in step - timed once a
// page, as Ian's "harden" did on every load (Ian, 2026-10-09: "work out whether to use the browser's native SHA-256").
// Resolves to { native, hash } - "hash" in step when not native, a promise when it is.
//
// Each is run a while first, untimed, then timed on 5000 rounds: timed cold on 2000, the library - slow until the engine has
// compiled it - lost, and "harden" ran about 1.8 times slower awaiting the browser's own (found 2026-10-16,
// "module_speed_probe.js"; in Chrome on localhost the library is the faster).
var ciyam_sha256_choice = null;

function ciyam_sha256_fastest( )
{
   if( ciyam_sha256_choice === null )
      ciyam_sha256_choice = ciyam_sha256_time( );

   return ciyam_sha256_choice;
}

async function ciyam_sha256_time( )
{
   var has_native = ( typeof crypto !== "undefined" ) && !!crypto.subtle && ( typeof crypto.subtle.digest === "function" );
   var has_library = ( typeof hex_sha256 === "function" );

   var in_step = function( text ) { return hex_sha256( ciyam_utf8_binary( text ) ).toLowerCase( ); };

   if( !has_native || !has_library )
      return has_native ? { native: true, hash: ciyam_sha256_hex } : { native: false, hash: in_step };

   var now = ( typeof performance !== "undefined" ) ? function( ) { return performance.now( ); } : Date.now;
   var s = "test";

   for( var w = 0; w < 1000; w++ )
   {
      s = in_step( s + "test" );
      s = await ciyam_sha256_hex( s + "test" );
   }

   var start = now( );

   for( var i = 0; i < 5000; i++ )
      s = await ciyam_sha256_hex( s + "test" );

   var native_time = now( ) - start;

   start = now( );

   for( var j = 0; j < 5000; j++ )
      s = in_step( s + "test" );

   return ( native_time < ( now( ) - start ) ) ? { native: true, hash: ciyam_sha256_hex } : { native: false, hash: in_step };
}

// NOTE: What the page offers a module - its progress ("fraction" 0 to 1, null when done), a QR code drawn, hashing, other
// modules, whether usage hints are wanted, and "expose( key, value )" - a value kept where a list can "wait" on it, as
// "ciyam_<name>_<key>" ("harden"'s "reset"), for a module run through the old globals; nothing otherwise. Through the
// harness's own page functions and its "test_image" where the page has them, as the console does.
//
// "stopped( )" says when the page has let the module go - unloaded, a session ended, or loaded again - so a long run ends
// there, and nothing more reaches the page: what it worked out, a password's hash say, must not outlast the session that ran
// it (found by "console_javascripts", 2026-10-16 - "harden" kept running and answering after a sign out).
function ciyam_module_page_context( host, prefix, stopped )
{
   var page = host || ( ( typeof window !== "undefined" ) ? window : { } );
   var let_go = stopped || function( ) { return false; };

   return {
      stopped: let_go,
      hints: function( )
      {
         return !!page.include_script_usage_hints;
      },
      expose: function( key, value )
      {
         if( prefix && !let_go( ) )
            page[ prefix + "_" + key ] = value;
      },
      progress: function( fraction )
      {
         if( let_go( ) )
            return;

         if( fraction === null )
         {
            if( typeof page.hide_progress === "function" )
               page.hide_progress( );
         }
         else if( ( fraction === 0 ) && ( typeof page.show_progress === "function" ) )
            page.show_progress( );
         else if( typeof page.update_progress === "function" )
            page.update_progress( fraction );
      },
      show_qr: function( text )
      {
         var holder = ( typeof document !== "undefined" ) ? document.getElementById( "test_image" ) : null;

         if( ( holder === null ) || ( typeof QRCode !== "function" ) )
            return false;

         holder.textContent = "";

         new QRCode( holder, String( text ) );

         return true;
      },
      sha256: ciyam_sha256_hex,
      sha256_fastest: ciyam_sha256_fastest,
      module: ciyam_module_named
   };
}

// NOTE: The globals the console, Ian's harness and the lists use - "ciyam_<name>_at_load( callback )" (with the page's
// "init_script_value" run at once), "_execute( callback, input )" and "_result" - made for a registered module. Usage
// hints ("include_script_usage_hints") as the old modules gave them: on loading, "employ javascript <name> <example>";
// with the first answer, its "hint" - a function of the result, where the hint holds it ("rpc_unlock"'s).
//
// What "run( )" and "loaded( )" answer - the result, or { result, output } - is said, and kept as "_result".
function ciyam_module_legacy( module_class, host )
{
   var about = module_class.about;
   var name = ciyam_module_short_name( module_class );
   var prefix = about.name;
   var instance = null;
   var shown_hint = false;

   function hints_wanted( )
   {
      return !!host.include_script_usage_hints;
   }

   // NOTE: Each instance is let go when another takes its place, or when the page takes the module's functions away (the
   // console's "unload_server_script( )", which every sign out runs), or when the page stops it.
   var current = null;

   ciyam_module_stoppers[ name ] = function( )
   {
      var had = ( instance !== null );

      current = null;
      instance = null;

      host[ prefix + "_result" ] = null;

      return had;
   };

   function make( )
   {
      var token = { };

      current = token;

      return new module_class( ciyam_module_page_context( host, prefix, function( )
      {
         return ( current !== token ) || ( host[ prefix + "_at_load" ] !== at_load );
      } ) );
   }

   // NOTE: What was answered - from "run( )", the result, kept and said; as { result, output }, "result" kept and "output"
   // said, and with no "result" the one kept before left as it was ("harden"'s "(ready)"). From "loaded( )", text is only
   // said; { result, output } keeps a result too ("bip39"'s words).
   function said( out, from_run )
   {
      var whole = ( out !== null ) && ( typeof out === "object" );
      var result = whole ? out.result : ( from_run ? out : undefined );
      var output = whole ? ( ( out.output !== undefined ) ? out.output : out.result ) : out;

      if( whole ? ( result !== undefined ) : from_run )
         host[ prefix + "_result" ] = ( result === undefined ) ? null : result;

      return { result: result, output: ( ( output === null ) || ( output === undefined ) ) ? "" : String( output ) };
   }

   async function answer( callback, input )
   {
      var running = instance;
      var answered = await running.run( input );

      // NOTE: Let go meanwhile - nothing kept, nothing said.
      if( running.context.stopped( ) )
         return;

      var out = said( answered, true );
      var output = out.output;

      if( hints_wanted( ) && !shown_hint && about.hint )
      {
         shown_hint = true;

         output += "\n" + ( ( typeof about.hint === "function" ) ? about.hint( out.result ) : about.hint );
      }

      callback( output );
   }

   host[ prefix + "_result" ] = null;

   var at_load = async function( callback )
   {
      host[ prefix + "_result" ] = null;

      instance = make( );
      shown_hint = false;

      var input = ( host.init_script_value === undefined ) ? null : host.init_script_value;

      if( input !== null )
         return answer( callback, input );

      var output = said( await instance.loaded( ), false ).output;

      // NOTE: A module whose loading answers with its own usage line ("bip39"'s "{@1}") gives no second one.
      if( hints_wanted( ) && ( about.example !== undefined ) )
      {
         output += "\nemploy javascript " + name + " " + about.example;

         shown_hint = shown_hint || !!about.hint_once;
      }

      callback( output );
   };

   host[ prefix + "_at_load" ] = at_load;

   host[ prefix + "_execute" ] = async function( callback, input )
   {
      if( instance === null )
         instance = make( );

      return answer( callback, ( input === undefined ) ? null : input );
   };
}

if( typeof module !== "undefined" )
{
   module.exports = {
      CIYAM: CIYAM,
      ciyam_module: ciyam_module,
      ciyam_module_named: ciyam_module_named,
      ciyam_register_module: ciyam_register_module,
      ciyam_unregister_module: ciyam_unregister_module,
      ciyam_module_about: ciyam_module_about,
      ciyam_module_legacy: ciyam_module_legacy,
      ciyam_module_page_context: ciyam_module_page_context,
      ciyam_sha256_hex: ciyam_sha256_hex,
      ciyam_utf8_binary: ciyam_utf8_binary,
      password_hash: password_hash,
      password_hash_rounds: password_hash_rounds,
      is_okay_response: is_okay_response,
      ciyam_sha256_fastest: ciyam_sha256_fastest,
      ciyam_module_stop: ciyam_module_stop
   };
}

if( typeof process !== "undefined" )
{
   var offset = 2;

   if( process.argv[ offset ] != null )
   {
      var host = "http://localhost:13031";

      var test = false;

      if( process.argv[ offset ] == "-test" )
      {
         ++offset;
         test = true;
      }

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

      ciyam_node( host, access, device, hashed, passwd, test, debug, quiet );
   }
}

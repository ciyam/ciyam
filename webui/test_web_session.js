// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

const pin_length = 5;

const own_name = "***";

const var_name = "var";
const vars_name = "vars";

const echo_name = "echo";
const exec_name = "exec";
const exit_name = "exit";
const help_name = "help";
const quit_name = "quit";
const save_name = "save";
const seed_name = "seed";
const view_name = "view";
const wait_name = "wait";

const clear_name = "clear";

const finish_name = "finish";

const admin_username = "admin";

const admin_session_name = "[admin]";
const standard_session_name = "[standard]";

const ciyam_prefix = "ciyam_";

const var_func_substr = "substr";

const creds_remove_help = "remove creds";
const creds_retain_help = "retain creds";

const javascripts_employ_help = "employ javacript";
const javascripts_reload_help = "reload javacript";
const javascripts_result_help = "result javacript";
const javascripts_unload_help = "unload javacript";

const stylesheets_reload_help = "reload stylesheet";
const stylesheets_unload_help = "unload stylesheet";

const creds_delete_cmd = "creds|delete";
const creds_remove_cmd = "creds|remove";
const creds_retain_cmd = "creds|retain";

const users_create_cmd = "users|create";
const users_delete_cmd = "users|delete";
const users_review_cmd = "users|review";

const messages_create_cmd = "messages|create";
const messages_review_cmd = "messages|review";

const storages_attach_cmd = "storages|attach";
const storages_review_cmd = "storages|review";

const javascripts_delete_cmd = "javascripts|delete";
const javascripts_employ_cmd = "javascripts|employ";
const javascripts_reload_cmd = "javascripts|reload";
const javascripts_result_cmd = "javascripts|result";
const javascripts_retain_cmd = "javascripts|retain";
const javascripts_review_cmd = "javascripts|review";
const javascripts_unload_cmd = "javascripts|unload";

const stylesheets_delete_cmd = "stylesheets|delete";
const stylesheets_reload_cmd = "stylesheets|reload";
const stylesheets_retain_cmd = "stylesheets|retain";
const stylesheets_review_cmd = "stylesheets|review";
const stylesheets_unload_cmd = "stylesheets|unload";

const webcmdlists_delete_cmd = "webcmdlists|delete";
const webcmdlists_retain_cmd = "webcmdlists|retain";
const webcmdlists_review_cmd = "webcmdlists|review";

const unlock_keys_create_cmd = "unlock-keys|create";
const unlock_keys_employ_cmd = "unlock-keys|employ";

const storage_modules_review_cmd = "storage-modules|review";
const storage_instances_review_cmd = "storage-instances|review";

const error_prefix = "Error: ";

var has_seed = false;
var was_exec = false;

var all_requests = "";

var wait_bool_repeats = 0;
var max_wait_bool_repeats = 100;

function test_web_session( )
{
   document.getElementById( "user_id_div" ).style.display = "none";
   document.getElementById( "password_div" ).style.display = "block";

   if( localStorage.getItem( "cws.device" ) != null )
      ciyam.device = localStorage.getItem( "cws.device" );

   if( localStorage.getItem( "cws.access" ) != null )
   {
      var access_array = localStorage.getItem( "cws.access" );

      var access_entries = access_array.split( "," );

      var user_droplist = document.getElementById( "user_droplist" );

      var access = "";

      for( var i = 0, l = access_entries.length; i < l; i++ )
      {
         if( ( i == 0 ) && ( access_entries.length == 1 ) )
            access = access_entries[ i ];

         var option = access_entries[ i ];

         user_droplist.options.add( new Option( option, option, false ) );
      }

      ciyam.access = access;

      document.getElementById( "user_access" ).value = ciyam.access;

      document.getElementById( "user_access" ).disabled = true;

      if( ciyam.access != "" )
      {
         document.getElementById( "user_retain" ).checked = true;

         if( localStorage.getItem( "cws.hashed_" + ciyam.access ) == null )
         {
            document.getElementById( "user_password" ).disabled = false;

            document.getElementById( "user_password" ).focus( );
         }
         else
         {
            ciyam.hashed = localStorage.getItem( "cws.hashed_" + ciyam.access );

            document.getElementById( "user_retain" ).checked = true;

            document.getElementById( "user_password" ).value = "***";
            document.getElementById( "user_password" ).disabled = true;

            document.getElementById( "user_unique" ).focus( );
         }
      }
   }

   document.getElementById( "user_retain" ).disabled = true;

   document.getElementById( "user_device" ).value = ciyam.device;

   if( document.getElementById( "user_access" ).value == "" )
   {
      document.getElementById( "user_remove" ).disabled = true;
      document.getElementById( "user_retain" ).disabled = true;
   }

   document.getElementById( "user_request" ).disabled = true;
}

function link_session( )
{
   document.getElementById( "user_access" ).value = ciyam.access;
   document.getElementById( "user_device" ).value = ciyam.device;
   document.getElementById( "user_unique" ).value = ciyam.unique;

   document.getElementById( "user_session" ).value = ciyam.sessid;

   document.getElementById( "user_password" ).value = "";

   document.getElementById( "user_access" ).disabled = true;
   document.getElementById( "user_device" ).disabled = true;

   document.getElementById( "user_request" ).disabled = false;

   document.getElementById( "test_special" ).innerText = "";

   document.getElementById( "user_request" ).focus( );
   document.getElementById( "user_droplist" ).disabled = true;
   document.getElementById( "reset_storage" ).disabled = true;

   document.getElementById( "user_arbitrary" ).value = "(session linked)";
}

function unlink_session( )
{
   document.getElementById( "user_access" ).value = ciyam.access;
   document.getElementById( "user_device" ).value = ciyam.device;
   document.getElementById( "user_unique" ).value = ciyam.unique;

   document.getElementById( "user_session" ).value = ciyam.sessid;

   document.getElementById( "user_password" ).value = "";

   document.getElementById( "user_request" ).value = "";

   document.getElementById( "user_request" ).disabled = true;

   document.getElementById( "test_special" ).innerText = "";

   document.getElementById( "user_unique" ).focus( );
   document.getElementById( "user_droplist" ).disabled = false;
   document.getElementById( "reset_storage" ).disabled = false;

   document.getElementById( "user_arbitrary" ).value = "(session unlinked)";
}

function select_access( )
{
   var access = document.getElementById( "user_droplist" ).value;

   document.getElementById( "user_name" ).value = "";

   document.getElementById( "user_id_div" ).style.display = "none";
   document.getElementById( "password_div" ).style.display = "block";

   if( access == "create" )
   {
      ciyam.access = "";
      ciyam.hashed = "";

      document.getElementById( "user_access" ).value = ciyam.access;
      document.getElementById( "user_device" ).value = ciyam.device;

      document.getElementById( "user_retain" ).checked = false;
      document.getElementById( "user_retain" ).disabled = false;

      document.getElementById( "user_access" ).disabled = false;
      document.getElementById( "user_access" ).focus( );

      document.getElementById( "user_password" ).value = "";
   }
   else
   {
      ciyam.access = access;

      document.getElementById( "user_access" ).value = ciyam.access;

      var new_hashed = "";

      if( localStorage.getItem( "cws.hashed_" + ciyam.access ) != null )
         new_hashed = localStorage.getItem( "cws.hashed_" + access );

      ciyam.hashed = new_hashed;

      if( ciyam.hashed == "" )
      {
         document.getElementById( "user_password" ).value = "";
         document.getElementById( "user_password" ).disabled = false;
      }
      else
      {
         document.getElementById( "user_password" ).value = "***";
         document.getElementById( "user_password" ).disabled = true;
      }

      document.getElementById( "user_device" ).value = ciyam.device;

      document.getElementById( "user_remove" ).disabled = false;

      document.getElementById( "user_retain" ).checked = true;
      document.getElementById( "user_retain" ).disabled = true;

      if( ciyam.hashed != "" )
         document.getElementById( "user_unique" ).focus( );
      else
         document.getElementById( "user_password" ).focus( );
   }

   document.getElementById( "user_droplist" ).value = "";
}

function populate_access( )
{
   var user_droplist = document.getElementById( "user_droplist" )

   var i, l = user_droplist.options.length - 1;

   for( i = l; i >= 2; i-- )
      user_droplist.remove( i );

   var access = document.getElementById( "user_access" ).value;

   ciyam.access = access;

   var is_retained = false;

   if( localStorage.getItem( "cws.access" ) != null )
   {
      var access_array = localStorage.getItem( "cws.access" );

      var access_entries = access_array.split( "," );

      for( var i = 0, l = access_entries.length; i < l; i++ )
      {
         var option = access_entries[ i ];

         if( option == access )
            is_retained = true;

         user_droplist.options.add( new Option( option, option, false ) );
      }
   }

   if( is_retained )
   {
      document.getElementById( "user_remove" ).disabled = false;

      document.getElementById( "user_retain" ).checked = true;
      document.getElementById( "user_retain" ).disabled = true;
   }
   else
   {
      document.getElementById( "user_remove" ).disabled = true;

      document.getElementById( "user_retain" ).checked = false;
      document.getElementById( "user_retain" ).disabled = false;
   }
}

function reformatted( request )
{
   var org = request;

   var parts = request.split( " " );

   if( parts.length > 1 )
   {
      var first = parts[ 0 ];
      var second = parts[ 1 ];

      if( ( second == "list" ) || ( second == "lists" ) )
         second = "webcmdlists";
      else if( ( second == "style" ) || ( second == "styles" ) )
         second = "stylesheets";
      else if( ( second == "script" ) || ( second == "scripts" ) )
         second = "javascripts";

      var suffix = "";

      var pos = first.indexOf( '_' );

      if( pos < 0 )
      {
         if( ( first == "load" )
          || ( first == "reload" ) )
            suffix = "|reload";
         else if( ( first == "get" )
          || ( first == "list" )
          || ( first == "read" )
          || ( first == "view" )
          || ( first == "review" ) )
            suffix = "|review";
         else if( ( first == "new" )
          || ( first == "create" ) )
            suffix = "|create";
         else if( ( first == "access" )
          || ( first == "attach" ) )
            suffix = "|attach";
         else if( ( first == "eval" )
          || ( first == "exec" )
          || ( first == "employ" )
          || ( first == "execute" ) )
            suffix = "|employ";
         else if( ( first == "save" )
          || ( first == "store" )
          || ( first == "retain" ) )
            suffix = "|retain";
         else if( ( first == "del" )
          || ( first == "kill" )
          || ( first == "erase" )
          || ( first == "delete" )
          || ( first == "remove" )
          || ( first == "destroy" ) )
            suffix = "|delete";
         else if( first == "result" )
            suffix = "|result";
         else if( first == "unload" )
            suffix = "|unload";
      }

      if( suffix != "" )
      {
         if( ( second.length > 3 )
          && ( second.slice( -1 ) != "s" ) )
            second += "s";

         request = second + suffix;

         if( request == creds_delete_cmd )
            request = creds_remove_cmd;

         if( parts.length > 2 )
         {
            var tail = parts.slice( 2 );

            request += " " + tail.join( " " );
         }
      }
   }
   else if( ( request == exit_name )
    || ( request == finish_name ) )
      request = quit_name;

   if( org != request )
      console.log( "reformatted = '" + request + "'" );

   return request;
}

function do_local_request( request, interactive )
{
   const access = ciyam.access;

   if( request == creds_retain_cmd )
      do_retain( true );
   else if( request.startsWith( creds_retain_cmd ) )
      do_retain( false );
   else if( request == creds_remove_cmd )
      do_remove( true );
   else if( request.startsWith( creds_remove_cmd ) )
      do_remove( request.substr( creds_remove_cmd.length + 1 ) );
   else if( request.startsWith( javascripts_employ_cmd ) )
   {
      var script = request.substr( javascripts_employ_cmd.length + 1 );

      var script_arg = null;

      var pos = script.indexOf( " " );

      if( pos > 0 )
      {
         script_arg = script.substring( pos + 1 );

         script = script.substr( 0, pos );
      }

      if( script == own_name )
         script = access;

      if( script != "" )
      {
         if( script_arg == null )
            eval( ciyam_prefix + script + "_execute( script_executed, )" );
         else
            eval( ciyam_prefix + script + "_execute( script_executed, \"" + script_arg + "\" )" );
      }
   }
   else if( request == javascripts_reload_cmd )
      load_js( ciyam_prefix + access, document.getElementById( "user_arbitrary" ).value );
   else if( request.startsWith( javascripts_reload_cmd ) )
   {
      init_script_value = null;

      include_script_usage_hints = !was_exec;

      var script = request.substr( javascripts_reload_cmd.length + 1 );

      var pos = script.indexOf( " " );

      if( pos > 0 )
      {
         init_script_value = script.substring( pos + 1 );

         script = script.substr( 0, pos );
      }

      if( script == own_name )
         script = access;

      // NOTE: If the script was already loaded but is being done
      // so interactively then force the hints to be shown again.
      if( ( interactive != null ) && ( interactive == true ) )
      {
         if( window[ ciyam_prefix + script + "_has_shown_hints" ] != null )
            window[ ciyam_prefix + script + "_has_shown_hints" ] = false;
      }

      if( script != "" )
         load_js( ciyam_prefix + script );
   }
   else if( request.startsWith( javascripts_result_cmd ) )
   {
      var script = request.substr( javascripts_reload_cmd.length + 1 );

      if( ( script == "" ) || ( script == own_name ) )
         script = access;

      if( window[ ciyam_prefix + script + "_result" ] != null )
         document.getElementById( "user_arbitrary" ).value = window[ ciyam_prefix + script + "_result" ];
   }
   else if( request == javascripts_unload_cmd )
      unload_js( ciyam_prefix + access );
   else if( request.startsWith( javascripts_unload_cmd ) )
   {
      var script = request.substr( javascripts_unload_cmd.length + 1 );

      if( script == own_name )
         script = access;

      if( script != "" )
         unload_js( ciyam_prefix + script );
   }
   else if( request == stylesheets_reload_cmd )
      load_style( access, document.getElementById( "user_arbitrary" ).value );
   else if( request.startsWith( stylesheets_reload_cmd ) )
   {
      var style = request.substr( stylesheets_reload_cmd.length + 1 );

      if( style == own_name )
         style = access;

      if( style != "" )
         load_style( style );
   }
   else if( request == stylesheets_unload_cmd )
      unload_style( access );
   else if( request.startsWith( stylesheets_unload_cmd ) )
   {
      var style = request.substr( stylesheets_unload_cmd.length + 1 );

      if( style == own_name )
         style = access;

      if( style != "" )
         unload_style( style );
   }

   if( all_requests == "" )
   {
      document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
      document.getElementById( "user_request" ).value = "";
   }
}

async function do_fetch( )
{
   var request = "";

   was_exec = false;

   if( all_requests == "" )
   {
      document.getElementById( "test_special" ).innerText = "";

      request = document.getElementById( "user_request" ).value;

      request = ciyam.replace_variables( request, document.getElementById( "user_arbitrary" ).value );
   }
   else
   {
      was_exec = true;

      var pos = all_requests.indexOf( "\n" );

      while( pos == 0 )
      {
         all_requests = all_requests.substring( 1 );

         pos = all_requests.indexOf( "\n" );
      }

      if( pos < 0 )
      {
         request = all_requests;

         request = ciyam.replace_variables( request, document.getElementById( "user_arbitrary" ).value );

         if( request.indexOf( "@" ) == 0 )
            request = ciyam.replace_variables( request.substring( 1 ), document.getElementById( "user_arbitrary" ).value );

         all_requests = "";
      }
      else
      {
         request = all_requests.substr( 0, pos );

         request = ciyam.replace_variables( request, document.getElementById( "user_arbitrary" ).value );

         if( request.indexOf( "@" ) == 0 )
            request = ciyam.replace_variables( request.substring( 1 ), document.getElementById( "user_arbitrary" ).value );

         all_requests = all_requests.substring( pos + 1 );

         var msecs = 50;

         if( request.indexOf( wait_name ) == 0 )
         {
            msecs = 100;

            pos = request.indexOf( " " );

            if( pos > 0 )
            {
               var first = "";

               var argument = request.substring( pos + 1 );

               if( argument.length > 0 )
                  first = argument.substr( 0, 1 );

               if( ( first >= "0" ) && ( first <= "9" ) )
                  msecs = argument;
               else if( first != "" )
               {
                  if( ( all_requests != "" )
                   && ( ( window[ argument ] == null )
                   || ( window[ argument ] == "" )
                   || ( window[ argument ] == false ) ) )
                  {
                     ++wait_bool_repeats;

                     if( wait_bool_repeats > max_wait_bool_repeats )
                     {
                        all_requests = "";

                        wait_bool_repeats = 0;

                        document.getElementById( "user_arbitrary" ).value += "\n(timed out waiting for '" + argument + "')";
                     }
                     else
                        all_requests = request + "\n" + all_requests;
                  }
                  else
                     wait_bool_repeats = 0;
               }
            }

            console.log( "request = '" + request + "'" );

            request = "";
         }

         pos = all_requests.indexOf( "\n" );

         while( pos == 0 )
         {
            all_requests = all_requests.substring( 1 );

            pos = all_requests.indexOf( "\n" );
         }

         if( all_requests != "" )
            window.setTimeout( do_fetch, msecs );
      }
   }

   // NOTE: If a request is prefixed
   // with "?{var} " then will erase
   // the request if "var" is empty.
   if( request.indexOf( "?" ) == 0 )
   {
      var pos = request.indexOf( " " );

      if( pos > 0 )
      {
         var check = request.substr( 0, pos );

         if( check == "?" )
            request = "";
         else
            request = request.substring( pos + 1 );
      }
   }

   // NOTE: Works just like "?" but
   // erases if "var" is not empty.
   if( request.indexOf( "!" ) == 0 )
   {
      var pos = request.indexOf( " " );

      if( pos > 0 )
      {
         var check = request.substr( 0, pos );

         if( check != "!" )
            request = "";
         else
            request = request.substring( pos + 1 );
      }
   }

   if( request != "" )
      console.log( "request = '" + request + "'" );

   // NOTE: Use "@" to replace the request which can
   // be used to create a command from a variable or
   // variables (e.g. "@{cmd}" (only is expected for
   // interactive input here).
   if( request.indexOf( "@" ) == 0 )
      document.getElementById( "user_request" ).value = request.substring( 1 );
   else if( request == vars_name )
   {
      document.getElementById( "user_arbitrary" ).value = ciyam.get_all_variables( );

      if( !was_exec )
      {
         document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
         document.getElementById( "user_request" ).value = "";
      }
   }
   else if( request == view_name )
   {
      if( ( ciyam.access != "" ) && ( localStorage.getItem( "cws.arbitrary_" + ciyam.access ) != null ) )
         document.getElementById( "user_arbitrary" ).value = localStorage.getItem( "cws.arbitrary_" + access );

      if( !was_exec )
      {
         document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
         document.getElementById( "user_request" ).value = "";
      }
   }
   else if( request == save_name )
   {
      if( ciyam.access != "" )
         localStorage.setItem( "cws.arbitrary_" + ciyam.access, document.getElementById( "user_arbitrary" ).value );

      if( !was_exec )
      {
         document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
         document.getElementById( "user_request" ).value = "";
      }
   }
   else if( request == clear_name )
   {
      document.getElementById( "user_arbitrary" ).value = "";

      if( !was_exec )
      {
         document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
         document.getElementById( "user_request" ).value = "";
      }
   }
   else if( request.startsWith( var_name ) )
   {
      pos = request.indexOf( " " );

      if( pos > 0 )
      {
         var var_info = request.substring( pos + 1 );

         pos = var_info.indexOf( " " );

         if( pos < 0 )
         {
            if( ciyam.has_variable( var_info ) )
            {
               var existing = document.getElementById( "user_arbitrary" ).value;

               if( existing != "" )
                  existing += "\n";

               output = existing + ciyam.get_variable( var_info );

               document.getElementById( "user_arbitrary" ).value = output;
            }
         }
         else
         {
            var name = var_info.substr( 0, pos );
            var value = var_info.substring( pos + 1 );

            var is_func = false;
            var skip_if_has = false;

            // NOTE: If the name is prefixed
            // with "!" then will not assign
            // it a value unless it does not
            // currently have one.
            if( name.substr( 0, 1 ) == "!" )
            {
               skip_if_has = true;
               name = name.substring( 1 );
            }

            if( name.substr( 0, 1 ) == "#" )
            {
               is_func = true;
               name = name.substring( 1 );
            }

            // NOTE: If name is prefixed with
            // "@" then assumes that value is
            // the name of an actual variable
            // like "ciyam_script_xxx_result"
            // with the variable's value then
            // being used instead.
            if( name.substr( 0, 1 ) == "@" )
            {
               name = name.substring( 1 );

               // NOTE: Use "var @name null" in
               // order to "delete" a variable.
               if( value == "null" )
                  value = "";
               else
               {
                  if( window[ value ] == null )
                     value = "";
                  else
                     value = window[ value ];
               }
            }

            if( is_func )
            {
               var pos = value.indexOf( ":" );

               var okay = true;

               if( pos > 0 )
               {
                  var func_name = value.substr( 0, pos );
                  var func_info = value.substring( pos + 1 )

                  if( func_name == var_func_substr )
                  {
                     var length = null;

                     pos = func_info.indexOf( "," );

                     if( pos > 0 )
                     {
                        length = func_info.substring( pos + 1 );
                        func_info = func_info.substr( 0, pos );
                     }

                     if( ( func_info != "" ) && ( ciyam.has_variable( name ) ) )
                     {
                        var existing = document.getElementById( "user_arbitrary" ).value;

                        if( existing != "" )
                           existing += "\n";

                        var value = ciyam.get_variable( name );

                        if( length == null )
                           output = existing + value.substring( func_info );
                        else
                           output = existing + value.substr( func_info, length );

                        document.getElementById( "user_arbitrary" ).value = output;
                     }
                  }
                  else
                     okay = false;
               }
               else
                  okay = false;

               if( okay == false )
                  handle_error( "Invalid or unknown variable function information '" + value + "'", true );
            }
            else if( !skip_if_has || !ciyam.has_variable( name ) )
            {
               var had_lower = false;
               var had_upper = false;
               var had_numeric = false;
               var had_underbar = false;

               var valid_user_var_name = true;

               for( i = 0; i < name.length; i++ )
               {
                  var next = name.substr( i, 1 );

                  if( ( next >= "a" ) && ( next <= "z" ) )
                     had_lower = true;
                  else if( ( next >= "A" ) && ( next <= "Z" ) )
                     had_upper = true;
                  else if( i == 0 )
                     valid_user_var_name = false;
                  else if( ( next >= '0' ) && ( next <= '9' ) )
                     had_numeric = true;
                  else if( next == "_" )
                     had_underbar = true;
                  else
                     valid_user_var_name = false;

                  if( !valid_user_var_name )
                     break;
               }

               // NOTE: All upper case are reserved for "special"
               // variable names (such as "ACCESS" and "DEVICE").
               if( !had_lower && !had_numeric && !had_underbar )
                  valid_user_var_name = false;

               if( !valid_user_var_name )
                  handle_error( "Invalid user variable name '" + name + "'.", true );
               else
               {
                  if( value == "" )
                     ciyam.remove_variable( name );
                  else
                     ciyam.set_variable( name, value );
               }
            }
         }
      }

      if( !was_exec )
      {
         document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
         document.getElementById( "user_request" ).value = "";
      }
   }
   else if( request.startsWith( echo_name ) )
   {
      var output = "";

      pos = request.indexOf( " " );

      if( pos > 0 )
         output = request.substring( pos + 1 );

      var existing = document.getElementById( "user_arbitrary" ).value;

      if( existing != "" )
         existing += "\n";

      output = existing + output;

      document.getElementById( "user_arbitrary" ).value = output;

      if( !was_exec )
      {
         document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
         document.getElementById( "user_request" ).value = "";
      }
   }
   else if( request.startsWith( seed_name ) )
   {
      var pos = request.indexOf( " " );

      var num = 11;

      if( pos > 0 )
         num = request.substring( pos + 1 );

      var output = CIYAM.generate_base64_key( num );

      request = "";

      var existing = document.getElementById( "user_arbitrary" ).value;

      if( existing != "" )
         existing += "\n";

      output = existing + output;

      document.getElementById( "user_arbitrary" ).value = output;

      if( !was_exec )
      {
         document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
         document.getElementById( "user_request" ).value = "";
      }
   }
   else
   {
      if( request.indexOf( "~" ) == 0 )
         request = request.substring( 1 );

      request = reformatted( request );

      if( request == exec_name )
      {
         include_script_usage_hints = false;

         all_requests = document.getElementById( "user_arbitrary" ).value;

         document.getElementById( "user_arbitrary" ).value = "";

         document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;
         document.getElementById( "user_request" ).value = "";

         window.setTimeout( do_fetch, 100 );
      }
      else if( request == quit_name )
      {
         ciyam.debug = true;

         ciyam.disconnect( handle_response );

         ciyam.debug = false;

         if( localStorage.getItem( "cws.hashed_" + ciyam.access ) == null )
         {
            document.getElementById( "user_password" ).disabled = false;

            document.getElementById( "user_password" ).focus( );
         }
         else
         {
            document.getElementById( "user_password" ).value = "***";
            document.getElementById( "user_password" ).disabled = true;

            document.getElementById( "user_unique" ).focus( );
         }
      }
      else if( ( request == "" )
       && ( ciyam.device != "" ) && ( ciyam.unique == "" )
       && ( document.getElementById( "user_password" ).value != "" ) )
      {
         if( ciyam.access == "" )
            ciyam.access = document.getElementById( "user_access" ).value;

         ciyam.debug = true;

         if( ciyam.hashed != "" )
            await ciyam.connect( ciyam.access,
             ciyam.device, ciyam.hashed, "", handle_response );
         else
            await ciyam.connect( ciyam.access, ciyam.device, "",
             document.getElementById( "user_password" ).value, handle_response );

         ciyam.debug = false;

         if( ciyam.sessid != "" )
         {
            document.getElementById( "user_session" ).value = ciyam.sessid;
            document.getElementById( "user_unique" ).value = ciyam.unique;

            document.getElementById( "user_password" ).value = "";

            document.getElementById( "user_request" ).focus( );
         }
      }
      else if( request.startsWith( creds_retain_cmd )
       || request.startsWith( creds_remove_cmd )
       || request.startsWith( javascripts_employ_cmd )
       || request.startsWith( javascripts_employ_cmd )
       || request.startsWith( javascripts_reload_cmd )
       || request.startsWith( javascripts_result_cmd )
       || request.startsWith( javascripts_unload_cmd )
       || request.startsWith( stylesheets_reload_cmd )
       || request.startsWith( stylesheets_unload_cmd ) )
         do_local_request( request, !was_exec );
      else if( !was_exec || ( request != "" ) )
      {
         var url = ciyam.get_cws_url( );

         var cmd = request;

         var pos = request.indexOf( " " );

         if( pos > 0 )
            cmd = request.substring( 0, pos );

         var cmd_initial = cmd;

         var request_type = "GET";

         if( ( cmd == javascripts_retain_cmd )
          || ( cmd == stylesheets_retain_cmd ) || ( cmd == webcmdlists_retain_cmd ) )
            request_type = "PUT";
         else if( ( cmd == "" ) || ( cmd == users_create_cmd )
          || ( cmd == messages_create_cmd ) || ( cmd == storages_attach_cmd )
          || ( cmd == unlock_keys_create_cmd ) || ( cmd == unlock_keys_employ_cmd ) )
            request_type = "POST";
         else if( ( cmd == users_delete_cmd ) || ( cmd == javascripts_delete_cmd )
          || ( cmd == stylesheets_delete_cmd ) || ( cmd == webcmdlists_delete_cmd ) )
            request_type = "DELETE";

         var has_cmd_options = false;
         var has_name_suffix = false;

         var cmd_is_url_suffix = false;
         var cmd_needs_truncating = false;

         var need_to_include_payload = false;

         if( request == help_name )
            cmd_is_url_suffix = true;
         else if( ( cmd == users_review_cmd ) || ( cmd == unlock_keys_create_cmd )
          || ( cmd == javascripts_delete_cmd ) || ( cmd == stylesheets_delete_cmd )
          || ( cmd == javascripts_retain_cmd ) || ( cmd == stylesheets_retain_cmd )
          || ( cmd == webcmdlists_delete_cmd ) || ( cmd == webcmdlists_retain_cmd ) )
         {
            cmd_is_url_suffix = true;
            cmd_needs_truncating = true;

            if( ( request == javascripts_retain_cmd )
             || ( request == stylesheets_retain_cmd ) || ( request == webcmdlists_retain_cmd ) )
               need_to_include_payload = true;
         }

         if( ( cmd == users_create_cmd ) || ( cmd == users_delete_cmd )
          || ( cmd == messages_create_cmd ) || ( cmd == messages_review_cmd )
          || ( cmd == storages_attach_cmd ) || ( cmd == storages_review_cmd )
          || ( cmd == unlock_keys_create_cmd ) || ( cmd == unlock_keys_employ_cmd )
          || ( cmd == javascripts_review_cmd ) || ( cmd == stylesheets_review_cmd )
          || ( cmd == webcmdlists_review_cmd ) || ( cmd == storage_modules_review_cmd ) || ( cmd == storage_instances_review_cmd ) )
         {
            if( ( cmd == users_create_cmd ) || ( cmd == unlock_keys_create_cmd ) )
               has_cmd_options = true;
            else
            {
               has_name_suffix = true;

               if( ( cmd == messages_create_cmd )
                || ( cmd == messages_review_cmd ) || ( cmd == storage_instances_review_cmd ) )
                  has_cmd_options = true;
            }

            cmd_is_url_suffix = true;
            cmd_needs_truncating = true;
         }

         if( cmd_needs_truncating )
         {
            var tpos = cmd.indexOf( "|" );

            if( tpos > 0 )
               cmd = cmd.substring( 0, tpos );
         }

         if( cmd_is_url_suffix )
         {
            var name = "";

            var stripped_name = false;

            if( has_name_suffix && ( pos > 0 ) )
            {
               name = request.substring( pos + 1 );

               if( name == own_name )
                  name = ciyam.access;

               pos = name.indexOf( " " );

               if( pos >= 0 )
               {
                  request = name.substring( pos + 1 );
                  stripped_name = true;

                  name = name.substring( 0, pos );
               }
               else
                  request = "";
            }

            if( !has_cmd_options )
               request = "";
            else if( !stripped_name )
            {
               var opos = request.indexOf( " " );

               if( opos < 0 )
                  request = "";
               else
                  request = request.substring( opos + 1 );
            }

            url += "/" + cmd;

            if( name != "" )
            {
               if( cmd_initial == unlock_keys_employ_cmd )
               {
                  // NOTE: For testing combine and hash a
                  // simple password (determined as being
                  // anything smaller than an unlock key)
                  // or if "!" prefixed then just removes
                  // the prefix.
                  if( name.length < 17 )
                  {
                     if( name.substr( 0, 1 ) == "!" )
                        name = name.substring( 1 );
                     else
                        name = ciyam.hash_combined( name );
                  }
               }

               url += "/" + name;
            }
         }

         if( cmd == quit_name )
            request = "";

         var access = ciyam.access;

         if( access == "" )
            access = document.getElementById( "user_access" ).value;

         if( ( ciyam.access == "" )
          && ( access.length == pin_length ) && ( access != admin_username ) )
            ciyam.access = access;

         var qry_data = "access=" + access;

         if( ciyam.device != "" )
            qry_data += "&device=" + ciyam.device;

         qry_data += "&format=text";

         var username = "";

         if( document.getElementById( "user_name" ).value != "" )
            username = document.getElementById( "user_name" ).value;

         var password = "";

         if( document.getElementById( "user_password" ).value != "" )
            password = document.getElementById( "user_password" ).value;

         var combined = "";

         var is_new_user = false;

         if( username != "" )
         {
            is_new_user = true;

            if( username == admin_username )
               combined = ciyam.hash_combined( password );
            else
            {
               // NOTE: For testing purposes the password is
               // just the "username" so to make it easy for
               // this UI just copies the username across to
               // the "password" input.
               combined = ciyam.hash_combined( username );

               document.getElementById( "user_password" ).value = username;
            }
         }

         var credentials = "";

         if( combined != "" )
         {
            if( username != "" )
               credentials = username + ":";

            credentials += combined;
         }

         if( is_new_user || ( ciyam.device == "" ) )
         {
            request_type = "POST";

            url += "/devices";

            // NOTE: Credentials must be base64 encoded.
            if( credentials != "" )
               qry_data += "&passwd=" + CIYAM.encode_base64_url( credentials );
         }

         var arbitrary = "";

         if( document.getElementById( "user_arbitrary" ).value != "" )
            arbitrary = document.getElementById( "user_arbitrary" ).value;

         if( ( arbitrary != "" ) && need_to_include_payload )
            qry_data += "&payload=" + encodeURIComponent( arbitrary );

         if( request != "" )
         {
            if( has_cmd_options )
               qry_data += "&options=" + encodeURIComponent( request );
            else
               qry_data += "&request=" + encodeURIComponent( request );
         }

         if( ( ciyam.device == "" ) && ( password != "" ) )
         {
            if( has_seed )
            {
               has_seed = false;

               document.getElementById( "user_request" ).value = "";
               document.getElementById( "user_request" ).disabled = true;
            }
         }
         else if( !is_new_user )
         {
            if( ciyam.unique == "" )
            {
               if( ciyam.device != "" )
               {
                  request_type = "POST";

                  url += "/sessions";
               }
            }
            else
            {
               if( ciyam.sessid == "" )
               {
                  if( ciyam.hashed == "" )
                     ciyam.determine_hashed( password );

                  ciyam.determine_sess_id( );

                  document.getElementById( "user_session" ).value = ciyam.sessid;

                  document.getElementById( "user_request" ).disabled = false;
                  document.getElementById( "user_request" ).focus( );

                  document.getElementById( "user_password" ).value = "";
                  document.getElementById( "user_password" ).disabled = true;

                  document.getElementById( "reset_storage" ).disabled = true;
               }

               if( cmd != quit_name )
               {
                  if( cmd == "" )
                  {
                     url += "/status";

                     request_type = "GET";
                  }

                  qry_data += "&session=" + ciyam.sessid;
               }
               else
               {
                  request_type = "DELETE";

                  url += "/sessions/" + ciyam.sessid;
               }
            }
         }

         if( qry_data != "" )
            url += "?" + qry_data;

         ciyam.debug = true;

         ciyam.fetch( url, request_type, handle_response );

         ciyam.debug = false;
      }
   }
}

function do_retain( force_full )
{
   if( ciyam.access != "" )
   {
      var scope = "partially";

      var access_array = localStorage.getItem( "cws.access" );

      if( access_array == null )
         localStorage.setItem( "cws.access", ciyam.access );
      else
      {
         var access_entries = access_array.split( "," );

         if( !binary_search( access_entries, ciyam.access ) )
         {
            access_entries.push( ciyam.access );

            access_entries.sort( );

            localStorage.setItem( "cws.access", access_entries.join( "," ) );
         }

         // NOTE: In case was previously retained remove the 'hashed'
         // password value (so it will not remain when is 'partial').
         if( localStorage.getItem( "cws.hashed_" + ciyam.access ) != null )
            localStorage.removeItem( "cws.hashed_" + ciyam.access );
      }

      if( ciyam.device != "" )
      {
         if( localStorage.getItem( "cws.device" ) == null )
            localStorage.setItem( "cws.device", ciyam.device );

         if( ciyam.hashed != "" )
         {
            var check = force_full;

            if( check == null )
               check = confirm( "User access information has now been stored.\nAlso retain the hashed password information?" );

            if( check )
            {
               scope = "completely";

               localStorage.setItem( "cws.hashed_" + ciyam.access, ciyam.hashed );
            }
         }
      }

      populate_access( );

      document.getElementById( "user_retain" ).disabled = true;
      document.getElementById( "user_remove" ).disabled = false;

      document.getElementById( "user_password" ).value = "";
      document.getElementById( "user_password" ).disabled = true;

      document.getElementById( "user_request" ).focus( );

      document.getElementById( "test_special" ).innerText = "(retained credentials " + scope + ")";
   }
}

function do_remove( force_full )
{
   var error = "";

   var scope = "partially";

   var access = "";

   if( ( force_full != null ) && ( force_full != true ) )
   {
      var is_num = /^\d+$/.test( force_full );

      if( is_num && ( force_full.length == pin_length ) )
         access = force_full;
      else
         force_full = false;
   }

   if( access == "" )
      access = ciyam.access;

   var removed_hash = false;

   if( localStorage.getItem( "cws.hashed_" + access ) != null )
   {
      removed_hash =  true;

      localStorage.removeItem( "cws.hashed_" + access );
   }

   var remove_prompt = "";

   if( !removed_hash )
      remove_prompt = "Remove all user access information?"
   else
      remove_prompt = "Hashed password has now been removed.\nAlso remove all user access information?";

   var check = force_full;

   if( check == null )
      check = confirm( remove_prompt );

   if( check )
   {
      scope = "completely";

      var original = localStorage.getItem( "cws.access" );

      if( localStorage.getItem( "cws.access" ) != null )
      {
         var access_array = localStorage.getItem( "cws.access" );

         var access_entries = access_array.split( "," );

         if( access == access_array )
            localStorage.removeItem( "cws.access" );
         else
         {
            var new_access_entries = access_entries.filter( ( item ) => item != access );

            localStorage.setItem( "cws.access", new_access_entries.join( "," ) );
         }
      }

      if( original == localStorage.getItem( "cws.access" ) )
         error = "Error: Unable to find credentials for '" + access + "'.";

      document.getElementById( "user_remove" ).disabled = true;

      document.getElementById( "user_retain" ).checked = false;
      document.getElementById( "user_retain" ).disabled = false;
   }

   populate_access( );

   document.getElementById( "user_password" ).value = "";

   document.getElementById( "user_remove" ).checked = false;

   document.getElementById( "user_request" ).focus( );

   if( error != "" )
      document.getElementById( "test_special" ).innerText = error;
   else
      document.getElementById( "test_special" ).innerText = "(removed credentials " + scope + " for " + access + ")";
}

function handle_error( error, keep_request )
{
   if( !error.startsWith( error_prefix ) )
      error = error_prefix + error;

   if( ciyam.debug == false )
      console.log( error );

   document.getElementById( "test_special" ).innerText = error;

   if( !started_session )
   {
      ciyam.unique = "";

      ciyam.remove_all_variables( );

      document.getElementById( "user_unique" ).value = "";
      document.getElementById( "user_session" ).value = "";

      document.getElementById( "user_request" ).disabled = true;

      document.getElementById( "user_droplist" ).disabled = false;
      document.getElementById( "reset_storage" ).disabled = false;

      document.getElementById( "user_unique" ).focus( );
   }

   if( ( keep_request == null ) || ( keep_request != true ) )
      document.getElementById( "user_request" ).value = "";
}

function handle_response( data )
{
   var lines = data.split( "\n" );

   // NOTE: Changes the placeholder to the request just issued (making it clear that a response was received).
   if( ( document.getElementById( "user_unique" ).value != "" )
    && ( document.getElementById( "user_request" ).value != "" ) )
      document.getElementById( "user_request" ).placeholder = document.getElementById( "user_request" ).value;

   if( data.startsWith( error_prefix ) )
      handle_error( data );
   else if( ciyam.error != "" )
      handle_error( ciyam.error );
   else
   {
      if( ciyam.debug == false )
      {
         if( lines.length < 10 )
            console.log( data );
         else
         {
            var head = lines.slice( 0, 10 );

            console.log( head.join( "\n" ) );
            console.log( "... (+" + ( lines.length - 10 ) + " further lines)" );
         }
      }

      document.getElementById( "test_special" ).innerText = "";

      if( ciyam.hashed == "" )
      {
         var pos = data.indexOf( " " );

         // NOTE: If is a PIN then need to provide password hash.
         if( pos == pin_length )
         {
            var extra = "";

            has_seed = true;

            ciyam.access = data.substr( 0, pos );

            document.getElementById( "user_access" ).value = ciyam.access;

            document.getElementById( "user_access" ).disabled = true;
            document.getElementById( "user_password" ).disabled = false;

            var entropy = data.substr( pos + 1 );

            pos = entropy.indexOf( " " );

            if( pos > 0 )
            {
               extra = entropy.substr( pos + 1 );
               entropy = entropy.substr( 0, pos );
            }

            if( entropy == "@none" )
            {
               document.getElementById( "user_id_div" ).style.display = "block";
               document.getElementById( "password_div" ).style.display = "none";

               document.getElementById( "user_name" ).value = extra;
               document.getElementById( "user_name" ).focus( );
            }
            else
            {
               document.getElementById( "user_name" ).value = admin_username;

               if( entropy != admin_username )
               {
                  document.getElementById( "user_request" ).disabled = false;
                  document.getElementById( "user_request" ).value = entropy;
               }

               document.getElementById( "user_password" ).focus( );
            }
         }
         else
         {
            if( ciyam.device == "" )
            {
               ciyam.device = data;

               if( localStorage.getItem( "cws.device" ) == null )
                  localStorage.setItem( "cws.device", ciyam.device );
            }

            document.getElementById( "user_device" ).value = ciyam.device;

            document.getElementById( "user_access" ).disabled = true;

            document.getElementById( "user_name" ).value = "";

            document.getElementById( "user_id_div" ).style.display = "none";
            document.getElementById( "password_div" ).style.display = "block";

            document.getElementById( "user_password" ).focus( );
         }
      }
      else if( ciyam.unique == "" )
      {
         if( data.indexOf( " " ) >= 0 )
            handle_error( "User does not appear to exist." );
         else
         {
            ciyam.unique = data;

            document.getElementById( "user_unique" ).value = ciyam.unique;

            document.getElementById( "user_access" ).disabled = true;
            document.getElementById( "user_device" ).disabled = true;

            document.getElementById( "user_droplist" ).disabled = true;

            if( ciyam.hashed != "" )
               document.getElementById( "user_session" ).focus( );
            else
            {
               document.getElementById( "user_password" ).disabled = false;
               document.getElementById( "user_password" ).focus( );
            }
         }
      }
      else
      {
         document.getElementById( "test_special" ).innerText = "";

         // NOTE: Add local commands to the help output.
         if( document.getElementById( "user_request" ).value == help_name )
         {
            var lines = data.split( "\n" );

            lines.push( var_name + " <name> [<text>]" );
            lines.push( vars_name );

            lines.push( echo_name + " [<text>]" );

            lines.push( exec_name );

            lines.push( save_name );
            lines.push( seed_name + " [<rand_chars:11>]" );
            lines.push( view_name );

            lines.push( clear_name );

            if( !is_admin_session )
               lines.push( creds_remove_help );
            else
               lines.push( creds_remove_help + " [<pin>]" );

            lines.push( creds_retain_help + " [partial]" );

            lines.push( javascripts_employ_help + " <name> [<argument>]" );
            lines.push( javascripts_reload_help + " [<name>]" );
            lines.push( javascripts_result_help + " [<name>]" );
            lines.push( javascripts_unload_help + " [<name>]" );

            lines.push( stylesheets_reload_help + " [<name>]" );
            lines.push( stylesheets_unload_help + " [<name>]" );

            lines.sort( word_length_compare );

            data = lines.join( "\n" );
         }

         document.getElementById( "user_arbitrary" ).value = data;

         var prefix = data;

         var pos = data.indexOf( " " );

         if( pos > 0 )
            prefix = data.substring( 0, pos );

         if( !started_session
          && ( prefix.startsWith( admin_session_name ) || prefix.startsWith( standard_session_name ) ) )
         {
            started_session = true;

            if( prefix == admin_session_name )
               is_admin_session = true;
            else
               is_admin_session = false;

            document.getElementById( "user_device" ).value = ciyam.device;

            var target = owner;

            if( target == 0 )
               target = self;

            if( target != 0 )
            {
               console.log( "target: " + target );

               test_web_channel.postMessage( target + "=" + ciyam.access + ","
                + ciyam.device + "," + ciyam.hashed + "," + ciyam.sessid + "," + ciyam.unique );
            }

            document.getElementById( "user_request" ).disabled = false;
         }

         var request = document.getElementById( "user_request" ).value;

         if( ( request == quit_name )
          || ( request == exit_name )
          || ( request == finish_name ) )
         {
            if( document.getElementById( "test_image" ).innerHTML != null )
               document.getElementById( "test_image" ).innerHTML = "";

            if( localStorage.getItem( "cws.hashed_" + ciyam.access ) == null )
               ciyam.hashed = "";
            else
            {
               document.getElementById( "user_password" ).value = "***";
               document.getElementById( "user_password" ).disabled = true;
            }

            started_session = false;

            document.getElementById( "user_unique" ).value = "";

            document.getElementById( "user_request" ).value = "";
            document.getElementById( "user_session" ).value = "";

            document.getElementById( "user_retain" ).disabled = true;

            document.getElementById( "user_request" ).disabled = true;

            document.getElementById( "user_droplist" ).disabled = false;
            document.getElementById( "reset_storage" ).disabled = false;

            var target = owner;

            if( target == 0 )
               target = self;

            test_web_channel.postMessage( String( target ) );

            owner = 0;
         }
         else if( document.getElementById( "user_retain" ).checked == false )
            document.getElementById( "user_retain" ).disabled = false;
      }
   }

   if( ( all_requests == "" ) && ( ciyam.sessid != "" ) )
      document.getElementById( "user_request" ).value = "";
}

// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <map>
#  include <set>
#  include <atomic>
#  include <memory>
#  include <string>
#  include <fstream>
#  include <sstream>
#  include <stdexcept>
#endif

#include "ciyam_web_session.h"

#include "sha256.h"
#include "threads.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "fs_iterator.h"
#include "crypt_stream.h"
#include "ciyam_variables.h"

using namespace std;

extern string g_none;

extern string g_css_suffix;

extern bool g_is_devt_system;

extern string g_cws_admin_token;
extern string g_cws_admin_device;

extern string g_web_session_check;

extern string g_cws_username_for_prefix;

namespace
{

#include "ciyam_constants.h"

const size_t c_cws_max_devices = 10;

const size_t c_cws_seed_reserve = 120;

const size_t c_cws_access_length = 5;

const size_t c_cws_device_length = 15;
const size_t c_cws_device_replen = 7;

const size_t c_cws_session_length = 20;

const size_t c_save_data_delay = 500;

const size_t c_admin_lock_attempts = 10;
const size_t c_admin_retry_timeout = 100;

const size_t c_min_passwd_retry_seconds = 3;

const size_t c_max_token_create_attempts = 10;

const char* const c_cws_own = "***";

const char* const c_web_demo_pin_1 = "10101";
const char* const c_web_demo_pin_2 = "10201";
const char* const c_web_demo_pin_3 = "10301";

const char* const c_web_lock_suffix = ".lock";

const char* const c_web_command_suffix = ".command";
const char* const c_web_message_suffix = ".message";
const char* const c_web_session_suffix = ".session";
const char* const c_web_started_suffix = ".started";

const char* const c_cws_uri_suffix_help = "help";
const char* const c_cws_uri_suffix_access = "access";
const char* const c_cws_uri_suffix_ssheet = "ssheet";
const char* const c_cws_uri_suffix_unlock = "unlock";
const char* const c_cws_uri_suffix_devices = "devices";
const char* const c_cws_uri_suffix_sessions = "sessions";

const char* const c_cws_uri_suffix_access_prefix = "access/";
const char* const c_cws_uri_suffix_ssheet_prefix = "ssheet/";
const char* const c_cws_uri_suffix_unlock_prefix = "unlock/";
const char* const c_cws_uri_suffix_sessions_prefix = "sessions/";

const char* const c_cws_request_access_create_op_secret = "-secret";
const char* const c_cws_request_access_create_opt_user_info_prefix = "-user-info=";

const char* const c_cws_help_request_output = "quit\nssheet_delete\nssheet_retain\nssheet_review [<name>]\n"
 "access_create [-secret|-user-info=[<pin>:]<username>]\naccess_delete <pin>\naccess_review\nunlock_create\nunlock_employ <key>";

const char* const c_web_session_script = "web_session.cin";

const char* const c_web_session_none_response = "[none]";
const char* const c_web_session_okay_response = "[okay]";

mutex g_mutex;

bool g_cws_admin_locked = false;

atomic< size_t > g_cws_active_commands;

set< string > g_cws_access_tokens;

map< string, set< string > > g_cws_access_devices;

inline string escaped_json( const string& s )
{
   return escaped( s, "/\"", '\\', c_json_escape_specials );
}

inline size_t get_num_access_devices( const string& token )
{
   return ( !g_cws_access_devices.count( token ) ? 0 : g_cws_access_devices[ token ].size( ) );
}

struct cws_active_command
{
   cws_active_command( bool lock_for_admin )
   {
      guard g( g_mutex );

      if( lock_for_admin )
      {
         for( size_t i = 0; i < c_admin_lock_attempts; i++ )
         {
            if( !g_cws_active_commands )
               break;

            msleep( c_admin_retry_timeout );
         }

         if( g_cws_active_commands )
            throw runtime_error( "timed out trying to lock for adminstration" );

         g_cws_admin_locked = true;
      }

      ++g_cws_active_commands;
   }

   ~cws_active_command( )
   {
      --g_cws_active_commands;
   }
};

bool is_pin_token( const string& token )
{
   bool rc = true;

   for( size_t i = 0; i < token.length( ); i++ )
   {
      if( ( token[ i ] < '0' ) || ( token[ i ] > '9' ) )
      {
         rc = false;

         break;
      }
   }

   return rc;
}

string create_or_check_device( const string* p_device = 0, bool* p_locked = 0 )
{
   string device( p_device ? *p_device : uuid( ).as_string( ).substr( 0, c_cws_device_length ) );

   // NOTE: Creates the "admin" device file
   // if is not already found (which should
   // only occur before the system identity
   // has been created).
   if( g_cws_admin_device.empty( ) )
   {
      if( p_device )
         throw runtime_error( "unexpected 'create_or_check_device' call without admin device" );

      temp_umask tum( 077 );

      write_file( c_web_device_name_admin, device );

      g_cws_admin_device = device;
   }
   else
   {
      // NOTE: First removes a number of characters from the end
      // of the device string and then hashes the smaller string
      // with the system identity string in order to append back
      // the identical number of characters from the hash digest
      // (which ensures that this device string is verifiable by
      // repeating the same process).
      device.erase( c_cws_device_length - c_cws_device_replen );

      string identity( get_check_identity( ) );

      if( identity.empty( ) )
      {
         if( p_locked )
            *p_locked = true;
         else
            throw runtime_error( "system identity is not available" );
      }

      sha256 hash( device + identity );

      device += hash.get_digest_as_string( ).substr( 0, c_cws_device_replen );
   }

   return device;
}

bool verify_whether_device_is_valid( const string& device )
{
   bool retval = false;

   if( device.length( ) == c_cws_device_length )
   {
      if( device == g_cws_admin_device )
         retval = true;
      else
      {
         bool locked = false;

         // NOTE: If the system identity is locked (and no identity
         // check file exists) will just have to assume any devices
         // are valid (which should not matter as very few commands
         // other than "unlock" will be able to be performed whilst
         // the system remains locked).
         string check( create_or_check_device( &device, &locked ) );

         if( locked || ( check == device ) )
            retval = true;
      }
   }

   return retval;
}

string create_empty_token_file( const string& file_prefix, printable_type ptype, const string* p_suffix = 0 )
{
   guard g( g_mutex );

   bool okay = false;

   string suffix( p_suffix ? *p_suffix : string( ) );

   string token_file_prefix( file_prefix );

   if( token_file_prefix.empty( ) )
      throw runtime_error( "unexpected empty file prefix" );

   if( token_file_prefix[ token_file_prefix.length( ) - 1 ] != '_' )
      token_file_prefix += '_';

   bool suffix_supplied = !suffix.empty( );

   for( size_t i = 0; i < c_max_token_create_attempts; i++ )
   {
      if( !suffix_supplied )
         suffix = random_characters( 5, 0, ptype );

      if( suffix == c_admin )
         continue;

      if( ptype == e_printable_type_numeric )
      {
         if( has_user_info( suffix ) )
            continue;

         okay = true;

         add_new_user_info( suffix );
      }
      else
      {
         string token_file( token_file_prefix + suffix );

         if( file_exists( token_file ) )
            continue;

         okay = true;

         file_touch( token_file, 0, true );
      }

      break;
   }

   if( !okay )
      throw runtime_error( "was unable to create a token file" );

   return suffix;
}

void remove_web_access( const string& token, const string& token_file )
{
   guard g( g_mutex );

   if( file_exists( token_file ) )
   {
      string old_token( opt_buffer_file( token_file ) );

      file_remove( token_file );

      if( !old_token.empty( )
       && ( old_token.length( ) < ( c_sha256_digest_size * 2 ) ) )
      {
         string old_token_file( token_file );

         replace( old_token_file, token, old_token );

         file_remove( old_token_file );
      }
   }
}

bool has_web_session_access_token( const string& token,
 const string& token_file, const string& credentials, string& pin )
{
   guard g( g_mutex );

   bool retval = false;

   temp_umask tum( 077 );

   if( token == c_admin )
   {
      if( !file_exists( token_file ) )
      {
         if( pin.empty( ) )
            pin = random_characters( 5, 0, e_printable_type_numeric );

         ofstream outf( token_file.c_str( ), ios::out );

         outf << pin;

         outf.close( );

         if( outf.good( ) )
         {
            retval = true;

            g_cws_admin_token = pin;

            string token_pin_file( token_file.substr( 0, token_file.rfind( '_' ) ) + '_' + pin );

            file_touch( token_pin_file, 0, true );
         }
      }
   }
   else
   {
      bool has_token = false;
      bool is_not_empty = false;

      bool is_pin = is_pin_token( token );

      has_token = file_exists( token_file );

      if( has_token )
         is_not_empty = file_size( token_file );

      if( is_pin && !has_token )
         has_token = has_user_info( token, &is_not_empty );

      if( is_not_empty )
         retval = true;
      else
      {
         if( !is_pin )
         {
            string prefix( token_file.substr( 0, token_file.rfind( '_' ) ) );

            // NOTE: It is expected that an "access_token" command using the
            // "secret" option had been issued previously so will now create
            // an empty access PIN file (and also set a system variable with
            // the name "@cws_token_<token>" to the PIN so that applications
            // can find it assuming they had been given the "token" before).
            pin = create_empty_token_file( prefix, e_printable_type_numeric );

            file_remove( token_file );

            string cws_token_var_name(
             get_special_var_name( e_special_var_cws_token ) );

            set_system_variable( cws_token_var_name + '_' + token, pin );
         }
         else
         {
            if( credentials.empty( ) )
            {
               pin = token;

               retval = true;
            }
            else
            {
               string::size_type pos = credentials.find( ':' );

               if( pos == string::npos )
                  throw runtime_error( "requires 'passwd=<user>:<password>' (in base64) for credentials" );

               string name, password;

               name = credentials.substr( 0, pos );
               password = credentials.substr( pos + 1 );

               if( ( name != c_admin ) && has_user_name( name ) )
                  throw runtime_error( "user name '" + name + "' has been taken" );

               string pwd_hash( sha256( password ).get_digest_as_string( ) );

               if( name != c_admin )
                  set_new_user_info( token, name, pwd_hash );
               else
               {
                  ofstream outf( token_file );

                  outf << pwd_hash;
               }

               retval = true;
            }
         }
      }
   }

   return retval;
}

}

bool process_cws_request( http_request_type request_type, const string& uri_suffix,
 const cws_paramaters& cws_params, bool& use_none_response, string& response, string& error )
{
   bool found = false;

   string prefix( c_web_access_prefix );

   string access( cws_params.access );
   string device( cws_params.device );
   string passwd( cws_params.passwd );
   string payload( cws_params.payload );
   string request( cws_params.request );
   string session( cws_params.session );

   if( uri_suffix.find( c_cws_uri_suffix_sessions_prefix ) == 0 )
      session = uri_suffix.substr( strlen( c_cws_uri_suffix_sessions_prefix ) );

   bool is_json_output = cws_params.is_json_output;

   bool is_get_request = ( request_type == e_http_request_type_get );
   bool is_put_request = ( request_type == e_http_request_type_put );
   bool is_post_request = ( request_type == e_http_request_type_post );
   bool is_delete_request = ( request_type == e_http_request_type_delete );

   string request_args;

   if( !request.empty( ) && ( request[ 0 ] == '-' ) )
      request_args = request;
   else
   {
      string::size_type pos = request.find( ' ' );

      if( pos != string::npos )
      {
         request_args = request.substr( pos + 1 );

         request.erase( pos );
      }
   }

   string access_file( prefix + access ); // i.e. ".web_access.<access>"

   string pin;

   string access_seed;

   access_seed.reserve( c_cws_seed_reserve );

   bool is_locked = false;

   bool is_identity_none = !has_identity( &is_locked );

   bool is_identity_reset = ( access.length( ) > c_cws_access_length );

   // NOTE: Only permits an "identity reset" to occur
   // if the system identity has not yet been created
   // or if it is currently locked.
   if( error.empty( ) && !is_locked
    && is_identity_reset && !is_identity_none )
      // FUTURE: This message should be handled as a server string message.
      error = "System identity is not currently locked.";

   bool lock_for_admin = ( error.empty( )
    && ( access != c_admin ) && ( is_identity_none || is_identity_reset ) );

   cws_active_command active_command( lock_for_admin );

   // NOTE: An "identity reset" can be used
   // to either change the "admin password"
   // or create the system identity.
   if( error.empty( ) && is_identity_reset )
   {
      string demo_pin;

      if( access == c_demo_entropy_1 )
         demo_pin = c_web_demo_pin_1;
      else if( access == c_demo_entropy_2 )
         demo_pin = c_web_demo_pin_2;
      else if( access == c_demo_entropy_3 )
         demo_pin = c_web_demo_pin_3;

      if( is_identity_none )
      {
         access_seed = access;

         access = c_admin;

         access_file = prefix + access;
      }
      else if( file_exists( c_ciyam_server_sid_chk_file ) )
      {
         string sid_chk_data( buffer_file( c_ciyam_server_sid_chk_file ) );

         sha256 hash( access );

         hash.update( hash.get_digest_as_string( ) );

         if( hash.get_digest_as_string( ) == sid_chk_data )
         {
            access_seed = access;

            access = c_admin;

            access_file = prefix + access;

            remove_web_access( access, access_file );
         }
         else
            // FUTURE: This message should be handled as a server string message.
            error = "System identity mismatch (incorrect mnemonics?).";
      }

      if( access == c_admin )
         pin = demo_pin;
   }

   // NOTE: Only allow administration to occur
   // if using the correct "admin" access PIN.
   if( g_cws_admin_locked && ( access != c_admin ) )
   {
      bool okay = true;

      if( g_cws_active_commands > 1 )
         okay = false;
      else
      {
         string admin_pin( opt_buffer_file( prefix + c_admin ) );

         if( access != admin_pin )
            okay = false;
      }

      if( !okay )
         // FUTURE: This message should be handled as a server string message.
         error = "System is currently locked for administration.";
   }

   if( is_locked && !passwd.empty( ) )
      // FUTURE: This message should be handled as a server string message.
      error = "System is currently locked for administration.";

   if( error.empty( ) && !access.empty( ) )
   {
      bool has_access_file = has_web_session_access_token( access, access_file, passwd, pin );

      if( has_access_file && pin.empty( ) && !g_cws_access_tokens.count( access ) )
         g_cws_access_tokens.insert( access );

      // NOTE: If is "admin locked" or the system identity is "unknown"
      // and the access token matches the "admin" PIN then will set the
      // identity (after first hardening the password that is then used
      // to encrypt the entropy provided in the "request").
      if( has_access_file && !request.empty( )
       && !passwd.empty( ) && ( is_identity_none || g_cws_admin_locked ) )
      {
         string admin_pin( buffer_file( prefix + c_admin ) );

         if( access == admin_pin )
         {
            guard g( g_mutex );

            string hardened( passwd );

            harden_key_with_hash_rounds( hardened, hardened, hardened, c_key_rounds_multiplier );

            data_encrypt( request, request, hardened );

            try
            {
               set_identity( request );

               set_identity( passwd, request.c_str( ) );
            }
            catch( exception& x )
            {
               error = x.what( );
            }

            clear_key( hardened );

            g_cws_admin_locked = false;
         }

         clear_key( request );
      }
   }

   clear_key( passwd );

   string request_and_args;

   if( error.empty( ) && !request.empty( ) )
   {
      request_and_args = request;

      if( !request_args.empty( ) )
         request_and_args += ' ' + request_args;
   }

   // NOTE: A seed will only be needed when
   // no existing system identity is found.
   bool is_seed_needed = is_identity_none;

   if( !error.empty( ) || access.empty( ) )
   {
      if( error.empty( ) )
         // FUTURE: This message should be handled as a server string message.
         error = "Need a valid access token to use a web session.";
   }
   else if( !pin.empty( ) )
   {
      string seed;

      seed.reserve( c_cws_seed_reserve );

      if( !access_seed.empty( ) )
         seed = access_seed;
      else if( !is_seed_needed )
      {
         if( access == c_admin )
            seed = c_admin;
         else
         {
            seed = g_none;

            string suggestion( get_system_variable( g_cws_username_for_prefix + pin ) );

            if( !suggestion.empty( ) )
            {
               seed += ' ' + suggestion;

               set_system_variable( g_cws_username_for_prefix + access, "" );
            }
         }
      }
      else
      {
         get_mnemonics_or_hex_seed( seed, "" );
         get_mnemonics_or_hex_seed( seed, seed );
      }

      found = true;

      if( !is_json_output )
         response = pin + ' ' + seed;
      else
         response = "{\"pin\":\"" + pin + "\", \"seed\":\"" + seed + "\"}";

      clear_key( seed );
      clear_key( access_seed );
   }
   else if( !g_cws_access_tokens.count( access ) )
      // FUTURE: This message should be handled as a server string message.
      error = "Web session access token '" + access + "' is invalid.";
   else if( uri_suffix == c_cws_uri_suffix_devices )
   {
      size_t num_devices = get_num_access_devices( access );

      if( num_devices >= c_cws_max_devices )
         // FUTURE: This message should be handled as a server string message.
         error = "Maximum devices have been created for web session access token '" + access + "'.";
      else
      {
         found = true;

         string new_device( create_or_check_device( ) );

         if( !is_json_output )
            response = new_device;
         else
            response = "{\"new_device\":\"" + new_device + "\"}";
      }
   }
   else
   {
      if( !are_hex_nibbles( device, false ) || ( device.length( ) != c_cws_device_length ) )
         // FUTURE: This message should be handled as a server string message.
         error = "Invalid device identity '" + device + "'.";
      else if( !g_cws_access_devices[ access ].count( device ) )
      {
         if( !verify_whether_device_is_valid( device ) )
            // FUTURE: This message should be handled as a server string message.
            error = "Invalid device identity '" + device + "'.";
         else
         {
            size_t num_devices = get_num_access_devices( access );

            if( num_devices < c_cws_max_devices )
               g_cws_access_devices[ access ].insert( device );
            else
               // FUTURE: This message should be handled as a server string message.
               error = "Maximum devices have been created for web session access token '" + access + "'.";
         }
      }

      if( error.empty( ) )
      {
         string var_prefix( get_special_var_name( e_special_var_web ) + '.' );

         string output_file_name( "/tmp/ciyam." + var_prefix.substr( 1 ) + access + '.' + device );

         string web_lock_name( var_prefix + access + '.' + device + c_web_lock_suffix );

         string web_command_name( var_prefix + access + '.' + device + c_web_command_suffix );
         string web_message_name( var_prefix + access + '.' + device + c_web_message_suffix );
         string web_session_name( var_prefix + access + '.' + device + c_web_session_suffix );
         string web_started_name( var_prefix + access + '.' + device + c_web_started_suffix );

         if( is_post_request && ( uri_suffix == c_cws_uri_suffix_sessions ) )
         {
            string access_token( opt_buffer_file( access_file ) );

            if( access_token.empty( ) )
               access_token = get_user_pwd_hash( access );

            string unique( uuid( ).as_string( ) );

            if( !is_json_output )
               response = unique;
            else
               response = "{\"unique\":\"" + unique + "\"}";

            int64_t now = unix_time( );

            // NOTE: Force authentication retry attempts to be slow.
            if( !set_system_variable( web_lock_name, to_string( now ), string( "" ) ) )
            {
               int64_t was = from_string< int64_t >( get_system_variable( web_lock_name ) );

               if( now < ( was + c_min_passwd_retry_seconds ) )
                  // FUTURE: This message should be handled as a server string message.
                  error = "Web session is currently busy (try again shortly).";
               else
                  set_system_variable( web_lock_name, to_string( now ) );
            }

            // NOTE: If is not currently handling another session
            // request for the same device then the session value
            // is obtained from the leading characters of a hash.
            // The hash is itself obtained using a "checked" hash
            // (being the "access_token" and "device" values) and
            // then combining its digest with the "unqiue" string
            // to determine a "combined" hash.
            if( error.empty( ) )
            {
               found = true;

               sha256 hash_checked( access_token + device );

               sha256 hash_combined( hash_checked.get_digest_as_string( ) + unique );

               string digest( hash_combined.get_digest_as_string( ) );

               set_system_variable( web_session_name, digest.substr( 0, c_cws_session_length ) );

               // NOTE: This "command" will force the web session script to
               // check that the session identity it was given when started
               // matches what has just been set (and to terminate if not).
               set_system_variable( web_command_name, g_web_session_check );
            }
         }
         else
         {
            if( session != get_system_variable( web_session_name ) )
               // FUTURE: This message should be handled as a server string message.
               error = "This web session is not valid (or has expired).";
            else
            {
               if( has_system_variable( web_lock_name ) )
               {
                  string when_locked( get_system_variable( web_lock_name ) );

                  set_system_variable( web_lock_name, "" );
                  set_system_variable( web_message_name, "" );

                  set_system_variable( web_started_name, when_locked );
               }

               if( uri_suffix == c_cws_uri_suffix_help )
               {
                  found = true;

                  if( !is_json_output )
                     response = c_cws_help_request_output;
                  else
                     response = "{\"commands\":\"" + escaped_json( c_cws_help_request_output ) + "\"}\n";
               }
               else if( is_delete_request && ( uri_suffix.find( c_cws_uri_suffix_sessions_prefix ) == 0 ) )
               {
                  found = true;

                  if( !is_json_output )
                     response = "Session terminated.";
                  else
                     response = "{\"message\":\"Session terminated.\"}\n";

                  set_system_variable( web_message_name, "" );
                  set_system_variable( web_session_name, "" );
                  set_system_variable( web_started_name, "" );

                  // NOTE: In case the access device was a temporary
                  // one will remove it now (so it is recommended to
                  // always terminate each session explicitly).
                  if( g_cws_access_devices.count( access ) )
                     g_cws_access_devices[ access ].erase( device );
               }
               else if( is_post_request && ( uri_suffix == c_cws_uri_suffix_unlock ) )
               {
                  if( access != g_cws_admin_token )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Unlock keys can only be created by the administrator.";
                  else if( is_locked )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Unlock keys are not able to be created whilst currently locked.";
                  else
                  {
                     found = true;

                     string unlock_key( create_unlock_sid_hash_key( false, false ) );

                     if( !is_json_output )
                        response = unlock_key;
                     else
                        response = "{\"unlock_key\":\"" + escaped_json( unlock_key ) + "\"}\n";
                  }
               }
               else if( is_post_request
                && ( uri_suffix.find( c_cws_uri_suffix_unlock_prefix ) == 0 ) )
               {
                  string key( uri_suffix.substr( strlen( c_cws_uri_suffix_unlock_prefix ) ) );

                  if( !has_system_variable( e_special_var_sid_locked ) )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Unlock key is not usuable when the system is not currently locked.";
                  else
                  {
                     try
                     {
                        set_identity( key );

                        found = true;
                     }
                     catch( exception& x )
                     {
                        error = x.what( );
                     }
                  }
               }
               else if( is_get_request && ( uri_suffix == c_cws_uri_suffix_ssheet ) )
               {
                  found = true;

                  file_filter ff;

                  fs_iterator fs( get_web_root( ), &ff );

                  string all_ssheets( g_none );

                  bool has_cached_ssheets = has_system_variable( e_special_var_cws_styles );

                  if( !has_cached_ssheets )
                     all_ssheets = g_none;
                  else
                     all_ssheets = get_system_variable( e_special_var_cws_styles );

                  if( !has_cached_ssheets )
                  {
                     set< string > css_files;

                     size_t suffix_length = g_css_suffix.length( );

                     while( fs.has_next( ) )
                     {
                        string next( fs.get_name( ) );

                        if( next.size( ) > suffix_length )
                        {
                           string::size_type pos = next.rfind( g_css_suffix );

                           if( pos == ( next.length( ) - suffix_length ) )
                              css_files.insert( next.substr( 0, pos ) );
                        }
                     }

                     for( set< string >::iterator i = css_files.begin( ); i!= css_files.end( ); ++i )
                        all_ssheets += ' ' + *i;

                     set_system_variable( e_special_var_cws_styles, all_ssheets );
                  }

                  string::size_type pos = all_ssheets.find( ' ' );

                  if( pos != string::npos )
                     all_ssheets.erase( 0, pos + 1 );

                  if( !all_ssheets.empty( ) )
                  {
                     vector< string > styles;

                     split( all_ssheets, styles, ' ' );

                     string allowed_ssheets;

                     bool is_admin = ( access == g_cws_admin_token );

                     // NOTE: Replace "own" style with "***" and
                     // filter other access PIN stylesheets from
                     // appearing (unless is the administrator).
                     for( size_t i = 0; i < styles.size( ); i++ )
                     {
                        string next( styles[ i ] );

                        if( next == access )
                           next = c_cws_own;

                        if( !is_admin && is_pin_token( next ) )
                           continue;

                        if( !allowed_ssheets.empty( ) )
                           allowed_ssheets += ' ';

                        allowed_ssheets += next;
                     }

                     all_ssheets = allowed_ssheets;
                  }

                  if( all_ssheets.empty( ) )
                     use_none_response = true;
                  else
                  {
                     if( !is_json_output )
                        response = all_ssheets;
                     else
                        response = "{\"all_ssheets\":\"" + escaped_json( all_ssheets ) + "\"}\n";
                  }
               }
               else if( is_put_request && ( uri_suffix == c_cws_uri_suffix_ssheet ) )
               {
                  string file_name( get_web_root( ) + '/' + access + g_css_suffix );

                  if( payload.empty( ) )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Stylesheet data was not provided.";
                  else if( is_locked )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Stylesheet data cannot be saved whilst system is locked.";
                  else
                  {
                     found = true;

                     write_file( file_name, payload );

                     set_system_variable( e_special_var_cws_styles, "" );

                     // NOTE: This delay is used to reduce possible I/O overloading.
                     msleep( c_save_data_delay );
                  }
               }
               else if( is_get_request && ( uri_suffix.find( c_cws_uri_suffix_ssheet_prefix ) == 0 ) )
               {
                  string name( uri_suffix.substr( strlen( c_cws_uri_suffix_ssheet_prefix ) ) );

                  string file_name( get_web_root( ) + '/' + name + g_css_suffix );

                  if( !file_exists( file_name ) )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Stylesheet '" + file_name + "' was not found.";
                  else
                  {
                     found = true;

                     string style_data( opt_buffer_file( file_name ) );

                     if( !is_json_output )
                        response = style_data;
                     else
                        response = "{\"style_data\":\"" + escaped_json( style_data ) + "\"}\n";
                  }
               }
               else if( is_delete_request && ( uri_suffix == c_cws_uri_suffix_ssheet ) )
               {
                  if( is_locked )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Stylesheet data cannot be erased whilst the system is locked.";
                  else
                  {
                     string file_name( get_web_root( ) + '/' + access + g_css_suffix );

                     file_remove( file_name );

                     if( !file_exists( file_name ) )
                     {
                        found = true;

                        set_system_variable( e_special_var_cws_styles, "" );
                     }
                     else
                        // FUTURE: This message should be handled as a server string message.
                        error = "Stylesheet could not be erased (contact the administrator).";
                  }
               }
               else if( is_post_request && ( uri_suffix == c_cws_uri_suffix_access ) )
               {
                  if( access != g_cws_admin_token )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Access tokens can only be maintained by the administrator.";
                  else
                  {
                     bool is_secret = false;

                     string pin, suggested_username;

                     if( request_args == c_cws_request_access_create_op_secret )
                        is_secret = true;

                     if( ( request_args.find( c_cws_request_access_create_opt_user_info_prefix ) == 0 )
                      && ( request_args.length( ) > ( strlen( c_cws_request_access_create_opt_user_info_prefix ) + 2 ) ) )
                     {
                        request_args.erase( 0, strlen( c_cws_request_access_create_opt_user_info_prefix ) );

                        string::size_type pos = request_args.find( ':' );

                        if( pos != string::npos )
                        {
                           pin = request_args.substr( 0, pos );

                           request_args.erase( 0, pos + 1 );
                        }

                        suggested_username = request_args;

                        request_args.erase( );
                     }

                     if( !is_secret && !request_args.empty( ) )
                        // FUTURE: This message should be handled as a server string message.
                        error = "Invalid argument(s) '" + request_args + "' for access token creation.";
                     else
                     {
                        found = true;

                        temp_umask tum( 077 );

                        string access_token( create_empty_token_file( prefix,
                         ( !is_secret ? e_printable_type_numeric : e_printable_type_alpha_mixed ), &pin ) );

                        if( !suggested_username.empty( ) )
                           set_system_variable( g_cws_username_for_prefix + access_token, suggested_username );

                        if( !is_json_output )
                           response = access_token;
                        else
                           response = "{\"access_token\":\"" + access_token + "\"}\n";
                     }
                  }
               }
               else if( is_delete_request && ( uri_suffix.find( c_cws_uri_suffix_access_prefix ) == 0 ) )
               {
                  if( access != g_cws_admin_token )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Access tokens can only be maintained by the administrator.";
                  else
                  {
                     string pin( uri_suffix.substr( strlen( c_cws_uri_suffix_access_prefix ) ) );

                     if( !has_user_info( pin ) )
                        // FUTURE: This message should be handled as a server string message.
                        error = "Unkknown access token '" + pin + "' for removal.";
                     else
                     {
                        found = true;

                        remove_user_info_from_storage( pin );
                     }
                  }
               }
               else if( is_get_request && ( uri_suffix == c_cws_uri_suffix_access ) )
               {
                  if( access != g_cws_admin_token )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Access tokens can only be maintained by the administrator.";
                  else
                  {
                     found = true;

                     string all_user_info( get_all_user_pins( ) );

                     if( all_user_info.empty( ) )
                        use_none_response = true;
                     else
                     {
                        if( !is_json_output )
                           response = all_user_info;
                        else
                           response = "{\"access_tokens\":\"" + escaped_json( all_user_info ) + "\"}\n";
                     }
                  }
               }
               else
               {
                  string script_name( c_web_session_script );

                  bool running = has_system_variable( web_message_name );

                  if( !running )
                  {
#ifndef SSL_SUPPORT
                     string cmd( "./ciyam_client -quiet -no_prompt -no_stderr -exec=\"<"
#else
                     string cmd( "./ciyam_client -tls -quiet -no_prompt -no_stderr -exec=\"<"
#endif
                      + script_name + ' ' + access + ' ' + device + ' ' + session + "\" > /dev/null &" );

                     int rc = system( cmd.c_str( ) );

                     ( void )rc;

                     msleep( 200 );
                  }

                  // NOTE: Allows for a couple of
                  // seconds to start the session
                  // script (which will result in
                  // an error if it's not found).
                  for( size_t i = 0; i < 9; i++ )
                  {
                     running = has_system_variable( web_message_name );

                     if( running )
                        break;

                     msleep( 200 );
                  }

                  if( !running )
                  {
                     set_system_variable( web_session_name, "" );
                     set_system_variable( web_started_name, "" );

                     // FUTURE: This message should be handled as a server string message.
                     error = "Was unable to start a web session with access token '" + access + "'.";
                  }
                  else
                  {
                     found = true;

                     bool allowed_command = true;

                     if( !request.empty( ) )
                     {
                        if( !g_is_devt_system || ( access != g_cws_admin_token ) )
                           allowed_command = false;
                        else
                        {
                           if( ( request[ 0 ] < 'a' ) || ( request[ 0 ] > 'z' ) )
                              allowed_command = false;
                        }
                     }

                     if( !allowed_command )
                        response = "[bad]";
                     else
                     {
                        if( !request.empty( ) )
                           set_system_variable( web_command_name, request_and_args );
                        else
                           set_system_variable( web_command_name, "variable " + web_message_name );

                        for( size_t i = 0; i < 10; i++ )
                        {
                           msleep( 150 );

                           if( file_exists( output_file_name ) )
                           {
                              response = buffer_file( output_file_name );

                              response = trim( response, false, false, "\n" );

                              file_remove( output_file_name );

                              break;
                           }
                        }
                     }
                  }
               }

               if( found && response.empty( ) )
               {
                  string okay_reponse( use_none_response ? c_web_session_none_response : c_web_session_okay_response );

                  if( !is_json_output )
                     response = okay_reponse;
                  else
                     response = "{\"response\":\"" + okay_reponse + "\"}\n";
               }
            }
         }
      }
   }

   return found;
}

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

#include "sio.h"
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

extern string g_none_tag;
extern string g_none_var;

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

const size_t c_save_data_delay = 250;

const size_t c_admin_lock_attempts = 10;
const size_t c_admin_retry_timeout = 100;

const size_t c_min_passwd_retry_seconds = 3;

const size_t c_max_token_create_attempts = 10;

constexpr const char* c_force = "force";

constexpr const char* c_cws_own = "***";

constexpr const char* c_css_suffix = ".css";

constexpr const char* c_web_demo_pin_1 = "10101";
constexpr const char* c_web_demo_pin_2 = "10201";
constexpr const char* c_web_demo_pin_3 = "10301";

constexpr const char* c_web_lock_suffix = ".lock";

constexpr const char* c_web_command_suffix = ".command";
constexpr const char* c_web_message_suffix = ".message";
constexpr const char* c_web_session_suffix = ".session";
constexpr const char* c_web_started_suffix = ".started";
constexpr const char* c_web_storage_suffix = ".storage";

constexpr const char* c_cws_uri_suffix_help = "help";
constexpr const char* c_cws_uri_suffix_users = "users";
constexpr const char* c_cws_uri_suffix_devices = "devices";
constexpr const char* c_cws_uri_suffix_sessions = "sessions";
constexpr const char* c_cws_uri_suffix_storages = "storages";
constexpr const char* c_cws_uri_suffix_stylesheets = "stylesheets";
constexpr const char* c_cws_uri_suffix_unlock_keys = "unlock-keys";
constexpr const char* c_cws_uri_suffix_storage_modules = "storage-modules";

constexpr const char* c_cws_uri_suffix_users_prefix = "users/";
constexpr const char* c_cws_uri_suffix_sessions_prefix = "sessions/";

constexpr const char* c_cws_uri_suffix_storages_prefix = "storages/";
constexpr const char* c_cws_uri_suffix_stylesheets_prefix = "stylesheets/";
constexpr const char* c_cws_uri_suffix_unlock_keys_prefix = "unlock-keys/";
constexpr const char* c_cws_uri_suffix_storage_modules_prefix = "storage-modules/";
constexpr const char* c_cws_uri_suffix_storage_instances_prefix = "storage-instances/";

constexpr const char* c_cws_uri_suffix_enums_extra = "/enums";
constexpr const char* c_cws_uri_suffix_lists_extra = "/lists";
constexpr const char* c_cws_uri_suffix_views_extra = "/views";

constexpr const char* c_cws_uri_suffix_enums_extra_prefix = "/enums/";
constexpr const char* c_cws_uri_suffix_lists_extra_prefix = "/lists/";
constexpr const char* c_cws_uri_suffix_views_extra_prefix = "/views/";

constexpr const char* c_cws_request_users_create_options_secret = "secret";
constexpr const char* c_cws_request_users_create_options_suggested = "suggested";

// NOTE: This help is only intended for the "test_web_session.html" page which will translate this more "user friendly" syntax.
constexpr const char* c_cws_help_request_output = "quit\nattach storage <name>\n"
 "delete stylesheet\nemploy unlock-key <key>\nretain stylesheet\nreview storages\nreview stylesheet[s] [<name>]\n"
 "review storage-modules [<id>/enums|lists|views[/<item_id>]]\nreview storage-instances <id>/<cid>[/<key>] [[key=<key>;][num=[-|+]<num>;][path=<path>;][fields=<fields]]";

constexpr const char* c_cws_help_request_admin_output = "quit\nattach storage <name>\ncreate user [secret|suggested=[<pin>:][<username>]]\n"
 "create unlock-key\ndelete user <pin>\ndelete stylesheet\nemploy unlock-key <key>\nretain stylesheet\nreview users\nreview storages\nreview stylesheet[s] [<name>]\n"
 "review storage-modules [<id>/enums|lists|views[/<item_id>]]\nreview storage-instances <id>/<cid>[/<key>] [[key=<key>;][num=[-|+]<num>;][path=<path>;][fields=<fields]]";

constexpr const char* c_web_session_script = "web_session.cin";

constexpr const char* c_web_session_none_response = "[none]";
constexpr const char* c_web_session_okay_response = "[okay]";

constexpr const char* c_web_session_unknown_response = "[unknown]";

constexpr const char* c_storage_attribute_id = "id";
constexpr const char* c_storage_attribute_user_info = "user_info";

constexpr const char* c_storage_module_enums_id_prefix = "enums.id=";
constexpr const char* c_storage_module_lists_id_prefix = "lists.id=";
constexpr const char* c_storage_module_views_id_prefix = "views.id=";

constexpr const char* c_storage_module_enums_available_query = "enums.*.@id,name";
constexpr const char* c_storage_module_lists_available_admin_query = "lists.*.@id,@cid,name,class";
constexpr const char* c_storage_module_views_available_admin_query = "views.*.@id,@cid,name,class";
constexpr const char* c_storage_module_lists_available_non_admin_query = "lists.*.@id,@cid,name,class,~!type=admin";
constexpr const char* c_storage_module_views_available_non_admin_query = "views.*.@id,@cid,name,class,~!type=admin";

constexpr const char* c_storage_module_instance_options_key = "key";
constexpr const char* c_storage_module_instance_options_num = "num";
constexpr const char* c_storage_module_instance_options_path = "path";
constexpr const char* c_storage_module_instance_options_fields = "fields";

mutex g_mutex;

bool g_cws_admin_locked = false;

atomic< size_t > g_cws_active_commands;

set< string > g_cws_access_tokens;

map< string, set< string > > g_cws_access_devices;

map< string, unique_ptr< sio_graph > > g_model_meta_data;

struct session_info
{
   string access;
   string device;

   string user_key;
   string username;

   int64_t init_request;
   int64_t last_request;
};

map< string, unique_ptr< session_info > > g_session_info;

inline string escaped_json( const string& s )
{
   return escaped( s, "/\"", '\\', c_json_escape_specials );
}

string as_json_array( const string& name, const vector< string >& array )
{
   string retval;

   if( name.empty( ) )
      retval = "   [\n   ";
   else
      retval = "{\n \"" + name + "\":\n [\n";

   for( size_t i = 0; i < array.size( ); i++ )
   {
      if( i > 0 )
         retval += ',';

      retval += "  \"" + escaped_json( array[ i ] ) + '"';
   }

   if( name.empty( ) )
      retval += "\n   ]";
   else
      retval += "\n ]\n}";

   return retval;
}

bool has_access_token( const string& access_token )
{
   guard g( g_mutex );

   return g_cws_access_tokens.count( access_token );
}

void add_access_token_if_new( const string& access_token )
{
   guard g( g_mutex );

   if( !g_cws_access_tokens.count( access_token ) )
   {
      g_cws_access_tokens.insert( access_token );

      TRACE_LOG( TRACE_VERBOSE | TRACE_SESSION, "(web_session) added access " + access_token );
   }
}

size_t get_num_access_devices( const string& access_token )
{
   guard g( g_mutex );

   return ( !g_cws_access_devices.count( access_token ) ? 0 : g_cws_access_devices[ access_token ].size( ) );
}

bool has_access_device( const string& access_token, const string& device_token )
{
   guard g( g_mutex );

   bool retval = false;

   if( g_cws_access_devices.count( access_token ) )
   {
      if( g_cws_access_devices[ access_token ].count( device_token ) )
         retval = true;
   }

   return retval;
}

bool has_added_access_device( const string& access_token, const string& device_token )
{
   guard g( g_mutex );

   bool retval = false;

   size_t num_devices = get_num_access_devices( access_token );

   // NOTE: Need to check that the device has not already been added
   // (in case concurrent threads called "has_access_device" and are
   // now both calling this function to add the same device token).
   if( ( num_devices < c_cws_max_devices )
    && !g_cws_access_devices[ access_token ].count( device_token ) )
   {
      retval = true;

      g_cws_access_devices[ access_token ].insert( device_token );

      TRACE_LOG( TRACE_VERBOSE | TRACE_SESSION, "(web_session) added device " + device_token + " for access " + access_token );
   }

   return retval;
}

void remove_access_device_if_present( const string& access_token, const string& device_token )
{
   guard g( g_mutex );

   if( g_cws_access_devices.count( access_token )
    && g_cws_access_devices[ access_token ].count( device_token ) )
   {
      g_cws_access_devices[ access_token ].erase( device_token );

      TRACE_LOG( TRACE_VERBOSE | TRACE_SESSION, "(web_session) removed device " + device_token + " for access " + access_token );
   }
}

void init_session_info( const string& access, const string& device, const string& session, int64_t now )
{
   guard g( g_mutex );

   g_session_info[ session ].reset( new session_info );

   g_session_info[ session ]->access = access;
   g_session_info[ session ]->device = device;

   if( access == g_cws_admin_token )
      g_session_info[ session ]->username = c_admin;
   else
      g_session_info[ session ]->username = get_user_name( access );

   g_session_info[ session ]->init_request = now;
   g_session_info[ session ]->last_request = now;
}

void remove_session_info( const string& session )
{
   guard g( g_mutex );

   if( g_session_info.count( session ) )
      g_session_info.erase( session );
}

void update_session_info( const string& session, int64_t now )
{
   guard g( g_mutex );

   if( g_session_info.count( session ) )
      g_session_info[ session ]->last_request = now;
}

void update_session_info( const string& session, const string& user_key )
{
   guard g( g_mutex );

   if( g_session_info.count( session ) )
      g_session_info[ session ]->user_key = user_key;
}

string get_session_info_user_key( const string& session )
{
   guard g( g_mutex );

   string retval;

   if( g_session_info.count( session ) )
      retval = g_session_info[ session ]->user_key;

   return retval;
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
         {
            string force_pin( replaced( token_file, c_admin, c_force ) );

            if( file_exists( force_pin ) )
            {
               pin = opt_buffer_file( force_pin );

               file_remove( force_pin );
            }

            if( pin.empty( ) || !is_pin_token( pin ) )
               pin = random_characters( 5, 0, e_printable_type_numeric );
         }

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

const sio_graph& get_meta_data( const string& model_name )
{
   guard g( g_mutex );

   if( !g_model_meta_data.count( model_name ) )
   {
      string model_sio_file( model_name + ".fcgi.sio" );

      if( !file_exists( model_sio_file ) )
         throw runtime_error( "structured I/O file '" + model_sio_file + "' was not found" );

      ifstream inpf( model_sio_file.c_str( ) );

      sio_reader reader( inpf );

      g_model_meta_data.insert( make_pair( model_name, new sio_graph( reader ) ) );
   }

   return *g_model_meta_data[ model_name ];
}

void process_user_info_response( const string& session, string& response )
{
   if( !response.empty( ) && ( response[ 0 ] == '[' ) )
   {
      string::size_type pos = response.find( ']' );

      if( pos != string::npos )
      {
         pos = response.find( ' ' );

         string user_key;

         if( pos != string::npos )
            user_key = response.substr( 1, pos - 1 );

         pos = response.rfind( ' ' );

         if( pos != string::npos )
         {
            string username( response.substr( pos + 1 ) );

            response = username;

            update_session_info( session, user_key );
         }
      }
   }
}

string opt_prefix_uri( const string& key_ver_and_state )
{
   string opt_prefixed( key_ver_and_state );

   string::size_type pos = opt_prefixed.find( ' ' );

   if( pos != string::npos )
   {
      string key( opt_prefixed.substr( 0, pos ) );

      // NOTE: As keys can include underbars and upper case letters will prefix
      // with a URI version of the key with lower case and hyphens (followed by
      // a colon and the actual key value).
      if( key.find_first_of( "_ABCDEFGHIJKLMNOPQRSTUVWXYZ" ) != string::npos )
      {
         string uri_key;

         uri_key.reserve( key.size( ) + 1 );

         for( size_t i = 0; i < key.size( ); i++ )
         {
            char ch = key[ i ];

            if( ch == '_' )
               ch = '-';

            uri_key += tolower( ch );
         }

         uri_key += ':';

         opt_prefixed.insert( 0, uri_key );
      }
   }

   return opt_prefixed;
}

void get_field_values( const string& output, vector< string >& values )
{
   string field_info( output );

   string::size_type pos = field_info.find( ']' );

   if( pos != string::npos )
   {
      string key_ver_and_state( field_info.substr( 1, pos - 1 ) );

      key_ver_and_state = opt_prefix_uri( key_ver_and_state );

      values.push_back( key_ver_and_state );

      field_info.erase( 0, pos + 1 );

      if( !field_info.empty( ) && ( field_info[ 0 ] == ' ' ) )
         field_info.erase( 0, 1 );

      if( !field_info.empty( ) )
         split( field_info, values, ',' );
   }
}

}

void dump_session_info( ostream& os )
{
   guard g( g_mutex );

   if( !g_session_info.empty( ) )
   {
      os << "session              pin   device          init_request        last_request        username\n";
      os << "-------------------- ----- --------------- ------------------- ------------------- ---------------\n";
   }

   for( auto i = g_session_info.begin( ); i != g_session_info.end( ); i++ )
      os << i->first << ' ' << i->second->access << ' ' << i->second->device
       << ' ' << date_time( i->second->init_request ).as_string( true, false )
       << ' ' << date_time( i->second->last_request ).as_string( true, false )
       << ' ' << ( i->second->username.empty( ) ? g_none_tag : i->second->username ) << '\n';
}

bool process_cws_request( http_request_type request_type, const string& uri_suffix,
 const cws_paramaters& cws_params, bool& use_none_response, string& response, string& error )
{
   bool found = false;

   bool use_unknown_response = false;

   string prefix( c_web_access_prefix );

   string access( cws_params.access );
   string device( cws_params.device );
   string passwd( cws_params.passwd );
   string options( cws_params.options );
   string payload( cws_params.payload );
   string request( cws_params.request );
   string session( cws_params.session );

   if( uri_suffix.find( c_cws_uri_suffix_sessions_prefix ) == 0 )
      session = uri_suffix.substr( CONST_LENGTH( c_cws_uri_suffix_sessions_prefix ) );

   string instance_module_id;
   string instance_mclass_id;

   string instance_record_key;

   map< string, string > option_parameters;

   if( !options.empty( ) )
   {
      vector< string > all_options;

      split( options, all_options, ';' );

      for( size_t i = 0; i < all_options.size( ); i++ )
      {
         string next_option( all_options[ i ] );

         string::size_type pos = next_option.find( '=' );

         string value;

         if( pos != string::npos )
            value = next_option.substr( pos + 1 );

         option_parameters[ next_option.substr( 0, pos ) ] = value;
      }
   }

   if( uri_suffix.find( c_cws_uri_suffix_storage_instances_prefix ) == 0 )
   {
      string instance_info( uri_suffix.substr( CONST_LENGTH( c_cws_uri_suffix_storage_instances_prefix ) ) );

      string::size_type pos = instance_info.find( '/' );

      instance_module_id = upper( instance_info.substr( 0, pos ) );

      if( pos != string::npos )
      {
         instance_mclass_id = instance_info.substr( pos + 1 );

         pos = instance_mclass_id.find( '/' );

         if( pos != string::npos )
         {
            instance_record_key = instance_mclass_id.substr( pos + 1 );

            string key_replacement;

            if( option_parameters.count( c_storage_module_instance_options_key ) )
            {
               key_replacement = option_parameters[ c_storage_module_instance_options_key ];

               bool is_valid_key_replacement = true;

               if( key_replacement.length( ) != instance_record_key.length( ) )
                  is_valid_key_replacement = false;
               else
               {
                  for( size_t i = 0; i < key_replacement.length( ); i++ )
                  {
                     char rep_ch = key_replacement[ i ];
                     char src_ch = instance_record_key[ i ];

                     rep_ch = tolower( rep_ch );

                     if( rep_ch == '_' )
                        rep_ch = '-';

                     if( rep_ch != src_ch )
                     {
                        is_valid_key_replacement = false;

                        break;
                     }
                  }
               }

               if( is_valid_key_replacement )
                  instance_record_key = key_replacement;
               else
                  throw runtime_error( "invalid key replacement value '" + key_replacement + "'" );
            }

            instance_mclass_id.erase( pos );
         }

         to_upper( instance_mclass_id );
      }
   }

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

      if( has_access_file && pin.empty( ) )
         add_access_token_if_new( access );

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
            seed = g_none_var;

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
   else if( !has_access_token( access ) )
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
      else if( !has_access_device( access, device ) )
      {
         if( !verify_whether_device_is_valid( device ) )
            // FUTURE: This message should be handled as a server string message.
            error = "Invalid device identity '" + device + "'.";
         else
         {
            if( !has_added_access_device( access, device ) )
               // FUTURE: This message should be handled as a server string message.
               error = "Maximum devices have been created for web session access token '" + access + "'.";
         }
      }

      if( error.empty( ) )
      {
         string var_prefix( get_special_var_name( e_special_var_web ) + '.' );

         string output_file_name( "/tmp/ciyam." + var_prefix.substr( 1 ) + access + '.' + device );

         string web_lock_name( var_prefix + access + '.' + device + c_web_lock_suffix );

         string web_command_var_name( var_prefix + access + '.' + device + c_web_command_suffix );
         string web_message_var_name( var_prefix + access + '.' + device + c_web_message_suffix );
         string web_session_var_name( var_prefix + access + '.' + device + c_web_session_suffix );
         string web_started_var_name( var_prefix + access + '.' + device + c_web_started_suffix );
         string web_storage_var_name( var_prefix + access + '.' + device + c_web_storage_suffix );

         bool is_admin = ( access == g_cws_admin_token );

         int64_t now = unix_time( );

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

               string new_session( digest.substr( 0, c_cws_session_length ) );

               string old_session( get_system_variable( web_session_var_name ) );

               if( !old_session.empty( ) && ( new_session != old_session ) )
               {
                  remove_session_info( old_session );

                  TRACE_LOG( TRACE_VERBOSE | TRACE_SESSION, "(web_session) replacing "
                   "session " + old_session + " with device " + device + " for access " + access );
               }

               set_system_variable( web_session_var_name, new_session );

               init_session_info( access, device, new_session, now );

               // NOTE: This "command" will force the web session script to
               // check that the session identity it was given when started
               // matches what has just been set (and to terminate if not).
               set_system_variable( web_command_var_name, g_web_session_check );
            }
         }
         else
         {
            if( session != get_system_variable( web_session_var_name ) )
               // FUTURE: This message should be handled as a server string message.
               error = "This web session is not valid (or has expired).";
            else
            {
               if( has_system_variable( web_lock_name ) )
               {
                  string when_locked( get_system_variable( web_lock_name ) );

                  set_system_variable( web_lock_name, "" );
                  set_system_variable( web_message_var_name, "" );

                  set_system_variable( web_started_var_name, when_locked );
               }

               if( uri_suffix == c_cws_uri_suffix_help )
               {
                  found = true;

                  string help_output( !is_admin ? c_cws_help_request_output : c_cws_help_request_admin_output );

                  if( !is_json_output )
                     response = help_output;
                  else
                     response = "{\"commands\":\"" + escaped_json( help_output ) + "\"}\n";
               }
               else if( is_delete_request && ( uri_suffix.find( c_cws_uri_suffix_sessions_prefix ) == 0 ) )
               {
                  found = true;

                  if( !is_json_output )
                     response = "Session terminated.";
                  else
                     response = "{\"message\":\"Session terminated.\"}\n";

                  set_system_variable( web_message_var_name, "" );
                  set_system_variable( web_session_var_name, "" );
                  set_system_variable( web_started_var_name, "" );
                  set_system_variable( web_storage_var_name, "" );

                  TRACE_LOG( TRACE_VERBOSE | TRACE_SESSION, "(web_session) finished "
                   "session " + session + " with device " + device + " for access " + access );

                  remove_session_info( session );

                  // NOTE: In case the access device was a temporary
                  // one will remove it now (so it is recommended to
                  // always terminate each session explicitly).
                  remove_access_device_if_present( access, device );
               }
               else if( is_post_request && ( uri_suffix == c_cws_uri_suffix_unlock_keys ) )
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

                     replace( unlock_key, " ", "-" );

                     if( !is_json_output )
                        response = unlock_key;
                     else
                        response = "{\"unlock_key\":\"" + escaped_json( unlock_key ) + "\"}\n";
                  }
               }
               else if( is_get_request
                && ( uri_suffix == c_cws_uri_suffix_storage_modules )
                && get_system_variable( web_storage_var_name ).empty( ) )
               {
                  // FUTURE: This message should be handled as a server string message.
                  error = "No storage is currently attached to this session.";
               }
               else if( is_get_request
                && ( uri_suffix.find( c_cws_uri_suffix_storage_modules_prefix ) == 0 ) )
               {
                  string storage_name( get_system_variable( web_storage_var_name ) );

                  if( storage_name.empty( ) )
                     // FUTURE: This message should be handled as a server string message.
                     error = "No storage is currently attached to this session.";
                  else
                  {
                     string extra( uri_suffix.substr( CONST_LENGTH( c_cws_uri_suffix_storage_modules_prefix ) ) );

                     string::size_type pos = extra.find( '/' );

                     if( pos != string::npos )
                     {
                        string module_id( extra.substr( 0, pos ) );

                        extra.erase( 0, pos );

                        const sio_graph& graph( get_meta_data( storage_name ) );

                        const section_node& root_node( graph.get_root_node( ) );

                        string storage_module_id(
                         root_node.get_attribute_value( c_storage_attribute_id ) );

                        if( storage_module_id != upper( module_id ) )
                           // FUTURE: This message should be handled as a server string message.
                           error = "Unknonwn module id '" + module_id + "' for storage '" + storage_name + "'.";
                        else
                        {
                           if( extra == c_cws_uri_suffix_enums_extra )
                              response = get_attributes_for_name_query(
                               root_node, c_storage_module_enums_available_query, is_json_output );
                           else if( extra.find( c_cws_uri_suffix_enums_extra_prefix ) == 0 )
                           {
                              extra = extra.substr( CONST_LENGTH( c_cws_uri_suffix_enums_extra_prefix ) );

                              extra = c_storage_module_enums_id_prefix + upper( extra );

                              ostringstream osstr;

                              if( !is_json_output )
                                 write_graph( graph, &osstr, &extra );
                              else
                              {
                                 const section_node* p_section_node
                                  = get_section_node_from_path( root_node, extra );

                                 convert_sio_to_json( *p_section_node, osstr );
                              }

                              response = osstr.str( );
                           }
                           else if( extra == c_cws_uri_suffix_lists_extra )
                           {
                              string query;

                              if( is_admin )
                                 query = c_storage_module_lists_available_admin_query;
                              else
                                 query = c_storage_module_lists_available_non_admin_query;

                              response = get_attributes_for_name_query( root_node, query, is_json_output );
                           }
                           else if( extra.find( c_cws_uri_suffix_lists_extra_prefix ) == 0 )
                           {
                              extra = extra.substr( CONST_LENGTH( c_cws_uri_suffix_lists_extra_prefix ) );

                              extra = c_storage_module_lists_id_prefix + upper( extra );

                              ostringstream osstr;

                              if( !is_json_output )
                                 write_graph( graph, &osstr, &extra );
                              else
                              {
                                 const section_node* p_section_node
                                  = get_section_node_from_path( root_node, extra );

                                 convert_sio_to_json( *p_section_node, osstr );
                              }

                              response = osstr.str( );
                           }
                           else if( extra == c_cws_uri_suffix_views_extra )
                           {
                              string query;

                              if( is_admin )
                                 query = c_storage_module_views_available_admin_query;
                              else
                                 query = c_storage_module_views_available_non_admin_query;

                              response = get_attributes_for_name_query( root_node, query, is_json_output );
                           }
                           else if( extra.find( c_cws_uri_suffix_views_extra_prefix ) == 0 )
                           {
                              extra = extra.substr( CONST_LENGTH( c_cws_uri_suffix_views_extra_prefix ) );

                              extra = c_storage_module_views_id_prefix + upper( extra );

                              ostringstream osstr;

                              if( !is_json_output )
                                 write_graph( graph, &osstr, &extra );
                              else
                              {
                                 const section_node* p_section_node
                                  = get_section_node_from_path( root_node, extra );

                                 convert_sio_to_json( *p_section_node, osstr );
                              }

                              response = osstr.str( );
                           }

                           if( !response.empty( ) )
                              found = true;
                        }
                     }
                  }
               }
               else if( is_get_request
                && get_system_variable( web_storage_var_name ).empty( )
                && ( uri_suffix.find( c_cws_uri_suffix_storage_instances_prefix ) == 0 ) )
               {
                  // FUTURE: This message should be handled as a server string message.
                  error = "No storage is currently attached to this session.";
               }
               else if( is_get_request
                && ( instance_module_id.empty( ) || instance_mclass_id.empty( ) )
                && ( uri_suffix.find( c_cws_uri_suffix_storage_instances_prefix ) == 0 ) )
               {
                  // FUTURE: This message should be handled as a server string message.
                  error = "Instance request requires an endpoint with module and class ids.";
               }
               else if( is_post_request
                && ( uri_suffix.find( c_cws_uri_suffix_unlock_keys_prefix ) == 0 ) )
               {
                  string key( uri_suffix.substr( CONST_LENGTH( c_cws_uri_suffix_unlock_keys_prefix ) ) );

                  if( !has_system_variable( e_special_var_sid_locked ) )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Unlock key is not usuable when the system is not currently locked.";
                  else
                  {
                     try
                     {
                        set_identity( replaced( key, "-", " " ) );

                        found = true;
                     }
                     catch( exception& x )
                     {
                        error = x.what( );
                     }
                  }
               }
               else if( is_get_request && ( uri_suffix == c_cws_uri_suffix_stylesheets ) )
               {
                  found = true;

                  file_filter ff;

                  fs_iterator fs( get_web_root( ), &ff );

                  string all_stylesheets( g_none_var );

                  bool has_cached_stylesheets = has_system_variable( e_special_var_cws_styles );

                  if( !has_cached_stylesheets )
                     all_stylesheets = g_none_var;
                  else
                     all_stylesheets = get_system_variable( e_special_var_cws_styles );

                  if( !has_cached_stylesheets )
                  {
                     set< string > css_files;

                     while( fs.has_next( ) )
                     {
                        string next( fs.get_name( ) );

                        if( next.size( ) > CONST_LENGTH( c_css_suffix ) )
                        {
                           string::size_type pos = next.rfind( c_css_suffix );

                           if( pos == ( next.length( ) - CONST_LENGTH( c_css_suffix ) ) )
                              css_files.insert( next.substr( 0, pos ) );
                        }
                     }

                     for( set< string >::iterator i = css_files.begin( ); i!= css_files.end( ); ++i )
                        all_stylesheets += ' ' + *i;

                     set_system_variable( e_special_var_cws_styles, all_stylesheets );
                  }

                  string::size_type pos = all_stylesheets.find( ' ' );

                  if( pos != string::npos )
                     all_stylesheets.erase( 0, pos + 1 );

                  if( !all_stylesheets.empty( ) )
                  {
                     vector< string > styles;

                     split( all_stylesheets, styles, ' ' );

                     string allowed_stylesheets;

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

                        if( !allowed_stylesheets.empty( ) )
                           allowed_stylesheets += ' ';

                        allowed_stylesheets += next;
                     }

                     all_stylesheets = allowed_stylesheets;
                  }

                  if( all_stylesheets.empty( ) )
                     use_none_response = true;
                  else
                  {
                     if( !is_json_output )
                        response = all_stylesheets;
                     else
                     {
                        vector< string > stylesheets;

                        split( all_stylesheets, stylesheets, ' ' );

                        response = as_json_array( ( !is_admin ? "stylesheets" : "all_stylesheets" ), stylesheets );
                     }
                  }
               }
               else if( is_put_request && ( uri_suffix == c_cws_uri_suffix_stylesheets ) )
               {
                  string file_name( get_web_root( ) + '/' + access + c_css_suffix );

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
               else if( is_get_request && ( uri_suffix.find( c_cws_uri_suffix_stylesheets_prefix ) == 0 ) )
               {
                  string name( uri_suffix.substr( CONST_LENGTH( c_cws_uri_suffix_stylesheets_prefix ) ) );

                  string file_name( get_web_root( ) + '/' + name + c_css_suffix );

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
                        response = "{\"stylesheet\":\"" + escaped_json( style_data ) + "\"}\n";
                  }
               }
               else if( is_delete_request && ( uri_suffix == c_cws_uri_suffix_stylesheets ) )
               {
                  if( is_locked )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Stylesheet data cannot be erased whilst the system is locked.";
                  else
                  {
                     string file_name( get_web_root( ) + '/' + access + c_css_suffix );

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
               else if( is_post_request && ( uri_suffix == c_cws_uri_suffix_users ) )
               {
                  if( access != g_cws_admin_token )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Users can only be maintained by the administrator.";
                  else
                  {
                     bool is_secret = false;

                     string pin, suggested_username;

                     size_t valid_options = 0;

                     if( option_parameters.count( c_cws_request_users_create_options_secret ) )
                     {
                        ++valid_options;

                        is_secret = true;
                     }

                     if( option_parameters.count( c_cws_request_users_create_options_suggested ) )
                     {
                        ++valid_options;

                        string value( option_parameters[ c_cws_request_users_create_options_suggested ] );

                        string::size_type pos = value.find( ':' );

                        if( pos != string::npos )
                        {
                           pin = value.substr( 0, pos );

                           value.erase( 0, pos + 1 );
                        }

                        if( value == c_admin )
                           // FUTURE: This message should be handled as a server string message.
                           error = "Invalid attempt to create an 'admin' user.";
                        else
                           suggested_username = value;
                     }

                     if( option_parameters.size( ) > valid_options )
                        // FUTURE: This message should be handled as a server string message.
                        error = "Invalid options '" + options + "' for user creation.";
                     else if( error.empty( ) )
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
               else if( is_delete_request && ( uri_suffix.find( c_cws_uri_suffix_users_prefix ) == 0 ) )
               {
                  if( access != g_cws_admin_token )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Users can only be maintained by the administrator.";
                  else
                  {
                     string pin( uri_suffix.substr( CONST_LENGTH( c_cws_uri_suffix_users_prefix ) ) );

                     if( !has_user_info( pin ) )
                        // FUTURE: This message should be handled as a server string message.
                        error = "Unkknown user '" + pin + "' for removal.";
                     else
                     {
                        found = true;

                        remove_user_info_from_storage( pin );
                     }
                  }
               }
               else if( is_get_request && ( uri_suffix == c_cws_uri_suffix_users ) )
               {
                  if( access != g_cws_admin_token )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Users can only be maintained by the administrator.";
                  else
                  {
                     found = true;

                     string all_user_pins( get_all_user_pins( ) );

                     if( all_user_pins.empty( ) )
                        use_none_response = true;
                     else
                     {
                        vector< string > all_pins;

                        split( all_user_pins, all_pins, '\n' );

                        string all_user_info;

                        for( size_t i = 0; i < all_pins.size( ); i++ )
                        {
                           string next_pin( all_pins[ i ] );

                           string next_name( get_user_name( next_pin ) );

                           if( !is_json_output )
                           {
                              if( !all_user_info.empty( ) )
                                 all_user_info += '\n';

                              all_user_info += next_pin + ' ' + next_name;
                           }
                           else
                           {
                              if( !all_user_info.empty( ) )
                                 all_user_info += ",";

                              all_user_info += "{\"user\":[{\"pin\":\"" + next_pin + "\"},{\"name\":\"" + next_name + "\"}]}";
                           }
                        }

                        if( !is_json_output )
                           response = all_user_info;
                        else
                           response = "{\"all_users\":[" + all_user_info + "]}\n";
                     }
                  }
               }
               else if( uri_suffix == c_cws_uri_suffix_storages )
               {
                  found = true;

                  // NOTE: Currently will only return the default system storage.
                  string default_storage( lower( get_system_variable( e_special_var_storage ) ) );

                  if( !is_json_output )
                     response = default_storage;
                  else
                     response = "{\"storages\":[" + default_storage + "]}\n";
               }
               else
               {
                  string script_name( c_web_session_script );

                  bool running = has_system_variable( web_message_var_name );

                  if( !running )
                  {
                     TRACE_LOG( TRACE_VERBOSE | TRACE_SESSION, "(web_session) starting "
                      "session " + session + " with device " + device + " for access " + access );

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
                     running = has_system_variable( web_message_var_name );

                     if( running )
                        break;

                     msleep( 200 );
                  }

                  if( !running )
                  {
                     set_system_variable( web_session_var_name, "" );
                     set_system_variable( web_started_var_name, "" );

                     // FUTURE: This message should be handled as a server string message.
                     error = "Was unable to start a web session with access token '" + access + "'.";
                  }
                  else
                  {
                     found = true;

                     bool allowed_command = true;

                     bool is_user_info_request = false;
                     bool is_module_info_request = false;
                     bool is_instance_fetch_request = false;

                     string storage_name;

                     if( uri_suffix.find( c_cws_uri_suffix_storages_prefix ) == 0 )
                     {
                        is_user_info_request = true;

                        storage_name = uri_suffix.substr( CONST_LENGTH( c_cws_uri_suffix_storages_prefix ) );

                        // NOTE: Storage names begin with an upper case letter
                        // (but by convention the URL will be all lower case).
                        if( !storage_name.empty( ) )
                           storage_name[ 0 ] = toupper( storage_name[ 0 ] );
                     }
                     else if( uri_suffix == c_cws_uri_suffix_storage_modules )
                        is_module_info_request = true;
                     else if( uri_suffix.find( c_cws_uri_suffix_storage_instances_prefix ) == 0 )
                        is_instance_fetch_request = true;

                     if( !request.empty( ) && !is_user_info_request )
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
                        if( !request.empty( ) || is_user_info_request
                         || is_module_info_request || is_instance_fetch_request )
                        {
                           if( is_user_info_request )
                           {
                              string username;

                              if( access == g_cws_admin_token )
                                 username = c_admin;
                              else
                                 username = get_user_name( access );

                              if( storage_name.empty( ) )
                                 storage_name = get_system_variable( e_special_var_storage );

                              const section_node& root_node( get_meta_data( storage_name ).get_root_node( ) );

                              string storage_user_info(
                               root_node.get_attribute_value( c_storage_attribute_user_info ) );

                              if( storage_user_info.empty( ) )
                                 throw runtime_error( "unexpected missing storage_user_info" );

                              vector< string > all_storage_user_info;

                              split( storage_user_info, all_storage_user_info );

                              if( all_storage_user_info.size( ) < 3 )
                                 throw runtime_error( "unexpected missing needed storage_user_info parts" );

                              string user_class_id( all_storage_user_info[ 0 ] );

                              string user_field_id_username( all_storage_user_info[ 2 ] );

                              string::size_type pos = user_field_id_username.find( '+' );

                              if( pos != string::npos )
                                 user_field_id_username.erase( pos );

                              string user_field_id_description( user_field_id_username );

                              if( all_storage_user_info.size( ) > 5 )
                              {
                                 user_field_id_description = all_storage_user_info[ 5 ];

                                 string::size_type pos = user_field_id_description.find( ';' );

                                 if( pos != string::npos )
                                    user_field_id_description.erase( pos );
                              }

                              // NOTE: Currently assumes that the
                              // module name and storage name are
                              // the same.
                              string module( storage_name );

                              string field_list( user_field_id_description );

                              set_system_variable( web_storage_var_name, storage_name );

                              request_and_args = "<web_session_instance_fetch.cin "
                               + module + ' ' + user_class_id + " _none 1 "
                               + field_list + ' ' + user_field_id_username + '=' + username;
                           }
                           else if( is_module_info_request )
                              request_and_args = "<web_session_modules_fetch.cin";
                           else if( is_instance_fetch_request )
                           {
                              string user_key = get_session_info_user_key( session );

                              request_and_args = "<web_session_instance_fetch.cin "
                               + instance_module_id + ' ' + instance_mclass_id + ' ' + user_key;

                              if( instance_record_key.empty( ) )
                              {
                                 string num( "0" );
                                 string path;
                                 string fields( "_none" );

                                 if( option_parameters.count( c_storage_module_instance_options_num ) )
                                    num = option_parameters[ c_storage_module_instance_options_num ];

                                 if( option_parameters.count( c_storage_module_instance_options_path ) )
                                    path = option_parameters[ c_storage_module_instance_options_path ];

                                 if( option_parameters.count( c_storage_module_instance_options_fields ) )
                                    fields = option_parameters[ c_storage_module_instance_options_fields ];

                                 use_none_response = true;

                                 request_and_args += ' ' + num + ' ' + fields;

                                 if( !path.empty( ) )
                                    request_and_args += ' ' + path;
                              }
                              else
                              {
                                 string fields( "_none" );

                                 if( option_parameters.count( c_storage_module_instance_options_fields ) )
                                    fields = option_parameters[ c_storage_module_instance_options_fields ];

                                 use_unknown_response = true;

                                 request_and_args += " 1 " + fields + " _key=" + instance_record_key;
                              }
                           }

                           update_session_info( session, now );

                           set_system_variable( web_command_var_name, request_and_args );
                        }
                        else
                        {
                           update_session_info( session, now );

                           set_system_variable( web_command_var_name, "variable " + web_message_var_name );
                        }

                        for( size_t i = 0; i < 10; i++ )
                        {
                           if( file_exists( output_file_name ) )
                           {
                              response = buffer_file( output_file_name );

                              response = trim( response, false, false, "\n" );

                              file_remove( output_file_name );

                              if( is_user_info_request && !response.empty( ) )
                                 process_user_info_response( session, response );

                              break;
                           }

                           msleep( 100 );
                        }

                        if( !response.empty( )
                         && !is_json_output && is_instance_fetch_request )
                        {
                           vector< string > all_lines;

                           split( response, all_lines, '\n' );

                           response.erase( );

                           for( size_t i = 0; i < all_lines.size( ); i++ )
                           {
                              string next_line( all_lines[ i ] );

                              if( !response.empty( ) )
                                 response += '\n';

                              if( next_line[ 0 ] == '[' )
                              {
                                 string::size_type pos = next_line.find( ']' );

                                 string remainder;

                                 if( pos != string::npos )
                                 {
                                    remainder = next_line.substr( pos + 1 );

                                    string key_ver_and_state( next_line.substr( 1, pos - 1 ) );

                                    key_ver_and_state = opt_prefix_uri( key_ver_and_state );

                                    next_line = '[' + key_ver_and_state + ']' + remainder;
                                 }

                                 response += next_line;
                              }
                              else
                                 response += next_line;
                           }
                        }

                        if( !response.empty( ) && is_json_output )
                        {
                           if( is_module_info_request )
                           {
                              vector< string > modules;

                              split( response, modules, '\n' );

                              response = as_json_array( "all_modules", modules );
                           }
                           else if( is_instance_fetch_request )
                           {
                              if( response[ 0 ] == '[' )
                              {
                                 if( !instance_record_key.empty( ) )
                                 {
                                    vector< string > values;

                                    get_field_values( response, values );

                                    response = as_json_array( "values", values );
                                 }
                                 else
                                 {
                                    vector< string > records;

                                    split( response, records, '\n' );

                                    response = "{\n \"records\":\n  [\n";

                                    for( size_t i = 0; i < records.size( ); i++ )
                                    {
                                       string next_record( records[ i ] );

                                       if( !next_record.empty( ) && next_record[ 0 ] == '[' )
                                       {
                                          if( i > 0 )
                                             response += ",\n";

                                          vector< string > values;

                                          get_field_values( next_record, values );

                                          response += as_json_array( "", values );
                                       }
                                    }

                                    response += "\n ]\n}";
                                 }
                              }
                           }
                        }
                     }
                  }
               }

               if( found && response.empty( ) )
               {
                  string use_response( c_web_session_okay_response );

                  if( use_none_response )
                     use_response = c_web_session_none_response;
                  else if( use_unknown_response )
                     use_response = c_web_session_unknown_response;

                  if( !is_json_output )
                     response = use_response;
                  else
                     response = "{\"response\":\"" + use_response + "\"}\n";
               }
            }
         }
      }
   }

   return found;
}

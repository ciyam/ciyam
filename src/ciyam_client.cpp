// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <cstdlib>
#  include <map>
#  include <memory>
#  include <string>
#  include <iomanip>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#include "regex.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "macros.h"
#include "sha256.h"
#include "sockets.h"
#include "console.h"
#include "date_time.h"
#include "utilities.h"
#include "file_utils.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#  include "crypto_keys.h"
#endif
#include "command_parser.h"
#include "console_commands.h"

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

//#define DEBUG

#define USE_NO_DELAY

using namespace std;

namespace
{

#include "ciyam_constants.h"

const char* const c_app_title = "ciyam_client";
const char* const c_app_version = "0.1";

const char* const c_cmd_tls = "tls";

const char* const c_cmd_exec = "exec";
const char* const c_cmd_exec_command = "command";
const char* const c_cmd_exec_arguments = "arguments";

const char* const c_cmd_vars = "vars";
const char* const c_cmd_vars_name_vals = "name_vals";

const char* const c_cmd_args_file = "args_file";
const char* const c_cmd_args_file_name = "name";

const char* const c_cmd_rpc_unlock = "rpc_unlock";
const char* const c_cmd_rpc_unlock_password = "password";

const char* const c_cmd_connect_retries = "connect_retries";
const char* const c_cmd_connect_retries_max_attempts = "max_attempts";

const char* const c_session_cmd_bye = "bye";
const char* const c_session_cmd_chk = "chk";
const char* const c_session_cmd_get = "get";
const char* const c_session_cmd_put = "put";
const char* const c_session_cmd_tls = "tls";
const char* const c_session_cmd_quit = "quit";
const char* const c_session_cmd_file_get = "file_get";
const char* const c_session_cmd_file_put = "file_put";
const char* const c_session_cmd_starttls = "starttls";
const char* const c_session_cmd_system_variable = "system_variable";
const char* const c_session_cmd_session_variable = "session_variable";
const char* const c_session_cmd_session_terminate = "session_terminate";
const char* const c_session_cmd_session_rpc_unlock = "session_rpc_unlock";

const char* const c_env_var_pid = "PID";
const char* const c_env_var_port = "PORT";
const char* const c_env_var_slot = "SLOT";
const char* const c_env_var_error = "ERROR";
const char* const c_env_var_slotx = "SLOTX";
const char* const c_env_var_no_udp = "NO_UDP";
const char* const c_env_var_output = "OUTPUT";
const char* const c_env_var_webdir = "WEBDIR";
const char* const c_env_var_pub_key = "PUB_KEY";
const char* const c_env_var_pub_keyx = "PUB_KEYX";
const char* const c_env_var_file_name = "FILE_NAME";
const char* const c_env_var_local_udp = "LOCAL_UDP";
const char* const c_env_var_rpc_password = "RPC_PASSWORD";
const char* const c_env_var_ciyam_seconds = "CIYAM_SECONDS";
const char* const c_env_var_max_file_size = "MAX_FILE_SIZE";
const char* const c_env_var_progress_prefix = "PROGRESS_PREFIX";
const char* const c_env_var_ciyam_file_name_only = "CIYAM_FILE_NAME_ONLY";

const char* const c_udp_msg_cancel = "XXX";

const char* const c_file_test_cmd = "file_test";

const char* const c_not_found_output = "Not Found";
const char* const c_error_output_prefix = "Error: ";

const size_t c_pid_timeout = 5000; // i.e. 5 secs
const size_t c_tls_timeout = 5000; // i.e. 5 secs
const size_t c_pubkey_timeout = 5000; // i.e. 5 secs
const size_t c_command_timeout = 60000; // i.e. 60 secs
const size_t c_connect_timeout = 10000; // i.e. 10 secs
const size_t c_greeting_timeout = 10000; // i.e. 10 secs

const size_t c_recv_datagram_timeout = 10; // i.e. 1/100 sec
const size_t c_send_datagram_timeout = 50; // i.e. 1/20 sec

const size_t c_udp_packet_buffer_size = 1500;
const size_t c_udp_file_bytes_per_packet = 1000;

const size_t c_max_length_for_output_env_var = 100000;

string application_title( app_info_request request )
{
   if( request == e_app_info_request_title )
      return string( c_app_title );
   else if( request == e_app_info_request_version )
      return string( c_app_version );
   else if( request == e_app_info_request_title_and_version )
   {
      string title( c_app_title );
      title += " v";
      title += string( c_app_version );

      return title;
   }
   else
   {
      ostringstream osstr;
      osstr << "unknown app_info_request: " << request;
      throw runtime_error( osstr.str( ) );
   }
}

int g_pid = get_pid( );

bool g_use_tls = false;
bool g_had_quit = false;

size_t g_seconds = 1;

size_t g_max_file_size = c_files_area_item_max_size_default;

size_t g_connect_retries = 0;

string g_exec_cmd;
string g_args_file;
string g_rpc_password;

string g_quiet_cmd_prefix( " ." );

#ifdef SSL_SUPPORT
unique_ptr< private_key > gup_priv_key;
#endif

class ciyam_console_startup_functor : public command_functor
{
   public:
   ciyam_console_startup_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters )
   {
      if( command == c_cmd_tls )
         g_use_tls = true;
      else if( command == c_cmd_exec )
      {
         g_exec_cmd = get_parm_val( parameters, c_cmd_exec_command );

         string arguments( get_parm_val( parameters, c_cmd_exec_arguments ) );

         if( !arguments.empty( ) )
            g_exec_cmd += " " + arguments;
      }
      else if( command == c_cmd_vars )
      {
         string name_vals( get_parm_val( parameters, c_cmd_vars_name_vals ) );

         vector< string  > all_name_vals;
         split( name_vals, all_name_vals );

         for( size_t i = 0; i < all_name_vals.size( ); i++ )
         {
            string next_name_val( all_name_vals[ i ] );

            string::size_type pos = next_name_val.find( '=' );

            if( pos == string::npos )
               set_environment_variable( next_name_val, c_true );
            else
               set_environment_variable( next_name_val.substr( 0, pos ), next_name_val.substr( pos + 1 ) );
         }
      }
      else if( command == c_cmd_args_file )
         g_args_file = get_parm_val( parameters, c_cmd_args_file_name );
      else if( command == c_cmd_rpc_unlock )
         g_rpc_password = get_parm_val( parameters, c_cmd_rpc_unlock_password );
      else if( command == c_cmd_connect_retries )
         g_connect_retries = from_string< size_t >( get_parm_val( parameters, c_cmd_connect_retries_max_attempts ) );
   }
};

class ciyam_console_command_handler : public console_command_handler
{
   public:
#ifdef SSL_SUPPORT
   ciyam_console_command_handler( ssl_socket& socket, udp_socket& usocket )
#else
   ciyam_console_command_handler( tcp_socket& socket, udp_socket& usocket )
#endif
    :
    port( c_default_ciyam_port ),
    host( c_default_ciyam_host ),
    chunk( 0 ),
    next_part( 0 ),
    file_bytes( 0 ),
    file_parts( 0 ),
    file_start( 0 ),
    chunk_size( 0 ),
    total_chunks( 0 ),
    socket( socket ),
    usocket( usocket ),
    num_udp_skips( 0 ),
    had_message( false ),
    had_chk_command( false ),
    had_chunk_progress( false ),
    had_single_char_message( false )
   {
      set_custom_startup_options( 1, "[<port> or <host[(:|-)port]>]" );

      string env_var_port( get_environment_variable( c_env_var_port ) );

      if( !env_var_port.empty( ) )
         port = atoi( env_var_port.c_str( ) );
   }

   const char* get_host( ) const { return host.c_str( ); }

   int get_port( ) const { return port; }

   private:
   int port;
   string host;

   string chunk_data;

   size_t chunk;
   size_t next_part;

   date_time dtm;

   size_t file_bytes;
   size_t file_parts;
   size_t file_start;

   size_t chunk_size;
   size_t total_chunks;

   string file_name;
   string file_extra;

   string file_list_data;
   string file_strip_prefix;

   string append_prefix;
   string append_last_name;

   console_progress progress;

   deque< string > additional_commands;

#ifdef SSL_SUPPORT
   ssl_socket& socket;
#else
   tcp_socket& socket;
#endif

   udp_socket& usocket;

   size_t num_udp_skips;

   bool had_message;
   bool had_chk_command;
   bool had_chunk_progress;
   bool had_single_char_message;

   string get_additional_command( );

   void preprocess_command_and_args( string& str, const string& cmd_and_args, bool /*skip_command_usage*/ );

   void process_custom_startup_option( size_t num, const string& option );
};

string ciyam_console_command_handler::get_additional_command( )
{
   string cmd;

   if( !additional_commands.empty( ) )
   {
      cmd = additional_commands.front( );
      additional_commands.pop_front( );

      if( !cmd.empty( ) && cmd[ 0 ] >= '0' && cmd[ 0 ] <= '9' )
      {
         total_chunks = atoi( cmd.c_str( ) );
         cmd.erase( );

         if( !additional_commands.empty( ) )
         {
            cmd = additional_commands.front( );
            additional_commands.pop_front( );
         }
      }
   }
   else if( had_chunk_progress )
   {
      if( !is_stdout_console( ) )
         cout << endl;
      else
         progress.output_progress( "" );

      had_chunk_progress = false;
      had_single_char_message = false;
   }

   return cmd;
}

void ciyam_console_command_handler::preprocess_command_and_args(
 string& str, const string& cmd_and_args, bool /*skip_command_usage*/ )
{
   if( is_handling_startup_options( ) )
      return console_command_handler::preprocess_command_and_args( str, cmd_and_args, false );

   str = cmd_and_args;

   if( !str.empty( ) )
   {
      try
      {
         console_command_handler::preprocess_command_and_args( str, cmd_and_args, true );
      }
      catch( exception& x )
      {
         str.erase( );

         string error_prefix( c_error_output_prefix );

         handle_command_response( error_prefix + x.what( ), true );
      }

      if( !str.empty( ) )
      {
         bool was_chk = false;
         bool was_msg = false;
         bool was_chk_tag = false;
         bool was_chk_token = false;
         bool was_list_prefix = false;
         bool was_no_compress = false;
         bool delete_after_put = false;

         string get_dest_file, put_file_hash, put_source_file;

         string::size_type pos = str.find( ' ' );

         if( str.substr( 0, pos ) == "msg" )
            was_msg = true;

         if( ( str.substr( 0, pos ) == c_session_cmd_chk )
          || ( str.substr( 0, pos ) == c_session_cmd_get )
          || ( str.substr( 0, pos ) == c_session_cmd_put )
          || ( str.substr( 0, pos ) == c_session_cmd_file_get )
          || ( str.substr( 0, pos ) == c_session_cmd_file_put ) )
         {
            string data( str.substr( pos + 1 ) );

            string extra;

            if( str.substr( 0, pos ) == c_session_cmd_chk )
            {
               was_chk = true;

               if( data.find( ' ' ) != string::npos )
                  was_chk_token = true;
            }

            if( ( str.substr( 0, pos ) == c_session_cmd_get )
             || ( str.substr( 0, pos ) == c_session_cmd_file_get )
             || ( str.substr( 0, pos ) == c_session_cmd_file_put ) )
            {
               string::size_type spos = data.find( ' ' );

               if( spos != string::npos )
               {
                  if( str.substr( 0, pos ) == c_session_cmd_file_put )
                     extra = data.substr( spos );
                  else
                     get_dest_file = data.substr( spos + 1 );

                  data.erase( spos );
               }
            }

            string prefix( c_file_type_str_blob );

            regex expr( c_regex_hash_256, true, true );

            if( was_chk && !was_chk_token && expr.search( data ) == string::npos )
               was_chk_tag = true;

            // NOTE: For testing purposes if a local file exists with the name given then
            // assumes what is wanted is a blob with this file content rather than a tag.
            if( was_chk_tag && file_exists( data ) )
            {
               str.erase( pos + 1 );
               data = buffer_file( data );

               str += sha256( prefix + data ).get_digest_as_string( ) + extra;

#ifdef DEBUG
               handle_command_response( str );
#endif
               was_chk_tag = false;
            }

            if( ( str.substr( 0, pos ) == c_session_cmd_put )
             || ( str.substr( 0, pos ) == c_session_cmd_file_put ) )
            {
               str.erase( pos + 1 );

               put_source_file = data;

               // NOTE: Special case to get rid of "./".
               replace( put_source_file, "*.//./", "*" );

               // NOTE: If file is in the form "dir/./name" then get rid of the "./".
               replace( put_source_file, "/./", "/" );

               size_t xpos = put_source_file.find( "//" );

               if( xpos != string::npos )
                  replace( put_source_file, "//", "/" );

               if( !put_source_file.empty( ) )
               {
                  // NOTE: If the file name starts with a ? then it will be handled
                  // as a "list" rather than a "blob".
                  if( put_source_file[ 0 ] == '?' )
                  {
                     was_list_prefix = true;
                     prefix = string( c_file_type_str_list );

                     put_source_file.erase( 0, 1 );

                     string buffer( buffer_file( put_source_file ) );

                     // NOTE: If the list had been constructed via a script it might have
                     // ended up with an invalid final trailing line feed (so remove it).
                     if( !buffer.empty( ) && buffer[ buffer.length( ) - 1 ] == '\n' )
                     {
                        buffer.erase( buffer.length( ) - 1 );
                        write_file( put_source_file, buffer );
                     }
                  }
                  else if( put_source_file[ 0 ] >= '1' && put_source_file[ 0 ] <= '9' )
                  {
                     pos = put_source_file.find( '*' );

                     if( pos != string::npos )
                     {
                        chunk = 0;

                        file_bytes = 0;
                        file_start = 0;

                        string::size_type spos = put_source_file.find( ':' );

                        if( spos != string::npos )
                        {
                           string::size_type lpos = put_source_file.find( '+', spos );

                           if( lpos != string::npos )
                           {
                              file_bytes = unformat_bytes( put_source_file.substr( lpos + 1 ) );
                              put_source_file.erase( lpos );
                           }

                           file_start = unformat_bytes( put_source_file.substr( spos + 1 ) );
                           put_source_file.erase( spos );
                        }

                        if( extra.size( ) > 0 )
                           file_extra = extra.substr( 1 );

                        if( xpos != string::npos )
                           xpos -= pos;

                        extra.erase( );
                        file_list_data.erase( );

                        chunk_size = unformat_bytes( put_source_file.substr( 0, pos ) );
                        put_source_file.erase( 0, pos + 1 );

                        if( chunk_size <= 1 )
                           throw runtime_error( "chunk size too small" );

                        // NOTE: Reduce the chunk size by one due to the one byte "type" prefix.
                        --chunk_size;

                        file_name = put_source_file;

                        pos = file_extra.find( '?' );

                        // NOTE: If no explicit file strip prefix has been provided but "//" was
                        // in the name provded then use the path before that as the strip prefix.
                        if( pos == string::npos )
                        {
                           file_strip_prefix.erase( );

                           if( xpos != string::npos )
                              file_strip_prefix = file_name.substr( 0, xpos );
                        }
                        else
                        {
                           file_strip_prefix = file_extra.substr( 0, pos );
                           file_extra.erase( 0, pos + 1 );
                        }
                     }
                  }
               }

               string put_file_error;

               if( !file_exists( put_source_file ) )
                  put_file_error = "File '" + put_source_file + "' not found.";
               else if( chunk_size && ( chunk == 0 ) )
               {
                  size_t name_length_for_calc = file_name.length( ) - file_strip_prefix.length( );

                  // NOTE: Allows for the hash, a space, the name plus chunk extension and a LF.
                  int list_items_per_chunk = g_max_file_size / ( 65 + name_length_for_calc + 8 );
                  int64_t max_size_using_chunks = ( list_items_per_chunk * g_max_file_size );

                  if( file_bytes )
                  {
                     if( file_bytes > max_size_using_chunks )
                        put_file_error = "File '" + put_source_file
                         + "' exceeds maximum file size (" + format_bytes( max_size_using_chunks ) + ").";
                  }
                  else
                  {
                     if( file_size( put_source_file ) > max_size_using_chunks )
                        put_file_error = "File '" + put_source_file
                         + "' exceeds maximum file size (" + format_bytes( max_size_using_chunks ) + ").";
                  }

                  dtm = date_time::local( );
                  had_chunk_progress = false;

                  if( put_file_error.empty( ) && !has_option_no_progress( ) )
                  {
                     string file_name( get_environment_variable( c_env_var_file_name ) );

                     if( !file_name.empty( ) )
                     {
                        cout << file_name;
                        cout.flush( );

                        had_chunk_progress = true;
                        progress.output_prefix = file_name;
                     }
                  }
               }

               if( !put_file_error.empty( ) )
               {
                  cerr << c_error_output_prefix << put_file_error << endl;

                  // NOTE: Will only set the error environment variable if hasn't already been set.
                  if( get_environment_variable( c_env_var_error ).empty( ) )
                     set_environment_variable( c_env_var_error, put_file_error.c_str( ) );

                  str.erase( );
                  chunk_size = 0;

                  return;
               }

               string chunk_name;

               if( chunk_size )
               {
                  // NOTE: In order to maximumise the file transfer throughput
                  // assume that the large files have already been compressed.
                  prefix = string( c_file_type_str_blob_no_compress );

                  was_no_compress = true;

                  chunk_name = file_name;
                  chunk_name += '.' + to_comparable_string( chunk, false, c_chunk_digits );

                  if( chunk == 0 )
                  {
                     size_t size_remaining = 0;

                     buffer_file( chunk_data, file_name, chunk_size, &size_remaining, file_start );

                     if( file_start )
                        size_remaining -= file_start;

                     if( !file_bytes )
                        file_bytes = size_remaining;
                     else
                        file_bytes = min( file_bytes, size_remaining );

                     total_chunks = ( file_bytes / chunk_size );
                  }
                  else
                     buffer_file( chunk_data, file_name,
                      min( file_bytes, chunk_size ), 0, file_start + ( chunk * chunk_size ) );

                  if( !has_option_no_progress( ) )
                  {
                     date_time now( date_time::local( ) );

                     uint64_t elapsed = seconds_between( dtm, now );

                     if( elapsed >= g_seconds )
                     {
                        if( is_stdout_console( ) )
                           progress.output_progress( " ", chunk, total_chunks );
                        else
                        {
                           cout << '.';
                           cout.flush( );
                        }

                        dtm = now;
                        had_chunk_progress = true;
                     }
                  }

                  if( file_bytes <= chunk_size )
                  {
                     file_bytes = 0;
                     chunk_size = 0;
                     total_chunks = 0;
                  }
                  else
                  {
                     ++chunk;
                     file_bytes -= chunk_size;

                     // NOTE: Ensure that the additional command will neither be
                     // included in console history nor sent to standard output.
                     additional_commands.push_back( g_quiet_cmd_prefix
                      + string( c_session_cmd_file_put ) + " " + file_name );
                  }
               }
               else if( !file_name.empty( ) )
               {
                  file_name.erase( );

                  was_list_prefix = true;
                  prefix = string( c_file_type_str_list );
               }

               sha256 tmp_hash( prefix );

               if( chunk_data.empty( ) && !file_list_data.empty( ) )
               {
                  chunk_data = file_list_data;
                  file_list_data.erase( );
               }

               if( chunk_data.length( ) )
                  tmp_hash.update( chunk_data );
               else
                  tmp_hash.update( put_source_file, true );

               put_file_hash = tmp_hash.get_digest_as_string( );

               if( !chunk_name.empty( ) )
               {
                  if( !file_list_data.empty( ) )
                     file_list_data += '\n';

                  string item_name( chunk_name );

                  if( !file_strip_prefix.empty( ) && item_name.find( file_strip_prefix ) == 0 )
                     item_name.erase( 0, file_strip_prefix.length( ) );

                  file_list_data += put_file_hash + ' ' + item_name;

                  if( file_bytes == 0 )
                  {
                     if( !file_extra.empty( ) )
                        file_extra = " " + file_extra;

                     // NOTE: Ensure that the additional command will neither be
                     // included in console history nor sent to standard output.
                     additional_commands.push_back( g_quiet_cmd_prefix
                      + string( c_session_cmd_file_put ) + " " + file_name + file_extra );
                  }
               }

               str += put_file_hash + extra;
#ifdef DEBUG
               handle_command_response( str );
#endif
            }
         }

         size_t num_datagrams = 0;
         bool has_sent_datagrams = false;

         if( ( str.find( c_file_test_cmd ) == 0 ) 
          && ( str.length( ) > strlen( c_file_test_cmd ) + 1 ) )
         {
            string::size_type pos = str.rfind( ' ' );

            if( usocket && ( pos != string::npos ) )
            {
               num_datagrams = from_string< size_t >( str.substr( pos + 1 ) );

               if( num_datagrams > 0 && num_datagrams <= 1000 )
               {
                  has_sent_datagrams = true;

                  send_test_datagrams( num_datagrams, get_host( ), get_port( ), c_send_datagram_timeout, &usocket );

                  // NOTE: Allow application server a little extra time to receive datagrams.
                  if( string( get_host( ) ) == c_local_host )
                     msleep( c_send_datagram_timeout * 2 );
                  else
                     msleep( c_send_datagram_timeout / 2 );
               }
            }
         }

         if( !g_had_quit && ( str == c_session_cmd_quit ) )
         {
            g_had_quit = true;

            if( !g_args_file.empty( ) )
            {
               string error( get_environment_variable( c_env_var_error ) );

               if( !error.empty( ) )
               {
                  str = string( c_session_cmd_system_variable )
                   + " " + g_args_file + " \"" + error + "\"";

                  // NOTE: As "str" has been replaced will include an additional ".quit".
                  additional_commands.push_back( g_quiet_cmd_prefix + c_session_cmd_quit );
               }
            }
         }

#ifdef DEBUG
         cerr << "sending command: " << str << endl;
#endif
         socket.write_line( str );

         if( has_sent_datagrams )
         {
            if( string( get_host( ) ) == c_local_host )
               msleep( c_send_datagram_timeout * 2 );
            else
               msleep( c_send_datagram_timeout / 2 );

            // NOTE: Now send all the datagrams again (this time in reverse order).
            send_test_datagrams( num_datagrams, get_host( ), get_port( ), c_send_datagram_timeout, &usocket, true );
         }

#ifdef SSL_SUPPORT
         if( !socket.is_secure( )
          && ( ( str == c_session_cmd_tls ) || str == c_session_cmd_starttls ) )
            socket.ssl_connect( c_tls_timeout );
#endif
         if( ( str == c_session_cmd_bye )
          || ( str == c_session_cmd_quit )
          || ( str == c_session_cmd_session_terminate ) )
         {
            clear_key( str );

            str.erase( );
            set_finished( );
         }
         else
         {
            bool was_wait = false;
            bool was_get_or_put = false;
            bool ignore_exception = false;

            try
            {
               string::size_type pos = str.find( ' ' );

               if( str.substr( 0, pos ) == "wait" )
                  was_wait = true;
               else if( ( str.substr( 0, pos ) == c_session_cmd_get )
                || ( str.substr( 0, pos ) == c_session_cmd_put ) )
                  was_get_or_put = true;

               if( ( str.substr( 0, pos ) == c_session_cmd_get )
                || ( str.substr( 0, pos ) == c_session_cmd_file_get ) )
               {
                  string filename( !get_dest_file.empty( ) ? get_dest_file : str.substr( pos + 1 ) );

                  unsigned char prefix = '\0';

                  bool appending = false;
                  bool append_chunks = false;
                  bool appending_below = false;
                  bool delete_after_transfer = false;

                  string append_filename;

                  if( !filename.empty( ) )
                  {
                     if( filename[ 0 ] == '?' )
                        appending = true;
                     else if( filename[ 0 ] == '*' )
                     {
                        append_chunks = true;
                        append_filename = filename.substr( 1 );

                        if( !is_additional_command( ) )
                        {
                           append_prefix.erase( );
                           append_last_name.erase( );

                           dtm = date_time::local( );
                           had_chunk_progress = false;

                           if( !has_option_no_progress( ) )
                           {
                              string file_name( get_environment_variable( c_env_var_file_name ) );

                              if( !file_name.empty( ) )
                              {
                                 cout << file_name;
                                 cout.flush( );

                                 had_chunk_progress = true;
                                 progress.output_prefix = file_name;
                              }
                           }
                        }

                        if( !append_filename.empty( )
                         && append_filename[ append_filename.length( ) - 1 ] == '/' )
                        {
                           appending_below = true;
                           append_filename.erase( append_filename.length( ) - 1 );
                        }

                        if( !appending_below )
                        {
                           if( append_prefix.empty( )
                            && !is_additional_command( ) && file_exists( append_filename ) )
                              append_chunks = false;
                           else
                              filename = "~" + uuid( ).as_string( );
                        }
                        else if( !append_filename.empty( ) )
                        {
                           if( !file_exists( append_filename ) )
                              append_chunks = false;
                           else
                           {
                              string path( get_cwd( ) );

                              bool okay = true;
                              set_cwd( append_filename, &okay );

                              if( !okay )
                                 append_chunks = false;
                              else
                                 filename = "~" + uuid( ).as_string( );

                              set_cwd( path );
                           }
                        }
                     }
                  }

                  long start_pos = 0;

                  if( appending )
                  {
                     filename.erase( 0, 1 );

                     if( file_exists( filename ) )
                        start_pos = file_size( filename );
                  }

                  if( ( !appending && file_exists( filename ) )
                   || ( !append_chunks && !append_filename.empty( ) ) )
                  {
                     string error_prefix( c_error_output_prefix );

                     if( append_filename.empty( ) )
                        handle_command_response( error_prefix + "local file '" + filename + "' already exists", true );
                     else if( !appending_below )
                        handle_command_response( error_prefix + "local file '" + append_filename + "' already exists", true );
                     else
                        handle_command_response( error_prefix + "local path '" + append_filename + "' not found or not valid", true );

                     append_filename.erase( );

                     ignore_exception = true;
                     delete_after_transfer = true;

                     filename = "~" + uuid( ).as_string( );
                  }

#ifdef DEBUG
                  if( appending )
                     handle_command_response( str );
#endif

                  ft_extra_info ft_extra( c_file_transfer_initial_timeout,
                   c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );

                  file_transfer( filename, socket,
                   ( !appending ? e_ft_direction_recv : e_ft_direction_recv_app ),
                   g_max_file_size, c_response_okay_more, &ft_extra );

#ifdef ZLIB_SUPPORT
                  if( ( prefix & c_file_type_char_compressed ) && !( prefix & c_file_type_char_encrypted ) )
                  {
                     string data( buffer_file( filename ) );
                     string expanded_data( g_max_file_size, '\0' );

                     size_t usize = g_max_file_size;

                     if( uncompress( ( Bytef* )&expanded_data[ 0 ], &usize, ( Bytef* )&data[ 0 ], data.size( ) ) != Z_OK )
                        throw runtime_error( "bad compressed file or buffer not big enough" );

                     expanded_data.erase( usize );

                     write_file( filename, expanded_data, false, start_pos );
                  }
#endif

                  // NOTE: If the local filename was prefixed with a '*' and the file content is a list
                  // then will assume it is a list of "chunks" that must be appended one after another.
                  if( append_chunks && ( prefix & c_file_type_char_list ) )
                  {
                     delete_after_transfer = true;

                     if( append_filename.empty( ) )
                        append_prefix = ".";
                     else if( appending_below )
                        append_prefix = append_filename;

                     string all_chunks( buffer_file( filename ) );

                     chunk = total_chunks = 0;

                     if( !all_chunks.empty( ) )
                     {
                        vector< string > chunks;

                        split_list_items( all_chunks, chunks );

                        total_chunks = chunks.size( );

                        // NOTE: If is additional command then need to store the next
                        // "total_chunks" value as a command so the value is restored
                        // prior to processing each chunk.
                        if( is_additional_command( ) )
                           additional_commands.push_back( to_string( total_chunks ) );

                        for( size_t i = 0; i < chunks.size( ); i++ )
                        {
                           string next( chunks[ i ] );
                           string::size_type pos = next.find( ' ' );

                           if( next.find( ':' ) != string::npos )
                              ++file_parts;

                           // NOTE: Ensure that the additional command will neither be
                           // included in console history nor sent to standard output.
                           additional_commands.push_back( " .file_get " + next.substr( 0, pos )
                            + " *" + ( append_prefix.empty( ) ? append_filename : next.substr( pos + 1 ) ) );
                        }
                     }
                  }
                  // NOTE: If appending and the file content is a blob rather than a list
                  // then assume it is actually the next chunk that needs to be appended.
                  else if( append_chunks )
                  {
                     delete_after_transfer = true;

                     if( append_prefix.empty( ) )
                        file_append( filename, append_filename );
                     else
                     {
                        // NOTE: It is assumed that a file chunk number is a final file extension.
                        pos = append_filename.rfind( '.' );

                        string name_without_chunk_ext( append_filename.substr( 0, pos ) );
                        string prefixed_append_name( append_prefix + '/' + name_without_chunk_ext );

                        string prefix_append_name_for_display( prefixed_append_name );

                        if( has_environment_variable( c_env_var_ciyam_file_name_only ) )
                           prefix_append_name_for_display = name_without_chunk_ext;

                        if( append_last_name != name_without_chunk_ext )
                        {
                           chunk = 0;
                           next_part = 0;

                           file_remove( prefixed_append_name );

                           create_directories( prefixed_append_name );

                           if( file_parts )
                              prefix_append_name_for_display += ":0";

                           if( !has_option_no_progress( ) )
                           {
                              if( is_stdout_console( ) )
                              {
                                 // NOTE: Force output here so that every
                                 // separate file has its own output line.
                                 if( !progress.output_prefix.empty( ) )
                                    progress.output_progress( "" );

                                 progress.output_prefix = prefix_append_name_for_display;
                              }
                              else if( !append_last_name.empty( ) )
                                 cout << '\n';

                              cout << prefix_append_name_for_display;
                              cout.flush( );

                              had_chunk_progress = true;
                           }
                           else
                              handle_command_response( prefix_append_name_for_display );
                        }
                        else if( chunk == 0 && file_parts )
                        {
                           prefix_append_name_for_display += ':' + to_string( ++next_part );

                           if( !has_option_no_progress( ) )
                           {
                              if( is_stdout_console( ) )
                              {
                                 // NOTE: Force output here so that every
                                 // separate part has its own output line.
                                 progress.output_progress( "" );
                                 progress.output_prefix = prefix_append_name_for_display;
                              }
                              else
                                 cout << '\n';

                              cout << prefix_append_name_for_display;
                              cout.flush( );

                              had_chunk_progress = true;
                           }
                           else
                              handle_command_response( prefix_append_name_for_display );

                           --file_parts;
                        }

                        append_last_name = name_without_chunk_ext;

                        file_append( filename, prefixed_append_name );
                     }

                     ++chunk;

                     if( !has_option_no_progress( ) )
                     {
                        date_time now( date_time::local( ) );

                        uint64_t elapsed = seconds_between( dtm, now );

                        if( elapsed >= g_seconds )
                        {
                           if( is_stdout_console( ) )
                              progress.output_progress( " ", chunk, total_chunks );
                           else
                           {
                              cout << '.';
                              cout.flush( );
                           }

                           dtm = now;
                           had_chunk_progress = true;
                        }
                     }

                     if( chunk >= total_chunks )
                     {
                        chunk = 0;
                        progress.previous_num = 0;
                     }
                  }

                  if( delete_after_transfer )
                     file_remove( filename );
               }
               else if( ( str.substr( 0, pos ) == c_session_cmd_put )
                || ( str.substr( 0, pos ) == c_session_cmd_file_put ) )
               {
                  string filename( !put_source_file.empty( ) ? put_source_file : str.substr( pos + 1 ) );

                  unsigned char prefix( !put_source_file.empty( ) ? c_file_type_char_blob : '\0' );

                  // NOTE: For a file that has been split into chunks the final file transfer is a list
                  // and each of the chunks are flagged so as not to even be attempted to be compressed.
                  if( prefix && was_list_prefix )
                     prefix = c_file_type_char_list;
                  else if( prefix && was_no_compress )
                     prefix = c_file_type_char_blob_no_compress;

                  was_list_prefix = false;
                  was_no_compress = false;

                  unsigned char* p_chunk = 0;
                  unsigned int chunk_size = chunk_data.length( );

                  if( chunk_size )
                     p_chunk = ( unsigned char* )chunk_data.data( );

                  unique_ptr< udp_helper > up_udp_helper;

                  bool no_udp = has_environment_variable( c_env_var_no_udp );

                  // NOTE: If using UDP then try and send the file/chunk content via UDP first
                  // with the TCP file transfer still starting (but could finish straight away
                  // if all content had already been received via UDP).
                  if( !no_udp && usocket )
                  {
                     if( num_udp_skips )
                        --num_udp_skips;
                     else
                     {
                        up_udp_helper.reset( new udp_helper( put_file_hash ) );

                        string slotx( get_environment_variable( c_env_var_slotx ) );

                        ip_address address( get_host( ), get_port( ) );

                        string data;
                        size_t num_bytes = 0;

                        size_t min_size_for_udp = ( c_udp_file_bytes_per_packet * 10 );

                        if( p_chunk )
                        {
                           num_bytes = chunk_size;

                           if( num_bytes >= min_size_for_udp )
                              data = ( char )prefix + string( ( const char* )p_chunk, chunk_size );
                        }
                        else
                        {
                           num_bytes = file_size( filename );

                           if( num_bytes >= min_size_for_udp )
                              data = ( char )prefix + buffer_file( filename );
                        }

                        if( !data.empty( ) )
                        {
                           size_t num = 0;

                           string data_copy;

                           bool was_cancelled = false;

                           if( string( get_host( ) ) == c_local_host )
                              data_copy = data;

                           while( data.length( ) )
                           {
                              string::size_type pos = c_udp_file_bytes_per_packet;

                              if( data.length( ) < pos )
                                 pos = data.length( );

                              string next_packet( slotx + ':'
                               + to_comparable_string( num++, false, 3 ) + ':' + put_file_hash.substr( 0, 9 ) + ':' );

                              // nyi - should include hash info here (for now just pad with a marker)...
                              while( next_packet.size( ) < 64 )
                                 next_packet += '*';

                              next_packet += base64::encode( data.substr( 0, pos ) );

                              int n = usocket.send_to(
                               ( unsigned char* )next_packet.data( ), next_packet.length( ), address, c_send_datagram_timeout );

                              if( n <= 0 )
                                 break;

                              data.erase( 0, pos );

                              if( ( ( num < 100 ) && ( num % 25 == 0 ) )
                               || ( num <= 1000 ) && ( num % 100 == 0 ) )
                              {
                                 unsigned char buffer[ c_udp_packet_buffer_size ];

                                 if( string( get_host( ) ) == c_local_host )
                                    msleep( 5 );

                                 n = usocket.recv_from( buffer, sizeof( buffer ), c_recv_datagram_timeout );

                                 if( n > 0 )
                                 {
                                    string msg( ( const char* )buffer, n );

                                    string::size_type pos = msg.find( ':' );
                                    if( pos != string::npos )
                                    {
                                       string slot( msg.substr( 0, pos ) );

                                       if( slot == get_environment_variable( c_env_var_slot ) )
                                       {
                                          msg.erase( 0, pos + 1 );
                                          pos = msg.find( ':' );

                                          if( pos != string::npos )
                                          {
                                             string chunk = msg.substr( 0, pos );
                                             msg.erase( 0, pos + 1 );

                                             if( msg == put_file_hash )
                                             {
                                                // NOTE: If server already has file content then cease sending.
                                                if( chunk == c_udp_msg_cancel )
                                                {
                                                   cout << "(UDP cancel)\n";

                                                   data.erase( );
                                                   data_copy.erase( );

                                                   was_cancelled = true;
                                                }
                                             }
                                          }
                                       }
                                    }
                                 }
                              }

                              if( data.empty( ) )
                              {
                                 num = 0;
                                 data = data_copy;

                                 data_copy.erase( );
                              }
                           }

                           if( !was_cancelled )
                           {
                              if( string( get_host( ) ) != c_local_host )
                                 msleep( c_send_datagram_timeout );
                              else
                                 msleep( c_send_datagram_timeout * 2 );
                           }
                        }
                     }
                  }

                  ft_extra_info ft_extra(
                   c_file_transfer_initial_timeout, c_file_transfer_line_timeout,
                   c_file_transfer_max_line_size, &prefix, p_chunk, chunk_size, c_response_okay_skip );

                  file_transfer( filename, socket, e_ft_direction_send,
                   g_max_file_size, c_response_okay_more, &ft_extra, 0, up_udp_helper.get( ) );

                  if( up_udp_helper.get( ) )
                  {
                     // NOTE: If the server is not using UDP then do not try again.
                     if( !up_udp_helper->had_recv_help )
                        usocket.close( );
                     else
                     {
                        float percent = up_udp_helper->recv_percent;

                        cout << '(' << setfill( '0' ) << ffmt( 1, 2 ) << percent << "% UDP)\n";

                        if( percent < 50.0 )
                        {
                           // NOTE: Try again later depending upon the percentage received.
                           if( percent < 10.0 )
                              num_udp_skips = 100;
                           else if( percent < 50.0 )
                              num_udp_skips = 10;
                        }
                     }
                  }

                  if( !file_bytes && !chunk_data.empty( ) )
                     chunk_data.resize( 0 );

                  if( delete_after_put )
                  {
                     delete_after_put = false;
                     file_remove( put_source_file );
                  }
               }
            }
            catch( exception& x )
            {
               clear_key( str );
               str.erase( );

               if( !ignore_exception )
               {
                  string s( x.what( ) );

                  size_t err_prefix_length( strlen( c_response_error_prefix ) );

                  if( s.length( ) > err_prefix_length
                   && s.substr( 0, err_prefix_length ) == string( c_response_error_prefix ) )
                  {
                     s = string( c_error_output_prefix ) + s.substr( err_prefix_length );
                  }

                  handle_command_response( s, true );
               }

               return;
            }

            string response;

            size_t num_singles = 0;

            bool is_first = true;
            bool had_not_found = false;
            bool is_in_progress = false;

            while( response.empty( ) || response[ 0 ] != '(' )
            {
               response.erase( );

               if( socket.read_line( response, c_command_timeout ) <= 0 )
               {
                  if( socket.had_timeout( ) )
                  {
                     // NOTE: Ignore timeouts if last received a progress message.
                     if( is_in_progress )
                        continue;
                     else if( had_not_found )
                        break;
                     else
                        throw runtime_error( "timeout occurred awaiting server response" );
                  }
                  else
                     throw runtime_error( "server has terminated this connection" );
               }

               // NOTE: If the response has no output then clear the OUTPUT environment variable.
               if( is_first && response == string( c_response_okay ) )
                  set_environment_variable( c_env_var_output, "" );

               is_first = false;

               if( was_chk || was_msg || was_get_or_put )
               {
                  // FUTURE: Verify the hash if "was_chk_token".
                  if( was_msg || was_chk_tag || was_chk_token )
                  {
                     handle_command_response( response );

                     response.erase( );
                     socket.read_line( response );
                  }

                  if( was_get_or_put || ( response != string( c_response_okay ) ) )
                  {
                     string::size_type pos = response.find( ' ' );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected response: " + response );

                     unsigned char prefix( c_file_type_char_blob );

                     ft_extra_info ft_extra( c_file_transfer_initial_timeout,
                      c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );

                     if( response.substr( 0, pos ) == c_session_cmd_get )
                     {
                        file_transfer( response.substr( pos + 1 ),
                         socket, e_ft_direction_send, g_max_file_size, c_response_okay_more, &ft_extra );
                     }
                     else if( response.substr( 0, pos ) == c_session_cmd_put )
                     {
                        file_transfer( response.substr( pos + 1 ),
                         socket, e_ft_direction_recv, g_max_file_size, c_response_okay_more, &ft_extra );

                        // NOTE: If is first "chk" and *not found* is returned then it is next expected
                        // that a file will be be sent and then fetched to ensure that the peer is able
                        // to store and fetch back the same file content.
                        if( was_chk_tag && !had_chk_command )
                        {
                           file_transfer( response.substr( pos + 1 ), socket,
                            e_ft_direction_send, g_max_file_size, c_response_okay_more, &ft_extra );
                        }
                     }
                     else if( response.substr( 0, pos ) == c_session_cmd_chk )
                     {
                        response.erase( 0, pos + 1 );

                        pos = response.find( ' ' );

                        if( pos == string::npos )
                           throw runtime_error( "unexpected chk args '" + response + "'" );

                        string hash( response.substr( 0, pos ) );
                        string token( response.substr( pos + 1 ) );

                        sha256 temp_hash;
                        temp_hash.update( token );
                        temp_hash.update( hash, true );

                        string hash_val( temp_hash.get_digest_as_string( ) );
                        handle_command_response( hash_val );

                        socket.write_line( hash_val );

                        response.erase( );
                        socket.read_line( response );
                     }
                     else if( response.substr( 0, pos ) == "msg" )
                     {
                        handle_command_response( response );

                        socket.write_line( response.substr( pos + 1 ) );

                        response.erase( );
                        socket.read_line( response );
                     }

                     was_msg = false;
                     was_chk_tag = false;
                     was_chk_token = false;
                     was_get_or_put = false;
                  }
               }

               if( was_chk )
                  had_chk_command = true;

               had_not_found = false;
               is_in_progress = false;

               if( response == string( c_response_okay_more ) )
               {
#ifdef DEBUG
                  cerr << response;
#endif
                  response.erase( );
                  continue;
               }
               else if( response == string( c_response_not_found ) )
               {
                  if( was_chk )
                  {
                     handle_command_response( response );

                     response.erase( );
                     continue;
                  }

                  had_not_found = true;
#ifdef DEBUG
                  cerr << response;
#endif
                  response = string( c_not_found_output );
               }

               if( response != string( c_response_okay ) )
               {
                  size_t start = 0;

                  bool is_error = false;

                  size_t err_prefix_length( strlen( c_response_error_prefix ) );

                  if( response.length( ) > err_prefix_length
                   && response.substr( 0, err_prefix_length ) == string( c_response_error_prefix ) )
                  {
                     is_error = true;
                     start = err_prefix_length;
                  }

                  bool is_message = false;

                  size_t msg_prefix_length( strlen( c_response_message_prefix ) );

                  if( response.length( ) > msg_prefix_length
                   && response.substr( 0, msg_prefix_length ) == string( c_response_message_prefix ) )
                  {
                     is_message = true;
                     start = msg_prefix_length;
                  }

                  string final_response( response.substr( start ) );

                  if( is_error )
                     final_response = string( c_error_output_prefix ) + final_response;

                  if( !is_message && had_message )
                  {
                     had_message = false;

                     if( is_stdout_console( ) )
                        progress.output_progress( "" );
                     else if( had_single_char_message )
                        handle_progress_message( "" );

                     had_single_char_message = false;
                  }

                  if( is_message )
                  {
                     had_message = true;

                     if( ( final_response != string( " " ) )
                      && ( final_response.length( ) == 1 ) )
                     {
                        if( !had_single_char_message )
                        {
                           // NOTE: Double the single character response in order
                           // to first erase the prior progress percentage output.
                           if( had_chunk_progress && is_stdout_console( ) )
                           {
                              num_singles = 2;
                              progress.output_progress( final_response + final_response );
                           }
                        }

                        had_single_char_message = true;
                     }

                     date_time now( date_time::local( ) );

                     uint64_t elapsed = seconds_between( dtm, now );

                     // NOTE: Only checks the elapsed time for single character
                     // messages (assumes all other messages sent by the server
                     // are intended to always be output).
                     if( !had_single_char_message || ( elapsed >= g_seconds ) )
                     {
                        dtm = now;

                        if( !has_option_no_progress( ) )
                        {
                           if( !is_stdout_console( ) )
                              handle_progress_message( final_response );
                           else
                           {
                              if( had_single_char_message )
                              {
                                 ++num_singles;
                                 progress.output_prefix.erase( );
                              }

                              string progress_prefix( get_environment_variable( c_env_var_progress_prefix ) );

                              set_environment_variable( c_env_var_progress_prefix, "" );

                              progress.output_progress( progress_prefix + final_response );
                           }
                        }
                     }
                  }
                  else if( is_error || !get_is_quiet_command( ) )
                     handle_command_response( final_response, is_error );

                  // NOTE: Will only set the error environment variable if hasn't already been set.
                  if( is_error && get_environment_variable( c_env_var_error ).empty( ) )
                     set_environment_variable( c_env_var_error, response.substr( start ).c_str( ) );

                  if( !is_error && !is_message )
                  {
                     if( response.length( ) <= c_max_length_for_output_env_var )
                        set_environment_variable( c_env_var_output, response.substr( start ).c_str( ) );
                     else
                        set_environment_variable( c_env_var_output, "*** response exceeded maximum allowed length ***" );
                  }

                  // NOTE: Make sure that progress messages do not end the conversation.
                  if( is_message )
                  {
                     response.erase( );
                     is_in_progress = true;
                  }

                  if( had_not_found )
                     break;
               }
               else
               {
                  if( had_message )
                  {
                     had_message = false;

                     if( !has_option_no_progress( ) )
                     {
                        if( is_stdout_console( ) )
                        {
                           if( !num_singles )
                              progress.output_progress( "" );
                           else
                           {
                              string eraser( num_singles, '\b' );
                              eraser += string( num_singles, ' ' ) + string( num_singles, '\b' );

                              cout << eraser;
                           }

                           progress.output_length = 0;
                           progress.output_prefix.erase( );

                           if( !was_wait && had_chunk_progress )
                              cout << endl;
                        }
                        else if( had_single_char_message )
                           handle_progress_message( "" );
                     }

                     had_chunk_progress = false;
                  }

                  had_single_char_message = false;
#ifdef DEBUG
                  cerr << response;
#endif
               }
            }

            clear_key( str );
            str.erase( );
         }
      }
   }
}

void ciyam_console_command_handler::process_custom_startup_option( size_t num, const string& option )
{
   if( num == 0 )
      parse_host_and_or_port( option, host, port );
   else
      throw runtime_error( "unexpected custom startup option '" + option + "'" );
}

}

int main( int argc, char* argv[ ] )
{
   int rc = 0;

#ifdef SSL_SUPPORT
   if( file_exists( c_ciyam_pem ) )
      init_ssl( c_ciyam_pem );
#endif

#ifdef SSL_SUPPORT
   ssl_socket socket;

   gup_priv_key.reset( new private_key( ) );
#else
   tcp_socket socket;
#endif

   udp_socket usocket;

   ciyam_console_command_handler cmd_handler( socket, usocket );

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         cmd_handler.add_command( c_cmd_tls, 1,
          "", "start TLS after connecting", new ciyam_console_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_exec, 1,
          "<val//command>[<list//arguments// >]", "single command to execute", new ciyam_console_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_vars, 1,
          "<list//name_vals//,>", "set environment variables", new ciyam_console_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_args_file, 1,
          "<val//name>", "name of console args file", new ciyam_console_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_rpc_unlock, 1,
          "<val//password>", "RPC access unlock password", new ciyam_console_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_connect_retries, 2,
          "<val//max_attempts>", "server connection retries", new ciyam_console_startup_functor( cmd_handler ) );

         processor.process_commands( );
      }

      const char* p_pid = getenv( c_env_var_pid );

      if( p_pid && *p_pid )
         g_pid = atoi( p_pid );

      const char* p_seconds = getenv( c_env_var_ciyam_seconds );

      if( p_seconds && *p_seconds )
         g_seconds = atoi( p_seconds );

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      bool no_udp = has_environment_variable( c_env_var_no_udp );
      bool local_udp = has_environment_variable( c_env_var_local_udp );

      if( socket.open( ) )
      {
         ip_address address( cmd_handler.get_host( ), cmd_handler.get_port( ) );

         bool is_default = false;

         if( string( cmd_handler.get_host( ) ) == c_default_ciyam_host )
            is_default = true;

         bool okay = socket.connect( address, c_connect_timeout );

         // NOTE: The "connect_retries" option can be useful where the
         // application server is started (in the background) prior to
         // starting "ciyam_client" - a small delay will occur between
         // each attempt and after the connection failure a new socket
         // needs to be used (hence the "close" and "open" calls).
         while( !okay && g_connect_retries )
         {
            --g_connect_retries;

            socket.close( );

            msleep( 250 );

            if( socket.open( ) )
               okay = socket.connect( address, c_connect_timeout );
         }

         if( okay )
         {
#ifdef USE_NO_DELAY
            if( !socket.set_no_delay( ) )
               cout << "warning: set_no_delay failed..." << endl;
#endif
            if( !no_udp && ( local_udp || !is_default ) && usocket.open( ) )
               usocket.set_reuse_addr( );

#ifdef SSL_SUPPORT
            if( g_use_tls )
               socket.ssl_connect( c_tls_timeout );
#endif

            if( socket.write_line( to_string( g_pid ) + c_key_exchange_suffix, c_pid_timeout ) <= 0 )
            {
               string error;

               if( socket.had_timeout( ) )
                  error = "timeout occurred trying to connect to server";
               else
                  error = "unable to connect to the application server (not active?)";

               socket.close( );
               usocket.close( );

               throw runtime_error( error );
            }

            string greeting;

            if( socket.read_line( greeting, c_greeting_timeout ) <= 0 )
            {
               string error;

               if( socket.had_timeout( ) )
                  error = "timeout occurred trying to connect to server";
               else
                  error = "application server has terminated this connection";

               socket.close( );
               usocket.close( );

               throw runtime_error( error );
            }

            version_info ver_info;

            if( get_version_info( greeting, ver_info ) != string( c_response_okay ) )
            {
               socket.close( );
               usocket.close( );

               size_t start = 0;

               size_t err_prefix_length( strlen( c_response_error_prefix ) );

               if( greeting.length( ) > err_prefix_length
                && greeting.substr( 0, err_prefix_length ) == string( c_response_error_prefix ) )
                  start = err_prefix_length;

               throw runtime_error( greeting.substr( start ) );
            }

            if( !check_version_info( ver_info, c_protocol_major_version, c_protocol_minor_version ) )
            {
               socket.close( );
               usocket.close( );

               throw runtime_error( "incompatible protocol version "
                + ver_info.ver + " (expecting " + string( c_protocol_version ) + ")" );
            }

            string max_file_size( get_environment_variable( c_env_var_max_file_size ) );

            if( !max_file_size.empty( ) )
               g_max_file_size = unformat_bytes( max_file_size );
            else if( !ver_info.extra.empty( ) )
            {
               string::size_type pos = ver_info.extra.rfind( ']' );

               if( pos != string::npos )
                  ver_info.extra.erase( pos );

               pos = ver_info.extra.find( '[' );

               if( pos != string::npos )
                  ver_info.extra.erase( 0, pos + 1 );

               g_max_file_size = unformat_bytes( ver_info.extra );
               set_environment_variable( c_env_var_max_file_size, format_bytes( g_max_file_size, false, 0, '\0' ) );
            }

            // NOTE: After handshake exchange public keys then commence command handling.
            string pubkey;

#ifdef SSL_SUPPORT
            pubkey = gup_priv_key->get_public( );
#endif
            if( pubkey.empty( ) )
               pubkey = string( c_none );
            else
               set_environment_variable( c_env_var_pub_key, pubkey );

            string slotx, pubkeyx, slotx_and_pubkeyx;
            socket.read_line( slotx_and_pubkeyx, c_pubkey_timeout );

            string::size_type pos = slotx_and_pubkeyx.find( '-' );

            if( pos != string::npos )
            {
               slotx = slotx_and_pubkeyx.substr( 0, pos );
               pubkeyx = slotx_and_pubkeyx.substr( pos + 1 );
            }

            if( !slotx.empty( ) )
               set_environment_variable( c_env_var_slotx, slotx );

            if( !pubkeyx.empty( ) && pubkeyx != string( c_none ) )
               set_environment_variable( c_env_var_pub_keyx, pubkeyx );

            string slot( slotx );

            if( !slot.empty( ) )
               slot[ 0 ] = 'C';
            else
               slot = string( c_none );

            set_environment_variable( c_env_var_slot, slot );

            string slot_and_pubkey( slot + '-' + pubkey );

            socket.write_line( slot_and_pubkey, c_pubkey_timeout );

            console_command_processor processor( cmd_handler );

            if( g_rpc_password.empty( ) )
               g_rpc_password = get_environment_variable( c_env_var_rpc_password );
            if( !g_rpc_password.empty( ) )
            {
               if( g_rpc_password == "?" )
                  g_rpc_password = get_password( "RPC Password: " );

               processor.execute_command( g_quiet_cmd_prefix
                + string( c_session_cmd_session_rpc_unlock ) + " \"" + g_rpc_password + "\"" );

               clear_key( g_rpc_password );
            }

            if( !g_args_file.empty( ) )
               processor.execute_command( g_quiet_cmd_prefix
                + string( c_session_cmd_session_variable ) + " @args_file \"" + g_args_file + "\"" );

            if( g_exec_cmd.empty( ) )
               processor.process_commands( );
            else
               processor.execute_command( g_exec_cmd );
         }
         else
         {
            socket.close( );

            if( string( cmd_handler.get_host( ) ) == c_local_host )
            {
               string web_dir( get_environment_variable( c_env_var_webdir ) );

               // NOTE: If the global FCGI UI "stop" file is found then is
               // likely to be performing an ODS DB reconstruction so will
               // prompt the user to check its log.
               if( !web_dir.empty( ) )
               {
                  if( file_exists( web_dir + '/' + c_ciyam_ui_stop_file ) )
                     throw runtime_error( "(application server appears to be busy - check its log file)" );
               }
            }

            throw runtime_error( "unable to connect to host '"
             + string( cmd_handler.get_host( ) ) + "' on port " + to_string( cmd_handler.get_port( ) ) );
         }

         socket.close( );
         usocket.close( );
      }
#ifdef __GNUG__
      if( isatty( STDIN_FILENO ) && g_exec_cmd.empty( ) )
         cout << endl;
#endif
   }
   catch( exception& x )
   {
      rc = 2;
      cerr << "error: " << x.what( ) << endl;
   }
   catch( ... )
   {
      rc = 3;
      cerr << "error: unexpected unknown exception caught" << endl;
   }

   // NOTE: Return an error if a script has set the ERROR variable.
   if( !rc && !get_environment_variable( c_env_var_error ).empty( ) )
      rc = 1;

#ifdef SSL_SUPPORT
   term_ssl( );
#endif

   return rc;
}

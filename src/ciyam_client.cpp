// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2021 CIYAM Developers
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
#  include <string>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#include "regex.h"
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
#  ifdef _WIN32
#     include <openssl/applink.c>
#  endif
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

const char* const c_cmd_args_file = "args_file";
const char* const c_cmd_args_file_name = "name";

const char* const c_cmd_rpc_unlock = "rpc_unlock";
const char* const c_cmd_rpc_unlock_password = "password";

const char* const c_env_var_pid = "PID";
const char* const c_env_var_error = "ERROR";
const char* const c_env_var_output = "OUTPUT";
const char* const c_env_var_rpc_password = "RPC_PASSWORD";
const char* const c_env_var_max_file_size = "MAX_FILE_SIZE";

const char* const c_list_file_ext = ".list";

const char* const c_not_found_output = "Not Found";
const char* const c_error_output_prefix = "Error: ";

const size_t c_pid_timeout = 5000;
const size_t c_command_timeout = 60000;
const size_t c_connect_timeout = 10000;
const size_t c_greeting_timeout = 10000;

const size_t c_default_max_file_size = 1000000;

#ifdef _WIN32
const size_t c_max_length_for_output_env_var = 1000;
#else
const size_t c_max_length_for_output_env_var = 100000;
#endif

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

bool g_had_error = false;

bool g_use_tls = false;

size_t g_max_file_size = c_default_max_file_size;

string g_exec_cmd;
string g_args_file;
string g_rpc_password;

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
      else if( command == c_cmd_args_file )
         g_args_file = get_parm_val( parameters, c_cmd_args_file_name );
      else if( command == c_cmd_rpc_unlock )
         g_rpc_password = get_parm_val( parameters, c_cmd_rpc_unlock_password );
   }
};

class ciyam_console_command_handler : public console_command_handler
{
   public:
#ifdef SSL_SUPPORT
   ciyam_console_command_handler( ssl_socket& socket )
#else
   ciyam_console_command_handler( tcp_socket& socket )
#endif
    :
    port( c_default_ciyam_port ),
    host( c_default_ciyam_host ),
    chunk( 0 ),
    file_pos( 0 ),
    file_bytes( 0 ),
    chunk_size( 0 ),
    total_chunks( 0 ),
    socket( socket ),
    had_message( false ),
    had_chk_command( false ),
    had_chunk_progress( false )
   {
      set_custom_startup_options( 1, "[<port> or <host[(:|-)port]>]" );
   }

   const char* get_host( ) const { return host.c_str( ); }
   int get_port( ) const { return port; }

   private:
   int port;
   string host;

   unsigned long chunk;

   date_time dtm;

   long file_pos;
   long file_bytes;

   unsigned long chunk_size;
   unsigned long total_chunks;

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

   bool had_message;
   bool had_chk_command;
   bool had_chunk_progress;

   string get_additional_command( );

   string preprocess_command_and_args( const string& cmd_and_args );

   void process_custom_startup_option( size_t num, const string& option );
};

string ciyam_console_command_handler::get_additional_command( )
{
   string cmd;

   if( !additional_commands.empty( ) )
   {
      cmd = additional_commands.front( );
      additional_commands.pop_front( );
   }
   else if( had_chunk_progress )
   {
      if( !is_stdout_console( ) )
         cout << endl;
      else
         progress.output_progress( "" );

      had_chunk_progress = false;
   }

   return cmd;
}

string ciyam_console_command_handler::preprocess_command_and_args( const string& cmd_and_args )
{
   if( is_handling_startup_options( ) )
      return console_command_handler::preprocess_command_and_args( cmd_and_args );

   string str( cmd_and_args );

   if( !str.empty( ) )
   {
      if( str[ 0 ] != '?' && str.substr( 0, str.find( ' ' ) ) != "help" )
         str = console_command_handler::preprocess_command_and_args( str );

      if( !str.empty( ) )
      {
         bool was_chk = false;
         bool was_pip = false;
         bool was_chk_tag = false;
         bool was_chk_token = false;
         bool was_list_prefix = false;
         bool was_no_compress = false;
         bool delete_after_put = false;

         string get_dest_file, put_source_file;

         string::size_type pos = str.find( ' ' );

         if( str.substr( 0, pos ) == "pip" )
            was_pip = true;

         if( str.substr( 0, pos ) == "chk"
          || str.substr( 0, pos ) == "get" || str.substr( 0, pos ) == "put"
          || str.substr( 0, pos ) == "file_get" || str.substr( 0, pos ) == "file_put" )
         {
            string data( str.substr( pos + 1 ) );
            string extra;

            if( str.substr( 0, pos ) == "chk" )
            {
               was_chk = true;
               if( data.find( ' ' ) != string::npos )
                  was_chk_token = true;
            }

            if( str.substr( 0, pos ) == "get"
             || str.substr( 0, pos ) == "file_get" || str.substr( 0, pos ) == "file_put" )
            {
               string::size_type spos = data.find( ' ' );

               if( spos != string::npos )
               {
                  if( str.substr( 0, pos ) == "file_put" )
                     extra = data.substr( spos );
                  else
                     get_dest_file = data.substr( spos + 1 );

                  data.erase( spos );
               }
            }

            string prefix( c_file_type_str_blob );

            regex expr( c_regex_hash_256 );
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

            if( str.substr( 0, pos ) == "put" || str.substr( 0, pos ) == "file_put" )
            {
               str.erase( pos + 1 );

               put_source_file = data;

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

                        file_pos = 0;
                        file_bytes = 0;

                        if( extra.size( ) > 0 )
                           file_extra = extra.substr( 1 );

                        extra.erase( );
                        file_list_data.clear( );

                        chunk_size = unformat_bytes( put_source_file.substr( 0, pos ) );
                        put_source_file.erase( 0, pos + 1 );

                        if( chunk_size <= 1 )
                           throw runtime_error( "chunk size too small" );

                        // NOTE: Reduce the chunk size by one due to the one byte "type" prefix.
                        --chunk_size;

                        file_name = put_source_file;

                        pos = file_extra.find( '?' );

                        if( pos == string::npos )
                           file_strip_prefix.erase( );
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
               else if( chunk == 0 )
               {
                  int list_items_per_chunk = g_max_file_size / ( 65 + file_name.length( ) + 7 );
                  int64_t total_file_size = ( list_items_per_chunk * g_max_file_size ) - g_max_file_size;

                  if( file_size( put_source_file ) > total_file_size )
                     put_file_error = "File '" + put_source_file
                      + "' exceeds maximum file size (" + format_bytes( total_file_size ) + ").";

                  dtm = date_time::local( );
                  had_chunk_progress = false;
               }

               if( !put_file_error.empty( ) )
               {
                  cerr << c_error_output_prefix << put_file_error << endl;

                  // NOTE: Will only set the error environment variable if hasn't already been set.
                  if( get_environment_variable( c_env_var_error ).empty( ) )
                     set_environment_variable( c_env_var_error, put_file_error.c_str( ) );

                  chunk_size = 0;

                  return string( );
               }

               string chunk_name;

               if( chunk_size )
               {
                  // NOTE: In order to maximumise the file transfer throughput
                  // assume that the large files have already been compressed.
                  prefix = string( c_file_type_str_blob_no_compress );

                  was_no_compress = true;

                  chunk_name = file_name;
                  chunk_name += '.' + to_comparable_string( chunk, false, 6 );

                  string chunk_data;

                  if( chunk == 0 )
                  {
                     chunk_data = buffer_file( file_name, chunk_size, &file_bytes );
                     total_chunks = ( file_bytes / chunk_size );
                  }
                  else
                     chunk_data = buffer_file( file_name, chunk_size, 0, ( chunk * chunk_size ) );

                  write_file( chunk_name, chunk_data );

                  if( !has_option_no_prompt( ) )
                  {
                     date_time now( date_time::local( ) );

                     uint64_t elapsed = seconds_between( dtm, now );

                     if( elapsed >= 1 )
                     {
                        if( is_stdout_console( ) )
                           progress.output_progress( "Transferring file...", chunk, total_chunks );
                        else
                        {
                           cout << '.';
                           cout.flush( );
                        }

                        dtm = now;
                        had_chunk_progress = true;
                     }
                  }

                  delete_after_put = true;
                  put_source_file = chunk_name;

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
                     additional_commands.push_back( " .file_put " + file_name );
                  }
               }
               else if( !file_name.empty( ) )
               {
                  was_list_prefix = true;
                  prefix = string( c_file_type_str_list );

                  string list_name( file_name );
                  list_name += string( c_list_file_ext );

                  delete_after_put = true;
                  put_source_file = list_name;

                  file_name.erase( );
               }

               sha256 tmp_hash( prefix );
               tmp_hash.update( put_source_file, true );

               string hash( tmp_hash.get_digest_as_string( ) );

               if( !chunk_name.empty( ) )
               {
                  if( !file_list_data.empty( ) )
                     file_list_data += '\n';

                  string item_name( chunk_name );

                  if( !file_strip_prefix.empty( ) && item_name.find( file_strip_prefix ) == 0 )
                     item_name.erase( 0, file_strip_prefix.length( ) );

                  file_list_data += hash + ' ' + item_name;

                  if( file_bytes == 0 )
                  {
                     string list_name( file_name );
                     list_name += string( c_list_file_ext );

                     write_file( list_name, file_list_data );

                     if( !file_extra.empty( ) )
                        file_extra = " " + file_extra;

                     // NOTE: Ensure that the additional command will neither be
                     // included in console history nor sent to standard output.
                     additional_commands.push_back( " .file_put " + file_name + file_extra );
                  }
               }

               str += hash + extra;
#ifdef DEBUG
               handle_command_response( str );
#endif
            }
         }

         if( str == "encrypt" )
            str += " " + get_password( "Enter Key/Data/Password: " );
#ifdef DEBUG
         cout << "sending command: " << str << endl;
#endif
         socket.write_line( str );

#ifdef SSL_SUPPORT
         if( ( str == "tls" || str == "starttls" ) && !socket.is_secure( ) )
            socket.ssl_connect( );
#endif
         if( str == "bye" || str == "quit" )
         {
            str.erase( );
            set_finished( );
         }
         else
         {
            bool was_get_or_put = false;
            bool ignore_exception = false;

            try
            {
               string::size_type pos = str.find( ' ' );

               if( str.substr( 0, pos ) == "get" || str.substr( 0, pos ) == "put" )
                  was_get_or_put = true;

               if( str.substr( 0, pos ) == "get" || str.substr( 0, pos ) == "file_get" )
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

                  file_transfer( filename, socket,
                   ( !appending ? e_ft_direction_recv : e_ft_direction_recv_app ),
                   c_max_file_transfer_size, c_response_okay_more, c_file_transfer_initial_timeout,
                   c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );

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
                        split( all_chunks, chunks, '\n' );

                        total_chunks = chunks.size( );

                        for( size_t i = 0; i < chunks.size( ); i++ )
                        {
                           string next( chunks[ i ] );
                           string::size_type pos = next.find( ' ' );

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
                     ++chunk;

                     delete_after_transfer = true;

                     if( append_prefix.empty( ) )
                        file_append( filename, append_filename );
                     else
                     {
                        // NOTE: It is assumed that a file chunk number is a final file extension.
                        pos = append_filename.rfind( '.' );

                        string name_without_chunk_ext( append_filename.substr( 0, pos ) );
                        string prefixed_append_name( append_prefix + '/' + name_without_chunk_ext );

                        if( append_last_name != name_without_chunk_ext )
                        {
                           file_remove( prefixed_append_name );
                           handle_command_response( prefixed_append_name );
                        }

                        append_last_name = name_without_chunk_ext;

                        create_directories( prefixed_append_name );
                        file_append( filename, prefixed_append_name );
                     }

                     if( !has_option_no_prompt( ) )
                     {
                        date_time now( date_time::local( ) );

                        uint64_t elapsed = seconds_between( dtm, now );

                        if( elapsed >= 1 )
                        {
                           if( is_stdout_console( ) )
                              progress.output_progress( "Transferring file...", chunk, total_chunks );
                           else
                           {
                              cout << '.';
                              cout.flush( );
                           }

                           dtm = now;
                           had_chunk_progress = true;
                        }
                     }
                  }

                  if( delete_after_transfer )
                     file_remove( filename );
               }
               else if( str.substr( 0, pos ) == "put" || str.substr( 0, pos ) == "file_put" )
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

                  file_transfer( filename, socket,
                   e_ft_direction_send, c_max_file_transfer_size,
                   c_response_okay_more, c_file_transfer_initial_timeout,
                   c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );

                  if( delete_after_put )
                  {
                     delete_after_put = false;
                     file_remove( put_source_file );
                  }
               }
            }
            catch( exception& x )
            {
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

               return str;
            }

            string response;

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

               // NOTE: If the reponse has no output then clear the OUTPUT environment variable.
               if( is_first && response == string( c_response_okay ) )
                  set_environment_variable( c_env_var_output, "" );

               is_first = false;

               if( was_chk || was_pip || was_get_or_put )
               {
                  // FUTURE: Verify the hash if "was_chk_token".
                  if( was_pip || was_chk_tag || was_chk_token )
                  {
                     handle_command_response( response );

                     response.erase( );
                     socket.read_line( response );
                  }

                  if( was_get_or_put || response != string( c_response_okay ) )
                  {
                     string::size_type pos = response.find( ' ' );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected response: " + response );

                     if( response.substr( 0, pos ) == "get" )
                     {
                        unsigned char prefix( c_file_type_char_blob );

                        file_transfer( response.substr( pos + 1 ),
                         socket, e_ft_direction_send, c_max_file_transfer_size, c_response_okay_more,
                         c_file_transfer_initial_timeout, c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );
                     }
                     else if( response.substr( 0, pos ) == "put" )
                     {
                        unsigned char prefix( c_file_type_char_blob );

                        file_transfer( response.substr( pos + 1 ),
                         socket, e_ft_direction_recv, c_max_file_transfer_size, c_response_okay_more,
                         c_file_transfer_initial_timeout, c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );

                        // NOTE: If is first "chk" and *not found* is returned then it is next expected
                        // that a file will be be sent and then fetched to ensure that the peer is able
                        // to store and fetch back the same file content.
                        if( was_chk_tag && !had_chk_command )
                        {
                           file_transfer( response.substr( pos + 1 ), socket,
                            e_ft_direction_send, c_max_file_transfer_size, c_response_okay_more,
                            c_file_transfer_initial_timeout, c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );
                        }
                     }
                     else if( response.substr( 0, pos ) == "chk" )
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
                     else if( response.substr( 0, pos ) == "pip" )
                     {
                        handle_command_response( response );

                        socket.write_line( c_local_ip_addr );

                        response.erase( );
                        socket.read_line( response );
                     }

                     was_pip = false;
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
                  cout << response;
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
                  cout << response;
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
                     g_had_error = true;
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
                  }

                  if( is_message && !get_is_quiet_command( ) )
                  {
                     had_message = true;

                     if( !is_stdout_console( ) )
                        handle_progress_message( final_response );
                     else
                        progress.output_progress( final_response );
                  }
                  else if( !is_message && ( is_error || !get_is_quiet_command( ) ) )
                     handle_command_response( final_response, is_error );

                  // NOTE: Will only set the error environment variable if hasn't already been set.
                  if( is_error && get_environment_variable( c_env_var_error ).empty( ) )
                     set_environment_variable( c_env_var_error, response.substr( start ).c_str( ) );

                  if( !is_error && !is_message && response.length( ) <= c_max_length_for_output_env_var )
                  {
                     if( response.length( ) <= c_max_length_for_output_env_var )
                        set_environment_variable( c_env_var_output, response.substr( start ).c_str( ) );
                     else
                        set_environment_variable( c_env_var_output, "*** reponse exceeded maximum allowed length ***" );
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

                     if( is_stdout_console( ) )
                        progress.output_progress( "" );
                  }
#ifdef DEBUG
                  cout << response;
#endif
               }
            }

            str.erase( );
         }
      }
   }

   return str;
}

void ciyam_console_command_handler::process_custom_startup_option( size_t num, const string& option )
{
   if( num == 0 )
   {
      if( !option.empty( )
       && option.find( '.' ) == string::npos
       && option[ 0 ] >= '0' && option[ 0 ] <= '9' )
         port = atoi( option.c_str( ) );
      else
      {
         host = option;
         string::size_type pos = host.find( ':' );

         // NOTE: If host is an IPV6 address then use '-' as the port separator.
         if( pos == string::npos || host.find( ':', pos + 1 ) != string::npos )
            pos = host.find( '-' );

         if( pos != string::npos )
         {
            port = atoi( host.substr( pos + 1 ).c_str( ) );
            host.erase( pos );
         }
      }
   }
   else
      throw runtime_error( "unexpected custom startup option '" + option + "'" );
}

}

int main( int argc, char* argv[ ] )
{
   int rc = 0;

#ifdef _WIN32
   winsock_init wsi;
#endif   

#ifdef SSL_SUPPORT
   if( file_exists( "ciyam_client.pem" ) )
      init_ssl( "ciyam_client.pem", "password" );
#endif

#ifdef SSL_SUPPORT
   ssl_socket socket;
#else
   tcp_socket socket;
#endif

   ciyam_console_command_handler cmd_handler( socket );

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         cmd_handler.add_command( c_cmd_tls, 1,
          "", "start TLS after connecting", new ciyam_console_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_exec, 1,
          "<val//command>[<list//arguments// >]", "single command to execute", new ciyam_console_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_args_file, 1,
          "<val//name>", "name of console args file", new ciyam_console_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_rpc_unlock, 1,
          "<val//password>", "RPC access unlock password", new ciyam_console_startup_functor( cmd_handler ) );

         processor.process_commands( );
      }

      const char* p_pid = getenv( c_env_var_pid );

      if( p_pid )
         g_pid = atoi( p_pid );

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      if( socket.open( ) )
      {
         ip_address address( cmd_handler.get_host( ), cmd_handler.get_port( ) );

         bool is_default = false;
         if( string( cmd_handler.get_host( ) ) == c_default_ciyam_host )
            is_default = true;

#ifdef _WIN32
         if( socket.connect( address, is_default ? 1000 : c_connect_timeout ) )
#else
         // NOTE: If the server was started asynchronously in a script immediately prior
         // to the client then wait for half a second and then try again just to be sure.
         if( socket.connect( address ) || ( is_default && ( msleep( 500 ), socket.connect( address ) ) ) )
#endif
         {
#ifdef USE_NO_DELAY
            if( !socket.set_no_delay( ) )
               cout << "warning: set_no_delay failed..." << endl;
#endif
            if( socket.write_line( to_string( g_pid ), c_pid_timeout ) <= 0 )
            {
               string error;
               if( socket.had_timeout( ) )
                  error = "timeout occurred trying to connect to server";
               else
                  error = "unable to connect to the application server (not active?)";

               socket.close( );
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
               throw runtime_error( error );
            }

            version_info ver_info;
            if( get_version_info( greeting, ver_info ) != string( c_response_okay ) )
            {
               socket.close( );
               throw runtime_error( greeting );
            }

            if( !check_version_info( ver_info, c_protocol_major_version, c_protocol_minor_version ) )
            {
               socket.close( );
               throw runtime_error( "incompatible protocol version "
                + ver_info.ver + " (expecting " + string( c_protocol_version ) + ")" );
            }

            if( !ver_info.extra.empty( ) )
            {
               g_max_file_size = from_string< size_t >( ver_info.extra );
               set_environment_variable( c_env_var_max_file_size, ver_info.extra.c_str( ) );
            }

            console_command_processor processor( cmd_handler );

            if( g_use_tls )
               processor.execute_command( ".tls" );

            if( g_rpc_password.empty( ) )
               g_rpc_password = get_environment_variable( c_env_var_rpc_password );
            if( !g_rpc_password.empty( ) )
            {
               if( g_rpc_password == "?" )
                  g_rpc_password = get_password( "RPC Password: " );

               processor.execute_command( ".session_rpc_unlock \"" + g_rpc_password + "\"" );
               clear_key( g_rpc_password );
            }

            if( !g_args_file.empty( ) )
               processor.execute_command( ".session_variable @args_file \"" + g_args_file + "\"" );

            if( g_exec_cmd.empty( ) )
               processor.process_commands( );
            else
               processor.execute_command( g_exec_cmd );
         }
         else
            throw runtime_error( "unable to connect to host '"
             + string( cmd_handler.get_host( ) ) + "' on port " + to_string( cmd_handler.get_port( ) ) );

         socket.close( );
      }
#ifdef __GNUG__
      if( isatty( STDIN_FILENO ) )
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

#ifdef SSL_SUPPORT
   term_ssl( );
#endif

   if( g_had_error )
      rc = 1;

   return rc;
}

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
#  include <csignal>
#  include <cstdlib>
#  include <memory>
#  include <sstream>
#  include <iomanip>
#  include <iostream>
#  include <stdexcept>
#  include <pthread.h>
#  ifdef __GNUG__
#     include <unistd.h>
#     include <sys/stat.h>
#  endif
#endif

#include "config.h"
#include "sockets.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "udp_stream.h"
#include "auto_script.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#endif
#include "peer_session.h"
#include "ciyam_session.h"
#include "dynamic_library.h"
#include "console_commands.h"

using namespace std;

sigset_t sig_set;

size_t g_active_sessions = 0;
size_t g_active_listeners = 0;

volatile sig_atomic_t g_server_shutdown = 0;

namespace
{

bool g_start_autoscript = true;
bool g_start_peer_sessions = true;
bool g_start_udp_stream_sessions = true;

void* signal_handler( void* id )
{
   int sig;

   sigwait( &sig_set, &sig );

   ++g_server_shutdown;

   return id;
}

const char* const c_app_title = "ciyam_server";
const char* const c_app_version = "0.1";

const char* const c_cmd_sid = "sid";
const char* const c_cmd_sid_entropy = "entropy";

const char* const c_cmd_chdir = "chdir";
const char* const c_cmd_chdir_directory = "directory";

const char* const c_cmd_quiet = "quiet";

const char* const c_cmd_trace = "trace";
const char* const c_cmd_trace_flags = "flags";

const char* const c_cmd_log_dir = "log_dir";
const char* const c_cmd_log_dir_directory = "directory";

const char* const c_cmd_files_area_dir = "files_area_dir";
const char* const c_cmd_files_area_dir_directory = "directory";

const char* const c_cmd_no_auto = "no_auto";
const char* const c_cmd_no_peers = "no_peers";
const char* const c_cmd_no_streams = "no_streams";

const char* const c_cmd_test_peer_port = "test_peer_port";
const char* const c_cmd_test_peer_port_port = "port";

bool g_is_quiet = false;
bool g_had_exiting_command = false;

#ifdef __GNUG__
const char* const c_cmd_daemon = "daemon";

bool g_is_daemon = false;
#endif

unsigned int g_port = c_default_ciyam_port;

string g_entropy;

uint32_t g_trace_flags = 0;

bool g_set_trace_flags = false;

string g_log_files_directory;

string g_files_area_directory;

unsigned int g_test_peer_port = 0;

const int c_accept_timeout = 500;
const int c_active_start_delay = 250;

const int c_max_wait_attempts = 20;

const char* const c_server_cmd_file = "ciyam_server.cmd";
const char* const c_update_signal_file = "ciyam_base.update";
const char* const c_shutdown_signal_file = "ciyam_server.stop";

const char* const c_ciyam_base_lib = "ciyam_base.so";

const char* const c_trace_flags_func_name = "trace_flags";
const char* const c_init_globals_func_name = "init_globals";
const char* const c_term_globals_func_name = "term_globals";
const char* const c_server_command_func_name = "server_command";
const char* const c_set_server_port_func_name = "set_server_port";
const char* const c_init_auto_script_func_name = "init_auto_script";
const char* const c_init_udp_streams_func_name = "init_udp_streams";
const char* const c_log_trace_string_func_name = "log_trace_string";
const char* const c_register_listener_func_name = "register_listener";
const char* const c_set_log_files_dir_func_name = "set_log_files_dir";
const char* const c_set_stream_socket_func_name = "set_stream_socket";
const char* const c_init_ciyam_session_func_name = "init_ciyam_session";
const char* const c_set_files_area_dir_func_name = "set_files_area_dir";
const char* const c_set_test_peer_port_func_name = "set_test_peer_port";
const char* const c_init_peer_sessions_func_name = "init_peer_sessions";
const char* const c_check_timezone_info_func_name = "check_timezone_info";
const char* const c_is_accepted_ip_addr_func_name = "is_accepted_ip_addr";
const char* const c_unregister_listener_func_name = "unregister_listener";

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

class ciyam_server_command_handler : public console_command_handler
{
   public:
   ciyam_server_command_handler( )
   {
      set_custom_startup_options( 1, "[<port>]" );
   }

   private:
   void process_custom_startup_option( size_t num, const string& option );
};

void ciyam_server_command_handler::process_custom_startup_option( size_t num, const string& option )
{
   if( num == 0 )
   {
      if( !option.empty( ) && ( option[ 0 ] >= '0' ) && ( option[ 0 ] <= '9' ) )
         g_port = from_string< unsigned int >( option );
      else
         throw runtime_error( "unexpected startup option '" + option + "'" );
   }
   else
      throw runtime_error( "unexpected startup option '" + option + "'" );
}

class ciyam_server_startup_functor : public command_functor
{
   public:
   ciyam_server_startup_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters )
   {
      if( command == c_cmd_sid )
      {
         string entropy( get_parm_val( parameters, c_cmd_sid_entropy ) );

         g_entropy = entropy;
      }
      else if( command == c_cmd_chdir )
      {
         string directory( get_parm_val( parameters, c_cmd_chdir_directory ) );

         set_cwd( directory );
      }
      else if( command == c_cmd_quiet )
         g_is_quiet = true;
      else if( command == c_cmd_trace )
      {
         string flags( get_parm_val( parameters, c_cmd_trace_flags ) );

         g_set_trace_flags = true;

         istringstream isstr( flags );

         isstr >> hex >> g_trace_flags;
      }
      else if( command == c_cmd_log_dir )
      {
         string directory( get_parm_val( parameters, c_cmd_log_dir_directory ) );

         g_log_files_directory = directory;
      }
      else if( command == c_cmd_no_auto )
         g_start_autoscript = false;
      else if( command == c_cmd_no_peers )
         g_start_peer_sessions = false;
      else if( command == c_cmd_no_streams )
         g_start_udp_stream_sessions = false;
      else if( command == c_cmd_files_area_dir )
      {
         string directory( get_parm_val( parameters, c_cmd_files_area_dir_directory ) );

         g_files_area_directory = directory;
      }
      else if( command == c_cmd_test_peer_port )
      {
         string port( get_parm_val( parameters, c_cmd_test_peer_port_port ) );

         g_test_peer_port = atoi( port.c_str( ) );
      }
#ifdef __GNUG__
      else if( command == c_cmd_daemon )
         g_is_daemon = true;
#endif
   }
};

}

int main( int argc, char* argv[ ] )
{
   int rc = 0;

   try
   {
      ciyam_server_command_handler cmd_handler;

      // NOTE: Use block scope for startup command processor object.
      {
         startup_command_processor processor( cmd_handler, application_title, argc, argv );

         cmd_handler.add_command( c_cmd_sid, 0,
          "<val//entropy>", "system identity entropy", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_chdir, 1,
          "<val//directory>", "change working directory", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_quiet, 2,
          "", "use quiet operating mode", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_trace, 3,
          "<val//flags>", "hex trace flags value", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_log_dir, 4,
          "<val//directory>", "log files directory", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_no_auto, 5,
          "", "do not start the autoscript thread", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_no_peers, 5,
          "", "do not start the peer sessions thread", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_no_streams, 5,
          "", "do not start any udp stream session threads", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_files_area_dir, 6,
          "<val//directory>", "files area directory", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_test_peer_port, 7,
          "<val//port>", "port number for interactive peer testing", new ciyam_server_startup_functor( cmd_handler ) );

#ifdef __GNUG__
         cmd_handler.add_command( c_cmd_daemon, 8,
          "", "run server as a daemon", new ciyam_server_startup_functor( cmd_handler ) );
#endif

         processor.process_commands( );
      }

      if( g_had_exiting_command )
         return 0;

      umask( STANDARD_UMASK );

      string exe_path( argv[ 0 ] );

      size_t pos = exe_path.find_last_of( "/" );

      if( pos != string::npos )
         exe_path.erase( pos );
      else
         exe_path.erase( );

      if( !exe_path.empty( ) )
         set_cwd( exe_path );

      if( g_is_daemon )
      {
         pid_t pid = fork( );

         if( pid < 0 )
            exit( EXIT_FAILURE );

         if( pid > 0 )
            exit( EXIT_SUCCESS );

         pid_t sid = setsid( );

         if( sid < 0 )
            exit( EXIT_FAILURE );

         // FUTURE: Standard error should probably also be closed here (but
         // is useful to help diagnose issues via simple "cerr" debugging).
         close( STDOUT_FILENO );
         // close( STDERR_FILENO );
      }

      // NOTE: For a multi-threaded application need to create a thread to wait on the signal.
      pthread_t tid;

      sigemptyset( &sig_set );

      sigaddset( &sig_set, SIGINT );
      sigaddset( &sig_set, SIGABRT );
      sigaddset( &sig_set, SIGTERM );

      pthread_sigmask( SIG_BLOCK, &sig_set, 0 );

      pthread_create( &tid, 0, signal_handler, ( void* )1 );

      srand( time( 0 ) );

      string pid( to_string( get_pid( ) ) );

      set_environment_variable( "PID", pid.c_str( ) );

      if( g_port != c_default_ciyam_port )
         set_environment_variable( "PORT", to_string( g_port ) );

      string shutdown_reason( "due to interrupt" );

      bool is_update = false;

      unique_ptr< dynamic_library > up_dynamic_library;

      while( true )
      {
         g_server_shutdown = 0;

         up_dynamic_library.reset( new dynamic_library( c_ciyam_base_lib, "ciyam_base" ) );

         fp_trace_flags fp_trace_flags_func;
         fp_trace_flags_func = ( fp_trace_flags )up_dynamic_library->bind_to_function( c_trace_flags_func_name );

         fp_init_globals fp_init_globals_func;
         fp_init_globals_func = ( fp_init_globals )up_dynamic_library->bind_to_function( c_init_globals_func_name );

         fp_term_globals fp_term_globals_func;
         fp_term_globals_func = ( fp_term_globals )up_dynamic_library->bind_to_function( c_term_globals_func_name );

         fp_server_command fp_server_command_func;
         fp_server_command_func = ( fp_server_command )up_dynamic_library->bind_to_function( c_server_command_func_name );

         fp_set_server_port fp_set_server_port_func;
         fp_set_server_port_func = ( fp_set_server_port )up_dynamic_library->bind_to_function( c_set_server_port_func_name );

         fp_init_auto_script fp_init_auto_script_func;
         fp_init_auto_script_func = ( fp_init_auto_script )up_dynamic_library->bind_to_function( c_init_auto_script_func_name );

         fp_init_udp_streams fp_init_udp_streams_func;
         fp_init_udp_streams_func = ( fp_init_udp_streams )up_dynamic_library->bind_to_function( c_init_udp_streams_func_name );

         fp_log_trace_string fp_log_trace_string_func;
         fp_log_trace_string_func = ( fp_log_trace_string )up_dynamic_library->bind_to_function( c_log_trace_string_func_name );

         fp_register_listener fp_register_listener_func;
         fp_register_listener_func = ( fp_register_listener )up_dynamic_library->bind_to_function( c_register_listener_func_name );

         fp_set_log_files_dir fp_set_log_files_dir_func;
         fp_set_log_files_dir_func = ( fp_set_log_files_dir )up_dynamic_library->bind_to_function( c_set_log_files_dir_func_name );

         fp_set_stream_socket fp_set_stream_socket_func;
         fp_set_stream_socket_func = ( fp_set_stream_socket )up_dynamic_library->bind_to_function( c_set_stream_socket_func_name );

         fp_init_ciyam_session fp_init_ciyam_session_func;
         fp_init_ciyam_session_func = ( fp_init_ciyam_session )up_dynamic_library->bind_to_function( c_init_ciyam_session_func_name );

         fp_set_files_area_dir fp_set_files_area_dir_func;
         fp_set_files_area_dir_func = ( fp_set_files_area_dir )up_dynamic_library->bind_to_function( c_set_files_area_dir_func_name );

         fp_set_test_peer_port fp_set_test_peer_port_func;
         fp_set_test_peer_port_func = ( fp_set_test_peer_port )up_dynamic_library->bind_to_function( c_set_test_peer_port_func_name );

         fp_init_peer_sessions fp_init_peer_sessions_func;
         fp_init_peer_sessions_func = ( fp_init_peer_sessions )up_dynamic_library->bind_to_function( c_init_peer_sessions_func_name );

         fp_check_timezone_info fp_check_timezone_info_func;
         fp_check_timezone_info_func = ( fp_check_timezone_info )up_dynamic_library->bind_to_function( c_check_timezone_info_func_name );

         fp_is_accepted_ip_addr fp_is_accepted_ip_addr_func;
         fp_is_accepted_ip_addr_func = ( fp_is_accepted_ip_addr )up_dynamic_library->bind_to_function( c_is_accepted_ip_addr_func_name );

         fp_unregister_listener fp_unregister_listener_func;
         fp_unregister_listener_func = ( fp_unregister_listener )up_dynamic_library->bind_to_function( c_unregister_listener_func_name );

         // NOTE: If trace flags has been provided then set before calling other functions
         // (as it could be of some assistance in tracing issues with the those functions).
         if( g_set_trace_flags )
            ( *fp_trace_flags_func )( g_trace_flags );

         if( !g_log_files_directory.empty( ) )
            ( *fp_set_log_files_dir_func )( g_log_files_directory.c_str( ) );

         if( !g_files_area_directory.empty( ) )
            ( *fp_set_files_area_dir_func )( g_files_area_directory.c_str( ) );

         if( g_test_peer_port )
            ( *fp_set_test_peer_port_func )( g_test_peer_port );

         int use_udp = 0;

         ( *fp_init_globals_func )( g_entropy.empty( ) ? 0 : g_entropy.c_str( ), &use_udp );

         file_remove( c_update_signal_file );

         if( !use_udp )
            g_start_udp_stream_sessions = false;

         tcp_socket s;
         udp_socket u;

         bool okay = s.open( );

         ip_address address( g_port );

         if( okay )
         {
            ( *fp_set_server_port_func )( g_port );

            if( !s.set_reuse_addr( ) && !g_is_quiet )
               cout << "warning: set_reuse_addr failed (for tcp)..." << endl;

            ( *fp_register_listener_func )( g_port, "main", "" );

            if( !is_update )
            {
               string init_message( "server started (pid = " + pid + ")" );

               ( *fp_log_trace_string_func )( TRACE_MINIMAL, init_message.c_str( ) );
            }

            okay = s.bind( address );

            if( okay )
               okay = s.listen( );

            if( okay )
            {
               if( !is_update && !g_is_quiet )
                  cout << "server now listening on tcp port " << g_port << "..." << endl;

               string start_message( "main listener started on tcp port " + to_string( g_port ) );

               ( *fp_log_trace_string_func )( TRACE_MINIMAL, start_message.c_str( ) );

               file_remove( c_shutdown_signal_file );

               bool has_udp = false;

               if( g_start_udp_stream_sessions )
                  has_udp = u.open( );

               if( has_udp )
               {
                  if( !u.set_reuse_addr( ) && !g_is_quiet )
                     cout << "warning: set_reuse_addr failed (for udp)..." << endl;

                  has_udp = u.bind( address );

                  if( has_udp )
                  {
                     ( *fp_set_stream_socket_func )( g_port, u.get_socket( ) );

                     if( !is_update && !g_is_quiet )
                        cout << "server now available on udp port " << g_port << "..." << endl;

                     string start_message( "main streamer started on udp port " + to_string( g_port ) );

                     ( *fp_log_trace_string_func )( TRACE_MINIMAL, start_message.c_str( ) );
                  }
               }

               is_update = false;

               int expected_min_active = 0;

               if( g_start_autoscript )
               {
                  ( *fp_init_auto_script_func )( );
                  ++expected_min_active;
               }

               if( has_udp && g_start_udp_stream_sessions )
                  ( *fp_init_udp_streams_func )( g_port, u.get_socket( ), 0, 0, &expected_min_active );

               int start_wait_attempts = 0;

               while( expected_min_active )
               {
                  msleep( c_active_start_delay );

                  if( g_active_sessions == expected_min_active )
                     break;

                  if( ++start_wait_attempts > c_max_wait_attempts )
                  {
                     ++g_server_shutdown;
                     shutdown_reason = "max. wait attempts for system sessions";

                     break;
                  }
               }

               bool reported_shutdown = false;

               int min_active_sessions = g_active_sessions;

               if( g_start_peer_sessions )
                  ( *fp_init_peer_sessions_func )( true );

               while( !g_server_shutdown || g_active_sessions )
               {
                  if( !g_server_shutdown && !s )
                  {
                     ++g_server_shutdown;
                     shutdown_reason = "bad listener";
                  }

                  if( !g_server_shutdown && file_exists( c_shutdown_signal_file ) )
                  {
                     ++g_server_shutdown;
                     shutdown_reason = "due to stop file";
                  }

                  if( !is_update && !g_server_shutdown && file_exists( c_update_signal_file ) )
                  {
                     is_update = true;
                     ++g_server_shutdown;
                  }

                  if( g_server_shutdown && !reported_shutdown )
                  {
                     reported_shutdown = true;

                     if( !is_update && !g_is_quiet )
                        cout << "server shutdown (" << shutdown_reason << ") now underway..." << endl;
                  }

                  if( g_server_shutdown ) 
                  {
                     if( !g_active_sessions )
                        break;
                     else
                        msleep( c_accept_timeout );
                  }

                  // NOTE: If there are no active sessions (apart from auto-started sessions) and is not
                  // shutting down then check and update the timezone information if it has been changed.
                  if( !g_server_shutdown && ( g_active_sessions <= min_active_sessions ) )
                     ( *fp_check_timezone_info_func )( );

                  // NOTE: Check for accepts and create new sessions.
                  if( !g_server_shutdown )
                  {
#ifdef SSL_SUPPORT
                     unique_ptr< ssl_socket > up_socket( new ssl_socket( s.accept( address, c_accept_timeout ) ) );
#else
                     unique_ptr< tcp_socket > up_socket( new tcp_socket( s.accept( address, c_accept_timeout ) ) );
#endif
                     if( *up_socket && ( *fp_is_accepted_ip_addr_func )( address.get_addr_string( ).c_str( ) ) )
                        ( *fp_init_ciyam_session_func )( up_socket.release( ), address.get_addr_string( ).c_str( ) );

                     // NOTE: Support for being able to issue special commands using a file
                     // (such as dumping mutexes) if connection via a socket is not working.
                     if( file_exists( c_server_cmd_file ) )
                     {
                        string cmd( buffer_file( c_server_cmd_file ) );

                        string::size_type pos = cmd.find( '\n' );

                        if( pos != string::npos )
                           cmd.erase( pos );

                        if( !cmd.empty( ) )
                           ( *fp_server_command_func )( cmd.c_str( ) );

                        file_remove( c_server_cmd_file );
                     }
                  }
               }

               s.close( );
               u.close( );

               file_remove( c_shutdown_signal_file );

               string finish_message;

               if( has_udp )
               {
                  finish_message = "main streamer finished (udp port " + to_string( g_port ) + ")";

                  ( *fp_log_trace_string_func )( TRACE_MINIMAL, finish_message.c_str( ) );
               }

               finish_message = "main listener finished (tcp port " + to_string( g_port ) + ")";

               ( *fp_log_trace_string_func )( TRACE_MINIMAL, finish_message.c_str( ) );

               if( !is_update )
               {
                  if( !g_is_quiet )
                     cout << "server shutdown (" << shutdown_reason << ") now completed..." << endl;

                  string term_message( "server shutdown (" + shutdown_reason + ")" );
                  ( *fp_log_trace_string_func )( TRACE_MINIMAL, term_message.c_str( ) );
               }
            }
            else
            {
               rc = 1;

               s.close( );
               u.close( );

               cerr << "error: unexpected socket error" << endl;
               ( *fp_log_trace_string_func )( TRACE_MINIMAL, "error: unexpected socket error" );
            }

            ( *fp_unregister_listener_func )( g_port, "" );
         }

         ( *fp_term_globals_func )( );

         if( !is_update )
            break;

         ( *fp_log_trace_string_func )( TRACE_MINIMAL, "*** reloading ciyam_base.so library ***" );

         // NOTE: Force the dynamic library to be unloaded.
         up_dynamic_library.reset( 0 );
      }
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

   return rc;
}

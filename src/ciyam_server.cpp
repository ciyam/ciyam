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
#  include <csignal>
#  include <cstdlib>
#  include <memory>
#  include <sstream>
#  include <iomanip>
#  include <iostream>
#  include <stdexcept>
#  ifdef _WIN32
#     ifndef STRICT
#        define STRICT // Needed for "windows.h" by various Borland headers.
#     endif
#     define NOMINMAX
#     define _WINSOCKAPI_
#     include <windows.h>
#  else
#     include <pthread.h>
#  endif
#  ifdef __BORLANDC__
#     include <dir.h>
#  endif
#  ifdef _MSC_VER
#     include <direct.h>
#  endif
#  ifdef __GNUG__
#     include <unistd.h>
#     include <sys/stat.h>
#  endif
#endif

#include "config.h"
#include "sockets.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "auto_script.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#endif
#include "peer_session.h"
#include "ciyam_session.h"
#include "dynamic_library.h"
#include "console_commands.h"

using namespace std;

#ifdef _WIN32
VOID WINAPI ServiceMain( DWORD argc, LPTSTR* argv );
VOID WINAPI ServiceCtrlHandler( DWORD dwControl );

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE ServiceStatusHandle;

int main( int argc, char* argv[ ] );
#else
sigset_t sig_set;
#endif

size_t g_active_sessions = 0;
size_t g_active_listeners = 0;

volatile sig_atomic_t g_server_shutdown = 0;

#ifdef _WIN32
LPSTR lpServiceName = "CIYAM Server";

VOID WINAPI ServiceMain( DWORD argc, LPTSTR* argv )
{
   ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
   ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
   ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
   ServiceStatus.dwWin32ExitCode = 0;
   ServiceStatus.dwServiceSpecificExitCode = 0;
   ServiceStatus.dwCheckPoint = 0;
   ServiceStatus.dwWaitHint = 0;

   ServiceStatusHandle = RegisterServiceCtrlHandler( lpServiceName, ServiceCtrlHandler );

   if( ServiceStatusHandle == ( SERVICE_STATUS_HANDLE )0 )
   {
      // FUTURE: Need to log failure.
      return;
   }

   ServiceStatus.dwCurrentState = SERVICE_RUNNING;
   ServiceStatus.dwCheckPoint = 0;
   ServiceStatus.dwWaitHint = 0;

   if( SetServiceStatus( ServiceStatusHandle, &ServiceStatus ) )
   {
      // NOTE: Change the current directory to where the executable itself is.
      char szPath[ MAX_PATH ];
      GetModuleFileName( GetModuleHandle( 0 ), szPath, MAX_PATH );

      string path( szPath );
      string::size_type pos = path.find_last_of( "\\" );
      if( pos != string::npos )
         set_cwd( path.substr( 0, pos ) );

      main( argc, argv );
   }
   else
      ; // FUTURE: Need to log failure.

   ServiceStatus.dwCurrentState = SERVICE_STOPPED;
   if( !SetServiceStatus( ServiceStatusHandle, &ServiceStatus ) )
      ; // FUTURE: Need to log failure.
}

VOID WINAPI ServiceCtrlHandler( DWORD dwControl )
{
   switch( dwControl )
   {
      case SERVICE_CONTROL_PAUSE:
      ServiceStatus.dwCurrentState = SERVICE_PAUSED;
      break;
 
      case SERVICE_CONTROL_CONTINUE:
      ServiceStatus.dwCurrentState = SERVICE_RUNNING;
      break;
 
      case SERVICE_CONTROL_STOP:
      ServiceStatus.dwWin32ExitCode = 0;
      ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
      ServiceStatus.dwCheckPoint = 0;
      ServiceStatus.dwWaitHint = 0;
      ++g_server_shutdown;
      break;

      case SERVICE_CONTROL_INTERROGATE:
      break;
   }
 
   if( !SetServiceStatus( ServiceStatusHandle, &ServiceStatus ) )
      ; // FUTURE: Need to log error.
}

SERVICE_TABLE_ENTRY DispatchTable[ ] = { { lpServiceName, ServiceMain }, { 0, 0 } };
#endif

namespace
{

bool g_start_autoscript = true;
bool g_start_peer_listeners = true;

#ifdef _WIN32
BOOL CtrlHandler( DWORD fdwCtrlType )
{ 
   switch( fdwCtrlType )
   {
      case CTRL_C_EVENT:
      case CTRL_CLOSE_EVENT:
      case CTRL_BREAK_EVENT:
      case CTRL_LOGOFF_EVENT:
      case CTRL_SHUTDOWN_EVENT:
      ++g_server_shutdown;
      return TRUE;

      default:
      return FALSE;
   }
}
#else
void* signal_handler( void* id )
{
   int sig;
   sigwait( &sig_set, &sig );

   ++g_server_shutdown;
}
#endif

const char* const c_app_title = "ciyam_server";
const char* const c_app_version = "0.1";

const char* const c_cmd_chdir = "chdir";
const char* const c_cmd_chdir_directory = "directory";

const char* const c_cmd_trace = "trace";
const char* const c_cmd_trace_flags = "flags";

const char* const c_cmd_quiet = "quiet";

const char* const c_cmd_no_auto = "no_auto";
const char* const c_cmd_no_peers = "no_peers";

#ifdef _WIN32
const char* const c_cmd_svcins = "svcins";
const char* const c_cmd_svcrem = "svcrem";
const char* const c_cmd_svcrun = "svcrun";
#endif

bool g_is_quiet = false;
bool g_had_exiting_command = false;

#ifdef __GNUG__
const char* const c_cmd_daemon = "daemon";

bool g_is_daemon = false;
#endif

int g_port = c_default_ciyam_port;

int g_flags = 0;
bool g_has_flags = false;

const int c_accept_timeout = 250;

const char* const c_update_signal_file = "ciyam_base.update";
const char* const c_shutdown_signal_file = "ciyam_server.stop";

#ifndef _WIN32
const char* const c_ciyam_base_lib = "ciyam_base.so";
#else
const char* const c_ciyam_base_lib = "ciyam_base.dll";
#endif

#ifndef __BORLANDC__
const char* const c_trace_flags_func_name = "trace_flags";
const char* const c_init_globals_func_name = "init_globals";
const char* const c_term_globals_func_name = "term_globals";
const char* const c_init_auto_script_func_name = "init_auto_script";
const char* const c_log_trace_string_func_name = "log_trace_string";
const char* const c_register_listener_func_name = "register_listener";
const char* const c_init_ciyam_session_func_name = "init_ciyam_session";
const char* const c_init_peer_sessions_func_name = "init_peer_sessions";
const char* const c_check_timezone_info_func_name = "check_timezone_info";
const char* const c_is_accepted_ip_addr_func_name = "is_accepted_ip_addr";
const char* const c_unregister_listener_func_name = "unregister_listener";
#else
const char* const c_trace_flags_func_name = "_trace_flags";
const char* const c_init_globals_func_name = "_init_globals";
const char* const c_term_globals_func_name = "_term_globals";
const char* const c_init_auto_script_func_name = "_init_auto_script";
const char* const c_log_trace_string_func_name = "_log_trace_string";
const char* const c_register_listener_func_name = "_register_listener";
const char* const c_init_ciyam_session_func_name = "_init_ciyam_session";
const char* const c_init_peer_sessions_func_name = "_init_peer_sessions";
const char* const c_check_timezone_info_func_name = "_check_timezone_info";
const char* const c_is_accepted_ip_addr_func_name = "_is_accepted_ip_addr";
const char* const c_unregister_listener_func_name = "_unregister_listener";
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
      if( !option.empty( ) && option[ 0 ] >= '0' && option[ 0 ] <= '9' )
         g_port = atoi( option.c_str( ) );
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
      if( command == c_cmd_chdir )
      {
         string directory( get_parm_val( parameters, c_cmd_chdir_directory ) );

         set_cwd( directory );
      }
      else if( command == c_cmd_trace )
      {
         string flags( get_parm_val( parameters, c_cmd_trace_flags ) );

         istringstream isstr( flags );
         isstr >> hex >> g_flags;

         g_has_flags = true;
      }
      else if( command == c_cmd_quiet )
         g_is_quiet = true;
      else if( command == c_cmd_no_auto )
         g_start_autoscript = false;
      else if( command == c_cmd_no_peers )
         g_start_peer_listeners = false;
#ifdef _WIN32
      else if( command == c_cmd_svcins )
      {
         char szPath[ MAX_PATH ];
         g_had_exiting_command = true;

         SC_HANDLE scmHandle = OpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
         if( scmHandle )
         {
            GetModuleFileName( GetModuleHandle( 0 ), szPath, MAX_PATH );

            string binary_path( string( szPath ) + " -svcrun" );

            SC_HANDLE scHandle = CreateService( scmHandle, lpServiceName, lpServiceName, SERVICE_ALL_ACCESS,
             SERVICE_WIN32_OWN_PROCESS, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, binary_path.c_str( ), 0, 0, 0, 0, 0 );

            if( !scHandle )
               cerr << "error: unable to install service (already installed?)" << endl;
            else
               cout << "'" << lpServiceName << "' service has now been installed" << endl;

            CloseServiceHandle( scHandle );
            CloseServiceHandle( scmHandle );
         }
         else
            cerr << "error: unable to access service control manager" << endl;
      }
      else if( command == c_cmd_svcrem )
      {
         g_had_exiting_command = true;

         SC_HANDLE scmHandle = OpenSCManager( 0, 0, SC_MANAGER_ALL_ACCESS );
         if( scmHandle )
         {
            SC_HANDLE scHandle = OpenService( scmHandle, lpServiceName, SERVICE_ALL_ACCESS );
            if( scHandle )
            {
               DeleteService( scHandle );
               if( !g_is_quiet )
                  cout << "'" << lpServiceName << "' service has now been removed" << endl;
            }
            else
               cerr << "error: unable to remove service (not present?)" << endl;
         }
         else
            cerr << "error: unable to access service control manager" << endl;
      }
      else if( command == c_cmd_svcrun )
      {
         if( !StartServiceCtrlDispatcher( DispatchTable ) )
            cerr << "error: unable to start service" << endl; // FUTURE: This message should be logged.

         g_had_exiting_command = true;
      }
#endif
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
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, argc, argv );

         cmd_handler.add_command( c_cmd_chdir, 0,
          "<val//directory>", "change directory", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_trace, 0,
          "<val//flags>", "set server trace flags", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_quiet, 1,
          "", "switch on quiet operating mode", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_no_auto, 1,
          "", "don't start the autoscript thread", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_no_peers, 1,
          "", "don't start any peer listener threads", new ciyam_server_startup_functor( cmd_handler ) );

#ifdef _WIN32
         cmd_handler.add_command( c_cmd_svcins, 2,
          "", "install Win32 service", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_svcrem, 2,
          "", "remove Win32 service", new ciyam_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_svcrun, 2,
          "", "start as Win32 service", new ciyam_server_startup_functor( cmd_handler ) );
#endif

#ifdef __GNUG__
         cmd_handler.add_command( c_cmd_daemon, 2,
          "", "run server as a daemon", new ciyam_server_startup_functor( cmd_handler ) );
#endif

         processor.process_commands( );
      }

      if( g_had_exiting_command )
         return 0;

#ifdef _WIN32
      winsock_init wsi;
      if( !SetConsoleCtrlHandler( ( PHANDLER_ROUTINE )CtrlHandler, TRUE ) )
         throw runtime_error( "unable to register CtrlHandler" );
#else
      umask( STANDARD_UMASK );

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

         // FUTURE: Standard out should probably also be closed here (but can be useful
         // to help diagnose problems by allowing for simple "cout" style debugging).
         close( STDOUT_FILENO );
         // close( STDERR_FILENO );
      }

      // NOTE: For a non-windows multi-threaded application need to create a thread to wait on the signal.
      pthread_t tid;

      sigemptyset( &sig_set );
      sigaddset( &sig_set, SIGINT );
      sigaddset( &sig_set, SIGABRT );
      sigaddset( &sig_set, SIGTERM );
      pthread_sigmask( SIG_BLOCK, &sig_set, 0 );

      pthread_create( &tid, 0, signal_handler, ( void* )1 );
#endif

      srand( time( 0 ) );

      string pid( to_string( get_pid( ) ) );
      set_environment_variable( "PID", pid.c_str( ) );

      string shutdown_reason( "due to interrupt" );

      bool is_update = false;
      auto_ptr< dynamic_library > ap_dynamic_library;

      while( true )
      {
         ap_dynamic_library.reset( new dynamic_library( c_ciyam_base_lib, "ciyam_base" ) );

         file_remove( c_update_signal_file );

         fp_trace_flags fp_trace_flags_func;
         fp_trace_flags_func = ( fp_trace_flags )ap_dynamic_library->bind_to_function( c_trace_flags_func_name );

         fp_init_globals fp_init_globals_func;
         fp_init_globals_func = ( fp_init_globals )ap_dynamic_library->bind_to_function( c_init_globals_func_name );

         fp_term_globals fp_term_globals_func;
         fp_term_globals_func = ( fp_term_globals )ap_dynamic_library->bind_to_function( c_term_globals_func_name );

         fp_init_auto_script fp_init_auto_script_func;
         fp_init_auto_script_func = ( fp_init_auto_script )ap_dynamic_library->bind_to_function( c_init_auto_script_func_name );

         fp_log_trace_string fp_log_trace_string_func;
         fp_log_trace_string_func = ( fp_log_trace_string )ap_dynamic_library->bind_to_function( c_log_trace_string_func_name );

         fp_register_listener fp_register_listener_func;
         fp_register_listener_func = ( fp_register_listener )ap_dynamic_library->bind_to_function( c_register_listener_func_name );

         fp_init_ciyam_session fp_init_ciyam_session_func;
         fp_init_ciyam_session_func = ( fp_init_ciyam_session )ap_dynamic_library->bind_to_function( c_init_ciyam_session_func_name );

         fp_init_peer_sessions fp_init_peer_sessions_func;
         fp_init_peer_sessions_func = ( fp_init_peer_sessions )ap_dynamic_library->bind_to_function( c_init_peer_sessions_func_name );

         fp_check_timezone_info fp_check_timezone_info_func;
         fp_check_timezone_info_func = ( fp_check_timezone_info )ap_dynamic_library->bind_to_function( c_check_timezone_info_func_name );

         fp_is_accepted_ip_addr fp_is_accepted_ip_addr_func;
         fp_is_accepted_ip_addr_func = ( fp_is_accepted_ip_addr )ap_dynamic_library->bind_to_function( c_is_accepted_ip_addr_func_name );

         fp_unregister_listener fp_unregister_listener_func;
         fp_unregister_listener_func = ( fp_unregister_listener )ap_dynamic_library->bind_to_function( c_unregister_listener_func_name );

         // NOTE: If trace flags are provided then set them before calling "init_globals"
         // as they may be of some assistance in tracing an issue with the init function.
         if( g_has_flags )
            ( *fp_trace_flags_func )( g_flags );

         ( *fp_init_globals_func )( );

         tcp_socket s;
         bool okay = s.open( );

         ip_address address( g_port );

         if( okay )
         {
            if( !s.set_reuse_addr( ) && !g_is_quiet )
               cout << "warning: set_reuse_addr failed..." << endl;

            ( *fp_register_listener_func )( g_port, "main" );

            if( !is_update )
            {
               string init_message( "server started (pid = " + pid + ")" );

               ( *fp_log_trace_string_func )( TRACE_ANYTHING, init_message.c_str( ) );
            }

            okay = s.bind( address );

            if( okay )
               okay = s.listen( );

            if( okay )
            {
               if( !g_is_quiet )
                  cout << "server now listening on port " << g_port << "..." << endl;

               string start_message( "main listener started on port " + to_string( g_port ) );

               ( *fp_log_trace_string_func )( TRACE_ANYTHING, start_message.c_str( ) );

               is_update = false;

               file_remove( c_shutdown_signal_file );

               if( g_start_autoscript )
                  ( *fp_init_auto_script_func )( );

               ( *fp_init_peer_sessions_func )( g_start_peer_listeners );

               bool reported_shutdown = false;
               while( s && ( !g_server_shutdown || g_active_sessions ) )
               {
                  if( !g_server_shutdown && file_exists( c_shutdown_signal_file ) )
                  {
                     ++g_server_shutdown;
                     shutdown_reason = "due to stop file";
                  }

                  if( g_server_shutdown && !reported_shutdown )
                  {
                     reported_shutdown = true;

                     if( !g_is_quiet )
                        cout << "server shutdown (due to interrupt) now underway..." << endl;
                  }

                  if( !is_update && !g_server_shutdown && file_exists( c_update_signal_file ) )
                     is_update = true;

                  if( is_update && !g_server_shutdown
                   && ( !g_active_sessions || ( g_start_autoscript && g_active_sessions == 1 ) ) )
                     break;

                  // NOTE: If there are no active sessions (apart from the autoscript session) and is not
                  // shutting down then check and update the timezone information if it has been changed.
                  if( !g_server_shutdown
                   && ( !g_active_sessions || ( g_start_autoscript && g_active_sessions == 1 ) ) )
                     ( *fp_check_timezone_info_func )( );

                  // NOTE: Check for accepts and create new sessions.
#ifdef SSL_SUPPORT
                  auto_ptr< ssl_socket > ap_socket( new ssl_socket( s.accept( address, c_accept_timeout ) ) );
#else
                  auto_ptr< tcp_socket > ap_socket( new tcp_socket( s.accept( address, c_accept_timeout ) ) );
#endif
                  if( *ap_socket && ( *fp_is_accepted_ip_addr_func )( address.get_addr_string( ).c_str( ) ) )
                     ( *fp_init_ciyam_session_func )( ap_socket.release( ), address.get_addr_string( ).c_str( ) );
               }

               s.close( );
               file_remove( c_shutdown_signal_file );

               if( is_update && !g_server_shutdown )
               {
                  g_server_shutdown = 1;

                  // NOTE: Wait for all active sessions and peer listeners to finish up before continuing.
                  while( g_active_sessions || g_active_listeners )
                     msleep( c_accept_timeout * 2 );

                  g_server_shutdown = 0;
               }

               string finish_message( "main listener finished (port " + to_string( g_port ) + ")" );

               ( *fp_log_trace_string_func )( TRACE_ANYTHING, finish_message.c_str( ) );

               if( !is_update )
               {
                  if( !g_is_quiet )
                     cout << "server shutdown (" << shutdown_reason << ") now completed..." << endl;

                  string term_message( "server shutdown (" + shutdown_reason + ")" );
                  ( *fp_log_trace_string_func )( TRACE_ANYTHING, term_message.c_str( ) );
               }
            }
            else
            {
               rc = 1;
               s.close( );

               cerr << "error: unexpected socket error" << endl;
               ( *fp_log_trace_string_func )( TRACE_ANYTHING, "error: unexpected socket error" );
            }

            ( *fp_unregister_listener_func )( g_port );
         }

         ( *fp_term_globals_func )( );

         if( !is_update )
            break;

         ( *fp_log_trace_string_func )( TRACE_ANYTHING, "*** reloading ciyam_base library ***" );

         // NOTE: Force the dynamic library to be unloaded.
         ap_dynamic_library.reset( 0 );
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

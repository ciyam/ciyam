// Copyright (c) 2006
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifdef __BORLANDC__
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

#ifdef __GNUG__
#  define _chdir chdir
#endif
#ifdef __BORLANDC__
#  define _chdir chdir
#endif

#include "mac.h"
#include "sha1.h"
#include "sql_db.h"
#include "sockets.h"
#include "cat_base.h"
#include "utilities.h"
#include "auto_script.h"
#include "cat_session.h"
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

size_t g_active_sessions;
volatile sig_atomic_t g_server_shutdown;

#ifdef _WIN32
LPSTR lpServiceName = "Cat Server";

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
         _chdir( path.substr( 0, pos ).c_str( ) );

      main( argc, argv );
   }
   else
      ;// FUTURE: Need to log failure.

   ServiceStatus.dwCurrentState = SERVICE_STOPPED;
   if( !SetServiceStatus( ServiceStatusHandle, &ServiceStatus ) )
      ;// FUTURE: Need to log failure.
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
 
    // Send current status. 
    if( !SetServiceStatus( ServiceStatusHandle, &ServiceStatus ) )
      ;// FUTURE: Need to log error.
}

SERVICE_TABLE_ENTRY DispatchTable[ ] = { { lpServiceName, ServiceMain }, { 0, 0 } };
#endif

namespace
{

bool start_autoscript = true;

const int c_default_cat_port = 11011;

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

const char* const c_app_title = "cat_server";
const char* const c_app_version = "0.1";

const char* const c_cmd_chdir = "chdir";
const char* const c_cmd_parm_chdir_directory = "directory";

const char* const c_cmd_trace = "trace";
const char* const c_cmd_parm_trace_flags = "flags";

const char* const c_cmd_no_auto = "no_auto";

const char* const c_cmd_register = "register";

#ifdef _WIN32
const char* const c_cmd_svcins = "svcins";
const char* const c_cmd_svcrem = "svcrem";
const char* const c_cmd_svcrun = "svcrun";
#endif

bool g_had_exiting_command = false;

#ifdef __GNUG__
const char* const c_cmd_daemon = "daemon";

bool g_is_daemon = false;
#endif

int g_port = c_default_cat_port;

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

class cat_server_command_handler : public console_command_handler
{
   public:
   cat_server_command_handler( )
   {
      set_custom_startup_options( 1, "[<port>]" );
   }

   private:
   void process_custom_startup_option( size_t num, const string& option );
};

void cat_server_command_handler::process_custom_startup_option( size_t num, const string& option )
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

class cat_server_startup_functor : public command_functor
{
   public:
   cat_server_startup_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters )
   {
      if( command == c_cmd_chdir )
      {
         string directory( get_parm_val( parameters, c_cmd_parm_chdir_directory ) );

         _chdir( directory.c_str( ) );
      }
      else if( command == c_cmd_no_auto )
      {
         start_autoscript = false;
      }
      else if( command == c_cmd_trace )
      {
         string flags( get_parm_val( parameters, c_cmd_parm_trace_flags ) );

         int trace_flags;
         istringstream isstr( flags );
         isstr >> hex >> trace_flags;

         set_trace_flags( trace_flags );
      }
      else if( command == c_cmd_register )
      {
         g_had_exiting_command = true;

         string mac_addr( get_mac_addr( ) );

         sha1 hash( get_license_salt( ) + mac_addr );
         vector< string > sha1_quads;
         split( hash.get_digest_as_string( ',' ), sha1_quads );

         if( sha1_quads.size( ) != 5 )
            throw runtime_error( "unexpected hash result" );

         cout << "Registration Key: " << upper( sha1_quads[ 2 ] ) << endl;
      }
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
      cat_server_command_handler cmd_handler;
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, argc, argv );

         cmd_handler.add_command( c_cmd_chdir, 0,
          "<val//directory>", "change directory", new cat_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_trace, 0,
          "<val//flags>", "set server trace flags", new cat_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_no_auto, 0,
          "", "switch off autoscript support", new cat_server_startup_functor( cmd_handler ) );

#ifdef _WIN32
         cmd_handler.add_command( c_cmd_svcins, 1,
          "", "install Win32 service", new cat_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_svcrem, 1,
          "", "remove Win32 service", new cat_server_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_svcrun, 1,
          "", "start as Win32 service", new cat_server_startup_functor( cmd_handler ) );
#endif

#ifdef __GNUG__
         cmd_handler.add_command( c_cmd_daemon, 1,
          "", "run server as a daemon", new cat_server_startup_functor( cmd_handler ) );
#endif

         cmd_handler.add_command( c_cmd_register, 2,
          "", "get registration key", new cat_server_startup_functor( cmd_handler ) );

         processor.process_commands( );
      }

      if( g_had_exiting_command )
         return 0;

#ifdef _WIN32
      winsock_init wsi;
      if( !SetConsoleCtrlHandler( ( PHANDLER_ROUTINE )CtrlHandler, TRUE ) )
         throw runtime_error( "unable to register CtrlHandler" );
#else
      if( g_is_daemon )
      {
         pid_t pid = fork( );
         if( pid < 0 )
            exit( EXIT_FAILURE );
         if( pid > 0 )
            exit( EXIT_SUCCESS );

         umask( 0 );

         // FUTURE: Log file(s) should be opened here and a chdir to "/" is
         // generally advised (as the current directory could be anything).

         pid_t sid = setsid( );
         if( sid < 0 )
            exit( EXIT_FAILURE );

         // FUTURE: Standard error should probably also be closed here (but can be useful for
         // now to help diagnose problems).
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

      init_globals( );

      // NOTE: Make sure that server has the correct license key.
      string mac_addr( get_mac_addr( ) );

      sha1 hash( get_license_salt( ) + mac_addr );
      vector< string > sha1_quads;
      split( hash.get_digest_as_string( ',' ), sha1_quads );

      if( sha1_quads.size( ) != 5 )
         throw runtime_error( "unexpected hash result" );

      string license_hash( sha1_quads[ 2 ] );

      bool is_licensed = false;
      for( int i = 0; i < 100; i++ )
      {
         hash.init( );
         hash.update( upper( license_hash ) + to_string( i ) );

         sha1_quads.clear( );
         split( hash.get_digest_as_string( ',' ), sha1_quads );

         if( sha1_quads.size( ) != 5 )
            throw runtime_error( "unexpected hash result" );

         string license( get_license( ) );

         set< string > all_licenses;
         split( license, all_licenses, '+' );

         if( all_licenses.count( upper( sha1_quads[ 2 ] ) ) )
         {
            set_max_user_limit( i );
            is_licensed = true;
            break;
         }
      }

      if( !is_licensed )
         throw runtime_error( "server is not licensed" );

      tcp_socket s;
      bool okay = s.open( );

      ip_address address( g_port );
      if( okay )
      {
         if( !s.set_reuse_addr( ) )
            cout << "warning: set_reuse_addr failed..." << endl;

         okay = s.bind( address );
         if( okay )
         {
            s.listen( );

            cout << "server now listening on port " << g_port << "..." << endl;
            TRACE_LOG( TRACE_ANYTHING,
             "server started on port " + to_string( g_port ) + " (pid = " + to_string( get_pid( ) ) + ")" );

            if( start_autoscript )
            {
               autoscript_session* p_autoscript_session = new autoscript_session;
               p_autoscript_session->start( );
            }

            bool reported_shutdown = false;
            while( s && ( !g_server_shutdown || g_active_sessions ) )
            {
               if( g_server_shutdown && !reported_shutdown )
               {
                  reported_shutdown = true;
                  cout << "server shutdown (due to interrupt) now underway..." << endl;
               }

               // NOTE: Check for accepts (timeout after 250ms if none occur) and create sessions.
               auto_ptr< tcp_socket > ap_socket( new tcp_socket( s.accept( address, 250 ) ) );
               if( !g_server_shutdown && *ap_socket )
               {
                  cat_session* p_session = new cat_session( ap_socket );
                  p_session->start( );
               }
            }

            cout << "server shutdown (due to interrupt) now completed..." << endl;
            TRACE_LOG( TRACE_ANYTHING, "server shutdown (due to interrupt)" );
         }
         else
         {
            cerr << "error: unexpected socket error" << endl;
            TRACE_LOG( TRACE_ANYTHING, "error: unexpected socket error" );

            rc = 1;
            s.close( );
         }
      }

      term_globals( );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      TRACE_LOG( TRACE_ANYTHING, "error: " + string( x.what( ) ) );

      rc = 2;
   }
   catch( ... )
   {
      cerr << "error: unexpected unknown exception caught" << endl;
      TRACE_LOG( TRACE_ANYTHING, "error: unexpected unknown exception caught" );

      rc = 3;
   }

   return rc;
}


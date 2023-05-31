// Copyright (c) 2023 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <csignal>
#  include <stdexcept>
#  include <vector>
#  include <sstream>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_notifier.h"

#include "notifier.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "ciyam_variables.h"

using namespace std;

extern size_t g_active_listeners;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

#include "ciyam_constants.h"

trace_mutex g_mutex;

const size_t c_wait_sleep_time = 25;

inline void issue_error( const string& message, bool possibly_expected = false )
{
   TRACE_LOG( ( possibly_expected ? TRACE_SESSIONS : TRACE_ANYTHING ), string( "notifier error: " ) + message );
}

inline void issue_warning( const string& message )
{
   TRACE_LOG( TRACE_SESSIONS, string( "notifier warning: " ) + message );
}

void increment_active_listeners( )
{
   guard g( g_mutex );

   ++g_active_listeners;
}

void decrement_active_listeners( )
{
   guard g( g_mutex );

   --g_active_listeners;
}

}

ciyam_notifier::ciyam_notifier( const string& watch_root )
 :
 watch_root( watch_root )
{
}

ciyam_notifier::~ciyam_notifier( )
{
}

void ciyam_notifier::on_start( )
{
   increment_active_listeners( );

   try
   {
      bool recurse = dir_exists( watch_root );

      notifier n( watch_root, recurse );

      TRACE_LOG( TRACE_SESSIONS, "notifier started for '" + watch_root + "'" );

      string watch_var_name( watch_root );

      if( recurse )
         watch_var_name += '/';

      set_system_variable( watch_var_name, c_watch_str );

      while( true )
      {
         if( g_server_shutdown )
         {
            issue_warning( "application server is being shutdown" );
            break;
         }

         if( n.has_new_events( ) )
         {
            stringstream ss;
            n.output_events( ss );

            string events( ss.str( ) );

            vector< string > all_events;

            split( events, all_events, '\n' );

            for( size_t i = 0; i < all_events.size( ); i++ )
            {
               string next_event( all_events[ i ] );

               if( !next_event.empty( ) )
               {
                  string::size_type pos = next_event.find( '|' );

                  string value;
                  string var_name( next_event.substr( 0, pos ) );

                  if( pos )
                  {
                     next_event.erase( 0, pos + 1 );
                     pos = next_event.find( '|' );

                     value = next_event.substr( 0, pos );
                  }

                  string old_value( get_raw_system_variable( var_name ) );

                  bool skip = false;

                  if( old_value == "create" )
                  {
                     if( value == "attrib" )
                        skip = true;

                     if( value == "delete" )
                        value = "";
                  }

                  if( value == "delete_self" )
                     value.erase( );

                  if( !skip )
                     set_system_variable( var_name, value );
               }
            }
         }
         else
         {
            msleep( c_wait_sleep_time );

            if( get_raw_system_variable( watch_var_name ).empty( ) )
            {
               if( recurse )
                  set_system_variable( watch_var_name + '*', "" );

               break;
            }
         }
      }
   }
   catch( exception& x )
   {
      issue_error( x.what( ), true );
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );
   }

   decrement_active_listeners( );

   TRACE_LOG( TRACE_SESSIONS, "notifier finished for '" + watch_root + "'" );

   delete this;
}

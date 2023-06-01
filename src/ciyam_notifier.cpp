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

const size_t c_wait_sleep_time = 10;

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

   bool recurse = false;
   bool has_set_system_variable = false;

   string watch_var_name( watch_root );

   try
   {
      recurse = dir_exists( watch_root );

      notifier n( watch_root, recurse );

      if( recurse )
         watch_var_name += '/';

      set_system_variable( watch_var_name, c_watching );

      has_set_system_variable = true;

      TRACE_LOG( TRACE_SESSIONS, "notifier started for '" + watch_root + "'" );

      string renamed_from_prefix( "renamed_from|" );

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

            map< string, string > cookie_names;

            for( size_t i = 0; i < all_events.size( ); i++ )
            {
               string value, old_value;

               string next_event( all_events[ i ] );

               if( !next_event.empty( ) )
               {
                  string::size_type pos = next_event.find( '|' );

                  string var_name( next_event.substr( 0, pos ) );

                  old_value = get_raw_system_variable( var_name );

                  if( pos != string::npos )
                  {
                     next_event.erase( 0, pos + 1 );
                     pos = next_event.find( '|' );

                     value = next_event.substr( 0, pos );

                     if( pos != string::npos )
                     {
                        next_event.erase( 0, pos + 1 );

                        if( next_event != "0" )
                        {
                           if( value == "moved_to" )
                              value = "renamed_from";
                           else if( value == "moved_from" )
                              value = "renamed_to";

                           if( cookie_names.find( next_event ) == cookie_names.end( ) )
                           {
                              if( old_value == "create" )
                              {
                                 value.erase( );
                                 cookie_names.insert( make_pair( next_event, "" ) );
                              }
                              else
                              {
                                 if( old_value.find( renamed_from_prefix ) == 0 )
                                 {
                                    old_value.erase( 0, renamed_from_prefix.length( ) );
                                    cookie_names.insert( make_pair( next_event, old_value ) );
                                    
                                    value.erase( );
                                    old_value.erase( );
                                 }
                                 else
                                    cookie_names.insert( make_pair( next_event, var_name ) );
                              }
                           }
                           else
                           {
                              if( cookie_names[ next_event ].empty( ) )
                                 value = "create";
                              else
                              {
                                 if( cookie_names[ next_event ] == var_name )
                                    value = "none";
                                 else
                                    value += '|' + cookie_names[ next_event ];

                                 string current_value( get_system_variable( cookie_names[ next_event ] ) );

                                 set_system_variable( cookie_names[ next_event ],
                                  current_value.substr( 0, current_value.find( '|' ) ) + '|' + var_name );
                              }
                           }
                        }
                     }
                  }

                  bool skip = false;

                  if( old_value == "create" )
                  {
                     if( value == "attrib" )
                        skip = true;

                     if( value == "delete" )
                        value = "";
                  }
                  else if( value == "attrib" )
                  {
                     if( old_value.find( renamed_from_prefix ) == 0 )
                        value = old_value;
                  }
                  else if( value == "create" )
                  {
                     if( old_value == "delete" )
                        value = "none";
                  }
                  else if( value == "delete" )
                  {
                     if( old_value.find( renamed_from_prefix ) == 0 )
                     {
                        old_value.erase( 0, renamed_from_prefix.length( ) );

                        set_system_variable( old_value, "delete" );

                        value.erase( );
                        old_value.erase( );
                     }
                  }

                  if( value == "delete_self" )
                  {
                     if( var_name != watch_var_name )
                        skip = true;
                     else
                        value = string( c_finishing );
                  }

                  if( !skip )
                     set_system_variable( var_name, value );
               }
            }
         }
         else
         {
            msleep( c_wait_sleep_time );

            if( get_raw_system_variable( watch_var_name ) == c_finishing )
               break;
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

   if( has_set_system_variable )
   {
      if( !recurse )
         set_system_variable( watch_var_name, "" );
      else
         set_system_variable( watch_var_name + '*', "" );
   }

   decrement_active_listeners( );

   TRACE_LOG( TRACE_SESSIONS, "notifier finished for '" + watch_root + "'" );

   delete this;
}

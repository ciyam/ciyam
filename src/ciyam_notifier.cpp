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
#include "fs_iterator.h"
#include "ciyam_variables.h"

using namespace std;

extern size_t g_active_listeners;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

#include "ciyam_constants.h"

const char* const c_notifier_none = "none";
const char* const c_notifier_created = "created";
const char* const c_notifier_deleted = "deleted";
const char* const c_notifier_modified = "modified";
const char* const c_notifier_moved_to = "moved_to";
const char* const c_notifier_moved_from = "moved_from";

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

   string watch_root_name( watch_root );
   string watch_variable_name( c_notifier_prefix + watch_root );

   try
   {
      recurse = dir_exists( watch_root );

      notifier n( watch_root, recurse );

      if( !recurse )
         set_system_variable( watch_root, c_notifier_none );
      else
      {
         watch_root_name += '/';
         watch_variable_name += '/';

         // NOTE: Add watches for all child folders and also
         // set system variables for child files and folders.
         directory_filter df;
         fs_iterator dfsi( watch_root, &df );

         string path_to_dir( dfsi.get_path_name( ) );

         string::size_type pos = path_to_dir.rfind( watch_root );

         if( pos == string::npos )
            throw runtime_error( "unexpected '" + watch_root + "' not found in '" + path_to_dir + "'" );

         path_to_dir.erase( pos );

         do
         {
            string next_dir( dfsi.get_path_name( ) );

            file_filter ff;
            fs_iterator fs( next_dir, &ff );

            pos = next_dir.find( path_to_dir );

            if( pos != 0 )
               throw runtime_error( "unexpected '" + next_dir + "' does start with '" + path_to_dir + "'" );

            next_dir.erase( 0, path_to_dir.length( ) );

            if( next_dir != watch_root )
               n.add_watch( next_dir );

            set_system_variable( next_dir + '/', c_notifier_none );

            while( fs.has_next( ) )
            {
               string next_file( fs.get_full_name( ) );

               pos = next_file.find( path_to_dir );

               if( pos != 0 )
                  throw runtime_error( "unexpected '" + next_file + "' does start with '" + path_to_dir + "'" );

               next_file.erase( 0, path_to_dir.length( ) );

               set_system_variable( next_file, c_notifier_none );
            }
         } while( dfsi.has_next( ) );
      }

      set_system_variable( watch_variable_name, c_watching );

      has_set_system_variable = true;

      TRACE_LOG( TRACE_SESSIONS, "notifier started for '" + watch_root + "'" );

      string moved_from_prefix( string( c_notifier_moved_from ) + '|' );

      while( true )
      {
         if( g_server_shutdown )
         {
            issue_warning( "application server is being shutdown" );
            break;
         }

         // NOTE: Supports event testing via a special system variable.
         string events( get_system_variable(
          get_special_var_name( e_special_var_notifier_events ) ) );

         if( !events.empty( ) || n.has_new_events( ) )
         {
            if( events.empty( ) )
            {
               stringstream ss;
               n.output_events( ss );

               events = ss.str( );
            }
            else
               set_system_variable( get_special_var_name( e_special_var_notifier_events ), "" );

            vector< string > all_events;

            split( events, all_events, '\n' );

            map< string, string > cookie_id_names;

            for( size_t i = 0; i < all_events.size( ); i++ )
            {
               string value, old_value;

               string next_event( all_events[ i ] );

               bool reportable_event = true;

               if( !next_event.empty( ) )
               {
                  TRACE_LOG( TRACE_NOTIFIER, "event: " + next_event );

                  if( next_event[ 0 ] == '-' )
                     reportable_event = false;

                  next_event.erase( 0, 1 );
               }

               if( !next_event.empty( ) && reportable_event )
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
                        string cookie_id( next_event.substr( pos + 1 ) );
                        next_event.erase( 0, pos + 1 );

                        if( cookie_id != "0" )
                        {
                           if( value == "moved_to" )
                              value = c_notifier_moved_from;
                           else if( value == "moved_from" )
                              value = c_notifier_moved_to;

                           if( cookie_id_names.find( cookie_id ) == cookie_id_names.end( ) )
                           {
                              if( old_value == c_notifier_created )
                              {
                                 value.erase( );
                                 cookie_id_names.insert( make_pair( cookie_id, "" ) );
                              }
                              else
                              {
                                 if( old_value.find( moved_from_prefix ) == 0 )
                                 {
                                    old_value.erase( 0, moved_from_prefix.length( ) );
                                    cookie_id_names.insert( make_pair( cookie_id, old_value ) );
                                    
                                    value.erase( );
                                    old_value.erase( );
                                 }
                                 else if( value == c_notifier_moved_from )
                                    value = c_notifier_created;
                                 else
                                    cookie_id_names.insert( make_pair( cookie_id, var_name ) );
                              }
                           }
                           else
                           {
                              if( cookie_id_names[ cookie_id ].empty( ) )
                              {
                                 if( old_value.empty( ) )
                                    value = c_notifier_created;
                                 else
                                    value = c_notifier_modified;
                              }
                              else
                              {
                                 if( cookie_id_names[ cookie_id ] == var_name )
                                    value = c_notifier_modified;
                                 else
                                    value += '|' + cookie_id_names[ cookie_id ];

                                 string current_value( get_system_variable( cookie_id_names[ cookie_id ] ) );

                                 set_system_variable( cookie_id_names[ cookie_id ],
                                  current_value.substr( 0, current_value.find( '|' ) ) + '|' + var_name );
                              }
                           }
                        }
                     }
                  }

                  bool skip = false;

                  if( old_value == c_notifier_created )
                  {
                     if( value == "attrib" )
                        skip = true;
                     else
                     {
                        if( value == "delete" )
                           value = "";
                        else if( !value.empty( ) )
                           value = c_notifier_created;
                     }
                  }
                  else if( value == "attrib" )
                  {
                     if( old_value.find( moved_from_prefix ) == 0 )
                        value = old_value;
                     else
                        value = c_notifier_modified;
                  }
                  else if( value == "create" )
                  {
                     if( ( old_value != c_notifier_deleted ) 
                      && ( old_value.find( c_notifier_moved_to ) != 0 ) )
                        value = c_notifier_created;
                     else
                        value = c_notifier_modified;
                  }
                  else if( value == "modify" )
                     value = c_notifier_modified;
                  else if( value == "delete" )
                  {
                     if( old_value.find( moved_from_prefix ) == 0 )
                     {
                        old_value.erase( 0, moved_from_prefix.length( ) );

                        if( old_value.find( c_notifier_moved_to ) == 0 )
                           set_system_variable( old_value, c_notifier_deleted );

                        value.erase( );
                        old_value.erase( );
                     }
                     else
                        value = c_notifier_deleted;
                  }

                  if( value == "delete_self" )
                  {
                     if( var_name != watch_root_name )
                        skip = true;
                     else
                     {
                        value = string( c_finishing );
                        var_name = watch_variable_name;
                     }
                  }

                  if( !skip )
                     set_system_variable( var_name, value );
               }
            }
         }
         else
         {
            msleep( c_wait_sleep_time );

            if( get_raw_system_variable( watch_variable_name ) == c_finishing )
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
         set_system_variable( watch_root_name, "" );
      else
         set_system_variable( watch_root_name + '*', "" );

      set_system_variable( watch_variable_name, "" );
   }

   TRACE_LOG( TRACE_SESSIONS, "notifier finished for '" + watch_root + "'" );

   decrement_active_listeners( );

   delete this;
}

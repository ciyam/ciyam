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

trace_mutex g_mutex;

const char c_created = '!';
const char c_unchanged = ':';

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

string get_value_and_unique( const string& raw_value, string* p_unique = 0 )
{
   string retval( raw_value );

   if( !retval.empty( ) )
   {
      if( retval[ 0 ] == '[' )
      {
         string::size_type pos = retval.find( ']', 1 );

         if( pos == string::npos )
            throw runtime_error( "unexpected raw value '" + raw_value + "'" );

         if( p_unique )
            *p_unique = retval.substr( 1, pos - 1 );

         retval.erase( 0, pos + 1 );
      }
   }

   return retval;
}

inline string get_value_from_system_variable( const string& var_name, string* p_unique = 0 )
{
   return get_value_and_unique( get_raw_system_variable( var_name ), p_unique );
}

}

ciyam_notifier::ciyam_notifier( const string& watch_root )
 :
 unique( 0 ),
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

            // NOTE Skip hidden folders.
            if( next_dir.find( "/." ) == string::npos )
            {
               file_filter ff;
               fs_iterator fs( next_dir, &ff );

               pos = next_dir.find( path_to_dir );

               if( pos != 0 )
                  throw runtime_error( "unexpected '" + next_dir + "' does start with '" + path_to_dir + "'" );

               next_dir.erase( 0, path_to_dir.length( ) );

               if( next_dir != watch_root )
                  n.add_watch( next_dir );

               string unique( get_next_unique( ) );

               string prefix( '[' + unique + ']' );

               set_system_variable( next_dir + '/', prefix + c_notifier_none );
               set_system_variable( watch_variable_name + unique, next_dir + '/' );

               while( fs.has_next( ) )
               {
                  string name( fs.get_name( ) );

                  // NOTE: SKip hidden files.
                  if( !name.empty( ) && name[ 0 ] != '.' )
                  {
                     string next_file( fs.get_full_name( ) );

                     pos = next_file.find( path_to_dir );

                     if( pos != 0 )
                        throw runtime_error( "unexpected '" + next_file + "' does start with '" + path_to_dir + "'" );

                     next_file.erase( 0, path_to_dir.length( ) );

                     string unique( get_next_unique( ) );

                     string prefix( '[' + unique + ']' );

                     string extra( get_raw_system_variable( next_file ) );

                     set_system_variable( next_file, prefix + extra + c_notifier_none );
                     set_system_variable( watch_variable_name + unique, next_file );
                  }
               }
            }
         } while( dfsi.has_next( ) );
      }

      set_system_variable( watch_variable_name, c_watching );

      has_set_system_variable = true;

      TRACE_LOG( TRACE_SESSIONS, "notifier started for '" + watch_root + "'" );

      string events;

      string moved_from_prefix( string( c_notifier_moved_from ) + '|' );
      string modified_from_prefix( string( c_notifier_modified_from ) + '|' );

      while( true )
      {
         if( g_server_shutdown )
         {
            issue_warning( "application server is being shutdown" );
            break;
         }

         // NOTE: Supports event testing via a special system variable.
         if( events.empty( ) )
            events = get_system_variable( get_special_var_name( e_special_var_notifier_events ) );

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

            events.erase( );

            map< string, string > cookie_id_current_names;
            map< string, string > cookie_id_unique_values;
            map< string, string > cookie_id_original_names;

            for( size_t i = 0; i < all_events.size( ); i++ )
            {
               string extra, value, old_value, unique_value;

               string next_event( all_events[ i ] );

               bool reportable_event = true;

               if( !next_event.empty( ) )
               {
                  TRACE_LOG( TRACE_NOTIFIER, "event: " + next_event );

                  if( next_event[ 0 ] == '-' )
                     reportable_event = false;

                  if( next_event[ 0 ] == '*' )
                  {
                     string::size_type pos = next_event.find( '+' );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected next event format '" + next_event + "'" );

                     unique_value = next_event.substr( 1, pos - 1 );

                     next_event.erase( 0, pos );
                  }

                  next_event.erase( 0, 1 );

               }

               if( !next_event.empty( ) && reportable_event )
               {
                  string::size_type pos = next_event.find( '|' );

                  string var_name( next_event.substr( 0, pos ) );

                  old_value = get_value_from_system_variable( var_name, &unique_value );

                  // NOTE: The extra string is all characters before the first 'a-z' character.
                  if( !old_value.empty( ) && ( old_value[ 0 ] < 'a' || old_value[ 0 ] > 'z' ) )
                  {
                     string::size_type pos = old_value.find_first_of( "abcdefghijklmnopqrstuvwxyz" );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected old_value '" + old_value + "'" );

                     extra = old_value.substr( 0, pos );
                     old_value.erase( 0, pos );
                  }

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
                              value.erase( );

                           if( cookie_id_original_names.find( cookie_id ) == cookie_id_original_names.end( ) )
                           {
                              if( old_value == c_notifier_created )
                              {
                                 value.erase( );

                                 if( !unique_value.empty( ) )
                                    cookie_id_unique_values.insert( make_pair( cookie_id, unique_value + extra ) );

                                 if( var_name.empty( ) || ( var_name[ var_name.length( ) - 1 ] != '/' ) )
                                    cookie_id_original_names.insert( make_pair( cookie_id, "" ) );
                                 else
                                    cookie_id_original_names.insert( make_pair( cookie_id, c_created + var_name ) );
                              }
                              else
                              {
                                 string prefix;

                                 if( old_value.find( moved_from_prefix ) == 0 )
                                    prefix = moved_from_prefix;
                                 else if( old_value.find( modified_from_prefix ) == 0 )
                                    prefix = modified_from_prefix;

                                 if( !unique_value.empty( ) )
                                    cookie_id_unique_values.insert( make_pair( cookie_id, unique_value + extra ) );

                                 if( !prefix.empty( ) )
                                 {
                                    old_value.erase( 0, prefix.length( ) );

                                    cookie_id_current_names.insert( make_pair( cookie_id, var_name ) );
                                    cookie_id_original_names.insert( make_pair( cookie_id, old_value ) );
                                    
                                    value.erase( );
                                    old_value.erase( );
                                 }
                                 else if( value == c_notifier_moved_from )
                                    value = c_notifier_created;
                                 else
                                 {
                                    if( old_value != c_notifier_none )
                                       cookie_id_original_names.insert( make_pair( cookie_id, var_name ) );
                                    else
                                       cookie_id_original_names.insert( make_pair( cookie_id, c_unchanged + var_name ) );
                                 }
                              }
                           }
                           else
                           {
                              string original_name( cookie_id_original_names[ cookie_id ] );

                              if( unique_value.empty( ) )
                              {
                                 unique_value = cookie_id_unique_values[ cookie_id ];

                                 string::size_type pos = unique_value.find_first_not_of( "0123456789" );

                                 if( pos != string::npos )
                                 {
                                    extra = unique_value.substr( pos );
                                    unique_value.erase( pos );
                                 }
                              }

                              if( original_name.empty( ) )
                              {
                                 if( old_value.empty( ) )
                                    value = c_notifier_created;
                                 else
                                 {
                                    string::size_type pos = old_value.find( '|' );

                                    if( pos == string::npos )
                                       value = c_notifier_modified;
                                    else
                                       value = c_notifier_modified_from + old_value.substr( pos );
                                 }
                              }
                              else
                              {
                                 if( original_name == var_name )
                                    value = c_notifier_modified;
                                 else if( original_name[ 0 ] == c_created )
                                 {
                                    value = c_notifier_created;
                                    original_name.erase( 0, 1 );
                                 }
                                 else
                                 {
                                    if( original_name[ 0 ] == c_unchanged )
                                    {
                                       original_name.erase( 0, 1 );
                                       value = string( c_notifier_moved_from ) + '|' + original_name;
                                    }
                                    else
                                       value = string( c_notifier_modified_from ) + '|' + original_name;
                                 }

                                 // NOTE: Remove the original name's system variable and then
                                 // replace any other system variables that are found to have
                                 // been prefixed by the original name (if it is a folder).
                                 set_system_variable( original_name, "" );

                                 if( original_name[ original_name.length( ) - 1 ] == '/' )
                                 {
                                    string all_prefixed_variables( get_raw_system_variable( original_name + "*" ) );

                                    if( !all_prefixed_variables.empty( ) )
                                    {
                                       vector< string > prefixed_variables;

                                       split( all_prefixed_variables, prefixed_variables, '\n' );

                                       for( size_t i = 0; i < prefixed_variables.size( ); i++ )
                                       {
                                          string next_name, next_value;

                                          next_name = variable_name_from_name_and_value( prefixed_variables[ i ], &next_value );

                                          set_system_variable( next_name, "" );

                                          next_name = var_name + next_name.substr( original_name.length( ) );

                                          string unique_value;
                                          get_value_and_unique( next_value, &unique_value );

                                          set_system_variable( next_name, next_value );

                                          if( !unique_value.empty( ) )
                                             set_system_variable( watch_variable_name + unique_value, next_name );
                                       }
                                    }
                                 }
                              }

                              cookie_id_current_names.erase( cookie_id );
                              cookie_id_unique_values.erase( cookie_id );
                              cookie_id_original_names.erase( cookie_id );
                           }
                        }
                     }
                  }

                  bool skip = false;
                  bool is_folder = false;

                  if( !var_name.empty( )
                   && ( var_name[ var_name.length( ) - 1 ] == '/' ) )
                     is_folder = true;

                  string tagged_extra;

                  if( old_value == c_notifier_created )
                  {
                     if( value == "attrib" )
                        skip = true;
                     else
                     {
                        if( value == "delete" )
                           value = "";
                        else if( !value.empty( ) )
                        {
                           value = c_notifier_created;

                           if( !is_folder )
                              tagged_extra = c_notifier_selection;
                        }
                     }
                  }
                  else if( value == "attrib" )
                  {
                     if( ( old_value.find( moved_from_prefix ) == 0 )
                      || ( old_value.find( modified_from_prefix ) == 0 ) )
                        value = old_value;
                     else
                     {
                        value = c_notifier_modified;

                        if( !is_folder )
                           tagged_extra = c_notifier_selection;
                     }
                  }
                  else if( value == "create" )
                  {
                     if( old_value != c_notifier_deleted ) 
                        value = c_notifier_created;
                     else
                        value = c_notifier_modified;

                     if( !is_folder )
                        tagged_extra = c_notifier_selection;
                  }
                  else if( value == "modify" )
                  {
                     value = c_notifier_modified;

                     if( !is_folder )
                        tagged_extra = c_notifier_selection;
                  }
                  else if( value == "delete" )
                  {
                     string prefix;

                     if( old_value.find( moved_from_prefix ) == 0 )
                        prefix = moved_from_prefix;
                     else if( old_value.find( modified_from_prefix ) == 0 )
                        prefix = modified_from_prefix;

                     extra.erase( );
                     value = c_notifier_deleted;

                     if( !prefix.empty( ) )
                     {
                        set_system_variable( var_name, "" );
                        var_name = old_value.substr( prefix.length( ) );

                        // NOTE: If a file was renamed but another with the original
                        // name had been created then will handle as a modification.
                        if( has_system_variable( var_name ) )
                        {
                           string prior_value( get_raw_system_variable( var_name ) );

                           string prior_unique;
                           get_value_and_unique( prior_value, &prior_unique );

                           value = c_notifier_modified;
                           tagged_extra = c_notifier_selection;

                           set_system_variable( watch_variable_name + prior_unique, "" );
                        }
                     }
                  }

                  if( extra.empty( ) )
                     extra = tagged_extra;

                  if( value == "delete_self" )
                  {
                     skip = true;

                     if( var_name == watch_root_name )
                     {
                        value = string( c_finishing );
                        var_name = watch_variable_name;

                        set_system_variable( var_name, value );
                     }
                  }

                  if( !skip )
                  {
                     if( !recurse )
                        set_system_variable( var_name, value );
                     else
                     {
                        if( unique_value.empty( ) )
                        {
                           unique_value = get_next_unique( );
                           set_system_variable( watch_variable_name + unique_value, var_name );
                        }

                        if( !value.empty( ) )
                        {
                           set_system_variable( watch_variable_name + unique_value, var_name );
                           set_system_variable( var_name, '[' + unique_value + ']' + extra + value );
                        }
                        else
                        {
                           set_system_variable( var_name, "" );
                           set_system_variable( watch_variable_name + unique_value, "" );
                        }
                     }
                  }
               }
            }

            // NOTE: Files that have been moved outside the scope of the notifier watches
            // will not receive a matching "move" event so the "events" variable is being
            // used to instead treat them as deletes.
            if( !cookie_id_original_names.empty( ) )
            {
               string removed_events;

               for( map< string, string >::iterator i = cookie_id_original_names.begin( ); i != cookie_id_original_names.end( ); ++i )
               {
                  string next_original_name( i->second );

                  if( !next_original_name.empty( ) && ( next_original_name[ 0 ] == c_unchanged ) )
                  {
                     if( !removed_events.empty( ) )
                        removed_events += '\n';

                     string unique_value( cookie_id_unique_values[ i->first ] );

                     // NOTE: Ensure that the original unique value is re-used for the delete.
                     if( !unique_value.empty( ) )
                        removed_events += '*' + unique_value;

                     removed_events += '+' + ( i->second ).substr( 1 ) + "|delete|0";
                  }
               }

               events = removed_events;
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

      set_system_variable( watch_variable_name + '*', "" );
   }

   TRACE_LOG( TRACE_SESSIONS, "notifier finished for '" + watch_root + "'" );

   decrement_active_listeners( );

   delete this;
}

string ciyam_notifier::get_next_unique( )
{
   return to_comparable_string( unique++, false, 6 );
}

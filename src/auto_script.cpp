// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2021 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <ctime>
#  include <csignal>
#  include <map>
#  include <vector>
#  include <string>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "auto_script.h"

#include "sio.h"
#include "format.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "ciyam_session.h"

//#define DEBUG

using namespace std;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

#include "ciyam_constants.h"

const char* const c_script_dummy_filename = "*script*";

const char* const c_section_script = "script";

const char* const c_attribute_name = "name";
const char* const c_attribute_time = "time";
const char* const c_attribute_cycle = "cycle";
const char* const c_attribute_start = "start";
const char* const c_attribute_finish = "finish";
const char* const c_attribute_exclude = "exclude";
const char* const c_attribute_filename = "filename";
const char* const c_attribute_arguments = "arguments";
const char* const c_attribute_timestamp = "timestamp";

const int c_min_cycle_seconds_for_logging = 3600;

// NOTE: This figure will allow an event that recurs every minute to be able to be skipped for max. 10 days.
// FUTURE: This value should probably be allowed to be overidden by a configuration option.
const size_t c_max_reschedule_attempts = 15000;

enum exclude_type
{
   e_exclude_type_none,
   e_exclude_type_mondays,
   e_exclude_type_tuesdays,
   e_exclude_type_wednesdays,
   e_exclude_type_thursdays,
   e_exclude_type_fridays,
   e_exclude_type_saturdays,
   e_exclude_type_sundays,
   e_exclude_type_weekdays,
   e_exclude_type_weekends
};

struct script_info
{
   script_info( ) : last_mod( 0 ), exclude( e_exclude_type_none ), allow_late_exec( false ) { }

   string name;

   mtime start_time;
   mtime finish_time;
   int cycle_seconds;

   udate start_date;
   udate finish_date;

   string filename;
   string arguments;

   time_t last_mod;

   string tsfilename;

   exclude_type exclude;

   bool allow_late_exec;
};

mutex g_mutex;

bool g_has_read = false;

time_t g_scripts_mod = 0;

vector< script_info > g_scripts;

typedef multimap< date_time, int > script_schedule_container;
typedef script_schedule_container::const_iterator script_schedule_const_iterator;

script_schedule_container g_script_schedule;

void read_script_info( )
{
   guard g( g_mutex );

   try
   {

      if( !g_has_read )
         g_has_read = true;
      else
      {
         // NOTE: If is not the first read then log the re-read.
         TRACE_LOG( TRACE_ANYTHING, "[autoscript.sio] updated" );
      }

      g_scripts.clear( );
      g_script_schedule.clear( );

      if( file_exists( c_autoscript_file ) )
      {
         ifstream inpf( c_autoscript_file );

         sio_reader reader( inpf );
         while( reader.has_started_section( c_section_script ) )
         {
            script_info info;

            info.name = reader.read_attribute( c_attribute_name );

            string time_info( reader.read_attribute( c_attribute_time ) );
            string::size_type pos = time_info.find( '-' );

            if( pos == string::npos )
               info.start_time = info.finish_time = mtime( time_info );
            else
            {
               info.start_time = mtime( time_info.substr( 0, pos ) );
               info.finish_time = mtime( time_info.substr( pos + 1 ) );
            }

            string cycle( reader.read_attribute( c_attribute_cycle ) );
            if( !cycle.empty( ) && cycle[ cycle.size( ) - 1 ] == '*' )
            {
               info.allow_late_exec = true;
               cycle.erase( cycle.size( ) - 1 );
            }

            info.cycle_seconds = unformat_duration( cycle );

            info.start_date = udate::local( );
            string s( reader.read_opt_attribute( c_attribute_start ) );
            if( !s.empty( ) )
               info.start_date = udate( s );

            s = reader.read_opt_attribute( c_attribute_finish );
            if( !s.empty( ) )
               info.finish_date = udate( s );

            string exclude( reader.read_opt_attribute( c_attribute_exclude ) );
            if( !exclude.empty( ) )
            {
               string val = lower( exclude );

               if( val == "mondays" )
                  info.exclude = e_exclude_type_mondays;
               else if( val == "tuesdays" )
                  info.exclude = e_exclude_type_tuesdays;
               else if( val == "wednesdays" )
                  info.exclude = e_exclude_type_wednesdays;
               else if( val == "thursdays" )
                  info.exclude = e_exclude_type_thursdays;
               else if( val == "fridays" )
                  info.exclude = e_exclude_type_fridays;
               else if( val == "saturdays" )
                  info.exclude = e_exclude_type_saturdays;
               else if( val == "sundays" )
                  info.exclude = e_exclude_type_sundays;
               else if( val == "weekdays" )
                  info.exclude = e_exclude_type_weekdays;
               else if( val == "weekends" )
                  info.exclude = e_exclude_type_weekends;
               else
                  throw runtime_error( "invalid exclude value '" + exclude + "'" );
            }

            info.filename = reader.read_attribute( c_attribute_filename );
            info.arguments = reader.read_opt_attribute( c_attribute_arguments );
            info.tsfilename = reader.read_opt_attribute( c_attribute_timestamp );

            if( file_exists( info.tsfilename ) )
               info.last_mod = last_modification_time( info.tsfilename );

            g_scripts.push_back( info );

            reader.finish_section( c_section_script );
         }

         reader.verify_finished_sections( );

         for( int i = 0; i < g_scripts.size( ); i++ )
         {
            date_time starts_at = max(
             date_time( udate::local( ), g_scripts[ i ].start_time ),
             date_time( g_scripts[ i ].start_date, g_scripts[ i ].start_time ) );

            g_script_schedule.insert( make_pair( starts_at, i ) );
         }
      }
   }
   catch( exception& x )
   {
      throw runtime_error( x.what( ) + string( " in " ) + string( c_autoscript_file ) );
   }
}

bool is_excluded( const script_info& info, const date_time& dt )
{
   bool excluded = false;

   if( info.finish_time != info.start_time )
   {
      if( dt.get_time( ) > info.finish_time )
         excluded = true;
   }

   switch( info.exclude )
   {
      case e_exclude_type_none:
      break;

      case e_exclude_type_mondays:
      if( ( weekday )dt == e_weekday_monday )
         excluded = true;
      break;

      case e_exclude_type_tuesdays:
      if( ( weekday )dt == e_weekday_tuesday )
         excluded = true;
      break;

      case e_exclude_type_wednesdays:
      if( ( weekday )dt == e_weekday_wednesday )
         excluded = true;
      break;

      case e_exclude_type_thursdays:
      if( ( weekday )dt == e_weekday_thursday )
         excluded = true;
      break;

      case e_exclude_type_fridays:
      if( ( weekday )dt == e_weekday_friday )
         excluded = true;
      break;

      case e_exclude_type_saturdays:
      if( ( weekday )dt == e_weekday_saturday )
         excluded = true;
      break;

      case e_exclude_type_sundays:
      if( ( weekday )dt == e_weekday_sunday )
         excluded = true;
      break;

      case e_exclude_type_weekdays:
      if( ( weekday )dt != e_weekday_saturday && ( weekday )dt != e_weekday_sunday )
         excluded = true;
      break;

      case e_exclude_type_weekends:
      if( ( weekday )dt == e_weekday_saturday || ( weekday )dt == e_weekday_sunday )
         excluded = true;
      break;

      default:
      throw runtime_error( "unexpected exclude value #" + to_string( info.exclude ) );
   }

   return excluded;
}

bool scripts_file_has_changed( )
{
   bool changed = false;

   time_t t = 0;
   if( file_exists( c_autoscript_file ) )
      t = last_modification_time( c_autoscript_file );

   if( t != g_scripts_mod )
      changed = true;

   g_scripts_mod = t;

   return changed;
}

}

void output_schedule( ostream& os )
{
   guard g( g_mutex );

   for( script_schedule_const_iterator ssci = g_script_schedule.begin( ); ssci != g_script_schedule.end( ); ++ssci )
   {
      os << ( ssci->first ).as_string( true, false ) << ' ' << g_scripts[ ssci->second ].name;

      if( !g_scripts[ ssci->second ].tsfilename.empty( ) )
         os << " [" << g_scripts[ ssci->second ].tsfilename << "]";

      os << '\n';
   }
}

autoscript_session::autoscript_session( )
{
   ciyam_session::increment_session_count( );
}

autoscript_session::~autoscript_session( )
{
   ciyam_session::decrement_session_count( );
}

void autoscript_session::on_start( )
{
#ifdef DEBUG
   cout << "started autoscript session..." << endl;
#endif
   try
   {
      bool changed = false;
      bool script_reconfig( get_script_reconfig( ) );

      if( !script_reconfig )
         changed = true;

      TRACE_LOG( TRACE_SESSIONS,
       "started autoscript session (tid = " + to_string( current_thread_id( ) ) + ")" );

      while( true )
      {
         msleep( 1000 );

         if( g_server_shutdown )
            break;

         // NOTE: The reading of the autoscript file is delayed by one
         // pass to try and make sure it is not being attempted during
         // an update.
         if( changed )
         {
            changed = false;
            read_script_info( );
         }
         else if( script_reconfig && scripts_file_has_changed( ) )
            changed = true;

         guard g( g_mutex );

         date_time now = date_time::local( );
         script_schedule_const_iterator i, j;

         vector< date_time > erase_items;
         script_schedule_container new_schedule_items;

         i = g_script_schedule.lower_bound( now );
         for( j = g_script_schedule.begin( ); j != i; ++j )
         {
            if( j->first <= now )
            {
               erase_items.push_back( j->first );

               date_time next( j->first );

               bool okay = true;
               time_t mod_time = 0;

               // NOTE: If a script is dependent upon file modification then
               // check whether the file's modificaton time has been changed.
               string tsfilename( g_scripts[ j->second ].tsfilename );
               if( !tsfilename.empty( ) && file_exists( tsfilename ) )
               {
                  mod_time = last_modification_time( tsfilename );

                  if( mod_time == g_scripts[ j->second ].last_mod )
                     okay = false;
               }

               if( okay && !is_excluded( g_scripts[ j->second ], now )
                && ( g_scripts[ j->second ].allow_late_exec || ( now - next <= 1.0 ) ) )
               {
                  string filename( g_scripts[ j->second ].filename );
                  bool is_script = ( filename == c_script_dummy_filename );

                  string arguments( process_script_args( g_scripts[ j->second ].arguments ) );

                  int cycle_seconds = g_scripts[ j->second ].cycle_seconds;

                  if( !tsfilename.empty( ) )
                  {
                     cycle_seconds = mod_time - g_scripts[ j->second ].last_mod;

                     if( cycle_seconds < 0 )
                        cycle_seconds *= -1;

                     g_scripts[ j->second ].last_mod = mod_time;
                  }

                  if( is_script )
                  {
                     string script_args( "~" + uuid( ).as_string( ) );

                     ofstream outf( script_args.c_str( ) );
                     if( !outf )
                        throw runtime_error( "unable to open '" + script_args + "' for output" );

                     outf << "<" << arguments << endl;
                     outf.close( );

                     // NOTE: Skip logging for any scripts that cycle too frequently.
                     if( cycle_seconds >= c_min_cycle_seconds_for_logging )
                        script_args += " " + g_scripts[ j->second ].name;

#ifdef _WIN32
                     // KLUDGE: For some reason under Windows if multiple scripts need to be run in
                     // one pass then subsequent execs can fail to work if this delay is not included.
                     msleep( 250 );
                     exec_system( "script " + script_args, true );
#else
                     exec_system( "./script " + script_args, true );
#endif
                  }
                  else
                  {
                     string cmd_and_args( filename );

#ifndef _WIN32
                     if( cmd_and_args.find( '/' ) == string::npos )
                        cmd_and_args = "./" + cmd_and_args;
#endif

                     if( !arguments.empty( ) )
                        cmd_and_args += " " + arguments;

                     exec_system( cmd_and_args, true );
                  }
               }

               size_t count = 0;
               while( next <= now || is_excluded( g_scripts[ j->second ], next ) )
               {
                  if( ++count >= c_max_reschedule_attempts )
                  {
                     TRACE_LOG( TRACE_ANYTHING,
                      "warning: unable to scheule autoscript '" + g_scripts[ j->second ].name + "'" );

                     next = date_time::maximum( );
                     break;
                  }

                  next += ( seconds )g_scripts[ j->second ].cycle_seconds;
               }

               if( g_scripts[ j->second ].finish_date != udate( )
                && now.get_date( ) > g_scripts[ j->second ].finish_date )
                  continue;

               new_schedule_items.insert( make_pair( next, j->second ) );
            }
         }

         for( int x = 0; x < erase_items.size( ); x++ )
            g_script_schedule.erase( erase_items[ x ] );

         g_script_schedule.insert( new_schedule_items.begin( ), new_schedule_items.end( ) );
      }

      TRACE_LOG( TRACE_SESSIONS, "finished autoscript session" );
   }
   catch( exception& x )
   {
#ifdef DEBUG
      cout << "autoscript error: " << x.what( ) << endl;
#endif
      TRACE_LOG( TRACE_ANYTHING, string( "autoscript error: " ) + x.what( ) );
   }
   catch( ... )
   {
#ifdef DEBUG
      cout << "unexpected autoscript exception..." << endl;
#endif
      TRACE_LOG( TRACE_ANYTHING, "autoscript error: unexpected unknown exception caught" );
   }
#ifdef DEBUG
   cout << "finished autoscript session..." << endl;
#endif
   delete this;
}

void init_auto_script( )
{
   autoscript_session* p_autoscript_session = new autoscript_session;
   p_autoscript_session->start( );
}

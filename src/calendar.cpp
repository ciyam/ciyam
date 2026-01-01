// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <string>
#  include <vector>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  include <unistd.h>
#endif

#include "sio.h"
#include "date_time.h"
#include "utilities.h"

using namespace std;

namespace
{

const char* const c_line = "-----------------------------------";
const char* const c_dline = "===================================";

const char* const c_week_days = "Mon  Tue  Wed  Thu  Fri  Sat  Sun";

const char* const c_section_event = "event";
const char* const c_section_events = "events";

const char* const c_attribute_name = "name";
const char* const c_attribute_type = "type";
const char* const c_attribute_extra = "extra";
const char* const c_attribute_occurs = "occurs";

struct event
{
   event( ) : repeat( 0 ) { }

   string name;
   string extra;

   udate date;

   int repeat;

   udate started_on;
   udate finished_on;

   mtime start_time;
   mtime finish_time;
};

vector< event > g_events;

string pad_left_for_line_size( const string& str, size_t len )
{
   string s( str );

   while( s.length( ) < len )
   {
      s = ' ' + s;
      len -= 1;
   }

   return s;
}

string put_title_in_line_center( const string& line, const string& title )
{
   string s( line );
   size_t len = line.size( );

   string::size_type pos = ( len / 2 ) - ( title.length( ) / 2 );

   s.erase( pos, title.length( ) );
   s.insert( pos, title );

   return s;
}

void read_calendar_events( ifstream& inpf, const udate& udm )
{
   sio_reader reader( inpf );

   if( reader.has_started_section( c_section_events ) )
   {
      while( reader.has_started_section( c_section_event ) )
      {
         event new_event;

         new_event.name = reader.read_attribute( c_attribute_name );

         string type( reader.read_opt_attribute( c_attribute_type ) );

         if( !type.empty( ) )
         {
            if( type == "EAS" )
               new_event.date = udate( udm.get_year( ), e_day_of_significance_easter_sunday );
            else if( type == "CNY" )
               new_event.date = udate( udm.get_year( ), e_day_of_significance_chinese_new_year );
            else
               throw runtime_error( "unknown event type '" + type + "'" );
         }

         new_event.extra = reader.read_opt_attribute( c_attribute_extra );

         string occurs( reader.read_opt_attribute( c_attribute_occurs ) );

         if( !occurs.empty( ) )
         {
            string::size_type pos = occurs.find( '-' );

            if( pos != string::npos )
            {
               string::size_type npos = occurs.find( '-', pos + 1 );

               year y = udm.get_year( );

               if( npos != string::npos )
               {
                  y = ( year )atoi( occurs.substr( 0, pos ).c_str( ) );

                  occurs.erase( 0, pos + 1 );

                  pos = occurs.find( '-' );

                  int max_repeats = 0;
                  string::size_type rpos = occurs.find( '+' );

                  if( rpos != string::npos )
                  {
                     if( rpos != occurs.length( ) - 1 )
                        max_repeats = atoi( occurs.substr( rpos + 1 ).c_str( ) );

                     new_event.repeat = udm.get_year( ) - y;

                     if( !max_repeats || new_event.repeat <= max_repeats )
                        y = udm.get_year( );
                     else
                     {
                        new_event.repeat = max_repeats;
                        y = udm.get_year( ) + max_repeats;
                     }

                     occurs.erase( rpos );
                  }
               }

               month m = ( month )atoi( occurs.substr( 0, pos ).c_str( ) );
               day d = ( day )atoi( occurs.substr( pos + 1 ).c_str( ) );

               new_event.date = udate( y, m, d );
            }
            else
            {
               pos = occurs.find( ' ' );

               if( pos != string::npos )
               {
                  month m = ( month )atoi( occurs.substr( 0, pos ).c_str( ) );

                  occurs.erase( 0, pos + 1 );
                  pos = occurs.find( ' ' );

                  if( pos != string::npos )
                  {
                     string day = occurs.substr( 0, pos );

                     weekday wd;

                     if( day == "Mon" )
                        wd = e_weekday_monday;
                     else if( day == "Tue" )
                        wd = e_weekday_tuesday;
                     else if( day == "Wed" )
                        wd = e_weekday_wednesday;
                     else if( day == "Thu" )
                        wd = e_weekday_thursday;
                     else if( day == "Fri" )
                        wd = e_weekday_friday;
                     else if( day == "Sat" )
                        wd = e_weekday_saturday;
                     else if( day == "Sun" )
                        wd = e_weekday_sunday;
                     else
                        throw runtime_error( "unexpected weekday: " + day );

                     occurrence occ = ( occurrence )atoi( occurs.substr( pos + 1 ).c_str( ) );

                     new_event.date = udate( udm.get_year( ), m, wd, occ );
                  }
               }
               else
                  throw runtime_error( "unexpected occurs: " + occurs );
            }
         }

         reader.finish_section( c_section_event );

         g_events.push_back( new_event );
      }

      reader.finish_section( c_section_events );
   }

   reader.verify_finished_sections( );
}

void output_calendar( ostream& outf, const udate& ud, const udate& udm )
{
   outf << '\n' << c_dline << '\n';

   string month_and_year( udm.month_name( ) );
   month_and_year += ' ' + to_string( udm.get_year( ) );

   outf << pad_left_for_line_size( month_and_year, 35 ) << '\n';

   outf << c_line << '\n';

   outf << ' ' << c_week_days << '\n';

   outf << c_line << '\n';

   udate ud_tmp( udm );
   weekday wd = ( weekday )ud_tmp;

   if( wd > e_weekday_monday )
   {
      string prefix( ( ( int )wd - 1 ) * 5, ' ' );

      outf << prefix;
   }

   bool has_today = false;

   multimap< udate, pair< string, string > > month_events;

   while( true )
   {
      if( ud_tmp == ud )
      {
         has_today = true;
         outf << '[';
      }
      else
         outf << ' ';

      outf << to_string( ( int )ud_tmp.get_day( ) );

      bool has_event = false;
      for( size_t i = 0; i < g_events.size( ); i++ )
      {
         if( g_events[ i ].date == ud_tmp && g_events[ i ].repeat >= 0 )
         {
            has_event = true;
            string extra( g_events[ i ].extra );

            if( g_events[ i ].repeat > 0 )
            {
               extra += " (" + to_string( g_events[ i ].repeat );

               if( g_events[ i ].repeat >= 10 && g_events[ i ].repeat <= 20 )
                  extra += "th";
               else if( g_events[ i ].repeat % 10 == 1 )
                  extra += "st";
               else if( g_events[ i ].repeat % 10 == 2 )
                  extra += "nd";
               else if( g_events[ i ].repeat % 10 == 3 )
                  extra += "rd";
               else
                  extra += "th";

               extra += ")";
            }

            month_events.insert(
             make_pair( g_events[ i ].date, make_pair( g_events[ i ].name, extra ) ) );
         }
      }

      if( has_event )
         outf << '*';
      else
         outf << ' ';

      if( ud_tmp.get_day( ) < 10 )
         outf << ' ';

      if( ud_tmp == ud )
         outf << ']';

      if( ( weekday )ud_tmp == e_weekday_sunday )
         outf << '\n';
      else if( ud_tmp != ud )
         outf << ' ';

      if( ud_tmp.get_day( ) == ud_tmp.days_in_month( ) )
         break;

      ud_tmp += ( days )1;
   }

   outf << '\n';

   if( has_today )
   {
      outf << c_line << '\n';
      string weekday_month_day_and_year( ud.weekday_name( ) );

      weekday_month_day_and_year += ", " + ud.month_name( );

      int day_of_month = ud.get_day( );

      weekday_month_day_and_year += ' ' + to_string( day_of_month );

      if( day_of_month >= 10 && day_of_month <= 20 )
         weekday_month_day_and_year += "th";
      else if( day_of_month % 10 == 1 )
         weekday_month_day_and_year += "st";
      else if( day_of_month % 10 == 2 )
         weekday_month_day_and_year += "nd";
      else if( day_of_month % 10 == 3 )
         weekday_month_day_and_year += "rd";
      else
         weekday_month_day_and_year += "th";

      weekday_month_day_and_year += ", " + to_string( ud.get_year( ) );

      outf << pad_left_for_line_size( weekday_month_day_and_year, 35 ) << '\n';
   }

   outf << c_dline << '\n';

   if( !month_events.empty( ) )
   {
      outf << '\n' << put_title_in_line_center( c_dline, " Events " ) << '\n';

      udate ud_mon( ud );
      while( ( weekday )ud_mon != e_weekday_monday )
         --ud_mon;

      bool todays = false;
      for( multimap< udate, pair< string, string > >::iterator
       i = month_events.begin( ); i != month_events.end( ); ++i )
      {
         outf << i->first << ": " << i->second.first << '\n';

         if( !i->second.second.empty( ) )
         {
            if( i->first == ud )
               outf << "** Today ** " << i->second.second << '\n';
            else if( i->first == ud + ( days )1 )
               outf << "Tomorrow... " << i->second.second << '\n';
            else if( i->first == ud - ( days )1 )
               outf << "(Yesterday) " << i->second.second << '\n';
            else if( i->first >= ud_mon && i->first < ud_mon + ( days )7 )
               outf << "(This Week) " << i->second.second << '\n';
            else if( i->first > ud && i->first < ud_mon + ( days )14 )
               outf << "(Next Week) " << i->second.second << '\n';
            else if( i->first < ud && i->first >= ud_mon - ( days )7 )
               outf << "(Last Week) " << i->second.second << '\n';
            else
               outf << "            " << i->second.second << '\n';
         }
      }

      outf << c_dline << '\n';
   }
#ifdef  __GNUG__
   outf << '\n';
#endif
}

}

int main( int argc, char* argv[ ] )
{
   int rc = 0;

   try
   {
      string exe_path( argv[ 0 ] );

      size_t pos = exe_path.find_last_of( "/" );

      if( pos != string::npos )
         exe_path.erase( pos );
      else
         exe_path.erase( );

      string sio_file( "calendar.sio" );

      if( !exe_path.empty( ) )
         sio_file = exe_path + "/" + sio_file;

      udate ud = udate::local( );

      day d = 1;

      year y = ud.get_year( );
      month m = ud.get_month( );

      udate udm( y, m, d );

      if( argc > 1 )
      {
         string input( argv[ 1 ] );

         if( input.length( ) <= 2 )
            m = ( month )atoi( input.c_str( ) );
         else if( input.length( ) == 4 )
            y = ( year )atoi( input.c_str( ) );
         else
         {
            string::size_type pos = input.find( '-' );
            if( pos != string::npos )
            {
               y = ( year )atoi( input.substr( 0, pos ).c_str( ) );
               m = ( month )atoi( input.substr( pos + 1 ).c_str( ) );
            }
            else
            {
               pos = input.find( '/' );
               if( pos != string::npos )
               {
                  m = ( month )atoi( input.substr( 0, pos ).c_str( ) );
                  y = ( year )atoi( input.substr( pos + 1 ).c_str( ) );
               }
            }
         }

         udm = udate( y, m, d );
      }

      if( file_exists( sio_file ) )
      {
         ifstream inpf( sio_file.c_str( ) );

         if( !inpf )
            throw runtime_error( "unable to open '" + sio_file + "' for input" );

         read_calendar_events( inpf, udm );
      }

      output_calendar( cout, ud, udm );
   }
   catch( exception& x )
   {
      rc = 1;

      cerr << "error: " << x.what( ) << endl;
   }
   catch( ... )
   {
      rc = 2;

      cerr << "unexpected exception caught" << endl;
   }

   return rc;
}

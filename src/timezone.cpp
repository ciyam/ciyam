// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <unistd.h>
#  include <ctime>
#  include <cstdio>
#  include <cstdlib>
#  include <string>
#  include <vector>
#  include <sstream>
#  include <iomanip>
#  include <iostream>
#endif

const int c_start_year = 2000;
const int c_step_seconds = 900;

using namespace std;

struct daylight_info
{
   int year_start;
   int year_finish;

   int start_month;
   int start_occurrence;
   int start_day_of_week;

   string start_time;

   int finish_month;
   int finish_occurrence;
   int finish_day_of_week;

   string finish_time;

   double utc_offset;

   bool operator ==( const daylight_info& o )
   {
      return utc_offset == o.utc_offset
       && start_month == o.start_month && start_occurrence == o.start_occurrence
       && start_day_of_week == o.start_day_of_week && start_time == o.start_time
       && finish_month == o.finish_month && finish_occurrence == o.finish_occurrence
       && finish_day_of_week == o.finish_day_of_week && finish_time == o.finish_time;
   }
};

void set_occurrence( int mday, int& occurrence )
{
   occurrence = 1;

   while( true )
   {
      mday -= 7;

      if( mday <= 0 )
         break;

      ++occurrence;
   }

   // NOTE: Assume 4th weekday actually means the last (represented by a 5).
   if( occurrence == 4 )
      occurrence = 5;
}

bool get_daylight_info( tm& t, daylight_info& daylight, double& utc_offset )
{
   int year = t.tm_year;

   time_t tt = mktime( &t );

   struct tm* p_tm;
   p_tm = localtime( &tt );

   int adjust = ( year < p_tm->tm_year ) ? c_step_seconds : c_step_seconds * -1;

   while( true )
   {
      p_tm = localtime( &tt );

      if( p_tm->tm_yday == 0 )
         break;

      tt += adjust;
   }

   daylight.year_start = daylight.year_finish = year + 1900;

   daylight.start_month = daylight.finish_month = 1;
   daylight.start_occurrence = daylight.finish_occurrence = 1;
   daylight.start_day_of_week = daylight.finish_day_of_week = p_tm->tm_wday ? p_tm->tm_wday : 7;
   daylight.start_time = daylight.finish_time = "00:00";

   int last_hour = 0;
   int last_min = 0;

   bool has_daylight = false;
   bool was_daylight = ( p_tm->tm_isdst > 0 );

   if( was_daylight )
      daylight.utc_offset = ( p_tm->tm_gmtoff / 3600.0 );

   while( true )
   {
      if( p_tm->tm_year > year )
         break;

      int gmtoff = p_tm->tm_gmtoff;
      int minutes = gmtoff / 60;

      bool is_daylight = ( p_tm->tm_isdst > 0 );

      double hours = ( gmtoff / 3600.0 );

      if( !is_daylight )
         utc_offset = hours;

      if( is_daylight != was_daylight )
      {
         has_daylight = true;

         if( is_daylight )
         {
            daylight.start_month = p_tm->tm_mon + 1;
            set_occurrence( p_tm->tm_mday, daylight.start_occurrence );
            daylight.start_day_of_week = p_tm->tm_wday ? p_tm->tm_wday : 7;

            t.tm_isdst = 1;

            // NOTE: Don't use the current hour and minute but instead add "step" minutes
            // to the previous hour and minute - in order to get the non-daylight time at
            // which daylight kicks in.
            last_min += c_step_seconds / 60;
            if( last_min >= 60 )
            {
               ++last_hour;
               last_min -= 60;
            }

            if( last_hour == 24 )
               last_hour = 0;

            ostringstream outs;
            outs << setw( 2 ) << setfill( '0' ) << last_hour << ':' << setw( 2 ) << last_min;
            daylight.start_time = outs.str( );

            daylight.utc_offset = hours;
         }
         else
         {
            daylight.finish_month = p_tm->tm_mon + 1;
            set_occurrence( p_tm->tm_mday, daylight.finish_occurrence );
            daylight.finish_day_of_week = p_tm->tm_wday ? p_tm->tm_wday : 7;

            t.tm_isdst = 0;

            ostringstream outs;
            outs << setw( 2 ) << setfill( '0' ) << p_tm->tm_hour << ':' << setw( 2 ) << p_tm->tm_min;
            daylight.finish_time = outs.str( );
         }
      }

      last_hour = p_tm->tm_hour;
      last_min = p_tm->tm_min;

      tt += c_step_seconds;
      p_tm = localtime( &tt );
      was_daylight = is_daylight;
   }

   return has_daylight;
}

int main( int argc, char* argv[ ] )
{
   if( argc < 4 )
      cout << "Usage: timezone <abbr>[:name] <tz_info> <description> [<format>]" << endl;
   else
   {
      string tz_abbr( "TZ=" );
      tz_abbr += argv[ 2 ];

      putenv( ( char* )tz_abbr.c_str( ) );

      time_t now = time( 0 );
      struct tm* p_t = localtime( &now );

      int this_year = p_t->tm_year + 1900;

      struct tm t;

      string format;
      if( argc > 4 )
         format = argv[ 4 ];

      t.tm_sec = 0;
      t.tm_min = 0;
      t.tm_hour = 0;

      t.tm_mon = 0;
      t.tm_mday = 1;
      t.tm_year = c_start_year - 1900;

      t.tm_yday = 0;
      t.tm_isdst = 0;

      daylight_info daylight;
      double utc_offset = 0.0;

      vector< daylight_info > changes;

      bool has_daylight = get_daylight_info( t, daylight, utc_offset );

      for( int i = c_start_year; i <= this_year + 2; i++ )
      {
         daylight_info new_daylight;
         double new_utc_offset = 0.0;

         t.tm_sec = 0;
         t.tm_min = 0;
         t.tm_hour = 0;

         t.tm_mon = 0;
         t.tm_mday = 1;
         t.tm_year = i - 1900;

         t.tm_yday = 0;
         t.tm_isdst = 0;

         bool new_has_daylight = get_daylight_info( t, new_daylight, new_utc_offset );

         if( new_has_daylight != has_daylight || !( new_daylight == daylight ) )
         {
            if( daylight.start_month != daylight.finish_month )
               changes.push_back( daylight );
            daylight = new_daylight;
         }
         else
            daylight.year_finish = new_daylight.year_finish;

         has_daylight = new_has_daylight;
      }

      bool found = false;

      string abbr( argv[ 1 ] ), name;
      string::size_type pos = abbr.find( ':' );

      if( pos != string::npos )
      {
         name = abbr.substr( pos + 1 );
         abbr.erase( pos );
      }

      string dabbr( abbr );

      if( dabbr == "GMT" )
         dabbr = "BST";
      else if( dabbr == "MST" )
         dabbr = "MSD";
      else if( dabbr == "PKT" )
         dabbr = "PKDT";
      else if( dabbr.length( ) > 2 && dabbr[ dabbr.length( ) - 2 ] == 'S' )
      {
         found = true;
         dabbr[ dabbr.length( ) - 2 ] = 'D';
      }

      if( !found )
      {
         if( dabbr.length( ) > 2 && dabbr[ dabbr.length( ) - 1 ] == 'T' )
            dabbr = dabbr.substr( 0, dabbr.length( ) - 1 ) + "ST";
      }

      if( format.empty( ) )
      {
         cout << "  <timezone/>\n";
         cout << "   <abbr>" << abbr << "\n";
         if( !name.empty( ) )
            cout << "   <name>" << name << "\n";
         cout << "   <tz_info>" << argv[ 2 ] << "\n";
         cout << "   <utc_offset>" << utc_offset << "\n";
         cout << "   <description>" << argv[ 3 ] << "\n";

         if( has_daylight || !changes.empty( ) )
            cout << "   <daylight_abbr>" << dabbr << "\n";

         if( has_daylight )
         {
            cout << "   <daylight_year_start>" << daylight.year_start << "\n";
            cout << "   <daylight_year_finish>" << daylight.year_finish << "\n";
            cout << "   <daylight_start_month>" << daylight.start_month << "\n";
            cout << "   <daylight_start_occurrence>" << daylight.start_occurrence << "\n";
            cout << "   <daylight_start_day_of_week>" << daylight.start_day_of_week << "\n";
            cout << "   <daylight_start_time>" << daylight.start_time << "\n";
            cout << "   <daylight_finish_month>" << daylight.finish_month << "\n";
            cout << "   <daylight_finish_occurrence>" << daylight.finish_occurrence << "\n";
            cout << "   <daylight_finish_day_of_week>" << daylight.finish_day_of_week << "\n";
            cout << "   <daylight_finish_time>" << daylight.finish_time << "\n";
            cout << "   <daylight_utc_offset>" << daylight.utc_offset << "\n";
         }

         if( !changes.empty( ) )
            cout << "   <daylight_saving_changes/>\n";
         for( size_t i = 0; i < changes.size( ); i++ )
         {
            cout << "    <daylight_saving_change/>\n";
            cout << "     <year_start>" << changes[ i ].year_start << "\n";
            cout << "     <year_finish>" << changes[ i ].year_finish << "\n";
            cout << "     <description>" << changes[ i ].year_start << "-" << changes[ i ].year_finish << "\n";
            cout << "     <start_month>" << changes[ i ].start_month << "\n";
            cout << "     <start_occurrence>" << changes[ i ].start_occurrence << "\n";
            cout << "     <start_day_of_week>" << changes[ i ].start_day_of_week << "\n";
            cout << "     <start_time>" << changes[ i ].start_time << "\n";
            cout << "     <finish_month>" << changes[ i ].finish_month << "\n";
            cout << "     <finish_occurrence>" << changes[ i ].finish_occurrence << "\n";
            cout << "     <finish_day_of_week>" << changes[ i ].finish_day_of_week << "\n";
            cout << "     <finish_time>" << changes[ i ].finish_time << "\n";
            cout << "     <utc_offset>" << changes[ i ].utc_offset << "\n";
            cout << "    </daylight_saving_change>\n";
         }
         if( !changes.empty( ) )
            cout << "   </daylight_saving_changes>\n";
         cout << "  </timezone>\n";
      }
      else if( format == "package" )
      {
         string key( abbr );
         if( !name.empty( ) )
            key = name;
         else
            name = abbr;

         cout << " <class/>\n";
         cout << "  <name>Timezone\n";
         if( !has_daylight )
         {
            cout << "  <fields>@key,Abbreviation,Name,TZ_Info,Description,UTC_Offset,Has_Daylight_Savings,Daylight_Abbreviation\n";
            cout << "  <record>" << key << ',' << abbr << ',' << name << ',' << argv[ 2 ] << ',' << argv[ 3 ] << ',' << utc_offset
             << ',' << has_daylight << ',' << dabbr << "\n";
         }
         else
         {
            cout << "  <fields>@key,Abbreviation,Name,TZ_Info,Description,UTC_Offset"
             ",Has_Daylight_Savings,Daylight_Abbreviation,Daylight_Year_Start,Daylight_Year_Finish"
             ",Daylight_Start_Month,Daylight_Start_Occurrence,Daylight_Start_Day_Of_Week,Daylight_Start_Time"
             ",Daylight_Finish_Month,Daylight_Finish_Occurrence,Daylight_Finish_Day_Of_Week,Daylight_Finish_Time,Daylight_UTC_Offset\n";

            cout << "  <record>" << key << ',' << abbr << ','
             << name << ',' << argv[ 2 ] << ',' << argv[ 3 ] << ',' << utc_offset << ','
             << has_daylight << ',' << dabbr << ',' << daylight.year_start << ',' << daylight.year_finish << ','
             << daylight.start_month << ',' << daylight.start_occurrence << ',' << daylight.start_day_of_week << ','
             << daylight.start_time << ',' << daylight.finish_month << ',' << daylight.finish_occurrence << ','
             << daylight.finish_day_of_week << ',' << daylight.finish_time << ',' << daylight.utc_offset << "\n";
         }
         cout << " </class>\n";

         if( !changes.empty( ) )
         {
            cout << " <class/>\n";
            cout << "  <name>Timezone_Daylight_Saving_Mod\n";
            cout << "  <fields>@key,Timezone,Year_Start,Year_Finish"
             ",Description,Start_Month,Start_Occurrence,Start_Day_Of_Week,Start_Time"
             ",Finish_Month,Finish_Occurrence,Finish_Day_Of_Week,Finish_Time,UTC_Offset\n";
         }

         for( size_t i = 0; i < changes.size( ); i++ )
         {
            ostringstream oss;
            oss << key << setw( 3 ) << setfill( '0' ) << i;

            cout << "  <record>" << oss.str( ) << ',' << key << ','
             << changes[ i ].year_start << ',' << changes[ i ].year_finish << ','
             << changes[ i ].year_start << '-' << changes[ i ].year_finish << ','
             << changes[ i ].start_month << ',' << changes[ i ].start_occurrence << ','
             << changes[ i ].start_day_of_week << ',' << changes[ i ].start_time << ','
             << changes[ i ].finish_month << ',' << changes[ i ].finish_occurrence << ','
             << changes[ i ].finish_day_of_week << ',' << changes[ i ].finish_time << ','
             << changes[ i ].utc_offset << "\n";
         }

         if( !changes.empty( ) )
            cout << " </class>\n";
      }
      else
         cerr << "error: unexpected format '" << format << "' (use 'package' or omit)" << endl;
   }
}

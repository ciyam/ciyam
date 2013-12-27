// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <string>
#  include <vector>
#  include <sstream>
#  include <stdexcept>
#endif

#include "format.h"

#include "date_time.h"
#include "utilities.h"

using namespace std;

namespace
{

const char byte_suffix = 'B';
const char scale_prefix[ ] = { 'K', 'M', 'G', 'T' };

enum byte_scale
{
   e_byte_scale_none,
   e_byte_scale_kilo,
   e_byte_scale_mega,
   e_byte_scale_giga,
   e_byte_scale_tera
};

const double c_kilobytes = 1024.0l;
const double c_megabytes = c_kilobytes * 1024.0l;
const double c_gigabytes = c_megabytes * 1024.0l;
const double c_terabytes = c_gigabytes * 1024.0l;

byte_scale convert_bytes_to_scaled_size( int64_t num_bytes, double& scaled_size )
{
   byte_scale bs = e_byte_scale_none;
   double divisor = 1.0, unscaled_size( num_bytes );

   if( num_bytes >= c_terabytes )
   {
      bs = e_byte_scale_tera;
      divisor = c_terabytes;
   }
   else if( num_bytes >= c_gigabytes )
   {
      bs = e_byte_scale_giga;
      divisor = c_gigabytes;
   }
   else if( num_bytes >= c_megabytes )
   {
      bs = e_byte_scale_mega;
      divisor = c_megabytes;
   }
   else if( num_bytes >= c_kilobytes )
   {
      bs = e_byte_scale_kilo;
      divisor = c_kilobytes;
   }

   scaled_size = unscaled_size / divisor;

   return bs;
}

const bool convert( const double& d, int64_t& i64 )
{
   // double representation of max acceptable value for double to int64 conversion
   static int64_t iMax = INT64_C( 0x43DFFFFFFFFFFFFF );

   static const double dMax = *reinterpret_cast< double* >( &iMax );
   static const double dMin = -dMax;

   static const bool is_IEEE754 = numeric_limits< double >::is_specialized && numeric_limits< double >::is_iec559;

   if( !is_IEEE754 || d > dMax || d < dMin )
      return false;
   else
   {
      i64 = int64_t( d );
      return true;
   }
}

}

string format_bytes( int64_t size )
{
   ostringstream osstr;

   double ss;
   byte_scale bs = convert_bytes_to_scaled_size( size, ss );

   if( bs > e_byte_scale_none )
   {
      char sp = scale_prefix[ bs - 1 ];

      int old_precision = osstr.precision( );
      ios::fmtflags old_flags = osstr.flags( );

      osstr.precision( 1 );
      osstr.setf( ios::fixed, ios::floatfield );

      osstr << ss << ' ' << sp << byte_suffix;

      osstr.flags( old_flags );
      osstr.precision( old_precision );
   }
   else
      osstr << ss << ' ' << byte_suffix;

   return osstr.str( );
}

int64_t unformat_bytes( const string& size_string, int64_t default_val )
{
   int64_t retval( default_val );

   double val( 0.0l );
   string str( size_string );
   if( !str.empty( ) )
   {
      if( str[ str.size( ) - 1 ] == 'B' || str[ str.size( ) - 1 ] == 'b' )
         str.erase( str.size( ) - 1 );

      if( str.size( ) < 1 )
         throw runtime_error( "invalid size string '" + size_string + "'" );

      byte_scale s( e_byte_scale_none );
      switch( str[ str.size( ) - 1 ] )
      {
         case 'K':
         case 'k':
         s = e_byte_scale_kilo;
         break;

         case 'M':
         case 'm':
         s = e_byte_scale_mega;
         break;

         case 'G':
         case 'g':
         s = e_byte_scale_giga;
         break;

         case 'T':
         case 't':
         s = e_byte_scale_tera;
         break;
      }

      if( s != e_byte_scale_none )
         str.erase( str.size( ) - 1 );

      if( str.size( ) && str[ str.size( ) - 1 ] == ' ' )
         str.erase( str.size( ) - 1 );

      if( str.size( ) < 1 )
         throw runtime_error( "invalid size string '" + size_string + "'" );

      bool had_point = false;
      double divisor = 0.1l;
      for( size_t i = 0; i < str.size( ); i++ )
      {
         if( !had_point && str[ i ] == '.' )
         {
            had_point = true;
            continue;
         }

         if( str[ i ] < '0' || str[ i ] > '9' )
            throw runtime_error( "invalid size string '" + size_string + "'" );

         double next = ( double )( ( int )( str[ i ] - '0' ) );

         if( !had_point )
            val = val * 10.0l;
         else
         {
            next = next * divisor;
            divisor = divisor * 0.1l;
         }

         val += next;
      }

      switch( s )
      {
         case e_byte_scale_kilo:
         val *= c_kilobytes;
         break;

         case e_byte_scale_mega:
         val *= c_megabytes;
         break;

         case e_byte_scale_giga:
         val *= c_gigabytes;
         break;

         case e_byte_scale_tera:
         val *= c_terabytes;
         break;
      }

      if( !convert( val, retval ) )
         throw runtime_error( "unable to convert " + to_string( val ) + " to int64_t" );
   }

   return retval;
}

string format_duration( int value, bool include_seconds )
{
   string str;
   if( value >= seconds_per_day( ) )
   {
      int days = value / ( int )seconds_per_day( );
      str = to_string( days ) + "d";
      value -= days * ( int )seconds_per_day( );
   }

   if( value >= 3600 )
   {
      int hours = value / 3600;
      if( !str.empty( ) )
         str += " ";
      str += to_string( hours ) + "h";
      value -= hours * 3600;
   }

   if( value >= 60 || ( !include_seconds && str.empty( ) ) )
   {
      int minutes = value / 60;
      if( !str.empty( ) )
         str += " ";
      str += to_string( minutes ) + "m";

      if( !include_seconds )
         value = 0;
      else
         value -= minutes * 60;
   }

   if( str.empty( ) || value > 0 )
   {
      if( !str.empty( ) )
         str += " ";
      str += to_string( value ) + "s";
   }

   return str;
}

int unformat_duration( const string& value )
{
   int rc = 0;

   bool had_days = false;
   bool had_hours = false;
   bool had_weeks = false;
   bool had_minutes = false;
   bool had_seconds = false;

   vector< string > parts;
   if( !value.empty( ) )
   {
      split( value, parts, ' ' );
      for( int i = 0; i < parts.size( ); i++ )
      {
         string next_part( parts[ i ] );
         if( next_part.size( ) < 2 )
            throw runtime_error( "invalid duration '" + value + "'" );

         bool okay = true;
         switch( next_part[ next_part.size( ) - 1 ] )
         {
            case 'w':
            if( had_weeks || had_days || had_hours || had_minutes || had_seconds )
               okay = false;
            else
            {
               had_weeks = true;
               rc += atoi( next_part.c_str( ) ) * ( int )seconds_per_day( ) * 7;
            }
            break;

            case 'd':
            if( had_days || had_hours || had_minutes || had_seconds )
               okay = false;
            else
            {
               had_days = true;
               rc += atoi( next_part.c_str( ) ) * ( int )seconds_per_day( );
            }
            break;

            case 'h':
            if( had_hours || had_minutes || had_seconds )
               okay = false;
            else
            {
               had_hours = true;
               rc += atoi( next_part.c_str( ) ) * 3600;
            }
            break;

            case 'm':
            if( had_minutes || had_seconds )
               okay = false;
            else
            {
               had_minutes = true;
               rc += atoi( next_part.c_str( ) ) * 60;
            }
            break;

            case 's':
            if( had_seconds )
               okay = false;
            else
               rc += atoi( next_part.c_str( ) );
            break;

            default:
            okay = false;
         }

         if( !okay )
            throw runtime_error( "invalid duration '" + value + "'" );
      }
   }

   return rc;
}

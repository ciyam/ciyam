// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
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
#  include <algorithm>
#  include <stdexcept>
#endif

#include "format.h"

#include "sha256.h"
#include "date_time.h"
#include "utilities.h"

using namespace std;

namespace
{

const char c_byte_suffix = 'B';

const char* const c_iec_byte_suffix = "iB";
const char* const c_metric_byte_suffix = "B";

const char iec_scale_prefix[ ] = { 'K', 'M', 'G', 'T' };
const char metric_scale_prefix[ ] = { 'k', 'M', 'G', 'T' };

enum iec_byte_scale
{
   e_iec_byte_scale_none,
   e_iec_byte_scale_kibi,
   e_iec_byte_scale_mebi,
   e_iec_byte_scale_gibi,
   e_iec_byte_scale_tebi
};

enum metric_byte_scale
{
   e_metric_byte_scale_none,
   e_metric_byte_scale_kilo,
   e_metric_byte_scale_mega,
   e_metric_byte_scale_giga,
   e_metric_byte_scale_tera
};

const int c_default_format_precision = 1;

const double c_kibibytes = 1024.0l;
const double c_mebibytes = c_kibibytes * c_kibibytes;
const double c_gibibytes = c_mebibytes * c_kibibytes;
const double c_tebibytes = c_gibibytes * c_kibibytes;

const double c_kilobytes = 1000.0l;
const double c_megabytes = c_kilobytes * c_kilobytes;
const double c_gigabytes = c_megabytes * c_kilobytes;
const double c_terabytes = c_gigabytes * c_kilobytes;

iec_byte_scale convert_bytes_to_iec_scaled_size( int64_t num_bytes, double& scaled_size, int round_to_decimals = -1 )
{
   iec_byte_scale bs = e_iec_byte_scale_none;

   double divisor = 1.0, unscaled_size( num_bytes );

   if( num_bytes >= c_tebibytes )
   {
      bs = e_iec_byte_scale_tebi;
      divisor = c_tebibytes;
   }
   else if( num_bytes >= c_gibibytes )
   {
      bs = e_iec_byte_scale_gibi;
      divisor = c_gibibytes;
   }
   else if( num_bytes >= c_mebibytes )
   {
      bs = e_iec_byte_scale_mebi;
      divisor = c_mebibytes;
   }
   else if( num_bytes >= c_kibibytes )
   {
      bs = e_iec_byte_scale_kibi;
      divisor = c_kibibytes;
   }

   scaled_size = unscaled_size / divisor;

   if( round_to_decimals >= 0 )
      scaled_size = round( scaled_size, round_to_decimals );

   // NOTE: Due to rounding a second call might be necessary (to reduce 1000.0 to 1.0).
   if( ( round_to_decimals < 0 )
    || ( bs == e_iec_byte_scale_tebi ) || ( scaled_size < c_kibibytes ) )
      return bs;
   else
      return convert_bytes_to_iec_scaled_size( scaled_size * divisor, scaled_size, round_to_decimals );
}

metric_byte_scale convert_bytes_to_metric_scaled_size( int64_t num_bytes, double& scaled_size, int round_to_decimals = -1 )
{
   metric_byte_scale bs = e_metric_byte_scale_none;

   double divisor = 1.0, unscaled_size( num_bytes );

   if( num_bytes >= c_terabytes )
   {
      bs = e_metric_byte_scale_tera;
      divisor = c_terabytes;
   }
   else if( num_bytes >= c_gigabytes )
   {
      bs = e_metric_byte_scale_giga;
      divisor = c_gigabytes;
   }
   else if( num_bytes >= c_megabytes )
   {
      bs = e_metric_byte_scale_mega;
      divisor = c_megabytes;
   }
   else if( num_bytes >= c_kilobytes )
   {
      bs = e_metric_byte_scale_kilo;
      divisor = c_kilobytes;
   }

   scaled_size = unscaled_size / divisor;

   if( round_to_decimals >= 0 )
      scaled_size = round( scaled_size, round_to_decimals );

   // NOTE: Due to rounding a second call might be necessary (to reduce 1000.0 to 1.0).
   if( ( round_to_decimals < 0 )
    || ( bs == e_metric_byte_scale_tera ) || ( scaled_size < c_kilobytes ) )
      return bs;
   else
      return convert_bytes_to_metric_scaled_size( scaled_size * divisor, scaled_size, round_to_decimals );
}

const bool convert( const double& d, int64_t& i64 )
{
   // NOTE: This is the double representation of the maximum acceptable value for double to int64 conversion.
   static int64_t imax = INT64_C( 0x43DFFFFFFFFFFFFF );

   static const double dmax = *reinterpret_cast< double* >( &imax );
   static const double dmin = -dmax;

   static const bool is_IEEE754 = numeric_limits< double >::is_specialized && numeric_limits< double >::is_iec559;

   if( !is_IEEE754 || ( d > dmax ) || ( d < dmin ) )
      return false;
   else
   {
      i64 = int64_t( d );

      return true;
   }
}

}

string format_int( int64_t i, char separator, unsigned int grouping )
{
   ostringstream osstr;

   osstr << i;

   string str( osstr.str( ) );
   string retval;

   bool is_negative = false;

   if( !str.empty( ) && ( str[ 0 ] == '-' ) )
   {
      is_negative = true;

      str.erase( 0, 1 );
   }

   reverse( str.begin( ), str.end( ) );

   for( size_t i = 0; i < str.size( ); i++ )
   {
      if( i && ( i % grouping == 0 ) )
         retval += separator;

      retval += str[ i ];
   }

   reverse( retval.begin( ), retval.end( ) );

   if( is_negative )
      retval = '-' + retval;

   return retval;
}

string format_bytes( int64_t size, bool use_iec, int decimal_precision, char separator )
{
   ostringstream osstr;

   double ss;

   string sep;

   if( separator != '\0' )
      sep += separator;

   int precision = ( decimal_precision < 0 ) ? c_default_format_precision : decimal_precision;

   iec_byte_scale ibs = !use_iec ? e_iec_byte_scale_none
    : convert_bytes_to_iec_scaled_size( size, ss, precision );

   metric_byte_scale mbs = use_iec ? e_metric_byte_scale_none
    : convert_bytes_to_metric_scaled_size( size, ss, precision );

   string suffix( use_iec ? c_iec_byte_suffix : c_metric_byte_suffix );

   if( ( ibs > e_iec_byte_scale_none ) || ( mbs > e_metric_byte_scale_none ) )
   {
      char sp = use_iec ? iec_scale_prefix[ ibs - 1 ] : metric_scale_prefix[ mbs - 1 ];

      int old_precision = osstr.precision( );
      ios::fmtflags old_flags = osstr.flags( );

      osstr.precision( precision );
      osstr.setf( ios::fixed, ios::floatfield );

      osstr << ss << sep << sp << suffix;

      osstr.flags( old_flags );
      osstr.precision( old_precision );
   }
   else
      osstr << ss << sep << c_byte_suffix;

   return osstr.str( );
}

int64_t unformat_bytes( const string& size_string, int64_t default_val, bool use_iec_always )
{
   int64_t retval( default_val );

   double val( 0.0l );
   string str( size_string );

   if( !str.empty( ) )
   {
      if( ( str[ str.size( ) - 1 ] == 'B' )
       || ( str[ str.size( ) - 1 ] == 'b' ) )
         str.erase( str.size( ) - 1 );

      if( str.size( ) < 1 )
         throw runtime_error( "invalid size string '" + size_string + "'" );

      iec_byte_scale is( e_iec_byte_scale_none );

      metric_byte_scale ms( e_metric_byte_scale_none );

      if( use_iec_always || ( str[ str.size( ) - 1 ] == 'i' ) )
      {
         if( str[ str.size( ) - 1 ] == 'i' )
         {
            str.erase( str.size( ) - 1 );

            if( str.size( ) < 1 )
               throw runtime_error( "invalid size string '" + size_string + "'" );
         }

         switch( str[ str.size( ) - 1 ] )
         {
            case 'K':
            case 'k':
            is = e_iec_byte_scale_kibi;
            break;

            case 'M':
            case 'm':
            is = e_iec_byte_scale_mebi;
            break;

            case 'G':
            case 'g':
            is = e_iec_byte_scale_gibi;
            break;

            case 'T':
            case 't':
            is = e_iec_byte_scale_tebi;
            break;
         }
      }
      else
      {
         switch( str[ str.size( ) - 1 ] )
         {
            case 'K':
            case 'k':
            ms = e_metric_byte_scale_kilo;
            break;

            case 'M':
            case 'm':
            ms = e_metric_byte_scale_mega;
            break;

            case 'G':
            case 'g':
            ms = e_metric_byte_scale_giga;
            break;

            case 'T':
            case 't':
            ms = e_metric_byte_scale_tera;
            break;
         }
      }

      if( ( is != e_iec_byte_scale_none )
       || ( ms != e_metric_byte_scale_none ) )
         str.erase( str.size( ) - 1 );

      if( str.size( ) && ( str[ str.size( ) - 1 ] == ' ' ) )
         str.erase( str.size( ) - 1 );

      if( str.size( ) < 1 )
         throw runtime_error( "invalid size string '" + size_string + "'" );

      double divisor = 0.1l;

      bool had_point = false;

      for( size_t i = 0; i < str.size( ); i++ )
      {
         if( !had_point && ( str[ i ] == '.' ) )
         {
            had_point = true;
            continue;
         }

         if( ( str[ i ] < '0' ) || ( str[ i ] > '9' ) )
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

      if( is != e_iec_byte_scale_none )
      {
         switch( is )
         {
            case e_iec_byte_scale_kibi:
            val *= c_kibibytes;
            break;

            case e_iec_byte_scale_mebi:
            val *= c_mebibytes;
            break;

            case e_iec_byte_scale_gibi:
            val *= c_gibibytes;
            break;

            case e_iec_byte_scale_tebi:
            val *= c_tebibytes;
            break;
         }
      }

      if( ms != e_metric_byte_scale_none )
      {
         switch( ms )
         {
            case e_metric_byte_scale_kilo:
            val *= c_kilobytes;
            break;

            case e_metric_byte_scale_mega:
            val *= c_megabytes;
            break;

            case e_metric_byte_scale_giga:
            val *= c_gigabytes;
            break;

            case e_metric_byte_scale_tera:
            val *= c_terabytes;
            break;
         }
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

      value -= ( hours * 3600 );
   }

   if( ( value >= 60 ) || ( !include_seconds && str.empty( ) ) )
   {
      int minutes = value / 60;

      if( !str.empty( ) )
         str += " ";

      str += to_string( minutes ) + "m";

      if( !include_seconds )
         value = 0;
      else
         value -= ( minutes * 60 );
   }

   if( str.empty( ) || ( value > 0 ) )
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

void split_list_items(
 const string& list_data, vector< string >& list_items,
 vector< string >* p_secondary_values, bool* p_secondary_prefixed )
{
   size_t separator_offset = ( c_sha256_digest_size * 2 );
   size_t minimum_item_size = ( c_sha256_digest_size * 2 ) + 2;

   bool had_secondary = false;

   string remaining( list_data );

   while( !remaining.empty( ) )
   {
      string prefix, next_item, next_secondary;

      bool has_secondary = false;

      string::size_type pos = string::npos;

      if( remaining.size( ) >= minimum_item_size )
      {
         if( remaining[ separator_offset ] == ':' )
            has_secondary = true;
         else if( remaining[ separator_offset ] == '@' )
         {
            prefix = "02";
            has_secondary = true;
         }
         else if( remaining[ separator_offset ] == '#' )
         {
            prefix = "03";
            has_secondary = true;
         }

         if( p_secondary_prefixed )
            *p_secondary_prefixed = !prefix.empty( );

         pos = remaining.find( '\n', minimum_item_size );
      }

      if( !has_secondary )
         next_item = remaining.substr( 0, pos );
      else
      {
         had_secondary = true;

         next_item = hex_encode( remaining.substr( 0, c_sha256_digest_size ) );

         if( p_secondary_values )
            next_secondary = hex_encode( remaining.substr( c_sha256_digest_size, c_sha256_digest_size ) );

         if( pos == string::npos )
            next_item += ' ' + remaining.substr( separator_offset + 1 );
         else
            next_item += ' ' + remaining.substr( separator_offset + 1, pos - ( separator_offset + 1 ) );
      }

      list_items.push_back( next_item );

      if( p_secondary_values )
         p_secondary_values->push_back( prefix + next_secondary );

      if( pos == string::npos )
         break;

      remaining.erase( 0, pos + 1 );
   }

   if( !had_secondary && p_secondary_values )
      p_secondary_values->clear( );
}

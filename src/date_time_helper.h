// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef DATE_TIME_HELPER_H
#  define DATE_TIME_HELPER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <sstream>
#     include <iostream>
#  endif

#  include "format.h"
#  include "utilities.h"
#  include "date_time.h"
#  include "class_utilities.h"

REGISTER_TYPEOF( 30, date_time )

inline size_t size_determiner( const date_time* ) { return sizeof( date_time ); }

inline std::string to_string( const julian& j )
{
   std::ostringstream oss;
   oss << ffmt( 6, 6 ) << j;

   return oss.str( );
}

inline std::string to_string( const date_time& dt )
{
   return dt.as_string( true );
}

inline std::string to_rep_string( const date_time& dt )
{
   return dt.as_string( false );
}

template< > inline date_time from_string< date_time >( const std::string& s )
{
   if( s.empty( ) )
      return date_time( );
   else
   {
      date_time dt( s );
      return dt;
   }
}

template< > inline std::string to_formatted_string< date_time >( const date_time& dt, const char* p_fmt )
{
   ( void )p_fmt;

   return utc_to_local( dt ).as_string( e_time_format_hhmm, true );
}

template< > inline bool is_valid_str_val< date_time >( const std::string& s ) { return is_valid_date_time( s ); }

#endif

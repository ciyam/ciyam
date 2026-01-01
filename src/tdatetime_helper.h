// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef TDATETIME_HELPER_H
#  define TDATETIME_HELPER_H

#  include "numeric.h"
#  include "nullable.h"
#  include "date_time_helper.h"
#  include "read_write_stream.h"

typedef nullable< date_time > nullable_date_time;

class tdatetime : public nullable_date_time
{
   public:
   tdatetime( ) { set_null( ); }

   tdatetime( const date_time& dt ) : nullable_date_time( dt ) { }

   tdatetime( julian jdt ) : nullable_date_time( date_time( jdt ) ) { }

   tdatetime( const numeric& n ) : nullable_date_time( date_time( n.as_int64_t( ) ) ) { }

   tdatetime( const std::string& s ) : nullable_date_time( s.empty( ) ? date_time( ) : date_time( s ) )
   {
      if( s.empty( ) )
         set_null( );
   }

   tdatetime( year yr, month mo, day_type dt, occurrence occ = e_occurrence_first )
    : nullable_date_time( date_time( yr, mo, dt, occ ) )
   {
   }

   tdatetime( year yr, month mo, weekday wd, occurrence occ = e_occurrence_first )
    : nullable_date_time( date_time( yr, mo, wd, occ ) )
   {
   }

   tdatetime( year yr, month mo, day dy ) : nullable_date_time( date_time( yr, mo, dy ) ) { }
   tdatetime( year yr, month mo, day dy, const mtime& mt ) : nullable_date_time( date_time( yr, mo, dy, mt ) ) { }
   tdatetime( year yr, month mo, day dy, hour hr, minute mn ) : nullable_date_time( date_time( yr, mo, dy, hr, mn ) ) { }

   tdatetime( year yr, month mo, day dy, hour hr, minute mn, second sc )
    : nullable_date_time( date_time( yr, mo, dy, hr, mn, sc ) )
   {
   }

   tdatetime( year yr, month mo, day dy, hour hr, minute mn, second sc, millisecond m )
    : nullable_date_time( date_time( yr, mo, dy, hr, mn, sc, m ) )
   {
   }

   tdatetime( year yr,
    month mo, day dy, hour hr, minute mn, second sc, tenth te, hundredth hd = 0, thousandth th = 0 )
    : nullable_date_time( date_time( yr, mo, dy, hr, mn, sc, te, hd, th ) )
   {
   }

   tdatetime( year yr, day_of_significance dos ) : nullable_date_time( date_time( yr, dos ) ) { }

   tdatetime( year yr, month mo, day dy, time_of_significance tos, const locality& l )
    : nullable_date_time( date_time( yr, mo, dy, tos, l ) )
   {
   }

   tdatetime( year yr, day_of_significance dos, time_of_significance tos, const locality& l )
    : nullable_date_time( date_time( yr, dos, tos, l ) )
   {
   }

   tdatetime( year yr, month mo, day dy, hour hr, minute mn, seconds s )
    : nullable_date_time( date_time( yr, mo, dy, hr, mn, s ) )
   {
   }

   tdatetime& operator =( const date_time& dtm )
   {
      nullable_date_time::operator =( dtm );
      return *this;
   }

   year get_year( ) const { return static_cast< const date_time& >( *this ).get_year( ); }
   month get_month( ) const { return static_cast< const date_time& >( *this ).get_month( ); }
   udate get_date( ) const { return static_cast< const date_time& >( *this ).get_date( ); }

   days get_day_of_year( ) const { return static_cast< const date_time& >( *this ).get_day_of_year( ); }

   hour get_hour( ) const { return static_cast< const date_time& >( *this ).get_hour( ); }
   minute get_minute( ) const { return static_cast< const date_time& >( *this ).get_minute( ); }
   second get_second( ) const { return static_cast< const date_time& >( *this ).get_second( ); }
   millisecond get_millisecond( ) const { return static_cast< const date_time& >( *this ).get_millisecond( ); }

   std::string as_string( bool use_separators = false, bool include_milliseconds = true ) const
   {
      if( is_null( ) )
         return std::string( );
      else
         return static_cast< const date_time& >( *this ).as_string( use_separators, include_milliseconds );
   }

   days days_in_year( ) const { return static_cast< const date_time& >( *this ).days_in_year( ); }

   static tdatetime local( ) { return date_time::local( ); }
   static tdatetime standard( ) { return date_time::standard( ); }
};

inline bool is_null( const tdatetime& tdt ) { return tdt.is_null( ); }

inline size_t size_determiner( const tdatetime* ) { return sizeof( bool ) + sizeof( date_time ); }

inline std::string to_string( const tdatetime& tdt )
{
   std::string s;
   if( !tdt.is_null( ) )
      s = to_string( *tdt );
   return s;
}

inline std::string to_rep_string( const tdatetime& tdt )
{
   std::string s;
   if( !tdt.is_null( ) )
      s = to_rep_string( *tdt );
   return s;
}

template< > inline tdatetime from_string< tdatetime >( const std::string& s )
{
   tdatetime tdt;
   if( !s.empty( ) )
      tdt = from_string< date_time >( s );
   return tdt;
}

template< > inline std::string to_formatted_string< tdatetime >( const tdatetime& tdt, const char* p_fmt )
{
   ( void )p_fmt;

   std::string s;
   if( !tdt.is_null( ) )
      s = to_formatted_string( *tdt, p_fmt );

   return s;
}

inline bool operator <( const tdatetime& lhs, const date_time& rhs )
{
   if( lhs.is_null( ) )
      return true;

   return static_cast< const nullable_date_time& >( lhs ) < rhs;
}

inline bool operator <( const date_time& lhs, const tdatetime& rhs )
{
   if( rhs.is_null( ) )
      return false;

   return lhs < static_cast< const nullable_date_time& >( rhs );
}

inline bool operator <( const tdatetime& lhs, const tdatetime& rhs )
{
   if( lhs.is_null( ) && !rhs.is_null( ) )
      return true;

   if( rhs.is_null( ) )
      return false;

   return static_cast< const nullable_date_time& >( lhs ) < static_cast< const nullable_date_time& >( rhs );
}

inline read_stream& operator >>( read_stream& rs, tdatetime& tdt )
{
   bool is_null;
   date_time dt;

   rs >> is_null >> dt;

   if( is_null )
      tdt.set_null( );
   else
      tdt = dt;

   return rs;
}

inline write_stream& operator <<( write_stream& ws, const tdatetime& tdt )
{
   if( tdt.is_null( ) )
      ws << true << date_time( );
   else
      ws << false << *tdt;

   return ws;
}

#endif

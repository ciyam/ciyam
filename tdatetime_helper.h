// Copyright (c) 2006
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifndef TDATETIME_HELPER_H
#  define TDATETIME_HELPER_H

#  include "nullable.h"
#  include "date_time_helper.h"
#  include "read_write_stream.h"

typedef nullable< date_time > nullable_date_time;

class tdatetime : public nullable_date_time
{
   public:
   tdatetime( ) { }

   tdatetime( const date_time& dt ) : nullable_date_time( dt ) { }

   tdatetime( julian jdt ) : nullable_date_time( date_time( jdt ) ) { }

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

   tdatetime( year yr, day_of_significance dos ) : nullable_date_time( date_time( dos ) ) { }

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
   udate get_date( ) const { return static_cast< const date_time& >( *this ).get_date( ); }

   std::string as_string( bool use_separators = false, bool include_milliseconds = true ) const
   {
      return static_cast< const date_time& >( *this ).as_string( use_separators, include_milliseconds );
   }

   days get_day_of_year( ) const { return static_cast< const date_time& >( *this ).get_day_of_year( ); }

   days days_in_year( ) const { return static_cast< const date_time& >( *this ).days_in_year( ); }

   static tdatetime local( ) { return date_time::local( ); }
   static tdatetime standard( ) { return date_time::standard( ); }
};

inline bool is_null( const tdatetime& tdt ) { return tdt.is_null( ); }

inline size_t size_determiner( const tdatetime* ) { return sizeof( bool ) + sizeof( date_time ); }

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif
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
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

template< > inline tdatetime from_string< tdatetime >( const std::string& s )
{
   tdatetime tdt;
   if( !s.empty( ) )
      tdt = from_string< date_time >( s );
   return tdt;
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


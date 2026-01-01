// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef TDATE_HELPER_H
#  define TDATE_HELPER_H

#  include "nullable.h"
#  include "udate_helper.h"
#  include "read_write_stream.h"

typedef nullable< udate > nullable_udate;

class tdate : public nullable_udate
{
   public:
   tdate( ) { set_null( ); }

   tdate( const udate& ud ) : nullable_udate( ud ) { }

   tdate( const std::string& s ) : nullable_udate( s.empty( ) ? udate( ) : udate( s ) )
   {
      if( s.empty( ) )
         set_null( );
   }

   tdate( year yr, month mo, weekday wd, occurrence occ = e_occurrence_first )
    : nullable_udate( udate( yr, mo, wd, occ ) )
   {
   }

   tdate( year yr, month mo, day_type dt, occurrence occ = e_occurrence_first )
    : nullable_udate( udate( yr, mo, dt, occ ) )
   {
   }

   tdate( year yr, month mo, day dy ) : nullable_udate( udate( yr, mo, dy ) ) { }

   tdate( year yr, day_of_significance dos ) : nullable_udate( udate( yr, dos ) ) { }

   tdate& operator =( const udate& ud )
   {
      nullable_udate::operator =( ud );
      return *this;
   }

   year get_year( ) const { return static_cast< const udate& >( *this ).get_year( ); }
   month get_month( ) const { return static_cast< const udate& >( *this ).get_month( ); }
   day get_day( ) const { return static_cast< const udate& >( *this ).get_day( ); }

   std::string as_string( bool use_separators = false ) const
   {
      if( is_null( ) )
         return std::string( );
      else
         return static_cast< const udate& >( *this ).as_string( use_separators );
   }

   days get_day_of_year( ) const { return static_cast< const udate& >( *this ).get_day_of_year( ); }

   days days_in_year( ) const { return static_cast< const udate& >( *this ).days_in_year( ); }

   static tdate local( ) { return udate::local( ); }
   static tdate standard( ) { return udate::standard( ); }
};

inline bool is_null( const tdate& td ) { return td.is_null( ); }

inline size_t size_determiner( const tdate* ) { return sizeof( bool ) + sizeof( udate ); }

inline std::string to_string( const tdate& td )
{
   std::string s;
   if( !td.is_null( ) )
      s = to_string( *td );
   return s;
}

template< > inline tdate from_string< tdate >( const std::string& s )
{
   tdate td;
   if( !s.empty( ) )
      td = from_string< udate >( s );
   return td;
}

inline bool operator <( const tdate& lhs, const udate& rhs )
{
   if( lhs.is_null( ) )
      return false;

   return static_cast< const nullable_udate& >( lhs ) < rhs;
}

inline bool operator <( const udate& lhs, const tdate& rhs )
{
   if( rhs.is_null( ) )
      return false;

   return lhs < static_cast< const nullable_udate& >( rhs );
}

inline bool operator <( const tdate& lhs, const tdate& rhs )
{
   if( lhs.is_null( ) && !rhs.is_null( ) )
      return true;

   if( rhs.is_null( ) )
      return false;

   return static_cast< const nullable_udate& >( lhs ) < static_cast< const nullable_udate& >( rhs );
}

inline read_stream& operator >>( read_stream& rs, tdate& td )
{
   bool is_null;
   udate ud;

   rs >> is_null >> ud;

   if( is_null )
      td.set_null( );
   else
      td = ud;

   return rs;
}

inline write_stream& operator <<( write_stream& ws, const tdate& td )
{
   if( td.is_null( ) )
      ws << true << udate( );
   else
      ws << false << *td;

   return ws;
}

#endif

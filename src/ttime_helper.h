// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef TTIME_HELPER_H
#  define TTIME_HELPER_H

#  include "nullable.h"
#  include "mtime_helper.h"
#  include "read_write_stream.h"

typedef nullable< mtime > nullable_mtime;

class ttime : public nullable_mtime
{
   public:
   ttime( ) { set_null( ); }

   ttime( const mtime& mt ) : nullable_mtime( mt ) { }

   ttime( const std::string& s ) : nullable_mtime( s.empty( ) ? mtime( ) : mtime( s ) )
   {
      if( s.empty( ) )
         set_null( );
   }

   ttime( hour hr, minute mn )
    : nullable_mtime( mtime( hr, mn ) )
   {
   }

   ttime( hour hr, minute mn, second sc )
    : nullable_mtime( mtime( hr, mn, sc ) )
   {
   }

   ttime& operator =( const mtime& mt )
   {
      nullable_mtime::operator =( mt );
      return *this;
   }

   hour get_hour( ) const { return static_cast< const mtime& >( *this ).get_hour( ); }
   minute get_minute( ) const { return static_cast< const mtime& >( *this ).get_minute( ); }
   second get_second( ) const { return static_cast< const mtime& >( *this ).get_second( ); }

   std::string as_string( bool use_separators = false, bool include_milliseconds = true ) const
   {
      if( is_null( ) )
         return std::string( );
      else
         return static_cast< const mtime& >( *this ).as_string( use_separators, include_milliseconds );
   }

   static mtime local( ) { return mtime::local( ); }
   static mtime standard( ) { return mtime::standard( ); }
};

inline bool is_null( const ttime& tt ) { return tt.is_null( ); }

inline size_t size_determiner( const ttime* ) { return sizeof( bool ) + sizeof( mtime ); }

inline std::string to_string( const ttime& tt )
{
   std::string s;
   if( !tt.is_null( ) )
      s = to_string( *tt );
   return s;
}

template< > inline ttime from_string< ttime >( const std::string& s )
{
   ttime tt;
   if( !s.empty( ) )
      tt = from_string< mtime >( s );
   return tt;
}

inline bool operator <( const ttime& lhs, const mtime& rhs )
{
   if( lhs.is_null( ) )
      return false;

   return static_cast< const nullable_mtime& >( lhs ) < rhs;
}

inline bool operator <( const mtime& lhs, const ttime& rhs )
{
   if( rhs.is_null( ) )
      return false;

   return lhs < static_cast< const nullable_mtime& >( rhs );
}

inline bool operator <( const ttime& lhs, const ttime& rhs )
{
   if( lhs.is_null( ) && !rhs.is_null( ) )
      return true;

   if( rhs.is_null( ) )
      return false;

   return static_cast< const nullable_mtime& >( lhs ) < static_cast< const nullable_mtime& >( rhs );
}

inline read_stream& operator >>( read_stream& rs, ttime& tt )
{
   bool is_null;
   mtime mt;

   rs >> is_null >> mt;

   if( is_null )
      tt.set_null( );
   else
      tt = mt;

   return rs;
}

inline write_stream& operator <<( write_stream& ws, const ttime& tt )
{
   if( tt.is_null( ) )
      ws << true << mtime( );
   else
      ws << false << *tt;

   return ws;
}

#endif

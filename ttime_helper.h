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

#ifndef TTIME_HELPER_H
#  define TTIME_HELPER_H

#  include "nullable.h"
#  include "mtime_helper.h"
#  include "read_write_stream.h"

typedef nullable< mtime > nullable_mtime;

class ttime : public nullable_mtime
{
   public:
   ttime( ) { }

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
      return static_cast< const mtime& >( *this ).as_string( use_separators, include_milliseconds );
   }

   static mtime local( ) { return mtime::local( ); }
   static mtime standard( ) { return mtime::standard( ); }
};

inline bool is_null( const ttime& tt ) { return tt.is_null( ); }

inline size_t size_determiner( const ttime* ) { return sizeof( bool ) + sizeof( mtime ); }

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif
inline std::string to_string( const ttime& tt )
{
   std::string s;
   if( !tt.is_null( ) )
      s = to_string( *tt );
   return s;
}
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

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


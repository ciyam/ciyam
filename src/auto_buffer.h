// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef AUTO_BUFFER_H
#  define AUTO_BUFFER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <memory.h>
#     include <stddef.h>
#     include <stdexcept>
#  endif

template< typename T > class auto_buffer
{
#  ifdef _DEBUG
   void check_range( size_t i ) const
   {
      if( i >= buf_size )
         throw std::range_error( "auto_buffer: index out of range" );
   }
#  endif

   void copy( const auto_buffer& src )
   {
      T* p_new_buf( new T[ src.buf_size ] );
      memcpy( p_new_buf, src.p_buf, src.buf_size );

      delete[ ] p_buf;

      p_buf = p_new_buf;
      buf_size = src.buf_size;
   }

   public:
   auto_buffer( )
    :
    buf_size( 0 ),
    p_buf( 0 )
   {
   }

   explicit auto_buffer( size_t buf_size )
    :
    buf_size( buf_size ),
    p_buf( ( buf_size == 0 ) ? 0 : new T[ buf_size ] )
   {
   }

   auto_buffer( const auto_buffer& other )
    :
    p_buf( 0 )
   {
      copy( other );
   }

   auto_buffer& operator =( const auto_buffer& other )
   {
      copy( other );
      return *this;
   }

   ~auto_buffer( ) { delete[ ] p_buf; }

   size_t size( ) const { return buf_size; }

   operator T*( ) { return p_buf; }
   operator const T*( ) const { return p_buf; }

   T* get( ) { return p_buf; }
   const T* get( ) const { return p_buf; }

#  ifndef _DEBUG
   T& operator [ ]( size_t i ) { return p_buf[ i ]; }
   const T& operator [ ]( size_t i ) const { return p_buf[ i ]; }
#  else
   T& operator [ ]( size_t i ) { check_range( i ); return p_buf[ i ]; }
   const T& operator [ ]( size_t i ) const { check_range( i ); return p_buf[ i ]; }
#  endif

   private:
   size_t buf_size;

   T* p_buf;
};

#  ifdef _DEBUG
namespace std
{

template< typename T > void* memset( auto_buffer< T >& dest, int c, size_t len )
{
   if( len > dest.size( ) )
      throw std::length_error( "memset: length exceeds buffer size" );
   return std::memset( dest.get( ), c, len );
}

template< typename T > void* memcpy( auto_buffer< T >& dest, const void* src, size_t len )
{
   if( len > dest.size( ) )
      throw std::length_error( "memcpy: length exceeds buffer size" );
   return std::memcpy( dest.get( ), src, len );
}

}
#  endif

#endif


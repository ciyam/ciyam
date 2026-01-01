// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef READ_WRITE_STREAM_H
#  define READ_WRITE_STREAM_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <set>
#     include <map>
#     include <list>
#     include <deque>
#     include <vector>
#     include <string>
#     include <utility>
#  endif

#  include "ptypes.h"

#  define USE_FAST_STRING_READ

enum read_write_type
{
   e_read_write_type_none,
   e_read_write_type_bool,
   e_read_write_type_char,
   e_read_write_type_schar,
   e_read_write_type_uchar,
   e_read_write_type_wchar,
   e_read_write_type_short,
   e_read_write_type_ushort,
   e_read_write_type_int,
   e_read_write_type_uint,
   e_read_write_type_long,
   e_read_write_type_ulong,
   e_read_write_type_long_long,
   e_read_write_type_ulong_long,
   e_read_write_type_float,
   e_read_write_type_double,
   e_read_write_type_ldouble
};

class read_stream
{
   public:
   virtual void read( unsigned char* p_buf, size_t len, read_write_type type = e_read_write_type_none ) = 0;

   virtual void read_meta( std::string& ) { }

   read_stream& operator >>( bool& val )
   {
      read( ( unsigned char* )&val, sizeof( bool ), e_read_write_type_bool );
      return *this;
   }

   read_stream& operator >>( char& val )
   {
      read( ( unsigned char* )&val, sizeof( char ), e_read_write_type_char );
      return *this;
   }

   read_stream& operator >>( signed char& val )
   {
      read( ( unsigned char* )&val, sizeof( signed char ), e_read_write_type_schar );
      return *this;
   }

   read_stream& operator >>( unsigned char& val )
   {
      read( ( unsigned char* )&val, sizeof( unsigned char ), e_read_write_type_uchar );
      return *this;
   }

   read_stream& operator >>( wchar_t& val )
   {
      read( ( unsigned char* )&val, sizeof( wchar_t ), e_read_write_type_wchar );
      return *this;
   }

   read_stream& operator >>( short& val )
   {
      read( ( unsigned char* )&val, sizeof( short ), e_read_write_type_short );
      return *this;
   }

   read_stream& operator >>( unsigned short& val )
   {
      read( ( unsigned char* )&val, sizeof( unsigned short ), e_read_write_type_ushort );
      return *this;
   }

   read_stream& operator >>( int& val )
   {
      read( ( unsigned char* )&val, sizeof( int ), e_read_write_type_int );
      return *this;
   }

   read_stream& operator >>( unsigned int& val )
   {
      read( ( unsigned char* )&val, sizeof( unsigned int ), e_read_write_type_uint );
      return *this;
   }

   read_stream& operator >>( long& val )
   {
      read( ( unsigned char* )&val, sizeof( long ), e_read_write_type_long );
      return *this;
   }

   read_stream& operator >>( unsigned long& val )
   {
      read( ( unsigned char* )&val, sizeof( unsigned long ), e_read_write_type_ulong );
      return *this;
   }

   read_stream& operator >>( long_long& val )
   {
      read( ( unsigned char* )&val, sizeof( long_long ), e_read_write_type_long_long );
      return *this;
   }

   read_stream& operator >>( unsigned_long_long& val )
   {
      read( ( unsigned char* )&val, sizeof( unsigned_long_long ), e_read_write_type_ulong_long );
      return *this;
   }

   read_stream& operator >>( float& val )
   {
      read( ( unsigned char* )&val, sizeof( float ), e_read_write_type_float );
      return *this;
   }

   read_stream& operator >>( double& val )
   {
      read( ( unsigned char* )&val, sizeof( double ), e_read_write_type_double );
      return *this;
   }

   read_stream& operator >>( long double& val )
   {
      read( ( unsigned char* )&val, sizeof( long double ), e_read_write_type_ldouble );
      return *this;
   }
};

class write_stream
{
   public:
   virtual void write( const unsigned char* p_buf, size_t len, read_write_type type = e_read_write_type_none ) = 0;

   virtual void write_meta( std::string& ) { }

   write_stream& operator <<( bool val )
   {
      write( ( const unsigned char* )&val, sizeof( bool ), e_read_write_type_bool );
      return *this;
   }

   write_stream& operator <<( char val )
   {
      write( ( const unsigned char* )&val, sizeof( char ), e_read_write_type_char );
      return *this;
   }

   write_stream& operator <<( signed char val )
   {
      write( ( const unsigned char* )&val, sizeof( signed char ), e_read_write_type_schar );
      return *this;
   }

   write_stream& operator <<( unsigned char val )
   {
      write( ( const unsigned char* )&val, sizeof( unsigned char ), e_read_write_type_uchar );
      return *this;
   }

   write_stream& operator <<( wchar_t val )
   {
      write( ( const unsigned char* )&val, sizeof( wchar_t ), e_read_write_type_wchar );
      return *this;
   }

   write_stream& operator <<( short val )
   {
      write( ( const unsigned char* )&val, sizeof( short ), e_read_write_type_short );
      return *this;
   }

   write_stream& operator <<( unsigned short val )
   {
      write( ( const unsigned char* )&val, sizeof( unsigned short ), e_read_write_type_ushort );
      return *this;
   }

   write_stream& operator <<( int val )
   {
      write( ( const unsigned char* )&val, sizeof( int ), e_read_write_type_int );
      return *this;
   }

   write_stream& operator <<( unsigned int val )
   {
      write( ( const unsigned char* )&val, sizeof( unsigned int ), e_read_write_type_uint );
      return *this;
   }

   write_stream& operator <<( long val )
   {
      write( ( const unsigned char* )&val, sizeof( long ), e_read_write_type_long );
      return *this;
   }

   write_stream& operator <<( unsigned long val )
   {
      write( ( const unsigned char* )&val, sizeof( unsigned long ), e_read_write_type_ulong );
      return *this;
   }

   write_stream& operator <<( long_long val )
   {
      write( ( const unsigned char* )&val, sizeof( long_long ), e_read_write_type_long_long );
      return *this;
   }

   write_stream& operator <<( unsigned_long_long val )
   {
      write( ( const unsigned char* )&val, sizeof( unsigned_long_long ), e_read_write_type_ulong_long );
      return *this;
   }

   write_stream& operator <<( float val )
   {
      write( ( const unsigned char* )&val, sizeof( float ), e_read_write_type_float );
      return *this;
   }

   write_stream& operator <<( double val )
   {
      write( ( const unsigned char* )&val, sizeof( double ), e_read_write_type_double );
      return *this;
   }

   write_stream& operator <<( long double val )
   {
      write( ( const unsigned char* )&val, sizeof( long double ), e_read_write_type_ldouble );
      return *this;
   }
};

class read_write_stream : public read_stream, public write_stream
{
};

template< typename T1, typename T2 > read_stream& operator >>( read_stream& rs, std::pair< T1, T2 >& val )
{
   rs >> val.first >> val.second;
   return rs;
}

template< typename T, class A > read_stream& operator >>( read_stream& rs, std::list< T, A >& ctr )
{
   typename std::list< T, A >::size_type size;
   rs >> size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy;
   rs >> dummy;
#  endif

   ctr.clear( );

   typename std::list< T, A >::value_type v;
   for( typename std::list< T, A >::size_type i = 0; i < size; i++ )
   {
      rs >> v;
      ctr.push_back( v );
   }

   return rs;
}

template< typename T, class A > read_stream& operator >>( read_stream& rs, std::deque< T, A >& ctr )
{
   typename std::deque< T, A >::size_type size;
   rs >> size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy;
   rs >> dummy;
#  endif

   ctr.clear( );

   typename std::deque< T, A >::value_type v;
   for( typename std::deque< T, A >::size_type i = 0; i < size; i++ )
   {
      rs >> v;
      ctr.push_back( v );
   }

   return rs;
}

template< typename T, class A > read_stream& operator >>( read_stream& rs, std::vector< T, A >& ctr )
{
   typename std::vector< T, A >::size_type size;
   rs >> size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy;
   rs >> dummy;
#  endif

   ctr.clear( );
   ctr.reserve( size );

   typename std::vector< T, A >::value_type v;
   for( typename std::vector< T, A >::size_type i = 0; i < size; i++ )
   {
      rs >> v;
      ctr.push_back( v );
   }

   return rs;
}

template< typename K, class C, class A > read_stream& operator >>( read_stream& rs, std::set< K, C, A >& ctr )
{
   typename std::set< K, C, A >::size_type size;
   rs >> size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy;
   rs >> dummy;
#  endif

   ctr.clear( );

   typename std::set< K, C, A >::value_type v;
   for( typename std::set< K, C, A >::size_type i = 0; i < size; i++ )
   {
      rs >> v;
      ctr.insert( v );
   }

   return rs;
}

template< typename K, class C, class A > read_stream& operator >>( read_stream& rs, std::multiset< K, C, A >& ctr )
{
   typename std::multiset< K, C, A >::size_type size;
   rs >> size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy;
   rs >> dummy;
#  endif

   ctr.clear( );

   typename std::multiset< K, C, A >::value_type v;
   for( typename std::multiset< K, C, A >::size_type i = 0; i < size; i++ )
   {
      rs >> v;
      ctr.insert( v );
   }

   return rs;
}

template< typename K, typename T, class C, class A > read_stream& operator >>( read_stream& rs, std::map< K, T, C, A >& ctr )
{
   typename std::map< K, T, C, A >::size_type size;
   rs >> size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy;
   rs >> dummy;
#  endif

   ctr.clear( );

   std::pair< typename std::map< K, T, C, A >::key_type, typename std::map< K, T, C, A >::mapped_type > v;
   for( typename std::map< K, T, C, A >::size_type i = 0; i < size; i++ )
   {
      rs >> v.first >> v.second;
      ctr.insert( v );
   }

   return rs;
}

template< typename K, typename T, class C, class A > read_stream& operator >>( read_stream& rs, std::multimap< K, T, C, A >& ctr )
{
   typename std::multimap< K, T, C, A >::size_type size;
   rs >> size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy;
   rs >> dummy;
#  endif

   ctr.clear( );

   std::pair< typename std::multimap< K, T, C, A >::key_type, typename std::multimap< K, T, C, A >::mapped_type > v;
   for( typename std::multimap< K, T, C, A >::size_type i = 0; i < size; i++ )
   {
      rs >> v.first >> v.second;
      ctr.insert( v );
   }

   return rs;
}

template< typename C, class T, class A > read_stream& operator >>( read_stream& rs, std::basic_string< C, T, A >& ctr )
{
   typename std::basic_string< C, T, A >::size_type size;
   rs >> size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy;
   rs >> dummy;
#  endif

#  ifdef USE_FAST_STRING_READ
   ctr = std::basic_string< C, T, A >( size, 0 );
   unsigned char* p_buf = ( unsigned char* )ctr.data( );
   rs.read( p_buf, size * sizeof( typename T::char_type ) );
#  else
   ctr.erase( );
   typename std::basic_string< C, T, A >::value_type v;
   for( typename std::basic_string< C, T, A >::size_type i = 0; i < size; i++ )
   {
      rs >> v;
      ctr.append( 1, v );
   }
#  endif

   return rs;
}

inline size_t size_determiner( const bool* ) { return sizeof( bool ); }
inline size_t size_determiner( const char* ) { return sizeof( char ); }
inline size_t size_determiner( const signed char* ) { return sizeof( signed char ); }
inline size_t size_determiner( const unsigned char* ) { return sizeof( unsigned char ); }
inline size_t size_determiner( const wchar_t* ) { return sizeof( wchar_t ); }
inline size_t size_determiner( const short* ) { return sizeof( short ); }
inline size_t size_determiner( const unsigned short* ) { return sizeof( unsigned short ); }
inline size_t size_determiner( const int* ) { return sizeof( int ); }
inline size_t size_determiner( const unsigned int* ) { return sizeof( unsigned int ); }
inline size_t size_determiner( const long* ) { return sizeof( long ); }
inline size_t size_determiner( const unsigned long* ) { return sizeof( unsigned long ); }
inline size_t size_determiner( const long_long* ) { return sizeof( long_long ); }
inline size_t size_determiner( const unsigned_long_long* ) { return sizeof( unsigned_long_long ); }
inline size_t size_determiner( const float* ) { return sizeof( float ); }
inline size_t size_determiner( const double* ) { return sizeof( double ); }
inline size_t size_determiner( const long double* ) { return sizeof( long double ); }

template< typename T1, typename T2 > inline size_t size_determiner( const std::pair< T1, T2 >* p )
{
   return p == 0 ? 0 : size_determiner( &p->first ) + size_determiner( &p->second );
}

template< typename T1, typename T2 > write_stream& operator <<( write_stream& ws, const std::pair< T1, T2 >& val )
{
   ws << val.first << val.second;
   return ws;
}

template< typename C, class T, class A > size_t size_determiner( const std::basic_string< C, T, A >* p )
{
   if( !p )
      return 0;

   C* p_c( 0 );
   size_t total_size = size_determiner( p_c );

   if( total_size )
      total_size *= p->size( );
   else for( typename std::basic_string< C, T, A >::const_iterator ci = p->begin( ), end = p->end( ); ci != end; ++ci )
      total_size += size_determiner( &*ci );

   total_size += sizeof( typename std::basic_string< C, T, A >::size_type );

   return total_size;
}

template< typename C, class T, class A > write_stream& operator <<( write_stream& ws, const std::basic_string< C, T, A >& ctr )
{
   typename std::basic_string< C, T, A >::size_type size = ctr.size( );
   ws << size;

   ws.write( ( unsigned char* )ctr.data( ), size * sizeof( typename T::char_type ) );

   return ws;
}

template< typename T, class A > size_t size_determiner( const std::list< T, A >* p )
{
   if( !p )
      return 0;

   T* p_t( 0 );
   size_t total_size = size_determiner( p_t );

   if( total_size )
      total_size *= p->size( );
   else for( typename std::list< T, A >::const_iterator ci = p->begin( ), end = p->end( ); ci != end; ++ci )
      total_size += size_determiner( &*ci );

   total_size += sizeof( typename std::list< T, A >::size_type );
#  ifdef USE_SIZE_PADDING
   total_size += sizeof( std::string::size_type );
#  endif

   return total_size;
}

template< typename T, class A > write_stream& operator <<( write_stream& ws, const std::list< T, A >& ctr )
{
   typename std::list< T, A >::size_type size = ctr.size( );
   ws << size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy( 0 );
   ws << dummy;
#  endif

   typename std::list< T, A >::const_iterator ci;
   for( ci = ctr.begin( ); ci != ctr.end( ); ++ci )
      ws << *ci;

   return ws;
}

template< typename T, class A > size_t size_determiner( const std::deque< T, A >* p )
{
   if( !p )
      return 0;

   T* p_t( 0 );
   size_t total_size = size_determiner( p_t );

   if( total_size )
      total_size *= p->size( );
   else for( typename std::deque< T, A >::const_iterator ci = p->begin( ), end = p->end( ); ci != end; ++ci )
      total_size += size_determiner( &*ci );

   total_size += sizeof( typename std::deque< T, A >::size_type );
#  ifdef USE_SIZE_PADDING
   total_size += sizeof( std::string::size_type );
#  endif

   return total_size;
}

template< typename T, class A > write_stream& operator <<( write_stream& ws, const std::deque< T, A >& ctr )
{
   typename std::deque< T, A >::size_type size = ctr.size( );
   ws << size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy( 0 );
   ws << dummy;
#  endif

   for( typename std::deque< T, A >::size_type i = 0; i < size; i++ )
      ws << ctr[ i ];

   return ws;
}

template< typename T, class A > size_t size_determiner( const std::vector< T, A >* p )
{
   if( !p )
      return 0;

   T* p_t( 0 );
   size_t total_size = size_determiner( p_t );

   if( total_size )
      total_size *= p->size( );
   else for( typename std::vector< T, A >::const_iterator ci = p->begin( ), end = p->end( ); ci != end; ++ci )
      total_size += size_determiner( &*ci );

   total_size += sizeof( typename std::vector< T, A >::size_type );
#  ifdef USE_SIZE_PADDING
   total_size += sizeof( std::string::size_type );
#  endif

   return total_size;
}

template< typename T, class A > write_stream& operator <<( write_stream& ws, const std::vector< T, A >& ctr )
{
   typename std::vector< T, A >::size_type size = ctr.size( );
   ws << size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy( 0 );
   ws << dummy;
#  endif

   for( typename std::vector< T, A >::size_type i = 0; i < size; i++ )
      ws << ctr[ i ];

   return ws;
}

template< typename K, class C, class A > size_t size_determiner( const std::set< K, C, A >* p )
{
   if( !p )
      return 0;

   K* p_k( 0 );
   size_t total_size = size_determiner( p_k );

   if( total_size )
      total_size *= p->size( );
   else for( typename std::set< K, C, A >::const_iterator ci = p->begin( ), end = p->end( ); ci != end; ++ci )
      total_size += size_determiner( &*ci );

   total_size += sizeof( typename std::set< K, C, A >::size_type );
#  ifdef USE_SIZE_PADDING
   total_size += sizeof( std::string::size_type );
#  endif

   return total_size;
}

template< typename K, class C, class A > write_stream& operator <<( write_stream& ws, const std::set< K, C, A >& ctr )
{
   typename std::set< K, C, A >::size_type size = ctr.size( );
   ws << size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy( 0 );
   ws << dummy;
#  endif

   typename std::set< K, C, A >::const_iterator ci;
   for( ci = ctr.begin( ); ci != ctr.end( ); ++ci )
      ws << *ci;

   return ws;
}

template< typename K, class C, class A > size_t size_determiner( const std::multiset< K, C, A >* p )
{
   if( !p )
      return 0;

   K* p_k( 0 );
   size_t total_size = size_determiner( p_k );

   if( total_size )
      total_size *= p->size( );
   else for( typename std::multiset< K, C, A >::const_iterator ci = p->begin( ), end = p->end( ); ci != end; ++ci )
      total_size += size_determiner( &*ci );

   total_size += sizeof( typename std::multiset< K, C, A >::size_type );
#  ifdef USE_SIZE_PADDING
   total_size += sizeof( std::string::size_type );
#  endif

   return total_size;
}

template< typename K, class C, class A > write_stream& operator <<( write_stream& ws, const std::multiset< K, C, A >& ctr )
{
   typename std::multiset< K, C, A >::size_type size = ctr.size( );
   ws << size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy( 0 );
   ws << dummy;
#  endif

   typename std::multiset< K, C, A >::const_iterator ci;
   for( ci = ctr.begin( ); ci != ctr.end( ); ++ci )
      ws << *ci;

   return ws;
}

template< typename K, typename T, class C, class A > size_t size_determiner( const std::map< K, T, C, A >* p )
{
   if( !p )
      return 0;

   K* p_k( 0 );
   size_t key_size = size_determiner( p_k );

   T* p_t( 0 );
   size_t data_size = size_determiner( p_t );

   size_t total_size( key_size + data_size );

   if( key_size && data_size )
      total_size += ( key_size * p->size( ) ) + ( data_size * p->size( ) );
   else for( typename std::map< K, T, C, A >::const_iterator ci = p->begin( ), end = p->end( ); ci != end; ++ci )
   {
      if( !key_size )
         total_size += size_determiner( &ci->first );

      if( !data_size )
         total_size += size_determiner( &ci->second );
   }

   if( key_size && !data_size )
      total_size += key_size * p->size( );
   else if( !key_size && data_size )
      total_size += data_size * p->size( );

   total_size += sizeof( typename std::map< K, T, C, A >::size_type );
#  ifdef USE_SIZE_PADDING
   total_size += sizeof( std::string::size_type );
#  endif

   return total_size;
}

template< typename K,
 typename T, class C, class A > write_stream& operator <<( write_stream& ws, const std::map< K, T, C, A >& ctr )
{
   typename std::map< K, T, C, A >::size_type size = ctr.size( );
   ws << size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy( 0 );
   ws << dummy;
#  endif

   typename std::map< K, T, C, A >::const_iterator ci;
   for( ci = ctr.begin( ); ci != ctr.end( ); ++ci )
      ws << ci->first << ci->second;

   return ws;
}

template< typename K, typename T, class C, class A > size_t size_determiner( const std::multimap< K, T, C, A >* p )
{
   if( !p )
      return 0;

   K* p_k( 0 );
   size_t key_size = size_determiner( p_k );

   T* p_t( 0 );
   size_t data_size = size_determiner( p_t );

   size_t total_size( key_size + data_size );

   if( key_size && data_size )
      total_size += ( key_size * p->size( ) ) + ( data_size * p->size( ) );
   else for( typename std::multimap< K, T, C, A >::const_iterator ci = p->begin( ), end = p->end( ); ci != end; ++ci )
   {
      if( !key_size )
         total_size += size_determiner( &ci->first );

      if( !data_size )
         total_size += size_determiner( &ci->second );
   }

   if( key_size && !data_size )
      total_size += key_size * p->size( );
   else if( !key_size && data_size )
      total_size += data_size * p->size( );

   total_size += sizeof( typename std::multimap< K, T, C, A >::size_type );
#  ifdef USE_SIZE_PADDING
   total_size += sizeof( std::string::size_type );
#  endif

   return total_size;
}

template< typename K,
 typename T, class C, class A > write_stream& operator <<( write_stream& ws, const std::multimap< K, T, C, A >& ctr )
{
   typename std::multimap< K, T, C, A >::size_type size = ctr.size( );
   ws << size;
#  ifdef USE_SIZE_PADDING
   std::string::size_type dummy( 0 );
   ws << dummy;
#  endif

   typename std::multimap< K, T, C, A >::const_iterator ci;
   for( ci = ctr.begin( ); ci != ctr.end( ); ++ci )
      ws << ci->first << ci->second;

   return ws;
}

#endif

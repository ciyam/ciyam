// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef NULLABLE_H
#  define NULLABLE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iostream>
#     include <stdexcept>
#  endif

#  define ALLOW_NULL_EQUALITY

const char* const c_null_output_value = "<null>";
const char* const c_null_exception_message = "is null";

template< typename T > class nullable
{
   public:
   nullable( ) : null( true ) { }

   nullable( const T& t ) : null( false ), object( t ) { }

   nullable( const nullable< T >& nt ) { copy( nt ); }

   nullable< T >& operator =( const nullable< T >& nt )
   {
      copy( nt );
      return *this;
   }

   void copy( const nullable< T >& nt )
   {
      null = nt.null;
      if( !null )
         object = nt.object;
   }

   nullable< T >& operator =( const T& t )
   {
      object = t;
      null = false;
      return *this;
   }

   bool is_null( ) const { return null; }

   void set_null( ) { null = true; }

   T& operator ++( )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );
      return ++object;
   }

   T operator ++( int )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );
      return object++;
   }

   T& operator --( )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );
      return --object;
   }

   T operator --( int )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );
      return object--;
   }

   operator T&( )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );
      return object;
   }

   operator const T&( ) const
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );
      return object;
   }

   T& operator *( )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );
      return object;
   }

   const T& operator *( ) const
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );
      return object;
   }

   T* operator ->( )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );

      return &object;
   }

   const T* operator ->( ) const
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );

      return &object;
   }

   template< typename U > nullable< T >& operator +=( const U& u )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );

      object += u;

      return *this;
   }

   template< typename U > nullable< T >& operator -=( const U& u )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );

      object -= u;

      return *this;
   }

   template< typename U > nullable< T >& operator *=( const U& u )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );

      object *= u;

      return *this;
   }

   template< typename U > nullable< T >& operator /=( const U& u )
   {
      if( null )
         throw std::runtime_error( c_null_exception_message );

      object /= u;

      return *this;
   }

   private:
   T object;
   bool null;
};

template< typename T > inline std::ostream& operator <<( std::ostream& os, const nullable< T >& nt )
{
   if( nt.is_null( ) )
      os << c_null_output_value;
   else
      os << *nt;
   return os;
}

template< typename T > inline bool operator <( const nullable< T >& lhs, const T& rhs )
{
   if( lhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs < rhs;
}

template< typename T > inline bool operator <( const T& lhs, const nullable< T >& rhs )
{
   if( rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return lhs < *rhs;
}

template< typename T > inline bool operator <=( const nullable< T >& lhs, const T& rhs )
{
   if( lhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs <= rhs;
}

template< typename T > inline bool operator <=( const T& lhs, const nullable< T >& rhs )
{
   if( rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return lhs <= *rhs;
}

template< typename T > inline bool operator >( const nullable< T >& lhs, const T& rhs )
{
   if( lhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs > rhs;
}

template< typename T > inline bool operator >( const T& lhs, const nullable< T >& rhs )
{
   if( rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return lhs > *rhs;
}

template< typename T > inline bool operator >=( const nullable< T >& lhs, const T& rhs )
{
   if( lhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs >= rhs;
}

template< typename T > inline bool operator >=( const T& lhs, const nullable< T >& rhs )
{
   if( rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return lhs >= *rhs;
}

template< typename T > inline bool operator ==( const nullable< T >& lhs, const T& rhs )
{
   if( lhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs == rhs;
}

template< typename T > inline bool operator ==( const T& lhs, const nullable< T >& rhs )
{
   if( rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return lhs == *rhs;
}

template< typename T > inline bool operator !=( const nullable< T >& lhs, const T& rhs )
{
   return !( lhs == rhs );
}

template< typename T > inline bool operator !=( const T& lhs, const nullable< T >& rhs )
{
   return !( lhs == rhs );
}

template< typename T > inline bool operator <( const nullable< T >& lhs, const nullable< T >& rhs )
{
   if( lhs.is_null( ) || rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs < *rhs;
}

template< typename T > inline bool operator <=( const nullable< T >& lhs, const nullable< T >& rhs )
{
   if( lhs.is_null( ) || rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs <= *rhs;
}

template< typename T > inline bool operator >( const nullable< T >& lhs, const nullable< T >& rhs )
{
   if( lhs.is_null( ) || rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs > rhs;
}

template< typename T > inline bool operator >=( const nullable< T >& lhs, const nullable< T >& rhs )
{
   if( lhs.is_null( ) || rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
   return *lhs >= rhs;
}

template< typename T > inline bool operator ==( const nullable< T >& lhs, const nullable< T >& rhs )
{
#  ifdef ALLOW_NULL_EQUALITY
   if( lhs.is_null( ) && rhs.is_null( ) )
      return true;

   if( lhs.is_null( ) || rhs.is_null( ) )
      return false;
#  else
   if( lhs.is_null( ) || rhs.is_null( ) )
      throw std::runtime_error( c_null_exception_message );
#  endif
   return *lhs == *rhs;
}

template< typename T > inline bool operator !=( const nullable< T >& lhs, const nullable< T >& rhs )
{
   return !( lhs == rhs );
}

#endif


// Copyright (c) 2000-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef POINTERS_H
#  define POINTERS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <functional>
#  endif

#  include "macros.h"

#  define SPECIALISE_STD_LESS_TO_DEREFERENCE

// NOTE: The following template class and specialisation permits a pointer's type to be used in
// situations where the type is a template parameter is actually a pointer (whilst being usable
// if the template parameter is not a pointer).
//
// Usage example:
// template< typename C > struct container_wrapper
// {
//    typedef typename remove_pointer< C::value_type >::type base_type;
//    base_type* construct_container_object( ) { return new base_type; }
// };
//
// The above source would create a new "string" with "container_wrapper< vector< string > >" or
// with "container_wrapper< vector< string* > >".

template< class T > struct remove_pointer
{
   typedef T type;
};

template< class T > struct remove_pointer< T* >
{
   typedef T type;
};

template< typename T > class ref_count_ptr
{
   typedef void ( ref_count_ptr< T >::*bool_type )( ) const;
   void this_type_does_not_support_comparisons( ) const { }

   public:
   ref_count_ptr( )
    : p_instance( 0 ), p_ref_count( new int( 1 ) ) { }

   explicit ref_count_ptr( T* p_instance )
    : p_instance( p_instance ), p_ref_count( new int( 1 ) ) { }

   ref_count_ptr( const ref_count_ptr< T >& src )
   {
      copy_from( src );
   }

   template< typename D > ref_count_ptr( const ref_count_ptr< D >& src )
   {
      copy_from( src );
   }

   ref_count_ptr< T >& operator =( T* p_new_instance )
   {
      if( p_instance == p_new_instance )
         return *this;

      if( !--*p_ref_count )
      {
         delete p_instance;
         *p_ref_count = 1;
      }
      else
         p_ref_count = new int( 1 );

      p_instance = p_new_instance;
      return *this;
   }

   ref_count_ptr< T >& operator =( const ref_count_ptr< T >& src )
   {
      if( !--*p_ref_count )
      {
         delete p_instance;
         delete p_ref_count;
      }
      copy_from( src );
      return *this;
   }

   template< typename D > ref_count_ptr< T >& operator =( const ref_count_ptr< D >& src )
   {
      if( !--*p_ref_count )
      {
         delete p_instance;
         delete p_ref_count;
      }
      copy_from( src );
      return *this;
   }

   ~ref_count_ptr( )
   {
      if( !--*p_ref_count )
      {
         delete p_instance;
         delete p_ref_count;
      }
   }

   operator bool_type( ) const
   {
      if( !p_instance )
         return 0;
      else
         return &ref_count_ptr< T >::this_type_does_not_support_comparisons;
   }

   template< typename TT > bool operator ==( const TT& rhs ) const
   {
      this_type_does_not_support_comparisons( );
      return false;
   }

   template< typename TT > bool operator !=( const TT& rhs ) const
   {
      this_type_does_not_support_comparisons( );
      return false;
   }

   T& operator *( ) const { return *p_instance; }
   T* operator ->( ) const { return p_instance; }

   T* get( ) const { return p_instance; }
   int* get_ref( ) const { return p_ref_count; }

   private:
   template< typename D > void copy_from( const ref_count_ptr< D >& src )
   {
      p_instance = src.get( );
      p_ref_count = src.get_ref( );

      ++*p_ref_count;
   }

   T* p_instance;
   int* p_ref_count;
};

template< typename T, typename U >
 inline bool operator ==( const ref_count_ptr< T >& lhs, const ref_count_ptr< U >& rhs )
{
    return lhs.get( ) == rhs.get( );
}

template< typename T, typename U >
 inline bool operator !=( const ref_count_ptr< T >& lhs, const ref_count_ptr< U >& rhs )
{
    return lhs.get( ) != rhs.get( );
}

template< typename T > inline bool operator <( const ref_count_ptr< T >& lhs, const ref_count_ptr< T >& rhs )
{
    return std::less< T* >( )( lhs.get( ), rhs.get( ) );
}

#  ifdef SPECIALISE_STD_LESS_TO_DEREFERENCE
namespace std
{

template< typename T > struct less< ref_count_ptr< T > >
{
   bool operator ( )( const ref_count_ptr< T >& lhs, const ref_count_ptr< T >& rhs ) const
   {
      return *lhs < *rhs;
   }
};

}
#  endif

#endif


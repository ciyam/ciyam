// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef OID_POINTER_H
#  define OID_POINTER_H

#  include "ods.h"

template< typename T > class oid_pointer;
template< typename T > int_t size_of( const oid_pointer< T >& );
template< typename T > read_stream& operator >>( read_stream& rs, oid_pointer< T >& op );
template< typename T > write_stream& operator <<( write_stream& ws, const oid_pointer< T >& op );

enum oid_pointer_opt
{
   e_oid_pointer_opt_if_changed,
   e_oid_pointer_opt_force_write
};

template< typename T > class oid_pointer
{
   friend int_t size_of< T >( const oid_pointer< T >& );

   friend read_stream& operator >> < T >( read_stream& rs, oid_pointer< T >& op );
   friend write_stream& operator << < T >( write_stream& ws, const oid_pointer< T >& op );

   public:
   oid_pointer( ) : p_T( 0 ), changed( false ) { }

   explicit oid_pointer( T* p_T );

   ~oid_pointer( ) { delete p_T; }

   oid_pointer( const oid_pointer< T >& src );

   oid_pointer& operator =( const oid_pointer< T >& src );

   oid get_id( ) const { return id; }

   T* get( ) const { return p_T; }

   T* operator ->( );
   const T* operator ->( ) const;

   T& operator *( );
   const T& operator *( ) const;

   void copy( const oid_pointer< T >& src );

   void store( oid_pointer_opt opt = e_oid_pointer_opt_if_changed );
   void destroy( );

   int_t get_size_of( ) const { return sizeof( oid ); }

   void reset( T* p );
   void set_id( const oid& new_id );

   private:
   void get_instance( ) const;

   oid id;

   mutable T* p_T;
   mutable bool changed;
};

template< typename T > inline oid_pointer< T >::oid_pointer( T* p_T )
 :
 p_T( p_T ),
 changed( false )
{
   if( p_T )
   {
      id = p_T->get_id( );
      changed = id.is_new( );
   }
}

template< typename T > inline oid_pointer< T >::oid_pointer( const oid_pointer< T >& src )
{
   copy( src );
}

template< typename T > inline oid_pointer< T >& oid_pointer< T >::operator =( const oid_pointer< T >& src )
{
   copy( src );
   return *this;
}

template< typename T > inline T* oid_pointer< T >::operator ->( )
{
   changed = true;
   get_instance( );
   return p_T;
}

template< typename T > inline const T* oid_pointer< T >::operator ->( ) const
{
   get_instance( );
   return p_T;
}

template< typename T > inline T& oid_pointer< T >::operator *( )
{
   changed = true;
   return *( operator ->( ) );
}

template< typename T > inline const T& oid_pointer< T >::operator *( ) const
{
   return *( operator ->( ) );
}

template< typename T > inline void oid_pointer< T >::copy( const oid_pointer< T >& src )
{
   id = src.id;

   if( !src.p_T )
      p_T = 0;
   else
      p_T = new T( *src.p_T );

   changed = src.changed;
}

template< typename T > inline void oid_pointer< T >::store( oid_pointer_opt opt )
{
   if( p_T && ( changed || opt == e_oid_pointer_opt_force_write ) )
   {
      *ods::instance( ) << *p_T;
      id = p_T->get_id( );
      changed = false;
   }
}

template< typename T > inline void oid_pointer< T >::destroy( )
{
   if( !id.is_new( ) )
      ods::instance( )->destroy( id );

   delete p_T;

   p_T = 0;
   id = oid( );
   changed = false;
}

template< typename T > inline void oid_pointer< T >::reset( T* p )
{
   delete p_T;

   p_T = p;
   if( !p_T )
   {
      id = oid( );
      changed = false;
   }
   else
   {
      id = p_T->get_id( );
      changed = id.is_new( );
   }
}

template< typename T > inline void oid_pointer< T >::set_id( const oid& new_id )
{
   delete p_T;

   p_T = 0;
   id = new_id;
   changed = false;
}

template< typename T > inline void oid_pointer< T >::get_instance( ) const
{
   if( !p_T )
   {
      p_T = new T;
      p_T->set_id( id );

      if( !id.is_new( ) )
         *ods::instance( ) >> *p_T;
   }
}

template< typename T > inline int_t size_of( const oid_pointer< T >& p )
{
   return p.get_size_of( );
}

template< typename T > inline read_stream& operator >>( read_stream& rs, oid_pointer< T >& op )
{
   delete op.p_T;
   op.p_T = 0;

   rs >> op.id;
   return rs;
}

template< typename T > inline write_stream& operator <<( write_stream& ws, const oid_pointer< T >& op )
{
   ws << op.id;
   return ws;
}

template< typename T > inline size_t size_determiner( const oid_pointer< T >* ) { return sizeof( oid ); }

#endif


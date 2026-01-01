// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CLASS_FACTORY_H
#  define CLASS_FACTORY_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <string>
#     include <sstream>
#     include <iostream>
#     include <stdexcept>
#  endif

#  include "macros.h"

#  define DECLARE_CLASS_FACTORY( key_type, base_class ) \
 extern class_factory< key_type, base_class, none, none >& get_##base_class##_factory( );

#  define DECLARE_CLASS_FACTORY_1( key_type, base_class, parm_type ) \
 extern class_factory< key_type, base_class, none, parm_type >& get_##base_class##_factory( );

#  define DECLARE_CLASS_FACTORY_0_1( key_type, base_class, parm_type ) \
 extern class_factory< key_type, base_class, none, parm_type >& get_##base_class##_factory( );

#  define DECLARE_COMPLEX_CLASS_FACTORY( key_type, base_class, inst_base ) \
 extern class_factory< key_type, base_class, inst_base, none >& get_##base_class##_factory( );

#  define DECLARE_COMPLEX_CLASS_FACTORY_1( key_type, base_class, inst_base, parm_type ) \
 extern class_factory< key_type, base_class, inst_base, parm_type >& get_##base_class##_factory( );

#  define DECLARE_COMPLEX_CLASS_FACTORY_0_1( key_type, base_class, inst_base, parm_type ) \
 extern class_factory< key_type, base_class, inst_base, parm_type >& get_##base_class##_factory( );

#  define DEFINE_CLASS_FACTORY( key_type, base_class ) \
 class_factory< key_type, base_class, none, none >& \
 get_##base_class##_factory( ) { return get_class_factory< key_type, base_class, none >( ); }

#  define DEFINE_CLASS_FACTORY_1( key_type, base_class, parm_type ) \
 class_factory< key_type, base_class, none, parm_type >& \
 get_##base_class##_factory( ) { return get_class_factory< key_type, base_class, none, parm_type >( ); }

#  define DEFINE_CLASS_FACTORY_0_1( key_type, base_class, parm_type ) \
 class_factory< key_type, base_class, none, parm_type >& \
 get_##base_class##_factory( ) { return get_class_factory< key_type, base_class, none, parm_type >( ); }

#  define DEFINE_COMPLEX_CLASS_FACTORY( key_type, base_class, inst_base ) \
 class_factory< key_type, base_class, inst_base, none >& \
 get_##base_class##_factory( ) { return get_class_factory< key_type, base_class, inst_base >( ); }

#  define DEFINE_COMPLEX_CLASS_FACTORY_1( key_type, base_class, inst_base, parm_type ) \
 class_factory< key_type, base_class, inst_base, parm_type >& \
 get_##base_class##_factory( ) { return get_class_factory< key_type, base_class, inst_base, parm_type >( ); }

#  define DEFINE_COMPLEX_CLASS_FACTORY_0_1( key_type, base_class, inst_base, parm_type ) \
 class_factory< key_type, base_class, inst_base, parm_type >& \
 get_##base_class##_factory( ) { return get_class_factory< key_type, base_class, inst_base, parm_type >( ); }

#  define DEFINE_CLASS_FACTORY_INSTANTIATOR( key_type, base_class, derived_class, static_func ) \
 template< > struct key_class_base< key_type, derived_class > \
 { key_type get_key( ) const; }; \
 key_type key_class_base< key_type, derived_class >::get_key( ) const \
 { return derived_class::static_func( ); } \
 concrete_instantiator_0< none, none, key_type, base_class, derived_class > \
 CONCATIZE( derived_class, _Instantiator )( \
 ( const key_class_base< key_type, derived_class >& )key_class_base< key_type, derived_class >( ) )

#  define DEFINE_CLASS_FACTORY_INSTANTIATOR_1( key_type, base_class, derived_class, parm_type, static_func ) \
 template< > struct key_class_base< key_type, derived_class > \
 { key_type get_key( ) const; }; \
 key_type key_class_base< key_type, derived_class >::get_key( ) const \
 { return derived_class::static_func( ); } \
 concrete_instantiator_1< none, none, key_type, base_class, parm_type, derived_class > \
 CONCATIZE( derived_class, _Instantiator )( \
 ( const key_class_base< key_type, derived_class >& )key_class_base< key_type, derived_class >( ) )

#  define DEFINE_CLASS_FACTORY_INSTANTIATOR_0_1( key_type, base_class, derived_class, parm_type, static_func ) \
 template< > struct key_class_base< key_type, derived_class > \
 { key_type get_key( ) const; }; \
 key_type key_class_base< key_type, derived_class >::get_key( ) const \
 { return derived_class::static_func( ); } \
 concrete_instantiator_0_1< none, none, key_type, base_class, parm_type, derived_class > \
 CONCATIZE( derived_class, _Instantiator )( \
 ( const key_class_base< key_type, derived_class >& )key_class_base< key_type, derived_class >( ) )

#  define DEFINE_COMPLEX_CLASS_FACTORY_INSTANTIATOR( inst_base, inst_class, key_type, base_class, derived_class, static_func ) \
 template< > struct key_class_base< key_type, derived_class > \
 { key_type get_key( ) const; }; \
 key_type key_class_base< key_type, derived_class >::get_key( ) const \
 { return derived_class::static_func( ); } \
 concrete_instantiator_0< inst_base, inst_class, key_type, base_class, derived_class > \
 CONCATIZE( derived_class, _Instantiator )( \
 ( const key_class_base< key_type, derived_class >& )key_class_base< key_type, derived_class >( ) )

#  define DEFINE_COMPLEX_CLASS_FACTORY_INSTANTIATOR_1( \
 inst_base, inst_class, key_type, base_class, derived_class, parm_type, static_func ) \
 template< > struct key_class_base< key_type, derived_class > \
 { key_type get_key( ) const; }; \
 key_type key_class_base< key_type, derived_class >::get_key( ) const \
 { return derived_class::static_func( ); } \
 concrete_instantiator_1< inst_base, inst_class, key_type, base_class, parm_type, derived_class > \
 CONCATIZE( derived_class, _Instantiator )( \
 ( const key_class_base< key_type, derived_class >& )key_class_base< key_type, derived_class >( ) )

#  define DEFINE_COMPLEX_CLASS_FACTORY_INSTANTIATOR_0_1( \
 inst_base, inst_class, key_type, base_class, derived_class, parm_type, static_func ) \
 template< > struct key_class_base< key_type, derived_class > \
 { key_type get_key( ) const; }; \
 key_type key_class_base< key_type, derived_class >::get_key( ) const \
 { return derived_class::static_func( ); } \
 concrete_instantiator_0_1< inst_base, inst_class, key_type, base_class, parm_type, derived_class > \
 CONCATIZE( derived_class, _Instantiator )( \
 ( const key_class_base< key_type, derived_class >& )key_class_base< key_type, derived_class >( ) )

class none { };

template< typename I, typename K, typename B, typename P > class instantiator;

template< typename K, typename B, typename I, typename P > class class_factory
{
   typedef typename std::map< K, std::pair< I*, instantiator< I, K, B, P >* > > class_instantiator_container;
   typedef typename class_instantiator_container::value_type class_instantiator_value;
   typedef typename class_instantiator_container::iterator class_instantiator_iterator;
   typedef typename class_instantiator_container::const_iterator class_instantiator_const_iterator;

   public:
   class_factory( ) : had_duplicate( false ) { }

   B* construct( const K& key );
   B* construct_1( const K& key, const P& param );

   template< typename T > void construct( const K& key, T*& p_object );
   template< typename T > void construct_1( const K& key, T*& p_object, const P& param );

   I* get_instantiator( const K& key ) const;

   int get_num_instantiators( ) const { return class_instantiators.size( ); }
   I* get_instantiator_num( int offset ) const;

   void register_class( const K& key, I* p_ibase, instantiator< I, K, B, P >* p_instantiator );

   void dump_registered_class_keys( std::ostream& ostr );

   private:
   K duplicate_key;
   bool had_duplicate;

   class_instantiator_container class_instantiators;
};

template< typename K, typename B, typename I, typename P > B* class_factory< K, B, I, P >::construct( const K& key )
{
   if( had_duplicate )
   {
      std::ostringstream osstr;
      osstr << "duplicate key '" << key << "' was registered in class factory";
      throw std::runtime_error( osstr.str( ) );
   }

   class_instantiator_iterator cii = class_instantiators.find( key );
   if( cii == class_instantiators.end( ) )
      return 0;

   return cii->second.second->construct( );
}

template< typename K, typename B, typename I, typename P >
 B* class_factory< K, B, I, P >::construct_1( const K& key, const P& param )
{
   if( had_duplicate )
   {
      std::ostringstream osstr;
      osstr << "duplicate key '" << key << "' was registered in class factory";
      throw std::runtime_error( osstr.str( ) );
   }

   class_instantiator_iterator cii = class_instantiators.find( key );
   if( cii == class_instantiators.end( ) )
      return 0;

   return cii->second.second->construct_1( param );
}

template< typename K, typename B, typename I, typename P >
 template< typename T > void class_factory< K, B, I, P >::construct( const K& key, T*& p_object )
{
   p_object = dynamic_cast< T* >( construct( key ) );
}

template< typename K, typename B, typename I, typename P > template< typename T >
 void class_factory< K, B, I, P >::construct_1( const K& key, T*& p_object, const P& param )
{
   p_object = dynamic_cast< T* >( construct_1( key, param ) );
}

template< typename K, typename B, typename I, typename P > I* class_factory< K, B, I, P >::get_instantiator( const K& key ) const
{
   I* p_instantiator_base = 0;

   class_instantiator_const_iterator cici = class_instantiators.find( key );
   if( cici != class_instantiators.end( ) )
      p_instantiator_base = cici->second.first;

   return p_instantiator_base;
}

template< typename K, typename B, typename I, typename P > I* class_factory< K, B, I, P >::get_instantiator_num( int offset ) const
{
   int i = 0;
   I* p_instantiator_base = 0;

   class_instantiator_const_iterator cici;
   for( cici = class_instantiators.begin( ); cici != class_instantiators.end( ); ++cici, ++i )
   {
      if( i == offset )
      {
         p_instantiator_base = cici->second.first;
         break;
      }
   }

   return p_instantiator_base;
}

template< typename K, typename B, typename I, typename P >
 void class_factory< K, B, I, P >::register_class( const K& key, I* p_ibase, instantiator< I, K, B, P >* p_instantiator )
{
   std::pair< class_instantiator_const_iterator, bool > rc =
    class_instantiators.insert( class_instantiator_value( key, make_pair( p_ibase, p_instantiator ) ) );

   if( !rc.second )
   {
      duplicate_key = key;
      had_duplicate = true;
   }
}

template< typename K, typename B, typename I, typename P >
 void class_factory< K, B, I, P >::dump_registered_class_keys( std::ostream& ostr )
{
   class_instantiator_iterator cii;
   for( cii = class_instantiators.begin( ); cii != class_instantiators.end( ); ++cii )
      ostr << cii->first << '\n';
}

template< typename K, typename B, typename I, typename P > class_factory< K, B, I, P >& get_class_factory( )
{
   static class_factory< K, B, I, P > class_factory;
   return class_factory;
}

template< typename K, typename B, typename I > class_factory< K, B, I, none >& get_class_factory( )
{
   return get_class_factory< K, B, I, none >( );
}

template< typename I, typename K, typename B, typename P = none > class instantiator
{
   public:
   instantiator( const K& key, I* p_ibase );

   virtual B* construct( ) = 0;
   virtual B* construct_1( const P& ) = 0;
};

template< typename I, typename K, typename B, typename P >
 instantiator< I, K, B, P >::instantiator( const K& key, I* p_ibase )
{
   get_class_factory< K, B, I, P >( ).register_class( key, p_ibase, this );
}

template< typename K, typename T > struct key_class_base
{
   virtual K get_key( ) const = 0;
};

template< typename I, typename C, typename K, typename B, typename P, typename T > class concrete_instantiator
 : public C, public instantiator< I, K, B, P >
{
   public:
   concrete_instantiator( const key_class_base< K, T >& key_object )
    : instantiator< I, K, B, P >( key_object.get_key( ), this )
   {
   }
};

template< typename I, typename C, typename K, typename B, typename T > class concrete_instantiator_0
 : public concrete_instantiator< I, C, K, B, none, T >
{
   public:
   concrete_instantiator_0( const key_class_base< K, T >& key_object )
    : concrete_instantiator< I, C, K, B, none, T >( key_object )
   {
   }

   T* construct( ) { return new T; }
   T* construct_1( const none& ) { return 0; }
};

template< typename I, typename C, typename K, typename B, typename P, typename T > class concrete_instantiator_1
 : public concrete_instantiator< I, C, K, B, P, T >
{
   public:
   concrete_instantiator_1( const key_class_base< K, T >& key_object )
    : concrete_instantiator< I, C, K, B, P, T >( key_object )
   {
   }

   T* construct( ) { return 0; }
   T* construct_1( const P& param ) { return new T( param ); }
};

template< typename I, typename C, typename K, typename B, typename P, typename T > class concrete_instantiator_0_1
 : public concrete_instantiator< I, C, K, B, P, T >
{
   public:
   concrete_instantiator_0_1( const key_class_base< K, T >& key_object )
    : concrete_instantiator< I, C, K, B, P, T >( key_object )
   {
   }

   T* construct( ) { return new T; }
   T* construct_1( const P& param ) { return new T( param ); }
};

#endif // CLASS_FACTORY_H


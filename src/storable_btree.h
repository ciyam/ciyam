// Copyright (c) 2006-2015 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2015-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef STORABLE_BTREE_H
#  define STORABLE_BTREE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <stdexcept>
#  endif

#  include "ods.h"
#  include "btree.h"
#  include "read_write_stream.h"

#  ifndef STORABLE_BTREE_SIZE
#     define STORABLE_BTREE_SIZE 256
#  endif

#  ifndef STORABLE_BTREE_NODE_SIZE
#     define STORABLE_BTREE_NODE_SIZE 1024
#  endif

using namespace btree;

// NOTE: This approach is necessary to force template instanciation to occur (at least with BCB).
template< typename T > class storable_node_base;
template< typename T > int64_t size_of( const storable_node_base< T >& s );
template< typename T > read_stream& operator >>( read_stream& rs, storable_node_base< T >& s );
template< typename T > write_stream& operator <<( write_stream& ws, const storable_node_base< T >& s );

template< typename T > class storable_node_base : public bt_node< T >, public storable_base
{
   public:
   friend int64_t size_of< T >( const storable_node_base< T >& s );

   // NOTE: (see NOTE above)
   friend read_stream& operator >> < T >( read_stream& rs, storable_node_base< T >& s );
   friend write_stream& operator << < T >( write_stream& ws, const storable_node_base< T >& s );

   static const uint8_t c_version = 1;
   static const size_t c_round_to_value = STORABLE_BTREE_NODE_SIZE;
};

template< typename T > int64_t size_of( const storable_node_base< T >& s )
{
   uint8_t num = s.size( );

   size_t total = ( sizeof( uint8_t ) * 2 ) + sizeof( typename storable_node_base< T >::node_data );

   for( uint8_t i = 0; i < num; i++ )
   {
      T& t = const_cast< T& >( s.get_item_data( i ) );
      total += size_of( t ) + sizeof( oid );
   }

   return total;
}

template< typename T > read_stream& operator >>( read_stream& rs, storable_node_base< T >& s )
{
   T t;
   uint64_t link;

   s.reset( );

   uint8_t ver, num;
   rs >> ver >> num;

   // FUTURE: If the persistent data structure is extended then the version should be used in
   // order to handle legacy objects.
   if( ver != storable_node_base< T >::c_version )
      throw std::runtime_error( "found unexpected storable_node_base version: " + to_string( ver ) );

   rs >> s.data.flags >> s.data.padding >> s.data.dge_link >> s.data.lft_link >> s.data.rgt_link;

   for( uint8_t i = 0; i < num; i++ )
   {
      rs >> t >> link;
      s.append_item( t, link );
   }

   return rs;
}

template< typename T > write_stream& operator <<( write_stream& ws, const storable_node_base< T >& s )
{
   uint8_t ver = storable_node_base< T >::c_version;
   uint8_t num = s.size( );

   ws << ver << num
    << s.data.flags << s.data.padding << s.data.dge_link << s.data.lft_link << s.data.rgt_link;

   for( uint8_t i = 0; i < num; i++ )
   {
      T& t = const_cast< T& >( s.get_item_data( i ) );
      ws << t << s.get_item_link( i );
   }

   return ws;
}

const int c_max_buffer_nodes = 10;

template< typename T > class storable_node_manager
 : public bt_node_manager< T, storable< storable_node_base< T >, storable_node_base< T >::c_round_to_value > >
{
   public:
   typedef T item_type;
   typedef storable< storable_node_base< T >, storable_node_base< T >::c_round_to_value > node_type;

   storable_node_manager( ) : p_ods( 0 ) { clear_nodes( ); }

   void set_ods( ods& o ) { p_ods = &o; }

   void clear_nodes( );

   virtual uint64_t create_node( );
   virtual void destroy_node( uint64_t id ) { p_ods->destroy( id ); }

   virtual void access_node( uint64_t id, bt_node< T >*& p_node );

   virtual void commit( );
   virtual void rollback( );

   virtual void reset( ) { clear_nodes( ); }

   private:
   ods* p_ods;

   std::vector< node_type > nodes;
};

template< typename T > void storable_node_manager< T >::clear_nodes( )
{
   nodes.resize( 0 );
   nodes.resize( c_max_buffer_nodes );
}

template< typename T > uint64_t storable_node_manager< T >::create_node( )
{
   size_t index = UINT_MAX;

   for( size_t i = 0; i < nodes.size( ); i++ )
   {
      if( !nodes[ i ].referenced( ) )
      {
         index = i;

         if( !nodes[ i ].touched( ) )
            break;
      }
   }

   if( index == UINT_MAX )
      throw std::runtime_error( "unexpected storable node manager has no room for node in create_node" );

   if( nodes[ index ].touched( ) )
   {
      *p_ods << nodes[ index ];
      nodes[ index ].untouch( );
   }

   node_type node;
   *p_ods << node;

   nodes[ index ] = node;

   return node.get_id( ).get_num( );
}

template< typename T > void storable_node_manager< T >::access_node( uint64_t id, bt_node< T >*& p_node )
{
   size_t index = UINT_MAX;

   for( size_t i = 0; i < nodes.size( ); i++ )
   {
      if( nodes[ i ].get_id( ) == id )
      {
         index = i;
         break;
      }
      else if( !nodes[ i ].referenced( )
       && ( index == UINT_MAX || nodes[ index ].touched( ) ) )
      {
         index = i;
      }
   }

   if( index == UINT_MAX )
      throw std::runtime_error( "unexpected storable node manager has no room for node in access_node" );

   if( !nodes[ index ].referenced( ) && nodes[ index ].get_id( ) != id )
   {
      if( nodes[ index ].touched( ) )
      {
         *p_ods << nodes[ index ];
         nodes[ index ].untouch( );
      }

      nodes[ index ].set_id( id );
      *p_ods >> nodes[ index ];
   }

   nodes[ index ].inc_ref_count( );
   p_node = &nodes[ index ];
}

template< typename T > void storable_node_manager< T >::commit( )
{
   for( size_t i = 0; i < nodes.size( ); i++ )
   {
      if( nodes[ i ].touched( ) )
      {
         *p_ods << nodes[ i ];
         nodes[ i ].untouch( );
      }
   }
}

template< typename T > void storable_node_manager< T >::rollback( )
{
   for( size_t i = 0; i < nodes.size( ); i++ )
   {
      if( nodes[ i ].touched( ) )
      {
         nodes[ i ].set_new( );
         nodes[ i ].untouch( );
      }
   }
}

#  ifdef STORABLE_BTREE_USE_MAGIC
const int32_t c_magic = 1234567890;
#  endif

// NOTE: This approach is necessary to force template instanciation to occur (at least with BCB).
template< typename T, typename L > class storable_btree_base;
template< typename T, typename L > int64_t size_of( const storable_btree_base< T, L >& bt );
template< typename T, typename L > read_stream& operator >>( read_stream& rs, storable_btree_base< T, L >& s );
template< typename T, typename L > write_stream& operator <<( write_stream& ws, const storable_btree_base< T, L >& s );

template< typename T, typename L = std::less< T > > class storable_btree_base
 : public bt_base< T, L,
 storable< storable_node_base< T >, storable_node_base< T >::c_round_to_value >, storable_node_manager< T > >, public storable_base
{
   typedef bt_base< T, L, storable< storable_node_base< T >,
    storable_node_base< T >::c_round_to_value >, storable_node_manager< T > > bt_base_class;

   public:
   storable_btree_base( const L& compare_less = std::less< T >( ) )
    : bt_base< T, L, storable< storable_node_base< T >, storable_node_base< T >::c_round_to_value >,
    storable_node_manager< T > >( compare_less ),
    o( *ods::instance( ) )
   {
      has_written = !o.is_new( );
      prior = storable_btree_base< T, L >::state;

      bt_base_class::get_node_manager( ).set_ods( o );
   }

   storable_btree_base( ods& o, const L& compare_less = std::less< T >( ) )
    : bt_base< T, L, storable< storable_node_base< T >, storable_node_base< T >::c_round_to_value >,
    storable_node_manager< T > >( compare_less ),
    o( o )
   {
      has_written = !o.is_new( );
      prior = storable_btree_base< T, L >::state;

      bt_base_class::get_node_manager( ).set_ods( o );
   }

   friend int64_t size_of< T, L >( const storable_btree_base< T, L >& bt );

   // NOTE: (see NOTE above)
   friend read_stream& operator >> < T, L >( read_stream& rs, storable_btree_base< T, L >& s );
   friend write_stream& operator << < T, L >( write_stream& ws, const storable_btree_base< T, L >& s );

   static const uint8_t c_version = 1;
   static const size_t c_round_to_value = STORABLE_BTREE_SIZE;

   mutable typename storable_btree_base< T, L >::state_t prior;

   private:
   ods& o;

   bool has_written;

   protected:
   virtual void commit( )
   {
      // NOTE: Do not store if state has not changed since was last written.
      if( !has_written || storable_btree_base< T, L >::has_changed( prior ) )
      {
         o << *this;
         has_written = true;
      }
   }
};

template< typename T, typename L > int64_t size_of( const storable_btree_base< T, L >& s )
{
   size_t total = ( sizeof( uint8_t ) * 2 ) + sizeof( typename storable_btree_base< T, L >::state_t );

#  ifdef STORABLE_BTREE_USE_MAGIC
   if( s.o.is_encrypted( ) )
      total += sizeof( int32_t );
#  endif

   return total;
}

template< typename T, typename L > read_stream& operator >>( read_stream& rs, storable_btree_base< T, L >& s )
{
#  ifdef STORABLE_BTREE_USE_MAGIC
   if( s.o.is_encrypted( ) )
   {
      int32_t val;
      rs >> val;

      if( val != c_magic )
         throw std::runtime_error( "invalid password" );
   }
#  endif

   uint8_t ver, items_per_node;

   rs >> ver >> items_per_node;

   // FUTURE: If the persistent data structure is extended then the version should be used in
   // order to handle legacy objects.
   if( ver != storable_btree_base< T, L >::c_version )
      throw std::runtime_error( "found unexpected storable_btree_base version: " + to_string( ver ) );

   rs >> s.state.num_levels
    >> s.state.node_fill_factor >> s.state.allow_duplicates
    >> s.state.padding >> s.state.total_nodes >> s.state.total_items
    >> s.state.root_node >> s.state.lft_leaf_node >> s.state.rgt_leaf_node
    >> s.state.free_list_node >> s.state.first_append_node >> s.state.current_append_node;

   s.prior = s.state;

   s.get_node_manager( ).reset( );

   s.get_node_manager( ).set_items_per_node( items_per_node );

   return rs;
}

template< typename T, typename L >
 write_stream& operator <<( write_stream& ws, const storable_btree_base< T, L >& s )
{
#  ifdef STORABLE_BTREE_USE_MAGIC
   if( s.o.is_encrypted( ) )
      ws << c_magic;
#  endif

   uint8_t ver = storable_btree_base< T, L >::c_version;
   uint8_t items_per_node = s.get_node_manager( ).get_items_per_node( );

   ws << ver << items_per_node << s.state.num_levels
    << s.state.node_fill_factor << s.state.allow_duplicates
    << s.state.padding << s.state.total_nodes << s.state.total_items
    << s.state.root_node << s.state.lft_leaf_node << s.state.rgt_leaf_node
    << s.state.free_list_node << s.state.first_append_node << s.state.current_append_node;

   s.prior = s.state;

   return ws;
}

#endif

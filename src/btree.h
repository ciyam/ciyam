// Copyright (c) 2000-2015 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef BTREE_H
#  define BTREE_H

//#  define BTREE_DEBUG

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <deque>
#     include <vector>
#     include <memory>
#     include <stdexcept>
#     include <algorithm>
#     include <functional>
#     ifdef BTREE_DEBUG
#        include <iostream>
#     endif
#  endif

#  include "macros.h"
#  include "pointers.h"
#  include "utilities.h"

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif

#  define USE_BTREE_NAMESPACE

#  ifdef USE_BTREE_NAMESPACE
namespace btree
{
#  endif

#  ifndef __GNUG__
typedef unsigned int uint;
#  endif
typedef unsigned char uchar;

const uint c_npos = ~0u;
const uchar c_none = 0xffu;

const uchar c_node_flag_is_leaf = 0x01u;
const uchar c_node_flag_is_rgt_leaf = 0x02u;
const uchar c_node_flag_has_dup_split = 0x04u;

const uchar c_maximum_items_per_node = 255;

#  ifdef BTREE_DEBUG
size_t total_number_of_node_splits = 0;
#  endif

template< typename T > class bt_node;

template< typename T > class bt_node_mgr_base
{
   public:
   bt_node_mgr_base( ) { }

   virtual ~bt_node_mgr_base( ) { }

   virtual uint create_node( ) = 0;
   virtual void destroy_node( uint ) { }

   virtual void access_node( uint id, bt_node< T >*& p_node ) = 0;

   virtual void commit( ) { }
   virtual void rollback( ) { }

   virtual void reset( ) { }
};

template< typename T > class bt_node_ref;

template< typename T > class bt_node
{
   template< typename T1 > friend class bt_node_ref;

   public:
   typedef T item_type;

   bt_node( )
    :
    link( c_npos ),
    ref_count( 0 ),
    was_touched( false )
   {
   }

   virtual ~bt_node( ) { }

   void touch( ) { was_touched = true; }

   void untouch( ) { was_touched = false; }

   bool touched( ) const { return was_touched; }

   void inc_ref_count( ) { ++ref_count; }

   void dec_ref_count( ) { --ref_count; }

   bool referenced( ) const { return ref_count > 0; }

   void append_item( const T& item, uint link )
   {
      item_pairs.push_back( std::make_pair( item, link ) );
   }

   void copy_items( size_t pos, bt_node< T >& dest )
   {
      std::copy( item_pairs.begin( ) + pos,
       item_pairs.end( ), std::back_inserter( dest.item_pairs ) );
   }

   void erase_item( size_t pos )
   {
      item_pairs.erase( item_pairs.begin( ) + pos );
   }

   void erase_items( size_t pos )
   {
      item_pairs.erase( item_pairs.begin( ) + pos, item_pairs.end( ) );
   }

   void insert_item( size_t pos, const T& item, uint link )
   {
      item_pairs.insert( item_pairs.begin( ) + pos, std::make_pair( item, link ) );
   }

   void clear_items( )
   {
      item_pairs.clear( );
   }

   void resize_items( size_t n )
   {
      item_pairs.resize( n );
   }

   void reset( )
   {
      ref_count = 0;
      was_touched = false;

      data.flags = c_node_flag_is_leaf;
      data.dge_link = data.lft_link = data.rgt_link = c_npos;

      item_pairs.clear( );
   }

   uchar size( ) const { return ( uchar )item_pairs.size( ); }

   void set_link( int new_link ) { link = new_link; }

   const T& get_item_data( size_t pos ) const { return item_pairs[ pos ].first; }
   uint get_item_link( size_t pos ) const { return item_pairs[ pos ].second; }

   void set_item_link( size_t pos, uint link ) { item_pairs[ pos ].second = link; }

   struct node_data;
   node_data& ref_data( ) { return data; }

   protected:
   uint link;
   uint ref_count;
   bool was_touched;

   struct node_data
   {
      node_data( ) : flags( c_node_flag_is_leaf ),
       dge_link( c_npos ), lft_link( c_npos ), rgt_link( c_npos ) { }

      uchar flags;

      uint dge_link;
      uint lft_link;
      uint rgt_link;
   } data;

   std::deque< std::pair< T, uint > > item_pairs;
};

template< typename T > class bt_node_ref
{
   public:
   bt_node_ref( uint id, bt_node_mgr_base< T >& node_manager )
    :
    id( id )
   {
      node_manager.access_node( id, p_node );
   }

   ~bt_node_ref( )
   {
      p_node->dec_ref_count( );
   }

   bt_node< T >& get_node( )
   {
      return *p_node;
   }

   bt_node< T >* operator ->( ) const
   {
      return p_node;
   }

   private:
   bt_node_ref( const bt_node_ref& );
   bt_node_ref& operator =( const bt_node_ref& );

   uint id;
   bt_node< T >* p_node;
};

template< typename T, typename N = bt_node< T > > class bt_node_manager : public bt_node_mgr_base< T >
{
   public:
   typedef T item_type;
   typedef N node_type;

   bt_node_manager( ) : items_per_node( c_maximum_items_per_node ) { }

   uchar get_items_per_node( ) const { return items_per_node; }

   void set_items_per_node( uchar val )
   {
      if( val % 2 == 0 || val < 3 )
         throw std::runtime_error( "unexpected invalid items per node" );

      items_per_node = val;
   }

   private:
   uchar items_per_node;
};

const size_t c_default_num_to_reserve = 1000;
const uchar c_default_num_items_per_node = 5;

template< typename T, typename N = bt_node< T > > class heap_node_manager : public bt_node_manager< T, N >
{
   public:
   typedef T item_type;
   typedef N node_type;

   typedef bt_node_manager< T, N > bt_base_type;

   heap_node_manager( size_t num_to_reserve = c_default_num_to_reserve )
    : max_node( 0 )
   {
      reserve( num_to_reserve );
      bt_base_type::set_items_per_node( c_default_num_items_per_node );
   }

   virtual uint create_node( );

   virtual void reset( ) { clear( ); }

   virtual void access_node( uint id, node_type*& p_node )
   {
      nodes[ id ].inc_ref_count( );
      p_node = &nodes[ id ];
   }

   protected:
   void setmax( size_t s ) { max_node = s; }
   void reserve( size_t size ) { nodes.reserve( size ); }

   uint size( ) { return nodes.size( ); }

   void clear( ) { nodes.resize( 0 ); max_node = 0; }

   void push_back( const node_type& node )
   {
      if( nodes.size( ) == nodes.capacity( ) )
         throw std::runtime_error( "max number of permitted nodes exceeded" );
      nodes.push_back( node );
   }

   node_type& get_node_ref( uint index )
   {
      return nodes[ index ];
   }

   private:
   uint max_node;

   std::vector< node_type > nodes;
};

template< typename T, typename N > uint heap_node_manager< T, N >::create_node( )
{
   if( max_node != nodes.size( ) )
      throw std::runtime_error( "unexpected max node value" );

   push_back( node_type( ) );
   nodes[ max_node ].set_link( max_node );

   return max_node++;
}

template< typename T, typename L, typename N, typename M > class bt_transaction;

template< typename T, typename L = std::less< T >,
 typename N = bt_node< T >, typename M = heap_node_manager< T, N > > class bt_base
{
   public:
   typedef T item_type;

   typedef L key_compare;
   typedef L value_compare;

   typedef T value_type;
   typedef value_type key_type;

   typedef const value_type* const_pointer;
   typedef const value_type& const_reference;

   typedef size_t size_type;
   typedef ptrdiff_t difference_type;

   typedef const_pointer pointer;
   typedef const_reference reference;

   template< typename T1, typename T2, typename T3, typename T4 > friend class bt_transaction;

   class const_iterator;
   friend class const_iterator;

   explicit bt_base( const L& compare_less = std::less< item_type >( ) )
    :
    is_okay( true ),
    p_transaction( 0 ),
    compare_less( compare_less )
   {
   }

   bt_base( const bt_base< T, L, N, M >& other );

   template< typename I > bt_base( I it_beg, I it_end, const L& compare_less = std::less< item_type >( ) );

   virtual ~bt_base( ) { }

   bool okay( ) const { return is_okay; }

   bool get_allow_duplicates( ) const { return state.allow_duplicates; }
   void set_allow_duplicates( bool val ) { state.allow_duplicates = val; }

   void set_fill_factor( float val ) { state.node_fill_factor = val; }

   uchar get_items_per_node( ) const { return node_manager.get_items_per_node( ); }
   void set_items_per_node( uchar val );

   key_compare key_comp( ) const { return compare_less; }

   void dump_root( std::ostream& outs ) const;

   void dump_node( std::ostream& outs, uint num ) const;

   void dump_all_info( std::ostream& outs ) const;

   class const_reverse_iterator;

   class const_iterator : public std::iterator< std::random_access_iterator_tag, T >
   {
      template< typename T1, typename T2, typename T3, typename T4 > friend class bt_base;

      protected:
      uint node;
      uchar item;

      const bt_base< T, L, N, M >* p_bt_base;
      ref_count_ptr< bt_node_ref< T > > rp_node_ref;

      const_iterator( const bt_base< T, L, N, M >* p_bt_base, uint new_node );

      const_iterator( const bt_base< T, L, N, M >* p_bt_base, uint node, uchar item )
       : p_bt_base( p_bt_base ), node( node ), item( item )
      {
         rp_node_ref = p_bt_base->allocate_node_ref( node );
      }

      public:
      const_iterator( )
       : p_bt_base( 0 ), node( c_npos ), item( c_none )
      {
      }

      ~const_iterator( ) { }

      void unbind( );

      void rebind( );

      bool operator ==( const const_iterator& other ) const
      {
         return node == other.node && item == other.item;
      }

      bool operator !=( const const_iterator& other ) const
      {
         return !( *this == other );
      }

      const_reference operator *( ) const
      {
         return rp_node_ref->get_node( ).get_item_data( item );
      }

      const_pointer operator ->( ) const
      {
         return &rp_node_ref->get_node( ).get_item_data( item );
      }

      const_iterator& operator ++( );
      const_iterator operator ++( int );

      const_iterator& operator --( );
      const_iterator operator --( int );
   };

   class const_reverse_iterator : public std::iterator< std::random_access_iterator_tag, T >
   {
      friend class const_iterator;

      public:
      const_reverse_iterator( ) { }
      explicit const_reverse_iterator( const const_iterator& iter ) : current( iter ) { }

      void unbind( )
      {
         current.unbind( );
      }

      void rebind( )
      {
         current.rebind( );
      }

      bool operator ==( const const_reverse_iterator& x ) const
      {
         return current == x.current;
      }

      bool operator !=( const const_reverse_iterator& x ) const
      {
         return !( this->current == x.current );
      }

      // IMPORTANT: The "decrement operator" can change which node is actually being referenced
      // so a "function scoped" temporary iterator is not usuable here (i.e. if an iterator has
      // "gone out of scope" it is quite possible that the node it is pointing can no longer be
      // referenced).
      const_reference operator *( ) const
      {
         tmp = current;
         return *--tmp;
      }

      const_iterator base( ) const { return current; }

      const_pointer operator ->( ) const
      {
         const_reference tmp( operator *( ) );
         return static_cast< const_pointer >( &tmp );
      }

      const_reverse_iterator& operator ++( )
      {
         --current;
         return *this;
      }

      const_reverse_iterator operator ++( int )
      {
         const_reverse_iterator tmp = *this;
         --current;
         return tmp;
      }

      const_reverse_iterator& operator --( )
      {
         ++current;
         return *this;
      }

      const_reverse_iterator operator --( int )
      {
         const_reverse_iterator tmp = *this;
         ++current;
         return tmp;
      }

      private:
      mutable const_iterator tmp;
      const_iterator current;
   };

   typedef const_iterator iterator;
   typedef const_reverse_iterator reverse_iterator;

   const_iterator begin( ) const
   {
      return const_iterator( this, state.lft_leaf_node );
   }

   const_iterator end( ) const
   {
      return const_iterator( this, c_npos );
   }

   const_reverse_iterator rbegin( ) const;

   const_reverse_iterator rend( ) const;

   const_iterator operator [ ]( size_type n ) const;

   size_type size( ) const { return state.total_items; }
   size_type max_size( ) const { return UINT_MAX; }

   bool empty( ) const { return state.total_items == 0; }

   const_iterator find( const key_type& key ) const;

   const_iterator lower_bound( const key_type& key ) const;

   const_iterator upper_bound( const key_type& key ) const;

   size_t count( const key_type& key ) const;

   std::pair< const_iterator, const_iterator > equal_range( const key_type& key ) const;

   void append( const T& item );

   const_iterator insert( const T& item );

   template< typename I > void insert( I it_beg, I it_end );

   size_type erase( const key_type& key );

   void erase( const_iterator& start, const_iterator& finish );

   void erase( const_iterator& position );

   void clear( );

   void build_index_nodes( );

   private:
   uint insert_item( uint& node_link, T& item,
    uint& last_insert_node, uchar& last_insert_item, uint& new_duplicate_dge_node );

   enum find_type
   {
      e_find_equal_to,
      e_find_greater_than,
      e_find_equal_to_or_greater_than
   };

   int find_item( uint& node_link, const key_type& key, find_type type_of_find ) const;

   bool keys_are_equal( const key_type& lhs, const key_type& rhs ) const;

   bool is_okay;
   mutable M node_manager;
   bt_transaction< T, L, N, M >* p_transaction;

   L compare_less;

   // NOTE: The "swap" function and "assignment" operator are yet to be implemented.
   void swap( bt_base< T, L, N, M >& other );
   bt_base< T, L, N, M >& operator =( const bt_base< T, L, N, M >& other );

   protected:
   M& get_node_manager( ) const { return node_manager; }
   bt_node_ref< T >* allocate_node_ref( uint link ) const;

   struct state_t
   {
      state_t( )
       :
       num_levels( 0 ),
       total_nodes( 0 ),
       total_items( 0 ),
       root_node( c_npos ),
       lft_leaf_node( c_npos ),
       rgt_leaf_node( c_npos ),
       free_list_node( c_npos ),
       first_append_node( c_npos ),
       current_append_node( c_npos ),
       node_fill_factor( ( float )0.85 ),
       allow_duplicates( true )
      {
      }

      uchar num_levels;

      size_t total_nodes;
      size_t total_items;

      uint root_node;
      uint lft_leaf_node;
      uint rgt_leaf_node;
      uint free_list_node;
      uint first_append_node;
      uint current_append_node;

      float node_fill_factor;
      bool allow_duplicates;
   } state;

   state_t& get_state( ) { return state; }

   virtual void commit( ) { }
   virtual void rollback( ) { }

   virtual bool allow_auto_rollback( ) const { return true; }
};

template< typename T, typename L = std::less< T >,
 typename N = bt_node< T >, typename M = heap_node_manager< T, N > > class bt_transaction
{
   template< typename T1, typename T2, typename T3, typename T4 > friend class bt_base;

   public:
   bt_transaction( bt_base< T, L, N, M >& btree_base );

   ~bt_transaction( );

   void commit( );
   void rollback( );

   private:
   bt_transaction( bt_base< T, L, N, M >& btree_base, bool );

   bool completed;
   uint first_append_node;
   bt_base< T, L, N, M >& btree_base;

   restorable< typename bt_base< T, L, N, M >::state_t > bt_base_state;
   restorable< bt_transaction< T, L, N, M >* > p_bt_transaction;
};

template< typename T, typename L, typename N, typename M >
 bt_base< T, L, N, M >::bt_base( const bt_base< T, L, N, M >& other )
 :
 is_okay( true ),
 p_transaction( 0 ),
 compare_less( other.compare_less )
{
   if( !other.is_okay || other.p_transaction )
      throw std::runtime_error( "attempt to copy invalid btree" );

   // NOTE: This copy constructor could be far more efficient if it copied "whole nodes" rather
   // individual items (could this be handled directly using each B+Tree's "node manager" ???).
   for( iterator i = other.begin( ); i != other.end( ); ++i )
      insert( *i );
}

template< typename T, typename L, typename N, typename M >
 template< typename I > bt_base< T, L, N, M >::bt_base( I it_beg, I it_end, const L& compare_less )
 :
 is_okay( true ),
 p_transaction( 0 ),
 compare_less( compare_less )
{
   for( I i = it_beg; i != it_end; ++i )
      insert( *i );
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::set_items_per_node( uchar val )
{
   if( state.total_nodes && val < node_manager.get_items_per_node( ) )
      throw std::runtime_error( "cannot shrink items per node" );

   node_manager.set_items_per_node( val );
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::dump_root( std::ostream& outs ) const
{
   std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( state.root_node ) );

   outs << "[Root " << state.root_node << "] flags = "
    << ( int )ap_node_ref->get_node( ).ref_data( ).flags
    << ", dge_link = " << ( int )ap_node_ref->get_node( ).ref_data( ).dge_link << '\n';

   for( int i = 0; i < ap_node_ref->get_node( ).size( ); i++ )
      outs << "Item #" << i << ", data = "
       << ap_node_ref->get_node( ).get_item_data( i )
       << ", link = " << ( int )ap_node_ref->get_node( ).get_item_link( i ) << '\n';
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::dump_node( std::ostream& outs, uint num ) const
{
   std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( num ) );

   outs << "[Node " << num
    << "] flags = " << ( int )ap_node_ref->get_node( ).ref_data( ).flags
    << ", dge_link = " << ( int )ap_node_ref->get_node( ).ref_data( ).dge_link
    << "\n         lft_link = " << ( int )ap_node_ref->get_node( ).ref_data( ).lft_link
    << ", rgt_link = " << ( int )ap_node_ref->get_node( ).ref_data( ).rgt_link << '\n';

   for( int i = 0; i < ap_node_ref->get_node( ).size( ); i++ )
      outs << "Item #" << i << ", data = "
       << ap_node_ref->get_node( ).get_item_data( i )
       << ", link = " << ( int )ap_node_ref->get_node( ).get_item_link( i ) << '\n';
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::dump_all_info( std::ostream& outs ) const
{
   outs << "Total index levels = " << ( int )state.num_levels;
   outs << "\nTotal number of nodes = " << state.total_nodes;
   outs << "\nTotal number of items = " << state.total_items << "\n\n";

   int level = 0;
   uint first_node_in_level( state.root_node );
   while( first_node_in_level != c_npos )
   {
      if( level > 0 )
         outs << '\n';

      outs << "Dumping level #" << level++ << '\n';
      uint current_node( first_node_in_level );

      first_node_in_level = c_npos;
      while( true )
      {
         std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( current_node ) );

         if( first_node_in_level == c_npos && ap_node_ref->get_node( ).size( ) > 0 )
            first_node_in_level = ap_node_ref->get_node( ).get_item_link( 0 );

         dump_node( outs, current_node );
         current_node = ap_node_ref->get_node( ).ref_data( ).rgt_link;
         if( current_node == c_npos )
            break;
         outs << '\n';
      }

      if( state.first_append_node != c_npos )
         outs << '\n';
   }

   if( state.first_append_node != c_npos )
   {
      uint current_node( state.first_append_node );
      outs << "Dumping appended node(s)\n";

      while( true )
      {
         std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( current_node ) );

         dump_node( outs, current_node );
         current_node = ap_node_ref->get_node( ).ref_data( ).rgt_link;

         if( current_node == c_npos )
            break;
         outs << '\n';
      }
   }
}

template< typename T, typename L, typename N, typename M >
 bt_base< T, L, N, M >::const_iterator::const_iterator(
 const bt_base< T, L, N, M >* p_bt_base, uint new_node )
 :
 p_bt_base( p_bt_base ),
 node( new_node )
{
   if( node == c_npos || p_bt_base->state.total_items == 0 )
   {
      node = c_npos;
      item = c_none;
   }
   else
   {
      rp_node_ref = p_bt_base->allocate_node_ref( node );

      while( rp_node_ref->get_node( ).size( ) == 0 )
      {
         node = rp_node_ref->get_node( ).ref_data( ).rgt_link;
         if( node == c_npos )
            throw std::runtime_error( "unexpected empty node" );

         rp_node_ref = p_bt_base->allocate_node_ref( node );
      }

      item = 0;
   }
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::const_iterator::unbind( )
{
   rp_node_ref = 0;
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::const_iterator::rebind( )
{
   if( node != c_npos )
      rp_node_ref = p_bt_base->allocate_node_ref( node );
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_iterator& bt_base< T, L, N, M >::const_iterator::operator ++( )
{
   if( item == c_none )
      throw std::runtime_error( "invalid iterator #0" );

   if( item < rp_node_ref->get_node( ).size( ) - 1 )
      ++item;
   else
   {
      while( true )
      {
         node = rp_node_ref->get_node( ).ref_data( ).rgt_link;
         if( rp_node_ref->get_node( ).ref_data( ).rgt_link == c_npos )
         {
            rp_node_ref = 0;
            item = c_none;
            break;
         }

         rp_node_ref = p_bt_base->allocate_node_ref( node );

         if( rp_node_ref->get_node( ).size( ) == 0 )
            continue;

         item = 0;
         break;
      }
   }

#  ifdef BTREE_DEBUG
   std::cout << "operator ++( ): node = " << node << '\n';
#  endif
   return *this;
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_iterator bt_base< T, L, N, M >::const_iterator::operator ++( int )
{
   const_iterator tmp = *this;
   ++*this;
   return tmp;
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_iterator& bt_base< T, L, N, M >::const_iterator::operator --( )
{
   if( item == c_none )
   {
      if( p_bt_base && p_bt_base->state.total_items )
      {
         node = p_bt_base->state.rgt_leaf_node;

         bool skipped = false;
         while( true )
         {
            rp_node_ref = p_bt_base->allocate_node_ref( node );

            // NOTE: If the "right leaf node" is in fact the start of a duplicate node sub-list
            // then skip forwards to the last node of the duplicate sub-list and continue.
            if( !skipped && node == p_bt_base->state.rgt_leaf_node
             && rp_node_ref->get_node( ).ref_data( ).dge_link != c_npos )
            {
               skipped = true;
               node = rp_node_ref->get_node( ).ref_data( ).dge_link;
               continue;
            }

            if( rp_node_ref->get_node( ).size( ) != 0 )
               break;

            node = rp_node_ref->get_node( ).ref_data( ).lft_link;
         }

         item = ( uchar )( rp_node_ref->get_node( ).size( ) - 1 );
      }
      else
         throw std::runtime_error( "invalid iterator #1" );

      return *this;
   }

   if( node == p_bt_base->state.lft_leaf_node && item == 0 )
      throw std::runtime_error( "invalid iterator #2" );

   if( item > 0 )
      --item;
   else
   {
      while( true )
      {
         if( rp_node_ref->get_node( ).ref_data( ).lft_link == c_npos )
            break;

         node = rp_node_ref->get_node( ).ref_data( ).lft_link;

         rp_node_ref = p_bt_base->allocate_node_ref( node );

         if( rp_node_ref->get_node( ).size( ) == 0 )
            continue;

         item = ( uchar )( rp_node_ref->get_node( ).size( ) - 1 );
         break;
      }
   }

#  ifdef BTREE_DEBUG
   std::cout << "operator --( ): node = " << node << '\n';
#  endif
   return *this;
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_iterator bt_base< T, L, N, M >::const_iterator::operator --( int )
{
   const_iterator tmp = *this;
   --*this;
   return tmp;
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_reverse_iterator bt_base< T, L, N, M >::rbegin( ) const
{
   const_reverse_iterator tmp( end( ) );
   return tmp;
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_reverse_iterator bt_base< T, L, N, M >::rend( ) const
{
   const_reverse_iterator tmp( begin( ) );
   return tmp;
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_iterator bt_base< T, L, N, M >::find( const key_type& key ) const
{
#  ifdef BTREE_DEBUG
   std::cout << "find: " << key << '\n';
#  endif
   if( state.root_node == c_npos )
      return const_iterator( this, c_npos );

   uint node_found( state.root_node );
   int item_pos = find_item( node_found, key, e_find_equal_to );
   if( item_pos != -1 )
      return const_iterator( this, node_found, ( uchar )item_pos );
   else
      return const_iterator( this, c_npos );
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_iterator
 bt_base< T, L, N, M >::lower_bound( const key_type& key ) const
{
#  ifdef BTREE_DEBUG
   std::cout << "lower_bound: " << key << '\n';
#  endif
   if( state.root_node == c_npos )
      return const_iterator( this, c_npos );

   uint node_found( state.root_node );
   int item_pos = find_item( node_found, key, e_find_equal_to_or_greater_than );
   if( item_pos != -1 )
      return const_iterator( this, node_found, ( uchar )item_pos );
   else
      return const_iterator( this, c_npos );
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_iterator
 bt_base< T, L, N, M >::upper_bound( const key_type& key ) const
{
#  ifdef BTREE_DEBUG
   std::cout << "upper_bound: " << key << '\n';
#  endif
   if( state.root_node == c_npos )
      return const_iterator( this, c_npos );

   uint node_found( state.root_node );
   int item_pos = find_item( node_found, key, e_find_greater_than );
   if( item_pos != -1 )
      return const_iterator( this, node_found, ( uchar )item_pos );
   else
      return const_iterator( this, c_npos );
}

template< typename T, typename L, typename N, typename M >
 int bt_base< T, L, N, M >::find_item( uint& node_link, const key_type& key, find_type type_of_find ) const
{
   uchar s;
   int pos = -1;

#  ifdef BTREE_DEBUG
   std::cout << "find_item: node_link = " << node_link << '\n';
#  endif
   std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( node_link ) );

   for( s = 0; s < ap_node_ref->get_node( ).size( ); s++ )
   {
      bool is_equal = false;
#  ifdef BTREE_DEBUG
      std::cout << "comparing item #" << ( int )s << '\n';
#  endif
      if( compare_less( key, ap_node_ref->get_node( ).get_item_data( s ) ) )
      {
         if( !( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf ) )
         {
            node_link = ap_node_ref->get_node( ).get_item_link( s );
            pos = find_item( node_link, key, type_of_find );
            break;
         }
         else if( type_of_find != e_find_equal_to )
         {
            pos = s;
            break;
         }
         else
            break;
      }
      else if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf )
      {
         if( !compare_less( ap_node_ref->get_node( ).get_item_data( s ), key ) )
            is_equal = true;

         if( is_equal && type_of_find != e_find_greater_than )
         {
            pos = s;
            break;
         }

         if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split )
         {
            node_link = ap_node_ref->get_node( ).ref_data( ).dge_link;

            do
            {
               ap_node_ref.reset( allocate_node_ref( node_link ) );

               if( ap_node_ref->get_node( ).size( ) )
               {
                  s = c_none;
                  break;
               }

               node_link = ap_node_ref->get_node( ).ref_data( ).rgt_link;
            } while( ap_node_ref->get_node( ).ref_data( ).rgt_link != c_npos );
#  ifdef BTREE_DEBUG
            std::cout << "node_link = " << node_link << '\n';
#  endif
         }
         else if( type_of_find == e_find_greater_than )
         {
            if( s == ap_node_ref->get_node( ).size( ) - 1 )
            {
               while( ap_node_ref->get_node( ).ref_data( ).rgt_link != c_npos )
               {
                  node_link = ap_node_ref->get_node( ).ref_data( ).rgt_link;

                  ap_node_ref.reset( allocate_node_ref( node_link ) );

                  if( ap_node_ref->get_node( ).size( ) )
                  {
                     s = c_none;
                     break;
                  }
               }
#  ifdef BTREE_DEBUG
               std::cout << "node_link = " << node_link << '\n';
#  endif
            }
         }
      }
   }

   if( s == ap_node_ref->get_node( ).size( ) )
   {
      if( !( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf ) )
      {
         node_link = ap_node_ref->get_node( ).ref_data( ).dge_link;
         if( node_link != c_npos )
            pos = find_item( node_link, key, type_of_find );
      }
   }

   if( pos == -1 && type_of_find == e_find_equal_to_or_greater_than )
   {
      if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf )
      {
         if( s < ap_node_ref->get_node( ).size( ) )
            pos = s;
         else
         {
            bool has_next_link = false;
            while( ap_node_ref->get_node( ).ref_data( ).rgt_link != c_npos )
            {
               node_link = ap_node_ref->get_node( ).ref_data( ).rgt_link;

               ap_node_ref.reset( allocate_node_ref( node_link ) );

               if( ap_node_ref->get_node( ).size( ) )
               {
#  ifdef BTREE_DEBUG
                  std::cout << "node_link = " << node_link << '\n';
#  endif
                  has_next_link = true;
                  break;
               }
            }

            if( has_next_link )
               pos = find_item( node_link, key, type_of_find );
         }
      }
   }

   return pos;
}

template< typename T, typename L, typename N, typename M >
 size_t bt_base< T, L, N, M >::count( const key_type& key ) const
{
   size_t retval = 0;
   const_iterator i( find( key ) );

   for( ; i != end( ); ++i )
   {
      if( !compare_less( key, *i ) && !compare_less( *i, key ) )
         ++retval;
      else
         break;
   }

   return retval;
}

template< typename T, typename L, typename N, typename M >
 std::pair< typename bt_base< T, L, N, M >::const_iterator, typename bt_base< T, L, N, M >::const_iterator >
 bt_base< T, L, N, M >::equal_range( const key_type& key ) const
{
   std::pair< const_iterator, const_iterator > retval;

   retval.first = lower_bound( key );
   retval.second = upper_bound( key );

   return retval;
}

template< typename T, typename L, typename N, typename M >
 bool bt_base< T, L, N, M >::keys_are_equal( const key_type& lhs, const key_type& rhs ) const
{
   return !compare_less( lhs, rhs ) && !compare_less( rhs, lhs );
}

template< typename T, typename L, typename N, typename M >
 bt_node_ref< T >* bt_base< T, L, N, M >::allocate_node_ref( uint link ) const
{
   return new bt_node_ref< T >( link, node_manager );
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::append( const T& item )
{
   bt_transaction< T, L, N, M > transaction( *this, true );

   if( state.current_append_node == c_npos )
   {
      state.first_append_node = state.current_append_node = node_manager.create_node( );

      ++state.total_nodes;
   }

   std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( state.current_append_node ) );

   uchar size = ap_node_ref->get_node( ).size( );

   bool is_duplicate = false;

   if( size == 0 && state.total_items )
   {
      typename bt_base< T, L, N, M >::const_iterator iter = end( );

      --iter;

      if( compare_less( item, *iter ) )
         throw std::runtime_error( "invalid attempt to append #0" );

      // IMPORTANT: The first item being appended cannot be equal to the last existing item (as
      // the duplicate sub-list handling for this special case has not been implemented).
      if( keys_are_equal( item, *iter ) )
         throw std::runtime_error( "invalid attempt to append #1" );
   }
   else if( size != 0 )
   {
      if( compare_less( item, ap_node_ref->get_node( ).get_item_data( size - 1 ) ) )
         throw std::runtime_error( "invalid attempt to append #2" );

      if( keys_are_equal( item, ap_node_ref->get_node( ).get_item_data( size - 1 ) ) )
      {
         if( !state.allow_duplicates )
            throw std::runtime_error( "invalid attempt to append #3" );
         is_duplicate = true;
      }
   }

   float fill_factor( state.node_fill_factor );
   if( is_duplicate )
      fill_factor = 1.0;

   uchar items_per_node( node_manager.get_items_per_node( ) );

   if( size < ( uchar )( items_per_node * fill_factor )
    && ( is_duplicate || ap_node_ref->get_node( ).ref_data( ).dge_link == c_npos ) )
      ap_node_ref->get_node( ).append_item( item, c_npos );
   else
   {
      uint new_append_node = node_manager.create_node( );

      ++state.total_nodes;

      std::auto_ptr< bt_node_ref< T > > ap_new_node_ref( allocate_node_ref( new_append_node ) );

      ap_new_node_ref->get_node( ).ref_data( ).lft_link = state.current_append_node;

      if( is_duplicate )
      {
         uchar first_dup;
         for( first_dup = 0; first_dup < items_per_node; first_dup++ )
         {
            if( keys_are_equal( item, ap_node_ref->get_node( ).get_item_data( first_dup ) ) )
               break;
         }

         if( first_dup != 0 )
         {
            for( uchar i = first_dup; i < items_per_node; i++ )
               ap_new_node_ref->get_node( ).append_item(
                ap_node_ref->get_node( ).get_item_data( i ),
                ap_node_ref->get_node( ).get_item_link( i ) );

            ap_node_ref->get_node( ).erase_items( first_dup );
         }
         else
         {
            uint dup_link( ap_node_ref->get_node( ).ref_data( ).dge_link );
            if( dup_link != c_npos )
            {
               std::auto_ptr< bt_node_ref< T > > ap_dup_node_ref( allocate_node_ref( dup_link ) );
               ap_dup_node_ref->get_node( ).ref_data( ).dge_link = new_append_node;
               ap_dup_node_ref->get_node( ).touch( );

               ap_node_ref->get_node( ).ref_data( ).dge_link = c_npos;
               ap_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_has_dup_split;

               ap_new_node_ref->get_node( ).ref_data( ).dge_link = dup_link;
            }
            else
            {
               ap_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_has_dup_split;
               ap_node_ref->get_node( ).ref_data( ).dge_link = new_append_node;
               ap_new_node_ref->get_node( ).ref_data( ).dge_link = state.current_append_node;
            }
         }
      }

      ap_new_node_ref->get_node( ).append_item( item, c_npos );

      ap_node_ref->get_node( ).ref_data( ).rgt_link = new_append_node;
      ap_node_ref->get_node( ).touch( );

      state.current_append_node = new_append_node;

      ap_node_ref = ap_new_node_ref;
   }

   size = ap_node_ref->get_node( ).size( );

   ap_node_ref->get_node( ).touch( );

   transaction.commit( );
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::const_iterator bt_base< T, L, N, M >::insert( const T& item )
{
#  ifdef BTREE_DEBUG
   std::cout << "insert: " << item << '\n';
#  endif
   if( state.current_append_node != c_npos )
      throw std::runtime_error( "attempt to insert whilst appending" );

   bt_transaction< T, L, N, M > transaction( *this, true );

   if( state.root_node == c_npos )
   {
      state.root_node = state.lft_leaf_node = state.rgt_leaf_node = node_manager.create_node( );

      ++state.total_nodes;

      std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( state.root_node ) );
      ap_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_is_rgt_leaf;
      ap_node_ref->get_node( ).touch( );
   }

   uint last_insert_node = c_npos;
   uchar last_insert_item = c_none;

   uint new_duplicate_dge_node = c_npos;

   T tmp_item( item );
   uint tmp_link( state.root_node );
   uint link = insert_item( tmp_link, tmp_item, last_insert_node, last_insert_item, new_duplicate_dge_node );

   ++state.total_items;

   if( link != c_npos )
   {
#  ifdef BTREE_DEBUG
      std::cout << "create new root node\n";
#  endif
      uint left( state.root_node );
      if( state.free_list_node != c_npos )
         state.root_node = state.free_list_node;
      else
      {
         state.root_node = node_manager.create_node( );
         ++state.total_nodes;
      }

      std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( state.root_node ) );

      if( state.free_list_node != c_npos )
      {
         state.free_list_node = ap_node_ref->get_node( ).ref_data( ).dge_link;
         ap_node_ref->get_node( ).reset( );
      }

      ap_node_ref->get_node( ).ref_data( ).dge_link = link;
      ap_node_ref->get_node( ).ref_data( ).flags &= ~c_node_flag_is_leaf;

      ap_node_ref->get_node( ).append_item( tmp_item, left );

      ap_node_ref->get_node( ).touch( );
      ++state.num_levels;
#  ifdef BTREE_DEBUG
      dump_root( std::cout );
#  endif
   }

   transaction.commit( );
#  ifdef BTREE_DEBUG
   std::cout << "*** DUMP ALL INFO *** \n";
   dump_all_info( std::cout );
   std::cout << std::endl;
#  endif
   return const_iterator( this, last_insert_node, last_insert_item );
}

template< typename T, typename L, typename N, typename M >
 template< typename I > void bt_base< T, L, N, M >::insert( I it_beg, I it_end )
{
   for( I i = it_beg; i != it_end; ++i )
      insert( *i );
}

template< typename T, typename L, typename N, typename M >
 uint bt_base< T, L, N, M >::insert_item( uint& node_link,
 T& item, uint& last_insert_node, uchar& last_insert_item, uint& new_duplicate_dge_node )
{
   uchar s = 0;
   int pos = -1;

   uint link = c_npos;
   uint new_link = c_npos;
   uint dup_node_link = c_npos;

   bool has_changed = false;

#  ifdef BTREE_DEBUG
   std::cout << "insert_item: node_link = " << node_link << '\n';
#  endif
   if( node_link == c_npos )
      throw std::runtime_error( "bad node link #0" );

   std::auto_ptr< bt_node_ref< T > > ap_node_ref( allocate_node_ref( node_link ) );

   uchar size = ap_node_ref->get_node( ).size( );

   while( true )
   {
      if( size == 0 )
      {
         pos = 0;
         break;
      }

      if( !compare_less( item, ap_node_ref->get_node( ).get_item_data( size - 1 ) ) )
      {
         if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split )
         {
            s = 0;
            dup_node_link = node_link;
            node_link = ap_node_ref->get_node( ).ref_data( ).dge_link;

            ap_node_ref.reset( allocate_node_ref( node_link ) );

            size = ap_node_ref->get_node( ).size( );
            continue;
         }
         else
         {
            s = size;
            if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf )
               dup_node_link = ap_node_ref->get_node( ).ref_data( ).dge_link;
            break;
         }
      }
      else if( size == 1 || compare_less( item, ap_node_ref->get_node( ).get_item_data( 0 ) ) )
      {
         s = 0;
      }
      else
      {
         s = ( uchar )( size - 1 );

         if( size >= 2 )
         {
            uchar lower = 1;
            uchar upper = ( uchar )( size - 1 );
            while( lower != upper )
            {
               uchar middle = ( uchar )( ( lower + upper ) / 2 );
               if( middle == size - 1
                || compare_less( item, ap_node_ref->get_node( ).get_item_data( middle ) ) )
               {
                  s = middle;
                  if( upper == middle )
                     break;
                  upper = middle;
               }
               else
               {
                  if( lower == middle )
                     break;
                  lower = ( uchar )( middle + 1 );
               }
            }
         }
      }

      if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf )
         pos = s;
      else
      {
         uint tmp_link( ap_node_ref->get_node( ).get_item_link( s ) );
         new_link = insert_item( tmp_link, item,
          last_insert_node, last_insert_item, new_duplicate_dge_node );

         if( new_link != c_npos )
         {
            pos = s;
            link = ap_node_ref->get_node( ).get_item_link( s );
            ap_node_ref->get_node( ).set_item_link( s, new_link );

            has_changed = true;
         }
      }

      break;
   }

   bool is_leaf_node = ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf;

   if( s == size )
   {
      if( is_leaf_node )
         pos = s;
      else
      {
         uint tmp_link( ap_node_ref->get_node( ).ref_data( ).dge_link );
         if( tmp_link == c_npos )
            throw std::runtime_error( "bad node link #1" );

         new_link = insert_item( tmp_link,
          item, last_insert_node, last_insert_item, new_duplicate_dge_node );

         if( new_link != c_npos )
         {
            pos = s;
            link = ap_node_ref->get_node( ).ref_data( ).dge_link;
            ap_node_ref->get_node( ).ref_data( ).dge_link = new_link;

            has_changed = true;
         }
         else if( new_duplicate_dge_node != c_npos )
         {
            ap_node_ref->get_node( ).ref_data( ).dge_link = new_duplicate_dge_node;
            has_changed = true;
         }
      }
   }

   if( pos != -1 )
   {
      bool items_are_equal = false;
      if( pos > 0 && keys_are_equal( item, ap_node_ref->get_node( ).get_item_data( pos - 1 ) ) )
         items_are_equal = true;

      if( !state.allow_duplicates && items_are_equal )
         throw std::runtime_error( "duplicate item" );

#  ifdef BTREE_DEBUG
      std::cout << "insert at pos " << pos << " for item " << item << '\n';
#  endif
      has_changed = true;

      if( size < node_manager.get_items_per_node( ) && ( items_are_equal || dup_node_link == c_npos ) )
      {
         new_link = c_npos;

         if( pos < ( short )size )
            ap_node_ref->get_node( ).insert_item( pos, item, link );
         else
            ap_node_ref->get_node( ).append_item( item, link );

         if( is_leaf_node )
         {
            last_insert_item = ( uchar )pos;
            last_insert_node = node_link;
         }
#  ifdef BTREE_DEBUG
         dump_node( std::cout, node_link );
#  endif
      }
      else
      {
#  ifdef BTREE_DEBUG
         ++total_number_of_node_splits;
#  endif
         if( state.free_list_node != c_npos )
            new_link = state.free_list_node;
         else
         {
            new_link = node_manager.create_node( );
            ++state.total_nodes;
         }

         std::auto_ptr< bt_node_ref< T > > ap_new_node_ref( allocate_node_ref( new_link ) );

         if( state.free_list_node != c_npos )
         {
            state.free_list_node = ap_new_node_ref->get_node( ).ref_data( ).dge_link;
            ap_new_node_ref->get_node( ).reset( );
         }

         uchar items_per_node = node_manager.get_items_per_node( );

         uchar split = ( uchar )( items_per_node / 2 );
         if( split >= size )
            split = ( uchar )( size - 1 );
#  ifdef BTREE_DEBUG
         std::cout << "initial split point = " << ( int )split << '\n';
#  endif

         // IMPORTANT: Duplicate items are only ever permitted to exist in leaf nodes. In order
         // to meet this requirement a leaf node split cannot occur within a group of duplicate
         // items. If splitting simply cannot avoid it (i.e. all items in the original node are
         // duplicate and the new item is also the same) then the node is flagged to be certain
         // that the "bubbling" up of a duplicate item into a non-leaf node cannot occur. Using
         // this flag also ensures that any further insertions whose values are greater than or
         // equal to the first duplicate will occur at the "tail" link of the "duplicate split"
         // nodes sub-list.
         bool split_okay = false;

         if( split == size - 1 )
            split_okay = true;
         else if( !( keys_are_equal(
          ap_node_ref->get_node( ).get_item_data( split ),
          ap_node_ref->get_node( ).get_item_data( split + 1 ) ) ) )
         {
            split_okay = true;
            if( pos == split + 1
             && keys_are_equal( item, ap_node_ref->get_node( ).get_item_data( split ) ) )
               split++;
#  ifdef BTREE_DEBUG
            std::cout << "split point passed initial compare at " << ( int )split << '\n';
#  endif
         }
         else
         {
            for( uchar s1 = ( uchar )( split - 1 ),
             s2 = ( uchar )( split + 1 ); ; s1--, s2++ )
            {
               if( !( keys_are_equal(
                ap_node_ref->get_node( ).get_item_data( s1 ),
                ap_node_ref->get_node( ).get_item_data( s1 + 1 ) ) ) )
               {
                  if( pos == s1 + 1
                   && keys_are_equal( item, ap_node_ref->get_node( ).get_item_data( s1 ) ) )
                     s1++;
#  ifdef BTREE_DEBUG
                  std::cout << "split point passed back compare at " << ( int )s1 << '\n';
#  endif
                  split_okay = true;
                  split = s1;
                  break;
               }

               if( !( keys_are_equal(
                ap_node_ref->get_node( ).get_item_data( s2 ),
                ap_node_ref->get_node( ).get_item_data( s2 - 1 ) ) ) )
               {
#  ifdef BTREE_DEBUG
                  std::cout << "split point passed forward compare at " << ( int )s2 << '\n';
#  endif
                  split_okay = true;
                  split = s2;
                  break;
               }

               if( s1 == 0 )
                  break;
            }

            if( !split_okay
             && ( pos == 0 || pos == items_per_node ) && !( keys_are_equal( item,
             ap_node_ref->get_node( ).get_item_data( pos ? items_per_node - 1 : 0 ) ) ) )
            {
               split_okay = true;
               split = pos ? ( uchar )( items_per_node - 1 ) : ( uchar )0;
#  ifdef BTREE_DEBUG
               std::cout << "split point passed item compare at " << ( int )split << '\n';
#  endif
            }
         }

         if( !split_okay )
         {
#  ifdef BTREE_DEBUG
            std::cout << "split point set to "
             << ( int )( items_per_node - 1 ) << " and duplicate flagged\n";
#  endif
            split = ( uchar )( items_per_node - 1 );
            ap_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_has_dup_split;
         }

#  ifdef BTREE_DEBUG
         std::cout << "create new node #" << new_link << '\n';
         std::cout << "splitting after item #" << ( int )split << '\n';
#  endif
         if( pos <= split )
         {
#  ifdef BTREE_DEBUG
            std::cout << "insert item into old node\n";
#  endif
            for( s = 0; s < size - split; s++ )
               ap_new_node_ref->get_node( ).append_item(
                ap_node_ref->get_node( ).get_item_data( split + s ),
                ap_node_ref->get_node( ).get_item_link( split + s ) );

            ap_node_ref->get_node( ).erase_items( split );

            ap_node_ref->get_node( ).insert_item( pos, item, link );

            if( is_leaf_node )
            {
               last_insert_item = ( uchar )pos;
               last_insert_node = node_link;
            }
         }
         else if( pos == split + 1 )
         {
#  ifdef BTREE_DEBUG
            std::cout << "put item at start of new node\n";
#  endif
            ap_new_node_ref->get_node( ).append_item( item, link );

            for( s = ( uchar )pos; s < size; s++ )
               ap_new_node_ref->get_node( ).append_item(
                ap_node_ref->get_node( ).get_item_data( s ),
                ap_node_ref->get_node( ).get_item_link( s ) );

            ap_node_ref->get_node( ).erase_items( split + 1 );

            if( is_leaf_node )
            {
               last_insert_item = 0;
               last_insert_node = new_link;
            }
         }
         else
         {
#  ifdef BTREE_DEBUG
            std::cout << "insert item into new node\n";
#  endif
            uchar new_pos = ( uchar )( pos - split - 1 );

            for( s = 0; s < new_pos; s++ )
               ap_new_node_ref->get_node( ).append_item(
                ap_node_ref->get_node( ).get_item_data( split + s + 1 ),
                ap_node_ref->get_node( ).get_item_link( split + s + 1 ) );

            ap_new_node_ref->get_node( ).append_item( item, link );

            for( s = ( uchar )pos; s < size; s++ )
               ap_new_node_ref->get_node( ).append_item(
                ap_node_ref->get_node( ).get_item_data( s ),
                ap_node_ref->get_node( ).get_item_link( s ) );

            ap_node_ref->get_node( ).erase_items( split + 1 );

            if( is_leaf_node )
            {
               last_insert_item = new_pos;
               last_insert_node = new_link;
            }
         }

         if( dup_node_link != c_npos
          && !( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split ) )
            ;
         else
         {
            ap_new_node_ref->get_node( ).ref_data( ).dge_link
             = ap_node_ref->get_node( ).ref_data( ).dge_link;
            ap_node_ref->get_node( ).ref_data( ).dge_link = c_npos;
         }

         // IMPORTANT: Do not change the "right leaf node" if the node that has just been split
         // was already part of a duplicate sub-list. If the node was not actually split (which
         // will be the case if the new item was greater than the item value of items that form
         // a duplicate sub-list) then move the "right leaf node" flag, if applicable, into the
         // new node.
         if( !( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split ) )
         {
            if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_rgt_leaf )
            {
               state.rgt_leaf_node = new_link;
               ap_node_ref->get_node( ).ref_data( ).flags &= ~c_node_flag_is_rgt_leaf;
               ap_new_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_is_rgt_leaf;
            }
            else if( dup_node_link != c_npos )
            {
               std::auto_ptr< bt_node_ref< T > > ap_dup_node_ref( allocate_node_ref( dup_node_link ) );
               if( ap_dup_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_rgt_leaf )
               {
                  ap_dup_node_ref->get_node( ).ref_data( ).flags &= ~c_node_flag_is_rgt_leaf;
                  ap_dup_node_ref->get_node( ).touch( );

                  state.rgt_leaf_node = new_link;
                  ap_new_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_is_rgt_leaf;
               }
            }
         }

         if( !( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf ) )
            ap_new_node_ref->get_node( ).ref_data( ).flags &= ~c_node_flag_is_leaf;

         ap_new_node_ref->get_node( ).ref_data( ).rgt_link = ap_node_ref->get_node( ).ref_data( ).rgt_link;

         if( ap_new_node_ref->get_node( ).ref_data( ).rgt_link != c_npos )
         {
            std::auto_ptr< bt_node_ref< T > > ap_rgt_node_ref(
             allocate_node_ref( ap_new_node_ref->get_node( ).ref_data( ).rgt_link ) );

            ap_rgt_node_ref->get_node( ).ref_data( ).lft_link = new_link;
            ap_rgt_node_ref->get_node( ).touch( );
         }

         ap_node_ref->get_node( ).ref_data( ).rgt_link = new_link;
         ap_new_node_ref->get_node( ).ref_data( ).lft_link = node_link;

         ap_node_ref->get_node( ).resize_items( split + 1 );
         ap_new_node_ref->get_node( ).resize_items( size - split );

         // IMPORTANT: If the split is occurring within duplicate key values then create a link
         // between the first and last split nodes to ensure that subsequent inserts will avoid
         // any performance overhead that would be needed to span across these duplicate links.
         if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split )
         {
            if( dup_node_link != c_npos )
            {
               std::auto_ptr< bt_node_ref< T > > ap_dup_node_ref( allocate_node_ref( dup_node_link ) );

               ap_dup_node_ref->get_node( ).ref_data( ).dge_link = new_link;
               ap_dup_node_ref->get_node( ).touch( );
            }
            else
            {
               ap_node_ref->get_node( ).ref_data( ).dge_link = new_link;
               ap_new_node_ref->get_node( ).ref_data( ).dge_link = node_link;
            }
         }

         ap_new_node_ref->get_node( ).touch( );

#  ifdef BTREE_DEBUG
         dump_node( std::cout, new_link );
         dump_node( std::cout, node_link );
#  endif
         // IMPORTANT: The item value is modified to being either that of the first item in the
         // "new node" (for a leaf node split) or to that of the last item in the original node
         // (for a non-leaf node split) in order for this value to be "bubbled" back up (to the
         // previous recursion). If this is a "duplicate split" then no value can be bubbled up
         // as only leaf nodes are permitted to contain duplicates.
         if( is_leaf_node )
         {
            if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split )
            {
               if( ap_new_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_rgt_leaf )
                  new_duplicate_dge_node = new_link;
               new_link = c_npos;
            }
            else
               item = ap_new_node_ref->get_node( ).get_item_data( 0 );
         }
         else
            item = ap_node_ref->get_node( ).get_item_data( split );
      }
   }

   if( has_changed )
      ap_node_ref->get_node( ).touch( );

   return new_link;
}

template< typename T, typename L, typename N, typename M >
 typename bt_base< T, L, N, M >::size_type bt_base< T, L, N, M >::erase( const key_type& key )
{
   size_t orig = size( );
   const_iterator i = find( key );

   while( true )
   {
      erase( i );
      if( i == end( ) )
         break;
      if( !( keys_are_equal( *i, key ) ) )
         break;
      if( size( ) == orig )
         break;
   }

   return orig - size( );
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::erase( const_iterator& start, const_iterator& finish )
{
   while( true )
   {
      if( start == finish )
         break;

      // KLUDGE: Due to the way that the iterators are currently implemented (i.e. item offsets
      // rather than pointers) the "finish" iterator's item offset must be decremented for each
      // item erased for the "start" iterator when the node identity matches.
      if( start.node == finish.node )
         --finish.item;

      erase( start );
   }
}

template< typename T, typename L, typename N, typename M >
 void bt_base< T, L, N, M >::erase( const_iterator& position )
{
   if( position.node != c_npos )
   {
      bool removed_position_count_info = false;
      uint orig_rgt_leaf_node = state.rgt_leaf_node;

      if( !position.rp_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf )
         throw std::runtime_error( "invalid iterator #7" );

      bt_transaction< T, L, N, M > transaction( *this, true );

      position.rp_node_ref->get_node( ).erase_item( position.item );

      if( position.rp_node_ref->get_node( ).size( ) == 0 )
      {
         // IMPORTANT: If this node is a member of a "duplicate split" sub-list and it's either
         // the first or last link it will require special handling to ensure that the sub-list
         // is correctly rearranged.
         if( position.rp_node_ref->get_node( ).ref_data( ).dge_link != c_npos )
         {
            if( position.rp_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split )
            {
               std::auto_ptr< bt_node_ref< T > > ap_rgt_node_ref(
                allocate_node_ref( position.rp_node_ref->get_node( ).ref_data( ).rgt_link ) );
               ap_rgt_node_ref->get_node( ).copy_items( 0, position.rp_node_ref->get_node( ) );

               if( position.rp_node_ref->get_node( ).ref_data( ).rgt_link
                == position.rp_node_ref->get_node( ).ref_data( ).dge_link )
               {
                  position.rp_node_ref->get_node( ).ref_data( ).dge_link = c_npos;
                  position.rp_node_ref->get_node( ).ref_data( ).flags &= ~c_node_flag_has_dup_split;
               }

               uint old_link = position.rp_node_ref->get_node( ).ref_data( ).rgt_link;
               position.rp_node_ref->get_node( ).ref_data( ).rgt_link
                = ap_rgt_node_ref->get_node( ).ref_data( ).rgt_link;

               if( ap_rgt_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_rgt_leaf )
               {
                  state.rgt_leaf_node = position.node;
                  position.rp_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_is_rgt_leaf;
               }

               if( position.rp_node_ref->get_node( ).ref_data( ).rgt_link != c_npos )
               {
                  std::auto_ptr< bt_node_ref< T > > ap_new_rgt_node_ref(
                   allocate_node_ref( position.rp_node_ref->get_node( ).ref_data( ).rgt_link ) );

                  ap_new_rgt_node_ref->get_node( ).ref_data( ).lft_link = position.node;
                  ap_new_rgt_node_ref->get_node( ).touch( );
               }

               ap_rgt_node_ref->get_node( ).reset( );
               ap_rgt_node_ref->get_node( ).ref_data( ).dge_link = state.free_list_node;
               state.free_list_node = old_link;

               ap_rgt_node_ref->get_node( ).touch( );
            }
            else
            {
               std::auto_ptr< bt_node_ref< T > > ap_lft_node_ref(
                allocate_node_ref( position.rp_node_ref->get_node( ).ref_data( ).lft_link ) );

               if( position.rp_node_ref->get_node( ).ref_data( ).dge_link
                == position.rp_node_ref->get_node( ).ref_data( ).lft_link )
               {
                  ap_lft_node_ref->get_node( ).ref_data( ).dge_link = c_npos;
                  ap_lft_node_ref->get_node( ).ref_data( ).flags &= ~c_node_flag_has_dup_split;
               }
               else
               {
                  ap_lft_node_ref->get_node( ).ref_data( ).dge_link
                   = position.rp_node_ref->get_node( ).ref_data( ).dge_link;

                  std::auto_ptr< bt_node_ref< T > > ap_dge_node_ref(
                   allocate_node_ref( ap_lft_node_ref->get_node( ).ref_data( ).dge_link ) );

                  ap_dge_node_ref->get_node( ).ref_data( ).dge_link
                   = position.rp_node_ref->get_node( ).ref_data( ).lft_link;

                  ap_dge_node_ref->get_node( ).touch( );
               }

               ap_lft_node_ref->get_node( ).ref_data( ).flags &= ~c_node_flag_has_dup_split;

               ap_lft_node_ref->get_node( ).ref_data( ).rgt_link
                = position.rp_node_ref->get_node( ).ref_data( ).rgt_link;

               if( position.rp_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_rgt_leaf )
               {
                  ap_lft_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_is_rgt_leaf;
                  state.rgt_leaf_node = position.rp_node_ref->get_node( ).ref_data( ).lft_link;
               }

               ap_lft_node_ref->get_node( ).touch( );

               uint rgt_link = position.rp_node_ref->get_node( ).ref_data( ).rgt_link;
               if( rgt_link != c_npos )
               {
                  std::auto_ptr< bt_node_ref< T > > ap_rgt_node_ref( allocate_node_ref( rgt_link ) );

                  ap_rgt_node_ref->get_node( ).ref_data( ).lft_link
                   = position.rp_node_ref->get_node( ).ref_data( ).lft_link;

                  ap_rgt_node_ref->get_node( ).touch( );
               }

               position.rp_node_ref->get_node( ).reset( );
               position.rp_node_ref->get_node( ).ref_data( ).dge_link = state.free_list_node;

               removed_position_count_info = true;
               state.free_list_node = position.node;
            }
         }
         else if( position.rp_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split )
         {
            std::auto_ptr< bt_node_ref< T > > ap_lft_node_ref(
             allocate_node_ref( position.rp_node_ref->get_node( ).ref_data( ).lft_link ) );
            std::auto_ptr< bt_node_ref< T > > ap_rgt_node_ref(
             allocate_node_ref( position.rp_node_ref->get_node( ).ref_data( ).rgt_link ) );

            ap_lft_node_ref->get_node( ).ref_data( ).rgt_link
             = position.rp_node_ref->get_node( ).ref_data( ).rgt_link;
            ap_rgt_node_ref->get_node( ).ref_data( ).lft_link
             = position.rp_node_ref->get_node( ).ref_data( ).lft_link;

            ap_lft_node_ref->get_node( ).touch( );
            ap_rgt_node_ref->get_node( ).touch( );

            position.rp_node_ref->get_node( ).reset( );
            position.rp_node_ref->get_node( ).ref_data( ).dge_link = state.free_list_node;

            removed_position_count_info = true;
            state.free_list_node = position.node;
         }
      }

      state.total_items--;
      position.rp_node_ref->get_node( ).touch( );

      if( orig_rgt_leaf_node != state.rgt_leaf_node )
      {
         std::auto_ptr< bt_node_ref< T > > ap_root_node_ref( allocate_node_ref( state.root_node ) );
         ap_root_node_ref->get_node( ).ref_data( ).dge_link = state.rgt_leaf_node;
         ap_root_node_ref->get_node( ).touch( );

         // NOTE: It would be very unsafe to allow an iterator to exist which is now presumably
         // a part of the free list, therefore set it to "end" to prevent potential corruption.
         if( position.node == orig_rgt_leaf_node )
            position = end( );
      }

      if( position.node != c_npos && position.item >= position.rp_node_ref->get_node( ).size( ) )
         ++position;

      transaction.commit( );
   }
}

template< typename T, typename L, typename N, typename M > void bt_base< T, L, N, M >::clear( )
{
   if( p_transaction )
      throw std::runtime_error( "unexpected transaction found in clear" );

   uint next_node = state.root_node;
   uint first_node_in_next_level = c_npos;
   std::auto_ptr< bt_node_ref< T > > ap_node_ref;

   while( next_node != c_npos )
   {
      ap_node_ref.reset( allocate_node_ref( next_node ) );

      if( first_node_in_next_level == c_npos && ap_node_ref->get_node( ).size( ) )
         first_node_in_next_level = ap_node_ref->get_node( ).get_item_link( 0 );

      node_manager.destroy_node( next_node );
      next_node = ap_node_ref->get_node( ).ref_data( ).rgt_link;

      if( next_node == c_npos )
      {
         next_node = first_node_in_next_level;
         first_node_in_next_level = c_npos;
      }
   }

   next_node = state.first_append_node;
   while( next_node != c_npos )
   {
      ap_node_ref.reset( allocate_node_ref( next_node ) );

      node_manager.destroy_node( next_node );
      next_node = ap_node_ref->get_node( ).ref_data( ).rgt_link;
   }

   next_node = state.free_list_node;
   while( next_node != c_npos )
   {
      ap_node_ref.reset( allocate_node_ref( next_node ) );

      node_manager.destroy_node( next_node );
      next_node = ap_node_ref->get_node( ).ref_data( ).rgt_link;
   }

   node_manager.reset( );

   state.num_levels = 0;
   state.total_nodes = 0;
   state.total_items = 0;
   state.root_node = c_npos;
   state.lft_leaf_node = c_npos;
   state.rgt_leaf_node = c_npos;
   state.free_list_node = c_npos;
   state.first_append_node = c_npos;
   state.current_append_node = c_npos;
}

template< typename T, typename L, typename N, typename M > void bt_base< T, L, N, M >::build_index_nodes( )
{
   if( p_transaction )
      throw std::runtime_error( "unexpected transaction found in build" );

   uint next_node, next_index_node, first_new_index_node;
   uint first_old_index_node_in_next_level = c_npos;

   uchar old_num_levels( state.num_levels );

   std::auto_ptr< bt_node_ref< T > > ap_node_ref;
   std::auto_ptr< bt_node_ref< T > > ap_index_node_ref;

   if( ( state.root_node == c_npos && state.first_append_node == c_npos )
    || ( state.root_node != c_npos
    && state.root_node == state.lft_leaf_node && state.first_append_node == c_npos ) )
      return;

   bt_transaction< T, L, N, M > transaction( *this, true );

   // If index nodes currently exist then they will be reused when constructing the "new" index
   // node structure. If there was no existing index structure then the construction will begin
   // with the first "free list" node. If the "free list" is empty then construction must begin
   // instead with a newly allocated node.
   if( state.root_node != c_npos && state.root_node != state.lft_leaf_node )
   {
      next_index_node = state.root_node;
      ap_index_node_ref.reset( allocate_node_ref( next_index_node ) );

      if( old_num_levels > 1 )
         first_old_index_node_in_next_level = ap_index_node_ref->get_node( ).get_item_link( 0 );
   }
   else
   {
      next_index_node = state.free_list_node;
      if( next_index_node == c_npos )
      {
         next_index_node = node_manager.create_node( );
         ap_index_node_ref.reset( allocate_node_ref( next_index_node ) );
         ++state.total_nodes;
      }
      else
      {
         ap_index_node_ref.reset( allocate_node_ref( next_index_node ) );
         state.free_list_node = ap_index_node_ref->get_node( ).ref_data( ).dge_link;
      }
   }

   first_new_index_node = next_index_node;
   ap_index_node_ref->get_node( ).clear_items( );
   ap_index_node_ref->get_node( ).ref_data( ).flags = 0;
   ap_index_node_ref->get_node( ).ref_data( ).lft_link = c_npos;
   ap_index_node_ref->get_node( ).ref_data( ).dge_link = c_npos;

   next_node = state.lft_leaf_node;
   bool is_processing_appended = false;
   bool set_left_link_to_previous = false;

   if( next_node == c_npos )
   {
      next_node = state.first_append_node;
      is_processing_appended = true;
   }

   uint dup_node( c_npos );
   uint last_node( next_node );

   state.lft_leaf_node = next_node;

   uchar items_per_node( node_manager.get_items_per_node( ) );
   uchar items_to_fill_per_node( ( uchar )( items_per_node * state.node_fill_factor ) );

   state.num_levels = 0;

   ap_node_ref.reset( allocate_node_ref( next_node ) );

   if( is_processing_appended )
      state.total_items += ap_node_ref->get_node( ).size( );

   next_node = ap_node_ref->get_node( ).ref_data( ).rgt_link;
   if( next_node == c_npos )
   {
      if( !is_processing_appended && state.first_append_node != c_npos )
      {
         next_node = state.first_append_node;
         is_processing_appended = true;
         set_left_link_to_previous = true;

         ap_node_ref->get_node( ).ref_data( ).flags = c_node_flag_is_leaf;
         ap_node_ref->get_node( ).ref_data( ).rgt_link = next_node;
         ap_node_ref->get_node( ).touch( );
      }
      else
      {
         state.root_node = last_node;
         state.rgt_leaf_node = last_node;
         ap_node_ref->get_node( ).ref_data( ).flags
          = ( c_node_flag_is_leaf | c_node_flag_is_rgt_leaf );
      }
   }

   if( next_node != c_npos )
      ++state.num_levels;

   while( next_node != c_npos )
   {
      ap_node_ref.reset( allocate_node_ref( next_node ) );

      if( is_processing_appended
       && ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf )
         state.total_items += ap_node_ref->get_node( ).size( );

      if( set_left_link_to_previous )
      {
         set_left_link_to_previous = false;
         ap_node_ref->get_node( ).ref_data( ).lft_link = last_node;
      }

      if( ap_node_ref->get_node( ).size( )
       && ( ( ap_node_ref->get_node( ).ref_data( ).dge_link == c_npos
       && !( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split ) )
       || ( ap_node_ref->get_node( ).ref_data( ).dge_link != c_npos
       && ( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_has_dup_split ) ) ) )
      {
         if( !ap_index_node_ref.get( )
          || ap_index_node_ref->get_node( ).size( ) == items_to_fill_per_node )
         {
            uint last_index_node( next_index_node );
            if( ap_index_node_ref.get( ) )
            {
               ap_index_node_ref->get_node( ).touch( );

               next_index_node = ap_index_node_ref->get_node( ).ref_data( ).rgt_link;
               if( next_index_node != c_npos )
                  ap_index_node_ref.reset( allocate_node_ref( next_index_node ) );
            }

            if( !ap_index_node_ref.get( ) || next_index_node == c_npos )
            {
               if( first_old_index_node_in_next_level != c_npos )
               {
                  next_index_node = first_old_index_node_in_next_level;

                  if( ap_index_node_ref.get( ) )
                  {
                     ap_index_node_ref->get_node( ).ref_data( ).rgt_link = next_index_node;
                     ap_index_node_ref->get_node( ).touch( );
                  }

                  ap_index_node_ref.reset( allocate_node_ref( next_index_node ) );

                  if( --old_num_levels <= 1 )
                     first_old_index_node_in_next_level = c_npos;
                  else
                     first_old_index_node_in_next_level = ap_index_node_ref->get_node( ).get_item_link( 0 );
               }
               else if( state.free_list_node != c_npos )
               {
                  next_index_node = state.free_list_node;

                  if( ap_index_node_ref.get( ) )
                  {
                     ap_index_node_ref->get_node( ).ref_data( ).rgt_link = next_index_node;
                     ap_index_node_ref->get_node( ).touch( );
                  }

                  ap_index_node_ref.reset( allocate_node_ref( next_index_node ) );
                  state.free_list_node = ap_index_node_ref->get_node( ).ref_data( ).dge_link;
               }
               else
               {
                  next_index_node = node_manager.create_node( );
                  ++state.total_nodes;

                  if( ap_index_node_ref.get( ) )
                  {
                     ap_index_node_ref->get_node( ).ref_data( ).rgt_link = next_index_node;
                     ap_index_node_ref->get_node( ).touch( );
                  }

                  ap_index_node_ref.reset( allocate_node_ref( next_index_node ) );
               }
            }

            ap_index_node_ref->get_node( ).clear_items( );
            ap_index_node_ref->get_node( ).ref_data( ).flags = 0;
            ap_index_node_ref->get_node( ).ref_data( ).dge_link = c_npos;

            if( first_new_index_node == c_npos )
               first_new_index_node = next_index_node;
            else
               ap_index_node_ref->get_node( ).ref_data( ).lft_link = last_index_node;
         }

         if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf )
            ap_index_node_ref->get_node( ).append_item(
             ap_node_ref->get_node( ).get_item_data( 0 ), dup_node == c_npos ? last_node : dup_node );
         else
            ap_index_node_ref->get_node( ).append_item(
             ap_node_ref->get_node( ).get_item_data( ap_node_ref->get_node( ).size( ) - 1 ), next_node );
      }

      last_node = next_node;
      dup_node = ap_node_ref->get_node( ).ref_data( ).dge_link;
      next_node = ap_node_ref->get_node( ).ref_data( ).rgt_link;

      if( next_node == c_npos && !is_processing_appended )
      {
         next_node = state.first_append_node;
         is_processing_appended = true;

         if( next_node != c_npos )
         {
            set_left_link_to_previous = true;
            ap_node_ref->get_node( ).ref_data( ).rgt_link = next_node;
         }
      }

      if( next_node == c_npos )
      {
         if( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf )
         {
            if( ap_node_ref->get_node( ).ref_data( ).dge_link == c_npos )
            {
               state.rgt_leaf_node = last_node;
               ap_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_is_rgt_leaf;
               ap_node_ref->get_node( ).touch( );
            }
            else
            {
               std::auto_ptr< bt_node_ref< T > > ap_dup_node_ref(
                allocate_node_ref( ap_node_ref->get_node( ).ref_data( ).dge_link ) );

               state.rgt_leaf_node = ap_node_ref->get_node( ).ref_data( ).dge_link;
               ap_dup_node_ref->get_node( ).ref_data( ).flags |= c_node_flag_is_rgt_leaf;
               ap_dup_node_ref->get_node( ).touch( );
            }
         }

         if( next_index_node == first_new_index_node )
         {
            state.root_node = next_index_node;
            ap_index_node_ref->get_node( ).ref_data( ).dge_link = state.rgt_leaf_node;
            ap_index_node_ref->get_node( ).touch( );
            break;
         }

         last_node = next_node = first_new_index_node;
         ap_node_ref.reset( allocate_node_ref( next_node ) );

         ++state.num_levels;
         if( ap_index_node_ref->get_node( ).ref_data( ).rgt_link != c_npos )
         {
            next_index_node = ap_index_node_ref->get_node( ).ref_data( ).rgt_link;
            ap_index_node_ref->get_node( ).ref_data( ).rgt_link = c_npos;
            ap_index_node_ref->get_node( ).touch( );

            first_new_index_node = next_index_node;
            ap_index_node_ref.reset( allocate_node_ref( next_index_node ) );

            ap_index_node_ref->get_node( ).clear_items( );
            ap_index_node_ref->get_node( ).ref_data( ).flags = 0;
            ap_index_node_ref->get_node( ).ref_data( ).lft_link = c_npos;
            ap_index_node_ref->get_node( ).ref_data( ).dge_link = c_npos;
         }
         else
         {
            ap_index_node_ref->get_node( ).touch( );

            ap_index_node_ref.reset( );

            first_new_index_node = c_npos;
         }
      }
      else
         ap_node_ref->get_node( ).touch( );
   }

   // NOTE: Any "old" index nodes which have not already been reused during this index building
   // process will need to be added to the "free list". This might involve having to handle the
   // possibility of one or more index levels that may be "left over".
   if( ap_index_node_ref.get( ) )
   {
      while( ap_index_node_ref->get_node( ).ref_data( ).rgt_link != c_npos )
      {
         next_node = ap_index_node_ref->get_node( ).ref_data( ).rgt_link;
         ap_index_node_ref->get_node( ).ref_data( ).rgt_link = c_npos;
         ap_index_node_ref->get_node( ).touch( );

         ap_index_node_ref.reset( allocate_node_ref( next_node ) );

         if( ap_index_node_ref->get_node( ).ref_data( ).rgt_link == c_npos )
         {
            if( first_old_index_node_in_next_level != c_npos )
            {
               next_node = first_old_index_node_in_next_level;
               ap_index_node_ref.reset( allocate_node_ref( next_node ) );

               if( --old_num_levels <= 1 )
                  first_old_index_node_in_next_level = c_npos;
               else
                  first_old_index_node_in_next_level = ap_index_node_ref->get_node( ).get_item_link( 0 );
            }
         }

         if( state.free_list_node != c_npos )
            ap_index_node_ref->get_node( ).ref_data( ).dge_link = state.free_list_node;

         state.free_list_node = next_node;
         ap_index_node_ref->get_node( ).clear_items( );
         ap_index_node_ref->get_node( ).touch( );
      }
   }

   // IMPORTANT: If the index consists of a root node containing zero items then all leaf nodes
   // are assumed to be part of a duplicate sub-list. For this case the root node will be moved
   // the left leaf node.
   if( state.num_levels == 1 )
   {
      std::auto_ptr< bt_node_ref< T > > ap_root_node_ref( allocate_node_ref( state.root_node ) );
      if( !ap_root_node_ref->get_node( ).size( ) )
      {
         uint new_root_node = ap_root_node_ref->get_node( ).ref_data( ).dge_link;
         ap_root_node_ref->get_node( ).ref_data( ).dge_link = state.free_list_node;
         ap_root_node_ref->get_node( ).touch( );

         state.free_list_node = state.root_node;
         state.root_node = new_root_node;
      }
   }

   state.first_append_node = c_npos;
   state.current_append_node = c_npos;

   transaction.commit( );
}

template< typename T, typename L, typename N, typename M >
 bt_transaction< T, L, N, M >::bt_transaction( bt_base< T, L, N, M >& btree_base )
 :
 completed( false ),
 btree_base( btree_base ),
 bt_base_state( btree_base.state ),
 p_bt_transaction( btree_base.p_transaction )
{
   btree_base.p_transaction = this;
}

template< typename T, typename L, typename N, typename M >
 bt_transaction< T, L, N, M >::bt_transaction( bt_base< T, L, N, M >& btree_base, bool )
 :
 completed( false ),
 btree_base( btree_base ),
 bt_base_state( btree_base.state ),
 p_bt_transaction( btree_base.p_transaction )
{
   btree_base.p_transaction = this;
}

template< typename T, typename L, typename N, typename M >
 bt_transaction< T, L, N, M >::~bt_transaction( )
{
   if( !completed && btree_base.allow_auto_rollback( ) )
      rollback( );
}

template< typename T, typename L, typename N, typename M >
 void bt_transaction< T, L, N, M >::commit( )
{
   if( completed )
      throw std::runtime_error( "transaction already completed #0" );

   completed = true;

   if( !p_bt_transaction.final_value( ) )
   {
      btree_base.is_okay = false;

      btree_base.commit( );
      btree_base.node_manager.commit( );

      btree_base.is_okay = true;
   }

   bt_base_state.commit( );
}

template< typename T, typename L, typename N, typename M >
 void bt_transaction< T, L, N, M >::rollback( )
{
   if( completed )
      throw std::runtime_error( "transaction already completed #1" );

   completed = true;

   if( !p_bt_transaction.final_value( ) )
   {
      btree_base.is_okay = false;

      btree_base.rollback( );
      btree_base.node_manager.rollback( );

      btree_base.is_okay = true;
   }
}

#  ifdef USE_BTREE_NAMESPACE
}
#  endif

#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

#endif


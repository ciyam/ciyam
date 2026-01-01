// Copyright (c) 2001-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CACHE_H
#  define CACHE_H

//#define CACHE_DEBUG

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <memory.h>
#     include <limits>
#     include <memory>
#     include <vector>
#     include <iostream>
#     include <algorithm>
#  endif

#  include "macros.h"
#  include "threads.h"
#  include "pointers.h"

#  define USE_CACHE_NAMESPACE

#  ifdef USE_CACHE_NAMESPACE
namespace cache
{
#  endif

#  ifndef __GNUG__
typedef unsigned int uint;
#  endif

const unsigned c_npos = ~0u;
const unsigned c_mask_for_counter = ~0u >> 1;
const unsigned c_flag_for_changed = 1 << ( std::numeric_limits< unsigned int >::digits - 1 );

template< typename T > class cache_item;
template< typename T > struct cache_region;

// General Usage:
// The following cache implementation has been designed to allow for removal of items (that are
// stored if changed) without affecting other cache "users". This requires that each cache user
// works with a copy of each cached item rather than having "direct" access to the cached item.
// Such an approach can be wasteful of resources if many copies of the same items are stored in
// memory at once. For application designs that are likely to result in such "resource wastage"
// the use of a "resource management" class between the cache and its users would be advisable.
//
// Thread Safety:
// Thread safety is provided via a common lock that is used to prevent the concurrent execution
// of all public functions that could result in state corruption. This approach means that it's
// not possible for cache operations to be performed concurrently. If it is required to provide
// for concurrent "fetch" or "store" I/O operations (where these would presumably occur through
// the specific use of separate storage devices) then physical fetches would need to take place
// before the "perform_fetch" function is actually called. For storing the item number and data
// would need to be copied in the "perform_store" function and a "perform_fetch" that is called
// for the same item number (before the physical write has occurred) would need to be carefully
// handled. To implement such a design it would be recommended to use this cache implementation
// only as a private member of an alternative cache interface class.
//
// Exception Safety:
// Type "T" must not throw from its destructor. If "use_placement_new" is passed as "true" then
// it is assumed that type "T" is P.O.D. and therefore must also not throw during an assignment
// operation. Other exceptions that might occur will not affect the internal consistency of the
// cache, however, the state is not guaranteed to be the same as it was prior to the calling of
// the cache function from which the user exception threw out of (i.e. the "basic guarantee").

template< typename T > class cache_base
{
   public:
   enum error
   {
      error_cannot_use_placement_new_when_sizeof_T_is_too_small
   };

   enum dump
   {
      dmp_info,
      dmp_summary,
      dmp_detailed
   };

   cache_base( unsigned max_cache_items,
    unsigned items_per_region, unsigned regions_in_cache = 1,
    bool use_placement_new = false, bool allow_lazy_writes = true );

   virtual ~cache_base( );

   T get( unsigned num, bool retain = true );
   void put( const T& data, unsigned num, bool retain = true, bool prevent_lazy_write = false );

   void flush( bool mark_as_most_recent = true );

   void clear( );

   void clear_from( unsigned num );

   void clear_statistics( );

   float get_item_hit_ratio( )
   {
      return item_req_count == 0 ? ( float )0.0 : ( float )item_hit_count / ( float )item_req_count;
   }

   unsigned get_num_cached( ) { return num_cached; }
   unsigned get_num_regions( ) { return num_regions; }
   unsigned get_max_cache_items( ) { return max_cache_items; }
   unsigned get_items_per_region( ) { return items_per_region; }

   void set_max_cache_items( unsigned new_max_cache_items );
   void set_items_per_region( unsigned new_items_per_region );
   void set_regions_in_cache( unsigned new_regions_in_cache );

   bool is_item_cached( unsigned num );

   void mark_cached_item( unsigned num, bool is_new );

   void dump_cached_item_info( std::ostream& outs, dump& dump_type );

   private:
   void locate_item( unsigned num,
    unsigned& index, unsigned& region, unsigned& offset );

   unsigned locate_region( unsigned region );

   void free_cache_region( unsigned index,
    bool will_reuse = false, bool discard_changes = false );

   void unlink_cached_item( unsigned index, unsigned offset, bool skip_used );
   void link_item_as_most_recent( unsigned index, unsigned offset, bool skip_used );
   void link_item_as_least_recent( unsigned index, unsigned offset, bool skip_used );

   void free_least_costly_item( );

   void free_cache_item( unsigned index,
    unsigned offset, bool discard_changes = false );

   bool retain_item_in_cache( const cache_item< T >& item, unsigned num );

   void push_item_on_free_list( cache_item< T >* p_item );
   cache_item< T >* pop_item_from_free_list( );

   cache_base( const cache_base< T >& );
   cache_base< T >& operator =( const cache_base< T >& );

   unsigned counter;
   unsigned num_cached;
   unsigned num_regions;

   struct free_list
   {
      free_list( ) : p_next_link( 0 ) { }
      free_list* p_next_link;
   };

   free_list* p_free_list;

   unsigned item_req_count;
   unsigned item_hit_count;

   unsigned temp_read_num;
   unsigned temp_write_num;

   unsigned max_cache_items;
   unsigned items_per_region;
   unsigned regions_in_cache;

   bool use_placement_new;
   bool allow_lazy_writes;

   std::unique_ptr< cache_item< T > > up_temp_read_item;
   std::unique_ptr< cache_item< T > > up_temp_write_item;

   std::unique_ptr< std::vector< cache_region< T > > > up_cache_regions;

   char* p_buffer;

   mutex thread_lock;

   protected:
   virtual void perform_fetch( T& data, unsigned num ) = 0;
   virtual void perform_store( const T& data, unsigned num ) = 0;

   virtual void perform_post_flush( ) { }

   virtual void observe_region_replacement( unsigned /*old_region*/, unsigned /*new_region*/ ) { }
};

#ifdef CACHE_DEBUG
unsigned total_active_cache_objects;
#endif

template< typename T > class cache_item
{
   template< typename T1 > friend class cache_base;

   public:
   cache_item( )
    : flags( 0 ),
    prev_used_link( c_npos ), next_used_link( c_npos ),
    prev_changed_link( c_npos ), next_changed_link( c_npos ),
    prev_unchanged_link( c_npos ), next_unchanged_link( c_npos )
   {
#ifdef CACHE_DEBUG
      ++total_active_cache_objects;
      std::cout << "cache_item( )..." << this << std::endl;
#endif
   }

   cache_item( const T& data )
    : flags( 0 ), data( data ),
    prev_used_link( c_npos ), next_used_link( c_npos ),
    prev_changed_link( c_npos ), next_changed_link( c_npos ),
    prev_unchanged_link( c_npos ), next_unchanged_link( c_npos )
   {
#ifdef CACHE_DEBUG
      ++total_active_cache_objects;
      std::cout << "cache_item( )..." << this << std::endl;
#endif
   }

   cache_item( const cache_item& src )
    : flags( src.flags ), data( src.data ),
    prev_used_link( src.prev_used_link ), next_used_link( src.next_used_link ),
    prev_changed_link( src.prev_changed_link ), next_changed_link( src.next_changed_link ),
    prev_unchanged_link( src.prev_unchanged_link ), next_unchanged_link( src.next_unchanged_link )
   {
#ifdef CACHE_DEBUG
      ++total_active_cache_objects;
      std::cout << "cache_item( )..." << this << std::endl;
#endif
   }

   void copy_flags_and_links( const cache_item& src )
   {
      flags = src.flags;
      prev_used_link = src.prev_used_link;
      next_used_link = src.next_used_link;
      prev_changed_link = src.prev_changed_link;
      next_changed_link = src.next_changed_link;
      prev_unchanged_link = src.prev_unchanged_link;
      next_unchanged_link = src.next_unchanged_link;
   }

#ifdef CACHE_DEBUG
   ~cache_item( )
   {
      --total_active_cache_objects;
      std::cout << "~cache_item( )..." << this << std::endl;

      flags = 0;
      prev_used_link = c_npos;
      next_used_link = c_npos;
      prev_changed_link = c_npos;
      next_changed_link = c_npos;
      prev_unchanged_link = c_npos;
      next_unchanged_link = c_npos;
   }
#endif

   private:
   void* operator new( size_t size ) { return ::operator new( size ); }
   void* operator new( size_t /*size*/, void* buf ) { return buf; }

   T data;
   unsigned flags;
   unsigned prev_used_link;
   unsigned next_used_link;
   unsigned prev_changed_link;
   unsigned next_changed_link;
   unsigned prev_unchanged_link;
   unsigned next_unchanged_link;
};

template< typename T > struct cache_region
{
   cache_region( )
    : region( c_npos ),
    item_cost( 0 ), flush_cost( 0 ), counter_total( 0 ),
    most_recently_used( c_npos ), least_recently_used( c_npos ),
    most_recently_changed( c_npos ), least_recently_changed( c_npos ),
    most_recently_unchanged( c_npos ), least_recently_unchanged( c_npos ),
    up_cache_items( new std::vector< cache_item< T >* > )
   {
   }

   cache_region( const cache_region< T >& src )
    : up_cache_items( new std::vector< cache_item< T >* > )
   {
      copy_from( src );
   }

   cache_region< T >& operator =( const cache_region< T >& src )
   {
      copy_from( src );
      return *this;
   }

   void copy_from( const cache_region< T >& src )
   {
      region = src.region;

      item_cost = src.item_cost;
      flush_cost = src.flush_cost;
      counter_total = src.counter_total;

      most_recently_used = src.most_recently_used;
      least_recently_used = src.least_recently_used;
      most_recently_changed = src.most_recently_changed;
      least_recently_changed = src.least_recently_changed;
      most_recently_unchanged = src.most_recently_unchanged;
      least_recently_unchanged = src.least_recently_unchanged;

      *up_cache_items = *src.up_cache_items;
   }

   void reset( )
   {
      region = c_npos;

      item_cost = 0;
      flush_cost = 0;
      counter_total = 0;

      most_recently_used = c_npos;
      least_recently_used = c_npos;
      most_recently_changed = c_npos;
      least_recently_changed = c_npos;
      most_recently_unchanged = c_npos;
      least_recently_unchanged = c_npos;
   }

   unsigned region;
   unsigned item_cost;
   unsigned flush_cost;
   unsigned counter_total;
   unsigned most_recently_used;
   unsigned least_recently_used;
   unsigned most_recently_changed;
   unsigned least_recently_changed;
   unsigned most_recently_unchanged;
   unsigned least_recently_unchanged;

   std::unique_ptr< std::vector< cache_item< T >* > > up_cache_items;
};

template< typename T > cache_base< T >::cache_base( unsigned max_cache_items,
 unsigned items_per_region, unsigned regions_in_cache, bool use_placement_new, bool allow_lazy_writes )
 :
 counter( 0 ),
 num_cached( 0 ),
 num_regions( 0 ),
 p_free_list( 0 ),
 item_req_count( 0 ),
 item_hit_count( 0 ),
 temp_read_num( c_npos ),
 temp_write_num( c_npos ),
 max_cache_items( max_cache_items ),
 items_per_region( items_per_region ),
 regions_in_cache( regions_in_cache ),
 use_placement_new( use_placement_new ),
 allow_lazy_writes( allow_lazy_writes ),
 up_temp_read_item( new cache_item< T > ),
 up_temp_write_item( new cache_item< T > ),
 up_cache_regions( new std::vector< cache_region< T > >( regions_in_cache ) )
{
   if( use_placement_new && sizeof( T ) < sizeof( free_list ) )
      throw error_cannot_use_placement_new_when_sizeof_T_is_too_small;

   for( unsigned i = 0; i < regions_in_cache; i++ )
      ( *up_cache_regions )[ i ].up_cache_items->resize( items_per_region, 0 );

   if( use_placement_new )
   {
      p_buffer = new char[ sizeof( cache_item< T > ) * max_cache_items ];
      memset( p_buffer, 0, sizeof( cache_item< T > ) * max_cache_items );
   }
}

template< typename T > cache_base< T >::~cache_base( )
{
   for( unsigned i = 0; i < regions_in_cache; i++ )
   {
      for( unsigned j = 0; j < items_per_region; j++ )
      {
#ifdef CACHE_DEBUG
         if( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]
          && ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags & c_flag_for_changed )
            std::cout << "warning: destroying changed cache item #"
             << ( i * items_per_region + j ) << "..." << std::endl;
#endif
         if( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ] )
         {
            if( use_placement_new )
               ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->cache_item< T >::~cache_item( );
            else
               delete ( *( *up_cache_regions )[ i ].up_cache_items )[ j ];
         }
      }
   }

   if( use_placement_new )
      delete p_buffer;
}

template< typename T > T cache_base< T >::get( unsigned num, bool retain )
{
   guard lock( thread_lock );

   unsigned index = c_npos;
   unsigned region, offset = c_npos;

   locate_item( num, index, region, offset );

   bool fetched = false;
   cache_item< T >* p_item;

   if( num == temp_write_num )
   {
      if( retain )
         retain_item_in_cache( *up_temp_write_item, num );

      p_item = up_temp_write_item.get( );
   }
   else
   {
      if( num != temp_read_num )
      {
         if( index != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] )
         {
            if( use_placement_new )
               *up_temp_read_item = *( *( *up_cache_regions )[ index ].up_cache_items )[ offset ];
            else
            {
               std::unique_ptr< cache_item< T > > up_temp_item(
                new cache_item< T >( *( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] ) );

               up_temp_read_item = std::move( up_temp_item );
            }
         }
         else
         {
            std::unique_ptr< cache_item< T > > up_temp_item( new cache_item< T > );
            perform_fetch( up_temp_item->data, num );
            fetched = true;

            up_temp_read_item = std::move( up_temp_item );
         }

         temp_read_num = num;
      }

      if( retain )
         retain_item_in_cache( *up_temp_read_item, num );

      p_item = up_temp_read_item.get( );
   }

   ++item_req_count;

   if( !fetched )
      ++item_hit_count;

   // IMPORTANT: In order to stop the hit ratios from being incorrectly skewed due to a numeric
   // "overflow" they are halved when the "item requests" reaches the maximum value.
   if( item_req_count == c_npos )
   {
      item_req_count /= 2;
      item_hit_count /= 2;
   }

   return p_item->data;
}

template< typename T > void cache_base< T >::put(
 const T& data, unsigned num, bool retain, bool prevent_lazy_write )
{
   guard lock( thread_lock );

   unsigned index = c_npos;
   unsigned region, offset = c_npos;

   locate_item( num, index, region, offset );

   bool was_retained = false;

   std::unique_ptr< cache_item< T > > up_temp_item( new cache_item< T >( data ) );

   up_temp_item->flags |= c_flag_for_changed;

   if( retain
    || ( index != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] ) )
      was_retained = retain_item_in_cache( *up_temp_item, num );

   unsigned old_temp_read_num( temp_read_num );

   if( temp_read_num == num )
      temp_read_num = c_npos;

   if( temp_write_num == num )
      temp_write_num = c_npos;

   if( !was_retained || !allow_lazy_writes || prevent_lazy_write )
   {
      perform_store( up_temp_item->data, num );

      if( index != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags &= ~c_flag_for_changed;
   }

   up_temp_write_item = std::move( up_temp_item );
   up_temp_write_item->flags = 0;

   temp_write_num = num;

   if( num == old_temp_read_num )
   {
      if( use_placement_new )
         *up_temp_read_item = *up_temp_write_item;
      else
      {
         std::unique_ptr< cache_item< T > >
          up_temp_item_copy( new cache_item< T >( *up_temp_write_item ) );

         up_temp_read_item = std::move( up_temp_item_copy );
      }

      temp_read_num = num;
   }
}

template< typename T > void cache_base< T >::flush( bool mark_as_most_recent )
{
   guard lock( thread_lock );

   for( unsigned i = 0; i < regions_in_cache; i++ )
   {
      for( unsigned j = 0; j < items_per_region; j++ )
      {
         if( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]
          && ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags & c_flag_for_changed )
         {
            perform_store(
             ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->data,
             ( *up_cache_regions )[ i ].region * items_per_region + j );

            unlink_cached_item( i, j, true );

            ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags &= ~c_flag_for_changed;
            --( *up_cache_regions )[ i ].flush_cost;

            if( mark_as_most_recent )
               link_item_as_most_recent( i, j, true );
            else
               link_item_as_least_recent( i, j, true );
         }
      }

      for( unsigned j = 0; j < items_per_region; j++ )
      {
         if( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ] )
         {
            unsigned prev_used_link = ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->prev_used_link;
            unsigned next_used_link = ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->next_used_link;

            ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->prev_unchanged_link = prev_used_link;
            ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->next_unchanged_link = next_used_link;
         }
      }

      ( *up_cache_regions )[ i ].most_recently_unchanged = ( *up_cache_regions )[ i ].most_recently_used;
      ( *up_cache_regions )[ i ].least_recently_unchanged = ( *up_cache_regions )[ i ].least_recently_used;
   }

   perform_post_flush( );
}

template< typename T > void cache_base< T >::clear( )
{
   guard lock( thread_lock );

   for( unsigned i = num_regions; i > 0; i-- )
      free_cache_region( i - 1, false, true );

   counter = 0;

   temp_read_num = c_npos;
   temp_write_num = c_npos;

   clear_statistics( );
}

template< typename T > void cache_base< T >::clear_from( unsigned num )
{
   guard lock( thread_lock );

   unsigned index = c_npos;
   unsigned region, offset = c_npos;

   locate_item( num, index, region, offset );

   if( index != c_npos )
   {
      for( unsigned i = 0; i < regions_in_cache; i++ )
      {
         if( ( *up_cache_regions )[ i ].region == c_npos
          || ( *up_cache_regions )[ i ].region < region )
            continue;

         if( offset != 0 && region == ( *up_cache_regions )[ i ].region )
         {
            for( unsigned j = offset; j < items_per_region; j++ )
               free_cache_item( i, j, true );
            continue;
         }

         free_cache_region( i, false, true );
      }
   }
}

template< typename T > void cache_base< T >::clear_statistics( )
{
   guard lock( thread_lock );

   item_req_count = 0;
   item_hit_count = 0;
}

template< typename T > void cache_base< T >::set_max_cache_items( unsigned new_max_cache_items )
{
   guard lock( thread_lock );

   unsigned cached = num_cached;
   if( new_max_cache_items < cached )
   {
      for( unsigned i = 0; i < cached - new_max_cache_items; i++ )
         free_least_costly_item( );
   }

   // IMPORTANT: The "placement new" operations have been designed for P.O.D. types exclusively
   // so the following block assumes that no exception will be thrown between the allocation of
   // the new buffer and its usurpation of the old buffer.
   if( use_placement_new )
   {
      char* p_new_buffer = new char[ sizeof( cache_item< T > ) * new_max_cache_items ];

      cache_item< T >* p_item = reinterpret_cast< cache_item< T >* >( p_new_buffer );

      for( unsigned i = 0; i < regions_in_cache; i++ )
      {
         for( unsigned j = 0; j < items_per_region; j++ )
         {
            if( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ] )
            {
               cache_item< T >* p_old_item = ( *( *up_cache_regions )[ i ].up_cache_items )[ j ];

               ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]
                = new( p_item++ ) cache_item< T >( *p_old_item );
            }
         }
      }

      while( pop_item_from_free_list( ) )
         ;

      if( new_max_cache_items > max_cache_items )
         memset( p_new_buffer + ( sizeof( cache_item< T > ) * max_cache_items ),
          0, sizeof( cache_item< T > ) * ( new_max_cache_items - max_cache_items ) );

      delete p_buffer;
      p_buffer = p_new_buffer;
   }

   max_cache_items = new_max_cache_items;
}

template< typename T > void cache_base< T >::set_items_per_region( unsigned new_items_per_region )
{
   guard lock( thread_lock );

   if( new_items_per_region < items_per_region )
   {
      for( unsigned i = 0; i < regions_in_cache; i++ )
      {
         for( unsigned j = new_items_per_region; j < items_per_region; j++ )
         {
            if( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ] )
               free_cache_item( i, j );
         }
      }
   }

   std::vector< cache_region< T > > tmp_cache_regions( regions_in_cache );

   for( unsigned i = 0; i < regions_in_cache; i++ )
      tmp_cache_regions[ i ].up_cache_items->resize( new_items_per_region, 0 );

   for( unsigned i = 0; i < regions_in_cache; i++ )
   {
      std::copy( ( *up_cache_regions )[ i ].up_cache_items->begin( ),
       ( *up_cache_regions )[ i ].up_cache_items->begin( )
       + std::min( items_per_region, new_items_per_region ),
       tmp_cache_regions[ i ].up_cache_items->begin( ) );

      ( *up_cache_regions )[ i ].up_cache_items = std::move( tmp_cache_regions[ i ].up_cache_items );
   }

   items_per_region = new_items_per_region;
}

template< typename T > void cache_base< T >::set_regions_in_cache( unsigned new_regions_in_cache )
{
   guard lock( thread_lock );

   if( new_regions_in_cache < num_regions )
   {
      for( unsigned i = num_regions; i > new_regions_in_cache; i-- )
         free_cache_region( i - 1 );
   }

   std::unique_ptr< std::vector< cache_region< T > > >
    up_tmp_cache_regions( new std::vector< cache_region< T > >( new_regions_in_cache ) );

   for( unsigned i = 0; i < new_regions_in_cache; i++ )
      ( *up_tmp_cache_regions )[ i ].up_cache_items->resize( items_per_region, 0 );

   for( unsigned i = 0; i < std::min( regions_in_cache, new_regions_in_cache ); i++ )
      ( *up_tmp_cache_regions )[ i ] = ( *up_cache_regions )[ i ];

   up_cache_regions = std::move( up_tmp_cache_regions );

   regions_in_cache = new_regions_in_cache;
}

template< typename T > bool cache_base< T >::is_item_cached( unsigned num )
{
   guard lock( thread_lock );

   unsigned index = c_npos;
   unsigned region, offset = c_npos;

   locate_item( num, index, region, offset );

   return index != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ];
}

template< typename T > void cache_base< T >::locate_item(
 unsigned num, unsigned& index, unsigned& region, unsigned& offset )
{
   if( items_per_region )
   {
      region = num / items_per_region;
      offset = num % items_per_region;

      index = locate_region( region );
   }
}

template< typename T > unsigned cache_base< T >::locate_region( unsigned region )
{
   for( unsigned i = 0; i < regions_in_cache; i++ )
      if( ( *up_cache_regions )[ i ].region == region )
         return i;
   return c_npos;
}

template< typename T > void cache_base< T >::free_cache_region(
 unsigned index, bool will_reuse, bool discard_changes )
{
   for( unsigned i = 0; i < items_per_region; i++ )
      free_cache_item( index, i, discard_changes );

   ( *up_cache_regions )[ index ].reset( );

   if( !will_reuse && index == num_regions - 1 )
      --num_regions;
}

template< typename T > void cache_base< T >::unlink_cached_item(
 unsigned index, unsigned offset, bool skip_used )
{
   if( !skip_used )
   {
      unsigned prev_used_link = ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_used_link;
      unsigned next_used_link = ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_used_link;

      if( prev_used_link != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ prev_used_link ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ prev_used_link ]->next_used_link = next_used_link;

      if( next_used_link != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ next_used_link ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ next_used_link ]->prev_used_link = prev_used_link;

      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_used_link = c_npos;
      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_used_link = c_npos;

      if( offset == ( *up_cache_regions )[ index ].most_recently_used )
         ( *up_cache_regions )[ index ].most_recently_used = prev_used_link;

      if( offset == ( *up_cache_regions )[ index ].least_recently_used )
         ( *up_cache_regions )[ index ].least_recently_used = next_used_link;
   }

   unsigned prev_changed_link = ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_changed_link;
   unsigned next_changed_link = ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_changed_link;

   if( prev_changed_link != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ prev_changed_link ] )
      ( *( *up_cache_regions )[ index ].up_cache_items )[ prev_changed_link ]->next_changed_link = next_changed_link;

   if( next_changed_link != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ next_changed_link ] )
      ( *( *up_cache_regions )[ index ].up_cache_items )[ next_changed_link ]->prev_changed_link = prev_changed_link;

   ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_changed_link = c_npos;
   ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_changed_link = c_npos;

   if( offset == ( *up_cache_regions )[ index ].most_recently_changed )
      ( *up_cache_regions )[ index ].most_recently_changed = prev_changed_link;

   if( offset == ( *up_cache_regions )[ index ].least_recently_changed )
      ( *up_cache_regions )[ index ].least_recently_changed = next_changed_link;

   unsigned prev_unchanged_link = ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_unchanged_link;
   unsigned next_unchanged_link = ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_unchanged_link;

   if( prev_unchanged_link != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ prev_unchanged_link ] )
      ( *( *up_cache_regions )[ index ].up_cache_items )[ prev_unchanged_link ]->next_unchanged_link = next_unchanged_link;

   if( next_unchanged_link != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ next_unchanged_link ] )
      ( *( *up_cache_regions )[ index ].up_cache_items )[ next_unchanged_link ]->prev_unchanged_link = prev_unchanged_link;

   ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_unchanged_link = c_npos;
   ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_unchanged_link = c_npos;

   if( offset == ( *up_cache_regions )[ index ].most_recently_unchanged )
      ( *up_cache_regions )[ index ].most_recently_unchanged = prev_unchanged_link;

   if( offset == ( *up_cache_regions )[ index ].least_recently_unchanged )
      ( *up_cache_regions )[ index ].least_recently_unchanged = next_unchanged_link;
}

template< typename T > void cache_base< T >::link_item_as_most_recent(
 unsigned index, unsigned offset, bool skip_used )
{
   if( !skip_used )
   {
      unsigned most_recently_used = ( *up_cache_regions )[ index ].most_recently_used;

      if( most_recently_used != c_npos
       && ( *( *up_cache_regions )[ index ].up_cache_items )[ most_recently_used ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ most_recently_used ]->next_used_link = offset;

      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_used_link = most_recently_used;

      ( *up_cache_regions )[ index ].most_recently_used = offset;

      if( ( *up_cache_regions )[ index ].least_recently_used == c_npos )
         ( *up_cache_regions )[ index ].least_recently_used = offset;
   }

   unsigned changed = ( *( *up_cache_regions )[
    index ].up_cache_items )[ offset ]->flags & c_flag_for_changed;

   unsigned most_recently_changed = ( *up_cache_regions )[ index ].most_recently_changed;
   unsigned most_recently_unchanged = ( *up_cache_regions )[ index ].most_recently_unchanged;

   if( changed )
   {
      if( most_recently_changed != c_npos
       && ( *( *up_cache_regions )[ index ].up_cache_items )[ most_recently_changed ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ most_recently_changed ]->next_changed_link = offset;

      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_changed_link = most_recently_changed;

      ( *up_cache_regions )[ index ].most_recently_changed = offset;

      if( ( *up_cache_regions )[ index ].least_recently_changed == c_npos )
         ( *up_cache_regions )[ index ].least_recently_changed = offset;
   }
   else
   {
      if( most_recently_unchanged != c_npos
       && ( *( *up_cache_regions )[ index ].up_cache_items )[ most_recently_unchanged ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ most_recently_unchanged ]->next_unchanged_link = offset;

      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->prev_unchanged_link = most_recently_unchanged;

      ( *up_cache_regions )[ index ].most_recently_unchanged = offset;

      if( ( *up_cache_regions )[ index ].least_recently_unchanged == c_npos )
         ( *up_cache_regions )[ index ].least_recently_unchanged = offset;
   }
}

template< typename T > void cache_base< T >::link_item_as_least_recent(
 unsigned index, unsigned offset, bool skip_used )
{
   if( !skip_used )
   {
      unsigned least_recently_used = ( *up_cache_regions )[ index ].least_recently_used;

      if( least_recently_used != c_npos
       && ( *( *up_cache_regions )[ index ].up_cache_items )[ least_recently_used ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ least_recently_used ]->prev_used_link = offset;

      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_used_link = least_recently_used;

      ( *up_cache_regions )[ index ].least_recently_used = offset;

      if( ( *up_cache_regions )[ index ].most_recently_used == c_npos )
         ( *up_cache_regions )[ index ].most_recently_used = offset;
   }

   unsigned changed =
    ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags & c_flag_for_changed;

   unsigned least_recently_changed = ( *up_cache_regions )[ index ].least_recently_changed;
   unsigned least_recently_unchanged = ( *up_cache_regions )[ index ].least_recently_unchanged;

   if( changed )
   {
      if( least_recently_changed != c_npos
       && ( *( *up_cache_regions )[ index ].up_cache_items )[ least_recently_changed ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ least_recently_changed ]->prev_changed_link = offset;

      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_changed_link = least_recently_changed;

      ( *up_cache_regions )[ index ].least_recently_changed = offset;

      if( ( *up_cache_regions )[ index ].most_recently_changed == c_npos )
         ( *up_cache_regions )[ index ].most_recently_changed = offset;
   }
   else
   {
      if( least_recently_unchanged != c_npos
       && ( *( *up_cache_regions )[ index ].up_cache_items )[ least_recently_unchanged ] )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ least_recently_unchanged ]->prev_unchanged_link = offset;

      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->next_unchanged_link = least_recently_unchanged;

      ( *up_cache_regions )[ index ].least_recently_unchanged = offset;

      if( ( *up_cache_regions )[ index ].most_recently_unchanged == c_npos )
         ( *up_cache_regions )[ index ].most_recently_unchanged = offset;
   }
}

template< typename T > void cache_base< T >::free_least_costly_item( )
{
   unsigned least_recently_changed_index = c_npos;
   unsigned least_recently_changed_offset = c_npos;
   unsigned least_recently_unchanged_index = c_npos;
   unsigned least_recently_unchanged_offset = c_npos;
   unsigned least_recently_changed_counter = c_mask_for_counter;
   unsigned least_recently_unchanged_counter = c_mask_for_counter;

   for( unsigned i = 0; i < regions_in_cache; i++ )
   {
      if( ( *up_cache_regions )[ i ].least_recently_changed != c_npos )
      {
         unsigned j = ( *up_cache_regions )[ i ].least_recently_changed;
         unsigned counter = ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags & c_mask_for_counter;

         if( counter < least_recently_changed_counter )
         {
            least_recently_changed_index = i;
            least_recently_changed_offset = j;
            least_recently_changed_counter = counter;
         }
      }

      if( ( *up_cache_regions )[ i ].least_recently_unchanged != c_npos )
      {
         unsigned j = ( *up_cache_regions )[ i ].least_recently_unchanged;
         unsigned counter = ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags & c_mask_for_counter;

         if( counter < least_recently_unchanged_counter )
         {
            least_recently_unchanged_index = i;
            least_recently_unchanged_offset = j;
            least_recently_unchanged_counter = counter;
         }
      }
   }

   if( least_recently_unchanged_index != c_npos )
      free_cache_item( least_recently_unchanged_index, least_recently_unchanged_offset );
   else if( least_recently_changed_index != c_npos )
      free_cache_item( least_recently_changed_index, least_recently_changed_offset );
}

template< typename T > void cache_base< T >::free_cache_item( unsigned index, unsigned offset, bool discard_changes )
{
   if( ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] )
   {
      if( ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags & c_flag_for_changed )
      {
         if( !discard_changes )
            perform_store(
             ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->data,
             ( *up_cache_regions )[ index ].region * items_per_region + offset );

         ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags &= ~c_flag_for_changed;
         --( *up_cache_regions )[ index ].flush_cost;
      }

      unlink_cached_item( index, offset, false );

      unsigned counter
       = ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags & c_mask_for_counter;

      if( use_placement_new )
      {
         ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->cache_item< T >::~cache_item( );
         push_item_on_free_list( ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] );
      }
      else
         delete ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ];

      ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] = 0;
      ( *up_cache_regions )[ index ].counter_total -= counter;

      --num_cached;
      --( *up_cache_regions )[ index ].item_cost;
   }
}

template< typename T > bool cache_base< T >::retain_item_in_cache( const cache_item< T >& item, unsigned num )
{
   if( max_cache_items == 0 || items_per_region == 0 || regions_in_cache == 0 )
      return false;

   unsigned region = num / items_per_region;
   unsigned offset = num % items_per_region;

   unsigned index = locate_region( region );

   bool region_was_in_cache = ( index != c_npos );

   bool is_cached = region_was_in_cache && ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ];

   if( !is_cached
    && ( index == c_npos || num_cached == max_cache_items ) )
   {
      unsigned region_item_index = c_npos;
      unsigned region_flush_index = c_npos;

      unsigned least_region_item_cost = c_mask_for_counter;
      unsigned least_region_flush_cost = c_mask_for_counter;
      unsigned least_region_item_total = c_mask_for_counter;
      unsigned least_region_flush_total = c_mask_for_counter;

      unsigned least_recently_changed_index = c_npos;
      unsigned least_recently_changed_offset = c_npos;
      unsigned least_recently_unchanged_index = c_npos;
      unsigned least_recently_unchanged_offset = c_npos;

      unsigned least_recently_changed_counter = c_mask_for_counter;
      unsigned least_recently_unchanged_counter = c_mask_for_counter;

      if( num_cached == max_cache_items
       || ( index == c_npos && num_regions == regions_in_cache ) )
      {
         for( unsigned i = 0; i < regions_in_cache; i++ )
         {
            if( ( index == c_npos ) && ( num_regions == regions_in_cache ) )
            {
               if( ( *up_cache_regions )[ i ].flush_cost )
               {
                  if( ( *up_cache_regions )[ i ].flush_cost < least_region_flush_cost )
                  {
                     least_region_flush_cost = ( *up_cache_regions )[ i ].flush_cost;
                     least_region_flush_total = ( *up_cache_regions )[ i ].counter_total;

                     region_flush_index = i;
                  }
                  else if( ( *up_cache_regions )[ i ].counter_total < least_region_flush_total )
                  {
                     least_region_flush_total = ( *up_cache_regions )[ i ].counter_total;

                     region_flush_index = i;
                  }
               }
               else
               {
                  if( ( *up_cache_regions )[ i ].item_cost < least_region_item_cost )
                  {
                     least_region_item_cost = ( *up_cache_regions )[ i ].item_cost;
                     least_region_item_total = ( *up_cache_regions )[ i ].counter_total;

                     region_item_index = i;
                  }
                  else if( ( *up_cache_regions )[ i ].counter_total < least_region_item_total )
                  {
                     least_region_item_total = ( *up_cache_regions )[ i ].counter_total;

                     region_item_index = i;
                  }
               }
            }

            if( ( *up_cache_regions )[ i ].least_recently_changed != c_npos )
            {
               unsigned j = ( *up_cache_regions )[ i ].least_recently_changed;
               unsigned counter = ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags & c_mask_for_counter;

               if( counter < least_recently_changed_counter )
               {
                  least_recently_changed_index = i;
                  least_recently_changed_offset = j;
                  least_recently_changed_counter = counter;
               }
            }

            if( ( *up_cache_regions )[ i ].least_recently_unchanged != c_npos )
            {
               unsigned j = ( *up_cache_regions )[ i ].least_recently_unchanged;
               unsigned counter = ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags & c_mask_for_counter;

               if( counter < least_recently_unchanged_counter )
               {
                  least_recently_unchanged_index = i;
                  least_recently_unchanged_offset = j;
                  least_recently_unchanged_counter = counter;
               }
            }
         }
      }

      if( index == c_npos )
      {
         if( region_item_index != c_npos )
            index = region_item_index;
         else if( region_flush_index != c_npos )
            index = region_flush_index;
         else if( num_regions < regions_in_cache )
            index = num_regions;
      }

      if( index == c_npos )
         return false;

      if( !region_was_in_cache )
      {
         unsigned last_region( ( *up_cache_regions )[ index ].region );
         if( last_region != c_npos )
            free_cache_region( index, true );
         else
            ++num_regions;

#ifdef CACHE_DEBUG
         std::cout << "index = " << index << ", region = " << region << std::endl;
#endif
         ( *up_cache_regions )[ index ].region = region;

         if( last_region != c_npos )
            observe_region_replacement( last_region, region );
      }

      if( num_cached == max_cache_items )
      {
         if( least_recently_unchanged_index != c_npos )
            free_cache_item( least_recently_unchanged_index, least_recently_unchanged_offset );
         else if( least_recently_changed_index != c_npos )
            free_cache_item( least_recently_changed_index, least_recently_changed_offset );
         else
            return false;
      }
   }

   // IMPORTANT: If the counter total is about to set the change flag (i.e. by causing the most
   // significant bit to be set) then halve the counter value and do the same to all the region
   // items and to each region's counter total.
   if( ( *up_cache_regions )[ index ].counter_total + counter + 1 & c_flag_for_changed )
   {
      counter /= 2;

      for( unsigned i = 0; i < regions_in_cache; i++ )
      {
         for( unsigned j = 0; j < items_per_region; j++ )
         {
            if( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ] )
            {
               unsigned changed_flag = ( *( *up_cache_regions )[
                i ].up_cache_items )[ j ]->flags & c_flag_for_changed;

               unsigned val = ( *( *up_cache_regions )[
                i ].up_cache_items )[ j ]->flags & c_mask_for_counter;

               ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags = ( val / 2 );
               ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags |= changed_flag;

               ( *up_cache_regions )[ i ].counter_total -= val - ( val / 2 );
            }
         }
      }
   }

   unsigned changed_flag = item.flags & c_flag_for_changed;

   if( is_cached )
   {
      if( changed_flag && !( ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags & c_flag_for_changed ) )
         ++( *up_cache_regions )[ index ].flush_cost;

      ( *up_cache_regions )[ index ].counter_total
       -= ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags & c_mask_for_counter;

      changed_flag |= ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags & c_flag_for_changed;

      if( use_placement_new )
         ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->data = item.data;
      else
      {
         std::unique_ptr< cache_item< T > > temp_cache_item( new cache_item< T >( item ) );

         temp_cache_item->copy_flags_and_links( *( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] );

         delete ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ];

         ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] = temp_cache_item.release( );
      }
   }
   else
   {
      if( use_placement_new )
      {
         cache_item< T >* p_free_list_item = pop_item_from_free_list( );

         if( p_free_list_item )
            ( *( *up_cache_regions )[ index ].up_cache_items )[
             offset ] = new( p_free_list_item ) cache_item< T >( item );
         else
            ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]
             = new( p_buffer + ( sizeof( cache_item< T > ) * num_cached ) ) cache_item< T >( item );
      }
      else
         ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] = new cache_item< T >( item );

      ++( *up_cache_regions )[ index ].item_cost;

      if( changed_flag )
         ++( *up_cache_regions )[ index ].flush_cost;
   }

   unlink_cached_item( index, offset, false );

   ( *up_cache_regions )[ index ].counter_total += ++counter;

   ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags = counter;
   ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ]->flags |= changed_flag;

   link_item_as_most_recent( index, offset, false );

   if( !is_cached )
      ++num_cached;

   return true;
}

template< typename T > void cache_base< T >::push_item_on_free_list( cache_item< T >* p_item )
{
   free_list* p_old_free_list = p_free_list;

   p_free_list = reinterpret_cast< free_list* >( p_item );

   p_free_list->p_next_link = p_old_free_list;
}

template< typename T > cache_item< T >* cache_base< T >::pop_item_from_free_list( )
{
   free_list* p_old_free_list = p_free_list;

   if( p_old_free_list )
      p_free_list = p_old_free_list->p_next_link;

   return reinterpret_cast< cache_item< T >* >( p_old_free_list );
}

template< typename T > void cache_base< T >::mark_cached_item( unsigned num, bool is_new )
{
   guard lock( thread_lock );

   unsigned index = c_npos;
   unsigned region, offset = c_npos;

   locate_item( num, index, region, offset );

   bool is_cached = index != c_npos && ( *( *up_cache_regions )[ index ].up_cache_items )[ offset ];

   if( is_cached )
   {
      unlink_cached_item( index, offset, false );

      cache_item< T >& item( *( *( *up_cache_regions )[ index ].up_cache_items )[ offset ] );
      ( *up_cache_regions )[ index ].counter_total -= item.flags & c_mask_for_counter;
      unsigned changed_flag = item.flags & c_flag_for_changed;

      item.flags = is_new ? ++counter : 0;
      ( *up_cache_regions )[ index ].counter_total += item.flags;

      item.flags |= changed_flag;

      if( is_new )
         link_item_as_most_recent( index, offset, false );
      else
         link_item_as_least_recent( index, offset, false );
   }
}

template< typename T > void cache_base< T >::dump_cached_item_info( std::ostream& outs, dump& dump_type )
{
   guard lock( thread_lock );

   outs << "<cache info>\n";
   outs << " items cached: " << ( signed )num_cached << "/" << ( signed )max_cache_items << "\n";
   outs << " regions in use: " << ( signed )num_regions << "/" << ( signed )regions_in_cache << "\n\n";

   outs << " items per region: " << ( signed ) items_per_region << "\n\n";

   outs << " counter: " << ( signed )counter << "\n";
   outs << " temp_read_num: " << ( signed )temp_read_num << "\n";
   outs << " temp_write_num: " << ( signed )temp_write_num << "\n\n";

   outs << " item_req_count = " << item_req_count << "\n";
   outs << " item_hit_count = " << item_hit_count << "\n";
   outs << " item hit ratio = " << ( get_item_hit_ratio( ) * 100.0 ) << "%\n";

   if( dump_type > dmp_info )
   {
      for( size_t i = 0; i < up_cache_regions->size( ); i++ )
      {
         if( ( *up_cache_regions )[ i ].region == c_npos )
            continue;

         outs << "\n<cache region: "
          << ( signed )( ( *up_cache_regions )[ i ].region * items_per_region ) << "-"
          << ( signed )( ( *up_cache_regions )[ i ].region * items_per_region + items_per_region - 1 )
          << ">\n";

         outs << " item_cost: " << ( signed )( *up_cache_regions )[ i ].item_cost << "\n";
         outs << " flush_cost: " << ( signed )( *up_cache_regions )[ i ].flush_cost << "\n";
         outs << " counter_total: " << ( signed )( *up_cache_regions )[ i ].counter_total << "\n\n";

         outs << " most_recently_used: "
          << ( signed )( *up_cache_regions )[ i ].most_recently_used << "\n";

         outs << " least_recently_used: "
          << ( signed )( *up_cache_regions )[ i ].least_recently_used << "\n";

         outs << " most_recently_changed: "
          << ( signed )( *up_cache_regions )[ i ].most_recently_changed << "\n";

         outs << " least_recently_changed: "
          << ( signed )( *up_cache_regions )[ i ].least_recently_changed << "\n";

         outs << " most_recently_unchanged: "
          << ( signed )( *up_cache_regions )[ i ].most_recently_unchanged << "\n";

         outs << " least_recently_unchanged: "
          << ( signed )( *up_cache_regions )[ i ].least_recently_unchanged << "\n";

         if( dump_type > dmp_summary )
         {
            if( ( *up_cache_regions )[ i ].item_cost )
               outs << "\n<cache region items>\n";

            for( size_t j = 0; j < ( *up_cache_regions )[ i ].up_cache_items->size( ); j++ )
            {
               if( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ] )
               {
                  signed counter =
                   ( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags & c_mask_for_counter );

                  unsigned changed =
                   ( ( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->flags & c_flag_for_changed );

                  outs << " item #"
                   << ( signed )( ( *up_cache_regions )[ i ].region * items_per_region + j )
                   << ", chg: " << ( changed != 0 ) << ", counter: " << counter << ", used ("
                   << ( signed )( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->prev_used_link
                   << ", "
                   << ( signed )( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->next_used_link
                   << "), chg ("
                   << ( signed )( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->prev_changed_link
                   << ", "
                   << ( signed )( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->next_changed_link
                   << "), unchg ("
                   << ( signed )( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->prev_unchanged_link
                   << ", "
                   << ( signed )( *( *up_cache_regions )[ i ].up_cache_items )[ j ]->next_unchanged_link
                   << ")\n";
               }
            }
         }
      }
   }
}

#  ifdef USE_CACHE_NAMESPACE
}
#  endif

#endif

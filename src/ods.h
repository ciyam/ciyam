// Copyright (c) 2000-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef ODS_H
#  define ODS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <limits>
#     include <string>
#     include <utility>
#  endif

#  include "macros.h"
#  include "ptypes.h"
#  include "threads.h"
#  include "auto_buffer.h"

#  ifdef CIYAM_BASE_LIB
#     ifdef CIYAM_BASE_IMPL
#        define ODS_DECL_SPEC DYNAMIC_EXPORT
#     else
#        define ODS_DECL_SPEC DYNAMIC_IMPORT
#     endif
#  else
#     define ODS_DECL_SPEC
#  endif

const int64_t c_max_int_val = std::numeric_limits< int64_t >::max( );

class read_stream;
class write_stream;

class ods;
class storable_base;

class ODS_DECL_SPEC char_buffer
{
   friend class ods;
   friend std::ostream& operator <<( std::ostream& outf, const char_buffer& c );

   public:
   char_buffer( int64_t max, int64_t len = 0, int64_t pos = 0 ) :
    max( max ),
    buf( max ),
    len( len > max ? max : len ),
    pos( pos >= len ? ( len >= max ? 0 : len ) : pos )
   {
      p_data = max ? ( char* )buf.get( ) + pos : 0;
   }

   char_buffer( const char_buffer& c )
   {
      c.init_copy( *this );
   }

   void init_copy( char_buffer& copy ) const
   {
      guard lock_buffer( buffer_lock );

      copy.max = max;
      copy.len = len;
      copy.pos = pos;

      copy.buf = buf;

      copy.p_data = copy.max ? copy.buf.get( ) + copy.pos : 0;
   }

   void operator =( const char* s );

   void operator =( const std::string& s )
   {
      guard lock_buffer( buffer_lock );

      int64_t n = std::min( max - pos, ( int64_t )s.length( ) );
      memcpy( p_data, s.data( ), n );
      len = n;
   }

   void operator =( const char_buffer& c )
   {
      guard lock_buffer( buffer_lock );

      c.assign_to( *this );
   }

   void assign_to( char_buffer& dest ) const
   {
      guard lock_buffer( buffer_lock );

      int64_t n = std::min( dest.max - dest.pos, len );
      memcpy( dest.p_data, p_data, n );
      dest.len = n;
   }

   void operator +=( const char* s );

   void operator +=( const std::string& s )
   {
      guard lock_buffer( buffer_lock );

      int64_t n = std::min( max - pos, len + ( int64_t )s.length( ) );
      memcpy( p_data + len, s.data( ), n );
      len = n;
   }

   void operator +=( const char_buffer& c )
   {
      guard lock_buffer( buffer_lock );

      c.append_to( *this );
   }

   void append_to( char_buffer& dest ) const
   {
      guard lock_buffer( buffer_lock );

      int64_t n = std::min( dest.max - dest.pos, dest.len + len );
      memcpy( dest.p_data + dest.len, p_data, n );
      dest.len = n;
   }

   bool operator ==( const char_buffer& c ) const
   {
      guard lock_buffer( buffer_lock );

      return c.is_equal_to( *this );
   }

   bool is_equal_to( const char_buffer& src ) const;

   bool operator <( const char_buffer& c ) const
   {
      guard lock_buffer( buffer_lock );

      return c.is_less_than( *this );
   }

   bool is_less_than( const char_buffer& src ) const;

   char operator [ ]( int64_t val ) const
   {
      guard lock_buffer( buffer_lock );

      if( val < len )
         return p_data[ val ];
      return '\0';
   }

   int64_t first_of( const char* matches ) const;

   bool has_one_of( const char* matches ) const;

   bool begins_with( const char_buffer& src, int64_t max = 0 ) const;

   void fill( char ch = '\0', int64_t from = 0, int64_t maxnum = c_max_int_val )
   {
      guard lock_buffer( buffer_lock );

      int fill_len = from >= max - pos ? 0 : std::min( max - pos - from, maxnum );
      if( fill_len )
         memset( p_data + from, ch, fill_len );
   }

   void copy_to( char* dest, int64_t from = 0, int64_t maxnum = c_max_int_val )
   {
      guard lock_buffer( buffer_lock );

      int64_t copy_len = from >= max - pos ? 0 : std::min( max - pos - from, maxnum );
      if( copy_len )
         memcpy( dest, p_data + from, copy_len );
   }

   void copy_from( const char* src, int64_t from = 0, int64_t maxnum = c_max_int_val )
   {
      guard lock_buffer( buffer_lock );

      int64_t copy_len = from >= max - pos ? 0 : std::min( max - pos - from, maxnum );
      if( copy_len )
         memcpy( p_data + from, src, copy_len );
   }

   // IMPORTANT: This final set of public functions should only be called within the scope of a
   // "char_buffer::lock" object if the "char_buffer" object is accessible to other threads. This
   // will ensure that no other thread can make changes whilst these accessors are being used.
   int64_t length( ) const { return len; }
   int64_t offset( ) const { return pos; }
   int64_t reserved( ) const { return max; }

   const char* data( ) const { return p_data; }

   friend read_stream& operator >>( read_stream& rs, char_buffer& cb );
   friend write_stream& operator <<( write_stream& ws, const char_buffer& cb );

   private:
   void set_pos( int64_t newpos )
   {
      guard lock_buffer( buffer_lock );

      pos = newpos >= len ? pos : newpos;
      p_data = buf.get( ) + pos;
   }

   void set_len( int64_t newlen )
   {
      guard lock_buffer( buffer_lock );

      len = newlen >= max ? len : newlen;
   }

   int64_t max;
   int64_t len;
   int64_t pos;

   char* p_data;
   auto_buffer< char > buf;

   mutable mutex buffer_lock;
};

class oid
{
   public:
   oid( int64_t num = -1 ) : num( num ) { }

   int64_t get_num( ) const { return num; }

   void set_new( ) { num = -1; }

   bool is_new( ) const { return num == -1; }

   friend bool operator ==( const oid& lhs, const oid& rhs );
   friend bool operator !=( const oid& lhs, const oid& rhs );

   private:
   int64_t num;

   friend class ods;
   friend ods ODS_DECL_SPEC& operator >>( ods& o, storable_base& s );
   friend ods ODS_DECL_SPEC& operator <<( ods& o, storable_base& s );
   friend read_stream ODS_DECL_SPEC& operator >>( read_stream& rs, oid& id );
   friend write_stream ODS_DECL_SPEC& operator <<( write_stream& ws, const oid& id );
};

inline size_t size_determiner( const oid* p_o ) { return sizeof( oid ); }

inline bool operator ==( const oid& lhs, const oid& rhs )
{
   return lhs.num == rhs.num;
}

inline bool operator !=( const oid& lhs, const oid& rhs )
{
   return !( lhs == rhs );
}

struct byte_skip
{
   byte_skip( int64_t num ) : num( num ) { }

   int64_t num;
};

read_stream ODS_DECL_SPEC& operator >>( read_stream& rs, byte_skip& bs );
write_stream ODS_DECL_SPEC& operator <<( write_stream& ws, const byte_skip& bs );

template< class T, int64_t R, class B > class storable;

struct storable_extra
{
   virtual ~storable_extra( ) { }
};

class ODS_DECL_SPEC storable_base
{
   template< class T, int64_t R, class B > friend class storable;

   public:
   enum flag
   {
      e_flag_interim_update = 0x1u
   };

   storable_base( ) : p_ods( 0 ), flags( 0 ), last_size( 0 ), last_tran_id( -1 ) { };

   storable_base( const storable_base& src )
   {
      copy_data( src );
   }

   storable_base& operator =( const storable_base& src )
   {
      copy_data( src );
      return *this;
   }

   void copy_data( const storable_base& src )
   {
      id = src.id;
      p_ods = src.p_ods;
      flags = src.flags;
      last_size = src.last_size;
      last_tran_id = src.last_tran_id;
   }

   void set_new( )
   {
      id.set_new( );
      last_size = 0;
      last_tran_id = -1;
   }

   oid get_id( ) const { return id; }

   void set_id( const oid& new_id )
   {
      id = new_id;
      last_size = 0;
      last_tran_id = -1;
   }

   ods* get_ods( ) const { return p_ods; }

   short get_flags( ) const { return flags; }

   int64_t get_last_size( ) const { return last_size; }
   int64_t get_last_tran_id( ) const { return last_tran_id; }

   bool had_interim_update( ) const { return ( flags & e_flag_interim_update ) ? true : false; }

   virtual void set_extra( storable_extra* /*p_extra*/ ) { }

   virtual int64_t get_size_of( ) const = 0;

   // NOTE: See the NOTE in "oid_pointer.h" about the purpose of this static function.
   static bool can_copy_direct( ) { return true; }

   private:
   void set_ods( ods* p ) { p_ods = p; }

   virtual void get_instance( read_stream& ) = 0;
   virtual void put_instance( write_stream& ) const = 0;

   struct read_scope
   {
      read_scope( storable_base& s, ods& o )
       :
       s( s )
      {
         s.begin_read( );
         s.set_ods( &o );
      }

      ~read_scope( )
      {
         s.set_ods( 0 );
         s.finish_read( );
      }

      storable_base& s;
   };

   friend struct read_scope;

   struct write_scope
   {
      write_scope( storable_base& s, ods& o )
       :
       s( s )
      {
         s.begin_write( );
         s.set_ods( &o );
      }

      ~write_scope( )
      {
         s.set_ods( 0 );
         s.finish_write( );
      }

      storable_base& s;
   };

   friend struct write_scope;

   oid id;
   ods* p_ods;
   short flags;

   int64_t last_size;
   int64_t last_tran_id;

   mutex storable_lock;

   friend class ods;
   friend ods ODS_DECL_SPEC& operator >>( ods& o, storable_base& s );
   friend ods ODS_DECL_SPEC& operator <<( ods& o, storable_base& s );

   protected:
   virtual void begin_read( ) { }
   virtual void finish_read( ) { }
   virtual void begin_write( ) { }
   virtual void finish_write( ) { }
};

class none { };

class final
{
   protected:
   final( ) { }
};

// KLUDGE: The function pointer approach that follows (which is used to match an exact signature in order to
// ensure that a function that takes a base class isn't being chosen) is perhaps not standard (although it's
// not clear why) so it is not being used when either GNU or Borland compilers are in use. It is still being
// used for Visual C++ so that at least compilation with it will reveal any missing required functions.
#  ifdef __GNUG__
#     define ODS_DO_NOT_USE_FUNCTION_POINTERS
#  endif
#  ifdef __BORLANDC__
#     define ODS_DO_NOT_USE_FUNCTION_POINTERS
#  endif

#  ifdef ODS_NON_FINAL_STORABLE
template< class T, int64_t R = 0, class B = none > class storable : public T, public B
#  else
template< class T, int64_t R = 0, class B = none > class storable : public T, public B, virtual private final
#  endif
{
   public:
   storable( ) { }
   template< typename U > explicit storable( U& u ) : T( u ) { }
   template< typename U, typename V > storable( U& u, V& v ) : T( u, v ) { }
   template< typename U, typename V, typename W > storable( U& u, V& v, W& w ) : T( u, v, w ) { }

   int64_t get_size_of( ) const
   {
#  ifndef ODS_DO_NOT_USE_FUNCTION_POINTERS
      int64_t ( *p_func )( const T& );
      p_func = &size_of;
      int64_t size = ( *p_func )( *this );
#  else
      int64_t size = size_of( *this );
#  endif

      // IMPORTANT: The R value is used to round up the storable size allowing storable objects to grow by
      // this amount before needing to be moved to the end of the data file (reducing the amount of wasted
      // space that might otherwise be created each time the storable size is increased).
      int64_t round( R );

      if( round && size % round )
         size += round - ( size % round );

      return size;
   }

   void get_instance( read_stream& rs )
   {
#  ifndef ODS_DO_NOT_USE_FUNCTION_POINTERS
      read_stream& ( *p_func )( read_stream&, T& );
      p_func = &operator >>;
      ( *p_func )( rs, *this );
#  else
      operator >>( rs, *this );
#  endif
   }

   void put_instance( write_stream& ws ) const
   {
#  ifndef ODS_DO_NOT_USE_FUNCTION_POINTERS
      write_stream& ( *p_func )( write_stream&, const T& );
      p_func = &operator <<;
      ( *p_func )( ws, *this );
#  else
      operator <<( ws, *this );
#  endif
   }
};

struct transaction_op
{
   enum op_type
   {
      e_op_type_none,
      e_op_type_update,
      e_op_type_append,
      e_op_type_destroy
   };

   struct data_t
   {
      oid id;
      int64_t pos;
      int64_t size;
      int64_t old_tran_id;
      int64_t old_tran_op;
   } data;

   op_type type;

   transaction_op( )
   {
      data.pos = 0;
      data.size = 0;
      data.old_tran_id = 0;
      data.old_tran_op = 0;

      type = e_op_type_none;
   }
};

struct transaction_level_info
{
   transaction_level_info( )
    :
    size( 0 ),
    offset( 0 ),
    op_count( 0 ),
    op_offset( 0 )
   {
   }

   int64_t size;
   int64_t offset;
   int64_t op_count;
   int64_t op_offset;
};

class ods_index_cache_buffer;

const int64_t c_trans_ops_per_item = 64;
const int64_t c_trans_bytes_per_item = 4096;

struct trans_op_buffer
{
   transaction_op::data_t item[ c_trans_ops_per_item ];
};

struct trans_data_buffer
{
   char data[ c_trans_bytes_per_item ];
};

class ods_index_entry;

class ODS_DECL_SPEC ods
{
   public:
   enum open_mode
   {
      e_open_mode_exist,
      e_open_mode_not_exist,
      e_open_mode_create_if_not_exist
   };

   enum share_mode
   {
      e_share_mode_shared,
      e_share_mode_exclusive
   };

   static ods* instance( ods* p_ods = 0, bool force_assign = false );

   ods( const ods& o );

   ods( const char* name, open_mode o_mode,
    share_mode s_mode = e_share_mode_shared, bool using_tranlog = false );

   virtual ~ods( );

   bool is_okay( ) const { return okay; }

   bool is_new( ) const;
   bool is_corrupt( ) const;

   bool is_bulk_locked( ) const;

   bool is_bulk_dump_locked( ) const;
   bool is_bulk_read_locked( ) const;
   bool is_bulk_write_locked( ) const;

   bool is_using_transaction_log( ) const;

   void repair_if_corrupt( );

   std::string get_meta( ) const { return meta; }
   void set_meta( const std::string& new_meta ) { meta = new_meta; }

   void destroy( const oid& id );

   int64_t get_size( const oid& id );

   void move_free_data_to_end( );

   void dump_file_info( std::ostream& os );
   void dump_free_list( std::ostream& os );
   void dump_index_entry( std::ostream& os, int64_t num );
   void dump_instance_data( std::ostream& os, int64_t num, bool only_pos_and_size );

   void dump_transaction_log( std::ostream& os, bool header_only );

   int64_t get_total_entries( );

   int64_t get_session_review_total( );
   int64_t get_session_create_total( );
   int64_t get_session_revive_total( );
   int64_t get_session_update_total( );
   int64_t get_session_delete_total( );

   int64_t get_transaction_id( ) const;
   int64_t get_transaction_level( ) const;

   int64_t get_next_transaction_id( ) const;

   struct bulk_dump;
   struct bulk_read;
   struct bulk_write;

   struct bulk_base
   {
      friend struct bulk_dump;
      friend struct bulk_read;
      friend struct bulk_write;

      void pause( );

      private:
      bulk_base( ods& o ) : o( o ) { }

      protected:
      ods& o;
   };

   friend struct bulk_base;

   struct bulk_dump : bulk_base
   {
      bulk_dump( ods& o );
      ~bulk_dump( );
   };

   friend struct bulk_dump;

   struct bulk_read : bulk_base
   {
      bulk_read( ods& o );
      ~bulk_read( );
   };

   friend struct bulk_read;

   struct bulk_write : bulk_base
   {
      bulk_write( ods& o );
      ~bulk_write( );
   };

   friend struct bulk_write;

   struct transaction
   {
      transaction( ods& o, bool is_not_dummy = true );
      ~transaction( );

      void commit( );
      void rollback( );

      private:
      ods& o;
      bool is_dummy;
      bool can_commit;
      bool has_committed;
   };

   friend struct transaction;

   private:
   bool okay;
   std::string meta;

   struct impl;
   impl* p_impl;

   friend struct impl;

   class ods_stream;
   friend class ods_stream;

   struct file_scope;
   friend struct file_scope;

   struct header_file_lock;
   friend struct header_file_lock;

   void open_store( );
   void close_store( );

   void bulk_operation_open( );
   void bulk_operation_close( );
   void bulk_operation_start( );
   void bulk_operation_pause( );
   void bulk_operation_finish( );

   void transaction_start( );
   void transaction_commit( );
   void transaction_rollback( );
   void transaction_completed( );

   void lock_header_file( );
   void unlock_header_file( );

   void data_and_index_write( bool flush = true );

   int64_t log_append_offset( );

   int64_t append_log_entry( int64_t tx_id );

   void log_entry_commit( int64_t entry_offset, int64_t commit_offs, int64_t commit_items );

   void append_log_entry_item( int64_t num,
    const ods_index_entry& index_entry, unsigned char flags, int64_t log_entry_offs = 0 );

   void rollback_dead_transactions( );
   void restore_from_transaction_log( );

   ods& operator =( const ods& o );
   bool operator ==( const ods& o );

   void read( unsigned char* p_buf, int64_t len );
   void write( const unsigned char* p_buf, int64_t len );

   void set_read_data_pos( int64_t pos, bool force_get = false );
   void set_write_data_pos( int64_t pos );

   void adjust_read_data_pos( int64_t adjust );
   void adjust_write_data_pos( int64_t adjust );

   void read_data_bytes( char* p_dest, int64_t len );
   void write_data_bytes( const char* p_src, int64_t len );

   int64_t data_read_buffer_num;
   int64_t data_read_buffer_offs;
   int64_t data_write_buffer_num;
   int64_t data_write_buffer_offs;

   bool is_in_read;
   bool is_in_write;

   friend class ods_index_cache_buffer;

   void read_index_entry( ods_index_entry& index_entry, int64_t num );
   void write_index_entry( const ods_index_entry& index_entry, int64_t num );

   int64_t index_item_buffer_num;
   bool has_written_index_item;

   void read_transaction_op( transaction_op& op, int64_t num );
   void write_transaction_op( transaction_op& op );

   void set_read_trans_data_pos( int64_t pos );
   void set_write_trans_data_pos( int64_t pos, int64_t old_trans_total_size );

   void adjust_read_trans_data_pos( int64_t adjust );
   void adjust_write_trans_data_pos( int64_t adjust );

   void read_trans_data_bytes( char* p_dest, int64_t len );
   void write_trans_data_bytes( const char* p_src, int64_t len );

   bool had_interim_trans_op_write;
   bool had_interim_trans_data_write;

   int64_t trans_read_ops_buffer_num;
   int64_t trans_read_data_buffer_num;
   int64_t trans_read_data_buffer_offs;

   bool has_written_trans_op;

   int64_t trans_write_ops_buffer_num;
   int64_t trans_write_data_buffer_num;
   int64_t trans_write_data_buffer_offs;

   trans_op_buffer trans_read_ops_buffer;
   trans_op_buffer trans_write_ops_buffer;

   trans_data_buffer trans_read_buffer;
   trans_data_buffer trans_write_buffer;

   mutable bool permit_copy;

   mutex read_lock;
   mutex write_lock;
   mutex index_lock;

   int64_t bytes_used;
   int64_t bytes_reserved;

   int64_t bytes_stored;
   int64_t bytes_retrieved;

   int64_t current_read_object_num;
   int64_t current_write_object_num;

   friend ods ODS_DECL_SPEC& operator >>( ods& o, storable_base& s );
   friend ods ODS_DECL_SPEC& operator <<( ods& o, storable_base& s );

   friend bool storable_base::had_interim_update( ) const;
};

struct scoped_ods_instance
{
   scoped_ods_instance( ods& ods )
    :
    p_ods( ods::instance( ) )
   {
      ods::instance( &ods, true );
   }

   ~scoped_ods_instance( )
   {
      ods::instance( p_ods, true );
   }

   ods* p_ods;
};

#endif


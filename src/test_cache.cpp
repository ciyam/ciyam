// Copyright (c) 2001-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <string>
#  include <sstream>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <fcntl.h>
#     include <unistd.h>
#  endif
#  include <cstdio>
#  include <sys/stat.h>
#endif

#  ifndef O_BINARY
#     define O_BINARY 0
#  endif

#ifdef __GNUG__
#  define _open open
#  define _read ::read
#  define _write ::write
#  define _close close
#  define _lseek lseek
#  define _O_RDWR O_RDWR
#  define _O_CREAT O_CREAT
#  define _O_WRONLY O_WRONLY
#endif

#include "cache.h"

#include "pointers.h"
#include "utilities.h"
#include "auto_buffer.h"
#include "console_commands.h"

//#define TEST_CACHE_DEBUG

using namespace std;
using namespace cache;

#include "test_cache.cmh"

int ctor_throw_count;
int fetch_throw_count;
int store_throw_count;

const unsigned c_test_item_size = 1024;

//#define USE_NON_POD

#ifdef USE_NON_POD
#  pragma message ( "Compiling testbed for cache containing UDT." )
#endif

#ifdef TEST_CACHE_DEBUG
int total_item_ctor_calls;
int total_item_cctor_calls;
int total_item_assop_calls;
int total_item_strass_calls;
#endif

const char* const c_app_title = "test_cache";
const char* const c_app_version = "0.1";

const char* const c_error_prefix = "error: ";

bool g_application_title_called = false;

string application_title( app_info_request request )
{
   g_application_title_called = true;

   if( request == e_app_info_request_title )
      return string( c_app_title );
   else if( request == e_app_info_request_version )
      return string( c_app_version );
   else if( request == e_app_info_request_title_and_version )
   {
      string title( c_app_title );
      title += " v";
      title += string( c_app_version );

      return title;
   }
   else
   {
      ostringstream osstr;
      osstr << "unknown app_info_request: " << request;
      throw runtime_error( osstr.str( ) );
   }
}

#ifndef USE_NON_POD
template< unsigned block_size > struct mem_block
{
   char buf[ block_size ];

   unsigned size( ) const { return block_size; }

   mem_block( )
   {
      if( ctor_throw_count && --ctor_throw_count == 0 )
         throw "ctor exception thrown";
#  ifdef TEST_CACHE_DEBUG
      ++total_item_ctor_calls;
#  endif
      memset( buf, 0, block_size );
   }

#  ifdef TEST_CACHE_DEBUG
   mem_block( const mem_block< block_size >& src )
   {
      ++total_item_cctor_calls;
      memcpy( buf, src.buf, block_size );
   }

   mem_block& operator =( const mem_block< block_size >& src )
   {
      ++total_item_assop_calls;
      memcpy( buf, src.buf, block_size );
      return *this;
   }
#  endif

   mem_block& operator =( const string& str )
   {
#  ifdef TEST_CACHE_DEBUG
      ++total_item_strass_calls;
#  endif
      memcpy( buf,
       str.c_str( ), min( str.length( ), ( string::size_type )( block_size - 1 ) ) );

      if( str.length( ) < block_size - 1 )
         buf[ str.length( ) ] = 0;
      else
         buf[ block_size - 1 ] = 0;

      return *this;
   }

   operator string( );
};

template< unsigned block_size > mem_block< block_size >::operator string( )
{
   return string( buf );
}
#else
template< unsigned block_size > struct mem_block
{
   auto_buffer< char > buf;

   unsigned size( ) const { return block_size; }

   mem_block( )
    :
    buf( block_size )
   {
      if( ctor_throw_count && --ctor_throw_count == 0 )
         throw "ctor exception thrown";
#  ifdef TEST_CACHE_DEBUG
      ++total_item_ctor_calls;
#  endif
      memset( buf.get( ), 0, block_size );
   }

   mem_block( const mem_block< block_size >& src )
    :
    buf( src.buf )
   {
#  ifdef TEST_CACHE_DEBUG
      ++total_item_cctor_calls;
#  endif
   }

   mem_block& operator =( const mem_block< block_size >& src )
   {
#  ifdef TEST_CACHE_DEBUG
      ++total_item_assop_calls;
#  endif
      buf = src.buf;
      return *this;
   }

   mem_block& operator =( const string& str )
   {
#  ifdef TEST_CACHE_DEBUG
      ++total_item_strass_calls;
#  endif
      memcpy( buf.get( ), str.c_str( ), min( str.length( ), ( unsigned )( block_size - 1 ) ) );

      if( str.length( ) < block_size - 1 )
         buf.get( )[ str.length( ) ] = 0;
      else
         buf.get( )[ block_size - 1 ] = 0;

      return *this;
   }

   operator string( );
};

template< unsigned block_size > mem_block< block_size >::operator string( )
{
   return string( buf.get( ) );
}
#endif

unsigned total_physical_fetch_count;
unsigned total_physical_store_count;

template< typename T > class test_cache : public cache_base< T >
{
   public:
   test_cache( unsigned max_cache_items,
    unsigned items_per_region, unsigned regions_in_cache = 1,
    bool use_placement_new = false, bool allow_lazy_writes = true )
    :
    cache_base< T >( max_cache_items, items_per_region,
     regions_in_cache, use_placement_new, allow_lazy_writes )
   {
      handle = -1;
   }

   void open_file_normal( const char* p_name )
   {
      if( handle != -1 )
         throw runtime_error( "attempt to open file when already open" );

      handle = _open( p_name, _O_CREAT | _O_RDWR | O_BINARY, S_IREAD | S_IWRITE );
      if( handle < 0 )
         throw runtime_error( "opening cache file for read and write" );
   }

   void open_file_read_only( const char* p_name )
   {
      if( handle != -1 )
         throw runtime_error( "error attempt to open file when already open" );

      handle = _open( p_name, O_RDONLY | O_BINARY );
      if( handle < 0 )
         throw runtime_error( "opening cache file for read" );
   }

   void open_file_write_only( const char* p_name )
   {
      if( handle != -1 )
         throw runtime_error( "error attempt to open file when already open" );

      handle = _open( p_name, _O_CREAT | _O_WRONLY | O_BINARY, S_IREAD | S_IWRITE );
      if( handle < 0 )
         throw runtime_error( "opening cache file for write" );
   }

   void close_file( )
   {
      if( handle >= 0 && _close( handle ) == -1 )
         throw runtime_error( "unexpected file close error" );
      handle = c_npos;
   }

   protected:
   void perform_fetch( T& data, unsigned num )
   {
      if( fetch_throw_count && --fetch_throw_count == 0 )
         throw "fetch exception thrown";

      if( handle < 0 )
         throw runtime_error( "fetch without file open" );

      if( _lseek( handle, ( num * data.size( ) ), SEEK_SET ) == -1 )
         throw runtime_error( "unexpected file seek error" );
#ifndef USE_NON_POD
      if( _read( handle, ( void* )data.buf, data.size( ) ) == -1 )
         throw runtime_error( "unexpected file read error" );
#else
      if( _read( handle, ( void* )data.buf.get( ), data.size( ) ) == -1 )
         throw runtime_error( "unexpected file read error" );
#endif

#ifdef TEST_CACHE_DEBUG
      cout << "*** physical fetch for " << num << " ***\n";
#endif
      ++total_physical_fetch_count;
   }

   void perform_store( const T& data, unsigned num )
   {
      if( store_throw_count && --store_throw_count == 0 )
         throw "store exception thrown";

      if( handle < 0 )
         throw runtime_error( "store without file open" );

      if( _lseek( handle, ( num * data.size( ) ), SEEK_SET ) == -1 )
         throw runtime_error( "unexpected file seek error" );
#ifndef USE_NON_POD
      if( _write( handle, ( void* )data.buf, data.size( ) ) == -1 )
         throw runtime_error( "unexpected file write error" );
#else
      if( _write( handle, ( void* )data.buf.get( ), data.size( ) ) == -1 )
         throw runtime_error( "unexpected file write error" );
#endif

#ifdef TEST_CACHE_DEBUG
      cout << "*** physical store for " << num << " ***\n";
#endif
      ++total_physical_store_count;
   }

   int handle;
};

const char* const c_cache_file_name = "test_cache.dat";

const unsigned c_default_max_items = 5;
const unsigned c_default_items_per_region = 10;
const unsigned c_default_number_of_regions = 1;

class test_cache_command_functor;

class test_cache_command_handler : public console_command_handler
{
   friend class test_cache_command_functor;

   public:
   test_cache_command_handler( )
    :
    retain( true ),
    limit( 10 ),
    max_cache_items( c_default_max_items ),
    items_per_region( c_default_items_per_region ),
    number_of_regions( c_default_number_of_regions )
   {
      set_custom_startup_options( 3, "[<max_items=5> [<items_per_region=10> [<num_regions=1>]]]" );
   }

   void init( )
   {
#ifdef USE_NON_POD
      ap_cache.reset( new test_cache<
       mem_block< c_test_item_size > >( max_cache_items, items_per_region, number_of_regions ) );
#else
      ap_cache.reset( new test_cache<
       mem_block< c_test_item_size > >( max_cache_items, items_per_region, number_of_regions, true ) );
#endif
      ap_cache->open_file_normal( c_cache_file_name );
   }

   private:
   unsigned max_cache_items;
   unsigned items_per_region;
   unsigned number_of_regions;

   auto_ptr< test_cache< mem_block< c_test_item_size > > > ap_cache;

   mem_block< c_test_item_size > test_item;

   bool retain;
   unsigned limit;

   void process_custom_startup_option( size_t num, const string& option );
};

void test_cache_command_handler::process_custom_startup_option( size_t num, const string& option )
{
   for( string::size_type i = 0; i < option.size( ); i++ )
   {
      if( option[ i ] < '0' || option[ i ] > '9' )
         throw runtime_error( "unexpected non-numeric value '" + option + "'" );
   }

   if( num == 0 )
      max_cache_items = atoi( option.c_str( ) );
   else if( num == 1 )
      items_per_region = atoi( option.c_str( ) );
   else
      number_of_regions = atoi( option.c_str( ) );
}

class test_cache_command_functor : public command_functor
{
   public:
   test_cache_command_functor( test_cache_command_handler& cache_handler )
    : command_functor( cache_handler ),
    cache_handler( cache_handler ),
    ap_cache( cache_handler.ap_cache ),
    test_item( cache_handler.test_item ),
    retain( cache_handler.retain ),
    limit( cache_handler.limit )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   test_cache_command_handler& cache_handler;

   auto_ptr< test_cache< mem_block< c_test_item_size > > >& ap_cache;

   mem_block< c_test_item_size >& test_item;

   bool& retain;
   unsigned& limit;
};

void test_cache_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   string::size_type pos;

   try
   {
      if( command == c_cmd_test_cache_get )
      {
         string items( get_parm_val( parameters, c_cmd_test_cache_get_items ) );
         string filename( get_parm_val( parameters, c_cmd_test_cache_get_filename ) );

         unsigned start, finish;
         pos = items.find( '-' );
         if( pos != string::npos )
            items[ pos ] = '\0';

         start = atoi( items.c_str( ) );
         if( pos == string::npos )
            finish = start;
         else
            finish = atoi( &items[ pos + 1 ] );

         if( finish < start )
         {
            handler.issue_command_response( to_string( c_error_prefix ) + "finish must be >= start" );
            return;
         }

         fstream outf;
         if( filename.length( ) )
         {
            outf.open( filename.c_str( ), ios::out );

            if( !outf )
            {
               handler.issue_command_response(
                to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );

               return;
            }
         }

         unsigned old_fetch_count = total_physical_fetch_count;
         unsigned old_store_count = total_physical_store_count;

         for( unsigned i = start; i <= finish; i++ )
         {
            test_item = ap_cache->get( i, retain );

            if( filename.length( ) )
            {
               items = static_cast< string >( test_item );
               outf << items << endl;
            }
            else if( !limit || ( finish - i < limit ) )
            {
               items = static_cast< string >( test_item );
               handler.issue_command_response( items );
            }
         }

         if( old_fetch_count != total_physical_fetch_count )
            handler.issue_command_response( "*** physical fetch count = "
             + to_string( total_physical_fetch_count - old_fetch_count ) + " ***" );

         if( old_store_count != total_physical_store_count )
            handler.issue_command_response( "*** physical store count = "
             + to_string( total_physical_store_count - old_store_count ) + " ***" );
      }
      else if( command == c_cmd_test_cache_put )
      {
         string items( get_parm_val( parameters, c_cmd_test_cache_put_items ) );

         unsigned start, finish;
         string::size_type rpos = items.find( '-' );
         if( rpos != string::npos )
            items[ rpos ] = '\0';

         start = atoi( items.c_str( ) );
         if( rpos == string::npos )
            finish = start;
         else
            finish = atoi( &items[ rpos + 1 ] );

         if( finish < start )
         {
            handler.issue_command_response( to_string( c_error_prefix ) + "finish must be >= start" );
            return;
         }

         string data( get_parm_val( parameters, c_cmd_test_cache_put_data ) );

         char buf[ c_test_item_size ];
         unsigned old_fetch_count = total_physical_fetch_count;
         unsigned old_store_count = total_physical_store_count;

         for( unsigned i = start; i <= finish; i++ )
         {
            sprintf( buf, data.c_str( ), i );

            test_item = buf;
            ap_cache->put( test_item, i, retain );
         }

         if( old_fetch_count != total_physical_fetch_count )
            handler.issue_command_response( "*** physical fetch count = "
             + to_string( total_physical_fetch_count - old_fetch_count ) + " ***" );

         if( old_store_count != total_physical_store_count )
            handler.issue_command_response( "*** physical store count = "
             + to_string( total_physical_store_count - old_store_count ) + " ***" );
      }
      else if( command == c_cmd_test_cache_mark )
      {
         string items( get_parm_val( parameters, c_cmd_test_cache_mark_items ) );

         bool is_new = false;
         if( has_parm_val( parameters, c_cmd_test_cache_mark_new ) )
            is_new = true;

         unsigned start, finish;
         string::size_type rpos = items.find( '-' );
         if( rpos != string::npos )
            items[ rpos ] = '\0';

         start = atoi( items.c_str( ) );
         if( rpos == string::npos )
            finish = start;
         else
            finish = atoi( &items[ rpos + 1 ] );

         if( finish < start )
         {
            handler.issue_command_response( to_string( c_error_prefix ) + "finish must be >= start", true );
            return;
         }

         for( unsigned i = start; i <= finish; i++ )
            ap_cache->mark_cached_item( i, is_new );
      }
      else if( command == c_cmd_test_cache_dump )
      {
         test_cache< mem_block< c_test_item_size > >::dump dump_type;

         string filename( get_parm_val( parameters, c_cmd_test_cache_dump_filename ) );

         if( has_parm_val( parameters, c_cmd_test_cache_dump_info ) )
            dump_type = test_cache< mem_block< c_test_item_size > >::dmp_info;
         else if( has_parm_val( parameters, c_cmd_test_cache_dump_summary ) )
            dump_type = test_cache< mem_block< c_test_item_size > >::dmp_summary;
         else
            dump_type = test_cache< mem_block< c_test_item_size > >::dmp_detailed;

         if( !filename.length( ) )
         {
            handler.issue_command_response( "total_physical_store_count = " + to_string( total_physical_store_count ) );
            handler.issue_command_response( "total_physical_fetch_count = " + to_string( total_physical_fetch_count ) );

            handler.issue_command_response( "" );

            ap_cache->dump_cached_item_info( *cache_handler.get_std_out( ), dump_type );
         }
         else
         {
            ofstream outf( filename.c_str( ) );

            if( !outf )
            {
               handler.issue_command_response(
                to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );

               return;
            }

            outf << "total_physical_store_count = " << total_physical_store_count << '\n';
            outf << "total_physical_fetch_count = " << total_physical_fetch_count << "\n\n";

            ap_cache->dump_cached_item_info( outf, dump_type );
         }
      }
      else if( command == c_cmd_test_cache_flush )
      {
         bool is_new = false;
         if( has_parm_val( parameters, c_cmd_test_cache_flush_new ) )
            is_new = true;

         unsigned old_store_count = total_physical_store_count;
         ap_cache->flush( is_new );

         if( old_store_count != total_physical_store_count )
            handler.issue_command_response( "*** physical store count = "
             + to_string( total_physical_store_count - old_store_count ) + " ***" );
      }
      else if( command == c_cmd_test_cache_limit )
      {
         if( has_parm_val( parameters, c_cmd_test_cache_limit_num ) )
            limit = atoi( get_parm_val( parameters, c_cmd_test_cache_limit_num ).c_str( ) );
         else
            handler.issue_command_response( to_string( limit ) );
      }
      else if( command == c_cmd_test_cache_retain )
      {
         bool retain = false;
         if( has_parm_val( parameters, c_cmd_test_cache_retain_true ) )
            retain = true;

         retain = retain;
      }
      else if( command == c_cmd_test_cache_throw )
      {
         int delay = 0;
         if( has_parm_val( parameters, c_cmd_test_cache_throw_delay ) )
            delay = atoi( get_parm_val( parameters, c_cmd_test_cache_throw_delay ).c_str( ) );

         if( has_parm_val( parameters, c_cmd_test_cache_throw_ctor ) )
            ctor_throw_count = delay + 1;
         else if( has_parm_val( parameters, c_cmd_test_cache_throw_fetch ) )
            fetch_throw_count = delay + 1;
         else
            store_throw_count = delay + 1;
      }
      else if( command == c_cmd_test_cache_clear )
      {
         if( !has_parm_val( parameters, c_cmd_test_cache_clear_stats ) )
            ap_cache->clear( );
         else
            ap_cache->clear_statistics( );
      }
      else if( command == c_cmd_test_cache_max )
      {
         unsigned old_fetch_count = total_physical_fetch_count;
         unsigned old_store_count = total_physical_store_count;

         if( has_parm_val( parameters, c_cmd_test_cache_max_items ) )
            ap_cache->set_max_cache_items(
             atoi( get_parm_val( parameters, c_cmd_test_cache_max_num ).c_str( ) ) );
         else if( has_parm_val( parameters, c_cmd_test_cache_max_itemspr ) )
            ap_cache->set_items_per_region(
             atoi( get_parm_val( parameters, c_cmd_test_cache_max_num ).c_str( ) ) );
         else
            ap_cache->set_regions_in_cache(
             atoi( get_parm_val( parameters, c_cmd_test_cache_max_num ).c_str( ) ) );

         if( old_fetch_count != total_physical_fetch_count )
            handler.issue_command_response( "*** physical fetch count = "
             + to_string( total_physical_fetch_count - old_fetch_count ) + " ***" );

         if( old_store_count != total_physical_store_count )
            handler.issue_command_response( "*** physical store count = "
             + to_string( total_physical_store_count - old_store_count ) + " ***" );
      }
      else if( command == c_cmd_test_cache_exit )
      {
   #ifdef TEST_CACHE_DEBUG
         cout << "total_item_ctor_calls = " << total_item_ctor_calls << endl;
         cout << "total_item_cctor_calls = " << total_item_cctor_calls << endl;
         cout << "total_item_assop_calls = " << total_item_assop_calls << endl;
         cout << "total_item_strass_calls = " << total_item_strass_calls << endl;
   #endif
         handler.set_finished( );
      }
   }
   catch( const char* msg )
   {
      handler.issue_command_response( msg, true );
   }
}

command_functor* test_cache_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new test_cache_command_functor( dynamic_cast< test_cache_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   test_cache_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         processor.process_commands( );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.init( );

      cmd_handler.add_commands( 0,
       test_cache_command_functor_factory, ARRAY_PTR_AND_SIZE( test_cache_command_definitions ) );

      console_command_processor processor( cmd_handler );
      processor.process_commands( );
   }
   catch( exception& x )
   {
      if( !g_application_title_called && !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      if( !g_application_title_called && !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;
      cerr << "error: unexpected exception occurred" << endl;
      return 2;
   }
}


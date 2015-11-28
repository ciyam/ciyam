// Copyright (c) 2000-2015 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstdio>
#  include <cstddef>
#  include <string>
#  include <sstream>
#  include <fstream>
#  include <iomanip>
#  include <iostream>
#  ifdef __GNUG__
#     include <limits.h>
#     include <unistd.h>
#  endif
#endif

#define BTREE_IMPL

#include "ods.h"
#include "btree.h"
#include "pointers.h"
#include "utilities.h"
#include "storable_btree.h"
#include "console_commands.h"

//#define USE_BULK_PAUSE

using namespace std;
using namespace btree;

#include "btstore.cmh"

const char* const c_opt_exclusive = "-x";

ostream& operator <<( ostream& os, const oid& id )
{
   os << id.get_num( );
   return os;
}

const char* const c_app_title = "btstore";
const char* const c_app_version = "0.1";

const char* const c_cmd_exclusive = "x";

bool g_shared_access = true;

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

struct test_item
{
   test_item( ) { }
   test_item( const char* str ) { val = str; }

   string& str( ) { return val; }
   const string& const_str( ) const { return val; }

   bool operator <( const test_item& src ) const { return val < src.val; }
   bool operator ==( const test_item& src ) const { return val == src.val; }

   friend int_t size_of( const test_item& t );

   friend read_stream& operator >>( read_stream& rs, test_item& t );
   friend write_stream& operator <<( write_stream& ws, const test_item& t );

   friend ostream& operator <<( ostream& outf, const test_item& t );
   friend istream& operator >>( istream& outf, test_item& t );

   string val;
};

int_t size_of( const test_item& t )
{
   return sizeof( size_t ) + t.val.length( );
}

read_stream& operator >>( read_stream& rs, test_item& t )
{
   rs >> t.val;
   return rs;
}

write_stream& operator <<( write_stream& ws, const test_item& t )
{
   ws << t.val;
   return ws;
}

ostream& operator <<( ostream& outf, const test_item& t )
{
   outf << t.val;
   return outf;
}

istream& operator >>( istream& inpf, test_item& t )
{
   inpf >> t.val;
   return inpf;
}

bool is_simple_contains_expression( const std::string& s )
{
   if( s.length( ) < 3 || s.find( '?' ) != string::npos )
      return false;

   if( s[ 0 ] != '*' && s[ s.length( ) - 1 ] != string::npos )
      return false;

   if( s.find( '*', 1 ) != s.length( ) - 1 )
      return false;

   return true;
}

inline size_t first_wildcard_character_pos( const string& str )
{
   return str.find_first_of( "?*" );
}

template< class S > struct wildcard_compare_less_functor : public std::binary_function< S, S, bool >
{
   wildcard_compare_less_functor( )
    :
    truncated_comparision( false )
   {
   }

   void use_truncated_comparision( bool val ) const
   {
      truncated_comparision = val;
   }

   bool operator( )( const S& x, const S& y ) const
   {
      if( !truncated_comparision )
         return less< S >( )( x, y );

      size_t xlen = x.const_str( ).length( );
      size_t ylen = y.const_str( ).length( );

      return x.const_str( ).compare( 0, min( xlen, ylen ), y.const_str( ), 0, min( xlen, ylen ) ) <= 0;
   }

   private:
   mutable bool truncated_comparision;
};

template< class S > struct wildcard_compare_equal_functor : public std::binary_function< S, S, bool >
{
   bool operator( )( const S& x, const S& y ) const
   {
      return wildcard_match( x.const_str( ).c_str( ), y.const_str( ).c_str( ) );
   }
};

template< typename B > void reposition_iterator(
 B& bt, typename B::item_type& item, typename B::iterator& iter )
{
   if( iter != bt.end( ) )
      iter = bt.lower_bound( item );
}

const unsigned char c_test_items_per_node = 255;

typedef storable< storable_btree_base< test_item > > btree_type;
typedef storable< storable_node_base< test_item >, storable_node_base< test_item >::c_round_to_value > btree_node_type;
typedef storable_node_manager< test_item > btree_node_manager_type;

typedef bt_transaction< test_item, std::less< test_item >, btree_node_type, btree_node_manager_type > btree_trans_type;

class btstore_startup_functor : public command_functor
{
   public:
   btstore_startup_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& /*parameters*/ )
   {
      if( command == c_cmd_exclusive )
         g_shared_access = false;
   }
};

class btstore_command_functor;

class btstore_command_handler : public console_command_handler
{
   friend class btstore_command_functor;

   public:
   btstore_command_handler( )
    :
    list_iter_limit( 10 )
   {
   }

   void init( const char* p_file_name );

   private:
   auto_ptr< ods > ap_ods;

   int list_iter_limit;

   btree_type::item_type item;

   wildcard_compare_less_functor< btree_type::item_type > compare_less;
   wildcard_compare_equal_functor< btree_type::item_type > compare_equal;

   auto_ptr< btree_type > ap_btree;

   btree_type::iterator iter;
};

void btstore_command_handler::init( const char* p_file_name )
{
   ap_ods.reset( new ods( p_file_name, ods::e_open_mode_create_if_not_exist,
    ( g_shared_access ? ods::e_share_mode_shared : ods::e_share_mode_exclusive ) ) );

   ods::bulk_write bulk_write( *ap_ods );
   if( !g_shared_access && !ap_ods->is_new( ) )
      ap_ods->rollback_dead_transactions( );

   ap_btree.reset( new btree_type( *ap_ods ) );

   iter = ap_btree->end( );
   ap_btree->set_items_per_node( c_test_items_per_node );

   if( !ap_ods->is_new( ) )
   {
      ap_btree->set_id( 0 );
      *ap_ods >> *ap_btree;

      iter = ap_btree->begin( );
      if( iter != ap_btree->end( ) )
         item.val = iter->val;
   }
   else
      *ap_ods << *ap_btree;
}

class btstore_command_functor : public command_functor
{
   public:
   btstore_command_functor( btstore_command_handler& btstore_handler )
    : command_functor( btstore_handler ),
    ap_ods( btstore_handler.ap_ods ),
    list_iter_limit( btstore_handler.list_iter_limit ),
    item( btstore_handler.item ),
    compare_less( btstore_handler.compare_less ),
    compare_equal( btstore_handler.compare_equal ),
    ap_btree( btstore_handler.ap_btree ),
    iter( btstore_handler.iter )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   auto_ptr< ods >& ap_ods;

   int& list_iter_limit;

   btree_type::item_type& item;

   wildcard_compare_less_functor< btree_type::item_type >& compare_less;
   wildcard_compare_equal_functor< btree_type::item_type >& compare_equal;

   auto_ptr< btree_type >& ap_btree;

   btree_type::iterator& iter;
};

void btstore_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_btstore_add )
   {
      ods::bulk_write bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      item.val = get_parm_val( parameters, c_cmd_parm_btstore_add_item );

      iter = ap_btree->insert( item );
   }
   else if( command == c_cmd_btstore_find )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;

      item.val = get_parm_val( parameters, c_cmd_parm_btstore_find_item );
      iter = ap_btree->find( item );
      if( iter == ap_btree->end( ) )
         cout << "*** item not found ***" << endl;
      else
      {
         item.val = iter->val;
         cout << iter->val << endl;
      }
   }
   else if( command == c_cmd_btstore_match || command == c_cmd_btstore_mcount )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;

      string expr( get_parm_val( parameters, c_cmd_parm_btstore_match_expr ) );
      size_t pos = first_wildcard_character_pos( expr );
      item.val = expr.substr( 0, pos );

#ifdef USE_BULK_PAUSE
      int count = 0;
      int last_tran_id = ap_btree->get_last_tran_id( );
#endif

      iter = ap_btree->lower_bound( item );

      if( pos != string::npos )
         compare_less.use_truncated_comparision( true );
      else
         compare_less.use_truncated_comparision( false );

      size_t count = 0;
      if( iter != ap_btree->end( ) )
      {
         btree_type::item_type tmp_item;
         tmp_item.val = expr;

         bool use_boyer_moore = false;
         auto_ptr< boyer_moore > ap_bm;
         if( is_simple_contains_expression( expr ) )
         {
            use_boyer_moore = true;
            ap_bm.reset( new boyer_moore( expr.substr( 1, expr.length( ) - 2 ) ) );
         }

         do
         {
#ifdef USE_BULK_PAUSE
            if( ++count % 5000 == 0 )
            {
               btree_type::item_type tmp_item;

               tmp_item.val = iter->val;

               bulk.pause( );
               if( last_tran_id != ap_btree->get_last_tran_id( ) )
               {
                  *ap_ods >> *ap_btree;
                  last_tran_id = ap_btree->get_last_tran_id( );

                  iter = ap_btree->lower_bound( tmp_item );
                  if( iter == ap_btree->end( ) )
                     break;
               }
            }
#endif
            if( ( !use_boyer_moore && compare_equal( tmp_item, *iter ) )
             || ( use_boyer_moore && ap_bm->find( iter->const_str( ) ) != string::npos ) )
            {
               ++count;
               if( command != c_cmd_btstore_mcount )
                  cout << iter->val << '\n';
            }
         } while( ++iter != ap_btree->end( ) && compare_less( *iter, item ) );
      }

      if( command == c_cmd_btstore_mcount )
         cout << count << endl;
      else if( count == 0 )
         cout << "*** item not found ***" << endl;

      compare_less.use_truncated_comparision( false );

      if( iter != ap_btree->end( ) )
         item.val = iter->val;
   }
   else if( command == c_cmd_btstore_lbound )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;

      item.val = get_parm_val( parameters, c_cmd_parm_btstore_lbound_item );
      iter = ap_btree->lower_bound( item );
      if( iter == ap_btree->end( ) )
         cout << "*** past the end ***" << endl;
      else
      {
         item.val = iter->val;
         cout << iter->val << endl;
      }
   }
   else if( command == c_cmd_btstore_ubound )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;

      item.val = get_parm_val( parameters, c_cmd_parm_btstore_ubound_item );
      iter = ap_btree->upper_bound( item );
      if( iter == ap_btree->end( ) )
         cout << "*** past the end ***" << endl;
      else
      {
         item.val = iter->val;
         cout << iter->val << endl;
      }
   }
   else if( command == c_cmd_btstore_remove )
   {
      ods::bulk_write bulk( *ap_ods );
      if( has_parm_val( parameters, c_cmd_parm_btstore_remove_item ) )
      {
         *ap_ods >> *ap_btree;
         item.val = get_parm_val( parameters, c_cmd_parm_btstore_remove_item );
         if( !ap_btree->erase( item ) )
            cout << "*** no equal items found ***" << endl;
         else
            iter = ap_btree->upper_bound( item );
      }
      else
      {
         *ap_ods >> *ap_btree;
         reposition_iterator( *ap_btree, item, iter );

         if( iter == ap_btree->end( ) )
            cout << "*** past the end ***";
         else
            ap_btree->erase( iter );
      }
   }
   else if( command == c_cmd_btstore_erange )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      item.val = get_parm_val( parameters, c_cmd_parm_btstore_erange_item );

      pair< btree_type::iterator, btree_type::iterator > range;

      range = ap_btree->equal_range( item );
      if( range.first == range.second )
         cout << "*** no equal items found ***" << endl;
      else
         cout << *range.first << " --> " << *--range.second << '\n';

      iter = range.second;
   }
   else if( command == c_cmd_btstore_count )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      item.val = get_parm_val( parameters, c_cmd_parm_btstore_count_item );

      cout << ap_btree->count( item ) << endl;
   }
   else if( command == c_cmd_btstore_import )
   {
      string filename( get_parm_val( parameters, c_cmd_parm_btstore_import_filename ) );

      ifstream inpf( filename.c_str( ) );
      if( !inpf )
         cout << "error: unable to open file '" << filename << "' for input" << endl;
      else
      {
         ods::bulk_write bulk( *ap_ods );

         *ap_ods >> *ap_btree;
         auto_ptr< btree_trans_type > ap_tx( new btree_trans_type( *ap_btree ) );

#ifdef USE_BULK_PAUSE
         int count = 0;
         int last_tran_id = ap_btree->get_last_tran_id( );
#endif

         while( getline( inpf, item.val ) )
         {
            remove_trailing_cr_from_text_file_line( item.val );

            if( item.val.length( ) )
            {
               iter = ap_btree->insert( item );

#ifdef USE_BULK_PAUSE
               if( ++count % 500 == 0 )
               {
                  ap_tx->commit( );
                  ap_tx.reset( );
                  ap_tx.reset( new btree_trans_type( *ap_btree ) );

                  bulk.pause( );
                  if( last_tran_id != ap_btree->get_last_tran_id( ) )
                  {
                     *ap_ods >> *ap_btree;
                     last_tran_id = ap_btree->get_last_tran_id( );
                  }
               }
#endif
            }
         }

         if( !inpf.eof( ) )
            throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );

         ap_tx->commit( );
      }
   }
   else if( command == c_cmd_btstore_export )
   {
      string filename( get_parm_val( parameters, c_cmd_parm_btstore_export_filename ) );

      ofstream outf( filename.c_str( ) );
      if( !outf )
         cout << "error: unable to open file '" << filename << "' for output" << endl;
      else
      {
         ods::bulk_read bulk( *ap_ods );

         *ap_ods >> *ap_btree;

         btree_type::iterator iter, end = ap_btree->end( );
         for( iter = ap_btree->begin( ); iter != end; ++iter )
            outf << iter->val << '\n';

         outf.close( );
      }
   }
   else if( command == c_cmd_btstore_append )
   {
      string filename( get_parm_val( parameters, c_cmd_parm_btstore_append_filename ) );

      ifstream inpf( filename.c_str( ) );
      if( !inpf )
         cout << "error: unable to open file '" << filename << "' for input" << endl;
      else
      {
         ods::bulk_write bulk( *ap_ods );

         *ap_ods >> *ap_btree;
         auto_ptr< btree_trans_type > ap_tx( new btree_trans_type( *ap_btree ) );

#ifdef USE_BULK_PAUSE
         int count = 0;
         int last_tran_id = ap_btree->.get_last_tran_id( );
#endif

         while( getline( inpf, item.val ) )
         {
            if( item.val.length( ) )
            {
               remove_trailing_cr_from_text_file_line( item.val );

               ap_btree->append( item );

#ifdef USE_BULK_PAUSE
               if( ++count % 1000 == 0 )
               {
                  ap_tx->commit( );
                  ap_tx.reset( );
                  ap_tx.reset( new btree_trans_type( *ap_btree ) );

                  bulk.pause( );
                  if( last_tran_id != ap_btree->get_last_tran_id( ) )
                  {
                     *ap_ods >> *ap_btree;
                     last_tran_id = ap_btree->get_last_tran_id( );
                  }
               }
#endif
            }
         }

         if( !inpf.eof( ) )
            throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );

         ap_tx->commit( );
      }
   }
   else if( command == c_cmd_btstore_build )
   {
      ods::bulk_write bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      ap_btree->build_index_nodes( );
   }
   else if( command == c_cmd_btstore_clear )
   {
      ods::bulk_write bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      ap_btree->clear( );

      *ap_ods << *ap_btree;

      item.val = "";
      iter = ap_btree->end( );
   }
   else if( command == c_cmd_btstore_depth )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      cout << "index depth = " << ( unsigned )ap_btree->depth( ) << endl;
   }
   else if( command == c_cmd_btstore_size )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      cout << "number of items in container = " << ap_btree->size( ) << endl;
   }
   else if( command == c_cmd_btstore_goto )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;

      reposition_iterator( *ap_btree, item, iter );

      if( has_parm_val( parameters, c_cmd_parm_btstore_goto_top ) )
      {
         iter = ap_btree->begin( );
         if( iter != ap_btree->end( ) )
            item.val = iter->val;
      }
      else if( has_parm_val( parameters, c_cmd_parm_btstore_goto_end ) )
      {
         iter = ap_btree->end( );
      }
      else if( has_parm_val( parameters, c_cmd_parm_btstore_goto_next ) )
      {
         if( iter == ap_btree->end( ) )
            cout << "*** past the end ***" << endl;
         else
         {
            if( ++iter != ap_btree->end( ) )
               item.val = iter->val;
         }
      }
      else
      {
         if( iter == ap_btree->begin( ) )
            cout << "*** at the top ***" << endl;
         else
         {
            --iter;
            item.val = iter->val;
         }
      }
   }
   else if( command == c_cmd_btstore_view )
   {
      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      btree_type::iterator tmp_iter;

      reposition_iterator( *ap_btree, item, iter );

      if( iter == ap_btree->end( ) )
         cout << "*** past the end ***" << endl;
      else
         cout << iter->val << endl;

      if( iter != ap_btree->end( ) )
         item.val = iter->val;
   }
   else if( command == c_cmd_btstore_list )
   {
      string filename( get_parm_val( parameters, c_cmd_parm_btstore_list_filename ) );

      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      reposition_iterator( *ap_btree, item, iter );

      if( iter == ap_btree->end( ) )
         cout << "*** past the end ***" << endl;
      else
      {
         btree_type::iterator end = ap_btree->end( );

         if( filename.length( ) )
         {
            ofstream outf( filename.c_str( ) );
            if( !outf )
               cout << "error: unable to open file '" << filename << "' for output" << endl;
            else
            {
               int num = 0;
               for( ; iter != end; ++iter )
               {
                  outf << iter->val << '\n';
                  if( ++num == list_iter_limit )
                  {
                     ++iter;
                     break;
                  }
               }
            }
         }
         else
         {
            int num = 0;
            for( ; iter != end; ++iter )
            {
               cout << iter->val << '\n';
               if( ++num == list_iter_limit )
               {
                  ++iter;
                  break;
               }
            }
         }

         if( iter != ap_btree->end( ) )
            item.val = iter->val;
      }
   }
   else if( command == c_cmd_btstore_rlist )
   {
      string filename( get_parm_val( parameters, c_cmd_parm_btstore_rlist_filename ) );

      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      reposition_iterator( *ap_btree, item, iter );

      if( iter == ap_btree->begin( ) )
         cout << "*** at the top ***" << endl;
      else
      {
         btree_type::const_reverse_iterator riter( iter ), rend = ap_btree->rend( );

         if( filename.length( ) )
         {
            ofstream outf( filename.c_str( ) );
            if( !outf )
               cout << "error: unable to open file '" << filename << "' for output" << endl;
            else
            {
               int num = 0;
               for( ; riter != rend; ++riter )
               {
                  outf << riter->val << '\n';
                  if( ++num == list_iter_limit )
                  {
                     ++riter;
                     break;
                  }
               }
               iter = riter.base( );
            }
         }
         else
         {
            int num = 0;
            for( ; riter != rend; ++riter )
            {
               cout << riter->val << '\n';
               if( ++num == list_iter_limit )
               {
                  ++riter;
                  break;
               }
            }
            iter = riter.base( );
         }

         if( iter != ap_btree->end( ) )
            item.val = iter->val;
      }
   }
   else if( command == c_cmd_btstore_limit )
   {
      if( has_parm_val( parameters, c_cmd_parm_btstore_limit_num ) )
         list_iter_limit = atoi( get_parm_val( parameters, c_cmd_parm_btstore_limit_num ).c_str( ) );
      else
         cout << "limit for listing iteration = " << list_iter_limit << endl;
   }
   else if( command == c_cmd_btstore_dump )
   {
      string filename( get_parm_val( parameters, c_cmd_parm_btstore_dump_filename ) );

      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;
      if( filename.length( ) )
      {
         ofstream outf( filename.c_str( ) );
         if( !outf )
            cout << "error: unable to open file '" << filename << "' for output" << endl;
         else
            ap_btree->dump_all_info( outf );
      }
      else
         ap_btree->dump_all_info( cout );
   }
   else if( command == c_cmd_btstore_compress )
   {
      if( g_shared_access )
         cout << "error: must be locked for exclusive use to perform this operation" << endl;
      else
      {
         cout << "moving free data to end..." << endl;
         ap_ods->move_free_data_to_end( );
         cout << "completed" << endl;

         iter = ap_btree->begin( );
      }
   }
   else if( command == c_cmd_btstore_exit )
      handler.set_finished( );
}

command_functor* btstore_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new btstore_command_functor( dynamic_cast< btstore_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   btstore_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         cmd_handler.add_command( c_cmd_exclusive, 1,
          "", "use ods exclusive file access", new btstore_startup_functor( cmd_handler ) );

         processor.process_commands( );

         cmd_handler.remove_command( c_cmd_exclusive );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.init( "btstore" );

      cmd_handler.add_commands( 0,
       btstore_command_functor_factory, ARRAY_PTR_AND_SIZE( btstore_command_definitions ) );

      console_command_processor processor( cmd_handler );
      processor.process_commands( );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ods_error& err )
   {
      cerr << "error: " << err.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "unexpected exception occurred..." << endl;
      return 2;
   }
}


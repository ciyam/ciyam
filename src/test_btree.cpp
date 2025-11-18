// Copyright (c) 2000-2015 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2015-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
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
#  include <algorithm>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <limits.h>
#     include <unistd.h>
#  endif
#endif

#include <iterator>

#define BTREE_IMPL

#include "btree.h"
#include "utilities.h"
#include "console_commands.h"

using namespace std;
using namespace btree;

#include "test_btree.cmh"

const int c_test_max_nodes = 1000;
const int c_test_items_per_node = 5;

const char* const c_app_title = "test_btree";
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

template< typename I > struct item
{
   I val;

   virtual size_t size( ) const = 0;
   virtual void resize( size_t size ) = 0;

   virtual string& str( ) = 0;
   virtual const string& const_str( ) const = 0;

   bool operator <( const item& src ) const { return val < src.val; }
   bool operator ==( const item& src ) const { return val == src.val; }
};

class test_item : public item< string >
{
   public:
   string& str( ) { return val; }
   const string& const_str( ) const { return val; }

   size_t size( ) const { return val.size( ); }
   void resize( size_t size ) { val.resize( size ); }

   friend ostream& operator <<( ostream& outf, const test_item& t );
   friend istream& operator >>( istream& outf, test_item& t );

   friend ofstream& operator <<( ofstream& outf, const test_item& t );
   friend ifstream& operator >>( ifstream& outf, test_item& t );
};

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

ofstream& operator <<( ofstream& outf, const test_item& t )
{
   size_t size = t.val.size( );
   outf.write( reinterpret_cast< char* >( &size ), sizeof( size_t ) );

   if( !outf.good( ) )
      throw runtime_error( "file write failure" );

   test_item& tmp( const_cast< test_item& >( t ) );
   outf.write( reinterpret_cast< char* >( const_cast< char* >( tmp.str( ).c_str( ) ) ), size );

   if( !outf.good( ) )
      throw runtime_error( "file write failure" );

   return outf;
}

ifstream& operator >>( ifstream& inpf, test_item& t )
{
   size_t size;
   inpf.read( reinterpret_cast< char* >( &size ), sizeof( size_t ) );

   if( !inpf.good( ) )
      throw runtime_error( "file read failure" );

   t.val.resize( size );
   inpf.read( reinterpret_cast< char* >( const_cast< char* >( t.str( ).c_str( ) ) ), size );

   if( !inpf.good( ) )
      throw runtime_error( "file read failure" );

   return inpf;
}

template< typename T > class test_heap_node_manager : public heap_node_manager< T, bt_node< T > >
{
   typedef heap_node_manager< T, bt_node< T > > base_class;

   public:
   enum test_error
   {
      e_error_invalid_input_file_stream,
      e_error_invalid_output_file_stream,
      e_error_read_from_input_stream_failed,
      e_error_write_to_output_stream_failed,
      e_error_unexpected_node_count_incorrect
   };

   test_heap_node_manager( ) : heap_node_manager< T, bt_node< T > >( c_test_max_nodes ) { }

   void load_node_data( ifstream& inpf );

   void save_node_data( ofstream& outf );
};

template< typename T > void test_heap_node_manager< T >::load_node_data( ifstream& inpf )
{
   size_t i, s;
   inpf.read( reinterpret_cast< char* >( &s ), sizeof( size_t ) );
   if( !inpf.good( ) )
      throw e_error_read_from_input_stream_failed;

   inpf.read( reinterpret_cast< char* >( &s ), sizeof( size_t ) );
   if( !inpf.good( ) )
      throw e_error_read_from_input_stream_failed;

   base_class::clear( );
   base_class::setmax( s );
   base_class::reserve( c_test_max_nodes );

   T item;
   uint64_t link;
   typename test_heap_node_manager< T >::node_type node;

   for( i = 0; i < s; i++ )
   {
      typename bt_node< T >::node_data& data( node.ref_data( ) );
      inpf.read( reinterpret_cast< char* >( &data ), sizeof( typename base_class::node_type::node_data ) );
      if( !inpf.good( ) )
         throw e_error_read_from_input_stream_failed;

      short size;
      inpf.read( reinterpret_cast< char* >( &size ), sizeof( size ) );
      if( !inpf.good( ) )
         throw e_error_read_from_input_stream_failed;

      node.clear_items( );
      for( short j = 0; j < size; j++ )
      {
         inpf >> item >> link;
         node.append_item( item, link );
      }

      node.set_link( i );
      this->push_back( node );
   }
}

template< typename T > void test_heap_node_manager< T >::save_node_data( ofstream& outf )
{
   size_t s = base_class::size( );

   outf.write( reinterpret_cast< char* >( &s ), sizeof( size_t ) );
   if( !outf.good( ) )
      throw e_error_write_to_output_stream_failed;

   s = base_class::size( );
   outf.write( reinterpret_cast< char* >( &s ), sizeof( size_t ) );
   if( !outf.good( ) )
      throw e_error_write_to_output_stream_failed;

   for( size_t i = 0; i < s; i++ )
   {
      typename bt_node< T >::node_data& data( base_class::get_node_ref( i ).ref_data( ) );
      outf.write( reinterpret_cast< char* >( &data ), sizeof( typename base_class::node_type::node_data ) );
      if( !outf.good( ) )
         throw e_error_write_to_output_stream_failed;

      short size = base_class::get_node_ref( i ).size( );
      outf.write( reinterpret_cast< char* >( &size ), sizeof( size ) );
      if( !outf.good( ) )
         throw e_error_write_to_output_stream_failed;

      for( short j = 0; j < size; j++ )
         outf << base_class::get_node_ref( i ).get_item_data( j ) << base_class::get_node_ref( i ).get_item_link( j );
   }
}

template< typename T, typename L = std::less< T > > class test_btree
 : public bt_base< T, L, bt_node< T >, test_heap_node_manager< T > >
{
   typedef bt_base< T, L, bt_node< T >, test_heap_node_manager< T > > base_class;

   public:
   enum test_error
   {
      e_error_invalid_input_file_stream,
      e_error_invalid_output_file_stream,
      e_error_non_matching_items_per_node,
      e_error_read_from_input_stream_failed,
      e_error_write_to_output_stream_failed
   };

   test_btree( const L& compare_less = std::less< T >( ) )
    : bt_base< T, L, bt_node< T >, test_heap_node_manager< T > >( compare_less )
   {
   }

   void create_as_xml_info( ostream& outf );

   void create_as_directory_info( ostream& outf );

   void dump_node_info( ostream& outf, uint64_t node, bool output_as_xml, int indent );

   void load_btree_data( ifstream& inpf );

   void save_btree_data( ofstream& outf );
};

template< typename T, typename L > void test_btree< T, L >::create_as_xml_info( ostream& outf )
{
   if( base_class::state.root_node != c_npos )
   {
      outf << "<btree_info>\n";
      dump_node_info( outf, base_class::state.root_node, true, 1 );
      outf << "</btree_info>\n";
   }
}

template< typename T, typename L > void test_btree< T, L >::create_as_directory_info( ostream& outf )
{
   if( base_class::state.root_node != c_npos )
   {
      outf << "mkdir \"btree info\"\n";
      outf << "cd \"btree info\"\n";
      dump_node_info( outf, base_class::state.root_node, false, 0 );
   }
}

template< typename T, typename L >
 void test_btree< T, L >::dump_node_info( ostream& outf, uint64_t node, bool output_as_xml, int indent )
{
   auto_ptr< bt_node_ref< T > > ap_node_ref( base_class::allocate_node_ref( node ) );

   for( short i = 0; i < ap_node_ref->get_node( ).size( ); i++ )
   {
      if( !output_as_xml )
      {
         outf << "mkdir \"" << ap_node_ref->get_node( ).get_item_data( i );
         if( !( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf ) )
            outf << " (less than)";
         outf << "\"\n";
      }

      if( !( ap_node_ref->get_node( ).ref_data( ).flags & c_node_flag_is_leaf ) )
      {
         if( output_as_xml )
         {
            outf << string( indent, ' ' );
            outf << "<less_than name=\"" << ap_node_ref->get_node( ).get_item_data( i ) << "\">\n";
         }
         else
             outf << "cd \""
              << ap_node_ref->get_node( ).get_item_data( i ) << " (less than)\"\n";

          dump_node_info( outf, ap_node_ref->get_node( ).get_item_link( i ), output_as_xml, indent + 1 );

         if( output_as_xml )
         {
            outf << string( indent, ' ' );
            outf << "</less_than>\n";
         }
      }
      else
      {
         if( output_as_xml )
         {
            outf << string( indent, ' ' );
            outf << "<equal_to name = \"" << ap_node_ref->get_node( ).get_item_data( i ) << "\"/>\n";
         }
      }

      if( i == ap_node_ref->get_node( ).size( ) - 1
       && ap_node_ref->get_node( ).ref_data( ).dge_link != c_npos )
      {
         if( output_as_xml )
         {
            outf << string( indent, ' ' );
            outf << "<same_or_greater_than name=\"" << ap_node_ref->get_node( ).get_item_data( i ) << "\">\n";
         }
         else
         {
            outf << "mkdir \"" << ap_node_ref->get_node( ).get_item_data( i );
            outf << " (same or greater)\"\n";

            outf << "cd \"" << ap_node_ref->get_node( ).get_item_data( i );
            outf << " (same or greater)\"\n";
         }

         dump_node_info( outf, ap_node_ref->get_node( ).ref_data( ).dge_link, output_as_xml, indent + 1 );

         if( output_as_xml )
         {
            outf << string( indent, ' ' );
            outf << "</same_or_greater_than>\n";
         }
      }
   }

   if( !output_as_xml )
      outf << "cd .." << endl;
}

template< typename T, typename L > void test_btree< T, L >::load_btree_data( ifstream& inpf )
{
   short tmp;
   inpf.read( reinterpret_cast< char* >( &tmp ), sizeof( tmp ) );
   if( !inpf.good( ) )
      throw e_error_read_from_input_stream_failed;

   if( tmp != base_class::get_node_manager( ).get_items_per_node( ) )
      throw e_error_non_matching_items_per_node;

   inpf.read( reinterpret_cast< char* >( &base_class::get_state( ) ), sizeof( base_class::get_state( ) ) );
   if( !inpf.good( ) )
      throw e_error_read_from_input_stream_failed;

   base_class::get_node_manager( ).load_node_data( inpf );
}

template< typename T, typename L > void test_btree< T, L >::save_btree_data( ofstream& outf )
{
   short tmp = base_class::get_node_manager( ).get_items_per_node( );
   outf.write( reinterpret_cast< char* >( &tmp ), sizeof( tmp ) );
   if( !outf.good( ) )
      throw e_error_write_to_output_stream_failed;

   outf.write( reinterpret_cast< char* >( &base_class::get_state( ) ), sizeof( base_class::get_state( ) ) );
   if( !outf.good( ) )
      throw e_error_write_to_output_stream_failed;

   base_class::get_node_manager( ).save_node_data( outf );
}

inline size_t first_wildcard_character_pos( const string& str )
{
   return str.find_first_of( "?*" );
}

template< class S > struct wildcard_compare_less_functor : public std::binary_function< S, S, bool >
{
   wildcard_compare_less_functor( )
    :
    truncated_comparison( false )
   {
   }

   void use_truncated_comparison( bool val ) const
   {
      truncated_comparison = val;
   }

   bool operator( )( const S& x, const S& y ) const
   {
      if( !truncated_comparison )
         return less< S >( )( x, y );

      size_t xlen = x.const_str( ).length( );
      size_t ylen = y.const_str( ).length( );

      return x.const_str( ).compare( 0, min( xlen, ylen ), y.const_str( ), 0, min( xlen, ylen ) ) <= 0;
   }

   private:
   mutable bool truncated_comparison;
};

template< class S > struct wildcard_compare_equal_functor : public std::binary_function< S, S, bool >
{
   bool operator( )( const S& x, const S& y ) const
   {
      return wildcard_match( x.const_str( ).c_str( ), y.const_str( ).c_str( ) );
   }
};

typedef test_btree< test_item > btree_type;

class test_btree_command_functor;

class test_btree_command_handler : public console_command_handler
{
   friend class test_btree_command_functor;

   public:
   test_btree_command_handler( )
    :
    list_iter_limit( 0 ),
    iter( bt.end( ) )
   {
      bt.set_items_per_node( c_test_items_per_node );
   }

   private:
   int list_iter_limit;

   btree_type::item_type item;

   wildcard_compare_less_functor< btree_type::item_type > compare_less;
   wildcard_compare_equal_functor< btree_type::item_type > compare_equal;

   btree_type bt;

   btree_type::iterator iter;
};

class test_btree_command_functor : public command_functor
{
   public:
   test_btree_command_functor( test_btree_command_handler& btree_handler )
    : command_functor( btree_handler ),
    bt( btree_handler.bt ),
    iter( btree_handler.iter ),
    item( btree_handler.item ),
    btree_handler( btree_handler ),
    compare_less( btree_handler.compare_less ),
    compare_equal( btree_handler.compare_equal ),
    list_iter_limit( btree_handler.list_iter_limit )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   int& list_iter_limit;

   btree_type& bt;

   btree_type::iterator& iter;
   btree_type::item_type& item;

   test_btree_command_handler& btree_handler;

   wildcard_compare_less_functor< btree_type::item_type >& compare_less;
   wildcard_compare_equal_functor< btree_type::item_type >& compare_equal;
};

void test_btree_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_test_btree_add )
   {
      item.val = get_parm_val( parameters, c_cmd_test_btree_add_item );
      iter = bt.insert( item );
   }
   else if( command == c_cmd_test_btree_find )
   {
      item.val = get_parm_val( parameters, c_cmd_test_btree_find_item );
      iter = bt.find( item );
      if( iter == bt.end( ) )
         handler.issue_command_response( "*** item not found ***", true );
      else
         handler.issue_command_response( iter->val );
   }
   else if( command == c_cmd_test_btree_match || command == c_cmd_test_btree_mcount )
   {
      string expr;
      if( command == c_cmd_test_btree_match )
         expr = get_parm_val( parameters, c_cmd_test_btree_match_expr );
      else
         expr = get_parm_val( parameters, c_cmd_test_btree_mcount_expr );

      size_t pos = first_wildcard_character_pos( expr );
      item.val = expr.substr( 0, pos );

      iter = bt.lower_bound( item );

      if( pos != string::npos )
         compare_less.use_truncated_comparison( true );
      else
         compare_less.use_truncated_comparison( false );

      size_t count = 0;
      if( iter != bt.end( ) )
      {
         btree_type::item_type tmp_item;
         tmp_item.val = expr;

         do
         {
            if( compare_equal( tmp_item, *iter ) )
            {
               ++count;
               if( command != c_cmd_test_btree_mcount )
                  handler.issue_command_response( iter->val );
            }
         } while( ++iter != bt.end( ) && compare_less( *iter, item ) );
      }

      if( command == c_cmd_test_btree_mcount )
         handler.issue_command_response( to_string( count ) );
      else if( count == 0 )
         handler.issue_command_response( "*** item not found ***", true );

      compare_less.use_truncated_comparison( false );
   }
   else if( command == c_cmd_test_btree_lbound )
   {
      item.val = get_parm_val( parameters, c_cmd_test_btree_lbound_item );

      iter = bt.lower_bound( item );
      if( iter == bt.end( ) )
         handler.issue_command_response( "*** past the end ***", true );
      else
         handler.issue_command_response( iter->val );
   }
   else if( command == c_cmd_test_btree_ubound )
   {
      item.val = get_parm_val( parameters, c_cmd_test_btree_lbound_item );
      iter = bt.upper_bound( item );
      if( iter == bt.end( ) )
         handler.issue_command_response( "*** past the end ***", true );
      else
         handler.issue_command_response( iter->val );
   }
   else if( command == c_cmd_test_btree_remove )
   {
      if( has_parm_val( parameters, c_cmd_test_btree_remove_item ) )
      {
         item.val = get_parm_val( parameters, c_cmd_test_btree_remove_item );
         if( !bt.erase( item ) )
            handler.issue_command_response( "*** no equal items found ***", true );
         else
            iter = bt.upper_bound( item );
      }
      else
      {
         if( iter == bt.end( ) )
            handler.issue_command_response( "*** past the end ***", true );
         else
            bt.erase( iter );
      }
   }
   else if( command == c_cmd_test_btree_erange )
   {
      item.val = get_parm_val( parameters, c_cmd_test_btree_erange_item );

      pair< btree_type::iterator, btree_type::iterator > range;

      range = bt.equal_range( item );

      if( range.first == range.second )
         handler.issue_command_response( "*** no equal items found ***", true );
      else
         handler.issue_command_response( to_string( *range.first ) + " --> " + to_string( *--range.second ) );
   }
   else if( command == c_cmd_test_btree_count )
   {
      item.val = get_parm_val( parameters, c_cmd_test_btree_count_item );
      handler.issue_command_response( to_string( bt.count( item ) ) );
   }
   else if( command == c_cmd_test_btree_load )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_load_filename ) );

      ifstream inpf( filename.c_str( ), ios::in | ios::binary );

      if( !inpf )
         handler.issue_command_response(
          to_string( c_error_prefix ) + "unable to open file '" + filename + "' for input", true );
      else
      {
         bt.load_btree_data( inpf );
         iter = bt.begin( );
      }
   }
   else if( command == c_cmd_test_btree_save )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_save_filename ) );

      ofstream outf( filename.c_str( ), ios::out | ios::binary );

      if( !outf )
         handler.issue_command_response(
          to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );
      else
         bt.save_btree_data( outf );
   }
   else if( command == c_cmd_test_btree_import )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_import_filename ) );

      ifstream inpf( filename.c_str( ) );

      if( !inpf )
         handler.issue_command_response(
          to_string( c_error_prefix ) + "unable to open file '" + filename + "' for input", true );
      else
      {
         while( getline( inpf, item.val ) )
         {
            remove_trailing_cr_from_text_file_line( item.val );

            if( item.val.length( ) )
               iter = bt.insert( item );
         }

         if( !inpf.eof( ) )
            throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );
      }
   }
   else if( command == c_cmd_test_btree_export )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_export_filename ) );

      ofstream outf( filename.c_str( ) );

      if( !outf )
         handler.issue_command_response(
          to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );
      else
      {
         btree_type::iterator iter, end = bt.end( );
         for( iter = bt.begin( ); iter != end; ++iter )
            handler.issue_command_response( iter->val );

         outf.close( );
      }
   }
   else if( command == c_cmd_test_btree_append )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_append_filename ) );

      ifstream inpf( filename.c_str( ) );

      if( !inpf )
         handler.issue_command_response(
          to_string( c_error_prefix ) + "unable to open file '" + filename + "' for input", true );
      else
      {
         while( getline( inpf, item.val ) )
         {
            remove_trailing_cr_from_text_file_line( item.val );

            if( item.val.length( ) )
               bt.append( item );
         }

         if( !inpf.eof( ) )
            throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );
      }
   }
   else if( command == c_cmd_test_btree_build )
      bt.build_index_nodes( );
   else if( command == c_cmd_test_btree_clear )
   {
      bt.clear( );
      item.val = "";
      iter = bt.end( );
   }
   else if( command == c_cmd_test_btree_depth )
      handler.issue_command_response( "index depth = " + to_string( ( unsigned )bt.depth( ) ) );
   else if( command == c_cmd_test_btree_size )
      handler.issue_command_response( "number of items in container = " + to_string( bt.size( ) ) );
   else if( command == c_cmd_test_btree_goto )
   {
      if( has_parm_val( parameters, c_cmd_test_btree_goto_top ) )
         iter = bt.begin( );
      else if( has_parm_val( parameters, c_cmd_test_btree_goto_end ) )
         iter = bt.end( );
      else if( has_parm_val( parameters, c_cmd_test_btree_goto_next ) )
      {
         if( iter == bt.end( ) )
            handler.issue_command_response( "*** past the end ***", true );
         else
            ++iter;
      }
      else
      {
         if( iter == bt.begin( ) )
            handler.issue_command_response( "*** at the top ***", true );
         else
            --iter;
      }
   }
   else if( command == c_cmd_test_btree_view )
   {
      if( iter == bt.end( ) )
         handler.issue_command_response( "*** past the end ***", true );
      else
         handler.issue_command_response( iter->val );
   }
   else if( command == c_cmd_test_btree_list )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_list_filename ) );

      if( iter == bt.end( ) )
         handler.issue_command_response( "*** past the end ***", true );
      else
      {
         btree_type::iterator end = bt.end( );
         if( filename.length( ) )
         {
            ofstream outf( filename.c_str( ) );

            if( !outf )
               handler.issue_command_response(
                to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );
            else
            {
               int num = 0;
               for( ; iter != end; ++iter )
               {
                  handler.issue_command_response( iter->val );

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
               handler.issue_command_response( iter->val );

               if( ++num == list_iter_limit )
               {
                  ++iter;
                  break;
               }
            }
         }
      }
   }
   else if( command == c_cmd_test_btree_rlist )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_rlist_filename ) );

      if( iter == bt.begin( ) )
         handler.issue_command_response( "*** at the top ***", true );
      else
      {
         btree_type::const_reverse_iterator riter( iter ), rend = bt.rend( );
         if( filename.length( ) )
         {
            ofstream outf( filename.c_str( ) );
            if( !outf )
               handler.issue_command_response(
                to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );
            else
            {
               int num = 0;
               for( ; riter != rend; ++riter )
               {
                  handler.issue_command_response( riter->val );

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
               handler.issue_command_response( riter->val );

               if( ++num == list_iter_limit )
               {
                  ++riter;
                  break;
               }
            }
            iter = riter.base( );
         }
      }
   }
   else if( command == c_cmd_test_btree_limit )
   {
      if( has_parm_val( parameters, c_cmd_test_btree_limit_num ) )
         list_iter_limit = atoi( get_parm_val( parameters, c_cmd_test_btree_limit_num ).c_str( ) );
      else
         handler.issue_command_response( "limit for listing iteration = " + to_string( list_iter_limit ) );
   }
   else if( command == c_cmd_test_btree_dump )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_dump_filename ) );

      if( filename.length( ) )
      {
         ofstream outf( filename.c_str( ) );

         if( !outf )
            handler.issue_command_response(
             to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );
         else
            bt.dump_all_info( outf );
      }
      else
         bt.dump_all_info( *btree_handler.get_std_out( ) );
   }
   else if( command == c_cmd_test_btree_xml )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_xml_filename ) );

      ofstream outf( filename.c_str( ) );

      if( !outf )
         handler.issue_command_response(
          to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );
      else
         bt.create_as_xml_info( outf );
   }
   else if( command == c_cmd_test_btree_mkdirs )
   {
      string filename( get_parm_val( parameters, c_cmd_test_btree_mkdirs_filename ) );

      ofstream outf( filename.c_str( ) );

      if( !outf )
         handler.issue_command_response(
          to_string( c_error_prefix ) + "unable to open file '" + filename + "' for output", true );
      else
         bt.create_as_directory_info( outf );
   }
   else if( command == c_cmd_test_btree_exit )
      handler.set_finished( );
}

command_functor* test_btree_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new test_btree_command_functor( dynamic_cast< test_btree_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   test_btree_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         processor.process_commands( );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.add_commands( 0,
       test_btree_command_functor_factory, ARRAY_PTR_AND_SIZE( test_btree_command_definitions ) );

      console_command_processor processor( cmd_handler );
      processor.process_commands( );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception occurred" << endl;
      return 2;
   }
}


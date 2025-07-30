// Copyright (c) 2016-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <ctime>
#  include <cstdio>
#  include <cstddef>
#  include <sstream>
#  include <fstream>
#  include <iomanip>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <limits.h>
#     include <unistd.h>
#  endif
#endif

#define BTREE_IMPL
#define CIYAM_BASE_IMPL
#define STORABLE_BTREE_USE_MAGIC

#include "ods_file_system.h"

#include "ods.h"
#include "btree.h"
#include "regex.h"
#include "base64.h"
#include "format.h"
#include "pointers.h"
#include "date_time.h"
#include "utilities.h"
#include "fs_iterator.h"
#include "oid_pointer.h"
#include "storable_file.h"
#include "storable_btree.h"

//#define DEBUG

//#define USE_BULK_PAUSE
//#define ALLOW_SAME_FILE_AND_FOLDER_NAMES

using namespace std;
using namespace btree;

namespace
{

const char c_pipe = '|';
const char c_colon = ':';
const char c_folder = '/';
const char c_special = '!';

const char* const c_root_folder = "/";
const char* const c_parent_folder = "..";
const char* const c_current_folder = ".";

const char* const c_dummy_suffix = "\t";

const char* const c_pipe_separator = "|";
const char* const c_colon_separator = ":";
const char* const c_folder_separator = "/";

const char* const c_doubled_folder = "//";
const char* const c_from_current_folder = "./";
const char* const c_below_current_folder = "/./";

const char* const c_unchanged_suffix = " *** unchanged ***";

// NOTE: Although the number of items could be
// set to 255 the average node size can end up
// being rather large (especially if the files
// are named using hashes) so instead uses the
// smaller value of 55 to keep the nodes small
// to avoid tx log bloat when adding files one
// by one (as each such tx will append both an
// old and new version of the node object).
const unsigned char c_ofs_items_per_node = 55;

uint16_t c_ofs_object_flag_type_file = 0x8000;
uint16_t c_ofs_object_flag_type_link = 0x4000;
uint16_t c_ofs_object_flag_type_perm = 0x2000;

uint16_t c_ofs_object_flag_type_vals = 0xe000;

uint16_t c_ofs_object_maxiumum_val_size = 0x1fff;

uint32_t c_ofs_object_includes_unix_time = 0x1000000;

const size_t c_rwx_perms_size = 9;

enum rwx_perm
{
   e_rwx_perm_read = 1,
   e_rwx_perm_write = 2,
   e_rwx_perm_execute = 4
};

string value_to_perms( uint32_t value )
{
   string retval;

   for( size_t i = 0; i < 3; i++ )
   {
      string next;

      unsigned char val = ( value & 0x0f );

      if( val & e_rwx_perm_read )
         next += 'r';
      else
         next += '-';

      if( val & e_rwx_perm_write )
         next += 'w';
      else
         next += '-';

      if( val & e_rwx_perm_execute )
         next += 'x';
      else
         next += '-';

      retval = next + retval;

      value >>= 4;
   }

   return retval;
}

uint32_t perms_to_value( const string& perms )
{
   uint32_t retval = 0;

   if( perms.length( ) && ( perms.length( ) != c_rwx_perms_size ) )
      throw runtime_error( "unexpected perms value '" + perms + "'" );

   for( size_t i = 0; i < perms.length( ); i++ )
   {
      unsigned char next = perms[ i ];

      if( i && ( i % 3 == 0 ) )
         retval <<= 4;

      switch( next )
      {
         case '-':
         break;

         case 'r':
         retval += e_rwx_perm_read;
         break;

         case 'w':
         retval += e_rwx_perm_write;
         break;

         case 'x':
         retval += e_rwx_perm_execute;
         break;
      }
   }

   return retval;
}

struct ofs_object
{
   ofs_object( ) : is_link( false ), perm_val( 0 ), time_val( 0 ) { }

   string& str( ) { return val; }
   const string& const_str( ) const { return val; }

   bool get_is_link( ) const { return is_link; }
   void set_is_link( ) { is_link = true; }

   string get_perms( ) const { return value_to_perms( perm_val ); }
   void set_perms( const string& perms ) { perm_val = perms_to_value( perms ); }

   int64_t get_time( ) const { return time_val; }
   void set_time( int64_t unix_time ) { time_val = unix_time; }

   oid_pointer< storable_file >& get_file( ) { return o_file; }
   const oid_pointer< storable_file >& get_file( ) const { return o_file; }

   oid_pointer< storable_file >& get_file( storable_file_extra* p_extra ) { o_file.set_extra( p_extra ); return o_file; }

   bool operator <( const ofs_object& src ) const { return val < src.val; }
   bool operator ==( const ofs_object& src ) const { return val == src.val; }

   friend int64_t size_of( const ofs_object& t );

   friend read_stream& operator >>( read_stream& rs, ofs_object& t );
   friend write_stream& operator <<( write_stream& ws, const ofs_object& t );

   friend ostream& operator <<( ostream& outf, const ofs_object& t );

   string val;

   bool is_link;

   uint32_t perm_val;

   int64_t time_val;

   oid_pointer< storable_file > o_file;
};

int64_t size_of( const ofs_object& o )
{
   if( o.val.length( ) > c_ofs_object_maxiumum_val_size )
      throw runtime_error( "maximum object length exceeded with ofs_object '" + o.val + "'" );

   size_t extra = 0;

   if( o.perm_val )
   {
      extra += sizeof( uint32_t );

      if( o.time_val )
         extra += sizeof( int64_t );
   }

   return sizeof( uint16_t ) + o.val.length( )
    + extra + ( o.o_file.get_id( ).is_new( ) ? 0 : sizeof( oid ) );
}

read_stream& operator >>( read_stream& rs, ofs_object& o )
{
   uint16_t size;

   rs >> size;

   bool has_file = false;
   bool has_perm = false;

   o.is_link = false;

   if( size & c_ofs_object_flag_type_vals )
   {
      if( size & c_ofs_object_flag_type_file )
         has_file = true;

      if( size & c_ofs_object_flag_type_link )
         o.is_link = true;

      if( size & c_ofs_object_flag_type_perm )
         has_perm = true;

      size &= ~c_ofs_object_flag_type_vals;
   }

   o.val.resize( size );

   for( uint16_t i = 0; i < size; i++ )
      rs >> o.val[ i ];

   if( has_perm )
   {
      rs >> o.perm_val;

      if( o.perm_val & c_ofs_object_includes_unix_time )
      {
         o.perm_val &= ~c_ofs_object_includes_unix_time;

         rs >> o.time_val;
      }
   }

   if( has_file )
      rs >> o.o_file;
   else
      o.o_file.get_id( ).set_new( );

   return rs;
}

write_stream& operator <<( write_stream& ws, const ofs_object& o )
{
   uint16_t size = o.val.length( );

   bool has_file = false;
   bool has_perm = false;

   if( o.o_file.get_id( ).is_new( ) )
   {
      if( o.perm_val )
      {
         has_perm = true;
         size |= c_ofs_object_flag_type_perm;
      }
   }
   else
   {
      has_file = true;
      size |= c_ofs_object_flag_type_file;

      if( o.is_link )
         size |= c_ofs_object_flag_type_link;

      if( o.perm_val )
      {
         has_perm = true;
         size |= c_ofs_object_flag_type_perm;
      }
   }

   ws << size;

   size &= ~c_ofs_object_flag_type_vals;

   for( uint16_t i = 0; i < size; i++ )
      ws << o.val[ i ];

   uint32_t perm_val = o.perm_val;

   if( has_perm )
   {
      if( o.time_val )
         perm_val |= c_ofs_object_includes_unix_time;

      ws << perm_val;

      if( o.time_val )
         ws << o.time_val;
   }

   if( has_file )
      ws << o.o_file;

   return ws;
}

ostream& operator <<( ostream& outf, const ofs_object& o )
{
   outf << o.val;
   return outf;
}

inline size_t first_wildcard_character_pos( const string& str )
{
   return str.find_first_of( "?*" );
}

template< class S > struct wildcard_compare_less_functor : public binary_function< S, S, bool >
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
      else
      {
         size_t xlen = x.const_str( ).length( );
         size_t ylen = y.const_str( ).length( );

         return x.const_str( ).compare( 0, min( xlen, ylen ), y.const_str( ), 0, min( xlen, ylen ) ) <= 0;
      }
   }

   private:
   mutable bool truncated_comparison;
};

template< class S > struct wildcard_compare_equal_functor : public binary_function< S, S, bool >
{
   bool operator( )( const S& x, const S& y ) const
   {
      return wildcard_match( x.const_str( ).c_str( ), y.const_str( ).c_str( ) );
   }
};

typedef storable< storable_btree_base< ofs_object >, storable_btree_base< ofs_object >::c_round_to_value > btree_type;
typedef storable< storable_node_base< ofs_object >, storable_node_base< ofs_object >::c_round_to_value > btree_node_type;

typedef storable_node_manager< ofs_object > btree_node_manager_type;

typedef bt_transaction< ofs_object, less< ofs_object >, btree_node_type, btree_node_manager_type > btree_trans_type;

const char* const c_rename_attribute_type_input = "input";
const char* const c_rename_attribute_type_folder = "folder";
const char* const c_rename_attribute_type_output = "output";
const char* const c_rename_attribute_type_prefix = "prefix";
const char* const c_rename_attribute_type_remove = "remove";
const char* const c_rename_attribute_type_suffix = "suffix";

const char* const c_rename_attribute_type_input_any = "@any";
const char* const c_rename_attribute_type_input_hex = "@hex";
const char* const c_rename_attribute_type_input_b64 = "@b64";

const char* const c_rename_attribute_type_output_any = "@any";
const char* const c_rename_attribute_type_output_hex = "@hex";
const char* const c_rename_attribute_type_output_b64 = "@b64";

string process_rename_expressions(
 vector< string >& rename_expressions, const string& folder, const string& input )
{
   string output( input );

   for( size_t i = 0; i < rename_expressions.size( ); i++ )
   {
      string next_expr( rename_expressions[ i ] );

      // NOTE: All rename expressions are in the form:
      // <attribute>=<value>[|<attribute>=<value>[...]]
      vector< string > attributes;

      split( next_expr, attributes, '|' );

      bool rename = true;

      string prefix, suffix, renamed_output( output );

      for( size_t j = 0; j < attributes.size( ); j++ )
      {
         string next_attr( attributes[ j ] );

         string::size_type pos = next_attr.find( '=' );

         if( pos == string::npos )
            throw runtime_error( "unepxected rename expr attribute '" + next_attr + "'" );

         string type( next_attr.substr( 0, pos ) );
         string value( next_attr.substr( pos + 1 ) );

         if( type == c_rename_attribute_type_input )
         {
            if( value == c_rename_attribute_type_input_any )
               ;
            else if( value == c_rename_attribute_type_input_hex )
            {
               if( !are_hex_nibbles( renamed_output ) )
                  rename = false;
               else
                  renamed_output = hex_decode( renamed_output );
            }
            else if( value == c_rename_attribute_type_input_b64 )
            {
               bool rc = false;

               base64::validate( renamed_output, &rc, true );

               if( !rc )
                  rename = false;
               else
                  renamed_output = base64::decode( renamed_output, true );
            }
            else
               throw runtime_error( "invalid rename expr attribute input type '" + value + "'" );
         }
         else if( type == c_rename_attribute_type_folder )
         {
            if( folder != value )
               rename = false;
         }
         else if( type == c_rename_attribute_type_output )
         {
            if( value == c_rename_attribute_type_output_any )
               renamed_output = output;
            else if( value == c_rename_attribute_type_output_hex )
            {
               string data( renamed_output );

               renamed_output = hex_encode( data );
            }
            else if( value == c_rename_attribute_type_output_b64 )
            {
               string data( renamed_output );

               renamed_output = base64::encode( data, true );
            }
            else
               throw runtime_error( "invalid rename expr attribute output type '" + value + "'" );
         }
         else if( type == c_rename_attribute_type_prefix )
            prefix = value;
         else if( type == c_rename_attribute_type_remove )
            renamed_output = replace( renamed_output, value, "" );
         else if( type == c_rename_attribute_type_suffix )
            suffix = value;

         if( !rename )
            break;
      }

      if( rename && !renamed_output.empty( ) )
         output = prefix + renamed_output + suffix;
   }

   return output;
}

bool folder_matches_suffix( const string& extra, const string& branch_suffix )
{
   string compare( extra );

   string::size_type pos = compare.rfind( c_folder, compare.length( ) - 2 );

   if( pos != string::npos )
      ++pos;
   else
      pos = 0;

   if( pos != 0 )
      compare.erase( 0, pos );

   if( !compare.empty( ) )
      compare.erase( compare.length( ) - 1 );

   return  wildcard_match( branch_suffix, compare );
}


}

struct ods_file_system::impl
{
   impl( ods& o )
    :
    bt( o ),
    force_write( false ),
    skip_hidden( true ),
    show_folders( true ),
    allow_specials( false ),
    skip_hidden_count( 0 ),
    for_regression_tests( false ),
    next_transaction_id( 0 )
   {
   }

   btree_type bt;

   bool force_write;
   bool skip_hidden;

   bool show_folders;

   bool allow_specials;

   string branch_prefix;
   string branch_suffix;

   size_t skip_hidden_count;

   bool for_regression_tests;

   int64_t next_transaction_id;
};

ods_file_system::ods_file_system( ods& o, int64_t i, bool for_regression_tests )
 :
 o( o ),
 current_folder( c_root_folder )
{
   p_impl = new impl( o );

   p_impl->for_regression_tests = for_regression_tests;

   btree_type& bt( p_impl->bt );

   if( o.get_total_entries( ) > i )
   {
      bt.set_id( i );
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }
   else
   {
      btree_trans_type bt_tx( bt );

      bt.set_items_per_node( c_ofs_items_per_node );

      bt_tx.commit( );

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }
}

ods_file_system::~ods_file_system( )
{
   delete p_impl;
}

string ods_file_system::get_folder( bool /* force reload */ )
{
   force_reload( );

   return get_folder( );
}

void ods_file_system::set_folder( const string& new_folder, bool* p_rc )
{
   force_reload( );

   string s( determine_folder( new_folder, false, true ) );

   if( !s.empty( ) )
   {
      current_folder = s;

      if( p_rc )
         *p_rc = true;
   }
   else
   {
      if( p_rc )
         *p_rc = false;
      else
         throw runtime_error( "folder '" + new_folder + "' does not exist" );
   }
}

void ods_file_system::set_root_folder( const string& new_folder, bool* p_rc )
{
   char ch = '\0';

   if( !new_folder.empty( ) )
      ch = new_folder[ 0 ];

   if( ch == c_folder )
      set_folder( new_folder, p_rc );
   else
      set_folder( string( c_root_folder ) + new_folder, p_rc );
}

string ods_file_system::determine_folder(
 const string& folder, bool explicit_child_only, bool ignore_not_found )
{
   string new_folder( folder );
   string old_current_folder( current_folder );

   if( new_folder == string( c_current_folder ) )
      new_folder = current_folder;
   else if( new_folder.find( c_from_current_folder ) == 0 )
      new_folder.erase( 0, strlen( c_from_current_folder ) );

   // NOTE: Replaces "xxx/./yyy" with "xxx/yyy".
   replace( new_folder, c_below_current_folder, c_folder_separator );

   // NOTE: Ignore trailing separator (unless is the only character).
   string::size_type pos = new_folder.rfind( c_folder );

   if( pos && ( pos != string::npos ) && ( pos == new_folder.length( ) - 1 ) )
      new_folder.erase( new_folder.size( ) - 1 );

   btree_type& bt( p_impl->bt );

   if( explicit_child_only )
   {
      string::size_type pos = new_folder.find( c_parent_folder );

      if( pos != string::npos || ( !new_folder.empty( ) && new_folder[ 0 ] == c_folder ) )
         throw runtime_error( "invalid non-explicit or non-child folder" );
   }

   if( !new_folder.empty( ) )
   {
      string::size_type pos = new_folder.find( c_parent_folder );

      while( pos == 0 )
      {
         string::size_type dpos = old_current_folder.rfind( c_folder );

         if( dpos != string::npos && old_current_folder != string( c_root_folder ) )
         {
            old_current_folder.erase( dpos );

            if( old_current_folder.empty( ) )
               old_current_folder = string( c_root_folder );
         }

         new_folder.erase( 0, 2 );

         if( !new_folder.empty( ) && new_folder.substr( 0, 1 ) == string( c_root_folder ) )
            new_folder.erase( 0, 1 );

         pos = new_folder.find( c_parent_folder );
      }

      pos = new_folder.find( c_folder_separator );

      if( pos != 0 )
      {
         if( old_current_folder == string( c_root_folder ) )
            new_folder = old_current_folder + new_folder;
         else if( new_folder.empty( ) )
            new_folder = old_current_folder;
         else
            new_folder = old_current_folder + string( c_folder_separator ) + new_folder;
      }

      if( new_folder != string( c_root_folder ) )
      {
         auto_ptr< ods::bulk_read > ap_bulk;

         if( !o.is_thread_bulk_locked( ) )
            ap_bulk.reset( new ods::bulk_read( o ) );

         if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
         {
            o >> bt;

            p_impl->next_transaction_id = o.get_next_transaction_id( );
         }

         btree_type::item_type tmp_item;

         tmp_item.val = new_folder;

         if( bt.find( tmp_item ) == bt.end( ) )
         {
            if( !ignore_not_found )
               throw runtime_error( "folder '" + new_folder + "' not found" );

            new_folder.erase( );
         }
      }
   }

   return new_folder;
}

string ods_file_system::determine_strip_and_change_folder( string& name )
{
   string original_folder;

   string::size_type pos = name.rfind( c_folder );

   if( pos != string::npos )
   {
      string source_folder( determine_folder( name.substr( 0, pos ), false, true ) );

      if( source_folder.empty( ) )
         throw runtime_error( "file '" + name + "' not found" );
      else
      {
         original_folder = get_folder( );

         name.erase( 0, pos + 1 );
         set_folder( source_folder );
      }
   }

   return original_folder;
}

void ods_file_system::list_files( const string& expr, vector< string >& list,
 bool include_links, const string& start_from, bool inclusive, size_t limit, bool in_reverse_order )
{
   ostringstream osstr;

   list_files( expr, osstr, start_from, e_list_style_brief, inclusive, limit, in_reverse_order );

   string file_list( osstr.str( ) );

   if( !file_list.empty( ) && file_list[ file_list.length( ) - 1 ] == '\n' )
      file_list = file_list.substr( 0, file_list.length( ) - 1 );

   if( !file_list.empty( ) )
      split( file_list, list, '\n' );

   if( !include_links )
   {
      vector< string > non_link_files;

      for( size_t i = 0; i < list.size( ); i++ )
      {
         string::size_type pos = list[ i ].find( '*' );

         if( pos == string::npos )
            non_link_files.push_back( list[ i ] );
      }

      list = non_link_files;
   }
}

void ods_file_system::list_links( const string& name, ostream& os )
{
   if( name.empty( ) )
      throw runtime_error( "unexpected empty file name in 'file_links'" );

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   tmp_item.val = value_folder_and_file_name( name );

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
      throw runtime_error( "file '" + name + "' not found" );
   else
   {
      oid id( tmp_iter->get_file( ).get_id( ) );

      bool is_link = tmp_iter->get_is_link( );

      if( !is_link )
      {
         string link_prefix( to_string( id.get_num( ) ) + c_pipe_separator );

         tmp_item.val = link_prefix;

         tmp_iter = bt.lower_bound( tmp_item );

         while( tmp_iter != bt.end( ) )
         {
            if( tmp_iter->val.find( link_prefix ) != 0 )
               break;

            string link_name( tmp_iter->val.substr( link_prefix.length( ) - 1 ) );

            replace( link_name, c_pipe_separator, c_folder_separator );
            replace( link_name, c_doubled_folder, c_root_folder );

            os << link_name << '\n';

            ++tmp_iter;
         }
      }
   }
}

void ods_file_system::list_folders( const string& expr,
 vector< string >& list, const string& start_from, bool inclusive, size_t limit, bool in_reverse_order )
{
   ostringstream osstr;

   pair< string, string > range;

   if( !start_from.empty( ) )
   {
      range.first = replaced( current_folder,
       c_folder_separator, c_colon_separator ) + c_folder_separator + start_from;
   }

   list_folders( expr, osstr, false, start_from.empty( ) ? 0 : &range, true, limit, in_reverse_order );

   string folder_list( osstr.str( ) );

   if( !folder_list.empty( ) && folder_list[ folder_list.length( ) - 1 ] == '\n' )
      folder_list = folder_list.substr( 0, folder_list.length( ) - 1 );

   if( !folder_list.empty( ) )
      split( folder_list, list, '\n' );
}

void ods_file_system::list_folders( const string& expr, ostream& os,
 bool full_path, pair< string, string >* p_range, bool inclusive, size_t limit, bool in_reverse_order )
{
   string entity_expr( current_folder );

   bool has_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

   expand_entity_expression( expr, entity_expr, has_wildcard ? "\0" : c_folder_separator );

   if( expr.empty( ) && ( current_folder != string( c_root_folder ) ) )
      entity_expr += string( c_folder_separator );

   if( !entity_expr.empty( ) )
   {
      string::size_type pos = entity_expr.rfind( c_folder );

      replace( entity_expr, c_folder_separator, c_colon_separator );

      if( pos == 0 )
         entity_expr += c_folder;
      else if( pos != string::npos )
         entity_expr[ pos ] = c_folder;

      if( !has_wildcard )
         entity_expr += "*";

      vector< pair< string, string > > search_replaces;

      search_replaces.push_back( make_pair( c_colon_separator, c_folder_separator ) );

      perform_match( os, entity_expr, "", 0, &search_replaces, 0, 0,
       full_path ? '\0' : c_folder, e_file_size_output_type_none,
       0, 0, p_range, inclusive, limit, in_reverse_order );
   }
}

void ods_file_system::branch_files( const std::string& expr, vector< string >& files, branch_style style )
{
   ostringstream osstr;

   branch_files( expr, osstr, style );

   string file_list( osstr.str( ) );

   if( !file_list.empty( ) && file_list[ file_list.length( ) - 1 ] == '\n' )
      file_list = file_list.substr( 0, file_list.length( ) - 1 );

   if( !file_list.empty( ) )
      split( file_list, files, '\n' );
}

void ods_file_system::branch_folders( const string& expr, vector< string >& folders, branch_style style )
{
   ostringstream osstr;

   branch_folders( expr, osstr, style );

   string folder_list( osstr.str( ) );

   if( !folder_list.empty( ) && folder_list[ folder_list.length( ) - 1 ] == '\n' )
      folder_list = folder_list.substr( 0, folder_list.length( ) - 1 );

   if( !folder_list.empty( ) )
      split( folder_list, folders, '\n' );
}

void ods_file_system::branch_folders( const string& expr, ostream& os, branch_style style )
{
   bool brief = ( style == e_branch_style_brief );
   bool full_blown = ( style == e_branch_style_full_blown );

   string entity_expr( current_folder );

   expand_entity_expression( expr, entity_expr );

#ifdef DEBUG
   cerr << "expr = '" << expr << "', entity_expr = '" << entity_expr << "' (branch folders)" << endl;
#endif

   vector< pair< string, string > > search_replaces;

   search_replaces.push_back( make_pair( c_pipe_separator, c_folder_separator ) );

   string prefix( entity_expr );

   string::size_type pos = prefix.find_first_of( "?*" );

   if( pos != string::npos )
   {
      pos = entity_expr.rfind( '/', pos );

      if( pos != string::npos )
         prefix = entity_expr.substr( 0, pos );
   }

   string prefix_1( prefix );
   string prefix_2( prefix );

   if( ( prefix_1.size( ) > 1 )
    && ( prefix_1[ prefix_1.length( ) - 1 ] != c_folder ) )
      prefix_1 += string( c_folder_separator );

   // NOTE: Unless a wildcard expression was provided need
   // to append "*" in order to match all branch items and
   // a trailing folder appended before that to ensure the
   // starting folder itself is not included (in order for
   // output to be consistent with other branch commands).
   if( entity_expr.find_first_of( "?*" ) == string::npos )
   {
      if( !entity_expr.empty( )
       && ( entity_expr[ entity_expr.length( ) - 1 ] != c_folder ) )
         entity_expr += c_folder;

      entity_expr += "*";
   }

   perform_match( os, entity_expr, "", 0, &search_replaces,
    ( full_blown ? 0 : prefix_1.c_str( ) ), ( full_blown ? 0 : prefix_2.c_str( ) ), '\0', e_file_size_output_type_none, "|/" );
}

void ods_file_system::add_file( const string& name,
 const string& source, ostream* p_os, istream* p_is, progress* p_progress, const string* p_perms )
{
   string file_name( source );

   bool is_empty_file = false;

   if( file_name == "*" )
      is_empty_file = true;

   if( file_name.empty( ) )
      file_name = name;

   btree_type& bt( p_impl->bt );

   bool has_utf8 = false;

   if( !p_impl->allow_specials && valid_file_name( name, &has_utf8 ) != name )
      throw runtime_error( "invalid file name '" + name + "'" );
   else
   {
      if( !p_is && !is_empty_file && !file_exists( file_name ) )
         throw runtime_error( "file '" + file_name + "' not found" );

      string value( key_value( name ) );

      auto_ptr< ods::bulk_write > ap_bulk;

      if( !o.is_thread_bulk_write_locked( ) )
         ap_bulk.reset( new ods::bulk_write( o, p_progress ) );

      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
      {
         o >> bt;

         p_impl->next_transaction_id = o.get_next_transaction_id( );
      }

      auto_ptr< ods::transaction > ap_ods_tx;

      if( !o.is_in_transaction( ) )
         ap_ods_tx.reset( new ods::transaction( o ) );

      btree_trans_type bt_tx( bt );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
      tmp_item.val = current_folder + string( c_folder_separator ) + name;

      tmp_iter = bt.find( tmp_item );

      if( tmp_iter != bt.end( ) )
         throw runtime_error( "a folder with the name '" + name + "' already exists" );
      else
#endif
      {
         tmp_item.val = value;
         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
            throw runtime_error( "file '" + name + "' already exists" );
         else
         {
            scoped_ods_instance so( o );

            if( !p_is )
            {
               if( !is_empty_file )
               {
                  tmp_item.set_perms( p_perms ? *p_perms : file_perms( file_name ) );

                  // NOTE: Omits the unix time for regression tests.
                  if( !p_impl->for_regression_tests )
                     tmp_item.set_time( last_modification_time( file_name ) );
               }

               // NOTE: If the source was "*" or is a zero length file then in order not
               // to waste space on a second object the special zero OID is instead used
               // as an empty file content indicator.
               if( is_empty_file
                || ( file_exists( file_name ) && !file_size( file_name, 0, 1 ) ) )
                  tmp_item.o_file.set_id( 0 );
               else
                  tmp_item.get_file( new storable_file_extra( file_name, 0, p_progress ) ).store( );
            }
            else
            {
               string next, content;

               if( p_os )
                  *p_os << "Enter multi-line content followed by a line with just a single dot (.) to finish:" << endl;

               bool first = true;

               while( getline( *p_is, next ) )
               {
                  if( next == "." )
                     break;

                  if( first )
                     first = false;
                  else
                     content += '\n';

                  content += next;
               }

               istringstream isstr( content );

               tmp_item.get_file( new storable_file_extra( file_name, isstr, content.size( ), p_progress ) ).store( );
            }

            bt.insert( tmp_item );

            bt_tx.commit( );

            if( ap_ods_tx.get( ) )
               ap_ods_tx->commit( );

            p_impl->next_transaction_id = o.get_next_transaction_id( );
         }
      }
   }
}

void ods_file_system::get_file( const string& name,
 const string& destination, ostream* p_os, progress* p_progress )
{
   string file_name( destination );

   if( file_name.empty( ) )
      file_name = name;

   btree_type& bt( p_impl->bt );

   string value( key_value( name ) );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   tmp_item.val = value;

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      if( !p_os )
         throw runtime_error( "file '" + name + "' not found" );
      else
      {
         *p_os << "*** file '" << name << "' not found ***" << endl;
         return;
      }
   }
   else
   {
      tmp_item = *tmp_iter;

      scoped_ods_instance so( o );

      *tmp_item.get_file( new storable_file_extra( file_name, p_os, p_progress ) );

      if( !p_os && tmp_item.perm_val )
      {
         file_perms( file_name, tmp_item.get_perms( ) );

         if( tmp_item.time_val )
         {
            time_t tm = tmp_item.time_val;
            file_touch( file_name, &tm );
         }
      }
   }
}

bool ods_file_system::is_link( const string& name )
{
   bool retval = false;

   btree_type& bt( p_impl->bt );

   string value( key_value( name ) );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::item_type tmp_item;
   btree_type::iterator i = bt.end( );

   tmp_item.val = value;

   i = bt.find( tmp_item );

   retval = ( i != bt.end( ) ) && i->get_is_link( );

   return retval;
}

bool ods_file_system::has_file( const string& name, bool is_prefix,
 string* p_suffix, string* p_perms, int64_t* p_tm_val, int64_t* p_num_bytes )
{
   bool retval = false;

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::item_type tmp_item;
   btree_type::iterator i = bt.end( );

   string value( key_value( name ) );

   tmp_item.val = value;

   if( !is_prefix )
   {
      i = bt.find( tmp_item );

      retval = ( i != bt.end( ) );
   }
   else
   {
      i = bt.lower_bound( tmp_item );

      if( i != bt.end( ) && ( i->val.find( value ) == 0 ) )
      {
         if( p_suffix )
            *p_suffix = i->val.substr( value.length( ) );

         retval = true;
      }
   }

   if( retval && p_perms )
   {
      tmp_item = *i;

      if( tmp_item.perm_val )
      {
         *p_perms = tmp_item.get_perms( );

         if( p_tm_val )
            *p_tm_val = tmp_item.get_time( );
      }
   }

   if( retval && p_num_bytes )
   {
      *p_num_bytes = 0;

      if( i->get_file( ).get_id( ).get_num( ) )
         *p_num_bytes = o.get_size( i->get_file( ).get_id( ) );
   }

   return retval;
}

string ods_file_system::last_file_name_with_prefix( const string& prefix )
{
   string retval;

   btree_type& bt( p_impl->bt );

   string value( current_folder );

   replace( value, c_folder_separator, c_pipe_separator );

   value += string( c_folder_separator );

   size_t path_length = value.length( );

   value += prefix;

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::item_type tmp_item;

   tmp_item.val = value;

   btree_type::iterator i = bt.lower_bound( tmp_item );

   if( i != bt.end( ) && ( i->val.find( value ) == 0 ) )
   {
      while( true )
      {
         retval = i->val.substr( path_length );

         btree_type::iterator j = ++i;

         if( j == bt.end( ) )
            break;
         else if( j->val.find( value ) != 0 )
            break;

         i = j;
      }
   }

   return retval;
}

string ods_file_system::link_source( const string& name )
{
   string retval( name );

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   string dest_name;
   string dest_folder;

   // NOTE: As link names will be returned by
   // "list_files" with a trailing '*' if the
   // last character is that then will ignore
   // it (but will not accept it in any other
   // position).
   string::size_type pos = name.rfind( '*' );

   if( pos != ( name.length( ) - 1 ) )
      pos = string::npos;

   string value( value_folder_and_file_name( name.substr( 0, pos ), &dest_folder, &dest_name ) );

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   tmp_item.val = value;

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter != bt.end( ) )
   {
      tmp_item = *tmp_iter;

      if( tmp_item.get_is_link( ) )
      {
         oid id( tmp_item.get_file( ).get_id( ) );

         string special_prefix( to_string( id.get_num( ) ) + c_special );

         tmp_item.val = special_prefix;

         tmp_iter = bt.lower_bound( tmp_item );

         if( ( tmp_iter != bt.end( ) )
          && ( tmp_iter->val.find( special_prefix ) == 0 ) )
         {
            retval = tmp_iter->val.substr( special_prefix.length( ) );

            replace( retval, c_pipe_separator, c_folder_separator );

            if( !retval.empty( ) && ( retval[ 0 ] != c_folder ) )
               retval = c_folder_separator + retval;
         }
      }
   }

   return retval;
}

void ods_file_system::link_file( const string& name, const string& source )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   string dest_name;
   string dest_folder;

   string value( value_folder_and_file_name( name, &dest_folder, &dest_name ) );

   bool has_utf8 = false;

   if( !p_impl->allow_specials
    && valid_file_name( dest_name, &has_utf8 ) != dest_name )
      throw runtime_error( "invalid file name '" + name + "'" );
   else
   {
      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      auto_ptr< ods::transaction > ap_ods_tx;

      if( !o.is_in_transaction( ) )
         ap_ods_tx.reset( new ods::transaction( o ) );

      btree_trans_type bt_tx( bt );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
      tmp_item.val = current_folder + string( c_folder_separator ) + name;

      tmp_iter = bt.find( tmp_item );

      if( tmp_iter != bt.end( ) )
         throw runtime_error( "a folder with the name '" + name + "' already exists" );
      else
#endif
      {
         tmp_item.val = value;

         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
            throw runtime_error( "file '" + name + "' already exists" );
         else
         {
            string source_name;
            string source_folder;

            string source_value( value_folder_and_file_name( source, &source_folder, &source_name ) );

            tmp_item.val = source_value;

            tmp_iter = bt.find( tmp_item );

            if( tmp_iter == bt.end( ) )
               throw runtime_error( "file '" + source + "' not found" );
            else
            {
               oid id( tmp_iter->get_file( ).get_id( ) );

               if( !id.get_num( ) )
                  throw runtime_error( "linking to a zero length file '" + source + "' is not supported" );

               string link_prefix( to_string( id.get_num( ) ) + c_pipe_separator );

               tmp_item.val = link_prefix;

               tmp_iter = bt.lower_bound( tmp_item );

               // NOTE: If is the first link then creates a special object for the source.
               if( ( tmp_iter == bt.end( ) ) || ( tmp_iter->val.find( link_prefix ) != 0 ) )
               {
                  tmp_item.val = to_string( id.get_num( ) ) + c_special + source_value.substr( 1 );

                  tmp_item.set_is_link( );

                  tmp_item.get_file( ).get_id( ).set_new( );

                  bt.insert( tmp_item );
               }

               tmp_item.val = value;
               tmp_item.set_is_link( );

               tmp_item.get_file( ).set_id( id );

               bt.insert( tmp_item );

               // NOTE: In order to make locating file links straight forward when
               // the file is later removed (or its content is replaced) a special
               // object that starts with the OID's number is created.
               tmp_item.val = to_string( id.get_num( ) ) + value;

               tmp_item.get_file( ).get_id( ).set_new( );

               bt.insert( tmp_item );

               bt_tx.commit( );

               if( ap_ods_tx.get( ) )
                  ap_ods_tx->commit( );

               p_impl->next_transaction_id = o.get_next_transaction_id( );
            }
         }
      }
   }
}

void ods_file_system::move_file( const string& source, const string& destination )
{
   string src_name;
   string src_folder;

   string src_value( value_folder_and_file_name( source, &src_folder, &src_name ) );

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   string dest_name;
   string dest_folder;

   string dest_path( destination );

   if( has_folder( destination ) )
      dest_path += c_folder + src_name;
   else if( ( destination == c_parent_folder ) || ( destination == c_current_folder ) )
      dest_path = current_folder + c_folder + src_name;

   string dest_value( value_folder_and_file_name( dest_path, &dest_folder, &dest_name ) );

   bool has_utf8 = false;

   if( !p_impl->allow_specials && valid_file_name( dest_name, &has_utf8 ) != dest_name )
      throw runtime_error( "invalid destination file name '" + dest_name + "'" );
   else
   {
      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      auto_ptr< ods::transaction > ap_ods_tx;
      if( !o.is_in_transaction( ) )
         ap_ods_tx.reset( new ods::transaction( o ) );

      btree_trans_type bt_tx( bt );

      tmp_item.val = src_value;
      tmp_iter = bt.find( tmp_item );

      if( tmp_iter == bt.end( ) )
         throw runtime_error( "file '" + source + "' not found" );
      else
      {
         tmp_item = *tmp_iter;

         bool is_link = tmp_item.get_is_link( );
         oid id( tmp_item.get_file( ).get_id( ) );

         if( !dest_folder.empty( ) )
         {
            tmp_item.val = dest_value;
            tmp_item.get_file( ).set_id( id );

            if( is_link )
               tmp_item.set_is_link( );

            if( bt.find( tmp_item ) != bt.end( ) )
               throw runtime_error( "destination file '" + dest_name + "' already exists" );
            else
            {
               bt.erase( tmp_iter );
               bt.insert( tmp_item );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
               replace( tmp_item.val, c_pipe_separator, c_colon_separator );

               tmp_iter = bt.find( tmp_item );

               if( tmp_iter != bt.end( ) )
                  throw runtime_error( "a folder with the name '" + dest_name + "' already exists" );
               else
#endif
               {
                  // NOTE: If this is a file link then need to also update its special object.
                  if( is_link )
                  {
                     tmp_item.val = to_string( id.get_num( ) ) + src_value;

                     tmp_iter = bt.find( tmp_item );

                     if( tmp_iter != bt.end( ) )
                     {
                        tmp_item = *tmp_iter;

                        bt.erase( tmp_iter );

                        tmp_item.val = to_string( id.get_num( ) ) + dest_value;

                        tmp_item.get_file( ).get_id( ).set_new( );

                        bt.insert( tmp_item );
                     }
                  }
                  // NOTE: Check and update the special object for the file (if has any links).
                  else if( id.get_num( ) )
                  {
                     tmp_item.val = to_string( id.get_num( ) ) + c_special + src_value.substr( 1 );

                     tmp_iter = bt.find( tmp_item );

                     if( tmp_iter != bt.end( ) )
                     {
                        tmp_item = *tmp_iter;

                        bt.erase( tmp_iter );

                        tmp_item.val = to_string( id.get_num( ) ) + c_special + dest_value.substr( 1 );

                        tmp_item.get_file( ).get_id( ).set_new( );

                        bt.insert( tmp_item );
                     }
                  }

                  bt_tx.commit( );

                  if( ap_ods_tx.get( ) )
                     ap_ods_tx->commit( );

                  p_impl->next_transaction_id = o.get_next_transaction_id( );
               }
            }
         }
      }
   }
}

bool ods_file_system::store_file( const string& name,
 const string& source, ostream* p_os, istream* p_is, progress* p_progress, bool force_write )
{
   string file_name( source );

   if( file_name.empty( ) )
      file_name = name;

   if( !force_write )
      force_write = p_impl->force_write;

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o, p_progress ) );

   bool changed = false;

   string perms;
   int64_t tm_val = 0;

   if( !p_is && file_exists( file_name ) )
   {
      perms = file_perms( file_name );
      tm_val = last_modification_time( file_name );
   }

   string old_perms;
   int64_t old_tm_val = 0;

   if( !has_file( name, false, 0, &old_perms, &old_tm_val ) )
   {
      changed = true;
      add_file( name, source, p_os, p_is, p_progress );
   }
   else if( p_is || !tm_val || force_write || ( perms != old_perms ) || ( tm_val != old_tm_val ) )
   {
      changed = true;
      replace_file( name, source, p_os, p_is, p_progress, force_write );
   }

   return changed;
}

void ods_file_system::remove_file( const string& name, ostream* p_os, progress* p_progress, bool is_prefix )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o, p_progress ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   auto_ptr< ods::transaction > ap_ods_tx;

   if( !o.is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( o ) );

   btree_trans_type bt_tx( bt );

   if( remove_items_for_file( name, is_prefix, true ) )
   {
      bt_tx.commit( );

      if( ap_ods_tx.get( ) )
         ap_ods_tx->commit( );

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }
}

void ods_file_system::replace_file( const string& name,
 const string& source, ostream* p_os, istream* p_is, progress* p_progress, bool force_write )
{
   string file_name( source );

   if( file_name.empty( ) )
      file_name = name;

   if( !force_write )
      force_write = p_impl->force_write;

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o, p_progress ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   auto_ptr< ods::transaction > ap_ods_tx;

   if( !o.is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( o ) );

   btree_trans_type bt_tx( bt );

   string value( key_value( name ) );

   tmp_item.val = value;

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      if( !p_os )
         throw runtime_error( "file '" + name + "' not found" );
      else
      {
         *p_os << "*** file '" << name << "' not found ***" << endl;
         return;
      }
   }
   else
   {
      oid id( tmp_iter->get_file( ).get_id( ) );

      tmp_item.get_file( ).set_id( id );

      scoped_ods_instance so( o );

      if( !p_is )
      {
         bool changed = false;

         string perms;
         int64_t tm_val = 0;

         if( file_name != "*" )
         {
            perms = file_perms( file_name );
            tm_val = last_modification_time( file_name );
         }

         string old_perms( tmp_iter->get_perms( ) );
         int64_t old_tm_val = tmp_iter->get_time( );

         if( ( perms != old_perms ) || ( tm_val != old_tm_val ) )
         {
            changed = true;

            tmp_item.set_perms( perms );

            // NOTE: Omits the unix time for regression tests.
            if( !p_impl->for_regression_tests )
               tmp_item.set_time( tm_val );
         }

         // NOTE: If the source was "*" or is a zero length file then in order not
         // to waste space on a second object the special zero OID is instead used
         // as an empty file content indicator.
         if( ( file_name == "*" )
          || ( file_exists( file_name ) && !file_size( file_name, 0, 1 ) ) )
         {
            if( id.get_num( ) )
            {
               changed = true;
               o.destroy( id );
            }

            tmp_item.get_file( ).set_id( 0 );
         }
         else
         {
            if( !id.get_num( ) )
            {
               changed = true;

               id.set_new( );
               tmp_item.get_file( ).set_id( id );
            }

            if( force_write || !old_tm_val || ( tm_val != old_tm_val ) )
               tmp_item.get_file(
                new storable_file_extra( file_name, 0, p_progress ) ).store( e_oid_pointer_opt_force_write_skip_read );
         }

         // NOTE: If anything has changed then need to replace the item.
         if( changed )
         {
            bt.erase( tmp_iter );
            bt.insert( tmp_item );
         }
      }
      else
      {
         string next, content;

         if( p_os )
            *p_os << "Enter multi-line content followed by a line with just a single dot (.) to finish:" << endl;

         bool first = true;

         while( getline( *p_is, next ) )
         {
            if( next == "." )
               break;

            if( first )
               first = false;
            else
               content += '\n';

            content += next;
         }

         istringstream isstr( content );

         tmp_item.get_file( new storable_file_extra(
          file_name, isstr, content.size( ), p_progress ) ).store( e_oid_pointer_opt_force_write_skip_read );
      }

      bt_tx.commit( );

      if( ap_ods_tx.get( ) )
         ap_ods_tx->commit( );

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }
}

void ods_file_system::get_time_stamp( const string& name, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::item_type tmp_item;
   btree_type::iterator tmp_iter = bt.end( );

   string value( key_value( name ) );

   string folder( full_folder_name( name ) );

   tmp_item.val = value;

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      tmp_item.val = folder;

      tmp_iter = bt.find( tmp_item );
   }

   if( tmp_iter != bt.end( ) )
   {
      tmp_item = *tmp_iter;

      if( p_os )
         *p_os << date_time( tmp_item.get_time( ) ).as_string( e_time_format_hhmmss, true ) << endl;
   }
   else
   {
      if( !p_os )
         throw runtime_error( "object '" + name + "' not found" );
      else
      {
         *p_os << "*** object '" << name << "' not found ***" << endl;
         return;
      }
   }
}

void ods_file_system::set_time_stamp( const string& name, int64_t tm_val, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::item_type tmp_item;
   btree_type::iterator tmp_iter = bt.end( );

   string value( key_value( name ) );

   string folder( full_folder_name( name ) );

   tmp_item.val = value;

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      tmp_item.val = folder;

      tmp_iter = bt.find( tmp_item );
   }

   if( tmp_iter == bt.end( ) )
   {
      if( !p_os )
         throw runtime_error( "object '" + name + "' not found" );
      else
      {
         *p_os << "*** object '" << name << "' not found ***" << endl;
         return;
      }
   }
   else
   {
      tmp_item = *tmp_iter;

      int64_t old_tm_val = tmp_item.get_time( );

      if( tm_val != old_tm_val )
      {
         auto_ptr< ods::transaction > ap_ods_tx;

         if( !o.is_in_transaction( ) )
            ap_ods_tx.reset( new ods::transaction( o ) );

         btree_trans_type bt_tx( bt );

         tmp_item.set_time( tm_val );

         bt.erase( tmp_iter );
         bt.insert( tmp_item );

         bt_tx.commit( );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );

         p_impl->next_transaction_id = o.get_next_transaction_id( );
      }
   }
}

void ods_file_system::get_permissions( const string& name, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::item_type tmp_item;
   btree_type::iterator tmp_iter = bt.end( );

   string value( key_value( name ) );

   string folder( full_folder_name( name ) );

   tmp_item.val = value;

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      tmp_item.val = folder;

      tmp_iter = bt.find( tmp_item );
   }

   if( tmp_iter == bt.end( ) )
   {
      if( !p_os )
         throw runtime_error( "object '" + name + "' not found" );
      else
      {
         *p_os << "*** object '" << name << "' not found ***" << endl;
         return;
      }
   }
   else
   {
      tmp_item = *tmp_iter;

      if( p_os )
         *p_os << tmp_item.get_perms( ) << endl;
   }
}

void ods_file_system::set_permissions( const string& name, const string& perms, ostream* p_os )
{
   string old_perms;

   uint32_t perms_val = perms_to_value( perms );

   if( value_to_perms( perms_val ) != perms )
   {
      if( !p_os )
         throw runtime_error( "invalid file permissions '" + perms + "'" );
      else
      {
         *p_os << "*** invalid file permissions '" << perms << "' ***" << endl;
         return;
      }
   }

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::item_type tmp_item;
   btree_type::iterator tmp_iter = bt.end( );

   string value( key_value( name ) );

   tmp_item.val = value;

   string folder( full_folder_name( name ) );

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      tmp_item.val = folder;

      tmp_iter = bt.find( tmp_item );
   }

   if( tmp_iter == bt.end( ) )
   {
      if( !p_os )
         throw runtime_error( "object '" + name + "' not found" );
      else
      {
         *p_os << "*** object '" << name << "' not found ***" << endl;
         return;
      }
   }
   else
   {
      tmp_item = *tmp_iter;

      string old_perms;

      if( tmp_item.perm_val )
         old_perms = tmp_item.get_perms( );

      if( perms != old_perms )
      {
         auto_ptr< ods::transaction > ap_ods_tx;

         if( !o.is_in_transaction( ) )
            ap_ods_tx.reset( new ods::transaction( o ) );

         btree_trans_type bt_tx( bt );

         tmp_item.set_perms( perms );

         bt.erase( tmp_iter );
         bt.insert( tmp_item );

         bt_tx.commit( );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );

         p_impl->next_transaction_id = o.get_next_transaction_id( );
      }
   }
}

void ods_file_system::store_as_text_file( const string& name, int32_t val )
{
   string value( to_comparable_string( val, false ) );

   istringstream isstr( value );

   store_file( name, &isstr );
}

void ods_file_system::store_as_text_file( const string& name, int64_t val )
{
   string value( to_comparable_string( val, false ) );

   istringstream isstr( value );

   store_file( name, &isstr );
}

void ods_file_system::store_as_text_file( const string& name, const string& val, size_t pad_to_len )
{
   string value( val );

   if( pad_to_len && value.length( ) < pad_to_len )
      value += string( pad_to_len - value.length( ), ' ' );

   istringstream isstr( value );

   store_file( name, &isstr );
}

void ods_file_system::fetch_from_text_file( const string& name, int32_t& val )
{
   ostringstream osstr;
   get_file( name, osstr );

   val = from_string< int32_t >( osstr.str( ) );
}

void ods_file_system::fetch_from_text_file( const string& name, int64_t& val )
{
   ostringstream osstr;
   get_file( name, osstr );

   val = from_string< int64_t >( osstr.str( ) );
}

void ods_file_system::fetch_from_text_file( const string& name, string& val, bool remove_padding )
{
   ostringstream osstr;
   get_file( name, osstr );

   val = osstr.str( );

   if( remove_padding )
      val = trim( val, false, true );
}

void ods_file_system::add_folder( const string& name, ostream* p_os, string* p_perms, int64_t* p_tm_val )
{
   btree_type& bt( p_impl->bt );

   string folder_name( name );

   auto_ptr< temporary_set_folder > ap_temp_folder;

   string::size_type pos = folder_name.rfind( c_folder );

   if( pos != string::npos )
   {
      ap_temp_folder.reset( new temporary_set_folder( *this, folder_name.substr( 0, pos ) ) );
      folder_name.erase( 0, pos + 1 );
   }

   bool has_utf8 = false;

   if( valid_file_name( folder_name, &has_utf8 ) != folder_name )
   {
      if( !p_os )
         throw runtime_error( "invalid folder name '" + folder_name + "'" );
      else
      {
         *p_os << "*** invalid folder name '" << folder_name << "' ***" << endl;
         return;
      }
   }
   else
   {
      auto_ptr< ods::bulk_write > ap_bulk;

      if( !o.is_thread_bulk_write_locked( ) )
         ap_bulk.reset( new ods::bulk_write( o ) );

      if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
      {
         o >> bt;

         p_impl->next_transaction_id = o.get_next_transaction_id( );
      }

      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      auto_ptr< ods::transaction > ap_ods_tx;

      if( !o.is_in_transaction( ) )
         ap_ods_tx.reset( new ods::transaction( o ) );

      btree_trans_type bt_tx( bt );

      bool okay = false;

      if( current_folder == string( c_root_folder ) )
      {
#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
         tmp_item.val = c_pipe + folder_name;
         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
         {
            if( !p_os )
               throw runtime_error( "a file with the name '" + folder_name + "' already exists" );
            else
            {
               *p_os << "*** a file with the name '" << folder_name << "' already exists ***" << endl;
               return;
            }
         }
         else
#endif
         {
            tmp_item.val = current_folder + folder_name;
            tmp_iter = bt.find( tmp_item );

            if( tmp_iter != bt.end( ) )
            {
               if( !p_os )
                  throw runtime_error( "folder '" + folder_name + "' already exists" );
               else
               {
                  *p_os << "*** folder '" << folder_name << "' already exists ***" << endl;
                  return;
               }
            }
            else
            {
               if( p_perms )
               {
                  tmp_item.set_perms( *p_perms );

                  // NOTE: Omits the unix time for regression tests.
                  if( p_tm_val && !p_impl->for_regression_tests )
                     tmp_item.set_time( *p_tm_val );
               }

               bt.insert( tmp_item );

               tmp_item.val = c_colon + tmp_item.val;
               bt.insert( tmp_item );

               okay = true;
            }
         }
      }
      else
      {
         string name_1( current_folder + c_folder + folder_name );
         string name_2( replaced( current_folder, c_folder_separator, c_colon_separator ) + c_folder + folder_name );
#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
         string name_x( replaced( current_folder, c_folder_separator, c_pipe_separator ) + c_folder + folder_name );

         tmp_item.val = name_x;
         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
         {
            if( !p_os )
               throw runtime_error( "a file with the name '" + folder_name + "' already exists" );
            else
            {
               *p_os << "*** a file with the name '" << folder_name << "' already exists ***" << endl;
               return;
            }
         }
         else
#endif
         {
            tmp_item.val = name_1;
            tmp_iter = bt.find( tmp_item );

            if( tmp_iter != bt.end( ) )
            {
               if( !p_os )
                  throw runtime_error( "folder '" + folder_name + "' already exists" );
               else
               {
                  *p_os << "*** folder '" << folder_name << "' already exists ***" << endl;
                  return;
               }
            }
            else
            {
               if( p_perms )
               {
                  tmp_item.set_perms( *p_perms );

                  // NOTE: Omits the unix time for regression tests.
                  if( p_tm_val && !p_impl->for_regression_tests )
                     tmp_item.set_time( *p_tm_val );
               }

               bt.insert( tmp_item );

               tmp_item.val = name_2;
               bt.insert( tmp_item );

               okay = true;
            }
         }
      }

      if( okay )
      {
         bt_tx.commit( );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );

         p_impl->next_transaction_id = o.get_next_transaction_id( );
      }
   }
}

bool ods_file_system::has_folder( const string& name, string* p_perms, int64_t* p_tm_val )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   string full_name( current_folder );

   if( !name.empty( ) )
   {
      if( name[ 0 ] == c_folder )
         full_name = name;
      else
      {
         if( current_folder == string( c_root_folder ) )
            full_name += name;
         else
            full_name += c_folder + name;
      }

      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      tmp_item.val = full_name;

      tmp_iter = bt.find( tmp_item );

      if( tmp_iter != bt.end( ) )
      {
         if( p_perms )
         {
            tmp_item = *tmp_iter;

            if( tmp_item.perm_val )
            {
               if( p_perms )
                  *p_perms = tmp_item.get_perms( );

               if( p_tm_val )
                  *p_tm_val = tmp_item.get_time( );
            }
         }

         return true;
      }
   }

   return false;
}

bool ods_file_system::has_root_folder( const string& name, string* p_perms, int64_t* p_tm_val )
{
   if( name.empty( ) )
      return false;
   else
   {
      string::size_type pos = 0;

      string full_name( c_root_folder );

      if( name[ 0 ] == c_folder )
         ++pos;

      full_name += name.substr( pos );

      return has_folder( full_name, p_perms, p_tm_val );
   }
}

void ods_file_system::move_folder( const string& name, const string& destination, bool overwrite, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   auto_ptr< ods::transaction > ap_ods_tx;

   if( !o.is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( o ) );

   btree_trans_type bt_tx( bt );

   string full_name( current_folder );

   bool src_is_root = false;

   if( current_folder == string( c_root_folder ) )
      src_is_root = true;
   else
      full_name += string( c_folder_separator );

   full_name += name;

   tmp_item.val = full_name;

   tmp_iter = bt.find( tmp_item );

   bool okay = false;

   if( tmp_iter == bt.end( ) )
   {
      if( !p_os )
         throw runtime_error( "folder '" + name + "' not found" );
      else
      {
         *p_os << "*** folder '" << name << "' not found ***" << endl;
         return;
      }
   }
   else
   {
      bool has_utf8 = false;

      string dest_folder = determine_folder( destination, false, true );

      if( dest_folder.empty( ) )
      {
         if( destination.find( c_folder_separator ) != string::npos )
         {
            if( !p_os )
               throw runtime_error( "folder '" + destination + "' does not exist" );
            else
            {
               *p_os << "*** folder '" << destination << "' does not exist ***" << endl;
               return;
            }
         }
         else if( valid_file_name( destination, &has_utf8 ) != destination )
         {
            if( !p_os )
               throw runtime_error( "invalid folder name '" + destination + "'" );
            else
            {
               *p_os << "*** invalid folder name '" << destination << "' ***" << endl;
               return;
            }
         }
         else
         {
            string dest( destination );

            if( !src_is_root )
               dest = current_folder + string( c_folder_separator ) + destination;

            okay = move_files_and_folders( full_name, dest, src_is_root, src_is_root, overwrite );
         }
      }
      else
      {
         bool dest_is_root = ( dest_folder == string( c_root_folder ) );

         if( dest_is_root )
            dest_folder.erase( );

         string::size_type pos = full_name.rfind( c_folder );

         string dest_name( dest_folder + full_name.substr( pos ) );

         if( dest_folder.size( ) >= full_name.size( ) && dest_folder.find( full_name ) == 0 )
         {
            if( !p_os )
               throw runtime_error( "a folder cannot be moved below itself" );
            else
            {
               *p_os << "*** a folder cannot be moved below itself ***" << endl;
               return;
            }
         }
         else
            okay = move_files_and_folders( full_name, dest_name, src_is_root, dest_is_root, overwrite );
      }

      if( okay )
      {
         bt_tx.commit( );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );

         p_impl->next_transaction_id = o.get_next_transaction_id( );
      }
   }
}

void ods_file_system::remove_folder( const string& name, ostream* p_os, bool remove_branch )
{
   bool okay = true;

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   auto_ptr< ods::transaction > ap_ods_tx;

   if( !o.is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( o ) );

   btree_trans_type bt_tx( bt );

   string tmp_folder( determine_folder( name, false, true ) );

   if( tmp_folder.empty( ) )
      okay = false;
   else
   {
      temporary_include_hidden include_hidden( *this );

      restorable< string > tmp_current_folder( current_folder, tmp_folder );

      vector< string > child_folders;

      branch_folders( "", child_folders );

      if( !remove_branch && !child_folders.empty( ) )
      {
         if( !p_os )
            throw runtime_error( "cannot remove '" + name + "' as it has one or more child folders" );
         else
         {
            okay = false;
            *p_os << "*** cannot remove '" << name << "' as it has one or more child folders ***" << endl;
         }
      }
      else
      {
         vector< string > child_files;

         branch_files( "", child_files, e_branch_style_brief );

         if( !remove_branch && !child_files.empty( ) )
         {
            if( !p_os )
               throw runtime_error( "cannot remove '" + name + "' as it has one or more files" );
            else
            {
               okay = false;
               *p_os << "*** cannot remove '" << name << "' as it has one or more files ***" << endl;
            }
         }
         else
         {
            for( size_t i = 0; i < child_files.size( ); i++ )
               remove_file( child_files[ i ] );

            for( size_t i = 0; i < child_folders.size( ); i++ )
               remove_items_for_folder( child_folders[ i ] );
         }
      }
   }

   if( okay )
   {
      if( remove_items_for_folder( tmp_folder, true ) )
      {
         bt_tx.commit( );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );

         p_impl->next_transaction_id = o.get_next_transaction_id( );
      }
   }
}

void ods_file_system::replace_folder( const string& name, ostream* p_os, string* p_perms, int64_t* p_tm_val )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   auto_ptr< ods::transaction > ap_ods_tx;

   if( !o.is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( o ) );

   btree_trans_type bt_tx( bt );

   string full_name( current_folder );

   if( !name.empty( ) )
   {
      if( name[ 0 ] == c_folder )
         full_name = name;
      else
      {
         if( current_folder == string( c_root_folder ) )
            full_name += name;
         else
            full_name += c_folder + name;
      }

      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      tmp_item.val = full_name;

      tmp_iter = bt.find( tmp_item );

      if( tmp_iter == bt.end( ) )
      {
         if( !p_os )
            throw runtime_error( "cannot replace unknown folder '" + name + "'" );
         else
         {
            *p_os << "*** cannot replace unknown folder '" << name << "' ***" << endl;
            return;
         }
      }
      else
      {
         if( !p_perms )
         {
            tmp_item.set_perms( "" );
            tmp_item.set_time( 0 );
         }
         else
         {
            tmp_item.set_perms( *p_perms );
            tmp_item.set_time( p_tm_val ? *p_tm_val : 0 );
         }

         bt.erase( tmp_iter );
         bt.insert( tmp_item );

         bt_tx.commit( );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );

         p_impl->next_transaction_id = o.get_next_transaction_id( );
      }
   }
}

void ods_file_system::rebuild_index( )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_thread_bulk_write_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   ods::transaction tx( o );

   bt.build_index_nodes( );

   tx.commit( );

   p_impl->next_transaction_id = o.get_next_transaction_id( );
}

void ods_file_system::dump_node_data( const string& file_name, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   if( file_name.length( ) )
   {
      ofstream outf( file_name.c_str( ) );

      if( !outf )
      {
         if( !p_os )
            throw runtime_error( "unable to open file '" + file_name + "' for output" );
         else
            *p_os << "error: unable to open file '" << file_name << "' for output" << endl;
      }
      else
         bt.dump_all_info( outf );
   }
   else if( p_os )
      bt.dump_all_info( *p_os );
}

void ods_file_system::force_reload( )
{
   p_impl->next_transaction_id = 0;
}

string ods_file_system::value_folder_and_file_name( const string& provided, string* p_folder, string* p_file_name )
{
   string folder;

   string::size_type pos = provided.rfind( c_folder );

   string file_name( provided.substr( ( pos == string::npos ) ? 0 : ( pos + 1 ) ) );

   if( pos == string::npos )
      folder = current_folder;
   else if( pos == 0 )
      folder = string( c_root_folder );
   else
      folder = determine_folder( provided.substr( 0, pos ) );

   string key( folder );

   replace( key, c_folder_separator, c_pipe_separator );

   key += c_folder + file_name;

   if( p_folder )
      *p_folder = folder;

   if( p_file_name )
      *p_file_name = file_name;

   return key;
}

void ods_file_system::perform_match(
 ostream& os, const string& expr, const string& regexpr, size_t* p_count,
 vector< pair< string, string > >* p_search_replaces, const char* p_prefix_1,
 const char* p_prefix_2, const char erase_all_before_and_including,
 file_size_output_type file_size_output, const char* p_ignore_with_prefix,
 deque< string >* p_extra_items, pair< string, string >* p_range,
 bool inclusive, size_t limit, bool in_reverse_order )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_thread_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
   {
      o >> bt;

      p_impl->next_transaction_id = o.get_next_transaction_id( );
   }

   string suffix( p_impl->branch_suffix );

#ifdef DEBUG
   cerr << "[perform_match] expr = '" << expr << "', suffix = '" << suffix << "', p_prefix_1 = '";

   if( p_prefix_1 )
      cerr << string( p_prefix_1 );

   cerr << "', p_prefix_2 = '";

   if( p_prefix_2 )
      cerr << string( p_prefix_2 );

   cerr << "'" << endl;
#endif

   btree_type::iterator match_iter;
   btree_type::item_type match_item;

   size_t pos = first_wildcard_character_pos( expr );

   match_item.val = expr.substr( 0, pos );

   size_t minimum_length = match_item.val.length( );

   bool is_folder_expr = false;

   if( ( pos != string::npos )
    && ( pos > 0 ) && ( expr[ pos - 1 ] == c_folder ) )
      is_folder_expr = true;

   auto_ptr< regex > ap_regex;

   if( !regexpr.empty( ) )
      ap_regex.reset( new regex( regexpr ) );

#ifdef USE_BULK_PAUSE
   size_t iterations = 0;
#endif

   if( p_range && !p_range->first.empty( ) )
   {
      btree_type::item_type tmp_item;
      tmp_item.val = p_range->first;

      match_iter = bt.lower_bound( tmp_item );
   }
   else
   {
      if( in_reverse_order && match_item.val.empty( ) )
      {
         match_iter = bt.end( );

         if( !bt.empty( ) )
            --match_iter;
      }
      else
      {
         btree_type::item_type tmp_match_item( match_item );

         // NOTE: In order to find the last item add a dummy
         // character (as ODS file system names are expected
         // to only contain valid OS file name characters).
         if( in_reverse_order )
            tmp_match_item.val += ( char )0xff;

         match_iter = bt.lower_bound( tmp_match_item );
      }
   }

   if( !inclusive )
   {
      if( !in_reverse_order )
      {
         if( match_iter != bt.end( ) )
            ++match_iter;
      }
      else
      {
         if( match_iter == bt.begin( ) )
            match_iter = bt.end( );
         else
            --match_iter;
      }
   }

   wildcard_compare_less_functor< btree_type::item_type > compare_less;
   wildcard_compare_equal_functor< btree_type::item_type > compare_equal;

   if( pos != string::npos )
      compare_less.use_truncated_comparison( true );
   else
      compare_less.use_truncated_comparison( false );

   size_t count = 0;

   if( match_iter != bt.end( ) )
   {
      bool okay = true;

      btree_type::item_type tmp_item;

      tmp_item.val = expr;

      do
      {
         if( p_range && !p_range->second.empty( ) && ( match_iter->val >= p_range->second ) )
            break;

#ifdef USE_BULK_PAUSE
         if( ap_bulk.get( ) && ( ++iterations % 5000 == 0 ) )
         {
            btree_type::item_type tmp_item;

            tmp_item.val = match_iter->val;

            ap_bulk->pause( );

            if( p_impl->next_transaction_id != o.get_next_transaction_id( ) )
            {
               o >> bt;

               p_impl->next_transaction_id = o.get_next_transaction_id( );

               match_iter = bt.lower_bound( tmp_item );

               if( match_iter == bt.end( ) )
                  break;
            }
         }
#endif

         bool skip = false;

         if( p_impl->skip_hidden )
         {
            string::size_type from = match_item.val.length( );

            if( from >= 1 )
               --from;

            // NOTE: A hidden folder might be part of the provided
            // expression's match value so checks after that point.
            if( match_iter->val.find( "/.", from ) != string::npos )
               skip = true;
         }

         if( !skip && compare_equal( tmp_item, *match_iter ) )
         {
            string::size_type pos = ap_regex.get( ) ? ap_regex->search( match_iter->val ) : 0;

            if( pos != string::npos )
            {
               if( p_count )
                  ++*p_count;
               else
               {
                  string val( match_iter->val );

                  if( is_folder_expr && ( ( val.length( ) < minimum_length )
                   || ( ( val.length( ) == minimum_length ) && ( val[ val.length( ) - 1 ] == c_folder ) ) ) )
                     val.erase( );

                  if( p_ignore_with_prefix && ( val.find( p_ignore_with_prefix ) == 0 ) )
                     val.erase( );

                  if( !val.empty( ) )
                  {
                     if( p_search_replaces )
                     {
                        for( size_t i = 0; i < p_search_replaces->size( ); i++ )
                           replace( val, ( *p_search_replaces )[ i ].first, ( *p_search_replaces )[ i ].second );

                        // NOTE: For root files and folders will end up with a
                        // double folder prefix (as ":/" or "|/" become "//").
                        if( ( val.size( ) >= 2 )
                         && ( val[ 0 ] == c_folder ) && ( val[ 1 ] == c_folder ) )
                           val.erase( 0, 1 );
                     }

                     if( p_prefix_1 && val.find( p_prefix_1 ) == 0 )
                        val.erase( 0, strlen( p_prefix_1 ) );
                     else if( p_prefix_2 && val.find( p_prefix_2 ) == 0 )
                        val.erase( 0, strlen( p_prefix_2 ) );

                     if( !p_impl->branch_prefix.empty( ) )
                     {
                        string::size_type pos = val.find( p_impl->branch_prefix );

                        if( pos != string::npos )
                           val.erase( 0, p_impl->branch_prefix.length( ) + pos );

                        if( !val.empty( ) && ( val[ 0 ] == c_folder ) )
                           val.erase( 0, 1 );
                     }
                     else if( erase_all_before_and_including )
                     {
                        pos = val.rfind( erase_all_before_and_including,
                         ( ( val.length( ) == 1 ) ? string::npos : ( val.length( ) - 2 ) ) );

                        if( pos != string::npos )
                           val.erase( 0, pos + 1 );
                     }

                     while( p_extra_items && !p_extra_items->empty( ) )
                     {
                        string extra( p_extra_items->front( ) );

                        string::size_type pos = extra.find( '\t' );

                        string extra_list_info;

                        if( pos != string::npos )
                        {
                           extra_list_info = extra.substr( pos + 1 );

                           extra.erase( pos );
                        }

                        string compare( extra );

                        // NOTE: If "val" does not include the path
                        // then remove parent folders from "extra".
                        if( val.find( c_folder ) == string::npos )
                        {
                           string::size_type start = 0;

                           for( size_t i = 0; i < compare.length( ) - 1; i++ )
                           {
                              if( compare[ i ] == c_folder )
                                 start = i + 1;
                           }

                           if( start )
                              compare.erase( 0, start );
                        }

                        if( compare <= val )
                        {
                           bool show_folder = p_impl->show_folders;

                           if( show_folder && !suffix.empty( ) )
                              show_folder = folder_matches_suffix( extra, suffix );

                           if( show_folder )
                              os << extra << '\n';

                           p_extra_items->pop_front( );

                           if( !extra_list_info.empty( ) )
                           {
                              vector< string > list_info_args;

                              split( extra_list_info, list_info_args, '\t' );

                              if( list_info_args.size( ) != 4 )
                                 throw runtime_error( "unexpected list_info_args size " + to_string( list_info_args.size( ) ) );

                              string origin( list_info_args[ 0 ] );
                              string folder( list_info_args[ 1 ] );
                              string objects( list_info_args[ 2 ] );
                              string style( list_info_args[ 3 ] );

#ifdef DEBUG
                              cerr << "0) origin = '" << origin << "', suffix = '" << suffix
                               << "', folder = '" << folder << "', objects = " << objects << ", style = " << style << endl;
#endif
                              string child_expr( origin );

                              if( !child_expr.empty( )
                               && ( child_expr[ child_expr.length( ) - 1 ] != c_folder ) )
                                 child_expr += c_folder;

                              child_expr += folder;

                              if( !suffix.empty( ) )
                                 child_expr += c_folder + suffix;

                              list_files_or_objects( child_expr, os, "",
                               ( objects == "1" ), ( list_style )( style[ 0 ] - '0' ), true, 0, false, true );
                           }
                        }
                        else
                           break;
                     }

                     if( match_iter->get_is_link( ) )
                        val += "*";

                     if( !match_iter->get_file( ).get_id( ).is_new( )
                      && ( file_size_output != e_file_size_output_type_none ) )
                     {
                        int64_t size = 0;

                        if( match_iter->get_file( ).get_id( ).get_num( ) )
                           size = o.get_size( match_iter->get_file( ).get_id( ) );

                        if( file_size_output != e_file_size_output_type_scaled )
                        {
                           val += " (" + format_int( size ) + ')';

                           if( match_iter->perm_val )
                           {
                              val += " " + match_iter->get_perms( );

                              if( match_iter->time_val )
                              {
                                 date_time dtm( match_iter->time_val );
                                 val += " " + dtm.as_string( e_time_format_hhmmss, true );
                              }
                           }
                        }
                        else
                           val += " (" + format_bytes( size ) + ')';
                     }

                     if( match_iter->get_file( ).get_id( ).is_new( )
                      && ( file_size_output == e_file_size_output_type_num_bytes ) )
                     {
                        if( match_iter->perm_val )
                        {
                           val += " " + match_iter->get_perms( );

                           if( match_iter->time_val )
                           {
                              date_time dtm( match_iter->time_val );
                              val += " " + dtm.as_string( e_time_format_hhmmss, true );
                           }
                        }
                     }

                     os << val << '\n';

                     ++count;

                     if( limit && count >= limit )
                        break;
                  }
               }
            }
         }

         if( !in_reverse_order )
         {
            if( ++match_iter == bt.end( ) || !compare_less( *match_iter, match_item ) )
               okay = false;
         }
         else
         {
            if( match_iter == bt.begin( ) || !compare_less( match_item, *--match_iter ) )
               okay = false;
         }
      } while( okay );
   }

   if( p_count )
   {
      if( p_extra_items )
         *p_count += p_extra_items->size( );

      os << *p_count << endl;
   }
   else
   {
      while( p_extra_items && !p_extra_items->empty( ) )
      {
         string extra( p_extra_items->front( ) );

         string::size_type pos = extra.find( '\t' );

         string extra_list_info;

         if( pos != string::npos )
         {
            extra_list_info = extra.substr( pos + 1 );

            extra.erase( pos );
         }

         bool show_folder = p_impl->show_folders;

         if( show_folder && !suffix.empty( ) )
            show_folder = folder_matches_suffix( extra, suffix );

         if( show_folder )
            os << extra << '\n';

         p_extra_items->pop_front( );

         if( !extra_list_info.empty( ) )
         {
            vector< string > list_info_args;

            split( extra_list_info, list_info_args, '\t' );

            if( list_info_args.size( ) != 4 )
               throw runtime_error( "unexpected list_info_args size " + to_string( list_info_args.size( ) ) );

            string origin( list_info_args[ 0 ] );
            string folder( list_info_args[ 1 ] );
            string objects( list_info_args[ 2 ] );
            string style( list_info_args[ 3 ] );

#ifdef DEBUG
            cerr << "1) origin = '" << origin << "', suffix = '" << suffix
             << "', folder = '" << folder << "', objects = " << objects << ", style = " << style << endl;
#endif
            string child_expr( origin );

            if( !child_expr.empty( )
             && ( child_expr[ child_expr.length( ) - 1 ] != c_folder ) )
               child_expr += c_folder;

            child_expr += folder;

            if( !suffix.empty( ) )
               child_expr += c_folder + suffix;

            list_files_or_objects( child_expr, os, "",
             ( objects == "1" ), ( list_style )( style[ 0 ] - '0' ), true, 0, false, true );
         }
      }
   }
}

void ods_file_system::expand_entity_expression(
 const string& expr, string& entity_expr, const char* p_suffix )
{
   bool has_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

   if( !expr.empty( ) )
   {
      // NOTE: If a superflous final folder separator had been provided then remove it.
      if( entity_expr.size( ) > 1
       && ( entity_expr.substr( entity_expr.length( ) - 1 ) == string( c_folder_separator ) ) )
         entity_expr.erase( entity_expr.length( ) - 1 );
   }

   string::size_type pos = expr.rfind( c_folder );

   if( ( pos != string::npos ) || ( expr == string( c_parent_folder ) ) )
   {
      if( pos == 0 )
      {
         if( !has_wildcard || ( expr == string( c_folder_separator ) ) )
            entity_expr = expr;
         else
            entity_expr = string( c_folder_separator ) + expr;

         if( p_suffix && ( entity_expr.size( ) > 1 ) )
            entity_expr += string( p_suffix );
      }
      else
      {
         // NOTE: For an expr like ../.. need to ensure
         // that the trailing parent folder is included.
         if( ( pos != string::npos )
          && ( expr.find( c_parent_folder, pos + 1 ) == ( pos + 1 ) ) )
            pos += strlen( c_parent_folder ) + 1;

         entity_expr = determine_folder( expr.substr( 0, pos ), false, true );

         if( p_suffix && ( pos == string::npos ) && ( entity_expr.size( ) > 1 ) )
            entity_expr += string( p_suffix );

         if( ( pos != string::npos ) && !entity_expr.empty( ) )
         {
            entity_expr += expr.substr( pos );

            if( p_suffix )
               entity_expr += string( p_suffix );
         }
      }
   }
   else if( !expr.empty( ) )
   {
      if( !has_wildcard && ( entity_expr == string( c_folder_separator ) ) )
         entity_expr += expr;
      else
         entity_expr += string( c_folder_separator ) + expr;

      if( p_suffix && !has_wildcard )
         entity_expr += string( p_suffix );
   }
}

void ods_file_system::get_child_folders(
 const string& expr, bool full_blown, deque< string >& folders, bool append_separator )
{
   bool has_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

   vector< pair< string, string > > search_replaces;

   string folder_expr( current_folder );

   expand_entity_expression( expr, folder_expr, has_wildcard ? "\0" : c_folder_separator );

   if( expr.empty( ) && ( current_folder != string( c_root_folder ) ) )
      folder_expr += string( c_folder_separator );

   string::size_type pos = folder_expr.rfind( c_folder );

   replace( folder_expr, c_folder_separator, c_colon_separator );

   if( pos == 0 )
      folder_expr += c_folder;
   else if( pos != string::npos )
      folder_expr[ pos ] = c_folder;

   if( !has_wildcard )
      folder_expr += "*";

   search_replaces.push_back( make_pair( c_colon_separator, c_folder_separator ) );

   ostringstream osstr;

   perform_match( osstr, folder_expr, "", 0, &search_replaces, 0, 0, ( full_blown ? '\0' : c_folder ) );

   split( osstr.str( ), folders, '\n' );

   if( !folders.empty( ) && folders.back( ).empty( ) )
      folders.pop_back( );

   if( full_blown || append_separator )
   {
      for( size_t i = 0; i < folders.size( ); i++ )
      {
         string extra;

         if( append_separator )
            extra += c_folder_separator;

         if( full_blown )
         {
            string perms;

            int64_t tm_val = 0;

            has_folder( folders[ i ], &perms, &tm_val );

            if( !perms.empty( ) )
            {
               extra += " " + perms;

               if( tm_val )
               {
                  date_time dtm( tm_val );
                  extra += " " + dtm.as_string( e_time_format_hhmmss, true );
               }
            }
         }

         folders[ i ] += extra;
      }
   }
}

void ods_file_system::list_files_or_objects(
 const string& expr, ostream& os, const string& start_from, bool objects,
 list_style style, bool inclusive, size_t limit, bool in_reverse_order, bool branch )
{
   bool brief = ( style == e_list_style_brief );
   bool full_blown = ( style == e_list_style_full_blown );

   string entity_expr( current_folder );

   expand_entity_expression( expr, entity_expr );

#ifdef DEBUG
   cerr << "expr = '" << expr << "', entity_expr = '" << entity_expr << "', branch = " << branch << endl;
#endif

   if( !entity_expr.empty( ) )
   {
      bool is_folder = has_folder( entity_expr )
       || ( entity_expr == string( c_root_folder ) );

      bool has_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

      pair< string, string > range;

      if( !start_from.empty( ) )
      {
         range.first = replaced( current_folder,
          c_folder_separator, c_pipe_separator ) + c_folder_separator + start_from;
      }

      deque< string > extras;

      vector< pair< string, string > > search_replaces;

      if( branch || objects )
      {
         if( !branch )
            get_child_folders( expr, full_blown, extras );
         else
         {
            string suffix( expr );

            string::size_type pos = suffix.rfind( c_folder );

            if( pos != string::npos )
               suffix.erase( 0, pos + 1 );

            if( suffix.find_first_of( "?*" ) == string::npos )
               suffix.erase( );

            string prefix( expr );

            if( !suffix.empty( ) )
            {
               pos = prefix.rfind( suffix );

               if( pos != string::npos )
                  prefix.erase( pos );
            }

            if( p_impl->branch_suffix == c_dummy_suffix )
            {
               p_impl->branch_suffix = suffix;

               // NOTE: If the full path will not be output
               // store the branch prefix for later removal.
               if( !full_blown )
               {
                  string branch_prefix( entity_expr );

                  pos = branch_prefix.find_first_of( "?*" );

                  if( pos != string::npos )
                  {
                     branch_prefix.erase( pos );

                     pos = branch_prefix.rfind( c_folder );

                     if( pos != branch_prefix.length( ) )
                        branch_prefix.erase( pos + 1 );

                  }

                  if( !branch_prefix.empty( )
                   && ( branch_prefix[ branch_prefix.length( ) - 1 ] != c_folder ) )
                     branch_prefix += c_folder;

                  p_impl->branch_prefix = branch_prefix;
               }
            }

            if( !prefix.empty( ) && ( prefix[ prefix.length( ) - 1 ] == c_folder ) )
               prefix.erase( prefix.length( ) - 1 );

            get_child_folders( prefix, false, extras, false );
#ifdef DEBUG
            cerr << "(added " << extras.size( ) << " folders for '"
             << prefix << "' suffix = '" << suffix << "')" << endl;
#endif

            // NOTE: Rework "extras" to include the prefix
            // and append data (tab separated columns) for
            // usage in nested calls to this function.
            for( size_t i = 0; i < extras.size( ); i++ )
            {
               string child_folder( extras[ i ] );

               string::size_type pos = child_folder.rfind( c_folder );

               if( pos != string::npos )
                  child_folder.erase( 0, pos + 1 );

               string extra( prefix );

               if( !extra.empty( ) )
                  extra += c_folder;

               extra += child_folder + c_folder;

               if( full_blown && !extra.empty( )
                && ( extra[ 0 ] != c_folder ) )
                  extra = c_folder + extra;

               // NOTE: Remove the branch prefix from "extra"
               // (which includes leading and trailing folder
               // characters that might not be present).
               if( !p_impl->branch_prefix.empty( ) )
               {
                  string branch_prefix( p_impl->branch_prefix );

                  if( !branch_prefix.empty( ) && ( branch_prefix[ 0 ] == c_folder ) )
                  {
                     if( !extra.empty( ) && ( extra[ 0 ] != c_folder ) )
                        branch_prefix.erase( 0, 1 );
                  }

                  pos = extra.find( branch_prefix );

                  if( pos != string::npos )
                     extra.erase( 0, branch_prefix.length( ) + pos );
               }

               extras[ i ] = extra + '\t' + prefix + '\t' + child_folder
                + '\t' + to_string( objects ) + '\t' + to_string( ( int )style );
            }
         }
      }

      string::size_type pos = entity_expr.rfind( c_folder );

      replace( entity_expr, c_folder_separator, c_pipe_separator );

      if( is_folder && !has_wildcard )
         entity_expr += "/*";
      else if( pos != string::npos )
         entity_expr[ pos ] = c_folder;

      search_replaces.clear( );
      search_replaces.push_back( make_pair( c_pipe_separator, c_folder_separator ) );

      perform_match( os, entity_expr, "", 0, &search_replaces, 0, 0,
       ( full_blown ? '\0' : c_folder ), ( brief ? e_file_size_output_type_none
       : ( full_blown ? e_file_size_output_type_num_bytes : e_file_size_output_type_scaled ) ),
       0, &extras, &range, inclusive, limit, in_reverse_order );
   }
}

void ods_file_system::branch_files_or_objects( ostream& os,
 const string& folder, const string& expr, branch_style style,
 bool show_folders, const string* p_start_folder, size_t depth )
{
   auto_ptr< restorable< bool > > ap_show_folders;

   restorable< string > tmp_branch_prefix( p_impl->branch_prefix, "" );
   restorable< string > tmp_branch_suffix( p_impl->branch_suffix, c_dummy_suffix );

   if( !show_folders )
      ap_show_folders.reset( new restorable< bool >( p_impl->show_folders, false ) );

   list_files_or_objects( expr, os, "", show_folders, ( list_style )style, true, 0, false, true );
}

bool ods_file_system::move_files_and_folders( const string& source,
 const string& destination, bool src_is_root, bool dest_is_root, bool replace_existing )
{
   string full_name( source );
   string dest_name( destination );

   btree_type& bt( p_impl->bt );

   string::size_type pos = destination.find( c_folder );

   if( pos == string::npos )
   {
      pos = full_name.rfind( c_folder );

      dest_name = full_name.substr( 0, pos + 1 ) + destination;
   }

   try
   {
      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      tmp_item.val = full_name;

      tmp_iter = bt.find( tmp_item );

      while( true )
      {
         string next( tmp_iter->val );

         string::size_type pos = next.find( full_name );

         if( pos != 0 )
            break;

         bt.erase( tmp_iter );

         next.erase( 0, full_name.length( ) );

         next = dest_name + next;

         tmp_item.val = next;

         if( bt.find( tmp_item ) != bt.end( ) )
         {
            if( !replace_existing )
               throw runtime_error( "folder '" + next + "' already exists" );
         }
         else
            bt.insert( tmp_item );

         tmp_item.val = full_name;
         tmp_iter = bt.lower_bound( tmp_item );

         if( tmp_iter == bt.end( ) )
            break;
      }

      string::size_type fpos = full_name.rfind( c_folder );
      string::size_type dpos = dest_name.rfind( c_folder );

      replace( full_name, c_folder_separator, c_colon_separator );
      replace( dest_name, c_folder_separator, c_colon_separator );

      if( src_is_root )
      {
         string root_name( full_name );

         root_name.insert( 1, c_folder_separator );

         tmp_item.val = root_name;

         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
            bt.erase( tmp_iter );
      }
      else
      {
         if( fpos != string::npos )
         {
            string folder_name( full_name );

            folder_name[ fpos ] = c_folder;

            tmp_item.val = folder_name;

            tmp_iter = bt.find( tmp_item );

            if( tmp_iter != bt.end( ) )
               bt.erase( tmp_iter );
         }
      }

      if( dest_is_root )
      {
         string new_root_name( dest_name );

         new_root_name.insert( 1, c_folder_separator );

         tmp_item.val = new_root_name;

         if( bt.find( tmp_item ) == bt.end( ) )
            bt.insert( tmp_item );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
         replace( new_root_name, c_colon_separator, c_pipe_separator );

         tmp_item.val = new_root_name;

         if( bt.find( tmp_item ) != bt.end( ) )
            throw runtime_error( "a file with the name '"
             + replaced( dest_name, c_colon_separator, c_folder_separator ) + "' already exists" );
#endif
      }
      else
      {
         string new_folder_name( dest_name );

         if( dpos != string::npos )
         {
            new_folder_name[ dpos ] = c_folder;

            tmp_item.val = new_folder_name;

            if( bt.find( tmp_item ) == bt.end( ) )
               bt.insert( tmp_item );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
            replace( new_folder_name, c_colon_separator, c_pipe_separator );

            tmp_item.val = new_folder_name;

            if( bt.find( tmp_item ) != bt.end( ) )
               throw runtime_error( "a file with the name '"
                + replaced( dest_name, c_colon_separator, c_folder_separator ) + "' already exists" );
#endif
         }
      }

      tmp_item.val = full_name;

      while( true )
      {
         tmp_item.val = full_name;

         tmp_iter = bt.lower_bound( tmp_item );

         if( tmp_iter == bt.end( ) )
            break;

         string next( tmp_iter->val );

         string::size_type pos = next.find( full_name );

         if( pos != 0 )
            break;

         bt.erase( tmp_iter );

         next.erase( 0, full_name.length( ) );

         next.insert( 0, dest_name );

         tmp_item.val = next;

         bool already_exists = false;

         if( replace_existing )
            already_exists = ( bt.find( tmp_item ) != bt.end( ) );

         if( !already_exists )
         {
            bt.insert( tmp_item );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
            replace( next, c_colon_separator, c_pipe_separator );

            tmp_item.val = next;

            if( bt.find( tmp_item ) != bt.end( ) )
               throw runtime_error( "a file with the name '"
                + replaced( next, c_pipe_separator, c_folder_separator ) + "' already exists" );
#endif
         }
      }

      replace( full_name, c_colon_separator, c_pipe_separator );
      replace( dest_name, c_colon_separator, c_pipe_separator );

      tmp_item.val = full_name;

      while( true )
      {
         tmp_item.val = full_name;

         tmp_iter = bt.lower_bound( tmp_item );

         if( tmp_iter == bt.end( ) )
            break;

         string next( tmp_iter->val );

         string old_name( next );

         string::size_type pos = next.find( full_name );

         if( pos != 0 )
            break;

         bool is_link = tmp_iter->get_is_link( );

         oid id = tmp_iter->get_file( ).get_id( );

         bt.erase( tmp_iter );

         next.erase( 0, full_name.length( ) );

         next.insert( 0, dest_name );

         if( is_link )
            tmp_item.set_is_link( );

         tmp_item.get_file( ).set_id( id );

         tmp_item.val = next;

         bool already_exists = false;

         if( replace_existing )
            already_exists = ( bt.find( tmp_item ) != bt.end( ) );

         if( already_exists && !replace_existing )
            throw runtime_error( "file '"
             + replaced( next, c_pipe_separator, c_folder_separator ) + "' already exists" );
         else
         {
            bt.insert( tmp_item );

            string next_name( next );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
            replace( next, c_pipe_separator, c_colon_separator );

            tmp_item.val = next;

            if( bt.find( tmp_item ) != bt.end( ) )
               throw runtime_error( "a folder with the name '"
                + replaced( next, c_colon_separator, c_folder_separator ) + "' already exists" );
#endif
            // NOTE: If this is a file link then need to also update its special object.
            if( is_link )
            {
               tmp_item.val = to_string( id.get_num( ) ) + old_name;

               tmp_iter = bt.find( tmp_item );

               if( tmp_iter != bt.end( ) )
               {
                  bt.erase( tmp_iter );

                  tmp_item.val = to_string( id.get_num( ) ) + next_name;

                  tmp_item.get_file( ).get_id( ).set_new( );

                  bt.insert( tmp_item );
               }
            }
            else
            {
               // NOTE: Check and update the special object for the file (if has any links).
               tmp_item.val = to_string( id.get_num( ) ) + c_special + old_name.substr( 1 );

               tmp_iter = bt.find( tmp_item );

               if( tmp_iter != bt.end( ) )
               {
                  bt.erase( tmp_iter );

                  tmp_item.val = to_string( id.get_num( ) ) + c_special + next_name.substr( 1 );

                  tmp_item.get_file( ).get_id( ).set_new( );

                  bt.insert( tmp_item );
               }
            }
         }
      }
   }
   catch( exception& x )
   {
      cout << x.what( ) << endl;
      return false;
   }

   return true;
}

bool ods_file_system::remove_items_for_file(
 const string& name, bool is_prefix, bool ignore_not_found )
{
   bool okay = true;

   btree_type& bt( p_impl->bt );

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   if( name.empty( ) )
      throw runtime_error( "unexpected empty file name in 'remove_items_for_file'" );

   string::size_type pos = name.rfind( c_folder );

   string src_name( name.substr( pos == string::npos ? 0 : pos + 1 ) );

   string src_folder( current_folder );

   if( pos != string::npos )
      src_folder = determine_folder( name.substr( 0, pos ) );

   tmp_item.val = src_folder;

   tmp_item.val += c_folder + src_name;

   pos = tmp_item.val.rfind( c_folder );

   replace( tmp_item.val, c_folder_separator, c_pipe_separator );

   if( pos != string::npos )
   {
      tmp_item.val[ pos ] = c_folder;

      if( pos == 0 )
         tmp_item.val = c_pipe + tmp_item.val;
   }

   if( !is_prefix )
      tmp_iter = bt.find( tmp_item );
   else
   {
      tmp_iter = bt.lower_bound( tmp_item );

      if( tmp_iter->val.find( tmp_item.val ) != 0 )
         tmp_iter = bt.end( );
   }

   string item_name( tmp_item.val );

   if( tmp_iter == bt.end( ) )
   {
      if( !ignore_not_found )
         throw runtime_error( "file '" + name + "' not found" );

      okay = false;
   }
   else
   {
      oid id( tmp_iter->get_file( ).get_id( ) );

      bool is_link = tmp_iter->get_is_link( );

      bt.erase( tmp_iter );

      if( !id.is_new( ) && id.get_num( ) )
      {
         if( is_link )
         {
            tmp_item.val = to_string( id.get_num( ) ) + tmp_item.val;

            tmp_iter = bt.find( tmp_item );

            if( tmp_iter != bt.end( ) )
            {
               bt.erase( tmp_iter );

               string link_prefix( to_string( id.get_num( ) ) + c_pipe_separator );

               tmp_item.val = link_prefix;

               tmp_iter = bt.lower_bound( tmp_item );

               // NOTE: If no other links exist then remove the special link object.
               if( tmp_iter == bt.end( ) || tmp_iter->val.find( link_prefix ) != 0 )
               {
                  string special_prefix( to_string( id.get_num( ) ) + c_special );

                  tmp_item.val = special_prefix;

                  tmp_iter = bt.lower_bound( tmp_item );

                  if( ( tmp_iter != bt.end( ) ) && ( tmp_iter->val.find( special_prefix ) == 0 ) )
                     bt.erase( tmp_iter );

                  tmp_item.val = to_string( id.get_num( ) ) + c_special + item_name.substr( 1 );
               }
            }
         }
         else
         {
            o.destroy( id );

            string special_name( to_string( id.get_num( ) ) + c_special + item_name.substr( 1 ) );

            tmp_item.val = special_name;

            tmp_iter = bt.find( tmp_item );

            // NOTE: If has any links then a special link object exists.
            if( tmp_iter != bt.end( ) )
            {
               bt.erase( tmp_iter );

               string link_prefix( to_string( id.get_num( ) ) + c_pipe_separator );

               // NOTE: Now need to delete all existing links to the file.
               while( true )
               {
                  tmp_item.val = link_prefix;

                  tmp_iter = bt.lower_bound( tmp_item );

                  if( ( tmp_iter == bt.end( ) )
                   || ( tmp_iter->val.find( link_prefix ) != 0 ) )
                     break;

                  string link_name( tmp_iter->val.substr( link_prefix.length( ) - 1 ) );

                  bt.erase( tmp_iter );

                  tmp_item.val = link_name;

                  tmp_iter = bt.find( tmp_item );

                  if( tmp_iter != bt.end( ) )
                     bt.erase( tmp_iter );
               }
            }
         }
      }
   }

   return okay;
}

bool ods_file_system::remove_items_for_folder( const string& name, bool ignore_not_found )
{
   bool okay = true;

   btree_type& bt( p_impl->bt );

   btree_type::item_type tmp_item;

   string::size_type pos = name.find( c_folder );

   // NOTE: If name is an absolute path (otherwise assumes a sub-path below the current folder).
   if( pos == 0 )
   {
      string name_1( name );
      string name_2( replaced( name_1, c_folder_separator, c_colon_separator ) );

      string::size_type pos = name_1.rfind( c_folder );

      if( pos == 0 )
         name_2.insert( 1, c_root_folder );
      else if( pos != string::npos )
         name_2[ pos ] = c_folder;

      tmp_item.val = name_1;

      if( !bt.erase( tmp_item ) )
      {
         if( !ignore_not_found )
            throw runtime_error( "folder '" + name + "' not found" );

         okay = false;
      }
      else
      {
         tmp_item.val = name_2;
         bt.erase( tmp_item );
      }
   }
   else
   {
      if( current_folder == string( c_root_folder ) )
      {
         tmp_item.val = current_folder + name;

         if( !bt.erase( tmp_item ) )
         {
            if( !ignore_not_found )
               throw runtime_error( "folder '" + name + "' not found" );

            okay = false;
         }
         else
         {
            if( name.find( c_folder_separator ) == string::npos )
               tmp_item.val = c_colon_separator + current_folder + name;
            else
            {
               string::size_type pos = tmp_item.val.rfind( c_folder );

               replace( tmp_item.val, c_folder_separator, c_colon_separator );

               if( pos != string::npos )
                  tmp_item.val[ pos ] = c_folder;
            }

            bt.erase( tmp_item );
         }
      }
      else
      {
         string name_1( current_folder + c_folder + name );
         string name_2( replaced( name_1, c_folder_separator, c_colon_separator ) );

         string::size_type pos = name_1.rfind( c_folder );

         if( pos != string::npos )
            name_2[ pos ] = c_folder;

         tmp_item.val = name_1;

         if( !bt.erase( tmp_item ) )
         {
            if( !ignore_not_found )
               throw runtime_error( "folder '" + name + "' not found" );

            okay = false;
         }
         else
         {
            tmp_item.val = name_2;
            bt.erase( tmp_item );
         }
      }
   }

   return okay;
}

temporary_force_write::temporary_force_write( ods_file_system& ofs )
 :
 ofs( ofs ),
 old_force_write( ofs.p_impl->force_write )
{
   ofs.p_impl->force_write = true;
}

temporary_force_write::~temporary_force_write( )
{
   ofs.p_impl->force_write = old_force_write;
}

temporary_allow_specials::temporary_allow_specials( ods_file_system& ofs )
 :
 ofs( ofs ),
 old_allow_specials( ofs.p_impl->allow_specials )
{
   ofs.p_impl->allow_specials = true;
}

temporary_allow_specials::~temporary_allow_specials( )
{
   ofs.p_impl->allow_specials = old_allow_specials;
}

temporary_include_hidden::temporary_include_hidden( ods_file_system& ofs, bool include_hidden )
 :
 ofs( ofs ),
 old_skip_hidden( ofs.p_impl->skip_hidden )
{
   ++ofs.p_impl->skip_hidden_count;

   ofs.p_impl->skip_hidden = !include_hidden;
}

temporary_include_hidden::~temporary_include_hidden( )
{
   ofs.p_impl->skip_hidden = old_skip_hidden;

   --ofs.p_impl->skip_hidden_count;
}

void export_objects( ods_file_system& ofs, const string& directory,
 vector< string >* p_rename_expressions, ostream* p_os, progress* p_progress, int level, int start_pos )
{
   vector< string > files;

   if( !level )
   {
      string folder( ofs.get_folder( ) );

      if( folder != string( c_root_folder ) )
         start_pos = folder.length( );
   }

   if( p_os && level )
      *p_os << directory << ofs.get_folder( ) << endl;

   temporary_include_hidden include_hidden( ofs );

   ofs.list_files( files, false );

   string next_directory( directory );

   if( level )
      next_directory += ofs.get_folder( ).substr( start_pos );

   if( !dir_exists( next_directory ) )
      create_dir( next_directory );

   for( size_t i = 0; i < files.size( ); i++ )
   {
      string next( files[ i ] );
      string destination( next );

      if( p_rename_expressions && !p_rename_expressions->empty( ) )
         destination = process_rename_expressions( *p_rename_expressions, ofs.get_folder( ), next );

      if( p_os )
      {
         if( !level )
            *p_os << directory << '/' << next;
         else
            *p_os << directory << ofs.get_folder( ) << '/' << next;

         if( destination != next )
            *p_os << " ==> " << destination;
      }

      string final_destination( directory );

      if( level )
         final_destination += ofs.get_folder( ).substr( start_pos );

      final_destination += '/' + destination;

      ofs.get_file( next, final_destination, 0, p_progress );

      if( p_os )
         *p_os << endl;
   }

   vector< string > folders;

   ofs.list_folders( folders );

   for( size_t i = 0; i < folders.size( ); i++ )
   {
      string next( folders[ i ] );
      string folder( ofs.get_folder( ) );

      ofs.set_folder( next );

      export_objects( ofs, directory, p_rename_expressions, p_os, p_progress, level + 1, start_pos );

      ofs.set_folder( folder );

      string perms;
      int64_t tm_val = 0;

      ofs.has_folder( next, &perms, &tm_val );

      if( !perms.empty( ) )
      {
         string next_dir_name( next_directory + '/' + next );

         file_perms( next_dir_name, perms );

         if( tm_val )
            file_touch( next_dir_name, &tm_val );
      }
   }
}

void import_objects( ods_file_system& ofs, const string& directory,
 vector< string >* p_rename_expressions, ostream* p_os, progress* p_progress, bool force_write )
{
   string start_path( absolute_path( directory ) );

   directory_filter df;
   fs_iterator dfsi( directory, &df );

   bool is_first = true;

   deque< string > folders;
   folders.push_back( start_path );

   string folder( ofs.get_folder( ) );

   vector< string > all_folders;
   set< string > unchanged_folders;

   do
   {
      all_folders.push_back( dfsi.get_path_name( ) );
   } while( dfsi.has_next( ) );

   for( size_t i = 0; i < all_folders.size( ); i++ )
   {
      if( i > 0 )
      {
         string next_folder( all_folders[ i ] );

         string::size_type pos = next_folder.find( folders.back( ) );

         if( pos != 0 )
         {
            while( folders.size( ) > 1 )
            {
               folders.pop_back( );

               ofs.set_folder( ".." );

               if( next_folder.find( folders.back( ) ) == 0 )
                  break;
            }
         }

         string perms( file_perms( next_folder ) );

         int64_t tm_val = last_modification_time( next_folder );

         next_folder.erase( 0, folders.back( ).length( ) + 1 );

         string old_perms;
         int64_t old_tm_val = 0;

         bool changed = false;

         bool has_folder = ofs.has_folder( next_folder, &old_perms, &old_tm_val );

         if( !has_folder )
         {
            changed = true;
            ofs.add_folder( next_folder, 0, &perms, &tm_val );
         }
         else if( force_write || ( perms != old_perms ) || ( tm_val != old_tm_val ) )
         {
            changed = true;
            ofs.replace_folder( next_folder, 0, &perms, &tm_val );
         }

         ofs.set_folder( next_folder );

         folders.push_back( all_folders[ i ] );

         if( !changed )
            unchanged_folders.insert( all_folders[ i ] );
      }

      vector< pair< string, string > > all_files;

      string suffix;
      string next_folder( all_folders[ i ] );

      if( unchanged_folders.count( next_folder ) )
         suffix = string( c_unchanged_suffix );

      if( p_os && ( i > 0 ) )
      {
         if( next_folder.length( ) )
            *p_os << next_folder << '/' << suffix << endl;
      }

      file_filter ff;
      fs_iterator ffsi( next_folder, &ff );

      while( ffsi.has_next( ) )
         all_files.push_back( make_pair( ffsi.get_name( ), ffsi.get_full_name( ) ) );

      for( size_t j = 0; j < all_files.size( ); j++ )
      {
         string name( all_files[ j ].first );
         string source( all_files[ j ].second );

         if( p_rename_expressions && !p_rename_expressions->empty( ) )
            name = process_rename_expressions( *p_rename_expressions, ofs.get_folder( ), name );

         if( p_os )
         {
            if( next_folder.length( ) )
               *p_os << next_folder << '/';

            *p_os << all_files[ j ].first;

            if( name != all_files[ j ].first )
               *p_os << " ==> " << name;
         }

         bool changed = ofs.store_file( name, source, 0, 0, p_progress, force_write );

         if( p_os )
         {
            if( !changed )
               *p_os << c_unchanged_suffix;

            *p_os << endl;
         }
      }
   }

   ofs.set_folder( folder );
}

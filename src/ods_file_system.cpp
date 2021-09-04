// Copyright (c) 2016-2021 CIYAM Developers
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
#include "format.h"
#include "pointers.h"
#include "utilities.h"
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

const char* const c_ods_prefix = "ODS ";

const char* const c_root_folder = "/";
const char* const c_parent_folder = "..";

const char* const c_pipe_separator = "|";
const char* const c_colon_separator = ":";
const char* const c_folder_separator = "/";

const unsigned char c_ofs_items_per_node = 255;

uint16_t c_ofs_object_flag_type_file = 0x8000;
uint16_t c_ofs_object_flag_type_link = 0x4000;

uint16_t c_ofs_object_flag_type_vals = 0xc000;

uint16_t c_ofs_object_maxiumum_val_size = 0x3fff;

struct ofs_object
{
   ofs_object( ) : is_link( false ) { }

   string& str( ) { return val; }
   const string& const_str( ) const { return val; }

   bool get_is_link( ) const { return is_link; }
   void set_is_link( ) { is_link = true; }

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

   oid_pointer< storable_file > o_file;
};

int64_t size_of( const ofs_object& o )
{
   if( o.val.length( ) > c_ofs_object_maxiumum_val_size )
      throw runtime_error( "maximum object length exceeded with ofs_object '" + o.val + "'" );

   return sizeof( uint16_t ) + o.val.length( ) + ( o.o_file.get_id( ).is_new( ) ? 0 : sizeof( oid ) );
}

read_stream& operator >>( read_stream& rs, ofs_object& o )
{
   uint16_t size;

   rs >> size;

   bool has_file = false;

   o.is_link = false;

   if( size & c_ofs_object_flag_type_vals )
   {
      has_file = true;

      if( size & c_ofs_object_flag_type_link )
         o.is_link = true;

      size &= ~c_ofs_object_flag_type_vals;
   }

   o.val.resize( size );

   for( uint16_t i = 0; i < size; i++ )
      rs >> o.val[ i ];

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

   if( !o.o_file.get_id( ).is_new( ) )
   {
      has_file = true;
      size |= c_ofs_object_flag_type_file;

      if( o.is_link )
         size |= c_ofs_object_flag_type_link;
   }

   ws << size;

   size &= ~c_ofs_object_flag_type_vals;

   for( uint16_t i = 0; i < size; i++ )
      ws << o.val[ i ];

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

}

struct ods_file_system::impl
{
   impl( ods& o ) : bt( o ) { }

   btree_type bt;
};

ods_file_system::ods_file_system( ods& o, int64_t i )
 :
 o( o ),
 current_folder( c_root_folder )
{
   p_impl = new impl( o );

   btree_type& bt( p_impl->bt );

   if( o.get_total_entries( ) > i )
   {
      bt.set_id( i );
      o >> bt;
   }
   else
   {
      bt.set_items_per_node( c_ofs_items_per_node );
      o << bt;
   }
}

ods_file_system::~ods_file_system( )
{
   delete p_impl;
}

void ods_file_system::set_folder( const string& new_folder, bool* p_rc )
{
   string s( determine_folder( new_folder ) );

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
   set_folder( string( c_root_folder ) + new_folder, p_rc );
}

string ods_file_system::determine_folder( const string& folder, ostream* p_os, bool explicit_child_only )
{
   string new_folder( folder );
   string old_current_folder( current_folder );

   btree_type& bt( p_impl->bt );

   if( explicit_child_only )
   {
      string::size_type pos = new_folder.find( c_parent_folder );

      if( pos != string::npos || ( !new_folder.empty( ) && new_folder[ 0 ] == c_folder ) )
      {
         if( p_os )
            *p_os << "*** invalid non-explicit or non-child folder ***" << endl;

         new_folder.erase( );
      }
   }

   if( !new_folder.empty( ) )
   {
      string::size_type pos = new_folder.find( c_parent_folder );

      while( pos == 0 )
      {
         string::size_type dpos = old_current_folder.rfind( c_folder_separator );

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

         if( !o.is_bulk_locked( ) )
         {
            ap_bulk.reset( new ods::bulk_read( o ) );

            o >> bt;
         }

         btree_type::item_type tmp_item;

         tmp_item.val = new_folder;

         if( bt.find( tmp_item ) == bt.end( ) )
         {
            if( p_os )
               *p_os << "*** folder '" << folder << "' not found ***" << endl;

            new_folder.erase( );
         }
      }
   }

   return new_folder;
}

string ods_file_system::determine_strip_and_change_folder( string& name, ostream* p_os )
{
   string original_folder;

   string::size_type pos = name.rfind( c_folder_separator );

   if( pos != string::npos )
   {
      string source_folder( determine_folder( name.substr( 0, pos ), p_os ) );

      if( source_folder.empty( ) )
         name.erase( );
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
   bool had_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

   expand_entity_expression( expr, had_wildcard, entity_expr, had_wildcard ? '\0' : c_folder_separator );

   if( expr.empty( ) && current_folder != string( c_root_folder ) )
      entity_expr += string( c_folder_separator );

   if( !entity_expr.empty( ) )
   {
      replace( entity_expr, c_folder_separator, c_colon_separator );

      string::size_type pos = entity_expr.rfind( c_colon );

      if( pos == 0 )
         entity_expr += c_folder;
      else if( pos != string::npos )
         entity_expr[ pos ] = c_folder;

      if( !had_wildcard )
         entity_expr += "*";

      vector< pair< string, string > > search_replaces;
      search_replaces.push_back( make_pair( c_colon_separator, c_folder_separator ) );

      if( full_path )
         search_replaces.push_back( make_pair( "//", c_root_folder ) );

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
   bool full = ( style == e_branch_style_extended );

   string entity_expr( current_folder );

   entity_expr += expr;

   vector< pair< string, string > > search_replaces;
   search_replaces.push_back( make_pair( c_pipe_separator, c_folder_separator ) );

   string prefix_1( current_folder );
   string prefix_2( prefix_1 );

   if( prefix_1.size( ) > 1 )
      prefix_1 += string( c_folder_separator );

   if( entity_expr.find_first_of( "?*" ) == string::npos )
      entity_expr += "*";

   perform_match( os, entity_expr,
    "", 0, &search_replaces, full ? 0 : prefix_1.c_str( ),
    full ? 0 : prefix_2.c_str( ), '\0', brief ? e_file_size_output_type_none
    : ( full ? e_file_size_output_type_num_bytes : e_file_size_output_type_scaled ), "|/" );
}

void ods_file_system::add_file( const string& name, const string& source, ostream* p_os, istream* p_is )
{
   string file_name( source );

   if( file_name.empty( ) )
      file_name = name;

   btree_type& bt( p_impl->bt );

   if( valid_file_name( name ) != name )
   {
      if( p_os )
         *p_os << "*** invalid file name '" << name << "' ***" << endl;
      else
         throw runtime_error( "invalid file name '" + name + "'" );
   }
   else
   {
      string value( current_folder );

      replace( value, c_folder_separator, c_pipe_separator );

      value += string( c_folder_separator ) + name;

      auto_ptr< ods::bulk_write > ap_bulk;

      if( !o.is_bulk_locked( ) )
         ap_bulk.reset( new ods::bulk_write( o ) );

      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      o >> bt;

      auto_ptr< ods::transaction > ap_ods_tx;
      if( !o.is_in_transaction( ) )
         ap_ods_tx.reset( new ods::transaction( o ) );

      btree_trans_type bt_tx( bt );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
      tmp_item.val = current_folder + string( c_folder_separator ) + name;

      tmp_iter = bt.find( tmp_item );

      if( tmp_iter != bt.end( ) )
      {
         if( p_os )
            *p_os << "*** a folder with the name '" << name << "' already exists ***" << endl;
         else
            throw runtime_error( "a folder with the name '" + name + "' already exists" );
      }
      else
#endif
      {
         tmp_item.val = value;
         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
         {
            if( p_os )
               *p_os << "*** file '" << name << "' already exists ***" << endl;
            else
               throw runtime_error( "file '" + name + "' already exists" );
         }
         else
         {
            scoped_ods_instance so( o );

            if( !p_is )
            {
               // NOTE: If the source was "*" or is a zero length file then in order not
               // to waste space on a second object the special zero OID is instead used
               // as an empty file content indicator.
               if( file_name == "*" || ( file_exists( file_name ) && !file_size( file_name ) ) )
                  tmp_item.o_file.set_id( 0 );
               else
                  tmp_item.get_file( new storable_file_extra( file_name ) ).store( );
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

               tmp_item.get_file( new storable_file_extra( file_name, isstr, content.size( ) ) ).store( );
            }

            bt.insert( tmp_item );

            bt_tx.commit( );

            if( ap_ods_tx.get( ) )
               ap_ods_tx->commit( );
         }
      }
   }
}

void ods_file_system::get_file( const string& name,
 const string& destination, ostream* p_os, bool output_to_stream )
{
   string file_name( destination );

   if( file_name.empty( ) )
      file_name = name;

   btree_type& bt( p_impl->bt );

   string value( current_folder );

   replace( value, c_folder_separator, c_pipe_separator );

   value += string( c_folder_separator ) + name;

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   o >> bt;

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   tmp_item.val = value;

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      if( p_os )
         *p_os << "*** file '" << name << "' not found ***" << endl;
      else
         throw runtime_error( "file '" + name + "' not found" );
   }
   else
   {
      tmp_item = *tmp_iter;

      scoped_ods_instance so( o );
      *tmp_item.get_file( new storable_file_extra( file_name, output_to_stream ? p_os : 0 ) );
   }
}

bool ods_file_system::has_file( const string& name )
{
   btree_type& bt( p_impl->bt );

   string value( current_folder );

   replace( value, c_folder_separator, c_pipe_separator );

   value += string( c_folder_separator ) + name;

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   o >> bt;

   btree_type::item_type tmp_item;

   tmp_item.val = value;

   return bt.find( tmp_item ) != bt.end( );
}

void ods_file_system::link_file( const string& name, const string& source, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   if( valid_file_name( name ) != name )
   {
      if( p_os )
         *p_os << "*** invalid file name '" << name << "' ***" << endl;
      else
         throw runtime_error( "invalid file name '" + name + "'" );
   }
   else
   {
      string value( current_folder );

      replace( value, c_folder_separator, c_pipe_separator );

      value += string( c_folder_separator ) + name;

      auto_ptr< ods::bulk_write > ap_bulk;

      if( !o.is_bulk_locked( ) )
         ap_bulk.reset( new ods::bulk_write( o ) );

      o >> bt;

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
      {
         if( p_os )
            *p_os << "*** a folder with the name '" << name << "' already exists ***" << endl;
         else
            throw runtime_error( "a folder with the name '" + name + "' already exists" );
      }
      else
#endif
      {
         tmp_item.val = value;

         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
         {
            if( p_os )
               *p_os << "*** file '" << name << "' already exists ***" << endl;
            else
               throw runtime_error( "file '" + name + "' already exists" );
         }
         else
         {
            string::size_type pos = source.rfind( c_folder );

            string source_name( source.substr( pos == string::npos ? 0 : pos + 1 ) );

            string source_folder( determine_folder( source.substr( 0, pos ) ) );

            if( source_folder.empty( ) )
               source_folder = string( c_root_folder );

            string source_value( source_folder + c_folder_separator + source_name );

            replace( source_value, c_folder_separator, c_pipe_separator );

            string::size_type rpos = source_value.rfind( c_pipe_separator );

            if( rpos != string::npos )
               source_value[ rpos ] = c_folder;

            tmp_item.val = source_value;

            tmp_iter = bt.find( tmp_item );

            if( tmp_iter == bt.end( ) )
            {
               if( p_os )
                  *p_os << "*** file '" << source << "' not found ***" << endl;
               else
                  throw runtime_error( "file '" + source + "' not found" );
            }
            else
            {
               tmp_item.val = value;
               tmp_item.set_is_link( );

               oid id( tmp_iter->get_file( ).get_id( ) );

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
            }
         }
      }
   }
}

void ods_file_system::move_file( const string& name, const string& destination, ostream* p_os )
{
   string dest( destination );

   string::size_type pos = dest.rfind( c_folder );

   string dest_name( dest.substr( pos == string::npos ? 0 : pos + 1 ) );

   btree_type& bt( p_impl->bt );

   if( valid_file_name( dest_name ) != dest_name )
   {
      if( p_os )
         *p_os << "*** invalid destination file name '" << dest_name << "' ***" << endl;
      else
         throw runtime_error( "invalid destination file name '" + dest_name + "'" );
   }
   else
   {
      auto_ptr< ods::bulk_write > ap_bulk;

      if( !o.is_bulk_locked( ) )
         ap_bulk.reset( new ods::bulk_write( o ) );

      o >> bt;

      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      auto_ptr< ods::transaction > ap_ods_tx;
      if( !o.is_in_transaction( ) )
         ap_ods_tx.reset( new ods::transaction( o ) );

      btree_trans_type bt_tx( bt );

      string value( current_folder );
      replace( value, c_folder_separator, c_pipe_separator );

      value += string( c_folder_separator ) + name;

      tmp_item.val = value;
      tmp_iter = bt.find( tmp_item );

      if( tmp_iter == bt.end( ) )
      {
         if( p_os )
            *p_os << "*** file '" << name << "' not found ***" << endl;
         else
            throw runtime_error( "file '" + name + "' not found" );
      }
      else
      {
         tmp_item = *tmp_iter;

         bool is_link = tmp_item.get_is_link( );
         oid id( tmp_item.get_file( ).get_id( ) );

         string dest_folder( determine_folder( dest ) );

         if( dest_folder.empty( ) )
         {
            if( pos == string::npos )
               dest_folder = current_folder;
            else if( pos == 0 )
            {
               dest.erase( 0, 1 );
               dest_folder = string( c_root_folder );
            }
            else
            {
               dest_folder = determine_folder( dest.substr( 0, pos ), p_os );

               if( !dest_folder.empty( ) )
                  dest_name = dest.substr( pos + 1 );
            }
         }
         else
            dest_name = name;

         if( !dest_folder.empty( ) )
         {
            replace( dest_folder, c_folder_separator, c_pipe_separator );

            dest_folder += string( c_folder_separator );

            string full_name( dest_folder + dest_name );

            tmp_item.val = full_name;
            tmp_item.get_file( ).set_id( id );

            if( is_link )
               tmp_item.set_is_link( );

            if( bt.find( tmp_item ) != bt.end( ) )
            {
               if( p_os )
                  *p_os << "*** destination file '" << dest_name << "' already exists ***" << endl;
               else
                  throw runtime_error( "destination file '" + dest_name + "' already exists" );
            }
            else
            {
               bt.erase( tmp_iter );
               bt.insert( tmp_item );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
               replace( tmp_item.val, c_pipe_separator, c_colon_separator );

               tmp_iter = bt.find( tmp_item );

               if( tmp_iter != bt.end( ) )
               {
                  if( p_os )
                     *p_os << "*** a folder with the name '" << dest_name << "' already exists ***" << endl;
                  else
                     throw runtime_error( "a folder with the name '" + dest_name + "' already exists" );
               }
               else
#endif
               {
                  // NOTE: If this is a file link then need to also update its special object.
                  if( is_link )
                  {
                     tmp_item.val = to_string( id.get_num( ) ) + value;

                     tmp_iter = bt.find( tmp_item );

                     if( tmp_iter != bt.end( ) )
                     {
                        tmp_item = *tmp_iter;

                        bt.erase( tmp_iter );
                        tmp_item.val = to_string( id.get_num( ) ) + full_name;

                        bt.insert( tmp_item );
                     }
                  }

                  bt_tx.commit( );

                  if( ap_ods_tx.get( ) )
                     ap_ods_tx->commit( );
               }
            }
         }
      }
   }
}

void ods_file_system::store_file( const string& name,
 const string& source, ostream* p_os, istream* p_is )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   if( !has_file( name ) )
      add_file( name, source, p_os, p_is );
   else
      replace_file( name, source, p_os, p_is );
}

void ods_file_system::remove_file( const string& name, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   o >> bt;

   auto_ptr< ods::transaction > ap_ods_tx;
   if( !o.is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( o ) );

   btree_trans_type bt_tx( bt );

   if( remove_items_for_file( name, p_os ) )
   {
      bt_tx.commit( );

      if( ap_ods_tx.get( ) )
         ap_ods_tx->commit( );
   }
}

void ods_file_system::replace_file( const string& name, const string& source, ostream* p_os, istream* p_is )
{
   string file_name( source );

   if( file_name.empty( ) )
      file_name = name;

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   o >> bt;

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   auto_ptr< ods::transaction > ap_ods_tx;
   if( !o.is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( o ) );

   btree_trans_type bt_tx( bt );

   string value( current_folder );
   replace( value, c_folder_separator, c_pipe_separator );

   value += string( c_folder_separator ) + name;

   tmp_item.val = value;

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      if( p_os )
         *p_os << "*** file '" << name << "' not found ***" << endl;
      else
         throw runtime_error( "file '" + name + "' not found" );
   }
   else
   {
      oid id( tmp_iter->get_file( ).get_id( ) );

      tmp_item.get_file( ).set_id( id );

      scoped_ods_instance so( o );

      if( !p_is )
      {
         bool id_changed = false;

         // NOTE: If the source was "*" or is a zero length file then in order not
         // to waste space on a second object the special zero OID is instead used
         // as an empty file content indicator.
         if( file_name == "*" || ( file_exists( file_name ) && !file_size( file_name ) ) )
         {
            if( id.get_num( ) )
            {
               o.destroy( id );
               id_changed = true;
            }

            tmp_item.get_file( ).set_id( 0 );
         }
         else
         {
            if( !id.get_num( ) )
            {
               id.set_new( );
               id_changed = true;

               tmp_item.get_file( ).set_id( id );
            }

            tmp_item.get_file(
             new storable_file_extra( file_name ) ).store( e_oid_pointer_opt_force_write_skip_read );
         }

         // NOTE: If the OID has changed then need to replace the item.
         if( id_changed )
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
          file_name, isstr, content.size( ) ) ).store( e_oid_pointer_opt_force_write_skip_read );
      }

      bt_tx.commit( );

      if( ap_ods_tx.get( ) )
         ap_ods_tx->commit( );
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

void ods_file_system::add_folder( const string& name, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   if( valid_file_name( name ) != name )
   {
      if( p_os )
         *p_os << "*** invalid folder name '" << name << "' ***" << endl;
      else
         throw runtime_error( "invalid folder name '" + name + "'" );
   }
   else
   {
      auto_ptr< ods::bulk_write > ap_bulk;

      if( !o.is_bulk_locked( ) )
         ap_bulk.reset( new ods::bulk_write( o ) );

      o >> bt;

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
         tmp_item.val = c_pipe + name;
         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
         {
            if( p_os )
               *p_os << "*** a file with the name '" << name << "' already exists ***" << endl;
            else
               throw runtime_error( "a file with the name '" + name + "' already exists" );
         }
         else
#endif
         {
            tmp_item.val = current_folder + name;
            tmp_iter = bt.find( tmp_item );

            if( tmp_iter != bt.end( ) )
            {
               if( p_os )
                  *p_os << "*** folder '" << name << "' already exists ***" << endl;
               else
                  throw runtime_error( "folder '" + name + "' already exists" );
            }
            else
            {
               bt.insert( tmp_item );

               tmp_item.val = c_colon + tmp_item.val;
               bt.insert( tmp_item );

               okay = true;
            }
         }
      }
      else
      {
         string name_1( current_folder + c_folder + name );
         string name_2( replaced( current_folder, c_folder_separator, c_colon_separator ) + c_folder + name );
#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
         string name_x( replaced( current_folder, c_folder_separator, c_pipe_separator ) + c_folder + name );

         tmp_item.val = name_x;
         tmp_iter = bt.find( tmp_item );

         if( tmp_iter != bt.end( ) )
         {
            if( p_os )
               *p_os << "*** a file with the name '" << name << "' already exists ***" << endl;
            else
               throw runtime_error( "a file with the name '" + name + "' already exists" );
         }
         else
#endif
         {
            tmp_item.val = name_1;
            tmp_iter = bt.find( tmp_item );

            if( tmp_iter != bt.end( ) )
            {
               if( p_os )
                  *p_os << "*** folder '" << name << "' already exists ***" << endl;
               else
                  throw runtime_error( "folder '" + name + "' already exists" );
            }
            else
            {
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
      }
   }
}

bool ods_file_system::has_folder( const string& name )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   o >> bt;

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

      btree_type::item_type tmp_item;

      tmp_item.val = full_name;

      return bt.find( tmp_item ) != bt.end( );
   }

   return false;
}

void ods_file_system::move_folder( const string& name, const string& destination, bool overwrite, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   o >> bt;

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
      if( p_os )
         *p_os << "*** folder '" << name << "' not found ***" << endl;
      else
         throw runtime_error( "folder '" + name + "' not found" );
   }
   else
   {
      string dest_folder = determine_folder( destination );

      if( dest_folder.empty( ) )
      {
         if( destination.find( c_folder_separator ) != string::npos )
         {
            if( p_os )
               *p_os << "*** folder '" << destination << "' does not exist ***" << endl;
            else
               throw runtime_error( "folder '" + destination + "' does not exist" );
         }
         else if( valid_file_name( destination ) != destination )
         {
            if( p_os )
               *p_os << "*** invalid folder name '" << destination << "' ***" << endl;
            else
               throw runtime_error( "invalid folder name '" + destination + "'" );
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
            if( p_os )
               *p_os << "*** a folder cannot be moved below itself ***" << endl;
            else
               throw runtime_error( "a folder cannot be moved below itself" );
         }
         else
            okay = move_files_and_folders( full_name, dest_name, src_is_root, dest_is_root, overwrite );
      }

      if( okay )
      {
         bt_tx.commit( );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );
      }
   }
}

void ods_file_system::remove_folder( const string& name, ostream* p_os, bool remove_branch )
{
   bool okay = true;

   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   o >> bt;

   auto_ptr< ods::transaction > ap_ods_tx;
   if( !o.is_in_transaction( ) )
      ap_ods_tx.reset( new ods::transaction( o ) );

   btree_trans_type bt_tx( bt );

   string tmp_folder( determine_folder( name, p_os, true ) );

   if( tmp_folder.empty( ) )
      okay = false;
   else
   {
      restorable< string > tmp_current_folder( current_folder, tmp_folder );

      vector< string > child_folders;
      branch_folders( "", child_folders );

      if( !remove_branch && !child_folders.empty( ) )
      {
         if( p_os )
         {
            okay = false;
            *p_os << "*** cannot remove '" << name << "' as it has one or more child folders ***" << endl;
         }
         else
            throw runtime_error( "cannot remove '" + name + "' as it has one or more child folders" );
      }
      else
      {
         vector< string > child_files;
         branch_files( "", child_files, e_branch_style_brief );

         if( !remove_branch && !child_files.empty( ) )
         {
            if( p_os )
            {
               okay = false;
               *p_os << "*** cannot remove '" << name << "' as it has one or more files ***" << endl;
            }
            else
               throw runtime_error( "cannot remove '" + name + "' as it has one or more files" );
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
      if( remove_items_for_folder( name, p_os ) )
      {
         bt_tx.commit( );

         if( ap_ods_tx.get( ) )
            ap_ods_tx->commit( );
      }
   }
}

void ods_file_system::rebuild_index( )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_write > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_write( o ) );

   ods::transaction tx( o );

   o >> bt;
   bt.build_index_nodes( );

   tx.commit( );
}

void ods_file_system::dump_node_data( const string& file_name, ostream* p_os )
{
   btree_type& bt( p_impl->bt );

   auto_ptr< ods::bulk_read > ap_bulk;

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   o >> bt;

   if( file_name.length( ) )
   {
      ofstream outf( file_name.c_str( ) );
      if( !outf )
      {
         if( p_os )
            *p_os << "error: unable to open file '" << file_name << "' for output" << endl;
         else
            throw runtime_error( "unable to open file '" + file_name + "' for output" );
      }
      else
         bt.dump_all_info( outf );
   }
   else if( p_os )
      bt.dump_all_info( *p_os );
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

   if( !o.is_bulk_locked( ) )
      ap_bulk.reset( new ods::bulk_read( o ) );

   o >> bt;

   btree_type::iterator match_iter;
   btree_type::item_type match_item;

   size_t pos = first_wildcard_character_pos( expr );
   match_item.val = expr.substr( 0, pos );

   size_t minimum_length = match_item.val.length( );

   bool is_folder_expr = false;

   if( pos != string::npos && pos > 0 && expr[ pos - 1 ] == c_folder )
      is_folder_expr = true;

   auto_ptr< regex > ap_regex;

   if( !regexpr.empty( ) )
      ap_regex.reset( new regex( regexpr ) );

#ifdef USE_BULK_PAUSE
   int count = 0;
   int last_tran_id = ap_btree->get_last_tran_id( );
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
   size_t iterations = 0;

   if( match_iter != bt.end( ) )
   {
      btree_type::item_type tmp_item;
      tmp_item.val = expr;

      bool okay = true;

      do
      {
         if( p_range && !p_range->second.empty( ) && match_iter->val >= p_range->second )
            break;

#ifdef USE_BULK_PAUSE
         if( ++iterations % 5000 == 0 )
         {
            btree_type::item_type tmp_item;
            tmp_item.val = match_iter->val;

            bulk.pause( );

            if( last_tran_id != ap_btree->get_last_tran_id( ) )
            {
               o >> bt;
               last_tran_id = ap_btree->get_last_tran_id( );

               match_iter = ap_btree->lower_bound( tmp_item );
               if( match_iter == ap_btree->end( ) )
                  break;
            }
         }
#endif
         if( compare_equal( tmp_item, *match_iter ) )
         {
            string::size_type pos = ap_regex.get( ) ? ap_regex->search( match_iter->val ) : 0;

            if( pos != string::npos )
            {
               if( p_count )
                  ++*p_count;
               else
               {
                  string val( match_iter->val );

                  if( is_folder_expr && ( val.length( ) < minimum_length
                   || ( val.length( ) == minimum_length && val[ val.length( ) - 1 ] == c_folder ) ) )
                     val.erase( );

                  if( p_ignore_with_prefix && val.find( p_ignore_with_prefix ) == 0 )
                     val.erase( );

                  if( !val.empty( ) && p_search_replaces )
                  {
                     for( size_t i = 0; i < p_search_replaces->size( ); i++ )
                        replace( val, ( *p_search_replaces )[ i ].first, ( *p_search_replaces )[ i ].second );
                  }

                  if( p_prefix_1 && val.find( p_prefix_1 ) == 0 )
                     val.erase( 0, strlen( p_prefix_1 ) );
                  else if( p_prefix_2 && val.find( p_prefix_2 ) == 0 )
                     val.erase( 0, strlen( p_prefix_2 ) );

                  if( erase_all_before_and_including )
                  {
                     pos = val.rfind( erase_all_before_and_including,
                      val.length( ) == 1 ? string::npos : val.length( ) - 2 );

                     if( pos != string::npos )
                        val.erase( 0, pos + 1 );
                  }

                  if( !val.empty( ) )
                  {
                     while( p_extra_items && !p_extra_items->empty( ) )
                     {
                        string extra( p_extra_items->front( ) );

                        if( extra <= val )
                        {
                           os << extra << '\n';
                           p_extra_items->pop_front( );
                        }
                        else
                           break;
                     }

                     if( match_iter->get_is_link( ) )
                        val += "*";

                     if( !match_iter->get_file( ).get_id( ).is_new( )
                      && file_size_output != e_file_size_output_type_none )
                     {
                        int64_t size = 0;

                        if( match_iter->get_file( ).get_id( ).get_num( ) )
                           size = o.get_size( match_iter->get_file( ).get_id( ) );

                        if( file_size_output != e_file_size_output_type_scaled )
                           val += " (" + format_int( size ) + ')';
                        else
                           val += " (" + format_bytes( size ) + ')';
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
         os << p_extra_items->front( ) << '\n';
         p_extra_items->pop_front( );
      }
   }
}

void ods_file_system::expand_entity_expression(
 const string& expr, bool had_wildcard, string& entity_expr, const char* p_suffix )
{
   if( !expr.empty( ) )
   {
      // NOTE: If a superflous final folder separator had been provided then remove it.
      if( entity_expr.size( ) > 1
       && entity_expr.substr( entity_expr.length( ) - 1 ) == string( c_folder_separator ) )
         entity_expr.erase( entity_expr.length( ) - 1 );
   }

   string::size_type pos = expr.rfind( c_folder_separator );

   if( pos != string::npos || expr == string( c_parent_folder ) )
   {
      if( pos == 0 )
      {
         if( !had_wildcard || expr == string( c_folder_separator ) )
            entity_expr = expr;
         else
            entity_expr = string( c_folder_separator ) + expr;

         if( entity_expr.size( ) > 1 && p_suffix )
            entity_expr += string( p_suffix );
      }
      else
      {
         // NOTE: For an expr such as: ../.. need to make sure that the trailing parent
         // folder is included.
         if( pos != string::npos && expr.find( c_parent_folder, pos + 1 ) == pos + 1 )
            pos += strlen( c_parent_folder ) + 1;

         entity_expr = determine_folder( expr.substr( 0, pos ) );

         if( entity_expr.size( ) > 1 && p_suffix && pos == string::npos )
            entity_expr += string( p_suffix );

         if( !entity_expr.empty( ) && pos != string::npos )
         {
            entity_expr += expr.substr( pos );

            if( p_suffix )
               entity_expr += string( p_suffix );
         }
      }
   }
   else if( !expr.empty( ) )
   {
      if( !had_wildcard && entity_expr == string( c_folder_separator ) )
         entity_expr += expr;
      else
         entity_expr += string( c_folder_separator ) + expr;

      if( !had_wildcard && p_suffix )
         entity_expr += string( p_suffix );
   }
}

void ods_file_system::get_child_folders(
 const string& expr, bool full, deque< string >& folders, bool append_separator )
{
   bool had_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

   vector< pair< string, string > > search_replaces;

   string folder_expr( current_folder );

   expand_entity_expression( expr, had_wildcard, folder_expr, had_wildcard ? '\0' : c_folder_separator );

   if( expr.empty( ) && current_folder != string( c_root_folder ) )
      folder_expr += string( c_folder_separator );

   replace( folder_expr, c_folder_separator, c_colon_separator );

   string::size_type pos = folder_expr.rfind( c_colon );

   if( pos == 0 )
      folder_expr += c_folder;
   else if( pos != string::npos )
      folder_expr[ pos ] = c_folder;

   if( !had_wildcard )
      folder_expr += "*";

   search_replaces.push_back( make_pair( c_colon_separator, c_folder_separator ) );

   if( full )
      search_replaces.push_back( make_pair( "//", c_root_folder ) );

   ostringstream osstr;

   perform_match( osstr, folder_expr, "",
    0, &search_replaces, 0, 0, full ? '\0' : c_folder );

   split( osstr.str( ), folders, '\n' );

   if( !folders.empty( ) && folders.back( ).empty( ) )
      folders.pop_back( );

   if( append_separator )
   {
      for( size_t i = 0; i < folders.size( ); i++ )
         folders[ i ] += c_folder;
   }
}

void ods_file_system::list_files_or_objects(
 const string& expr, ostream& os, const string& start_from,
 bool objects, list_style style, bool inclusive, size_t limit, bool in_reverse_order )
{
   bool brief = ( style == e_list_style_brief );
   bool full = ( style == e_list_style_extended );

   string entity_expr( current_folder );
   bool had_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

   expand_entity_expression( expr, had_wildcard, entity_expr );

   if( !entity_expr.empty( ) )
   {
      bool is_folder = has_folder( entity_expr )
       || entity_expr == string( c_root_folder );

      bool had_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

      pair< string, string > range;

      if( !start_from.empty( ) )
      {
         range.first = replaced( current_folder,
          c_folder_separator, c_pipe_separator ) + c_folder_separator + start_from;
      }

      deque< string > extras;

      vector< pair< string, string > > search_replaces;

      if( objects )
         get_child_folders( expr, full, extras );

      replace( entity_expr, c_folder_separator, c_pipe_separator );

      if( is_folder && !had_wildcard )
         entity_expr += "/*";
      else
      {
         string::size_type pos = entity_expr.rfind( c_pipe );

         if( pos != string::npos )
            entity_expr[ pos ] = c_folder;
      }

      search_replaces.clear( );
      search_replaces.push_back( make_pair( c_pipe_separator, c_folder_separator ) );

      if( full )
         search_replaces.push_back( make_pair( "//", c_root_folder ) );

      perform_match( os, entity_expr, "", 0, &search_replaces,
       0, 0, full ? '\0' : c_folder, brief ? e_file_size_output_type_none
       : ( full ? e_file_size_output_type_num_bytes : e_file_size_output_type_scaled ),
       0, &extras, &range, inclusive, limit, in_reverse_order );
   }
}

void ods_file_system::branch_files_or_objects( ostream& os, const string& folder,
 const string& expr, branch_style style, bool show_folders, const string* p_start_folder )
{
   bool brief = ( style == e_branch_style_brief );
   bool full = ( style == e_branch_style_extended );

   deque< string > folders;

   bool had_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

   get_child_folders( folder, false, folders, false );

   pair< string, string > range;

   vector< pair< string, string > > search_replaces;
   search_replaces.push_back( make_pair( c_pipe_separator, c_folder_separator ) );

   search_replaces.push_back( make_pair( "//", c_root_folder ) );

   string prefix_1( p_start_folder ? *p_start_folder : folder );
   string prefix_2( prefix_1 );

   if( prefix_1.size( ) > 1 )
      prefix_1 += string( c_folder_separator );

   string entity_expr( folder );

   replace( entity_expr, c_folder_separator, c_pipe_separator );

   entity_expr += c_folder_separator + expr;

   bool is_root_folder = ( folder == string( c_root_folder ) );

   for( size_t i = 0; i < folders.size( ); i++ )
   {
      string next_entity_expr( entity_expr );

      string::size_type pos = next_entity_expr.find_first_of( "?*" );

      string wildcard_expr( "*" );

      if( pos != string::npos )
         wildcard_expr = next_entity_expr.substr( pos );

      if( i == 0 )
      {
         range.first = next_entity_expr.substr( 0, pos );

         if( pos == string::npos )
            next_entity_expr += wildcard_expr;

         if( is_root_folder )
            range.second = range.first;
         else
         {
            pos = range.first.rfind( c_folder );
            range.second = range.first.substr( 0, pos + 1 );
         }
      }
      else
      {
         if( is_root_folder )
            pos = range.first.rfind( c_pipe, range.first.length( ) - 2 );
         else
            pos = range.first.rfind( c_folder, range.first.length( ) - 2 );

         range.second = range.first.substr( 0, ( pos == string::npos ) ? string::npos : pos + 1 );

         if( next_entity_expr.find_first_of( "?*" ) == string::npos )
            next_entity_expr += wildcard_expr;
      }

      if( i > 0 && is_root_folder )
         range.second += c_folder;

      range.second += folders[ i ];

#ifdef DEBUG
      cout << "i = " << i << ", (" << folder << ") "
       << next_entity_expr << " " << range.first << " ==> " << range.second << endl;
#endif

      perform_match( os, next_entity_expr,
       "", 0, &search_replaces, full ? 0 : prefix_1.c_str( ),
       full ? 0 : prefix_2.c_str( ), '\0', brief ? e_file_size_output_type_none
       : ( full ? e_file_size_output_type_num_bytes : e_file_size_output_type_scaled ), 0, 0, &range );

      string next_folder( folder );

      if( folder.size( ) > 1 )
         next_folder += c_folder_separator;

      next_folder += folders[ i ];

      // NOTE: Don't output the folder if it doesn't match the provided expression.
      if( show_folders && wildcard_match( expr, folders[ i ] ) )
      {
         string output( next_folder );

         if( !full )
         {
            string::size_type pos = output.find( current_folder );

            size_t length( current_folder.length( ) );

            if( length > 1 )
               ++length;

            if( pos == 0 )
               output.erase( 0, length );
         }

         if( !output.empty( ) )
            os << output << c_folder_separator << endl;
      }

      branch_files_or_objects( os, next_folder, expr,
       style, show_folders, p_start_folder ? p_start_folder : &folder );

      range.first = range.second + c_folder;
   }

   range.second.erase( );

   string::size_type pos = entity_expr.find_first_of( "?*" );

   if( pos == string::npos )
      entity_expr += "*";

#ifdef DEBUG
   cout << "(" << folder << ") " << entity_expr << " " << range.first << " ==> " << range.second << endl;
#endif

   perform_match( os, entity_expr,
    "", 0, &search_replaces, full ? 0 : prefix_1.c_str( ),
    full ? 0 : prefix_2.c_str( ), '\0', brief ? e_file_size_output_type_none
    : ( full ? e_file_size_output_type_num_bytes : e_file_size_output_type_scaled ), 0, 0, &range );
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
               throw runtime_error( "*** the folder '" + next + "' already exists ***" );
         }
         else
            bt.insert( tmp_item );

         tmp_item.val = full_name;
         tmp_iter = bt.lower_bound( tmp_item );

         if( tmp_iter == bt.end( ) )
            break;
      }

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
         string folder_name( full_name );

         pos = folder_name.rfind( c_colon );

         if( pos != string::npos )
         {
            folder_name[ pos ] = c_folder;

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
            throw runtime_error( "*** a file with the name '"
             + replaced( dest_name, c_colon_separator, c_folder_separator ) + "' already exists ***" );
#endif
      }
      else
      {
         string new_folder_name( dest_name );

         pos = new_folder_name.rfind( c_colon );

         if( pos != string::npos )
         {
            new_folder_name[ pos ] = c_folder;

            tmp_item.val = new_folder_name;

            if( bt.find( tmp_item ) == bt.end( ) )
               bt.insert( tmp_item );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
            replace( new_folder_name, c_colon_separator, c_pipe_separator );
            tmp_item.val = new_folder_name;

            if( bt.find( tmp_item ) != bt.end( ) )
               throw runtime_error( "*** a file with the name '"
                + replaced( dest_name, c_colon_separator, c_folder_separator ) + "' already exists ***" );
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
               throw runtime_error( "*** a file with the name '"
                + replaced( next, c_pipe_separator, c_folder_separator ) + "' already exists ***" );
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
            throw runtime_error( "*** file '"
             + replaced( next, c_pipe_separator, c_folder_separator ) + "' already exists ***" );
         else
         {
            bt.insert( tmp_item );

            string next_name( next );

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
            replace( next, c_pipe_separator, c_colon_separator );
            tmp_item.val = next;

            if( bt.find( tmp_item ) != bt.end( ) )
               throw runtime_error( "*** a folder with the name '"
                + replaced( next, c_colon_separator, c_folder_separator ) + "' already exists ***" );
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

bool ods_file_system::remove_items_for_file( const string& name, ostream* p_os )
{
   bool okay = true;

   btree_type& bt( p_impl->bt );

   btree_type::iterator tmp_iter;
   btree_type::item_type tmp_item;

   if( current_folder == string( c_root_folder ) )
      tmp_item.val = current_folder + name;
   else
      tmp_item.val = current_folder + c_folder_separator + name;

   replace( tmp_item.val, c_folder_separator, c_pipe_separator );

   string::size_type pos = tmp_item.val.rfind( c_pipe_separator );

   if( pos != string::npos )
   {
      tmp_item.val[ pos ] = c_folder;

      if( pos == 0 )
         tmp_item.val = c_pipe + tmp_item.val;
   }

   tmp_iter = bt.find( tmp_item );

   if( tmp_iter == bt.end( ) )
   {
      if( p_os )
      {
         okay = false;
         *p_os << "*** file '" << name << "' not found ***" << endl;
      }
      else
         throw runtime_error( "file '" + name + "' not found" );
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
               bt.erase( tmp_iter );
         }
         else
         {
            o.destroy( id );

            string link_prefix( to_string( id.get_num( ) ) + c_pipe_separator );

            // NOTE: Now need to delete any existing links to the file.
            while( true )
            {
               tmp_item.val = link_prefix;

               tmp_iter = bt.lower_bound( tmp_item );

               if( tmp_iter == bt.end( ) || tmp_iter->val.find( link_prefix ) != 0 )
                  break;

               string link_name = tmp_iter->val.substr( link_prefix.length( ) - 1 );

               bt.erase( tmp_iter );

               tmp_item.val = link_name;

               tmp_iter = bt.find( tmp_item );

               if( tmp_iter != bt.end( ) )
                  bt.erase( tmp_iter );
            }
         }
      }
   }

   return okay;
}

bool ods_file_system::remove_items_for_folder( const string& name, ostream* p_os )
{
   bool okay = true;

   btree_type& bt( p_impl->bt );

   btree_type::item_type tmp_item;

   if( current_folder == string( c_root_folder ) )
   {
      tmp_item.val = current_folder + name;

      if( !bt.erase( tmp_item ) )
      {
         if( p_os )
         {
            okay = false;
            *p_os << "*** folder '" << name << "' not found ***" << endl;
         }
         else
            throw runtime_error( "folder '" + name + "' not found" );
      }
      else
      {
         if( name.find( c_folder_separator ) == string::npos )
            tmp_item.val = c_colon_separator + current_folder + name;
         else
         {
            replace( tmp_item.val, c_folder_separator, c_colon_separator );

            string::size_type pos = tmp_item.val.rfind( c_colon_separator );

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

      string::size_type pos = name_2.rfind( c_colon_separator );

      if( pos != string::npos )
         name_2[ pos ] = c_folder;

      tmp_item.val = name_1;

      if( !bt.erase( tmp_item ) )
      {
         if( p_os )
         {
            okay = false;
            *p_os << "*** folder '" << name << "' not found ***" << endl;
         }
         else
            throw runtime_error( "folder '" + name + "' not found" );
      }
      else
      {
         tmp_item.val = name_2;
         bt.erase( tmp_item );
      }
   }

   return okay;
}


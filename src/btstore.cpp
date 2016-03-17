// Copyright (c) 2000-2015 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2015-2016 CIYAM Developers
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
#define CIYAM_BASE_IMPL

#include "ods.h"
#include "btree.h"
#include "regex.h"
#include "format.h"
#include "pointers.h"
#include "utilities.h"
#include "oid_pointer.h"
#include "storable_file.h"
#include "storable_btree.h"
#include "console_commands.h"

//#define USE_BULK_PAUSE
//#define ALLOW_SAME_FILE_AND_FOLDER_NAMES

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

const char c_folder_separator = '/';

const char* const c_root_folder = "/";
const char* const c_parent_folder = "..";

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

   oid_pointer< storable_file >& get_file( ) { return o_file; }
   const oid_pointer< storable_file >& get_file( ) const { return o_file; }

   bool operator <( const test_item& src ) const { return val < src.val; }
   bool operator ==( const test_item& src ) const { return val == src.val; }

   friend int_t size_of( const test_item& t );

   friend read_stream& operator >>( read_stream& rs, test_item& t );
   friend write_stream& operator <<( write_stream& ws, const test_item& t );

   friend ostream& operator <<( ostream& outf, const test_item& t );
   friend istream& operator >>( istream& outf, test_item& t );

   string val;
   oid_pointer< storable_file > o_file;
};

int_t size_of( const test_item& t )
{
   return sizeof( size_t ) + t.val.length( ) + sizeof( oid );
}

read_stream& operator >>( read_stream& rs, test_item& t )
{
   rs >> t.val;
   rs >> t.o_file;

   return rs;
}

write_stream& operator <<( write_stream& ws, const test_item& t )
{
   ws << t.val;
   ws << t.o_file;

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
    list_iter_limit( 10 ),
    current_folder( c_root_folder )
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

   string current_folder;
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
    btstore_handler( btstore_handler ),
    ap_ods( btstore_handler.ap_ods ),
    list_iter_limit( btstore_handler.list_iter_limit ),
    item( btstore_handler.item ),
    compare_less( btstore_handler.compare_less ),
    compare_equal( btstore_handler.compare_equal ),
    ap_btree( btstore_handler.ap_btree ),
    iter( btstore_handler.iter ),
    current_folder( btstore_handler.current_folder )
   {
      btstore_handler.set_prompt_prefix( current_folder );
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   auto_ptr< ods >& ap_ods;

   btstore_command_handler& btstore_handler;

   void perform_match( ostream& os, const string& expr, const string& regexpr,
    size_t* p_count = 0, vector< pair< string, string > >* p_search_replaces = 0,
    const char* p_prefix_1 = 0, const char* p_prefix_2 = 0,
    const char erase_all_before_and_including = '\0', bool show_file_sizes = false,
    const char* p_ignore_with_prefix = 0 );

   string determine_folder( const string& folder );

   void expand_entity_expression( const string& expr,
    bool had_wildcard, string& entity_expr, const char* p_suffix = 0 );

   int& list_iter_limit;

   btree_type::item_type& item;

   wildcard_compare_less_functor< btree_type::item_type >& compare_less;
   wildcard_compare_equal_functor< btree_type::item_type >& compare_equal;

   auto_ptr< btree_type >& ap_btree;

   btree_type::iterator& iter;

   string& current_folder;
};

void btstore_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_btstore_cd )
   {
      string folder( get_parm_val( parameters, c_cmd_parm_btstore_cd_folder ) );

      if( folder.empty( ) )
         cout << current_folder << endl;
      else
      {
         folder = determine_folder( folder );

         if( !folder.empty( ) )
         {
            current_folder = folder;
            btstore_handler.set_prompt_prefix( current_folder );
         }
      }   
   }
   else if( command == c_cmd_btstore_files || command == c_cmd_btstore_objects )
   {
      bool full( has_parm_val( parameters, c_cmd_parm_btstore_files_full ) );
      bool brief( has_parm_val( parameters, c_cmd_parm_btstore_files_brief ) );
      string expr( get_parm_val( parameters, c_cmd_parm_btstore_files_expr ) );

      string entity_expr( current_folder );
      bool had_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

      expand_entity_expression( expr, had_wildcard, entity_expr );

      if( !entity_expr.empty( ) )
      {
         entity_expr = replace( entity_expr, "/", "|" );

         if( !had_wildcard )
            entity_expr += "/*";
         else
         {
            string::size_type pos = entity_expr.rfind( '|' );

            if( pos != string::npos )
               entity_expr[ pos ] = '/';
         }

         string regexpr( ( command == c_cmd_btstore_objects ) ? "" : ".*[^/]$" );

         vector< pair< string, string > > search_replaces;
         search_replaces.push_back( make_pair( "|", "/" ) );
         search_replaces.push_back( make_pair( "//", "/" ) );

         perform_match( cout, entity_expr, regexpr,
          0, &search_replaces, 0, 0, full ? '\0' : c_folder_separator, !brief );
      }
   }
   else if( command == c_cmd_btstore_branch )
   {
      bool full( has_parm_val( parameters, c_cmd_parm_btstore_branch_full ) );
      bool brief( has_parm_val( parameters, c_cmd_parm_btstore_branch_brief ) );
      string expr( get_parm_val( parameters, c_cmd_parm_btstore_branch_expr ) );

      string entity_expr( current_folder );

      if( !has_parm_val( parameters, c_cmd_parm_btstore_branch_folders ) )
         entity_expr = replace( entity_expr, "/", "|" );

      entity_expr += expr;

      string regexpr( !has_parm_val( parameters, c_cmd_parm_btstore_branch_files ) ? "" : ".*[^/]$" );

      vector< pair< string, string > > search_replaces;
      search_replaces.push_back( make_pair( "|", "/" ) );
      search_replaces.push_back( make_pair( "//", "/" ) );

      string prefix_1( current_folder );
      string prefix_2( prefix_1 );

      if( prefix_1.size( ) > 1 )
         prefix_1 += string( c_root_folder );

      if( entity_expr.find_first_of( "?*" ) == string::npos )
         entity_expr += "*";

      perform_match( cout, entity_expr, regexpr, 0, &search_replaces,
       full ? 0 : prefix_1.c_str( ), full ? 0 : prefix_2.c_str( ), '\0', !brief, "|/" );
   }
   else if( command == c_cmd_btstore_folders )
   {
      bool full( has_parm_val( parameters, c_cmd_parm_btstore_folders_full ) );
      string expr( get_parm_val( parameters, c_cmd_parm_btstore_folders_expr ) );

      string entity_expr( current_folder );
      bool had_wildcard = ( expr.find_first_of( "?*" ) != string::npos );

      expand_entity_expression( expr, had_wildcard, entity_expr, c_root_folder );

      if( !had_wildcard && current_folder != string( c_root_folder ) )
         entity_expr += string( c_root_folder );

      if( !entity_expr.empty( ) )
      {
         entity_expr = replace( entity_expr, "/", ":" );

         if( !had_wildcard )
            entity_expr += "/*";
         else
         {
            string::size_type pos = entity_expr.rfind( ':' );

            if( pos != string::npos )
               entity_expr[ pos ] = '/';
         }

         string regexpr;

         vector< pair< string, string > > search_replaces;
         search_replaces.push_back( make_pair( ":", "/" ) );
         search_replaces.push_back( make_pair( "//", "/" ) );

         perform_match( cout, entity_expr, regexpr,
          0, &search_replaces, 0, 0, full ? '\0' : c_folder_separator );
      }
   }
   else if( command == c_cmd_btstore_file_add )
   {
      string name( get_parm_val( parameters, c_cmd_parm_btstore_file_add_name ) );
      string file_name( get_parm_val( parameters, c_cmd_parm_btstore_file_add_file_name ) );

      if( file_name.empty( ) )
         file_name = name;

      if( valid_file_name( name ) != name )
         cout << "*** invalid file name ***" << endl;
      else
      {
         string value( current_folder );

         if( !value.empty( ) )
            value[ 0 ] = '|';

         value += string( c_root_folder ) + name;

         ods::bulk_write bulk( *ap_ods );

         btree_type::iterator tmp_iter;
         btree_type::item_type tmp_item;

         *ap_ods >> *ap_btree;

#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
         tmp_item.val = value + string( c_root_folder );

         tmp_iter = ap_btree->find( tmp_item );

         if( tmp_iter != ap_btree->end( ) )
            cout << "*** a folder with the name '" << name << "' already exists" << endl;
         else
#endif
         {
            tmp_item.val = value;
            tmp_iter = ap_btree->find( tmp_item );

            if( tmp_iter != ap_btree->end( ) )
               cout << "*** file '" << name << "' already exists" << endl;
            else
            {
               tmp_item.get_file( ).reset( new storable_file( file_name ) );

               scoped_ods_instance so( *ap_ods );
               tmp_item.get_file( ).store( );

               ap_btree->insert( tmp_item );

               if( current_folder == string( c_root_folder ) )
               {
                  tmp_item.val = "|" + name;
                  ap_btree->insert( tmp_item );
               }
            }
         }
      }
   }
   else if( command == c_cmd_btstore_file_get )
   {
      string name( get_parm_val( parameters, c_cmd_parm_btstore_file_get_name ) );
      string file_name( get_parm_val( parameters, c_cmd_parm_btstore_file_get_file_name ) );

      if( file_name.empty( ) )
         file_name = name;

      string value( current_folder );

      if( !value.empty( ) )
         value[ 0 ] = '|';

      value += string( c_root_folder ) + name;

      ods::bulk_read bulk( *ap_ods );

      *ap_ods >> *ap_btree;

      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      tmp_item.val = value;

      tmp_iter = ap_btree->find( tmp_item );

      if( tmp_iter == ap_btree->end( ) )
         cout << "*** file '" << name << "' not found ***" << endl;
      else
      {
         // NOTE: Force the file to be saved by resetting its id as the
         // node may still be cached (in which case the "storable file"
         // wouldn't be saved).
         tmp_item = *tmp_iter;
         tmp_item.get_file( ).set_id( tmp_item.get_file( ).get_id( ) );

         scoped_ods_instance so( *ap_ods );

         ap_ods->set_string( file_name + ':'
          + to_string( ap_ods->get_size( tmp_item.get_file( ).get_id( ) ) ) );

         *tmp_item.get_file( );
      }
   }
   else if( command == c_cmd_btstore_folder_add )
   {
      string name( get_parm_val( parameters, c_cmd_parm_btstore_file_add_name ) );

      if( valid_file_name( name ) != name )
         cout << "*** invalid folder name ***" << endl;
      else
      {
         ods::bulk_write bulk( *ap_ods );

         btree_type::iterator tmp_iter;
         btree_type::item_type tmp_item;

         *ap_ods >> *ap_btree;

         if( current_folder == string( c_root_folder ) )
         {
#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
            tmp_item.val = '|' + name;
            tmp_iter = ap_btree->find( tmp_item );

            if( tmp_iter != ap_btree->end( ) )
               cout << "*** a file with the name '" << name << "' already exists" << endl;
            else
#endif
            {
               tmp_item.val = current_folder + name;
               tmp_iter = ap_btree->find( tmp_item );

               if( tmp_iter != ap_btree->end( ) )
                  cout << "*** folder '" << name << "' already exists" << endl;
               else
               {
                  ap_btree->insert( tmp_item );

                  tmp_item.val = ':' + tmp_item.val;
                  ap_btree->insert( tmp_item );

                  tmp_item.val = '|' + name + '/';
                  ap_btree->insert( tmp_item );

                  tmp_item.val = "|/" + name + '/';
                  ap_btree->insert( tmp_item );
               }
            }
         }
         else
         {
            string name_1( current_folder + '/' + name );
            string name_2( replaced( current_folder, "/", ":" ) + ":/" + name );
            string name_3( replaced( current_folder, "/", "|" ) + '/' + name + '/' );
#ifndef ALLOW_SAME_FILE_AND_FOLDER_NAMES
            string name_x( replaced( current_folder, "/", "|" ) + '/' + name );

            tmp_item.val = name_x;
            tmp_iter = ap_btree->find( tmp_item );

            if( tmp_iter != ap_btree->end( ) )
               cout << "*** a file with the name '" << name << "' already exists" << endl;
            else
#endif
            {
               tmp_item.val = name_1;
               tmp_iter = ap_btree->find( tmp_item );

               if( tmp_iter != ap_btree->end( ) )
                  cout << "*** folder '" << name << "' already exists" << endl;
               else
               {
                  ap_btree->insert( tmp_item );

                  tmp_item.val = name_2;
                  ap_btree->insert( tmp_item );

                  tmp_item.val = name_3;
                  ap_btree->insert( tmp_item );
               }
            }
         }
      }
   }
   else if( command == c_cmd_btstore_file_remove )
   {
      string name( get_parm_val( parameters, c_cmd_parm_btstore_file_remove_name ) );

      ods::bulk_write bulk( *ap_ods );

      btree_type::iterator tmp_iter;
      btree_type::item_type tmp_item;

      *ap_ods >> *ap_btree;
      tmp_item.val = "|" + current_folder + name;

      tmp_iter = ap_btree->find( tmp_item );

      if( tmp_iter == ap_btree->end( ) )
         cout << "*** file '" << name << "' not found" << endl;
      else
      {
         if( !tmp_iter->get_file( ).get_id( ).is_new( ) )
            ap_ods->destroy( tmp_iter->get_file( ).get_id( ) );

         ap_btree->erase( tmp_iter );

         if( current_folder == string( c_root_folder ) )
         {
            tmp_item.val = "|" + name;

            tmp_iter = ap_btree->find( tmp_item );

            if( tmp_iter != ap_btree->end( ) )
               ap_btree->erase( tmp_iter );
         }
      }
   }
   else if( command == c_cmd_btstore_folder_remove )
   {
      string name( get_parm_val( parameters, c_cmd_parm_btstore_folder_remove_name ) );

      ods::bulk_write bulk( *ap_ods );

      btree_type::item_type tmp_item;

      *ap_ods >> *ap_btree;

      if( current_folder == string( c_root_folder ) )
      {
         tmp_item.val = current_folder + name;

         if( !ap_btree->erase( tmp_item ) )
            cout << "*** folder '" << name << "' not found ***" << endl;
         else
         {
            tmp_item.val = ':' + tmp_item.val;
            ap_btree->erase( tmp_item );

            tmp_item.val = '|' + name + '/';
            ap_btree->erase( tmp_item );

            tmp_item.val = "|/" + name + '/';
            ap_btree->erase( tmp_item );
         }
      }
      else
      {
         string name_1( current_folder + '/' + name );
         string name_2( replaced( current_folder, "/", ":" ) + ":/" + name );
         string name_3( replaced( current_folder, "/", "|" ) + '/' + name + '/' );

         tmp_item.val = name_1;

         if( !ap_btree->erase( tmp_item ) )
            cout << "*** folder '" << name << "' not found ***" << endl;
         else
         {
            tmp_item.val = name_2;
            ap_btree->erase( tmp_item );

            tmp_item.val = name_3;
            ap_btree->erase( tmp_item );
         }
      }
   }
   else if( command == c_cmd_btstore_add )
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
      string expr( get_parm_val( parameters, c_cmd_parm_btstore_match_expr ) );
      string regexpr( get_parm_val( parameters, c_cmd_parm_btstore_match_regexpr ) );

      size_t count = 0;

      perform_match( cout, expr, regexpr, command == c_cmd_btstore_match ? 0 : &count );
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

void btstore_command_functor::perform_match(
 ostream& os, const string& expr, const string& regexpr, size_t* p_count,
 vector< pair< string, string > >* p_search_replaces,
 const char* p_prefix_1, const char* p_prefix_2,
 const char erase_all_before_and_including, bool show_file_sizes, const char* p_ignore_with_prefix )
{
   ods::bulk_read bulk( *ap_ods );

   *ap_ods >> *ap_btree;

   btree_type::iterator match_iter;
   btree_type::item_type match_item;

   size_t pos = first_wildcard_character_pos( expr );
   match_item.val = expr.substr( 0, pos );

   size_t minimum_length = match_item.val.length( );

   auto_ptr< regex > ap_regex;

   if( !regexpr.empty( ) )
      ap_regex.reset( new regex( regexpr ) );

#ifdef USE_BULK_PAUSE
   int count = 0;
   int last_tran_id = ap_btree->get_last_tran_id( );
#endif

   match_iter = ap_btree->lower_bound( match_item );

   if( pos != string::npos )
      compare_less.use_truncated_comparison( true );
   else
      compare_less.use_truncated_comparison( false );

   size_t count = 0;
   if( match_iter != ap_btree->end( ) )
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

            tmp_item.val = match_iter->val;

            bulk.pause( );
            if( last_tran_id != ap_btree->get_last_tran_id( ) )
            {
               *ap_ods >> *ap_btree;
               last_tran_id = ap_btree->get_last_tran_id( );

               match_iter = ap_btree->lower_bound( tmp_item );
               if( match_iter == ap_btree->end( ) )
                  break;
            }
         }
#endif
         if( ( !use_boyer_moore && compare_equal( tmp_item, *match_iter ) )
          || ( use_boyer_moore && ap_bm->find( match_iter->const_str( ) ) != string::npos ) )
         {
            string::size_type pos = ap_regex.get( ) ? ap_regex->search( match_iter->val ) : 0;

            if( pos != string::npos )
            {
               if( p_count )
                  ++*p_count;
               else
               {
                  string val( match_iter->val );

                  if( val.length( ) < minimum_length )
                     val.erase( );

                  if( p_ignore_with_prefix && val.find( p_ignore_with_prefix ) == 0 )
                     val.erase( );

                  if( !val.empty( ) && p_search_replaces )
                  {
                     for( size_t i = 0; i < p_search_replaces->size( ); i++ )
                        val = replace( val, ( *p_search_replaces )[ i ].first, ( *p_search_replaces )[ i ].second );
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
                     if( show_file_sizes && !match_iter->get_file( ).get_id( ).is_new( ) )
                        val += " (" + format_bytes( ap_ods->get_size( match_iter->get_file( ).get_id( ) ) ) + ')';

                     os << val << '\n';
                  }
               }
            }
         }
      } while( ++match_iter != ap_btree->end( ) && compare_less( *match_iter, match_item ) );
   }

   if( p_count )
      os << *p_count << endl;

   compare_less.use_truncated_comparison( false );
}

string btstore_command_functor::determine_folder( const string& folder )
{
   string new_folder( folder );
   string old_current_folder( current_folder );

   if( !new_folder.empty( ) )
   {
      string::size_type pos = new_folder.find( c_parent_folder );

      while( pos == 0 )
      {
         string::size_type dpos = old_current_folder.rfind( c_root_folder );

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

      pos = new_folder.find( c_root_folder );

      if( pos != 0 )
      {
         if( old_current_folder == string( c_root_folder ) )
            new_folder = old_current_folder + new_folder;
         else if( new_folder.empty( ) )
            new_folder = old_current_folder;
         else
            new_folder = old_current_folder + string( c_root_folder ) + new_folder;
      }

      if( new_folder != string( c_root_folder ) )
      {
         ods::bulk_read bulk( *ap_ods );

         *ap_ods >> *ap_btree;

         btree_type::item_type tmp_item;

         tmp_item.val = new_folder;

         if( ap_btree->find( tmp_item ) == ap_btree->end( ) )
         {
            cout << "*** folder '" << new_folder << "' not found ***" << endl;
            new_folder.erase( );
         }
      }
   }

   return new_folder;
}

void btstore_command_functor::expand_entity_expression(
 const string& expr, bool had_wildcard, string& entity_expr, const char* p_suffix )
{
   if( !expr.empty( ) )
   {
      // NOTE: If a superflous final folder separator had been provided then remove it.
      if( entity_expr.size( ) > 1
       && entity_expr.substr( entity_expr.length( ) - 1 ) == string( c_root_folder ) )
         entity_expr.erase( entity_expr.length( ) - 1 );
   }

   string::size_type pos = expr.rfind( c_root_folder );

   if( pos != string::npos || expr == string( c_parent_folder ) )
   {
      if( pos == 0 )
      {
         if( !had_wildcard || expr == string( c_root_folder ) )
            entity_expr = expr;
         else
            entity_expr = string( c_root_folder ) + expr;

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
      if( !had_wildcard && entity_expr == string( c_root_folder ) )
         entity_expr += expr;
      else
         entity_expr += string( c_root_folder ) + expr;

      if( !had_wildcard && p_suffix )
         entity_expr += string( p_suffix );
   }
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


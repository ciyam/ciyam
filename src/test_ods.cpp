// Copyright (c) 2000-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2021 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <stack>
#  include <memory>
#  include <vector>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <exception>
#endif

#define USE_CHAR_BUF
#ifdef USE_CHAR_BUF
#  ifndef _LP64
#     define USE_SIZE_PADDING
#  endif
#  define MAX_DESC_LEN 100
#endif

#define CIYAM_BASE_IMPL

#include "ods.h"
#include "format.h"
#include "console.h"
#include "pointers.h"
#include "utilities.h"
#include "oid_pointer.h"
#include "storable_file.h"
#include "console_commands.h"
#include "read_write_stream.h"

using namespace std;

const char* const c_root_node_description = "root";

#include "test_ods.cmh"

const int32_t c_magic = 1234567890;

const int c_max_path_size = 256;
const int c_max_trans_depth = 100;

const char* const c_app_title = "test_ods";
const char* const c_app_version = "0.1";

const char* const c_cmd_password = "p";
const char* const c_cmd_exclusive = "x";
const char* const c_cmd_use_transaction_log = "tlg";

bool g_encrypted = false;
bool g_needs_magic = false;
bool g_shared_write = true;
bool g_use_transaction_log = false;

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

struct temp_read_outline_description;

class outline_base;
typedef storable< outline_base > outline;

class outline_base : public storable_base
{
   friend struct temp_read_outline_description;

   public:
   outline_base( )
    :
#ifdef USE_CHAR_BUF
    description( MAX_DESC_LEN ),
#endif
    do_not_read_children( false ),
    do_not_write_children( false )
   {
#ifdef USE_CHAR_BUF
      description.fill( );
#endif
   }

   outline_base( const char* description )
    :
#ifndef USE_CHAR_BUF
    description( description ),
#else
    description( MAX_DESC_LEN ),
#endif
    do_not_read_children( false ),
    do_not_write_children( false )
   {
#ifdef USE_CHAR_BUF
      this->description.fill( );
      this->description = description;
#endif
   }

   string get_description( ) const
   {
#ifdef USE_CHAR_BUF
      return description.data( );
#else
      return description;
#endif
   }

   void set_description( const string& new_description )
   {
#ifdef USE_CHAR_BUF
      description = new_description.c_str( );
#else
      description = new_description;
#endif
   }

   oid_pointer< storable_file >& get_file( ) { return o_file; }

   oid_pointer< storable_file >& get_file( storable_file_extra* p_extra ) { o_file.set_extra( p_extra ); return o_file; }

   void iter( )
   {
      count = 0;
   }

   bool more( )
   {
      return count < children.size( );
   }

   void next( )
   {
      ++count;
   }

   oid child( )
   {
      return children[ count ];
   }

   void add_child( const outline& child )
   {
      if( children.size( ) == children.capacity( ) )
         children.reserve( children.capacity( ) * 2 );
      children.push_back( child.get_id( ) );
   }

   void del_child( size_t index )
   {
      vector< oid >::iterator iter = children.begin( ) + index;
      children.erase( iter );
      if( index == count )
         count--;
   }

   void del_children( size_t index, long num )
   {
      vector< oid >::iterator first = children.begin( ) + index;
      vector< oid >::iterator final = children.begin( ) + index + num;
      children.erase( first, final );
      if( count >= index && count < index + num )
         count = index - 1;
      else if( count >= index + num )
         count -= num;
   }

   bool has_children( ) const
   {
      return !children.empty( );
   }

   friend int64_t size_of( const outline_base& o );

   friend read_stream& operator >>( read_stream& rs, outline_base& o );
   friend write_stream& operator <<( write_stream& ws, const outline_base& o );

   private:
   size_t count;
   bool do_not_read_children;
   bool do_not_write_children;

#ifdef USE_CHAR_BUF
   char_buffer description;
#else
   string description;
#endif
   oid_pointer< storable_file > o_file;

   vector< oid > children;
};

int64_t size_of( const outline_base& o )
{
   int size_holder = sizeof( size_t );

#ifdef USE_SIZE_PADDING
   // KLUDGE: In order for regression test output to match under both
   // 32 and 64 bit environments dummy padding is added under 32 bit.
   size_holder += sizeof( size_t );
#endif

   if( g_encrypted && ( g_needs_magic || o.get_id( ) == 0 ) )
      size_holder += sizeof( int32_t );

#ifdef USE_CHAR_BUF
   return sizeof( int64_t ) + o.description.length( )
    + sizeof( oid ) + size_holder + ( o.children.size( ) * sizeof( oid ) );
#else
   return sizeof( string::size_type ) + o.description.length( )
    + sizeof( oid ) + size_holder + ( o.children.size( ) * sizeof( oid ) );
#endif
}

read_stream& operator >>( read_stream& rs, outline_base& o )
{
#ifdef USE_CHAR_BUF
   o.description.fill( );
#endif
   o.children.erase( o.children.begin( ), o.children.end( ) );

   // NOTE: If encrypted then use a "magic number" to ensure
   // that the correct password has been used (as ODS itself
   // does not attempt to verify if the password is correct).
   if( g_encrypted && ( g_needs_magic || o.get_id( ) == 0 ) )
   {
      int32_t val;
      rs >> val;

      if( val != c_magic )
         throw runtime_error( "invalid password" );

      g_needs_magic = false;
   }

   rs >> o.description;
   rs >> o.o_file;

   if( !o.do_not_read_children )
      rs >> o.children;
   else
   {
      byte_skip bs( 0 );
      rs >> bs;
   }

   return rs;
}

write_stream& operator <<( write_stream& ws, const outline_base& o )
{
   if( g_encrypted && ( g_needs_magic || o.get_id( ) == 0 ) )
      ws << c_magic;

   ws << o.description;
   ws << o.o_file;

   if( !o.do_not_write_children )
      ws << o.children;

   return ws;
}

struct temp_read_outline_description
{
   temp_read_outline_description( outline& node )
    : node( node )
   {
      val = node.do_not_read_children;
      node.do_not_read_children = true;
   }

   ~temp_read_outline_description( )
   {
      node.do_not_read_children = val;
   }

   private:
   bool val;
   outline& node;
};

struct pathchar_buffer : public char_buffer
{
   pathchar_buffer( ) : char_buffer( c_max_path_size ) { }

   pathchar_buffer( const string& str ) : char_buffer( c_max_path_size )
   {
      operator =( str.c_str( ) );
   }

   void operator =( const char* s ) { char_buffer::operator =( s ); }
};

pathchar_buffer operator +( const char* s, pathchar_buffer& c )
{
   pathchar_buffer buf;
   buf.operator =( s );
   buf.operator +=( c );
   return buf;
}

class test_ods_startup_functor : public command_functor
{
   public:
   test_ods_startup_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& /*parameters*/ )
   {
      if( command == c_cmd_password )
         g_encrypted = true;
      else if( command == c_cmd_exclusive )
         g_shared_write = false;
      else if( command == c_cmd_use_transaction_log )
         g_use_transaction_log = true;
   }
};

class test_ods_command_functor;

class test_ods_command_handler : public console_command_handler
{
   friend class test_ods_command_functor;

   public:
   test_ods_command_handler( )
    :
    trans_level( 0 )
   {
   }

   void init_ods( const char* p_file_name );

   ods& get_ods( ) { return *ap_ods.get( ); }

   outline& get_node( ) { return node; }

   stack< oid, vector< oid > >& get_oid_stack( ) { return oid_stack; }

   vector< pathchar_buffer >& get_path_strings( ) { return path_strings; }

   size_t& get_trans_level( ) { return trans_level; }

   size_t* get_trans_stack_levels( ) { return trans_stack_levels; }

   ods::transaction** get_trans_buffer( ) { return trans_buffer; }

   private:
   auto_ptr< ods > ap_ods;

   outline node;
   stack< oid, vector< oid > > oid_stack;
   vector< pathchar_buffer > path_strings;

   size_t trans_level;
   size_t trans_stack_levels[ c_max_trans_depth ];
   ods::transaction* trans_buffer[ c_max_trans_depth ];
};

void test_ods_command_handler::init_ods( const char* p_file_name )
{
   string password;
   bool not_found = false;

   if( g_encrypted )
      password = get_password( "Password: " );

   ap_ods.reset( new ods( p_file_name, ods::e_open_mode_create_if_not_exist,
    ( g_shared_write ? ods::e_write_mode_shared : ods::e_write_mode_exclusive ),
    g_use_transaction_log, &not_found, password.empty( ) ? 0 : password.c_str( ) ) );

   clear_key( password );

   if( g_encrypted )
      g_needs_magic = true;

   if( not_found )
      throw runtime_error( "unexpected database not found" );
}

class test_ods_command_functor : public command_functor
{
   public:
   test_ods_command_functor( test_ods_command_handler& test_handler )
    : command_functor( test_handler ),
    test_handler( test_handler ),
    o( test_handler.get_ods( ) ),
    node( test_handler.get_node( ) ),
    trans_level( test_handler.get_trans_level( ) ),
    trans_buffer( test_handler.get_trans_buffer( ) ),
    trans_stack_levels( test_handler.get_trans_stack_levels( ) ),
    oid_stack( test_handler.get_oid_stack( ) ),
    path_strings( test_handler.get_path_strings( ) )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   ods& o;
   outline& node;

   size_t& trans_level;
   ods::transaction** trans_buffer;
   size_t* trans_stack_levels;

   stack< oid, vector< oid > >& oid_stack;
   vector< pathchar_buffer >& path_strings;

   test_ods_command_handler& test_handler;
};

void test_ods_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   string::size_type pos;
   outline temp_node;

   node.set_id( oid_stack.top( ) );

   if( command == c_cmd_test_ods_abort )
      exit( 1 );
   else if( command == c_cmd_test_ods_list )
   {
      bool sorted( has_parm_val( parameters, c_cmd_test_ods_list_sorted ) );

      o >> node;

      set< string > folders;
      set< string > file_info;

      temp_read_outline_description tmp_description( temp_node );

      for( node.iter( ); node.more( ); node.next( ) )
      {
         temp_node.set_id( node.child( ) );
         o >> temp_node;

         if( temp_node.get_file( ).get_id( ).is_new( ) )
         {
            if( !sorted )
               handler.issue_command_reponse( temp_node.get_description( ) );
            else
               folders.insert( temp_node.get_description( ) );
         }
         else
         {
            string name( temp_node.get_description( ) );
            name += " (" + format_bytes( o.get_size( temp_node.get_file( ).get_id( ) ) ) + ')';

            if( !sorted )
               handler.issue_command_reponse( name );
            else
               file_info.insert( name );
         }
      }

      if( sorted )
      {
         for( set< string >::iterator i = folders.begin( ); i!= folders.end( ); ++i )
            handler.issue_command_reponse( *i );

         for( set< string >::iterator i = file_info.begin( ); i!= file_info.end( ); ++i )
            handler.issue_command_reponse( *i );
      }
   }
   else if( command == c_cmd_test_ods_in )
   {
      string name_or_path( get_parm_val( parameters, c_cmd_test_ods_in_name_or_path ) );

      bool found = false;
      bool had_error = false;

      string error_name;

      outline next_node( node );
      outline next_child_node;

      stack< oid, vector< oid > > old_oid_stack( oid_stack );
      vector< pathchar_buffer > old_path_strings( path_strings );

      if( !name_or_path.empty( ) && name_or_path[ 0 ] == '/' )
      {
         next_node.set_id( 0 );

         error_name += name_or_path;
         name_or_path.erase( 0, 1 );

         while( oid_stack.size( ) > 1 )
         {
            oid_stack.pop( );
            path_strings.pop_back( );
         }

         if( name_or_path.empty( ) )
            found = true;
      }
      else
      {
         for( size_t i = 0; i < path_strings.size( ); i++ )
            error_name += path_strings[ i ].data( ) + '/';

         error_name += name_or_path;
      }

      vector< string > parts;
      split( name_or_path, parts, '/' );

      for( size_t i = 0; i < parts.size( ); i++ )
      {
         string next_name( parts[ i ] );

         o >> next_node;

         for( next_node.iter( ); next_node.more( ); next_node.next( ) )
         {
            next_child_node.set_id( next_node.child( ) );

            o >> next_child_node;

            if( next_child_node.get_description( ) == next_name )
            {
               if( !next_child_node.get_file( ).get_id( ).is_new( ) )
               {
                  handler.issue_command_reponse( "'" + error_name + "' is a file not a folder" );
                  had_error = true;
                  break;
               }

               next_node.set_id( next_child_node.get_id( ) );

               oid_stack.push( next_child_node.get_id( ) );
               path_strings.push_back( next_child_node.get_description( ) );
               found = true;
               break;
            }
         }

         if( !found || had_error )
            break;
         else if( i < parts.size( ) - 1 )
            found = false;
      }

      if( !found && !had_error )
         handler.issue_command_reponse( "could not find folder: " + error_name );

      if( !found || had_error )
      {
         oid_stack = old_oid_stack;
         path_strings = old_path_strings;
      }
   }
   else if( command == c_cmd_test_ods_out )
   {
      if( oid_stack.size( ) > 1 )
      {
         oid_stack.pop( );
         path_strings.pop_back( );
      }
   }
   else if( command == c_cmd_test_ods_add )
   {
      string name( get_parm_val( parameters, c_cmd_test_ods_add_name ) );

      if( name.find( '/' ) != string::npos )
         handler.issue_command_reponse( "cannot use '"
          + name + "' for a folder name (contains special characters)" );
      else
      {
         ods::bulk_write bulk( o );
         o >> node;

         int num = 1;
         bool is_multi = false;
         pos = name.find( '*' );
         if( pos != string::npos )
         {
            is_multi = true;
            num = atoi( name.substr( pos + 1 ).c_str( ) );
            name = name.substr( 0, pos );
         }

         for( int i = 0; i < num; i++ )
         {
            temp_node.set_new( );
            if( is_multi )
            {
               ostringstream str;
               str << name << "#" << i;
               temp_node.set_description( str.str( ) );
            }
            else
               temp_node.set_description( name );

            o << temp_node;
            node.add_child( temp_node );
         }

         o << node;
      }
   }
   else if( command == c_cmd_test_ods_del )
   {
      string name( get_parm_val( parameters, c_cmd_test_ods_add_name ) );

      bool found = false;
      ods::bulk_write bulk( o );
      o >> node;

      int num = 1;
      size_t index = 0;
      bool is_multi = false;
      pos = name.find( '*' );
      if( pos != string::npos )
      {
         is_multi = true;
         num = atoi( name.substr( pos + 1 ).c_str( ) );
         name = name.substr( 0, pos );
      }

      size_t start = 0;
      long num_deleted = 0;

      int val;
      string str;
      for( node.iter( ); node.more( ); node.next( ), ++index )
      {
         temp_node.set_id( node.child( ) );
         o >> temp_node;
         str = temp_node.get_description( );
         if( is_multi )
         {
            pos = str.find( '#' );
            if( pos != string::npos )
            {
               val = atoi( str.substr( pos + 1 ).c_str( ) );
               str = str.substr( 0, pos );
            }

            if( str == name && val < num )
            {
               if( !temp_node.has_children( ) )
               {
                  found = true;
                  ++num_deleted;
                  if( !temp_node.get_file( ).get_id( ).is_new( ) )
                     o.destroy( temp_node.get_file( ).get_id( ) );
                  o.destroy( temp_node.get_id( ) );
               }
               else if( num_deleted )
               {
                  node.del_children( start, num_deleted );
                  index -= num_deleted;
                  num_deleted = 0;
                  start = index + 1;
               }
            }
            else if( num_deleted )
            {
               node.del_children( start, num_deleted );
               index -= num_deleted;
               num_deleted = 0;
               start = index + 1;
            }
            else
               start = index + 1;
         }
         else if( temp_node.get_description( ) == name )
         {
            found = true;
            if( temp_node.has_children( ) )
            {
               handler.issue_command_reponse( "cannot delete folder with children" );
               break;
            }

            node.del_child( index );

            if( !temp_node.get_file( ).get_id( ).is_new( ) )
               o.destroy( temp_node.get_file( ).get_id( ) );

            o.destroy( temp_node.get_id( ) );
            o << node;

            break;
         }
      }

      if( is_multi )
      {
         if( num_deleted )
            node.del_children( start, num_deleted );
         o << node;

         if( !found )
            handler.issue_command_reponse( "no folders deleted for: " + name );
         else
            o << node;
      }
      else
      {
         if( !found )
            cout << "cannot find folder: " << name << endl;
      }
   }
   else if( command == c_cmd_test_ods_import )
   {
      string file_name( get_parm_val( parameters, c_cmd_test_ods_import_file_name ) );

      ods::bulk_write bulk( o );
      o >> node;

#ifndef _WIN32
      string::size_type pos = file_name.find_last_of( "/" );
#else
      string::size_type pos = file_name.find_last_of( ":/\\" );
#endif
      temp_node.set_new( );

      // NOTE: If a path was specified then don't store it.
      if( pos == string::npos )
         temp_node.set_description( file_name );
      else
         temp_node.set_description( file_name.substr( pos + 1 ) );

      o << temp_node;

      scoped_ods_instance so( o );
      temp_node.get_file( new storable_file_extra( file_name ) ).store( );

      o << temp_node;

      node.add_child( temp_node );
      o << node;
   }
   else if( command == c_cmd_test_ods_export )
   {
      string file_name( get_parm_val( parameters, c_cmd_test_ods_export_file_name ) );
      string output_name( get_parm_val( parameters, c_cmd_test_ods_export_output_name ) );

      if( output_name.empty( ) )
         output_name = file_name;

      o >> node;

      bool found = false;
      string error_extra;
      for( node.iter( ); node.more( ); node.next( ) )
      {
         temp_node.set_id( node.child( ) );
         o >> temp_node;

         if( temp_node.get_description( ) == file_name )
         {
            if( temp_node.get_file( ).get_id( ).is_new( ) )
            {
               error_extra = " ('" + file_name + "' is a folder)";
               break;
            }
            else
            {
               scoped_ods_instance so( o );

               *temp_node.get_file( new storable_file_extra( output_name ) );

               handler.issue_command_reponse( "saved " + output_name
                + " (" + format_bytes( o.get_size( temp_node.get_file( ).get_id( ) ) ) + ")" );

               found = true;
               break;
            }
         }
      }

      if( !found )
         handler.issue_command_reponse( "file '" + file_name + "' not found" + error_extra );
   }
   else if( command == c_cmd_test_ods_trans )
   {
      string label( get_parm_val( parameters, c_cmd_test_ods_trans_label ) );

      if( trans_level < c_max_trans_depth - 1 )
      {
         trans_stack_levels[ trans_level ] = oid_stack.size( );
         trans_buffer[ trans_level++ ] = new ods::transaction( o, label );

         handler.issue_command_reponse( "begin transaction (level = " + to_string( trans_level ) + ")" );
      }
      else
         handler.issue_command_reponse( "cannot exceed max. transaction depth (" + to_string( c_max_trans_depth ) + ")" );
   }
   else if( command == c_cmd_test_ods_commit )
   {
      if( trans_level )
      {
         handler.issue_command_reponse( "commit transaction (level = " + to_string( trans_level ) + ")" );
         trans_buffer[ trans_level - 1 ]->commit( );
         delete trans_buffer[ --trans_level ];
      }
      else
         handler.issue_command_reponse( "no transaction to commit" );
   }
   else if( command == c_cmd_test_ods_rollback )
   {
      if( trans_level )
      {
         handler.issue_command_reponse( "rollback transaction (level = " + to_string( trans_level ) + ")" );
         while( oid_stack.size( ) > trans_stack_levels[ trans_level - 1 ] )
         {
            oid_stack.pop( );
            path_strings.pop_back( );
         }
         delete trans_buffer[ --trans_level ];
      }
      else
         handler.issue_command_reponse( "no transaction to rollback" );
   }
   else if( command == c_cmd_test_ods_trans_id )
   {
      if( !o.get_transaction_level( ) )
         handler.issue_command_reponse( "0" );
      else
         handler.issue_command_reponse( to_string( o.get_transaction_id( ) ) );
   }
   else if( command == c_cmd_test_ods_trans_level )
      handler.issue_command_reponse( to_string( o.get_transaction_level( ) ) );
   else if( command == c_cmd_test_ods_rewind )
   {
      string label_or_txid( get_parm_val( parameters, c_cmd_test_ods_rewind_label_or_txid ) );

      if( g_shared_write )
         handler.issue_command_reponse( "*** must be locked for exclusive write to perform this operation ***" );
      else
         o.rewind_transactions( label_or_txid );
   }
   else if( command == c_cmd_test_ods_compress )
   {
      if( g_shared_write )
         handler.issue_command_reponse( "*** must be locked for exclusive write to perform this operation ***" );
      else
      {
         handler.issue_command_reponse( "moving free data to end..." );
         o.move_free_data_to_end( );
         handler.issue_command_reponse( "completed" );
      }
   }
   else if( command == c_cmd_test_ods_truncate )
   {
      if( g_shared_write )
         handler.issue_command_reponse( "*** must be locked for exclusive write to perform this operation ***" );
      else
         o.truncate_log( );
   }
   else if( command == c_cmd_test_ods_exit )
   {
      while( trans_level )
      {
         handler.issue_command_reponse( "rollback transaction (level = " + to_string( trans_level ) + ")" );
         delete trans_buffer[ --trans_level ];
      }
      test_handler.set_finished( );
   }

   pathchar_buffer path( "/" );
   for( vector< pathchar_buffer >::size_type i = 1; i < path_strings.size( ); i++ )
   {
      if( i > 1 )
         path += "/";
      path += path_strings[ i ];
   }

   test_handler.set_prompt_prefix( string( path.data( ), path.length( ) ) );
}

command_functor* test_ods_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new test_ods_command_functor( dynamic_cast< test_ods_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   test_ods_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         cmd_handler.add_command( c_cmd_password, 1,
          "", "use encryption password", new test_ods_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_exclusive, 1,
          "", "use exclusive write access", new test_ods_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_use_transaction_log, 1,
          "", "use transaction log file", new test_ods_startup_functor( cmd_handler ) );

         processor.process_commands( );

         cmd_handler.remove_command( c_cmd_password );
         cmd_handler.remove_command( c_cmd_exclusive );
         cmd_handler.remove_command( c_cmd_use_transaction_log );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.init_ods( "test_ods" );

      if( cmd_handler.get_ods( ).is_new( ) && !cmd_handler.get_ods( ).is_corrupt( ) )
      {
         outline root( c_root_node_description );

         cmd_handler.get_ods( ) << root;
      }
      else if( !g_shared_write )
         cmd_handler.get_ods( ).repair_if_corrupt( );

      cmd_handler.get_node( ).set_id( 0 );
      cmd_handler.get_ods( ) >> cmd_handler.get_node( );
      cmd_handler.get_oid_stack( ).push( cmd_handler.get_node( ).get_id( ) );
      cmd_handler.get_path_strings( ).push_back( cmd_handler.get_node( ).get_description( ) );

      cmd_handler.set_prompt_prefix( "/" );

      cmd_handler.add_commands( 0,
       test_ods_command_functor_factory, ARRAY_PTR_AND_SIZE( test_ods_command_definitions ) );

      console_command_processor processor( cmd_handler );
      processor.process_commands( );

      return 0;
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
      cerr << "unexpected exception occurred..." << endl;
      return 2;
   }
}


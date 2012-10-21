// Copyright (c) 2000
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifdef __BORLANDC__
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

#include "ods.h"
#include "pointers.h"
#include "utilities.h"
#include "console_commands.h"
#include "read_write_stream.h"

using namespace std;

#define USE_CHAR_BUF
#ifdef USE_CHAR_BUF
#  define MAX_DESC_LEN 50
#endif

const char* const c_root_node_description = "root";

#include "test_ods.cmh"

const int c_max_path_size = 256;
const int c_max_trans_depth = 100;

const char* const c_app_title = "test_ods";
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

struct temp_read_outline_description;

class outline_base;
typedef storable< outline_base > outline;

class outline_base : public storable_base
{
   friend struct temp_read_outline_description;
   friend struct temp_write_outline_description;

   public:
   outline_base( )
    :
#ifdef USE_CHAR_BUF
    description( MAX_DESC_LEN ),
#endif
    only_read_description( false ),
    only_write_description( false )
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
    only_read_description( false ),
    only_write_description( false )
   {
#ifdef USE_CHAR_BUF
      this->description.fill( );
      this->description = description;
#endif
   }

   string get_description( )
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

   bool has_children( )
   {
      return !children.empty( );
   }

   friend int_t size_of( const outline_base& o );

   friend read_stream& operator >>( read_stream& rs, outline_base& o );
   friend write_stream& operator <<( write_stream& ws, const outline_base& o );

   private:
   size_t count;
   bool only_read_description;
   bool only_write_description;

#ifdef USE_CHAR_BUF
   char_buffer description;
#else
   string description;
#endif
   vector< oid > children;
};

int_t size_of( const outline_base& o )
{
#ifdef USE_CHAR_BUF
   return sizeof( int_t ) + o.description.length( )
    + sizeof( size_t ) + ( o.children.size( ) * sizeof( oid ) );
#else
   return sizeof( string::size_type ) + o.description.length( )
    + sizeof( size_t ) + ( o.children.size( ) * sizeof( oid ) );
#endif
}

read_stream& operator >>( read_stream& rs, outline_base& o )
{
#ifdef USE_CHAR_BUF
   o.description.fill( );
#endif
   o.children.erase( o.children.begin( ), o.children.end( ) );

   rs >> o.description;

   if( !o.only_read_description )
      rs >> o.children;
   else
   {
      byte_skip bs( -1 );
      rs >> bs;
   }

   return rs;
}

write_stream& operator <<( write_stream& ws, const outline_base& o )
{
   ws << o.description;

   if( !o.only_write_description )
      ws << o.children;

   return ws;
}

struct temp_read_outline_description
{
   temp_read_outline_description( outline& node )
    : node( node )
   {
      val = node.only_read_description;
      node.only_read_description = true;
   }

   ~temp_read_outline_description( )
   {
      node.only_read_description = val;
   }

   private:
   bool val;
   outline& node;
};

struct temp_write_outline_description
{
   temp_write_outline_description( outline& node )
    : node( node )
   {
      val = node.only_write_description;
      node.only_write_description = true;
   }

   ~temp_write_outline_description( )
   {
      node.only_write_description = val;
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
      if( command == c_cmd_exclusive )
         g_shared_access = false;
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
   ap_ods.reset( new ods( p_file_name, ods::e_open_mode_create_if_not_exist,
    ( g_shared_access ? ods::e_share_mode_shared : ods::e_share_mode_exclusive ) ) );
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
      o >> node;
      temp_read_outline_description tmp_description( temp_node );
      for( node.iter( ); node.more( ); node.next( ) )
      {
         temp_node.set_id( node.child( ) );
         o >> temp_node;
         cout << temp_node.get_description( ) << '\n';
      }
   }
   else if( command == c_cmd_test_ods_in )
   {
      string name( get_parm_val( parameters, c_cmd_parm_test_ods_in_name ) );

      bool found = false;
      o >> node;

      for( node.iter( ); node.more( ); node.next( ) )
      {
         temp_node.set_id( node.child( ) );
         o >> temp_node;
         if( temp_node.get_description( ) == name )
         {
            oid_stack.push( temp_node.get_id( ) );
            path_strings.push_back( temp_node.get_description( ) );
            found = true;
            break;
         }
      }

      if( !found )
         cout << "could not find folder: " << name << endl;
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
      string name( get_parm_val( parameters, c_cmd_parm_test_ods_add_name ) );

      ods::bulk_write bulk( o );
      o >> node;

      // NOTE: If not in a transaction then will need to ensure that no read locks
      // are currently active for this node (before creating any new child nodes).
      // Therefore attempt to re-write the node's description - if it is currently
      // being read then this attempt will fail. The "bulk write" lock itself will
      // prevent any further attempts from commencing until it goes out of scope.
      if( !trans_level )
      {
         temp_write_outline_description tmp_description( temp_node );
         o << node;
      }

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
   else if( command == c_cmd_test_ods_del )
   {
      string name( get_parm_val( parameters, c_cmd_parm_test_ods_add_name ) );

      bool found = false;
      ods::bulk_write bulk( o );
      o >> node;

      // NOTE: (refer to the NOTE in "add" command above)
      if( !trans_level )
      {
         temp_write_outline_description tmp_description( temp_node );
         o << node;
      }

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
               cout << "cannot delete folder with children" << endl;
               break;
            }
            node.del_child( index );
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
            cout << "no folders deleted for: " << name << endl;
         else
            o << node;
      }
      else
      {
         if( !found )
            cout << "cannot find folder: " << name << endl;
      }
   }
   else if( command == c_cmd_test_ods_trans )
   {
      if( trans_level < c_max_trans_depth - 1 )
      {
         trans_stack_levels[ trans_level ] = oid_stack.size( );
         trans_buffer[ trans_level++ ] = new ods::transaction( o );
         cout << "begin transaction (level = " << trans_level << ")" << endl;
      }
      else
         cout << "cannot exceed max. transaction depth (" << c_max_trans_depth << ")" << endl;
   }
   else if( command == c_cmd_test_ods_commit )
   {
      if( trans_level )
      {
         cout << "commit transaction (level = " << trans_level << ")" << endl;
         trans_buffer[ trans_level - 1 ]->commit( );
         delete trans_buffer[ --trans_level ];
      }
      else
         cout << "no transaction to commit" << endl;
   }
   else if( command == c_cmd_test_ods_rollback )
   {
      if( trans_level )
      {
         cout << "rollback transaction (level = " << trans_level << ")" << endl;
         while( oid_stack.size( ) > trans_stack_levels[ trans_level - 1 ] )
         {
            oid_stack.pop( );
            path_strings.pop_back( );
         }
         delete trans_buffer[ --trans_level ];
      }
      else
         cout << "no transaction to rollback" << endl;
   }
   else if( command == c_cmd_test_ods_trans_id )
      cout << o.get_transaction_id( ) << endl;
   else if( command == c_cmd_test_ods_trans_level )
      cout << o.get_transaction_level( ) << endl;
   else if( command == c_cmd_test_ods_compress )
   {
      if( g_shared_access )
         cout << "error: must be locked for exclusive use to perform this operation" << endl;
      else
      {
         cout << "moving free data to end..." << endl;
         o.move_free_data_to_end( );
         cout << "completed" << endl;
      }
   }
   else if( command == c_cmd_test_ods_exit )
   {
      while( trans_level )
      {
         cout << "rollback transaction (level = " << trans_level << ")" << '\n';
         delete trans_buffer[ --trans_level ];
      }
      test_handler.set_finished( );
   }

   pathchar_buffer path;
   for( vector< pathchar_buffer >::size_type i = 0; i < path_strings.size( ); i++ )
      path += "/" + path_strings[ i ];

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

         cmd_handler.add_command( c_cmd_exclusive, 1,
          "", "use ods exclusive file access", new test_ods_startup_functor( cmd_handler ) );

         processor.process_commands( );

         cmd_handler.remove_command( c_cmd_exclusive );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.init_ods( "test_ods" );

      if( cmd_handler.get_ods( ).is_new( ) )
      {
         outline root( c_root_node_description );
         try
         {
            cmd_handler.get_ods( ) << root;
         }
         catch( ods_error& err )
         {
            cout << "error: " << err.what( ) << endl;
            exit( 1 );
         }
      }
      else if( !g_shared_access )
         cmd_handler.get_ods( ).rollback_dead_transactions( );

      cmd_handler.get_node( ).set_id( 0 );
      cmd_handler.get_ods( ) >> cmd_handler.get_node( );
      cmd_handler.get_oid_stack( ).push( cmd_handler.get_node( ).get_id( ) );
      cmd_handler.get_path_strings( ).push_back( cmd_handler.get_node( ).get_description( ) );

      string path( "/" );
      path += string( cmd_handler.get_path_strings( ).back( ).data( ) );
      cmd_handler.set_prompt_prefix( path );

      cmd_handler.add_commands( 0,
       test_ods_command_functor_factory, ARRAY_PTR_AND_SIZE( test_ods_command_definitions ) );

      console_command_processor processor( cmd_handler );
      processor.process_commands( );

      return 0;
   }
   catch( ods_error& err )
   {
      if( !g_application_title_called && !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;
      cerr << "ods error: " << err.what( ) << endl;
      return 1;
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


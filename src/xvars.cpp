// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "sio.h"
#include "utilities.h"

using namespace std;

const char* const c_node_name_targets = "targets";

const char* const c_node_name_libs = "libs";
const char* const c_node_name_dyn_libs = "dyn_libs";
const char* const c_node_name_executables = "executables";

const char* const c_node_name_lib = "lib";
const char* const c_node_name_dyn_lib = "dyn_lib";
const char* const c_node_name_executable = "executable";

const char* const c_node_name_cpp_files = "cpp_files";
const char* const c_node_name_cms_files = "cms_files";

const char* const c_attribute_name = "name";
const char* const c_attribute_include = "include";
const char* const c_attribute_gen_ext = "gen_ext";
const char* const c_attribute_threads = "threads";
const char* const c_attribute_sockets = "sockets";
const char* const c_attribute_openssl = "openssl";
const char* const c_attribute_libfcgi = "libfcgi";
const char* const c_attribute_libharu = "libharu";
const char* const c_attribute_libicnv = "libicnv";
const char* const c_attribute_mysqldb = "mysqldb";
const char* const c_attribute_zlibuse = "zlibuse";
const char* const c_attribute_dynamic = "dynamic";
const char* const c_attribute_readline = "readline";
const char* const c_attribute_link_libs = "link_libs";
const char* const c_attribute_dlink_libs = "dlink_libs";

const char* const c_attribute_filename = "filename";

const char* const c_true = "true";

void verify_node_name( const section_node& node, const string& name_expected )
{
   if( node.get_name( ) != name_expected )
      throw runtime_error( "expecting node named '" + name_expected + "' but instead found node named '" + node.get_name( ) + "'" );
}

void verify_num_children( const section_node& node, size_t num_children_expected )
{
   if( node.get_num_child_nodes( ) != num_children_expected )
      throw runtime_error( "expecting " + to_string( num_children_expected ) + " children for node '"
       + node.get_name( ) + "' but instead found " + to_string( node.get_num_child_nodes( ) ) + " children" );
}

void verify_attribute_name( const attribute& attr, const string& name_expected )
{
   if( attr.get_name( ) != name_expected )
      throw runtime_error( "expecting attribute named '"
       + name_expected + "' but found attribute named '" + attr.get_name( ) + "'" );
}

void generate_lib_vars( const section_node& libs_node, ostream& os, bool output_header )
{
   if( output_header )
      os << "`{`$libs`=`'";

   for( size_t i = 0; i < libs_node.get_num_child_nodes( ); i++ )
   {
      const section_node& lib_node = libs_node.get_child_node( i );
      verify_node_name( lib_node, c_node_name_lib );

      if( output_header )
      {
         if( i > 0 )
            os << "\\\n ";
         os << lib_node.get_attribute_value( c_attribute_name );
      }
      else
      {
         os << "`{`}\n";

         if( lib_node.has_attribute( c_attribute_include ) )
            os << "`{`#" << lib_node.get_attribute_value( c_attribute_include ) << " @`}\n";
         else
         {
            os << "`{`$" << lib_node.get_attribute_value( c_attribute_name ) << "_cpps`=`'";

            verify_num_children( lib_node, 1 );

            const section_node& cpp_files_node = lib_node.get_child_node( 0 );
            verify_node_name( cpp_files_node, c_node_name_cpp_files );

            for( size_t j = 0; j < cpp_files_node.get_num_attributes( ); j++ )
            {
               if( j > 0 )
                  os << "\\\n ";
               os << cpp_files_node.get_attribute( j ).get_value( );
            }
            os << "`'`}\n";
         }
      }
   }

   if( output_header )
      os << "`'`}\n";
}

void generate_dyn_lib_vars( const section_node& dyn_libs_node, ostream& os, bool output_header )
{
   if( output_header )
      os << "`{`$dyn_libs`=`'";

   set< string > dlink_targets;
   for( size_t i = 0; i < dyn_libs_node.get_num_child_nodes( ); i++ )
   {
      const section_node& dyn_lib_node = dyn_libs_node.get_child_node( i );
      verify_node_name( dyn_lib_node, c_node_name_dyn_lib );

      if( output_header )
      {
         if( i > 0 )
            os << "\\\n ";
         os << dyn_lib_node.get_attribute_value( c_attribute_name );
      }
      else
      {
         os << "`{`}\n";
         string dyn_lib_name( dyn_lib_node.get_attribute_value( c_attribute_name ) );

         if( dyn_lib_node.has_attribute( c_attribute_include ) )
            os << "`{`#" << dyn_lib_node.get_attribute_value( c_attribute_include ) << " @`}\n";
         else
         {
            os << "`{`$" << dyn_lib_name << "_cpps`=`'";

            verify_num_children( dyn_lib_node, 2 );

            const section_node& cpp_files_node = dyn_lib_node.get_child_node( 0 );
            verify_node_name( cpp_files_node, c_node_name_cpp_files );

            for( size_t j = 0; j < cpp_files_node.get_num_attributes( ); j++ )
            {
               verify_attribute_name( cpp_files_node.get_attribute( j ), c_attribute_filename );

               if( j > 0 )
                  os << "\\\n ";
               os << cpp_files_node.get_attribute( j ).get_value( );
            }
            os << "`'`}\n";

            const section_node& cms_files_node = dyn_lib_node.get_child_node( 1 );
            verify_node_name( cms_files_node, c_node_name_cms_files );

            if( cms_files_node.get_num_attributes( ) )
            {
               os << "`{`$" << dyn_lib_name << "_cmss`=`'";

               for( size_t j = 0; j < cms_files_node.get_num_attributes( ); j++ )
               {
                  verify_attribute_name( cms_files_node.get_attribute( j ), c_attribute_filename );

                  if( j > 0 )
                     os << "\\\n ";

                  os << cms_files_node.get_attribute( j ).get_value( );
               }
               os << "`'`}\n";
            }
         }

         if( dyn_lib_node.get_attribute_value( c_attribute_sockets ) == c_true )
            os << "`{`$" << dyn_lib_name << "_issk`=`'1`'`}\n";

         if( dyn_lib_node.get_attribute_value( c_attribute_openssl ) == c_true )
            os << "`{`$" << dyn_lib_name << "_ossl`=`'1`'`}\n";

         if( dyn_lib_node.get_attribute_value( c_attribute_libharu ) == c_true )
            os << "`{`$" << dyn_lib_name << "_haru`=`'1`'`}\n";

         if( dyn_lib_node.get_attribute_value( c_attribute_libicnv ) == c_true )
            os << "`{`$" << dyn_lib_name << "_icnv`=`'1`'`}\n";

         if( dyn_lib_node.get_attribute_value( c_attribute_mysqldb ) == c_true )
            os << "`{`$" << dyn_lib_name << "_msql`=`'1`'`}\n";

         if( dyn_lib_node.get_attribute_value( c_attribute_zlibuse ) == c_true )
            os << "`{`$" << dyn_lib_name << "_zlib`=`'1`'`}\n";

         if( dyn_lib_node.get_attribute_value( c_attribute_readline ) == c_true )
            os << "`{`$" << dyn_lib_name << "_isrd`=`'1`'`}\n";

         string link_libs( dyn_lib_node.get_attribute_value( c_attribute_link_libs ) );
         if( !link_libs.empty( ) )
            os << "`{`$" << dyn_lib_name << "_libs`=`'" << link_libs << "`'`}\n";

         string dlink_libs( dyn_lib_node.get_attribute_value( c_attribute_dlink_libs ) );
         if( !dlink_libs.empty( ) )
         {
            os << "`{`$" << dyn_lib_name << "_dylk`=`'" << dlink_libs << "`'`}\n";

            vector< string > next_dlink_targets;
            split( dlink_libs, next_dlink_targets );

            for( size_t j = 0; j < next_dlink_targets.size( ); j++ )
               dlink_targets.insert( next_dlink_targets[ j ] );
         }
      }
   }

   if( output_header )
      os << "`'`}\n";
   else if( !dlink_targets.empty( ) )
   {
      bool is_first = true;
      os << "`{`}\n";
      os << "`{`$dyn_dlinks`=`'";
      for( set< string >::iterator i = dlink_targets.begin( ), end = dlink_targets.end( ); i != end; ++i )
      {
         if( is_first )
            is_first = false;
         else
            os << ' ';
         os << *i;
      }
      os << "`'`}\n";
   }
}

void generate_executable_vars( const section_node& executables_node, ostream& os, bool output_header )
{
   if( output_header )
      os << "`{`$executables`=`'";

   set< string > dlink_targets;
   for( size_t i = 0; i < executables_node.get_num_child_nodes( ); i++ )
   {
      const section_node& executable_node = executables_node.get_child_node( i );
      verify_node_name( executable_node, c_node_name_executable );

      if( output_header )
      {
         if( i > 0 )
            os << "\\\n ";
         os << executable_node.get_attribute_value( c_attribute_name );
      }
      else
      {
         os << "`{`}\n";
         string executable_name( executable_node.get_attribute_value( c_attribute_name ) );

         if( executable_node.has_attribute( c_attribute_include ) )
            os << "`{`#" << executable_node.get_attribute_value( c_attribute_include ) << " @`}\n";
         else
         {
            os << "`{`$" << executable_name << "_cpps`=`'";

            verify_num_children( executable_node, 2 );

            const section_node& cpp_files_node = executable_node.get_child_node( 0 );
            verify_node_name( cpp_files_node, c_node_name_cpp_files );

            for( size_t j = 0; j < cpp_files_node.get_num_attributes( ); j++ )
            {
               verify_attribute_name( cpp_files_node.get_attribute( j ), c_attribute_filename );

               if( j > 0 )
                  os << "\\\n ";
               os << cpp_files_node.get_attribute( j ).get_value( );
            }
            os << "`'`}\n";

            const section_node& cms_files_node = executable_node.get_child_node( 1 );
            verify_node_name( cms_files_node, c_node_name_cms_files );

            if( cms_files_node.get_num_attributes( ) )
            {
               os << "`{`$" << executable_name << "_cmss`=`'";

               for( size_t j = 0; j < cms_files_node.get_num_attributes( ); j++ )
               {
                  verify_attribute_name( cms_files_node.get_attribute( j ), c_attribute_filename );

                  if( j > 0 )
                     os << "\\\n ";

                  os << cms_files_node.get_attribute( j ).get_value( );
               }
               os << "`'`}\n";
            }
         }

         if( executable_node.get_attribute_value( c_attribute_threads ) == c_true )
            os << "`{`$" << executable_name << "_ismt`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_sockets ) == c_true )
            os << "`{`$" << executable_name << "_issk`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_openssl ) == c_true )
            os << "`{`$" << executable_name << "_ossl`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_libfcgi ) == c_true )
            os << "`{`$" << executable_name << "_fcgi`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_libharu ) == c_true )
            os << "`{`$" << executable_name << "_haru`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_libicnv ) == c_true )
            os << "`{`$" << executable_name << "_icnv`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_mysqldb ) == c_true )
            os << "`{`$" << executable_name << "_msql`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_zlibuse ) == c_true )
            os << "`{`$" << executable_name << "_zlib`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_dynamic ) == c_true )
            os << "`{`$" << executable_name << "_dyld`=`'1`'`}\n";

         if( executable_node.get_attribute_value( c_attribute_readline ) == c_true )
            os << "`{`$" << executable_name << "_isrd`=`'1`'`}\n";

         string link_libs( executable_node.get_attribute_value( c_attribute_link_libs ) );
         if( !link_libs.empty( ) )
            os << "`{`$" << executable_name << "_libs`=`'" << link_libs << "`'`}\n";

         string dlink_libs( executable_node.get_attribute_value( c_attribute_dlink_libs ) );
         if( !dlink_libs.empty( ) )
         {
            os << "`{`$" << executable_name << "_dylk`=`'" << dlink_libs << "`'`}\n";

            vector< string > next_dlink_targets;
            split( dlink_libs, next_dlink_targets );

            for( size_t j = 0; j < next_dlink_targets.size( ); j++ )
               dlink_targets.insert( next_dlink_targets[ j ] );
         }

         string gen_ext( executable_node.get_attribute_value( c_attribute_gen_ext ) );
         if( !gen_ext.empty( ) )
            os << "`{`$" << executable_name << "_gext`=`'" << gen_ext << "`'`}\n";
      }
   }

   if( output_header )
      os << "`'`}\n";
   else if( !dlink_targets.empty( ) )
   {
      bool is_first = true;
      os << "`{`}\n";
      os << "`{`$exe_dlinks`=`'";
      for( set< string >::iterator i = dlink_targets.begin( ), end = dlink_targets.end( ); i != end; ++i )
      {
         if( is_first )
            is_first = false;
         else
            os << ' ';
         os << *i;
      }
      os << "`'`}\n";
   }
}

void generate_makefile_vars( istream& is, ostream& os )
{
   sio_reader reader( is );
   sio_graph graph( reader );

   if( graph.empty( ) )
      throw runtime_error( "unexpected empty graph" );

   const section_node& root_node = graph.get_root_node( );
   verify_num_children( root_node, 1 );

   const section_node& targets_node = root_node.get_child_node( 0 );
   verify_node_name( targets_node, c_node_name_targets );
   verify_num_children( targets_node, 3 );

   const section_node& libs_node = targets_node.get_child_node( 0 );
   verify_node_name( libs_node, c_node_name_libs );

   const section_node& dyn_libs_node = targets_node.get_child_node( 1 );
   verify_node_name( dyn_libs_node, c_node_name_dyn_libs );

   const section_node& executables_node = targets_node.get_child_node( 2 );
   verify_node_name( executables_node, c_node_name_executables );

   generate_lib_vars( libs_node, os, true );
   generate_dyn_lib_vars( dyn_libs_node, os, true );
   generate_executable_vars( executables_node, os, true );

   os << "`{`}\n";
   os << "`{`$all_targets`=`$libs`+`$dyn_libs`+`$executables`}\n";

   generate_lib_vars( libs_node, os, false );
   generate_dyn_lib_vars( dyn_libs_node, os, false );
   generate_executable_vars( executables_node, os, false );

   os << "`{`}\n";
   os << "`{`$all_dlinks`=`'`'`}\n";
   os << "`{`(`?`$dyn_dlinks`)`$all_dlinks`=`$all_dlinks`+`$dyn_dlinks`}\n";
   os << "`{`(`?`$exe_dlinks`)`$all_dlinks`=`$all_dlinks`+`$exe_dlinks`}\n";

   os << "`{`$dlink_targets`=`'`'`}\n";
   os << "`{`!`@eq`(`$all_dlinks`,`'`'`)`[`$all_dlinks`,\\\n";
   os << "`{`!`@in`(`'\\$`'`,`$all_targets`)`$dlink_targets`=`$dlink_targets`+`'\\$`'`}`]`}\n";
}

int main( )
{
   try
   {
      ifstream inpf( "makefile.sio" );
      if( !inpf )
         throw runtime_error( "unable to open file 'makefile.sio' for input" );

      generate_makefile_vars( inpf, cout );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "unexpected exception occurred..." << endl;
      return 2;
   }
}


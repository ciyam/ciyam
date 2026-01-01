// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cassert>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "module_interface.h"

using namespace std;

string& get_module_directory( )
{
   static string g_md;
   return g_md;
}

module_strings_container& get_module_strings( )
{
   static module_strings_container g_ms;
   return g_ms;
}

class_registry_container& get_class_registry( )
{
   static class_registry_container g_cr;
   return g_cr;
}

void init_dir( const char* p_dir )
{
   get_module_directory( ) = p_dir;
}

bool load_strings( const char* p_name )
{
   assert( p_name != 0 );

   try
   {
      string file_name( p_name );
      file_name += ".txt";

      string directory( get_module_directory( ) );
      if( !directory.empty( ) )
         file_name = directory + file_name;

      ifstream inpf( file_name.c_str( ) );

      if( !inpf )
         throw runtime_error( "unable to open module string file '" + file_name + "' for input" );

      string next_line;
      bool is_first_line = true;
      while( getline( inpf, next_line ) )
      {
         remove_trailing_cr_from_text_file_line( next_line, is_first_line );

         if( is_first_line )
            is_first_line = false;

         if( next_line.empty( ) )
            continue;

         string::size_type pos = next_line.find( ' ' );
         if( pos == string::npos )
            throw runtime_error( "unexpected string file line '" + next_line + "'" );

         string key( next_line.substr( 0, pos ) );

         size_t npos = pos + 1;
         while( npos < next_line.size( ) && next_line[ npos ] == ' ' )
            ++npos;
         next_line.erase( 0, npos );

         while( !next_line.empty( ) && next_line[ next_line.size( ) - 1 ] == ' ' )
            next_line.erase( next_line.size( ) - 1 );

         if( next_line.empty( ) || next_line[ 0 ] != '"' || next_line[ next_line.size( ) - 1 ] != '"' )
            throw runtime_error( "unexpected string format '" + next_line + "'" );

         string data( next_line.substr( 1, next_line.length( ) - 2 ) );
         get_module_strings( ).insert( module_strings_value_type( key, data ) );
      }

      if( !inpf.eof( ) )
         throw runtime_error( "unexpected error occurred whilst reading '" + file_name + "' for input" );

      return true;
   }
   catch( ... )
   {
      return false;
   }
}

void init_classes( const char* p_name )
{
   assert( p_name != 0 );

   class_registry_const_iterator crci;
   for( crci = get_class_registry( ).begin( ); crci != get_class_registry( ).end( ); ++crci )
      crci->second->class_init( p_name );
}

void term_classes( const char* p_name )
{
   assert( p_name != 0 );

   class_registry_const_iterator crci;
   for( crci = get_class_registry( ).begin( ); crci != get_class_registry( ).end( ); ++crci )
      crci->second->class_term( p_name );
}

void create_class_object( const char* p_name, class_base*& p_object )
{
   class_registry_const_iterator crci = get_class_registry( ).find( p_name );
   if( crci == get_class_registry( ).end( ) )
      p_object = 0;
   else
      p_object = crci->second->create_instance( );
}

void destroy_class_object( const char* p_name, class_base*& p_object )
{
   class_registry_const_iterator crci = get_class_registry( ).find( p_name );
   if( crci != get_class_registry( ).end( ) )
   {
      crci->second->destroy_instance( p_object );
      p_object = 0;
   }
}

void obtain_class_registry( const class_registry_container*& p_class_registry )
{
   p_class_registry = &get_class_registry( );
}

void obtain_module_strings( const module_strings_container*& p_module_strings )
{
   p_module_strings = &get_module_strings( );
}

string get_module_string( const string& key, pair< string, string >* p_next )
{
   string str( key );
   const module_strings_container& msc( get_module_strings( ) );

   module_strings_const_iterator msci = msc.find( key );
   if( msci != msc.end( ) )
   {
      str = msci->second;

      if( p_next && ++msci != msc.end( ) )
      {
         p_next->first = msci->first;
         p_next->second = msci->second;
      }
   }

   return str;
}


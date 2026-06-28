// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory>
#  include <fstream>
#  include <iostream>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#include "sio.h"
#include "config.h"
#include "macros.h"
#include "console.h"
#include "utilities.h"

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
extern "C"
{
#     include <readline/history.h>
}
#  endif
#endif

//#define DEBUG

using namespace std;

int main( int argc, char* argv[ ] )
{
   try
   {
#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
      if( isatty( STDIN_FILENO ) )
         using_history( );
#  endif
#endif
      string cmd, next;

      unique_ptr< sio_graph > up_sio_graph;

      while( cin )
      {
         next = get_line( "\n> " );

         if( next.empty( ) )
            continue;
#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
         if( isatty( STDIN_FILENO ) )
            add_history( next.c_str( ) );
#  endif
#endif
         string::size_type pos = next.find( ' ' );

         cmd = next.substr( 0, pos );

         if( pos == string::npos )
            next.erase( );
         else
            next.erase( 0, pos + 1 );

         if( cmd == "?" )
         {
            cout << "commands:\n";
            cout << "=========\n";
            cout << "load <file>\n";
            cout << "save <file>\n";
            cout << "sections [<path>]\n";
            cout << "attribute [<path>]<name>\n";
            cout << "attributes [<path>]\n";
            cout << "output_json [<path>]\n";
            cout << "exit\n";
         }
         else if( cmd == "load" )
         {
            if( next.empty( ) )
               cout << "load needs an sio file name" << endl;
            else
            {
               ifstream inpf( next.c_str( ) );

               sio_reader reader( inpf );

               up_sio_graph.reset( new sio_graph( reader ) );
            }
         }
         else if( cmd == "save" )
         {
            if( !up_sio_graph.get( ) )
               cerr << "no sio content exists to save" << endl;
            else
            {
               if( next.empty( ) )
                  write_graph( *up_sio_graph );
               else
               {
                  ofstream outf( next.c_str( ) );

                  write_graph( *up_sio_graph, &outf );
               }
            }
         }
         else if( cmd == "sections" )
         {
            if( up_sio_graph.get( ) )
            {
               const section_node* p_section_node = get_section_node_from_path( up_sio_graph->get_root_node( ), next );

               if( !p_section_node )
                  cerr << "unable to find section '" << next << "'" << endl;
               else
               {
                  size_t num_children = p_section_node->get_num_child_nodes( );

                  for( size_t i = 0; i < num_children; i++ )
                     cout << to_comparable_string( i, false, 5 ) << ' ' << p_section_node->get_child_node( i ).get_name( ) << '\n';
               }
            }
         }
         else if( cmd == "attribute" )
         {
            if( !up_sio_graph.get( ) )
               cerr << "no sio content is available" << endl;
            else
            {
               string section;

               bool is_range = false;

               size_t range_offset = 0;

               const section_node* p_range_node = 0;

               const section_node* p_start_node = &up_sio_graph->get_root_node( );

               const section_node* p_section_node = p_start_node;

               string::size_type pos = next.rfind( '.' );

               if( pos != string::npos )
               {
                  section = next.substr( 0, pos );

                  next.erase( 0, pos + 1 );

                  // NOTE: If section ends with a '*' then iterate through all children.
                  if( !section.empty( ) && ( section[ section.length( ) - 1 ] == '*' ) )
                  {
                     is_range = true;

                     section.erase( section.length( ) - 1 );

                     p_range_node = get_section_node_from_path(
                      *p_section_node, section.substr( 0, section.length( ) - 1 ) );

                     if( p_range_node )
                        p_section_node = get_section_node_from_path(
                         *p_start_node, section + to_string( range_offset ) );
                  }
                  else
                     p_section_node = get_section_node_from_path( *p_section_node, section );
               }

               if( !p_section_node || ( is_range && !p_range_node ) )
                  cerr << "unable to find section '" << section << "'" << endl;
               else
               {
                  while( p_section_node )
                  {
                     string output( get_node_attributes( *p_section_node, next ) );

                     cout << output << endl;

                     if( !is_range )
                        break;

                     p_section_node = get_section_node_from_path(
                      *p_start_node, section + to_string( ++range_offset ), true );
                  }
               }
            }
         }
         else if( cmd == "attributes" )
         {
            if( up_sio_graph.get( ) )
            {
               const section_node* p_section_node = get_section_node_from_path( up_sio_graph->get_root_node( ), next );

               if( !p_section_node )
                  cerr << "unable to find section '" << next << "'" << endl;
               else
               {
                  size_t num_attributes = p_section_node->get_num_attributes( );

                  for( size_t i = 0; i < num_attributes; i++ )
                     cout << p_section_node->get_attribute( i ).get_name( ) << '\n';
               }
            }
         }
         else if( cmd == "output_json" )
         {
            if( up_sio_graph.get( ) )
            {
               const section_node* p_section_node = get_section_node_from_path( up_sio_graph->get_root_node( ), next );

               if( !p_section_node )
                  cerr << "unable to find section '" << next << "'" << endl;
               else
                  convert_sio_to_json( *p_section_node, cout );
            }
         }
         else if( cmd == "exit" )
            break;
         else
            cout << "unknown command '" << cmd << "' (type ? for list of commands)" << endl;
      }
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception caught" << endl;
   }
}

// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <string>
#  include <vector>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "utilities.h"

using namespace std;

const char* const c_section_prefix = "### ";
const char* const c_outline_extension = ".outline";

int main( int argc, char* argv[ ] )
{
   if( argc < 2 || argc > 4 )
   {
      cout << "Usage: construct [@<package.lst>] <target>" << endl;
      return 0;
   }

   int rc = 0;

   try
   {
      int arg_num = 1;
      vector< string > packages;
      map< string, vector< string > > package_details;

      if( argc > 2 )
         buffer_file_lines( string( argv[ arg_num++ ] ).substr( 1 ), packages );

      string outline_filename( argv[ arg_num ] );
      outline_filename += c_outline_extension;

      ifstream inpf( outline_filename.c_str( ) );
      if( !inpf )
         throw runtime_error( "unable to open '" + outline_filename + "' for input" );

      string target_filename( argv[ arg_num ] );
      for( size_t i = 0; i < packages.size( ); i++ )
      {
         string package_filename( packages[ i ] + "." + target_filename );
         if( file_exists( package_filename ) )
         {
            vector< string > details;
            buffer_file_lines( package_filename, details );

            package_details.insert( make_pair( packages[ i ], details ) );
         }
      }

      string new_filename( target_filename + ".new" );

      // NOTE: Empty code block for scope purposes.
      {
         ofstream outf( new_filename.c_str( ) );
         if( !outf )
            throw runtime_error( "unable to open '" + new_filename + "' for output" );

         string next;
         size_t line_num = 0;
         while( getline( inpf, next ) )
         {
            remove_trailing_cr_from_text_file_line( next, ++line_num == 1 );

            if( !next.empty( ) && !packages.empty( ) )
            {
               string::size_type pos = next.find( c_section_prefix );
               if( pos == 0 )
               {
                  for( size_t i = 0; i < packages.size( ); i++ )
                  {
                     if( package_details.count( packages[ i ] ) )
                     {
                        bool found_matching = false;
                        for( size_t j = 0; j < package_details[ packages[ i ] ].size( ); j++ )
                        {
                           pos = package_details[ packages[ i ] ][ j ].find( c_section_prefix );
                           if( pos == 0 )
                           {
                              if( package_details[ packages[ i ] ][ j ] == next )
                                 found_matching = true;
                              else
                                 found_matching = false;
                           }
                           else if( found_matching )
                              outf << package_details[ packages[ i ] ][ j ] << '\n';
                        }
                     }
                  }

                  continue;
               }
            }

            outf << next << '\n';
         }

         if( !inpf.eof( ) )
            throw runtime_error( "unexpected error occurred whilst reading '" + outline_filename + "'" );

         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "unexpected error occurred whilst writing '" + new_filename + "'" );

      }

      string cmd( "./update" );

      cmd += " " + target_filename + " " + new_filename;

      int rc = system( cmd.c_str( ) );
      ( void )rc;
   }
   catch( exception& x )
   {
      rc = 1;

      cerr << "error: " << x.what( ) << endl;
   }
   catch( ... )
   {
      rc = 2;

      cerr << "unexpected exception occurred..." << endl;
   }

   return rc;
}

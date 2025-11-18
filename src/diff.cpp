// Copyright (c) 2002-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <stdio.h>
#  include <unistd.h>
#  include <memory>
#  include <iostream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  define _access access
#endif

//#define DEBUG

#include "diff.h"

#include "text_file_buffer.h"

using namespace std;

const char* const c_quiet_opt = "-q";
const char* const c_min_opt = "-min";
const char* const c_rcs_opt = "-rcs";

const int c_path_items_initial_reserve = 1000;

string append_first_if_second_is_path( const string& first, const string& second )
{
   string retval( second );
   bool second_is_path = false;

   if( !second.empty( ) )
   {
      string::size_type lpos = second.size( ) - 1;

      if( second[ lpos ] == '/' )
         second_is_path = true;

      if( second_is_path || ( _access( second.c_str( ), 0 ) == 0 ) )
      {
         string tmp( retval );

         if( !second_is_path )
            tmp += '/';

         string::size_type fpos = first.find_last_of( "/" );

         if( fpos != string::npos )
            ++fpos;
         else
            fpos = 0;

         tmp += first.substr( fpos );

         if( second_is_path || _access( tmp.c_str( ), 0 ) == 0 )
            retval = tmp;
      }
   }

   return retval;
}

void output_context( int x, int y, int ox, int oy, bool rcs_format )
{
   if( x != ox || y != oy )
   {
      if( rcs_format )
      {
         if( x != ox )
            cout << 'd' << ( ox + 1 ) << ' ' << ( x - ox ) << '\n';
         if( y != oy )
            cout << 'a' << x << ' ' << ( y - oy ) << '\n';
      }
      else
      {
         if( x == ox || x == ox + 1 )
            cout << x;
         else
            cout << ( ox + 1 ) << ',' << x;

         if( x == ox )
            cout << 'a';
         else if( y == oy )
            cout << 'd';
         else
            cout << 'c';

         if( y == oy || y == oy + 1 )
            cout << y;
         else
            cout << ( oy + 1 ) << ',' << y;
         cout << '\n';
      }
   }
}

int main( int argc, char* argv[ ] )
{
   int first_arg = 1;

   if( argc < 3 || argc > 6
    || ( argc == 6 && argv[ 1 ] != string( c_quiet_opt )
    && argv[ 2 ] != string( c_min_opt ) && argv[ 3 ] != string( c_rcs_opt ) )
    || ( argc == 5 && ( argv[ 1 ] != string( c_quiet_opt ) && argv[ 2 ] != string( c_min_opt ) )
    && ( argv[ 1 ] != string( c_quiet_opt ) && argv[ 2 ] != string( c_rcs_opt ) )
    && ( argv[ 1 ] != string( c_min_opt ) && argv[ 2 ] != string( c_rcs_opt ) ) )
    || ( argc == 4 && ( argv[ 1 ] != string( c_quiet_opt )
    && argv[ 1 ] != string( c_min_opt ) && argv[ 1 ] != string( c_rcs_opt ) ) ) )
   {
      cout << "diff v0.1g\n";

      cout << "Usage: diff [" << c_quiet_opt
       << "] [" << c_min_opt << "] [" << c_rcs_opt << "] <file1> <file2 or path>" << endl;

      return 1;
   }

   int rc = 0;

   bool quiet = false;

   if( argc > 2 + first_arg && argv[ first_arg ] == string( c_quiet_opt ) )
   {
      ++first_arg;

      quiet = true;
   }

   bool minimal = false;

   if( argc > 2 + first_arg && argv[ first_arg ] == string( c_min_opt ) )
   {
      ++first_arg;

      minimal = true;
   }

   bool rcs_format = false;

   if( argc > 2 + first_arg && argv[ first_arg ] == string( c_rcs_opt ) )
   {
      ++first_arg;

      rcs_format = true;
   }

   try
   {
      int ed;

      string file1( argv[ first_arg ] );
      string file2( append_first_if_second_is_path( argv[ first_arg ], argv[ first_arg + 1 ] ) );

      text_file_buffer a( file1 );
      text_file_buffer b( file2 );

      if( !quiet )
         cout << "Files: " << file1 << "\t\t" << file2 << endl;

      diff< text_file_buffer > diffs( a, b, true, minimal, a.size( ), b.size( ), c_path_items_initial_reserve );

      ed = diffs.get_edit_distance( );
#ifdef DEBUG
      cout << "(edit distance = " << ed << ")\n" << endl;
#endif

      int len = 0;
      int num_matches;

      pair< int, int > match_point;

      int last_x = 0;
      int last_y = 0;

      while( diffs.get_next_match_chunk( match_point, num_matches ) )
      {
         len += num_matches;

         int x = match_point.first;
         int y = match_point.second;

         output_context( x, y, last_x, last_y, rcs_format );

         if( rcs_format )
         {
            last_x = x;

            while( last_y < y )
               cout << b[ last_y++ ] << '\n';
         }
         else
         {
            bool output_divider = false;

            if( ( last_x < x ) && ( last_y < y ) )
               output_divider = true;

            while( last_x < x )
               cout << "< " << a[ last_x++ ] << '\n';

            if( output_divider )
               cout << "---\n";

            while( last_y < y )
               cout << "> " << b[ last_y++ ] << '\n';
         }

         last_x += num_matches;
         last_y += num_matches;
      }

      int x = a.size( );
      int y = b.size( );

      output_context( x, y, last_x, last_y, rcs_format );

      if( rcs_format )
      {
         while( last_y < y )
            cout << b[ last_y++ ] << '\n';
      }
      else
      {
         bool output_divider = false;

         if( last_x < x && last_y < y )
            output_divider = true;

         while( last_x < x )
            cout << "< " << a[ last_x++ ] << '\n';

         if( output_divider )
            cout << "---\n";

         while( last_y < y )
            cout << "> " << b[ last_y++ ] << '\n';
      }

      if( minimal )
      {
         int lcs = ( a.size( ) + b.size( ) - ed ) / 2;

         if( lcs != len )
            cerr << "\n*** LCS was not found (path length is "
             << len << " but if is LCS should be " << lcs << ") ***\n";
      }
   }
   catch( exception& e )
   {
      rc = 1;

      cerr << "error: " << e.what( ) << endl;
   }

   return rc;
}

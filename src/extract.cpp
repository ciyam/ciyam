// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <string>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

using namespace std;

const char* const c_end_marker = ">]";
const char* const c_start_comment = "// [<start ";
const char* const c_finish_comment = "// [<finish ";

const char* const c_package_record = "<record>";

string escape( const string& input )
{
   string output;
   for( size_t i = 0; i < input.size( ); i++ )
   {
      if( input[ i ] == '\\' )
         output += "\\";
      output += input[ i ];
   }

   return output;
}

int main( int argc, char* argv[ ] )
{
   bool is_package = false;

   if( argc < 2 || ( argc < 3 && string( argv[ 1 ] ) == "-p" )
    || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "/?" || string( argv[ 1 ] ) == "-?" )
   {
      cout << "usage: extract [-p] <filename>" << endl;
      return 0;
   }

   int first_arg = 1;
   if( argc > 2 && string( argv[ 1 ] ) == "-p" )
   {
      ++first_arg;
      is_package = true;
   }

   try
   {
      ifstream inpf( argv[ first_arg ] );
      if( !inpf )
      {
         cerr << "error: unable to open file '" << argv[ first_arg ] << "' for input" << endl;
         return 1;
      }

      string next;
      bool in_user_block( false );

      string start_comment( c_start_comment );
      string finish_comment( c_finish_comment );

      size_t start_comment_len( strlen( c_start_comment ) );
      size_t finish_comment_len( strlen( c_finish_comment ) );

      while( getline( inpf, next ) )
      {
         string::size_type len( next.length( ) );

         string::size_type xlen( len );
         for( string::size_type i = 0; i < len; i++ )
         {
            if( next[ i ] != ' ' )
            {
               xlen += i;
               break;
            }
         }

         if( is_package )
         {
            size_t plen = strlen( c_package_record );

            next = next.substr( xlen - len );
            if( next.length( ) > plen && next.substr( 0, plen ) == c_package_record )
            {
               next = next.substr( plen );
               string::size_type pos = next.find( ',' );
               if( pos != string::npos )
                  next.erase( pos );

               if( next.length( ) > 1
                && ( next.length( ) <= 4 || next.length( ) > 4 && next.substr( 0, 4 ) != "var_" ) )
                  cout << next << "=@key\n";
            }
         }
         else
         {
            if( !in_user_block && xlen > start_comment_len
             && next.substr( xlen - len, start_comment_len ) == string( start_comment ) )
            {
               in_user_block = true;
               string::size_type pos = next.find( c_end_marker );
               if( pos == string::npos )
                  throw runtime_error( "missing expected '" + string( c_end_marker ) + "' in '" + next + "'" );

               cout << "`{`@eq`(`$section`,`'"
                << next.substr( ( xlen - len ) + start_comment_len, pos - ( xlen - len ) - start_comment_len ) << "`'`)\\\n";
            }
            else if( in_user_block && xlen > finish_comment_len
             && next.substr( xlen - len, finish_comment_len ) == string( finish_comment ) )
            {
               in_user_block = false;
               string::size_type pos = next.find( c_end_marker );
               if( pos == string::npos )
                  throw runtime_error( "missing expected '" + string( c_end_marker ) + "' in '" + next + "'" );

               cout << "`}\n";
            }
            else if( in_user_block )
               cout << escape( next ) << '\n';
         }
      }

      if( !inpf.eof( ) )
         throw runtime_error( "unexpected error occurred whilst reading '" + string( argv[ 1 ] ) + "' for input" );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }

   return 0;
}


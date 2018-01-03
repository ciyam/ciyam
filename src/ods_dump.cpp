// Copyright (c) 2000-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2018 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <string>
#  include <iomanip>
#  include <iostream>
#  include <stdexcept>
#endif

#include "ods.h"
#include "utilities.h"

using namespace std;

int main( int argc, char* argv[ ] )
{
   try
   {
      if( argc < 2
       || string( argv[ 1 ] ) == "?" || string( argv[ 1 ] ) == "-?" || string( argv[ 1 ] ) == "/?" )
      {
         cout << "Usage: ods_dump [[-d] <entries>] <file>" << endl;
         return 0;
      }

      int name_arg = 1;

      size_t next_pos;
      int64_t first_entry;
      int64_t final_entry;
      bool show_data = false;
      string entry, entries;

      if( argc > 2 )
      {
         name_arg++;
         if( string( argv[ 1 ] ) == "-d" )
         {
            if( argc == 4 )
            {
               name_arg++;
               show_data = true;
               entries = string( argv[ 2 ] );
            }
         }
         else
            entries = string( argv[ 1 ] );
      }

      bool has_data = file_exists( string( argv[ name_arg ] ) + ".dat" );
      bool has_index = file_exists( string( argv[ name_arg ] ) + ".idx" );
      bool has_header = file_exists( string( argv[ name_arg ] ) + ".hdr" );

      if( !has_data && !has_index && !has_header )
         throw runtime_error( "database '" + string( argv[ name_arg ] ) + "' was not found" );

      if( !has_data )
         throw runtime_error( "database data file not found" );

      if( !has_index )
         throw runtime_error( "database index file not found" );

      if( !has_header )
         throw runtime_error( "database header file not found" );

      ods o( argv[ name_arg ], ods::e_open_mode_exist, ods::e_write_mode_none );

      ods::bulk_dump bulk_dump( o );

      cout << "** File Info" << endl;
      o.dump_file_info( cout );

      if( entries.length( ) )
      {
         next_pos = entries.find( "," );

         entry = entries.substr( 0, next_pos );

         if( entry.find( "all" ) != string::npos )
         {
            first_entry = 0;
            final_entry = o.get_total_entries( );
         }
         else
         {
            size_t range = entry.find( "-" );
            first_entry = atoi( entry.substr( 0, range ).c_str( ) );
            if( range == string::npos )
               final_entry = first_entry + 1;
            else
               final_entry = atoi( entry.substr( range + 1 ).c_str( ) ) + 1;
         }
      }

      if( entries.length( ) )
      {
         int64_t total_entries_dumped = 0;
         cout << endl << "** Entry Info for: " << entries << endl;

         while( true )
         {
            for( int64_t i = first_entry; i < final_entry; i++ )
            {
               o.dump_index_entry( cout, i );

               if( show_data )
                  o.dump_instance_data( cout, i, false );
               else
                  o.dump_instance_data( cout, i, true );

               cout << '\n';

               if( ++total_entries_dumped % 500 == 0 )
                  bulk_dump.pause( );
            }

            if( next_pos != string::npos )
            {
               entry = entries.substr( next_pos + 1 );
               if( entry.find( "," ) != string::npos )
                  entry = entry.substr( 0, entry.find( "," ) );

               if( entry.find( "all" ) != string::npos )
               {
                  first_entry = 0;
                  final_entry = o.get_total_entries( );
               }
               else
               {
                  size_t range = entry.find( "-" );
                  first_entry = atoi( entry.substr( 0, range ).c_str( ) );
                  if( range == string::npos )
                     final_entry = first_entry + 1;
                  else
                     final_entry = atoi( entry.substr( range + 1 ).c_str( ) ) + 1;
               }

               next_pos = entries.find( ",", next_pos + 1 );
            }
            else
               break;
         }
      }
      else
         cout << endl;

      cout << "** Freelist Info" << endl;
      o.dump_free_list( cout );

      if( o.is_using_transaction_log( ) )
      {
         cout << "\n** Transaction Log" << endl;
         o.dump_transaction_log( cout, !show_data );
      }

      return 0;
   }
   catch( exception& x )
   {
      cout << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cout << "unexpected exception occurred..." << endl;
      return 2;
   }
}


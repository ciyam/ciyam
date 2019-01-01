// Copyright (c) 2000-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <string>
#  include <iomanip>
#  include <iostream>
#  include <algorithm>
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
         cout << "Usage: ods_dump [[-d|-t|-dt] <entries>] <database>" << endl;
         return 0;
      }

      int name_arg = 1;

      size_t next_pos;

      bool show_data = false;
      bool show_tranlog_entries = false;

      string all_entries;
      map< int64_t, int64_t > entry_items;

      if( argc > 2 )
      {
         name_arg++;

         string first_arg( argv[ 1 ] );

         if( first_arg == "-d" || first_arg == "-t" || first_arg == "-dt" || first_arg == "-td" )
         {
            if( argc == 4 )
            {
               name_arg++;

               if( first_arg == "-d" )
                  show_data = true;
               else if( first_arg == "-t" )
                  show_tranlog_entries = true;
               else
                  show_data = show_tranlog_entries = true;

               all_entries = string( argv[ 2 ] );
            }
            else
               throw runtime_error( "invalid usage (use no args or -? for usage)" );
         }
         else
            all_entries = first_arg;
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

      if( !all_entries.empty( ) && o.get_total_entries( ) )
      {
         split_and_condense_range_pairs( all_entries, entry_items, o.get_total_entries( ) );

         // NOTE: After the range pairs have been condensed reconstruct "all_entries" from the result.
         all_entries.erase( );
         for( map< int64_t, int64_t >::iterator i = entry_items.begin( ); i != entry_items.end( ); ++i )
         {
            if( !all_entries.empty( ) )
               all_entries += ',';

            if( i->first == i->second )
               all_entries += to_string( i->first );
            else
               all_entries += to_string( i->first ) + '-' + to_string( i->second );
         }
      }

      if( !entry_items.empty( ) )
      {
         int64_t total_entries_dumped = 0;
         cout << "\n** Entry Info for: " << all_entries << endl;

         for( map< int64_t, int64_t >::iterator i = entry_items.begin( ); i != entry_items.end( ); ++i )
         {
            for( int64_t j = i->first; j <= i->second; j++ )
            {
               o.dump_index_entry( cout, j );
               o.dump_instance_data( cout, j, !show_data );

               cout << '\n';

               if( ++total_entries_dumped % 500 == 0 )
                  bulk_dump.pause( );
            }
         }
      }
      else
         cout << endl;

      cout << "** Freelist Info" << endl;
      o.dump_free_list( cout );

      if( o.is_using_transaction_log( ) )
      {
         cout << "\n** Transaction Log Info" << endl;
         o.dump_transaction_log( cout );

         if( !all_entries.empty( ) && show_tranlog_entries )
         {
            cout << "\n** Transaction Log Info for: " << all_entries << endl;
            o.dump_transaction_log( cout, false, &all_entries, true, true );
         }
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


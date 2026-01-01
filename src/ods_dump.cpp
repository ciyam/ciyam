// Copyright (c) 2000-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
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
         cout << "Usage: ods_dump [[-d|-t|-tt|-dt|-dtt] <entries>] [path/to/]<database>" << endl;
         cout << "\nwhere: entries can be 'all' for all or ':all' for only tx log entries" << endl;
         cout << "   or: a single OID or comma separated list of OIDs and/or OID ranges" << endl;
         cout << "   or: :X for tx log entry X (where :0 is for the final tx log entry)" << endl;
         cout << "   or: :X+Y for tx log entry X and Y more entries (:0-Y before final)" << endl;
#ifdef FUTURE_SUPPORT_DATE_RANGE_TXS
         cout << "   or: @yyyy[mm[dd[hh[mm[ss]]]]] for tx log entries matching the time" << endl;
         cout << "   or: @..yyyy[mm[dd[hh[mm[ss]]]]] for tx log entries up to this time" << endl;
         cout << "   or: @yyyy[mm[dd[hh[mm[ss]]]]].. for tx log entries from given time" << endl;
         cout << "   or: @yyyy[mm[dd[hh[mm[ss]]]]]..yyyy[mm[dd[hh[mm[ss]]]]] time range" << endl;
#endif
         return 0;
      }

      int name_arg = 1;

      size_t next_pos;

      bool omit_dtms = false;

      bool show_data = false;
      bool show_tranlog_entries = false;

      string all_entries;
      map< int64_t, int64_t > entry_items;

      if( argc > 2 )
      {
         name_arg++;

         string first_arg( argv[ 1 ] );

         if( first_arg == "-d" || first_arg == "-t" || first_arg == "-tt" || first_arg == "-dt" || first_arg == "-dtt" )
         {
            if( argc == 4 )
            {
               name_arg++;

               if( first_arg.find( "tt" ) != string::npos )
               {
                  omit_dtms = true;
                  first_arg.erase( first_arg.length( ) - 1 );
               }

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

      bool is_all = false;

      string file_names( ods_file_names( argv[ name_arg ], ',', false ) );

      vector< string > names;
      split( file_names, names );

      if( names.size( ) != 3 )
         throw runtime_error( "unexpected names.size( ) != 3" );

      bool has_data = file_exists( names[ 0 ] );
      bool has_index = file_exists( names[ 1 ] );
      bool has_header = file_exists( names[ 2 ] );

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
      o.dump_file_info( cout, omit_dtms );

      bool skip_entries = false;

      if( !all_entries.empty( ) && o.get_total_entries( ) )
      {
         if( all_entries[ 0 ] == ':' )
         {
            skip_entries = true;
            show_tranlog_entries = true;
         }
         else
         {
            split_and_condense_range_pairs( all_entries, entry_items, o.get_total_entries( ) );

            if( all_entries == "*" || all_entries == "all" )
               is_all = true;
            else
            {
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
         }
      }

      if( !skip_entries && !entry_items.empty( ) )
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
         o.dump_transaction_log( cout, omit_dtms );

         if( !all_entries.empty( ) && show_tranlog_entries )
         {
            cout << "\n** Transaction Log Info for: " << all_entries << endl;
            o.dump_transaction_log( cout, omit_dtms, false, is_all ? 0 : &all_entries, true, true );
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


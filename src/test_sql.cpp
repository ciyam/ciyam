// Copyright (c) 2012-2026 CIYAM Developers

#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "sql_db.h"
#include "utilities.h"

using namespace std;

int main( int argc, char* argv[ ] )
{
#ifdef RDBMS_SQLITE
   if( argc < 3 || argc > 5 )
   {
      cout << "Usage: " << argv[ 0 ] << " <db_file> <sql|@sql_file> [-group [order_columns]]" << endl;
      return 0;
   }
#else
   if( argc < 4 || argc > 6 )
   {
      cout << "Usage: " << argv[ 0 ] << " <db> <uid> <sql|@sql_file> [-group [order_columns]]" << endl;
      return 0;
   }
#endif

   try
   {
      string order_columns;

      bool use_dataset_group = false;

#ifdef RDBMS_SQLITE
      sql_db db( argv[ 1 ] );

      string sql( argv[ 2 ] );

      if( ( argc >= 4 ) && string( argv[ 3 ] ) == "-group" )
      {
         use_dataset_group = true;

         if( argc > 4 )
            order_columns = string( argv[ 4 ] );
      }
#else
      sql_db db( argv[ 1 ], argv[ 2 ] );

      string sql( argv[ 3 ] );

      if( ( argc >= 5 ) && string( argv[ 4 ] ) == "-group" )
      {
         use_dataset_group = true;

         if( argc > 5 )
            order_columns = string( argv[ 5 ] );
      }
#endif

      if( sql.length( ) && sql[ 0 ] == '@' )
      {
         if( !use_dataset_group )
            exec_sql_from_file( db, sql.substr( 1 ) );
         else
         {
            vector< string > queries;
            buffer_file_lines( sql.substr( 1 ), queries );

            unique_ptr< sql_dataset_group > up_dsg;

            if( order_columns.empty( ) )
               up_dsg.reset( new sql_dataset_group( db, queries ) );
            else
            {
               vector< string > all_order_columns;
               split( order_columns, all_order_columns );

               up_dsg.reset( new sql_dataset_group( db, queries, false, false, &all_order_columns ) );
            }

            while( up_dsg->next( ) )
            {
               for( int i = 0; i < up_dsg->get_fieldcount( ); i++ )
               {
                  if( i > 0 )
                     cout << '|';
                  cout << up_dsg->as_string( i );
               }
               cout << '\n';
            }
         }
      }
      else
      {
         sql_dataset ds( db, sql );

         while( ds.next( ) )
         {
            for( int i = 0; i < ds.get_fieldcount( ); i++ )
            {
               if( i > 0 )
                  cout << '|';
               cout << ds.as_string( i );
            }
            cout << '\n';
         }
      }
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
   }
}

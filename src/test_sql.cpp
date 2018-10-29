// Copyright (c) 2012-2018 CIYAM Developers

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
   if( argc < 3 || argc > 4 )
   {
      cout << "Usage: " << argv[ 0 ] << " <db_file> <sql|@sql_file> [-group]" << endl;
      return 0;
   }
#else
   if( argc < 4 || argc > 5 )
   {
      cout << "Usage: " << argv[ 0 ] << " <db> <uid> <sql|@sql_file> [-group]" << endl;
      return 0;
   }
#endif

   try
   {
      bool use_dataset_group = false;

#ifdef RDBMS_SQLITE
      sql_db db( argv[ 1 ] );

      string sql( argv[ 2 ] );

      if( ( argc == 4 ) && string( argv[ 3 ] ) == "-group" )
         use_dataset_group = true;
#else
      sql_db db( argv[ 1 ], argv[ 2 ] );

      string sql( argv[ 3 ] );

      if( ( argc == 5 ) && string( argv[ 4 ] ) == "-group" )
         use_dataset_group = true;
#endif

      if( sql.length( ) && sql[ 0 ] == '@' )
      {
         if( !use_dataset_group )
            exec_sql_from_file( db, sql.substr( 1 ) );
         else
         {
            vector< string > queries;
            buffer_file_lines( sql.substr( 1 ), queries );

            sql_dataset_group dsg( db, queries );

            while( dsg.next( ) )
            {
               for( int i = 0; i < dsg.get_fieldcount( ); i++ )
               {
                  if( i > 0 )
                     cout << '|';
                  cout << dsg.as_string( i );
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

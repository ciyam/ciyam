// Copyright (c) 2012-2018 CIYAM Developers

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "sql_db.h"

using namespace std;

int main( int argc, char* argv[ ] )
{
#ifdef RDBMS_SQLITE
   if( argc != 3 )
   {
      cout << "Usage: " << argv[ 0 ] << " <db_file> <sql|@sql_file>" << endl;
      return 0;
   }
#else
   if( argc != 4 )
   {
      cout << "Usage: " << argv[ 0 ] << " <db> <uid> <sql|@sql_file>" << endl;
      return 0;
   }
#endif

   try
   {
#ifdef RDBMS_SQLITE
      sql_db db( argv[ 1 ] );

      string sql( argv[ 2 ] );
#else
      sql_db db( argv[ 1 ], argv[ 2 ] );

      string sql( argv[ 3 ] );
#endif

      if( sql.length( ) && sql[ 0 ] == '@' )
         exec_sql_from_file( db, sql.substr( 1 ) );
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


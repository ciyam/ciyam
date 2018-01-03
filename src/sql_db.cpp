// Copyright (c) 2007-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2018 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <fstream>
#  include <iostream>
#endif

#include "sql_db.h"

#include "utilities.h"

#ifdef RDBMS_SQLITE
#  include "sqlite3.h"
#else
#  ifdef _WIN32
#     include <winsock2.h> // NOTE: Required to get around a MySQL include issue...
#     include <mysql.h>
#  else
#     include <mysql/mysql.h>
#  endif
#endif

using namespace std;

sql_db::sql_db( const string& name )
 :
 p_db( 0 )
{
   init_database_connection( name, "", "" );
}

sql_db::sql_db( const string& name, const string& uid )
 :
 p_db( 0 )
{
   init_database_connection( name, uid, "" );
}

sql_db::sql_db( const string& name, const string& uid, const string& pwd )
 :
 p_db( 0 )
{
   init_database_connection( name, uid, pwd );
}

void sql_db::init_database_connection( const string& name, const string& uid, const string& pwd )
{
#ifdef RDBMS_SQLITE
   if( sqlite3_open( name.c_str( ), &p_db ) )
      throw sql_exception( p_db );
#else
   p_db = mysql_init( NULL );
   if( !p_db )
      throw sql_exception( "MySQL initialisation failed." );

   p_db = mysql_real_connect( p_db, "localhost", uid.c_str( ), pwd.c_str( ), name.c_str( ), 0, NULL, 0 );
   if( !p_db )
      throw sql_exception( "Database connection failed." );
#endif
}

sql_db::~sql_db( )
{
#ifdef RDBMS_SQLITE
   sqlite3_close( p_db );
#else
   mysql_close( p_db );
#endif
   p_db = 0;
}

void exec_sql( sql_db& db, const string& sql )
{
   sql_dataset ds( db );
   ds.exec_sql( sql );
}

void exec_sql_from_file( sql_db& db, const string& sql_file, progress* p_progress, bool unescape )
{
   ifstream inpf( sql_file.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + sql_file + "' for input" );

   string sql, next;
   bool is_first = true;
   while( getline( inpf, next ) )
   {
      remove_trailing_cr_from_text_file_line( next, is_first );

      if( is_first )
         is_first = false;

      if( !sql.empty( ) )
         sql += "\n";
      sql += next;

      if( !next.empty( ) )
      {
         if( next[ 0 ] == '#' )
         {
            if( p_progress )
               p_progress->output_progress( next.substr( 1 ) );
            continue;
         }

         bool is_done = false;
         for( size_t i = next.size( ) - 1; i != 0; i-- )
         {
            if( next[ i ] == ';' )
               is_done = true;
            else if( next[ i ] != ' ' )
               break;
         }

         if( is_done )
         {
            sql_dataset ds( db );

            if( !unescape )
               ds.exec_sql( sql );
            else
               ds.exec_sql( unescaped( sql, "rn\r\n" ) );

            sql.erase( );
         }
      }
   }

   if( !inpf.eof( ) )
      throw runtime_error( "unexpected error occurred whilst reading '" + sql_file + "' for input" );
}

sql_dataset::sql_dataset( sql_db& db, const string& sql )
 :
 p_db( db.get_db_ptr( ) ),
 p_stmt( 0 ),
#ifdef RDBMS_MYSQL
 p_rowset( 0 ),
#endif
 fieldcount( 0 )
{
   set_sql( sql );
}

sql_dataset::~sql_dataset( )
{
#ifdef RDBMS_SQLITE
   if( p_stmt )
      sqlite3_finalize( p_stmt );
#else
   if( p_stmt )
      mysql_free_result( p_stmt );
   if( p_rowset )
      delete p_rowset;
#endif
   p_stmt = 0;
   p_rowset = 0;
}

void sql_dataset::get_params( )
{
#ifdef RDBMS_SQLITE
   int pcount = sqlite3_bind_parameter_count( p_stmt );
   for( int i = 1; i <= pcount; ++i )
   {
      const char* p = sqlite3_bind_parameter_name( p_stmt, i );
      if( p )
         params[ &p[ 1 ] ] = i;
   }
#endif
}

void sql_dataset::get_fields( )
{
#ifdef RDBMS_SQLITE
   int count = p_stmt ? sqlite3_column_count( p_stmt ) : 0;
#else
   int count = p_stmt ? mysql_num_fields( p_stmt ) : 0;
#endif

   fieldcount = count;

#ifdef RDBMS_SQLITE
   for( int i = 0; i < count; i++ )
   {
      const char* p_fld = sqlite3_column_name( p_stmt, i );
      if( p_fld )
         fields[ p_fld ] = i;
   }
#else
   int i = 0;
   MYSQL_FIELD* p_field;

   if( p_stmt )
   {
      while( ( p_field = mysql_fetch_field( p_stmt ) ) )
         fields[ p_field->name ] = i++;
   }
#endif
}

void sql_dataset::set_sql( const string& sql )
{
   if( !p_db )
      throw sql_exception( "Database connection not set" );

   if( p_stmt )
   {
#ifdef RDBMS_SQLITE
      sqlite3_finalize( p_stmt );
#else
      mysql_free_result( p_stmt );
#endif
      p_stmt = 0;
   }

#ifdef RDBMS_SQLITE
   if( sqlite3_prepare( p_db, sql.c_str( ), sql.length( ), &p_stmt, 0 ) != SQLITE_OK )
      throw sql_exception( p_db );
#else
   mysql_query( p_db, sql.c_str( ) );
   p_stmt = mysql_store_result( p_db );

   if( mysql_errno( p_db ) != 0 )
      throw sql_exception( p_db );

   if( p_rowset )
      delete p_rowset;
   p_rowset = new MYSQL_ROW;
#endif

   get_params( );
   get_fields( );
}

void sql_dataset::exec_sql( const string& sql )
{
#ifdef RDBMS_SQLITE
   set_sql( sql );
#else
   set_sql( escaped( sql ) );
#endif   
   next( );
}

bool sql_dataset::next( )
{
#ifdef RDBMS_SQLITE
   if( p_stmt )
   {
      int rc = sqlite3_step( p_stmt );

      switch( rc )
      {
         case SQLITE_ROW:
         return true;

         case SQLITE_DONE:
         case SQLITE_OK:
         return false;

         default:
         throw sql_exception( p_db );
      }
   }
   else
      return false;
#else
   if( p_stmt )
   {
      if( ( *p_rowset = mysql_fetch_row( p_stmt ) ) != NULL )
         return true;
      else
      {
         mysql_free_result( p_stmt );
         p_stmt = 0;

         return false;
      }
   }
   else
      return false;
#endif
}

void sql_dataset::set_param( const string& param, const string& value )
{
#ifdef RDBMS_SQLITE
   int col = params[ param ];

   if( sqlite3_bind_text( p_stmt, col, value.c_str( ), value.length( ), SQLITE_STATIC ) )
      throw sql_exception( p_db );
#else
   throw sql_exception( "Query parameters not yet implemented in MySQL wrapper." );
#endif
}

void sql_dataset::set_param( const string& param, int value )
{
#ifdef RDBMS_SQLITE
   int col = params[ param ];

   if( sqlite3_bind_int( p_stmt, col, value ) )
      throw sql_exception( p_db );
#else
   throw sql_exception( "Query parameters not yet implemented in MySQL wrapper." );
#endif
}

void sql_dataset::set_param( int param, const string& value )
{
#ifdef RDBMS_SQLITE
   if( sqlite3_bind_text( p_stmt, param, value.c_str( ), value.length( ), SQLITE_STATIC ) )
      throw sql_exception( p_db );
#else
   throw sql_exception( "Query parameters not yet implemented in MySQL wrapper." );
#endif
}

void sql_dataset::set_param( int p, int val )
{
#ifdef RDBMS_SQLITE
   if( sqlite3_bind_int( p_stmt, p, val ) )
      throw sql_exception( p_db );
#else
   throw sql_exception( "Query parameters not yet implemented in MySQL wrapper." );
#endif
}

int sql_dataset::as_int( const string& column ) const
{
   int col = fields.find( column )->second;
#ifdef RDBMS_SQLITE
   return sqlite3_column_int( p_stmt, col );
#else
   return atoi( ( *p_rowset )[ col ] );
#endif
}

int sql_dataset::as_int( int col ) const
{
   if( col >= fieldcount )
      throw sql_exception( "Column is out of range" );

#ifdef RDBMS_SQLITE
   return sqlite3_column_int( p_stmt, col );
#else
   return atoi( ( *p_rowset )[ col ] );
#endif
}

string sql_dataset::as_string( const string& column ) const
{
   int col = fields.find( column )->second;
#ifdef RDBMS_SQLITE
   const char* p_text = ( const char* )sqlite3_column_text( p_stmt, col );
#else
   const char* p_text = ( *p_rowset )[ col ];
#endif

   if( !p_text )
      return string( );
   else
      return string( p_text );
}

string sql_dataset::as_string( int col ) const
{
   if( col >= fieldcount )
      throw sql_exception( "Column is out of range" );

#ifdef RDBMS_SQLITE
   const char* p_text = ( const char* )sqlite3_column_text( p_stmt, col );
#else
   const char* p_text = ( *p_rowset )[ col ];
#endif

   if( !p_text )
      return string( );
   else
      return string( p_text );
}

#ifdef RDBMS_SQLITE
sql_exception::sql_exception( sqlite3* p_db )
 : runtime_error( sqlite3_errmsg( p_db ) )
{
}
#else
sql_exception::sql_exception( MYSQL* p_db )
 : runtime_error( mysql_error( p_db ) )
{
}
#endif


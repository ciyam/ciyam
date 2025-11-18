// Copyright (c) 2007-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
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

#include "pointers.h"
#include "utilities.h"

#ifdef RDBMS_SQLITE
#  include "sqlite3.h"
#else
#  include <mysql/mysql.h>
#endif

using namespace std;

struct sql_db::impl
{
   impl( ) : p_db( 0 ) { }

   ~impl( )
   {
#ifdef RDBMS_MYSQL
      mysql_close( p_db );
#else
      sqlite3_close( p_db );
#endif
      p_db = 0;
   }

#ifdef RDBMS_MYSQL
   MYSQL* p_db;
   MYSQL* get_db_ptr( ) { return p_db; }
#else
   sqlite3* p_db;
   sqlite3* get_db_ptr( ) { return p_db; }
#endif
};

sql_db::sql_db( const string& name )
{
   init_database_connection( name, "", "" );
}

sql_db::sql_db( const string& name, const string& uid )
{
   init_database_connection( name, uid, "" );
}

sql_db::sql_db( const string& name, const string& uid, const string& pwd )
{
   init_database_connection( name, uid, pwd );
}

void* sql_db::get_db_ptr( ) const
{
   return p_impl->p_db;
}

void sql_db::init_database_connection( const string& name, const string& uid, const string& pwd )
{
   p_impl = new impl;

#ifdef RDBMS_SQLITE
   if( sqlite3_open( name.c_str( ), &p_impl->p_db ) )
      throw sql_exception( p_impl->p_db );
#else
   p_impl->p_db = mysql_init( NULL );
   if( !p_impl->p_db )
      throw sql_exception( "MySQL initialisation failed." );

   p_impl->p_db = mysql_real_connect( p_impl->p_db, "localhost", uid.c_str( ), pwd.c_str( ), name.c_str( ), 0, NULL, 0 );
   if( !p_impl->p_db )
      throw sql_exception( "Database connection failed." );
#endif
}

sql_db::~sql_db( )
{
   delete p_impl;
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
         sql += '\n';

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

struct sql_dataset::impl
{
   impl( sql_db& db )
    :
    p_stmt( 0 ),
#ifdef RDBMS_MYSQL
    p_rowset( 0 )
#endif
   {
#ifdef RDBMS_MYSQL
      p_db = ( MYSQL* )db.get_db_ptr( );
#else
      p_db = ( sqlite3* )db.get_db_ptr( );
#endif
   }

   ~impl( )
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
#ifdef RDBMS_MYSQL
      p_rowset = 0;
#endif
   }

#ifdef RDBMS_MYSQL
   MYSQL* p_db;
   MYSQL_RES* p_stmt;
   MYSQL_ROW* p_rowset;
#else
   sqlite3* p_db;
   sqlite3_stmt* p_stmt;
#endif
};

sql_dataset::sql_dataset( sql_db& db )
{
   p_impl = new impl( db );
}

sql_dataset::sql_dataset( sql_db& db, const string& sql )
 :
 fieldcount( 0 )
{
   p_impl = new impl( db );

   set_sql( sql );
}

sql_dataset::~sql_dataset( )
{
   delete p_impl;
}

void sql_dataset::get_params( )
{
#ifdef RDBMS_SQLITE
   int pcount = sqlite3_bind_parameter_count( p_impl->p_stmt );

   for( int i = 1; i <= pcount; ++i )
   {
      const char* p = sqlite3_bind_parameter_name( p_impl->p_stmt, i );

      if( p )
         params[ &p[ 1 ] ] = i;
   }
#endif
}

void sql_dataset::get_fields( )
{
#ifdef RDBMS_MYSQL
   int count = p_impl->p_stmt ? mysql_num_fields( p_impl->p_stmt ) : 0;
#else
   int count = p_impl->p_stmt ? sqlite3_column_count( p_impl->p_stmt ) : 0;
#endif

   fieldcount = count;

#ifdef RDBMS_SQLITE
   for( int i = 0; i < count; i++ )
   {
      const char* p_fld = sqlite3_column_name( p_impl->p_stmt, i );

      if( p_fld )
      {
         fields[ p_fld ] = i;
         names[ i ] = string( p_fld );
      }
   }
#else
   int i = 0;
   MYSQL_FIELD* p_field;

   if( p_impl->p_stmt )
   {
      while( ( p_field = mysql_fetch_field( p_impl->p_stmt ) ) )
      {
         names[ i ] = p_field->name;
         fields[ p_field->name ] = i++;
      }
   }
#endif
}

void sql_dataset::set_sql( const string& sql )
{
   if( !p_impl->p_db )
      throw sql_exception( "Database connection not set" );

   if( p_impl->p_stmt )
   {
#ifdef RDBMS_SQLITE
      sqlite3_finalize( p_impl->p_stmt );
#else
      mysql_free_result( p_impl->p_stmt );
#endif
      p_impl->p_stmt = 0;
   }

#ifdef RDBMS_SQLITE
   if( sqlite3_prepare( p_impl->p_db, sql.c_str( ), sql.length( ), &p_impl->p_stmt, 0 ) != SQLITE_OK )
      throw sql_exception( p_impl->p_db );
#else
   mysql_query( p_impl->p_db, sql.c_str( ) );
   p_impl->p_stmt = mysql_store_result( p_impl->p_db );

   if( mysql_errno( p_impl->p_db ) != 0 )
      throw sql_exception( p_impl->p_db );

   if( p_impl->p_rowset )
      delete p_impl->p_rowset;

   p_impl->p_rowset = new MYSQL_ROW;
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
   if( p_impl->p_stmt )
   {
      int rc = sqlite3_step( p_impl->p_stmt );

      switch( rc )
      {
         case SQLITE_ROW:
         return true;

         case SQLITE_DONE:
         case SQLITE_OK:
         return false;

         default:
         throw sql_exception( p_impl->p_db );
      }
   }
   else
      return false;
#else
   if( p_impl->p_stmt )
   {
      if( ( *p_impl->p_rowset = mysql_fetch_row( p_impl->p_stmt ) ) != NULL )
         return true;
      else
      {
         mysql_free_result( p_impl->p_stmt );
         p_impl->p_stmt = 0;

         return false;
      }
   }
   else
      return false;
#endif
}

void sql_dataset::set_param( const string& param, int value )
{
#ifdef RDBMS_SQLITE
   int col = params[ param ];

   if( sqlite3_bind_int( p_impl->p_stmt, col, value ) )
      throw sql_exception( p_impl->p_db );
#else
   throw sql_exception( "Query parameters not yet implemented in MySQL wrapper." );
#endif
}

void sql_dataset::set_param( const string& param, const string& value )
{
#ifdef RDBMS_SQLITE
   int col = params[ param ];

   if( sqlite3_bind_text( p_impl->p_stmt, col, value.c_str( ), value.length( ), SQLITE_STATIC ) )
      throw sql_exception( p_impl->p_db );
#else
   throw sql_exception( "Query parameters not yet implemented in MySQL wrapper." );
#endif
}

void sql_dataset::set_param( int param, int value )
{
#ifdef RDBMS_SQLITE
   if( sqlite3_bind_int( p_impl->p_stmt, param, value ) )
      throw sql_exception( p_impl->p_db );
#else
   throw sql_exception( "Query parameters not yet implemented in MySQL wrapper." );
#endif
}

void sql_dataset::set_param( int param, const string& value )
{
#ifdef RDBMS_SQLITE
   if( sqlite3_bind_text( p_impl->p_stmt, param, value.c_str( ), value.length( ), SQLITE_STATIC ) )
      throw sql_exception( p_impl->p_db );
#else
   throw sql_exception( "Query parameters not yet implemented in MySQL wrapper." );
#endif
}

int sql_dataset::as_int( const string& column ) const
{
   int col = fields.find( column )->second;
#ifdef RDBMS_SQLITE
   return sqlite3_column_int( p_impl->p_stmt, col );
#else
   return atoi( ( *p_impl->p_rowset )[ col ] );
#endif
}

int sql_dataset::as_int( int col ) const
{
   if( ( col < 0 ) || ( col >= fieldcount ) )
      throw sql_exception( "column is out of range" );

#ifdef RDBMS_SQLITE
   return sqlite3_column_int( p_impl->p_stmt, col );
#else
   return atoi( ( *p_impl->p_rowset )[ col ] );
#endif
}

string sql_dataset::as_string( const string& column ) const
{
   int col = fields.find( column )->second;
#ifdef RDBMS_SQLITE
   const char* p_text = ( const char* )sqlite3_column_text( p_impl->p_stmt, col );
#else
   const char* p_text = ( *p_impl->p_rowset )[ col ];
#endif

   if( !p_text )
      return string( );
   else
      return string( p_text );
}

string sql_dataset::as_string( int col ) const
{
   if( ( col < 0 ) || ( col >= fieldcount ) )
      throw sql_exception( "column is out of range" );

#ifdef RDBMS_SQLITE
   const char* p_text = ( const char* )sqlite3_column_text( p_impl->p_stmt, col );
#else
   const char* p_text = ( *p_impl->p_rowset )[ col ];
#endif

   if( !p_text )
      return string( );
   else
      return string( p_text );
}

int sql_dataset::get_col( const string& name ) const
{
   if( !fields.count( name ) )
      throw sql_exception( "unknown column name '" + name + "'" );

   return fields.find( name )->second;
}

string sql_dataset::get_column( int col ) const
{
   if( !names.count( col ) )
      throw sql_exception( "column name for " + to_string( col ) + " not found" );

   return names.find( col )->second;
}

struct sql_dataset_group::impl
{
   impl( sql_db& db, const vector< string >& sql_queries,
    bool is_reverse, bool ignore_first, vector< string >* p_order_columns )
    :
    is_new( true ),
    next_dataset( -1 ),
    is_reverse( is_reverse ),
    ignore_first( ignore_first )
   {
      if( p_order_columns )
         order_columns = *p_order_columns;

      for( size_t i = 0; i < sql_queries.size( ); i++ )
         sql_datasets.push_back( ref_count_ptr< sql_dataset >( new sql_dataset( db, sql_queries[ i ] ) ) );
   }

   bool is_new;
   bool is_reverse;
   bool ignore_first;

   int next_dataset;
   vector< string > next_values;

   vector< int > order_cols;
   vector< string > order_columns;

   vector< bool > has_more;
   vector< ref_count_ptr< sql_dataset > > sql_datasets;
};

sql_dataset_group::sql_dataset_group(
 sql_db& db, const vector< string >& sql_queries,
 bool is_reverse, bool ignore_first_order_column, vector< string >* p_order_columns )
{
   p_impl = new impl( db, sql_queries,
    is_reverse, ignore_first_order_column, p_order_columns );
}

sql_dataset_group::~sql_dataset_group( )
{
   delete p_impl;
}

bool sql_dataset_group::next( )
{
   bool has_next = false;

   if( !p_impl->is_new )
   {
      if( p_impl->next_dataset < 0 || p_impl->next_dataset >= p_impl->sql_datasets.size( ) )
         throw runtime_error( "unexpected next_dataset out of range" );

      if( p_impl->has_more[ p_impl->next_dataset ] )
         p_impl->has_more[ p_impl->next_dataset ] = p_impl->sql_datasets[ p_impl->next_dataset ]->next( );

      p_impl->next_values.clear( );
   }

   for( size_t i = 0; i < p_impl->sql_datasets.size( ); i++ )
   {
      bool is_next = false;
      bool next = p_impl->is_new ? p_impl->sql_datasets[ i ]->next( ) : p_impl->has_more[ i ];

      if( p_impl->is_new )
      {
         p_impl->has_more.push_back( next );

         if( p_impl->order_cols.empty( ) && !p_impl->order_columns.empty( ) )
         {
            for( size_t j = 0; j < p_impl->order_columns.size( ); j++ )
            {
               if( ( j > 0 ) || !p_impl->ignore_first )
               {
                  string next_column( p_impl->order_columns[ j ] );

                  p_impl->order_cols.push_back( p_impl->sql_datasets[ i ]->get_col( next_column ) );
               }
            }
         }
      }

      if( next )
      {
         size_t field_count = ( size_t )p_impl->sql_datasets[ i ]->get_fieldcount( );

         has_next = true;

         if( p_impl->next_values.empty( ) )
         {
            is_next = true;
            p_impl->next_dataset = i;
         }
         else
         {
            size_t start = 0;
            size_t finish = field_count;

            bool using_order_cols = ( p_impl->order_cols.size( ) > 0 );

            if( using_order_cols )
               finish = p_impl->order_cols.size( );
            else if( p_impl->ignore_first )
               ++start;

            for( size_t j = start; j < finish; j++ )
            {
               int col = j;

               if( using_order_cols )
                  col = p_impl->order_cols[ j ];

               string value( p_impl->sql_datasets[ i ]->as_string( col ) );

               if( col >= p_impl->next_values.size( ) )
                  break;

               if( ( !p_impl->is_reverse && value < p_impl->next_values[ col ] )
                || ( p_impl->is_reverse && value > p_impl->next_values[ col ] ) )
               {
                  is_next = true;
                  p_impl->next_dataset = i;
               }

               if( is_next
                || ( ( !p_impl->is_reverse && value > p_impl->next_values[ col ] )
                || ( p_impl->is_reverse && value < p_impl->next_values[ col ] ) ) )
                  break;
            }
         }

         if( is_next )
         {
            for( size_t j = 0; j < field_count; j++ )
            {
               if( j == 0 )
                  p_impl->next_values.clear( );

               p_impl->next_values.push_back( p_impl->sql_datasets[ i ]->as_string( j ) );
            }
         }
      }
   }

   p_impl->is_new = false;

   return has_next;
}

int sql_dataset_group::get_fieldcount( ) const
{
   if( p_impl->next_dataset < 0 || p_impl->next_dataset >= p_impl->sql_datasets.size( ) )
      throw runtime_error( "unexpected next_dataset out of range" );

   return p_impl->sql_datasets[ p_impl->next_dataset ]->get_fieldcount( );
}

string sql_dataset_group::as_string( int col ) const
{
   if( p_impl->next_dataset < 0 || p_impl->next_dataset >= p_impl->sql_datasets.size( ) )
      throw runtime_error( "unexpected next_dataset out of range" );

   return p_impl->sql_datasets[ p_impl->next_dataset ]->as_string( col );
}

#ifdef RDBMS_MYSQL
sql_exception::sql_exception( void* p_db )
 : runtime_error( mysql_error( ( MYSQL* )p_db ) )
{
}
#else
sql_exception::sql_exception( void* p_db )
 : runtime_error( sqlite3_errmsg( ( sqlite3* )p_db ) )
{
}
#endif

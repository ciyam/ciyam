// Copyright (c) 2007-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2018 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SQL_DB_H
#  define SQL_DB_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <iosfwd>
#     include <string>
#     include <vector>
#     include <stdexcept>
#  endif

#  include "progress.h"

#  define RDBMS_MYSQL

#  ifndef RDBMS_MYSQL
#     define RDBMS_SQLITE
#  endif

#  ifdef RDBMS_SQLITE
struct sqlite3;
struct sqlite3_stmt;
#  else
typedef char** MYSQL_ROW;

struct st_mysql;
struct st_mysql_res;
typedef struct st_mysql MYSQL;
typedef struct st_mysql_res MYSQL_RES;
#  endif

class sql_dataset;

class sql_db
{
   friend class sql_dataset;

   public:
   sql_db( const std::string& name );
   sql_db( const std::string& name, const std::string& uid );
   sql_db( const std::string& name, const std::string& uid, const std::string& pwd );
   ~sql_db( );

   private:
   void init_database_connection( const std::string& name, const std::string& uid, const std::string& pwd );

#  ifdef RDBMS_SQLITE
   sqlite3* p_db;
   sqlite3* get_db_ptr( ) { return p_db; }
#  else
   MYSQL* p_db;
   MYSQL* get_db_ptr( ) { return p_db; }
#  endif
};

void exec_sql( sql_db& db, const std::string& sql );

void exec_sql_from_file( sql_db& db,
 const std::string& sql_file, progress* p_progress = 0, bool unescape = false );

struct sql_data
{
   virtual ~sql_data( ) { }

   virtual bool next( ) = 0;

   virtual int get_fieldcount( ) const = 0;

   virtual std::string as_string( int col ) const = 0;
};

class sql_dataset : public sql_data
{
   private:
#  ifdef RDBMS_SQLITE
   sqlite3* p_db;
   sqlite3_stmt* p_stmt;
#  else
   MYSQL* p_db;
   MYSQL_RES* p_stmt;
   MYSQL_ROW* p_rowset;
#  endif

   protected:
   std::map< std::string, int > params;
   std::map< std::string, int > fields;

   void get_params( );
   void get_fields( );

   int fieldcount;
    
   public:
   sql_dataset( ) : p_db( 0 ), p_stmt( 0 ), fieldcount( 0 ) { }

#  ifdef RDBMS_SQLITE
   sql_dataset( sql_db& db ) : p_db( db.get_db_ptr( ) ), p_stmt( 0 ), fieldcount( 0 ) { }
#  else
   sql_dataset( sql_db& db ) : p_db( db.get_db_ptr( ) ), p_stmt( 0 ), p_rowset( 0 ), fieldcount( 0 ) { }
#  endif
   sql_dataset( sql_db& db, const std::string& sql );

   virtual ~sql_dataset( );

   void set_sql( const std::string& sql );
   void exec_sql( const std::string& sql );

   bool next( );

   void set_param( const std::string&, const std::string& );
   void set_param( const std::string&, int );
   void set_param( int, const std::string& );
   void set_param( int, int );

   int get_fieldcount( ) const { return fieldcount; }

   int as_int( const std::string& ) const;
   int as_int( int ) const;
   std::string as_string( const std::string& ) const;
   std::string as_string( int ) const;
};

class sql_dataset_group : public sql_data
{
   public:
   sql_dataset_group( sql_db& db,
    const std::vector< std::string >& sql_queries,
    bool is_reverse = false, bool ignore_first_column_for_ordering = true );

   ~sql_dataset_group( );

   bool next( );

   int get_fieldcount( ) const;

   std::string as_string( int col ) const;

   private:
   struct impl;
   impl* p_impl;
};

class sql_exception : public std::runtime_error
{
   public:
   sql_exception( const std::string& m ) : runtime_error( m.c_str( ) ) { }

#  ifdef RDBMS_SQLITE
   sql_exception( sqlite3* );
#  else
   sql_exception( MYSQL* );
#  endif
};

#endif

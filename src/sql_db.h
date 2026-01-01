// Copyright (c) 2007-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
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

class sql_dataset;

class sql_db
{
   friend class sql_dataset;

   public:
   sql_db( const std::string& name );
   sql_db( const std::string& name, const std::string& uid );
   sql_db( const std::string& name, const std::string& uid, const std::string& pwd );

   virtual ~sql_db( );

   void* get_db_ptr( ) const;

   private:
   void init_database_connection( const std::string& name, const std::string& uid, const std::string& pwd );

   private:
   struct impl;
   impl* p_impl;
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
   protected:
   std::map< int, std::string > names;
   std::map< std::string, int > fields;

   std::map< std::string, int > params;

   void get_params( );
   void get_fields( );

   int fieldcount;
    
   public:
   sql_dataset( sql_db& db );
   sql_dataset( sql_db& db, const std::string& sql );

   virtual ~sql_dataset( );

   void set_sql( const std::string& sql );
   void exec_sql( const std::string& sql );

   bool next( );

   void set_param( const std::string& param, int value );
   void set_param( const std::string& param, const std::string& value );

   void set_param( int param, int value );
   void set_param( int param, const std::string& value );

   int get_fieldcount( ) const { return fieldcount; }

   int as_int( const std::string& column ) const;
   int as_int( int col ) const;

   std::string as_string( const std::string& column ) const;
   std::string as_string( int col ) const;

   int get_col( const std::string& name ) const;
   std::string get_column( int col ) const;

   private:
   struct impl;
   impl* p_impl;
};

class sql_dataset_group : public sql_data
{
   public:
   sql_dataset_group( sql_db& db,
    const std::vector< std::string >& sql_queries, bool is_reverse = false,
    bool ignore_first_order_column = true, std::vector< std::string >* p_order_columns = 0 );

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
   sql_exception( void* p_db );

   sql_exception( const std::string& m ) : runtime_error( m.c_str( ) ) { }
};

#endif

// Copyright (c) 2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <set>
#  include <deque>
#  include <fstream>
#  include <iostream>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_base_ops.h"

#include "ods.h"
#include "sio.h"
#include "sql_db.h"
#include "date_time.h"
#include "class_base.h"
#include "ciyam_base.h"
#include "ciyam_files.h"
#include "ciyam_strings.h"
#include "ciyam_variables.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "ods_file_system.h"
#include "module_management.h"

using namespace std;

//#define COMPILE_PROTOTYPE_CODE

namespace
{

#include "ciyam_constants.h"

const string c_null_key( 1, '\0' );

const int c_loop_variable_digits = 8;

const char c_security_suffix = '-';

const char* const c_all = "all";

const char* const c_ui_type_repl_name = "TYPE";

const char* const c_ui_type_submit_file = "ui_TYPE_submit";

const char* const c_ui_submit_type_peer = "peer";

const char* const c_primary_key_name = "@pk";

const char* const c_links_instance_lock_key = "@links";

const char* const c_invalid_key_characters = "`~!@#$%^&*<>()[]{}/\\?|-+=.,;:'\"";

const size_t c_sec_prefix_length = 6;

const size_t c_num_links_lock_retries = 10;

const size_t c_iteration_row_cache_limit = 100;

const size_t c_max_required_field_iterations = 100;

const char c_global_storage_file_not_folder_suffix = '!';

trace_mutex g_mutex;

string instance_op_name( instance_op op )
{
   string retval( "unknown op " + to_string( op ) );

   switch( op )
   {
      case e_instance_op_none:
      retval = "none";
      break;

      case e_instance_op_review:
      retval = "review";
      break;

      case e_instance_op_create:
      retval = "create";
      break;

      case e_instance_op_update:
      retval = "update";
      break;

      case e_instance_op_destroy:
      retval = "destroy";
      break;
   }

   return retval;
}

string instance_op_name( class_base::op_type op )
{
   string retval( "unknown op " + to_string( op ) );

   switch( op )
   {
      case class_base::e_op_type_none:
      retval = "none";
      break;

      case class_base::e_op_type_review:
      retval = "review";
      break;

      case class_base::e_op_type_create:
      retval = "create";
      break;

      case class_base::e_op_type_update:
      retval = "update";
      break;

      case class_base::e_op_type_destroy:
      retval = "destroy";
      break;
   }

   return retval;
}

size_t split_csv_values( const string& line,
 vector< string >& values, bool& last_value_incomplete, size_t continuation_offset )
{
   string next_value;

   // NOTE: If the last value was continued into the next line then only process the
   // portion that has not already been processed (i.e. continue where it left off).
   if( continuation_offset )
      next_value = line.substr( 0, continuation_offset );

   bool in_quotes = last_value_incomplete;

   for( size_t i = continuation_offset; i < line.size( ); i++ )
   {
      if( in_quotes )
      {
         if( line[ i ] == '"' )
         {
            if( i < line.size( ) - 1 && line[ i + 1 ] != ',' )
            {
               i++;
               next_value += '"';
            }
            else
               in_quotes = false;
         }
         else
            next_value += line[ i ];
      }
      else if( line[ i ] == '"' )
         in_quotes = true;
      else if( line[ i ] != ',' )
         next_value += line[ i ];
      else
      {
         values.push_back( next_value );
         next_value.erase( );
      }
   }

   values.push_back( next_value );

   last_value_incomplete = in_quotes;

   return ( values.size( ) - last_value_incomplete );
}

void split_key_info( const string& key_info,
 vector< pair< string, string > >& fixed_info,
 vector< pair< string, string > >& paging_info,
 vector< string >& order_info, bool order_required )
{
   string::size_type pos = key_info.find( ' ' );

   if( pos == string::npos )
   {
      if( !key_info.empty( ) )
         fixed_info.push_back( make_pair( c_key_field_name, key_info ) );

      // NOTE: The primary key is always being added to the end of the order info as it
      // is not known here if the query will have repeatably ordered results without it.
      if( order_required )
         order_info.push_back( c_key_field_name );
   }
   else
   {
      int num_fixed = 0;

      string fields( key_info.substr( 0, pos ) );
      string values( key_info.substr( pos + 1 ) );

      pos = fields.find( '#' );

      if( pos != string::npos )
      {
         num_fixed = atoi( fields.substr( pos + 1 ).c_str( ) );
         fields.erase( pos );
      }

      size_t order_provided = order_info.size( );

      split( fields, order_info );

      // NOTE: The primary key is always being added to the end of the order info as it
      // is not known here if the query will have repeatably ordered results without it
      // (unless it has already been provided as the only field).
      if( fields != c_key_field )
         order_info.push_back( c_key_field_name );

      if( values.empty( ) )
      {
         if( num_fixed > 1 )
            throw runtime_error( "specified # of fixed values were not provided" );
         else if( num_fixed == 1 )
         {
            if( order_info.size( ) <= order_provided + 1 )
               throw runtime_error( "missing expected ordering field" );

            fixed_info.push_back( make_pair( order_info[ order_provided ], string( ) ) );
         }
      }
      else
      {
         vector< string > all_values;

         split( values, all_values );

         if( all_values.size( ) < num_fixed )
            throw runtime_error( "specified # of fixed values were not provided" );

         if( order_info.size( ) < all_values.size( ) + order_provided )
            throw runtime_error( "not enough order info for all provided values" );

         for( size_t i = 0; i < all_values.size( ); i++ )
         {
            if( i < num_fixed )
               fixed_info.push_back( make_pair( order_info[ order_provided + i ], all_values[ i ] ) );
            else
               paging_info.push_back( make_pair( order_info[ order_provided + i ], all_values[ i ] ) );
         }
      }
   }
}

string get_field_name( class_base& instance,
 string& field_name, bool* p_sql_numeric = 0, string* p_type_name = 0 )
{
   const char* p_str;
   bool was_name = true;
   class_base_accessor instance_accessor( instance );

   if( field_name == c_key_field )
      field_name = c_key_field_name;

   if( ( field_name == c_key_field_name ) || ( field_name == c_typ_field_name ) )
   {
      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( ( field_name == c_ver_field_name )
    || ( field_name == c_rev_field_name ) || ( field_name == c_sec_field_name ) )
   {
      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else
   {
      p_str = instance_accessor.get_field_name( field_name, p_sql_numeric, p_type_name );

      if( p_str )
         field_name = p_str;
      else
         p_str = instance_accessor.get_field_id( field_name, p_sql_numeric, p_type_name );

      if( !p_str )
         throw runtime_error( "field name/id " + field_name + " is invalid" );
   }

   return field_name;
}

string formatted_value( const string& value, const string& field_type )
{
   string formatted_value( value );

   if( !formatted_value.empty( ) )
   {
      if( field_type == "udate" || field_type == "tdate" )
      {
         udate today( udate::standard( ) );

         if( formatted_value == c_date_today )
            formatted_value = today.as_string( );
         else if( formatted_value == c_date_tomorrow )
            formatted_value = ( ++today ).as_string( );

         formatted_value = udate( formatted_value ).as_string( true );
      }
      else if( field_type == "mtime" || field_type == "ttime" )
      {
         if( formatted_value == c_dtm_now )
            formatted_value = mtime::standard( ).as_string( );

         formatted_value = mtime( formatted_value ).as_string( true );
      }
      else if( field_type == "datetime" || field_type == "tdatetime"
       || field_type == "date_time" || field_type == "tdate_time" )
      {
         date_time now( date_time::standard( ) );

         if( formatted_value == c_dtm_now || formatted_value == c_date_today )
            formatted_value = now.as_string( );
         else if( formatted_value == c_date_tomorrow )
            formatted_value = ( ++now ).as_string( );
         // NOTE: If a date_time string starts with 'U' then it is considered as already being UTC.
         else if( formatted_value[ 0 ] == 'U' )
            formatted_value.erase( 0, 1 );
         else
         {
            string tz_name( get_tz_name( ) );
            if( tz_name.empty( ) )
               tz_name = get_timezone( );

            formatted_value = local_to_utc( date_time( formatted_value ), tz_name ).as_string( );
         }

         formatted_value = date_time( formatted_value ).as_string( true );
      }
   }

   return formatted_value;
}

void split_text_search( const string& text_search, vector< string >& words )
{
   string next_word;

   bool in_quotes = false;
   bool had_escape = false;

   for( size_t i = 0; i < text_search.size( ); i++ )
   {
      char ch = text_search[ i ];

      if( had_escape )
      {
         had_escape = false;

         // NOTE: Backslashes can be used in order to escape quote characters
         // but if not being used for this purpose then they are instead just
         // included in the query.
         if( ch != '"' )
            next_word += '\\';

         next_word += ch;
      }
      else if( ch == '\\' )
         had_escape = true;
      else
      {
         if( ch == '"' )
         {
            in_quotes = !in_quotes;
            if( !in_quotes )
            {
               if( !next_word.empty( ) )
               {
                  words.push_back( next_word );
                  next_word.erase( );
               }
            }
            else if( i < text_search.size( ) - 1 && text_search[ i + 1 ] == '"' )
            {
               in_quotes = false;
               next_word += ch;
               ++i;
            }
         }
         else
         {
            if( ch == ' ' && !in_quotes )
            {
               if( !next_word.empty( ) )
               {
                  words.push_back( next_word );
                  next_word.erase( );
               }
            }
            else
               next_word += ch;
         }
      }
   }

   // NOTE: If the one and only character provided was a backslash then escape it
   // so that at least the query won't be empty.
   if( words.empty( ) && had_escape )
      next_word = "\\";

   if( !next_word.empty( ) )
      words.push_back( next_word );
}

void setup_select_columns( class_base& instance, const vector< string >& field_info )
{
   class_base_accessor instance_accessor( instance );

   vector< int > column_nums;
   vector< string > column_names;

   map< int, int > field_nums;
   map< string, int > field_names;

   int num_fields = instance.get_num_fields( );

   for( int i = 0, c = 0; i < num_fields; i++ )
   {
      if( instance.is_field_transient( i ) )
         continue;

      field_nums.insert( make_pair( c++, i ) );
      field_names.insert( make_pair( instance.get_field_name( i ), i ) );
   }

   instance.get_sql_column_names( column_names );

   if( field_info.empty( ) )
   {
      for( size_t i = 0; i < column_names.size( ); i++ )
         column_nums.push_back( i );
   }
   else
   {
      field_nums.clear( );

      for( size_t i = 0; i < field_info.size( ); i++ )
      {
         string next_field( field_info[ i ] );
         get_field_name( instance, next_field );

         string column_name( "C_" + next_field );

         for( size_t j = 0; j < column_names.size( ); j++ )
         {
            if( column_name == column_names[ j ] )
            {
               column_nums.push_back( j );
               field_nums.insert( make_pair( j, field_names[ next_field ] ) );
            }
         }
      }

      if( column_nums.size( ) != field_info.size( ) )
         throw runtime_error( "unexpected fields/columns size mismatch" );
   }

   instance_accessor.select_fields( ) = field_nums;
   instance_accessor.select_columns( ) = column_nums;
}

string construct_paging_sql( const vector< pair< string, string > >& paging_info, bool reverse, bool inclusive )
{
   string page_exclusive( reverse ? " < " : " > " );
   string page_inclusive( reverse ? " <= " : " >= " );

   string sql( "(" );

   for( size_t i = 0; i < paging_info.size( ); i++ )
   {
      if( i == 0 )
      {
         if( !inclusive || ( i != paging_info.size( ) - 1 ) )
            sql += "C_" + paging_info[ i ].first + page_exclusive + paging_info[ i ].second;
         else
            sql += "C_" + paging_info[ i ].first + page_inclusive + paging_info[ i ].second;
      }
      else
      {
         sql += " OR (";

         for( size_t j = 0; j < i; j++ )
         {
            if( j > 0 )
               sql += " AND ";
            sql += "C_" + paging_info[ j ].first + " = " + paging_info[ j ].second;
         }

         sql += " AND ";

         if( !inclusive || ( i != paging_info.size( ) - 1 ) )
            sql += "C_" + paging_info[ i ].first + page_exclusive + paging_info[ i ].second;
         else
            sql += "C_" + paging_info[ i ].first + page_inclusive + paging_info[ i ].second;

         sql += ")";
      }
   }

   sql += ")";

   return sql;
}

string construct_sql_select(
 class_base& instance,
 const vector< string >& field_info,
 const vector< string >& order_info,
 const vector< pair< string, string > >& query_info,
 const vector< pair< string, string > >& fixed_info,
 const vector< pair< string, string > >& paging_info,
 bool is_reverse, bool is_inclusive, int row_limit, bool only_sys_fields,
 const string& text_search, vector< string >* p_order_columns = 0, const string* p_sec_marker = 0 )
{
   string sql, sql_fields_and_table( "SELECT " );

   class_base_accessor instance_accessor( instance );

   set< string > field_set;

   bool has_all_fields = false;

   vector< string > use_index_fields;

   if( field_info.empty( ) )
   {
      if( !only_sys_fields )
      {
         has_all_fields = true;
         sql_fields_and_table += "*";
      }
      else
         sql_fields_and_table += "C_Key_,C_Ver_,C_Rev_,C_Sec_,C_Typ_";
   }
   else
   {
      sql_fields_and_table += "C_Key_,C_Ver_,C_Rev_,C_Sec_,C_Typ_";

      for( size_t i = 0; i < field_info.size( ); i++ )
      {
         string next_field( field_info[ i ] );

         if( next_field.empty( ) )
            throw runtime_error( "unexpected empty field found in sql select preparation" );

         get_field_name( instance, next_field );

         field_set.insert( next_field );

         sql_fields_and_table += ",C_" + next_field;
      }
   }

   bool had_any_restrict = false;
   bool had_sec_restrict = false;

   sql_fields_and_table += " FROM T_"
    + string( instance.get_module_name( ) ) + "_" + string( instance.get_class_name( ) );

   if( !query_info.empty( ) || !text_search.empty( )
    || ( fixed_info.empty( ) && !order_info.empty( ) )
    || !fixed_info.empty( ) || !paging_info.empty( ) )
   {
      sql += " WHERE ";

      bool is_primary_only = false;

      if( ( fixed_info.size( ) == 1 )
       && ( fixed_info.size( ) == order_info.size( ) ) )
      {
         if( fixed_info.back( ).first == c_key_field_name )
            is_primary_only = true;
      }

      if( !is_primary_only && !order_info.empty( ) )
      {
         sql += "C_Sec_ ";

         if( !p_sec_marker )
         {
            if( !is_reverse )
               sql += ">= 0";
            else
               sql += "<= 99999999";
         }
         else
            sql += "= " + *p_sec_marker;

         had_any_restrict = had_sec_restrict = true;
      }
   }

   if( !query_info.empty( ) )
   {
      for( size_t i = 0; i < query_info.size( ); i++ )
      {
         string field_name( query_info[ i ].first );
         string field_values( query_info[ i ].second );

         if( had_any_restrict )
            sql += " AND ";

         had_any_restrict = true;

         bool invert = false;

         string invert_prefix;

         if( field_values.length( ) && ( field_values[ 0 ] == '!' ) )
         {
            invert = true;
            invert_prefix = "NOT ";

            field_values.erase( 0, 1 );
         }

         bool is_sub_select = false;

         class_base* p_instance( &instance );

         string sub_select_sql_prefix, sub_select_sql_suffix;

         // NOTE: If a child sub-context & field has been provided then one or more sub-selects
         // (depending upon the operator used) will be included in the generated SQL expression.
         if( !field_name.empty( ) && ( field_name[ 0 ] == '_' ) )
         {
            string::size_type pos = field_name.find( '.' );

            if( pos == string::npos )
               throw runtime_error( "unexpected field name format '" + field_name + "'" );

            class_base& child_instance( instance.get_or_create_graph_child( field_name.substr( 0, pos ) ) );

            is_sub_select = true;
            p_instance = &child_instance;

            field_name.erase( 0, pos + 1 );

            string graph_parent_fk_field( p_instance->get_graph_parent_fk_field( ) );

            get_field_name( *p_instance, graph_parent_fk_field );

            sub_select_sql_prefix = "C_Key_ " + invert_prefix + "IN (SELECT C_"
             + graph_parent_fk_field + " FROM T_" + to_string( p_instance->get_module_name( ) )
             + "_" + p_instance->get_class_name( ) + " WHERE ";

            sub_select_sql_suffix = " GROUP BY C_" + graph_parent_fk_field + ")";

            sql += sub_select_sql_prefix;
         }

         string field_type;

         bool is_sql_numeric = false;

         get_field_name( *p_instance, field_name, &is_sql_numeric, &field_type );

         vector< string > replacements;
         instance.get_transient_replacement_field_names( field_name, replacements );

         string columns;

         if( replacements.empty( ) )
            columns = "C_" + field_name;
         else
         {
            for( size_t j = 0; j < replacements.size( ); j++ )
            {
               if( !columns.empty( ) )
                  columns += ',';

               columns += "C_" + get_field_name( instance, replacements[ j ] );
            }

            columns = "CONCAT(" + columns + ")";
         }

         bool was_like = false;

         if( field_values.length( ) >= 2 )
         {
            if( ( field_values[ 0 ] == '*' )
             && ( field_values[ field_values.length( ) - 1 ] == '*' ) )
            {
               was_like = true;

               field_values.erase( 0, 1 );
               field_values.erase( field_values.size( ) - 1 );

               field_values = '%' + field_values + '%';

               // NOTE: For end-user convenience LIKE queries are performed case-insensitively.
               sql += "LOWER(" + columns + ") " + invert_prefix + "LIKE " + sql_quote( lower( field_values ) );
            }
            else if( field_values[ field_values.length( ) - 1 ] == '*' )
            {
               was_like = true;

               field_values.erase( field_values.size( ) - 1 );

               field_values += '%';

               sql += columns + " " + invert_prefix + "LIKE " + sql_quote( field_values );
            }
         }

         if( !was_like )
         {
            string::size_type pos = field_values.find( ".." );

            if( pos != string::npos )
            {
               sql += columns + invert_prefix + " BETWEEN ";

               if( is_sql_numeric )
               {
                  sql += field_values.substr( 0, pos );
                  sql += " AND ";
                  sql += field_values.substr( pos + 2 );
               }
               else
               {
                  sql += sql_quote( formatted_value( field_values.substr( 0, pos ), field_type ) );
                  sql += " AND ";
                  sql += sql_quote( formatted_value( field_values.substr( pos + 2 ), field_type ) );
               }
            }
            else
            {
               pos = field_values.find( '|' );

               if( pos == string::npos )
               {
                  pos = field_values.find( '&' );

                  if( pos == string::npos )
                  {
                     sql += columns + ( invert ? " <> " : " = " );

                     sql += is_sql_numeric ? field_values : sql_quote( formatted_value( field_values, field_type ) );
                  }
                  else
                  {
                     if( !is_sub_select )
                        throw runtime_error( "unexpected '&' operator found in '" + field_values + "'" );

                     bool is_first = true;

                     while( true )
                     {
                        if( is_first )
                           is_first = false;
                        else
                        {
                           sql += sub_select_sql_suffix;
                           sql += " AND ";
                           sql += sub_select_sql_prefix;
                        }

                        sql += columns + ( invert ? " <> " : " = " );

                        string next_value( field_values.substr( 0, pos ) );

                        sql += is_sql_numeric ? next_value : sql_quote( formatted_value( next_value, field_type ) );

                        if( pos == string::npos )
                           break;

                        field_values.erase( 0, pos + 1 );

                        pos = field_values.find( '&' );
                     }
                  }
               }
               else
               {
                  sql += columns + " " + invert_prefix + "IN (";

                  while( true )
                  {
                     string next_value( field_values.substr( 0, pos ) );

                     sql += is_sql_numeric ? next_value : sql_quote( formatted_value( next_value, field_type ) );

                     if( pos == string::npos )
                        break;

                     sql += ",";

                     field_values.erase( 0, pos + 1 );

                     pos = field_values.find( '|' );
                  }

                  sql += ")";
               }
            }
         }

         if( is_sub_select )
            sql += sub_select_sql_suffix;
      }
   }

   if( !text_search.empty( ) )
   {
      vector< string > text_search_fields;
      instance.get_text_search_fields( text_search_fields );

      if( text_search_fields.empty( ) )
         throw runtime_error( "no text search fields defined in class '" + string( instance.get_class_name( ) ) + "'" );

      vector< string > text_search_words;
      split_text_search( text_search, text_search_words );

      if( had_any_restrict )
         sql += " AND ";

      had_any_restrict = true;

      if( text_search_words.size( ) > 1 )
         sql += "(";

      for( size_t i = 0; i < text_search_words.size( ); i++ )
      {
         if( i > 0 )
            sql += " AND ";

         sql += "(";

         for( size_t j = 0; j < text_search_fields.size( ); j++ )
         {
            if( j > 0 )
               sql += " OR ";

            string like_expr( escaped( text_search_words[ i ], "'%_" ) );

            like_expr = "%" + like_expr + "%";

            // NOTE: Due to issues with UTF-8 and MySQL the query needs to use both the case
            // sensitive and insensitive tests in order to get the correct results.
            sql += "C_" + text_search_fields[ j ] + " LIKE '" + like_expr + "'";
            sql += " OR LOWER(C_" + text_search_fields[ j ] + ") LIKE LOWER('" + like_expr + "')";
         }

         sql += ")";
      }

      if( text_search_words.size( ) > 1 )
         sql += ")";
   }

   if( !fixed_info.empty( ) )
   {
      for( size_t i = 0; i < fixed_info.size( ); i++ )
      {
         string next_field( fixed_info[ i ].first );
         string next_value( fixed_info[ i ].second );

         if( had_any_restrict )
            sql += " AND ";

         had_any_restrict = true;

         string field_type;

         bool is_sql_numeric = false;

         get_field_name( instance, next_field, &is_sql_numeric, &field_type );

         if( query_info.empty( ) && text_search.empty( ) )
            use_index_fields.push_back( "C_" + next_field );

         bool invert = false;
         bool has_multiple_values = false;

         string invert_prefix;

         // NOTE: If a fixed key value contains one or more "+val" suffixes then these
         // are stripped off to behave as though they are in an IN list (although "ORs"
         // are actually used in the SQL generated here).
         string::size_type pos = next_value.find( '+' );

         if( pos != string::npos )
         {
            has_multiple_values = true;

            sql += "(";

            string::size_type spos( pos + 1 );

            while( true )
            {
               string::size_type npos = next_value.find( '+', spos );

               string next_opt_value;

               if( npos == string::npos )
                  next_opt_value = next_value.substr( spos );
               else
                  next_opt_value = next_value.substr( spos, npos - spos );

               if( !next_opt_value.empty( ) && next_opt_value[ 0 ] == '!' )
                  sql += "C_" + next_field + " <> " + next_opt_value.substr( 1 );
               else
                  sql += "C_" + next_field + " = " + next_opt_value;

               sql += " OR ";

               if( npos == string::npos )
                  break;

               spos = npos + 1;
            }

            next_value.erase( pos );
         }
         else if( !next_value.empty( ) && next_value[ 0 ] == '!' )
         {
            invert = true;
            invert_prefix = "NOT ";

            next_value.erase( 0, 1 );
         }

         // NOTE: If a fixed key value ends with a '*' then the value preceeding
         // it will become a LIKE prefix and the '*' is changed to a SQL wildcard.
         bool value_is_like_prefix = false;

         if( !next_value.empty( )
          && ( next_value[ next_value.length( ) - 1 ] == '*' ) )
         {
            value_is_like_prefix = true;

            next_value[ next_value.length( ) - 1 ] = '%';
         }

         string lhs, rhs;
         bool is_between = false;

         if( !value_is_like_prefix )
         {
            string::size_type pos = next_value.find( ".." );

            if( pos != string::npos )
            {
               is_between = true;

               if( is_sql_numeric )
               {
                  lhs = next_value.substr( 0, pos );
                  rhs = next_value.substr( pos + 2 );
               }
               else
               {
                  lhs = sql_quote( formatted_value( next_value.substr( 0, pos ), field_type ) );
                  rhs = sql_quote( formatted_value( next_value.substr( pos + 2 ), field_type ) );
               }
            }
         }

         if( !is_between && !is_sql_numeric )
            next_value = sql_quote( formatted_value( next_value, field_type ) );

         if( next_value == "NULL" )
            sql += "C_" + next_field + " " + invert_prefix + " IS NULL";
         else if( !value_is_like_prefix )
         {
            if( !is_between )
               sql += "C_" + next_field + ( invert ? " <> " : " = " ) + next_value;
            else
               sql += "C_" + next_field + " " + invert_prefix + "BETWEEN " + lhs + " AND " + rhs;
         }
         else
            sql += "C_" + next_field + " " + invert_prefix + "LIKE " + next_value;

         if( has_multiple_values )
            sql += ")";
      }
   }

   if( !paging_info.empty( ) )
   {
      vector< pair< string, string > > modified_paging_info;

      for( size_t i = 0; i < paging_info.size( ); i++ )
      {
         string next_field( paging_info[ i ].first );
         string next_value( paging_info[ i ].second );

         string field_type;

         bool is_sql_numeric = false;

         get_field_name( instance, next_field, &is_sql_numeric, &field_type );

         if( query_info.empty( ) && text_search.empty( ) )
            use_index_fields.push_back( "C_" + next_field );

         if( !is_sql_numeric )
            next_value = sql_quote( formatted_value( next_value, field_type ) );

         modified_paging_info.push_back( make_pair( next_field, next_value ) );
      }

      if( had_any_restrict )
         sql += " AND ";

      had_any_restrict = true;

      sql += construct_paging_sql( modified_paging_info, is_reverse, is_inclusive );
   }

   if( !order_info.empty( ) )
   {
      sql += " ORDER BY ";

      if( had_sec_restrict )
      {
         sql += "C_Sec_";

         if( is_reverse )
            sql += " DESC";

         sql += ",";
      }

      vector< string > unique_indexes;

      instance.get_sql_unique_indexes( unique_indexes );

      set< string > sorted_unique_indexes( unique_indexes.begin( ), unique_indexes.end( ) );

      string index;

      for( size_t i = 0; i < order_info.size( ); i++ )
      {
         string next_field( order_info[ i ] );

         get_field_name( instance, next_field );

         // NOTE: As the "key" is always appended to the ordering (in case no unique index found)
         // here if a unique index is found on all but the last order field then skip this field.
         if( ( i == order_info.size( ) - 1 ) && sorted_unique_indexes.count( index ) )
            break;

         if( i > 0 )
         {
            sql += ',';
            index += ',';
         }

         sql += "C_" + next_field;
         index += "C_" + next_field;

         if( p_order_columns )
         {
            if( has_all_fields || field_set.count( next_field ) )
               p_order_columns->push_back( "C_" + next_field );
         }

         if( is_reverse )
            sql += " DESC";
      }
   }

   if( row_limit )
      sql += " LIMIT " + to_string( row_limit );

   // NOTE: If no query or text search info was supplied then the fixed and paging fields will be
   // checked against each table index to determine if an index covers all of them. If one is found
   // then force the query to use this index (to ensure the RDBMS optimiser does not get it wrong).
   if( !use_index_fields.empty( ) )
   {
      vector< string > indexes;

      instance.get_sql_indexes( indexes );

      vector< string > unique_indexes;

      instance.get_sql_unique_indexes( unique_indexes );

      set< string > sorted_unique_indexes( unique_indexes.begin( ), unique_indexes.end( ) );

      for( size_t i = use_index_fields.size( ); i < order_info.size( ); i++ )
      {
         string next_field( order_info[ i ] );

         get_field_name( instance, next_field );

         use_index_fields.push_back( "C_" + next_field );
      }

      bool found = false;

      string index_to_use;

      for( size_t i = 0; i < indexes.size( ); i++ )
      {
         vector< string > index_fields;

         split( indexes[ i ], index_fields );

         // NOTE: As the "key" is always appended to the ordering (in case no unique index found)
         // the key is appended to unique indexes so the field matching will operate as expected.
         if( sorted_unique_indexes.count( indexes[ i ] ) )
            index_fields.push_back( "C_Key_" );

         if( index_fields.size( ) == use_index_fields.size( ) )
         {
            for( size_t j = 0; j < use_index_fields.size( ); j++ )
            {
               if( index_fields[ j ] != use_index_fields[ j ] )
                  break;

               if( j == ( use_index_fields.size( ) - 1 ) )
                  found = true;
            }
         }

         if( found )
         {
            index_to_use = "I_" + string( instance.get_module_name( ) ) + "_"
             + string( instance.get_class_name( ) ) + "_" + ( i < 10 ? "0" : "" ) + to_string( i );

            break;
         }
      }

      if( !index_to_use.empty( ) )
         sql_fields_and_table += " USE INDEX (" + index_to_use + ")";
   }

   return sql_fields_and_table + sql;
}

string determine_local_iteration_info(
 class_base& instance, const vector< string >& order_info,
 const vector< pair< string, string > >& fixed_info, string& prefix )
{
   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );

   class_base_accessor instance_accessor( instance );

   string folder, sub_folder, order_field_list;

   for( size_t i = 0; i < order_info.size( ); i++ )
   {
      if( i > 0 )
         order_field_list += ',';

      if( order_info[ i ] == c_key_field_name )
         order_field_list += c_key_field;
      else
         order_field_list += get_field_name_for_id_or_name( module_name, class_id, order_info[ i ] );
   }

   string order_without_key;

   string::size_type pos = order_field_list.rfind( ',' );

   if( pos != string::npos )
      order_without_key = order_field_list.substr( 0, pos );

   // NOTE: To simplify index comparisions will always append the
   // key field (if it had not already been provided as the final
   // field).
   if( !order_field_list.empty( ) )
   {
      string::size_type pos = order_field_list.find( c_key_field );

      if( pos == string::npos )
         order_field_list += ',' + string( c_key_field );
   }

   string local_starts_name( get_special_var_name( e_special_var_local_starts ) );

   instance.set_variable( local_starts_name, c_key_field );

   if( order_info.empty( ) || ( order_field_list == c_key_field ) )
      folder += c_storage_folder_name_dot_dat;
   else
   {
      vector< pair< string, string > > all_index_pairs;

      instance.get_all_index_pairs( all_index_pairs );

      size_t num_index_pairs = all_index_pairs.size( );

      int index_num = -1;
      int index_partial = -1;

      for( size_t i = 0; i < num_index_pairs; i++ )
      {
         pair< string, string > next_pair = all_index_pairs[ i ];

         string next_field_list( next_pair.first );

         string::size_type pos = next_field_list.find( c_key_field );

         // NOTE: Always append the key field to simplify
         // comparisions. This should not cause any issue
         // as it does not make sense to have both unique
         // and non-unique indexes on the same fields.
         if( pos == string::npos )
            next_field_list += ',' + string( c_key_field );

         if( next_field_list == order_field_list )
         {
            index_num = i;
            break;
         }
         else if( ( index_partial < 0 ) && ( next_field_list.find( order_without_key ) == 0 ) )
            index_partial = i;
      }

      if( ( index_num < 0 ) && ( index_partial < 0 ) )
         throw runtime_error( "unable to find an index pair for '"
          + order_field_list + "' in class '" + instance.get_class_name( ) + "'" );

      // NOTE: Will always prefer an exact index match but if
      // none was found then will use the first partial match.
      if( index_num < 0 )
         index_num = index_partial;

      string field_name( all_index_pairs[ index_num ].first );

      string::size_type pos = field_name.rfind( ',' );

      if( pos != string::npos )
         field_name.erase( 0, pos + 1 );

      instance.set_variable( local_starts_name, field_name );

      folder += c_storage_folder_name_dot_idx;

      if( index_num < 10 )
         sub_folder += '0';

      sub_folder += to_string( index_num );

      if( !fixed_info.empty( ) )
      {
         for( size_t i = 0; i < fixed_info.size( ); i++ )
         {
            if( ( i > 0 ) || !prefix.empty( ) )
               prefix += '\t';

            prefix += fixed_info[ i ].second;
         }
      }
   }

   folder += '/' + class_id;

   if( !sub_folder.empty( ) )
      folder += '/' + sub_folder;

   return folder;
}

bool fetch_instance_from_db( class_base& instance,
 const map< int, int >& fields, const vector< int >& columns, bool skip_after_fetch )
{
   class_base_accessor instance_accessor( instance );

   sql_data& sd( *instance_accessor.p_sql_data( ) );

   bool found = sd.next( );

   instance_accessor.clear( );

   if( !found )
   {
      delete instance_accessor.p_sql_data( );

      instance_accessor.p_sql_data( ) = 0;
   }
   else
   {
      TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES, "(from instance dataset)" );

      instance_accessor.set_key( sd.as_string( 0 ), true );

      instance_accessor.set_version( from_string< uint16_t >( sd.as_string( 1 ) ) );
      instance_accessor.set_revision( from_string< uint64_t >( sd.as_string( 2 ) ) );

      instance_accessor.set_security( from_string< uint64_t >( sd.as_string( 3 ) ) );

      instance_accessor.set_original_identity( sd.as_string( 4 ) );

      instance_accessor.set_original_revision( instance.get_revision( ) );

      int fcount = sd.get_fieldcount( );

      for( int i = c_num_sys_field_names; i < fcount; i++ )
      {
         if( !fields.count( columns[ i - c_num_sys_field_names ] ) )
            throw runtime_error( "unexpected field # not found for column #" + to_string( i - c_num_sys_field_names ) );

         int fnum( fields.find( columns[ i - c_num_sys_field_names ] )->second );

         TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES,
          "setting field #" + to_string( fnum + 1 ) + " to " + sd.as_string( i ) );

         instance.set_field_value( fnum, sd.as_string( i ) );
      }
   }

   instance_accessor.after_fetch_from_db( );

   if( !skip_after_fetch )
      instance_accessor.perform_after_fetch( );

   return found;
}

bool global_storage_persistence_is_file( string& root_child_folder )
{
   bool is_file_not_folder = false;

   if( !root_child_folder.empty( )
    && root_child_folder[ root_child_folder.length( ) - 1 ] == c_global_storage_file_not_folder_suffix )
   {
      is_file_not_folder = true;

      root_child_folder.erase( root_child_folder.length( ) - 1 );
   }

   return is_file_not_folder;
}

bool has_instance_in_local_storage( class_base& instance, const string& key )
{
   bool retval = false;

   string class_id( instance.get_class_id( ) );

   storage_ods_bulk_read ods_bulk_read;

   ods_file_system ofs( storage_ods_instance( ) );

   ofs.set_root_folder( c_storage_folder_name_dot_dat );

   if( ofs.has_folder( class_id ) )
   {
      ofs.set_folder( class_id );

      string key_prefix( key );

      string::size_type pos = key_prefix.find( '\t' );

      if( pos == string::npos )
         key_prefix += '\t';
      else
         key_prefix.erase( pos + 1 );

      retval = ofs.has_file( key_prefix, true );
   }

   return retval;
}

bool has_instance_in_global_storage( class_base& instance, const string& key )
{
   string persistence_extra( instance.get_persistence_extra( ) );

   string root_child_folder( persistence_extra );

   bool is_file_not_folder = global_storage_persistence_is_file( root_child_folder );

   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_read ods_bulk_read;

   ods_file_system& sys_ods_fs( system_ods_file_system( ) );

   sys_ods_fs.set_root_folder( root_child_folder );

   if( is_file_not_folder )
      return sys_ods_fs.has_file( key );
   else
      return sys_ods_fs.has_folder( key );
}

void fetch_keys_from_local_storage( class_base& instance,
 bool inclusive, size_t limit, vector< string >& instance_keys, bool in_reverse_order )
{
   string folder( instance.get_local_folder( ) );
   string origin( instance.get_local_origin( ) );
   string prefix( instance.get_local_prefix( ) );

   storage_ods_bulk_read ods_bulk_read;

   ods_file_system ofs( storage_ods_instance( ) );

   string expr;

   bool index_branch = false;

   if( folder.find( c_storage_folder_name_dot_idx ) == 0 )
   {
      index_branch = true;

      expr = string( c_sec_prefix_length, '0' ) + c_security_suffix;
   }

   expr += prefix;

   string start_from;

   if( !origin.empty( ) )
   {
      start_from = expr;

      if( !prefix.empty( ) )
         start_from += '\t';

      start_from += origin;
   }

   string suffix( instance.get_variable(
    get_special_var_name( e_special_var_local_suffix ) ) );

   if( suffix.empty( ) )
      expr += "*";
   else
      expr += suffix;

   // NOTE: First check if the folder exists
   // as it might not yet have been created.
   if( ofs.has_root_folder( folder ) )
   {
      ofs.set_root_folder( folder );

      ofs.list_files( expr, instance_keys, true, start_from, inclusive, limit, in_reverse_order );
   }

   if( index_branch )
   {
      for( size_t i = 0; i < instance_keys.size( ); i++ )
      {
         string next_link( instance_keys[ i ] );

         // NOTE: The link source will include the full
         // path so removes the path to leave the key.
         string source( ofs.link_source( next_link ) );

         string::size_type pos = source.rfind( '/' );

         if( pos != string::npos )
            source.erase( 0, pos + 1 );

         instance_keys[ i ] = source;
      }
   }
}

void fetch_keys_from_global_storage( class_base& instance,
 const string& start_from, bool inclusive, size_t limit, vector< string >& instance_keys, bool in_reverse_order )
{
   string persistence_extra( instance.get_persistence_extra( ) );

   string root_child_folder( persistence_extra );

   bool is_file_not_folder = global_storage_persistence_is_file( root_child_folder );

   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_read ods_bulk_read;

   ods_file_system& sys_ods_fs( system_ods_file_system( ) );

   sys_ods_fs.set_root_folder( root_child_folder );

   if( limit == 1 )
   {
      if( ( is_file_not_folder && sys_ods_fs.has_file( start_from ) )
       || ( !is_file_not_folder && sys_ods_fs.has_folder( start_from ) ) )
         instance_keys.push_back( start_from );
   }
   else
   {
      if( is_file_not_folder )
         sys_ods_fs.list_files( instance_keys, start_from, inclusive, limit, in_reverse_order );
      else
         sys_ods_fs.list_folders( instance_keys, start_from, inclusive, limit, in_reverse_order );
   }
}

void fetch_keys_from_system_variables( class_base& instance,
 const string& start_from, bool inclusive, size_t limit, vector< string >& instance_keys, bool in_reverse_order )
{
   string prefix, parent_key_value;
   string persistence_extra( instance.get_persistence_extra( ) );

   if( instance.get_graph_parent( ) )
      parent_key_value = instance.get_graph_parent( )->get_key( );

   if( persistence_extra == get_special_var_name( e_special_var_notifier ) )
   {
      persistence_extra = get_system_variable( get_special_var_name( e_special_var_opened_files ) );

      if( !parent_key_value.empty( ) )
         persistence_extra += '/' + parent_key_value;

      persistence_extra += '/';

      prefix = persistence_extra;

      persistence_extra += '?';
   }

   string sys_var_name( persistence_extra + '*' );

   if( in_reverse_order )
      sys_var_name += '*';

   string sys_vars( get_raw_system_variable( sys_var_name ) );

   if( !sys_vars.empty( ) )
   {
      vector< string > all_sys_vars;

      split( sys_vars, all_sys_vars, '\n' );

      size_t num_vars = all_sys_vars.size( );

      for( size_t i = 0; i < num_vars; i++ )
      {
         string next_var( all_sys_vars[ i ] );
         string next_value;

         string key( variable_name_from_name_and_value( next_var, &next_value ) );

         if( !next_value.empty( ) && next_value[ 0 ] == '[' )
         {
            string::size_type pos = next_value.find( ']' );

            if( pos == string::npos )
               throw runtime_error( "unexpected next_value '" + next_value + "' in fetch_keys_from_system_variables" );

            key = next_value.substr( 1, pos - 1 );
         }

         if( key.find( prefix ) == 0 )
            key.erase( 0, prefix.length( ) );

         if( !start_from.empty( ) )
         {
            bool skip = false;

            if( in_reverse_order )
            {
               if( key > start_from )
                  skip = true;
            }
            else
            {
               if( key < start_from )
                  skip = true;
            }

            if( key == start_from )
               skip = !inclusive;

            if( skip )
               continue;
         }

         instance_keys.push_back( key );

         if( limit && ( instance_keys.size( ) >= limit ) )
            break;
      }
   }
}

void fetch_instance_from_row_cache( class_base& instance, bool skip_after_fetch, int persistence_type )
{
   class_base_accessor instance_accessor( instance );

   if( instance_accessor.row_cache( ).empty( ) )
      throw runtime_error( "unexpected empty row cache" );

   instance_accessor.clear( );

   instance_accessor.set_key( instance_accessor.row_cache( )[ 0 ][ 0 ], true );

   instance_accessor.set_version( from_string< uint16_t >( instance_accessor.row_cache( )[ 0 ][ 1 ] ) );
   instance_accessor.set_revision( from_string< uint64_t >( instance_accessor.row_cache( )[ 0 ][ 2 ] ) );

   instance_accessor.set_security( from_string< uint64_t >( instance_accessor.row_cache( )[ 0 ][ 3 ] ) );

   instance_accessor.set_original_identity( instance_accessor.row_cache( )[ 0 ][ 4 ] );

   instance_accessor.set_original_revision( instance.get_revision( ) );

   if( persistence_type == 0 ) // i.e. SQL persistence
   {
      TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES,
       "(row cache for '" + instance.get_class_id( ) + ")"
       + string( !skip_after_fetch ? "" : " *** skip_after_fetch ***" ) );

      const map< int, int >& fields( instance_accessor.select_fields( ) );
      const vector< int >& columns( instance_accessor.select_columns( ) );

      TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES, "(from row cache)" );

      for( int i = c_num_sys_field_names; i < instance_accessor.row_cache( )[ 0 ].size( ); i++ )
      {
         if( !fields.count( columns[ i - c_num_sys_field_names ] ) )
            throw runtime_error( "unexpected field # not found for column #" + to_string( i - c_num_sys_field_names ) );

         int fnum = fields.find( columns[ i - c_num_sys_field_names ] )->second;

         TRACE_LOG( TRACE_VERBOSE | TRACE_QUERIES,
          "setting field #" + to_string( fnum + 1 ) + " to " + instance_accessor.row_cache( )[ 0 ][ i ] );

         instance.set_field_value( fnum, instance_accessor.row_cache( )[ 0 ][ i ] );
      }
   }
   else
   {
      for( int i = c_num_sys_field_names; i < instance_accessor.row_cache( )[ 0 ].size( ); i++ )
      {
         if( instance_accessor.field_nums( ).size( ) < ( i - c_num_sys_field_names ) )
            throw runtime_error( "unexpected field # not found for for column #" + to_string( i - c_num_sys_field_names ) );

         int fnum = instance_accessor.field_nums( )[ i - c_num_sys_field_names ];

         instance.set_field_value( fnum, instance_accessor.row_cache( )[ 0 ][ i ] );
      }
   }

   instance_accessor.row_cache( ).pop_front( );

   instance_accessor.after_fetch_from_db( );

   if( !skip_after_fetch )
      instance_accessor.perform_after_fetch( );
}

bool fetch_instance_from_local_storage(
 class_base& instance, const string& key_info,
 bool only_sys_fields, const vector< string >& field_names,
 vector< string >* p_columns = 0, bool skip_after_fetch = false )
{
   bool found = false;
   bool is_file_link = false;

   field_info_container field_info;
   instance.get_field_info( field_info );

   class_base_accessor instance_accessor( instance );

   storage_ods_bulk_read ods_bulk_read;

   ods_file_system ofs( storage_ods_instance( ) );

   string file_name( "/" );

   string key( key_info );

   string prefix;

   string::size_type pos = key_info.find( ' ' );

   if( pos == string::npos )
   {
      file_name += c_storage_folder_name_dot_dat;

      ofs.set_root_folder( c_storage_folder_name_dot_dat );
   }
   else
   {
      string field_id( key.substr( 0, pos ) );

      key.erase( 0, pos + 1 );

      file_name += c_storage_folder_name_dot_idx;

      ofs.set_root_folder( c_storage_folder_name_dot_idx );

      vector< pair< string, string > > all_index_pairs;

      instance.get_all_index_pairs( all_index_pairs );

      size_t num_index_pairs = all_index_pairs.size( );

      string field_name( instance_accessor.get_field_name( field_id ) );

      int index_num = -1;

      for( size_t i = 0; i < num_index_pairs; i++ )
      {
         pair< string, string > next_pair = all_index_pairs[ i ];

         if( next_pair.first == field_name )
         {
            index_num = i;
            break;
         }
      }

      if( index_num < 0 )
         throw runtime_error( "unable to find an index pair for '" + field_id + "'" );

      is_file_link = true;

      if( index_num < 10 )
         prefix += "0";

      prefix += to_string( index_num ) + '/';

      // NOTE: For unique index fetching an instance will only
      // currently be retrieved if is not security restricted.
      prefix += string( c_sec_prefix_length, '0' ) + c_security_suffix;
   }

   string class_id( instance.get_class_id( ) );

   if( ofs.has_folder( class_id ) )
   {
      ofs.set_folder( class_id );

      file_name += '/' + class_id;

      file_name += '/' + prefix + key;

      if( is_file_link || ( file_name.find( '\t' ) != string::npos ) )
         found = ofs.has_file( file_name );
      else
      {
         string suffix;

         found = ofs.has_file( file_name + '\t', true, &suffix );

         if( !suffix.empty( ) )
         {
            file_name += '\t' + suffix;

            pos = file_name.rfind( '/' );

            if( pos == string::npos )
               throw runtime_error( "unexpected file_name '"
                + file_name + "' in fetch_instance_from_local_storage" );

            key = file_name.substr( pos + 1 );
         }
      }

      if( found )
      {
         string::size_type pos = string::npos;

         string version_info, original_identity;

         // NOTE: The link source will include the full
         // path so removes this path in order to leave
         // just the source file name.
         if( is_file_link )
         {
            string source( ofs.link_source( file_name ) );

            pos = source.rfind( '/' );

            if( pos != string::npos )
               source.erase( 0, pos + 1 );

            key = source;
         }

         pos = key.find( '\t' );

         uint64_t security = 0;

         // NOTE: It is expected that the file name is
         // in the form of: <key>\t<sec>\t<typ>\t<ver>
         if( pos != string::npos )
         {
            string extras( key.substr( pos + 1 ) );

            if( !extras.empty( ) )
            {
               vector< string > all_extras;

               split( extras, all_extras, '\t' );

               security = from_string< uint64_t >( all_extras[ 0 ] );

               if( all_extras.size( ) > 1 )
                  original_identity = all_extras[ 1 ];

               if( all_extras.size( ) > 2 )
                  version_info = all_extras[ 2 ];
            }

            key.erase( pos );
         }

         pos = version_info.find( '.' );

         if( pos == string::npos )
            throw runtime_error( "unexpected version_info '"
             + version_info + "' in fetch_instance_from_local_storage" );

         string version( version_info.substr( 0, pos ) );
         string revision( version_info.substr( pos + 1 ) );

         if( p_columns )
         {
            p_columns->push_back( key );

            p_columns->push_back( version );
            p_columns->push_back( revision );

            p_columns->push_back( to_string( security ) );

            p_columns->push_back( original_identity );
         }
         else
         {
            instance_accessor.set_key( key, true );

            instance_accessor.set_version( from_string< int16_t >( version ) );
            instance_accessor.set_revision( from_string< int64_t >( revision ) );

            instance_accessor.set_security( security );

            instance_accessor.set_original_identity( original_identity );
            instance_accessor.set_original_revision( instance.get_revision( ) );
         }

         if( !only_sys_fields )
         {
            string data;

            stringstream sio_data;

            ofs.get_file( file_name, &sio_data );

            sio_reader reader( sio_data );

            int num_fields = field_names.size( );

            for( size_t i = 0; i < num_fields; i++ )
            {
               size_t field_num = instance.get_field_num( field_names[ i ] );

               if( instance.is_field_transient( field_num ) )
               {
                  if( p_columns )
                     p_columns->push_back( instance.get_field_value( field_num ) );

                  continue;
               }

               string attribute_name( lower( field_names[ i ] ) );

               data = unescaped( reader.read_attribute( attribute_name ), "trn\t\r\n" );

               if( p_columns )
                  p_columns->push_back( data );
               else
                  instance.set_field_value( field_num, data );
            }

            if( !p_columns )
            {
               instance_accessor.after_fetch_from_db( );

               if( !skip_after_fetch )
                  instance_accessor.perform_after_fetch( );
            }
         }
      }
   }

   return found;
}

bool fetch_instance_from_local_storage( class_base& instance, const string& key_info, bool only_sys_fields )
{
   field_info_container field_info;

   instance.get_field_info( field_info );

   vector< string > field_names;

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_info[ i ].is_transient )
         field_names.push_back( field_info[ i ].name );
   }

   return fetch_instance_from_local_storage( instance, key_info, only_sys_fields, field_names );
}

bool fetch_instance_from_global_storage( class_base& instance, const string& key,
 const vector< string >& field_names, vector< string >* p_columns = 0, bool skip_after_fetch = false )
{
   bool found = false;

   field_info_container field_info;
   instance.get_field_info( field_info );

   class_base_accessor instance_accessor( instance );
   string persistence_extra( instance.get_persistence_extra( ) );

   string root_child_folder( persistence_extra );
   bool is_file_not_folder( global_storage_persistence_is_file( root_child_folder ) );

   system_ods_fs_guard ods_fs_guard;

   system_ods_bulk_read ods_bulk_read;

   ods_file_system& sys_ods_fs( system_ods_file_system( ) );

   sys_ods_fs.set_root_folder( root_child_folder );

   if( is_file_not_folder )
      found = sys_ods_fs.has_file( key );
   else
      found = sys_ods_fs.has_folder( key );

   if( found )
   {
      if( !is_file_not_folder )
         sys_ods_fs.set_folder( key );

      if( p_columns )
      {
         p_columns->push_back( key );

         p_columns->push_back( "1" );
         p_columns->push_back( "0" );

         p_columns->push_back( "0" );

         p_columns->push_back( instance.get_module_id( ) + ':' + instance.get_class_id( ) );
      }
      else
      {
         instance_accessor.set_key( key, true );

         instance_accessor.set_version( 1 );
         instance_accessor.set_revision( 0 );

         instance_accessor.set_security( 0 );

         instance_accessor.set_original_revision( instance.get_revision( ) );
         instance_accessor.set_original_identity( instance.get_module_id( ) + ':' + instance.get_class_id( ) );
      }

      // NOTE: If is stored as a file then attributes are expected to be
      // in the format of a structured I/O file otherwise each attribute
      // is expected to be a file within the record's folder.
      if( !field_names.empty( ) )
      {
         stringstream sio_data;

         auto_ptr< sio_reader > ap_sio_reader;

         if( is_file_not_folder )
         {
            sys_ods_fs.get_file( key, &sio_data );

            ap_sio_reader.reset( new sio_reader( sio_data ) );
         }

         for( size_t i = 0; i < field_names.size( ); i++ )
         {
            string data, attribute_name( lower( field_names[ i ] ) );

            size_t field_num = instance.get_field_num( field_names[ i ] );

            if( instance.is_field_transient( field_num ) )
            {
               if( p_columns )
                  p_columns->push_back( instance.get_field_value( field_num ) );

               continue;
            }

            if( is_file_not_folder )
               data = ap_sio_reader->read_opt_attribute( attribute_name );
            else if( sys_ods_fs.has_file( attribute_name ) )
               sys_ods_fs.fetch_from_text_file( attribute_name, data );

            if( p_columns )
               p_columns->push_back( data );
            else
               instance.set_field_value( field_num, data );
         }
      }

      if( !p_columns )
      {
         instance_accessor.after_fetch_from_db( );

         if( !skip_after_fetch )
            instance_accessor.perform_after_fetch( );
      }
   }

   return found;
}

bool fetch_instance_from_global_storage( class_base& instance, const string& key )
{
   field_info_container field_info;

   instance.get_field_info( field_info );

   vector< string > field_names;

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_info[ i ].is_transient )
         field_names.push_back( field_info[ i ].name );
   }

   return fetch_instance_from_global_storage( instance, key, field_names );
}

bool fetch_instance_from_system_variable( class_base& instance, const string& key,
 const vector< string >& field_names, vector< string >* p_columns = 0, bool skip_after_fetch = false )
{
   bool found = false;

   field_info_container field_info;
   instance.get_field_info( field_info );

   class_base_accessor instance_accessor( instance );

   string prefix, parent_key_value;
   string persistence_extra( instance.get_persistence_extra( ) );

   string graph_parent_fk_field( instance.get_graph_parent_fk_field( ) );

   if( instance.get_graph_parent( ) )
   {
      parent_key_value = instance.get_graph_parent( )->get_key( );

      const char* p_str = instance_accessor.get_field_name( graph_parent_fk_field );

      if( p_str )
         graph_parent_fk_field = string( p_str );
   }

   bool is_indirect_key = false;

   if( persistence_extra == get_special_var_name( e_special_var_notifier ) )
   {
      is_indirect_key = true;

      persistence_extra = string( c_notifier_prefix )
       + get_system_variable( get_special_var_name( e_special_var_opened_files ) );

      if( !parent_key_value.empty( ) )
         persistence_extra += '/' + parent_key_value;

      persistence_extra += '/';
   }

   string key_value( persistence_extra + key );

   string row_data( get_raw_system_variable( key_value ) );

   if( is_indirect_key )
   {
      string indirect_row_data( get_raw_system_variable( row_data ) );

      string prefix( '[' + key + ']' );

      if( indirect_row_data.find( prefix ) == 0 )
         indirect_row_data.erase( 0, prefix.length( ) );

      if( !indirect_row_data.empty( ) && ( indirect_row_data[ 0 ] == c_notifier_ignore_char ) )
         indirect_row_data.erase( 0, 1 );

      if( !indirect_row_data.empty( ) && ( indirect_row_data[ 0 ] == c_notifier_select_char ) )
         indirect_row_data.erase( 0, 1 );

      if( !indirect_row_data.empty( ) && ( indirect_row_data[ 0 ] == c_notifier_unselect_char ) )
         indirect_row_data.erase( 0, 1 );

      if( persistence_extra.find( c_notifier_prefix ) == 0 )
         persistence_extra.erase( 0, strlen( c_notifier_prefix ) );

      if( !indirect_row_data.empty( ) )
         row_data += '|' + indirect_row_data;
   }

   if( row_data.empty( ) )
      found = false;
   else
   {
      found = true;

      if( p_columns )
      {
         p_columns->push_back( key );

         p_columns->push_back( "1" );
         p_columns->push_back( "0" );

         p_columns->push_back( "0" );

         p_columns->push_back( instance.get_module_id( ) + ':' + instance.get_class_id( ) );
      }
      else
      {
         instance_accessor.set_key( key, true );

         instance_accessor.set_version( 1 );
         instance_accessor.set_revision( 0 );

         instance_accessor.set_security( 0 );

         instance_accessor.set_original_revision( instance.get_revision( ) );
         instance_accessor.set_original_identity( instance.get_module_id( ) + ':' + instance.get_class_id( ) );
      }

      if( !field_names.empty( ) )
      {
         size_t num = 0;
         vector< string > columns;

         split( row_data, columns, '|' );

         for( size_t i = 0; i < field_names.size( ); i++ )
         {
            string data;

            size_t field_num = instance.get_field_num( field_names[ i ] );

            if( instance.is_field_transient( field_num ) )
            {
               if( p_columns )
                  p_columns->push_back( instance.get_field_value( field_num ) );

               continue;
            }

            if( field_names[ i ] == graph_parent_fk_field )
               data = parent_key_value;
            else if( num < columns.size( ) )
            {
               data = columns[ num++ ];

               if( is_indirect_key && data.find( persistence_extra ) == 0 )
                  data.erase( 0, persistence_extra.length( ) );
            }

            if( p_columns )
               p_columns->push_back( data );
            else
               instance.set_field_value( field_num, data );
         }
      }

      if( !p_columns )
      {
         instance_accessor.after_fetch_from_db( );

         if( !skip_after_fetch )
            instance_accessor.perform_after_fetch( );
      }
   }

   return found;
}

bool fetch_instance_from_system_variable( class_base& instance, const string& key )
{
   field_info_container field_info;
   instance.get_field_info( field_info );

   vector< string > field_names;

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_info[ i ].is_transient )
         field_names.push_back( field_info[ i ].name );
   }

   return fetch_instance_from_system_variable( instance, key, field_names );
}

inline void perform_op_cancel( class_base& instance, class_base::op_type op )
{
   class_base_accessor instance_accessor( instance );

   // FUTURE: It would be more efficient not to call "release_locks_for_owner" unless
   // it is known that cascade locks had actually been obtained when the destroy began.
   if( !instance.get_is_being_cascaded( ) && ( op == class_base::e_op_type_destroy ) )
      release_instance_locks( instance, true );

   instance_accessor.cancel( );
}

}

mutex& get_mutex_for_ciyam_base_ops( )
{
   return g_mutex;
}

string exec_bulk_ops( const string& module,
 const string& uid, const string& dtm, const string& mclass,
 const string& filename, const string& export_fields, const string& tz_name, bool destroy_records,
 const string& search_text, const string& search_query, const string& fixed_field_values, command_handler& handler )
{
   string response;

   string module_id( module_identity( module ) );

   string class_id = get_class_id_for_id_or_name( module_id, mclass );

   bool is_using_verbose_logging = get_storage_using_verbose_logging( );

   ifstream inpf;
   ofstream outf;

   size_t line = 1;
   time_t ts( time( 0 ) );

   bool in_trans = false;
   bool is_export = false;

   if( !export_fields.empty( ) )
   {
      is_export = true;

      outf.open( filename.c_str( ), ios::out );

      if( !outf )
         throw runtime_error( "unable to open '" + filename + "' for output" );
   }
   else
   {
      inpf.open( filename.c_str( ), ios::in );

      if( !inpf )
         throw runtime_error( "unable to open '" + filename + "' for input" );

      string log_file_name( filename.substr( 0, filename.find_last_of( "." ) ) + c_log_file_ext );

      outf.open( log_file_name.c_str( ), ios::out );

      if( !outf )
         throw runtime_error( "unable to open '" + log_file_name + "' for output" );
   }

   size_t handle = create_object_instance( module_id, class_id, 0, false );

   try
   {
      set< int > non_nulls;

      vector< string > fields;
      vector< string > column_names;
      vector< string > original_fields;

      if( is_export )
      {
         if( !fixed_field_values.empty( ) )
         {
            class_base& instance( get_class_base_from_handle( handle, "" ) );

            instance.set_variable( get_special_var_name( e_special_var_fixed_field_values ), fixed_field_values );
         }

         if( export_fields == "*" )
         {
            fields.push_back( c_key_field );
            original_fields.push_back( c_key_field );

            get_all_field_names( handle, "", fields );
            get_all_field_names( handle, "", column_names );
            get_all_field_names( handle, "", original_fields );
         }
         else
         {
            split( export_fields, fields );

            for( size_t i = 0; i < fields.size( ); i++ )
            {
               string next_field( fields[ i ] );
               string next_column;

               if( !next_field.empty( ) && ( next_field[ 0 ] == '!' ) )
               {
                  non_nulls.insert( i );
                  next_field.erase( 0, 1 );
               }

               string::size_type pos = next_field.find( ':' );

               if( pos == string::npos )
                  next_column = next_field;
               else
               {
                  next_column = next_field.substr( pos + 1 );
                  next_field.erase( pos );

                  fields[ i ] = next_field;
               }

               column_names.push_back( next_column );
               original_fields.push_back( next_field );
            }
         }

         set_any_field_ids_to_names( handle, "", fields );

         string fields_for_iteration;

         string key_info;

         for( size_t i = 0; i < original_fields.size( ); i++ )
         {
            if( original_fields[ i ] != c_key_field )
            {
               // NOTE: Use the first field to determine the order (if the first
               // field was the key then no need to add any ordering information).
               if( i == 0 )
                  key_info = original_fields[ i ] + " ";

               if( !fields_for_iteration.empty( ) )
                  fields_for_iteration += ",";

               fields_for_iteration += original_fields[ i ];
            }

            if( i > 0 )
               outf << ",";

            outf << column_names[ i ];
         }

         outf << "\n";

         if( instance_iterate( handle, "", key_info,
          fields_for_iteration, search_text, search_query, e_iter_direction_forwards, true ) )
         {
            do
            {
               if( !non_nulls.empty( ) )
               {
                  bool okay = true;

                  for( set< int >::iterator i = non_nulls.begin( ); i != non_nulls.end( ); ++i )
                  {
                     if( get_field_value( handle, "", fields[ *i ] ).empty( ) )
                     {
                        okay = false;
                        break;
                     }
                  }

                  if( !okay )
                     continue;
               }

               outf << get_field_values( handle, "", fields, tz_name, false, true ) << "\n";

            } while( instance_iterate_next( handle, "" ) );
         }
      }
      else
      {
         set_uid( uid );
         set_dtm( dtm );

         set_tz_name( tz_name );

         string log_lines;

         vector< string > key_fields;

         set< string > sorted_key_fields;

         vector< string > fixed_fields;
         vector< string > fixed_values;

         if( !fixed_field_values.empty( ) )
         {
            vector< string > fixed_field_value_pairs;

            split( fixed_field_values, fixed_field_value_pairs );

            for( size_t i = 0; i < fixed_field_value_pairs.size( ); i++ )
            {
               string::size_type pos = fixed_field_value_pairs[ i ].find( '=' );

               if( pos == string::npos )
                  throw runtime_error( "unexpected field=value pair format '" + fixed_field_value_pairs[ i ] + "'" );

               fixed_fields.push_back( fixed_field_value_pairs[ i ].substr( 0, pos ) );
               fixed_values.push_back( fixed_field_value_pairs[ i ].substr( pos + 1 ) );
            }
         }

         string next;

         size_t errors = 0;

         bool is_first = true;
         bool can_fetch = false;
         bool has_key_field = false;
         bool key_prefix_is_first = false;

         size_t num_created = 0;
         size_t num_skipped = 0;
         size_t num_updated = 0;

         size_t num_destroyed = 0;

         size_t key_field_num = 0;

         size_t transaction_id = 0;

         string last_suffixed_key;

         string key_suffix( get_raw_session_variable(
          get_special_var_name( e_special_var_key_suffix ) ) );

         if( !get_raw_session_variable(
          get_special_var_name( e_special_var_key_prefix_is_first ) ).empty( ) )
            key_prefix_is_first = true;

         while( getline( inpf, next ) )
         {
            remove_trailing_cr_from_text_file_line( next, is_first );

            if( is_first )
            {
               is_first = false;

               // NOTE: UTF-8 text files will often begin with an identifying sequence "EF BB BF" as the
               // first three characters of the file so if the first byte is "EF" assume UTF-8 and strip.
               if( next.size( ) >= 3 && next[ 0 ] == ( char )0xef )
                  next.erase( 0, 3 );

               if( split( next, fields ) < 1 )
                  throw runtime_error( "cannot import without at least one field name" );

               set_any_field_names_to_ids( handle, "", fields );
               set_any_field_names_to_ids( handle, "", fixed_fields );

               // NOTE: Incorrerct field names are detected by attempting to convert
               // all the names that were changed to ids back to names and comparing.
               vector< string > test_fields( fields );

               set_any_field_ids_to_names( handle, "", test_fields );

               for( size_t i = 0; i < fields.size( ); i++ )
               {
                  if( fields[ i ] == c_key_field )
                  {
                     key_field_num = i;
                     has_key_field = true;
                  }
                  else if( fields[ i ] == c_ignore_field )
                     continue;
                  else if( fields[ i ] == test_fields[ i ] )
                     // FUTURE: This needs to be implemented as a string message.
                     throw runtime_error( "Unknown field '"
                      + fields[ i ] + "' for '" + instance_class( handle, "" ) + "' records." );
               }

               vector< string > test_fixed_fields( fixed_fields );

               set_any_field_ids_to_names( handle, "", test_fixed_fields );

               for( size_t i = 0; i < fixed_fields.size( ); i++ )
               {
                  // FUTURE: This needs to be implemented as a string message.
                  if( fixed_fields[ i ] == test_fixed_fields[ i ] )
                     throw runtime_error( "Unknown field '"
                      + fixed_fields[ i ] + "' for " + instance_class( handle, "" ) + " records." );
               }

               if( has_key_field || key_prefix_is_first )
                  can_fetch = true;
               else if( determine_alternative_key_fields( handle, "", fields, key_fields ) )
               {
                  can_fetch = true;

                  for( size_t i = 0; i < key_fields.size( ); i++ )
                     sorted_key_fields.insert( key_fields[ i ] );
               }

               continue;
            }

            // FUTURE: This needs to be implemented as a string message.
            if( !can_fetch && destroy_records )
               throw runtime_error( "Cannot destroy records without providing their key fields." );

            ++line;

            if( next.empty( ) )
               continue;

            bool destroy_record = destroy_records;

            size_t num_values = 0;

            vector< string > values;

            size_t continuation_offset = 0;

            bool last_value_incomplete = false;

            while( true )
            {
               num_values = split_csv_values( next, values, last_value_incomplete, continuation_offset );

               if( !last_value_incomplete )
                  break;

               next = values.back( );

               values.pop_back( );

               string continuation;

               if( !getline( inpf, continuation ) )
                  throw runtime_error( "unexpected incomplete record found at line #" + to_string( line ) );

               // NOTE: In case the string file has ended up with CRLFs remove the trailing CR.
               if( continuation.size( ) && ( continuation[ continuation.size( ) - 1 ] == '\r' ) )
                  continuation.erase( continuation.size( ) - 1 );

               continuation_offset = next.size( ) + 1;

               ++line;

               next += '\n' + continuation;
            }

            if( num_values != fields.size( ) )
            {
               // FUTURE: This needs to be implemented as a string message.
               string message( "Wrong number of field values (expected "
                + to_string( fields.size( ) ) + " but found " + to_string( num_values )
                + ") in import file line #" + to_string( line ) + "." );

               // NOTE: If the second line is incorrect then is assuming all will fail.
               if( line == 2 )
                  throw runtime_error( message );
               else
               {
                  outf << "Error: " << message << endl;

                  ++errors;
                  continue;
               }
            }

            string key;

            if( !in_trans )
            {
               in_trans = true;

               transaction_start( );

               transaction_id = next_transaction_id( );
            }

            string next_log_line;

            bool found_instance = false;
            bool skipped_instance = false;
            bool skipping_fk_checks = false;

            if( !key_suffix.empty( ) )
            {
               bool has_key_suffix = false;

               string key_value;

               if( has_key_field )
               {
                  key_value = values[ key_field_num ];

                  string::size_type pos = key_value.find( key_suffix );

                  if( pos != string::npos )
                  {
                     if( pos + key_suffix.length( ) == key_value.length( ) )
                     {
                        has_key_suffix = true;
                        last_suffixed_key = key;
                     }
                     else if( !last_suffixed_key.empty( ) )
                     {
                        if( key_value.find( last_suffixed_key ) == 0 )
                           has_key_suffix = true;
                     }
                  }
               }

               if( !has_key_suffix && !key_prefix_is_first )
                  throw runtime_error( "key suffix '" + key_suffix + "' not found in key value '" + key_value + "'" );
            }

            if( can_fetch )
            {
               string key_info;

               if( has_key_field )
                  key_info = values[ key_field_num ];
               else if( key_prefix_is_first )
                  key_info = values[ 0 ] + key_suffix;
               else
               {
                  size_t num_fixed = 0;

                  string key_info_fields;
                  string key_info_values;

                  for( size_t i = 0; i < num_values; i++ )
                  {
                     if( sorted_key_fields.count( fields[ i ] ) )
                     {
                        ++num_fixed;

                        if( !key_info_fields.empty( ) )
                           key_info_fields += ",";
                        key_info_fields += fields[ i ];

                        if( !key_info_values.empty( ) )
                           key_info_values += ",";
                        key_info_values += escape( values[ i ], "," );
                     }
                  }

                  key_info = key_info_fields + "#" + to_string( num_fixed ) + " " + key_info_values;
               }

               instance_fetch_rc rc;
               instance_fetch( handle, "", key_info, &rc );

               if( rc == e_instance_fetch_rc_okay )
               {
                  found_instance = true;

                  string key_info( instance_key_info( handle, "" ) );
                  key = key_info.substr( 0, key_info.find( ' ' ) );

                  if( !destroy_record )
                  {
                     op_update_rc rc;
                     op_instance_update( handle, "", key, "", false, &rc );

                     if( rc == e_op_update_rc_okay )
                        next_log_line = c_cmd_update;
                     else
                     {
                        // FUTURE: These need to be implemented as string messages.
                        if( rc == e_op_update_rc_locked )
                           outf << "Error: Processing line #" << line << " - unable to lock for update." << endl;
                        else if( rc == e_op_update_rc_not_found )
                           outf << "Error: Processing line #" << line << " - record not found for update." << endl;
                        else
                           throw runtime_error( "unexpected op_update rc #" + to_string( rc ) );

                        ++errors;
                        continue;
                     }
                  }
                  else
                  {
                     op_destroy_rc rc;
                     op_instance_destroy( handle, "", key, "", false, &rc );

                     if( rc == e_op_destroy_rc_okay )
                        next_log_line = c_cmd_destroy;
                     else
                     {
                        // FUTURE: These need to be implemented as string messages.
                        if( rc == e_op_destroy_rc_locked )
                           outf << "Error: Processing line #" << line << " - unable to lock for destroy." << endl;
                        else if( rc == e_op_destroy_rc_not_found )
                           outf << "Error: Processing line #" << line << " - record not found for destroy." << endl;
                        else if( rc == e_op_destroy_rc_constrained )
                           outf << "Error: Processing line #" << line << " - record cannot be destroyed due to being used by other records." << endl;
                        else
                           throw runtime_error( "unexpected op_destroy rc #" + to_string( rc ) );

                        ++errors;
                        continue;
                     }
                  }
               }
            }

            if( !destroy_record && !found_instance )
            {
               if( !has_key_field )
               {
                  if( !key_prefix_is_first )
                     key = gen_key( );
                  else
                  {
                     key = values[ 0 ];
                     key += key_suffix;
                  }
               }
               else
                  key = values[ key_field_num ];

               op_create_rc rc;
               op_instance_create( handle, "", key, false, &rc );

               if( rc == e_op_create_rc_okay )
                  next_log_line = c_cmd_create;
               else
               {
                  // FUTURE: These need to be implemented as string messages.
                  if( rc == e_op_create_rc_locked )
                     outf << "Error: Processing line #" << line << " - unable to lock for create." << endl;
                  else if( rc == e_op_create_rc_exists )
                     outf << "Error: Processing line #" << line << " - cannot create duplicate record." << endl;
                  else
                     throw runtime_error( "unexpected op_create rc #" + to_string( rc ) );

                  ++errors;
                  continue;
               }
            }

            string class_id_to_log( class_id );

            if( !is_using_verbose_logging && class_id_to_log.find( module_id ) == 0 )
               class_id_to_log.erase( 0, module_id.length( ) );

            next_log_line += " " + uid + " " + dtm + " " + module_id + " " + class_id_to_log + " " + key;

            if( !destroy_record )
            {
               next_log_line += " ";

               string log_field_value_pairs;

               for( size_t i = 0; i < num_values; i++ )
               {
                  if( ( has_key_field && ( i == key_field_num ) ) || ( fields[ i ] == c_ignore_field ) )
                     continue;

                  bool is_transient = false;
                  bool was_date_time = false;

                  string type_name( get_field_type_name( handle, "", fields[ i ], &is_transient ) );

                  if( ( type_name == "date_time" ) || ( type_name == "tdatetime" ) )
                  {
                     was_date_time = true;

                     if( !values[ i ].empty( ) )
                     {
                        // NOTE: If a date_time string starts with 'U' then it is considered as already being UTC.
                        if( values[ i ][ 0 ] == 'U' )
                           values[ i ].erase( 0, 1 );
                        else if( !tz_name.empty( ) )
                           values[ i ] = local_to_utc( date_time( values[ i ] ), tz_name ).as_string( );
                     }
                  }

                  string value, method_name_and_args( "get " );

                  if( !is_using_verbose_logging )
                  {
                     method_name_and_args += fields[ i ];

                     value = execute_object_command( handle, "", method_name_and_args );

                     if( !value.empty( ) && was_date_time )
                     {
                        method_name_and_args = "cmd " + fields[ i ] + " raw";
                        value = execute_object_command( handle, "", method_name_and_args );
                     }
                  }

                  // NOTE: If not using verbose logging then unchanged field values will be omitted.
                  if( is_using_verbose_logging || ( value != values[ i ] ) )
                  {
                     string method_name_and_args( "set " );

                     method_name_and_args += fields[ i ] + " ";

                     method_name_and_args += "\"" + escaped( escaped( values[ i ] ), "\"", c_nul ) + "\"";

                     execute_object_command( handle, "", method_name_and_args );

                     if( !log_field_value_pairs.empty( ) )
                        log_field_value_pairs += ",";

                     string field_id_to_log( fields[ i ] );

                     if( !is_using_verbose_logging && field_id_to_log.find( module_id ) == 0 )
                     {
                        field_id_to_log.erase( 0, module_id.length( ) );

                        if( field_id_to_log.find( class_id_to_log ) == 0 )
                           field_id_to_log.erase( 0, class_id_to_log.length( ) );
                     }

                     log_field_value_pairs += field_id_to_log + "="
                      + search_replace( escaped( escaped( values[ i ] ), "\"", c_esc, "rn\r\n" ), ",", "\\\\," );
                  }
               }

               for( size_t i = 0; i < fixed_fields.size( ); i++ )
               {
                  string method_name_and_args( "set " );

                  method_name_and_args += fixed_fields[ i ] + " ";

                  method_name_and_args += "\"" + escaped( escaped( fixed_values[ i ] ), "\"", c_nul ) + "\"";

                  if( !log_field_value_pairs.empty( ) )
                     log_field_value_pairs += ",";

                  string field_id_to_log( fixed_fields[ i ] );

                  if( !is_using_verbose_logging && field_id_to_log.find( module_id ) == 0 )
                  {
                     field_id_to_log.erase( 0, module_id.length( ) );

                     if( field_id_to_log.find( class_id_to_log ) == 0 )
                        field_id_to_log.erase( 0, class_id_to_log.length( ) );
                  }

                  log_field_value_pairs += field_id_to_log + "="
                   + search_replace( escaped( escaped( fixed_values[ i ] ), "\"", c_esc, "rn\r\n" ), ",", "\\\\," );

                  execute_object_command( handle, "", method_name_and_args );
               }

               next_log_line += "\"" + log_field_value_pairs + "\"";
            }

            if( found_instance && !destroy_record && !instance_has_changed( handle, "" ) )
            {
               skipped_instance = true;

               op_instance_cancel( handle, "", false );
            }
            else
            {
               op_apply_rc rc;

               op_instance_apply( handle, "", false, &rc );

               if( rc != e_op_apply_rc_okay )
               {
                  class_base& instance( get_class_base_from_handle( handle, "" ) );

                  string validation_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );

                  op_instance_cancel( handle, "", false );

                  // FUTURE: These need to be implemented as string messages.
                  if( rc == e_op_apply_rc_locked )
                     outf << "Error: Processing line #" << line << " - record was locked." << endl;
                  else if( rc == e_op_apply_rc_invalid )
                  {
                     outf << "Error: Processing line #" << line << " - record was invalid.";
                     if( !validation_error.empty( ) )
                        outf << ' ' << validation_error;

                     outf << endl;
                  }
                  else
                     throw runtime_error( "unexpected op_apply rc #" + to_string( rc ) );

                  ++errors;

                  continue;
               }

               if( !log_lines.empty( ) )
                  log_lines += "\n";

               log_lines += next_log_line;
            }

            if( log_lines.empty( ) )
            {
               if( is_condemned_session( ) )
                  break;
            }
            else
            {
               if( time( 0 ) - ts >= 10 )
               {
                  ts = time( 0 );

                  transaction_log_command( log_lines );

                  transaction_commit( );

                  in_trans = false;

                  log_lines.clear( );

                  // FUTURE: This message should be handled as a server string message.
                  handler.output_progress( "Processed " + to_string( line ) + " lines..." );

                  if( is_condemned_session( ) )
                     break;
               }
            }

            if( found_instance )
            {
               if( skipped_instance )
                  ++num_skipped;
               else if( !destroy_record )
                  ++num_updated;
               else
                  ++num_destroyed;
            }
            else
               ++num_created;
         }

         if( in_trans )
         {
            transaction_log_command( log_lines );

            transaction_commit( );
         }

         // FUTURE: These should be handled as string messages.
         if( num_created )
            outf << "Created " << num_created << " new record(s)." << endl;
         if( num_skipped )
            outf << "Skipped " << num_skipped << " existing record(s)." << endl;
         if( num_updated )
            outf << "Updated " << num_updated << " existing record(s)." << endl;
         if( num_destroyed )
            outf << "Removed " << num_destroyed << " existing record(s)." << endl;

         response = "Processed " + to_string( line ) + " lines with " + to_string( errors ) + " error(s).";
      }

      destroy_object_instance( handle );
   }
   catch( exception& x )
   {
      if( !is_export )
         // FUTURE: This message should be handled as a server string message.
         outf << "Error: Processing line #" << line << " - " << x.what( ) << endl;

      if( in_trans )
         transaction_rollback( );

      destroy_object_instance( handle );

      throw;
   }
   catch( ... )
   {
      if( !is_export )
         // FUTURE: This message should be handled as a server string message.
         outf << "Error: Processing line #" << line << " - unknown exception caught" << endl;

      if( in_trans )
         transaction_rollback( );

      destroy_object_instance( handle );

      throw;
   }

   return response;
}

void begin_instance_op( instance_op op, class_base& instance,
 const string& key, bool internal_modification, instance_op_rc* p_rc )
{
   check_storage( );

   if( p_rc )
      *p_rc = e_instance_op_rc_okay;

   if( op == e_instance_op_none )
      return;

   if( is_condemned_session( ) )
   {
      string stop_condemned( get_raw_system_variable(
       get_special_var_name( e_special_var_stop_condemned ) ) );

      if( ( stop_condemned == c_all )
       || ( stop_condemned == to_string( session_id( ) ) ) )
         throw runtime_error( "*** condemned session has been stopped ***" );
   }

   TRACE_LOG( TRACE_INITIAL | TRACE_OBJECTS, "begin (enter) op = "
    + instance_op_name( op ) + ", class = " + instance.get_class_name( )
    + ", internal = " + to_string( internal_modification ) + ", key = " + key );

   string sql;

   bool is_cascade_op = false;

   string key_in_use( instance.get_key( ) );

   string key_for_op( key );

   if( key_for_op.empty( ) )
      key_for_op = key_in_use;

   if( key_for_op.empty( ) )
      throw runtime_error( "cannot perform an instance operation without a key or current instance" );

   string clone_key;

   if( op == e_instance_op_create )
   {
      string::size_type pos = key_for_op.find( ' ' );

      if( pos != string::npos )
      {
         clone_key = key.substr( pos + 1 );
         key_for_op = key.substr( 0, pos );
      }

      if( key_for_op.find_first_of( c_invalid_key_characters ) != string::npos )
         throw runtime_error( get_string_message(
          GS( c_str_key_invalid ), make_pair( c_str_parm_key_invalid_key, key_for_op ) ) );
   }

   size_t lock_handle = 0;
   size_t xlock_handle = 0;

   class_base_accessor instance_accessor( instance );

   if( instance.get_is_in_op( ) && !instance_accessor.get_in_op_begin( ) )
      throw runtime_error( "cannot begin an instance operation whilst already perfoming an instance operation" );

   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );
   string lock_class_id( instance.get_lock_class_id( ) );

   int persistence_type = instance.get_persistence_type( );

#ifdef COMPILE_PROTOTYPE_CODE
   string app_name( storage_name( ) );

   if( app_name == c_meta_storage_name )
      persistence_type = 1;
#endif

   bool is_minimal_update = ( op == e_instance_op_update ) && !instance_accessor.fetch_field_names( ).empty( );

   if( is_minimal_update )
      TRACE_LOG( TRACE_INITIAL | TRACE_OBJECTS, "*** minimal update ***" );

   // NOTE: A create op can be started (but not applied) without an instance key (this is to help with record
   // preparation when cloning in order to create a new instance).
   if( ( op != e_instance_op_create ) || !key_for_op.empty( ) )
   {
      bool obtained_lock = false;

      if( ( op == e_instance_op_create )
       && ( lock_handle = obtain_instance_lock( c_create_lock_name, instance, 0, &key_for_op ) ) )
         obtained_lock = true;

      if( ( op == e_instance_op_review )
       && ( lock_handle = obtain_instance_lock( c_review_lock_name, instance, 0, &key_for_op ) ) )
         obtained_lock = true;

      if( ( op == e_instance_op_update ) || ( op == e_instance_op_destroy ) )
      {
         if( instance.get_is_being_cascaded( ) )
         {
            obtained_lock = true;
            is_cascade_op = true;
         }

         if( !obtained_lock )
         {
            if( ( op == e_instance_op_update )
             && ( lock_handle = obtain_instance_lock( c_update_lock_name, instance, 0, &key_for_op ) ) )
               obtained_lock = true;

            if( ( op == e_instance_op_destroy )
             && ( lock_handle = obtain_instance_lock( c_destroy_lock_name, instance, 0, &key_for_op ) ) )
               obtained_lock = true;
         }
      }

      if( !obtained_lock )
      {
         if( p_rc )
         {
            *p_rc = e_instance_op_rc_locked;
            return;
         }
         else
            throw runtime_error( "unable to obtain lock for '" + key_for_op + "'" );
      }
   }

   if( op == e_instance_op_create )
   {
      // NOTE: If cloning then retrieve the instance values of the clone source under a review lock. This review lock
      // will be held until the create operation is finished so that link locks to any foreign keys are not required.
      if( !clone_key.empty( ) )
      {
         if( has_locked_instance( lock_class_id, clone_key )
          || ( xlock_handle = obtain_instance_lock( c_review_lock_name, instance, 0, &clone_key ) ) )
         {
            guard g( g_mutex, "begin_instance_op" );

            storage_scoped_lock_holder xlock_holder( xlock_handle );

            instance_accessor.set_key( clone_key, true );

            bool found = false;

            if( persistence_type == 0 ) // i.e. SQL persistence
            {
               instance_accessor.fetch( sql, false );
               found = fetch_instance_from_db( instance, sql );
            }
            else if( persistence_type == 1 ) // i.e. ODS local persistence
               found = fetch_instance_from_local_storage( instance, clone_key, false );
            else if( persistence_type == 2 ) // i.e. ODS global persistence
               found = fetch_instance_from_global_storage( instance, clone_key );
            else
               throw runtime_error( "unexpected persistence type #" + to_string( persistence_type ) + " for cloning in begin_instance_op" );

            xlock_holder.release( );

            if( !found )
            {
               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_not_found;
                  return;
               }
               else
                  throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                   make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
                   make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
            }
         }
         else
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_locked;
               return;
            }
            else
               throw runtime_error( "unable to obtain lock for '" + clone_key + "'" );
         }
      }
      else
         instance.init( true );
   }

   instance_accessor.set_key( key_for_op, true );
   instance_accessor.set_clone_key( clone_key );

   instance_accessor.set_in_op_begin( true );

   instance_accessor.set_is_after_store( false );

   if( op != e_instance_op_update )
      instance_accessor.fetch_field_names( ).clear( );

   class_base::op_type old_op( instance.get_op( ) );

   if( op == e_instance_op_review )
      instance_accessor.set_op( class_base::e_op_type_review, key_for_op != key_in_use );
   else if( op == e_instance_op_create )
      instance_accessor.set_op( class_base::e_op_type_create, true );
   else if( op == e_instance_op_update )
      instance_accessor.set_op( class_base::e_op_type_update, key_for_op != key_in_use );
   else if( op == e_instance_op_destroy )
      instance_accessor.set_op( class_base::e_op_type_destroy, key_for_op != key_in_use );

   try
   {
      storage_scoped_lock_holder lock_holder( lock_handle );
      storage_scoped_lock_holder xlock_holder( xlock_handle );

      if( op == e_instance_op_create )
      {
         string sql;

         // NOTE: Skip this check during a restore as an optimsation to reduce DB ops.
         if( !storage_locked_for_admin( ) || !session_skip_fk_fetches( ) )
         {
            bool found = false;

            if( persistence_type == 0 ) // i.e. SQL persistence
            {
               instance_accessor.fetch( sql, true );
               found = fetch_instance_from_db( instance, sql, true );
            }
            else if( persistence_type == 1 ) // i.e. ODS local persistence
               found = has_instance_in_local_storage( instance, key_for_op );
            else if( persistence_type == 2 ) // i.e. ODS global persistence
               found = has_instance_in_global_storage( instance, key_for_op );
            else
               throw runtime_error( "unexpected persistence type #" + to_string( persistence_type ) + " in begin_instance_op" );

            if( found )
            {
               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_already_exists;

                  instance_accessor.set_op( old_op, false );
                  instance_accessor.set_in_op_begin( false );

                  return;
               }
               else
                  throw runtime_error( get_string_message( GS( c_str_record_exists ),
                   make_pair( c_str_parm_record_exists_class, instance.get_display_name( ) ) ) );
            }
         }

         instance_accessor.at_create( );
      }
      else if( ( op == e_instance_op_review ) || ( op == e_instance_op_update ) )
      {
         if( instance.get_is_for_peer( ) )
         {
            if( op == e_instance_op_update )
               instance_accessor.clear( );
            else
               throw runtime_error( "unexpected review op for peer record in begin_instance_op" );
         }
         else
         {
            bool found = false;

            if( persistence_type == 0 ) // i.e. SQL persistence
            {
               string sql;

               instance_accessor.fetch( sql, false, false );

               found = fetch_instance_from_db( instance, sql,
                false, ( is_minimal_update && ( op == e_instance_op_update ) ) );
            }
            else if( persistence_type == 1 ) // i.e. ODS local persistence
               found = fetch_instance_from_local_storage( instance, key_for_op, false );
            else if( persistence_type == 2 ) // i.e. ODS global persistence
            {
               found = fetch_instance_from_global_storage( instance, key_for_op );
               instance_accessor.set_original_identity( instance.get_current_identity( ) );
            }
            else
               throw runtime_error( "unexpected persistence type #" + to_string( persistence_type ) + " in begin_instance_op" );

            if( !found )
            {
               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_not_found;

                  instance_accessor.set_op( old_op, false );
                  instance_accessor.set_in_op_begin( false );

                  return;
               }
               else
                  throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                   make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
                   make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
            }

            string ver_expected( instance_accessor.get_ver_exp( ) );

            if( !ver_expected.empty( ) && ver_expected != instance.get_version_info( ) )
               throw runtime_error( get_string_message( GS( c_str_version_mismatch ),
                make_pair( c_str_parm_version_mismatch_found, instance.get_version_info( ) ),
                make_pair( c_str_parm_version_mismatch_expected, ver_expected ) ) );

            if( ( op == e_instance_op_update ) && !storage_locked_for_admin( )
             && ( !internal_modification
             && ( instance.get_state( ) & c_state_uneditable )
             && !( instance.get_state( ) & c_state_ignore_uneditable ) ) )
               throw runtime_error( get_string_message( GS( c_str_cannot_update ),
                make_pair( c_str_parm_cannot_update_class, instance.get_display_name( ) ) ) );

            if( ( op == e_instance_op_update )
             && ( instance.get_current_identity( ) != instance.get_original_identity( ) ) )
               throw runtime_error( "cannot update '" + instance.get_original_identity( )
                + "' stored instance using '" + instance.get_current_identity( ) + "' object instance" );
         }
      }
      else if( !instance.get_is_for_peer( ) && ( op == e_instance_op_destroy ) )
      {
         // NOTE: In order to correctly determine whether an instance is constrained it must be first fetched.
         bool found = false;

         if( persistence_type == 0 ) // i.e. SQL persistence
         {
            instance_accessor.fetch( sql, false );

            found = fetch_instance_from_db( instance, sql );
         }
         else if( persistence_type == 1 ) // i.e. ODS local persistence
            found = fetch_instance_from_local_storage( instance, key_for_op, false );
         else if( persistence_type == 2 ) // i.e. ODS global persistence
         {
            found = fetch_instance_from_global_storage( instance, key_for_op );

            instance_accessor.set_original_identity( instance.get_current_identity( ) );
         }
         else
            throw runtime_error( "unexpected persistence type #" + to_string( persistence_type ) + " in begin_instance_op" );

         if( !found )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_not_found;

               instance_accessor.set_op( old_op, false );
               instance_accessor.set_in_op_begin( false );

               return;
            }
            else
               throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
                make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
         }

         string ver_expected( instance_accessor.get_ver_exp( ) );

         if( !ver_expected.empty( ) && ver_expected != instance.get_version_info( ) )
            throw runtime_error( get_string_message( GS( c_str_version_mismatch ),
             make_pair( c_str_parm_version_mismatch_found, instance.get_version_info( ) ),
             make_pair( c_str_parm_version_mismatch_expected, ver_expected ) ) );

         if( !instance_accessor.can_destroy( internal_modification ) )
            throw runtime_error( get_string_message( GS( c_str_cannot_destroy ),
             make_pair( c_str_parm_cannot_destroy_class, instance.get_display_name( ) ) ) );

         if( instance.get_current_identity( ) != instance.get_original_identity( ) )
            throw runtime_error( "cannot destroy '" + instance.get_original_identity( )
             + "' stored instance using '" + instance.get_current_identity( ) + "' object instance" );

         string constraining_class;

         bool is_constrained = false;

         // NOTE: Unless performing the cascade obtain locks to all children (which are held in
         // each child's graph parent) and then check for the existence of restricting children.
         if( !is_cascade_op )
         {
            if( !obtain_cascade_locks( instance ) )
            {
               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_child_locked;

                  instance_accessor.set_op( old_op, false );
                  instance_accessor.set_in_op_begin( false );

                  return;
               }
               else
                  throw runtime_error( "instance '" + key_for_op
                   + "' cannot be destroyed as a lock to a dependent child could not be obtained" );
            }

            if( !session_skip_is_constained( ) )
               is_constrained = is_child_constrained( instance, constraining_class );
         }

         if( is_constrained )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_constrained;

               instance_accessor.set_op( old_op, false );
               instance_accessor.set_in_op_begin( false );

               return;
            }
            else
               throw runtime_error( get_string_message( GS( c_str_record_constrained ),
                make_pair( c_str_parm_record_constrained_class, constraining_class ) ) );
         }
      }

      lock_holder.release( );
      xlock_holder.release( );
   }
   catch( ... )
   {
      instance_accessor.set_op( old_op, false );
      instance_accessor.set_in_op_begin( false );

      if( !is_cascade_op && ( op == e_instance_op_destroy ) )
         release_instance_locks( instance, true );

      throw;
   }

   instance_accessor.set_in_op_begin( false );
   instance_accessor.set_lock_handle( lock_handle );

   if( xlock_handle )
      instance_accessor.set_xlock_handle( xlock_handle );

   TRACE_LOG( TRACE_INITIAL | TRACE_OBJECTS, "begin (leave) op = "
    + instance_op_name( op ) + ", class = " + instance.get_class_name( )
    + ", internal = " + to_string( internal_modification ) + ", key = " + key );
}

void finish_instance_op( class_base& instance, bool apply_changes,
 bool internal_operation, instance_op_rc* p_rc, set< string >* p_fields_set )
{
   check_storage( );

   if( p_rc )
      *p_rc = e_instance_op_rc_okay;

   class_base::op_type op = instance.get_op( );

   if( op == class_base::e_op_type_none )
      return;

   TRACE_LOG( TRACE_INITIAL | TRACE_OBJECTS, "finish (enter) op = "
    + instance_op_name( op ) + ", class = " + instance.get_class_name( )
    + ", internal = " + to_string( internal_operation )
    + ", apply_changes = " + to_string( apply_changes ) + ", key = " + instance.get_key( ) );

   class_base_accessor instance_accessor( instance );

   int persistence_type = instance.get_persistence_type( );

#ifdef COMPILE_PROTOTYPE_CODE
   string app_name( storage_name( ) );

   if( app_name == c_meta_storage_name )
      persistence_type = 1;
#endif

   if( !apply_changes || ( op == class_base::e_op_type_review ) )
      perform_op_cancel( instance, op );
   else
   {
      // NOTE: In order to make "minimal" updates as minimal as possible both the "to_store" trigger function
      // and validation is being skipped, so such updates must only be used in circumstances when it is known
      // that they will not affect the record's validity (i.e. not to be used lightly).
      if( op == class_base::e_op_type_create
       || ( op == class_base::e_op_type_update && !instance.get_is_minimal_update( ) ) )
      {
         bool valid = true;

         // NOTE: Validation is not applicable for "peer records" and can also be switched off as a
         // performance optimisation during a "storage restore".
         if( p_fields_set && !instance.get_is_for_peer( )
          && !session_skip_validation( ) && !instance.is_valid( internal_operation, p_fields_set ) )
            valid = false;

         if( valid )
         {
            instance_accessor.perform_to_store( op == class_base::e_op_type_create, internal_operation );

            if( !instance.get_is_for_peer( ) && !session_skip_validation( ) && !instance.is_valid( internal_operation ) )
               valid = false;
         }

         if( !valid )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_invalid;
               return;
            }
            else
            {
               string validation_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );

               perform_op_cancel( instance, op );

               set_session_variable( get_special_var_name( e_special_var_val_error ), validation_error );

               throw runtime_error( validation_error );
            }
         }
      }

      ods* p_ods( ods::instance( ) );

      bool op_is_in_transaction = is_in_transaction( );

      instance_tx_check( instance );

      // NOTE: If a "clone" lock had been obtained when the op was started then release it
      // now in order to allow "for_store" or "for_destroy" to perform operations with the
      // clone record itself if required.
      size_t xlock_handle = instance_accessor.get_xlock_handle( );

      if( xlock_handle )
      {
         release_obtained_lock( xlock_handle );
         instance_accessor.set_xlock_handle( 0 );
      }

      bool executing_sql = false;

      try
      {
         // NOTE: The "for_store" or "for_destroy" triggers may result in further create, update
         // or destroy operations so a transaction must be commenced here (unless already in one).
         transaction tx( !op_is_in_transaction );

         if( op == class_base::e_op_type_destroy )
            instance_accessor.for_destroy( internal_operation );
         else if( op == class_base::e_op_type_create
          || ( op == class_base::e_op_type_update && !instance.get_is_minimal_update( ) ) )
         {
            instance_accessor.for_store( op == class_base::e_op_type_create, internal_operation );

            // NOTE: As it's possible that "for_store" might inadvertantly have made the record invalid
            // the validation call is repeated now and the first error (if any is found) will be thrown
            // (this is referred to as "secondary validation" and an object variable is being set first
            // so that the rules code can detect this in situations such as when a foreign key record's
            // field might have needed to be a certain value in "to_store" but has been changed by code
            // that was executed in "for_store").
            temporary_object_variable tmp_object_secondary_validation(
             instance_accessor.get_obj( ), get_special_var_name( e_special_var_secondary_validation ), c_true_value );

            if( !instance.get_is_for_peer( ) && !session_skip_validation( ) && !instance.is_valid( internal_operation ) )
            {
               string validation_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );

               perform_op_cancel( instance, op );

               set_session_variable( get_special_var_name( e_special_var_val_error ), validation_error );

               throw runtime_error( validation_error );
            }
         }

         // NOTE: For the purposes of cascading must also set this true when destroying
         // as locks for cascaded child record updates are tied to the parent instance.
         instance_accessor.set_is_after_store( true );

         bool supports_sql_undo = storage_supports_sql_undo( );

         bool is_special_storage = false;

         string app_name( storage_name( ) );

         if( ( app_name == c_meta_storage_name ) || ( app_name == c_ciyam_storage_name ) )
            is_special_storage = true;

         string app_dir_name( lower( app_name ) );

         if( instance.get_is_for_peer( ) )
         {
            string identity( instance.get_peer_identity( ) );

            string class_id( instance.get_class_id( ) );
            string module_id( instance.get_module_id( ) );

            if( class_id.find( module_id ) == 0 )
               class_id.erase( 0, module_id.length( ) );

            string log_command;

            if( op == class_base::e_op_type_create )
               log_command += c_cmd_create;
            else if( op == class_base::e_op_type_update )
               log_command += c_cmd_update;
            else if( op == class_base::e_op_type_destroy )
               log_command += c_cmd_destroy;

            log_command += ' ';
            log_command += c_peer;

            log_command += ' ';
            log_command += date_time::standard( ).as_string( );

            log_command += ' ' + module_id + ' ' + class_id + ' ' + instance.get_key( );

            if( op != class_base::e_op_type_destroy )
               log_command += " \"" + instance.get_fields_and_values( class_base::e_field_label_type_short_id ) + "\"";

            append_peerchain_log_command( identity, log_command );

            set_session_variable( get_special_var_name( e_special_var_peer_data_created ), c_true_value );

            if( get_session_variable( get_special_var_name( e_special_var_skip_submit_file ) ).empty( ) )
            {
               string submit_type( get_session_variable( get_special_var_name( e_special_var_submit_type ) ) );

               if( submit_type.empty( ) )
                  submit_type = c_ui_submit_type_peer;
               else if( submit_type == get_special_var_name( e_special_var_identity ) )
                  submit_type = instance.get_peer_identity( );

               if( submit_type != get_special_var_name( e_special_var_none ) )
               {
                  string submit_file( replaced( c_ui_type_submit_file, c_ui_type_repl_name, submit_type.c_str( ) ) );

                  file_touch( get_web_root( ) + '/' + app_dir_name + '/' + submit_file, 0, true );
               }
            }
         }
         else
         {
            // NOTE: If not Meta and instance is being created or updated then if either
            // a group or level fields are being used then determine the security value.
            if( !is_special_storage && ( op != class_base::e_op_type_destroy ) )
            {
               string group_field_name( instance.get_group_field_name( ) );
               string level_field_name( instance.get_level_field_name( ) );

               if( !group_field_name.empty( ) || !level_field_name.empty( ) )
               {
                  int64_t security_value = 0;

                  if( instance.get_class_type( ) != 1 ) // i.e. user
                  {
                     if( !group_field_name.empty( ) )
                     {
                        string group_key_value( instance.get_field_value( instance.get_field_num( group_field_name ) ) );

                        if( !group_key_value.empty( ) )
                           security_value = group_security_value( group_key_value );
                     }

                     if( !level_field_name.empty( ) )
                     {
                        string level_key_value( instance.get_field_value( instance.get_field_num( level_field_name ) ) );

                        security_value += ( 10 - level_key_value.length( ) );
                     }
                  }

                  instance_accessor.set_security( security_value );
               }
            }

            int16_t version = instance.get_version( );
            int64_t revision = instance.get_revision( );

            if( persistence_type == 0 ) // i.e. SQL persistence
            {
               vector< string > sql_stmts;
               vector< string > sql_undo_stmts;

               vector< string >* p_sql_undo_stmts = 0;

               if( supports_sql_undo )
                  p_sql_undo_stmts = &sql_undo_stmts;

               get_instance_sql_stmts( instance, sql_stmts, p_sql_undo_stmts );

               bool skipped_empty_update = false;

               // NOTE: If updating but no fields apart from the revision one were changed (by any
               // derivation) then all update statements are discarded to skip the unnecessary SQL.
               if( ( op == class_base::e_op_type_update )
                && instance_accessor.has_skipped_empty_update( ) )
               {
                  sql_stmts.clear( );

                  skipped_empty_update = true;
               }

               if( sql_stmts.empty( ) && ( op == class_base::e_op_type_create ) )
               {
                  // NOTE: If creating and no SQL exists then will log if is tracing.
                  TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, "*** no SQL statement for create ***" );
               }

               if( !sql_stmts.empty( ) && has_sql_db( ) )
               {
                  executing_sql = true;

                  for( size_t i = 0; i < sql_stmts.size( ); i++ )
                  {
                     if( sql_stmts[ i ].empty( ) )
                        continue;

                     TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, sql_stmts[ i ] );

                     exec_sql( get_sql_db( ), sql_stmts[ i ] );

                     session_inc_sql_count( );
                  }
               }

               executing_sql = false;

               if( supports_sql_undo && !skipped_empty_update )
                  append_undo_sql_stmts( sql_undo_stmts );

            }
            else if( persistence_type == 1 ) // i.e. ODS local persistence
            {
               if( op == class_base::e_op_type_destroy )
                  instance_accessor.destroy( );

               // NOTE: Objects are stored in the form of a structured I/O
               // file using the path "/.dat/<class_id>/" with a file name
               // that consists of the following formatted object instance
               // core data: "<key>\t<sec>\t<typ>\t<ver>". A primary index
               // file link is stored in "/.idx/<class_id>" using the name
               // "<security>-<key>" whilst all other index file links are
               // found in a two digit folder named according to the index
               // number with the name "<security>-<index_data>". If index
               // is not unique then the instance key is appended (like an
               // additional field). Security digits are followed by field
               // values which are separated by tab characters.
               size_t links_lock = 0;

               string class_id( instance.get_class_id( ) );

               links_lock = obtain_storage_lock( c_update_lock_name,
                class_id, c_links_instance_lock_key, c_num_links_lock_retries );

               if( links_lock )
               {
                  storage_scoped_lock_holder lock_holder( links_lock );

                  ods& ods_db( storage_ods_instance( ) );

                  storage_ods_bulk_write ods_bulk_write;

                  ods::transaction ods_tx( ods_db );

                  ods_file_system ofs( ods_db );

                  string source_file_name( "/" );

                  ofs.set_root_folder( c_storage_folder_name_dot_dat );

                  source_file_name += c_storage_folder_name_dot_dat;

                  if( !ofs.has_folder( class_id ) )
                     ofs.add_folder( class_id );

                  ofs.set_folder( class_id );

                  source_file_name += '/' + class_id;

                  string instance_file_name( instance.get_key( ) );

                  string security_value( to_comparable_string(
                   instance.get_security( ), false, c_sec_prefix_length ) );

                  instance_file_name += '\t' + security_value;

                  if( op == class_base::e_op_type_create )
                     instance_file_name += '\t' + instance.get_current_identity( );
                  else
                     instance_file_name += '\t' + instance.get_original_identity( );

                  instance_file_name += '\t' + to_string( version ) + '.' + to_string( revision );

                  source_file_name += '/' + instance_file_name;

                  temporary_allow_specials allow_specials( ofs );

                  // NOTE: Removing the instance data file
                  // results in all index link files being
                  // automatically removed.
                  if( ( op == class_base::e_op_type_update )
                   || ( op == class_base::e_op_type_destroy ) )
                     ofs.remove_file( instance_file_name );

                  if( ( op == class_base::e_op_type_create )
                   || ( op == class_base::e_op_type_update ) )
                  {
                     stringstream sio_data;

                     sio_writer writer( sio_data );

                     bool had_any_non_transients = false;

                     int num_fields = instance.get_num_fields( );

                     if( op == class_base::e_op_type_update )
                        instance_accessor.set_revision( ++revision );

                     string version_info(
                      to_string( version ) + '.' + to_string( revision ) );

                     string::size_type pos = instance_file_name.rfind( '\t' );

                     if( ( pos != string::npos ) && ( op == class_base::e_op_type_update ) )
                     {
                        instance_file_name.erase( pos + 1 );
                        instance_file_name += version_info;

                        pos = source_file_name.rfind( '\t' );

                        if( pos == string::npos )
                           throw runtime_error( "unexpected source_file_name '" + source_file_name + "' missing ver/rev info" );

                        source_file_name.erase( pos + 1 );
                        source_file_name += version_info;
                     }

                     for( int i = 0; i < num_fields; i++ )
                     {
                        if( instance.is_field_transient( i ) )
                           continue;

                        had_any_non_transients = true;

                        string data( instance.get_field_value( i ) );
                        string attribute_name( lower( instance.get_field_name( i ) ) );

                        writer.write_attribute( attribute_name, escaped( data, 0, '\\', "trn\t\r\n" ) );
                     }

                     if( had_any_non_transients )
                     {
                        writer.finish_sections( );

                        ofs.store_file( instance_file_name, 0, &sio_data );
                     }
                     else
                        ofs.store_file( instance_file_name, c_file_zero_length );

                     vector< pair< string, string > > all_index_pairs;

                     instance.get_all_index_pairs( all_index_pairs );

                     size_t num_index_pairs = all_index_pairs.size( );

                     ofs.set_root_folder( c_storage_folder_name_dot_idx );

                     if( !ofs.has_folder( class_id ) )
                        ofs.add_folder( class_id );

                     ofs.set_folder( class_id );

                     // NOTE: Create a primary key link (so instance iteration confined by
                     // a security prefix is possible even if no explicit indexes exist).
                     ofs.link_file( security_value + c_security_suffix + instance.get_key( ), source_file_name );

                     vector< string > duplicate_fields;

                     for( size_t i = 0; i < num_index_pairs; i++ )
                     {
                        pair< string, string > next_pair = all_index_pairs[ i ];

                        vector< string > names;
                        vector< string > types;

                        split( next_pair.first, names );
                        split( next_pair.second, types );

                        if( names.size( ) != types.size( ) )
                           throw runtime_error( "unexpected index pairs names.size( ) != types.size( )" );

                        string index_num_folder( to_string( i ) );

                        if( i < 10 )
                           index_num_folder = "0" + index_num_folder;

                        if( !ofs.has_folder( index_num_folder ) )
                           ofs.add_folder( index_num_folder );

                        ofs.set_folder( index_num_folder );

                        size_t num_index_fields = names.size( );

                        bool is_unique = true;

                        string link_file_name;

                        for( size_t j = 0; j < num_index_fields; j++ )
                        {
                           string next_name( names[ j ] );
                           string next_type( types[ j ] );

                           if( next_name == c_primary_key_name )
                              is_unique = false;
                           else
                           {
                              if( j > 0 )
                                 link_file_name += '\t';

                              // FUTURE: Need to use comparable values for both integer and numeric types.
                              link_file_name += instance.get_field_value( instance.get_field_num( next_name ) );
                           }
                        }

                        size_t link_file_name_len = link_file_name.size( );

                        // NOTE: Although special characters are allowed because
                        // ODS FS needs '/', ':' and '|' characters for internal
                        // folder names they are replaced by non-printable ASCII
                        // characters. Although not a problem for unique indexes
                        // it will potentially be noticed due to index file name
                        // ordering (although not likely to be a serious issue).
                        for( size_t i = 0; i < link_file_name_len; i++ )
                        {
                           if( link_file_name [ i ] == '/' )
                              link_file_name [ i ] = '\x01';
                           else if( link_file_name [ i ] == ':' )
                              link_file_name [ i ] = '\x02';
                           else if( link_file_name [ i ] == '|' )
                              link_file_name [ i ] = '\x03';
                        }

                        link_file_name = security_value + c_security_suffix + link_file_name;

                        if( !is_unique )
                           link_file_name += ( '\t' + instance.get_key( ) );

                        // NOTE: If a link file exists then this is because a
                        // duplicate record has been found (and an error will
                        // occur). Execution will continue in order to ensure
                        // that the duplicate error chosen to be displayed is
                        // for the shortest possible index (as this should be
                        // the easiest to understand such as simply "Name").
                        if( ofs.has_file( link_file_name ) )
                        {
                           if( duplicate_fields.empty( ) || ( names.size( ) < duplicate_fields.size( ) ) )
                           {
                              duplicate_fields.clear( );

                              if( names.size( ) > 1 )
                                 duplicate_fields.push_back( names[ names.size( ) - 2 ] );

                              duplicate_fields.push_back( names[ names.size( ) - 1 ] );
                           }
                        }
                        else
                           ofs.link_file( link_file_name, source_file_name );

                        ofs.set_folder( ".." );
                     }

                     if( !duplicate_fields.empty( ) )
                        throw runtime_error( "@" + join( duplicate_fields ) );
                  }

                  ods_tx.commit( );
               }
            }
            else if( persistence_type == 2 ) // i.e. ODS global persistence
            {
               string skip_persistence_name( get_special_var_name( e_special_var_skip_persistence ) );

               if( instance.has_variable( skip_persistence_name ) )
               {
                  instance.set_variable( skip_persistence_name, "" );

                  set_session_variable( skip_persistence_name, c_true_value );
               }
               else if( ( op == class_base::e_op_type_create ) || ( op == class_base::e_op_type_update ) || ( op == class_base::e_op_type_destroy ) )
               {
                  if( op == class_base::e_op_type_destroy )
                     instance_accessor.destroy( );

                  string persistence_extra( instance.get_persistence_extra( ) );

                  string root_child_folder( persistence_extra );

                  bool is_file_not_folder( global_storage_persistence_is_file( root_child_folder ) );

                  system_ods_bulk_write ods_bulk_write;

                  ods_file_system& sys_ods_fs( system_ods_file_system( ) );

                  sys_ods_fs.set_root_folder( root_child_folder );

                  if( op == class_base::e_op_type_destroy )
                  {
                     if( is_file_not_folder && sys_ods_fs.has_file( instance.get_key( ) ) )
                        sys_ods_fs.remove_file( instance.get_key( ) );
                     else if( !is_file_not_folder && sys_ods_fs.has_folder( instance.get_key( ) ) )
                        sys_ods_fs.remove_folder( instance.get_key( ), 0, true );
                  }
                  else
                  {
                     stringstream sio_data;

                     auto_ptr< sio_writer > ap_sio_writer;

                     if( is_file_not_folder )
                        ap_sio_writer.reset( new sio_writer( sio_data ) );

                     bool had_any_non_transients = false;

                     int num_fields = instance.get_num_fields( );

                     if( !is_file_not_folder )
                     {
                        if( !sys_ods_fs.has_folder( instance.get_key( ) ) )
                           sys_ods_fs.add_folder( instance.get_key( ) );

                        sys_ods_fs.set_folder( instance.get_key( ) );
                     }

                     // NOTE: If is stored as a file then attributes are expected to be
                     // in the format of a structured I/O file otherwise each attribute
                     // is expected to be a file within the record's folder.
                     for( int i = 0; i < num_fields; i++ )
                     {
                        if( instance.is_field_transient( i ) )
                           continue;

                        had_any_non_transients = true;

                        string data( instance.get_field_value( i ) );
                        string attribute_name( lower( instance.get_field_name( i ) ) );

                        if( !is_file_not_folder )
                           sys_ods_fs.store_as_text_file( attribute_name, data );
                        else
                           ap_sio_writer->write_attribute( attribute_name, data );
                     }

                     if( is_file_not_folder )
                     {
                        if( had_any_non_transients )
                        {
                           ap_sio_writer->finish_sections( );

                           sys_ods_fs.store_file( instance.get_key( ), 0, &sio_data );
                        }
                        else
                           sys_ods_fs.store_file( instance.get_key( ), c_file_zero_length );
                     }
                  }
               }
            }
            else
               throw runtime_error( "unexpected persistence type #" + to_string( persistence_type ) + " in finish_instance_op" );
         }

         string blockchain_backup_height( get_system_variable(
          get_special_var_name( e_special_var_blockchain_backup_height ) ) );

         if( !is_special_storage
          && !storage_locked_for_admin( )
          && !blockchain_backup_height.empty( ) )
         {
            if( ( blockchain_backup_height != c_false_value )
             || is_null( instance.get_variable( get_special_var_name( e_special_var_can_omit_prepare ) ) ) )
            {
               string prepare_var_name( get_special_var_name( e_special_var_prepare_backup_needed ) );

               string prepare_file_name( c_hidden_file_prefix + prepare_var_name.substr( 1 ) );

               if( !file_exists( prepare_file_name ) )
                  set_system_variable( prepare_var_name, c_true_value );
            }
         }

         // NOTE: In order to be able to create child records (or to review the just created instance)
         // the "create" lock is downgraded to an "update" lock after DB record is stored but prior to
         // the transaction being committed. If the DB engine permits "phantom" reads then it could be
         // possible for data to become corrupted if the transaction does not end up being committed.
         if( op == class_base::e_op_type_create )
            transform_obtained_lock( instance_accessor.get_lock_handle( ), c_update_lock_name );

         if( op == class_base::e_op_type_destroy )
            instance_accessor.after_destroy( internal_operation );
         // NOTE: Although "after_store" is normally skipped for "minimal" updates in the case of Meta
         // it must still be called so that "aliased" class artifacts will behave as would be expected.
         else if( ( op == class_base::e_op_type_create ) || !instance.get_is_minimal_update( )
          || ( ( op == class_base::e_op_type_update ) && ( app_name == c_meta_storage_name ) ) )
            instance_accessor.after_store( ( op == class_base::e_op_type_create ), internal_operation );

         const string& key( instance.get_key( ) );

         // NOTE: If creating a new "user_group" record then also will
         // create a group identity number (to be used for "security").
         if( !is_special_storage
          && ( instance.get_class_type( ) == 2 ) // i.e. user_group
          && ( op == class_base::e_op_type_create ) )
            add_security_group( instance.get_key( ) );

         if( !op_is_in_transaction )
            tx.commit( );

         if( !instance.get_is_being_cascaded( ) && ( op == class_base::e_op_type_destroy ) )
            release_instance_locks( instance );

         instance_accessor.finish( );
      }
      catch( exception& e )
      {
         string field, second;

         perform_op_cancel( instance, op );

         // NOTE: A failed INSERT or UPDATE could be due to a unique index violation
         // so query all unique indexes to determine if indeed this has occurred and
         // if so then report the error against the last field of the smallest index
         // (or last two if the variable "@display_last_two_for_duplicate" is set).
         if( executing_sql
          && ( op == class_base::e_op_type_create || op == class_base::e_op_type_update ) )
         {
            vector< string > all_unique_indexes;

            instance.get_sql_unique_indexes( all_unique_indexes );

            vector< string > all_column_names;

            instance.get_sql_column_names( all_column_names );

            vector< string > all_column_values;

            instance.get_sql_column_values( all_column_values );

            map< string, size_t > column_numbers;

            for( size_t i = 0; i < all_column_names.size( ); i++ )
               column_numbers.insert( make_pair( all_column_names[ i ], i ) );

            size_t num_columns = 0;

            for( size_t i = 0; i < all_unique_indexes.size( ); i++ )
            {
               vector< string > unique_index_columns;

               split( all_unique_indexes[ i ], unique_index_columns );

               string sql( "SELECT C_Key_ FROM T_" + string( instance.get_module_name( ) )
                + "_" + instance.get_class_name( ) + " WHERE " );

               for( size_t j = 0; j < unique_index_columns.size( ); j++ )
               {
                  if( column_numbers.count( unique_index_columns[ j ] ) == 0 )
                     throw runtime_error( "unexpected column name '" + unique_index_columns[ j ] + "'" );

                  if( j > 0 )
                     sql += " AND ";
                  sql += all_column_names[ column_numbers[ unique_index_columns[ j ] ] ];
                  sql += " = " + all_column_values[ column_numbers[ unique_index_columns[ j ] ] ];
               }

               TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, sql );

               sql_dataset ds( get_sql_db( ), sql );

               if( ds.next( ) && ds.as_string( 0 ) != instance.get_key( ) )
               {
                  if( field.empty( ) || unique_index_columns.size( ) < num_columns )
                  {
                     num_columns = unique_index_columns.size( );

                     field = unique_index_columns[ unique_index_columns.size( ) - 1 ].substr( 2 );

                     if( unique_index_columns.size( ) > 1 )
                        second = unique_index_columns[ unique_index_columns.size( ) - 2 ].substr( 2 );
                  }
               }
            }
         }

         if( !executing_sql
          && ( op == class_base::e_op_type_create || op == class_base::e_op_type_update ) )
         {
            string error( e.what( ) );

            // NOTE: For ODS local storage the exception should be "@<fld1>[,<fld2>[,...]]".
            if( !error.empty( ) && ( error[ 0 ] == '@' ) )
            {
               string fields( error.substr( 1 ) );

               if( !fields.empty( ) )
               {
                  vector< string > all_fields;

                  split( fields, all_fields );

                  if( all_fields.size( ) > 1 )
                     second = all_fields[ all_fields.size( ) - 2 ];

                  field = all_fields[ all_fields.size( ) - 1 ];
               }
            }
         }

         if( field.empty( ) )
            throw;
         else
         {
            TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, e.what( ) );

            string names;
            string values;

            if( !second.empty( ) && !instance.get_variable(
             get_special_var_name( e_special_var_display_last_two_for_duplicate ) ).empty( ) )
            {
               values += instance.get_field_value( instance.get_field_num( second ) );
               values += ',';

               names += instance.get_field_display_name( instance_accessor.get_field_id( second ) );
               names += ' ' + GS( c_str_and ) + ' ';
            }

            values += instance.get_field_value( instance.get_field_num( field ) );
            names += instance.get_field_display_name( instance_accessor.get_field_id( field ) );

            throw runtime_error(
             get_string_message( GS( c_str_index_duplicate ),
             make_pair( c_str_parm_index_duplicate_field, names ),
             make_pair( c_str_parm_index_duplicate_value, values ),
             make_pair( c_str_parm_index_duplicate_class, instance.get_display_name( ) ) ) );
         }
      }
   }

   instance_accessor.set_ver_exp( "" );

   TRACE_LOG( TRACE_INITIAL | TRACE_OBJECTS, "finish (leave) op = "
    + instance_op_name( op ) + ", class = " + instance.get_class_name( )
    + ", internal = " + to_string( internal_operation )
    + ", apply_changes = " + to_string( apply_changes ) + ", key = " + instance.get_key( ) );
}

bool perform_instance_check( class_base& instance, const string& key )
{
   bool found = false;

   check_storage( );

   class_base_accessor instance_accessor( instance );

   int persistence_type = instance.get_persistence_type( );

#ifdef COMPILE_PROTOTYPE_CODE
   string app_name( storage_name( ) );

   if( app_name == c_meta_storage_name )
      persistence_type = 1;
#endif

   if( persistence_type == 0 ) // i.e. SQL persistence
   {
      string sql;

      instance_accessor.fetch( sql, true, true );

      found = fetch_instance_from_db( instance, sql, true );
   }
   else if( persistence_type == 1 ) // i.e. ODS local persistence
      found = has_instance_in_local_storage( instance, key );
   else if( persistence_type == 2 ) // i.e. ODS global persistence
      found = has_instance_in_local_storage( instance, key );
   else
      found = fetch_instance_from_system_variable( instance, key );

   return found;
}

void perform_instance_fetch( class_base& instance,
 const string& key_info, instance_fetch_rc* p_rc, bool only_sys_fields, bool do_not_use_cache )
{
   check_storage( );

   if( key_info.empty( ) )
      throw runtime_error( "cannot fetch "
       + instance.get_class_name( ) + " record without key information" );

   if( p_rc )
      *p_rc = e_instance_fetch_rc_okay;

   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );

   class_base_accessor instance_accessor( instance );

   int persistence_type = instance.get_persistence_type( );

#ifdef COMPILE_PROTOTYPE_CODE
   string app_name( storage_name( ) );

   if( app_name == c_meta_storage_name )
      persistence_type = 1;
#endif

   if( instance.get_is_in_op( ) && !instance_accessor.get_in_op_begin( ) )
      throw runtime_error( "cannot fetch "
       + instance.get_class_name( ) + " record whilst currently perfoming an instance operation" );

   bool found_in_cache = false;

   bool has_simple_keyinfo = ( key_info.find( ' ' ) == string::npos );

   bool has_sess_tx_key_info = false;

   if( has_simple_keyinfo )
      has_sess_tx_key_info = has_tx_key_info( instance.get_class_id( ) + ":" + key_info );

   if( !has_sess_tx_key_info && !do_not_use_cache )
      found_in_cache = fetch_instance_from_cache( instance, key_info, only_sys_fields );

   bool found = found_in_cache;

   if( !found )
   {
      if( persistence_type == 0 ) // i.e. SQL persistence
      {
         string sql;

         vector< string > field_info;
         vector< string > order_info;

         vector< pair< string, string > > query_info;
         vector< pair< string, string > > fixed_info;
         vector< pair< string, string > > paging_info;

         split_key_info( key_info, fixed_info, paging_info, order_info, false );

         sql = construct_sql_select( instance, field_info, order_info,
          query_info, fixed_info, paging_info, false, true, 1, only_sys_fields, "" );

         found = fetch_instance_from_db( instance, sql,
          only_sys_fields, false, ( has_simple_keyinfo && !has_sess_tx_key_info ) );
      }
      else if( persistence_type == 1 ) // i.e. ODS local persistence
      {
         if( key_info.find( ' ' ) == string::npos )
            found = fetch_instance_from_local_storage( instance, key_info, only_sys_fields );
         else
         {
            vector< string > order_info;

            vector< pair< string, string > > fixed_info;
            vector< pair< string, string > > paging_info;

            split_key_info( key_info, fixed_info, paging_info, order_info, false );

            string prefix, origin;

            string folder( determine_local_iteration_info( instance, order_info, fixed_info, prefix ) );

            if( !paging_info.empty( ) )
            {
               for( size_t i = 0; i < paging_info.size( ); i++ )
               {
                  if( i > 0 )
                     origin += '\t';

                  origin += paging_info[ i ].second;
               }
            }

            instance.set_local_info( folder, origin, prefix );

            vector< string > instance_keys;

            fetch_keys_from_local_storage( instance, true, 1, instance_keys, false );

            if( instance_keys.empty( ) )
               found = false;
            else
               found = fetch_instance_from_local_storage( instance, instance_keys[ 0 ], only_sys_fields );
         }
      }
      else if( persistence_type == 2 ) // i.e. ODS global persistence
      {
         if( only_sys_fields )
            found = has_instance_in_global_storage( instance, key_info );
         else
            found = fetch_instance_from_global_storage( instance, key_info );
      }
      else
         found = fetch_instance_from_system_variable( instance, key_info );
   }

   if( !found )
   {
      if( p_rc )
         *p_rc = e_instance_fetch_rc_not_found;
      else
      {
         string keys( key_info );

         string::size_type pos = keys.find( ' ' );

         if( pos != string::npos )
            keys.erase( 0, pos + 1 );

         throw runtime_error( get_string_message( GS( c_str_record_not_found ),
          make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
          make_pair( c_str_parm_record_not_found_key, keys ) ) );
      }
   }
}

bool perform_instance_iterate( class_base& instance,
 const string& key_info, const string& fields, const string& text,
 const string& query, iter_direction direction, bool inclusive,
 int row_limit, sql_optimisation optimisation, const set< string >* p_filters )
{
   check_storage( );

   bool found = false;

   if( is_condemned_session( ) )
   {
      string stop_condemned( get_raw_system_variable(
       get_special_var_name( e_special_var_stop_condemned ) ) );

      if( ( stop_condemned == c_all )
       || ( stop_condemned == to_string( session_id( ) ) ) )
         throw runtime_error( "*** condemned session has been stopped ***" );
   }

   TRACE_LOG( TRACE_INITIAL | TRACE_OBJECTS, "[iterate] class = '"
    + instance.get_class_name( ) + "', key_info = '" + key_info + "', fields = '" + fields
    + "', direction = " + to_string( direction ) + ", text = '" + text + "', query = '" + query + "'" );

   string sql, key;

   size_t num_fields = 0;

   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );

   class_base_accessor instance_accessor( instance );

   // NOTE: If row_limit < 0 then iteration is being continued.
   if( ( row_limit >= 0 ) && instance.get_is_iterating( ) )
   {
      string class_name( instance.get_class_name( ) );

      class_base* p_parent = instance.get_graph_parent( );

      while( p_parent )
      {
         class_name += string( " <- " ) + p_parent->get_class_name( );

         p_parent = p_parent->get_graph_parent( );
      }

      throw runtime_error( "iterate called whilst already iterating (class: " + class_name + ")" );
   }

   // NOTE: Because filtering can exclude records from the DB fetch
   // the limit has to be zeroed if any filters have been supplied.
   if( p_filters )
   {
      row_limit = 0;

      instance_accessor.filters( ) = *p_filters;
   }

   int persistence_type = instance.get_persistence_type( );

#ifdef COMPILE_PROTOTYPE_CODE
   string app_name( storage_name( ) );

   if( app_name == c_meta_storage_name )
      persistence_type = 1;
#endif

   if( instance.get_is_in_op( ) && !instance_accessor.get_in_op_begin( ) )
      throw runtime_error( "cannot begin iteration whilst currently perfoming an instance operation" );
   else
   {
      auto_ptr< storage_ods_bulk_read > ap_storage_ods_bulk_read;

      // IMPORTANT: As bulk locking is being performed when fetching
      // the instance keys and then again for each separate instance
      // in order to prevent potential bulk locking conflicts obtain
      // the ODS bulk read lock now.
      if( persistence_type == 1 )
         ap_storage_ods_bulk_read.reset( new storage_ods_bulk_read( ) );

      if( ( row_limit >= 0 ) && ( key_info != c_null_key ) )
      {
         vector< string > field_info;
         vector< string > order_info;

         vector< pair< string, string > > fixed_info;
         vector< pair< string, string > > query_info;

         vector< pair< string, string > > paging_info;

         string sec_marker;

         vector< string > sec_values;

         string group_field_name( instance.get_group_field_name( ) );
         string level_field_name( instance.get_level_field_name( ) );
         string order_field_name( instance.get_order_field_name( ) );
         string owner_field_name( instance.get_owner_field_name( ) );

         string sec( get_session_variable( get_special_var_name( e_special_var_sec ) ) );
         string uid( get_session_variable( get_special_var_name( e_special_var_uid ) ) );

         string gids;

         size_t sec_level = 0;

         // NOTE: Cascade operations are not constrained by security
         // (and the "admin" user is also likewise always exempted).
         if( instance.get_is_being_cascaded( ) || ( uid == c_admin ) )
            sec.erase( );
         else
         {
            if( instance.get_class_type( ) != 1 ) // i.e. not user
            {
               bool found_uid_data = false;

               if( !uid.empty( ) && get_uid_data( uid, sec_level, gids ) )
                  found_uid_data = true;

               if( !found_uid_data )
               {
                  if( !sec.empty( ) )
                     sec_level = ( 10 - sec.length( ) );

                  gids = get_session_variable( get_special_var_name( e_special_var_gids ) );
               }

               if( gids.empty( ) && !group_field_name.empty( ) )
                  gids = get_gid( );
            }

            if( !gids.empty( ) && !group_field_name.empty( ) )
            {
               sec_marker = uuid( ).as_string( );

               vector< string > gid_vals;

               split( gids, gid_vals, '|' );

               for( size_t i = 0; i < gid_vals.size( ); i++ )
               {
                  size_t next = from_string< int64_t >( gid_vals[ i ] );

                  sec_values.push_back( to_string( next * 10 ) );

                  if( sec_level && !level_field_name.empty( ) )
                  {
                     for( size_t j = 1; j <= sec_level; j++ )
                        sec_values.push_back( to_string( ( next * 10 ) + j ) );
                  }
               }
            }

            // NOTE: Handle records where security level is used without a group.
            if( !sec.empty( ) && sec_values.empty( ) && !level_field_name.empty( ) )
            {
               sec_marker = uuid( ).as_string( );

               for( size_t i = 0; i <= sec_level; i++ )
                  sec_values.push_back( to_string( i ) );
            }
         }

         set< string > transient_field_names;

         int num_fields = instance.get_num_fields( );

         for( int i = 0; i < num_fields; i++ )
         {
            if( instance.is_field_transient( i ) )
               transient_field_names.insert( instance.get_field_name( i ) );
         }

         instance_accessor.transient_filter_field_values( ).clear( );

         if( !fields.empty( ) && ( fields != c_key_field ) )
         {
            set< string > fetch_field_names;

            vector< string > tmp_field_info;

            split( fields, tmp_field_info );

            for( size_t i = 0; i < tmp_field_info.size( ); i++ )
            {
               if( tmp_field_info[ i ].find( '.' ) == string::npos )
               {
                  string next( tmp_field_info[ i ] );

                  if( next != c_ignore_field )
                  {
                     get_field_name( instance, next );

                     fetch_field_names.insert( next );

                     if( !transient_field_names.count( next ) )
                        field_info.push_back( tmp_field_info[ i ] );
                  }
               }
            }

            instance_accessor.fetch_field_names( ) = fetch_field_names;

            // NOTE: If there are fields that are required in order to determine
            // state correctly then these fields are appended to the field list.
            set< string > required_fields;
            set< string > field_dependents;

            // NOTE: Any special fields found are treated as required fields.
            if( !group_field_name.empty( ) )
               required_fields.insert( group_field_name );

            if( !level_field_name.empty( ) )
               required_fields.insert( level_field_name );

            if( !order_field_name.empty( ) )
               required_fields.insert( order_field_name );

            if( !owner_field_name.empty( ) )
               required_fields.insert( owner_field_name );

            instance.get_required_field_names( required_fields, false, &field_dependents );

            size_t iterations = 0;

            // NOTE: It is possible that due to "interdependent" required fields
            // some required fields may not have been added in the first or even
            // later calls to "get_required_field_names" so continue calling the
            // function until no further field names (or deps) have been added.
            size_t required_fields_size = required_fields.size( );
            size_t field_dependents_size = field_dependents.size( );

            while( required_fields_size )
            {
               instance.get_required_field_names( required_fields, false, &field_dependents );

               if( ( required_fields.size( ) == required_fields_size )
                && ( field_dependents_size == field_dependents.size( ) ) )
                  break;

               if( ++iterations > c_max_required_field_iterations )
                  throw runtime_error( "unexpected excessive get_required_field_names( ) iterations" );

               required_fields_size = required_fields.size( );
               field_dependents_size = field_dependents.size( );
            }

            for( set< string >::iterator i = required_fields.begin( ); i != required_fields.end( ); ++i )
            {
               if( !fetch_field_names.count( *i ) )
               {
                  field_info.push_back( *i );

                  instance_accessor.fetch_field_names( ).insert( *i );
               }
            }
         }

         // NOTE: If this class instance is its graph parent's "child" then the iteration is
         // restricted to those instances that belong to the parent (via a secondary index).
         if( instance.get_graph_parent( ) && !instance.get_is_singular( ) )
         {
            string fk_field = instance.get_graph_parent_fk_field( );

            bool is_transient = instance.is_field_transient( instance.get_field_num( fk_field ) );

            if( !is_transient )
            {
               string parent_key_value( instance.get_graph_parent( )->get_key( ) );

               fixed_info.push_back( make_pair( fk_field, parent_key_value ) );
            }
            else
            {
               vector< string > replacements;

               instance.get_transient_replacement_field_names( fk_field, replacements );

               for( size_t i = 0; i < replacements.size( ); i++ )
                  fixed_info.push_back( make_pair( replacements[ i ], instance.get_graph_parent( )->get_key( ) ) );
            }
         }

         // NOTE: Fixed info can be provided either through an object variable
         // or by the "add_extra_field_info" virtual function in "class_base".
         string fixed_field_values( instance.get_variable(
          get_special_var_name( e_special_var_fixed_field_values ) ) );

         string extra_order_field;

         if( !fixed_field_values.empty( ) )
         {
            vector< string > field_and_values;

            split( fixed_field_values, field_and_values );

            for( size_t i = 0; i < field_and_values.size( ); i++ )
            {
               string next_pair( field_and_values[ i ] );

               string::size_type pos = next_pair.find( '=' );

               if( pos == string::npos )
                  extra_order_field = next_pair;
               else
                  fixed_info.push_back( make_pair( next_pair.substr( 0, pos ), next_pair.substr( pos + 1 ) ) );
            }
         }

         instance_accessor.add_extra_fixed_info( fixed_info );
         instance_accessor.add_extra_paging_info( paging_info );

         for( size_t i = 0; i < fixed_info.size( ); i++ )
            order_info.push_back( fixed_info[ i ].first );

         for( size_t i = 0; i < paging_info.size( ); i++ )
            order_info.push_back( paging_info[ i ].first );

         if( !extra_order_field.empty( ) )
            order_info.push_back( extra_order_field );

         // NOTE: If the key info contains any transient field names (for ordering)
         // then these need to be replaced by zero or more persistent field names.
         string keys( key_info );

         string extra_key_info;
         string final_key_info;

         size_t pos = keys.find_first_of( " #" );

         if( pos != string::npos )
         {
            extra_key_info = keys.substr( pos );
            keys.erase( pos );
         }

         if( pos == string::npos )
            final_key_info = key_info;
         else
         {
            vector< string > all_keys;
            vector< string > extra_key_values;

            if( !keys.empty( ) )
            {
               split( keys, all_keys );

               pos = extra_key_info.find( ' ' );

               if( ( pos != string::npos ) && ( pos != extra_key_info.size( ) - 1 ) )
               {
                  split( extra_key_info.substr( pos + 1 ), extra_key_values );

                  extra_key_info.erase( pos + 1 );
               }
            }

            // NOTE: If we have one more extra key value then assume it is that of the primary key.
            if( extra_key_values.size( ) == ( all_keys.size( ) + 1 ) )
               all_keys.push_back( get_special_var_name( e_special_var_key ) );

            bool first_extra = true;

            vector< string > final_keys;

            for( size_t i = 0; i < all_keys.size( ); i++ )
            {
               string next( all_keys[ i ] );

               get_field_name( instance, next );

               bool add_extra = false;

               if( !transient_field_names.count( next ) )
               {
                  add_extra = true;
                  final_keys.push_back( next );
               }
               else
               {
                  size_t size = final_keys.size( );

                  instance.get_transient_replacement_field_names( next, final_keys );

                  if( size != final_keys.size( ) )
                     add_extra = true;
                  else
                  {
                     string next_value;

                     if( i < extra_key_values.size( ) )
                     {
                        next_value = escaped( extra_key_values[ i ], "," );

                        // NOTE: If transients have been used as restricts, and no non-transient replacement
                        // was provided, then tests for record filtering based upon each and every transient
                        // restriction will need to occur after the record has been fetched from the DB. The
                        // row limit therefore must be cleared as it is not known which records may later be
                        // filtered.
                        row_limit = 0;

                        instance_accessor.transient_filter_field_values( ).insert( make_pair( next, next_value ) );
                     }
                  }
               }

               if( add_extra )
               {
                  if( i < extra_key_values.size( ) )
                  {
                     if( !first_extra )
                        extra_key_info += ',';

                     first_extra = false;

                     extra_key_info += escaped( extra_key_values[ i ], "," );
                  }
               }
            }

            if( !extra_key_info.empty( ) && ( extra_key_info[ 0 ] == '#' )
             && !instance_accessor.transient_filter_field_values( ).empty( ) )
               extra_key_info[ 1 ] -= ( unsigned char )instance_accessor.transient_filter_field_values( ).size( );

            for( size_t i = 0; i < final_keys.size( ); i++ )
            {
               if( !final_key_info.empty( ) )
                  final_key_info += ',';

               final_key_info += final_keys[ i ];
            }

            if( final_key_info.empty( ) && !extra_key_info.empty( ) )
               final_key_info = c_key_field;

            final_key_info += extra_key_info;
         }

         split_key_info( final_key_info, fixed_info,
          paging_info, order_info, ( optimisation != e_sql_optimisation_unordered ) );

         if( !query.empty( ) )
         {
            vector< string > query_parts;

            split( query, query_parts );

            for( size_t i = 0; i < query_parts.size( ); i++ )
            {
               string::size_type pos = query_parts[ i ].find( ':' );

               if( pos == string::npos )
                  pos = query_parts[ i ].find( '=' ); // i.e. allow for either <field>:<value_info> or <field>=<value_info>

               if( pos == string::npos )
                  throw runtime_error( "unexpected query part format '" + query_parts[ i ] + "'" );

               string field_name( query_parts[ i ].substr( 0, pos ) );
               string field_values( query_parts[ i ].substr( pos + 1 ) );

               query_info.push_back( make_pair( field_name, field_values ) );
            }
         }

         if( persistence_type == 0 ) // i.e. SQL persistence
         {
            vector< string > order_columns;

            sql = construct_sql_select( instance,
             field_info, order_info, query_info, fixed_info, paging_info,
             ( direction == e_iter_direction_backwards ), inclusive, row_limit,
             ( fields == c_key_field ), text, &order_columns, ( !sec_marker.empty( ) ? &sec_marker : 0 ) );

            if( instance_accessor.p_sql_data( ) )
               delete instance_accessor.p_sql_data( );

            if( !sec_marker.empty( ) )
            {
               vector< string > sql_stmts;

               for( size_t i = 0; i < sec_values.size( ); i++ )
               {
                  sql_stmts.push_back( replaced( sql, sec_marker, sec_values[ i ] ) );

                  TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, sql_stmts.back( ) );
               }

               instance_accessor.p_sql_data( ) = new sql_dataset_group(
                get_sql_db( ), sql_stmts, ( direction == e_iter_direction_backwards ), false, &order_columns );

               // NOTE: Replace the 'sec_marker' so the SQL will be valid for creating a query plan.
               replace( sql, sec_marker, "0" );
            }
            else
            {
               TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, sql );

               instance_accessor.p_sql_data( ) = new sql_dataset( get_sql_db( ), sql );
            }

            setup_select_columns( instance, field_info );
         }
         else if( persistence_type == 1 ) // i.e. ODS local persistence
         {
            string prefix, origin;

            string folder( determine_local_iteration_info( instance, order_info, fixed_info, prefix ) );

            if( !paging_info.empty( ) )
            {
               for( size_t i = 0; i < paging_info.size( ); i++ )
               {
                  if( i > 0 )
                     origin += '\t';

                  origin += paging_info[ i ].second;
               }
            }

            string local_suffix_name( get_special_var_name( e_special_var_local_suffix ) );

            if( query_info.empty( ) )
               instance.set_variable( local_suffix_name, "" );
            else
            {
               // KLUDGE: Currently can only handle an extemely simplistic query.
               if( query_info.size( ) > 1 )
                  throw runtime_error( "complex query support not yet implemented" );

               string first_query_field( query_info[ 0 ].first );

               string last_order_field;

               // NOTE: Will allow a single field query that is on the last field
               // (or second last for non-unique indexes) in the order field list.
               if( !order_info.empty( ) )
               {
                  last_order_field = order_info[ order_info.size( ) - 1 ];

                  if( ( order_info.size( ) > 1 ) && ( last_order_field == c_key_field_name ) )
                     last_order_field = order_info[ order_info.size( ) - 2 ];
               }

               if( first_query_field != last_order_field )
                  throw runtime_error( "only a simple query for '" + last_order_field + "' is currently supported" );

               instance.set_variable( local_suffix_name, query_info[ 0 ].second );
            }

            instance.set_local_info( folder, origin, prefix );
         }
      }

      vector< string > instance_keys;

      bool skip_after_fetch = false;

      string skip_after_fetch_var(
       instance.get_raw_variable( get_special_var_name( e_special_var_skip_after_fetch ) ) );

      if( ( skip_after_fetch_var == c_true ) || ( skip_after_fetch_var == c_true_value ) )
         skip_after_fetch = true;

      size_t row_cache_limit = c_iteration_row_cache_limit;

      string row_cache_limit_value(
       get_session_variable( get_special_var_name( e_special_var_row_cache_limit ) ) );

      if( !row_cache_limit_value.empty( ) )
         row_cache_limit = from_string< size_t >( row_cache_limit_value );

      if( row_cache_limit < 2 )
         throw runtime_error( "unexpected invalid < 2 row_cache_limit" );

      if( ( row_limit < 0 ) && ( persistence_type != 0 ) ) // i.e. not SQL persistence
         row_limit = 0;

      if( row_limit < 0 )
         found = true;
      else
      {
         // NOTE: Unless a single row limit was specified (which is an alternate way of performing an
         // instance fetch) then iteration is flagged so that "after_fetch" triggers can detect this.
         if( row_limit != 1 )
         {
            if( !instance.get_is_iterating( ) )
            {
               instance.set_variable(
                get_special_var_name( e_special_var_loop ),
                to_comparable_string( 0, false, c_loop_variable_digits ) );

               instance_accessor.set_is_in_iteration( true, ( direction == e_iter_direction_forwards ) );
            }
         }

         if( persistence_type == 0 ) // i.e. SQL persistence
         {
            found = fetch_instance_from_db( instance,
             instance_accessor.select_fields( ), instance_accessor.select_columns( ), skip_after_fetch );
         }
         else if( persistence_type == 1 ) // i.e. ODS local persistence
         {
            size_t limit = ( row_limit > 0 ) ? row_limit : row_cache_limit;

            if( limit > row_cache_limit )
               limit = row_cache_limit;

            if( key_info == c_null_key )
            {
               string start_field_name( instance.get_variable( get_special_var_name( e_special_var_local_starts ) ) );

               if( start_field_name.empty( ) || ( start_field_name == c_key_field ) )
                  instance.set_local_origin( instance.get_key( ) );
               else
                  instance.set_local_origin( instance.get_field_value( instance.get_field_num( start_field_name ) ) );
            }

            fetch_keys_from_local_storage( instance,
             inclusive, limit, instance_keys, ( direction == e_iter_direction_backwards ) );

            found = ( instance_keys.size( ) > 0 );
         }
         else if( ( persistence_type == 2 ) || ( persistence_type == 3 ) ) // i.e. ODS global persistence or system variables
         {
            size_t limit = ( row_limit > 0 ) ? row_limit : row_cache_limit;

            if( limit > row_cache_limit )
               limit = row_cache_limit;

            string start_from( key_info );

            if( key_info == c_null_key )
               start_from = instance.get_key( );

            if( persistence_type == 2 ) // i.e. ODS global persistence
               fetch_keys_from_global_storage( instance,
                start_from, inclusive, limit, instance_keys, ( direction == e_iter_direction_backwards ) );
            else
               fetch_keys_from_system_variables( instance,
                start_from, inclusive, limit, instance_keys, ( direction == e_iter_direction_backwards ) );

            found = ( instance_keys.size( ) > 0 );
         }
         else
            throw runtime_error( "unexpected persistence type #"
               + to_string( persistence_type ) + " in perform_instance_iterate" );

         // NOTE: It is expected that the "after_fetch" trigger will be being skipped due to a later
         // "prepare" call which will call the trigger and then clear this flag (otherwise dependent
         // fields might be missed when the trigger is actually called).
         if( !skip_after_fetch )
            instance_accessor.set_iteration_starting( true );

         if( persistence_type == 0 ) // i.e. SQL persistence
         {
            session_inc_sql_count( );

            IF_IS_TRACING( TRACE_DETAILS | TRACE_QUERIES )
            {
               string sql_plan( "EXPLAIN " + sql );

               sql_dataset ds( get_sql_db( ), sql_plan );

               sql_plan = "QUERY PLAN:";

               while( ds.next( ) )
               {
                  sql_plan += '\n';

                  for( size_t i = 0; i < ds.get_fieldcount( ); i++ )
                  {
                     if( i > 0 )
                        sql_plan += " | ";

                     sql_plan += ds.as_string( i );
                  }
               }

               TRACE_LOG( TRACE_DETAILS | TRACE_QUERIES, sql_plan );
            }
         }
      }

      if( found )
      {
         bool found_next = false;
         bool query_finished = true;

         deque< vector< string > > rows;

         if( persistence_type == 0 ) // i.e. SQL persistence
         {
            if( !instance_accessor.p_sql_data( ) )
               throw runtime_error( "unexpected null sql_data in perform_instance_iterate" );

            sql_data& sd( *instance_accessor.p_sql_data( ) );

            while( sd.next( ) )
            {
               found_next = true;

               vector< string > columns;

               int fcount = sd.get_fieldcount( );

               for( int i = 0; i < fcount; i++ )
                  columns.push_back( sd.as_string( i ) );

               rows.push_back( columns );

               if( rows.size( ) == row_cache_limit )
               {
                  query_finished = false;
                  break;
               }
            }
         }
         else if( ( persistence_type >= 1 ) && ( persistence_type <= 3 ) ) // i.e. ODS local/global persistence or system variables
         {
            if( !instance_keys.empty( ) )
            {
               vector< int > field_nums;
               vector< string > field_names;

               int num_fields = instance.get_num_fields( );

               for( int i = 0; i < num_fields; i++ )
               {
                  if( !instance.is_field_transient( i ) )
                  {
                     field_nums.push_back( i );
                     field_names.push_back( instance.get_field_name( i ) );
                  }
               }

               instance_accessor.field_nums( ) = field_nums;

               for( size_t i = 0; i < instance_keys.size( ); i++ )
               {
                  if( ( i == 0 ) && ( key_info != c_null_key ) )
                  {
                     if( persistence_type == 1 ) // i.e. ODS local persistence
                        fetch_instance_from_local_storage(
                         instance, instance_keys[ i ], false, field_names, 0, skip_after_fetch );
                     else if( persistence_type == 2 ) // i.e. ODS global persistence
                        fetch_instance_from_global_storage(
                         instance, instance_keys[ i ], field_names, 0, skip_after_fetch );
                     else
                        fetch_instance_from_system_variable(
                         instance, instance_keys[ i ], field_names, 0, skip_after_fetch );
                  }
                  else
                  {
                     vector< string > columns;

                     if( persistence_type == 1 ) // i.e. ODS local persistence
                     {
                        if( !fetch_instance_from_local_storage( instance, instance_keys[ i ], false, field_names, &columns ) )
                           break;
                     }
                     else if( persistence_type == 2 ) // i.e. ODS global persistence
                     {
                        if( !fetch_instance_from_global_storage( instance, instance_keys[ i ], field_names, &columns ) )
                           break;
                     }
                     else
                     {
                        if( !fetch_instance_from_system_variable( instance, instance_keys[ i ], field_names, &columns ) )
                           break;
                     }

                     found_next = true;

                     rows.push_back( columns );
                  }

                  if( rows.size( ) == ( row_cache_limit - 1 ) )
                  {
                     query_finished = false;
                     break;
                  }
               }
            }
         }
         else
            throw runtime_error( "unexpected persistence type #"
               + to_string( persistence_type ) + " in perform_instance_iterate" );

         // NOTE: Put a dummy row at the end to stop iteration.
         if( query_finished && ( found_next || ( key_info != c_null_key ) ) )
            rows.push_back( vector< string >( ) );

         instance_accessor.row_cache( ) = rows;

         if( key_info == c_null_key )
         {
            if( !found_next )
               found = false;
            else
               fetch_instance_from_row_cache( instance, skip_after_fetch, persistence_type );
         }

         if( query_finished )
         {
            if( persistence_type == 0 ) // i.e. SQL persistence
            {
               delete instance_accessor.p_sql_data( );
               instance_accessor.p_sql_data( ) = 0;
            }
         }
      }

      if( !found )
      {
         if( persistence_type == 0 ) // i.e. SQL persistence
         {
            if( instance_accessor.p_sql_data( ) )
            {
               delete instance_accessor.p_sql_data( );
               instance_accessor.p_sql_data( ) = 0;
            }
         }

         instance_accessor.transient_filter_field_values( ).clear( );

         instance_accessor.set_is_in_iteration( false );
      }

      instance_accessor.set_iteration_starting( false );
   }

   return found;
}

bool perform_instance_iterate_next( class_base& instance )
{
   if( !instance.get_is_iterating( ) )
      return false;

   string class_id( instance.get_class_id( ) );
   string module_name( instance.get_module_name( ) );

   class_base_accessor instance_accessor( instance );

   if( instance.get_is_in_op( ) && !instance_accessor.get_in_op_begin( ) )
      throw runtime_error( "cannot continue iteration whilst currently perfoming an instance operation" );

   int persistence_type = instance.get_persistence_type( );

#ifdef COMPILE_PROTOTYPE_CODE
   string app_name( storage_name( ) );

   if( app_name == c_meta_storage_name )
      persistence_type = 1;
#endif

   bool found = false, cache_depleted = false;

   if( !instance_accessor.row_cache( ).empty( ) )
   {
      if( ( instance_accessor.row_cache( ).size( ) == 1 ) && instance_accessor.row_cache( )[ 0 ].empty( ) )
      {
         cache_depleted = true;

         instance.iterate_stop( );
      }
      else
      {
         found = true;

         bool skip_after_fetch = false;

         string skip_after_fetch_var(
          instance.get_raw_variable( get_special_var_name( e_special_var_skip_after_fetch ) ) );

         if( ( skip_after_fetch_var == c_true ) || ( skip_after_fetch_var == c_true_value ) )
            skip_after_fetch = true;

         fetch_instance_from_row_cache( instance, skip_after_fetch, persistence_type );
      }
   }

   string loop_var_name( get_special_var_name( e_special_var_loop ) );

   int loop_num = atoi( instance.get_raw_variable( loop_var_name ).c_str( ) );

   instance.set_variable( loop_var_name, to_comparable_string( ++loop_num, false, c_loop_variable_digits ) );

   if( found || cache_depleted )
      return found;
   else
      return perform_instance_iterate( instance, c_null_key, "", "", "",
       instance.get_is_in_forwards_iteration( ) ? e_iter_direction_forwards : e_iter_direction_backwards, false, -1 );
}

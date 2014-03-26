// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cerrno>
#  include <cstdio>
#  include <climits>
#  include <cassert>
#  include <locale>
#  include <sstream>
#  include <fstream>
#  include <iomanip>
#  include <iostream>
#  ifdef __BORLANDC__
#     include <dir.h>
#  endif
#  ifdef _MSC_VER
#     include <direct.h>
#  endif
#  ifdef _WIN32
#     include <ctime>
#  endif
#  ifdef __GNUG__
#     include <unistd.h>
#     include <sys/stat.h>
#     include <sys/time.h>
#  endif
#endif

#define CIYAM_BASE_IMPL

#include "class_base.h"

#include "sio.h"
#include "pop3.h"
#include "mime.h"
#include "smtp.h"
#include "sha1.h"
#include "regex.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "sha256.h"
#include "sql_db.h"
#include "numeric.h"
#include "hashcash.h"
#include "date_time.h"
#include "tx_create.h"
#include "file_utils.h"
#include "ciyam_base.h"
#include "mail_source.h"
#include "oid_pointer.h"
#include "ciyam_common.h"
#include "crypt_stream.h"
#include "module_management.h"

#ifdef ICONV_SUPPORT
#  include <iconv.h>
#endif

#ifdef __GNUG__
#  define _chdir chdir
#  define _mkdir mkdir
#  define _getcwd getcwd
#  define _MAX_PATH PATH_MAX
#endif

using namespace std;

namespace
{

const string g_empty_fixed_key;

const int c_max_graph_depth = 50;
const int c_max_email_text_line = 8192;

const char* const c_files_directory = "files";

const char* const c_email_subject_script_marker = "[CIYAM]";

const char* const c_special_regex_for_email_address = "@email_address";

const char* const c_section_timezone = "timezone";
const char* const c_section_timezones = "timezones";
const char* const c_section_daylight_saving_change = "daylight_saving_change";
const char* const c_section_daylight_saving_changes = "daylight_saving_changes";

const char* const c_attribute_abbr = "abbr";
const char* const c_attribute_name = "name";
const char* const c_attribute_tz_info = "tz_info";
const char* const c_attribute_utc_offset = "utc_offset";
const char* const c_attribute_description = "description";
const char* const c_attribute_daylight_abbr = "daylight_abbr";
const char* const c_attribute_daylight_year_start = "daylight_year_start";
const char* const c_attribute_daylight_year_finish = "daylight_year_finish";
const char* const c_attribute_daylight_start_month = "daylight_start_month";
const char* const c_attribute_daylight_start_occurrence = "daylight_start_occurrence";
const char* const c_attribute_daylight_start_day_of_week = "daylight_start_day_of_week";
const char* const c_attribute_daylight_start_time = "daylight_start_time";
const char* const c_attribute_daylight_finish_month = "daylight_finish_month";
const char* const c_attribute_daylight_finish_occurrence = "daylight_finish_occurrence";
const char* const c_attribute_daylight_finish_day_of_week = "daylight_finish_day_of_week";
const char* const c_attribute_daylight_finish_time = "daylight_finish_time";
const char* const c_attribute_daylight_utc_offset = "daylight_utc_offset";
const char* const c_attribute_year_start = "year_start";
const char* const c_attribute_year_finish = "year_finish";
const char* const c_attribute_start_month = "start_month";
const char* const c_attribute_start_occurrence = "start_occurrence";
const char* const c_attribute_start_day_of_week = "start_day_of_week";
const char* const c_attribute_start_time = "start_time";
const char* const c_attribute_finish_month = "finish_month";
const char* const c_attribute_finish_occurrence = "finish_occurrence";
const char* const c_attribute_finish_day_of_week = "finish_day_of_week";
const char* const c_attribute_finish_time = "finish_time";

const char* const c_gpg_key_fingerprint_prefix = "Key fingerprint = ";

typedef map< string, size_t > foreign_key_lock_container;
typedef foreign_key_lock_container::iterator foreign_key_lock_iterator;
typedef foreign_key_lock_container::const_iterator foreign_key_lock_const_iterator;

timezone_container g_timezones;

map< string, string > g_timezone_abbrs;
map< string, string > g_daylight_names;

mutex g_mutex;

map< string, pair< int, map< string, string > > > g_class_maps;

const string& attached_file_path_var_name( )
{
   static string s( get_special_var_name( e_special_var_attached_file_path ) );
   return s;
}

void deconstruct_original_identity( class_base& cb, string& module_id, string& class_id )
{
   string identity( cb.get_original_identity( ) );
   string::size_type pos = identity.find( ':' );
   if( pos == string::npos )
      throw runtime_error( "unexpected identity format" );

   module_id = identity.substr( 0, pos );
   class_id = identity.substr( pos + 1 );
}

void remove_suffix( string& str, const string& suffix, const string& separator )
{
   string::size_type pos = str.rfind( suffix );
   if( pos != string::npos && pos == str.length( ) - suffix.length( ) )
   {
      str.erase( pos );
      if( !separator.empty( ) && str.length( ) >= separator.length( ) )
      {
         pos = str.rfind( separator );
         if( pos != string::npos && pos == str.length( ) - separator.length( ) )
            str.erase( pos );
      }
   }
}

string get_mask( int numeric_digits,
 int numeric_decimals, bool is_numeric, bool show_plus_sign, int zero_padding, bool include_commas = true )
{
   string mask;

   if( show_plus_sign )
      mask = "*";
   else
      mask = "-";

   int whole_digits = numeric_digits;
   if( !whole_digits )
      whole_digits = is_numeric ? 19 : 10;

   whole_digits -= numeric_decimals;

   if( zero_padding < 2 )
      mask += string( whole_digits, '#' );
   else
      mask += string( whole_digits, '0' );

   if( numeric_decimals )
   {
      mask += '.';
      if( zero_padding < 1 )
         mask += string( numeric_decimals, '#' );
      else
         mask += string( numeric_decimals, '0' );
   }

   if( include_commas )
   {
      string::size_type pos = mask.find( '.' );
      if( pos == string::npos )
         pos = mask.length( ) - 1;
      else if( pos > 0 )
         --pos;

      while( pos > 3 )
      {
         mask.insert( pos - 2, "," );
         pos -= 3;
      }
   }

   return mask;
}

date_time local_utc_conv( const date_time& dt, int utc_offset,
 daylight_savings_info* p_daylight_savings_info, bool to_local, int* p_offset = 0, bool is_daylight = false )
{
   int bias = 0;
   date_time retval( dt );

   if( p_daylight_savings_info && !p_daylight_savings_info->years_info.empty( ) )
   {
      years_info_const_iterator yici;
      yici = p_daylight_savings_info->years_info.lower_bound( dt.get_year( ) );

      // NOTE: If no entry found then it is assumed that DST is no longer applicable.
      if( yici != p_daylight_savings_info->years_info.end( ) )
      {
         // NOTE: If the entry found was greater than the current year then look for a previous entry.
         if( yici != p_daylight_savings_info->years_info.begin( ) && yici->first > dt.get_year( ) )
            --yici;

         // NOTE: If the year being checked is not equal to the DST entry found then it is assumed
         // that DST was not being used for that year.
         if( dt.get_year( ) == yici->first )
         {
            string begin( yici->second.begin );
            string finish( yici->second.finish );

            date_time dt_check( dt );

            // NOTE: If converting from UTC then need to add the standard offset before testing (as
            // the daylight savings date pairs are expected to be passed as "standard local times").
            if( to_local )
               dt_check += ( seconds )utc_offset;

            date_time dt_begin( begin );
            date_time dt_finish( finish );

            if( !to_local && is_daylight )
               dt_finish += ( seconds )yici->second.bias;

            if( dt_begin < dt_finish )
            {
               if( dt_check >= dt_begin && dt_check < dt_finish )
                  bias = yici->second.bias;
            }
            else
            {
               if( dt_check < dt_finish || dt_check >= dt_begin )
                  bias = yici->second.bias;
            }
         }
      }
   }

   if( p_offset )
      *p_offset = utc_offset + bias;

   if( !to_local )
      retval -= ( seconds )( utc_offset + bias );
   else
      retval += ( seconds )( utc_offset + bias );

   return retval;
}

string decode_text( const string& encoding, const string& charset, const string& data )
{
   string decoded;

   if( encoding == "7bit" || encoding == "8bit" || encoding.empty( ) )
      decoded = data;
   else if( encoding == "b" || encoding == "base64" )
   {
      vector< string > lines;
      raw_split( data, lines, '\n' );

      decoded = base64::decode( raw_join( lines ) );
   }
   else if( encoding == "q" || encoding == "quoted-printable" )
      decoded = decode_quoted_printable( data );
   else
      throw runtime_error( "unexpected text encoding '" + encoding + "'" );

#ifndef ICONV_SUPPORT
   ( void )charset;
#else
   string cs( lower( charset ) );
   if( cs != "utf-8" && cs != "us-ascii" && cs != "iso-8859-1" )
   {
      vector< string > lines;
      split( decoded, lines, '\n' );

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string& next_line( lines[ i ] );
         iconv_t cd;

         cd = iconv_open( "utf-8", cs.c_str( ) );
         if( cd == ( iconv_t )-1 )
         {
            if( errno == EINVAL )
               throw runtime_error( "conversion from '" + cs + "' to UTF-8 not available" );
            else
               throw runtime_error( "unexpected iconv_open error" );
         }

         char buffer[ c_max_email_text_line ];
         memset( buffer, '\0', sizeof( buffer ) );

         size_t isize = next_line.length( );
         size_t avail = c_max_email_text_line - 1;

         char* p_buf = buffer;
         const char* p_src = next_line.c_str( );

#  ifdef _WIN32
         size_t rc = iconv( cd, &p_src, &isize, &p_buf, &avail );
#  else
         size_t rc = iconv( cd, ( char** )&p_src, &isize, &p_buf, &avail );
#  endif

         if( rc < 0 )
         {
            if( errno == EILSEQ )
               throw runtime_error( "invalid " + charset + " character sequence" );
            else if( errno == E2BIG )
               throw runtime_error( "output buffer too small for conversion" );
            else if( errno == EINVAL )
               throw runtime_error( "incomplete " + charset + " character sequence" );
            else if( errno == EBADF )
               throw runtime_error( "invalid conversion descriptor" );

            // NOTE: If error is not known then will just skip conversion for that
            // line (as it has been noticed that headers which are placed into the
            // body by some email clients can end up incorrectly encoded).
         }
         else
            next_line = string( buffer );

         if( iconv_close( cd ) != 0 )
            throw runtime_error( "unexpected iconv_close error" );
      }

      decoded = join( lines, '\n' );
   }
#endif
   return decoded;
}

string replace_charset( const string& html_content )
{
   string s;
   string tag;
   bool in_tag = false;
   for( size_t i = 0; i < html_content.size( ); i++ )
   {
      if( html_content[ i ] == '<' )
         in_tag = true;
      else if( in_tag )
      {
         if( html_content[ i ] != '>' )
            tag += html_content[ i ];
         else
         {
            in_tag = false;
            string::size_type pos = lower( tag ).find( "charset=" );
            if( pos != string::npos )
            {
               string::size_type epos = tag.find( "\"", pos );
               if( epos != string::npos )
               {
                  string append = tag.substr( epos );
                  tag.erase( pos );
                  tag += "charset=UTF-8";
                  tag += append;

                  s += "<" + tag + ">";

                  s += html_content.substr( i + 1 );
                  break;
               }
            }

            s += "<" + tag + ">";
            tag.erase( );
         }
      }
      else
         s += html_content[ i ];
   }

   return s;
}

void decode_mime( mime_decoder& decoder, string& message,
 string& html_message, vector< pair< string, string > >& attachments )
{
   if( decoder.get_type( ) == "text" && decoder.get_subtype( ) == "plain" )
      message = decode_text( decoder.get_encoding( ), decoder.get_attribute( ), decoder.get_text_data( ) );
   else if( decoder.get_type( ) == "text" && decoder.get_subtype( ) == "html" )
      html_message = decode_text( decoder.get_encoding( ), decoder.get_attribute( ), decoder.get_text_data( ) );

   for( size_t i = 0; i < decoder.num_parts( ); i++ )
   {
      mime_part& next_part( decoder.get_part( i ) );

      if( next_part.type == "text" && next_part.subtype == "plain" )
         message = decode_text( next_part.encoding, next_part.attribute, next_part.data );
      else if( next_part.type == "text" && next_part.subtype == "html" )
         html_message = decode_text( next_part.encoding, next_part.attribute, next_part.data );
      else if( next_part.type == "image" || next_part.type == "application" )
         attachments.push_back( make_pair( next_part.encoding + " " + next_part.attribute, next_part.data ) );
   }

   if( !html_message.empty( ) )
      html_message = replace_charset( html_message );

   if( decoder.has_child( ) )
      decode_mime( decoder.get_child( ), message, html_message, attachments );
}

string escape_sep_if_quoted( const string& s, char sep )
{
   string str;

   bool in_quotes = false;
   for( size_t i = 0; i < s.length( ); i++ )
   {
      if( s[ i ] == '"' )
         in_quotes = !in_quotes;

      if( in_quotes && s[ i ] == sep )
         str += '\\';

      str += s[ i ];
   }

   return str;
}

#include "trace_progress.cpp"

}

struct class_cascade::impl
{
   impl( class_base& cb ) : tmp_being_cascaded( cb.is_being_cascaded, true ) { }

   restorable< bool > tmp_being_cascaded;
};

class_cascade::class_cascade( class_base& cb )
{
   p_impl = new impl( cb );
}

class_cascade::~class_cascade( )
{
   delete p_impl;
}

struct class_after_store::impl
{
   impl( class_base& cb ) : tmp_being_cascaded( cb.is_after_store, true ) { }

   restorable< bool > tmp_being_cascaded;
};

class_after_store::class_after_store( class_base& cb )
{
   p_impl = new impl( cb );
}

class_after_store::~class_after_store( )
{
   delete p_impl;
}

struct class_base::impl
{
   bool has_changed_user_fields;
   map< string, string > variables;

   search_replace_container search_replacements;

   foreign_key_lock_container foreign_key_locks;
   foreign_key_data_container foreign_key_values;

   void release_fk_locks( );
};

void class_base::impl::release_fk_locks( )
{
   for( foreign_key_lock_const_iterator
    fklci = foreign_key_locks.begin( ), end = foreign_key_locks.end( ); fklci != end; ++fklci )
      release_obtained_lock( fklci->second );

   foreign_key_locks.clear( );
}

void perform_lazy_fetch( class_base* p_class_base )
{
   p_class_base->perform_lazy_fetch( );
}

class_base::class_base( )
 :
 version( 0 ),
 revision( 0 ),
 op( e_op_type_none ),
 index_num( 0 ),
 lock_handle( 0 ),
 xlock_handle( 0 ),
 p_sql_dataset( 0 ),
 p_graph_parent( 0 ),
 in_op_begin( false ),
 is_singular( false ),
 is_fetching( false ),
 is_executing( false ),
 is_preparing( false ),
 is_after_store( false ),
 is_being_cascaded( false ),
 is_dynamic_enabled( false ),
 iteration_starting( false ),
 in_forwards_iteration( false ),
 in_backwards_iteration( false ),
 p_owning_instance( 0 ),
 p_class_pointer_base( 0 )
{
   p_impl = new impl;
   p_dynamic_instance = this;

   TRACE_LOG( TRACE_CTR_DTRS, "class_base( ) [this = " + to_string( this ) + "]" );
}

bool class_base::class_has_derivations( ) const
{
   return get_module_class_has_derivations( get_module_id( ), get_class_id( ) );
}

void class_base::set_dynamic_if_class_has_derivations( )
{
   is_dynamic_enabled = get_module_class_has_derivations( get_module_id( ), get_class_id( ) );
}

class_base::~class_base( )
{
   TRACE_LOG( TRACE_CTR_DTRS, "~class_base( ) [this = "
    + to_string( this ) + ", owner = " + to_string( p_owning_instance ) + "]" );

   if( p_class_pointer_base )
      p_class_pointer_base->instance_destroyed( );

   if( p_dynamic_instance != this )
      delete p_dynamic_instance;

   delete p_sql_dataset;
   delete p_impl;
}

void class_base::op_create( const string& key, op_create_rc* p_rc, bool is_internal )
{
   fetch_field_names.clear( );
   last_lazy_fetch_key.erase( );

   instance_op_rc rc;
   begin_instance_op( e_instance_op_create, *this, key, is_internal, p_rc ? &rc : 0 );

   if( p_rc )
   {
      switch( rc )
      {
         case e_instance_op_rc_okay:
         *p_rc = e_op_create_rc_okay;
         break;

         case e_instance_op_rc_locked:
         *p_rc = e_op_create_rc_locked;
         break;

         case e_instance_op_rc_already_exists:
         *p_rc = e_op_create_rc_exists;
         break;

         default:
         throw runtime_error( "unexpected instance_op_rc #" + to_string( rc ) + " in op_create" );
      }
   }
}

void class_base::op_update( const string& key, const string& fields, op_update_rc* p_rc, bool is_internal )
{
   fetch_field_names.clear( );
   last_lazy_fetch_key.erase( );

   bool could_be_dynamic = false;
   if( is_dynamic_enabled )
   {
      instance_fetch_rc rc;
      perform_instance_fetch( *this, key, p_rc ? &rc : 0, true );

      if( p_rc )
      {
         if( rc == e_instance_fetch_rc_not_found )
         {
            *p_rc = e_op_update_rc_not_found;
            return;
         }
      }

      could_be_dynamic = true;
      construct_dynamic_instance( );

      if( is_being_cascaded )
         p_dynamic_instance->is_being_cascaded = true;
   }

   if( fields.empty( ) )
      utype = e_update_type_normal;
   else
   {
      utype = e_update_type_minimal;
      split( fields, fetch_field_names );
   }

   try
   {
      instance_op_rc rc;
      begin_instance_op( e_instance_op_update, *p_dynamic_instance, key, is_internal, p_rc ? &rc : 0 );

      if( p_rc )
      {
         switch( rc )
         {
            case e_instance_op_rc_okay:
            *p_rc = e_op_update_rc_okay;
            break;

            case e_instance_op_rc_locked:
            *p_rc = e_op_update_rc_locked;
            break;

            case e_instance_op_rc_not_found:
            *p_rc = e_op_update_rc_not_found;
            break;

            default:
            throw runtime_error( "unexpected instance_op_rc #" + to_string( rc ) + " in op_update" );
         }
      }

      if( could_be_dynamic && p_dynamic_instance->op != e_op_type_update )
         cleanup_dynamic_instance( );
   }
   catch( ... )
   {
      if( could_be_dynamic )
         cleanup_dynamic_instance( );
      throw;
   }
}

void class_base::op_destroy( const string& key, op_destroy_rc* p_rc, bool is_internal )
{
   fetch_field_names.clear( );
   last_lazy_fetch_key.erase( );

   bool could_be_dynamic = false;
   if( is_dynamic_enabled )
   {
      instance_fetch_rc rc;
      perform_instance_fetch( *this, key, p_rc ? &rc : 0, true );

      if( p_rc )
      {
         if( rc == e_instance_fetch_rc_not_found )
         {
            *p_rc = e_op_destroy_rc_not_found;
            return;
         }
      }

      could_be_dynamic = true;
      construct_dynamic_instance( );

      if( is_being_cascaded )
         p_dynamic_instance->is_being_cascaded = true;
   }

   try
   {
      instance_op_rc rc;
      begin_instance_op( e_instance_op_destroy, *p_dynamic_instance, key, is_internal, p_rc ? &rc : 0 );

      if( p_rc )
      {
         switch( rc )
         {
            case e_instance_op_rc_okay:
            *p_rc = e_op_destroy_rc_okay;
            break;

            case e_instance_op_rc_locked:
            *p_rc = e_op_destroy_rc_locked;
            break;

            case e_instance_op_rc_not_found:
            *p_rc = e_op_destroy_rc_not_found;
            break;

            case e_instance_op_rc_constrained:
            *p_rc = e_op_destroy_rc_constrained;
            break;

            default:
            throw runtime_error( "unexpected instance_op_rc #" + to_string( rc ) + " in op_destroy" );
         }
      }

      if( could_be_dynamic && p_dynamic_instance->op != e_op_type_destroy )
         cleanup_dynamic_instance( );
   }
   catch( ... )
   {
      if( could_be_dynamic )
         cleanup_dynamic_instance( );
      throw;
   }
}

void class_base::op_apply( op_apply_rc* p_rc, bool is_internal, set< string >* p_fields_set )
{
   instance_op_rc rc;
   finish_instance_op( *p_dynamic_instance, true, is_internal, p_rc ? &rc : 0, p_fields_set );

   if( p_rc )
      *p_rc = ( op_apply_rc )( int )rc;

   if( p_dynamic_instance->op == e_op_type_none )
      cleanup_dynamic_instance( );
}

void class_base::op_cancel( bool is_internal )
{
   finish_instance_op( *p_dynamic_instance, false, is_internal );

   cleanup_dynamic_instance( );
}

void class_base::init( bool for_create )
{
   bool is_create( op == e_op_type_create );

   set_default_values( );

   if( is_create || for_create )
      at_create( );

   original_values.clear( );

   int num_fields = get_num_fields( );
   for( size_t i = 0; i < num_fields; i++ )
      original_values.push_back( get_field_value( i ) );

   p_impl->foreign_key_values.clear( );
   get_foreign_key_values( p_impl->foreign_key_values );
}

void class_base::prepare( bool for_create, bool call_to_store )
{
   bool is_create( op == e_op_type_create );

   TRACE_LOG( TRACE_CLASSOPS, "prepare( ) [class: " + get_class_name( )
    + "] is_create = " + to_string( is_create ) + ", for_create = " + to_string( for_create ) );

   restorable< bool > tmp_is_preparing( is_preparing, true );

   // NOTE: As "prepare" is expected to be called after field values have been set
   // the call to "perform_after_fetch" is occurring here (rather than immediately
   // after the physical fetch) in order to make sure that transient values can be
   // determined correctly.
   perform_after_fetch( false, true );

   set_iteration_starting( false ); // NOTE: as per above

   if( call_to_store )
      to_store( is_create || for_create, false );
}

bool class_base::is_valid( bool is_internal, set< string >* p_fields_set )
{
   validation_errors.clear( );

   post_init( );

   if( !p_fields_set )
      validate( get_state( ), is_internal );
   else
      validate_set_fields( *p_fields_set );

   IF_IS_TRACING( TRACE_CLASSOPS )
   {
      for( validation_error_const_iterator veci = validation_errors.begin( ), end = validation_errors.end( ); veci != end; ++veci )
         TRACE_LOG( TRACE_CLASSOPS, "[validation error] (" + veci->first + ") " + veci->second );
   }

   return validation_errors.empty( );
}

bool class_base::has_changed( ) const
{
   // NOTE: For foreign keys only report having changed if the current key is not
   // the same as the original. For other situations this function will report if
   // any field has changed.
   if( !p_graph_parent || graph_parent_fk_field.empty( ) )
   {
      int num_fields = get_num_fields( );

      for( size_t i = 0; i < num_fields; i++ )
      {
         if( has_field_changed( i ) )
            return true;
      }

      return false;
   }

   string original_key;
   if( p_graph_parent->p_impl->foreign_key_values.count( graph_parent_fk_field ) )
      original_key = p_graph_parent->p_impl->foreign_key_values[ graph_parent_fk_field ];

   return get_key( ) != original_key;
}

void class_base::link_to_graph_parent( )
{
   if( !p_graph_parent || !p_graph_parent->get_is_editing( ) )
      throw runtime_error( "require a graph parent being edited to perform this operation" );

   set_key( get_key( ) );
}

void class_base::unlink_from_graph_parent( )
{
   if( !p_graph_parent || !p_graph_parent->get_is_editing( ) )
      throw runtime_error( "require a graph parent being edited to perform this operation" );

   set_key( "" );
}

void class_base::begin_review( const string& key, begin_review_rc* p_rc )
{
   bool could_be_dynamic = false;
   if( is_dynamic_enabled )
   {
      instance_fetch_rc rc;
      perform_instance_fetch( *this, key, p_rc ? &rc : 0, true );

      if( p_rc )
      {
         if( rc == e_instance_fetch_rc_not_found )
         {
            *p_rc = e_begin_review_rc_not_found;
            return;
         }
      }

      could_be_dynamic = true;
      construct_dynamic_instance( );
   }

   last_lazy_fetch_key.erase( );

   try
   {
      instance_op_rc rc;
      begin_instance_op( e_instance_op_review, *p_dynamic_instance, key, true, p_rc ? &rc : 0 );

      if( p_rc )
         *p_rc = ( begin_review_rc )( int )rc;

      if( could_be_dynamic && p_dynamic_instance->op != e_op_type_review )
         cleanup_dynamic_instance( );
   }
   catch( ... )
   {
      if( could_be_dynamic )
         cleanup_dynamic_instance( );
      throw;
   }
}

void class_base::finish_review( )
{
   finish_instance_op( *this, false, true );

   cleanup_dynamic_instance( );
}

void class_base::perform_fetch( const string& key_info, perform_fetch_rc* p_rc )
{
   instance_fetch_rc rc;

   perform_instance_fetch( *this, key_info, p_rc ? &rc : 0 );

   if( p_rc )
      *p_rc = ( perform_fetch_rc )( int )rc;
}

void class_base::perform_lazy_fetch( )
{
   // NOTE: Call back through all graph parents in order to
   // perform any required lazy fetches for the graph branch.
   if( p_graph_parent )
      p_graph_parent->perform_lazy_fetch( );

   if( !lazy_fetch_key.empty( ) )
   {
      string s( lazy_fetch_key );
      lazy_fetch_key.erase( );

      // NOTE: If not in an op and the new lazy fetch key is the same as the previous
      // one then don't perform the fetch (i.e. optimsation to avoid excess queries).
      if( ( get_is_in_op( ) && !get_in_op_begin( ) ) || s != last_lazy_fetch_key )
      {
         perform_fetch_rc rc;
         perform_fetch( s, &rc );

         last_lazy_fetch_key = s;
         last_lazy_fetch_ver = version;
         last_lazy_fetch_rev = revision;

         last_lazy_fetch_field_values.clear( );

         int num_fields = get_num_fields( );
         for( size_t i = 0; i < num_fields; i++ )
            last_lazy_fetch_field_values.push_back( get_field_value( i ) );
      }
      else
      {
         key = last_lazy_fetch_key;
         version = last_lazy_fetch_ver;
         revision = last_lazy_fetch_rev;

         for( size_t i = 0; i < last_lazy_fetch_field_values.size( ); i++ )
            set_field_value( i, last_lazy_fetch_field_values[ i ] );
      }
   }
}

void class_base::ensure_original_fetch( )
{
   // NOTE: For the purpose of iterating child records without unnecessary fetching
   // a parent's key can simply be set - so to ensure the record will be fetched if
   // this has occurred (and a field is later requested) the lazy fetch key will be
   // set here.
   if( !key.empty( ) && original_identity.empty( ) )
      lazy_fetch_key = key;
}

bool class_base::iterate_forwards( bool inclusive, int row_limit, sql_optimisation optimisation )
{
   return perform_instance_iterate( *this, "", "", "", "", "",
    e_iter_direction_forwards, inclusive, row_limit, optimisation );
}

bool class_base::iterate_forwards( const string& key_info,
 bool inclusive, int row_limit, sql_optimisation optimisation )
{
   return perform_instance_iterate( *this, key_info, "", "", "", "",
    e_iter_direction_forwards, inclusive, row_limit, optimisation );
}

bool class_base::iterate_forwards( const string& key_info,
 const string& fields, bool inclusive, int row_limit, sql_optimisation optimisation )
{
   return perform_instance_iterate( *this, key_info, fields, "", "", "",
    e_iter_direction_forwards, inclusive, row_limit, optimisation );
}

bool class_base::iterate_backwards( bool inclusive, int row_limit, sql_optimisation optimisation )
{
   return perform_instance_iterate( *this, "", "", "", "", "",
    e_iter_direction_backwards, inclusive, row_limit, optimisation );
}

bool class_base::iterate_backwards( const string& key_info,
 bool inclusive, int row_limit, sql_optimisation optimisation )
{
   return perform_instance_iterate( *this, key_info, "", "", "", "",
    e_iter_direction_backwards, inclusive, row_limit, optimisation );
}

bool class_base::iterate_backwards( const string& key_info,
 const string& fields, bool inclusive, int row_limit, sql_optimisation optimisation )
{
   return perform_instance_iterate( *this, key_info, fields, "", "", "",
    e_iter_direction_backwards, inclusive, row_limit, optimisation );
}

bool class_base::iterate_next( )
{
   return perform_instance_iterate_next( *this );
}

void class_base::iterate_stop( )
{
   cleanup_dynamic_instance( );

   iteration_starting = false;
   in_forwards_iteration = false;
   in_backwards_iteration = false;

   perform_instance_iterate_stop( *this );
}

void class_base::set_instance( const string& key )
{
   if( get_is_in_op( ) || get_is_iterating( ) )
      throw runtime_error( "cannot set instance key whilst iterating or performing an instance operation" );

   set_key( key );
}

void class_base::copy_all_field_values( const class_base& src )
{
   if( get_class_id( ) != src.get_class_id( ) )
      throw runtime_error( "cannot copy all field values from a '"
       + src.get_class_name( ) + "' to a '" + get_class_name( ) + "'" );

   size_t num_fields( get_num_fields( ) );
   for( size_t i = 0; i < num_fields; i++ )
   {
      // NOTE: If either the source or destination is in a "minimal update" then only copy
      // those fields that have been fetched (otherwise fields will incorrectly be changed).
      if( ( op != e_op_type_update
       || utype != e_update_type_minimal
       || fetch_field_names.count( get_field_name( i ) ) )
       && ( src.op != e_op_type_update
       || src.utype != e_update_type_minimal
       || src.fetch_field_names.count( src.get_field_name( i ) ) ) )
         set_field_value( i, src.get_field_value( i ) );
   }
}

void class_base::copy_original_field_values( const class_base& src )
{
   if( get_class_id( ) != src.get_class_id( ) )
      throw runtime_error( "cannot copy all field values from a '"
       + src.get_class_name( ) + "' to a '" + get_class_name( ) + "'" );

   if( get_num_fields( ) != src.original_values.size( ) )
      throw runtime_error( "unexpected get_num_fields( ) != src.original_values.size( )" );

   for( size_t i = 0; i < src.original_values.size( ); i++ )
      set_field_value( i, src.original_values[ i ] );
}

bool class_base::filtered( ) const
{
   if( !transient_filter_field_values.empty( ) )
   {
      for( map< string, string >::const_iterator
       ci = transient_filter_field_values.begin( ); ci != transient_filter_field_values.end( ); ++ci )
      {
         if( get_field_value( get_field_num( ci->first ) ) != ci->second )
            return true;
      }
   }

   return filters.empty( ) ? false : is_filtered( );
}

string class_base::get_validation_errors( validation_errors_type type )
{
   string retval;
   for( validation_error_iterator vei = validation_errors.begin( ), end = validation_errors.end( ); vei != end; ++vei )
   {
      if( !retval.empty( ) )
         retval += '\n';
      retval += vei->second;

      if( type == e_validation_errors_type_first_only )
         break;
   }

   return retval;
}

string class_base::get_version_info( ) const
{
   return to_string( version ) + "." + to_string( revision );
}

const string class_base::get_current_identity( ) const
{
   return construct_class_identity( *this );
}

const string& class_base::get_key( ) const
{
   // NOTE: As graph child (foreign key) objects are often only accessed with
   // non-const getters a parent's (and possibly grandparent's, etc...) fetch
   // may not have occurred prior to requesting the key. So such fetches will
   // be peformed prior to returing the key.
   if( p_graph_parent )
      lazy_fetch( p_graph_parent );

   return key;
}

const string& class_base::get_fixed_key( ) const
{
   return g_empty_fixed_key;
}

string class_base::get_parent_key( ) const
{
   string parent_key;

   if( p_graph_parent )
      parent_key = p_graph_parent->get_key( );

   return parent_key;
}

string get_app_dir( )
{
   return lower( storage_name( ) );
}

string get_app_file( const string& module_name )
{
   string app_file, web_root( storage_web_root( true ) );

   if( !exists_file( web_root + "/" + lower( module_name ) + ".html" ) )
      app_file = "index.html";
   else
      app_file = lower( module_name ) + ".html";

   return app_file;
}

string class_base::get_attached_file_path( const string& file_name ) const
{
   string path( get_session_variable( attached_file_path_var_name( ) ) );

   if( path.empty( ) )
   {
      path = storage_web_root( true );
      path += "/" + string( c_files_directory ) + "/" + get_module_id( ) + "/" + get_class_id( );
   }

   if( !file_name.empty( ) )
      path += "/" + file_name;

   return path;
}

int class_base::get_graph_depth( ) const
{
   int depth = 1;
   class_base* p_next( p_graph_parent );

   while( p_next )
   {
      ++depth;
      p_next = p_next->p_graph_parent;
   }

   return depth;
}

class_base* class_base::get_graph_root( )
{
   class_base* p_last( this );
   class_base* p_next( p_graph_parent );

   while( p_next )
   {
      p_last = p_next;
      p_next = p_next->p_graph_parent;
   }

   return p_last;
}

bool class_base::has_field_changed( int field ) const
{
   bool has_changed = true;

   if( field < original_values.size( ) && original_values[ field ] == get_field_value( field ) )
      has_changed = false;

   return has_changed;
}

string class_base::get_variable( const string& vname ) const
{
   return p_impl->variables[ vname ];
}

void class_base::set_variable( const string& vname, const string& value )
{
   if( value.empty( ) )
   {
      if( p_impl->variables.count( vname ) )
         p_impl->variables.erase( vname );
   }
   else
   {
      if( p_impl->variables.count( vname ) )
         p_impl->variables[ vname ] = value;
      else
         p_impl->variables.insert( make_pair( vname, value ) );
   }
}

bool class_base::needs_field_value( const string& field_name ) const
{
   return fetch_field_names.empty( ) || fetch_field_names.count( field_name );
}

bool class_base::needs_field_value( const string& field_name, const set< string >& names ) const
{
   return fetch_field_names.empty( ) || fetch_field_names.count( field_name ) || names.count( field_name );
}

string class_base::get_original_field_value( int field ) const
{
   string str;

   if( field < original_values.size( ) )
      str = original_values[ field ];

   return str;
}

bool class_base::get_sql_stmts( vector< string >& sql_stmts, set< string >& tx_key_info )
{
   bool retval = false;
   sql_stmts.clear( );

   switch( op )
   {
      case e_op_type_none:
      case e_op_type_review:
      break;

      case e_op_type_create:
      revision = 0;
      original_identity = construct_class_identity( *this );
      do_generate_sql( e_generate_sql_type_insert, sql_stmts, tx_key_info );
      /* drop through */

      case e_op_type_update:
      ++revision;
      p_impl->has_changed_user_fields = false;

      if( sql_stmts.empty( ) )
         do_generate_sql( e_generate_sql_type_update, sql_stmts, tx_key_info );

      retval = true;
      break;

      case e_op_type_destroy:
      do_generate_sql( e_generate_sql_type_delete, sql_stmts, tx_key_info );

      destroy( );
      retval = true;
      break;
   }

   return retval;
}

bool class_base::has_skipped_empty_update( )
{
   if( p_impl->has_changed_user_fields )
      return false;
   else
   {
      --revision;
      return true;
   }
}

void class_base::clean_up( )
{
   clone_key.erase( );
   op = e_op_type_none;

   fetch_field_names.clear( );

   validation_errors.clear( );

   p_impl->release_fk_locks( );

   if( lock_handle )
      release_obtained_lock( lock_handle );

   if( xlock_handle )
      release_obtained_lock( xlock_handle );

   lock_handle = xlock_handle = 0;
}

void class_base::finish( )
{
   clean_up( );
}

void class_base::cancel( )
{
   clean_up( );
}

void class_base::fetch( string& sql, bool check_only, bool use_lazy_key )
{
   vector< string > sql_column_names;
   get_sql_column_names( sql_column_names );

   if( !fetch_field_names.empty( ) )
   {
      vector< string > required_sql_columns;

      // NOTE: As a performance optimisation a "minimal" fetch will plug in the current (presumably
      // default) value as a literal for all non-required fields (which should reduce query I/O for
      // tables with a large number of columns). These "dummy" values are required as because it is
      // assumed that the columns being fetched are the same number (and also in the same order) as
      // the fields in the class.
      size_t transient_offset = 0;
      for( size_t i = 0; i < sql_column_names.size( ); i++ )
      {
         while( is_field_transient( i + transient_offset ) )
            ++transient_offset;

         if( fetch_field_names.count( sql_column_names[ i ].substr( 2 ) ) ) // i.e. skip the "C_" prefix
            required_sql_columns.push_back( sql_column_names[ i ] );
         else
            required_sql_columns.push_back( sql_quote( get_field_value( i + transient_offset ) ) );
      }

      sql_column_names.swap( required_sql_columns );
   }

   if( sql_column_names.empty( ) )
      sql.erase( );
   else
   {
      if( check_only )
         sql = "SELECT C_Key_";
      else
      {
         sql = "SELECT C_Key_,C_Ver_,C_Rev_,C_Typ_";
         for( size_t i = 0; i < sql_column_names.size( ); i++ )
         {
            sql += ',';
            sql += sql_column_names[ i ];
         }
      }

      string table_name( "T_" + get_module_name( ) + "_" + get_class_name( ) );

      sql += " FROM " + table_name;
      sql += " WHERE C_Key_ = " + sql_quote( use_lazy_key ? lazy_fetch_key : key );
   }
}

void class_base::destroy( )
{
   string next_child_field;
   class_base* p_class_base;
   size_t num_children = get_num_foreign_key_children( );

   // NOTE: Perform any applicable cascade operations to the child instances.
   if( num_children > 0 )
   {
      for( int pass = 0; pass < 2; ++pass )
      {
         cascade_op next_op;
         if( pass == 0 )
            next_op = e_cascade_op_destroy;
         else
            next_op = e_cascade_op_unlink;

         for( size_t i = 0; i < num_children; i++ )
         {
            p_class_base = get_next_foreign_key_child( i, next_child_field, next_op );

            auto_ptr< class_cascade > ap_tmp_cascading;
            if( p_class_base )
               ap_tmp_cascading.reset( new class_cascade( *p_class_base ) );

            // FUTURE: The handling of cascades needs to be revisited at some stage to improve performance
            // as currently iteration occurs three times (firstly to lock, secondly to test if constrained
            // and thirdly for the actual deletions).
            if( p_class_base
             && p_class_base->iterate_forwards( "", c_key_field, true, 0, e_sql_optimisation_unordered ) )
            {
               p_class_base->set_dynamic_if_class_has_derivations( );
               do
               {
                  if( next_op == e_cascade_op_destroy )
                  {
                     op_destroy_rc rc;
                     p_class_base->op_destroy( &rc );

                     if( rc != e_op_destroy_rc_not_found )
                        p_class_base->op_apply( );
                  }
                  // NOTE: The model relationships may end up with instances being destroyed trying to
                  // unlink themselves from parents which are also being destroyed so these unlink ops
                  // are simply ignored.
                  else if( !is_destroy_locked_by_own_session( *p_class_base ) )
                  {
                     op_update_rc rc;
                     p_class_base->op_update( &rc );

                     if( rc != e_op_update_rc_not_found )
                     {
                        p_class_base->clear_foreign_key( next_child_field );
                        p_class_base->op_apply( );
                     }
                  }
               } while( p_class_base->iterate_next( ) );
            }
         }
      }
   }
}

void class_base::after_fetch_from_db( )
{
   if( !key.empty( ) && key[ 0 ] != ' ' )
   {
      lazy_fetch_key.erase( );

      p_impl->foreign_key_values.clear( );
      get_foreign_key_values( p_impl->foreign_key_values );

      original_values.clear( );

      int num_fields = get_num_fields( );
      for( size_t i = 0; i < num_fields; i++ )
         original_values.push_back( get_field_value( i ) );
   }
}

void class_base::perform_after_fetch( bool is_minimal, bool is_for_prepare )
{
   restorable< bool > tmp_is_fetching( is_fetching, true );

   TRACE_LOG( TRACE_CLASSOPS, "perform_after_fetch( ) [class: " + get_class_name( )
     + "] key = " + key + ", is_minimal = " + to_string( is_minimal ) + ", is_for_prepare = "
     + to_string( is_for_prepare ) + ", is_being_cascaded = " + to_string( is_being_cascaded ) );

   if( !is_minimal && !is_being_cascaded )
   {
      search_replace_separators.clear( );
      p_impl->search_replacements.clear( );
      search_replace_has_opt_prefixing.clear( );

      after_fetch( );
      perform_field_search_replacements( );

      finalise_fetch( );
   }
}

void class_base::perform_to_store( bool is_create, bool is_internal )
{
   search_replace_separators.clear( );
   p_impl->search_replacements.clear( );
   search_replace_has_opt_prefixing.clear( );

   TRACE_LOG( TRACE_CLASSOPS, "perform_to_store( ) [class: " + get_class_name( ) + "]" );

   to_store( is_create, is_internal );
   perform_field_search_replacements( );
}

void class_base::cleanup_dynamic_instance( )
{
   class_base* p_cb( this );

   if( p_owning_instance )
      p_cb = p_owning_instance;

   p_cb->destroy_dynamic_instance( );
}

void class_base::destroy_dynamic_instance( )
{
   if( p_dynamic_instance != this )
   {
      TRACE_LOG( TRACE_CLASSOPS, "destroy dynamic instance for " + p_dynamic_instance->get_current_identity( ) );

      delete p_dynamic_instance;
      p_dynamic_instance = this;
   }
}

void class_base::construct_dynamic_instance( )
{
   if( get_current_identity( ) != get_original_identity( ) )
   {
      if( p_owning_instance )
         throw runtime_error( "unexpected attempt to construct a dynamic instance within a dynamic instance" );

      if( p_dynamic_instance != this )
         destroy_dynamic_instance( );

      TRACE_LOG( TRACE_CLASSOPS, "constructing dynamic instance for " + get_original_identity( ) );

      string module_id, class_id;
      deconstruct_original_identity( *this, module_id, class_id );

      p_dynamic_instance = construct_object( module_id, class_id );
      p_dynamic_instance->p_owning_instance = this;

      p_dynamic_instance->key = key;
      p_dynamic_instance->ver_exp = ver_exp;
      p_dynamic_instance->original_identity = original_identity;

      p_dynamic_instance->p_graph_parent = p_graph_parent;
      p_dynamic_instance->graph_parent_fk_field = graph_parent_fk_field;

      p_dynamic_instance->in_op_begin = in_op_begin;

      p_dynamic_instance->is_singular = is_singular;

      p_dynamic_instance->is_fetching = is_fetching;
      p_dynamic_instance->is_executing = is_executing;
      p_dynamic_instance->is_preparing = is_preparing;

      p_dynamic_instance->is_being_cascaded = is_being_cascaded;

      p_dynamic_instance->iteration_starting = iteration_starting;
      p_dynamic_instance->in_forwards_iteration = in_forwards_iteration;
      p_dynamic_instance->in_backwards_iteration = in_backwards_iteration;

      p_dynamic_instance->p_impl->variables = p_impl->variables;
   }
}

class_base* class_base::fetch_dynamic_instance( string& sql, bool check_only )
{
   construct_dynamic_instance( );

   if( p_dynamic_instance != this )
      p_dynamic_instance->fetch( sql, check_only );

   return p_dynamic_instance;
}

void class_base::set_op( op_type new_op, bool is_new_key )
{
   if( is_new_key )
      set_is_in_iteration( false );

   op = new_op;
}

void class_base::set_is_in_iteration( bool is_in_iter, bool is_forwards )
{
   in_forwards_iteration = ( is_in_iter && is_forwards );
   in_backwards_iteration = ( is_in_iter && !is_forwards );
}

void class_base::add_validation_error( const string& source, const string& error_message )
{
   validation_errors.insert( make_pair( source, error_message ) );
}

void class_base::perform_field_search_replacements( )
{
   for( search_replace_const_iterator
    srci = p_impl->search_replacements.begin( ), end = p_impl->search_replacements.end( ); srci != end; ++srci )
   {
      int field_num = get_field_num( srci->first );

      string str( get_field_value( field_num ) );

      for( size_t i = 0; i < ( srci->second ).size( ); i++ )
      {
         string replacement( ( srci->second )[ i ].second );

         if( !replacement.empty( ) && search_replace_separators.count( srci->first ) )
            replacement += search_replace_separators[ srci->first ];

         str = search_replace( str, ( srci->second )[ i ].first, replacement );
      }

      // NOTE: Any option tokens that were not found in the search/replacements are now removed.
      while( true )
      {
         string::size_type lpos = str.find( "{?" );
         if( lpos != string::npos )
         {
            string::size_type epos = str.find( "}", lpos );
            if( epos == string::npos )
               throw runtime_error( "unexpected missing '}' in '" + str + "'" );

            str.erase( lpos, epos - lpos + 1 );
         }

         string::size_type rpos = str.find( "{#" );
         if( rpos != string::npos )
         {
            string::size_type epos = str.find( "}", rpos );
            if( epos == string::npos )
               throw runtime_error( "unexpected missing '}' in '" + str + "'" );

            str.erase( rpos, epos - rpos + 1 );
         }

         if( lpos == string::npos && rpos == string::npos )
            break;
      }

      if( search_replace_has_opt_prefixing.count( srci->first ) )
      {
         while( true )
         {
            string::size_type pos = str.find( '[' );
            if( pos == string::npos )
               break;

            string::size_type epos = str.find( ']', pos + 1 );
            if( epos == string::npos )
               throw runtime_error( "unexpected missing ']' in '" + str + "'" );

            string replace( str.substr( pos + 1, epos - pos - 1 ) );

            string empty_replace;
            string::size_type npos = replace.find( '|' );
            if( npos != string::npos )
            {
               empty_replace = replace.substr( npos + 1 );
               replace.erase( npos );
            }

            // NOTE: If another optional prefix follows then it is assumed
            // that the search/replace following this optional prefix must
            // have been empty.
            bool was_at_end( epos == str.length( ) - 1 );
            if( !was_at_end && str[ epos + 1 ] == '[' )
               was_at_end = true;

            str.erase( pos, epos - pos + 1 );

            if( !was_at_end )
               str.insert( pos, replace );
            else if( !empty_replace.empty( ) )
               str.insert( pos, empty_replace );
         }
      }

      if( search_replace_separators.count( srci->first )
       && str.length( ) && str[ str.length( ) - 1 ] == search_replace_separators[ srci->first ] )
         str.erase( str.length( ) - 1 );

      set_field_value( field_num, str );
   }
}

void class_base::get_alternative_key_field_info( vector< key_field_info_container >& all_key_field_info ) const
{
   vector< string > unique_indexes;
   get_sql_unique_indexes( unique_indexes );

   for( size_t i = 0; i < unique_indexes.size( ); i++ )
   {
      vector< string > fields;
      split( unique_indexes[ i ], fields );

      key_field_info_container next_key_fields;

      for( size_t j = 0; j < fields.size( ); j++ )
      {
         string field_name( fields[ j ].substr( 2 ) ); // i.e. skip the "C_" prefix
         string field_id( get_field_id( field_name ) );

         next_key_fields.push_back( key_field_info( field_id, field_name ) );
      }

      all_key_field_info.push_back( next_key_fields );
   }
}

int class_base::get_max_index_depth( const vector< string >& field_names ) const
{
   int max_depth = 0;

   vector< string > indexes;
   get_sql_indexes( indexes );

   for( size_t i = 0; i < indexes.size( ); i++ )
   {
      vector< string > columns;
      split( indexes[ i ], columns );

      int depth = 0;
      for( size_t j = 0; j < min( columns.size( ), field_names.size( ) ); j++ )
      {
         if( columns[ j ] == "C_" + field_names[ j ] )
            ++depth;
         else
            break;
      }

      if( depth > max_depth )
         max_depth = depth;

      if( max_depth == field_names.size( ) )
         break;
   }

   return max_depth;
}

void class_base::generate_sql( const string& class_name,
 generate_sql_type type, vector< string >& sql_stmts, set< string >& tx_key_info ) const
{
   switch( type )
   {
      case e_generate_sql_type_insert:
      sql_stmts.push_back( generate_sql_insert( class_name ) );
      tx_key_info.insert( get_class_id( ) + ":" + key );
      break;

      case e_generate_sql_type_update:
      sql_stmts.push_back( generate_sql_update( class_name ) );
      tx_key_info.insert( get_class_id( ) + ":" + key );
      break;

      case e_generate_sql_type_delete:
      sql_stmts.push_back( generate_sql_delete( class_name ) );
      tx_key_info.insert( get_class_id( ) + ":" + key );
      break;

      default:
      throw runtime_error( "unexpected generate_sql_type #" + to_string( type ) );
   }
}

string class_base::generate_sql_insert( const string& class_name ) const
{
   string sql_stmt( "INSERT INTO T_" + get_module_name( ) + "_" + class_name );

   bool done = false;
   vector< string > sql_column_names;
   get_sql_column_names( sql_column_names, &done, &class_name );

   if( sql_column_names.empty( ) )
      sql_stmt.erase( );
   else
   {
      sql_stmt += " (C_Key_,C_Ver_,C_Rev_,C_Typ_";
      for( size_t i = 0; i < sql_column_names.size( ); i++ )
      {
         sql_stmt += ',';
         sql_stmt += sql_column_names[ i ];
      }

      sql_stmt += ") VALUES (";
      sql_stmt += sql_quote( key );

      sql_stmt += ',';
      sql_stmt += to_string( version );

      sql_stmt += ',';
      sql_stmt += to_string( revision );

      sql_stmt += ',';
      sql_stmt += sql_quote( original_identity );

      done = false;
      vector< string > sql_column_values;
      get_sql_column_values( sql_column_values, &done, &class_name );

      for( size_t i = 0; i < sql_column_values.size( ); i++ )
      {
         sql_stmt += ',';
         sql_stmt += sql_column_values[ i ];
      }

      sql_stmt += ");";
   }

   return sql_stmt;
}

string class_base::generate_sql_update( const string& class_name ) const
{
   string sql_stmt( "UPDATE T_" + get_module_name( ) + "_" + class_name );
   sql_stmt += " SET C_Ver_=" + to_string( version ) + ",C_Rev_=" + to_string( revision );

   bool done = false;
   vector< string > sql_column_names;
   get_sql_column_names( sql_column_names, &done, &class_name );

   if( sql_column_names.empty( ) )
      sql_stmt.erase( );
   else
   {
      done = false;

      vector< string > sql_column_values;
      get_sql_column_values( sql_column_values, &done, &class_name );

      // NOTE: Need to use a separate variable for the column lookup
      // as transients are not found in either of the "sql" vectors.
      size_t j = 0;
      done = false;

      int num_fields = get_num_fields( &done, &class_name );
      for( int i = 0; i < num_fields; i++ )
      {
         if( is_field_transient( i ) )
            continue;

         if( has_field_changed( i ) )
         {
            p_impl->has_changed_user_fields = true;

            sql_stmt += ",";
            sql_stmt += sql_column_names[ j ];
            sql_stmt += '=';
            sql_stmt += sql_column_values[ j ];
         }
         ++j;
      }

      sql_stmt += " WHERE C_Key_=" + sql_quote( key ) + ";";
   }

   return sql_stmt;
}

string class_base::generate_sql_delete( const string& class_name ) const
{
   string sql_stmt( "DELETE FROM T_" + get_module_name( ) + "_" + class_name );

   sql_stmt += " WHERE C_Key_=" + sql_quote( key ) + ";";

   return sql_stmt;
}

void class_base::set_class_pointer_base( class_pointer_base* p_cpb )
{
   p_class_pointer_base = p_cpb;
}

void class_base::cleanup( )
{
   // NOTE: If it is a dynamic instance being cleaned up then do nothing here.
   if( !p_owning_instance && op != e_op_type_none && ods::instance( ) )
   {
      if( op != e_op_type_review )
         op_cancel( );
      else
         finish_review( );
   }
}

void class_base::add_search_replacement( const string& field, const string& search, const string& replace )
{
   p_impl->search_replacements[ field ].push_back( make_pair( search, replace ) );
}

void class_base::set_graph_parent( class_base* p_parent, const string& fk_field, bool is_fk )
{
   is_singular = is_fk;

   p_graph_parent = p_parent;
   graph_parent_fk_field = fk_field;

   if( get_graph_depth( ) > c_max_graph_depth )
      throw runtime_error( "maximum graph depth exceeded" );

   set_default_values( );
}

void class_base::trace( const string& s ) const
{
   TRACE_LOG( TRACE_MODS_GEN, s );
}

void class_base::fetch_updated_instance( )
{
   perform_fetch( );

   set_ver_exp( get_version_info( ) );
}

void class_base::add_required_transients( set< string >& required_transients )
{
   for( map< string, string >::const_iterator
    ci = transient_filter_field_values.begin( ); ci != transient_filter_field_values.end( ); ++ci )
      required_transients.insert( ci->first );
}

void class_base::set_key( const string& new_key, bool skip_fk_handling )
{
   // NOTE: Although potentially allowing the creation of an invalid record it is
   // being permitted for an object to bypass the normal FK locking and the check
   // for record existence especially for the situation where a child record that
   // is created in the "after_store" function needs to be linked to the creating
   // parent's record. The variable is immediately cleared to ensure that it will
   // not accidently be left set (so needs to be set before each such usage).
   if( !get_variable( c_object_variable_skip_fk_handling ).empty( ) )
   {
      skip_fk_handling = true;
      set_variable( c_object_variable_skip_fk_handling, "" );
   }

   if( new_key.size( ) > c_max_key_length )
      throw runtime_error( new_key + " exceeds max key length of " + to_string( c_max_key_length ) );

   if( p_graph_parent && is_singular && !is_fetching && !skip_fk_handling && !graph_parent_fk_field.empty( ) )
   {
      if( !p_graph_parent->in_op_begin
       && ( p_graph_parent->op == e_op_type_create || p_graph_parent->op == e_op_type_update ) )
      {
         size_t fk_lock_handle( 0 );
         bool found_locked_instance = false;

         // NOTE: A link lock is held in the graph parent for every foreign key value that was set during
         // the create/update operation. A new lock will be obtained before replacing an existing one and
         // setting the key to an empty string will result in the foreign key's link lock (if held) being
         // released. Whether attempting to link to a new foreign key or unlinking from the current value
         // the derived class might throw an exception via the "set_foreign_key_value" function.
         if( !new_key.empty( ) )
         {
            // NOTE: If a record that is being created by a session is used (prior to the actual DB store
            // operation) as a FK then rather than attempt to lock and read the record (which would fail)
            // just copy the values from the record being created.
            if( is_create_locked_by_own_session( *this, new_key.c_str( ), true ) )
            {
               lazy_fetch_key.erase( );
               found_locked_instance = true;

               p_graph_parent->set_foreign_key_value( graph_parent_fk_field, new_key );

               if( p_impl->foreign_key_locks.find( graph_parent_fk_field ) != p_impl->foreign_key_locks.end( ) )
                  release_obtained_lock( p_impl->foreign_key_locks[ graph_parent_fk_field ] );
            }
            else
            {
               fk_lock_handle = obtain_instance_fk_lock( get_lock_class_id( ), new_key, false );

               if( !fk_lock_handle )
                  throw runtime_error( "unable to obtain lock for '" + new_key + "'" );

               lazy_fetch_key = new_key;
            }
         }
         else
         {
            lazy_fetch_key.erase( );
            clear( );
         }

         if( !found_locked_instance )
         {
            try
            {
               // NOTE: The foreign key's existence is now checked, however, as a performance optimsation
               // this can be omitted during a restore (as invalid foreign keys should not be in the log).
               if( !lazy_fetch_key.empty( ) && !session_skip_fk_fetches( ) )
                  instance_check( *this );

               p_graph_parent->set_foreign_key_value( graph_parent_fk_field, lazy_fetch_key );
            }
            catch( ... )
            {
               if( fk_lock_handle )
                  release_obtained_lock( fk_lock_handle );
               throw;
            }

            foreign_key_lock_container& foreign_key_locks( p_graph_parent->p_impl->foreign_key_locks );

            if( foreign_key_locks.find( graph_parent_fk_field ) != foreign_key_locks.end( ) )
               release_obtained_lock( foreign_key_locks[ graph_parent_fk_field ] );

            if( fk_lock_handle )
               foreign_key_locks[ graph_parent_fk_field ] = fk_lock_handle;
            else
               foreign_key_locks.erase( graph_parent_fk_field );
         }
      }
      else
      {
         lazy_fetch_key.erase( );
         clear( );

         // NOTE: If the key value of a foreign key is being set in this manner then the
         // instance (if present) will be fetched when any of its field values are read.
         lazy_fetch_key = new_key;

         p_graph_parent->set_foreign_key_value( graph_parent_fk_field, lazy_fetch_key );
      }
   }

   key = new_key;
}

#ifdef __BORLANDC__
string construct_class_identity( const class_base& cb )
{
   string identity( cb.get_module_id( ) );
   identity += ':';
   identity += cb.get_class_id( );
   return identity;
}
#endif

struct procedure_progress::impl
{
   impl( )
   {
      ts = time_t( 0 );
   }

   time_t ts;
   size_t seconds;
};

procedure_progress::procedure_progress( size_t seconds )
{
   p_impl = new impl;

   p_impl->ts = time( 0 );
   p_impl->seconds = seconds;
}

procedure_progress::~procedure_progress( )
{
   delete p_impl;
}

void procedure_progress::check_progress( size_t amount )
{
   if( time( 0 ) - p_impl->ts >= p_impl->seconds )
   {
      p_impl->ts = time( 0 );
      // FUTURE: This message should be handled as a server string message.
      output_progress_message( "Processed " + to_string( amount ) + " records..." );
   }
}

class_base_filter::class_base_filter( class_base& cb, const string& filter_ids )
 :
 cba( cb )
{
   split( filter_ids, filters );
   swap( cba.filters( ), filters );
}

class_base_filter::~class_base_filter( )
{
   swap( filters, cba.filters( ) );
}

bool is_valid_int( const string& s )
{
   bool rc = true;
   if( s.empty( ) )
      rc = false;
   else
   {
      if( s.length( ) == 1 && s[ 0 ] == '-' )
         rc = false;

      size_t spos = 0;
      if( s[ 0 ] == '-' )
         ++spos;

      if( s.length( ) - spos > c_int_digits10 )
         rc = false;
      else
      {
         for( size_t i = spos; i < s.length( ); i++ )
         {
            if( s[ i ] < '0' || s[ i ] > '9' )
            {
               rc = false;
               break;
            }
         }
      }
   }

   return rc;
}

bool is_valid_bool( const string& s )
{
   return s == "0" || s == "1";
}

bool is_valid_date( const string& s )
{
   bool rc = true;
   try
   {
      udate ud( s );
   }
   catch( exception& x )
   {
      rc = false;
   }

   return rc;
}

bool is_valid_time( const string& s )
{
   bool rc = true;
   try
   {
      mtime mt( s );
   }
   catch( exception& x )
   {
      rc = false;
   }

   return rc;
}

bool is_valid_numeric( const string& s )
{
   bool rc = true;
   try
   {
      numeric n( s.c_str( ) );
   }
   catch( exception& x )
   {
      rc = false;
   }

   return rc;
}

bool is_valid_date_time( const string& s )
{
   bool rc = true;
   try
   {
      date_time dtm( s );
   }
   catch( exception& x )
   {
      rc = false;
   }

   return rc;
}

bool is_valid_value( const string& s, primitive p,
 unsigned int max_size, const char* p_min_value, const char* p_max_value )
{
   int rc = true;

   bool has_min_and_max = p_min_value && p_max_value
    && string( p_min_value ).length( ) > 0 && string( p_max_value ).length( ) > 0;

   switch( p )
   {
      case e_primitive_string:
      if( max_size && s.length( ) > max_size )
         rc = false;
      break;

      case e_primitive_datetime:
      rc = is_valid_date_time( s );
      if( rc )
      {
         if( has_min_and_max )
         {
            date_time dtm( s );

            date_time min( p_min_value );
            date_time max( p_max_value );

            if( dtm < min || dtm > max )
               rc = false;
         }
      }
      break;

      case e_primitive_date:
      rc = is_valid_date( s );
      if( rc )
      {
         if( has_min_and_max )
         {
            udate ud( s );

            udate min( p_min_value );
            udate max( p_max_value );

            if( ud < min || ud > max )
               rc = false;
         }
      }
      break;

      case e_primitive_time:
      rc = is_valid_time( s );
      if( rc )
      {
         if( has_min_and_max )
         {
            mtime mt( s );

            mtime min( p_min_value );
            mtime max( p_max_value );

            if( mt < min || mt > max )
               rc = false;
         }
      }
      break;

      case e_primitive_numeric:
      rc = is_valid_numeric( s );
      if( rc )
      {
         if( has_min_and_max )
         {
            numeric n( s.c_str( ) );

            numeric min( p_min_value );
            numeric max( p_max_value );

            if( n < min || n > max )
               rc = false;
         }
      }
      break;

      case e_primitive_int:
      rc = is_valid_int( s );
      if( rc )
      {
         if( has_min_and_max )
         {
            int i = atoi( s.c_str( ) );

            int min = atoi( p_min_value );
            int max = atoi( p_max_value );

            if( i < min || i > max )
               rc = false;
         }
      }
      break;

      case e_primitive_bool:
      rc = is_valid_bool( s );
      break;

      default:
      throw runtime_error( "unexpected primitive value #" + to_string( p ) + " found in is_valid_value" );
   }

   return rc;
}

string sql_quote( const string& s )
{
   // NOTE: Always use '' rather than NULL here as it is expected
   // that NOT NULL is used for all column definitions in the DDL.
   if( s.empty( ) )
      return "''";
   else
      return quote( s, '\'', '\0' );
}

void from_string( class_base& cb, const string& s )
{
   class_base_accessor( cb ).set_key( s );
}

string int_to_comparable_string( int i, bool prefix_with_sign, int max_digits )
{
   string retval;

   if( prefix_with_sign )
   {
      if( i < 0 )
         retval += '-';
      else
         retval += '+';
   }

   string digits( to_string( i ) );

   if( digits[ 0 ] == '-' )
      digits.erase( 0, 1 );

   if( max_digits == 0 )
      max_digits = c_int_digits10 + 1;

   int extra = max_digits - digits.size( );

   if( extra > 0 )
      retval += string( extra, '0' );
   retval += digits;

   return retval;
}

string construct_key_from_int( const string& prefix, int num )
{
   string retval( prefix );
   retval += int_to_comparable_string( num, false, 5 );
   return retval;
}

void wait( unsigned long ms )
{
   msleep( ms );
}

string get_uuid( )
{
   return uuid( ).as_string( );
}

string get_ext( const string& filename )
{
   string str( filename );

   string::size_type pos;
   pos = str.find_last_of( "/\\" );

   if( pos != string::npos )
      str.erase( 0, pos + 1 );

   pos = str.find( '.' );
   if( pos == string::npos )
      str.erase( );
   else
      str.erase( 0, pos );

   return str;
}

string get_path( const string& filename )
{
   string str( filename );

   string::size_type pos;
   pos = str.find_last_of( "/\\" );

   if( pos == string::npos )
      str.erase( );
   else
      str.erase( pos + 1 );

   return str;
}

bool exists_file( const string& filename, bool check_link_target )
{
   return file_exists( filename, check_link_target );
}

void remove_file( const string& filename )
{
   file_remove( filename );

   if( file_exists( filename ) )
      throw runtime_error( "unable to remove file '" + filename + "'" );
}

void rename_file( const string& oldname, const string& newname )
{
   if( rename( oldname.c_str( ), newname.c_str( ) ) != 0 )
      throw runtime_error( "unable to rename file '" + oldname + "' to '" + newname + "'" );
}

int64_t size_file( const string& filename )
{
   int64_t size = 0;

   if( file_exists( filename ) )
      size = file_size( filename );

   return size;
}

int64_t last_mod_time( const string& filename )
{
   return last_modification_time( filename );
}

void copy_file( const string& source, const string& destination )
{
   ifstream inpf( source.c_str( ), ios::in | ios::binary );
   if( !inpf )
      throw runtime_error( "unable to open file '" + source + "' for input" );

   ofstream outf( destination.c_str( ), ios::out | ios::binary );
   if( !outf )
      throw runtime_error( "unable to open file '" + destination + "' for output" );

   copy_stream( inpf, outf );
}

string load_file( const string& filename, bool is_optional )
{
   if( is_optional && !file_exists( filename ) )
      return string( );

   return buffer_file( filename );
}

void read_file_lines( const string& filename, set< string >& lines )
{
   buffer_file_lines( filename, lines );
}

void read_file_lines( const string& filename, vector< string >& lines )
{
   buffer_file_lines( filename, lines );
}

void link_file( const string& source, const string& name )
{
   file_link( source, name );
}

string copy_class_file( const string& src_path,
 const string& dest_class_id, const string& dest_file_name, bool return_name_only, bool return_full_path )
{
   string dest_path( src_path );

   string::size_type pos = dest_path.rfind( '-' );

   if( pos == string::npos )
      pos = dest_path.rfind( '.' );

   if( pos == string::npos )
      throw runtime_error( "unexpected dest path format '" + dest_path + "'" );

   string ext( dest_path.substr( pos ) );

   pos = dest_path.rfind( '/' );
   if( pos == string::npos )
      throw runtime_error( "unexpected dest path format '" + dest_path + "'" );

   dest_path.erase( pos );

   pos = dest_path.rfind( '/' );
   if( pos == string::npos )
      throw runtime_error( "unexpected dest path format '" + dest_path + "'" );

   dest_path.erase( pos + 1 );

   dest_path += dest_class_id + "/" + dest_file_name + ext;

   if( !return_name_only )
      copy_file( src_path, dest_path );

   if( !return_full_path )
      dest_path = dest_file_name + ext;

   return dest_path;
}

void copy_class_files( const class_base& src, class_base& dest )
{
   if( src.get_class_id( ) != dest.get_class_id( ) )
      throw runtime_error( "cannot copy class files from a '"
       + src.get_class_name( ) + "' to a '" + dest.get_class_name( ) + "'" );

   vector< string > file_field_names;
   src.get_file_field_names( file_field_names );

   for( size_t i = 0; i < file_field_names.size( ); i++ )
   {
      string next_file( src.get_field_value( src.get_field_num( file_field_names[ i ] ) ) );

      if( !next_file.empty( ) )
         dest.set_field_value( dest.get_field_num( file_field_names[ i ] ),
          copy_class_file( src.get_attached_file_path( next_file ), dest.get_class_id( ), dest.get_key( ), storage_locked_for_admin( ) ) );
   }
}

void copy_class_files_for_clone(
 const vector< pair< string, string > >& file_field_name_and_values, class_base& dest )
{
   for( size_t i = 0; i < file_field_name_and_values.size( ); i++ )
   {
      string next_file( file_field_name_and_values[ i ].second );

      if( !next_file.empty( ) )
         dest.set_field_value( dest.get_field_num( file_field_name_and_values[ i ].first ),
          copy_class_file( dest.get_attached_file_path( next_file ), dest.get_class_id( ), dest.get_key( ), storage_locked_for_admin( ) ) );
   }
}

string get_attached_file_dir( )
{
   return string( c_files_directory );
}

string get_attached_file_path( const string& module_id, const string& class_id )
{
   string path( storage_web_root( true ) );
   path += "/" + string( c_files_directory ) + "/" + module_id + "/" + class_id;

   return path;
}

string expand_lf_to_cr_lf( const string& input )
{
   string output;
   for( size_t i = 0; i < input.size( ); i++ )
   {
      if( input[ i ] == '\n' )
         output += '\r';
      output += input[ i ];
   }

   return output;
}

void delete_directory_tree( const string& path )
{
   delete_directory_files( path, true );
}

void create_directories_for_file_name( const string& file_name )
{
   string::size_type pos = file_name.find_last_of( "/\\" );

   // NOTE: It is not recommended to use this function with relative paths.
   if( pos != string::npos )
   {
      vector< string > sub_directories;
      string directories( file_name.substr( 0, pos ) );

#ifdef _WIN32
      string::size_type dpos = directories.find( ':' );
      if( dpos != string::npos || directories[ 0 ] == '/' || directories[ 0 ] == '\\' )
      {
         pos = directories.find_first_of( "/\\" );
         sub_directories.push_back( directories.substr( 0, pos + 1 ) );
         directories.erase( 0, pos + 1 );
      }
#else
      if( directories[ 0 ] == '/' )
      {
         directories.erase( 0, 1 );
         sub_directories.push_back( "/" );
      }
#endif

      while( true )
      {
         pos = directories.find_first_of( "/\\" );
         if( pos == string::npos )
            break;

         sub_directories.push_back( directories.substr( 0, pos ) );
         directories.erase( 0, pos + 1 );
      }

      if( !directories.empty( ) )
         sub_directories.push_back( directories );

      char buf[ _MAX_PATH ];
      _getcwd( buf, _MAX_PATH );

      for( size_t i = 0; i < sub_directories.size( ); i++ )
      {
         if( _chdir( sub_directories[ i ].c_str( ) ) != 0 )
         {
#ifdef _WIN32
            if( _mkdir( sub_directories[ i ].c_str( ) ) != 0 )
#else
            if( _mkdir( sub_directories[ i ].c_str( ), S_IRWXU | S_IRWXG | S_IRWXO ) != 0 )
#endif
               throw runtime_error( "unable to create '" + sub_directories[ i ] + "' directory" );

            if( _chdir( sub_directories[ i ].c_str( ) ) != 0 )
               throw runtime_error( "unable to _chdir to '" + sub_directories[ i ] + "'" );
         }
      }

      // NOTE: Restore the original working directory.
      if( _chdir( buf ) != 0 )
         throw runtime_error( "unable to _chdir to '" + string( buf ) + "'" );
   }
}

string get_directory_for_file_name( const string& file_name )
{
   string directory;
   string::size_type pos = file_name.find_last_of( "/\\" );

   if( pos != string::npos )
      directory = file_name.substr( 0, pos );

   return directory;
}

void remove_gpg_key( const string& gpg_key_id, bool ignore_error )
{
   guard g( g_mutex );

   string tmp( "~" + uuid( ).as_string( ) );
   string cmd( "gpg --yes --batch --delete-key " + gpg_key_id + ">" + tmp + " 2>&1" );

   TRACE_LOG( TRACE_SESSIONS, cmd );

   system( cmd.c_str( ) );

   vector< string > lines;
   buffer_file_lines( tmp, lines );

   string response( buffer_file( tmp ) );
   file_remove( tmp );

   if( !ignore_error && !lines.empty( ) )
      throw runtime_error( lines[ 0 ] );
}

void locate_gpg_key( const string& email, string& gpg_key_id, string& gpg_fingerprint )
{
   guard g( g_mutex );

   string tmp( "~" + uuid( ).as_string( ) );

   string cmd( "gpg --fingerprint " + email + ">" + tmp +  " 2>&1" );

   TRACE_LOG( TRACE_SESSIONS, cmd );

   system( cmd.c_str( ) );

   vector< string > lines;
   buffer_file_lines( tmp, lines );

   file_remove( tmp );

   if( lines.size( ) >= 3 )
   {
      string::size_type pos = lines[ 0 ].find( '/' );
      if( pos != string::npos )
      {
         string::size_type epos = lines[ 0 ].find( ' ', pos + 1 );
         if( epos != string::npos )
         {
            string key = lines[ 0 ].substr( pos + 1, epos - pos );

            pos = lines[ 1 ].find( c_gpg_key_fingerprint_prefix );
            if( pos != string::npos )
            {
               gpg_key_id = key;
               gpg_fingerprint = lines[ 1 ].substr( pos + strlen( c_gpg_key_fingerprint_prefix ) );
            }
         }
      }
   }
}

void install_gpg_key( const string& key_file,
 const string& email, string& gpg_key_id, string& gpg_fingerprint, string* p_new_email )
{
   guard g( g_mutex );

   for( size_t i = 0; i < 2; i++ )
   {
      string tmp( "~" + uuid( ).as_string( ) );
      string cmd( "gpg --batch --import " );

      if( i == 0 )
         cmd += "--dry-run ";

      cmd += "\"" + key_file + "\">" + tmp + " 2>&1";

      TRACE_LOG( TRACE_SESSIONS, cmd );

      system( cmd.c_str( ) );

      vector< string > lines;
      buffer_file_lines( tmp, lines );

      file_remove( tmp );

      bool had_unexpected_error = false;

      if( lines.empty( ) )
         had_unexpected_error = true;
      else if( lines.size( ) < 3 )
         throw runtime_error( lines[ 0 ] );
      else if( lines[ 0 ].find( "CRC error" ) != string::npos )
      {
         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( "Invalid or corrupt GPG key file." );
      }
      else
      {
         string result;
         string first_line( lines[ 0 ] );

         regex expr( "\".*\"" );

         string::size_type len;
         string::size_type pos = expr.search( first_line, &len );

         if( pos == string::npos )
            had_unexpected_error = true;
         else
         {
            result = first_line.substr( pos + len + 1 );

            if( result == "imported" || result == "not changed" )
            {
               pos = first_line.find( " key " );
               if( pos == string::npos )
                  had_unexpected_error = true;
               else
               {
                  string::size_type epos = first_line.find( ':' );
                  if( epos == string::npos )
                     had_unexpected_error = true;
                  else
                  {
                     string key( first_line.substr( pos + 5, 8 ) );

                     pos = first_line.find_last_of( '<' );

                     if( pos != string::npos )
                     {
                        regex expr( "<" + string( c_regex_email_address ) + ">" );

                        if( expr.search( first_line.substr( pos ), &len ) != 0 )
                           pos = string::npos;

                        if( pos == string::npos )
                           // FUTURE: This message should be handled as a server string message.
                           throw runtime_error( "GPG key is missing email address." );
                        else
                        {
                           string email_addr( first_line.substr( pos + 1, len - 2 ) );

                           if( !email.empty( ) && email_addr != email )
                           {
                              if( i == 1 )
                                 remove_gpg_key( key, true );

                              // FUTURE: This message should be handled as a server string message.
                              throw runtime_error( "GPG key has incorrect email address (found '"
                               + email_addr + "' but expecting '" + email + "')." );
                           }

                           if( i == 1 )
                           {
                              if( p_new_email )
                                 *p_new_email = email_addr;

                              cmd = "gpg --fingerprint ";
                              cmd += key + ">" + tmp +  " 2>&1";

                              TRACE_LOG( TRACE_SESSIONS, cmd );

                              system( cmd.c_str( ) );

                              lines.clear( );
                              buffer_file_lines( tmp, lines );

                              file_remove( tmp );

                              if( lines.empty( ) )
                                 had_unexpected_error = true;
                              else if( lines.size( ) < 3 )
                                 throw runtime_error( lines[ 0 ] );

                              pos = lines[ 1 ].find( c_gpg_key_fingerprint_prefix );
                              if( pos == string::npos )
                                 had_unexpected_error = true;
                              else
                              {
                                 gpg_key_id = key;
                                 gpg_fingerprint = lines[ 1 ].substr( pos + strlen( c_gpg_key_fingerprint_prefix ) );
                              }
                           }
                        }
                     }
                  }
               }
            }
            else
               had_unexpected_error = true;
         }
      }

      if( had_unexpected_error )
         throw runtime_error( "unexpected error occurred installing GPG key" );
   }
}

string trim_whitespace( const string& s )
{
   return trim( s );
}

string trim_whitespace_and_quotes( const string& s )
{
   string str( trim( s ) );

   if( str.length( ) >= 2
    && str[ 0 ] == '"' && str[ str.length( ) - 1 ] == '"' )
   {
      str.erase( 0, 1 );
      str.erase( str.length( ) - 1 );
   }

   return str;
}

string truncate_string( const string& s, int max_length, const char* p_overflow_suffix )
{
   string tmp( s );
   return utf8_truncate( tmp, max_length, p_overflow_suffix );
}

size_t split_count( const string& s, char sep )
{
   return split_size( s, sep );
}

void split_string( const string& s, set< string >& c, char sep )
{
   split( escape_sep_if_quoted( s, sep ), c, sep );
}

void split_string( const string& s, deque< string >& c, char sep )
{
   split( escape_sep_if_quoted( s, sep ), c, sep );
}

void split_string( const string& s, vector< string >& c, char sep )
{
   split( escape_sep_if_quoted( s, sep ), c, sep );
}

void split_string( const string& s, set< string >& c, const string& sep )
{
   if( sep.length( ) != 1 )
      split( s, c, sep );
   else
      split_string( s, c, sep[ 0 ] );
}

void split_string( const string& s, deque< string >& c, const string& sep )
{
   if( sep.length( ) != 1 )
      split( s, c, sep );
   else
      split_string( s, c, sep[ 0 ] );
}

void split_string( const string& s, vector< string >& c, const string& sep )
{
   if( sep.length( ) != 1 )
      split( s, c, sep );
   else
      split_string( s, c, sep[ 0 ] );
}

string search_replace( const string& s, const string& search, const string& replace )
{
   return replaced( s, search, replace );
}

string search_replace( const string& s,
 const string& search1, const string& replace1, const string& search2, const string& replace2 )
{
   return search_replace( search_replace( s, search1, replace1 ), search2, replace2 );
}

string escaped_string( const string& s, const char* p_chars, char esc )
{
   return escaped( s, p_chars, esc );
}

string quoted_literal( const string& s, char esc, bool add_quotes )
{
   string qs;

   size_t start = 0;
   size_t finish = s.length( );

   // NOTE: If the original string is found to be surrounded by quotes then change both the
   // start and finish as it is assumed that such surrounding quotes are not to be escaped.
   if( s.size( ) >= 2 && s[ 0 ] == '"' && s[ s.length( ) - 1 ] == '"' )
   {
      ++start;
      --finish;
   }

   if( add_quotes )
      qs += '"';

   // NOTE: If 'null' is provided (and requires quoting) then returns the empty quoted string.
   if( !add_quotes || start > 0 || s != "null" )
   {
      for( size_t i = start; i < finish; i++ )
      {
         if( s[ i ] == '"' )
            qs += esc;
         qs += s[ i ];
      }
   }

   if( add_quotes )
      qs += '"';

   return qs;
}

string replace_leading_cols_with_ws( const string& s, const string& sep, size_t num_spaces )
{
   vector< string > vs;
   split_string( s, vs, sep );

   string retval, rep( num_spaces, ' ' );
   for( size_t i = 0; i < vs.size( ); i++ )
   {
      if( i == vs.size( ) - 1 )
         retval += vs[ i ];
      else
         retval += rep;
   }

   return retval;
}

void check_with_regex( const string& r, const string& s, bool* p_rc )
{
   string re( r );

   if( re == c_special_regex_for_email_address )
      re = "^" + string( c_regex_email_address ) + "$";

   regex expr( re );

   if( p_rc )
      *p_rc = true;

   if( expr.search( s ) == string::npos )
   {
      if( !s.empty( ) )
      {
         if( p_rc )
            *p_rc = false;
         else
            throw runtime_error( s );
      }
      else
         throw runtime_error( "unexpected check for empty string" );
   }
}

string hash_sha1( const string& s )
{
   sha1 hash( s );
   return lower( hash.get_digest_as_string( ) );
}

string hash_sha256( const string& s )
{
   sha256 hash( s );
   return lower( hash.get_digest_as_string( ) );
}

string decrypt( const string& s )
{
   // NOTE: Password fields that are < 20 characters are assumed to not have been encrypted.
   if( s.length( ) < 20 )
      return s;
   else
      return decrypt_password( s, false, false, true );
}

string encrypt( const string& s )
{
   return encrypt_password( s, false, false, true );
}

string totp_pin( const string& secret )
{
   return get_totp( secret );
}

string totp_secret( const string& unique )
{
   return totp_secret_key( unique );
}

string valid_utf8_filename( const string& str )
{
   bool has_utf8 = false;
   return valid_file_name( str, &has_utf8 );
}

string valid_non_utf8_filename( const string& str )
{
   return search_replace( valid_file_name( str ), " ", "_" );
}

string formatted_int( int n, const string& mask )
{
   return format_numeric( n, mask );
}

string formatted_numeric( const numeric& n, const string& mask )
{
   return format_numeric( n, mask );
}

string numeric_name( const string& s, bool show_plus_if_no_sign )
{
   string retval;
   string no_sign_prefix;

   if( show_plus_if_no_sign )
      no_sign_prefix = "plus_";

   for( size_t i = 0; i < s.length( ); i++ )
   {
      if( i == 0 )
      {
         if( s[ i ] == '-' )
            retval += "minus_";
         else if( s[ i ] == '+' )
            retval += "plus_";
         else
            retval += no_sign_prefix + s[ i ];
      }
      else if( s[ i ] == ' ' || s[ i ] == '.' )
         retval += "_";
      else
         retval += s[ i ];
   }

   return retval;
}

string value_label( const string& s )
{
   string rs;
   for( size_t i = 0; i < s.size( ); i++ )
   {
      if( s[ i ] == ' ' || s[ i ] == '.' )
         rs += '_';
      else if( s[ i ] == '>' )
      {
         if( i < s.size( ) - 1 && s[ i + 1 ] == '=' )
         {
            rs += "_gteq_";
            ++i;
         }
         else
            rs += "_gt_";
      }
      else if( s[ i ] == '<' )
      {
         if( i < s.size( ) - 1 && s[ i + 1 ] == '=' )
         {
            rs += "_lteq_";
            ++i;
         }
         else
            rs += "_lt_";
      }
      else if( s[ i ] == '!' )
      {
         if( i < s.size( ) - 1 && s[ i + 1 ] == '=' )
         {
            rs += "_not_eq_";
            ++i;
         }
         else
            rs += "_not_";
      }
      else if( s[ i ] == '=' )
         rs += "_eq_";
      else if( s[ i ] == '_' || ( s[ i ] >= 'A' && s[ i ] <= 'Z' )
       || ( s[ i ] >= 'a' && s[ i ] <= 'z' ) || s[ i ] >= '0' && s[ i ] <= '9' )
         rs += s[ i ];
   }

   return rs;
}

string value_leftpart( const string& s )
{
   string::size_type pos = s.find( ' ' );
   return s.substr( 0, pos );
}

string value_rightpart( const string& s )
{
   string::size_type pos = s.find( ' ' );

   if( pos == string::npos )
      return "";
   else
      return s.substr( pos + 1 );
}

string auto_int_increment( const string& current )
{
   string new_auto_int;

   // NOTE: If the current value is empty then no auto-increment will occur. If the current value
   // is equal to the maximum possible then "new_auto_int" will be left as an empty string so that
   // all possible values can be used (assuming the target field is mandatory and the auto-increment
   // is optional).
   if( !current.empty( ) )
   {
      int val = 0;
      size_t i = 0;
      int num_digits = 0;
      bool had_digits = false;
      for( i = current.size( ) - 1; i >= 0; i-- )
      {
         if( current[ i ] >= '0' && current[ i ] <= '9' )
         {
            ++num_digits;

            if( i == 0 )
               break;
         }
         else
         {
            ++i;
            break;
         }
      }

      val = atoi( current.substr( i ).c_str( ) );

      string max( num_digits, '9' );
      int max_val = atoi( max.c_str( ) );

      if( val < max_val )
      {
         ++val;

         ostringstream osstr;
         osstr << setw( num_digits ) << setfill( '0' ) << val;

         new_auto_int += osstr.str( );

         if( i > 0 )
            new_auto_int = current.substr( 0, i ) + new_auto_int;
      }
   }

   return new_auto_int;
}

void generate_timezones_sio( const vector< timezone_info >& timezones )
{
   ofstream outf( "timezones.sio" );
   if( !outf )
      throw runtime_error( "unable to open file 'timezones.sio' for output" );

   sio_writer writer( outf );
   writer.start_section( c_section_timezones );

   for( size_t i = 0; i < timezones.size( ); i++ )
   {
      writer.start_section( c_section_timezone );

      writer.write_attribute( c_attribute_abbr, timezones[ i ].abbr );
      writer.write_opt_attribute( c_attribute_name, timezones[ i ].name, timezones[ i ].abbr );
      writer.write_attribute( c_attribute_tz_info, timezones[ i ].tz_info );
      writer.write_attribute( c_attribute_utc_offset, to_string( timezones[ i ].utc_offset ) );
      writer.write_attribute( c_attribute_description, timezones[ i ].description );

      writer.write_opt_attribute( c_attribute_daylight_abbr, timezones[ i ].daylight_abbr );

      if( timezones[ i ].daylight_year_start )
      {
         writer.write_attribute( c_attribute_daylight_year_start, to_string( timezones[ i ].daylight_year_start ) );
         writer.write_attribute( c_attribute_daylight_year_finish, to_string( timezones[ i ].daylight_year_finish ) );

         writer.write_attribute( c_attribute_daylight_start_month, to_string( timezones[ i ].daylight_start_month ) );
         writer.write_attribute( c_attribute_daylight_start_occurrence, to_string( timezones[ i ].daylight_start_occurrence ) );
         writer.write_attribute( c_attribute_daylight_start_day_of_week, to_string( timezones[ i ].daylight_start_day_of_week ) );
         writer.write_attribute( c_attribute_daylight_start_time, timezones[ i ].daylight_start_time );

         writer.write_attribute( c_attribute_daylight_finish_month, to_string( timezones[ i ].daylight_finish_month ) );
         writer.write_attribute( c_attribute_daylight_finish_occurrence, to_string( timezones[ i ].daylight_finish_occurrence ) );
         writer.write_attribute( c_attribute_daylight_finish_day_of_week, to_string( timezones[ i ].daylight_finish_day_of_week ) );
         writer.write_attribute( c_attribute_daylight_finish_time, timezones[ i ].daylight_finish_time );

         writer.write_attribute( c_attribute_daylight_utc_offset, to_string( timezones[ i ].daylight_utc_offset ) );
      }

      if( timezones[ i ].daylight_changes.size( ) )
      {
         writer.start_section( c_section_daylight_saving_changes );

         for( int j = 0; j < timezones[ i ].daylight_changes.size( ); j++ )
         {
            writer.start_section( c_section_daylight_saving_change );

            writer.write_attribute( c_attribute_year_start, to_string( timezones[ i ].daylight_changes[ j ].year_start ) );
            writer.write_attribute( c_attribute_year_finish, to_string( timezones[ i ].daylight_changes[ j ].year_finish ) );
            writer.write_attribute( c_attribute_description, timezones[ i ].daylight_changes[ j ].description );

            writer.write_attribute( c_attribute_start_month, to_string( timezones[ i ].daylight_changes[ j ].start_month ) );
            writer.write_attribute( c_attribute_start_occurrence, to_string( timezones[ i ].daylight_changes[ j ].start_occurrence ) );
            writer.write_attribute( c_attribute_start_day_of_week, to_string( timezones[ i ].daylight_changes[ j ].start_day_of_week ) );
            writer.write_attribute( c_attribute_start_time, timezones[ i ].daylight_changes[ j ].start_time );

            writer.write_attribute( c_attribute_finish_month, to_string( timezones[ i ].daylight_changes[ j ].finish_month ) );
            writer.write_attribute( c_attribute_finish_occurrence, to_string( timezones[ i ].daylight_changes[ j ].finish_occurrence ) );
            writer.write_attribute( c_attribute_finish_day_of_week, to_string( timezones[ i ].daylight_changes[ j ].finish_day_of_week ) );
            writer.write_attribute( c_attribute_finish_time, timezones[ i ].daylight_changes[ j ].finish_time );

            writer.write_attribute( c_attribute_utc_offset, to_string( timezones[ i ].daylight_changes[ j ].utc_offset ) );

            writer.finish_section( c_section_daylight_saving_change );
         }

         writer.finish_section( c_section_daylight_saving_changes );
      }

      writer.finish_section( c_section_timezone );
   }

   writer.finish_section( c_section_timezones );
   writer.finish_sections( );
}

void setup_timezones( )
{
   ifstream inpf( "timezones.sio" );
   if( !inpf )
      throw runtime_error( "unable to open file 'timezones.sio' for input" );

   sio_reader reader( inpf );
   reader.start_section( c_section_timezones );

   g_timezones.clear( );

   while( reader.has_started_section( c_section_timezone ) )
   {
      timezone_data tz_data;

      string abbr = reader.read_attribute( c_attribute_abbr );
      string name = reader.read_opt_attribute( c_attribute_name );
      tz_data.tz_info = reader.read_opt_attribute( c_attribute_tz_info );

      tz_data.utc_offset = ( int )( atof( reader.read_attribute( c_attribute_utc_offset ).c_str( ) ) * 3600.0 );

      tz_data.description = reader.read_attribute( c_attribute_description );
      tz_data.daylight_abbr = reader.read_opt_attribute( c_attribute_daylight_abbr );

      string daylight_year_start_value = reader.read_opt_attribute( c_attribute_daylight_year_start );

      if( !daylight_year_start_value.empty( ) )
      {
         int daylight_year_start = atoi( daylight_year_start_value.c_str( ) );
         int daylight_year_finish = atoi( reader.read_attribute( c_attribute_daylight_year_finish ).c_str( ) );

         int daylight_start_month = atoi( reader.read_attribute( c_attribute_daylight_start_month ).c_str( ) );
         int daylight_start_occurrence = atoi( reader.read_attribute( c_attribute_daylight_start_occurrence ).c_str( ) );
         int daylight_start_day_of_week = atoi( reader.read_attribute( c_attribute_daylight_start_day_of_week ).c_str( ) );

         mtime daylight_start_time( reader.read_attribute( c_attribute_daylight_start_time ) );

         int daylight_finish_month = atoi( reader.read_attribute( c_attribute_daylight_finish_month ).c_str( ) );
         int daylight_finish_occurrence = atoi( reader.read_attribute( c_attribute_daylight_finish_occurrence ).c_str( ) );
         int daylight_finish_day_of_week = atoi( reader.read_attribute( c_attribute_daylight_finish_day_of_week ).c_str( ) );

         mtime daylight_finish_time( reader.read_attribute( c_attribute_daylight_finish_time ) );

         int daylight_utc_offset = ( int )( atof( reader.read_attribute( c_attribute_daylight_utc_offset ).c_str( ) ) * 3600.0 );
         int bias = daylight_utc_offset - tz_data.utc_offset;

         for( int i = daylight_year_start; i <= daylight_year_finish; i++ )
         {
            date_time ds_start( ( year )i,
             ( month )daylight_start_month,
             ( weekday )daylight_start_day_of_week,
             ( occurrence )daylight_start_occurrence,
             daylight_start_time.get_hour( ), daylight_start_time.get_minute( ) );

            date_time ds_finish( ( year )i,
             ( month )daylight_finish_month,
             ( weekday )daylight_finish_day_of_week,
             ( occurrence )daylight_finish_occurrence,
             daylight_finish_time.get_hour( ), daylight_finish_time.get_minute( ) );

            tz_data.daylight_savings.years_info.insert( years_info_value_type( i,
             daylight_bias_info( bias, ds_start.as_string( ), ds_finish.as_string( ) ) ) );
         }
      }

      if( reader.has_started_section( c_section_daylight_saving_changes ) )
      {
         while( reader.has_started_section( c_section_daylight_saving_change ) )
         {
            int year_start = atoi( reader.read_attribute( c_attribute_year_start ).c_str( ) );
            int year_finish = atoi( reader.read_attribute( c_attribute_year_finish ).c_str( ) );

            reader.read_attribute( c_attribute_description ); // i.e. not actually used

            int start_month = atoi( reader.read_attribute( c_attribute_start_month ).c_str( ) );
            int start_occurrence = atoi( reader.read_attribute( c_attribute_start_occurrence ).c_str( ) );
            int start_day_of_week = atoi( reader.read_attribute( c_attribute_start_day_of_week ).c_str( ) );

            mtime start_time( reader.read_attribute( c_attribute_start_time ) );

            int finish_month = atoi( reader.read_attribute( c_attribute_finish_month ).c_str( ) );
            int finish_occurrence = atoi( reader.read_attribute( c_attribute_finish_occurrence ).c_str( ) );
            int finish_day_of_week = atoi( reader.read_attribute( c_attribute_finish_day_of_week ).c_str( ) );

            mtime finish_time( reader.read_attribute( c_attribute_finish_time ) );

            int utc_offset = ( int )( atof( reader.read_attribute( c_attribute_utc_offset ).c_str( ) ) * 3600.0 );
            int bias = utc_offset - tz_data.utc_offset;

            for( int i = year_start; i <= year_finish; i++ )
            {
               date_time ds_start( ( year )i,
                ( month )start_month,
                ( weekday )start_day_of_week,
                ( occurrence )start_occurrence,
                start_time.get_hour( ), start_time.get_minute( ) );

               date_time ds_finish;

               if( !finish_month )
                  ds_finish = date_time( ( year )i, ( month )1, ( day )1, ( hour )0, ( minute )0 );
               else
                  ds_finish = date_time( ( year )i, ( month )finish_month,
                   ( weekday )finish_day_of_week, ( occurrence )finish_occurrence,
                   finish_time.get_hour( ), finish_time.get_minute( ) );

               if( tz_data.daylight_savings.years_info.count( i ) )
                  tz_data.daylight_savings.years_info.erase( i );

               tz_data.daylight_savings.years_info.insert( years_info_value_type( i,
                daylight_bias_info( bias, ds_start.as_string( ), ds_finish.as_string( ) ) ) );
            }

            reader.finish_section( c_section_daylight_saving_change );
         }

         reader.finish_section( c_section_daylight_saving_changes );
      }

      g_timezones.insert( make_pair( name.empty( ) ? abbr : name, tz_data ) );

      g_timezone_abbrs[ name.empty( ) ? abbr : name ] = abbr;

      if( !tz_data.daylight_abbr.empty( ) )
      {
         if( name.empty( ) )
            g_daylight_names[ tz_data.daylight_abbr ] = abbr;
         else
            g_daylight_names[ name + "_DST" ] = name;
      }

      reader.finish_section( c_section_timezone );
   }

   reader.finish_section( c_section_timezones );
   reader.verify_finished_sections( );
}

string list_timezones( )
{
   string retval;

   for( timezone_const_iterator tci = g_timezones.begin( ); tci != g_timezones.end( ); ++tci )
   {
      if( !retval.empty( ) )
         retval += '\n';

      retval += tci->first + " " + tci->second.description + " " + g_timezone_abbrs[ tci->first ];

      if( !tci->second.daylight_savings.years_info.empty( ) )
         retval += "/" + tci->second.daylight_abbr;
   }

   return retval;
}

string get_tz_desc( const string& tz_name )
{
   string name( tz_name );
   if( !name.empty( ) && name[ name.length( ) - 1 ] == '+' )
      name.erase( name.length( ) - 1 );

   if( name.empty( ) )
      name = get_timezone( );
      
   if( !g_timezones.count( name ) )
      throw runtime_error( "unable to find timezone information for '" + name + "'" );

   return g_timezones[ name ].description;
}

void get_tz_info( const date_time& dt, string& tz_name, float& offset )
{
   string tz( tz_name );
   bool is_daylight = false;
   bool use_daylight = false;

   if( g_daylight_names.count( tz ) )
   {
      is_daylight = true;
      use_daylight = true;
      tz = g_daylight_names[ tz ];
   }

   if( !tz.empty( ) && tz[ tz.length( ) - 1 ] == '+' )
   {
      use_daylight = true;
      tz.erase( tz.length( ) - 1 );
   }

   if( !g_timezones.count( tz ) )
      throw runtime_error( "unable to find timezone information for '" + tz + "'" );

   int utc_offset;

   local_utc_conv( dt, g_timezones[ tz ].utc_offset,
    ( use_daylight ? &g_timezones[ tz ].daylight_savings : 0 ), false, &utc_offset, is_daylight );

   offset = ( float )utc_offset / 3600.0;

   if( utc_offset == g_timezones[ tz ].utc_offset )
      tz_name = g_timezone_abbrs[ tz ];
   else
      tz_name = g_timezones[ tz ].daylight_abbr;
}

date_time utc_to_local( const date_time& dt )
{
   return utc_to_local( dt, get_tz_name( ) );
}

date_time utc_to_local( const date_time& dt, string& tz_name )
{
   string tz( tz_name );
   bool use_daylight = false;

   if( g_daylight_names.count( tz ) )
   {
      use_daylight = true;
      tz = g_daylight_names[ tz ];
   }

   if( !tz.empty( ) && tz[ tz.length( ) - 1 ] == '+' )
   {
      use_daylight = true;
      tz.erase( tz.length( ) - 1 );
   }

   if( !g_timezones.count( tz ) )
      throw runtime_error( "unable to find timezone information for '" + tz + "'" );

   int utc_offset;

   date_time rc = local_utc_conv( dt, g_timezones[ tz ].utc_offset,
    ( use_daylight ? &g_timezones[ tz ].daylight_savings : 0 ), true, &utc_offset );

   if( utc_offset == g_timezones[ tz ].utc_offset )
      tz_name = g_timezone_abbrs[ tz ];
   else
      tz_name = g_timezones[ tz ].daylight_abbr;

   return rc;
}

date_time utc_to_local( const date_time& dt, const string& tz_name )
{
   string tz( tz_name );
   bool use_daylight = false;

   if( g_daylight_names.count( tz ) )
   {
      use_daylight = true;
      tz = g_daylight_names[ tz ];
   }

   if( !tz.empty( ) && tz[ tz.length( ) - 1 ] == '+' )
   {
      use_daylight = true;
      tz.erase( tz.length( ) - 1 );
   }

   if( !g_timezones.count( tz ) )
      throw runtime_error( "unable to find timezone information for '" + tz + "'" );

   return local_utc_conv( dt, g_timezones[ tz ].utc_offset,
    ( use_daylight ? &g_timezones[ tz ].daylight_savings : 0 ), true );
}

date_time local_to_utc( const date_time& dt, const string& tz_name )
{
   string tz( tz_name );
   bool is_daylight = false;
   bool use_daylight = false;

   if( g_daylight_names.count( tz ) )
   {
      is_daylight = true;
      use_daylight = true;
      tz = g_daylight_names[ tz ];
   }

   if( !tz.empty( ) && tz[ tz.length( ) - 1 ] == '+' )
   {
      use_daylight = true;
      tz.erase( tz.length( ) - 1 );
   }

   if( !g_timezones.count( tz ) )
      throw runtime_error( "unable to find timezone information for '" + tz + "'" );

   return local_utc_conv( dt, g_timezones[ tz ].utc_offset,
    ( use_daylight ? &g_timezones[ tz ].daylight_savings : 0 ), false, 0, is_daylight );
}

bool schedulable_month_and_day( int month, int day )
{
   bool okay = false;

   switch( month )
   {
      case 1:
      case 3:
      case 5:
      case 7:
      case 8:
      case 10:
      case 12:
      if( day >= 1 && day <= 31 )
         okay = true;
      break;

      case 4:
      case 6:
      case 9:
      case 11:
      if( day >= 1 && day <= 30 )
         okay = true;
      break;

      case 2:
      if( day >= 1 && day <= 28 )
         okay = true;
      break;
   }

   return okay;
}

void add_class_map( const string& class_id, const string& map_id, const string& file_name, bool in_reverse )
{
   guard g( g_mutex );

   string map_name( class_id + ":" + map_id );

   if( g_class_maps.count( map_name ) )
      g_class_maps[ map_name ].first++;
   else
   {
      map< string, string > new_map;

      vector< string > lines;
      buffer_file_lines( module_directory( ) + file_name, lines );

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );
         string::size_type pos = next_line.find( ' ' );
         if( pos == string::npos )
            throw runtime_error( "found incorrect line formatting '"
             + next_line + "' whilst adding class map '" + map_name + "'" );

         if( !in_reverse )
            new_map.insert( make_pair( next_line.substr( 0, pos ), next_line.substr( pos + 1 ) ) );
         else
            new_map.insert( make_pair( next_line.substr( pos + 1 ), next_line.substr( 0, pos ) ) );
      }

      g_class_maps.insert( make_pair( map_name, make_pair( 1, new_map ) ) );
      TRACE_LOG( TRACE_MODS_GEN, "[add_class_map] " + map_name + " " + file_name + ( !in_reverse ? "" : " (reverse)" ) );
   }
}

void remove_class_map( const string& class_id, const string& map_id )
{
   guard g( g_mutex );

   string map_name( class_id + ":" + map_id );
   if( g_class_maps.count( map_name ) )
   {
      if( g_class_maps[ map_name ].first > 1 )
         g_class_maps[ map_name ].first--;
      else
      {
         g_class_maps.erase( map_name );
         TRACE_LOG( TRACE_MODS_GEN, "[remove_class_map] " + map_name );
      }
   }
}

string get_class_map_value( const string& class_id, const string& map_id, const string& key )
{
   guard g( g_mutex );

   string map_name( class_id + ":" + map_id );
   if( !g_class_maps.count( map_name ) )
      throw runtime_error( "class map '" + map_name + "' was not found" );

   string retval;
   if( g_class_maps[ map_name ].second.count( key ) )
      retval = g_class_maps[ map_name ].second[ key ];

   return retval;
}

void send_email_message(
 const string& recipient, const string& subject,
 const string& message, const string& html_source, const vector< string >* p_extra_headers,
 const vector< string >* p_file_names, const string* p_tz_name, const vector< string >* p_image_names,
 const string* p_image_path_prefix )
{
   user_account account;

   account.sender = get_smtp_sender( );
   account.username = get_smtp_username( );
   account.password = get_smtp_password( );

   string suffix( get_smtp_suffix( ) );
   if( !suffix.empty( ) && account.username.find( '@' ) == string::npos )
      account.username += "@" + suffix;

   vector< string > recipients;

   string to( recipient );
   if( !to.empty( ) && to[ 0 ] == '@' )
      to = get_session_variable( to );

   recipients.push_back( to );

   send_email_message( account, recipients, subject, message,
    html_source, p_extra_headers, p_file_names, p_tz_name, p_image_names, p_image_path_prefix );
}

void send_email_message(
 const vector< string >& recipients, const string& subject,
 const string& message, const string& html_source, const vector< string >* p_extra_headers,
 const vector< string >* p_file_names, const string* p_tz_name, const vector< string >* p_image_names,
 const string* p_image_path_prefix )
{
   user_account account;

   account.sender = get_smtp_sender( );
   account.username = get_smtp_username( );
   account.password = get_smtp_password( );

   string suffix( get_smtp_suffix( ) );
   if( !suffix.empty( ) && account.username.find( '@' ) == string::npos )
      account.username += "@" + suffix;

   send_email_message( account, recipients, subject, message,
    html_source, p_extra_headers, p_file_names, p_tz_name, p_image_names, p_image_path_prefix );
}

void send_email_message( const user_account& account,
 const vector< string >& recipients, const string& subject,
 const string& message, const string& html_source, const vector< string >* p_extra_headers,
 const vector< string >* p_file_names, const string* p_tz_name, const vector< string >* p_image_names,
 const string* p_image_path_prefix )
{
   if( account.username.empty( ) )
      throw runtime_error( "missing SMTP account information" );

   string tz_name;
   float utc_offset = 0.0;

   date_time dt( date_time::standard( ) );

   if( p_tz_name && !p_tz_name->empty( ) )
   {
      tz_name = *p_tz_name;
      get_tz_info( dt, tz_name, utc_offset );

      dt += minutes( ( int32_t )( utc_offset * 60.0 ) );
   }

   string charset( "utf-8" );

   string password( account.password );

   smtp_user_info user_info( account.sender,
    account.username, password, &dt, utc_offset, &tz_name, &charset );

   string security( get_smtp_security( ) );
   if( !security.empty( ) )
   {
      string auth_type;

      string::size_type pos = security.find( ' ' );
      if( pos != string::npos )
      {
         auth_type = security.substr( pos + 1 );
         string ssl_tls( security.substr( 0, pos ) );

         if( ssl_tls != "SSL" && ssl_tls != "TLS" )
            throw runtime_error( "invalid SMTP security setting '" + security + "'" );

         if( ssl_tls == "SSL" )
            user_info.use_ssl = true;
         else if( ssl_tls == "TLS" )
            user_info.use_tls = true;
      }
      else if( security == "SSL" )
         user_info.use_ssl = true;
      else if( security == "TLS" )
         user_info.use_ssl = true;
      else
         auth_type = security;

      if( !auth_type.empty( ) )
      {
         if( auth_type == "PLAIN" )
            user_info.auth_type = e_smtp_auth_type_plain;
         else if( auth_type == "LOGIN" )
            user_info.auth_type = e_smtp_auth_type_login;
         else if( auth_type == "CRAM-MD5" )
            user_info.auth_type = e_smtp_auth_type_cram_md5;
         else
            throw runtime_error( "invalid SMTP security setting '" + security + "'" );
      }
   }

   if( user_info.use_ssl || user_info.use_tls )
   {
      if( !get_using_ssl( ) )
         throw runtime_error( "unable to use SSL/TLS without SSL support and a valid PEM file" );
   }

   user_info.max_attachment_bytes = get_smtp_max_attached_data( );

   string html;
   if( !html_source.empty( ) )
      html = buffer_file( html_source );

   progress* p_progress = 0;
   trace_progress progress( TRACE_MAIL_OPS );

   if( get_trace_flags( ) & TRACE_MAIL_OPS )
      p_progress = &progress;

   int attempt = 0;
   while( true )
   {
      try
      {
         send_smtp_message( get_smtp_server( ), user_info, recipients, subject,
          message, html, p_extra_headers, p_file_names, p_image_names, p_image_path_prefix, p_progress );

         break;
      }
      catch( exception& x )
      {
         if( ++attempt >= get_smtp_max_send_attempts( ) )
            throw;
      }
      catch( ... )
      {
         throw;
      }
   }
}

string generate_hashcash( const string& recipient )
{
   string::size_type pos = recipient.find_last_of( "<" );

   string resource( recipient );
   if( pos != string::npos )
   {
      resource.erase( 0, pos + 1 );
      if( resource[ resource.size( ) - 1 ] == '>' )
         resource.erase( resource.size( ) - 1 );
   }

   return create_hashcash( resource );
}

bool has_valid_hashcash( const string& value )
{
   return check_hashcash( value );
}

string check_email_headers( const vector< string >& headers, bool create_script_output )
{
   ostringstream osstr;

   auto_ptr< mail_source > ap_mail_source;

   if( !get_mbox_username( ).empty( ) )
      ap_mail_source.reset( new mbox_source( ) );
   else
      ap_mail_source.reset( new pop3_source( ) );

   ap_mail_source->start_processing( );

   int num_messages = ap_mail_source->get_num_messages( );
   if( !create_script_output )
      osstr << "\nfound " << num_messages << ( num_messages == 1 ? " message\n" : " messages\n" ) << endl;

   bool has_any_scripts = false;
   for( int i = 0; i < num_messages; i++ )
   {
      if( i > 0 && !create_script_output )
         osstr << "\n";

      vector< string > email_headers( headers.begin( ), headers.end( ) );

      if( email_headers.empty( ) )
      {
         email_headers.push_back( "From:" );
         email_headers.push_back( "Subject:" );
         email_headers.push_back( "Date:" );
      }

      ap_mail_source->get_message_headers( i + 1, email_headers );

      string script_from;
      string script_name;
      for( size_t j = 0; j < email_headers.size( ); j++ )
      {
         if( create_script_output )
         {
            if( lower( email_headers[ j ] ).find( "from: " ) == 0 )
               script_from = email_headers[ j ].substr( 6 );
            else if( lower( email_headers[ j ] ).find( "subject: " ) == 0 )
            {
               string::size_type pos = email_headers[ j ].find( c_email_subject_script_marker );
               if( pos != string::npos )
               {
                  script_name = email_headers[ j ].substr( pos + strlen( c_email_subject_script_marker ) );
                  while( !script_name.empty( ) && script_name[ 0 ] == ' ' )
                     script_name.erase( 0, 1 );
               }
            }
         }
         else
            osstr << email_headers[ j ] << endl;
      }

      if( !script_from.empty( ) && !script_name.empty( ) )
      {
         osstr << ".session_variable @from \"" << escaped( script_from, "\"" ) << "\"\n";
         osstr << ".runscript " << script_name << "\n";

         has_any_scripts = true;
         ap_mail_source->delete_message( i + 1 );
      }
   }

   if( create_script_output && has_any_scripts )
      osstr << ".quit\n";

   ap_mail_source->finish_processing( );

   return osstr.str( );
}

void fetch_email_messages( const user_account& account,
 const string* p_file_name_prefix, vector< pair< bool, string > >* p_messages, bool skip_scripts )
{
   auto_ptr< mail_source > ap_mail_source;

   string::size_type pos = account.username.find( '@' );

   string password( account.password );

   if( !get_mbox_path( ).empty( ) && ( pos == string::npos
    || account.username.substr( pos + 1 ) == get_domain( ) ) )
      ap_mail_source.reset( new mbox_source( account.username ) );
   else
      ap_mail_source.reset( new pop3_source( account.username, password ) );

   ap_mail_source->start_processing( );

   int num_messages = ap_mail_source->get_num_messages( );

   for( int i = 0; i < num_messages; i++ )
   {
      if( p_file_name_prefix )
      {
         ostringstream osstr;
         osstr << *p_file_name_prefix << ifmt( 4 ) << ( i + 1 ) << ".txt";

         ofstream outf( osstr.str( ).c_str( ) );
         ap_mail_source->get_message( i + 1, outf );

         outf.flush( );
         if( !outf.good( ) )
            throw runtime_error( "unexpected bad output stream" );
      }

      if( p_messages )
      {
         ostringstream osstr;

         bool is_mime = false;

         if( skip_scripts )
         {
            vector< string > email_headers;
            email_headers.push_back( "Subject:" );

            ap_mail_source->get_message_headers( i + 1, email_headers );

            if( lower( email_headers[ 0 ] ).find( "subject: " ) == 0
             && email_headers[ 0 ].find( c_email_subject_script_marker ) != string::npos )
               continue;
         }

         ap_mail_source->get_message( i + 1, osstr, &is_mime );
         p_messages->push_back( make_pair( is_mime, osstr.str( ) ) );
      }

      ap_mail_source->delete_message( i + 1 );
   }

   ap_mail_source->finish_processing( );
}

void fetch_email_messages( const string& file_name_prefix, bool skip_scripts )
{
   user_account account;

   if( !get_mbox_username( ).empty( ) )
      account.username = get_mbox_username( );
   else
   {
      account.username = get_pop3_username( );
      account.password = get_pop3_password( );

      string suffix( get_pop3_suffix( ) );
      if( !suffix.empty( ) && account.username.find( '@' ) == string::npos )
         account.username += "@" + suffix;
   }

   fetch_email_messages( account, &file_name_prefix, 0, skip_scripts );
}

void fetch_email_messages( vector< pair< bool, string > >& messages, bool skip_scripts )
{
   user_account account;

   if( !get_mbox_username( ).empty( ) )
      account.username = get_mbox_username( );
   else
   {
      account.username = get_pop3_username( );
      account.password = get_pop3_password( );

      string suffix( get_pop3_suffix( ) );
      if( !suffix.empty( ) && account.username.find( '@' ) == string::npos )
         account.username += "@" + suffix;
   }

   fetch_email_messages( account, 0, &messages, skip_scripts );
}

void fetch_email_messages( const user_account& account,
 vector< pair< bool, string > >& messages, bool skip_scripts )
{
   fetch_email_messages( account, 0, &messages, skip_scripts );
}

string decode_email_header( const string& header )
{
   string retval( header );

   string s( header );
   string encoding, charset;

   string::size_type pos = s.find( "=?" );
   if( pos != string::npos )
   {
      string::size_type epos = s.find( "?", pos + 2 );
      if( epos != string::npos )
      {
         charset = header.substr( pos + 2, epos - pos - 2 );
         s.erase( 0, epos + 1 );

         epos = s.find( "?" );
         if( epos != string::npos )
         {
            encoding = s.substr( 0, epos );
            s.erase( 0, epos + 1 );

            epos = s.find( "?=" );
            if( epos != string::npos )
               retval = decode_text( lower( encoding ), lower( charset ), s.substr( 0, epos ) );
         }
      }
   }

   return retval;
}

void decode_mime_message( const string& mime, string& message,
 string& html_message, vector< pair< string, string > >& attachments )
{
   mime_decoder decoder( mime );

   decode_mime( decoder, message, html_message, attachments );
}

void parse_email_address( const string& address, string& name, string& email )
{
   name.erase( );
   email.erase( );

   bool finished = false;
   bool in_email = false;
   bool in_quotes = false;
   for( size_t i = 0; i < address.size( ); i++ )
   {
      char c = address[ i ];

      if( c == '"' )
         in_quotes = !in_quotes;
      else if( !in_quotes )
      {
         if( c == '<' )
            in_email = true;
         else if( c == '>' )
            finished = true;
         else if( in_email )
            email += c;
         else
            name += c;
      }
      else
         name += c;

      if( finished )
         break;
   }

   while( !name.empty( ) && name[ name.length( ) - 1 ] == ' ' )
      name.erase( name.length( ) - 1 );

   if( email.empty( ) )
      email = name;
}

void save_attachment( const string& encoding, const string& data, const string& output_file )
{
   if( encoding == "base64" )
   {
      ofstream outf( output_file.c_str( ), ios::out | ios::binary );
      if( !outf )
         throw runtime_error( "unable to open file '" + output_file + "' for output" );

      vector< string > lines;
      raw_split( data, lines, '\n' );

      outf << base64::decode( raw_join( lines ) );

      outf.flush( );
      if( !outf.good( ) )
         throw runtime_error( "unexpected bad output stream" );
   }
   else if( encoding == "7bit" || encoding == "8bit" || encoding == "quoted-printable" )
   {
      ofstream outf( output_file.c_str( ) );
      if( !outf )
         throw runtime_error( "unable to open file '" + output_file + "' for output" );

      if( encoding != "quoted-printable" )
         outf << data;
      else
         outf << decode_quoted_printable( data );

      outf.flush( );
      if( !outf.good( ) )
         throw runtime_error( "unexpected bad output stream" );
   }
   else
      throw runtime_error( "unsupported attachment encoding '" + encoding + "'" );
}

string remove_html_scripts( const string& html )
{
   static regex html_tag_pair( c_regex_html_paired_tags );

   string old_html( html );
   string new_html;

   while( true )
   {
      string::size_type len;
      vector< string > refs;

      string::size_type pos = html_tag_pair.search( old_html, &len, &refs );

      if( pos == string::npos )
      {
         new_html += old_html;
         break;
      }

      if( refs.empty( ) )
         throw runtime_error( "unexpected missing refs for html_tag_pair" );

      string tag( lower( refs[ 0 ] ) );
      string::size_type skip_len = refs[ 0 ].size( ) + 2;

      bool extend_len = false;

      if( tag == "script" )
         extend_len = true;
      else
      {
         string data;
         if( refs.size( ) > 1 )
            data += refs[ 1 ];
         if( refs.size( ) > 2 )
            data += refs[ 2 ];

         // NOTE: If this tag doesn't contain other tags then no need to process its data.
         if( data.find( '<' ) == string::npos )
            extend_len = true;
      }

      if( extend_len )
      {
         skip_len += skip_len + 1; // i.e. closing tag is one extra

         if( refs.size( ) > 1 )
            skip_len += refs[ 1 ].size( );
         if( refs.size( ) > 2 )
            skip_len += refs[ 2 ].size( );
      }

      if( tag == "script" )
         new_html += old_html.substr( 0, pos );
      else
         new_html += old_html.substr( 0, pos + skip_len );

      old_html.erase( 0, pos + skip_len );
   }

   return new_html;
}

string convert_html_to_text( const string& html )
{
   return extract_text_from_html( html );
}

string create_html_embedded_image( const string& source_file )
{
   string s, file_ext;
   string::size_type pos = source_file.find( "." );

   if( pos != string::npos )
   {
      file_ext = source_file.substr( pos + 1 );
      string buffer( buffer_file( source_file ) );

      s = "data:image/" + file_ext + ";base64," + base64::encode( buffer );
   }

   return s;
}

void load_utxo_information( const string& source_address, const string& file_name )
{
   get_utxo_information( source_address, file_name );
}

uint64_t determine_utxo_balance( const string& file_name )
{
   return get_utxos_balance_amt( file_name );
}

string construct_raw_transaction(
 const string& source_address, const string& destination_address,
 uint64_t amount, quote_style qs, uint64_t& fee, string& sign_tx_template, const string& file_name )
{
   return create_raw_transaction( source_address,
    destination_address, amount, qs, fee, sign_tx_template, &file_name );
}

string send_raw_transaction( const string& tx )
{
   string s;

   string tmp( "~" + uuid( ).as_string( ) );

   // NOTE: In order to get the transaction id need to use "decode-tx" (do this first).
   string cmd( "curl -s --data tx=" + tx + " http://blockchain.info/decode-tx >" + tmp );

   system( cmd.c_str( ) );

   s = buffer_file( tmp );
   file_remove( tmp );

   string marker( "&#034;hash&#034;:&#034;" );

   string::size_type pos = s.rfind( marker );
   if( pos != string::npos )
   {
      s.erase( 0, pos + marker.length( ) );

      pos = s.find( '&' );
      if( pos != string::npos )
      {
         s.erase( pos );

         string cmd( "curl -s --data tx=" + tx + " http://blockchain.info/pushtx >" + tmp );
         system( cmd.c_str( ) );

         string ss( buffer_file( tmp ) );
         file_remove( tmp );

         // NOTE: If we don't find an 'error' token then assume that the tx push worked.
         if( ss.find( "error" ) != string::npos || ss.find( "Error" ) != string::npos )
         {
            s = ss;
            pos = s.find( '\n' );
            if( pos != string::npos )
               s.erase( pos );
         }
      }
   }

   return s;
}

void meta_relationship_child_name( string& name,
 const string& child_name, const string& parent_name, const string& separator )
{
   remove_suffix( name, child_name, separator );
   remove_suffix( name, parent_name, separator );

   if( name.empty( ) )
      name = child_name;
   else
      name = child_name + separator + name;
}

string meta_sql_type( const string& field_type, bool is_mandatory, sql_char_type char_type )
{
   string sql_type;

   if( field_type == "tdate" || field_type == "udate" )
      sql_type = "CHAR(10)";
   else if( field_type == "ttime" || field_type == "mtime" )
      sql_type = "CHAR(8)";
   else if( field_type == "datetime" || field_type == "tdatetime" || field_type == "date_time" )
      sql_type = "CHAR(23)";
   else if( field_type == "int" || field_type == "bool" || field_type == "tbool" )
      sql_type = "INTEGER";
   else if( field_type == "numeric" )
      sql_type = "NUMERIC(28\\\\,8)";
   else if( field_type == "string" || char_type == e_sql_char_type_foreign_key )
   {
      if( char_type == e_sql_char_type_std )
         sql_type = "VARCHAR(" + to_string( c_sql_std_char_size ) + ")";
      else if( char_type == e_sql_char_type_small )
         sql_type = "VARCHAR(" + to_string( c_sql_small_char_size ) + ")";
      else if( char_type == e_sql_char_type_large )
         sql_type = "LONGTEXT";
      else if( char_type == e_sql_char_type_security )
         sql_type = "VARCHAR(10)";
      else if( char_type == e_sql_char_type_foreign_key )
         sql_type = "VARCHAR(" + to_string( c_max_key_length ) + ")";
      else
         throw runtime_error( "unexpected char_type '" + to_string( char_type ) + "' in meta_sql_type" );
   }
   else
      throw runtime_error( "unknown field type '" + field_type + "' for meta_sql_type" );

   // NOTE: As '' is used rather than NULL by the server (and the server does not support NULL for numeric
   // types nor permit it as a seperate state to empty string for text types) NOT NULL is always used here.
   ( void )is_mandatory;
   sql_type += " NOT NULL";

   return sql_type;
}

string meta_field_uom( int uom )
{
   string str;

   switch( uom )
   {
      case 0:
      break;

      case 1:
      str = "sqm";
      break;

      case 2:
      str = "km";
      break;

      case 3:
      str = "m";
      break;

      case 4:
      str = "cm";
      break;

      case 5:
      str = "mm";
      break;

      case 6:
      str = "kg";
      break;

      case 7:
      str = "g";
      break;

      case 8:
      str = "mg";
      break;

      case 900:
      str = "#";
      break;

      case 901:
      str = "$";
      break;

      case 902:
      str = "%";
      break;

      default:
      throw runtime_error( "unexpected uom #" + to_string( uom ) + " in meta_field_uom" );
   }

   return str;
}

int meta_field_type_primitive( const string& type )
{
   int rc;

   if( type == "string" )
      rc = e_primitive_string;
   else if( type == "datetime" || type == "tdatetime" || type == "date_time" )
      rc = e_primitive_datetime;
   else if( type == "date" || type == "udate" || type == "tdate" )
      rc = e_primitive_date;
   else if( type == "time" || type == "mtime" || type == "ttime" )
      rc = e_primitive_time;
   else if( type == "numeric" )
      rc = e_primitive_numeric;
   else if( type == "int" )
      rc = e_primitive_int;
   else if( type == "bool" || type == "tbool" )
      rc = e_primitive_bool;
   else
      throw runtime_error( "unknown primitive type '" + type + "' in meta_primitive" );

   return rc;
}

string meta_field_type_name( int primitive,
 bool mandatory, const string& parent_class_name, const string& model_name, bool* p_is_customised )
{
   string type;
   bool custom = false;

   if( !parent_class_name.empty( ) )
      type = model_name + "_" + parent_class_name;
   else
   {
      switch( primitive )
      {
         case e_primitive_string:
         type = "string";
         break;

         case e_primitive_datetime:
         if( mandatory )
            type = "date_time";
         else
            type = "tdatetime";
         custom = true;
         break;

         case e_primitive_date:
         if( mandatory )
            type = "udate";
         else
            type = "tdate";
         custom = true;
         break;

         case e_primitive_time:
         if( mandatory )
            type = "mtime";
         else
            type = "ttime";
         custom = true;
         break;

         case e_primitive_numeric:
         type = "numeric";
         custom = true;
         break;

         case e_primitive_int:
         type = "int";
         break;

         case e_primitive_bool:
         if( mandatory )
            type = "bool";
         else
         {
            type = "tbool";
            custom = true;
         }
         break;

         default:
         throw runtime_error( "unknown primitive #" + to_string( primitive ) + " in meta_field_type_name" );
      }
   }

   if( p_is_customised )
      *p_is_customised = custom;

   return type;
}

string meta_field_domain_type( const string& enum_id, int primitive, int max_size,
 const string& min_value, const string& max_value, int numeric_digits, int numeric_decimals,
 int string_domain, int date_precision, int time_precision, bool show_plus_sign, int zero_padding,
 int fraction_limit, string& mask, string* p_tmask )
{
   string domain_type;

   if( max_size
    || !max_value.empty( ) || !min_value.empty( )
    || numeric_decimals || numeric_digits || string_domain )
   {
      string field_type;

      switch( primitive )
      {
         case 0:
         field_type = "string";
         break;

         case 1:
         field_type = "datetime";
         break;

         case 2:
         field_type = "date";
         break;

         case 3:
         field_type = "time";
         break;

         case 4:
         field_type = "numeric";
         break;

         case 5:
         field_type = "int";
         break;

         default:
         throw runtime_error( "unexpected primitive #" + to_string( primitive ) + " in meta_field_domain_type" );
      }

      string primary_domain_type;
      string secondary_domain_type;

      if( string_domain )
      {
         if( primitive != 0 )
            throw runtime_error( "unexpected string_domain found for non-string primitive in meta_field_domain_type" );

         switch( string_domain )
         {
            case 1:
            primary_domain_type = "domain_string_identifier_format";
            break;

            case 2:
            primary_domain_type = "domain_string_label_format";
            break;

            case 3:
            primary_domain_type = "domain_string_filename_format";
            break;

            default:
            throw runtime_error( "unknown string_domain #" + to_string( string_domain ) + " in meta_field_domain_type" );
         }
      }

      if( max_size )
      {
         if( primitive != 0 )
            throw runtime_error( "unexpected max_size found for non-string primitive in meta_field_domain_type" );

         string new_domain_type( "domain_string_max_size< " + to_string( max_size ) + " >" );

         if( primary_domain_type.empty( ) )
            primary_domain_type = new_domain_type;
         else
            secondary_domain_type = new_domain_type;
      }

      if( primitive == 4 ) // i.e. numeric
      {
         if( !numeric_digits )
            numeric_digits = numeric::e_max_digits;

         if( !max_value.empty( ) && numeric_digits == numeric::e_max_digits )
         {
            // NOTE: As decimals might all be zero add an extra non-zero digit.
            numeric n( string( max_value + "1" ).c_str( ) );
            if( numeric_digits > n.digits( ) - 1 )
               numeric_digits = n.digits( ) - 1;
         }

         if( !min_value.empty( ) && numeric_digits == numeric::e_max_digits )
         {
            // NOTE: As decimals might all be zero add an extra non-zero digit.
            numeric n( string( min_value + "1" ).c_str( ) );
            if( numeric_digits < n.digits( ) - 1 )
               numeric_digits = n.digits( ) - 1;
         }

         string new_domain_type( "domain_numeric_format< numeric, "
          + to_string( numeric_digits ) + ", " + to_string( numeric_decimals ) );

         if( fraction_limit > 0 )
            new_domain_type += ", " + to_string( fraction_limit );

         new_domain_type += " >";

         if( primary_domain_type.empty( ) )
            primary_domain_type = new_domain_type;
         else
            secondary_domain_type = new_domain_type;
      }

      if( !max_value.empty( ) )
      {
         string new_domain_type;

         if( primitive == 1 )
         {
            date_time dt_min( min_value );
            date_time dt_max( max_value );

            new_domain_type = "domain_datetime_range< "
             + to_string( ( int )dt_min.get_year( ) ) + ", " + to_string( ( int )dt_min.get_month( ) )
             + ", " + to_string( ( int )dt_min.get_day( ) ) + ", " + to_string( ( int )dt_min.get_hour( ) )
             + ", " + to_string( ( int )dt_min.get_minute( ) ) + ", " + to_string( ( int )dt_min.get_second( ) )
             + ", " + to_string( ( int )dt_max.get_year( ) ) + ", " + to_string( ( int )dt_max.get_month( ) )
             + ", " + to_string( ( int )dt_max.get_day( ) ) + ", " + to_string( ( int )dt_max.get_hour( ) )
             + ", " + to_string( ( int )dt_max.get_minute( ) ) + ", " + to_string( ( int )dt_max.get_second( ) ) + " >";
         }
         else if( primitive == 2 )
         {
            udate ud_min( min_value );
            udate ud_max( max_value );

            new_domain_type = "domain_date_range< "
             + to_string( ( int )ud_min.get_year( ) )
             + ", " + to_string( ( int )ud_min.get_month( ) ) + ", " + to_string( ( int )ud_min.get_day( ) )
             + ", " + to_string( ( int )ud_max.get_year( ) )
             + ", " + to_string( ( int )ud_max.get_month( ) ) + ", " + to_string( ( int )ud_max.get_day( ) ) + " >";
         }
         else if( primitive == 4 )
         {
            string::size_type pos = min_value.find( '.' );
            int64_t min_whole = from_string< int64_t >( min_value.substr( 0, pos ) );
            int64_t min_decimal = 0;
            if( pos != string::npos )
               min_decimal = from_string< int64_t >( min_value.substr( pos + 1 ) );

            pos = max_value.find( '.' );
            int64_t max_whole = from_string< int64_t >( max_value.substr( 0, pos ) );
            int64_t max_decimal = 0;
            if( pos != string::npos )
               max_decimal = from_string< int64_t >( max_value.substr( pos + 1 ) );

            new_domain_type = "domain_numeric_range< numeric, UINT64_C( "
             + to_string( min_whole ) + " ), UINT64_C( " + to_string( min_decimal )
             + " ), UINT64_C( " + to_string( max_whole ) + " ), UINT64_C( " + to_string( max_decimal ) + " ) >";
         }
         else if( primitive == 5 )
         {
            new_domain_type = "domain_int_range< "
             + to_string( min_value ) + ", " + to_string( max_value ) + " >";
         }
         else
            throw runtime_error( "unexpected primitive #" + to_string( primitive ) + " for range domain in meta_field_domain_type" );

         if( primary_domain_type.empty( ) )
            primary_domain_type = new_domain_type;
         else
            secondary_domain_type = new_domain_type;
      }

      if( secondary_domain_type.empty( ) )
         domain_type = primary_domain_type;
      else
      {
         domain_type = "aggregate_domain< " + field_type + ",\n";
         domain_type += ' ' + primary_domain_type + ",\n";
         domain_type += ' ' + secondary_domain_type + " >";
      }
   }

   if( primitive == 1 ) // i.e. datetime
   {
      switch( time_precision )
      {
         case 0:
         mask = "yyyy-mm-dd hh:mm:ss.tht";
         break;

         case 1:
         mask = "yyyy-mm-dd hh:mm";
         break;

         case 2:
         mask = "yyyy-mm-dd hh:mm:ss";
         break;

         default:
         throw runtime_error( "unknown time_precision #" + to_string( time_precision ) + " in meta_field_domain_type" );
      }
   }
   else if( primitive == 2 ) // i.e. date
   {
      switch( date_precision )
      {
         case 0:
         mask = "yyyy-mm-dd";
         break;

         case 1:
         mask = "mm-dd";
         break;

         case 2:
         mask = "yy-mm-dd";
         break;

         default:
         throw runtime_error( "unknown date_precision #" + to_string( date_precision ) + " in meta_field_domain_type" );
      }
   }
   else if( primitive == 3 ) // i.e. time
   {
      switch( time_precision )
      {
         case 0:
         mask = "hh:mm:ss.tht";
         break;

         case 1:
         mask = "hh:mm";
         break;

         case 2:
         mask = "hh:mm:ss";
         break;

         default:
         throw runtime_error( "unknown time_precision #" + to_string( time_precision ) + " in meta_field_domain_type" );
      }
   }

   if( enum_id.empty( ) && ( primitive == 4 || primitive == 5 ) ) // i.e. numeric or int
   {
      mask = get_mask( numeric_digits, numeric_decimals, ( primitive == 4 ), show_plus_sign, zero_padding );

      if( p_tmask )
         *p_tmask = get_mask( numeric_digits == 0 ? 0 : numeric_digits + 4,
          numeric_decimals, ( primitive == 4 ), show_plus_sign, zero_padding );
   }

   return domain_type;
}

string meta_field_extras( int uom, const string& uom_name,
 int extra, bool transient, int max_size, const string& enum_id, int primitive,
 const string& min_value, const string& max_value, int numeric_digits, int numeric_decimals,
 int string_domain, int date_precision, int time_precision, bool show_plus_sign, int zero_padding,
 int int_type, int numeric_type )
{
   vector< string > all_extras;

   switch( extra )
   {
      case -5:
      all_extras.push_back( "modify_user_key" );
      break;

      case -4:
      all_extras.push_back( "create_user_key" );
      break;

      case -3:
      all_extras.push_back( "user_other" );
      break;

      case -2:
      all_extras.push_back( "user_group" );
      break;

      case -1:
      all_extras.push_back( "owner" );
      break;

      case 0:
      break;

      case 1:
      all_extras.push_back( "file" );
      break;

      case 2:
      all_extras.push_back( "href" );
      break;

      case 3:
      all_extras.push_back( "image" );
      break;

      case 4:
      all_extras.push_back( "notes" );
      break;

      case 5:
      all_extras.push_back( "order" );
      break;

      case 6:
      all_extras.push_back( "active" );
      break;

      case 7:
      all_extras.push_back( "mailto" );
      break;

      case 8:
      all_extras.push_back( "actions" );
      break;

      case 9:
      all_extras.push_back( "html" );
      break;

      case 10:
      all_extras.push_back( "password" );
      break;

      case 11:
      all_extras.push_back( "defcurrent" );
      break;

      case 12:
      all_extras.push_back( "user_perms" );
      break;

      case 13:
      all_extras.push_back( "manual_link" );
      break;

      case 14:
      all_extras.push_back( "defcurrentyear" );
      break;

      case 15:
      all_extras.push_back( "create_datetime" );
      break;

      case 16:
      all_extras.push_back( "modify_datetime" );
      break;

      case 17:
      all_extras.push_back( "permission" );
      break;

      case 18:
      all_extras.push_back( "security_level" );
      break;

      case 19:
      all_extras.push_back( "replace_underbars" );
      break;

      case 20:
      all_extras.push_back( "upper" );
      break;

      case 21:
      all_extras.push_back( "flink" );
      break;

      case 22:
      all_extras.push_back( "filename" );
      break;

      case 23:
      all_extras.push_back( "text" );
      break;

      case 24:
      all_extras.push_back( "orientation" );
      break;

      case 25:
      all_extras.push_back( "encrypted" );
      break;

      case 26:
      all_extras.push_back( "hpassword" );
      break;

      case 27:
      all_extras.push_back( "hpassword_salt" );
      break;

      case 28:
      all_extras.push_back( "is_effective_owner" );
      break;

      case 29:
      all_extras.push_back( "qr_code" );
      break;

      default:
      throw runtime_error( "unexpected field extra #" + to_string( extra ) + " in meta_field_extras" );
   }

   if( uom == 999 )
      all_extras.push_back( "uom=uom_" + lower( uom_name ) );
   else if( uom > 0 )
      all_extras.push_back( "uom=" + meta_field_uom( uom ) );

   switch( string_domain )
   {
      case 0:
      break;

      case 1:
      all_extras.push_back( "format=identifier" );
      break;

      case 2:
      all_extras.push_back( "format=label" );
      break;

      case 3:
      all_extras.push_back( "format=filename" );
      break;

      default:
      throw runtime_error( "unknown string_domain #" + to_string( string_domain ) + " in meta_field_extras" );
   }

   switch( date_precision )
   {
      case 0:
      break;

      case 1:
      all_extras.push_back( "date_precision=months" );
      break;

      case 2:
      all_extras.push_back( "date_precision=decades" );
      break;

      default:
      throw runtime_error( "unknown date_precision #" + to_string( date_precision ) + " in meta_field_extras" );
   }

   switch( time_precision )
   {
      case 0:
      break;

      case 1:
      all_extras.push_back( "time_precision=minutes" );
      break;

      case 2:
      all_extras.push_back( "time_precision=seconds" );
      break;

      default:
      throw runtime_error( "unknown time_precision #" + to_string( time_precision ) + " in meta_field_extras" );
   }

   if( !enum_id.empty( ) )
      all_extras.push_back( "enum=" + enum_id );

   if( max_size > 0 )
      all_extras.push_back( "max_size=" + to_string( max_size ) );

   if( !min_value.empty( ) )
      all_extras.push_back( "range=" + min_value + ".." + max_value );

   if( numeric_digits > 0 )
      all_extras.push_back( "format=" + to_string( numeric_digits ) + '.' + to_string( numeric_decimals ) );

   if( enum_id.empty( ) && ( primitive == 4 || primitive == 5 ) ) // i.e. numeric or int
   {
      all_extras.push_back( "mask="
       + get_mask( numeric_digits, numeric_decimals, ( primitive == 4 ), show_plus_sign, zero_padding ) );

      all_extras.push_back( "pmask="
       + get_mask( numeric_digits == 0 ? 0 : numeric_digits + 4,
       numeric_decimals, ( primitive == 4 ), show_plus_sign, zero_padding ) );
   }

   switch( int_type )
   {
      case 0:
      break;

      case 1:
      all_extras.push_back( "int_type=duration_dhm" );
      break;

      case 2:
      all_extras.push_back( "int_type=duration_hms" );
      break;

      case 3:
      all_extras.push_back( "int_type=bytes" );
      break;

      default:
      throw runtime_error( "unknown int_type #" + to_string( int_type ) + " in meta_field_extras" );
   }

   switch( numeric_type )
   {
      case 0:
      break;

      case 1:
      all_extras.push_back( "numeric_type=bytes" );
      break;

      default:
      throw runtime_error( "unknown numeric_type #" + to_string( numeric_type ) + " in meta_field_extras" );
   }

   if( transient )
      all_extras.push_back( "transient" );

   return join( all_extras, '+' );
}

string meta_procedure_arg_type( int primitive )
{
   string type;

   switch( primitive )
   {
      case 0:
      type = "string";
      break;

      case 1:
      type = "date_time";
      break;

      case 2:
      type = "udate";
      break;

      case 3:
      type = "mtime";
      break;

      case 4:
      type = "numeric";
      break;

      case 5:
      type = "int";
      break;

      case 6:
      type = "bool";
      break;

      default:
      throw runtime_error( "unknown primitive #" + to_string( primitive ) + " in meta_primitive_type" );
   }

   return type;
}


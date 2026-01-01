// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "fcgi_list.h"

#include "base64.h"
#include "format.h"
#include "numeric.h"
#include "date_time.h"
#include "fcgi_info.h"
#include "utilities.h"
#include "ciyam_core.h"
#include "fcgi_utils.h"
#include "ciyam_interface.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

const size_t c_default_search_opt_limit = 3;

const char* const c_gt = "&gt;";
const char* const c_lt = "&lt;";
const char* const c_nbsp = "&nbsp;";
const char* const c_dbl_nbsp = "&nbsp;&nbsp;";

const char* const c_dummy_check_symbol = ": :";

string g_nbsp( c_nbsp );

inline const string& data_or_nbsp( const string& input )
{
   if( !input.empty( ) )
      return input;
   else
      return g_nbsp;
}

inline string format_int_value( int i, const string& mask )
{
   return format_numeric( i, mask );
}

inline string format_numeric_value( const numeric& n, const string& mask )
{
   return format_numeric( n, mask );
}

void append_hash_values_query_update( ostream& os,
 const string& sel_id, bool has_text_search, const string& source_id, const string& value_name,
 const map< string, string >& list_selections, const session_info& sess_info, const string* p_findinfo_and_listsrch = 0 )
{
   os << "var hash_values = '" << sess_info.hashval_prefix << "'; ";

   if( p_findinfo_and_listsrch )
      os << "hash_values += '" << *p_findinfo_and_listsrch << "'; ";
   else
   {
      os << "hash_values += get_all_search_values( document." << source_id << ", true ); ";

      if( has_text_search )
         os << "hash_values += document." << source_id << ".text_search.value; ";
   }

   string name, other_values;

   for( int i = 0; i < 10; i++ )
   {
      name = c_list_prefix;
      name += c_prnt_suffix;
      name += to_string( i );

      if( name == sel_id )
         os << "hash_values += " << value_name << "; ";
      else if( list_selections.count( name ) )
         os << "hash_values += '" << list_selections.find( name )->second << "'; ";

      name = c_list_prefix;
      name += c_rest_suffix;
      name += to_string( i );

      if( name == sel_id )
         os << "hash_values += " << value_name << "; ";
      else if( list_selections.count( name ) )
         os << "hash_values += '" << list_selections.find( name )->second << "'; ";
   }

#ifdef DEBUG
   os << "alert( 'hash_values = ' + hash_values );";
#endif

   os << "query_update( '" << c_param_hashval << "', hex_sha1( hash_values ).substring( 16, 24 ), true ); ";
}

bool has_access( const string& extra, const session_info& sess_info, bool has_owner_parent, uint64_t parent_state )
{
   bool rc = true;

   map< string, string > extras;

   if( !extra.empty( ) )
      parse_field_extra( extra, extras );

   if( extras.count( c_list_field_extra_pstate ) )
   {
      istringstream isstr( extras[ c_list_field_extra_pstate ] );

      uint64_t flag;
      isstr >> hex >> flag;

      if( !( parent_state & flag ) )
         rc = false;
   }

   if( rc && extras.count( c_field_extra_hidden )
    && has_perm_extra( c_field_extra_hidden, extras, sess_info ) )
      rc = false;

   if( rc && extras.count( c_field_extra_no_anon ) && sess_info.user_id.empty( ) )
      rc = false;

   if( rc && extras.count( c_field_extra_owner_only )
    && !has_owner_parent && has_perm_extra( c_field_extra_owner_only, extras, sess_info ) )
      rc = false;

   if( rc && extras.count( c_field_extra_admin_only )
    && ( !sess_info.is_admin_user && has_perm_extra( c_field_extra_admin_only, extras, sess_info ) ) )
      rc = false;

   if( rc && extras.count( c_field_extra_admin_owner_only )
    && ( !has_owner_parent && !sess_info.is_admin_user && has_perm_extra( c_field_extra_admin_owner_only, extras, sess_info ) ) )
      rc = false;

   return rc;
}

void replace_with_search_matches_highlighted( string& cell_data,
 const string& text_search_value, const map< string, string >& extras, bool is_text_only = false )
{
   string prefix, suffix( "</span>" );

   if( extras.count( c_list_type_extra_text_highlight ) )
      prefix = "<span class = \"text_highlight\">";
   else
      prefix = "<span class = \"text_highlight1\">";

   vector< pair< bool, string > > parts;

   if( is_text_only )
      parts.push_back( make_pair( false, cell_data ) );
   else
   {
      bool in_tag = false;

      string next_string;

      size_t num_chars = cell_data.size( );

      // NOTE: Separate tags from text so that
      // matches will not occur within a tag.
      for( size_t i = 0; i < num_chars; i++ )
      {
         char ch = cell_data[ i ];

         if( ch == '>' )
         {
            next_string += ch;

            parts.push_back( make_pair( in_tag, next_string ) );

            in_tag = false;

            next_string.erase( );
         }
         else if( ch == '<' )
         {
            if( !next_string.empty( ) )
               parts.push_back( make_pair( in_tag, next_string ) );

            in_tag = true;

            next_string = ch;
         }
         else
            next_string += ch;
      }

      if( !next_string.empty( ) )
         parts.push_back( make_pair( in_tag, next_string ) );
   }

   cell_data.erase( );

   size_t num_parts = parts.size( );

   string search_lower( lower( text_search_value ) );

   for( size_t i = 0; i < num_parts; i++ )
   {
      bool next_is_tag = parts[ i ].first;

      string next_string( parts[ i ].second );

      if( next_is_tag )
      {
         cell_data += next_string;
         continue;
      }

      string next_lower( lower( next_string ) );

      // NOTE: The matching is case insensitive but will preserve
      // the original text for what will actually be displayed.
      while( true )
      {
         string::size_type pos = next_lower.find( search_lower );

         if( pos == string::npos )
         {
            cell_data += next_string;
            break;
         }

         string original( next_string.substr( pos, search_lower.size( ) ) );

         cell_data += next_string.substr( 0, pos );
         cell_data += prefix + original + suffix;

         next_lower.erase( 0, pos + search_lower.size( ) );
         next_string.erase( 0, pos + search_lower.size( ) );
      }
   }
}

}

void setup_list_fields( list_source& list,
 const string& pkey, const string& module_name, const session_info& sess_info, bool is_printable )
{
   const module_info& mod_info( *get_storage_info( ).modules_index.find( module_name )->second );

   list.sid = ( list.lici->second )->id;
   list.cid = ( list.lici->second )->cid;
   list.type = ( list.lici->second )->type;
   list.perm = ( list.lici->second )->perm;
   list.name = ( list.lici->second )->name;
   list.view = ( list.lici->second )->view;
   list.pfield = ( list.lici->second )->pfield;
   list.mclass = ( list.lici->second )->mclass;
   list.bclass = ( list.lici->second )->bclass;
   list.module = ( list.lici->second )->module;

   list.pdf_spec_name = ( list.lici->second )->pdf_spec;

   if( !pkey.empty( ) )
   {
      list.new_pkey = pkey;
      list.new_pfield = ( list.lici->second )->pfield;
   }

   map< string, size_t > field_id_counts;

   for( size_t i = 0; i < ( list.lici->second )->fields.size( ); i++ )
   {
      fld_info fld( ( list.lici->second )->fields[ i ] );

      if( !field_id_counts.count( fld.field ) )
         field_id_counts[ fld.field ] = 0;
      else
         field_id_counts[ fld.field ] = field_id_counts[ fld.field ] + 1;
   }

   size_t index_count = 0;

   bool allow_sorting = true;
   bool check_for_indexed = false;

   for( size_t i = 0; i < ( list.lici->second )->fields.size( ); i++ )
   {
      fld_info fld( ( list.lici->second )->fields[ i ] );

      bool is_hidden = false;
      bool child_always = false;
      bool non_prefixed = false;

      string field_id( fld.field );

      string value_id( field_id );

      if( !fld.pfield.empty( ) )
         value_id += "." + fld.pfield;

      if( field_id != c_key_field && field_id != c_row_field )
      {
         map< string, string > extra_data;

         if( !fld.extra.empty( ) )
            parse_field_extra( fld.extra, extra_data );

         if( !list.field_list.empty( ) )
            list.field_list += ",";

         if( !list.pfield_list.empty( ) )
            list.pfield_list += ",";

         if( is_printable
          && ( extra_data.count( c_field_extra_print_summary )
          || extra_data.count( c_field_extra_print_only_summary ) ) )
            list.pfield_list += "!";

         list.field_list += field_id;
         list.pfield_list += field_id;

         // NOTE: The first non-summary field will be used for ordering (if indexed).
         if( !check_for_indexed
          && !extra_data.count( c_field_extra_print_summary )
          && !extra_data.count( c_field_extra_print_only_summary ) )
         {
            if( fld.indexed )
            {
               check_for_indexed = true;

               list.sort_field = 1;
               list.unique_index = fld.unique;
               list.first_index_field = field_id;
            }
         }

         if( extra_data.count( c_field_extra_html ) )
            list.html_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_text ) )
            list.text_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_notes ) )
            list.notes_fields.insert( value_id );
         else if( extra_data.count( c_list_type_extra_actions ) )
            ( list.lici->second )->extras.insert( make_pair( c_list_type_extra_actions, field_id ) );

         if( extra_data.count( c_field_extra_url ) )
            list.url_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_href ) )
            list.href_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_file ) || extra_data.count( c_field_extra_flink ) )
         {
            if( !sess_info.user_id.empty( ) || get_storage_info( ).embed_images )
               list.file_fields.insert( value_id );
            else
               list.hidden_fields.insert( value_id );
         }
         else if( extra_data.count( c_field_extra_image ) )
         {
            if( !sess_info.user_id.empty( ) || get_storage_info( ).embed_images )
               list.image_fields.insert( value_id );
            else
               list.hidden_fields.insert( value_id );
         }
         else if( extra_data.count( c_field_extra_mailto ) )
            list.mailto_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_qr_code ) )
            list.qr_code_fields.insert( value_id );

         if( extra_data.count( c_field_extra_special ) )
            list.special_field = field_id;

         if( extra_data.count( c_field_extra_filename ) )
            list.filename_field = field_id;

         if( extra_data.count( c_field_extra_uom ) )
            list.uom_fields.insert( make_pair( value_id, extra_data[ c_field_extra_uom ] ) );

         if( extra_data.count( c_field_extra_enum ) )
            list.enum_fields.insert( make_pair( value_id, extra_data[ c_field_extra_enum ] ) );

         if( extra_data.count( c_field_extra_mask ) )
            list.mask_fields.insert( make_pair( value_id, extra_data[ c_field_extra_mask ] ) );

         if( extra_data.count( c_field_extra_pmask ) )
            list.pmask_fields.insert( make_pair( value_id, extra_data[ c_field_extra_pmask ] ) );

         is_hidden = has_perm_extra( c_field_extra_hidden, extra_data, sess_info );

         if( sess_info.user_id.empty( ) && extra_data.count( c_field_extra_no_anon ) )
            is_hidden = true;

         if( extra_data.count( c_field_extra_admin_only )
          && ( !sess_info.is_admin_user && has_perm_extra( c_field_extra_admin_only, extra_data, sess_info ) ) )
            is_hidden = true;

         if( is_hidden )
         {
            list.hidden_fields.insert( value_id );

            if( !index_count && fld.indexed )
               index_count = fld.index_count;

            // NOTE: If the first field was hidden and indexed then user selectable ordering will not
            // be permitted (as it is being assumed that this field is being used for fixed ordering).
            if( i == 0 && fld.indexed && ( fld.index_count == 1 ) )
               allow_sorting = false;
         }

         if( extra_data.count( c_list_field_extra_pstate ) )
         {
            istringstream isstr( extra_data[ c_list_field_extra_pstate ] );

            uint64_t flag;
            isstr >> hex >> flag;

            list.pstate_fields.insert( make_pair( value_id, flag ) );
         }

         if( has_perm_extra( c_field_extra_owner_only, extra_data, sess_info ) )
            list.owner_fields.insert( value_id );

         if( extra_data.count( c_field_extra_admin_owner_only ) )
         {
            if( !sess_info.is_admin_user
             && has_perm_extra( c_field_extra_admin_owner_only, extra_data, sess_info ) )
               list.owner_fields.insert( value_id );
         }

         if( has_perm_extra( c_field_extra_link, extra_data, sess_info ) )
            list.link_fields.insert( value_id );

         if( has_perm_extra( c_field_extra_owner_link, extra_data, sess_info ) )
         {
            list.link_fields.insert( value_id );
            list.owner_link_fields.insert( value_id );
         }

         if( extra_data.count( c_field_extra_admin_link ) )
         {
            list.link_fields.insert( value_id );

            if( !sess_info.is_admin_user
             && has_perm_extra( c_field_extra_admin_link, extra_data, sess_info ) )
               list.admin_link_fields.insert( value_id );
         }

         if( extra_data.count( c_field_extra_admin_owner_link ) )
         {
            list.link_fields.insert( value_id );

            if( !sess_info.is_admin_user
             && has_perm_extra( c_field_extra_admin_owner_link, extra_data, sess_info ) )
               list.owner_link_fields.insert( value_id );
         }

         if( has_perm_extra( c_field_extra_non_link, extra_data, sess_info ) )
            list.non_link_fields.insert( value_id );

         if( extra_data.count( c_field_extra_non_fk_link ) )
            list.non_fk_link_fields.insert( value_id );

         if( extra_data.count( c_field_extra_manual_link ) )
            list.manual_link_fields.insert( value_id );

         if( extra_data.count( c_field_extra_permission ) )
            list.permission_field = field_id;

         if( extra_data.count( c_field_extra_orientation ) )
            list.orientation_field = field_id;

         if( extra_data.count( c_field_extra_security_level ) )
            list.security_level_field = field_id;

         if( extra_data.count( c_field_extra_ignore_encrypted ) )
            list.ignore_encrypted_field = field_id;

         if( extra_data.count( c_field_extra_int_type ) )
            list.int_type_fields.insert( make_pair( value_id, extra_data[ c_field_extra_int_type ] ) );

         if( extra_data.count( c_field_extra_numeric_type ) )
            list.numeric_type_fields.insert( make_pair( value_id, extra_data[ c_field_extra_numeric_type ] ) );

         if( extra_data.count( c_field_extra_date_precision ) )
            list.date_precision_fields.insert( make_pair( value_id, extra_data[ c_field_extra_date_precision ] ) );

         if( extra_data.count( c_field_extra_time_precision ) )
            list.time_precision_fields.insert( make_pair( value_id, extra_data[ c_field_extra_time_precision ] ) );

         if( extra_data.count( c_field_extra_text_search_title ) )
            list.text_search_title_fields.push_back( get_display_string( fld.name ) );

         if( extra_data.count( c_field_extra_replace_underbars ) )
            list.replace_underbar_fields.insert( value_id );

         if( fld.ftype == c_field_type_bool )
            list.bool_fields.insert( value_id );
         else if( ( fld.ftype == c_field_type_date )
          || ( fld.ftype == c_field_type_tdate ) || ( fld.ftype == c_field_type_udate ) )
            list.date_fields.insert( value_id );
         else if( ( fld.ftype == c_field_type_time )
          || ( fld.ftype == c_field_type_mtime ) || ( fld.ftype == c_field_type_ttime ) )
            list.time_fields.insert( value_id );
         else if( ( fld.ftype == c_field_type_datetime )
          || ( fld.ftype == c_field_type_tdatetime ) || ( fld.ftype == c_field_type_date_time ) )
            list.datetime_fields.insert( value_id );
         else if( fld.ftype == c_field_type_int )
            list.int_fields.insert( value_id );
         else if( fld.ftype == c_field_type_numeric )
            list.numeric_fields.insert( value_id );

         if( !fld.pclass.empty( ) )
         {
            // NOTE: If the field is hidden then don't add the parent field in order to
            // help prevent the application server from performing unnecessary queries.
            if( !is_hidden )
            {
               list.fk_field_ids.push_back( field_id );
               list.fk_field_classes[ field_id ] = fld.pclass;
            }
         }

         if( !fld.pfield.empty( ) )
         {
            // NOTE: see above...
            if( !is_hidden )
            {
               list.field_list += "." + fld.pfield;
               list.pfield_list += "." + fld.pfield;
            }
         }
         else if( fld.indexed
          && !extra_data.count( c_field_extra_print_summary )
          && !extra_data.count( c_field_extra_print_only_summary ) )
         {
            if( !is_hidden && allow_sorting )
               list.sort_fields.insert( field_id );

            string field_ids;

            for( int j = 0; j < fld.index_count; j++ )
            {
               if( i + j >= ( list.lici->second )->fields.size( ) )
                  break;

               if( !field_ids.empty( ) )
                  field_ids += ',';

               field_ids += ( list.lici->second )->fields[ i + j ].field;
            }

            list.index_fields.push_back( make_pair( field_ids, fld.unique ) );
         }

         // NOTE: If an index begins with one or more hidden fields
         // then uses the first non-hidden field as the sort field.
         if( index_count )
         {
            --index_count;

            if( !is_hidden && allow_sorting )
            {
               index_count = 0;
               list.sort_fields.insert( field_id );
            }
         }

         if( extra_data.count( c_field_extra_encrypted ) )
            list.encrypted_fields.insert( value_id );

         if( extra_data.count( c_field_extra_non_print ) )
            list.non_print_fields.insert( value_id );

         if( extra_data.count( c_field_extra_print_only ) )
            list.print_only_fields.insert( value_id );

         if( extra_data.count( c_field_extra_print_total ) )
            list.print_total_fields.insert( value_id );

         if( extra_data.count( c_field_extra_left ) )
            list.force_left_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_center ) )
            list.force_center_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_justify ) )
            list.force_justify_fields.insert( value_id );
         else if( !extra_data.count( c_field_extra_enum )
          && ( extra_data.count( c_field_extra_right )
          || ( ( fld.ftype == c_field_type_int ) || ( fld.ftype == c_field_type_numeric ) ) ) )
            list.force_right_fields.insert( value_id );

         if( extra_data.count( c_field_extra_large ) )
            list.large_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_larger ) )
            list.larger_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_small ) )
            list.small_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_smaller ) )
            list.smaller_fields.insert( value_id );

         if( extra_data.count( c_list_field_extra_trunc ) )
            list.notes_character_trunc.insert(
             make_pair( value_id, atoi( extra_data[ c_list_field_extra_trunc ].c_str( ) ) ) );

         if( extra_data.count( c_list_field_extra_omit_label ) )
            list.omit_label_fields.insert( value_id );
         else if( extra_data.count( c_list_field_extra_use_list_title ) )
            list.use_list_title_fields.insert( value_id );

         if( extra_data.count( c_field_extra_prefix_special ) )
            list.special_prefixed_fields.insert( value_id );

         if( extra_data.count( c_field_extra_suffix_special ) )
            list.special_suffixed_fields.insert( value_id );

         if( extra_data.count( c_field_extra_child_always ) )
            child_always = true;

         if( extra_data.count( c_field_extra_non_prefixed ) )
            non_prefixed = true;

         if( extra_data.count( c_field_extra_print_only_total ) )
         {
            list.print_only_fields.insert( value_id );
            list.print_total_fields.insert( value_id );
         }

         if( is_printable && extra_data.count( c_field_extra_print_summary ) )
         {
            list.print_only_fields.insert( value_id );

            list.print_summary_fields.insert( value_id );
            list.print_summary_field_ids.push_back( field_id );
         }

         if( extra_data.count( c_field_extra_print_only_summary ) )
         {
            list.print_only_fields.insert( value_id );

            list.print_summary_fields.insert( value_id );
            list.print_summary_field_ids.push_back( field_id );
         }
      }

      list.field_ids.push_back( field_id );
      list.value_ids.push_back( value_id );

      if( !is_hidden )
      {
         string display_name( get_display_string( fld.name ) );

         if( child_always || ( field_id_counts[ field_id ] > 0 ) )
         {
            if( child_always || non_prefixed )
               display_name = get_display_string( fld.pfname );
            else
               display_name += " " + get_display_string( fld.pfname );
         }

         list.display_names.push_back( display_name );
      }
   }

   // NOTE: Append all foreign keys to the field list.
   for( size_t i = 0; i < list.fk_field_ids.size( ); i++ )
   {
      list.field_list += "," + list.fk_field_ids[ i ];
      list.pfield_list += "," + list.fk_field_ids[ i ];
   }
}

void output_list_form( ostream& os,
 const list_source& source, const string& session_id,
 const string& user_select_key, const string& error_message,
 bool is_printable, bool using_session_cookie, bool allow_list_actions,
 bool is_parent_edit, const map< string, string >& list_selections,
 const map< string, string >& list_search_text, const map< string, string >& list_search_values,
 uint64_t parent_state, bool is_child_list, const string& parent_key, bool keep_checks,
 const string& pident, const string& oident, const session_info& sess_info, string& extra_content_func,
 const set< string >& specials, bool use_url_checksum, const string& qlink,
 const string& findinfo_and_listsrch, const set< string >& selected_records, bool embed_images,
 bool has_hashval, bool has_owner_parent, bool& has_any_changing, int back_count, const string* p_pdf_file_name )
{
   string list_type( source.lici->second->type );

   const storage_info& sinfo( get_storage_info( ) );

   const map< string, string >& extras( source.lici->second->extras );

   const module_info& mod_info( *sinfo.modules_index.find( source.module )->second );

   set< string > print_list_opts;

   if( !sess_info.print_list_opts.empty( ) )
      split( sess_info.print_list_opts, print_list_opts, '+' );

   bool allow_new_record = true;

   if( is_parent_edit || sess_info.is_read_only || sess_info.user_id.empty( ) )
      allow_list_actions = false;

   bool is_no_new = has_perm_extra( c_list_type_extra_no_new, extras, sess_info );
   bool is_no_erase = has_perm_extra( c_list_type_extra_no_erase, extras, sess_info );
   bool is_owner_new = has_perm_extra( c_list_type_extra_owner_new, extras, sess_info );
   bool is_admin_new = has_perm_extra( c_list_type_extra_admin_new, extras, sess_info );
   bool is_admin_owner_new = has_perm_extra( c_list_type_extra_admin_owner_new, extras, sess_info );
   bool is_owner_erase = has_perm_extra( c_list_type_extra_owner_erase, extras, sess_info );
   bool is_admin_erase = has_perm_extra( c_list_type_extra_admin_erase, extras, sess_info );
   bool is_admin_owner_erase = has_perm_extra( c_list_type_extra_admin_owner_erase, extras, sess_info );

   bool ignore_parent_state = false;

   if( extras.count( c_list_type_extra_ignore_parent_state ) )
      ignore_parent_state = true;

   if( is_no_new
    || !allow_list_actions
    || sess_info.is_read_only
    || sess_info.user_id.empty( )
    || ( is_admin_new && !sess_info.is_admin_user )
    || ( is_owner_new && is_child_list && !has_owner_parent )
    || source.view.empty( ) || ( ( parent_state & c_state_uneditable )
    && ( !ignore_parent_state || !( parent_state & c_state_ignore_uneditable ) ) )
    || ( is_admin_owner_new && !( has_owner_parent || sess_info.is_admin_user ) ) )
      allow_new_record = false;

   bool has_owner_parent_or_is_user_list = ( has_owner_parent || ( list_type == c_list_type_user ) );

   if( extras.count( c_list_type_extra_cpstate ) )
   {
      istringstream isstr( extras.find( c_list_type_extra_cpstate )->second );

      uint64_t flag;
      isstr >> hex >> flag;

      if( !( parent_state & flag ) )
         allow_new_record = false;
   }

   string image_width( to_string( sess_info.image_width ) );
   string image_height( to_string( sess_info.image_height ) );

   if( p_pdf_file_name && !p_pdf_file_name->empty( ) )
      os << "<p class=\"screen\"><a href=\"" << *p_pdf_file_name << "\" target=\"_blank\">"
      "<img src=\"pdf_icon.gif\" border=\"0\">" << c_nbsp << GDS( c_display_pdf_version ) << "</a></p>";

   if( !is_printable && !qlink.empty( ) )
   {
      int qlink_num = atoi( qlink.c_str( ) );

      string columns( sess_info.quick_link_data[ qlink_num ].second );

      vector< string > column_info;

      raw_split( columns, column_info );

      if( column_info.size( ) != 3 )
         throw runtime_error( "unexpected incorrect # columns for quick link info" );

      os << "<p align=\"center\"><a href=\"" << column_info[ 0 ];

      if( use_url_checksum )
      {
         string::size_type pos = column_info[ 2 ].find( '+' );
         if( pos == string::npos )
            throw runtime_error( "unexpected quick link data format" );

         string checksum_values( string( c_cmd_list ) + column_info[ 2 ].substr( 0, pos ) + user_select_key );

         os << "&" << c_param_chksum << "="
          << get_checksum( sess_info, checksum_values + c_hash_suffix );

         os << "&" << c_param_hashval
          << "=" << get_hash( sess_info.hashval_prefix + column_info[ 2 ].substr( pos + 1 ) );
      }

      os << "\"><i>" << GDS( c_display_show_search_criteria ) << "</i></a></p>\n";
   }

   if( is_child_list && !is_ui_prototype( ) )
   {
      os << "<table width=\"100%\">\n";
      os << "<tr><td class=\"selected_panel\">\n";
   }

   os << "<form name=\"" << source.id << "\" id=\"" << source.id << "\"";

   bool allow_quick_links = false;

   if( using_session_cookie && !sess_info.user_id.empty( ) && !has_any_changing
    && !mod_info.user_qlink_class_id.empty( ) && extras.count( c_list_type_extra_quick_link ) )
      allow_quick_links = true;

   size_t search_opt_limit = c_default_search_opt_limit;

   if( extras.count( c_list_type_extra_search_opt_limit ) )
   {
      search_opt_limit = atoi( extras.find( c_list_type_extra_search_opt_limit )->second.c_str( ) );

      if( search_opt_limit < 1 || search_opt_limit > 7 )
         throw runtime_error( "unexpected search_opt_limit value #" + to_string( search_opt_limit ) );
   }

   if( !is_printable && qlink.empty( ) )
   {
      string checksum_values;
      string new_checksum_value;

      if( use_url_checksum )
         checksum_values = string( c_cmd_list ) + oident + user_select_key;

      if( extras.count( c_list_type_extra_search ) )
      {
         os << " onsubmit=\"if( validate( " << source.id << " ) ) { query_update( '"
          << c_param_listextra << "', '" << c_list_extra_find << "', true ); ";

         if( extras.count( c_list_type_extra_text_search ) )
            os << "query_update( '" << source.id
             << c_srch_suffix << "', document." << source.id << ".text_search.value, true ); ";

         // NOTE: Remove next/prev page info for a new search.
         os << "query_update( '" << source.id << c_info_suffix << "', '', true ); ";

         if( use_url_checksum )
         {
            new_checksum_value = get_checksum( sess_info, checksum_values + c_hash_suffix );

            os << "query_update( '" << c_param_uselextra << "', '', true ); ";
            os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";

            append_hash_values_query_update( os, "",
             ( extras.count( c_list_type_extra_text_search ) > 0 ), source.id, "", list_selections, sess_info );
         }

         if( !allow_quick_links )
            os << "query_update( '" << c_param_findinfo
             << "', get_all_search_values( document." << source.id << ", true ) ); } return false;\"";
         else
         {
            os << "if( document." << source.id << ".quick_link.value == '' ) ";

            os << "query_update( '" << c_param_findinfo
             << "', get_all_search_values( document." << source.id << ", true ) );";

            os << " else { query_update( '" << c_param_findinfo
             << "', get_all_search_values( document." << source.id << ", true ), true ); ";

            os << "query_update( '" << c_param_qlink << "', '"
             << to_string( sess_info.quick_link_data.size( ) ) << "', true ); ";

            if( use_url_checksum )
            {
               new_checksum_value = get_checksum( sess_info,
                checksum_values + to_string( sess_info.checksum_serial ) + c_hash_suffix );

               os << "query_update( '" << c_param_uselextra << "', '', true ); ";
               os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
            }

            os << "dyn_load( null, 'act=" + to_string( c_act_qlink ) << "&" << to_string( c_param_quicklink )
             << "=' + encodeURIComponent( document." << source.id << ".quick_link.value ), false ); } } return false;\"";
         }
      }
      else if( extras.count( c_list_type_extra_text_search ) )
      {
         os << " onsubmit=\"";

         if( use_url_checksum )
         {
            new_checksum_value = get_checksum( sess_info, checksum_values + c_hash_suffix );

            os << "query_update( '" << c_param_uselextra << "', '', true ); ";
            os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";

            append_hash_values_query_update( os, "", true, source.id, "", list_selections, sess_info );
         }

         os << "query_update( '" << source.id
          << c_srch_suffix << "', document." << source.id << ".text_search.value ); return false;\"";
      }
   }

   os << ">\n";

   string text_search_value;

   if( !is_printable && qlink.empty( ) && extras.count( c_list_type_extra_search ) )
   {
      os << "<table name=\"" << source.id << "_search\" id=\"" << source.id << "_search\""
       << " class=\"list\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n";

      os << "<thead><tr>\n";

      os << "<th class=\"list\">" << GDS( c_display_search_field ) << "</th><th class=\"list\">"
       << GDS( c_display_search_type ) << "</th><th class=\"list\">" << GDS( c_display_search_values ) << "</th>";

      os << "\n</tr></thead>\n";

      os << "<tbody>\n";

      if( extras.count( c_list_type_extra_text_search ) )
      {
         os << "<tr>";

         if( source.text_search_title_fields.empty( ) )
            os << "<td class=\"list\">" << GDS( c_display_any_text_field ) << "</td>";
         else
         {
            string text_search_title;

            for( size_t j = 0; j < source.text_search_title_fields.size( ); j++ )
            {
               if( j > 0 )
               {
                  if( j == ( source.text_search_title_fields.size( ) - 1 ) )
                     text_search_title += GDS( c_display_or );
                  else
                     text_search_title += ", ";
               }

               text_search_title += source.text_search_title_fields[ j ];
            }

            os << "<td class=\"list\">" << text_search_title << "</td>";
         }

         os << "<td class=\"list\">" << GDS( c_display_contains ) << "</td>";

         os << "<td class=\"list\">";

         os << "<input type=\"text\" id=\"text_search\" size=\"25\"";

         if( list_search_text.count( source.id + c_srch_suffix ) )
         {
            text_search_value = list_search_text.find( source.id + c_srch_suffix )->second;

            os << " value=\"" << escape_markup( text_search_value ) << "\"";
         }

         os << "/>";

         os << "</td></tr>\n";
      }

      values skey_values;

      for( size_t i = 0; i < ( source.lici->second )->parents.size( ); i++ )
      {
         if( !has_access( ( source.lici->second )->parents[ i ].extra, sess_info, has_owner_parent, parent_state ) )
            continue;

         if( ( source.lici->second )->parents[ i ].operations.count( c_operation_select )
          || ( source.lici->second )->parents[ i ].operations.count( c_operation_select_child ) )
         {
            string op( c_operation_select );

            bool is_select_child = false;

            if( ( source.lici->second )->parents[ i ].operations.count( c_operation_select_child ) )
            {
               is_select_child = true;

               op = string( c_operation_select_child );
            }

            if( !( source.lici->second )->parents[ i ].skey.empty( ) )
            {
               string skey( ( source.lici->second )->parents[ i ].skey );

               if( ( skey == c_parent_extra_skey0 ) && skey_values.value0.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey1 ) && skey_values.value1.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey2 ) && skey_values.value2.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey3 ) && skey_values.value3.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey4 ) && skey_values.value4.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey5 ) && skey_values.value5.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey6 ) && skey_values.value6.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey7 ) && skey_values.value7.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey8 ) && skey_values.value8.empty( ) )
                  continue;

               if( ( skey == c_parent_extra_skey9 ) && skey_values.value9.empty( ) )
                  continue;
            }

            string suffix( ( source.lici->second )->parents[ i ].operations.find( op )->second );

            string::size_type pos = suffix.find( '!' );

            size_t next_search_opt_limit( search_opt_limit );

            string::size_type lpos = suffix.substr( 0, pos ).find( '#' );

            if( lpos != string::npos )
            {
               size_t specific_limit = atoi( suffix.substr( lpos + 1, pos - lpos - 1 ).c_str( ) );

               if( specific_limit < next_search_opt_limit )
                  next_search_opt_limit = specific_limit;

               pos = lpos;
            }

            string pextra_special;
            string* p_skey_value = 0;

            if( pos != string::npos && pos != 0 )
            {
               pextra_special = "@" + suffix.substr( 0, pos );

               if( pextra_special == c_parent_extra_skey0 )
                  p_skey_value = &skey_values.value0;
               else if( pextra_special == c_parent_extra_skey1 )
                  p_skey_value = &skey_values.value1;
               else if( pextra_special == c_parent_extra_skey2 )
                  p_skey_value = &skey_values.value2;
               else if( pextra_special == c_parent_extra_skey3 )
                  p_skey_value = &skey_values.value3;
               else if( pextra_special == c_parent_extra_skey4 )
                  p_skey_value = &skey_values.value4;
               else if( pextra_special == c_parent_extra_skey5 )
                  p_skey_value = &skey_values.value5;
               else if( pextra_special == c_parent_extra_skey6 )
                  p_skey_value = &skey_values.value6;
               else if( pextra_special == c_parent_extra_skey7 )
                  p_skey_value = &skey_values.value7;
               else if( pextra_special == c_parent_extra_skey8 )
                  p_skey_value = &skey_values.value8;
               else if( pextra_special == c_parent_extra_skey9 )
                  p_skey_value = &skey_values.value9;
               else
                  throw runtime_error( "unexpected select pextra_special '" + pextra_special.substr( 1 ) + "' found" );
            }

            bool is_unselected_skey_owner = false;

            map< string, string > extra_data;

            if( !( source.lici->second )->parents[ i ].extra.empty( ) )
               parse_field_extra( ( source.lici->second )->parents[ i ].extra, extra_data );

            os << "<tr>";

            string label_name( ( source.lici->second )->parents[ i ].name );

            if( extra_data.count( c_list_field_extra_label_class ) )
               label_name = extra_data.find( c_list_field_extra_label_class )->second;

            os << "<td class=\"list\">" << get_display_string( label_name ) << "</td>";

            if( is_select_child )
               os << "<td class=\"list\">" << GDS( c_display_includes ) << "</td>";
            else
               os << "<td class=\"list\">" << GDS( c_display_is_equal_to ) << "</td>";

            os << "<td class=\"list\">";

            for( size_t j = 0; j < next_search_opt_limit; j++ )
            {
               if( j > 0 )
                  os << c_dbl_nbsp;

               char first_prefix = 'A';

               if( is_select_child )
                  first_prefix = 'H';

               char opt( first_prefix + j );

               string svname( opt + ( source.lici->second )->parents[ i ].field );

               string any_display( GDS( c_display_any ) );

               if( opt != first_prefix )
               {
                  if( !is_select_child )
                     os << GDS( c_display_or );
                  else
                     os << GDS( c_display_and );

                  os << c_dbl_nbsp;

                  any_display = GDS( c_display_choose );
               }

               string value;

               if( list_search_values.count( svname ) )
               {
                  value = list_search_values.find( svname )->second;

                  // NOTE: If an "skey" owner has changed then ignore any pre-existing "skey" user
                  // value as it is not applicable to the new "skey" owner.
                  for( set< string >::const_iterator ci = specials.begin( ); ci != specials.end( ); ++ci )
                  {
                     if( ( source.lici->second )->parents[ i ].pextra.find( *ci ) != string::npos )
                     {
                        value.erase( );
                        break;
                     }
                  }

                  if( p_skey_value )
                     *p_skey_value = value;
               }

               os << "<select name=\"search_" << svname << "\"";

               if( extra_data.count( c_field_extra_large ) || extra_data.count( c_field_extra_larger )
                || extra_data.count( c_field_extra_small ) || extra_data.count( c_field_extra_smaller ) )
               {
                  os << " class=\"";

                  if( extra_data.count( c_field_extra_large ) )
                     os << "large";
                  else if( extra_data.count( c_field_extra_larger ) )
                     os << "larger";
                  else if( extra_data.count( c_field_extra_small ) )
                     os << "small";
                  else if( extra_data.count( c_field_extra_smaller ) )
                     os << "smaller";

                 os << "\"";
               }

               // NOTE: If an "skey" owner is changed then need to know which one as any existing
               // "skey" user's selections are no longer applicable to be used for restrictions.
               os << " onchange=\"query_update( '"
                << c_param_special << "', '" << pextra_special << "', true ); return search.click( );\"";

               os << " onkeypress=\"return form_keys( event, search );\">\n";

               os << "<option value=\"\">" << c_lt << any_display << c_gt << c_dbl_nbsp << "</option>\n";

               size_t opt_count = 0;

               if( !is_select_child && !( source.lici->second )->parents[ i ].mandatory )
               {
                  ++opt_count;

                  os << "<option value=\"~\"";

                  if( value == "~" )
                     os << " selected";

                  os << ">" << c_lt << GDS( c_display_none ) << c_gt << c_dbl_nbsp << "</option>\n";
               }

               set< string > parent_extras;

               if( !( source.lici->second )->parents[ i ].pextra.empty( ) )
                  split( ( source.lici->second )->parents[ i ].pextra, parent_extras, '+' );

               const data_container& parent_row_data = source.parent_lists[ i ];

               for( size_t k = 0; k < parent_row_data.size( ); k++ )
               {
                  ++opt_count;

                  string key( parent_row_data[ k ].first );
                  string display( parent_row_data[ k ].second );

                  if( parent_extras.count( c_parent_extra_manuallink ) )
                  {
                     stringstream ss;

                     replace_links_and_output( display, "",
                      source.module, source.module_ref, ss, false, false, session_id,
                      sess_info, user_select_key, using_session_cookie, use_url_checksum );

                     display = ss.str( );
                  }

                  // NOTE: Remove parent version information as its not relevant for a select operation.
                  size_t pos = key.find( ' ' );

                  if( pos != string::npos )
                     key.erase( pos );

                  if( display.empty( ) )
                     display = key;

                  os << "<option value=\"" << key << "\"";

                  if( key == value )
                     os << " selected";

                  os << ">" << unescaped( display ) << c_dbl_nbsp << "</option>\n";
               }
               os << "</select>";

               // NOTE: If the parent is an "skey" owner then can only select the one value.
               if( !pextra_special.empty( ) )
               {
                  if( value.empty( ) )
                     is_unselected_skey_owner = true;
                  break;
               }

               int z = ( int )is_select_child;

               if( ( opt_count < 2 ) || ( ( opt_count == 2 ) && ( j == z ) )
                || ( ( opt_count == 3 ) && ( j == 1 + z ) ) || ( ( opt_count == 4 ) && ( j == 2 + z ) )
                || ( ( opt_count == 5 ) && ( j == 3 + z ) ) || ( ( opt_count == 6 ) && ( j == 4 + z ) )
                || ( ( opt_count == 7 ) && ( j == 5 + z ) ) || ( ( opt_count == 8 ) && ( j == 6 + z ) ) )
                  break;
            }

            // FUTURE: Rather than an asterisk probably should use an image with a text flyover explanation.
            if( is_unselected_skey_owner )
               os << c_dbl_nbsp << "*";

            os << "</td></tr>\n";
         }
      }

      for( size_t i = 0; i < ( source.lici->second )->restricts.size( ); i++ )
      {
         if( !has_access( ( source.lici->second )->restricts[ i ].extra, sess_info, has_owner_parent, parent_state ) )
            continue;

         if( ( source.lici->second )->restricts[ i ].operations.count( c_operation_search ) )
         {
            string field_type( ( source.lici->second )->restricts[ i ].ftype );

            map< string, string > field_extras;

            if( !( source.lici->second )->restricts[ i ].extra.empty( ) )
               parse_field_extra( ( source.lici->second )->restricts[ i ].extra, field_extras );

            // NOTE: If the restrict search field is actually the security level and the user is
            // anonymous or only has "level 0" security then will not display the security level.
            if( field_extras.count( c_field_extra_enum )
             && field_extras.count( c_field_extra_security_level ) )
            {
               const enum_info& info( sinfo.enums.find( field_extras.find( c_field_extra_enum )->second )->second );

               if( sess_info.user_id.empty( ) || ( info.values[ 0 ].first == sess_info.user_slevel ) )
                  continue;
            }

            os << "<tr>";

            os << "<td class=\"list\">"
             << get_display_string( ( source.lici->second )->restricts[ i ].name ) << "</td>";

            char opt = '\0';

            if( !field_extras.count( c_field_extra_range ) )
            {
               if( !field_extras.count( c_field_extra_enum )
                && !field_extras.count( c_list_field_extra_exact_match )
                && ( source.lici->second )->restricts[ i ].ftype == "string" )
               {
                  opt = 'L';
                  os << "<td class=\"list\">" << GDS( c_display_contains ) << "</td>";
               }
               else
               {
                  if( !field_extras.count( c_field_extra_enum ) )
                     opt = 'V';
                  os << "<td class=\"list\">" << GDS( c_display_is_equal_to ) << "</td>";
               }
            }
            else
            {
               if( field_type == c_field_type_datetime
                || field_type == c_field_type_tdatetime || field_type == c_field_type_date_time )
                  opt = 'T';
               else
                  opt = 'N';

               if( !field_extras.count( c_list_field_extra_exact_match ) )
                  os << "<td class=\"list\">" << GDS( c_display_is_between ) << "</td>";
               else
                  os << "<td class=\"list\">" << GDS( c_display_is_equal_to ) << "</td>";
            }

            os << "<td class=\"list\">";

            string svname, value;

            if( field_extras.count( c_field_extra_enum ) )
            {
               for( size_t j = 0; j < search_opt_limit; j++ )
               {
                  if( j > 0 )
                     os << c_dbl_nbsp;

                  opt = 'A' + j;

                  svname = opt + ( source.lici->second )->restricts[ i ].field;

                  string any_display( GDS( c_display_any ) );

                  if( opt != 'A' )
                  {
                     os << GDS( c_display_or );
                     os << c_dbl_nbsp;
                     any_display = GDS( c_display_choose );
                  }

                  if( list_search_values.count( svname ) )
                     value = list_search_values.find( svname )->second;
                  else
                     value.erase( );

                  os << "<select name=\"search_" << svname << "\"";

                  if( field_extras.count( c_field_extra_large ) || field_extras.count( c_field_extra_larger )
                   || field_extras.count( c_field_extra_small ) || field_extras.count( c_field_extra_smaller ) )
                  {
                     os << " class=\"";

                     if( field_extras.count( c_field_extra_large ) )
                        os << "large";
                     else if( field_extras.count( c_field_extra_larger ) )
                        os << "larger";
                     else if( field_extras.count( c_field_extra_small ) )
                        os << "small";
                     else if( field_extras.count( c_field_extra_smaller ) )
                        os << "smaller";

                    os << "\"";
                  }

                  os << " onchange=\"return search.click( );\"";
                  os << " onkeypress=\"return form_keys( event, search );\">\n";

                  os << "<option value=\"\">" << c_lt << any_display << c_gt << c_dbl_nbsp << "</option>\n";

                  size_t opt_count = 0;

                  if( !field_extras.count( c_field_extra_security_level ) && !( source.lici->second )->restricts[ i ].mandatory )
                  {
                     ++opt_count;

                     os << "<option value=\"~\"";

                     if( value == "~" )
                        os << " selected";

                     os << ">" << c_lt << GDS( c_display_none ) << c_gt << c_dbl_nbsp << "</option>\n";
                  }

                  const enum_info& info( sinfo.enums.find( field_extras.find( c_field_extra_enum )->second )->second );

                  size_t value_offset = 0;

                  for( size_t k = 0; k < info.values.size( ); k++ )
                  {
                     if( info.values[ k ].first == value )
                     {
                        value_offset = k;
                        break;
                     }
                  }

                  for( size_t k = 0; k < info.values.size( ); k++ )
                  {
                     // NOTE: Enum values that start with a '-' are not included for user selection
                     // as they are deemed as being only applicable for internal application usage.
                     if( info.values[ k ].first[ 0 ] == '-' )
                        continue;

                     ++opt_count;

                     os << "<option";

                     if( !value.empty( ) && ( k == value_offset ) )
                        os << " selected";

                     os << " value=\"" << info.values[ k ].first << "\">"
                      << get_display_string( info.values[ k ].second ) << c_dbl_nbsp << "</option>\n";

                     // NOTE: Stop security enumeration at the user's limit.
                     if( field_extras.count( c_field_extra_security_level )
                      && ( info.values[ k ].first == sess_info.user_slevel ) )
                        break;
                  }

                  os << "</select>";

                  if( ( opt_count <= 2 ) || ( ( opt_count == 3 ) && ( j == 1 ) )
                   || ( ( opt_count == 4 ) && ( j == 2 ) ) || ( ( opt_count == 5 ) && ( j == 3 ) )
                   || ( ( opt_count == 6 ) && ( j == 4 ) ) || ( ( opt_count == 7 ) && ( j == 5 ) ) )
                     break;
               }
            }
            else
            {
               svname = opt + ( source.lici->second )->restricts[ i ].field;

               if( list_search_values.count( svname ) )
                  value = list_search_values.find( svname )->second;
               else
                  value.erase( );

               if( field_type == c_field_type_bool )
               {
                  os << "<select id=\"search_" << svname << "\" name=\"search_" << svname << "\"";

                  os << " onchange=\"return search.click( );\"";
                  os << " onkeypress=\"return form_keys( event, search );\">\n";

                  os << "<option value=\"\">" << c_lt << GDS( c_display_any ) << c_gt << c_dbl_nbsp << "</option>\n";

                  os << "<option value=\"0\"";

                  if( value == c_false_value )
                     os << " selected";

                  os << ">" << GDS( c_display_false ) << c_dbl_nbsp << "</option>\n";

                  os << "<option value=\"1\"";

                  if( value == c_true_value )
                     os << " selected";

                  os << ">" << GDS( c_display_true ) << c_dbl_nbsp << "</option>\n";

                  os << "</select>";
               }
               else
               {
                  int input_size = 25;
                  int max_length = 100;

                  bool is_datetime = false;

                  string extra, validate, use_time( "false" ), use_secs( "true" );

                  string range_min, range_max, range_extra;

                  if( field_extras.count( c_field_extra_range ) )
                  {
                     string::size_type pos = field_extras[ c_field_extra_range ].find( ".." );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected range format" );

                     range_min = field_extras[ c_field_extra_range ].substr( 0, pos );
                     range_max = field_extras[ c_field_extra_range ].substr( pos + 2 );

                     range_extra = ", minDate: new Date('" + range_min + "'), maxDate: new Date('" + range_max + "')";
                  }

                  if( field_type == c_field_type_date || field_type == c_field_type_tdate || field_type == c_field_type_udate )
                  {
                     input_size = 11;
                     max_length = 10;

                     is_datetime = true;

                     validate = "datetime";

                     string date_precision;

                     if( field_extras.count( c_field_extra_date_precision ) )
                        date_precision = field_extras[ c_field_extra_date_precision ];

                     if( date_precision == "months" )
                     {
                        input_size = 6;
                        max_length = 5;
                     }
                     else if( date_precision == "decades" )
                     {
                        input_size = 9;
                        max_length = 8;
                     }

                     if( !value.empty( ) )
                     {
                        udate ud( value );
                        value = format_date( ud, date_precision.c_str( ) );
                     }
                  }
                  else if( ( field_type == c_field_type_time )
                   || ( field_type == c_field_type_mtime ) || ( field_type == c_field_type_ttime ) )
                  {
                     input_size = 9;
                     max_length = 8;

                     validate = "time";

                     string time_precision;

                     if( field_extras.count( c_field_extra_time_precision ) )
                        time_precision = field_extras[ c_field_extra_time_precision ];

                     if( time_precision == "minutes" )
                     {
                        input_size = 6;
                        max_length = 5;
                     }

                     if( !value.empty( ) )
                     {
                        mtime mt( value );
                        value = format_time( mt, time_precision.c_str( ) );
                     }
                  }
                  else if( ( field_type == c_field_type_datetime )
                   || ( field_type == c_field_type_tdatetime ) || ( field_type == c_field_type_date_time ) )
                  {
                     input_size = 21;
                     max_length = 19;

                     validate = "datetime";
                     use_time = "true";

                     is_datetime = true;

                     string time_precision;

                     if( field_extras.count( c_field_extra_time_precision ) )
                        time_precision = field_extras[ c_field_extra_time_precision ];

                     if( time_precision == "minutes" )
                     {
                        input_size = 18;
                        max_length = 16;

                        use_secs = "false";
                     }

                     if( !value.empty( ) )
                     {
                        date_time dt( value );
                        value = format_date_time( dt, time_precision.c_str( ) );
                     }
                  }
                  else if( field_type == c_field_type_int )
                  {
                     input_size = 9;
                     max_length = 8;

                     validate = "int";
                  }
                  else if( field_type == c_field_type_numeric )
                  {
                     input_size = 13;
                     max_length = 12;

                     validate = "numeric";
                  }

                  if( !validate.empty( ) )
                  {
                     if( !extra.empty( ) )
                        extra += " ";

                     extra += "validate=\"" + validate + "\" ";
                  }

                  os << "<input type=\"text\" " << extra
                   << "size=\"" << input_size << "\" maxlength=\"" << max_length
                   << "\" id=\"search_" << svname << "\" name=\"search_" << svname << "\" value=\""
                   << value << "\" onkeypress=\"className = 'text'; return form_keys( event, search );\"/>";

                  if( is_datetime )
                  {
                     os << c_nbsp << "<input type=\"button\" id=\"search_" << svname
                      << "img\" class=\"pikaday_button\"" << GDS( c_display_pick_a_date ) << "\">";
                     
                     extra_content_func += "var pika" + svname + " = new Pikaday("
                      + "{ field: document.getElementById( 'search_" + svname + "' ),"
                      + " calbutton: document.getElementById( 'search_" + svname + "img' ),"
                      + " yearRange: 10, formatPreset: 1, confirm: true, useTime: "
                      + use_time + ", useSecs: " + use_secs + " } );";
                  }

                  if( field_extras.count( c_field_extra_range )
                   && !field_extras.count( c_list_field_extra_exact_match ) )
                  {
                     if( ( field_type == c_field_type_datetime )
                      || ( field_type == c_field_type_tdatetime ) || ( field_type == c_field_type_date_time ) )
                        opt = 'U';
                     else
                        opt = 'O';

                     svname = opt + ( source.lici->second )->restricts[ i ].field;

                     if( list_search_values.count( svname ) )
                        value = list_search_values.find( svname )->second;
                     else
                        value.erase( );

                     if( ( field_type == c_field_type_date )
                      || ( field_type == c_field_type_tdate ) || ( field_type == c_field_type_udate ) )
                     {
                        if( !value.empty( ) )
                        {
                           udate ud( value );

                           string date_precision;

                           if( field_extras.count( c_field_extra_date_precision ) )
                              date_precision = field_extras[ c_field_extra_date_precision ];

                           value = format_date( ud, date_precision.c_str( ) );
                        }
                     }
                     else if( ( field_type == c_field_type_time )
                      || ( field_type == c_field_type_mtime ) || ( field_type == c_field_type_ttime ) )
                     {
                        if( !value.empty( ) )
                        {
                           mtime mt( value );

                           string time_precision;

                           if( field_extras.count( c_field_extra_time_precision ) )
                              time_precision = field_extras[ c_field_extra_time_precision ];

                           value = format_time( mt, time_precision.c_str( ) );
                        }
                     }
                     else if( ( field_type == c_field_type_datetime )
                      || ( field_type == c_field_type_tdatetime ) || ( field_type == c_field_type_date_time ) )
                     {
                        if( !value.empty( ) )
                        {
                           date_time dt( value );

                           string time_precision;

                           if( field_extras.count( c_field_extra_time_precision ) )
                              time_precision = field_extras[ c_field_extra_time_precision ];

                           value = format_date_time( dt, time_precision.c_str( ) );
                        }
                     }

                     os << c_dbl_nbsp;
                     os << GDS( c_display_and );
                     os << c_dbl_nbsp;

                     os << "<input type=\"text\" " << extra
                      << "size=\"" << input_size << "\" maxlength=\"" << max_length
                      << "\" id=\"search_" << svname << "\" name=\"search_" << svname << "\" value=\""
                      << value << "\" onkeypress=\"className = 'text'; return form_keys( event, search );\"/>";

                     if( is_datetime )
                     {
                        os << c_nbsp << "<input type=\"button\" id=\"search_" << svname
                         << "_img\" class=\"pikaday_button\""
                         << GDS( c_display_pick_a_date ) << "\">";
                     
                        extra_content_func += "var pika" + svname + " = new Pikaday("
                         + "{ field: document.getElementById( 'search_" + svname + "' ),"
                         + " calbutton: document.getElementById( 'search_" + svname + "_img' ),"
                         + " yearRange: 10, formatPreset: 1, confirm: true, useTime: "
                         + use_time + ", useSecs: " + use_secs + " } );";
                     }
                  }
               }
            }

            os << "</td></tr>\n";
         }
      }

      if( !is_ui_prototype( ) )
      {
         if( allow_quick_links )
            os << "<tr><td colspan=\"2\" class=\"center\">";
         else
            os << "<tr><td colspan=\"3\" class=\"center\">";

         os << "<input id=\"search\" name=\"search\" type=\"submit\""
          " class=\"button\" value=\"" << GDS( c_display_search ) << "\"/></td>";

         if( allow_quick_links )
         {
            os << "<td class=\"center\"><label>"
             << GDS( c_display_add_user_link ) << "</label>" << c_dbl_nbsp;

            os << "<input type=\"text\" " << "size=\"40\" maxlength=\"100\""
             << " id=\"quick_link\" name=\"quick_link\" value=\"\"/></td>";
         }

         os << "</tr>\n";
      }

      os << "</tbody>\n";
      os << "</table>\n<br/><br/>\n";

      if( is_ui_prototype( ) )
      {
         os << "<div class=\"list-search-row center\">\n";
         os << "<div class=\"list-search-align\">\n";

         os << "<input id=\"search\" name=\"search\" type=\"submit\""
          " class=\"button\" value=\"" << GDS( c_display_search ) << "\"/>";

         if( allow_quick_links )
         {
            os << "<span class=\"spacer-ten-percent\"/><label>"
             << GDS( c_display_add_user_link ) << "</label>" << c_dbl_nbsp;

            os << "<input type=\"text\" " << "size=\"40\" maxlength=\"100\""
             << " id=\"quick_link\" name=\"quick_link\" value=\"\"/>";
         }

         os << "</div>\n";
         os << "</div>\n";
      }
   }

   bool had_data = false;
   bool has_text_search = ( extras.count( c_list_type_extra_text_search ) > 0 );

   string new_record_fields;
   string new_record_values;

   size_t num_sel_actions = 0;

   bool has_delete_action = false;
   bool has_any_selection_actions = false;

   if( !is_printable && ( list_type != c_list_type_home ) )
   {
      if( is_child_list )
      {
         if( !is_ui_prototype( ) )
            os << "<table><tr>";
         else
         {
            os << "<div class=\"table-row\">\n";
            os << "<div class=\"childlist-actions\">";
         }
      }
      else
      {
         if( is_ui_prototype( ) )
         {
            os << "<div class=\"topnav\">\n";
            os << "<div class=\"table-row\">\n";
            os << "<div class=\"table-cell\" id=\"list-actions\">\n";
         }
         else
            os << "<table class=\"full_width_header\"><tr>";
      }

      if( !is_ui_prototype( ) )
         os << "<td>";

      if( allow_list_actions )
      {
         // NOTE: Deleting and other operations that change data are not permitted for the
         // child lists if the parent is found to be uneditable (unless purposefully ignoring).
         // FUTURE: Non-modifying actions ought to be permitted (if they can be identified as such).
         if( !( parent_state & c_state_uneditable )
          || ( ignore_parent_state && ( parent_state & c_state_ignore_uneditable ) ) )
         {
            if( extras.count( c_list_type_extra_dpstate ) )
            {
               istringstream isstr( extras.find( c_list_type_extra_dpstate )->second );

               uint64_t flag;

               isstr >> hex >> flag;

               if( !( parent_state & flag ) )
                  is_no_erase = true;
            }

            if( ( source.lici->second )->extras.count( c_list_type_extra_erase_if_default_other ) )
            {
               if( !sess_info.is_admin_user )
                  is_no_erase = !sess_info.is_default_other( );
            }

            if( !is_no_erase
             && !has_any_changing && source.can_delete_any
             && ( !is_admin_erase || sess_info.is_admin_user )
             && ( !is_owner_erase || has_owner_parent_or_is_user_list )
             && ( !is_admin_owner_erase || sess_info.is_admin_user || has_owner_parent_or_is_user_list ) )
            {
               had_data = true;

               has_delete_action = true;

               has_any_selection_actions = ++num_sel_actions;

               string checksum_values;
               string new_checksum_value;

               string act( c_act_del );

               if( extras.count( c_list_type_extra_reverse_del ) )
                  act = c_act_rdel;

               if( use_url_checksum )
               {
                  checksum_values = act
                   + ( is_child_list ? string( c_cmd_view ) : string( c_cmd_list ) ) + parent_key
                   + ( pident.empty( ) ? oident : pident ) + user_select_key + to_string( sess_info.checksum_serial );

                  if( has_hashval )
                     checksum_values += c_hash_suffix;

                  new_checksum_value = get_checksum( sess_info, checksum_values );
               }

               os << "<input type=\"button\" class=\"button\" value=\"" << GDS( c_display_delete )
                << "\" onclick=\"";

               if( use_url_checksum )
               {
                  os << "\nvar old_checksum = query_value( '" << c_param_ochksum << "' );\n";
                  os << "if( old_checksum == '' ) old_checksum = query_value( '" << c_param_chksum << "' );\n";
               }

               os << "confirm_delete( document." << source.id << " )"
                " && list_action( document." << source.id << ", '" << source.cid
                << "', '" << act << "', '" << source.id;

               if( use_url_checksum )
                  os << "', '" << c_param_chksum << "', '" << new_checksum_value;

               os << "' );";

               if( use_url_checksum )
                  os << "\nquery_update( '" << c_param_chksum << "', old_checksum, true );";

               os << "\" style=\"cursor:pointer\"/>";
            }

            if( sess_info.is_admin_user
             || ( ( list_type != c_list_type_admin ) && ( list_type != c_list_type_child_admin ) ) )
            {
               for( size_t i = 0; i < ( source.lici->second )->actions.size( ); i++ )
               {
                  had_data = true;

                  string next_action( ( source.lici->second )->actions[ i ] );

                  if( next_action.empty( ) )
                     continue;

                  // KLUDGE: Much of the following action syntax checking is duplicated in "output_actions".
                  string::size_type pos = next_action.find( '&' );

                  if( pos != string::npos )
                  {
                     if( !( sess_info.is_admin_user && ( list_type == c_list_type_admin ) )
                      && !has_permission( next_action.substr( pos + 1 ), sess_info ) )
                        continue;

                     next_action.erase( pos );
                  }

                  if( next_action[ 0 ] == '[' )
                  {
                     string pstate;

                     bool okay = false;

                     for( pos = 1; pos < next_action.length( ); pos++ )
                     {
                        if( next_action[ pos ] == ']' )
                        {
                           okay = true;
                           break;
                        }

                        pstate += next_action[ pos ];
                     }

                     if( !okay )
                        throw runtime_error( "invalid action syntax '" + ( source.lici->second )->actions[ i ] + "'" );

                     istringstream isstr( pstate );

                     uint64_t flag;

                     isstr >> hex >> flag;

                     if( !( parent_state & flag ) )
                        continue;

                     next_action.erase( 0, pos + 1 );
                  }

                  // NOTE: Actions starting with '@' will only be available to an "admin" user.
                  if( next_action[ 0 ] == '@' )
                  {
                     if( !sess_info.is_admin_user )
                        continue;
                     else
                        next_action.erase( 0, 1 );
                  }

                  // NOTE: Actions starting with '#' will only be available to an "owner".
                  if( next_action[ 0 ] == '#' )
                  {
                     if( !has_owner_parent )
                        continue;
                     else
                        next_action.erase( 0, 1 );
                  }

                  // NOTE: Actions starting with '%' will only be available to an "owner" or the "admin" user.
                  if( next_action[ 0 ] == '%' )
                  {
                     if( !sess_info.is_admin_user && !has_owner_parent )
                        continue;
                     else
                        next_action.erase( 0, 1 );
                  }

                  // NOTE: Actions starting with ':' are not output if "has_any_changing".
                  if( next_action[ 0 ] == ':' )
                  {
                     if( has_any_changing )
                        continue;

                     next_action.erase( 0, 1 );
                  }

                  // NOTE: Actions starting with '+' will retain the list row "checks".
                  bool keep_checks = false;

                  if( next_action[ 0 ] == '+' )
                  {
                     keep_checks = true;
                     next_action.erase( 0, 1 );
                  }

                  // NOTE: Actions starting with '-' are not passed any instance keys.
                  bool ignore_selections = false;

                  if( next_action[ 0 ] == '-' )
                  {
                     ignore_selections = true;
                     next_action.erase( 0, 1 );
                  }

                  string next_id( next_action );

                  int num_fixed_key_values = 0;

                  bool is_reverse = source.is_reverse;

                  string restrict_fields, restrict_values;

                  // NOTE: Although not otherwise used is including "set_field_values"
                  // to ensure no exceptions are thrown due to transient restrictions.
                  string set_field_values;

                  determine_fixed_query_info( restrict_fields,
                   restrict_values, num_fixed_key_values, is_reverse, source,
                   source.pfield, parent_key, list_selections, sess_info, &set_field_values );

                  bool no_log = false;
                  bool not_changing = false;

                  if( !replace_action_parms( next_id, next_action, restrict_fields, restrict_values, &no_log, &not_changing ) )
                     continue;

                  if( !ignore_selections )
                     has_any_selection_actions = ++num_sel_actions;

                  if( no_log )
                     next_action = '_' + next_action;

                  string next_label( get_display_string( "procedure_" + next_id ) );

                  string checksum_values;
                  string new_checksum_value;

                  if( use_url_checksum )
                  {
                     checksum_values = string( c_act_exec )
                      + ( is_child_list ? string( c_cmd_view ) : string( c_cmd_list ) ) + parent_key
                      + next_action + ( pident.empty( ) ? oident : pident ) + user_select_key + to_string( sess_info.checksum_serial );

                     if( has_hashval )
                        checksum_values += c_hash_suffix;

                     new_checksum_value = get_checksum( sess_info, checksum_values );
                  }

                  os << "<input type=\"button\" class=\"button\" value=\"" << next_label;
                  
                  if( not_changing )
                     os << "\" not_changing=\"" << c_true_value;

                  os << "\" onclick=\"this.style.display = 'none';";

                  if( !use_url_checksum )
                     os << " ";
                  else
                  {
                     os << "\nvar old_checksum = query_value( '" << c_param_ochksum << "' );\n";
                     os << "if( old_checksum == '' ) old_checksum = query_value( '" << c_param_chksum << "' );\n";
                  }

                  os << "list_exec_action( document."
                   << source.id << ", '" << source.cid << "', '" << source.id << "', '" << next_action;

                  os << "', '" << c_param_keepchecks << "', '" << keep_checks;

                  if( !use_url_checksum )
                     os << "', '', '";
                  else
                     os << "', '" << c_param_chksum << "', '" << new_checksum_value;

                  os << "', " << ( ignore_selections ? "true" : "false" ) << " );";

                  if( use_url_checksum )
                     os << "\nquery_update( '" << c_param_chksum << "', old_checksum, true );";

                  os << "\" style=\"cursor:pointer\"/>";
               }
            }

            if( !has_any_changing )
            {
               for( size_t i = 0; i < ( source.lici->second )->parents.size( ); i++ )
               {
                  if( !has_access( ( source.lici->second )->parents[ i ].extra, sess_info, has_owner_parent, parent_state ) )
                     continue;

                  if( ( source.lici->second )->parents[ i ].operations.count( c_operation_link )
                   && ( ( ( source.lici->second )->parents[ i ].operations.find( c_operation_link ) )->second.empty( )
                   || sess_info.user_perms.count( ( source.lici->second )->parents[ i ].operations.find( c_operation_link )->second ) ) )
                  {
                     map< string, string > parent_extras;

                     if( !( source.lici->second )->parents[ i ].extra.empty( ) )
                        parse_field_extra( ( source.lici->second )->parents[ i ].extra, parent_extras );

                     if( !had_data )
                        had_data = true;
                     else
                        os << c_dbl_nbsp;

                     has_any_selection_actions = ++num_sel_actions;

                     string checksum_values;
                     string new_checksum_value;

                     if( use_url_checksum )
                     {
                        checksum_values = string( c_act_link )
                         + ( is_child_list ? string( c_cmd_view ) : string( c_cmd_list ) ) + parent_key
                         + ( pident.empty( ) ? oident : pident ) + user_select_key + to_string( sess_info.checksum_serial );

                        if( has_hashval )
                           checksum_values += c_hash_suffix;

                        new_checksum_value = get_checksum( sess_info, checksum_values );
                     }

                     os << "<select onchange=\"";

                     if( use_url_checksum )
                     {
                        os << "\nvar old_checksum = query_value( '" << c_param_ochksum << "' );\n";
                        os << "if( old_checksum == '' ) old_checksum = query_value( '" << c_param_chksum << "' );\n";
                     }

                     os << "sel_list_action( document."
                      << source.id << ", '" << source.cid << "', '" << c_act_link << "', '"
                      << source.id << "', this, '" << ( source.lici->second )->parents[ i ].field;

                     if( use_url_checksum )
                        os << "', '" << c_param_chksum << "', '" << new_checksum_value;

                     if( !use_url_checksum )
                        os << "' );\">\n";
                     else
                        os << "' );\nquery_update( '" << c_param_chksum << "', old_checksum, true );\">\n";

                     os << "<option value=\"\" disabled=\"disabled\" selected=\"selected\">" << GDS( c_display_assign_to )
                      << " " << get_display_string( ( source.lici->second )->parents[ i ].name ) << c_dbl_nbsp << "</option>\n";

                     bool is_folder = false;
                     if( ( source.lici->second )->parents[ i ].folder )
                        is_folder = true;

                     if( !( source.lici->second )->parents[ i ].mandatory
                      && !parent_extras.count( c_list_field_extra_link_none_denied_always )
                      && ( has_owner_parent || !parent_extras.count( c_list_field_extra_link_none_owner_only ) )
                      && ( sess_info.is_admin_user || !parent_extras.count( c_list_field_extra_link_none_admin_only ) )
                      && ( ( has_owner_parent || sess_info.is_admin_user ) || !parent_extras.count( c_list_field_extra_link_none_admin_owner ) )
                      && ( !is_folder || ( source.lici->second )->parents[ i ].field != ( source.lici->second )->parents[ i ].pclass ) )
                        os << "<option value=\"\">" << c_lt << GDS( c_display_none ) << c_gt << c_dbl_nbsp << "</option>\n";

                     const data_container& parent_row_data = source.parent_lists[ i ];

                     set< string > parent_pextras;
                     if( !( source.lici->second )->parents[ i ].pextra.empty( ) )
                        split( ( source.lici->second )->parents[ i ].pextra, parent_pextras, '+' );

                     for( size_t j = 0; j < parent_row_data.size( ); j++ )
                     {
                        string key( parent_row_data[ j ].first );
                        string display( parent_row_data[ j ].second );

                        // NOTE: Remove parent version information as its not relevant for a link operation.
                        size_t pos = key.find( ' ' );
                        if( pos != string::npos )
                           key.erase( pos );

                        if( display.empty( ) )
                           display = key;

                        if( parent_pextras.count( c_parent_extra_manuallink ) )
                        {
                           stringstream ss;

                           replace_links_and_output( display, "",
                            source.module, source.module_ref, ss, false, false, session_id,
                            sess_info, user_select_key, using_session_cookie, use_url_checksum );

                           display = ss.str( );
                        }

                        os << "<option value=\"" << key << "\">" << unescaped( display ) << c_dbl_nbsp << "</option>\n";
                     }

                     os << "</select>";
                  }
               }

               for( size_t i = 0; i < ( source.lici->second )->restricts.size( ); i++ )
               {
                  if( !has_access( ( source.lici->second )->restricts[ i ].extra, sess_info, has_owner_parent, parent_state ) )
                     continue;

                  if( ( source.lici->second )->restricts[ i ].operations.count( c_operation_link )
                   && ( ( ( source.lici->second )->restricts[ i ].operations.find( c_operation_link ) )->second.empty( )
                   || sess_info.user_perms.count( ( source.lici->second )->restricts[ i ].operations.find( c_operation_link )->second ) ) )
                  {
                     map< string, string > restrict_extras;

                     if( !( source.lici->second )->restricts[ i ].extra.empty( ) )
                        parse_field_extra( ( source.lici->second )->restricts[ i ].extra, restrict_extras );

                     // NOTE: If the restrict link field is actually the security level and the user is
                     // anonymous or only has "level 0" security then don't display the security level.
                     if( restrict_extras.count( c_field_extra_enum )
                      && restrict_extras.count( c_field_extra_security_level ) )
                     {
                        const enum_info& info( sinfo.enums.find( restrict_extras.find( c_field_extra_enum )->second )->second );

                        if( sess_info.user_id.empty( ) || ( info.values[ 0 ].first == sess_info.user_slevel ) )
                           continue;
                     }

                     if( !had_data )
                        had_data = true;
                     else
                        os << c_dbl_nbsp;

                     has_any_selection_actions = ++num_sel_actions;

                     string checksum_values;
                     string new_checksum_value;

                     if( use_url_checksum )
                     {
                        checksum_values = string( c_act_link )
                         + ( is_child_list ? string( c_cmd_view ) : string( c_cmd_list ) ) + parent_key
                         + ( pident.empty( ) ? oident : pident ) + user_select_key + to_string( sess_info.checksum_serial );

                        if( has_hashval )
                           checksum_values += c_hash_suffix;

                        new_checksum_value = get_checksum( sess_info, checksum_values );
                     }

                     os << "<select onchange=\"sel_list_action( document."
                      << source.id << ", '" << source.cid << "', '" << c_act_link << "', '"
                      << source.id << "', this, '" << ( source.lici->second )->restricts[ i ].field;

                     if( use_url_checksum )
                        os << "', '" << c_param_chksum << "', '" << new_checksum_value;

                     os << "' );\">\n";

                     os << "<option value=\"\" disabled=\"disabled\" selected=\"selected\">" << GDS( c_display_assign_to )
                      << " " << get_display_string( ( source.lici->second )->restricts[ i ].name ) << c_dbl_nbsp << "</option>\n";

                     if( ( source.lici->second )->restricts[ i ].ftype == c_field_type_bool )
                     {
                        os << "<option value=\"0\">" << GDS( c_display_false ) << c_dbl_nbsp << "</option>\n";
                        os << "<option value=\"1\">" << GDS( c_display_true ) << c_dbl_nbsp << "</option>\n";
                     }
                     else
                     {
                        if( restrict_extras.count( c_field_extra_enum ) )
                        {
                           const enum_info& info( sinfo.enums.find( restrict_extras.find( c_field_extra_enum )->second )->second );

                           for( size_t j = 0; j < info.values.size( ); j++ )
                           {
                              // NOTE: Enum values that start with a '-' are not included for user selection
                              // as they are deemed as being only available for internal application purposes.
                              if( info.values[ j ].first[ 0 ] == '-' )
                                 continue;

                              os << "<option value=\"" << info.values[ j ].first << "\">"
                               << get_display_string( info.values[ j ].second ) << c_dbl_nbsp << "</option>\n";

                              // NOTE: Security level enumeration is stopped at the user's level so it is
                              // not possible for a user to create or modify an instance's security level
                              // to a level greater than the level the user has been granted.
                              if( restrict_extras.count( c_field_extra_security_level )
                               && ( sess_info.user_id.empty( ) || ( info.values[ j ].first == sess_info.user_slevel ) ) )
                                 break;
                           }
                        }
                        else
                           throw runtime_error( "unexpected non-bool/enum restrict link" );
                     }

                     os << "</select>";
                  }
               }
            }
         }
      }

      if( !extras.count( c_list_type_extra_search ) && extras.count( c_list_type_extra_text_search ) )
      {
         if( !had_data )
            had_data = true;
         else
            os << c_dbl_nbsp;

         if( !source.text_search_title_fields.empty( ) )
         {
            string text_search_title;

            for( size_t j = 0; j < source.text_search_title_fields.size( ); j++ )
            {
               if( j > 0 )
               {
                  if( j == source.text_search_title_fields.size( ) - 1 )
                     text_search_title += GDS( c_display_or );
                  else
                     text_search_title += ", ";
               }

               text_search_title += source.text_search_title_fields[ j ];
            }

            text_search_title += " " + GDS( c_display_contains );

            os << text_search_title << c_nbsp;
         }

         os << "<input type=\"text\" id=\"text_search\" size=\"15\"";

         if( list_search_text.count( source.id + c_srch_suffix ) )
         {
            text_search_value = list_search_text.find( source.id + c_srch_suffix )->second;
            os << " value=\"" << escape_markup( text_search_value ) << "\"";
         }

         os << "/>";

         os << c_nbsp << "<input type=\"submit\" class=\"button\" value=\"" << GDS( c_display_search ) << "\"></input>";
      }

      string checksum_values;
      string new_checksum_value;

      if( use_url_checksum )
      {
         checksum_values = string( c_cmd_list ) + oident + user_select_key;
         new_checksum_value = get_checksum( sess_info, checksum_values );
      }

      if( !extras.count( c_list_type_extra_search ) )
      {
         for( size_t i = 0; i < ( source.lici->second )->parents.size( ); i++ )
         {
            if( !has_access( ( source.lici->second )->parents[ i ].extra, sess_info, has_owner_parent, parent_state ) )
               continue;

            if( ( source.lici->second )->parents[ i ].operations.count( c_operation_select ) )
            {
               set< string > parent_extras;

               if( !( source.lici->second )->parents[ i ].pextra.empty( ) )
                  split( ( source.lici->second )->parents[ i ].pextra, parent_extras, '+' );

               if( !had_data )
                  had_data = true;
               else
                  os << c_dbl_nbsp;

               string sel_id( source.id );
               sel_id += c_prnt_suffix;
               sel_id += ( '0' + i );

               os << get_display_string( ( source.lici->second )->parents[ i ].name ) << ": ";
               os << "<select onchange=\"";

               if( is_child_list )
                  os << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";

               if( use_url_checksum )
               {
                  checksum_values = ( is_child_list ? string( c_cmd_view )
                   : string( c_cmd_list ) ) + parent_key + ( pident.empty( ) ? oident : pident ) + user_select_key;

                  if( has_hashval || extras.count( c_list_type_extra_text_search ) )
                     checksum_values += c_hash_suffix;

                  new_checksum_value = get_checksum( sess_info, checksum_values );

                  os << "query_update( '" << c_param_uselextra << "', '', true ); ";
                  os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";

                  string value_name( "this.options[ this.selectedIndex ].value" );

                  if( extras.count( c_list_type_extra_text_search ) )
                     append_hash_values_query_update( os,
                      sel_id, true, source.id, value_name, list_selections, sess_info, &findinfo_and_listsrch );
               }

               os << "sel_qry_update( this, '" << sel_id << "' );\">\n";

               os << "<option value=\"\">" << c_lt << GDS( c_display_any ) << c_gt << c_dbl_nbsp << "</option>\n";

               string selected_value;
               if( list_selections.count( sel_id ) )
               {
                  selected_value = list_selections.find( sel_id )->second;

                  if( !new_record_fields.empty( ) )
                  {
                     new_record_fields += ",";
                     new_record_values += ",";
                  }

                  new_record_fields += ( source.lici->second )->parents[ i ].field;
                  new_record_values += selected_value;
               }

               if( !( source.lici->second )->parents[ i ].mandatory )
               {
                  os << "<option value=\"~\"";
                  if( selected_value == "~" )
                     os << " selected";
                  os << ">" << c_lt << GDS( c_display_none ) << c_gt << c_dbl_nbsp << "</option>\n";
               }

               const data_container& parent_row_data = source.parent_lists[ i ];

               for( size_t j = 0; j < parent_row_data.size( ); j++ )
               {
                  string key( parent_row_data[ j ].first );
                  string display( parent_row_data[ j ].second );

                  // NOTE: Remove parent version information as its not relevant for a select operation.
                  size_t pos = key.find( ' ' );
                  if( pos != string::npos )
                     key.erase( pos );

                  if( display.empty( ) )
                     display = key;

                  if( parent_extras.count( c_parent_extra_manuallink ) )
                  {
                     stringstream ss;

                     replace_links_and_output( display, "",
                      source.module, source.module_ref, ss, false, false, session_id,
                      sess_info, user_select_key, using_session_cookie, use_url_checksum );

                     display = ss.str( );
                  }

                  os << "<option value=\"" << key << "\"";
                  if( key == selected_value )
                     os << " selected";

                  os << ">" << unescaped( display ) << c_dbl_nbsp << "</option>\n";
               }

               os << "</select>";
            }
         }
      }

      for( size_t i = 0; i < ( source.lici->second )->parents.size( ); i++ )
      {
         if( !has_access( ( source.lici->second )->parents[ i ].extra, sess_info, has_owner_parent, parent_state ) )
            continue;

         if( ( source.lici->second )->parents[ i ].operations.count( c_operation_checked ) )
         {
            if( !had_data )
               had_data = true;
            else
               os << c_dbl_nbsp;

            string sel_id( source.id );
            sel_id += c_prnt_suffix;
            sel_id += ( '0' + i );

            bool is_checked = true;
            if( list_selections.count( sel_id ) && list_selections.find( sel_id )->second == c_false )
               is_checked = false;

            string checksum_values;
            string new_checksum_value;

            os << "<input type=\"checkbox\" id=\"check" << i << "\""
             << ( is_checked ? " checked=\"checked\"" : "" ) << " onclick=\"";

            if( is_child_list )
               os << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";

            if( use_url_checksum )
            {
               checksum_values = ( is_child_list ? string( c_cmd_view )
                : string( c_cmd_list ) ) + parent_key + ( pident.empty( ) ? oident : pident ) + user_select_key;

               if( has_hashval || extras.count( c_list_type_extra_search ) )
                  checksum_values += c_hash_suffix;

               new_checksum_value = get_checksum( sess_info, checksum_values );

               os << "query_update( '" << c_param_uselextra << "', '', true ); ";
               os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";

               string value_name( "document." + source.id + ".check" + to_string( i ) + ".checked" );

               if( extras.count( c_list_type_extra_search ) )
                  append_hash_values_query_update( os, sel_id, has_text_search,
                   source.id, value_name, list_selections, sess_info, &findinfo_and_listsrch );
            }

            os << "query_update( '"
             << sel_id << "', '' + document." << source.id << ".check" << i << ".checked );\">";

            string display_name;
            const data_container& parent_row_data = source.parent_lists[ i ];
            for( size_t j = 0; j < parent_row_data.size( ); j++ )
            {
               string key( parent_row_data[ j ].first );
               string display( parent_row_data[ j ].second );

               // NOTE: Remove parent version information as its not relevant for a checked operation.
               size_t pos = key.find( ' ' );
               if( pos != string::npos )
                  key.erase( pos );

               if( key == ( source.lici->second )->parents[ i ].operations[ c_operation_checked ] )
               {
                  display_name = display;
                  break;
               }
            }

            os << c_nbsp << display_name << " " << GDS( c_display_only ) << "</input>";
         }
      }

      for( size_t i = 0; i < ( source.lici->second )->restricts.size( ); i++ )
      {
         if( !has_access( ( source.lici->second )->restricts[ i ].extra, sess_info, has_owner_parent, parent_state ) )
            continue;

         if( ( source.lici->second )->restricts[ i ].operations.count( c_operation_select ) )
         {
            map< string, string > restrict_extras;
            parse_field_extra( ( source.lici->second )->restricts[ i ].extra, restrict_extras );

            if( !restrict_extras.count( c_field_extra_enum ) )
               continue;

            if( !had_data )
               had_data = true;
            else
               os << c_dbl_nbsp;

            string sel_id( source.id );
            sel_id += c_rest_suffix;
            sel_id += ( '0' + i );

            const enum_info& info( sinfo.enums.find( restrict_extras.find( c_field_extra_enum )->second )->second );

            // NOTE: If the restrict link field is actually the security level and the user is
            // anonymous or only has "level 0" security then don't display the security level.
            if( restrict_extras.count( c_field_extra_enum )
             && restrict_extras.count( c_field_extra_security_level ) )
            {
               const enum_info& info( sinfo.enums.find( restrict_extras.find( c_field_extra_enum )->second )->second );

               if( sess_info.user_id.empty( ) || ( info.values[ 0 ].first == sess_info.user_slevel ) )
                  continue;
            }

            string current_value;
            if( list_selections.count( sel_id ) )
            {
               current_value = list_selections.find( sel_id )->second;

               if( !new_record_fields.empty( ) )
               {
                  new_record_fields += ",";
                  new_record_values += ",";
               }

               new_record_fields += ( source.lici->second )->restricts[ i ].field;
               new_record_values += current_value;
            }

            size_t value_offset = 0;
            for( size_t j = 0; j < info.values.size( ); j++ )
            {
               if( info.values[ j ].first == current_value )
               {
                  value_offset = j + 1;
                  break;
               }
            }

            os << get_display_string( ( source.lici->second )->restricts[ i ].name ) << ": ";
            os << "<select onchange=\"";

            if( is_child_list )
               os << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";

            if( use_url_checksum )
            {
               string checksum_values( ( is_child_list ? string( c_cmd_view )
                : string( c_cmd_list ) ) + parent_key + ( pident.empty( ) ? oident : pident ) + user_select_key );

               if( has_hashval || extras.count( c_list_type_extra_search ) )
                  checksum_values += c_hash_suffix;

               string new_checksum_value( get_checksum( sess_info, checksum_values ) );

               os << "query_update( '" << c_param_uselextra << "', '', true ); ";

               os << "query_update( '" << c_param_chksum
                << "', '" << new_checksum_value << "', true ); ";

               string value_name( "this.options[ this.selectedIndex ].value" );

               if( extras.count( c_list_type_extra_search ) )
                  append_hash_values_query_update( os, sel_id, has_text_search,
                   source.id, value_name, list_selections, sess_info, &findinfo_and_listsrch );
            }

            os << "sel_qry_update( this, '" << sel_id << "' );\">\n";

            os << "<option value=\"\">" << c_lt << GDS( c_display_any ) << c_gt << c_dbl_nbsp << "</option>\n";

            for( size_t j = 0; j < info.values.size( ); j++ )
            {
               // NOTE: Enum values that start with a '-' are not included for user selection
               // as they are deemed as being only available for internal application purposes.
               if( info.values[ j ].first[ 0 ] == '-' )
                  continue;

               os << "<option";
               if( value_offset && ( j + 1 == value_offset ) )
                  os << " selected";
               os << " value=\"" << info.values[ j ].first << "\">"
                << get_display_string( info.values[ j ].second ) << c_dbl_nbsp << "</option>\n";

               // NOTE: Security level enumeration is stopped at the user's level.
               if( restrict_extras.count( c_field_extra_security_level )
                && ( sess_info.user_id.empty( ) || ( info.values[ j ].first == sess_info.user_slevel ) ) )
                  break;
            }

            os << "</select>";
         }
         else if( ( source.lici->second )->restricts[ i ].operations.count( c_operation_checked )
          || ( source.lici->second )->restricts[ i ].operations.count( c_operation_rchecked )
          || ( source.lici->second )->restricts[ i ].operations.count( c_operation_unchecked )
          || ( source.lici->second )->restricts[ i ].operations.count( c_operation_runchecked ) )
         {
            map< string, string > restrict_extras;
            parse_field_extra( ( source.lici->second )->restricts[ i ].extra, restrict_extras );

            if( !had_data )
               had_data = true;
            else
               os << c_dbl_nbsp;

            string sel_id( source.id );
            sel_id += c_rest_suffix;
            sel_id += ( '0' + i );

            bool is_checked = true;
            bool is_uncheck = false;

            if( ( source.lici->second )->restricts[ i ].operations.count( c_operation_unchecked )
             || ( source.lici->second )->restricts[ i ].operations.count( c_operation_runchecked ) )
            {
               is_uncheck = true;
               is_checked = false;
            }

            if( list_selections.count( sel_id ) && list_selections.find( sel_id )->second == c_true )
               is_checked = true;

            if( list_selections.count( sel_id ) && list_selections.find( sel_id )->second == c_false )
               is_checked = false;

            os << "<input type=\"checkbox\" id=\"check" << i << "\""
             << ( is_checked ? " checked=\"checked\"" : "" ) << " onclick=\"";

            // NOTE: For a checkbox that changes ordering clear existing sort information.
            if( ( source.lici->second )->restricts[ i ].operations.count( c_operation_rchecked )
             || ( source.lici->second )->restricts[ i ].operations.count( c_operation_runchecked ) )
               os << "query_update( '" << source.id << c_sort_suffix << "', '', true ); ";

            // NOTE: Remove next/prev page info for a new search.
            os << "query_update( '" << source.id << c_info_suffix << "', '', true ); ";

            if( is_child_list )
               os << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";

            if( use_url_checksum )
            {
               string checksum_values( ( is_child_list ? string( c_cmd_view )
                : string( c_cmd_list ) ) + parent_key + ( pident.empty( ) ? oident : pident ) + user_select_key );

               if( has_hashval || extras.count( c_list_type_extra_search ) )
                  checksum_values += c_hash_suffix;

               string new_checksum_value( get_checksum( sess_info, checksum_values ) );

               os << "query_update( '" << c_param_uselextra << "', '', true ); ";
               os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";

               string value_name( "document." + source.id + ".check" + to_string( i ) + ".checked" );

               if( extras.count( c_list_type_extra_search ) )
                  append_hash_values_query_update( os, sel_id, has_text_search,
                   source.id, value_name, list_selections, sess_info, &findinfo_and_listsrch );
            }

            if( restrict_extras.count( c_vext_prefix ) )
            {
               if( is_checked )
               {
                  if( is_uncheck )
                     os << "query_update( '" << c_vext_prefix << restrict_extras[ c_vext_prefix ] << "', '', true ); ";
                  else
                     os << "query_update( '" << c_vext_prefix << restrict_extras[ c_vext_prefix ] << "', '0', true ); ";
               }
               else
               {
                  if( !is_uncheck )
                     os << "query_update( '" << c_vext_prefix << restrict_extras[ c_vext_prefix ] << "', '', true ); ";
                  else
                     os << "query_update( '" << c_vext_prefix << restrict_extras[ c_vext_prefix ] << "', '1', true ); ";
               }
            }

            os << "query_update( '"
             << sel_id << "', '' + document." << source.id << ".check" << i << ".checked );\">";

            os << c_nbsp << get_display_string( ( source.lici->second )->restricts[ i ].name )
             << " " << GDS( c_display_only ) << "</input>";
         }
      }

      if( !is_ui_prototype( ) )
         os << "</td>";
      else
         os << "</div>\n<div class=\"table-cell message center\">";

      if( !error_message.empty( ) )
      {
         // NOTE: Don't display as an error unless was actually received that way from the server.
         if( error_message.find( GDS( c_display_error ) ) != 0 )
         {
            if( !is_ui_prototype( ) )
               os << "<td>" << error_message << "</td>";
            else
               os << "<p class=\"center list-message\">" << error_message << "</p>";
         }
         else
         {
            if( !is_ui_prototype( ) )
               os << "<td class=\"error\">" << remove_key( error_message ) << "</td>";
            else
               os << "<p class=\"error center list-message\">" << remove_key( error_message ) << "</p>";
         }
      }

      if( is_ui_prototype( ) )
         os << "</div>\n<div class=\"table-cell right-relative\">\n";

      if( !is_child_list && !sess_info.user_id.empty( )
       && list_type != c_list_type_home && !extras.count( c_list_type_extra_no_print )
       && ( sess_info.is_admin_user || !extras.count( c_list_type_extra_admin_print ) ) )
      {
         if( !is_ui_prototype( ) )
            os << "<td class=\"right\">";
         else
         {
            if( error_message.empty( ) )
               os << "<div id=\"print-list\" class=\"print\">";
            else
               os << "<div id=\"print-list-message\" class=\"print\">";
         }

         os << "<a href=\"javascript:";

         if( use_url_checksum )
         {
            string checksum_values( string( c_cmd_plist ) + oident + user_select_key );

            if( has_hashval )
               checksum_values += c_hash_suffix;

            string new_checksum_value( get_checksum( sess_info, checksum_values ) );

            os << "query_update( '" << c_param_uselextra << "', '', true ); ";
            os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";

            if( has_hashval )
               append_hash_values_query_update( os, "", has_text_search,
                source.id, "", list_selections, sess_info, &findinfo_and_listsrch );
         }

         os << "query_update( 'cmd', 'plist' );\">" << GDS( c_display_print ) << "</a>";

         if( !is_ui_prototype( ) )
            os << "</td>\n";
         else
            os << "</div>\n";
      }

      if( is_ui_prototype( ) )
      {
         if( !is_child_list )
            os << "</div>\n";
         os << "</div>\n</div>\n";
      }
      else
         os << "</tr></table>\n";
   }

   os << "<table name=\"" << source.id << "_table\" id=\"" << source.id
    << "_table\"" << " class=\"list\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n";
   os << "<thead>\n";
   os << "<tr>\n";

   bool display_list_checks = false;
   bool display_row_numbers = false;
   bool display_dummy_checks = false;

   if( list_type != c_list_type_home && !sess_info.is_read_only && !sess_info.user_id.empty( ) )
   {
      if( !is_printable )
      {
         if( has_any_selection_actions )
            display_list_checks = ( allow_list_actions && source.can_action_any );

         if( !display_list_checks )
            display_dummy_checks = true;
      }
      else if( print_list_opts.count( c_list_print_opt_show_checks ) )
         display_list_checks = true;

      if( is_printable && print_list_opts.count( c_list_print_opt_show_numbers ) )
         display_row_numbers = true;

      if( display_dummy_checks )
      {
         os << "  <th class=\"list\" width=\"25\" align=\"center\">";
         os << c_dummy_check_symbol;
         os << "  </th>\n";
      }
      else if( display_list_checks )
      {
         os << "  <th class=\"list\" width=\"25\" align=\"center\">";
         os << "<input type=\"checkbox\" name=\"all\" onclick=\"check_or_uncheck_all( document." << source.id << ", this );\"/>";
         os << "  </th>\n";
      }

      if( display_row_numbers )
         os << "  <th class=\"list\" width=\"30\" align=\"center\">#</th>\n";
   }

   int col_num = 0;
   int special_col = -1;
   int filename_col = -1;
   int nextsortfield = 0;
   int display_offset = 0;
   int orientation_col = 0;
   int total_display_cols = 0;
   int ignore_encrypted_col = -1;

   map< string, string > display_names;
   vector< numeric > print_total_values;
   vector< size_t > print_total_col_nums;
   vector< size_t > print_summary_counts;
   vector< size_t > print_summary_col_nums;
   vector< string > print_summary_value_ids;
   vector< string > print_total_col_value_ids;
   vector< numeric > print_summary_sub_totals;

   for( size_t i = 0; i < source.field_ids.size( ); i++ )
   {
      if( source.field_ids[ i ] == source.special_field )
         special_col = col_num;

      if( source.field_ids[ i ] == source.filename_field )
         filename_col = col_num;

      if( source.field_ids[ i ] == source.orientation_field )
         orientation_col = col_num;

      if( source.field_ids[ i ] == source.ignore_encrypted_field )
         ignore_encrypted_col = col_num;

      if( source.field_ids[ i ] != c_key_field && source.field_ids[ i ] != c_row_field )
         col_num++;

      if( source.hidden_fields.count( source.value_ids[ i ] ) )
         continue;

      if( source.pstate_fields.count( source.value_ids[ i ] )
       && !( parent_state & source.pstate_fields.find( source.value_ids[ i ] )->second ) )
      {
         display_offset++;
         continue;
      }

      if( is_printable && source.non_print_fields.count( source.value_ids[ i ] ) )
      {
         display_offset++;
         continue;
      }

      if( !is_printable && source.print_only_fields.count( source.value_ids[ i ] ) )
      {
         display_offset++;
         continue;
      }

      if( source.print_summary_fields.count( source.value_ids[ i ] ) )
      {
         print_summary_counts.push_back( 0 );
         print_summary_col_nums.push_back( col_num - 1 );
         print_summary_value_ids.push_back( source.value_ids[ i ] );

         display_offset++;
         continue;
      }

      ++total_display_cols;

      if( source.print_total_fields.count( source.value_ids[ i ] ) )
      {
         print_total_values.push_back( 0 );
         print_total_col_nums.push_back( total_display_cols - 1 );
         print_total_col_value_ids.push_back( source.value_ids[ i ] );
      }

      string class_tag( "list" );
      if( source.force_right_fields.count( source.value_ids[ i ] ) )
         class_tag += " right";
      else if( source.force_center_fields.count( source.value_ids[ i ] ) )
         class_tag += " center";
      else if( source.force_justify_fields.count( source.value_ids[ i ] ) )
         class_tag += " justify";

      if( source.omit_label_fields.count( source.value_ids[ i ] ) )
         os << "  <th class=\"" << class_tag << "\">" << c_nbsp;
      else if( source.use_list_title_fields.count( source.value_ids[ i ] ) )
         os << "  <th class=\"" << class_tag << "\">" << mod_info.get_string( source.lici->second->id + "_name" );
      else
         os << "  <th class=\"" << class_tag << "\">" << data_or_nbsp( source.display_names[ display_offset ] );

      display_names.insert( make_pair( source.field_ids[ i ], source.display_names[ display_offset ] ) );

      ++display_offset;

      if( source.uom_fields.count( source.value_ids[ i ] ) )
      {
         string symbol( source.uom_fields.find( source.value_ids[ i ] )->second );

         if( symbol.find( c_uom_prefix ) == 0 )
         {
            string name_key( source.lici->second->fields[ i ].pfname );
            if( name_key.empty( ) )
               name_key = source.lici->second->fields[ i ].name;

            symbol = get_display_string( name_key + "_(" + symbol.substr( strlen( c_uom_prefix ) ) + ")" );
         }

         if( !symbol.empty( ) )
            os << " (" << symbol << ")";
      }

      if( !is_printable && ( list_type != c_list_type_home ) )
      {
         if( source.sort_fields.count( source.field_ids[ i ] ) )
         {
            os << c_nbsp << "<a href=\"javascript:";

            if( use_url_checksum )
            {
               string checksum_values( ( is_child_list ? string( c_cmd_view )
                : string( c_cmd_list ) ) + parent_key + ( pident.empty( ) ? oident : pident ) + user_select_key );

               if( has_hashval || extras.count( c_list_type_extra_search ) )
                  checksum_values += c_hash_suffix;

               string new_checksum_value( get_checksum( sess_info, checksum_values ) );

               os << "query_update( '" << c_param_uselextra << "', '', true ); ";
               os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";

               if( extras.count( c_list_type_extra_search ) )
                  append_hash_values_query_update( os, "", has_text_search,
                   source.id, "", list_selections, sess_info, &findinfo_and_listsrch );
            }

            os << "query_update( '" << source.id << c_info_suffix << "', "
             "'', true ); query_update( '" << source.id << c_sort_suffix << "', '" << ++nextsortfield;

            if( source.sort_field == nextsortfield )
            {
               if( source.is_reverse )
                  os << "F' );\"><img src=\"sort_reverse.png\" border=\"0\"></a>";
               else
                  os << "R' );\"><img src=\"sort_forward.png\" border=\"0\"></a>";
            }
            else
               os << "F' );\"><img src=\"sort_select.png\" border=\"0\"></a>";
         }
      }

      os << "</th>\n";
   }

   for( size_t i = 0; i < print_total_col_nums.size( ) * print_summary_col_nums.size( ); i++ )
      print_summary_sub_totals.push_back( 0 );

   os << "</tr>\n";
   os << "</thead>\n";

   os << "<tbody>\n";

   set< string > new_rec_fks_in_use;

   size_t last_j_val = 0;
   size_t row = 0, row_num = 0;

   string special_value;
   vector< string > final_subtotals;

   for( size_t i = 0; i < source.row_data.size( ); i++ )
   {
      bool was_odd = false;

      for( size_t j = 0; j < print_summary_col_nums.size( ); j++ )
         ++print_summary_counts.at( j );

      uint64_t state;
      string type_info;
      string key_and_version;

      string key_ver_rev_state_and_type_info( source.row_data[ i ].first );
      string key( key_ver_rev_state_and_type_info.substr( 0, key_ver_rev_state_and_type_info.find( ' ' ) ) );

      parse_key_ver_rev_state_and_type_info( key_ver_rev_state_and_type_info, key_and_version, state, type_info );

      string extra_effect, display_effect;

      // NOTE: Determine whether rows should be displayed differently according to class scoped modifiers
      // and state. It is being assumed here that the original list fields and source fields have the same
      // offsets.
      if( !extras.count( c_list_type_extra_ignore_display_state )
       && ( !is_printable || !extras.count( c_list_type_extra_print_no_highlight ) ) )
      {
         for( size_t k = 0; k < ARRAY_SIZE( state_modifiers ); k++ )
         {
            if( state & state_modifiers[ k ] )
            {
               pair< modifier_const_iterator, modifier_const_iterator > range
                = source.lici->second->modifiers.equal_range( state_modifiers[ k ] );

               for( modifier_const_iterator ci = range.first; ci != range.second; ++ci )
               {
                  string new_effect( ci->second );

                  if( ( new_effect == c_modifier_effect_lowlight )
                   || ( new_effect == c_modifier_effect_lowlight1 )
                   || ( new_effect == c_modifier_effect_highlight )
                   || ( new_effect == c_modifier_effect_highlight1 ) )
                     display_effect = new_effect;

                  if( ( new_effect == c_modifier_effect_extralight )
                   || ( new_effect == c_modifier_effect_extralight1 ) )
                     extra_effect = new_effect;
               }
            }
         }
      }

      string row_class_suffix( "data" );

      if( display_effect == c_modifier_effect_lowlight )
         row_class_suffix = "lowlight";
      else if( display_effect == c_modifier_effect_lowlight1 )
         row_class_suffix = "lowlight1";
      else if( display_effect == c_modifier_effect_highlight )
         row_class_suffix = "highlight";
      else if( display_effect == c_modifier_effect_highlight1 )
         row_class_suffix = "highlight1";

      string checked;
      map< string, string >::const_iterator rci( source.row_errors.find( key ) );

      vector< string > columns;
      raw_split( source.row_data[ i ].second, columns );

      size_t column = 0;
      size_t fk_column = 0;

      set< string > fk_refs;

      bool had_link = false;
      size_t total_column = 0;
      bool is_first_column = true;

      for( size_t j = 0; j < source.field_ids.size( ); j++ )
      {
         string cell_data;

         string source_field_id( source.field_ids[ j ] );
         string source_value_id( source.value_ids[ j ] );

         map< string, string > extra_data;

         if( !( source.lici->second )->fields[ j ].extra.empty( ) )
            parse_field_extra( ( source.lici->second )->fields[ j ].extra, extra_data );

         if( source_field_id == c_key_field )
            cell_data = key;
         else if( source_field_id == c_row_field )
         {
            ostringstream osstr;
            osstr << ifmt( ( int )( log10( ( float )source.row_data.size( ) ) ) + 1 ) << ( i + 1 );
            cell_data = osstr.str( );
         }
         else
         {
            if( column < columns.size( ) )
               cell_data = columns[ column++ ];
            else
               cell_data = "*** COLUMN ERROR ***";
         }

         if( special_col >= 0 )
            special_value = columns[ special_col ];

         bool skip_column = false;
         bool is_fk_column = false;

         if( source.fk_field_ids.size( ) && !fk_refs.count( source_field_id )
          && fk_column < source.fk_field_ids.size( ) && source.fk_field_ids[ fk_column ] == source_field_id )
            is_fk_column = true;

         if( source.hidden_fields.count( source_value_id ) )
            skip_column = true;

         if( is_printable && source.non_print_fields.count( source_value_id ) )
            skip_column = true;

         if( !is_printable && source.print_only_fields.count( source_value_id ) )
            skip_column = true;

         if( source.pstate_fields.count( source_value_id )
          && !( parent_state & source.pstate_fields.find( source_value_id )->second ) )
            skip_column = true;

         if( skip_column )
         {
            if( is_fk_column )
               ++fk_column;

            continue;
         }

         if( source.print_total_fields.count( source_value_id ) )
         {
            numeric val( cell_data.c_str( ) );

            for( size_t k = 0; k < print_summary_col_nums.size( ); k++ )
               print_summary_sub_totals.at( ( source.print_total_fields.size( ) * k ) + total_column ) += val;

            print_total_values.at( total_column++ ) += val;
         }

         if( j > last_j_val )
            last_j_val = j;

         bool is_last_row = ( i == ( source.row_data.size( ) - 1 ) );

         // NOTE: Subtotals are normally output when a change to a summary field is detected (i.e. when
         // the next row is being processed), however, if we are processing the last column of the last
         // row then need to prepare the subtotals here so they can be output before the totals.
         if( ( is_first_column && !print_summary_col_nums.empty( ) )
          || ( ( j == last_j_val ) && is_last_row && !print_summary_col_nums.empty( ) ) )
         {
            ostringstream osx;
            deque< string > subtotals;

            for( size_t k = 0; k < print_summary_col_nums.size( ); k++ )
            {
               vector< string > last_columns;
               if( i > 0 )
                  raw_split( source.row_data[ i - 1 ].second, last_columns );

               if( i == 0 || ( j == last_j_val && i == source.row_data.size( ) - 1 )
                || columns.at( print_summary_col_nums[ k ] ) != last_columns.at( print_summary_col_nums[ k ] ) )
               {
                  string cell_data( columns.at( print_summary_col_nums[ k ] ) );

                  // NOTE: A print summary field could be an enumated type.
                  if( source.enum_fields.count( print_summary_value_ids[ k ] ) )
                  {
                     const enum_info& info(
                      sinfo.enums.find( source.enum_fields.find( print_summary_value_ids[ k ] )->second )->second );

                     for( size_t i = 0; i < info.values.size( ); i++ )
                     {
                        if( info.values[ i ].first == cell_data )
                        {
                           cell_data = get_display_string( info.values[ i ].second );
                           break;
                        }
                     }
                  }

                  ostringstream osxs;

                  if( i > 0 && !print_total_col_nums.empty( )
                   && !extras.count( c_list_type_extra_no_sub_totals ) )
                  {
                     int next_total = 0;

                     if( row++ % 2 == 0 )
                        osxs << "<tr class=\"data\">\n";
                     else
                        osxs << "<tr class=\"odd_data\">\n";

                     if( display_list_checks || display_dummy_checks )
                        osxs << "  <td class=\"list\" width=\"25\" align=\"center\">" << c_nbsp << "</td>\n";

                     if( display_row_numbers )
                        osxs << "  <td class=\"list\" width=\"30\" align=\"center\">" << c_nbsp << "</td>\n";

                     osxs << "  <td>";

                     // FUTURE: Rather than using the bold tag a different style tag should
                     // be used for each level (to support different appearances per level).
                     if( k > 0 )
                        osxs << c_dbl_nbsp << GDS( c_display_subtotal );
                     else
                        osxs << "<b>" << GDS( c_display_subtotal ) << "</b>";

                     size_t count( print_summary_counts[ k ] );
                     if( j != last_j_val || i != source.row_data.size( ) - 1 )
                        --count;

                     osxs << c_dbl_nbsp << count << " " << GDS( c_display_records ) << "</td>\n";
                     print_summary_counts.at( k ) = 1;

                     for( int x = 1; x < total_display_cols; x++ )
                     {
                        string class_tag( "list" );
                        if( source.force_right_fields.count( print_total_col_value_ids[ next_total ] ) )
                           class_tag += " right";
                        else if( source.force_center_fields.count( print_total_col_value_ids[ next_total ] ) )
                           class_tag += " center";
                        else if( source.force_justify_fields.count( print_total_col_value_ids[ next_total ] ) )
                           class_tag += " justify";

                        numeric total = print_summary_sub_totals.at( ( source.print_total_fields.size( ) * k ) + next_total );

                        string total_string( total.as_string( ) );

                        if( source.pmask_fields.count( print_total_col_value_ids.at( next_total ) ) )
                           total_string = format_numeric_value( total, source.pmask_fields.find( print_total_col_value_ids[ next_total ] )->second );

                        // NOTE: If the field being sub-totalled has a special prefix/suffix then it is assumed that
                        // all such records will have the same suffix/prefix so it will appear as a prefix/suffix in
                        // the displayed sub-total value also (if it is possible that the records would not all have
                        // the same prefix/suffix then they should probably not be being sub-totalled).
                        if( !special_value.empty( ) )
                        {
                           if( source.special_prefixed_fields.count( print_total_col_value_ids.at( next_total ) ) )
                              total_string = special_value + " " + total_string;
                           else if( source.special_suffixed_fields.count( print_total_col_value_ids[ next_total ] ) )
                              total_string += " " + special_value;
                        }

                        if( print_total_col_nums.at( next_total ) != x )
                           osxs << "  <td>" << c_nbsp << "</td>\n";
                        else
                        {
                           print_summary_sub_totals[ ( source.print_total_fields.size( ) * k ) + next_total ] = 0;
                           ++next_total;
                           osxs << "  <td class=\"" << class_tag << "\">" << total_string << "</td>\n";
                        }
                     }

                     osxs << "</tr>\n";
                     subtotals.push_front( osxs.str( ) );
                  }

                  if( j != last_j_val || i != source.row_data.size( ) - 1 )
                  {
                     if( row++ % 2 == 0 )
                        osx << "<tr class=\"data\">\n";
                     else
                        osx << "<tr class=\"odd_data\">\n";

                     if( display_list_checks || display_dummy_checks )
                        osx << "  <td class=\"list\" width=\"25\" align=\"center\">" << c_nbsp << "</td>\n";

                     if( display_row_numbers )
                        osx << "  <td class=\"list\" width=\"30\" align=\"center\">" << c_nbsp << "</td>\n";

                     // FUTURE: Rather than using the bold tag a different style tag should
                     // be used for each level (to support different appearances per level).
                     osx << "  <td colspan=\"" << ( source.field_ids.size( ) ) << "\">";
                     if( k > 0 )
                        osx << c_dbl_nbsp << cell_data;
                     else
                        osx << "<b>" << cell_data << "</b>";
                     osx << "</td>\n</tr>\n";
                  }
               }
            }

            if( j == last_j_val && i == source.row_data.size( ) - 1 )
            {
               for( size_t k = 0; k < subtotals.size( ); k++ )
                  final_subtotals.push_back( subtotals[ k ] );
            }
            else
            {
               for( size_t k = 0; k < subtotals.size( ); k++ )
                  os << subtotals[ k ];

               os << osx.str( );
            }
         }

         if( is_first_column )
         {
            is_first_column = false;

            if( row++ % 2 == 0 )
            {
               os << "<tr class=\"" << row_class_suffix;

               if( !extra_effect.empty( ) )
                  os << " " << extra_effect;

               os << "\">\n";
            }
            else
            {
               was_odd = true;
               os << "<tr class=\"odd_" << row_class_suffix;

               if( !extra_effect.empty( ) )
                  os << " odd_" << extra_effect;

               os << "\">\n";
            }

            if( ( keep_checks && selected_records.count( key ) ) || ( rci != source.row_errors.end( ) ) )
               checked = " checked";

            if( display_list_checks || display_dummy_checks )
            {
               os << "   <td class=\"list\" width=\"25\" align=\"center\">";

               if( !display_list_checks
                || sess_info.user_id.empty( ) || ( list_type == c_list_type_home )
                || ( ( state & c_state_undeletable ) && has_delete_action && ( num_sel_actions == 1 ) )
                || ( ( state & c_state_unactionable ) && !extras.count( c_list_type_extra_ignore_unactionable ) ) )
                  os << c_dummy_check_symbol;
               else
               {
                  os << "<input type=\"checkbox\" name=\"item " << key_and_version << "\"" << checked;

                  if( !checked.empty( ) && !source.row_errors.empty( ) )
                     os << " list_id=\"" << source.id << "_table\" list_error=\""
                      << row << "\" onclick=\"remove_table_row_from_checkbox( this );\"";

                  os << "/>";
               }
               os << "</td>\n";
            }

            if( display_row_numbers )
               os << "  <td class=\"list\" width=\"30\" align=\"center\">" << row << "</td>\n";
         }

         if( is_printable && source.print_summary_fields.count( source_value_id ) )
         {
            if( is_fk_column )
               ++fk_column;

            continue;
         }

         if( extras.count( c_list_type_extra_actions )
          && source_field_id == extras.find( c_list_type_extra_actions )->second )
         {
            os << "  <td class=\"list center\">";

            string cmd( c_cmd_list );
            string ident( source.lici->second->id );

            if( !parent_key.empty( ) )
            {
               ident = pident;
               cmd = c_cmd_view;
            }

            string listarg;

            if( is_child_list )
               listarg = source.id;

            if( !is_printable && allow_list_actions
             && !cell_data.empty( ) && !sess_info.is_read_only && !sess_info.user_id.empty( ) )
               output_actions( os, source, cmd, parent_key, sess_info, ident, key_and_version,
                source.lici->second->cid, source.lici->second->mclass, cell_data, "", session_id, user_select_key,
                listarg, using_session_cookie, use_url_checksum, has_hashval, 0, &source.pfield, has_any_changing, back_count );
            else
               os << c_nbsp;

            os << "</td>\n";
         }
         else
         {
            if( state & c_state_is_changing )
               has_any_changing = true;

            // NOTE: Determine whether fields should be protected, relegated or displayed differently according
            // to modifiers and state. It is being assumed here that the original list fields and source fields
            // have the same offsets.
            string extra_effect, display_effect, view_edit_effect;

            if( !is_printable || !extras.count( c_list_type_extra_print_no_highlight ) )
            {
               for( size_t k = 0; k < ARRAY_SIZE( state_modifiers ); k++ )
               {
                  if( state & state_modifiers[ k ] )
                  {
                     pair< modifier_const_iterator, modifier_const_iterator > range
                      = source.lici->second->fields[ j ].modifiers.equal_range( state_modifiers[ k ] );

                     for( modifier_const_iterator ci = range.first; ci != range.second; ++ci )
                     {
                        string new_effect( ci->second );

                        if( ( new_effect == c_modifier_effect_lowlight )
                         || ( new_effect == c_modifier_effect_lowlight1 )
                         || ( new_effect == c_modifier_effect_highlight )
                         || ( new_effect == c_modifier_effect_highlight1 ) )
                           display_effect = new_effect;

                        if( ( new_effect == c_modifier_effect_relegate )
                         || ( view_edit_effect.empty( ) && ( new_effect == c_modifier_effect_protect ) ) )
                           view_edit_effect = new_effect;

                        if( ( new_effect == c_modifier_effect_extralight )
                         || ( new_effect == c_modifier_effect_extralight1 ) )
                           extra_effect = new_effect;
                     }
                  }
               }
            }

            if( view_edit_effect == c_modifier_effect_relegate )
               cell_data.erase( );

            bool is_href = false;
            bool is_image = false;
            bool was_output = false;

            string class_tag( "list" );
            if( display_effect == c_modifier_effect_lowlight )
               class_tag += " lowlight";
            else if( display_effect == c_modifier_effect_lowlight1 )
               class_tag += " lowlight1";
            else if( display_effect == c_modifier_effect_highlight )
               class_tag += " highlight";
            else if( display_effect == c_modifier_effect_highlight1 )
               class_tag += " highlight1";

            if( !extra_effect.empty( ) )
               class_tag += " " + extra_effect;

            if( source.force_right_fields.count( source_value_id ) )
               class_tag += " right";
            else if( source.force_center_fields.count( source_value_id ) )
               class_tag += " center";
            else if( source.force_justify_fields.count( source_value_id ) )
               class_tag += " justify";

            if( source.large_fields.count( source_value_id ) )
               class_tag += " large";
            else if( source.larger_fields.count( source_value_id ) )
               class_tag += " larger";
            else if( source.small_fields.count( source_value_id ) )
               class_tag += " small";
            else if( source.smaller_fields.count( source_value_id ) )
               class_tag += " smaller";

            bool align_top = false;
            if( is_printable && extras.count( c_list_type_extra_print_no_trunc ) )
               align_top = true;

            if( !align_top )
               os << "   <td class=\"" << class_tag << "\"";
            else
               os << "   <td class=\"" << class_tag << "\" style=\"vertical-align:top\"";

            // NOTE: If column appears to be for icons then shrink the size and make it centered.
            if( source.enum_fields.count( source_value_id ) )
            {
               const enum_info& info(
                sinfo.enums.find( source.enum_fields.find( source_value_id )->second )->second );

               if( !info.values.empty( )
                && ( file_exists( info.values[ 0 ].second + ".png" )
                || file_exists( info.values[ 0 ].second + ".gif" ) ) )
               {
                  is_image = true;
                  os << " width=\"25\" align=\"center\"";
               }
            }

            os << ">";

            string view_id( source.view );
            string view_key( key );

            // NOTE: Foreign key field values were appended to the field list and are now extracted
            // so that the hyperlink will be attached to the parent record rather than to the child.
            // If more than one parent field is displayed then only use a hyperlink to the first one
            // (if wanting to hyperlink to all then the ++fk_column approach will have to be changed).
            bool is_foreign_link = false;
            size_t num_fk_fields( source.fk_field_ids.size( ) );

            bool inc_fk_column = false;

            if( fk_column < num_fk_fields && source.fk_field_ids[ fk_column ] == source_field_id )
               inc_fk_column = true;

            if( num_fk_fields && !fk_refs.count( source_field_id )
             && fk_column < num_fk_fields && source.fk_field_ids[ fk_column ] == source_field_id )
            {
               if( !source.non_link_fields.count( source_value_id )
                && !source.non_fk_link_fields.count( source_value_id )
                && ( !source.owner_link_fields.count( source_value_id ) || has_owner_parent ) )
               {
                  is_foreign_link = true;
                  fk_refs.insert( source_field_id );

                  if( mod_info.view_cids.count( source.fk_field_classes.find( source_field_id )->second ) )
                  {
                     view_id = mod_info.view_cids.find( source.fk_field_classes.find( source_field_id )->second )->second;
                     view_key = columns[ source.field_ids.size( ) + fk_column ];
                  }
                  else
                     view_id.erase( );
               }
            }

            // NOTE: Only provide a hyperlink to the first non-fk link permitted field.
            if( !is_foreign_link
             && ( had_link || source.non_link_fields.count( source_value_id )
             || ( source.owner_link_fields.count( source_value_id ) && !has_owner_parent ) ) )
               view_id.erase( );

            if( inc_fk_column )
               ++fk_column;

            // NOTE: Do not allow the hyperlink if the view requires a permission that the user does not have.
            if( !view_id.empty( ) )
            {
               string::size_type pos = type_info.find( ':' );
               if( pos == string::npos )
                  throw runtime_error( "unexpected format for type_info '" + type_info + "'" );

               // NOTE: If the record is actually a class derivation (but not an alias) then
               // the view to link to will be the view for the derived class (if one exists).
               string module = type_info.substr( 0, pos );
               if( module == get_module_id( source.module ) )
               {
                  string class_id = type_info.substr( pos + 1 );

                  if( class_id != source.cid )
                  {
                     view_info_const_iterator vici;
                     for( vici = mod_info.view_info.begin( ); vici != mod_info.view_info.end( ); ++vici )
                     {
                        if( ( vici->second )->cid == class_id )
                        {
                           view_id = vici->first;
                           break;
                        }
                     }
                  }
               }

               view_info_const_iterator vici = mod_info.view_info.find( view_id );
               if( vici == mod_info.view_info.end( ) )
                  throw runtime_error( "unknown view '" + view_id + "' for link" );

               string perm( ( vici->second )->perm );
               if( !perm.empty( ) && perm[ 0 ] == '!' )
                  perm.erase( 0, 1 );

               bool has_perm = false;
               if( ( vici->second )->type == c_view_type_admin )
               {
                  if( sess_info.is_admin_user
                   || ( !sess_info.is_admin_user && !perm.empty( ) && sess_info.user_perms.count( perm ) ) )
                     has_perm = true;
               }
               else if( perm.empty( ) || sess_info.user_perms.count( perm ) )
                  has_perm = true;

               if( !has_perm )
                  view_id.erase( );
            }

            // NOTE: If a foreign key new record list is being used in an exclusive manner then store the key of
            // foreign key records currently in use to prevent them from being added as an option for new records.
            if( ( source.lici->second )->nexclude && source_field_id == ( source.lici->second )->dfield )
               new_rec_fks_in_use.insert( view_key );

            if( !is_printable
             && !source.non_link_fields.count( source_value_id )
             && !is_parent_edit && !cell_data.empty( ) && !view_id.empty( )
             && ( !is_foreign_link || source.link_fields.count( source_value_id ) )
             && ( !sess_info.checkbox_bools || !source.bool_fields.count( source_value_id ) )
             && ( !source.url_fields.count( source_value_id )
             && !source.href_fields.count( source_value_id ) && !source.mailto_fields.count( source_value_id )
             && !source.file_fields.count( source_value_id ) && !source.image_fields.count( source_value_id ) )
             && ( !source.admin_link_fields.count( source_value_id ) || sess_info.is_admin_user ) )
            {
               is_href = true;

               os << "<a href=\""
                << get_module_page_name( source.module_ref ) << "?cmd=" << c_cmd_view << "&data=" << view_key;

               if( !is_foreign_link )
                  had_link = true;

               os << "&ident=" << view_id;

               if( !user_select_key.empty( ) )
                  os << "&" << c_param_uselect << "=" << user_select_key;

               if( !using_session_cookie )
                  os << "&session=" << session_id;

               if( use_url_checksum )
               {
                  string checksum_values( string( c_cmd_view ) + view_key + view_id + user_select_key );
                  os << "&" << c_param_chksum << "=" << get_checksum( sess_info, checksum_values );
               }

               os << "\" onclick=\"javascript:replace_with_scroll_info( true );\">";
            }

            if( !is_printable && !cell_data.empty( ) && ( source.url_fields.count( source_value_id )
             || source.href_fields.count( source_value_id ) || source.mailto_fields.count( source_value_id ) ) )
            {
               string type, extra;

               if( source.href_fields.count( source_value_id ) )
               {
                  if( cell_data.find( "//" ) == string::npos )
                     type = "http://";

                  extra = " target=\"_blank\"";
               }
               else if( source.mailto_fields.count( source_value_id ) )
                  type = "mailto:";

               string display( cell_data );

               if( !display.empty( ) && display[ display.length( ) - 1 ] == '/' )
                  display.erase( display.length( ) - 1 );

               string::size_type pos = display.rfind( '/' );

               if( pos != string::npos )
                  display.erase( 0, pos + 1 );

               is_href = true;
               was_output = true;

               os << "<a href=\"" << type << cell_data << "\"" << extra << ">"
                << data_or_nbsp( escape_markup( unescaped( display ) ) ) << "</a>";
            }

            if( !cell_data.empty( )
             && ( source.file_fields.count( source_value_id )
             || source.image_fields.count( source_value_id ) ) )
            {
               string file_path( string( c_files_directory )
                + "/" + get_module_id_for_attached_file( source ) + "/" + ( source.lici->second )->cid );

               string file_name( file_path + "/" + unescaped( cell_data ) );

               string file_full_ext, file_last_ext;
               string::size_type pos = file_name.find( "." );

               if( pos != string::npos )
                  file_full_ext = file_name.substr( pos + 1 );

               pos = file_name.find_last_of( "." );

               string width( image_width );
               string height( image_height );

               bool do_not_size_image = false;

               if( !source.orientation_field.empty( ) )
               {
                  int orientation = atoi( columns[ orientation_col ].c_str( ) );

                  if( orientation == 1 ) // i.e. Portrait
                     swap( height, width );
                  else if( orientation == 2 ) // i.e. Neither
                     do_not_size_image = true;
               }

               if( file_exists( file_name ) )
               {
                  string tmp_link_path( c_files_directory );
                  tmp_link_path += "/" + string( c_tmp_directory );
                  tmp_link_path += "/" + session_id;

                  bool has_utf8_chars;
                  string link_file_name( display_names[ source_field_id ] );

                  if( filename_col >= 0 )
                  {
                     // NOTE: If an empty filename is found then "file_ext" will end up becoming the name.
                     if( columns[ filename_col ].empty( ) )
                        link_file_name.erase( );
                     else
                     {
                        link_file_name = valid_file_name( columns[ filename_col ], &has_utf8_chars );

                        if( source.file_fields.size( ) + source.image_fields.size( ) > 1
                         && !extras.count( c_list_type_extra_file_links_always_as_single ) )
                           link_file_name += " " + display_names[ source_field_id ];
                     }
                  }

                  // NOTE: For the link file name in a list always append the row number just
                  // in case duplicate names exist.
                  link_file_name += '[' + to_string( i ) + ']';

                  // NOTE: If access is anonymous then no temporary session directory exists so
                  // file link aliasing is not supported for this case.
                  if( sess_info.user_id.empty( ) )
                  {
                     tmp_link_path = file_path;
                     link_file_name = file_name;
                  }
                  else
                  {
                     bool is_encrypted = source.encrypted_fields.count( source_value_id );

                     if( is_encrypted && ignore_encrypted_col >= 0 )
                     {
                        if( columns[ ignore_encrypted_col ] == string( c_true_value ) )
                           is_encrypted = false;
                     }

                     create_tmp_file_link_or_copy( tmp_link_path, file_name, file_full_ext, link_file_name );
                  }

                  if( !is_href && !is_printable
                   && ( !embed_images || source.file_fields.count( source_value_id ) ) )
                  {
                     is_href = true;
                     os << "<a href=\"" << tmp_link_path << "\" target=\"_blank\">";
                  }

                  was_output = true;

                  if( source.file_fields.count( source_value_id ) )
                     os << file_full_ext;
                  else
                  {
                     is_image = true;

                     string image_src( tmp_link_path );
                     if( embed_images )
                     {
                        string buffer( buffer_file( file_name ) );
                        image_src = "data:image/" + file_last_ext + ";base64," + base64::encode( buffer );
                     }

                     if( source.orientation_field.empty( ) )
                     {
                        if( extra_data.count( c_field_extra_portrait ) )
                           swap( height, width );
                        else if( extra_data.count( c_field_extra_neither ) )
                           do_not_size_image = true;
                     }

                     os << "<img src=\"" << image_src;

                     if( !embed_images && !do_not_size_image )
                        os << "\" width=\"" << width << "\" height=\"" << height;

                     os << "\" border=\"0\" alt=\"" << GDS( c_display_image ) << "\">";
                  }
               }
            }

            if( !cell_data.empty( ) && source.qr_code_fields.count( source_value_id ) )
            {
               string temp_file_name( c_files_directory );
               temp_file_name += "/" + string( c_tmp_directory );
               temp_file_name += "/" + session_id;

               ostringstream outs;
               outs << ifmt( 6 ) << row;
               temp_file_name += "/" + outs.str( ) + source_value_id + ".png";

               int qr_pixels = c_default_qr_code_pixels;

               if( source.large_fields.count( source_value_id ) )
                  qr_pixels += 1;
               else if( source.larger_fields.count( source_value_id ) )
                  qr_pixels += 2;
               else if( source.small_fields.count( source_value_id ) )
                  qr_pixels -= 1;
               else if( source.smaller_fields.count( source_value_id ) )
                  qr_pixels -= 2;

               string cmd( "qrencode -o " + temp_file_name
                + " -s " + to_string( qr_pixels ) + " \"" + escaped( cell_data, "\"" ) + "\"" );

               int rc = system( cmd.c_str( ) );
               ( void )rc;

               bool is_href = false;
               if( !embed_images )
               {
                  is_href = true;
                  os << "<a href=\"" << temp_file_name << "\" target=\"_blank\">";
               }

               string image_src( temp_file_name );
               if( embed_images )
               {
                  string buffer( buffer_file( temp_file_name ) );
                  image_src = "data:image/png;base64," + base64::encode( buffer );
               }

               os << "<img src=\"" << image_src;

               os << "\" border=\"0\" alt=\"" << GDS( c_display_image ) << "\">";

               if( is_href )
                  os << "</a>";

               is_image = true;
               was_output = true;
            }

            if( source.enum_fields.count( source_value_id ) )
            {
               const enum_info& info(
                sinfo.enums.find( source.enum_fields.find( source_value_id )->second )->second );

               for( size_t i = 0; i < info.values.size( ); i++ )
               {
                  if( info.values[ i ].first == cell_data )
                  {
                     string enum_image_file;
                     if( file_exists( info.values[ i ].second + ".png" ) )
                        enum_image_file = info.values[ i ].second + ".png";
                     else if( file_exists( info.values[ i ].second + ".gif" ) )
                        enum_image_file = info.values[ i ].second + ".gif";

                     // NOTE: If an image file with the enum value name exists
                     // then display the image rather than the string value.
                     if( !enum_image_file.empty( ) )
                        os << "<img src=\"" << enum_image_file << "\" border=\"0\">";

                     cell_data = get_display_string( info.values[ i ].second );
                     break;
                  }
               }
            }
            else if( source.bool_fields.count( source_value_id ) )
            {
               if( sess_info.checkbox_bools )
               {
                  bool is_checked = false;
                  if( cell_data == c_true_value )
                     is_checked = true;

                  cell_data.erase( );
                  was_output = true;
                  os << "<input disabled type=\"checkbox\"" << ( is_checked ? " checked=\"checked\"" : "" ) << ">";
               }
               else
               {
                  if( cell_data == c_true_value )
                     cell_data = GDS( c_display_true );
                  else if( !cell_data.empty( ) )
                     cell_data = GDS( c_display_false );
                  else // FUTURE: This provides support for a tri-state boolean (currently not in use).
                     cell_data = GDS( c_display_unknown );
               }
            }
            else if( source.int_fields.count( source_value_id ) )
            {
               if( source.int_type_fields.count( source_value_id ) )
               {
                  string int_type = source.int_type_fields.find( source_value_id )->second;

                  if( int_type == "bytes" )
                     cell_data = format_bytes( atoi( cell_data.c_str( ) ) );
                  else if( int_type == "duration_dhm" || int_type == "duration_hms" )
                     cell_data = format_duration( atoi( cell_data.c_str( ) ), ( int_type == "duration_hms" ) );
                  else
                     throw runtime_error( "unsupported int_type '" + int_type + "'" );
               }
               else
               {
                  string mask( c_default_int_mask );
                  if( source.mask_fields.count( source_value_id ) )
                     mask = source.mask_fields.find( source_value_id )->second;

                  cell_data = format_int_value( atoi( cell_data.c_str( ) ), mask );
               }
            }
            else if( source.numeric_fields.count( source_value_id ) )
            {
               if( source.numeric_type_fields.count( source_value_id ) )
               {
                  string numeric_type = source.numeric_type_fields.find( source_value_id )->second;

                  if( numeric_type == "bytes" )
                     cell_data = format_bytes( numeric( cell_data.c_str( ) ).as_uint64( ) );
                  else
                     throw runtime_error( "unsupported numeric_type '" + numeric_type + "'" );
               }
               else
               {
                  string mask( c_default_numeric_mask );
                  if( source.mask_fields.count( source_value_id ) )
                     mask = source.mask_fields.find( source_value_id )->second;

                  cell_data = format_numeric_value( numeric( cell_data.c_str( ) ), mask );
               }
            }
            else if( source.date_fields.count( source_value_id ) )
            {
               if( !cell_data.empty( ) )
               {
                  udate ud( cell_data );

                  string date_precision;
                  if( source.date_precision_fields.count( source_value_id ) )
                     date_precision = source.date_precision_fields.find( source_value_id )->second;

                  cell_data = format_date( ud, date_precision.c_str( ) );
               }
            }
            else if( source.time_fields.count( source_value_id ) )
            {
               if( !cell_data.empty( ) )
               {
                  mtime mt( cell_data );

                  string time_precision;
                  if( source.time_precision_fields.count( source_value_id ) )
                     time_precision = source.time_precision_fields.find( source_value_id )->second;

                  cell_data = format_time( mt, time_precision.c_str( ) );
               }
            }
            else if( source.datetime_fields.count( source_value_id ) )
            {
               if( !cell_data.empty( ) )
               {
                  date_time dt( cell_data );

                  if( sess_info.tz_name.empty( ) )
                     dt += ( seconds )sess_info.gmt_offset;

                  string time_precision;
                  if( source.time_precision_fields.count( source_value_id ) )
                     time_precision = source.time_precision_fields.find( source_value_id )->second;

                  cell_data = format_date_time( dt, time_precision.c_str( ) );
               }
            }
            else if( is_printable && extras.count( c_list_type_extra_print_no_trunc ) )
            {
               was_output = true;
               os << data_or_nbsp( unescaped( replace_crlfs_and_spaces( escape_markup( cell_data ), "<br/>", c_nbsp ) ) );
            }
            else if( source.html_fields.count( source_value_id ) )
            {
               was_output = true;

               if( !cell_data.empty( ) )
                  unescape( cell_data, "rn\r\n" );

               cell_data = replace_display_strings( cell_data );

               if( !text_search_value.empty( )
                && ( extras.count( c_list_type_extra_text_highlight )
                || extras.count( c_list_type_extra_text_highlight1 ) ) )
                  replace_with_search_matches_highlighted( cell_data, text_search_value, extras );

               replace_links_and_output( cell_data, source.view,
                source.module, source.module_ref, os, true, !is_printable,
                session_id, sess_info, user_select_key, using_session_cookie, use_url_checksum, &key );
            }
            else if( source.manual_link_fields.count( source_value_id ) )
            {
               was_output = true;

               replace_links_and_output( cell_data, source.view,
                source.module, source.module_ref, os, false, !is_printable,
                session_id, sess_info, user_select_key, using_session_cookie, use_url_checksum, &key );
            }
            else if( source.text_fields.count( source_value_id ) || source.notes_fields.count( source_value_id ) )
            {
               int character_trunc_limit = sess_info.notes_trunc;

               if( source.notes_character_trunc.count( source_value_id ) )
                  character_trunc_limit = source.notes_character_trunc.find( source_value_id )->second;

               // FUTURE: A "content" type field should be truncated differently (as an actual HTML fragment
               // rathern than as raw text). Careful handling of tags would be required for this (where they
               // have not been closed due to the truncation such as might occur within a <b>bold</b> pair).
               if( !cell_data.empty( ) )
               {
                  unescape( cell_data, "rn\r\n" );

                  if( character_trunc_limit != 1 ) // i.e. none
                     utf8_truncate( cell_data, character_trunc_limit, "..." );
               }

               if( !text_search_value.empty( )
                && ( extras.count( c_list_type_extra_text_highlight )
                || extras.count( c_list_type_extra_text_highlight1 ) ) )
                  replace_with_search_matches_highlighted( cell_data, text_search_value, extras, true );
            }
            else if( source.replace_underbar_fields.count( source_value_id ) )
               cell_data = replace_underbars( cell_data );

            if( !special_value.empty( ) )
            {
               if( source.special_prefixed_fields.count( source_value_id ) )
                  cell_data = special_value + " " + cell_data;
               else if( source.special_suffixed_fields.count( source_value_id ) )
                  cell_data += " " + special_value;
            }

            if( !is_image && !was_output )
               os << data_or_nbsp( escape_markup( unescaped( cell_data ) ) );

            if( is_href )
               os << "</a>";

            os << "</td>\n";
         }
      }

      os << "</tr>\n";

      if( !checked.empty( ) && rci != source.row_errors.end( ) )
      {
         string error( rci->second );

         bool is_real_error = false;

         // NOTE: The "row_errors" container is also used to store non-error action response messages
         // so need to differentiate between them here in order to display them in the correct manner.
         if( error.length( ) > strlen( c_response_error_prefix )
          && error.substr( 0, strlen( c_response_error_prefix ) ) == c_response_error_prefix )
         {
            is_real_error = true;
            error = string( GDS( c_display_error ) ) + ": " + remove_key( error.substr( strlen( c_response_error_prefix ) ) );
         }

         if( !was_odd )
            os << "<tr class=\"error\">\n";
         else
            os << "<tr class=\"odd_error\">\n";

         os << "  <td width=\"25\">" << c_nbsp << "</td>\n";

         if( !is_real_error )
            os << "  <td class=\"list\" colspan=\"" << ( source.field_ids.size( ) ) << "\">" << error << "</td>\n";
         else
            os << "  <td class=\"error\" colspan=\"" << ( source.field_ids.size( ) ) << "\">" << error << "</td>\n";

         os << "</tr>\n";
      }
   }

   os << "</tbody>\n";

   if( is_printable )
   {
      for( size_t i = 0; i < final_subtotals.size( ); i++ )
         os << final_subtotals[ i ] << endl;

      os << "<tfoot>\n";

      int extra = 0;
      if( print_list_opts.count( c_list_print_opt_show_numbers ) )
         extra++;

      if( !extras.count( c_list_type_extra_no_totals ) )
      {
         os << "<tr class=\"footer\">\n";

         if( display_list_checks || display_dummy_checks )
            os << "  <td>" << c_nbsp << "</td>\n";

         if( print_total_values.empty( ) )
            os << "  <td colspan=\"" << ( total_display_cols + extra )
             << "\"><b>" << GDS( c_display_total ) << "</b>" << c_dbl_nbsp
             << source.row_data.size( ) << " " << GDS( c_display_records ) << "</td>\n";
         else
         {
            int next_total = 0;

            if( !print_total_col_nums.empty( ) )
               os << "  <td><b>" << GDS( c_display_total ) << "</b>" << c_dbl_nbsp
                << source.row_data.size( ) << " " << GDS( c_display_records ) << "</td>\n";

            if( extra )
               os << "  <td>" << c_nbsp << "</td>\n";

            for( int i = 1; i < total_display_cols; i++ )
            {
               if( next_total >= print_total_col_nums.size( ) || print_total_col_nums.at( next_total ) != i )
                  os << "  <td>" << c_nbsp << "</td>\n";
               else
               {
                  string class_tag( "list" );
                  if( source.force_right_fields.count( print_total_col_value_ids.at( next_total ) ) )
                     class_tag += " right";
                  else if( source.force_center_fields.count( print_total_col_value_ids.at( next_total ) ) )
                     class_tag += " center";
                  else if( source.force_justify_fields.count( print_total_col_value_ids.at( next_total ) ) )
                     class_tag += " justify";

                  numeric total = print_total_values.at( next_total );

                  string total_string( total.as_string( ) );

                  if( source.pmask_fields.count( print_total_col_value_ids.at( next_total ) ) )
                     total_string = format_numeric_value( total, source.pmask_fields.find( print_total_col_value_ids[ next_total ] )->second );

                  // NOTE: See the note that applies to sub-totals and special prefix/suffix values above.
                  if( !special_value.empty( ) )
                  {
                     if( source.special_prefixed_fields.count( print_total_col_value_ids.at( next_total ) ) )
                        total_string = special_value + " " + total_string;
                     else if( source.special_suffixed_fields.count( print_total_col_value_ids[ next_total ] ) )
                        total_string += " " + special_value;
                  }

                  ++next_total;
                  os << "  <td class=\"" << class_tag << "\">" << total_string << "</td>\n";
               }
            }
         }
         os << "</tr>\n";
      }

      if( source.print_limited )
      {
         os << "<tr class=\"footer\">\n";

         if( display_list_checks || display_dummy_checks )
            os << "  <td>" << c_nbsp << "</td>\n";

         os << "  <td class=\"center\" colspan=\"" << ( total_display_cols + extra )
          << "\">(" << GDS( c_display_output_has_been_limited ) << ")</td>\n";
         os << "</tr>\n";
      }

      os << "</tfoot>\n";
   }

   os << "</table>\n";

   if( !is_printable )
   {
      if( !is_ui_prototype( ) )
      {
         os << "<table class=\"list\" width=\"100%\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\">\n";
         if( row % 2 == 0 )
            os << "<tr>\n";
         else
            os << "<tr class=\"odd\">\n";
      }

      bool allow_scroll = !( ( source.lici->second )->extras.count( c_list_type_extra_no_limit )
       || ( source.lici->second )->extras.count( c_list_type_extra_sort_no_limit ) );

      if( ( source.lici->second )->extras.count( c_list_type_extra_fixed ) )
      {
         allow_scroll = false;

         if( ( source.lici->second )->extras.count( c_list_type_extra_limit ) )
         {
            size_t row_limit = atoi( ( source.lici->second )->extras.find( c_list_type_extra_limit )->second.c_str( ) );

            if( row_limit && ( source.row_data.size( ) >= row_limit ) )
               allow_new_record = false;
         }
      }

      if( ( source.lici->second )->extras.count( c_list_type_extra_new_if_default_other ) )
      {
         // NOTE: As "user other" is not applicable to the "admin" user simply do not permit
         // record creation for "admin" if this list option was found.
         if( sess_info.is_admin_user )
            allow_new_record = false;
         else if( allow_new_record )
            allow_new_record = sess_info.is_default_other( );
      }

      if( is_ui_prototype( ) )
      {
         os << "</tr>\n";
         os << "</table>\n";

         if( row % 2 == 0 )
            os << "<div class=\"width-fix firstrow lastring stylebottom\">\n";
         else
            os << "<div class=\"width-fix firstrow lastring stylebottom odd-row\">\n";
      }

      if( !allow_new_record )
      {
         if( !is_ui_prototype( ) )
            os << "  <td>" << c_nbsp << "</td>\n";
      }
      else
      {
         if( !is_ui_prototype( ) )
            os << "  <td>";
         else
            os << "<div id=\"newrecord-view\">\n";

         if( !( source.lici->second )->dfield.empty( ) && ( source.lici->second )->dvalue.empty( ) )
         {
            if( !is_ui_prototype( ) )
               os << "<select name=\"new_record\"";
            else
               os << "<select class=\"newrecord_select\" name=\"new_record\"";

            os << " onchange=\"sel_new_loc( document." << source.id << ".new_record );\">\n";

            os << "<option value=\"\" disabled=\"disabled\" selected=\"selected\">" << GDS( c_display_create_new ) << "</option>\n";

            for( size_t i = 0; i < source.new_record_list.size( ); i++ )
            {
               string key_and_ver_info( source.new_record_list[ i ].first );
               string::size_type pos = key_and_ver_info.find( ' ' );

               string key( key_and_ver_info.substr( 0, pos ) );

               if( new_rec_fks_in_use.count( key ) )
                  continue;

               vector< string > columns;
               raw_split( source.new_record_list[ i ].second, columns );

               string view_id( source.view );

               if( source.new_record_list_has_view_id && !columns[ 1 ].empty( ) )
                  view_id = columns[ 1 ];

               view_info_const_iterator vici = mod_info.view_info.find( view_id );

               if( vici == mod_info.view_info.end( ) )
                  throw runtime_error( "unknown view '" + view_id + "'" );

               const map< string, string >& view_extras( vici->second->extras );

               string new_data( c_new_record );

               bool is_clone = false;

               if( ( source.lici->second )->nclass == ( source.lici->second )->cid )
               {
                  new_data += key;
                  is_clone = true;
               }

               os << "<option value=\"" << get_module_page_name( source.module_ref ) << "?cmd=" << c_cmd_view << "&data=" << new_data;

               if( !source.new_pfield.empty( )
                && !view_extras.count( c_view_type_extra_ignore_parent_record ) )
                  os << "&extra=" << source.new_pkey << "&field=" << source.new_pfield;

               os << "&ident=" << view_id;

               if( !is_clone )
               {
                  string new_fields( new_record_fields );
                  string new_values( new_record_values );

                  if( !new_fields.empty( ) )
                  {
                     new_fields += ",";
                     new_values += ",";
                  }

                  new_fields += ( source.lici->second )->dfield;
                  new_values += key;

                  os << "&" << c_param_newflds << "=" << new_fields << "&" << c_param_newvals << "=" << new_values;
               }

               if( !user_select_key.empty( ) )
                  os << "&" << c_param_uselect << "=" << user_select_key;

               if( !using_session_cookie )
                  os << "&session=" << session_id;

               string checksum_values( string( c_cmd_view )
                + new_data + view_id + user_select_key + to_string( sess_info.checksum_serial ) );

               os << "&chksum=" << get_checksum( sess_info, checksum_values );

               os << "\">" << columns[ 0 ] << "</option>\n";
            }

            os << "</select>";
         }
         else
         {
            os << "<a class=\"newrecord\" href=\""
             << get_module_page_name( source.module_ref )
             << "?cmd=" << c_cmd_view << "&data=" << string( c_new_record );

            view_info_const_iterator vici = mod_info.view_info.find( source.view );
            if( vici == mod_info.view_info.end( ) )
               throw runtime_error( "unknown view '" + source.view + "'" );

            const map< string, string >& view_extras( vici->second->extras );

            if( !source.new_pfield.empty( ) && !view_extras.count( c_view_type_extra_ignore_parent_record ) )
               os << "&extra=" << source.new_pkey << "&field=" << source.new_pfield;

            os << "&ident=" << source.view << "&serial=" << to_string( sess_info.checksum_serial );

            if( !( source.lici->second )->dvalue.empty( ) )
            {
               if( !new_record_fields.empty( ) )
               {
                  new_record_fields += ",";
                  new_record_values += ",";
               }

               new_record_fields += ( source.lici->second )->dfield;
               new_record_values += ( source.lici->second )->dvalue;
            }

            if( !new_record_fields.empty( ) )
               os << "&" << c_param_newflds << "=" << new_record_fields
                << "&" << c_param_newvals << "=" << new_record_values;

            if( !user_select_key.empty( ) )
               os << "&" << c_param_uselect << "=" << user_select_key;

            if( !using_session_cookie )
               os << "&session=" << session_id;

            string checksum_values( string( c_cmd_view )
             + " " + source.view + user_select_key + to_string( sess_info.checksum_serial ) );

            os << "&chksum=" << get_checksum( sess_info, checksum_values );

            os << "\">" << GDS( c_display_create_new ) << "</a>";
         }

         if( !is_ui_prototype( ) )
            os << "</td>\n";
         else
            os << "</div>\n";
      }

      if( allow_scroll )
      {
         string checksum_values;
         string new_checksum_value;

         if( use_url_checksum )
         {
            checksum_values = ( is_child_list ? string( c_cmd_view )
             : string( c_cmd_list ) ) + parent_key + ( pident.empty( ) ? oident : pident ) + user_select_key;

            if( has_hashval )
               checksum_values += c_hash_suffix;

            new_checksum_value = get_checksum( sess_info, checksum_values );
         }

         if( !is_ui_prototype( ) )
            os << "  <td class=\"right\">";
         else
            os << "<div class=\"navigation\">\n";

         if( !source.prev_key_info.empty( ) 
          || !source.next_key_info.empty( ) )
         {
            if( !source.prev_key_info.empty( ) )
            {
               os << "&laquo; <a href=\"javascript:";

               if( is_child_list )
                  os << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";

               if( use_url_checksum )
               {
                  os << "query_update( '" << c_param_uselextra << "', '', true ); ";
                  os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
               }

               os << "query_update( '" << source.id << c_info_suffix
                << "', 'P" << source.prev_key_info << "' );\">" << GDS( c_display_last ) << "</a>";
            }
            else
               os << "<span class=\"disabled\">&laquo; " << GDS( c_display_last ) << "</span>";

            os << " " << c_nbsp << " | " << c_nbsp << " ";

            if( !source.next_key_info.empty( ) )
            {
               os << "<a href=\"javascript:";

               if( is_child_list )
                  os << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";

               if( use_url_checksum )
               {
                  os << "query_update( '" << c_param_uselextra << "', '', true ); ";
                  os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
               }

               os << "query_update( '" << source.id << c_info_suffix
                << "', 'N" << source.next_key_info << "' );\">" << GDS( c_display_next ) << "</a>\n";

               os << " &raquo;";
            }
            else
               os << "<span class=\"disabled\">" << GDS( c_display_next ) << " &raquo;</span>\n";
         }

         if( !is_ui_prototype( ) )
            os << "</td>\n";
         else
            os << "</div>\n";
      }

      if( is_ui_prototype( ) )
         os << "</div>\n";
      else
      {
         os << "</tr>\n";
         os << "</table>\n";
      }
   }

   os << "</form>";

   if( is_child_list && !is_ui_prototype( ) )
      os << "</td></tr></table>\n";

   os << "\n";
}

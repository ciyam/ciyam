// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
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

#include "fcgi_view.h"

#include "base64.h"
#include "format.h"
#include "numeric.h"
#include "date_time.h"
#include "fcgi_info.h"
#include "utilities.h"
#include "ciyam_core.h"
#include "fcgi_utils.h"
#include "crypt_stream.h"
#include "ciyam_interface.h"

//#define OMIT_QRCODE_SCANNER

using namespace std;

namespace
{

#include "ciyam_constants.h"

const char* const c_nbsp = "&nbsp;";

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

void output_view_tabs( ostream& os, const view_source& source,
 const session_info& sess_info, int vtab_num, bool is_in_edit, const string& data,
 const string& user_select_key, bool use_url_checksum, const string& cont_act, int back_count,
 bool is_new_record, bool is_record_owner, bool is_user_class_view )
{
   const storage_info& sinfo( get_storage_info( ) );

   const module_info& mod_info( *sinfo.modules_index.find( source.module )->second );

   os << "<tr><td colspan=\"2\">\n";
   os << "<table class=\"nonchildtabs\">\n";

   os << "<thead>\n";
   os << "<tr>\n";

   for( size_t i = 0; i < source.tab_names.size( ); i++ )
   {
      map< string, string > extra_data;

      if( !source.tab_extras[ i ].empty( ) )
         parse_field_extra( source.tab_extras[ i ], extra_data );

      if( sess_info.user_id.empty( )
       && extra_data.count( c_field_extra_no_anon ) )
         continue;

      if( !sess_info.is_admin_user
       && has_perm_extra( c_field_extra_admin_only, extra_data, sess_info ) )
         continue;

      // NOTE: Although "no_anon" would generally not make any sense for "admin_only"
      // access but it is being used as a special case to prevent the output of a tab
      // for the actual "admin" user record.
      if( sess_info.is_admin_user
       && is_user_class_view && ( data == c_admin )
       && extra_data.count( c_field_extra_no_anon )
       && has_perm_extra( c_field_extra_admin_only, extra_data, sess_info ) )
         continue;

      if( !is_new_record && !is_record_owner
       && has_perm_extra( c_view_field_extra_owner_only, extra_data, sess_info ) )
         continue;

      if( !is_new_record && !sess_info.is_admin_user && !is_record_owner
       && has_perm_extra( c_view_field_extra_admin_owner_only, extra_data, sess_info ) )
         continue;

      if( i + 1 == vtab_num )
         os << "<td class=\"selected_tab\" align=\"center\">" << mod_info.get_string( source.tab_names[ i ] ) << "</td>\n";
      else
      {
         if( !is_in_edit )
         {
            os << "<td class=\"tab\" align=\"center\"><a href=\"javascript:";

            if( use_url_checksum )
            {
               string checksum_values( string( c_cmd_view ) + data + source.vici->second->id + user_select_key );

               string new_checksum_value( get_checksum( sess_info, checksum_values ) );

               os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
            }

            os << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";
            os << "query_update( 'vtab', '" << ( i + 1 )
             << "' );\">" << mod_info.get_string( source.tab_names[ i ] ) << "</a></td>\n";
         }
         else
         {
            os << "<td class=\"tab\" align=\"center\"><a href=\"#\" onclick=\"javascript:";

            if( use_url_checksum )
            {
               string checksum_values( string( c_cmd_view ) + data + source.vici->second->id + user_select_key );

               string new_checksum_value( get_checksum( sess_info, checksum_values ) );

               os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
            }

            os << "query_update( 'vtab', '" << ( i + 1 ) << "', true ); ";

            os << "dyn_load( null, 'act="
             << cont_act << "&app=' + get_all_field_values( document." << source.id << " ), false ); ";

            os << "event.returnValue = false; return false;";

            os << "\">" << mod_info.get_string( source.tab_names[ i ] ) << "</a></td>\n";
         }
      }
   }

   os << "</tr>\n";
   os << "</thead>\n";

   os << "</table>\n";
   os << "</tr>\n";
}

bool is_not_accessible( bool is_in_edit, bool is_new_record,
 const map< string, string >& extra_data, const session_info& sess_info )
{
   return ( !is_in_edit
    && has_perm_extra( c_view_field_extra_non_view, extra_data, sess_info ) )
    || ( is_in_edit
    && has_perm_extra( c_view_field_extra_view_only, extra_data, sess_info ) )
    || ( !is_new_record
    && has_perm_extra( c_view_field_extra_new_only, extra_data, sess_info ) )
    || ( is_new_record
    && has_perm_extra( c_view_field_extra_non_new, extra_data, sess_info ) )
    || ( ( is_new_record || !is_in_edit )
    && has_perm_extra( c_view_field_extra_edit_only, extra_data, sess_info ) )
    || ( !sess_info.is_admin_user
    && has_perm_extra( c_field_extra_admin_only, extra_data, sess_info ) );
}

}

void setup_view_fields( view_source& view,
 const view_info& vinfo, const module_info& mod_info,
 const session_info& sess_info, const string& ident, const set< string >& login_opts,
 const string& module_id, const string& module_ref, bool is_in_edit, bool is_new_record )
{
   view.sid = vinfo.id;
   view.cid = vinfo.cid;
   view.name = vinfo.name;
   view.type = vinfo.type;
   view.perm = vinfo.perm;
   view.mclass = vinfo.mclass;
   view.bclass = vinfo.bclass;
   view.module = vinfo.module;

   view.module_id = module_id;
   view.module_ref = module_ref;

   view.has_quick_link = false;

   view.pdf_spec_name = vinfo.pdf_spec;

   view.filename_field = vinfo.filename_field_id;

   for( size_t i = 0; i < vinfo.tabs.size( ); i++ )
   {
      view.tab_names.push_back( mod_info.get_string( ident + "_" + vinfo.tabs[ i ].first ) );
      view.tab_extras.push_back( vinfo.tabs[ i ].second );
   }

   map< string, size_t > field_id_counts;

   for( size_t i = 0; i < vinfo.fields.size( ); i++ )
   {
      fld_info fld( vinfo.fields[ i ] );

      if( !field_id_counts.count( fld.field ) )
         field_id_counts[ fld.field ] = 0;
      else
      {
         map< string, string > extra_data;
         if( !fld.extra.empty( ) )
            parse_field_extra( fld.extra, extra_data );

         if( !extra_data.count( c_field_extra_hidden ) )
            field_id_counts[ fld.field ] = field_id_counts[ fld.field ] + 1;
      }
   }

   int key_field_index = -1;

   string key_field_display_name;

   for( size_t i = 0; i < vinfo.fields.size( ); i++ )
   {
      fld_info fld( vinfo.fields[ i ] );

      string field_id( fld.field );

      string value_id( field_id );

      if( !fld.pfield.empty( ) )
         value_id += "." + fld.pfield;

      map< string, string > extra_data;

      if( !fld.extra.empty( ) )
         parse_field_extra( fld.extra, extra_data );

      if( field_id != c_key_field )
      {
         if( fld.ftype == c_field_type_bool )
            view.bool_fields.insert( value_id );
         else if( fld.ftype == c_field_type_date || fld.ftype == c_field_type_tdate || fld.ftype == c_field_type_udate )
            view.date_fields.insert( value_id );
         else if( fld.ftype == c_field_type_time || fld.ftype == c_field_type_mtime || fld.ftype == c_field_type_ttime )
            view.time_fields.insert( value_id );
         else if( fld.ftype == c_field_type_datetime
          || fld.ftype == c_field_type_tdatetime || fld.ftype == c_field_type_date_time )
            view.datetime_fields.insert( value_id );
         else if( fld.ftype == c_field_type_int )
            view.int_fields.insert( value_id );
         else if( fld.ftype == c_field_type_numeric )
            view.numeric_fields.insert( value_id );

         if( extra_data.count( c_field_extra_left ) )
            view.force_left_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_center ) )
            view.force_center_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_justify ) )
            view.force_justify_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_right ) )
            view.force_right_fields.insert( value_id );

         if( extra_data.count( c_field_extra_html ) )
            view.html_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_text ) )
            view.text_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_notes ) )
            view.notes_fields.insert( value_id );

         if( extra_data.count( c_view_field_extra_upper ) )
            view.upper_fields.insert( value_id );

         if( extra_data.count( c_view_field_extra_actions )
          || extra_data.count( c_view_field_extra_key_name )
          || has_perm_extra( c_field_extra_hidden, extra_data, sess_info )
          || is_not_accessible( is_in_edit, is_new_record, extra_data, sess_info ) )
            view.hidden_fields.insert( value_id );

         if( extra_data.count( c_field_extra_special ) )
            view.special_field = value_id;

         if( extra_data.count( c_field_extra_filename ) )
            view.filename_field = value_id;

         if( has_perm_extra( c_field_extra_link, extra_data, sess_info ) )
            view.link_fields.insert( value_id );

         if( has_perm_extra( c_field_extra_owner_link, extra_data, sess_info ) )
         {
            view.link_fields.insert( value_id );
            view.owner_link_fields.insert( value_id );
         }

         if( extra_data.count( c_field_extra_admin_link ) )
         {
            view.link_fields.insert( value_id );

            if( !sess_info.is_admin_user
             && has_perm_extra( c_field_extra_admin_link, extra_data, sess_info ) )
               view.admin_link_fields.insert( value_id );
         }

         if( extra_data.count( c_field_extra_admin_owner_link ) )
         {
            view.link_fields.insert( value_id );

            if( !sess_info.is_admin_user
             && has_perm_extra( c_field_extra_admin_owner_link, extra_data, sess_info ) )
               view.owner_link_fields.insert( value_id );
         }

         if( extra_data.count( c_field_extra_encrypted ) )
            view.encrypted_fields.insert( value_id );

         if( extra_data.count( c_field_extra_field_hash ) )
            view.field_hash_fields.insert( value_id );

         if( extra_data.count( c_field_extra_manual_link ) )
            view.manual_link_fields.insert( value_id );

         if( extra_data.count( c_field_extra_ignore_links ) )
            view.manual_link_ignores.insert( value_id );

         if( fld.pclass.empty( ) )
         {
            // NOTE: It is expected that only one actions and hpassword salt field will exist in a view.
            if( extra_data.count( c_view_field_extra_actions )
             && ( extra_data.count( c_field_extra_hidden )
             || !has_perm_extra( c_field_extra_hidden, extra_data, sess_info ) ) )
            {
               view.actions_field = field_id;
               view.actions_extra = fld.extra;
            }

            if( extra_data.count( c_view_field_extra_hpassword_salt ) )
               view.hpassword_salt_field = field_id;

            if( extra_data.count( c_view_field_extra_password ) )
               view.password_fields.insert( value_id );
            else if( extra_data.count( c_view_field_extra_hpassword ) )
               view.hpassword_fields.insert( value_id );

            if( is_new_record && extra_data.count( c_view_field_extra_new_value ) )
            {
               string value( extra_data[ c_view_field_extra_new_value ] );

               if( value == c_parent_extra_user )
                  value = sess_info.user_key;
               else if( value == c_parent_extra_group )
                  value = sess_info.user_group;
               else if( value == c_parent_extra_other )
                  value = sess_info.user_other;
               else
               {
                  string::size_type pos = value.find( '=' );
                  if( pos != string::npos && value.substr( 0, pos ) == c_extkey )
                     value = get_extkey( value.substr( pos + 1 ).c_str( ) );
               }

               view.new_field_values.insert( make_pair( field_id, value ) );
            }
         }

         if( extra_data.count( c_view_field_extra_owning_user ) )
            view.owning_user_field = field_id;

         if( has_perm_extra( c_view_field_extra_no_edit, extra_data, sess_info )
          || ( !is_new_record && has_perm_extra( c_view_field_extra_new_edit, extra_data, sess_info ) )
          || ( is_new_record && has_perm_extra( c_view_field_extra_edit_edit, extra_data, sess_info ) )
          || ( !sess_info.is_admin_user && has_perm_extra( c_view_field_extra_admin_edit, extra_data, sess_info ) ) )
            view.protected_fields.insert( value_id );

         if( extra_data.count( c_field_extra_non_print ) )
            view.non_print_fields.insert( value_id );

         if( extra_data.count( c_field_extra_print_only ) )
            view.print_only_fields.insert( value_id );

         if( extra_data.count( c_field_extra_large ) )
            view.large_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_larger ) )
            view.larger_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_small ) )
            view.small_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_smaller ) )
            view.smaller_fields.insert( value_id );

         if( extra_data.count( c_field_extra_url ) )
            view.url_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_href ) )
            view.href_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_file ) || extra_data.count( c_field_extra_flink ) )
         {
            if( !sess_info.user_id.empty( ) || get_storage_info( ).embed_images )
               view.file_fields.insert( value_id );
            else
               view.hidden_fields.insert( value_id );
         }
         else if( extra_data.count( c_field_extra_image ) )
         {
            if( !sess_info.user_id.empty( ) || get_storage_info( ).embed_images )
               view.image_fields.insert( value_id );
            else
               view.hidden_fields.insert( value_id );
         }
         else if( extra_data.count( c_field_extra_mailto ) )
            view.mailto_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_qr_code ) )
            view.qr_code_fields.insert( value_id );

         if( extra_data.count( c_field_extra_file ) || extra_data.count( c_field_extra_image ) )
         {
            if( !sess_info.user_id.empty( ) )
               view.has_file_attachments = true;
         }

         if( fld.mandatory )
            view.mandatory_fields.insert( value_id );

         if( extra_data.count( c_field_extra_uom ) )
            view.uom_fields.insert( make_pair( value_id, extra_data[ c_field_extra_uom ] ) );

         if( extra_data.count( c_view_field_extra_force ) )
            view.user_force_fields.push_back( field_id );

         if( extra_data.count( c_view_field_extra_forced ) )
            view.server_forced_fields.push_back( field_id );

         if( extra_data.count( c_view_field_extra_qr_scan ) )
            view.qr_scan_field = field_id;

         if( extra_data.count( c_view_field_extra_key_suffix ) )
            view.key_suffix_field = field_id;

         if( extra_data.count( c_view_field_extra_qr_scan_key ) )
            view.qr_scan_key_field = field_id;

         if( extra_data.count( c_view_field_extra_key_from_hash ) )
            view.key_from_hash_field = field_id;

         if( fld.pclass.empty( ) )
         {
            if( extra_data.count( c_field_extra_enum ) )
               view.enum_fields.insert( make_pair( value_id, extra_data[ c_field_extra_enum ] ) );

            if( extra_data.count( c_field_extra_defcurrent ) )
               view.defcurrent_fields.insert( value_id );

            if( extra_data.count( c_field_extra_defcurrentyear ) )
               view.defcurrentyear_fields.insert( value_id );

            // NOTE: Only one permission field is expected to exist in a view.
            if( extra_data.count( c_field_extra_permission ) )
            {
               view.permission_field = field_id;
               view.hidden_fields.insert( value_id );
            }

            // NOTE: Only one orientation field is expected to exist in a view.
            if( extra_data.count( c_field_extra_orientation ) )
               view.orientation_field = field_id;

            // NOTE: Only one security level field is expected to exist in a view.
            if( extra_data.count( c_field_extra_security_level ) )
               view.security_level_field = field_id;

            // NOTE: Only one ignore encrypted field is expected to exist in a view.
            if( extra_data.count( c_field_extra_ignore_encrypted ) )
               view.ignore_encrypted_field = field_id;

            // NOTE: Only one is effective owner field is expected to exist in a view.
            if( extra_data.count( c_field_extra_is_effective_owner ) )
               view.is_effective_owner_field = field_id;

            if( extra_data.count( c_field_extra_replace_underbars ) )
               view.replace_underbar_fields.insert( value_id );

            // NOTE: Only one create/modify datetime field is expected to exist in a view.
            if( extra_data.count( c_view_field_extra_create_datetime ) )
               view.create_datetime_field = field_id;

            if( extra_data.count( c_view_field_extra_modify_datetime ) )
               view.modify_datetime_field = field_id;

            if( extra_data.count( c_view_field_extra_enum_filter ) )
               view.enum_filter_fields.insert( make_pair( extra_data[ c_view_field_extra_enum_filter ], value_id ) );

            if( extra_data.count( c_view_field_extra_key0 ) )
               view.key_ids.id0 = field_id;
            else if( extra_data.count( c_view_field_extra_key1 ) )
               view.key_ids.id1 = field_id;
            else if( extra_data.count( c_view_field_extra_key2 ) )
               view.key_ids.id2 = field_id;
            else if( extra_data.count( c_view_field_extra_key3 ) )
               view.key_ids.id3 = field_id;
            else if( extra_data.count( c_view_field_extra_key4 ) )
               view.key_ids.id4 = field_id;
            else if( extra_data.count( c_view_field_extra_key5 ) )
               view.key_ids.id5 = field_id;
            else if( extra_data.count( c_view_field_extra_key6 ) )
               view.key_ids.id6 = field_id;
            else if( extra_data.count( c_view_field_extra_key7 ) )
               view.key_ids.id7 = field_id;
            else if( extra_data.count( c_view_field_extra_key8 ) )
               view.key_ids.id8 = field_id;
            else if( extra_data.count( c_view_field_extra_key9 ) )
               view.key_ids.id9 = field_id;
         }
         else
         {
            view.fk_field_ids.push_back( field_id );
            view.fk_field_classes[ field_id ] = fld.pclass;

            // NOTE: Only one create/modify user field is expected to exist in a view.
            if( extra_data.count( c_view_field_extra_create_user_key ) )
               view.create_user_key_field = field_id;

            if( extra_data.count( c_view_field_extra_modify_user_key ) )
               view.modify_user_key_field = field_id;

            if( extra_data.count( c_view_field_extra_fkey0 ) )
               view.fkey_ids.id0 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey1 ) )
               view.fkey_ids.id1 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey2 ) )
               view.fkey_ids.id2 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey3 ) )
               view.fkey_ids.id3 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey4 ) )
               view.fkey_ids.id4 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey5 ) )
               view.fkey_ids.id5 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey6 ) )
               view.fkey_ids.id6 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey7 ) )
               view.fkey_ids.id7 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey8 ) )
               view.fkey_ids.id8 = field_id;
            else if( extra_data.count( c_view_field_extra_fkey9 ) )
               view.fkey_ids.id9 = field_id;
         }

         if( extra_data.count( c_view_field_extra_vextra1 ) )
            view.vextra1_id = field_id;
         else if( extra_data.count( c_view_field_extra_vextra2 ) )
            view.vextra2_id = field_id;

         if( !login_opts.count( c_login_opt_allow_multiple )
          && extra_data.count( c_view_field_extra_quick_link ) )
         {
            view.has_quick_link = true;
            view.quick_link_value_id = value_id;
         }
      }

      view.field_ids.push_back( field_id );
      view.field_tab_ids.push_back( fld.tab_id );

      if( fld.pfield.empty( ) )
         view.value_ids.push_back( field_id );
      else
         view.value_ids.push_back( value_id );

      bool child_always = false;

      if( extra_data.count( c_field_extra_child_always ) )
         child_always = true;

      bool non_prefixed = false;

      if( extra_data.count( c_field_extra_non_prefixed ) )
         non_prefixed = true;

      string display_name( get_display_string( fld.name ) );

      if( child_always || field_id_counts[ field_id ] > 0 )
      {
         if( child_always || non_prefixed )
            display_name = get_display_string( fld.pfname );
         else
            display_name += " " + get_display_string( fld.pfname );
      }

      view.display_names.push_back( display_name );

      if( field_id == c_key_field )
         key_field_index = view.display_names.size( ) - 1;

      if( extra_data.count( c_view_field_extra_key_name ) )
      {
         key_field_display_name = display_name;

         // NOTE: If is both "key name" and "new_only" then when is not new will hide the key field.
         if( !is_new_record && has_perm_extra( c_view_field_extra_new_only, extra_data, sess_info ) )
            view.hidden_fields.insert( c_key_field );
      }

      string display_name_for_edit( display_name );

      if( !fld.pdname.empty( ) )
      {
         if( non_prefixed )
            display_name_for_edit = get_display_string( fld.pdname );
         else
            display_name_for_edit = get_display_string( fld.name ) + " " + get_display_string( fld.pdname );
      }

      view.edit_display_names.push_back( display_name_for_edit );

      if( fld.pclass == view.cid )
         view.self_relationships.insert( field_id );
   }

   if( ( key_field_index >= 0 ) && !key_field_display_name.empty( ) )
   {
      view.display_names[ key_field_index ] = key_field_display_name;
      view.edit_display_names[ key_field_index ] = key_field_display_name;
   }

   view.root_folder = vinfo.static_instance_key;
}

bool output_view_form( ostream& os, const string& act,
 bool was_invalid, const map< string, string >& user_values,
 const string& exec, const string& cont, const string& data,
 const string& error_message, const string& extra, const string& pfield,
 const view_source& source, int vtab_num, const string& session_id,
 const string& user_select_key, bool using_session_cookie, bool embed_images,
 const map< string, string >& new_field_and_values, const session_info& sess_info,
 string& field_list, string& edit_timeout_func, string& extra_content_func, bool use_url_checksum,
 bool is_quick_link_view, const map< string, string >& show_opts, bool is_printable, int back_count,
 const string& pdf_view_file_name, bool& is_record_owner, string& extra_html_content, bool& is_changing )
{
   const storage_info& sinfo( get_storage_info( ) );

   const module_info& mod_info( *sinfo.modules_index.find( source.module )->second );

   bool is_new_record = false;

   if( !data.empty( ) && ( data[ 0 ] == ' ' ) )
      is_new_record = true;

   bool is_in_edit = false;

   if( ( act == c_act_edit ) || ( is_new_record && ( act.empty( ) || ( act == c_act_undo ) ) ) )
      is_in_edit = true;

   bool is_user_class_view = ( mod_info.user_class_id == source.cid );

   bool is_editable = !is_in_edit;

   string cont_act = ( is_in_edit ? c_act_cont : c_act_view );

   const map< string, string >& view_extras( source.vici->second->extras );

   bool is_no_edit = has_perm_extra( c_view_type_extra_no_edit, view_extras, sess_info );
   bool is_no_print = has_perm_extra( c_view_type_extra_no_print, view_extras, sess_info );
   bool is_admin_edit = has_perm_extra( c_view_type_extra_admin_edit, view_extras, sess_info );
   bool is_owner_edit = has_perm_extra( c_view_type_extra_owner_edit, view_extras, sess_info );
   bool is_admin_owner_edit = has_perm_extra( c_view_type_extra_admin_owner_edit, view_extras, sess_info );

   if( sess_info.is_read_only || sess_info.user_id.empty( ) )
      is_no_edit = true;

   if( is_printable )
   {
      is_no_edit = true;

      if( !pdf_view_file_name.empty( ) )
         os << "<p class=\"screen\"><a href=\"" << pdf_view_file_name << "\" target=\"_blank\">"
         "<img src=\"pdf_icon.gif\" border=\"0\">&nbsp;" << GDS( c_display_pdf_version ) << "</a></p>";
   }

   string owner;

   if( !source.owning_user_field.empty( ) && source.fk_field_values.count( source.owning_user_field ) )
      owner = source.fk_field_values.find( source.owning_user_field )->second;

   if( owner == sess_info.user_key )
      is_record_owner = true;
   else
      is_record_owner = false;

   if( !source.is_effective_owner_field.empty( ) )
   {
      int is_effective_owner_field = atoi(
       source.field_values.find( source.is_effective_owner_field )->second.c_str( ) );

      if( is_effective_owner_field )
         is_record_owner = true;
   }

   // NOTE: If is "user_info" then set "is_record_owner" according to the record and current user's key.
   if( source.vici->second->id == get_storage_info( ).user_info_view_id )
      is_record_owner = ( data == sess_info.user_key );

   // NOTE: If session is anonymous then will never be considered as a record owner.
   if( sess_info.user_id.empty( ) )
      is_record_owner = false;

   string image_width( to_string( sess_info.image_width ) );
   string image_height( to_string( sess_info.image_height ) );

   bool do_not_size_image = false;

   if( !source.orientation_field.empty( ) )
   {
      int orientation = atoi( source.field_values.find( source.orientation_field )->second.c_str( ) );

      if( orientation == 1 ) // i.e. Portrait
         swap( image_height, image_width );
      else if( orientation == 2 ) // i.e. Neither
         do_not_size_image = true;
   }

   string enter_action = "null";
   string cancel_action = "null";

   if( !is_printable )
   {
      // NOTE: A form is used for the view even if not editing (for send always fields), provided
      // that the view does not contain any file attachments (as these require a form). Therefore
      // send always fields will not work if the view has file attachments (unless "uneditable").
      // FUTURE: If file upload forms are placed in a seperate "iframe" then this limitation can
      // be removed.
      if( is_in_edit || !source.has_file_attachments || ( source.state & c_state_uneditable ) )
         os << "<form name=\"" << source.id << "\" id=\"" << source.id << "\">\n";

      if( is_ui_prototype( ) )
      {
         os << "<div class=\"topnav\">\n";
         os << "<div class=\"table-row\">\n";
         os << "<div class=\"table-cell\" id=\"view-actions\">\n";
      }
      else
      {
         os << "<table class=\"full_width_header\">\n";
         os << "<tr><td>";
      }

      if( !is_in_edit )
      {
         bool had_any = false;

         if( !is_no_edit
          && ( !is_admin_edit || sess_info.is_admin_user )
          && ( !is_owner_edit || owner.empty( ) || is_record_owner ) )
         {
            // NOTE: Unowned records (marked as "owner_edit") cannot be edited, however,
            // record specific actions will still appear (unless the record is changing
            // or is currently unactionable). If "admin_owner_edit" is used then "edit"
            // will only be allowed for owned records unless user is "admin". Also when
            // the view has been identified as the "user_info" view then unless user is
            // "admin" or the user key matches the current user prevent editing (unless
            // it is the "admin" user which is never allowed to be edited by the UI).
            if( !( source.state & c_state_uneditable )
             && !( source.state & c_state_is_changing )
             && !( source.state & c_state_unactionable )
             && ( !is_owner_edit || is_record_owner )
             && ( !is_admin_owner_edit || sess_info.is_admin_user || is_record_owner )
             && ( ( data != c_admin ) || ( mod_info.user_info_view_id != source.vici->second->id ) )
             && ( sess_info.is_admin_user || ( mod_info.user_info_view_id != source.vici->second->id ) || ( data == sess_info.user_key ) ) )
            {
               had_any = true;
               enter_action = "edit";

               os << "<input id=\"edit\" name=\"edit\" type=\"button\" class=\"button\" value=\""
                << GDS( c_display_edit_details ) << "\" onclick=\"";

               if( use_url_checksum )
               {
                  string checksum_values( to_string( c_act_edit )
                   + string( c_cmd_view ) + data + source.vici->second->id
                   + user_select_key + to_string( sess_info.checksum_serial ) );

                  string new_checksum_value( get_checksum( sess_info, checksum_values ) );

                  os << "query_update( '" << c_param_uselextra << "', '', true ); ";
                  os << "query_update( '" << c_param_ochksum << "', query_value( '" << c_param_chksum << "' ), true ); ";
                  os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true );";
               }

               os << " dyn_load( null, 'act="
                << c_act_edit << "&app=' + get_all_field_values( document." << source.id
                << " ) + '" << "&chk=" << source.key_info << "', false );\" style=\"cursor:pointer\"/>";
            }
            else
               is_editable = false;

            string all_actions( source.actions_value );

            // NOTE: Check for access restriction/permission to actions.
            if( !source.actions_extra.empty( ) )
            {
               map< string, string > extra_data;

               parse_field_extra( source.actions_extra, extra_data );

               if( !sess_info.is_admin_user
                && has_perm_extra( c_field_extra_admin_only, extra_data, sess_info ) )
                  all_actions.erase( );

               if( !is_record_owner
                && has_perm_extra( c_view_field_extra_owner_only, extra_data, sess_info ) )
                  all_actions.erase( );

               if( !sess_info.is_admin_user && !is_record_owner
                && has_perm_extra( c_view_field_extra_admin_owner_only, extra_data, sess_info ) )
                  all_actions.erase( );
            }

            if( view_extras.count( c_view_type_extra_can_copy ) && !( source.state & c_state_is_changing ) )
            {
               if( !all_actions.empty( ) )
                  all_actions = "," + all_actions;

               all_actions = "create_copy$" + source.vici->second->cid + "%" + data + all_actions;
            }

            if( !all_actions.empty( )
             && ( !( source.state & c_state_unactionable )
             || view_extras.count( c_view_type_extra_ignore_unactionable ) ) )
            {
               if( had_any )
                  os << '\n';
               else
                  had_any = true;

               output_actions( os, source, c_cmd_view, data, sess_info,
                source.vici->second->id, source.key_info, source.vici->second->cid,
                source.vici->second->mclass, all_actions, owner, session_id, user_select_key, "",
                using_session_cookie, use_url_checksum, false, &enter_action, 0, is_changing, back_count );
            }
         }
         else
         {
            is_editable = false;

            if( !sess_info.is_read_only
             && !sess_info.user_id.empty( )
             && !source.actions_value.empty( )
             && ( !( source.state & c_state_unactionable )
             || view_extras.count( c_view_type_extra_ignore_unactionable ) ) )
            {
               if( had_any )
                  os << c_nbsp;
               else
                  had_any = true;

               output_actions( os, source, c_cmd_view, data, sess_info,
                source.vici->second->id, source.key_info, source.vici->second->cid,
                source.vici->second->mclass, source.actions_value, owner, session_id, user_select_key, "",
                using_session_cookie, use_url_checksum, false, &enter_action, 0, is_changing, back_count );
            }
         }

         if( is_ui_prototype( ) )
         {
            if( had_any )
               os << "\n";
         }
         else
         {
            if( !had_any )
               os << c_nbsp;
            os << "</td>";
         }
      }
      else
      {
         enter_action = "save";
         cancel_action = "undo";

         string save_label( GDS( c_display_save_details ) );
         string cancel_label( GDS( c_display_cancel_edit ) );

         if( !exec.empty( ) )
         {
            save_label = string( GDS( c_display_confirm ) ) + " " + exec;
            cancel_label = string( GDS( c_display_cancel ) ) + " " + exec;
         }

         string checksum_values;
         string new_checksum_value;

         if( use_url_checksum )
         {
            checksum_values = to_string( cont_act )
             + string( c_cmd_view ) + data + source.vici->second->id
             + user_select_key + to_string( sess_info.checksum_serial );

            new_checksum_value = get_checksum( sess_info, checksum_values );

            edit_timeout_func = "query_update( \\'"
             + to_string( c_param_chksum ) + "\\', \\'" + new_checksum_value + "\\', true ); ";
         }

         if( vtab_num )
            edit_timeout_func = "query_update( \\'"
             + to_string( c_param_vtab ) + "\\', \\'" + to_string( vtab_num ) + "\\', true ); ";

         edit_timeout_func += "dyn_load( null, \\'act=" + cont_act
          + "&app=\\' + get_all_field_values( document." + to_string( source.id ) + " ), false );";

         os << "<input id=\"save\" name=\"save\" type=\"button\" class=\"button\" value=\"" << save_label << "\" onclick=\"";

         if( use_url_checksum )
         {
            checksum_values = to_string( c_act_save )
             + string( c_cmd_view ) + data + source.vici->second->id
             + user_select_key + to_string( sess_info.checksum_serial );

            new_checksum_value = get_checksum( sess_info, checksum_values );

            os << "query_update( '" << c_param_uselextra << "', '', true ); ";
            os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
         }

         if( vtab_num )
            os << "query_update( 'vtab', '" << vtab_num << "', true ); ";

         os << "dyn_load( document." << source.id << ", 'act=" << c_act_save
          << "&app=' + get_all_field_values( document." << source.id << " ) + '&chk=" << source.key_info << "'";

#ifdef ALLOW_MULTIPLE_RECORD_ENTRY
         // KLUDGE: Only show the "Create Multiple Records" if specially built to allow it.
         if( is_new_record )
            os << " + '&cont=' + document." << source.id << ".cont.checked";
#endif

         os << " + '&exec=" << exec << "&flags=" << source.state << "', false );";

         if( use_url_checksum )
            os << " query_update( '" << c_param_chksum << "', query_value( '" << c_param_ochksum << "' ), true );";

         // NOTE: Will only perform an 'auto_back' if is creating a new record.
         if( is_new_record && view_extras.count( c_view_type_extra_auto_back ) )
            os << " sessionStorage.setItem( 'auto_back', '1' );";

         os << "\" style=\"cursor:pointer\"/>";

         os << c_nbsp;
         os << "<input id=\"undo\" name=\"undo\" type=\"button\" class=\"button\"";

         if( is_new_record )
            os << " value=\"" << GDS( c_display_reset ) << "\"";
         else
            os << " value=\"" << cancel_label << "\"";

         os << " onclick=\"";

         if( use_url_checksum )
         {
            checksum_values.erase( );

            if( is_new_record )
               checksum_values += string( c_act_edit );

            checksum_values += string( c_cmd_view ) + data + source.vici->second->id + user_select_key;

            new_checksum_value = get_checksum( sess_info, checksum_values );
            os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
         }

         os << "dyn_load( null, 'act=" << c_act_undo << "', false );";

         if( use_url_checksum )
            os << " query_update( '" << c_param_chksum << "', query_value( '" << c_param_ochksum << "' ), true ); ";

         os << "\" style=\"cursor:pointer\"/>";

#ifdef ALLOW_MULTIPLE_RECORD_ENTRY
         // KLUDGE: Only show the "Create Multiple Records" if specially built to allow it.
         if( is_new_record )
         {
            os << "&nbsp;<input type=\"checkbox\" id=\"cont\" name=\"cont\"";
            if( cont == c_true )
               os << " checked";

            os << ">" << GDS( c_display_create_multiple_records ) << "</input>";
         }
#endif
         if( is_ui_prototype( ) )
            os << "\n";
         else
            os << "</td>";
      }

      if( is_ui_prototype( ) )
         os << "</div>\n<div class=\"table-cell message center\">";

      if( !error_message.empty( ) )
      {
         // NOTE: Don't display as an error unless it was actually received that way from the server.
         if( error_message.find( GDS( c_display_error ) ) != 0 )
         {
            string message( error_message );

            string::size_type pos = message.find( c_clipboard );

            if( pos == 0 )
            {
               pos = message.find( ':' );

               if( pos != string::npos )
                  message.erase( 0, pos + 1 );

               pos = message.find( ';' );

               string extra( " " );
               if( pos != string::npos )
               {
                  extra += message.substr( 0, pos );
                  message.erase( 0, pos + 1 );
               }

               string display( string_message(
                GDS( c_display_copy_to_clipboard ),
                make_pair( c_display_copy_to_clipboard_parm_extra, extra ) ) );

               message = "<input id=\"clipboard\" name=\"clipboard\" type=\"button\" class=\"button\" value=\"" + display
                + "\" onClick=\"this.style.display = 'none'; navigator.clipboard.writeText( '" + message + "' );\" style=\"cursor:pointer\">";
            }

            if( !is_ui_prototype( ) )
               os << "<td>" << replace_crlfs( message, "<br/>" ) << "</td>";
            else
               os << "<p class=\"table-cell center view-message\">" << replace_crlfs( message, "<br/>" ) << "</p>\n";

            extra_content_func += "had_act_error = false;\n";
         }
         else
         {
            extra_content_func += "had_act_error = true;\n";

            if( !is_ui_prototype( ) )
               os << "<td class=\"error\">" << remove_key( error_message ) << "</td>";
            else
               os << "<p class=\"error table-cell center view-message\">&nbsp;&nbsp;" << remove_key( error_message ) << "</p>\n";
         }
      }
      else
         extra_content_func += "had_act_error = false;\n";

      if( is_ui_prototype( ) )
         os << "</div>\n";

      bool has_started_right = false;

      bool has_quick_link = false;
      for( size_t i = 0; i < sess_info.quick_link_data.size( ); i++ )
      {
         string columns( sess_info.quick_link_data[ i ].second );

         vector< string > column_info;
         raw_split( columns, column_info );

         if( column_info.size( ) > 2
          && column_info[ 2 ].find( data + source.vici->second->id ) == 0 )
         {
            has_quick_link = true;
            break;
         }
      }

      if( is_ui_prototype( ) )
         os << "<div class=\"table-cell right-relative\">\n";

      // NOTE: If this view supports "quick linking" (and it is being used) then create an "add quick link" action.
      if( !is_in_edit && !has_quick_link && !mod_info.user_qlink_class_id.empty( )
       && source.has_quick_link && using_session_cookie && !sess_info.user_id.empty( )
       && ( mod_info.user_qlink_permission.empty( ) || has_permission( mod_info.user_qlink_permission, sess_info ) ) )
      {
         if( !is_quick_link_view && ( sess_info.quick_link_data.size( ) < sess_info.quick_link_limit ) )
         {
            has_started_right = true;

            if( !is_ui_prototype( ) )
               os << "<td class=\"right\">";
            else
               os << "<span id=\"link-view\" class=\"link\">";

            string qlink_title( source.field_values.find( source.quick_link_value_id )->second );

            os << "<input id=\"quick_link\" name=\"quick_link\" type=\"button\" class=\"button\" value=\""
             << GDS( c_display_add_user_link ) << "\" onclick=\"";

            if( use_url_checksum )
            {
               string checksum_values( string( c_cmd_view )
                + data + source.vici->second->id + user_select_key + to_string( sess_info.checksum_serial ) );

               string new_checksum_value( get_checksum( sess_info, checksum_values ) );

               os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
            }

            os << "query_update( '" << c_param_qlink << "', '"
             << to_string( sess_info.quick_link_data.size( ) ) << "', true ); ";

            os << "dyn_load( document." << source.id << ", 'act="
             << c_act_qlink << "&extra=" << escape_specials( qlink_title ) << "', false );\" style=\"cursor:pointer\"/>";

            if( is_ui_prototype( ) )
               os << "</span>\n";
         }
      }

      // NOTE: If is "standard" or "admin" and a specific "printable" version exists then provide a
      // link to it. If no specific "printable" version exists then the default "printview" version
      // will be linked to instead.
      if( !is_in_edit && !is_no_print && ( source.type == c_view_type_admin || source.type == c_view_type_standard ) )
      {
         bool found_print_version = false;
         for( view_const_iterator vci = mod_info.views.begin( ), end = mod_info.views.end( ); vci !=end; ++vci )
         {
            if( vci->vlink == source.vici->second->id
             && ( vci->type == c_view_type_print || vci->type == c_view_type_admin_print ) )
            {
               if( is_ui_prototype( ) )
                  os << "<span id=\"print-view\" class=\"print\">";
               else
               {
                  if( has_started_right )
                     os << "&nbsp;&nbsp;";
                  else
                  {
                     has_started_right = true;
                     os << "<td class=\"right\">";
                  }
               }

               os << "<a href=\"" << get_module_page_name( source.module_ref )
                << "?cmd=" << c_cmd_pview << "&data=" << data << "&ident=" << vci->id;

               if( !user_select_key.empty( ) )
                  os << "&" << c_param_uselect << "=" << user_select_key;

               if( !using_session_cookie )
                  os << "&session=" << session_id;

               if( use_url_checksum )
               {
                  string checksum_values( to_string( c_cmd_pview ) + data + vci->id + user_select_key );
                  os << "&" << c_param_chksum << "=" << get_checksum( sess_info, checksum_values );
               }

               os << "\">" << GDS( c_display_print ) << "</a>";

               if( is_ui_prototype( ) )
                  os << "</span>\n";

               found_print_version = true;
               break;
            }
         }

         if( !found_print_version )
         {
            if( is_ui_prototype( ) )
               os << "<span id=\"print-view\" class=\"print\">";
            else
            {
               if( has_started_right )
                  os << "&nbsp;&nbsp;";
               else
               {
                  has_started_right = true;
                  os << "<td class=\"right\">";
               }
            }

            // NOTE: In order to improve package maintenance productivity
            // can display the record key in order to more easily copy it
            // to the clipboard.
            if( !is_in_edit && sess_info.view_show_key )
            {
               string::size_type pos = source.key_info.find( ' ' );
               os << source.key_info.substr( 0, pos ) << "&nbsp;";
            }

            os << "<a href=\"" << get_module_page_name( source.module_ref )
             << "?cmd=" << c_cmd_pview << "&data=" << data << "&ident=" << source.vici->second->id;

            if( !user_select_key.empty( ) )
               os << "&" << c_param_uselect << "=" << user_select_key;

            if( !using_session_cookie )
               os << "&session=" << session_id;

            if( use_url_checksum )
            {
               string checksum_values(
                to_string( c_cmd_pview ) + data + source.vici->second->id + user_select_key );

               os << "&" << c_param_chksum << "=" << get_checksum( sess_info, checksum_values );
            }

            os << "\">" << GDS( c_display_print ) << "</a>";

            if( is_ui_prototype( ) )
               os << "</span>\n";
         }
      }

      if( !is_ui_prototype( ) && has_started_right )
         os << "</td>";

      if( is_ui_prototype( ) )
         os << "</div>\n</div>\n</div>\n";
      else
         os << "</tr></table>\n";
   }

   if( is_ui_prototype( ) )
      os << "<div class=\"menu width-fix\">\n";

   os << "<table class=\"list\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n";

   if( view_extras.count( c_view_type_extra_use_first_row_as_header ) )
      os << "<thead>\n";
   else
      os << "<tbody>\n";

   set< string > fk_refs;

   string extra_fields, extra_values;

   bool display_tabs = false;
   bool has_displayed_tabs = false;

   // NOTE: Tabs are only displayed for non-print views.
   if( !is_printable && !source.tab_names.empty( ) )
      display_tabs = true;

   string class_extra_effect;

   // NOTE: Determine whether fields should be displayed differently according
   // to class scoped modifiers and the current instance state.
   string class_display_effect;

   if( !view_extras.count( c_view_type_extra_print_no_highlight ) )
   {
      for( size_t j = 0; j < ARRAY_SIZE( state_modifiers ); j++ )
      {
         if( source.state & state_modifiers[ j ] )
         {
            pair< modifier_const_iterator, modifier_const_iterator > range
             = source.vici->second->modifiers.equal_range( state_modifiers[ j ] );

            for( modifier_const_iterator ci = range.first; ci != range.second; ++ci )
            {
               string new_class_effect( ci->second );

               if( ( new_class_effect == c_modifier_effect_lowlight )
                || ( new_class_effect == c_modifier_effect_lowlight1 )
                || ( new_class_effect == c_modifier_effect_highlight )
                || ( new_class_effect == c_modifier_effect_highlight1 ) )
                  class_display_effect = new_class_effect;

               if( ( new_class_effect == c_modifier_effect_extralight )
                || ( new_class_effect == c_modifier_effect_extralight1 ) )
                  class_extra_effect = new_class_effect;
            }
         }
      }
   }

   int skip_tab_num = -1;
   int num_displayed = 0;
   bool hide_slevel = false;
   bool finished_head = false;

   date_time dt_current;
   get_session_dtm( sess_info, dt_current );

   string sid;
   get_server_sid( sid );

   for( size_t i = 0; i < source.field_ids.size( ); i++ )
   {
      string cell_data;
      string source_field_id( source.field_ids[ i ] );
      string source_value_id( source.value_ids[ i ] );

      map< string, string > extra_data;
      if( !source.vici->second->fields[ i ].extra.empty( ) )
         parse_field_extra( source.vici->second->fields[ i ].extra, extra_data );

      if( source.hidden_fields.count( source_value_id ) )
      {
         if( extra_data.count( c_view_field_extra_force )
          && !is_not_accessible( is_in_edit, is_new_record, extra_data, sess_info ) )
         {
            if( !extra_fields.empty( ) )
            {
               extra_fields += ",";
               extra_values += ",";
            }

            extra_fields += source_field_id;
            extra_values += double_escaped( source.field_values.find( source_value_id )->second, ",&" );
         }

         continue;
      }

      if( sess_info.user_id.empty( ) && extra_data.count( c_field_extra_no_anon ) )
         continue;

      if( !is_new_record && !is_record_owner && source.owner_fields.count( source_value_id ) )
         continue;

      if( !is_new_record && !is_record_owner
       && has_perm_extra( c_view_field_extra_owner_only, extra_data, sess_info ) )
         continue;

      if( !is_new_record && !sess_info.is_admin_user && !is_record_owner
       && has_perm_extra( c_view_field_extra_admin_owner_only, extra_data, sess_info ) )
         continue;

      // NOTE: If the user is anonymous or has "level 0" security then don't display the security level.
      if( extra_data.count( c_field_extra_security_level ) )
      {
         if( !source.enum_fields.count( source_value_id ) )
            throw runtime_error( "security level enum not found for " + source_value_id );

         const enum_info& info(
          sinfo.enums.find( source.enum_fields.find( source_value_id )->second )->second );

         if( sess_info.user_id.empty( ) || ( info.values[ 0 ].first == sess_info.user_slevel ) )
         {
            if( !is_in_edit )
               continue;
            else
               hide_slevel = true;
         }
      }

      // NOTE: Fields that are not in the current tab cannot be skipped when editing as all field
      // values need to be sent to the server when the POST occurs (so their rows instead will be
      // given an invisible style).
      if( !is_in_edit && !is_printable
       && source.field_tab_ids[ i ] && ( source.field_tab_ids[ i ] != vtab_num ) )
         continue;

      // NOTE: As above for show more/less fields.
      if( !is_in_edit && ( skip_tab_num == source.field_tab_ids[ i ] ) )
         continue;

      // NOTE: If the field belongs to a tab which the user does not have access to
      // then this field must be skipped (otherwise URL tampering could allow these
      // fields to appear as 'vtab' is not part of the URL checksum).
      if( ( ( source.field_tab_ids[ i ] != 0 )
       && ( vtab_num == source.field_tab_ids[ i ] ) ) )
      {
         map< string, string > extra_data;

         if( !source.tab_extras[ vtab_num - 1 ].empty( ) )
            parse_field_extra( source.tab_extras[ vtab_num - 1 ], extra_data );

         if( !sess_info.is_admin_user
          && has_perm_extra( c_field_extra_admin_only, extra_data, sess_info ) )
            continue;

         if( !is_new_record && !is_record_owner
          && has_perm_extra( c_view_field_extra_owner_only, extra_data, sess_info ) )
            continue;

         if( !is_new_record && !sess_info.is_admin_user && !is_record_owner
          && has_perm_extra( c_view_field_extra_admin_owner_only, extra_data, sess_info ) )
            continue;

         if( sess_info.user_id.empty( ) && extra_data.count( c_field_extra_no_anon ) )
            continue;
      }

      // NOTE: If tabs exist and should be displayed (and haven't already been) then output them
      // now (after any context fields that preceeded the first tab heading have been displayed).
      if( display_tabs && !has_displayed_tabs && source.field_tab_ids[ i ] )
      {
         ++num_displayed; // NOTE: Increment for even/odd row display of row following the tabs.
         has_displayed_tabs = true;

         output_view_tabs( os, source, sess_info, vtab_num, is_in_edit, data, user_select_key,
          use_url_checksum, cont_act, back_count, is_new_record, is_record_owner, is_user_class_view );
      }

      // NOTE: Determine whether fields should be protected, relegated or displayed differently according
      // to modifiers and state. It is being assumed here that the original view fields and source fields
      // have the same offsets.
      string display_effect( class_display_effect ), extra_effect( class_extra_effect ), view_edit_effect;

      if( !view_extras.count( c_view_type_extra_print_no_highlight ) )
      {
         for( size_t j = 0; j < ARRAY_SIZE( state_modifiers ); j++ )
         {
            if( source.state & state_modifiers[ j ] )
            {
               pair< modifier_const_iterator, modifier_const_iterator > range
                = source.vici->second->fields[ i ].modifiers.equal_range( state_modifiers[ j ] );

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

      bool is_special_field = false;

      if( !view_edit_effect.empty( ) || source.protected_fields.count( source_value_id ) )
      {
         bool is_relegated = ( view_edit_effect == c_modifier_effect_relegate );
         bool is_protected = ( ( view_edit_effect == c_modifier_effect_protect ) || source.protected_fields.count( source_value_id ) );

         if( is_relegated )
            is_protected = false;

         if( source.field_values.count( source_value_id )
          && extra_data.count( c_view_field_extra_force )
          && !is_not_accessible( is_in_edit, is_new_record, extra_data, sess_info )
          && ( ( is_protected && extra_data.count( c_view_field_extra_trigger_protected ) )
          || ( ( is_relegated || is_protected ) && extra_data.count( c_view_field_extra_trigger_relegated ) ) ) )
         {
            if( !extra_fields.empty( ) )
            {
               extra_fields += ",";
               extra_values += ",";
            }

            extra_fields += source_field_id;
            extra_values += double_escaped( source.field_values.find( source_value_id )->second, ",&" );
         }
      }

      if( view_edit_effect == c_modifier_effect_relegate )
         continue;
      else if( view_edit_effect == c_modifier_effect_protect )
         is_special_field = true;

      if( is_printable && source.file_fields.count( source_value_id ) )
         continue;

      string show_opt( c_show_prefix );
      show_opt += '0' + source.field_tab_ids[ i ];

      // NOTE: The first field in a tab (including fields before any tabs) that contains the "show"
      // extra will only be shown (and any following fields from the same tab) if the user chooses.
      // This feature is not used for new records (perhaps this should be an option in the future).
      if( !is_printable && !is_new_record && extra_data.count( c_view_field_extra_show )
       && ( source.field_tab_ids[ i ] == 0 || vtab_num == source.field_tab_ids[ i ] ) )
      {
         if( ++num_displayed % 2 == 1 )
            os << "<tr class=\"list\">\n";
         else
            os << "<tr class=\"list odd_data\">\n";

         os << "<td class=\"list center\" colspan=\"2\">";

         if( !is_in_edit )
            os << "<a href=\"javascript:";
         else
            os << "<a href=\"#\" onclick=\"javascript:";

         bool show = show_opts.count( show_opt ) && show_opts.find( show_opt )->second == "1";

         if( !show )
            skip_tab_num = source.field_tab_ids[ i ];

         string show_less_image( "show_less.png" );
         string show_more_image( "show_more.png" );

         if( !is_in_edit )
         {
            os << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";

            if( !show )
               os << "query_update( '" << show_opt
                << "', '1' );\"><img src=\"" << show_more_image << "\" border=\"0\"></a>";
            else
               os << "query_update( '" << show_opt
                << "', '0' );\"><img src=\"" << show_less_image << "\" border=\"0\"></a>";
         }
         else
         {
            os << "query_update( '" << show_opt << "', '" << ( show ? "0" : "1" ) << "', true ); ";

            os << "dyn_load( null, 'act="
             << cont_act << "&app=' + get_all_field_values( document." << source.id << " ), false ); ";

            os << "event.returnValue = false; return false;";

            os << "\">" << ( show ? "<img src=\"" + show_less_image + "\" border=\"0\">"
             : "<img src=\"" + show_more_image + "\" border=\"0\">" ) << "</a>";
         }

         os << "</td></tr>\n";

         if( !show && !is_in_edit )
            continue;
      }

      if( !is_new_record && ( source_field_id == c_key_field ) )
         cell_data = data;
      else if( source_field_id == pfield )
         cell_data = extra;

      if( source.field_values.count( source_value_id ) )
         cell_data = source.field_values.find( source_value_id )->second;

      string special_prefix, special_suffix;

      if( !source.special_field.empty( ) && source.field_values.count( source.special_field ) )
      {
         if( extra_data.count( c_field_extra_prefix_special ) )
            special_prefix = source.field_values.find( source.special_field )->second;
         else if( extra_data.count( c_field_extra_suffix_special ) )
            special_suffix = source.field_values.find( source.special_field )->second;
      }

      string td_type( "td" );

      bool is_visible = true;

      // NOTE: Hide rows that don't belong to the "active" tab (this only applies when editing)
      // as well as the "security level" field if the user has only the minimum security level.
      if( is_in_edit && ( ( skip_tab_num == source.field_tab_ids[ i ] )
       || ( extra_data.count( c_field_extra_security_level ) && hide_slevel )
       || ( source.field_tab_ids[ i ] && source.field_tab_ids[ i ] != vtab_num ) ) )
      {
         is_visible = false;

         if( ++num_displayed % 2 == 1 )
            os << "<tr class=\"invisible\">\n";
         else
            os << "<tr class=\"odd_invisible\">\n";
      }
      else if( i != 0 || !view_extras.count( c_view_type_extra_use_first_row_as_header ) )
      {
         if( ++num_displayed % 2 == 1 )
            os << "<tr class=\"list\">\n";
         else
            os << "<tr class=\"list odd_data\">\n";
      }
      else
      {
         os << "<tr>\n";
         td_type = "th";
      }

      string td_extra;

      if( !extra_data.count( c_field_extra_full_width )
       || ( !is_in_edit
       && ( source.file_fields.count( source_value_id )
       || source.image_fields.count( source_value_id ) ) ) )
      {
         os << "   <" << td_type << " width=\"250\" class=\"list"
          << ( extra_effect.empty( ) ? string( "" ) : " " + extra_effect ) << "\">";

         os << escape_markup( is_in_edit ? source.edit_display_names[ i ] : source.display_names[ i ] );

         if( source.uom_fields.count( source_value_id ) )
         {
            string symbol( source.uom_fields.find( source_value_id )->second );

            if( symbol.find( c_uom_prefix ) == 0 )
            {
               string name_key( source.vici->second->fields[ i ].pfname );
               if( name_key.empty( ) )
                  name_key = source.vici->second->fields[ i ].name;

               symbol = get_display_string( name_key + "_(" + symbol.substr( strlen( c_uom_prefix ) ) + ")" );
            }

            if( !symbol.empty( ) )
               os << " (" << symbol << ")";
         }

         os << "</" << td_type << ">\n";
      }
      else
         td_extra = " colspan=\"2\"";

      string class_extra;

      if( !extra_effect.empty( ) )
         class_extra += " " + extra_effect;

      if( source.force_right_fields.count( source_value_id ) )
         class_extra += " right";
      else if( source.force_center_fields.count( source_value_id ) )
         class_extra += " center";
      else if( source.force_justify_fields.count( source_value_id ) )
         class_extra += " justify";

      if( source.large_fields.count( source_value_id ) )
         class_extra += " large";
      else if( source.larger_fields.count( source_value_id ) )
         class_extra += " larger";
      else if( source.small_fields.count( source_value_id ) )
         class_extra += " small";
      else if( source.smaller_fields.count( source_value_id ) )
         class_extra += " smaller";

      if( !is_in_edit && display_effect == c_modifier_effect_lowlight )
         os << "   <" << td_type << " class=\"lowlight" << class_extra << "\"" << td_extra << ">";
      else if( !is_in_edit && display_effect == c_modifier_effect_lowlight1 )
         os << "   <" << td_type << " class=\"lowlight1" << class_extra << "\"" << td_extra << ">";
      else if( !is_in_edit && display_effect == c_modifier_effect_highlight )
         os << "   <" << td_type << " class=\"highlight" << class_extra << "\"" << td_extra << ">";
      else if( !is_in_edit && display_effect == c_modifier_effect_highlight1 )
         os << "   <" << td_type << " class=\"highlight1" << class_extra << "\"" << td_extra << ">";
      else
         os << "   <" << td_type << " class=\"list" << class_extra << "\"" << td_extra << ">";

      if( source_field_id == pfield
       || source_field_id == source.security_level_field
       || source_field_id == source.create_user_key_field || source_field_id == source.modify_user_key_field
       || source_field_id == source.create_datetime_field || source_field_id == source.modify_datetime_field )
         is_special_field = true;

      if( source.enum_fields.count( source_value_id ) || source.html_fields.count( source_value_id )
       || source.text_fields.count( source_value_id ) || source.notes_fields.count( source_value_id )
       || source.file_fields.count( source_value_id ) || source.image_fields.count( source_value_id )
       || source.qr_code_fields.count( source_value_id ) || source.bool_fields.count( source_value_id )
       || source.protected_fields.count( source_value_id ) )
         is_special_field = true;

      bool is_protected_field = false;

      if( ( view_edit_effect == c_modifier_effect_protect )
       || new_field_and_values.count( source_field_id ) || source.protected_fields.count( source_value_id ) )
      {
         is_special_field = true;
         is_protected_field = true;
      }

      if( !is_record_owner
       && has_perm_extra( c_view_field_extra_owner_edit, extra_data, sess_info ) )
         is_protected_field = true;

      if( !sess_info.is_admin_user && !is_record_owner
       && has_perm_extra( c_view_field_extra_admin_owner_edit, extra_data, sess_info ) )
         is_protected_field = true;

      bool has_value = false;
      bool has_new_value = false;

      if( is_in_edit && user_values.count( source_field_id ) )
         has_value = true;

      bool is_always_editable = false;
      bool cell_data_is_placeholder = false;

      if( !is_printable && !is_protected_field
       && ( !is_in_edit || !extra_data.count( c_view_field_extra_view_only ) )
       && !( source.state & c_state_is_changing ) && extra_data.count( c_view_field_extra_always_editable ) )
         is_always_editable = true;

      // NOTE: The primary key and parent folder of a "root" folder may not be edited and record
      // editing below a parent does not allow the parent field value itself to be changed. Also
      // any "special fields" will require specialised editing (if editable).
      if( !is_special_field && ( is_new_record
       || is_always_editable || ( is_in_edit && source_field_id != c_key_field ) )
       && ( source.root_folder.empty( )
       || data != source.root_folder || !source.self_relationships.count( source_field_id ) ) )
      {
         if( has_value )
            cell_data = user_values.find( source_field_id )->second;
         else if( is_new_record && source.new_field_values.count( source_field_id ) )
         {
            has_new_value = true;
            cell_data = source.new_field_values.find( source_field_id )->second;
         }

         // NOTE: If a foreign key display value is encrypted then decrypt it.
         set< string > parent_extras;

         if( !source.vici->second->fields[ i ].pextra.empty( ) )
            split( source.vici->second->fields[ i ].pextra, parent_extras, '+' );

         if( !cell_data.empty( ) && parent_extras.count( c_parent_extra_decrypt ) )
            cell_data = data_decrypt( cell_data, sid );

         // NOTE: If a new record involves a trigger event then the placeholder will
         // be erased (as effectively a "blank" value has been provided).
         if( is_new_record && !has_value && !has_new_value
          && !cell_data.empty( ) && extra_data.count( c_field_extra_default_placeholder ) )
            cell_data_is_placeholder = true;

         if( !field_list.empty( ) )
            field_list += ',';

         field_list += source_field_id;

         if( !source.parent_lists.count( source_field_id ) )
         {
            string cls( "textinput" );

            string type( "text" );
            string name( "field_" + source_field_id );

            int max_length = 100;

            string extra, extra_keys, validate, use_time( "false" ), use_secs( "false" );

            bool is_password = false;
            bool is_datetime = false;

            if( source.int_fields.count( source_value_id ) )
            {
               cls = "text";
               extra = "size=\"9\" ";
               max_length = 8;
               validate = "int";

               if( extra_data.count( c_field_extra_int_type ) )
               {
                  string int_type = extra_data[ c_field_extra_int_type ];

                  extra = "size=\"17\" ";
                  max_length = 16;
                  validate = int_type;

                  if( !has_value )
                  {
                     if( int_type == "bytes" )
                        cell_data = format_bytes( atoi( cell_data.c_str( ) ) );
                     else if( int_type == "duration_dhm" || int_type == "duration_hms" )
                        cell_data = format_duration( atoi( cell_data.c_str( ) ), ( int_type == "duration_hms" ) );
                     else
                        throw runtime_error( "unsupported int_type '" + int_type + "'" );
                  }
               }
               else
               {
                  if( ( cell_data.empty( ) || !atoi( cell_data.c_str( ) ) )
                   && source.defcurrentyear_fields.count( source_value_id ) )
                     cell_data = to_string( dt_current.get_date( ).get_year( ) );

                  if( !has_value )
                  {
                     string mask( c_default_int_mask );

                     if( extra_data.count( c_field_extra_mask ) )
                        mask = extra_data[ c_field_extra_mask ];

                     cell_data = format_int_value( atoi( cell_data.c_str( ) ), mask );
                  }
               }
            }
            else if( source.numeric_fields.count( source_value_id ) )
            {
               cls = "text";
               extra = "size=\"13\" ";
               max_length = 12;
               validate = "numeric";

               if( extra_data.count( c_field_extra_numeric_type ) )
               {
                  string numeric_type = extra_data[ c_field_extra_numeric_type ];

                  validate = numeric_type;

                  if( !has_value )
                  {
                     if( numeric_type == "bytes" )
                        cell_data = format_bytes( numeric( cell_data.c_str( ) ).as_uint64( ) );
                     else
                        throw runtime_error( "unsupported numeric_type '" + numeric_type + "'" );
                  }
               }
               else
               {
                  if( !has_value )
                  {
                     string mask( c_default_numeric_mask );

                     if( extra_data.count( c_field_extra_mask ) )
                        mask = extra_data[ c_field_extra_mask ];

                     cell_data = format_numeric_value( numeric( cell_data.c_str( ) ), mask );
                  }
               }
            }
            else if( source.date_fields.count( source_value_id ) )
            {
               cls = "text";
               extra = "size=\"11\" ";
               validate = "datetime";
               max_length = 10;
               is_datetime = true;

               bool is_empty( cell_data.empty( ) );

               udate ud;

               if( !is_empty )
                  ud = udate( cell_data );

               // NOTE: If no attempt to save has been performed then a date is considered as having
               // a "default" value if it is either blank or equal to the default "udate" value. The
               // default will be displayed as an empty date unless the "defcurrent" extra has been
               // provided (then it will default to the current date according to the user's timezone).
               if( !was_invalid && ( ud == udate( ) ) )
               {
                  is_empty = true;
                  cell_data.erase( );
               }

               if( is_empty && source.defcurrent_fields.count( source_value_id ) )
               {
                  is_empty = false;
                  ud = dt_current.get_date( );
               }

               string date_precision;

               if( extra_data.count( c_field_extra_date_precision ) )
                  date_precision = extra_data[ c_field_extra_date_precision ];

               if( date_precision == "months" )
               {
                  extra = "size=\"6\" ";
                  max_length = 5;
               }
               else if( date_precision == "decades" )
               {
                  extra = "size=\"9\" ";
                  max_length = 8;
               }

               if( !is_empty )
                  cell_data = format_date( ud, date_precision.c_str( ) );
            }
            else if( source.time_fields.count( source_value_id ) )
            {
               cls = "text";
               extra = "size=\"9\" ";
               validate = "time";
               max_length = 8;

               bool is_empty( cell_data.empty( ) );

               mtime mt;
               if( !is_empty )
                  mt = mtime( cell_data );

               if( is_empty && source.defcurrent_fields.count( source_value_id ) )
               {
                  is_empty = false;
                  mt = dt_current.get_time( );
               }

               string time_precision;

               if( extra_data.count( c_field_extra_time_precision ) )
                  time_precision = extra_data[ c_field_extra_time_precision ];

               if( time_precision == "minutes" )
               {
                  extra = "size=\"6\" ";
                  max_length = 5;
               }

               if( !is_empty )
                  cell_data = format_time( mt, time_precision.c_str( ) );

            }
            else if( source.datetime_fields.count( source_value_id ) )
            {
               cls = "text";
               extra = "size=\"21\" ";
               validate = "datetime";
               max_length = 19;
               use_time = "true";
               is_datetime = true;

               bool is_empty( cell_data.empty( ) );

               date_time dt;

               if( !is_empty )
                  dt = date_time( cell_data );

               // NOTE: If no attempt to save has been performed then a datetime is considered as having
               // a "default" value if it is either blank or equal to the default "date_time" value. The
               // default will be displayed as an empty datetime unless the "defcurrent" extra has been
               // provided (then it will default to the current datetime according to the user's timezone).
               if( !was_invalid && dt == date_time( ) )
               {
                  is_empty = true;
                  cell_data.erase( );
               }

               if( !is_empty && sess_info.tz_name.empty( ) )
                  dt += ( seconds )sess_info.gmt_offset;

               if( is_empty && source.defcurrent_fields.count( source_value_id ) )
               {
                  is_empty = false;
                  dt = dt_current;
               }

               string time_precision;

               if( extra_data.count( c_field_extra_time_precision ) )
                  time_precision = extra_data[ c_field_extra_time_precision ];

               if( time_precision == "minutes" )
               {
                  extra = "size=\"18\" ";
                  max_length = 16;
                  use_secs = "false";
               }

               if( !is_empty )
                  cell_data = format_date_time( dt, time_precision.c_str( ) );
            }
            else if( source.hpassword_salt_field == source_field_id )
               extra = "salt=\"1\" ";
            else if( source.password_fields.count( source_value_id ) )
            {
               type = "password";
               is_password = true;
            }
            else if( source.hpassword_fields.count( source_value_id ) )
            {
               type = "password";
               is_password = true;
               extra = "hash=\"1\" ";
            }
            else if( source.fk_field_values.count( source_value_id ) )
            {
               cls = "text";
               extra = "size=\"20\" ";
            }
            else
            {
               if( extra_data.count( c_field_extra_max_size ) )
                  max_length = atoi( extra_data[ c_field_extra_max_size ].c_str( ) );
            }

            extra_keys = "className = '" + cls + "'; ";

            // NOTE: Validation is omitted from fields that are not visible as otherwise
            // Javascript errors would occur if it was attempted to focus on the field.
            if( !source.field_tab_ids[ i ] || source.field_tab_ids[ i ] == vtab_num )
            {
               if( source.mandatory_fields.count( source_value_id ) )
               {
                  if( !validate.empty( ) )
                     validate += ";";
                  validate += "reqd";
               }

               if( !validate.empty( ) )
               {
                  if( !extra.empty( ) )
                     extra += " ";
                  extra += "validate=\"" + validate + "\" ";
               }
            }

            if( cell_data_is_placeholder )
            {
               if( !extra.empty( ) )
                  extra += " ";

               extra += "placeholder=\"" + cell_data + "\"";

               cell_data.erase( );
            }

            os << "<input class=\"" << cls << class_extra << "\" type=\""
             << type << "\" " << extra << "name=\"" << name << "\" id=\"" << name
             << "\" maxlength=\"" << max_length << "\" value=\"" << escape_markup( unescaped( cell_data ) );

            if( source.field_tab_ids[ i ] && source.field_tab_ids[ i ] != vtab_num )
               os << "\" nofocus=\"" << c_true;

            string on_keyup;

            if( source.upper_fields.count( source_value_id ) )
               on_keyup = "this.value = this.value.toUpperCase( ); ";

            if( !on_keyup.empty( ) )
            {
               // NOTE: In order to prevent mouse clipboard operations from bypassing the 'onkeyup'
               // event also attach the same code to 'onchange' and to the "extra_keys" source.
               extra_keys += on_keyup;
               os << "\" onkeyup=\"" << on_keyup << "\" onchange=\"" << on_keyup;
            }

            os << "\" onkeypress=\"" << extra_keys
             << "return form_keys( event, " << enter_action << ", " << cancel_action << " );\"/>";

            if( is_datetime )
            {
               string range_extra;
               if( extra_data.count( c_field_extra_range ) )
               {
                  string::size_type pos = extra_data[ c_field_extra_range ].find( ".." );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected range format" );

                  range_extra = ", minDate: new Date('" + extra_data[ c_field_extra_range ].substr( 0, pos ) + "')";
                  range_extra += ", maxDate: new Date('" + extra_data[ c_field_extra_range ].substr( pos + 2 ) + "')";
               }

               os << "&nbsp;<input type=\"button\" id=\"" << name
                << "_img\" class=\"pikaday_button\"" << GDS( c_display_pick_a_date ) << "\">";
             
               extra_content_func += "var pika" + name + " = new Pikaday( "
                + "{ field: document.getElementById( '" + name + "' ),"
                + " calbutton: document.getElementById( '" + name + "_img' ),"
                + " yearRange: 10, formatPreset: 1, confirm: true, useTime: "
                + use_time + ", useSecs: " + use_secs + " } );\n";
            }
            else if( is_password && !is_always_editable )
            {
               if( !source.field_tab_ids[ i ] || source.field_tab_ids[ i ] == vtab_num )
               {
                  os << "</td>\n";
                  os << "</tr>\n";

                  if( ++num_displayed % 2 == 1 )
                  {
                     if( is_visible )
                        os << "<tr class=\"list\">\n";
                     else
                        os << "<tr class=\"invisible\">\n";
                  }
                  else
                  {
                     if( is_visible )
                        os << "<tr class=\"list odd_data\">\n";
                     else
                        os << "<tr class=\"odd_invisible\">\n";
                  }

                  os << "   <td class=\"list\">";
                  os << GDS( c_display_verify_password ) << "</td>\n";
                  os << "   <td class=\"list\">";

                  string extra;
                  if( source.mandatory_fields.count( source_value_id ) )
                     extra = "reqd;";

                  os << "<input class=\"textinput\" type=\"password\" validate=\"" << extra << "verify:"
                   << name << "\" maxlength=\"100\" value=\"" << escape_markup( unescaped( cell_data ) )
                   << "\" onkeypress=\"" << extra_keys << "return form_keys( event, " << enter_action
                   << ", " << cancel_action << " );\"></input>";
               }
            }
#ifndef OMIT_QRCODE_SCANNER
            else if( ( source.qr_scan_field == source_value_id ) || ( source.qr_scan_key_field == source_value_id ) )
            {
               os << "<br/><div id=\"qrcode_reader\"></div>";

               extra_content_func += "const scanner = new Html5QrcodeScanner( 'qrcode_reader', { qrbox: { width: 250, height: 250, }, fps: 20, } );\n\n";

               extra_content_func += "scanner.render( qrcode_reader_success, qrcode_reader_failure );\n\n";

               extra_content_func += "function qrcode_reader_success( result )\n";
               extra_content_func += "{\n";
               extra_content_func += "   document.getElementById( 'field_" + source_field_id + "' ).value = result;\n";

               extra_content_func += "   scanner.clear( );\n";
               extra_content_func += "   document.getElementById( 'qrcode_reader' ).remove( );\n";
               extra_content_func += "}\n\n";

               extra_content_func += "function qrcode_reader_failure( err ) { console.log( err ); }\n";
            }
#endif
         }
         else
         {
            os << "<select name=\"field_" << source_field_id;

            if( !class_extra.empty( ) )
               os << "\" class=\"" << class_extra.substr( 1 );

            os << "\" onkeypress=\"return form_keys( event, " << enter_action << ", " << cancel_action << " );\"";

            bool was_special_parent = false;

            // NOTE: If a field has been marked with "force" or as an "key/fkey" then its select is
            // expected to affect one or more other fields so a "cont" action is executed on change
            // to ensure that the dependent fields (or select lists) can be correctly determined.
            if( extra_data.count( c_view_field_extra_force )
             || extra_data.count( c_view_field_extra_key0 ) || extra_data.count( c_view_field_extra_key1 )
             || extra_data.count( c_view_field_extra_key2 ) || extra_data.count( c_view_field_extra_key3 )
             || extra_data.count( c_view_field_extra_key4 ) || extra_data.count( c_view_field_extra_key5 )
             || extra_data.count( c_view_field_extra_key6 ) || extra_data.count( c_view_field_extra_key7 )
             || extra_data.count( c_view_field_extra_key8 ) || extra_data.count( c_view_field_extra_key9 )
             || extra_data.count( c_view_field_extra_fkey0 ) || extra_data.count( c_view_field_extra_fkey1 )
             || extra_data.count( c_view_field_extra_fkey2 ) || extra_data.count( c_view_field_extra_fkey3 )
             || extra_data.count( c_view_field_extra_fkey4 ) || extra_data.count( c_view_field_extra_fkey5 )
             || extra_data.count( c_view_field_extra_fkey6 ) || extra_data.count( c_view_field_extra_fkey7 )
             || extra_data.count( c_view_field_extra_fkey8 ) || extra_data.count( c_view_field_extra_fkey9 ) )
            {
               os << " onchange=\"";
               was_special_parent = true;

               if( use_url_checksum )
               {
                  string checksum_values( string( cont_act )
                   + string( c_cmd_view ) + data + source.vici->second->id + user_select_key );

                  if( is_in_edit )
                     checksum_values += sess_info.checksum_serial;

                  string new_checksum( get_checksum( sess_info, checksum_values ) );

                  os << "\nvar old_checksum = query_value( '" << c_param_ochksum << "' );\n";
                  os << "if( old_checksum == '' ) old_checksum = query_value( '" << c_param_chksum << "' );\n";

                  os << "query_update( '" << c_param_chksum << "', '" + new_checksum + "', true ); ";
               }

               string pextra_special;
               if( extra_data.count( c_view_field_extra_key0 ) )
                  pextra_special = c_parent_extra_key0;
               else if( extra_data.count( c_view_field_extra_key1 ) )
                  pextra_special = c_parent_extra_key1;
               else if( extra_data.count( c_view_field_extra_key2 ) )
                  pextra_special = c_parent_extra_key2;
               else if( extra_data.count( c_view_field_extra_key3 ) )
                  pextra_special = c_parent_extra_key3;
               else if( extra_data.count( c_view_field_extra_key4 ) )
                  pextra_special = c_parent_extra_key4;
               else if( extra_data.count( c_view_field_extra_key5 ) )
                  pextra_special = c_parent_extra_key5;
               else if( extra_data.count( c_view_field_extra_key6 ) )
                  pextra_special = c_parent_extra_key6;
               else if( extra_data.count( c_view_field_extra_key7 ) )
                  pextra_special = c_parent_extra_key7;
               else if( extra_data.count( c_view_field_extra_key8 ) )
                  pextra_special = c_parent_extra_key8;
               else if( extra_data.count( c_view_field_extra_key9 ) )
                  pextra_special = c_parent_extra_key9;
               else if( extra_data.count( c_view_field_extra_fkey1 ) )
                  pextra_special = c_parent_extra_fkey1;
               else if( extra_data.count( c_view_field_extra_fkey2 ) )
                  pextra_special = c_parent_extra_fkey2;
               else if( extra_data.count( c_view_field_extra_fkey3 ) )
                  pextra_special = c_parent_extra_fkey3;
               else if( extra_data.count( c_view_field_extra_fkey4 ) )
                  pextra_special = c_parent_extra_fkey4;
               else if( extra_data.count( c_view_field_extra_fkey5 ) )
                  pextra_special = c_parent_extra_fkey5;
               else if( extra_data.count( c_view_field_extra_fkey6 ) )
                  pextra_special = c_parent_extra_fkey6;
               else if( extra_data.count( c_view_field_extra_fkey7 ) )
                  pextra_special = c_parent_extra_fkey7;
               else if( extra_data.count( c_view_field_extra_fkey8 ) )
                  pextra_special = c_parent_extra_fkey8;
               else if( extra_data.count( c_view_field_extra_fkey9 ) )
                  pextra_special = c_parent_extra_fkey9;

               if( !pextra_special.empty( ) )
                  os << "query_update( '"
                   << c_param_special << "', '" << pextra_special << "', true ); ";

               os << "dyn_load( null, 'act=" << cont_act;
               os << "&" << c_param_userfetch << "=" << source_field_id;
               os << "&app=' + get_all_field_values( document." << source.id << " ), false );";

               if( use_url_checksum )
                  os << "\nquery_update( '" << c_param_chksum << "', old_checksum, true );";

               os << "\"";
            }

            os << ">";

            const data_container& parent_row_data = source.parent_lists.find( source_field_id )->second;

            if( !source.mandatory_fields.count( source_value_id )
             && ( source.root_folder.empty( ) || !source.self_relationships.count( source_field_id ) ) )
               os << "<option value=\"\">&lt;" << GDS( c_display_none ) << "&gt;&nbsp;&nbsp;</option>\n";

            // NOTE: If mandatory then put "Choose" as an indicator that something needs to be selected
            // unless it isn't a "special" and there is only a single item (in which case it can simply
            // be automatically selected).
            if( source.mandatory_fields.count( source_value_id )
             && ( was_special_parent || parent_row_data.size( ) != 1 ) )
            {
               if( data != c_new_record )
                  os << "<option value=\"\" disabled=\"disabled\">&lt;";
               else
                  os << "<option value=\"\" disabled=\"disabled\" selected=\"selected\">&lt;";

               os << GDS( c_display_choose ) << "&gt;&nbsp;&nbsp;</option>\n";
            }

            for( size_t j = 0; j < parent_row_data.size( ); j++ )
            {
               string key( parent_row_data[ j ].first );
               string display( parent_row_data[ j ].second );

               // NOTE: Remove parent version information as its not really relevant for foreign key selection.
               size_t pos = key.find( ' ' );

               if( pos != string::npos )
                  key.erase( pos );

               if( display.empty( ) )
                  display = key;

               // NOTE: For "self relationships" do not allow the record being edited to be chosen as
               // its own parent.
               if( key == data && source.self_relationships.count( source_field_id ) )
                  continue;

               string original_display( display );

               if( source.manual_link_fields.count( source_value_id ) )
               {
                  stringstream ss;

                  replace_links_and_output( display, source.vici->second->id,
                   source.module, source.module_ref, ss, false, false, session_id,
                   sess_info, user_select_key, using_session_cookie, use_url_checksum );

                  display = ss.str( );
               }

               // NOTE: The "default" selection is based upon name rather than key (unless values have been
               // submitted). Also if this selection is a "special" one but it's mandatory and has only the
               // one item then that item will become automatically selected.
               os << "<option";
               if( has_new_value )
               {
                  if( key == cell_data )
                     os << " selected";
               }
               else if( ( has_value && key == cell_data )
                || ( !has_value && original_display == cell_data )
                || ( !was_special_parent && source.mandatory_fields.count( source_value_id ) && parent_row_data.size( ) == 1 ) )
                  os << " selected";

               os << " value=\"" << key << "\">";
               os << unescaped( display );

               os << "&nbsp;&nbsp;</option>\n";
            }

            os << "</select>";
         }
      }
      else
      {
         bool add_to_field_list = false;
         bool check_for_link_and_output = false;

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

            os << "<a href=\"" << type << cell_data << "\"" << extra << ">"
             << data_or_nbsp( escape_markup( unescaped( display ) ) ) << "</a>";
         }
         else if( source.text_fields.count( source_value_id ) )
         {
            if( has_value )
               cell_data = user_values.find( source_field_id )->second;

            if( is_always_editable || ( is_in_edit && !is_protected_field ) )
            {
               add_to_field_list = true;

               os << "<textarea id=\"field_" << source_field_id << "\" name=\"field_"
                << source_field_id << "\" rows=\"2\" cols=\"" << c_default_notes_cols << "\"";

               os << " onKeyUp=\"resize_text_rows( document." << source.id << ".field_" << source_field_id << ", 2 );\"";

               os << " onMouseOut=\"resize_text_rows( document." << source.id << ".field_" << source_field_id << ", 2 );\"";

               os << ">" << escape_markup( unescaped( cell_data, "trn\t\r\n" ) ) << "</textarea>";

               // NOTE: Resize text rows after content is inject to set the initial size of the textarea.
               extra_content_func += "resize_text_rows( document." + source.id + ".field_" + source_field_id + ", 2 );\n";
            }
            else if( !extra_data.count( c_field_extra_full_height ) )
            {
               string rows( "2" );

               if( cell_data.size( ) > 200 ) // NOTE: Double the height if likely to be bigger than 2 lines.
                  rows = "4";

               os << "<textarea readonly rows=\"" << rows << "\" cols=\"" << c_default_notes_cols << "\"";
               os << ">" << escape_markup( unescaped( cell_data, "trn\t\r\n" ) ) << "</textarea>";
            }
            else
               os << data_or_nbsp( unescaped( replace_crlfs_and_spaces( escape_markup( cell_data ), "<br/>", "&nbsp;" ) ) );
         }
         else if( source.notes_fields.count( source_value_id ) )
         {
            if( has_value )
               cell_data = user_values.find( source_field_id )->second;

            if( is_always_editable || ( is_in_edit && !is_protected_field ) )
            {
               add_to_field_list = true;

               os << "<textarea id=\"field_" << source_field_id << "\" name=\"field_" << source_field_id
                << "\" rows=\"" << sess_info.notes_rmin << "\" cols=\"" << c_default_notes_cols << "\"";

               os << " onKeyUp=\"limit_text( document." << source.id << ".field_" << source_field_id
                << ", 'countdown_" << source_field_id << "', " << sess_info.notes_limit
                << "); resize_text_rows( document." << source.id << ".field_" << source_field_id
                << ", " << sess_info.notes_rmin << ", " << sess_info.notes_rmax << " );\"";

               os << " onKeyDown=\"limit_text( document." << source.id << ".field_" << source_field_id
                << ", 'countdown_" << source_field_id << "', " << sess_info.notes_limit << " );\"";

               // NOTE: The "onBlur" and "onMouseOut" are to try and make sure that pasting via the
               // mouse (rather than via typing "ctrl-v") will not get around the textarea size limit.
               os << " onBlur=\"limit_text( document." << source.id << ".field_" << source_field_id
                << ", 'countdown_" << source_field_id << "', " << sess_info.notes_limit << " );\"";

               os << " onMouseOut=\"limit_text( document." << source.id << ".field_" << source_field_id
                << ", 'countdown_" << source_field_id << "', " << sess_info.notes_limit
                << "); resize_text_rows( document." << source.id << ".field_" << source_field_id
                << ", " << sess_info.notes_rmin << ", " << sess_info.notes_rmax << " );\"";

               os << ">" << escape_markup( unescaped( cell_data, "trn\t\r\n" ) ) << "</textarea>";

               // NOTE: Limit text after content is injected to get the initial remaining characters.
               extra_content_func += "limit_text( document." + source.id + ".field_" + source_field_id
                + ", 'countdown_" + source_field_id + "', " + to_string( sess_info.notes_limit ) + " );\n";

               // NOTE: Resize text rows after content is inject to set the initial size of the textarea.
               extra_content_func += "resize_text_rows( document." + source.id + ".field_" + source_field_id
                + ", " + to_string( sess_info.notes_rmin ) + ", " + to_string( sess_info.notes_rmax ) + " );\n";

               // NOTE: Impose a maximum character limit for the textarea (due to the hard-coded
               // buffer that is used for fetching the FCGI input information from the web-server).
               os << "<br><font size=\"1\">("
                << GDS( c_display_maximum_characters ) << ": " << sess_info.notes_limit << ")<br>\n";

               os << "<label class=\"charcount\" id=\"countdown_"
                << source_field_id << "\" name=\"countdown_" << source_field_id
                << "\">" << ( sess_info.notes_limit - cell_data.size( ) )
                << "</label> " << GDS( c_display_characters_left ) << ".</font>\n";
            }
            else if( !extra_data.count( c_field_extra_full_height ) )
            {
               os << "<textarea readonly rows=\""
                << sess_info.notes_rmin << "\" cols=\"" << c_default_notes_cols << "\"";

               os << ">" << escape_markup( unescaped( cell_data, "trn\t\r\n" ) ) << "</textarea>";
            }
            else
               os << data_or_nbsp( unescaped( replace_crlfs_and_spaces( escape_markup( cell_data ), "<br/>", "&nbsp;" ) ) );
         }
         else if( source.html_fields.count( source_value_id ) )
         {
            if( has_value )
               cell_data = user_values.find( source_field_id )->second;

            if( is_always_editable || ( is_in_edit && !is_protected_field ) )
            {
               add_to_field_list = true;

               os << "<textarea id=\"field_" << source_field_id << "\" name=\"field_" << source_field_id
                << "\" rows=\"" << sess_info.notes_rmin << "\" cols=\"" << c_default_notes_cols << "\">";
               os << escape_markup( unescaped( cell_data, "rn\r\n" ) ) << "</textarea>";

               // FUTURE: These widths should be able to be determined by the text area itself.
               int width = 500;
               if( extra_data.count( c_field_extra_full_width ) )
                  width = 700;

               extra_content_func += "wysiwygWidth = " + to_string( width )
                + "; wysiwygHeight = 450; WYSIWYG.generate( 'field_" + source_field_id + "', full );\n";
            }
            else
            {
               if( ( cell_data.find( "</HTML>" ) == string::npos )
                && ( cell_data.find( "</html>" ) == string::npos ) )
               {
                  bool output_hrefs = true;

                  if( is_in_edit || is_printable
                   || source.manual_link_ignores.count( source_value_id ) )
                     output_hrefs = false;

                  if( !cell_data.empty( ) )
                     unescape( cell_data, "rn\r\n" );

                  cell_data = replace_display_strings( cell_data );

                  replace_links_and_output( cell_data, source.vici->second->id,
                   source.module, source.module_ref, os, true, output_hrefs, session_id,
                   sess_info, user_select_key, using_session_cookie, use_url_checksum );
               }
               else
               {
                  // NOTE: If found content that is actually a complete HTML page then create a temporary
                  // file for it and put a link here. The filename is returned so that an "iframe" can be
                  // appended after all other content.
                  string tmp_html_file( string( c_files_directory )
                   + "/" + string( c_tmp_directory ) + "/" + sess_info.session_id + "/" + data + ".html" );

                  ofstream outf( tmp_html_file.c_str( ) );
                  outf << unescaped( replace_crlfs_and_spaces( cell_data, "\n", "" ) );

                  extra_html_content = tmp_html_file;

                  if( is_printable )
                     os << "<p>" << GDS( c_display_see_below ) << "</p>";
                  else
                     os << "<p>" << string_message( GDS( c_display_click_here_to_view_html ),
                      make_pair( c_display_click_here_to_view_html_parm_href,
                      "<a href=\"" + tmp_html_file + "\" target=\"_blank\">" ), "</a>" ) << "</p>";
               }
            }
         }
         else if( source.file_fields.count( source_value_id ) || source.image_fields.count( source_value_id ) )
         {
            if( source.new_file_name == "." )
               cell_data.erase( );
            else if( !source.new_file_name.empty( ) )
               cell_data = source.new_file_name;

            bool can_attach_or_remove_file = true;

            if( is_protected_field )
               can_attach_or_remove_file = false;

            if( is_admin_edit && !sess_info.is_admin_user )
               can_attach_or_remove_file = false;

            if( is_owner_edit && !is_record_owner )
               can_attach_or_remove_file = false;

            if( is_admin_owner_edit && !( sess_info.is_admin_user || is_record_owner ) )
               can_attach_or_remove_file = false;

            if( !extra_data.count( c_field_extra_file ) && !extra_data.count( c_field_extra_image ) )
               can_attach_or_remove_file = false;

            if( is_in_edit || ( source.state & c_state_uneditable ) || ( source.state & c_state_is_changing ) )
               can_attach_or_remove_file = false;

            // NOTE: In the same way that a "user" record is being prevented from being edited by any
            // other user (except admin) this also applies to the attaching or removing "user" files.
            if( !sess_info.is_admin_user
             && mod_info.user_info_view_id == source.vici->second->id && data != sess_info.user_key )
               can_attach_or_remove_file = false;

            // NOTE: If an instance can be identified as having been created by the currently logged in
            // user then if a file has yet to be attached this will be allowed, as it logically follows
            // that if a user can create a record they should also be able to attach a file to it.
            if( is_no_edit && ( !cell_data.empty( ) || source.create_user_key_field.empty( )
             || ( source.fk_field_values.find( source.create_user_key_field )->second != sess_info.user_key ) ) )
               can_attach_or_remove_file = false;

            bool has_attached_file_link = false;

            if( cell_data.empty( ) )
            {
               if( can_attach_or_remove_file )
                  os << "<p>" << string_message( GDS( c_display_select_local_upload_file ),
                   make_pair( c_display_select_local_upload_file_parm_attach, GDS( c_display_attach ) ) ) << "</p>";
               else
                  os << c_nbsp;
            }
            else
            {
               string file_path( string( c_files_directory )
                + "/" + get_module_id_for_attached_file( source ) + "/" + ( source.vici->second )->cid );

               string file_name( file_path + "/" + unescaped( cell_data ) );

               string file_full_ext, file_last_ext;
               string::size_type pos = file_name.find( "." );

               if( pos != string::npos )
                  file_full_ext = file_name.substr( pos + 1 );

               pos = file_name.find_last_of( "." );

               if( pos != string::npos )
                  file_last_ext = file_name.substr( pos + 1 );

               if( file_exists( file_name ) )
               {
                  string tmp_link_path( c_files_directory );

                  tmp_link_path += "/" + string( c_tmp_directory );
                  tmp_link_path += "/" + session_id;

                  bool has_utf8_chars;

                  string link_file_name( source.display_names[ i ] );

                  if( !source.filename_field.empty( ) && source.field_values.count( source.filename_field ) )
                  {
                     // NOTE: If an empty filename is found then "file_ext" will end up becoming the name.
                     if( source.field_values.find( source.filename_field )->second.empty( ) )
                        link_file_name.erase( );
                     else
                     {
                        link_file_name = valid_file_name( source.field_values.find( source.filename_field )->second, &has_utf8_chars );

                        if( source.file_fields.size( ) + source.image_fields.size( ) > 1
                         && !view_extras.count( c_view_type_extra_file_links_always_as_single ) )
                           link_file_name += " " + source.display_names[ i ];
                     }
                  }

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

                     if( is_encrypted && !source.ignore_encrypted_field.empty( )
                      && source.field_values.count( source.ignore_encrypted_field ) )
                     {
                        if( source.field_values.find( source.ignore_encrypted_field )->second == string( c_true_value ) )
                           is_encrypted = false;
                     }

                     create_tmp_file_link_or_copy( tmp_link_path, file_name, file_full_ext, link_file_name );
                  }

                  bool has_image = false;

                  if( !is_in_edit && !is_printable )
                  {
                     has_attached_file_link = true;

                     if( !source.image_fields.count( source_value_id ) )
                     {
                        os << "<p>" << string_message( GDS( c_display_click_here_to_view_file ),
                         make_pair( c_display_click_here_to_view_file_parm_href,
                         "<a href=\"" + tmp_link_path + "\" target=\"_blank\">" ), "</a>",
                         make_pair( c_display_click_here_to_view_file_parm_ext, escape_markup( file_full_ext ) ) ) << "</p>";
                     }
                     else
                        has_image = true;
                  }
                  else
                  {
                     if( !source.image_fields.count( source_value_id ) )
                        os << "<p>" << string_message( GDS( c_display_has_file ),
                         make_pair( c_display_has_file_parm_ext, escape_markup( file_full_ext ) ) ) << "</p>";
                     else
                        has_image = true;
                  }

                  if( has_image )
                  {
                     bool is_href = false;

                     if( !is_printable && !embed_images )
                     {
                        is_href = true;
                        os << "<a href=\"" << tmp_link_path << "\" target=\"_blank\">";
                     }

                     string image_src( tmp_link_path );

                     if( embed_images )
                     {
                        string buffer( buffer_file( file_name ) );
                        image_src = "data:image/" + file_last_ext + ";base64," + base64::encode( buffer );
                     }

                     os << "<img src=\"" << image_src;

                     string tmp_image_width( image_width );
                     string tmp_image_height( image_height );

                     bool tmp_do_not_size_image( do_not_size_image );

                     if( source.orientation_field.empty( ) )
                     {
                        if( extra_data.count( c_field_extra_portrait ) )
                           swap( tmp_image_height, tmp_image_width );
                        else if( extra_data.count( c_field_extra_neither ) )
                           tmp_do_not_size_image = true;
                     }

                     if( !embed_images && !tmp_do_not_size_image )
                        os << "\" width=\"" << tmp_image_width << "\" height=\"" << tmp_image_height;

                     os << "\" border=\"0\" alt=\"" << GDS( c_display_image ) << "\">";

                     if( is_href )
                        os << "</a>";

                     if( can_attach_or_remove_file )
                        os << "<br/>";
                  }
               }
            }

            if( can_attach_or_remove_file )
            {
               if( has_attached_file_link )
                  os << "\n<form name=\"remove\" id=\"remove\">\n";
               else
                  os << "\n<form name=\"upload\" id=\"upload\" method=\"POST\" enctype=\"multipart/form-data\" action=\"upload.fcgi\" target=\"hidden_frame\">";

               if( has_attached_file_link )
               {
                  os << "<input type=\"button\" class=\"button\" value=\""
                   << GDS( c_display_remove_attached_file ) << "\" onclick=\"";

                  string checksum_values;
                  string new_checksum_value;

                  if( use_url_checksum )
                  {
                     checksum_values = string( c_act_remove )
                      + string( c_cmd_view ) + data + source.vici->second->id
                      + user_select_key + to_string( sess_info.checksum_serial );

                     new_checksum_value = get_checksum( sess_info, checksum_values );

                     os << "query_update( '" + to_string( c_param_chksum ) + "', '" + new_checksum_value + "', true ); ";
                  }

                  if( vtab_num )
                     os << "query_update( '"
                      + to_string( c_param_vtab ) + "', '" + to_string( vtab_num ) + "', true ); ";

                  os << "dyn_load( null, 'act=" + string( c_act_remove ) + "&app=" + source_field_id + "', false );\"/>";
               }
               else
               {
                  string upload_info( session_id );

                  upload_info += ":" + get_module_id_for_attached_file( source ) + "/" + ( source.vici->second )->cid;
                  upload_info += ";" + data + "-" + source_field_id + "?" + to_string( sinfo.filesize_limit );

#ifndef REMOVE_OR_COMMENT_THIS_OUT_IN_CONFIG_H
                  // NOTE: In order to prevent any "arbitrary" upload from being able to take place
                  // a verification file must contain the same information as the "name" attribute.
                  string verification_file( string( c_files_directory ) + "/"
                   + string( c_tmp_directory ) + "/" + sess_info.session_id + "/" + data + "-" + source_field_id );

                  if( sess_info.was_file_remove )
                     file_remove( verification_file );
                  else
                  {
                     ofstream outf( verification_file.c_str( ), ios::out | ios::binary );
                     outf << upload_info;
                  }
#endif
                  os << "\n<input type=\"" << "file\" name=\"" << upload_info << "\"/>\n";
                  os << "<input type=\"submit\" class=\"button\" value=\"" << GDS( c_display_attach ) << "\"/>";
               }

               os << "</form>";
            }
         }
         else if( source.qr_code_fields.count( source_value_id ) )
         {
            if( !cell_data.empty( ) )
            {
               string temp_file_name( c_files_directory );

               temp_file_name += "/" + string( c_tmp_directory );
               temp_file_name += "/" + session_id;

               temp_file_name += "/" + source_value_id + ".png";

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
            }
         }
         else if( source.enum_fields.count( source_value_id ) )
         {
            const enum_info& info(
             sinfo.enums.find( source.enum_fields.find( source_value_id )->second )->second );

            // NOTE: If there is only one enum item then "protect" the field.
            if( info.values.size( ) == 1 )
               is_protected_field = true;

            if( has_value )
               cell_data = user_values.find( source_field_id )->second;

            string enum_image_file;

            size_t value_offset = 0;

            for( size_t i = 0; i < info.values.size( ); i++ )
            {
               if( info.values[ i ].first == cell_data )
               {
                  value_offset = i;

                  // NOTE: If an "image" with the enum value name exists
                  // then display the image rather than the string value.
                  // This only will occur if not editing as standard HTML
                  // select controls don't support the use of images.
                  if( file_exists( info.values[ i ].second + ".png" ) )
                     enum_image_file = info.values[ i ].second + ".png";
                  else if( file_exists( info.values[ i ].second + ".gif" ) )
                     enum_image_file = info.values[ i ].second + ".gif";

                  cell_data = get_display_string( info.values[ i ].second );

                  break;
               }
            }

            if( is_always_editable || ( is_in_edit && !is_protected_field ) )
            {
               add_to_field_list = true;

               os << "<select name=\"field_" << source_field_id
                << "\" onkeypress=\"return form_keys( event, " << enter_action << ", " << cancel_action << " );\"";

               // NOTE: If an enum field has been marked as a "key" or with "force" then changing its
               // value is expected to affect other fields so a "cont" action is executed on change.
               if( extra_data.count( c_view_field_extra_force )
                || extra_data.count( c_view_field_extra_key0 ) || extra_data.count( c_view_field_extra_key1 )
                || extra_data.count( c_view_field_extra_key2 ) || extra_data.count( c_view_field_extra_key3 )
                || extra_data.count( c_view_field_extra_key4 ) || extra_data.count( c_view_field_extra_key5 )
                || extra_data.count( c_view_field_extra_key6 ) || extra_data.count( c_view_field_extra_key7 )
                || extra_data.count( c_view_field_extra_key8 ) || extra_data.count( c_view_field_extra_key9 ) )
               {
                  os << " onchange=\"";

                  if( use_url_checksum )
                  {
                     string checksum_values( string( cont_act )
                      + string( c_cmd_view ) + data + source.vici->second->id + user_select_key );

                     if( is_in_edit )
                        checksum_values += sess_info.checksum_serial;

                     string new_checksum( get_checksum( sess_info, checksum_values ) );

                     os << "query_update( '" << c_param_ochksum << "', query_value( '" << c_param_chksum << "' ), true ); ";
                     os << "query_update( '" << c_param_chksum << "', '" + new_checksum + "', true ); ";
                  }

                  os << "dyn_load( null, 'act=" << cont_act;
                  os << "&" << c_param_userfetch << "=" << source_field_id;
                  os << "&app=' + get_all_field_values( document." << source.id << " ), false );\"";
               }

               os << ">";

               string estart, efinish;

               if( extra_data.count( c_view_field_extra_estart ) )
               {
                  string from( extra_data[ c_view_field_extra_estart ] );

                  for( size_t j = 0; j < source.field_ids.size( ); j++ )
                  {
                     string source_field_id( source.field_ids[ j ] );
                     string source_value_id( source.value_ids[ j ] );

                     map< string, string > extra_data;

                     if( !source.vici->second->fields[ j ].extra.empty( ) )
                        parse_field_extra( source.vici->second->fields[ j ].extra, extra_data );

                     if( extra_data.count( from ) )
                     {
                        estart = source.field_values.find( source_value_id )->second;
                        break;
                     }
                  }
               }

               if( extra_data.count( c_view_field_extra_efinish ) )
               {
                  string from( extra_data[ c_view_field_extra_efinish ] );

                  for( size_t j = 0; j < source.field_ids.size( ); j++ )
                  {
                     string source_field_id( source.field_ids[ j ] );
                     string source_value_id( source.value_ids[ j ] );

                     map< string, string > extra_data;

                     if( !source.vici->second->fields[ j ].extra.empty( ) )
                        parse_field_extra( source.vici->second->fields[ j ].extra, extra_data );

                     if( extra_data.count( from ) )
                     {
                        efinish = source.field_values.find( source_value_id )->second;
                        break;
                     }
                  }
               }

               string enum_filter_value;

               if( source.enum_filter_fields.count( info.id ) )
                  enum_filter_value = source.field_values.find(
                   source.enum_filter_fields.find( info.id )->second )->second;

               bool found_start = false;

               for( size_t i = 0; i < info.values.size( ); i++ )
               {
                  // NOTE: Enum values that start with a '-' are not included for user selection
                  // as they are deemed as being only available for internal application purposes.
                  if( info.values[ i ].first[ 0 ] == '-' )
                     continue;

                  // NOTE: If there is an applicable enum filter then need to check that each value
                  // should be included in the selection.
                  if( !enum_filter_value.empty( ) && info.filters.count( info.values[ i ].first ) )
                  {
                     string all_filters( info.filters.find( info.values[ i ].first )->second );

                     set< string > filters;

                     if( !all_filters.empty( ) )
                        split( all_filters, filters, '|' );

                     if( !filters.count( enum_filter_value ) )
                        continue;
                  }

                  if( !estart.empty( ) && !found_start )
                  {
                     if( info.values[ i ].first == estart )
                        found_start = true;
                     else
                        continue;
                  }

                  os << "<option";

                  if( i == value_offset )
                     os << " selected";

                  os << " value=\"" << info.values[ i ].first << "\">"
                   << escape_markup( get_display_string( info.values[ i ].second ) ) << "&nbsp;&nbsp;</option>\n";

                  if( !efinish.empty( ) && info.values[ i ].first == efinish )
                     break;

                  // NOTE: Security level enumeration is stopped at the user's level so it is
                  // not possible for a user to create or modify an instance's security level
                  // to a level greater than the level the user has been granted.
                  if( extra_data.count( c_field_extra_security_level )
                   && ( sess_info.user_id.empty( ) || ( info.values[ i ].first == sess_info.user_slevel ) ) )
                     break;
               }

               os << "</select>";

               if( !enum_image_file.empty( ) )
                  os << "&nbsp;&nbsp;";
            }

            // NOTE: If is in edit but is protected then display the name
            // rather than an icon (which may actually be a "blank icon").
            if( is_in_edit && is_protected_field )
               enum_image_file.erase( );

            if( !enum_image_file.empty( ) )
               os << "<img src=\"" << enum_image_file << "\" border=\"0\">";
            else if( !add_to_field_list )
               os << data_or_nbsp( escape_markup( cell_data ) );
         }
         else if( source.bool_fields.count( source_value_id ) )
         {
            if( has_value && !is_protected_field )
               cell_data = user_values.find( source_field_id )->second;

            if( sess_info.checkbox_bools || is_always_editable || ( is_in_edit && !is_protected_field ) )
            {
               if( is_always_editable || ( is_in_edit && !is_protected_field ) )
                  add_to_field_list = true;

               bool is_checked = false;

               if( cell_data == "1" )
                  is_checked = true;

               if( sess_info.checkbox_bools )
               {
                  os << "<input";

                  if( is_protected_field || ( !is_in_edit && !is_always_editable ) )
                     os << " disabled";

                  os << " type=\"checkbox\"";

                  if( !is_protected_field && ( is_in_edit || is_always_editable ) )
                     os << " name=\"field_" << source_field_id << "\"";

                  os << ( is_checked ? " checked=\"checked\"" : "" ) << " value=\"" << cell_data
                   << "\" onkeypress=\"return form_keys( event, " << enter_action << ", " << cancel_action << " );\"";
               }
               else
                  os << "<select name=\"field_" << source_field_id
                   << "\" onkeypress=\"return form_keys( event, " << enter_action << ", " << cancel_action << " );\"";

               // NOTE: If a boolean field has been marked as a "key" or with "force" then changing its
               // value is expected to affect other fields so a "cont" action is executed on change.
               if( !is_protected_field && ( extra_data.count( c_view_field_extra_force )
                || extra_data.count( c_view_field_extra_key0 ) || extra_data.count( c_view_field_extra_key1 )
                || extra_data.count( c_view_field_extra_key2 ) || extra_data.count( c_view_field_extra_key3 )
                || extra_data.count( c_view_field_extra_key4 ) || extra_data.count( c_view_field_extra_key5 )
                || extra_data.count( c_view_field_extra_key6 ) || extra_data.count( c_view_field_extra_key7 )
                || extra_data.count( c_view_field_extra_key8 ) || extra_data.count( c_view_field_extra_key9 ) ) )
               {
                  if( sess_info.checkbox_bools )
                     os << " onclick=\"";
                  else
                     os << " onchange=\"";

                  if( use_url_checksum )
                  {
                     string checksum_values( cont_act
                      + string( c_cmd_view ) + data + source.vici->second->id + user_select_key );

                     if( is_in_edit )
                        checksum_values += sess_info.checksum_serial;

                     string new_checksum( get_checksum( sess_info, checksum_values ) );

                     os << "query_update( '" << c_param_ochksum << "', query_value( '" << c_param_chksum << "' ), true ); ";
                     os << "query_update( '" << c_param_chksum << "', '" + new_checksum + "', true ); ";
                  }

                  os << "dyn_load( null, 'act=" << cont_act;
                  os << "&" << c_param_userfetch << "=" << source_field_id;
                  os << "&app=' + get_all_field_values( document." << source.id << " ), false );\"";
               }

               os << ">";

               if( !sess_info.checkbox_bools )
               {
                  os << "<option";

                  if( cell_data == c_true_value )
                     os << " selected";

                  os << " value=\"" << c_true_value << "\">" << GDS( c_display_true ) << "&nbsp;&nbsp;</option>\n";

                  os << "<option";

                  if( cell_data == c_false_value )
                     os << " selected";

                  os << " value=\"" << c_false_value << "\">" << GDS( c_display_false ) << "&nbsp;&nbsp;</option>\n";

                  // FUTURE: This provides support for a tri-state boolean (currently not in use).
                  if( !source.mandatory_fields.count( source_value_id ) )
                  {
                     os << "<option";

                     if( cell_data.empty( ) )
                        os << " selected";

                     os << " value=\"\">" << GDS( c_display_unknown ) << "&nbsp;&nbsp;</option>\n";
                  }

                  os << "</select>";
               }
            }
            else
            {
               if( cell_data == "1" )
                  os << GDS( c_display_true );
               else if( !cell_data.empty( ) )
                  os << GDS( c_display_false );
               else // FUTURE: This provides support for a tri-state boolean (currently not in use).
                  os << GDS( c_display_unknown );
            }
         }
         else if( source.int_fields.count( source_value_id ) )
         {
            if( extra_data.count( c_field_extra_int_type ) )
            {
               string int_type = extra_data[ c_field_extra_int_type ];

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
               if( extra_data.count( c_field_extra_mask ) )
                  mask = extra_data[ c_field_extra_mask ];

               cell_data = format_int_value( atoi( cell_data.c_str( ) ), mask );
            }

            check_for_link_and_output = true;
         }
         else if( source.numeric_fields.count( source_value_id ) )
         {
            if( extra_data.count( c_field_extra_numeric_type ) )
            {
               string numeric_type = extra_data[ c_field_extra_numeric_type ];

               if( numeric_type == "bytes" )
                  cell_data = format_bytes( numeric( cell_data.c_str( ) ).as_uint64( ) );
               else
                  throw runtime_error( "unsupported numeric_type '" + numeric_type + "'" );
            }
            else
            {
               string mask( c_default_numeric_mask );
               if( extra_data.count( c_field_extra_mask ) )
                  mask = extra_data[ c_field_extra_mask ];

               cell_data = format_numeric_value( numeric( cell_data.c_str( ) ), mask );
            }

            check_for_link_and_output = true;
         }
         else if( source.date_fields.count( source_value_id ) )
         {
            if( is_new_record )
            {
               if( source.defcurrent_fields.count( source_value_id ) )
                  cell_data = dt_current.get_date( ).as_string( );
               else if( !source.protected_fields.count( source_value_id ) )
                  cell_data.erase( );
            }

            if( !cell_data.empty( ) )
            {
               udate ud( cell_data );

               string date_precision;
               if( extra_data.count( c_field_extra_date_precision ) )
                  date_precision = extra_data[ c_field_extra_date_precision ];

               cell_data = format_date( ud, date_precision.c_str( ) );
            }

            check_for_link_and_output = true;
         }
         else if( source.time_fields.count( source_value_id ) )
         {
            if( is_new_record )
            {
               if( source.defcurrent_fields.count( source_value_id ) )
                  cell_data = dt_current.get_time( ).as_string( );
               else if( !source.protected_fields.count( source_value_id ) )
                  cell_data.erase( );
            }

            if( !cell_data.empty( ) )
            {
               mtime mt( cell_data );

               string time_precision;
               if( extra_data.count( c_field_extra_time_precision ) )
                  time_precision = extra_data[ c_field_extra_time_precision ];

               cell_data = format_time( mt, time_precision.c_str( ) );
            }

            check_for_link_and_output = true;
         }
         else if( source.datetime_fields.count( source_value_id ) )
         {
            bool is_current = false;

            if( is_new_record )
            {
               if( source.defcurrent_fields.count( source_value_id ) )
               {
                  is_current = true;
                  cell_data = dt_current.as_string( );
               }
               else if( !source.protected_fields.count( source_value_id ) )
                  cell_data.erase( );
            }

            if( !cell_data.empty( ) )
            {
               date_time dt( cell_data );

               if( !is_current && sess_info.tz_name.empty( ) )
                  dt += ( seconds )sess_info.gmt_offset;

               string time_precision;

               if( extra_data.count( c_field_extra_time_precision ) )
                  time_precision = extra_data[ c_field_extra_time_precision ];

               cell_data = format_date_time( dt, time_precision.c_str( ) );
            }

            check_for_link_and_output = true;
         }
         else
         {
            // NOTE: For new "cloned" records make sure that the "creator/modifier" fields are blanked out.
            if( is_new_record
             && ( source_field_id == source.create_user_key_field || source_field_id == source.modify_user_key_field ) )
               cell_data.erase( );

            if( is_new_record && source_field_id != pfield
             && source.new_field_values.count( source_field_id ) )
            {
               cell_data = source.new_field_values.find( source_field_id )->second;

               if( source.parent_lists.count( source_field_id ) )
               {
                  const data_container& parent_row_data = source.parent_lists.find( source_field_id )->second;

                  for( size_t j = 0; j < parent_row_data.size( ); j++ )
                  {
                     string key( parent_row_data[ j ].first );
                     string display( parent_row_data[ j ].second );

                     // NOTE: Remove parent version information as its not really relevant for foreign key selection.
                     size_t pos = key.find( ' ' );
                     if( pos != string::npos )
                        key.erase( pos );

                     if( display.empty( ) )
                        display = key;

                     if( key == cell_data )
                     {
                        cell_data = display;
                        break;
                     }
                  }
               }
            }

            check_for_link_and_output = true;
         }

         if( check_for_link_and_output )
         {
            bool is_foreign_key_link = false;

            // NOTE: If more than one parent field is found in the view then only provide a hyperlink
            // for the first parent field - also don't provide any hyperlinks for printable versions.
            if( !is_printable
             && !fk_refs.count( source_field_id )
             && source.link_fields.count( source_value_id )
             && ( !source.owner_link_fields.count( source_value_id ) || is_record_owner )
             && ( !source.admin_link_fields.count( source_value_id ) || sess_info.is_admin_user )
             && source.fk_field_values.count( source_field_id )
             && mod_info.view_cids.count( source.fk_field_classes.find( source_field_id )->second ) )
            {
               if( source.fk_field_values.find( source_field_id )->second.empty( ) )
               {
                  // NOTE: If a new field value had been explicitly provided by the interface
                  // specification then just ignore the empty foreign key field mapped value.
                  if( !is_new_record || !source.new_field_values.count( source_field_id ) )
                     cell_data.erase( );
               }
               else if( !is_in_edit && !cell_data.empty( ) )
               {
                  is_foreign_key_link = true;
                  fk_refs.insert( source_field_id );

                  os << "<a href=\""
                   << get_module_page_name( source.module_ref ) << "?cmd=" << c_cmd_view
                   << "&data=" << source.fk_field_values.find( source_field_id )->second << "&ident="
                   << mod_info.view_cids.find( source.fk_field_classes.find( source_field_id )->second )->second;

                  if( !user_select_key.empty( ) )
                     os << "&" << c_param_uselect << "=" << user_select_key;

                  if( !using_session_cookie )
                     os << "&session=" << session_id;

                  if( use_url_checksum )
                  {
                     string checksum_values( string( c_cmd_view )
                      + source.fk_field_values.find( source_field_id )->second
                      + mod_info.view_cids.find( source.fk_field_classes.find( source_field_id )->second )->second + user_select_key );

                     os << "&" << c_param_chksum << "=" << get_checksum( sess_info, checksum_values );
                  }

                  os << "\" onclick=\"javascript:replace_with_scroll_info( true );\">";
               }
            }

            if( source.manual_link_fields.count( source_value_id ) )
            {
               bool output_hrefs = true;

               if( is_in_edit || is_printable
                || source.manual_link_ignores.count( source_value_id ) )
                  output_hrefs = false;

               replace_links_and_output( cell_data, source.vici->second->id,
                source.module, source.module_ref, os, false, output_hrefs, session_id,
                sess_info, user_select_key, using_session_cookie, use_url_checksum );

               cell_data.erase( );
            }

            // NOTE: If a fixed parent field is one of the fields in the view its display value is
            // obtained in the same way as a normal foreign key would be (although it is not editable).
            if( ( source_field_id == pfield ) && source.parent_lists.count( source_field_id ) )
            {
               string value( extra );

               const data_container& parent_row_data = source.parent_lists.find( source_field_id )->second;

               for( size_t j = 0; j < parent_row_data.size( ); j++ )
               {
                  string key( parent_row_data[ j ].first );
                  string display( parent_row_data[ j ].second );

                  // NOTE: Remove parent version information as its not really relevant for foreign key selection.
                  size_t pos = key.find( ' ' );

                  if( pos != string::npos )
                     key.erase( pos );

                  if( key != value )
                     continue;

                  if( display.empty( ) )
                     display = key;

                  cell_data = display;

                  break;
               }
            }

            // NOTE: Change the password value to asterisks for display or if applicable
            // replace underbars with spaces.
            if( source.password_fields.count( source_value_id )
             || source.hpassword_fields.count( source_value_id ) )
               cell_data = string( cell_data.length( ), '*' );
            else if( source.replace_underbar_fields.count( source_value_id ) )
               cell_data = replace_underbars( cell_data );

            if( !special_prefix.empty( ) )
               cell_data = special_prefix + " " + cell_data;

            if( !special_suffix.empty( ) )
               cell_data += " " + special_suffix;

            os << data_or_nbsp( escape_markup( unescaped( cell_data ) ) );

            if( is_foreign_key_link )
               os << "</a>";
         }

         if( add_to_field_list )
         {
            if( !field_list.empty( ) )
               field_list += ',';

            field_list += source_field_id;
         }
      }

      os << "</" << td_type << ">\n";
      os << "</tr>\n";

      if( !finished_head && view_extras.count( c_view_type_extra_use_first_row_as_header ) )
      {
         os << "</thead>\n";

         finished_head = true;

         os << "<tbody>\n";
      }
   }

   clear_key( sid );

   if( display_tabs && !has_displayed_tabs )
      output_view_tabs( os, source, sess_info, vtab_num, is_in_edit, data, user_select_key,
       use_url_checksum, cont_act, back_count, is_new_record, is_record_owner, is_user_class_view );

   os << "</tbody>\n";
   os << "</table>\n";

   if( is_ui_prototype( ) )
      os << "</div>\n";

   if( is_in_edit || !source.has_file_attachments || ( source.state & c_state_uneditable ) )
      os << "</form>\n";

   if( extra_fields.empty( ) )
      extra_content_func += "extra_fields = '';\n";
   else
      extra_content_func += "extra_fields = '" + extra_fields + "';\n";

   if( extra_values.empty( ) )
      extra_content_func += "extra_values = '';\n";
   else
      extra_content_func += "extra_values = '" + replaced( extra_values, "\"", "\\x22", "'", "\\x27" ) + "';\n";

   if( source.state & c_state_is_changing )
      is_changing = true;

   return is_editable;
}

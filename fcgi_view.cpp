// Copyright (c) 2011
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
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
#include "fcgi_utils.h"
#include "ciyam_interface.h"

using namespace std;

namespace
{

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
 bool is_new_record, bool is_record_owner )
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

      if( !sess_info.is_admin_user
       && has_perm_extra( c_field_extra_admin_only, extra_data, sess_info ) )
         continue;

      if( !is_new_record && !is_record_owner
       && has_perm_extra( c_view_field_extra_owner_only, extra_data, sess_info ) )
         continue;

      if( !is_new_record && !sess_info.is_admin_user && !is_record_owner
       && has_perm_extra( c_view_field_extra_admin_owner_only, extra_data, sess_info ) )
         continue;

      if( i + 1 == vtab_num )
         os << "<th class=\"tab\" align=\"center\">" << mod_info.get_string( source.tab_names[ i ] ) << "</th>\n";
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

}

void setup_view_fields( view_source& view,
 const view_info& vinfo, const module_info& mod_info,
 const session_info& sess_info, const string& ident, const set< string >& login_opts,
 const string& module_id, const string& module_ref, bool is_in_edit, bool is_new_record )
{
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
   view.attached_file_field = vinfo.file_field_id;

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

         if( extra_data.count( c_field_extra_html ) )
            view.html_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_text ) )
            view.text_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_notes ) )
            view.notes_fields.insert( value_id );

         if( extra_data.count( c_view_field_extra_upper ) )
            view.upper_fields.insert( value_id );

         if( extra_data.count( c_view_field_extra_actions )
          || has_perm_extra( c_field_extra_hidden, extra_data, sess_info )
          || ( !is_in_edit
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
          && has_perm_extra( c_field_extra_admin_only, extra_data, sess_info ) ) )
            view.hidden_fields.insert( value_id );

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

         if( extra_data.count( c_field_extra_manual_link ) )
            view.manual_link_fields.insert( value_id );

         if( extra_data.count( c_field_extra_ignore_links ) )
            view.manual_link_ignores.insert( value_id );

         if( fld.pclass.empty( ) )
         {
            // NOTE: It is expected that only one actions and hpassword salt field will exist in a view.
            if( extra_data.count( c_view_field_extra_actions ) )
               view.actions_field = field_id;

            if( extra_data.count( c_view_field_extra_hpassword_salt ) )
               view.hpassword_salt_field = field_id;

            if( extra_data.count( c_view_field_extra_password ) )
               view.password_fields.insert( value_id );
            else if( extra_data.count( c_view_field_extra_epassword ) )
               view.epassword_fields.insert( value_id );
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
                     value = get_string( value.substr( pos + 1 ).c_str( ) );
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
            view.file_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_image ) )
            view.image_fields.insert( value_id );
         else if( extra_data.count( c_field_extra_mailto ) )
            view.mailto_fields.insert( value_id );

         if( fld.mandatory )
            view.mandatory_fields.insert( value_id );

         if( extra_data.count( c_field_extra_uom ) )
            view.uom_fields.insert( make_pair( value_id, extra_data[ c_field_extra_uom ] ) );

         if( extra_data.count( c_view_field_extra_force ) )
            view.user_force_fields.push_back( field_id );

         if( extra_data.count( c_view_field_extra_forced ) )
            view.server_forced_fields.push_back( field_id );

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

      bool non_prefixed = false;
      if( extra_data.count( c_field_extra_non_prefixed ) )
         non_prefixed = true;

      string display_name( get_display_string( fld.name ) );
      if( field_id_counts[ field_id ] > 0 )
      {
         if( non_prefixed )
            display_name = get_display_string( fld.pfname );
         else
            display_name += " " + get_display_string( fld.pfname );
      }
      view.display_names.push_back( display_name );

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
   if( !data.empty( ) && data[ 0 ] == ' ' )
      is_new_record = true;

   bool is_in_edit = false;
   if( act == c_act_edit || ( is_new_record && ( act.empty( ) || act == c_act_undo ) ) )
      is_in_edit = true;

   bool is_editable( !is_in_edit );

   string cont_act = is_in_edit ? c_act_cont : c_act_view;

   const map< string, string >& view_extras( source.vici->second->extras );

   bool is_no_edit = has_perm_extra( c_view_type_extra_no_edit, view_extras, sess_info );
   bool is_no_print = has_perm_extra( c_view_type_extra_no_print, view_extras, sess_info );
   bool is_admin_edit = has_perm_extra( c_view_type_extra_admin_edit, view_extras, sess_info );
   bool is_owner_edit = has_perm_extra( c_view_type_extra_owner_edit, view_extras, sess_info );
   bool is_admin_owner_edit = has_perm_extra( c_view_type_extra_admin_owner_edit, view_extras, sess_info );

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
      int is_effective_owner_field = atoi( source.field_values.find( source.is_effective_owner_field )->second.c_str( ) );
      if( is_effective_owner_field )
         is_record_owner = true;
   }

   // NOTE: If is the "user_info" view and the key matches the current user then "is_owner".
   if( source.vici->second->id == get_storage_info( ).user_info_view_id && data == sess_info.user_key )
      is_record_owner = true;

   string image_width( to_string( sess_info.image_width ) );
   string image_height( to_string( sess_info.image_height ) );

   if( !source.orientation_field.empty( ) )
   {
      int is_portrait = atoi( source.field_values.find( source.orientation_field )->second.c_str( ) );
      if( is_portrait )
         swap( image_height, image_width );
   }

   string enter_action = "null";
   string cancel_action = "null";

   if( !is_printable )
   {
      // NOTE: A form is used for the view even if not editing (for send always fields) provided
      // that the view does not contain a file attachment (as this requires a form itself). Thus
      // send always fields will not work if the view has a file attachment.
      // FUTURE: If file upload forms are placed in a seperate "iframe" then this limitation can
      // be removed.
      if( is_in_edit || source.attached_file_field.empty( ) )
         os << "<form name=\"" << source.id << "\" id=\"" << source.id << "\">\n";

      os << "<table class=\"list\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n";
      os << "<tr><td>";

      if( !is_in_edit )
      {
         bool had_any = false;

         if( !is_no_edit
          && ( !is_admin_edit || sess_info.is_admin_user )
          && ( !is_owner_edit || owner.empty( ) || is_record_owner ) )
         {
            // NOTE: Unowned records (marked as "owner edit") cannot be edited, however,
            // record specific actions will still appear (unless the record is changing
            // or is currently unactionable). If "admin_owner_edit" is used then "edit"
            // will only be allowed for owned records unless user is "admin". Also when
            // the view has been identified as the "user_info" view then unless user is
            // "admin" or the user key matches the current user prevent editing.
            if( !( source.state & c_state_uneditable )
             && !( source.state & c_state_is_changing )
             && ( !is_owner_edit || is_record_owner )
             && ( !is_admin_owner_edit || sess_info.is_admin_user || is_record_owner )
             && ( sess_info.is_admin_user || mod_info.user_info_view_id != source.vici->second->id || data == sess_info.user_key ) )
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
                  os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true );";
               }

               os << " dyn_load( null, 'act="
                << c_act_edit << "&app=' + get_all_field_values( document." << source.id
                << " ) + '" << "&chk=" << source.key_info << "', false );\" style=\"cursor:pointer\">";
            }
            else
               is_editable = false;

            string all_actions( source.actions_value );

            if( view_extras.count( c_view_type_extra_can_copy ) )
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
                  os << c_nbsp;
               else
                  had_any = true;

               output_actions( os, source, c_cmd_view, data,
                sess_info, source.vici->second->id, source.key_info,
                source.vici->second->cid, source.vici->second->mclass, all_actions, owner,
                session_id, user_select_key, using_session_cookie, use_url_checksum, false, &enter_action );
            }
         }
         else
         {
            is_editable = false;

            if( !source.actions_value.empty( )
             && ( !( source.state & c_state_unactionable )
             || view_extras.count( c_view_type_extra_ignore_unactionable ) ) )
            {
               if( had_any )
                  os << c_nbsp;
               else
                  had_any = true;

               output_actions( os, source, c_cmd_view, data,
                sess_info, source.vici->second->id, source.key_info,
                source.vici->second->cid, source.vici->second->mclass, source.actions_value, owner,
                session_id, user_select_key, using_session_cookie, use_url_checksum, false, &enter_action );
            }
         }

         if( !had_any )
            os << c_nbsp;

         os << "</td>";
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

         bool go_back_after_save = false;
         if( view_extras.count( c_view_type_extra_auto_back ) )
            go_back_after_save = is_new_record;

         if( go_back_after_save )
            os << " + '&back=1'";

         os << " + '&exec=" << exec << "&flags=" << source.state << "', false );";

         if( go_back_after_save )
            os << " if( !had_act_error ) jump_back = true;";

         os << "\" style=\"cursor:pointer\">";

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

            os << "var old_checksum = query_value( '" << c_param_chksum << "' ); ";
            os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
         }

         os << "dyn_load( null, 'act=" << c_act_undo << "', false );";
         if( use_url_checksum )
            os << " query_update( '" << c_param_chksum << "', old_checksum, true );";

         os << "\" style=\"cursor:pointer\">";

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
         os << "</td>";
      }

      if( !error_message.empty( ) )
      {
         // NOTE: Don't display as an error unless was actually received that way from the server.
         if( error_message.find( GDS( c_display_error ) ) != 0 )
         {
            os << "<td>" << error_message << "</td>";
            extra_content_func += "had_act_error = false;\n";
         }
         else
         {
            extra_content_func += "had_act_error = true;\n";
            os << "<td class=\"error\">" << remove_key( error_message ) << "</td>";
         }
      }
      else
         extra_content_func += "had_act_error = false;\n";

      bool has_started_right = false;

      // NOTE: If this view supports "quick linking" then create an "add quick link" action.
      if( !is_in_edit && source.has_quick_link && using_session_cookie
       && ( mod_info.user_qlink_permission.empty( ) || has_permission( mod_info.user_qlink_permission, sess_info ) ) )
      {
         if( !is_quick_link_view && ( sess_info.quick_link_data.size( ) < sess_info.quick_link_limit ) )
         {
            has_started_right = true;
            os << "<td class=\"right\">";

            string qlink_title( source.field_values.find( source.quick_link_value_id )->second );

            os << "<input id=\"quick_link\" name=\"quick_link\" type=\"button\" class=\"button\" value=\""
             << GDS( c_display_add_quick_link ) << "\" onclick=\"";

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
             << c_act_qlink << "&extra=" << escape_specials( qlink_title ) << "', false );\" style=\"cursor:pointer\">";
         }
      }

      if( !is_in_edit && !is_printable )
      {
         if( has_started_right )
            os << "&nbsp;&nbsp;";
         else
         {
            has_started_right = true;
            os << "<td class=\"right\">";
         }

         os << "<input id=\"edit\" name=\"edit\" type=\"button\" class=\"button\"";
         os << " value=\"" << GDS( c_display_back ) << "\" onclick=\"history.go( -" << back_count << " );\" style=\"cursor:pointer\">";

         has_started_right = true;
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
               if( has_started_right )
                  os << "&nbsp;&nbsp;";
               else
               {
                  has_started_right = true;
                  os << "<td class=\"right\">";
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

               found_print_version = true;
               break;
            }
         }

         if( !found_print_version )
         {
            if( has_started_right )
               os << "&nbsp;&nbsp;";
            else
            {
               has_started_right = true;
               os << "<td class=\"right\">";
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
         }
      }

      if( has_started_right )
         os << "</td>";

      os << "</tr></table>\n";
   }

   os << "<table class=\"list\" cellpadding=\"0\" cellspacing=\"0\" border=\"0\" width=\"100%\">\n";

   if( view_extras.count( c_view_type_extra_use_first_row_as_header ) )
      os << "<thead>\n";
   else
   {
      // NOTE: For a printable version do not bother with the field name/value column headings.
      if( !is_printable )
      {
         os << "<thead>\n";
         os << "<tr>\n";
         os << "   <th class=\"list\" width=\"30%\">" << GDS( c_display_field_name ) << "</th>\n";
         os << "   <th class=\"list\" >" << GDS( c_display_field_value ) << "</th>\n";
         os << "</tr>\n";
         os << "</thead>\n";
      }

      os << "<tbody>\n";
   }

   bool has_attached_file_link = false;

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
         string new_class_effect;
         if( source.state & state_modifiers[ j ] )
         {
            if( source.vici->second->modifiers.count( state_modifiers[ j ] ) )
               new_class_effect = source.vici->second->modifiers.find( state_modifiers[ j ] )->second;
         }

         if( new_class_effect == c_modifier_effect_lowlight || new_class_effect == c_modifier_effect_lowlight1
          || new_class_effect == c_modifier_effect_highlight || new_class_effect == c_modifier_effect_highlight1 )
            class_display_effect = new_class_effect;

         if( new_class_effect == c_modifier_effect_extralight || new_class_effect == c_modifier_effect_extralight1 )
            class_extra_effect = new_class_effect;
      }
   }

   int skip_tab_num = -1;
   int num_displayed = 0;
   bool hide_slevel = false;
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
         if( extra_data.count( c_view_field_extra_force ) )
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

      if( !is_new_record && !is_record_owner && source.owner_fields.count( source_value_id ) )
         continue;

      if( !is_new_record && !is_record_owner
       && has_perm_extra( c_view_field_extra_owner_only, extra_data, sess_info ) )
         continue;

      if( !is_new_record && !sess_info.is_admin_user && !is_record_owner
       && has_perm_extra( c_view_field_extra_admin_owner_only, extra_data, sess_info ) )
         continue;

      // NOTE: If the user has "level 0" security then don't display the security level.
      if( extra_data.count( c_field_extra_security_level ) )
      {
         if( !source.enum_fields.count( source_value_id ) )
            throw runtime_error( "security level enum not found for " + source_value_id );

         const enum_info& info(
          sinfo.enums.find( source.enum_fields.find( source_value_id )->second )->second );

         if( info.values[ 0 ].first == sess_info.user_slevel )
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
       && source.field_tab_ids[ i ] && source.field_tab_ids[ i ] != vtab_num )
         continue;

      // NOTE: As above for show more/less fields.
      if( !is_in_edit && skip_tab_num == source.field_tab_ids[ i ] )
         continue;

      // NOTE: If the field belongs to a tab which the user does not have access to
      // then this field must be skipped (otherwise URL tampering could allow these
      // fields to appear as 'vtab' is not part of the URL checksum).
      if( ( source.field_tab_ids[ i ] != 0 && vtab_num == source.field_tab_ids[ i ] ) )
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
      }

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

         string show_less_image( "show_less.gif" );
         string show_more_image( "show_more.gif" );

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

      // NOTE: If tabs exist and should be displayed (and haven't already been) then output them
      // now (after any context fields that preceeded the first tab heading have been displayed).
      if( display_tabs && !has_displayed_tabs && source.field_tab_ids[ i ] )
      {
         ++num_displayed; // NOTE: Increment for even/odd row display of row following the tabs.
         has_displayed_tabs = true;

         output_view_tabs( os, source, sess_info, vtab_num, is_in_edit, data,
          user_select_key, use_url_checksum, cont_act, back_count, is_new_record, is_record_owner );
      }

      // NOTE: Determine whether fields should be protected, relegated or displayed differently according
      // to modifiers and state. It is being assumed here that the original view fields and source fields
      // have the same offsets.
      string display_effect( class_display_effect ), extra_effect( class_extra_effect ), view_edit_effect;
      if( !view_extras.count( c_view_type_extra_print_no_highlight ) )
      {
         for( size_t j = 0; j < ARRAY_SIZE( state_modifiers ); j++ )
         {
            string new_effect;
            if( source.state & state_modifiers[ j ] )
            {
               if( source.vici->second->fields[ i ].modifiers.count( state_modifiers[ j ] ) )
                  new_effect = source.vici->second->fields[ i ].modifiers.find( state_modifiers[ j ] )->second;
            }

            if( new_effect == c_modifier_effect_lowlight || new_effect == c_modifier_effect_lowlight1
             || new_effect == c_modifier_effect_highlight || new_effect == c_modifier_effect_highlight1 )
               display_effect = new_effect;

            if( new_effect == c_modifier_effect_relegate
             || ( view_edit_effect.empty( ) && new_effect == c_modifier_effect_protect ) )
               view_edit_effect = new_effect;

            if( new_effect == c_modifier_effect_extralight || new_effect == c_modifier_effect_extralight1 )
               extra_effect = new_effect;
         }
      }

      bool is_special_field = false;

      if( !view_edit_effect.empty( ) || source.protected_fields.count( source_value_id ) )
      {
         bool is_relegated = ( view_edit_effect == c_modifier_effect_relegate );
         bool is_protected = ( view_edit_effect == c_modifier_effect_protect || source.protected_fields.count( source_value_id ) );

         if( source.field_values.count( source_value_id )
          && extra_data.count( c_view_field_extra_force )
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

      // NOTE: For printable versions skip the attached file field (if exists).
      if( is_printable
       && source_field_id == source.attached_file_field && !source.image_fields.count( source_value_id ) )
         continue;

      if( !is_new_record && source_field_id == c_key_field )
         cell_data = data;
      else if( source_field_id == pfield )
         cell_data = extra;

      if( source.field_values.count( source_value_id ) )
         cell_data = source.field_values.find( source_value_id )->second;

      string td_type( "td" );

      // NOTE: Hide rows that don't belong to the "active" tab (this only applies when editing)
      // as well as the "security level" field if the user has only the minimum security level.
      if( is_in_edit && ( ( skip_tab_num == source.field_tab_ids[ i ] )
       || ( extra_data.count( c_field_extra_security_level ) && hide_slevel )
       || ( source.field_tab_ids[ i ] && source.field_tab_ids[ i ] != vtab_num ) ) )
      {
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
       || source.image_fields.count( source_value_id )
       || source_field_id == source.attached_file_field ) ) )
      {
         os << "   <" << td_type << " class=\"list\">";
         os << escape_markup( is_in_edit ? source.edit_display_names[ i ] : source.display_names[ i ] );
         if( source.uom_fields.count( source_value_id ) )
            os << " (" << source.uom_fields.find( source_value_id )->second << ")";
         os << "</" << td_type << ">\n";
      }
      else
         td_extra = " colspan=\"2\"";

      string class_extra;
      if( !extra_effect.empty( ) )
         class_extra += " " + extra_effect;

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
       || source_field_id == source.attached_file_field
       || source_field_id == source.security_level_field
       || extra_data.count( c_field_extra_file ) || extra_data.count( c_field_extra_flink )
       || source_field_id == source.create_user_key_field || source_field_id == source.modify_user_key_field
       || source_field_id == source.create_datetime_field || source_field_id == source.modify_datetime_field )
         is_special_field = true;

      if( source.text_fields.count( source_value_id ) || source.notes_fields.count( source_value_id )
       || source.html_fields.count( source_value_id ) || source.enum_fields.count( source_value_id )
       || source.bool_fields.count( source_value_id ) || source.protected_fields.count( source_value_id ) )
         is_special_field = true;

      bool is_protected_field = false;
      if( view_edit_effect == c_modifier_effect_protect
       || new_field_and_values.count( source_field_id ) || source.protected_fields.count( source_value_id ) )
      {
         is_special_field = true;
         is_protected_field = true;
      }

      if( owner != sess_info.user_key
       && has_perm_extra( c_view_field_extra_owner_edit, extra_data, sess_info ) )
         is_protected_field = true;

      if( !sess_info.is_admin_user && owner != sess_info.user_key
       && has_perm_extra( c_view_field_extra_admin_owner_edit, extra_data, sess_info ) )
         is_protected_field = true;

      bool has_value = false;
      bool has_new_value = false;

      if( is_in_edit && user_values.count( source_field_id ) )
         has_value = true;

      bool is_always_editable = false;
      if( !is_printable && !is_protected_field
       && ( !is_in_edit || !extra_data.count( c_view_field_extra_view_only ) )
       && !( source.state & c_state_is_changing ) && extra_data.count( c_view_field_extra_always_editable ) )
         is_always_editable = true;

      // NOTE: The primary key and the parent folder of a "root" folder cannot be edited and record
      // editing below a parent does not allow the parent field value itself to be changed. Also any
      // "special fields" will require specialised editing (if editable) rather than standard handling.
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

         if( !field_list.empty( ) )
            field_list += ',';

         field_list += source_field_id;

         if( !source.parent_lists.count( source_field_id ) )
         {
            string cls( "textinput" );
            string type( "text" );
            string name( "field_" + source_field_id );

            int max_length = 100;

            string extra, extra_keys, validate, dt_extra( "false" );
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
                  {
                     date_time dt( date_time::standard( ) + ( seconds )sess_info.gmt_offset );
                     cell_data = to_string( dt.get_date( ).get_year( ) );
                  }

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
               if( !was_invalid && ud == udate( ) )
               {
                  is_empty = true;
                  cell_data.erase( );
               }

               if( is_empty && source.defcurrent_fields.count( source_value_id ) )
               {
                  date_time dt( date_time::standard( ) + ( seconds )sess_info.gmt_offset );
                  ud = dt.get_date( );
                  is_empty = false;
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
                  date_time dt( date_time::standard( ) + ( seconds )sess_info.gmt_offset );
                  mt = dt.get_time( );
                  is_empty = false;
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
               dt_extra = "true";
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

               if( is_empty && source.defcurrent_fields.count( source_value_id ) )
               {
                  dt = date_time::standard( ) + ( seconds )sess_info.gmt_offset;
                  is_empty = false;
               }

               string time_precision;
               if( extra_data.count( c_field_extra_time_precision ) )
                  time_precision = extra_data[ c_field_extra_time_precision ];

               if( time_precision == "minutes" )
               {
                  extra = "size=\"18\" ";
                  max_length = 16;
               }

               if( !is_empty )
                  cell_data = format_date_time( dt, time_precision.c_str( ) );
            }
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
             << "return form_keys( event, " << enter_action << ", " << cancel_action << " );\"></input>";

            if( is_datetime )
            {
               string range_extra;
               if( extra_data.count( c_field_extra_range ) )
               {
                  string::size_type pos = extra_data[ c_field_extra_range ].find( ".." );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected range format" );

                  range_extra = ", '" + extra_data[ c_field_extra_range ].substr( 0, pos ) + "'";
                  range_extra += ", '" + extra_data[ c_field_extra_range ].substr( pos + 2 ) + "'";
               }

               os << "&nbsp;<a href=\"javascript:NewCal( '" << name
                << "', 'yyyymmdd', " << dt_extra << ", 24" << range_extra << " );\">";
               os << "<img src=\"cal.gif\" width=\"16\" height=\"16\" border=\"0\" alt=\""
                << GDS( c_display_pick_a_date ) << "\"></a>";
            }
            else if( is_password )
            {
               if( !source.field_tab_ids[ i ] || source.field_tab_ids[ i ] == vtab_num )
               {
                  os << "</td>\n";
                  os << "</tr>\n";

                  if( ++num_displayed % 2 == 1 )
                     os << "<tr class=\"list\">\n";
                  else
                     os << "<tr class=\"list odd_data\">\n";

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
         }
         else
         {
            os << "<select name=\"field_" << source_field_id;
            if( !class_extra.empty( ) )
               os << "\" class=\"" << class_extra.substr( 1 );

            os << "\" onkeypress=\"return form_keys( event, " << enter_action << ", " << cancel_action << " );\"";

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

               if( use_url_checksum )
               {
                  string checksum_values( string( cont_act )
                   + string( c_cmd_view ) + data + source.vici->second->id + user_select_key );

                  if( is_in_edit )
                     checksum_values += sess_info.checksum_serial;

                  string new_checksum( get_checksum( sess_info, checksum_values ) );

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
               os << "&app=' + get_all_field_values( document." << source.id << " ), false );\"";
            }

            os << ">";

            const data_container& parent_row_data = source.parent_lists.find( source_field_id )->second;

            if( !source.mandatory_fields.count( source_value_id )
             && ( source.root_folder.empty( ) || !source.self_relationships.count( source_field_id ) ) )
               os << "<option value=\"\">&lt;" << GDS( c_display_none ) << "&gt;&nbsp;&nbsp;</option>\n";

            // NOTE: If mandatory then put "Choose" as an indicator that something needs to be selected.
            if( source.mandatory_fields.count( source_value_id ) )
               os << "<option value=\"\">&lt;" << GDS( c_display_choose ) << "&gt;&nbsp;&nbsp;</option>\n";

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

               // NOTE: For "self relationships" do not allow the record being edited to be chosen as its own parent.
               if( key == data && source.self_relationships.count( source_field_id ) )
                  continue;

               // NOTE: The "default" selection is based upon name rather than key (unless values have been submitted).
               os << "<option";
               if( has_new_value )
               {
                  if( key == cell_data )
                     os << " selected";
               }
               else if( ( has_value && key == cell_data ) || ( !has_value && unescaped( display ) == cell_data ) )
                  os << " selected";

               os << " value=\"" << key << "\">";

               if( source.manual_link_fields.count( source_value_id ) )
                  replace_links_and_output( display, source.vici->second->id,
                   source.module, source.module_ref, os, false, false, session_id,
                   sess_info, user_select_key, using_session_cookie, use_url_checksum );
               else
                  os << unescaped( display );

               os << "&nbsp;&nbsp;</option>\n";
            }

            os << "</select>";
         }
      }
      else
      {
         bool add_to_field_list = false;

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

            os << "<a href=\"" << type << cell_data << "\"" << extra << ">"
             << data_or_nbsp( escape_markup( unescaped( cell_data ) ) ) << "</a>";
         }
         else if( source.text_fields.count( source_value_id ) )
         {
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
               if( cell_data.find( "</HTML>" ) == string::npos && cell_data.find( "</html>" ) == string::npos )
               {
                  bool output_hrefs = true;
                  if( is_in_edit || is_printable
                   || source.manual_link_ignores.count( source_value_id ) )
                     output_hrefs = false;

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
         else if( source.file_fields.count( source_value_id ) || source_field_id == source.attached_file_field )
         {
            if( source.new_file_name == "." )
               cell_data.erase( );
            else if( !source.new_file_name.empty( ) )
               cell_data = source.new_file_name;

            bool can_attach_or_remove_file = true;

            if( is_admin_edit && !sess_info.is_admin_user )
               can_attach_or_remove_file = false;

            if( is_owner_edit && owner != sess_info.user_key )
               can_attach_or_remove_file = false;

            if( is_admin_owner_edit && !( sess_info.is_admin_user || owner == sess_info.user_key ) )
               can_attach_or_remove_file = false;

            if( source_field_id != source.attached_file_field )
               can_attach_or_remove_file = false;

            if( source.protected_fields.count( source_value_id ) )
               can_attach_or_remove_file = false;

            if( is_in_edit || ( source.state & c_state_uneditable ) || ( source.state & c_state_is_changing ) )
               can_attach_or_remove_file = false;

            if( !sess_info.is_admin_user && mod_info.user_info_view_id == source.vici->second->id && data != sess_info.user_key )
               can_attach_or_remove_file = false;

            // NOTE: If an instance can be identified as having been created by the currently logged in
            // user then if a file has yet to be attached this will be allowed, as it logically follows
            // that if a user can create a record they should also be able to attach a file to it.
            if( is_no_edit && ( !cell_data.empty( ) || source.create_user_key_field.empty( )
             || source.fk_field_values.find( source.create_user_key_field )->second != sess_info.user_key ) )
               can_attach_or_remove_file = false;

            if( cell_data.empty( ) )
            {
               if( can_attach_or_remove_file && !has_attached_file_link )
                  os << "<p>" << string_message( GDS( c_display_select_local_upload_file ),
                   make_pair( c_display_select_local_upload_file_parm_attach, GDS( c_display_attach ) ) ) << "</p>";
               else
                  os << c_nbsp;
            }
            else
            {
               string file_name( string( c_files_directory )
                + "/" + get_module_id_for_attached_file( source ) + "/" + ( source.vici->second )->cid + "/" );
               file_name += unescaped( cell_data );

               string::size_type pos = file_name.find_last_of( "." );

               string file_ext;
               if( pos != string::npos )
                  file_ext = file_name.substr( pos + 1 );

               if( file_exists( file_name ) )
               {
                  string tmp_link_path( c_files_directory );
                  tmp_link_path += "/" + string( c_tmp_directory );
                  tmp_link_path += "/" + session_id;

                  bool has_utf8_chars = false;
                  string link_file_name( uuid( ).as_string( ) );
                  if( !source.filename_field.empty( ) && source.field_values.count( source.filename_field ) )
                     link_file_name = valid_file_name( source.field_values.find( source.filename_field )->second, &has_utf8_chars );

                  create_tmp_file_link( tmp_link_path, file_name, file_ext, link_file_name );

                  bool has_image = false;
                  if( !is_in_edit && !is_printable )
                  {
                     has_attached_file_link = true;

                     if( !source.image_fields.count( source_value_id ) )
                     {
                        os << "<p>" << string_message( GDS( c_display_click_here_to_view_file ),
                         make_pair( c_display_click_here_to_view_file_parm_href,
                         "<a href=\"" + tmp_link_path + "\" target=\"_blank\">" ), "</a>",
                         make_pair( c_display_click_here_to_view_file_parm_ext, escape_markup( file_ext ) ) ) << "</p>";
                     }
                     else
                        has_image = true;
                  }
                  else
                  {
                     if( !source.image_fields.count( source_value_id ) )
                        os << "<p>" << string_message( GDS( c_display_has_file ),
                         make_pair( c_display_has_file_parm_ext, escape_markup( file_ext ) ) ) << "</p>";
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
                        image_src = "data:image/" + file_ext + ";base64," + base64::encode( buffer );
                     }

                     os << "<img src=\"" << image_src;

                     // NOTE: As
                     if( !embed_images )
                        os << "\" width=\"" << image_width << "\" height=\"" << image_height;

                     os << "\" border=\"0\" alt=\"" << GDS( c_display_image ) << "\">";

                     if( is_href )
                        os << "</a>";
                  }
               }
            }

            if( can_attach_or_remove_file )
            {
               string upload_info( session_id );
               upload_info += ":" + get_module_id_for_attached_file( source ) + "/" + ( source.vici->second )->cid;
               upload_info += ";" + data + "?" + to_string( sinfo.filesize_limit );

               os << "\n<form name=\"upload\" id=\"upload\" method=\"POST\" enctype=\"multipart/form-data\" action=\"upload.fcgi\" target=\"hidden_frame\">";
               os << "\n<input type=\""
                << ( has_attached_file_link ? "hidden" : "file" ) << "\" name=\"" << upload_info << "\"/>\n";
               os << "<input type=\"submit\" class=\"button\" value=\""
                << ( has_attached_file_link ? GDS( c_display_remove_attached_file ) : GDS( c_display_attach ) ) << "\"/>";
               os << "</form>";
            }
         }
         else if( source.enum_fields.count( source_value_id ) )
         {
            if( has_value && !is_protected_field )
               cell_data = user_values.find( source_field_id )->second;

            const enum_info& info(
             sinfo.enums.find( source.enum_fields.find( source_value_id )->second )->second );

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
                  if( file_exists( info.values[ i ].second + ".gif" ) )
                     enum_image_file = info.values[ i ].second + ".gif";
                  else if( file_exists( info.values[ i ].second + ".png" ) )
                     enum_image_file = info.values[ i ].second + ".png";

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

               bool found_start = false;
               for( size_t i = 0; i < info.values.size( ); i++ )
               {
                  // NOTE: Enum values that start with a '-' are not included for user selection
                  // as they are deemed as being only available for internal application purposes.
                  if( info.values[ i ].first[ 0 ] == '-' )
                     continue;

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
                   && info.values[ i ].first == sess_info.user_slevel )
                     break;
               }

               os << "</select>";

               if( !enum_image_file.empty( ) )
                  os << "&nbsp;&nbsp;";
            }

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

            os << data_or_nbsp( cell_data );
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

            os << data_or_nbsp( cell_data );
         }
         else if( source.date_fields.count( source_value_id ) )
         {
            if( is_new_record )
            {
               if( source.defcurrent_fields.count( source_value_id ) )
               {
                  date_time dt( date_time::standard( ) + ( seconds )sess_info.gmt_offset );
                  cell_data = dt.get_date( ).as_string( );
               }
               else
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

            os << data_or_nbsp( cell_data );
         }
         else if( source.time_fields.count( source_value_id ) )
         {
            if( is_new_record )
            {
               if( source.defcurrent_fields.count( source_value_id ) )
               {
                  date_time dt( date_time::standard( ) + ( seconds )sess_info.gmt_offset );
                  cell_data = dt.get_time( ).as_string( );
               }
               else
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

            os << data_or_nbsp( cell_data );
         }
         else if( source.datetime_fields.count( source_value_id ) )
         {
            if( is_new_record )
            {
               if( source.defcurrent_fields.count( source_value_id ) )
               {
                  date_time dt( date_time::standard( ) + ( seconds )sess_info.gmt_offset );
                  cell_data = dt.as_string( );
               }
               else
                  cell_data.erase( );
            }

            if( !cell_data.empty( ) )
            {
               date_time dt( cell_data );

               string time_precision;
               if( extra_data.count( c_field_extra_time_precision ) )
                  time_precision = extra_data[ c_field_extra_time_precision ];

               cell_data = format_date_time( dt, time_precision.c_str( ) );
            }

            os << data_or_nbsp( cell_data );
         }
         else
         {
            bool is_foreign_key_link = false;

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

                  os << "\">";
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
            if( source.password_fields.count( source_value_id ) || source.hpassword_fields.count( source_value_id ) )
               cell_data = string( cell_data.length( ), '*' );
            else if( source.replace_underbar_fields.count( source_value_id ) )
               cell_data = replace_underbars( cell_data );

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

      if( view_extras.count( c_view_type_extra_use_first_row_as_header ) )
      {
         os << "</thead>\n";
         os << "<tbody>\n";
      }
   }

   if( display_tabs && !has_displayed_tabs )
      output_view_tabs( os, source, sess_info, vtab_num, is_in_edit, data,
       user_select_key, use_url_checksum, cont_act, back_count, is_new_record, is_record_owner );

   os << "</tbody>\n";
   os << "</table>\n";

   if( is_in_edit || source.attached_file_field.empty( ) )
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


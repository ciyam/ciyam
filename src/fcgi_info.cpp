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
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "fcgi_info.h"

#include "sio.h"
#include "sockets.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#endif
#include "utilities.h"
#include "fcgi_utils.h"
#include "ciyam_interface.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

storage_info g_storage_info;

const size_t c_default_quick_link_limit = 10;

const char* const c_tab_field = "@tab";

const char* const c_section_enum = "enum";
const char* const c_section_enums = "enums";

const char* const c_section_value = "value";
const char* const c_section_values = "values";

const char* const c_section_view = "view";
const char* const c_section_views = "views";

const char* const c_section_field = "field";
const char* const c_section_fields = "fields";

const char* const c_section_list = "list";
const char* const c_section_lists = "lists";

const char* const c_section_module = "module";
const char* const c_section_modules = "modules";

const char* const c_section_column = "column";
const char* const c_section_columns = "columns";

const char* const c_section_parent = "parent";
const char* const c_section_parents = "parents";

const char* const c_section_restrict = "restrict";
const char* const c_section_restricts = "restricts";

const char* const c_attribute_id = "id";
const char* const c_attribute_cid = "cid";
const char* const c_attribute_pid = "pid";
const char* const c_attribute_data = "data";
const char* const c_attribute_name = "name";
const char* const c_attribute_perm = "perm";
const char* const c_attribute_type = "type";
const char* const c_attribute_view = "view";
const char* const c_attribute_title = "title";
const char* const c_attribute_extra = "extra";
const char* const c_attribute_vlink = "vlink";
const char* const c_attribute_class = "class";
const char* const c_attribute_field = "field";
const char* const c_attribute_ftype = "ftype";
const char* const c_attribute_order = "order";
const char* const c_attribute_bclass = "bclass";
const char* const c_attribute_pclass = "pclass";
const char* const c_attribute_pfield = "pfield";
const char* const c_attribute_pfname = "pfname";
const char* const c_attribute_pdname = "pdname";
const char* const c_attribute_pfldnm = "pfldnm";
const char* const c_attribute_pextra = "pextra";
const char* const c_attribute_ufield = "ufield";
const char* const c_attribute_nclass = "nclass";
const char* const c_attribute_nfield = "nfield";
const char* const c_attribute_nextra = "nextra";
const char* const c_attribute_dfenum = "dfenum";
const char* const c_attribute_dfield = "dfield";
const char* const c_attribute_dvalue = "dvalue";
const char* const c_attribute_filter = "filter";
const char* const c_attribute_module = "module";
const char* const c_attribute_unique = "unique";
const char* const c_attribute_actions = "actions";
const char* const c_attribute_filters = "filters";
const char* const c_attribute_indexed = "indexed";
const char* const c_attribute_use_tls = "use_tls";
const char* const c_attribute_pdf_spec = "pdf_spec";
const char* const c_attribute_sys_info = "sys_info";
const char* const c_attribute_url_opts = "url_opts";
const char* const c_attribute_home_info = "home_info";
const char* const c_attribute_mandatory = "mandatory";
const char* const c_attribute_menu_opts = "menu_opts";
const char* const c_attribute_modifiers = "modifiers";
const char* const c_attribute_row_limit = "row_limit";
const char* const c_attribute_user_info = "user_info";
const char* const c_attribute_user_perm = "user_perm";
const char* const c_attribute_blockchain = "blockchain";
const char* const c_attribute_user_qlink = "user_qlink";
const char* const c_attribute_login_days = "login_days";
const char* const c_attribute_login_opts = "login_opts";
const char* const c_attribute_notes_rmin = "notes_rmin";
const char* const c_attribute_notes_rmax = "notes_rmax";
const char* const c_attribute_operations = "operations";
const char* const c_attribute_user_extra = "user_extra";
const char* const c_attribute_user_group = "user_group";
const char* const c_attribute_user_other = "user_other";
const char* const c_attribute_image_width = "image_width";
const char* const c_attribute_notes_trunc = "notes_trunc";
const char* const c_attribute_notes_limit = "notes_limit";
const char* const c_attribute_print_limit = "print_limit";
const char* const c_attribute_user_parent = "user_parent";
const char* const c_attribute_user_active = "user_active";
const char* const c_attribute_user_select = "user_select";
const char* const c_attribute_user_slevel = "user_slevel";
const char* const c_attribute_user_unique = "user_unique";
const char* const c_attribute_embed_images = "embed_images";
const char* const c_attribute_encrypt_data = "encrypt_data";
const char* const c_attribute_image_height = "image_height";
const char* const c_attribute_rpc_password = "rpc_password";
const char* const c_attribute_storage_name = "storage_name";
const char* const c_attribute_user_tz_name = "user_tz_name";
const char* const c_attribute_module_prefix = "module_prefix";
const char* const c_attribute_user_has_auth = "user_has_auth";
const char* const c_attribute_view_show_key = "view_show_key";
const char* const c_attribute_checkbox_bools = "checkbox_bools";
const char* const c_attribute_filesize_limit = "filesize_limit";
const char* const c_attribute_user_pin_value = "user_pin_value";
const char* const c_attribute_print_list_opts = "print_list_opts";
const char* const c_attribute_user_gpg_install = "user_gpg_install";

const char* const c_list_field_parent_extra_folder = "[folder]";

template< typename T > void setup_modifiers( const string& modifiers, T& t )
{
   if( !modifiers.empty( ) )
   {
      vector< string > all_modifiers;
      split( modifiers, all_modifiers );

      for( size_t i = 0; i < all_modifiers.size( ); i++ )
      {
         string::size_type pos = all_modifiers[ i ].find( ':' );
         if( pos == string::npos )
            throw runtime_error( "unexpected modifier format '" + all_modifiers[ i ] + "'" );

         istringstream isstr( all_modifiers[ i ].substr( 0, pos ) );

         uint64_t flag;
         isstr >> hex >> flag;

         t.modifiers.insert( make_pair( flag, all_modifiers[ i ].substr( pos + 1 ) ) );
      }
   }
}

}

void module_info::clear( )
{
   name.erase( );
   title.erase( );

   home_info.erase( );

   user_class_id.erase( );
   user_class_name.erase( );

   user_info_view_id.erase( );

   user_uid_field_id.erase( );
   user_pwd_field_id.erase( );
   user_hash_field_id.erase( );
   user_name_field_id.erase( );
   user_crypt_field_id.erase( );
   user_email_field_id.erase( );

   user_perm_field_id.erase( );
   user_group_field_id.erase( );
   user_other_field_id.erase( );
   user_parent_field_id.erase( );
   user_active_field_id.erase( );
   user_slevel_field_id.erase( );
   user_unique_field_id.erase( );
   user_tz_name_field_id.erase( );
   user_has_auth_field_id.erase( );
   user_pin_value_field_id.erase( );
   user_read_only_field_id.erase( );
   user_gpg_install_proc_id.erase( );
   user_change_pwd_tm_field_id.erase( );

   user_select_perm.erase( );
   user_select_field.erase( );
   user_select_cfield.erase( );
   user_select_ofield.erase( );
   user_select_pfield.erase( );
   user_select_str_key.erase( );
   user_select_sl_field.erase( );
   user_select_uo_field.erase( );

   user_select_is_strict = false;
   allows_anonymous_access = false;

   user_qlink_list_id.erase( );
   user_qlink_class_id.erase( );
   user_qlink_pfield_id.erase( );
   user_qlink_url_field_id.erase( );
   user_qlink_name_field_id.erase( );
   user_qlink_test_field_id.erase( );
   user_qlink_test_field_val.erase( );
   user_qlink_order_field_id.erase( );
   user_qlink_checksum_field_id.erase( );
   user_qlink_permission.erase( );

   views.clear( );
   lists.clear( );

   view_info.clear( );
   list_info.clear( );

   list_menus.clear( );

   view_cids.clear( );
   view_classes.clear( );

   strings.clear( );
}

const string& module_info::get_string( const string& key ) const
{
   const string* p_string( &key );

   if( strings.count( key ) )
      p_string = &strings.find( key )->second;

   return *p_string;
}

storage_info::storage_info( )
 :
 use_tls( c_default_use_tls ),
 row_limit( c_default_row_limit ),
 login_days( c_default_login_days ),
 notes_rmin( c_default_notes_rmin ),
 notes_rmax( c_default_notes_rmax ),
 notes_trunc( c_default_notes_trunc ),
 notes_limit( c_default_notes_limit ),
 print_limit( c_default_print_limit ),
 image_width( c_default_image_width ),
 image_height( c_default_image_height ),
 embed_images( c_default_embed_images ),
 encrypt_data( c_default_encrypt_data ),
 view_show_key( c_default_view_show_key ),
 checkbox_bools( c_default_checkbox_bools ),
 filesize_limit( c_default_filesize_limit ),
 quick_link_limit( c_default_quick_link_limit )
{
}

void storage_info::clear( )
{
   use_tls = c_default_use_tls;
   row_limit = c_default_row_limit;
   notes_rmin = c_default_notes_rmin;
   notes_rmax = c_default_notes_rmax;
   notes_trunc = c_default_notes_trunc;
   notes_limit = c_default_notes_limit;
   print_limit = c_default_print_limit;

   image_width = c_default_image_width;
   image_height = c_default_image_height;

   embed_images = c_default_embed_images;
   encrypt_data = c_default_encrypt_data;

   view_show_key = c_default_view_show_key;
   checkbox_bools = c_default_checkbox_bools;

   filesize_limit = c_default_filesize_limit;

   quick_link_limit = c_default_quick_link_limit;

   url_opts.erase( );
   menu_opts.erase( );
   login_opts.erase( );
   blockchain.erase( );

   storage_name.erase( );
   module_prefix.erase( );

   print_list_opts.erase( );

   user_info_view_id.erase( );
   user_info_module_ref.erase( );

   web_root.erase( );

   enums.clear( );

   modules.clear( );
   modules_index.clear( );
}

bool storage_info::has_web_root( )
{
   guard g( lock_mutex );
   return !web_root.empty( );
}

string storage_info::get_module_ref( const string& module_name )
{
   string module_ref;

   if( module_prefix.empty( ) )
      module_ref = module_name;
   else
   {
      string::size_type pos = module_name.find( module_prefix );

      if( pos != 0 )
         throw runtime_error( "unexpected module prefix '"
          + module_prefix + "' not found in module name '" + module_name + "'" );

      module_ref = module_name.substr( module_prefix.length( ) );
   }

   return module_ref;
}

session_info::session_info( storage_info& si )
 :
 locked( false ),
 logged_in( false ),
 needs_pin( false ),
 is_meta_module( false ),
 was_file_remove( false ),
 change_pwd_tm( 0 ),
 checksum_serial( 0 ),
 pwd_encrypted( true ),
 is_openid( false ),
 is_read_only( false ),
 is_admin_user( false ),
 is_persistent( false ),
 row_limit( si.row_limit ),
 notes_rmin( si.notes_rmin ),
 notes_rmax( si.notes_rmax ),
 notes_trunc( si.notes_trunc ),
 notes_limit( si.notes_limit ),
 print_limit( si.print_limit ),
 image_width( si.image_width ),
 image_height( si.image_height ),
 view_show_key( si.view_show_key ),
 checkbox_bools( si.checkbox_bools ),
 print_list_opts( si.print_list_opts ),
 quick_link_limit( si.quick_link_limit ),
 gmt_offset( 0 ),
 dtm_offset( 0 ),
 p_socket( 0 )
{
   tm_created = time( 0 );
   tm_last_request = tm_created;

   hashval_prefix = uuid( ).as_string( );
   checksum_prefix = uuid( ).as_string( );

   user_slevel = string( c_default_security_level );
}

bool session_info::is_default_other( ) const
{
   bool default_other = ( user_other == default_user_other );

   if( other_aliases.count( user_other )
    && other_aliases.count( default_user_other ) )
   {
      default_other
       = ( other_aliases.find( user_other )->second == other_aliases.find( default_user_other )->second );
   }

   return default_other;
}

storage_info& get_storage_info( )
{
   return g_storage_info;
}

void read_storage_info( storage_info& info, vector< string > log_messages )
{
   string filename( c_fcgi_sio );

   ifstream inpf( filename.c_str( ) );

   if( inpf )
   {
      info.clear( );

      sio_reader reader( inpf );

      string use_tls = reader.read_opt_attribute( c_attribute_use_tls );

      if( use_tls == "1" || use_tls == c_true )
         info.use_tls = true;

      info.url_opts = reader.read_opt_attribute( c_attribute_url_opts );

      string row_limit = reader.read_opt_attribute( c_attribute_row_limit );

      if( !row_limit.empty( ) )
         info.row_limit = atoi( row_limit.c_str( ) );

      info.menu_opts = reader.read_opt_attribute( c_attribute_menu_opts );

      info.blockchain = reader.read_opt_attribute( c_attribute_blockchain );

      string login_days = reader.read_opt_attribute( c_attribute_login_days );

      if( !login_days.empty( ) )
         info.login_days = atoi( login_days.c_str( ) );

      info.login_opts = reader.read_opt_attribute( c_attribute_login_opts );

      string notes_rmin = reader.read_opt_attribute( c_attribute_notes_rmin );

      if( !notes_rmin.empty( ) )
         info.notes_rmin = atoi( notes_rmin.c_str( ) );

      string notes_rmax = reader.read_opt_attribute( c_attribute_notes_rmax );

      if( !notes_rmax.empty( ) )
         info.notes_rmax = atoi( notes_rmax.c_str( ) );

      string notes_trunc = reader.read_opt_attribute( c_attribute_notes_trunc );

      if( !notes_trunc.empty( ) )
         info.notes_trunc = atoi( notes_trunc.c_str( ) );

      string notes_limit = reader.read_opt_attribute( c_attribute_notes_limit );

      if( !notes_limit.empty( ) )
         info.notes_limit = atoi( notes_limit.c_str( ) );

      string print_limit = reader.read_opt_attribute( c_attribute_print_limit );

      if( !print_limit.empty( ) )
         info.print_limit = atoi( print_limit.c_str( ) );

      info.print_list_opts = reader.read_opt_attribute( c_attribute_print_list_opts );

      string image_width = reader.read_opt_attribute( c_attribute_image_width );

      if( !image_width.empty( ) )
         info.image_width = atoi( image_width.c_str( ) );

      string image_height = reader.read_opt_attribute( c_attribute_image_height );

      if( !image_height.empty( ) )
         info.image_height = atoi( image_height.c_str( ) );

      string embed_images = reader.read_opt_attribute( c_attribute_embed_images );

      if( !embed_images.empty( ) )
         info.embed_images = ( embed_images == c_true );

      string encrypt_data = reader.read_opt_attribute( c_attribute_encrypt_data );

      if( !encrypt_data.empty( ) )
         info.encrypt_data = ( encrypt_data == c_true );

      info.rpc_password = reader.read_opt_attribute( c_attribute_rpc_password );

      string view_show_key = reader.read_opt_attribute( c_attribute_view_show_key );

      if( !view_show_key.empty( ) )
         info.view_show_key = ( view_show_key == c_true );

      string checkbox_bools = reader.read_opt_attribute( c_attribute_checkbox_bools );

      if( !checkbox_bools.empty( ) )
         info.checkbox_bools = ( checkbox_bools == c_true );

      string filesize_limit = reader.read_opt_attribute( c_attribute_filesize_limit );

      if( !filesize_limit.empty( ) )
         info.filesize_limit = atol( filesize_limit.c_str( ) );

      info.storage_name = reader.read_attribute( c_attribute_storage_name );
      info.module_prefix = reader.read_attribute( c_attribute_module_prefix );

      if( reader.has_started_section( c_section_modules ) )
      {
         while( reader.has_started_section( c_section_module ) )
         {
            string module_name = reader.read_attribute( c_attribute_name );

            info.modules.push_back( module_info( ) );
            info.modules_index.insert( module_index_value_type( module_name, &info.modules.back( ) ) );

            if( read_module_info( module_name, info.modules.back( ), info ) )
            {
               if( info.user_info_view_id.empty( ) && !info.modules.back( ).user_info_view_id.empty( ) )
               {
                  string module_ref( info.get_module_ref( module_name ) );

                  info.user_info_module_ref = module_ref;
                  info.user_info_view_id = info.modules.back( ).user_info_view_id;
               }
            }
            else
            {
               log_messages.push_back( "*** '" + module_name + ".fcgi.sio' could not be loaded ***" );

               info.modules_index.erase( module_name );
               info.modules.pop_back( );
            }

            reader.finish_section( c_section_module );
         }

         reader.finish_section( c_section_modules );
      }

      inpf.close( );
      info.sio_mod = last_modification_time( filename );
   }
}

bool read_module_info( const string& name, module_info& info, storage_info& sinfo )
{
   string filename( name + c_fcgi_sio_ext );

   ifstream inpf( filename.c_str( ) );

   if( inpf )
   {
      sio_reader reader( inpf );

      info.clear( );

      info.id = reader.read_attribute( c_attribute_id );
      info.name = reader.read_attribute( c_attribute_name );
      info.perm = reader.read_attribute( c_attribute_perm );

      if( !info.perm.empty( ) )
      {
         vector< string > perms;
         split( info.perm, perms, ',' );

         vector< string > new_perms;

         for( int i = 0; i < perms.size( ); ++i )
         {
            if( perms[ i ] == "@anon" )
               info.allows_anonymous_access = true;
            else
               new_perms.push_back( perms[ i ] );
         }

         info.perm = join( new_perms, ',' );
      }

      info.title = reader.read_attribute( c_attribute_title );

      string sys_info = reader.read_opt_attribute( c_attribute_sys_info );

      if( !sys_info.empty( ) )
      {
         string str( sys_info );

         string::size_type pos = str.find( ':' );

         if( pos == string::npos )
            throw runtime_error( "unexpected sys_info format '" + sys_info + "'" );

         info.sys_class_id = str.substr( 0, pos );
         str.erase( 0, pos + 1 );

         vector< string > fields;
         split( str, fields );

         if( fields.size( ) != 5 )
            throw runtime_error( "unexpected sys_info format '" + sys_info + "'" );

         info.sys_name_field_id = fields[ 0 ];
         info.sys_vendor_field_id = fields[ 1 ];
         info.sys_actions_field_id = fields[ 2 ];
         info.sys_message_field_id = fields[ 3 ];
         info.sys_identity_field_id = fields[ 4 ];
      }

      string user_info = reader.read_attribute( c_attribute_user_info );

      if( !user_info.empty( ) )
      {
         string str( user_info );

         string::size_type pos = str.find( '@' );

         if( pos != string::npos )
         {
            info.user_change_pwd_tm_field_id = str.substr( pos + 1 );
            str.erase( pos );
         }

         pos = str.find( '!' );

         if( pos != string::npos )
         {
            info.user_read_only_field_id = str.substr( pos + 1 );
            str.erase( pos );
         }

         pos = str.find( ':' );

         if( pos != string::npos )
         {
            info.user_crypt_field_id = str.substr( pos + 1 );
            str.erase( pos );
         }

         pos = str.find( ';' );

         if( pos != string::npos )
         {
            info.user_email_field_id = str.substr( pos + 1 );
            str.erase( pos );
         }

         pos = str.find( ',' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user_info format '" + user_info + "'" );

         info.user_class_id = str.substr( 0, pos );
         str.erase( 0, pos + 1 );

         pos = str.find( ',' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user_info format '" + user_info + "'" );

         info.user_class_name = str.substr( 0, pos );
         str.erase( 0, pos + 1 );

         pos = str.find( ',' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user_info format '" + user_info + "'" );

         info.user_uid_field_id = str.substr( 0, pos );
         str.erase( 0, pos + 1 );

         pos = info.user_uid_field_id.find( '+' );

         if( pos != string::npos )
         {
            info.user_hdl_field_id = info.user_uid_field_id.substr( pos + 1 );
            info.user_uid_field_id.erase( pos );
         }

         pos = str.find( ',' );

         info.user_pwd_field_id = str.substr( 0, pos );

         if( pos != string::npos )
         {
            str.erase( 0, pos + 1 );
            pos = str.find( ',' );

            info.user_hash_field_id = str.substr( 0, pos );

            if( pos != string::npos )
               info.user_name_field_id = str.substr( pos + 1 );
         }
      }

      info.user_perm_field_id = reader.read_opt_attribute( c_attribute_user_perm );

      string user_extra_info = reader.read_opt_attribute( c_attribute_user_extra );
      if( !user_extra_info.empty( ) )
      {
         string::size_type pos = user_extra_info.find( ',' );
         info.user_extra1_field_id = user_extra_info.substr( 0, pos );

         if( pos != string::npos )
            info.user_extra2_field_id = user_extra_info.substr( pos + 1 );
      }

      info.user_group_field_id = reader.read_opt_attribute( c_attribute_user_group );

      if( !info.user_group_field_id.empty( ) )
      {
         string::size_type pos = info.user_group_field_id.find( ';' );

         if( pos != string::npos )
         {
            info.anon_ginfo = info.user_group_field_id.substr( pos + 1 );
            info.user_group_field_id.erase( pos );
         }
      }

      string user_qlink_info = reader.read_opt_attribute( c_attribute_user_qlink );

      if( !user_qlink_info.empty( ) )
      {
         string str( user_qlink_info );

         string::size_type pos = str.find( ':' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user qlink info format '" + user_qlink_info + "'" );

         info.user_qlink_class_id = str.substr( 0, pos );
         str.erase( 0, pos + 1 );

         pos = str.find( ',' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user qlink info format '" + user_qlink_info + "'" );

         info.user_qlink_pfield_id = str.substr( 0, pos );
         str.erase( 0, pos + 1 );

         pos = str.find( ',' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user qlink info format '" + user_qlink_info + "'" );

         info.user_qlink_url_field_id = str.substr( 0, pos );
         str.erase( 0, pos + 1 );

         pos = str.find( ',' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user qlink info format '" + user_qlink_info + "'" );

         info.user_qlink_name_field_id = str.substr( 0, pos );
         str.erase( 0, pos + 1 );

         pos = str.find( ',' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user qlink info format '" + user_qlink_info + "'" );

         info.user_qlink_order_field_id = str.substr( 0, pos );

         string::size_type tpos = info.user_qlink_order_field_id.find_last_of( ':' );

         if( tpos != string::npos )
         {
            string test_field_and_value = info.user_qlink_order_field_id.substr( 0, tpos );
            info.user_qlink_order_field_id.erase( 0, tpos + 1 );

            tpos = test_field_and_value.find( '=' );
            if( tpos == string::npos )
               throw runtime_error( "unexpected user qlink test format '" + test_field_and_value + "'" );

            info.user_qlink_test_field_id = test_field_and_value.substr( 0, tpos );
            info.user_qlink_test_field_val = test_field_and_value.substr( tpos + 1 );
         }

         info.user_qlink_checksum_field_id = str.substr( pos + 1 );

         pos = info.user_qlink_checksum_field_id.find( '!' );
         if( pos != string::npos )
         {
            info.user_qlink_permission = info.user_qlink_checksum_field_id.substr( pos + 1 );
            info.user_qlink_checksum_field_id.erase( pos );
         }
      }
      
      info.user_other_field_id = reader.read_opt_attribute( c_attribute_user_other );
      info.user_parent_field_id = reader.read_opt_attribute( c_attribute_user_parent );
      info.user_active_field_id = reader.read_opt_attribute( c_attribute_user_active );

      string user_select_info( reader.read_opt_attribute( c_attribute_user_select ) );

      if( !user_select_info.empty( ) )
      {
         if( user_select_info[ 0 ] == '!' )
         {
            info.user_select_is_strict = true;
            user_select_info.erase( 0, 1 );
         }

         string::size_type pos = user_select_info.find( '#' );

         if( pos != string::npos )
         {
            info.user_select_sl_field = user_select_info.substr( pos + 1 );
            user_select_info.erase( pos );
         }

         pos = user_select_info.find( '@' );

         if( pos != string::npos )
         {
            info.user_select_uo_field = user_select_info.substr( pos + 1 );
            user_select_info.erase( pos );
         }

         pos = user_select_info.find( ':' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user select info format '" + user_select_info + "'" );

         info.user_select_field = user_select_info.substr( 0, pos );
         user_select_info.erase( 0, pos + 1 );

         pos = info.user_select_field.find( '.' );

         if( pos != string::npos )
         {
            info.user_select_cfield = info.user_select_field.substr( pos + 1 );
            info.user_select_field.erase( pos );
         }

         pos = user_select_info.find( ';' );

         if( pos == string::npos )
            throw runtime_error( "unexpected user select info format '" + user_select_info + "'" );

         info.user_select_pfield = user_select_info.substr( 0, pos );
         user_select_info.erase( 0, pos + 1 );

         pos = info.user_select_pfield.find( '!' );

         if( pos != string::npos )
         {
            info.user_select_ofield = info.user_select_pfield.substr( pos + 1 );
            info.user_select_pfield.erase( pos );
         }

         pos = user_select_info.find( ',' );
         info.user_select_str_key = user_select_info.substr( 0, pos );

         if( pos != string::npos )
            info.user_select_perm = user_select_info.substr( pos + 1 );
      }

      info.user_slevel_field_id = reader.read_opt_attribute( c_attribute_user_slevel );

      info.user_unique_field_id = reader.read_opt_attribute( c_attribute_user_unique );
      info.user_tz_name_field_id = reader.read_opt_attribute( c_attribute_user_tz_name );
      info.user_has_auth_field_id = reader.read_opt_attribute( c_attribute_user_has_auth );
      info.user_pin_value_field_id = reader.read_opt_attribute( c_attribute_user_pin_value );
      info.user_gpg_install_proc_id = reader.read_opt_attribute( c_attribute_user_gpg_install );

      info.home_info = reader.read_attribute( c_attribute_home_info );

      if( reader.has_started_section( c_section_enums ) )
      {
         while( reader.has_started_section( c_section_enum ) )
         {
            enum_info e;

            e.id = reader.read_attribute( c_attribute_id );
            e.name = reader.read_attribute( c_attribute_name );

            reader.start_section( c_section_values );

            while( reader.has_started_section( c_section_value ) )
            {
               string data( reader.read_attribute( c_attribute_data ) );
               string name( reader.read_attribute( c_attribute_name ) );
               string filter( reader.read_opt_attribute( c_attribute_filter ) );

               e.values.push_back( make_pair( data, name ) );

               if( !filter.empty( ) )
                  e.filters.insert( make_pair( data, filter ) );

               reader.finish_section( c_section_value );
            }

            reader.finish_section( c_section_values );
            reader.finish_section( c_section_enum );

            // NOTE: In order for the fields in external classes to be correctly displayed
            // and edited enums are kept with the storage info rather than within a module.
            sinfo.enums.insert( make_pair( e.id, e ) );
         }

         reader.finish_section( c_section_enums );
      }

      if( reader.has_started_section( c_section_views ) )
      {
         while( reader.has_started_section( c_section_view ) )
         {
            view_info view;

            view.id = reader.read_attribute( c_attribute_id );
            view.cid = reader.read_attribute( c_attribute_cid );
            view.name = reader.read_attribute( c_attribute_name );
            view.type = reader.read_attribute( c_attribute_type );

            string::size_type pos = view.type.find( '=' );

            if( pos != string::npos )
            {
               view.perm = view.type.substr( pos + 1 );
               view.type.erase( pos );
            }

            string extra = reader.read_attribute( c_attribute_extra );

            vector< string > extras;

            if( !extra.empty( ) )
               split( extra, extras );

            for( size_t i = 0; i < extras.size( ); i++ )
            {
               if( extras[ i ] == c_view_type_extra_user_info )
                  info.user_info_view_id = view.id;

               string::size_type pos = extras[ i ].find( '=' );
               string key( extras[ i ].substr( 0, pos ) );
               string data;

               if( pos != string::npos )
                  data = extras[ i ].substr( pos + 1 );

               if( key == c_view_type_extra_static_instance_key )
                  view.static_instance_key = data;
               else
                  view.extras.insert( make_pair( key, data ) );
            }

            view.pdf_spec = reader.read_opt_attribute( c_attribute_pdf_spec );

            setup_modifiers( reader.read_attribute( c_attribute_modifiers ), view );

            view.vlink = reader.read_opt_attribute( c_attribute_vlink );

            view.mclass = reader.read_attribute( c_attribute_class );
            view.bclass = reader.read_attribute( c_attribute_bclass );
            view.module = reader.read_attribute( c_attribute_module );

            reader.start_section( c_section_fields );

            int tab_id = 0;

            while( reader.has_started_section( c_section_field ) )
            {
               fld_info fld;

               fld.name = reader.read_attribute( c_attribute_name );
               fld.field = reader.read_attribute( c_attribute_field );
               fld.ftype = reader.read_attribute( c_attribute_ftype );
               fld.extra = reader.read_attribute( c_attribute_extra );
               fld.mandatory = ( reader.read_attribute( c_attribute_mandatory ) == c_true );

               set< string > extras;
               if( !fld.extra.empty( ) )
                  split( fld.extra, extras, '+' );

               if( extras.count( c_field_extra_file ) || extras.count( c_field_extra_image ) )
                  view.file_ids.push_back( fld.field );

               if( extras.count( c_field_extra_filename ) )
                  view.filename_field_id = fld.field;

               setup_modifiers( reader.read_attribute( c_attribute_modifiers ), fld );

               fld.pclass = reader.read_opt_attribute( c_attribute_pclass );
               fld.pfield = reader.read_opt_attribute( c_attribute_pfield );
               fld.pfname = reader.read_opt_attribute( c_attribute_pfname );
               fld.pdname = reader.read_opt_attribute( c_attribute_pdname );
               fld.pextra = reader.read_opt_attribute( c_attribute_pextra );

               fld.tab_id = tab_id;

               if( fld.field != c_tab_field )
                  view.fields.push_back( fld );
               else
               {
                  ++tab_id;
                  view.tabs.push_back( make_pair( fld.name, fld.extra ) );
               }

               reader.finish_section( c_section_field );
            }

            reader.finish_section( c_section_fields );

            info.views.push_back( view );
            reader.finish_section( c_section_view );
         }

         reader.finish_section( c_section_views );

         for( size_t i = 0; i < info.views.size( ); i++ )
         {
            // NOTE: These first two containers are provided in order to locate the
            // "standard" class view and so must not consider the "print" versions.
            if( info.views[ i ].type != c_view_type_print
             && info.views[ i ].type != c_view_type_admin_print )
            {
               info.view_cids[ info.views[ i ].cid ] = info.views[ i ].id;
               info.view_classes[ info.views[ i ].mclass ] = info.views[ i ].id;
            }

            info.view_info.insert( make_pair( info.views[ i ].id, &info.views[ i ] ) );
         }
      }

      if( reader.has_started_section( c_section_lists ) )
      {
         while( reader.has_started_section( c_section_list ) )
         {
            list_info list;

            list.id = reader.read_attribute( c_attribute_id );
            list.cid = reader.read_attribute( c_attribute_cid );
            list.pid = reader.read_opt_attribute( c_attribute_pid );
            list.name = reader.read_attribute( c_attribute_name );
            list.type = reader.read_attribute( c_attribute_type );

            if( list.cid == info.user_qlink_class_id )
               info.user_qlink_list_id = list.id;

            string::size_type pos = list.type.find( '=' );

            if( pos != string::npos )
            {
               list.perm = list.type.substr( pos + 1 );
               list.type.erase( pos );
            }

            string extra = reader.read_attribute( c_attribute_extra );

            vector< string > extras;

            if( !extra.empty( ) )
               split( extra, extras );

            for( size_t i = 0; i < extras.size( ); i++ )
            {
               string::size_type pos = extras[ i ].find( '=' );
               string key( extras[ i ].substr( 0, pos ) );
               string data;

               if( pos != string::npos )
                  data = extras[ i ].substr( pos + 1 );

               list.extras.insert( make_pair( key, data ) );
            }

            string actions( reader.read_attribute( c_attribute_actions ) );

            if( !actions.empty( ) )
               split( actions, list.actions );

            list.pdf_spec = reader.read_opt_attribute( c_attribute_pdf_spec );

            setup_modifiers( reader.read_attribute( c_attribute_modifiers ), list );

            list.view = reader.read_opt_attribute( c_attribute_view );
            list.mclass = reader.read_attribute( c_attribute_class );
            list.bclass = reader.read_attribute( c_attribute_bclass );

            list.order = reader.read_attribute( c_attribute_order );

            list.pclass = reader.read_opt_attribute( c_attribute_pclass );
            list.pfield = reader.read_opt_attribute( c_attribute_pfield );
            list.pfldnm = reader.read_opt_attribute( c_attribute_pfldnm );
            list.ufield = reader.read_opt_attribute( c_attribute_ufield );

            list.nclass = reader.read_opt_attribute( c_attribute_nclass );
            list.nfield = reader.read_opt_attribute( c_attribute_nfield );
            list.nextra = reader.read_opt_attribute( c_attribute_nextra );

            if( list.nextra.empty( ) || list.nextra[ 0 ] != '~' )
               list.nexclude = false;
            else
            {
               list.nexclude = true;
               list.nextra.erase( 0, 1 );
            }

            list.dfenum = reader.read_opt_attribute( c_attribute_dfenum );
            list.dfield = reader.read_opt_attribute( c_attribute_dfield );
            list.dvalue = reader.read_opt_attribute( c_attribute_dvalue );

            list.module = reader.read_attribute( c_attribute_module );

            list.filters = reader.read_opt_attribute( c_attribute_filters );

            reader.start_section( c_section_columns );

            while( reader.has_started_section( c_section_column ) )
            {
               fld_info fld;

               fld.name = reader.read_attribute( c_attribute_name );
               fld.field = reader.read_attribute( c_attribute_field );
               fld.ftype = reader.read_attribute( c_attribute_ftype );
               fld.extra = reader.read_attribute( c_attribute_extra );

               setup_modifiers( reader.read_attribute( c_attribute_modifiers ), fld );

               fld.pclass = reader.read_opt_attribute( c_attribute_pclass );
               fld.pfield = reader.read_opt_attribute( c_attribute_pfield );
               fld.pfname = reader.read_opt_attribute( c_attribute_pfname );

               fld.unique = ( reader.read_attribute( c_attribute_unique ) == c_true );

               string indexed( reader.read_attribute( c_attribute_indexed ) );

               int index_count = 1;

               string::size_type pos = indexed.find( ':' );

               if( pos != string::npos )
               {
                  index_count = atoi( indexed.substr( pos + 1 ).c_str( ) );
                  indexed.erase( pos );
               }

               fld.indexed = ( indexed == c_true );
               fld.index_count = ( indexed == c_true ) ? index_count : 0;

               list.fields.push_back( fld );
               reader.finish_section( c_section_column );
            }

            reader.finish_section( c_section_columns );

            if( reader.has_started_section( c_section_parents ) )
            {
               while( reader.has_started_section( c_section_parent ) )
               {
                  par_info par;

                  par.name = reader.read_attribute( c_attribute_name );
                  par.field = reader.read_attribute( c_attribute_field );
                  par.extra = reader.read_opt_attribute( c_attribute_extra );

                  par.pclass = reader.read_attribute( c_attribute_pclass );
                  par.pfield = reader.read_attribute( c_attribute_pfield );
                  par.pextra = reader.read_attribute( c_attribute_pextra );

                  string::size_type fpos = par.pextra.find( c_list_field_parent_extra_folder );

                  if( fpos == 0 )
                  {
                     par.folder = true;
                     par.pextra.erase( 0, strlen( c_list_field_parent_extra_folder ) );
                  }
                  else if( fpos != string::npos )
                     throw runtime_error( "unexpected position for '" + to_string( c_list_field_parent_extra_folder ) + "' extra" );

                  string::size_type xpos = par.pextra.find( '-' );

                  if( xpos != string::npos )
                  {
                     string excludes( par.pextra.substr( xpos + 1 ) );
                     par.pextra.erase( xpos );

                     while( true )
                     {
                        xpos = excludes.find( '-' );
                        string next( excludes.substr( 0, xpos ) );

                        par.exclude_keys.insert( next );

                        if( xpos == string::npos )
                           break;

                        excludes.erase( 0, xpos + 1 );
                     }
                  }

                  par.mandatory = ( reader.read_attribute( c_attribute_mandatory ) == c_true );

                  string operations = reader.read_attribute( c_attribute_operations );

                  if( !operations.empty( ) )
                  {
                     vector< string > operation_items;
                     split( operations, operation_items, ';' );

                     for( size_t i = 0; i < operation_items.size( ); i++ )
                     {
                        string::size_type pos = operation_items[ i ].find( ':' );

                        string operation_key( operation_items[ i ].substr( 0, pos ) );

                        string operation_data;

                        if( pos != string::npos )
                           operation_data = operation_items[ i ].substr( pos + 1 );

                        par.operations.insert( make_pair( operation_key, operation_data ) );
                     }
                  }

                  list.parents.push_back( par );
                  reader.finish_section( c_section_parent );
               }

               reader.finish_section( c_section_parents );
            }

            if( reader.has_started_section( c_section_restricts ) )
            {
               while( reader.has_started_section( c_section_restrict ) )
               {
                  rest_info rest;

                  rest.name = reader.read_attribute( c_attribute_name );
                  rest.field = reader.read_attribute( c_attribute_field );
                  rest.ftype = reader.read_attribute( c_attribute_ftype );
                  rest.extra = reader.read_attribute( c_attribute_extra );

                  rest.mandatory = ( reader.read_attribute( c_attribute_mandatory ) == c_true );

                  string operations = reader.read_attribute( c_attribute_operations );

                  if( !operations.empty( ) )
                  {
                     vector< string > operation_items;
                     split( operations, operation_items, ';' );

                     for( size_t i = 0; i < operation_items.size( ); i++ )
                     {
                        string::size_type pos = operation_items[ i ].find( ':' );
                        string operation_key( operation_items[ i ].substr( 0, pos ) );
                        string operation_data;
                        if( pos != string::npos )
                           operation_data = operation_items[ i ].substr( pos + 1 );

                        rest.operations.insert( make_pair( operation_key, operation_data ) );
                     }
                  }

                  list.restricts.push_back( rest );
                  reader.finish_section( c_section_restrict );
               }

               reader.finish_section( c_section_restricts );
            }

            info.lists.push_back( list );
            reader.finish_section( c_section_list );
         }

         reader.finish_section( c_section_lists );
      }

      reader.verify_finished_sections( );

      inpf.close( );
      info.sio_mod = last_modification_time( filename );

      return true;
   }
   else
      return false;
}

void sort_row_data_manually( data_container& row_data, bool remove_manual_links )
{
   map< string, pair< string, string > > sorted_items;
   for( size_t i = 0; i < row_data.size( ); i++ )
   {
      if( !remove_manual_links )
         sorted_items.insert( make_pair( row_data[ i ].second,
          make_pair( row_data[ i ].first, row_data[ i ].second ) ) );
      else
         sorted_items.insert(
          make_pair( remove_links( row_data[ i ].second ),
          make_pair( row_data[ i ].first, row_data[ i ].second ) ) );
   }

   row_data.clear( );

   for( map< string, pair< string, string > >::iterator i
    = sorted_items.begin( ), end = sorted_items.end( ); i != end; ++i )
      row_data.push_back( make_pair( i->second.first, i->second.second ) );
}


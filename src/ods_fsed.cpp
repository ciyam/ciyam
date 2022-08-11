// Copyright (c) 2000-2015 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2015-2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory>
#  include <string>
#  include <sstream>
#  include <iostream>
#endif

#include "ods.h"
#include "base64.h"
#include "console.h"
#include "progress.h"
#include "date_time.h"
#include "utilities.h"
#include "fs_iterator.h"
#include "ods_file_system.h"
#include "console_commands.h"

using namespace std;

#include "ods_fsed.cmh"

namespace
{

const size_t c_max_pwd_size = 128;

const char* const c_opt_exclusive = "-x";

const char* const c_app_title = "ods_fsed";
const char* const c_app_version = "0.1";

const char* const c_cmd_password = "p";
const char* const c_cmd_exclusive = "x";
const char* const c_cmd_use_transaction_log = "tlg";
const char* const c_cmd_use_synchronised_write = "sync";
const char* const c_cmd_use_for_regression_tests = "test";

const char* const c_rename_attribute_type_input = "input";
const char* const c_rename_attribute_type_folder = "folder";
const char* const c_rename_attribute_type_output = "output";
const char* const c_rename_attribute_type_prefix = "prefix";
const char* const c_rename_attribute_type_remove = "remove";
const char* const c_rename_attribute_type_suffix = "suffix";

const char* const c_rename_attribute_type_input_any = "@any";
const char* const c_rename_attribute_type_input_hex = "@hex";
const char* const c_rename_attribute_type_input_b64 = "@b64";

const char* const c_rename_attribute_type_output_any = "@any";
const char* const c_rename_attribute_type_output_hex = "@hex";
const char* const c_rename_attribute_type_output_b64 = "@b64";

int64_t g_oid = 0;

string g_name( c_app_title );

bool g_encrypted = false;
bool g_shared_write = true;
bool g_use_transaction_log = false;
bool g_use_synchronised_write = false;
bool g_use_for_regression_tests = false;

bool g_application_title_called = false;

string application_title( app_info_request request )
{
   g_application_title_called = true;

   if( request == e_app_info_request_title )
      return string( c_app_title );
   else if( request == e_app_info_request_version )
      return string( c_app_version );
   else if( request == e_app_info_request_title_and_version )
   {
      string title( c_app_title );
      title += " v";
      title += string( c_app_version );

      return title;
   }
   else
   {
      ostringstream osstr;
      osstr << "unknown app_info_request: " << request;
      throw runtime_error( osstr.str( ) );
   }
}

string process_rename_expressions(
 vector< string >& rename_expressions, const string& folder, const string& input )
{
   string output( input );

   for( size_t i = 0; i < rename_expressions.size( ); i++ )
   {
      string next_expr( rename_expressions[ i ] );

      // NOTE: All rename expressions are in the form:
      // <attribute>=<value>[|<attribute>=<value>[...]]
      vector< string > attributes;

      split( next_expr, attributes, '|' );

      bool rename = true;

      string prefix, suffix, renamed_output( output );

      for( size_t j = 0; j < attributes.size( ); j++ )
      {
         string next_attr( attributes[ j ] );

         string::size_type pos = next_attr.find( '=' );

         if( pos == string::npos )
            throw runtime_error( "unepxected rename expr attribute '" + next_attr + "'" );

         string type( next_attr.substr( 0, pos ) );
         string value( next_attr.substr( pos + 1 ) );

         if( type == c_rename_attribute_type_input )
         {
            if( value == c_rename_attribute_type_input_any )
               ;
            else if( value == c_rename_attribute_type_input_hex )
            {
               if( !are_hex_nibbles( renamed_output ) )
                  rename = false;
               else
                  renamed_output = hex_decode( renamed_output );
            }
            else if( value == c_rename_attribute_type_input_b64 )
            {
               bool rc = false;

               base64::validate( renamed_output, &rc, true );

               if( !rc )
                  rename = false;
               else
                  renamed_output = base64::decode( renamed_output, true );
            }
            else
               throw runtime_error( "invalid rename expr attribute input type '" + value + "'" );
         }
         else if( type == c_rename_attribute_type_folder )
         {
            if( folder != value )
               rename = false;
         }
         else if( type == c_rename_attribute_type_output )
         {
            if( value == c_rename_attribute_type_output_any )
               renamed_output = output;
            else if( value == c_rename_attribute_type_output_hex )
            {
               string data( renamed_output );

               renamed_output = hex_encode( data );
            }
            else if( value == c_rename_attribute_type_output_b64 )
            {
               string data( renamed_output );

               renamed_output = base64::encode( data, true );
            }
            else
               throw runtime_error( "invalid rename expr attribute output type '" + value + "'" );
         }
         else if( type == c_rename_attribute_type_prefix )
            prefix = value;
         else if( type == c_rename_attribute_type_remove )
            renamed_output = replace( renamed_output, value, "" );
         else if( type == c_rename_attribute_type_suffix )
            suffix = value;

         if( !rename )
            break;
      }

      if( rename && !renamed_output.empty( ) )
         output = prefix + renamed_output + suffix;
   }

   return output;
}

void export_objects( ods_file_system& ofs, const string& directory,
 vector< string >& rename_expressions, ostream* p_os = 0, progress* p_progress = 0, int level = 0 )
{
   vector< string > files;

   if( p_os && level )
      *p_os << directory << ofs.get_folder( ) << endl;

   ofs.list_files( files, false );

   for( size_t i = 0; i < files.size( ); i++ )
   {
      string next( files[ i ] );
      string destination( next );

      if( !rename_expressions.empty( ) )
         destination = process_rename_expressions( rename_expressions, ofs.get_folder( ), next );

      if( p_os )
      {
         if( !level )
            *p_os << directory << '/' << next;
         else
            *p_os << directory << ofs.get_folder( ) << '/' << next;

         if( destination != next )
            *p_os << " ==> " << destination;
      }

      ofs.get_file( next, destination, p_os, false, p_progress );

      if( p_os )
         *p_os << endl;
   }

   vector< string > folders;

   ofs.list_folders( folders );

   for( size_t i = 0; i < folders.size( ); i++ )
   {
      string next( folders[ i ] );

      string cwd( get_cwd( ) );

      bool rc = false;
      set_cwd( next, &rc );

      if( !rc )
      {
         create_dir( next );
         set_cwd( next );
      }

      string folder( ofs.get_folder( ) );

      ofs.set_folder( next );
      export_objects( ofs, directory, rename_expressions, p_os, p_progress, level + 1 );

      ofs.set_folder( folder );

      set_cwd( cwd );
   }
}

void import_objects( ods_file_system& ofs, const string& directory,
 vector< string >& rename_expressions, ostream* p_os = 0, progress* p_progress = 0 )
{
   string cwd( get_cwd( ) );

   set_cwd( directory );

   directory_filter df;
   fs_iterator dfsi( get_cwd( ), &df );

   bool is_first = true;

   deque< string > folders;
   folders.push_back( get_cwd( ) );

   string folder( ofs.get_folder( ) );

   vector< string > all_folders;

   do
   {
      all_folders.push_back( dfsi.get_path_name( ) );
   } while( dfsi.has_next( ) );

   set_cwd( cwd );

   for( size_t i = 0; i < all_folders.size( ); i++ )
   {
      if( i > 0 )
      {
         string next_folder( all_folders[ i ] );
         string::size_type pos = next_folder.find( folders.back( ) );

         if( pos != 0 )
         {
            while( folders.size( ) > 1 )
            {
               folders.pop_back( );

               ofs.set_folder( ".." );

               if( next_folder.find( folders.back( ) ) == 0 )
                  break;
            }
         }

         next_folder.erase( 0, folders.back( ).length( ) + 1 );

         if( !ofs.has_folder( next_folder ) )
            ofs.add_folder( next_folder );

         ofs.set_folder( next_folder );

         folders.push_back( all_folders[ i ] );
      }

      vector< pair< string, string > > all_files;

      string next_folder( all_folders[ i ] );

      if( next_folder.find( directory ) == 0 )
         next_folder.erase( 0, directory.length( ) );

      if( !next_folder.empty( ) && next_folder[ 0 ] == '/' )
         next_folder.erase( 0, 1 );

      if( p_os && ( i > 0 ) )
         *p_os << next_folder << endl;

      file_filter ff;
      fs_iterator ffsi( all_folders[ i ], &ff );

      while( ffsi.has_next( ) )
         all_files.push_back( make_pair( ffsi.get_name( ), ffsi.get_full_name( ) ) );

      for( size_t j = 0; j < all_files.size( ); j++ )
      {
         string name( all_files[ j ].first );
         string source( all_files[ j ].second );

         if( !rename_expressions.empty( ) )
            name = process_rename_expressions( rename_expressions, ofs.get_folder( ), name );

         if( p_os )
         {
            *p_os << next_folder << '/' << all_files[ j ].first;

            if( name != all_files[ j ].first )
               *p_os << " ==> " << name;
         }

         ofs.store_file( name, source, 0, 0, p_progress );

         if( p_os )
            *p_os << endl;
      }
   }

   ofs.set_folder( folder );
}

}

class ods_fsed_startup_functor : public command_functor
{
   public:
   ods_fsed_startup_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& /*parameters*/ )
   {
      if( command == c_cmd_password )
         g_encrypted = true;
      else if( command == c_cmd_exclusive )
         g_shared_write = false;
      else if( command == c_cmd_use_transaction_log )
         g_use_transaction_log = true;
      else if( command == c_cmd_use_synchronised_write )
         g_use_synchronised_write = true;
      else if( command == c_cmd_use_for_regression_tests )
         g_use_for_regression_tests = true;
   }
};

class ods_fsed_command_functor;

class ods_fsed_command_handler : public console_command_handler
{
   friend class ods_fsed_command_functor;

   public:
   ods_fsed_command_handler( )
   {
      set_custom_startup_options( 2, "[<name> [<ident>]]" );
   }

   void init_ods( const char* p_file_name );

   private:
   auto_ptr< ods > ap_ods;
   auto_ptr< ods_file_system > ap_ofs;

   void process_custom_startup_option( size_t num, const string& option );
};

void ods_fsed_command_handler::init_ods( const char* p_file_name )
{
   string password;
   bool not_found = false;

   if( g_encrypted )
   {
      if( g_use_for_regression_tests )
         password = "test";
      else
      {
         char buffer[ c_max_pwd_size ];

         get_password( "Password: ", buffer, c_max_pwd_size );

         password.resize( strlen( buffer ) + 1 );
         strncpy( &password[ 0 ], buffer, password.length( ) );

         memset( buffer, '\0', c_max_pwd_size );
      }
   }

   const char* p_password = 0;

   if( !password.empty( ) )
      p_password = password.c_str( );

   ap_ods.reset( new ods( p_file_name, ods::e_open_mode_create_if_not_exist,
    ( g_shared_write ? ods::e_write_mode_shared : ods::e_write_mode_exclusive ),
    g_use_transaction_log, &not_found, p_password, g_use_synchronised_write ) );

   clear_key( password );

   if( not_found )
      throw runtime_error( "unexpected database not found" );

   if( ap_ods->is_corrupt( ) )
   {
      if( g_shared_write )
         throw runtime_error( "ODS DB is corrupt - re-start using exclusive write access in order to repair" );

      console_progress progress;
      console_progress* p_progress = has_option_no_progress( ) ? 0 : &progress;

      ap_ods->repair_corrupt_database( p_progress );
   }

   ap_ofs.reset( new ods_file_system( *ap_ods, g_oid ) );
}

void ods_fsed_command_handler::process_custom_startup_option( size_t num, const string& option )
{
   if( num == 0 )
      g_name = option;
   else if( num == 1 )
   {
      if( !option.empty( ) && option[ 0 ] >= '0' && option[ 0 ] <= '9' )
         g_oid = from_string< int64_t >( option );
      else
         throw runtime_error( "unexpected startup option '" + option + "'" );
   }
   else
      throw runtime_error( "unexpected startup option '" + option + "'" );
}

class ods_fsed_command_functor : public command_functor
{
   public:
   ods_fsed_command_functor( ods_fsed_command_handler& ods_fsed_handler )
    : command_functor( ods_fsed_handler ),
    ods_fsed_handler( ods_fsed_handler ),
    ap_ods( ods_fsed_handler.ap_ods ),
    ap_ofs( ods_fsed_handler.ap_ofs )
   {
      ods_fsed_handler.set_prompt_prefix( ap_ofs->get_folder( ) );
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   auto_ptr< ods >& ap_ods;

   ods_fsed_command_handler& ods_fsed_handler;

   auto_ptr< ods_file_system >& ap_ofs;
};

void ods_fsed_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   console_command_handler& console_handler( dynamic_cast< console_command_handler& >( handler ) );

   if( command == c_cmd_ods_fsed_cd )
   {
      string folder( get_parm_val( parameters, c_cmd_ods_fsed_cd_folder ) );

      if( folder.empty( ) )
         handler.issue_command_response( ap_ofs->get_folder( ) );
      else
      {
         folder = ap_ofs->determine_folder( folder, ods_fsed_handler.get_std_out( ) );

         if( !folder.empty( ) )
         {
            ap_ofs->set_folder( folder );
            ods_fsed_handler.set_prompt_prefix( folder );
         }
      }
   }
   else if( command == c_cmd_ods_fsed_files )
   {
      bool full( has_parm_val( parameters, c_cmd_ods_fsed_files_full ) );
      bool brief( has_parm_val( parameters, c_cmd_ods_fsed_files_brief ) );
      string expr( get_parm_val( parameters, c_cmd_ods_fsed_files_expr ) );

      ods_file_system::list_style style = ods_file_system::e_list_style_default;

      if( brief )
         style = ods_file_system::e_list_style_brief;
      else if( full )
         style = ods_file_system::e_list_style_extended;

      ap_ofs->list_files( expr, *ods_fsed_handler.get_std_out( ), style );
   }
   else if( command == c_cmd_ods_fsed_folders )
   {
      bool full( has_parm_val( parameters, c_cmd_ods_fsed_folders_full ) );
      string expr( get_parm_val( parameters, c_cmd_ods_fsed_folders_expr ) );

      ap_ofs->list_folders( expr, *ods_fsed_handler.get_std_out( ), full );
   }
   else if( command == c_cmd_ods_fsed_objects )
   {
      bool full( has_parm_val( parameters, c_cmd_ods_fsed_objects_full ) );
      bool brief( has_parm_val( parameters, c_cmd_ods_fsed_objects_brief ) );
      string expr( get_parm_val( parameters, c_cmd_ods_fsed_objects_expr ) );

      ods_file_system::list_style style = ods_file_system::e_list_style_default;

      if( brief )
         style = ods_file_system::e_list_style_brief;
      else if( full )
         style = ods_file_system::e_list_style_extended;

      ap_ofs->list_objects( expr, *ods_fsed_handler.get_std_out( ), style );
   }
   else if( command == c_cmd_ods_fsed_branch )
   {
      bool full( has_parm_val( parameters, c_cmd_ods_fsed_branch_full ) );
      bool brief( has_parm_val( parameters, c_cmd_ods_fsed_branch_brief ) );
      string expr( get_parm_val( parameters, c_cmd_ods_fsed_branch_expr ) );

      ods_file_system::branch_style style = ods_file_system::e_branch_style_default;

      if( brief )
         style = ods_file_system::e_branch_style_brief;
      else if( full )
         style = ods_file_system::e_branch_style_extended;

      if( has_parm_val( parameters, c_cmd_ods_fsed_branch_folders ) )
         ap_ofs->branch_folders( expr, *ods_fsed_handler.get_std_out( ), style );
      else
      {
         if( expr.empty( ) )
            expr = "*";

         if( !has_parm_val( parameters, c_cmd_ods_fsed_branch_objects ) )
            ap_ofs->branch_files( expr, *ods_fsed_handler.get_std_out( ), style );
         else
            ap_ofs->branch_objects( expr, *ods_fsed_handler.get_std_out( ), style );
      }
   }
   else if( command == c_cmd_ods_fsed_file_add )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_file_add_name ) );
      bool use_cin( has_parm_val( parameters, c_cmd_ods_fsed_file_add_cin ) );
      string file_name( get_parm_val( parameters, c_cmd_ods_fsed_file_add_file_name ) );

      console_progress progress;
      console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

      ap_ofs->add_file( name, file_name, ods_fsed_handler.get_std_out( ), use_cin ? &cin : 0, p_progress );
   }
   else if( command == c_cmd_ods_fsed_file_get )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_file_get_name ) );
      bool use_cout( has_parm_val( parameters, c_cmd_ods_fsed_file_get_cout ) );
      string file_name( get_parm_val( parameters, c_cmd_ods_fsed_file_get_file_name ) );

      string original_folder( ap_ofs->determine_strip_and_change_folder( name, ods_fsed_handler.get_std_out( ) ) );

      if( !name.empty( ) )
      {
         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ap_ofs->get_file( name, file_name, ods_fsed_handler.get_std_out( ), use_cout, p_progress );

         if( !original_folder.empty( ) )
            ap_ofs->set_folder( original_folder );
      }
   }
   else if( command == c_cmd_ods_fsed_file_link )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_file_link_name ) );
      string source( get_parm_val( parameters, c_cmd_ods_fsed_file_link_source ) );

      ap_ofs->link_file( name, source, ods_fsed_handler.get_std_out( ) );
   }
   else if( command == c_cmd_ods_fsed_file_move )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_file_move_name ) );
      string destination( get_parm_val( parameters, c_cmd_ods_fsed_file_move_destination ) );

      ap_ofs->move_file( name, destination, ods_fsed_handler.get_std_out( ) );
   }
   else if( command == c_cmd_ods_fsed_file_remove )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_file_remove_name ) );

      console_progress progress;
      console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

      ap_ofs->remove_file( name, ods_fsed_handler.get_std_out( ), p_progress );
   }
   else if( command == c_cmd_ods_fsed_file_replace )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_file_replace_name ) );
      bool use_cin( has_parm_val( parameters, c_cmd_ods_fsed_file_replace_cin ) );
      string file_name( get_parm_val( parameters, c_cmd_ods_fsed_file_replace_file_name ) );

      console_progress progress;
      console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

      ap_ofs->replace_file( name, file_name, ods_fsed_handler.get_std_out( ), use_cin ? &cin : 0, p_progress );
   }
   else if( command == c_cmd_ods_fsed_folder_add )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_file_add_name ) );

      ap_ofs->add_folder( name, ods_fsed_handler.get_std_out( ) );
   }
   else if( command == c_cmd_ods_fsed_folder_move )
   {
      bool overwrite( has_parm_val( parameters, c_cmd_ods_fsed_folder_move_overwrite ) );
      string name( get_parm_val( parameters, c_cmd_ods_fsed_folder_move_name ) );
      string destination( get_parm_val( parameters, c_cmd_ods_fsed_folder_move_destination ) );

      ap_ofs->move_folder( name, destination, overwrite, ods_fsed_handler.get_std_out( ) );
   }
   else if( command == c_cmd_ods_fsed_folder_remove )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_folder_remove_name ) );
      bool recurse( has_parm_val( parameters, c_cmd_ods_fsed_folder_remove_recurse ) );

      ap_ofs->remove_folder( name, ods_fsed_handler.get_std_out( ), recurse );
   }
   else if( command == c_cmd_ods_fsed_export )
   {
      string directory( get_parm_val( parameters, c_cmd_ods_fsed_export_directory ) );
      string rename_exprs( get_parm_val( parameters, c_cmd_ods_fsed_export_rename_exprs ) );

      vector< string > rename_expressions;

      if( !rename_exprs.empty( ) )
         split( rename_exprs, rename_expressions );

      string cwd( get_cwd( ) );

      bool rc = false;
      set_cwd( directory, &rc );

      if( !rc )
      {
         create_dir( directory );
         set_cwd( directory );
      }

      console_progress progress;
      console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

      ods::bulk_read bulk( *ap_ods );

      export_objects( *ap_ofs, directory, rename_expressions, ods_fsed_handler.get_std_out( ), p_progress );

      set_cwd( cwd );
   }
   else if( command == c_cmd_ods_fsed_import )
   {
      string directory( get_parm_val( parameters, c_cmd_ods_fsed_import_directory ) );
      string rename_exprs( get_parm_val( parameters, c_cmd_ods_fsed_import_rename_exprs ) );

      vector< string > rename_expressions;

      if( !rename_exprs.empty( ) )
         split( rename_exprs, rename_expressions );

      console_progress progress;
      console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

      ods::bulk_write bulk( *ap_ods, p_progress );

      ods::transaction tx( *ap_ods );

      import_objects( *ap_ofs, directory, rename_expressions, ods_fsed_handler.get_std_out( ), p_progress );

      tx.commit( );
   }
   else if( command == c_cmd_ods_fsed_label )
   {
      string name( get_parm_val( parameters, c_cmd_ods_fsed_label_name ) );

      ods::transaction label_tx( *ap_ods, name );
   }
   else if( command == c_cmd_ods_fsed_rewind )
   {
      bool is_dtm( has_parm_val( parameters, c_cmd_ods_fsed_rewind_dtm ) );
      bool is_last( has_parm_val( parameters, c_cmd_ods_fsed_rewind_last ) );
      bool is_unix( has_parm_val( parameters, c_cmd_ods_fsed_rewind_unix ) );
      string label_or_value( get_parm_val( parameters, c_cmd_ods_fsed_rewind_label_or_value ) );

      int64_t rewind_value = 0;

      if( is_dtm )
      {
         date_time dtm( label_or_value );
         rewind_value = unix_time( dtm );
      }
      else if( is_last )
         rewind_value = from_string< int64_t >( label_or_value ) * -1;
      else if( is_unix )
         rewind_value = from_string< int64_t >( label_or_value );

      if( is_dtm || is_last || is_unix )
         label_or_value.erase( );

      if( g_shared_write )
         handler.issue_command_response( "*** must be locked for exclusive write to perform this operation ***" );
      else
      {
         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ap_ods->rewind_transactions( label_or_value, rewind_value, p_progress );

         // NOTE: Need to reconstruct the ODS FS to ensure data integrity.
         ap_ofs.reset( new ods_file_system( *ap_ods, g_oid ) );
      }
   }
   else if( command == c_cmd_ods_fsed_rebuild )
   {
      ap_ofs->rebuild_index( );
   }
   else if( command == c_cmd_ods_fsed_dump )
   {
      string filename( get_parm_val( parameters, c_cmd_ods_fsed_dump_filename ) );

      ap_ofs->dump_node_data( filename, ods_fsed_handler.get_std_out( ) );
   }
   else if( command == c_cmd_ods_fsed_compress )
   {
      if( g_shared_write )
         handler.issue_command_response( "*** must be locked for exclusive write to perform this operation ***" );
      else
      {
         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ap_ods->move_free_data_to_end( p_progress );
      }
   }
   else if( command == c_cmd_ods_fsed_truncate )
   {
      if( g_shared_write )
         handler.issue_command_response( "*** must be locked for exclusive write to perform this operation ***" );
      else
         ap_ods->truncate_log( );
   }
   else if( command == c_cmd_ods_fsed_exit )
      handler.set_finished( );
}

command_functor* ods_fsed_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new ods_fsed_command_functor( dynamic_cast< ods_fsed_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   ods_fsed_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         cmd_handler.add_command( c_cmd_password, 1,
          "", "use encryption password", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_exclusive, 1,
          "", "use exclusive write access", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_use_transaction_log, 1,
          "", "use transaction log file", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_use_synchronised_write, 1,
          "", "use synchronised file write", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_use_for_regression_tests, 2,
          "", "use this for regression tests", new ods_fsed_startup_functor( cmd_handler ) );

         processor.process_commands( );

         cmd_handler.remove_command( c_cmd_password );
         cmd_handler.remove_command( c_cmd_exclusive );
         cmd_handler.remove_command( c_cmd_use_transaction_log );
         cmd_handler.remove_command( c_cmd_use_synchronised_write );
         cmd_handler.remove_command( c_cmd_use_for_regression_tests );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.init_ods( g_name.c_str( ) );

      cmd_handler.add_commands( 0,
       ods_fsed_command_functor_factory, ARRAY_PTR_AND_SIZE( ods_fsed_command_definitions ) );

      console_command_processor processor( cmd_handler );
      processor.process_commands( );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "unexpected exception occurred..." << endl;
      return 2;
   }
}

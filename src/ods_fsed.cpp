// Copyright (c) 2000-2015 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2015-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <stack>
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

const char c_trans_suffix = '!';

const char* const c_opt_exclusive = "-x";

const char* const c_app_title = "ods_fsed";
const char* const c_app_version = "0.1";

const char* const c_cmd_password = "p";
const char* const c_cmd_read_only_mode = "ro";
const char* const c_cmd_shared_write_mode = "sw";
const char* const c_cmd_use_transaction_log = "tlg";

const char* const c_cmd_exec = "exec";
const char* const c_cmd_exec_command = "command";
const char* const c_cmd_exec_arguments = "arguments";

const char* const c_cmd_use_for_regression_tests = "test";
const char* const c_cmd_use_unsynchronised_write = "no_sync";

const char* const c_cmd_reconstruct_from_transaction_log = "reconstruct";

const char* const c_error_prefix = "error: ";

int64_t g_oid = 0;

string g_name( c_app_title );

string g_exec_cmd;

bool g_encrypted = false;
bool g_read_only = false;
bool g_shared_write = false;
bool g_use_transaction_log = false;
bool g_use_for_regression_tests = false;
bool g_use_unsynchronised_write = false;
bool g_reconstruct_from_transaction_log = false;

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

}

class ods_fsed_startup_functor : public command_functor
{
   public:
   ods_fsed_startup_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters )
   {
      if( command == c_cmd_exec )
      {
         g_exec_cmd = get_parm_val( parameters, c_cmd_exec_command );

         string arguments( get_parm_val( parameters, c_cmd_exec_arguments ) );

         if( !arguments.empty( ) )
            g_exec_cmd += " " + arguments;
      }
      else if( command == c_cmd_password )
         g_encrypted = true;
      else if( command == c_cmd_read_only_mode )
         g_read_only = true;
      else if( command == c_cmd_shared_write_mode )
         g_shared_write = true;
      else if( command == c_cmd_use_transaction_log )
         g_use_transaction_log = true;
      else if( command == c_cmd_use_for_regression_tests )
         g_use_for_regression_tests = true;
      else if( command == c_cmd_use_unsynchronised_write )
         g_use_unsynchronised_write = true;
      else if( command == c_cmd_reconstruct_from_transaction_log )
         g_reconstruct_from_transaction_log = true;

      if( g_read_only && g_shared_write )
         throw runtime_error( "invalid option combination ('read only' and 'shared write')" );
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

   stack< string, deque< string > > folder_stack;
   stack< ods::transaction*, deque< ods::transaction* > > ods_tx_stack;

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

   if( g_read_only )
      ap_ods.reset( new ods( p_file_name, ods::e_open_mode_exist,
       ods::e_write_mode_none, g_use_transaction_log, &not_found, p_password ) );
   else
      ap_ods.reset( new ods( p_file_name, ods::e_open_mode_create_if_not_exist,
       ( g_shared_write ? ods::e_write_mode_shared : ods::e_write_mode_exclusive ),
       g_use_transaction_log, &not_found, p_password, !g_use_unsynchronised_write ) );

   clear_key( password );

   if( not_found )
      throw runtime_error( "unexpected database not found" );

   if( ap_ods->is_corrupt( ) || g_reconstruct_from_transaction_log )
   {
      if( g_shared_write )
      {
         if( !ap_ods->is_corrupt( ) )
            throw runtime_error( "restart using exclusive write access in order to reconstruct" );
         else
            throw runtime_error( "ODS DB is corrupt - restart using exclusive write access in order to repair/reconstruct" );
      }

      console_progress progress;
      console_progress* p_progress = has_option_no_progress( ) ? 0 : &progress;

      if( g_reconstruct_from_transaction_log )
         ap_ods->reconstruct_database( p_progress );
      else
         ap_ods->repair_corrupt_database( p_progress );
   }

   ap_ofs.reset( new ods_file_system( *ap_ods, g_oid, g_use_for_regression_tests ) );
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
    ap_ofs( ods_fsed_handler.ap_ofs ),
    folder_stack( ods_fsed_handler.folder_stack ),
    ods_tx_stack( ods_fsed_handler.ods_tx_stack )
   {
      ods_fsed_handler.set_prompt_prefix( ap_ofs->get_folder( ) );
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   ods_fsed_command_handler& ods_fsed_handler;

   auto_ptr< ods >& ap_ods;
   auto_ptr< ods_file_system >& ap_ofs;

   stack< string, deque< string > >& folder_stack;
   stack< ods::transaction*, deque< ods::transaction* > >& ods_tx_stack;
};

void ods_fsed_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   console_command_handler& console_handler( dynamic_cast< console_command_handler& >( handler ) );

   try
   {
      if( command == c_cmd_ods_fsed_cd )
      {
         string folder( get_parm_val( parameters, c_cmd_ods_fsed_cd_folder ) );

         if( folder.empty( ) )
            handler.issue_command_response( ap_ofs->get_folder( ) );
         else
         {
            folder = ap_ofs->determine_folder( folder );

            if( !folder.empty( ) )
            {
               ap_ofs->set_folder( folder );

               if( ods_tx_stack.size( ) )
                  folder += c_trans_suffix;

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

         ap_ofs->add_file( name, file_name, ( use_cin ? &cout : 0 ), ( use_cin ? &cin : 0 ), p_progress );
      }
      else if( command == c_cmd_ods_fsed_file_get )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_file_get_name ) );
         bool use_cout( has_parm_val( parameters, c_cmd_ods_fsed_file_get_cout ) );
         string file_name( get_parm_val( parameters, c_cmd_ods_fsed_file_get_file_name ) );

         string original_name( name );
         string original_folder( ap_ofs->determine_strip_and_change_folder( name ) );

         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ap_ofs->get_file( name, file_name, ( use_cout ? &cout : 0 ), p_progress );

         if( use_cout )
            cout << endl;

         if( !original_folder.empty( ) )
            ap_ofs->set_folder( original_folder );
      }
      else if( command == c_cmd_ods_fsed_file_link )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_file_link_name ) );
         string source( get_parm_val( parameters, c_cmd_ods_fsed_file_link_source ) );

         if( !source.empty( ) )
            ap_ofs->link_file( name, source );
         else
            *ods_fsed_handler.get_std_out( ) << ap_ofs->link_source( name ) << '\n';
      }
      else if( command == c_cmd_ods_fsed_file_links )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_file_links_name ) );

         ap_ofs->list_links( name, *ods_fsed_handler.get_std_out( ) );
      }
      else if( command == c_cmd_ods_fsed_file_move )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_file_move_name ) );
         string destination( get_parm_val( parameters, c_cmd_ods_fsed_file_move_destination ) );

         ap_ofs->move_file( name, destination );
      }
      else if( command == c_cmd_ods_fsed_file_remove )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_file_remove_name ) );

         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ap_ofs->remove_file( name, 0, p_progress );
      }
      else if( command == c_cmd_ods_fsed_file_replace )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_file_replace_name ) );
         bool use_cin( has_parm_val( parameters, c_cmd_ods_fsed_file_replace_cin ) );
         string file_name( get_parm_val( parameters, c_cmd_ods_fsed_file_replace_file_name ) );

         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ap_ofs->replace_file( name, file_name, 0, ( use_cin ? &cin : 0 ), p_progress );
      }
      else if( command == c_cmd_ods_fsed_file_permissions )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_file_permissions_name ) );
         string permissions( get_parm_val( parameters, c_cmd_ods_fsed_file_permissions_permissions ) );

         if( permissions.empty( ) )
            ap_ofs->permissions_file( name, ods_fsed_handler.get_std_out( ) );
         else
            ap_ofs->permissions_file( name, permissions, ods_fsed_handler.get_std_out( ) );
      }
      else if( command == c_cmd_ods_fsed_folder_add )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_file_add_name ) );

         ap_ofs->add_folder( name );
      }
      else if( command == c_cmd_ods_fsed_folder_move )
      {
         bool overwrite( has_parm_val( parameters, c_cmd_ods_fsed_folder_move_overwrite ) );
         string name( get_parm_val( parameters, c_cmd_ods_fsed_folder_move_name ) );
         string destination( get_parm_val( parameters, c_cmd_ods_fsed_folder_move_destination ) );

         ap_ofs->move_folder( name, destination, overwrite );
      }
      else if( command == c_cmd_ods_fsed_folder_remove )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_folder_remove_name ) );
         bool recurse( has_parm_val( parameters, c_cmd_ods_fsed_folder_remove_recurse ) );

         ap_ofs->remove_folder( name, 0, recurse );
      }
      else if( command == c_cmd_ods_fsed_export )
      {
         string directory( get_parm_val( parameters, c_cmd_ods_fsed_export_directory ) );
         string rename_exprs( get_parm_val( parameters, c_cmd_ods_fsed_export_rename_exprs ) );

         vector< string > rename_expressions;

         if( !rename_exprs.empty( ) )
            split( rename_exprs, rename_expressions );

         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ods::bulk_read bulk( *ap_ods );

         export_objects( *ap_ofs, directory, &rename_expressions, ods_fsed_handler.get_std_out( ), p_progress );
      }
      else if( command == c_cmd_ods_fsed_import )
      {
         bool force( has_parm_val( parameters, c_cmd_ods_fsed_import_force ) );
         string directory( get_parm_val( parameters, c_cmd_ods_fsed_import_directory ) );
         string rename_exprs( get_parm_val( parameters, c_cmd_ods_fsed_import_rename_exprs ) );

         vector< string > rename_expressions;

         if( !rename_exprs.empty( ) )
            split( rename_exprs, rename_expressions );

         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ods::bulk_write bulk( *ap_ods, p_progress );

         ods::transaction tx( *ap_ods );

         import_objects( *ap_ofs, directory, &rename_expressions, ods_fsed_handler.get_std_out( ), p_progress, force );

         tx.commit( );
      }
      else if( command == c_cmd_ods_fsed_label )
      {
         string name( get_parm_val( parameters, c_cmd_ods_fsed_label_name ) );

         if( ods_tx_stack.size( ) )
            throw runtime_error( "currently in a transaction" );

         ods::transaction label_tx( *ap_ods, name );
      }
      else if( command == c_cmd_ods_fsed_trans )
      {
         folder_stack.push( ap_ofs->get_folder( ) );
         ods_tx_stack.push( new ods::transaction( *ap_ods ) );

         handler.issue_command_response( "begin transaction (level = " + to_string( ods_tx_stack.size( ) ) + ")" );

         ods_fsed_handler.set_prompt_prefix( ap_ofs->get_folder( ) + c_trans_suffix );
      }
      else if( command == c_cmd_ods_fsed_commit )
      {
         if( !ods_tx_stack.size( ) )
            throw runtime_error( "not currently in a transaction" );

         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ods::bulk_write bulk( *ap_ods, p_progress );

         ods_tx_stack.top( )->commit( );

         handler.issue_command_response( "transaction committed (level = " + to_string( ods_tx_stack.size( ) ) + ")" );

         delete ods_tx_stack.top( );
         ods_tx_stack.pop( );

         folder_stack.pop( );

         string folder( ap_ofs->get_folder( true ) );

         if( !ods_tx_stack.empty( ) )
            folder += c_trans_suffix;

         ods_fsed_handler.set_prompt_prefix( folder );
      }
      else if( command == c_cmd_ods_fsed_rollback )
      {
         if( !ods_tx_stack.size( ) )
            throw runtime_error( "not currently in a transaction" );

         console_progress progress;
         console_progress* p_progress = console_handler.has_option_no_progress( ) ? 0 : &progress;

         ods::bulk_write bulk( *ap_ods, p_progress );

         string folder( ap_ofs->get_folder( ) );

         ods_tx_stack.top( )->rollback( );

         handler.issue_command_response( "transaction rolled back (level = " + to_string( ods_tx_stack.size( ) ) + ")" );

         delete ods_tx_stack.top( );
         ods_tx_stack.pop( );

         if( !ap_ofs->has_folder( folder ) )
            folder = folder_stack.top( );

         folder_stack.pop( );

         ap_ofs->set_folder( folder );

         folder = ap_ofs->get_folder( );

         if( !ods_tx_stack.empty( ) )
            folder += c_trans_suffix;

         ods_fsed_handler.set_prompt_prefix( folder );
      }
      else if( command == c_cmd_ods_fsed_rewind )
      {
         bool is_dtm( has_parm_val( parameters, c_cmd_ods_fsed_rewind_dtm ) );
         bool is_last( has_parm_val( parameters, c_cmd_ods_fsed_rewind_last ) );
         bool is_unix( has_parm_val( parameters, c_cmd_ods_fsed_rewind_unix ) );
         string label_or_value( get_parm_val( parameters, c_cmd_ods_fsed_rewind_label_or_value ) );

         if( ods_tx_stack.size( ) )
            throw runtime_error( "currently in a transaction" );

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

            ods_fsed_handler.set_prompt_prefix( ap_ofs->get_folder( ) );
         }
      }
      else if( command == c_cmd_ods_fsed_rebuild )
      {
         if( ods_tx_stack.size( ) )
            throw runtime_error( "currently in a transaction" );

         ap_ofs->rebuild_index( );
      }
      else if( command == c_cmd_ods_fsed_dump )
      {
         string filename( get_parm_val( parameters, c_cmd_ods_fsed_dump_filename ) );

         ap_ofs->dump_node_data( filename, ods_fsed_handler.get_std_out( ) );
      }
      else if( command == c_cmd_ods_fsed_compress )
      {
         if( ods_tx_stack.size( ) )
            throw runtime_error( "currently in a transaction" );

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
         bool reset( has_parm_val( parameters, c_cmd_ods_fsed_truncate_reset ) );

         if( ods_tx_stack.size( ) )
            throw runtime_error( "currently in a transaction" );

         if( g_shared_write )
            handler.issue_command_response( "*** must be locked for exclusive write to perform this operation ***" );
         else
            ap_ods->truncate_log( "", reset );
      }
      else if( command == c_cmd_ods_fsed_exit )
      {
         while( ods_tx_stack.size( ) )
         {
            handler.issue_command_response( "rollback transaction (level = " + to_string( ods_tx_stack.size( ) ) + ")" );

            delete ods_tx_stack.top( );
            ods_tx_stack.pop( );
         }

         handler.set_finished( );
      }
   }
   catch( exception& x )
   {
      handler.issue_command_response( to_string( c_error_prefix ) + x.what( ) );
   }
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
          "", "use crypt password", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_read_only_mode, 1,
          "", "use read only mode", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_shared_write_mode, 1,
          "", "use shared write mode", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_use_transaction_log, 1,
          "", "use transaction log file", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_exec, 2, "<val//command>[<list//arguments// >]",
          "single command to execute", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_use_for_regression_tests, 3,
          "", "use this for regression tests", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_use_unsynchronised_write, 3,
          "", "use unsynchronised file writing", new ods_fsed_startup_functor( cmd_handler ) );

         cmd_handler.add_command( c_cmd_reconstruct_from_transaction_log, 4,
          "", "use to reconstruct from transaction log", new ods_fsed_startup_functor( cmd_handler ) );

         processor.process_commands( );

         cmd_handler.remove_command( c_cmd_password );
         cmd_handler.remove_command( c_cmd_read_only_mode );
         cmd_handler.remove_command( c_cmd_shared_write_mode );
         cmd_handler.remove_command( c_cmd_exec );
         cmd_handler.remove_command( c_cmd_use_transaction_log );
         cmd_handler.remove_command( c_cmd_use_for_regression_tests );
         cmd_handler.remove_command( c_cmd_use_unsynchronised_write );
         cmd_handler.remove_command( c_cmd_reconstruct_from_transaction_log );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.init_ods( g_name.c_str( ) );

      if( g_reconstruct_from_transaction_log )
         cout << "ODS DB reconstruct completed." << endl;
      else
      {
         cmd_handler.add_commands( 0,
          ods_fsed_command_functor_factory, ARRAY_PTR_AND_SIZE( ods_fsed_command_definitions ) );

         console_command_processor processor( cmd_handler );

         if( g_exec_cmd.empty( ) )
            processor.process_commands( );
         else
            processor.execute_command( g_exec_cmd );
      }
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

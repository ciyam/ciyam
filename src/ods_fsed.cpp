// Copyright (c) 2000-2015 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2015-2016 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef __BORLANDC__
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
#include "utilities.h"
#include "ods_file_system.h"
#include "console_commands.h"

using namespace std;

#include "ods_fsed.cmh"

namespace
{

const char* const c_opt_exclusive = "-x";

const char* const c_app_title = "ods_fsed";
const char* const c_app_version = "0.1";

const char* const c_cmd_exclusive = "x";

int64_t g_oid = 0;

string g_name( c_app_title );

bool g_shared_access = true;

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

   void operator ( )( const string& command, const parameter_info& /*parameters*/ )
   {
      if( command == c_cmd_exclusive )
         g_shared_access = false;
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

   void init( );

   private:
   auto_ptr< ods > ap_ods;
   auto_ptr< ods_file_system > ap_ofs;

   void process_custom_startup_option( size_t num, const string& option );
};

void ods_fsed_command_handler::init( )
{
   ap_ods.reset( new ods( g_name.c_str( ), ods::e_open_mode_create_if_not_exist,
    ( g_shared_access ? ods::e_share_mode_shared : ods::e_share_mode_exclusive ) ) );

   ods::bulk_write bulk_write( *ap_ods );

   if( !g_shared_access && !ap_ods->is_new( ) )
      ap_ods->rollback_dead_transactions( );

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
   if( command == c_cmd_ods_fsed_cd )
   {
      string folder( get_parm_val( parameters, c_cmd_parm_ods_fsed_cd_folder ) );

      if( folder.empty( ) )
         cout << ap_ofs->get_folder( ) << endl;
      else
      {
         folder = ap_ofs->determine_folder( folder, &cout );

         if( !folder.empty( ) )
         {
            ap_ofs->set_folder( folder );
            ods_fsed_handler.set_prompt_prefix( folder );
         }
      }   
   }
   else if( command == c_cmd_ods_fsed_files )
   {
      bool full( has_parm_val( parameters, c_cmd_parm_ods_fsed_files_full ) );
      bool brief( has_parm_val( parameters, c_cmd_parm_ods_fsed_files_brief ) );
      string expr( get_parm_val( parameters, c_cmd_parm_ods_fsed_files_expr ) );

      ods_file_system::list_style style = ods_file_system::e_list_style_default;

      if( brief )
         style = ods_file_system::e_list_style_brief;
      else if( full )
         style = ods_file_system::e_list_style_extended;

      ap_ofs->list_files( expr, cout, style );
   }
   else if( command == c_cmd_ods_fsed_folders )
   {
      bool full( has_parm_val( parameters, c_cmd_parm_ods_fsed_folders_full ) );
      string expr( get_parm_val( parameters, c_cmd_parm_ods_fsed_folders_expr ) );

      ap_ofs->list_folders( expr, cout, full );
   }
   else if( command == c_cmd_ods_fsed_objects )
   {
      bool full( has_parm_val( parameters, c_cmd_parm_ods_fsed_objects_full ) );
      bool brief( has_parm_val( parameters, c_cmd_parm_ods_fsed_objects_brief ) );
      string expr( get_parm_val( parameters, c_cmd_parm_ods_fsed_objects_expr ) );

      ods_file_system::list_style style = ods_file_system::e_list_style_default;

      if( brief )
         style = ods_file_system::e_list_style_brief;
      else if( full )
         style = ods_file_system::e_list_style_extended;

      ap_ofs->list_objects( expr, cout, style );
   }
   else if( command == c_cmd_ods_fsed_branch )
   {
      bool full( has_parm_val( parameters, c_cmd_parm_ods_fsed_branch_full ) );
      bool brief( has_parm_val( parameters, c_cmd_parm_ods_fsed_branch_brief ) );
      string expr( get_parm_val( parameters, c_cmd_parm_ods_fsed_branch_expr ) );

      ods_file_system::branch_style style = ods_file_system::e_branch_style_default;

      if( brief )
         style = ods_file_system::e_branch_style_brief;
      else if( full )
         style = ods_file_system::e_branch_style_extended;

      if( has_parm_val( parameters, c_cmd_parm_ods_fsed_branch_folders ) )
         ap_ofs->branch_folders( expr, cout, style );
      else
      {
         if( expr.empty( ) )
            expr = "*";

         if( !has_parm_val( parameters, c_cmd_parm_ods_fsed_branch_objects ) )
            ap_ofs->branch_files( expr, cout, style );
         else
            ap_ofs->branch_objects( expr, cout, style );
      }
   }
   else if( command == c_cmd_ods_fsed_file_add )
   {
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_add_name ) );
      bool use_cin( has_parm_val( parameters, c_cmd_parm_ods_fsed_file_add_cin ) );
      string file_name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_add_file_name ) );

      ap_ofs->add_file( name, file_name, &cout, use_cin ? &cin : 0 );
   }
   else if( command == c_cmd_ods_fsed_file_get )
   {
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_get_name ) );
      bool use_cout( has_parm_val( parameters, c_cmd_parm_ods_fsed_file_get_cout ) );
      string file_name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_get_file_name ) );

      ap_ofs->get_file( name, file_name, &cout, use_cout );
   }
   else if( command == c_cmd_ods_fsed_file_link )
   {
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_link_name ) );
      string source( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_link_source ) );

      ap_ofs->link_file( name, source, &cout );
   }
   else if( command == c_cmd_ods_fsed_file_move )
   {
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_move_name ) );
      string destination( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_move_destination ) );

      ap_ofs->move_file( name, destination, &cout );
   }
   else if( command == c_cmd_ods_fsed_file_remove )
   {
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_remove_name ) );

      ap_ofs->remove_file( name, &cout );
   }
   else if( command == c_cmd_ods_fsed_file_replace )
   {
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_replace_name ) );
      bool use_cin( has_parm_val( parameters, c_cmd_parm_ods_fsed_file_replace_cin ) );
      string file_name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_replace_file_name ) );

      ap_ofs->replace_file( name, file_name, &cout, use_cin ? &cin : 0 );
   }
   else if( command == c_cmd_ods_fsed_folder_add )
   {
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_file_add_name ) );

      ap_ofs->add_folder( name, &cout );
   }
   else if( command == c_cmd_ods_fsed_folder_move )
   {
      bool overwrite( has_parm_val( parameters, c_cmd_parm_ods_fsed_folder_move_overwrite ) );
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_folder_move_name ) );
      string destination( get_parm_val( parameters, c_cmd_parm_ods_fsed_folder_move_destination ) );

      ap_ofs->move_folder( name, destination, overwrite, &cout );
   }
   else if( command == c_cmd_ods_fsed_folder_remove )
   {
      string name( get_parm_val( parameters, c_cmd_parm_ods_fsed_folder_remove_name ) );

      ap_ofs->remove_folder( name, &cout );
   }
   else if( command == c_cmd_ods_fsed_rebuild )
   {
      ap_ofs->rebuild_index( );
   }
   else if( command == c_cmd_ods_fsed_dump )
   {
      string filename( get_parm_val( parameters, c_cmd_parm_ods_fsed_dump_filename ) );

      ap_ofs->dump_node_data( filename, &cout );
   }
   else if( command == c_cmd_ods_fsed_compress )
   {
      if( g_shared_access )
         cout << "error: must be locked for exclusive use to perform this operation" << endl;
      else
      {
         cout << "moving free data to end..." << endl;
         ap_ods->move_free_data_to_end( );

         cout << "completed" << endl;
      }
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

         cmd_handler.add_command( c_cmd_exclusive, 1,
          "", "use ODS exclusive file access", new ods_fsed_startup_functor( cmd_handler ) );

         processor.process_commands( );

         cmd_handler.remove_command( c_cmd_exclusive );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.init( );

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
   catch( ods_error& err )
   {
      cerr << "error: " << err.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "unexpected exception occurred..." << endl;
      return 2;
   }
}


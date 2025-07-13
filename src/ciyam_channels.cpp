// Copyright (c) 2023-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <set>
#  include <vector>
#  include <fstream>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_channels.h"

#include "ods.h"
#include "config.h"
#include "threads.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "file_utils.h"
#include "fs_iterator.h"
#include "peer_session.h"
#include "ciyam_notifier.h"
#include "ciyam_variables.h"
#include "ods_file_system.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

trace_mutex g_mutex;

const char* const c_channel_files = ".files";
const char* const c_channel_fetch = ".fetch";

const char* const c_channel_fetched = "fetched";
const char* const c_channel_pending = "pending";
const char* const c_channel_updated = "updated";
const char* const c_channel_waiting = "waiting";
const char* const c_channel_blog_info = "blog_info";
const char* const c_channel_peer_info = "peer_info";
const char* const c_channel_user_info = "user_info";
const char* const c_channel_submitted = "submitted";
const char* const c_channel_reviewing = "reviewing";
const char* const c_channel_forum_info = "forum_info";
const char* const c_channel_submitting = "submitting";

const char* const c_submit_type_view = "*view*";
const char* const c_channel_folder_ciyam = ".ciyam";

const char* const c_channel_readme_file = "README.md";
const char* const c_channel_readme_source = "channel_readme.md";

const char* const c_datachain_type = "type";
const char* const c_datachain_height = "height";
const char* const c_datachain_channels = "channels";

void update_channel_info( ods_file_system& ofs,
 const string& identity, const string& blockchain_identity,
 const string& channel_info_name, bool& has_created_directory )
{
   string info_file_name( identity + '.' + channel_info_name + c_csv_file_ext );

   if( file_exists( info_file_name ) )
   {
      string new_info( buffer_file( info_file_name ) );

      file_remove( info_file_name );

      string info_changes;

      string all_channel_info;
      string all_column_names;

      bool append_all_as_changes = false;

      vector< string > lines;

      map< string, string > new_info_keyed;
      map< string, string > keyed_channel_info;

      split( new_info, lines, '\n' );

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         if( i == 0 )
            all_column_names = next_line;
         else
         {
            if( !next_line.empty( ) )
            {
               string::size_type pos = next_line.find( ',' );

               if( pos == string::npos )
                  throw runtime_error( "unexpected new channel_info line '" + next_line + "'" );

               string key( next_line.substr( 0, pos ) );
               string data( next_line.substr( pos + 1 ) );

               new_info_keyed.insert( make_pair( key, data ) );

               keyed_channel_info.insert( make_pair( key, data ) );
            }
         }
      }

      if( !ofs.has_file( channel_info_name ) )
      {
         append_all_as_changes = true;
         info_changes = all_channel_info = all_column_names;
      }
      else
      {
         lines.clear( );
         info_changes.erase( );
         all_channel_info.erase( );

         string old_channel_info;

         ofs.fetch_from_text_file( channel_info_name, old_channel_info );

         map< string, string > old_info_keyed;

         split( old_channel_info, lines, '\n' );

         for( size_t i = 0; i < lines.size( ); i++ )
         {
            string next_line( lines[ i ] );

            if( !next_line.empty( ) )
            {
               if( i == 0 )
               {
                  if( next_line != all_column_names )
                     throw runtime_error( "unexpected next_line != all_column_names (in update_channel_info)" );
               }
               else
               {
                  string::size_type pos = next_line.find( ',' );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected old channel info line '" + next_line + "'" );

                  string key( next_line.substr( 0, pos ) );
                  string data( next_line.substr( pos + 1 ) );

                  old_info_keyed.insert( make_pair( key, data ) );

                  if( !keyed_channel_info.count( key ) )
                     keyed_channel_info.insert( make_pair( key, data ) );
               }
            }
         }

         for( map< string, string >::const_iterator ci = new_info_keyed.begin( ); ci != new_info_keyed.end( ); ++ci )
         {
            bool append_change = false;

            if( old_info_keyed[ ci->first ] != ci->second )
               append_change = true;

            if( append_change )
            {
               if( info_changes.empty( ) )
                  info_changes = all_column_names;

               info_changes += '\n' + ci->first + ',' + ci->second;
            }
         }
      }

      for( map< string, string >::const_iterator ci = keyed_channel_info.begin( ); ci != keyed_channel_info.end( ); ++ci )
      {
         if( all_channel_info.empty( ) )
            all_channel_info = all_column_names;

         all_channel_info += '\n' + ci->first + ',' + ci->second;

         if( append_all_as_changes )
            info_changes += '\n' + ci->first + ',' + ci->second;
      }

      if( !info_changes.empty( ) )
      {
         ofs.store_as_text_file( channel_info_name, all_channel_info );

         if( !has_created_directory )
         {
            create_dir( blockchain_identity );
            has_created_directory = true;
         }

         write_file( blockchain_identity + "/." + channel_info_name + c_csv_file_ext, info_changes );
      }
   }
}

}

mutex& get_mutex_for_ciyam_channels( )
{
   return g_mutex;
}

channel_lock::channel_lock( const class_base& cb, const string& identity )
 :
 identity( identity )
{
   string class_display_name( cb.get_display_name( ) );

   if( set_system_variable( get_special_var_name(
    e_special_var_opening ) + '_' + identity, c_true_value, string( "" ) ) )
   {
      if( has_raw_system_variable(
       get_special_var_name( e_special_var_opened ) + '_' + identity ) )
      {
         set_system_variable(
          get_special_var_name( e_special_var_opening ) + '_' + identity, "" );

         // FUTURE: This message should be handled as a server string message.
         throw runtime_error( class_display_name + " '" + identity + "' is currently opened." );
      }
   }
   else
      // FUTURE: This message should be handled as a server string message.
      throw runtime_error( class_display_name + " '" + identity + "' is not currently available." );
}

channel_lock::~channel_lock( )
{
   set_system_variable( get_special_var_name(
    e_special_var_opening ) + '_' + identity, "" );
}

int64_t channel_height_fetched( const string& identity )
{
   return from_string< int64_t >( storage_channel_documents_fetched( identity ) );
}

void storage_channel_document_submit( const string& file_path )
{
   string notifier_value( get_raw_system_variable( file_path ) );

   if( !notifier_value.empty( ) )
   {
      string prefix;

      string::size_type pos = notifier_value.find( ']' );

      if( notifier_value[ 0 ] == '[' )
      {
         if( pos == string::npos )
            throw runtime_error( "unexpected notifier value '" + notifier_value + "' for '" + file_path + "'" );

         prefix = notifier_value.substr( 0, pos + 1 );
         notifier_value.erase( 0, pos + 1 );
      }

      if( !notifier_value.empty( ) && ( notifier_value[ 0 ] != c_notifier_select_char ) )
      {
         notifier_value = c_notifier_select_char + notifier_value;

         set_system_variable( file_path, prefix + notifier_value );
      }
   }
}

void storage_channel_document_unsubmit( const string& file_path )
{
   string notifier_value( get_raw_system_variable( file_path ) );

   if( !notifier_value.empty( ) )
   {
      string prefix;

      string::size_type pos = notifier_value.find( ']' );

      if( notifier_value[ 0 ] == '[' )
      {
         if( pos == string::npos )
            throw runtime_error( "unexpected notifier value '" + notifier_value + "' for '" + file_path + "'" );

         prefix = notifier_value.substr( 0, pos + 1 );
         notifier_value.erase( 0, pos + 1 );
      }

      if( !notifier_value.empty( ) && ( notifier_value[ 0 ] == c_notifier_select_char ) )
      {
         notifier_value.erase( 0, 1 );

         notifier_value = c_notifier_unselect_char + notifier_value;

         set_system_variable( file_path, prefix + notifier_value );
      }
   }
}

void storage_channel_document_restore( const string& identity_path )
{
   if( !identity_path.empty( ) )
   {
      guard g( g_mutex );

      bool is_standard = false;

      string name( storage_name( &is_standard ) );

      if( is_standard )
         throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_document_restore" );

      string opened_files_path( get_system_variable( get_special_var_name( e_special_var_opened_files ) ) );

      if( identity_path.find( opened_files_path ) != 0 )
         throw runtime_error( "unexpected identity_path '" + identity_path + "' in storage_channel_document_restore" );

      string::size_type pos = identity_path.find( '/', opened_files_path.length( ) + 1 );

      if( pos == string::npos )
         throw runtime_error( "unexpected identity_path '" + identity_path + "' in storage_channel_document_restore" );

      string identity( identity_path.substr( opened_files_path.length( ) + 1, pos - opened_files_path.length( ) - 1 ) );
      string file_path( identity_path.substr( pos + 1 ) );

      ods& ods_db( storage_ods_instance( ) );

      auto_ptr< ods::bulk_read > ap_bulk_read;

      if( !ods_db.is_thread_bulk_locked( ) )
         ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

      ods_file_system ofs( ods_db );

      ofs.set_root_folder( c_storage_folder_name_channels );

      string destination( opened_files_path + '/' + identity + '/' + file_path );

      ofs.set_folder( identity );

      set< string > selected;

      ofs.set_folder( c_channel_folder_ciyam );

      if( ofs.has_file( c_channel_submitting ) )
      {
         string all_selected;

         ofs.fetch_from_text_file( c_channel_submitting, all_selected );

         if( !all_selected.empty( ) )
            split( all_selected, selected, '\n' );
      }

      ofs.set_folder( ".." );

      string notifier_value( get_raw_system_variable( destination ) );

      int64_t num_bytes = 0;

      ofs.has_file( file_path, false, 0, 0, 0, &num_bytes );

      if( !notifier_value.empty( ) )
      {
         string prefix;

         string::size_type pos = notifier_value.find( ']' );

         if( notifier_value[ 0 ] == '[' )
         {
            if( pos == string::npos )
               throw runtime_error( "unexpected notifier value '" + notifier_value + "' for '" + file_path + "'" );

            prefix = notifier_value.substr( 0, pos + 1 );
            notifier_value.erase( 0, pos + 1 );
         }

         if( !notifier_value.empty( ) )
         {
            if( !( ( notifier_value[ 0 ] >= 'a' ) && ( notifier_value[ 0 ] <= 'z' ) ) )
               notifier_value.erase( 0, 1 );

            // NOTE: Set a special tag for the notifier to ignore either a create
            // or modify event (depending whether or not the file is zero length)
            // and also to indicate whether or not the file should be selected.
            if( num_bytes )
            {
               if( !selected.count( file_path ) )
                  notifier_value = c_notifier_mod_ignore_char + notifier_value;
               else
                  notifier_value = c_notifier_mod_sel_ignore_char + notifier_value;
            }
            else
            {
               if( !selected.count( file_path ) )
                  notifier_value = c_notifier_new_ignore_char + notifier_value;
               else
                  notifier_value = c_notifier_new_sel_ignore_char + notifier_value;
            }

            set_system_variable( destination, prefix + notifier_value );
         }
      }

      string path_to_file( destination );

      pos = path_to_file.rfind( '/' );

      if( pos != string::npos )
      {
         path_to_file.erase( pos );
         vector< string > directories;

         if( !path_to_file.empty( ) )
         {
            split( path_to_file, directories, '/' );

            string directory_path;

            for( size_t i = 0; i < directories.size( ); i++ )
            {
               string next_directory( directories[ i ] );

               if( !directory_path.empty( ) )
                  directory_path += '/';

               directory_path += next_directory;

               if( !dir_exists( directory_path ) )
               {
                  create_dir( directory_path );

                  // NOTE: Wait for the notifier to add its watch for each directory.
                  for( int i = 0; i < c_max_notifer_checks; i++ )
                  {
                     string notifier_value( get_raw_system_variable( directory_path + '/' ) );

                     if( notifier_value.find( c_notifier_deleted ) == string::npos )
                        break;

                     msleep( c_notifer_check_wait );
                  }
               }
            }
         }
      }

      ofs.get_file( file_path, destination );

      msleep( c_notifer_check_wait );
   }
}

bool storage_channel_document_ignoring( const string& file_path )
{
   bool retval = false;

   string notifier_value( get_raw_system_variable( file_path ) );

   if( !notifier_value.empty( ) )
   {
      string::size_type pos = notifier_value.find( ']' );

      if( notifier_value[ 0 ] == '[' )
      {
         if( pos == string::npos )
            throw runtime_error( "unexpected notifier value '" + notifier_value + "' for '" + file_path + "'" );

         notifier_value.erase( 0, pos + 1 );
      }

      if( !notifier_value.empty( ) && ( notifier_value[ 0 ] == c_notifier_ignore_char ) )
         retval = true;
   }

   return retval;
}

bool storage_channel_document_submitting( const string& file_path )
{
   bool retval = false;

   string notifier_value( get_raw_system_variable( file_path ) );

   if( !notifier_value.empty( ) )
   {
      string::size_type pos = notifier_value.find( ']' );

      if( notifier_value[ 0 ] == '[' )
      {
         if( pos == string::npos )
            throw runtime_error( "unexpected notifier value '" + notifier_value + "' for '" + file_path + "'" );

         notifier_value.erase( 0, pos + 1 );
      }

      if( !notifier_value.empty( ) && ( notifier_value[ 0 ] == c_notifier_select_char ) )
         retval = true;
   }

   return retval;
}

void storage_channel_list( ostream& os )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   ofs.list_folders( os );
}

void storage_channel_create( const char* p_identity, const char* p_channel_information )
{
   guard g( g_mutex );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_create" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "identity not found for 'storage_channel_create'" );

   // NOTE: If an alias prefix has been included then need to remove it.
   string::size_type pos = identity.find( ':' );

   if( pos != string::npos )
      identity.erase( 0, pos + 1 );

   check_with_regex( c_special_regex_for_peerchain_identity, identity );

   ofs.add_folder( identity );

   ofs.set_folder( identity );

   string perms( c_perms_r_r );
   ofs.add_file( c_channel_readme_file, c_channel_readme_source, 0, 0, 0, &perms );

   ofs.add_folder( c_channel_folder_ciyam );
   ofs.set_folder( c_channel_folder_ciyam );

   string channel_information;

   if( p_channel_information && ( *p_channel_information != 0 ) )
      channel_information = *p_channel_information;
   else
      channel_information = get_session_variable( get_special_var_name( e_special_var_arg2 ) );

   if( channel_information.empty( ) )
      throw runtime_error( "channel information not found for 'storage_channel_create'" );

   ofs.store_as_text_file( c_channel_peer_info, channel_information );
}

void storage_channel_destroy( const char* p_identity )
{
   guard g( g_mutex );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_destroy" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "identity not found for 'storage_channel_destroy'" );

   check_with_regex( c_special_regex_for_peerchain_identity, identity );

   ofs.remove_folder( identity, 0, true );
}

void storage_channel_update( const char* p_identity, const char* p_channel_information )
{
   guard g( g_mutex );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_update" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "identity not found for 'storage_channel_update'" );

   string channel_information;

   if( p_channel_information && ( *p_channel_information != 0 ) )
      channel_information = *p_channel_information;
   else
      channel_information = get_session_variable( get_special_var_name( e_special_var_arg2 ) );

   if( channel_information.empty( ) )
      throw runtime_error( "channel information not found for 'storage_channel_update'" );

   ofs.set_folder( identity );
   ofs.set_folder( c_channel_folder_ciyam );

   ofs.store_as_text_file( c_channel_peer_info, channel_information );
}

int64_t storage_channel_received_height( const string& identity )
{
   return from_string< int64_t >( peer_channel_height( identity, true, true ) );
}

int64_t storage_channel_submitted_height( const string& identity )
{
   return from_string< int64_t >( peer_channel_height( identity, true, false ) );
}

string storage_channel_documents( const string& identity, bool get_height, bool for_fetched )
{
   guard g( g_mutex );

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '"
       + name + "' for storage_channel_documents" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   string retval;

   if( ofs.has_folder( identity ) )
   {
      ofs.set_folder( identity );

      if( !get_height )
      {
         stringstream ss;

         ods_file_system::branch_style style = ods_file_system::e_branch_style_default;

         temporary_include_hidden include_hidden( ofs, false );

         if( !get_raw_session_variable(
          get_special_var_name( e_special_var_style_brief ) ).empty( ) )
            style = ods_file_system::e_branch_style_brief;

         if( !get_raw_session_variable(
          get_special_var_name( e_special_var_style_extended ) ).empty( ) )
            style = ods_file_system::e_branch_style_extended;

         ofs.branch_objects( "*", ss, style );

         retval = ss.str( );
      }
      else
      {
         int64_t height = 0;

         string height_file_name( for_fetched ? c_channel_fetched : c_channel_submitted );

         string height_file_path( c_channel_folder_ciyam );
         height_file_path += '/' + height_file_name;

         if( ofs.has_file( height_file_path ) )
            ofs.fetch_from_text_file( height_file_path, height );

         retval = to_string( height );
      }
   }

   return retval;
}

void storage_channel_documents_open( const char* p_identity )
{
   guard g( g_mutex );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '"
       + name + "' for storage_channel_documents_open" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "identity not found for 'storage_channel_documents_open'" );

   if( !ofs.has_folder( identity ) )
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   ofs.set_folder( identity );

   string path( get_system_variable(
    get_special_var_name( e_special_var_opened_files ) ) + '/' + identity );

   string prefix( path + '/' );

   create_dir( path );

   export_objects( ofs, path );

   ofs.set_folder( c_channel_folder_ciyam );

   // NOTE: Files "pending approval" are set to read-only.
   if( ofs.has_file( c_channel_pending ) )
   {
      string pending;

      ofs.fetch_from_text_file( c_channel_pending, pending );

      if( !pending.empty( ) )
      {
         vector< string > all_pending;

         split( pending, all_pending, '\n' );

         for( size_t i = 0; i < all_pending.size( ); i++ )
         {
            string next_pending( all_pending[ i ] );
            file_perms( prefix + next_pending, c_perms_r_r );

            set_system_variable( prefix + next_pending, string( 1, c_notifier_ignore_char ) );
         }
      }
   }

   // NOTE: Files "waiting approval" are set to read-only.
   if( ofs.has_file( c_channel_waiting ) )
   {
      string waiting;

      ofs.fetch_from_text_file( c_channel_waiting, waiting );

      if( !waiting.empty( ) )
      {
         vector< string > all_waiting;

         split( waiting, all_waiting, '\n' );

         for( size_t i = 0; i < all_waiting.size( ); i++ )
         {
            string next_waiting( all_waiting[ i ] );
            file_perms( prefix + next_waiting, c_perms_r_r );

            set_system_variable( prefix + next_waiting, string( 1, c_notifier_ignore_char ) );
         }
      }
   }

   // NOTE: Files "being reviewed" are set to read-only.
   if( ofs.has_file( c_channel_reviewing ) )
   {
      string reviewing;

      ofs.fetch_from_text_file( c_channel_reviewing, reviewing );

      if( !reviewing.empty( ) )
      {
         vector< string > all_reviewing;

         split( reviewing, all_reviewing, '\n' );

         for( size_t i = 0; i < all_reviewing.size( ); i++ )
         {
            string next_reviewing( all_reviewing[ i ] );
            file_perms( prefix + next_reviewing, c_perms_r_r );

            set_system_variable( prefix + next_reviewing, string( 1, c_notifier_ignore_char ) );
         }
      }
   }

   if( ofs.has_file( c_channel_submitting ) )
   {
      string submitting;

      ofs.fetch_from_text_file( c_channel_submitting, submitting );

      if( !submitting.empty( ) )
      {
         vector< string > all_submitting;

         split( submitting, all_submitting, '\n' );

         for( size_t i = 0; i < all_submitting.size( ); i++ )
            set_system_variable( prefix + all_submitting[ i ], string( 1, c_notifier_select_char ) );
      }
   }
}

void storage_channel_documents_close( const char* p_identity )
{
   guard g( g_mutex );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '"
       + name + "' for storage_channel_documents_close" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "identity not found for 'storage_channel_documents_close'" );

   if( !ofs.has_folder( identity ) )
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   ofs.set_folder( identity );

   string path( get_system_variable(
    get_special_var_name( e_special_var_opened_files ) ) + '/' + identity );

   ods::transaction ods_tx( ods_db );

   string prefix( get_raw_system_variable(
    get_special_var_name( e_special_var_opened_files ) ) + '/' + identity + '/' );

   ofs.set_folder( c_channel_folder_ciyam );

   if( file_exists( prefix + c_channel_readme_file ) )
      file_remove( prefix + c_channel_readme_file );

   // NOTE: Files "pending approval" are removed prior to import.
   if( ofs.has_file( c_channel_pending ) )
   {
      string pending;

      ofs.fetch_from_text_file( c_channel_pending, pending );

      if( !pending.empty( ) )
      {
         vector< string > all_pending;

         split( pending, all_pending, '\n' );

         for( size_t i = 0; i < all_pending.size( ); i++ )
         {
            string next_pending( all_pending[ i ] );
            file_remove( prefix + next_pending );
         }
      }
   }

   ofs.set_folder( ".." );

   import_objects( ofs, path );

   string all_selected;

   string all_file_lines( get_raw_system_variable( prefix + "?*" ) );

   vector< string > file_lines;

   split( all_file_lines, file_lines, '\n' );

   for( size_t i = 0; i < file_lines.size( ); i++ )
   {
      string next_line( file_lines[ i ] );

      if( !next_line.empty( ) )
      {
         string next_name, next_value;

         next_name = variable_name_from_name_and_value( next_line, &next_value );

         if( next_name.find( prefix ) == 0 )
            next_name.erase( 0, prefix.length( ) );

         if( !next_value.empty( ) && next_value[ 0 ] == '[' )
         {
            string::size_type pos = next_value.find( ']' );

            if( pos == string::npos )
               throw runtime_error( "unexpected next_value '" + next_value + "' in storage_channel_documents_close" );

            next_value.erase( 0, pos + 1 );
         }

         string next_selected;

         if( !next_value.empty( ) && ( next_value[ 0 ] == c_notifier_select_char ) )
         {
            next_value.erase( 0, 1 );

            next_selected = next_name;
         }

         if( next_name.find( prefix ) == 0 )
            next_name.erase( 0, prefix.length( ) );

         if( !next_value.empty( ) )
         {
            string::size_type pos = next_value.find( '|' );
            string notifier_op( next_value.substr( 0, pos ) );

            if( notifier_op == c_notifier_deleted )
            {
               next_selected.erase( );

               if( pos != string::npos )
               {
                  next_name = next_value.substr( pos + 1 );

                  if( next_name.find( prefix ) == 0 )
                     next_name.erase( 0, prefix.length( ) );
               }

               if( next_name[ next_name.length( ) - 1 ] != '/' )
                  ofs.remove_file( next_name );
               else
                  ofs.remove_folder( next_name, 0, true );
            }
            else if( ( notifier_op == c_notifier_moved_from )
             || ( notifier_op == c_notifier_modified_from ) )
            {
               string old_name;

               if( pos != string::npos )
                  old_name = next_value.substr( pos + 1 );

               if( !old_name.empty( ) )
               {
                  if( old_name.find( prefix ) == 0 )
                     old_name.erase( 0, prefix.length( ) );

                  if( old_name[ old_name.length( ) - 1 ] != '/' )
                     ofs.remove_file( old_name );
                  else
                     ofs.remove_folder( old_name, 0, true );
               }
            }
         }

         if( !next_selected.empty( ) )
         {
            if( !all_selected.empty( ) )
               all_selected += '\n';

            all_selected += next_selected;
         }
      }
   }

   ofs.set_folder( c_channel_folder_ciyam );

   if( !all_selected.empty( ) )
   {
      string pending( get_system_variable(
       get_special_var_name( e_special_var_pending ) + '_' + identity ) );

      if( !pending.empty( ) )
      {
         ofs.store_as_text_file( c_channel_pending, all_selected );

         if( ofs.has_file( c_channel_submitting ) )
            ofs.remove_file( c_channel_submitting );
      }
      else
         ofs.store_as_text_file( c_channel_submitting, all_selected );
   }
   else if( ofs.has_file( c_channel_submitting ) )
      ofs.remove_file( c_channel_submitting );

   if( ofs.has_file( c_channel_updated ) )
      ofs.remove_file( c_channel_updated );

   ods_tx.commit( );

   delete_directory_files( path, true );

   set_system_variable( get_special_var_name( e_special_var_pending ) + '_' + identity, "" );
}

bool storage_channel_documents_marked( const string& identity )
{
   bool retval = false;

   if( !identity.empty( ) )
   {
      guard g( g_mutex );

      bool is_standard = false;

      string name( storage_name( &is_standard ) );

      if( is_standard )
         throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_documents_marked" );

      string opened_variable_name( get_special_var_name( e_special_var_opened ) + '_' + identity );

      if( has_raw_system_variable( opened_variable_name ) )
      {
         string prefix( get_raw_system_variable(
          get_special_var_name( e_special_var_opened_files ) ) + '/' + identity + '/' );

         string all_selected;

         string all_file_lines( get_raw_system_variable( prefix + "?*" ) );

         vector< string > file_lines;

         split( all_file_lines, file_lines, '\n' );

         for( size_t i = 0; i < file_lines.size( ); i++ )
         {
            string next_line( file_lines[ i ] );

            if( !next_line.empty( ) )
            {
               string next_name, next_value;

               next_name = variable_name_from_name_and_value( next_line, &next_value );

               if( next_name.find( prefix ) == 0 )
                  next_name.erase( 0, prefix.length( ) );

               if( !next_value.empty( ) && next_value[ 0 ] == '[' )
               {
                  string::size_type pos = next_value.find( ']' );

                  if( pos == string::npos )
                     throw runtime_error( "unexpected next_value '" + next_value + "' in storage_channel_documents_close" );

                  next_value.erase( 0, pos + 1 );
               }

               string next_selected;

               if( !next_value.empty( ) && ( next_value[ 0 ] == c_notifier_select_char ) )
               {
                  retval = true;
                  break;
               }
            }
         }
      }
   }

   return retval;
}

bool storage_channel_documents_opened( const string& identity )
{
   bool retval = false;

   if( !identity.empty( ) )
   {
      guard g( g_mutex );

      bool is_standard = false;

      string name( storage_name( &is_standard ) );

      if( is_standard )
         throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_documents_opened" );

      string opened_variable_name( get_special_var_name( e_special_var_opened ) + '_' + identity );
      string opening_variable_name( get_special_var_name( e_special_var_opening ) + '_' + identity );

      if( has_raw_system_variable( opened_variable_name ) )
         retval = true;
      else if( !has_raw_system_variable( opening_variable_name ) )
      {
         string opened_files_directory( get_raw_system_variable(
          get_special_var_name( e_special_var_opened_files ) ) );

         if( !opened_files_directory.empty( ) )
         {
            string identity_directory( opened_files_directory + '/' + identity );

            if( dir_exists( identity_directory ) )
            {
               string prefix( identity_directory + '/' );
               string user( file_user( identity_directory ) );

               // NOTE: Restores the "@opened_user_<identity>" system
               // variable (that was set in the "open_channel" script).
               set_system_variable( get_special_var_name(
                e_special_var_opened_user ) + '_' + identity, user );

               temporary_session_variable tmp_style_extended(
                get_special_var_name( e_special_var_style_extended ), c_true_value );

               set_system_variable( prefix + c_channel_readme_file, string( 1, c_notifier_ignore_char ) );

               string all_pending( storage_channel_documents_specific( identity, e_channel_documents_type_pending, false ) );

               if( !all_pending.empty( ) )
               {
                  vector< string > pending;
                  split( all_pending, pending, '\n' );

                  for( size_t i = 0; i < pending.size( ); i++ )
                     set_system_variable( prefix + pending[ i ], string( 1, c_notifier_ignore_char ) );
               }

               string all_submitting( storage_channel_documents_specific( identity, e_channel_documents_type_submitting, false ) );

               string all_documents( storage_channel_documents( identity ) );

               vector< string > documents;

               split( all_documents, documents, '\n' );

               string paths_and_time_stamps;

               for( size_t i = 0; i < documents.size( ); i++ )
               {
                  string next_document( documents[ i ] );

                  if( !next_document.empty( ) )
                  {
                     string::size_type pos = next_document.rfind( " (" );

                     if( pos != string::npos )
                     {
                        string extended_information( next_document.substr( pos + 1 ) );
                        next_document.erase( pos );

                        pos = extended_information.find( ") " );

                        if( pos == string::npos )
                           throw runtime_error( "unexpected extended_information '" + extended_information + "'" );

                        string size( extended_information.substr( 1, pos - 1 ) );

                        extended_information.erase( 0, pos + 2 );

                        pos = extended_information.find( ' ' );

                        if( pos == string::npos )
                           throw runtime_error( "unexpected extended_information '" + extended_information + "'" );

                        string perms( extended_information.substr( 0, pos ) );

                        string date_and_time( extended_information.substr( pos + 1 ) );

                        replace( next_document, '/' + string( c_storage_folder_name_channels ), opened_files_directory );

                        if( !paths_and_time_stamps.empty( ) )
                           paths_and_time_stamps += '\n';

                        paths_and_time_stamps += ( next_document + '@' + date_and_time );
                     }
                  }
               }

               ciyam_notifier* p_notifier = new ciyam_notifier( identity_directory, &all_submitting, &paths_and_time_stamps );

               p_notifier->start( );

               bool has_notifier_variables = false;

               for( size_t i = 0; i < c_max_notifer_checks; i++ )
               {
                  msleep( c_notifer_check_wait );

                  if( has_raw_system_variable( c_notifier_prefix + identity_directory + '/' ) )
                  {
                     has_notifier_variables = true;
                     break;
                  }
               }

               if( has_notifier_variables )
               {
                  retval = true;

                  set_system_variable( opened_variable_name, c_true_value );
               }
            }
         }
      }
   }

   return retval;
}

string storage_channel_documents_update( const string& identity, bool submitted )
{
   guard g( g_mutex );

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_update" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '"
       + name + "' for storage_channel_documents_update" );

   string retval;

   string blockchain_identity( get_raw_system_variable( '$' + identity + "_identity" ) );

   if( blockchain_identity.empty( ) )
      throw runtime_error( "blockchain identity for '"
       + identity + "' not found in storage_channel_documents_update" );

   if( !submitted )
      reverse( blockchain_identity.begin( ), blockchain_identity.end( ) );

   if( dir_exists( blockchain_identity ) )
      throw runtime_error( "unexpected blockchain identity directory '"
       + blockchain_identity + "' for channel '" + identity + "' already exists" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   if( !ofs.has_folder( identity ) )
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   ofs.set_folder( identity );

   int64_t height = 0;

   string height_file_path( c_channel_folder_ciyam );
   height_file_path += '/' + string( !submitted ? c_channel_fetched : c_channel_submitted );

   if( ofs.has_file( height_file_path ) )
      ofs.fetch_from_text_file( height_file_path, height );

   string height_updating( get_session_variable(
    get_special_var_name( e_special_var_updating ) + '_' + identity ) );

   if( height_updating.empty( ) )
      ++height;
   else
      height = from_string< int64_t >( height_updating );

   string bundle_file_name( blockchain_identity + ".bun.gz" );

   if( file_exists( bundle_file_name ) )
   {
      create_dir( blockchain_identity );

#ifdef _WIN32
      string cmd( "unbundle" );
#else
      string cmd( "./unbundle" );
#endif

      string temp_file( "~" + uuid( ).as_string( ) );

      cmd += " -qq " + bundle_file_name + " -d " + blockchain_identity + " 2>" + temp_file;

      int rc = system( cmd.c_str( ) );
      ( void )rc;

      if( exists_file( temp_file ) )
      {
         string error_output( load_file( temp_file ) );

         file_remove( temp_file );

         // NOTE: If errors then throw an exception with the first error line.
         if( !error_output.empty( ) )
         {
            remove_dir( blockchain_identity );

            error_output = error_output.substr( 0, error_output.find( '\n' ) );

            string error_prefix( c_error );
            error_prefix += string( ": " );

            if( lower( error_output ).find( error_prefix ) == 0 )
               error_output.erase( 0, error_prefix.length( ) );

            // NOTE: If the error message includes "bundle header" then most likely an
            // incorrect shared secret is the issue so issue a more appropriate error.
            if( error_output.find( " bundle header " ) != string::npos )
               // FUTURE: This message should be handled as a server string message.
               throw runtime_error( "Found invalid/corrupt peer data (incorrect shared secret?)." );
            else
               throw runtime_error( !error_output.empty( ) ? error_output.c_str( ) : "unexpected unknonwn error" );
         }
      }

      string updated;
      string files_name( blockchain_identity + '/' + c_channel_files );

      string blog_info_name( blockchain_identity + "/." + string( c_channel_blog_info ) + c_csv_file_ext );

      if( !submitted && file_exists( blog_info_name ) )
         file_rename( blog_info_name, blockchain_identity + '.' + string( c_channel_blog_info ) + c_csv_file_ext );

      string forum_info_name( blockchain_identity + "/." + string( c_channel_forum_info ) + c_csv_file_ext );

      if( !submitted && file_exists( forum_info_name ) )
         file_rename( forum_info_name, blockchain_identity + '.' + string( c_channel_forum_info ) + c_csv_file_ext );

      string user_info_name( blockchain_identity + "/." + string( c_channel_user_info ) + c_csv_file_ext );

      if( !submitted && file_exists( user_info_name ) )
         file_rename( user_info_name, blockchain_identity + '.' + string( c_channel_user_info ) + c_csv_file_ext );

      string app_log_file_name( blockchain_identity + '/' + blockchain_identity + c_log_file_ext );

      if( !submitted && file_exists( app_log_file_name ) )
      {
         file_rename( app_log_file_name, blockchain_identity + c_log_file_ext );

         file_filter ff;
         fs_iterator fs( blockchain_identity, &ff );

         while( fs.has_next( ) )
         {
            string dest_file( fs.get_name( ) );

            string::size_type pos = dest_file.find( '.' );

            if( pos != string::npos && dest_file[ 0 ] == 'M' )
            {
               string class_id( dest_file.substr( 0, pos ) );

               dest_file.erase( 0, pos + 1 );

               pos = class_id.find( 'C' );

               if( pos == string::npos )
                  throw runtime_error( "unexpected file name '" + fs.get_name( ) + "' in storage_channel_documents_update" );

               string module_id( class_id.substr( 0, pos ) );

               dest_file = get_web_root( ) + '/' + lower( name )
                + '/' + string( c_files_directory ) + '/' + module_id + '/' + class_id + '/' + dest_file;

               pos = dest_file.find( identity );

               // NOTE: Attached files must contain the peer's identity (otherwise
               // could replace files created for own identity or for other peers).
               if( pos == string::npos )
                  continue;

               create_directories( dest_file, c_web_files_dir_perm_val, WEB_FILES_UMASK );

               // NOTE: If renaming fails (perhaps due to perms) then try to copy the file instead.
               if( !file_rename( blockchain_identity + '/' + fs.get_name( ), dest_file ) )
                  file_copy( blockchain_identity + '/' + fs.get_name( ), dest_file );

               file_perms( dest_file, c_perms_rw_rw_r );
            }
         }
      }

      ods::transaction ods_tx( ods_db );

      if( file_exists( files_name ) )
      {
         string all_files( buffer_file( files_name ) );

         if( !all_files.empty( ) )
         {
            temporary_force_write ofs_force_write( ofs );

            vector< string > files;

            set< string > paths;
            split( all_files, files, '\n' );

            bool pending_approval = false;

            for( size_t i = 0; i < files.size( ); i++ )
            {
               string next_file_info( files[ i ] );

               if( next_file_info == c_submit_type_view )
               {
                  pending_approval = true;
                  continue;
               }

               string::size_type pos = next_file_info.find( ' ' );

               if( pos == string::npos )
                  throw runtime_error( "unexpected next_file_info '" + next_file_info + "' in storage_channel_documents_update" );

               string local_file( blockchain_identity + '/' + next_file_info.substr( 0, pos ) );

               string next_file_path( next_file_info.substr( pos + 1 ) );

               if( !updated.empty( ) )
                  updated += '\n';

               updated += next_file_path;

               paths.insert( next_file_path );

               pos = next_file_path.rfind( '/' );

               bool has_stored = false;

               if( ( pos != 0 ) && ( pos != string::npos ) )
               {
                  string file_name( next_file_path.substr( pos + 1 ) );
                  string all_folders( next_file_path.substr( 0, pos ) );

                  vector< string > folders;
                  split( all_folders, folders, '/' );

                  string folder( ofs.get_folder( ) );

                  for( size_t i = 0; i < folders.size( ); i++ )
                  {
                     string next_folder( folders[ i ] );

                     if( ofs.has_folder( next_folder ) )
                        ofs.set_folder( next_folder );
                     else
                     {
                        ofs.add_folder( next_folder );
                        ofs.set_folder( next_folder );
                     }
                  }

                  ofs.store_file( file_name, local_file );

                  has_stored = true;
                  ofs.set_folder( folder );
               }

               if( !has_stored )
                  ofs.store_file( next_file_path, local_file );
            }

            ofs.set_folder( c_channel_folder_ciyam );

            string file_paths_name( c_channel_updated );

            if( pending_approval )
               file_paths_name = string( c_channel_reviewing );

            if( ofs.has_file( file_paths_name ) )
            {
               string all_existing;
               ofs.fetch_from_text_file( file_paths_name, all_existing );

               if( !all_existing.empty( ) )
               {
                  vector< string > existing;

                  split( all_existing, existing, '\n' );

                  for( size_t i = 0; i < existing.size( ); i++ )
                     paths.insert( existing[ i ] );
               }
            }

            string all_file_paths;
            for( set< string >::iterator i = paths.begin( ); i != paths.end( ); ++i )
            {
               if( !all_file_paths.empty( ) )
                  all_file_paths += '\n';

               all_file_paths += *i;
            }

            ofs.store_as_text_file( file_paths_name, all_file_paths );

            retval = updated;
         }
         else
            ofs.set_folder( c_channel_folder_ciyam );

         ofs.store_as_text_file( ( !submitted ? c_channel_fetched : c_channel_submitted ), height );
      }
      else
      {
         ofs.set_folder( c_channel_folder_ciyam );

         ofs.store_as_text_file( ( !submitted ? c_channel_fetched : c_channel_submitted ), height );
      }

      ods_tx.commit( );

      file_remove( bundle_file_name );

      delete_directory_files( blockchain_identity, true );
   }

   return retval;
}

string storage_channel_documents_prepare( const string& identity )
{
   guard g( g_mutex );

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_prepare" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_documents_prepare" );

   string retval;

   string blockchain_identity( get_raw_system_variable( '$' + identity + "_identity" ) );

   if( blockchain_identity.empty( ) )
      throw runtime_error( "blockchain identity for '"
       + identity + "' not found in 'storage_channel_documents_prepare'" );

   if( dir_exists( blockchain_identity ) )
      throw runtime_error( "unexpected blockchain identity directory '"
       + blockchain_identity + "' for channel '" + identity + "' already exists" );

   string bundle_file_name( blockchain_identity + ".bun.gz" );

   if( file_exists( bundle_file_name ) )
      file_remove( bundle_file_name );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   if( !ofs.has_folder( identity ) )
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   ofs.set_folder( identity );

   int64_t height_submitted = 0;
   bool has_created_directory = false;

   string submitted_file_path( c_channel_folder_ciyam );
   submitted_file_path += '/' + string( c_channel_submitted );

   if( ofs.has_file( submitted_file_path ) )
      ofs.fetch_from_text_file( submitted_file_path, height_submitted );

   string height_preparing( get_system_variable(
    get_special_var_name( e_special_var_preparing ) + '_' + identity ) );

   if( height_preparing.empty( ) )
      ++height_submitted;
   else
      height_submitted = from_string< int64_t >( height_preparing );

   ofs.set_folder( c_channel_folder_ciyam );

   update_channel_info( ofs, identity, blockchain_identity, c_channel_blog_info, has_created_directory );
   update_channel_info( ofs, identity, blockchain_identity, c_channel_forum_info, has_created_directory );
   update_channel_info( ofs, identity, blockchain_identity, c_channel_user_info, has_created_directory );

   string submitting;

   string identity_log_file_name( identity + c_log_file_ext );
   string blockchain_log_file_name( blockchain_identity + c_log_file_ext );

   if( has_created_directory || ofs.has_file( c_channel_pending )
    || ofs.has_file( c_channel_submitting ) || file_exists( identity_log_file_name ) )
   {
      if( !has_created_directory )
         create_dir( blockchain_identity );

      if( file_exists( identity_log_file_name ) )
         file_rename( identity_log_file_name, blockchain_identity + '/' + blockchain_log_file_name );

      string identity_files_dir( identity + c_files_ext );

      if( dir_exists( identity_files_dir ) )
      {
         file_filter ff;
         fs_iterator fs( identity_files_dir, &ff );

         while( fs.has_next( ) )
            file_rename( identity_files_dir + '/' + fs.get_name( ), blockchain_identity + '/' + fs.get_name( ) );
      }

      int64_t height_fetched = 0;

      string fetched_file( c_channel_fetched );

      if( ofs.has_file( fetched_file ) )
         ofs.fetch_from_text_file( fetched_file, height_fetched );

      write_file( blockchain_identity + '/'
       + string( c_channel_fetch ), to_string( height_fetched ) );

      string files;

      string waiting( get_system_variable(
       get_special_var_name( e_special_var_waiting ) + '_' + identity ) );

      if( !waiting.empty( ) && ofs.has_file( c_channel_pending ) )
      {
         files = c_submit_type_view;
         ofs.fetch_from_text_file( c_channel_pending, submitting );
      }
      else if( ofs.has_file( c_channel_submitting ) )
         ofs.fetch_from_text_file( c_channel_submitting, submitting );

      set_system_variable( get_special_var_name( e_special_var_waiting ) + '_' + identity, "" );

      ofs.set_folder( ".." );

      if( !submitting.empty( ) )
      {
         vector< string > all_submitting;

         split( submitting, all_submitting, '\n' );

         for( size_t i = 0; i < all_submitting.size( ); i++ )
         {
            string next_submitting( all_submitting[ i ] );

            string next_item_num( to_comparable_string( i, false, 6 ) );

            if( !files.empty( ) )
               files += '\n';
            files += ( next_item_num + ' ' + next_submitting );

            ofs.get_file( next_submitting, blockchain_identity + '/' + next_item_num );
         }
      }

      write_file( blockchain_identity + '/' + string( c_channel_files ), files );

#ifdef _WIN32
      string cmd( "bundle" );
#else
      string cmd( "./bundle" );
#endif

      cmd += " -qq " + blockchain_identity + " \"" + blockchain_identity + "/*\"";

      int rc = system( cmd.c_str( ) );
      ( void )rc;

      if( file_exists( blockchain_identity + ".bun.gz" ) )
         retval = blockchain_identity;

      ods::transaction ods_tx( ods_db );

      ofs.set_folder( c_channel_folder_ciyam );

      if( ofs.has_file( c_channel_submitting ) )
         ofs.remove_file( c_channel_submitting );

      if( !waiting.empty( ) && !submitting.empty( ) )
      {
         if( !ofs.has_file( c_channel_waiting ) )
            ofs.move_file( c_channel_pending, c_channel_waiting );
         else
         {
            set< string > paths;
            string existing_paths;

            ofs.fetch_from_text_file( c_channel_waiting, existing_paths );

            if( !existing_paths.empty( ) )
               split( existing_paths, paths, '\n' );

            split( submitting, paths, '\n' );

            string all_file_paths;
            for( set< string >::iterator i = paths.begin( ); i != paths.end( ); ++i )
            {
               if( !all_file_paths.empty( ) )
                  all_file_paths += '\n';

               all_file_paths += *i;
            }

            ofs.remove_file( c_channel_pending );
            ofs.store_as_text_file( c_channel_waiting, all_file_paths );
         }
      }

      ofs.store_as_text_file( c_channel_submitted, height_submitted );

      ods_tx.commit( );

      delete_directory_files( blockchain_identity, true );
   }

   return retval;
}

void storage_channel_documents_cancel_pending( const char* p_identity )
{
   guard g( g_mutex );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_cancel_pending" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '"
       + name + "' for storage_channel_documents_cancel_pending" );

   string blockchain_identity( get_raw_system_variable( '$' + identity + "_identity" ) );

   if( blockchain_identity.empty( ) )
      throw runtime_error( "blockchain identity for '"
       + identity + "' not found in storage_channel_documents_cancel_pending" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   if( ofs.has_folder( identity ) )
      ofs.set_folder( identity );
   else
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   ofs.set_folder( c_channel_folder_ciyam );

   if( ofs.has_file( c_channel_pending ) )
      ofs.move_file( c_channel_pending, c_channel_submitting );
}

void storage_channel_documents_reject_waiting( const char* p_identity, const char* p_file_list_for_rejection )
{
   guard g( g_mutex );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_reject_waiting" );

   string file_list_for_rejection;

   if( p_file_list_for_rejection && ( *p_file_list_for_rejection != 0 ) )
      file_list_for_rejection = string( p_file_list_for_rejection );
   else
   {
      file_list_for_rejection = get_session_variable( get_special_var_name( e_special_var_arg2 ) );

      if( file_list_for_rejection.empty( ) )
         file_list_for_rejection = get_session_variable( get_special_var_name( e_special_var_file_list ) );
   }

   if( file_list_for_rejection.empty( ) )
      throw runtime_error( "unexpected null file_list_for_rejection in storage_channel_documents_reject_waiting" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_documents_reject_waiting" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   if( ofs.has_folder( identity ) )
      ofs.set_folder( identity );
   else
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   set< string > rejecting;

   split( file_list_for_rejection, rejecting, '\n' );

   map< string, string > file_sizes;

   stringstream ss;

   ofs.branch_objects( "*", ss );

   string all_objects( ss.str( ) );

   if( !all_objects.empty( ) )
   {
      vector< string > objects;

      split( all_objects, objects, '\n' );

      for( size_t i = 0; i < objects.size( ); i++ )
      {
         string next( objects[ i ] );

         string::size_type pos = next.rfind( " (" );

         if( pos != string::npos )
            file_sizes.insert( make_pair( next.substr( 0, pos ), next.substr( pos ) ) );
      }
   }

   ofs.set_folder( c_channel_folder_ciyam );

   if( ofs.has_file( c_channel_waiting ) )
   {
      string waiting;
      string new_waiting;

      ofs.fetch_from_text_file( c_channel_waiting, waiting );
      
      vector< string > all_waiting;

      split( waiting, all_waiting, '\n' );

      for( size_t i = 0; i < all_waiting.size( ); i++ )
      {
         string next( all_waiting[ i ] );

         if( !rejecting.count( next + file_sizes[ next ] ) )
         {
            if( !new_waiting.empty( ) )
               new_waiting += '\n';

            new_waiting += next;
         }
      }

      if( new_waiting.empty( ) )
         ofs.remove_file( c_channel_waiting );
      else
         ofs.store_as_text_file( c_channel_waiting, new_waiting );
   }
}

void storage_channel_documents_reject_reviewing( const char* p_identity, const char* p_file_list_for_rejection )
{
   guard g( g_mutex );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_reject_reviewing" );

   string file_list_for_rejection;

   if( p_file_list_for_rejection && ( *p_file_list_for_rejection != 0 ) )
      file_list_for_rejection = string( p_file_list_for_rejection );
   else
   {
      file_list_for_rejection = get_session_variable( get_special_var_name( e_special_var_arg2 ) );

      if( file_list_for_rejection.empty( ) )
         file_list_for_rejection = get_session_variable( get_special_var_name( e_special_var_file_list ) );
   }

   if( file_list_for_rejection.empty( ) )
      throw runtime_error( "unexpected null file_list_for_rejection in storage_channel_documents_reject_reviewing" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_documents_reject_reviewing" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   if( ofs.has_folder( identity ) )
      ofs.set_folder( identity );
   else
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   set< string > rejecting;

   split( file_list_for_rejection, rejecting, '\n' );

   map< string, string > file_sizes;

   stringstream ss;

   ofs.branch_objects( "*", ss );

   string all_objects( ss.str( ) );

   if( !all_objects.empty( ) )
   {
      vector< string > objects;

      split( all_objects, objects, '\n' );

      for( size_t i = 0; i < objects.size( ); i++ )
      {
         string next( objects[ i ] );

         string::size_type pos = next.rfind( " (" );

         if( pos != string::npos )
            file_sizes.insert( make_pair( next.substr( 0, pos ), next.substr( pos ) ) );
      }
   }

   ofs.set_folder( c_channel_folder_ciyam );

   if( ofs.has_file( c_channel_reviewing ) )
   {
      string reviewing;
      string new_reviewing;

      ods::transaction ods_tx( ods_db );

      ofs.fetch_from_text_file( c_channel_reviewing, reviewing );
      
      vector< string > all_reviewing;

      split( reviewing, all_reviewing, '\n' );

      for( size_t i = 0; i < all_reviewing.size( ); i++ )
      {
         string next( all_reviewing[ i ] );

         if( rejecting.count( next + file_sizes[ next ] ) )
            ofs.remove_file( "../" + next );
         else
         {
            if( !new_reviewing.empty( ) )
               new_reviewing += '\n';

            new_reviewing += next;
         }
      }

      if( new_reviewing.empty( ) )
         ofs.remove_file( c_channel_reviewing );
      else
         ofs.store_as_text_file( c_channel_reviewing, new_reviewing );

      ods_tx.commit( );
   }
}

void storage_channel_documents_approve_waiting( const char* p_identity, const char* p_file_list_for_approval )
{
   guard g( g_mutex );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_approve_waiting" );

   string file_list_for_approval;

   if( p_file_list_for_approval && ( *p_file_list_for_approval != 0 ) )
      file_list_for_approval = string( p_file_list_for_approval );
   else
   {
      file_list_for_approval = get_session_variable( get_special_var_name( e_special_var_arg2 ) );

      if( file_list_for_approval.empty( ) )
         file_list_for_approval = get_session_variable( get_special_var_name( e_special_var_file_list ) );
   }

   if( file_list_for_approval.empty( ) )
      throw runtime_error( "unexpected null file_list_for_approval in storage_channel_documents_approve_waiting" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_documents_approve_waiting" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   if( ofs.has_folder( identity ) )
      ofs.set_folder( identity );
   else
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   set< string > approving;

   split( file_list_for_approval, approving, '\n' );

   map< string, string > file_sizes;

   stringstream ss;

   ofs.branch_objects( "*", ss );

   string all_objects( ss.str( ) );

   if( !all_objects.empty( ) )
   {
      vector< string > objects;

      split( all_objects, objects, '\n' );

      for( size_t i = 0; i < objects.size( ); i++ )
      {
         string next( objects[ i ] );

         string::size_type pos = next.rfind( " (" );

         if( pos != string::npos )
            file_sizes.insert( make_pair( next.substr( 0, pos ), next.substr( pos ) ) );
      }
   }

   ofs.set_folder( c_channel_folder_ciyam );

   if( ofs.has_file( c_channel_waiting ) )
   {
      string waiting;
      string new_waiting;

      ods::transaction ods_tx( ods_db );

      ofs.fetch_from_text_file( c_channel_waiting, waiting );
      
      vector< string > all_waiting;

      split( waiting, all_waiting, '\n' );

      for( size_t i = 0; i < all_waiting.size( ); i++ )
      {
         string next( all_waiting[ i ] );

         if( approving.count( next + file_sizes[ next ] ) )
            ofs.permissions_file( "../" + next, c_perms_r_r );
         else
         {
            if( !new_waiting.empty( ) )
               new_waiting += '\n';

            new_waiting += next;
         }
      }

      if( new_waiting.empty( ) )
         ofs.remove_file( c_channel_waiting );
      else
         ofs.store_as_text_file( c_channel_waiting, new_waiting );

      ods_tx.commit( );
   }
}

void storage_channel_documents_approve_reviewing( const char* p_identity, const char* p_file_list_for_approval )
{
   guard g( g_mutex );

   string identity;

   if( p_identity && ( *p_identity != 0 ) )
      identity = string( p_identity );
   else
   {
      identity = get_session_variable( get_special_var_name( e_special_var_arg1 ) );

      if( identity.empty( ) )
         identity = get_session_variable( get_special_var_name( e_special_var_identity ) );
   }

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_approve_reviewing" );

   string file_list_for_approval;

   if( p_file_list_for_approval && ( *p_file_list_for_approval != 0 ) )
      file_list_for_approval = string( p_file_list_for_approval );
   else
   {
      file_list_for_approval = get_session_variable( get_special_var_name( e_special_var_arg2 ) );

      if( file_list_for_approval.empty( ) )
         file_list_for_approval = get_session_variable( get_special_var_name( e_special_var_file_list ) );
   }

   if( file_list_for_approval.empty( ) )
      throw runtime_error( "unexpected null file_list_for_approval in storage_channel_documents_approve_reviewing" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_documents_approve_reviewing" );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   if( ofs.has_folder( identity ) )
      ofs.set_folder( identity );
   else
      throw runtime_error( "channel folder for '" + identity + "' was not found" );

   set< string > approving;

   split( file_list_for_approval, approving, '\n' );

   map< string, string > file_sizes;

   stringstream ss;

   ofs.branch_objects( "*", ss );

   string all_objects( ss.str( ) );

   if( !all_objects.empty( ) )
   {
      vector< string > objects;

      split( all_objects, objects, '\n' );

      for( size_t i = 0; i < objects.size( ); i++ )
      {
         string next( objects[ i ] );

         string::size_type pos = next.rfind( " (" );

         if( pos != string::npos )
            file_sizes.insert( make_pair( next.substr( 0, pos ), next.substr( pos ) ) );
      }
   }

   ofs.set_folder( c_channel_folder_ciyam );

   if( ofs.has_file( c_channel_reviewing ) )
   {
      string reviewing;
      string new_reviewing;

      ods::transaction ods_tx( ods_db );

      ofs.fetch_from_text_file( c_channel_reviewing, reviewing );
      
      vector< string > all_reviewing;

      split( reviewing, all_reviewing, '\n' );

      for( size_t i = 0; i < all_reviewing.size( ); i++ )
      {
         string next( all_reviewing[ i ] );

         if( approving.count( next + file_sizes[ next ] ) )
            ofs.permissions_file( "../" + next, c_perms_r_r );
         else
         {
            if( !new_reviewing.empty( ) )
               new_reviewing += '\n';

            new_reviewing += next;
         }
      }

      if( new_reviewing.empty( ) )
         ofs.remove_file( c_channel_reviewing );
      else
         ofs.store_as_text_file( c_channel_reviewing, new_reviewing );

      ods_tx.commit( );
   }
}

string storage_channel_documents_specific(
 const string& identity, channel_documents_type type, bool include_sizes )
{
   guard g( g_mutex );

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_specific" );

   bool is_standard = false;

   string name( storage_name( &is_standard ) );

   if( is_standard )
      throw runtime_error( "invalid non-peerchain storage '" + name + "' for storage_channel_documents_specific" );

   string retval;

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   if( identity.empty( ) )
      throw runtime_error( "unexpected null identity in storage_channel_documents_specific" );

   if( ofs.has_folder( identity ) )
   {
      ofs.set_folder( identity );

      map< string, string > file_sizes;

      if( include_sizes )
      {
         stringstream ss;

         ofs.branch_objects( "*", ss );

         string all_objects( ss.str( ) );

         if( !all_objects.empty( ) )
         {
            vector< string > objects;

            split( all_objects, objects, '\n' );

            for( size_t i = 0; i < objects.size( ); i++ )
            {
               string next( objects[ i ] );

               string::size_type pos = next.rfind( " (" );

               if( pos != string::npos )
                  file_sizes.insert( make_pair( next.substr( 0, pos ), next.substr( pos ) ) );
            }
         }
      }

      ofs.set_folder( c_channel_folder_ciyam );

      bool has_specifics = false;

      if( ( type == e_channel_documents_type_pending ) && ofs.has_file( c_channel_pending ) )
         has_specifics = true;

      if( ( type == e_channel_documents_type_waiting ) && ofs.has_file( c_channel_waiting ) )
         has_specifics = true;

      if( ( type == e_channel_documents_type_retrieved ) && ofs.has_file( c_channel_updated ) )
         has_specifics = true;

      if( ( type == e_channel_documents_type_reviewing ) && ofs.has_file( c_channel_reviewing ) )
         has_specifics = true;

      if( ( type == e_channel_documents_type_submitting ) && ofs.has_file( c_channel_submitting ) )
         has_specifics = true;

      if( has_specifics )
      {
         string specifics;

         if( type == e_channel_documents_type_pending )
            ofs.fetch_from_text_file( c_channel_pending, specifics );
         else if( type == e_channel_documents_type_waiting )
            ofs.fetch_from_text_file( c_channel_waiting, specifics );
         else if( type == e_channel_documents_type_retrieved )
            ofs.fetch_from_text_file( c_channel_updated, specifics );
         else if( type == e_channel_documents_type_reviewing )
            ofs.fetch_from_text_file( c_channel_reviewing, specifics );
         else if( type == e_channel_documents_type_submitting )
            ofs.fetch_from_text_file( c_channel_submitting, specifics );
         else
            throw runtime_error( "unexpected channel_documents_type #"
             + to_string( type ) + " in storage_channel_documents_specific" );

         if( !specifics.empty( ) )
         {
            vector< string > all_specifics;

            split( specifics, all_specifics, '\n' );

            for( size_t i = 0; i < all_specifics.size( ); i++ )
            {
               string next_specific( all_specifics[ i ] );

               if( !retval.empty( ) )
                  retval += '\n';

               if( !include_sizes )
                  retval += next_specific;
               else
                  retval += next_specific + file_sizes[ next_specific ];
            }
         }
      }
   }

   return retval;
}

void list_datachains( ostream& os )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   ofs.list_folders( os );
}

void list_datachains( vector< string >& datachains )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   ofs.list_folders( datachains );
}

bool has_datachain( const string& identity )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   if( !ofs.has_folder( identity ) )
      throw runtime_error( "datachain '" + identity + "' not found" );

   return ofs.has_folder( identity );
}

string get_datachain_info( const string& identity, size_t* p_data_type, size_t* p_height )
{
   guard g( g_mutex );

   string retval;

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   if( !ofs.has_folder( identity ) )
      throw runtime_error( "datachain '" + identity + "' not found" );

   ofs.set_folder( identity );

   int32_t data_type;
   int32_t height;

   string channels;

   ofs.fetch_from_text_file( c_datachain_type, data_type );
   ofs.fetch_from_text_file( c_datachain_height, height );
   ofs.fetch_from_text_file( c_datachain_channels, channels );

   if( p_data_type )
      *p_data_type = data_type;

   if( p_height )
      *p_height = height;

   retval = to_string( data_type ) + ' ' + to_string( height );

   if( !channels.empty( ) )
      retval += ' ' + channels;

   return retval;
}

void create_datachain_info( const string& identity, size_t data_type )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_channels );

   vector< string > folders;

   ofs.list_folders( folders );

   set< string > channels( folders.begin( ), folders.end( ) );

   if( channels.find( identity ) != channels.end( ) )
      throw runtime_error( "datachain '" + identity + "' is a channel identity" );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   folders.clear( );

   if( ofs.has_folder( identity ) )
      throw runtime_error( "datachain '" + identity + "' already exists" );
   else
   {
      ods::transaction ods_tx( ods_db );

      ofs.add_folder( identity );

      ofs.set_folder( identity );

      ofs.store_as_text_file( c_datachain_type, ( int32_t )data_type );

      ofs.store_as_text_file( c_datachain_height, ( int32_t )0 );
      ofs.store_as_text_file( c_datachain_channels, "" );

      ods_tx.commit( );
   }
}

void remove_datachain_info( const string& identity )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   if( ofs.has_folder( identity ) )
   {
      ods::transaction ods_tx( ods_db );

      ofs.remove_folder( identity, 0, true );

      ods_tx.commit( );
   }
}

void set_datachain_height( const string& identity, size_t new_height )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   if( !ofs.has_folder( identity ) )
      throw runtime_error( "datachain '" + identity + "' not found" );
   else
   {
      ofs.set_folder( identity );

      ods::transaction ods_tx( ods_db );

      int32_t height;
      ofs.fetch_from_text_file( c_datachain_height, height );

      if( height == new_height )
         ods_tx.rollback( );
      else
      {
         ofs.store_as_text_file( c_datachain_height, ( int32_t )new_height );

         ods_tx.commit( );
      }
   }
}

bool is_linked_to_datachain( const string& channel, const string& identity )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_read > ap_bulk_read;

   if( !ods_db.is_thread_bulk_locked( ) )
      ap_bulk_read.reset( new ods::bulk_read( ods_db ) );

   ods_file_system ofs( ods_db );

   vector< string > folders;

   ofs.set_root_folder( c_storage_folder_name_datachains );

   bool retval = false;

   if( ofs.has_folder( identity ) )
   {
      ofs.set_folder( identity );

      string channels;

      ofs.fetch_from_text_file( c_datachain_channels, channels );

      set< string > all_channels;

      if( !channels.empty( ) )
         split( channels, all_channels );

      retval = all_channels.count( channel );
   }

   return retval;
}


void link_channel_to_datachain( const string& channel, const string& identity )
{
   guard g( g_mutex );

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   vector< string > folders;

   ofs.set_root_folder( c_storage_folder_name_channels );

   ofs.list_folders( folders );

   set< string > channels( folders.begin( ), folders.end( ) );

   if( channels.find( channel ) == channels.end( ) )
      throw runtime_error( "channel '" + channel + "' not found" );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   folders.clear( );

   ofs.list_folders( folders );

   set< string > datachains( folders.begin( ), folders.end( ) );

   if( datachains.find( identity ) == datachains.end( ) )
      throw runtime_error( "datachain '" + identity + "' not found" );
   else
   {
      ods::transaction ods_tx( ods_db );

      ofs.set_root_folder( c_storage_folder_name_datachains );

      ofs.set_folder( identity );

      string channels;

      ofs.fetch_from_text_file( c_datachain_channels, channels );

      set< string > all_channels;

      if( !channels.empty( ) )
         split( channels, all_channels );

      if( all_channels.find( channel ) != all_channels.end( ) )
         ods_tx.rollback( );
      else
      {
         all_channels.insert( channel );

         channels = join( all_channels );

         ofs.store_as_text_file( c_datachain_channels, channels );

         ods_tx.commit( );
      }
   }
}

void unlink_channel_from_datachain( const string& channel, const string& identity )
{
   guard g( g_mutex );

   vector< string > folders;

   ods& ods_db( storage_ods_instance( ) );

   auto_ptr< ods::bulk_write > ap_bulk_write;

   if( !ods_db.is_thread_bulk_write_locked( ) )
      ap_bulk_write.reset( new ods::bulk_write( ods_db ) );

   ods_file_system ofs( ods_db );

   ofs.set_root_folder( c_storage_folder_name_datachains );

   ofs.list_folders( folders );

   set< string > datachains( folders.begin( ), folders.end( ) );

   if( datachains.find( identity ) == datachains.end( ) )
      throw runtime_error( "datachain '" + identity + "' not found" );
   else
   {
      ods::transaction ods_tx( ods_db );

      ofs.set_root_folder( c_storage_folder_name_datachains );

      ofs.set_folder( identity );

      string channels;

      ofs.fetch_from_text_file( c_datachain_channels, channels );

      set< string > all_channels;

      if( !channels.empty( ) )
         split( channels, all_channels );

      if( all_channels.find( channel ) == all_channels.end( ) )
         ods_tx.rollback( );
      else
      {
         all_channels.erase( channel );

         channels = join( all_channels );

         ofs.store_as_text_file( c_datachain_channels, channels );

         ods_tx.commit( );
      }
   }
}

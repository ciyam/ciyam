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
#  include <algorithm>
#  include <stdexcept>
#endif

// [<start macros>]
// [<finish macros>]

#include "Meta_Global_Peerchain_Entry.h"

#include "ciyam_base.h"
#include "ciyam_core.h"
#include "class_domains.h"
#include "ciyam_channels.h"
#include "module_strings.h"
#include "ciyam_constants.h"
#include "ciyam_variables.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

template< typename T > inline void sanity_check( const T& t ) { }

inline void sanity_check( const string& s )
{
   if( s.length( ) > c_max_string_length_limit )
      throw runtime_error( "unexpected max string length limit exceeded with: " + s );
}

#include "Meta_Global_Peerchain_Entry.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Actions = "145111";
const char* const c_field_id_Auto_Start = "145107";
const char* const c_field_id_Chain_Id = "145101";
const char* const c_field_id_Description = "145102";
const char* const c_field_id_Extra_Value = "145110";
const char* const c_field_id_Host_Name = "145104";
const char* const c_field_id_Host_Port = "145105";
const char* const c_field_id_Local_Port = "145106";
const char* const c_field_id_Num_Helpers = "145109";
const char* const c_field_id_Peer_Type = "145103";
const char* const c_field_id_Port_Numbers = "145112";
const char* const c_field_id_Shared_Secret = "145113";
const char* const c_field_id_Status = "145108";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_Auto_Start = "Auto_Start";
const char* const c_field_name_Chain_Id = "Chain_Id";
const char* const c_field_name_Description = "Description";
const char* const c_field_name_Extra_Value = "Extra_Value";
const char* const c_field_name_Host_Name = "Host_Name";
const char* const c_field_name_Host_Port = "Host_Port";
const char* const c_field_name_Local_Port = "Local_Port";
const char* const c_field_name_Num_Helpers = "Num_Helpers";
const char* const c_field_name_Peer_Type = "Peer_Type";
const char* const c_field_name_Port_Numbers = "Port_Numbers";
const char* const c_field_name_Shared_Secret = "Shared_Secret";
const char* const c_field_name_Status = "Status";

const char* const c_field_display_name_Actions = "field_global_peerchain_entry_actions";
const char* const c_field_display_name_Auto_Start = "field_global_peerchain_entry_auto_start";
const char* const c_field_display_name_Chain_Id = "field_global_peerchain_entry_chain_id";
const char* const c_field_display_name_Description = "field_global_peerchain_entry_description";
const char* const c_field_display_name_Extra_Value = "field_global_peerchain_entry_extra_value";
const char* const c_field_display_name_Host_Name = "field_global_peerchain_entry_host_name";
const char* const c_field_display_name_Host_Port = "field_global_peerchain_entry_host_port";
const char* const c_field_display_name_Local_Port = "field_global_peerchain_entry_local_port";
const char* const c_field_display_name_Num_Helpers = "field_global_peerchain_entry_num_helpers";
const char* const c_field_display_name_Peer_Type = "field_global_peerchain_entry_peer_type";
const char* const c_field_display_name_Port_Numbers = "field_global_peerchain_entry_port_numbers";
const char* const c_field_display_name_Shared_Secret = "field_global_peerchain_entry_shared_secret";
const char* const c_field_display_name_Status = "field_global_peerchain_entry_status";

const int c_num_fields = 13;

const char* const c_all_sorted_field_ids[ ] =
{
   "145101",
   "145102",
   "145103",
   "145104",
   "145105",
   "145106",
   "145107",
   "145108",
   "145109",
   "145110",
   "145111",
   "145112",
   "145113"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "Auto_Start",
   "Chain_Id",
   "Description",
   "Extra_Value",
   "Host_Name",
   "Host_Port",
   "Local_Port",
   "Num_Helpers",
   "Peer_Type",
   "Port_Numbers",
   "Shared_Secret",
   "Status"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_encrypted_fields = 1;

const char* const c_encrypted_sorted_field_ids[ ] =
{
   "145113"
};

const char* const c_encrypted_sorted_field_names[ ] =
{
   "Shared_Secret"
};

inline bool is_encrypted_field( const string& field )
{
   return binary_search( c_encrypted_sorted_field_ids,
    c_encrypted_sorted_field_ids + c_num_encrypted_fields, field.c_str( ), compare )
    || binary_search( c_encrypted_sorted_field_names,
    c_encrypted_sorted_field_names + c_num_encrypted_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 4;

const char* const c_transient_sorted_field_ids[ ] =
{
   "145101",
   "145108",
   "145111",
   "145112"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Actions",
   "Chain_Id",
   "Port_Numbers",
   "Status"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Connect = "145410";
const char* const c_procedure_id_Disconnect = "145420";
const char* const c_procedure_id_Finish = "145440";
const char* const c_procedure_id_Start = "145430";

const uint64_t c_modifier_Is_Backup_Only = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Connected = UINT64_C( 0x200 );
const uint64_t c_modifier_Is_Connecting = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Disconnecting = UINT64_C( 0x800 );
const uint64_t c_modifier_Is_Halted = UINT64_C( 0x1000 );
const uint64_t c_modifier_Is_Hub = UINT64_C( 0x2000 );
const uint64_t c_modifier_Is_Local_Only = UINT64_C( 0x4000 );
const uint64_t c_modifier_Not_Is_Halted = UINT64_C( 0x8000 );

domain_string_max_size< 50 > g_Chain_Id_domain;
domain_string_max_size< 100 > g_Description_domain;
domain_string_max_size< 10 > g_Extra_Value_domain;
domain_string_max_size< 100 > g_Host_Name_domain;
domain_int_range< 0, 65535 > g_Host_Port_domain;
domain_int_range< 0, 65535 > g_Local_Port_domain;
domain_int_range< 0, 99 > g_Num_Helpers_domain;
domain_string_max_size< 200 > g_Shared_Secret_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Global_Peerchain_Entry* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Global_Peerchain_Entry* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

struct validate_formatter
{
   validate_formatter( ) : num( 0 ) { }

   string get( const string& name ) { return masks[ name ]; }

   void set( const string& name, const string& mask )
   {
      masks.insert( make_pair( name, mask ) );
   }

   int num;

   map< string, string > masks;
};

inline validation_error_value_type
 construct_validation_error( int& num, const string& field_name, const string& error_message )
{
   return validation_error_value_type(
    construct_key_from_int( "", ++num, 4 ) + ':' + field_name, error_message );
}

string g_default_Actions = string( );
bool g_default_Auto_Start = bool( 0 );
string g_default_Chain_Id = string( );
string g_default_Description = string( );
string g_default_Extra_Value = string( );
string g_default_Host_Name = string( );
int g_default_Host_Port = int( 0 );
int g_default_Local_Port = int( 0 );
int g_default_Num_Helpers = int( 5 );
int g_default_Peer_Type = int( 0 );
string g_default_Port_Numbers = string( );
string g_default_Shared_Secret = string( );
int g_default_Status = int( 0 );

set< int > g_peerchain_peer_type_enum;
set< int > g_peerchain_status_enum;

const int c_enum_peerchain_peer_type_Hub( -2 );
const int c_enum_peerchain_peer_type_User( -1 );
const int c_enum_peerchain_peer_type_Hosted( 0 );
const int c_enum_peerchain_peer_type_Local_Only( 1 );
const int c_enum_peerchain_peer_type_Backup_Only( 2 );
const int c_enum_peerchain_peer_type_Shared_Only( 3 );

string get_enum_string_peerchain_peer_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for peerchain_peer_type" );
   else if( to_string( val ) == to_string( "-2" ) )
      string_name = "enum_peerchain_peer_type_Hub";
   else if( to_string( val ) == to_string( "-1" ) )
      string_name = "enum_peerchain_peer_type_User";
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_peerchain_peer_type_Hosted";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_peerchain_peer_type_Local_Only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_peerchain_peer_type_Backup_Only";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_peerchain_peer_type_Shared_Only";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for peerchain_peer_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_peerchain_status_Halted( 0 );
const int c_enum_peerchain_status_Waiting( 1 );
const int c_enum_peerchain_status_Connected( 2 );
const int c_enum_peerchain_status_Connecting( 3 );
const int c_enum_peerchain_status_Disconnecting( 4 );

string get_enum_string_peerchain_status( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for peerchain_status" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_peerchain_status_Halted";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_peerchain_status_Waiting";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_peerchain_status_Connected";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_peerchain_status_Connecting";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_peerchain_status_Disconnecting";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for peerchain_status" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Global_Peerchain_Entry > Global_Peerchain_Entry_registration( get_class_registry( ), "145100" );

class Meta_Global_Peerchain_Entry_command_functor;

class Meta_Global_Peerchain_Entry_command_handler : public command_handler
{
   friend class Meta_Global_Peerchain_Entry_command_functor;

   public:
   Meta_Global_Peerchain_Entry_command_handler( ) : p_Meta_Global_Peerchain_Entry( 0 ) { }

   void set_Meta_Global_Peerchain_Entry( Meta_Global_Peerchain_Entry* p_new_Meta_Global_Peerchain_Entry ) { p_Meta_Global_Peerchain_Entry = p_new_Meta_Global_Peerchain_Entry; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Global_Peerchain_Entry* p_Meta_Global_Peerchain_Entry;

   protected:
   string retval;
};

class Meta_Global_Peerchain_Entry_command_functor : public command_functor
{
   public:
   Meta_Global_Peerchain_Entry_command_functor( Meta_Global_Peerchain_Entry_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Global_Peerchain_Entry_command_handler& cmd_handler;
};

command_functor* Meta_Global_Peerchain_Entry_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Global_Peerchain_Entry_command_functor( dynamic_cast< Meta_Global_Peerchain_Entry_command_handler& >( handler ) );
}

void Meta_Global_Peerchain_Entry_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Global_Peerchain_Entry_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Global_Peerchain_Entry_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Global_Peerchain_Entry->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Global_Peerchain_Entry->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Global_Peerchain_Entry_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Global_Peerchain_Entry->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Global_Peerchain_Entry->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Global_Peerchain_Entry_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Global_Peerchain_Entry_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Global_Peerchain_Entry->Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Auto_Start ) || ( field_name == c_field_name_Auto_Start ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Global_Peerchain_Entry->Auto_Start( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Chain_Id ) || ( field_name == c_field_name_Chain_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Global_Peerchain_Entry->Chain_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Description ) || ( field_name == c_field_name_Description ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Global_Peerchain_Entry->Description( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Extra_Value ) || ( field_name == c_field_name_Extra_Value ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Global_Peerchain_Entry->Extra_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Host_Name ) || ( field_name == c_field_name_Host_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Global_Peerchain_Entry->Host_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Host_Port ) || ( field_name == c_field_name_Host_Port ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Global_Peerchain_Entry->Host_Port( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Local_Port ) || ( field_name == c_field_name_Local_Port ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Global_Peerchain_Entry->Local_Port( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Num_Helpers ) || ( field_name == c_field_name_Num_Helpers ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Global_Peerchain_Entry->Num_Helpers( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Peer_Type ) || ( field_name == c_field_name_Peer_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Global_Peerchain_Entry->Peer_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Port_Numbers ) || ( field_name == c_field_name_Port_Numbers ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Global_Peerchain_Entry->Port_Numbers( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Shared_Secret ) || ( field_name == c_field_name_Shared_Secret ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Global_Peerchain_Entry->Shared_Secret( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Status ) || ( field_name == c_field_name_Status ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Global_Peerchain_Entry->Status( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Global_Peerchain_Entry_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Global_Peerchain_Entry_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Global_Peerchain_Entry_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, string >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Auto_Start ) || ( field_name == c_field_name_Auto_Start ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, bool >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Auto_Start, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Chain_Id ) || ( field_name == c_field_name_Chain_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, string >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Chain_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Description ) || ( field_name == c_field_name_Description ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, string >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Description, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Extra_Value ) || ( field_name == c_field_name_Extra_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, string >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Extra_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Host_Name ) || ( field_name == c_field_name_Host_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, string >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Host_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Host_Port ) || ( field_name == c_field_name_Host_Port ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, int >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Host_Port, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Local_Port ) || ( field_name == c_field_name_Local_Port ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, int >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Local_Port, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Num_Helpers ) || ( field_name == c_field_name_Num_Helpers ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, int >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Num_Helpers, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Peer_Type ) || ( field_name == c_field_name_Peer_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, int >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Peer_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Port_Numbers ) || ( field_name == c_field_name_Port_Numbers ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, string >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Port_Numbers, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Shared_Secret ) || ( field_name == c_field_name_Shared_Secret ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, string >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Shared_Secret, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Status ) || ( field_name == c_field_name_Status ) ) )
      {
         handled = true;

         func_string_setter< Meta_Global_Peerchain_Entry, int >(
          *cmd_handler.p_Meta_Global_Peerchain_Entry, &Meta_Global_Peerchain_Entry::Status, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Global_Peerchain_Entry_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Global_Peerchain_Entry_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Global_Peerchain_Entry_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Global_Peerchain_Entry_Connect )
   {
      cmd_handler.p_Meta_Global_Peerchain_Entry->Connect( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Global_Peerchain_Entry_Disconnect )
   {
      cmd_handler.p_Meta_Global_Peerchain_Entry->Disconnect( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Global_Peerchain_Entry_Finish )
   {
      cmd_handler.p_Meta_Global_Peerchain_Entry->Finish( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Global_Peerchain_Entry_Start )
   {
      cmd_handler.p_Meta_Global_Peerchain_Entry->Start( );

      cmd_handler.retval.erase( );

   }
}

struct Meta_Global_Peerchain_Entry::impl : public Meta_Global_Peerchain_Entry_command_handler
{
   impl( Meta_Global_Peerchain_Entry& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Global_Peerchain_Entry( &o );

      add_commands( 0, Meta_Global_Peerchain_Entry_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Global_Peerchain_Entry_command_definitions ) );
   }

   Meta_Global_Peerchain_Entry& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Actions( ) const { return lazy_fetch( p_obj ), v_Actions; }
   void impl_Actions( const string& Actions ) { sanity_check( Actions ); v_Actions = Actions; }

   bool impl_Auto_Start( ) const { return lazy_fetch( p_obj ), v_Auto_Start; }
   void impl_Auto_Start( bool Auto_Start ) { v_Auto_Start = Auto_Start; }

   const string& impl_Chain_Id( ) const { return lazy_fetch( p_obj ), v_Chain_Id; }
   void impl_Chain_Id( const string& Chain_Id ) { sanity_check( Chain_Id ); v_Chain_Id = Chain_Id; }

   const string& impl_Description( ) const { return lazy_fetch( p_obj ), v_Description; }
   void impl_Description( const string& Description ) { sanity_check( Description ); v_Description = Description; }

   const string& impl_Extra_Value( ) const { return lazy_fetch( p_obj ), v_Extra_Value; }
   void impl_Extra_Value( const string& Extra_Value ) { sanity_check( Extra_Value ); v_Extra_Value = Extra_Value; }

   const string& impl_Host_Name( ) const { return lazy_fetch( p_obj ), v_Host_Name; }
   void impl_Host_Name( const string& Host_Name ) { sanity_check( Host_Name ); v_Host_Name = Host_Name; }

   int impl_Host_Port( ) const { return lazy_fetch( p_obj ), v_Host_Port; }
   void impl_Host_Port( int Host_Port ) { v_Host_Port = Host_Port; }

   int impl_Local_Port( ) const { return lazy_fetch( p_obj ), v_Local_Port; }
   void impl_Local_Port( int Local_Port ) { v_Local_Port = Local_Port; }

   int impl_Num_Helpers( ) const { return lazy_fetch( p_obj ), v_Num_Helpers; }
   void impl_Num_Helpers( int Num_Helpers ) { v_Num_Helpers = Num_Helpers; }

   int impl_Peer_Type( ) const { return lazy_fetch( p_obj ), v_Peer_Type; }
   void impl_Peer_Type( int Peer_Type ) { v_Peer_Type = Peer_Type; }

   const string& impl_Port_Numbers( ) const { return lazy_fetch( p_obj ), v_Port_Numbers; }
   void impl_Port_Numbers( const string& Port_Numbers ) { sanity_check( Port_Numbers ); v_Port_Numbers = Port_Numbers; }

   const string& impl_Shared_Secret( ) const { return lazy_fetch( p_obj ), v_Shared_Secret; }
   void impl_Shared_Secret( const string& Shared_Secret ) { sanity_check( Shared_Secret ); v_Shared_Secret = Shared_Secret; }

   int impl_Status( ) const { return lazy_fetch( p_obj ), v_Status; }
   void impl_Status( int Status ) { v_Status = Status; }

   void impl_Connect( );

   void impl_Disconnect( );

   void impl_Finish( );

   void impl_Start( );

   string get_field_value( int field ) const;
   void set_field_value( int field, const string& value );
   void set_field_default( int field );

   bool is_field_default( int field ) const;

   uint64_t get_state( ) const;

   string get_state_names( ) const;

   const string& execute( const string& cmd_and_args );

   void clear_foreign_key( const string& field );

   void set_foreign_key_value( const string& field, const string& value );

   const string& get_foreign_key_value( const string& field );

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   void add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const;
   void add_extra_paging_info( vector< pair< string, string > >& paging_info ) const;

   void clear( );

   bool value_will_be_provided( const string& field_name );

   void validate( uint64_t state, bool is_internal, validation_error_container* p_validation_errors );
   void validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors );

   void after_fetch( );
   void completed_fetch( bool skip_set_original );

   void at_create( );
   void post_init( );

   void to_store( bool is_create, bool is_internal );
   void for_store( bool is_create, bool is_internal );
   void after_store( bool is_create, bool is_internal );

   bool can_destroy( bool is_internal );
   void for_destroy( bool is_internal );
   void after_destroy( bool is_internal );

   void set_default_values( );

   bool is_filtered( ) const;

   void get_required_transients( ) const;

   Meta_Global_Peerchain_Entry* p_obj;
   class_pointer< Meta_Global_Peerchain_Entry > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions;
   bool v_Auto_Start;
   string v_Chain_Id;
   string v_Description;
   string v_Extra_Value;
   string v_Host_Name;
   int v_Host_Port;
   int v_Local_Port;
   int v_Num_Helpers;
   int v_Peer_Type;
   string v_Port_Numbers;
   string v_Shared_Secret;
   int v_Status;
};

void Meta_Global_Peerchain_Entry::impl::impl_Connect( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Connect_impl>]
//nyi
   connect_peerchain( get_obj( ).Chain_Id( ) );
   // [<finish Connect_impl>]
}

void Meta_Global_Peerchain_Entry::impl::impl_Disconnect( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Disconnect_impl>]
//nyi
   disconnect_peerchain( get_obj( ).Chain_Id( ) );
   // [<finish Disconnect_impl>]
}

void Meta_Global_Peerchain_Entry::impl::impl_Finish( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Finish_impl>]
//nyi
   disuse_peerchain( get_obj( ).Chain_Id( ) );
   // [<finish Finish_impl>]
}

void Meta_Global_Peerchain_Entry::impl::impl_Start( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Start_impl>]
//nyi
   use_peerchain( get_obj( ).Chain_Id( ) );
   // [<finish Start_impl>]
}

string Meta_Global_Peerchain_Entry::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Actions( ) );
      break;

      case 1:
      retval = to_string( impl_Auto_Start( ) );
      break;

      case 2:
      retval = to_string( impl_Chain_Id( ) );
      break;

      case 3:
      retval = to_string( impl_Description( ) );
      break;

      case 4:
      retval = to_string( impl_Extra_Value( ) );
      break;

      case 5:
      retval = to_string( impl_Host_Name( ) );
      break;

      case 6:
      retval = to_string( impl_Host_Port( ) );
      break;

      case 7:
      retval = to_string( impl_Local_Port( ) );
      break;

      case 8:
      retval = to_string( impl_Num_Helpers( ) );
      break;

      case 9:
      retval = to_string( impl_Peer_Type( ) );
      break;

      case 10:
      retval = to_string( impl_Port_Numbers( ) );
      break;

      case 11:
      retval = to_string( impl_Shared_Secret( ) );
      break;

      case 12:
      retval = to_string( impl_Status( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Global_Peerchain_Entry::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, string >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Actions, value );
      break;

      case 1:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, bool >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Auto_Start, value );
      break;

      case 2:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, string >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Chain_Id, value );
      break;

      case 3:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, string >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Description, value );
      break;

      case 4:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, string >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Extra_Value, value );
      break;

      case 5:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, string >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Host_Name, value );
      break;

      case 6:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, int >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Host_Port, value );
      break;

      case 7:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, int >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Local_Port, value );
      break;

      case 8:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, int >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Num_Helpers, value );
      break;

      case 9:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, int >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Peer_Type, value );
      break;

      case 10:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, string >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Port_Numbers, value );
      break;

      case 11:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, string >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Shared_Secret, value );
      break;

      case 12:
      func_string_setter< Meta_Global_Peerchain_Entry::impl, int >( *this, &Meta_Global_Peerchain_Entry::impl::impl_Status, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Global_Peerchain_Entry::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Actions( g_default_Actions );
      break;

      case 1:
      impl_Auto_Start( g_default_Auto_Start );
      break;

      case 2:
      impl_Chain_Id( g_default_Chain_Id );
      break;

      case 3:
      impl_Description( g_default_Description );
      break;

      case 4:
      impl_Extra_Value( g_default_Extra_Value );
      break;

      case 5:
      impl_Host_Name( g_default_Host_Name );
      break;

      case 6:
      impl_Host_Port( g_default_Host_Port );
      break;

      case 7:
      impl_Local_Port( g_default_Local_Port );
      break;

      case 8:
      impl_Num_Helpers( g_default_Num_Helpers );
      break;

      case 9:
      impl_Peer_Type( g_default_Peer_Type );
      break;

      case 10:
      impl_Port_Numbers( g_default_Port_Numbers );
      break;

      case 11:
      impl_Shared_Secret( g_default_Shared_Secret );
      break;

      case 12:
      impl_Status( g_default_Status );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Global_Peerchain_Entry::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Actions == g_default_Actions );
      break;

      case 1:
      retval = ( v_Auto_Start == g_default_Auto_Start );
      break;

      case 2:
      retval = ( v_Chain_Id == g_default_Chain_Id );
      break;

      case 3:
      retval = ( v_Description == g_default_Description );
      break;

      case 4:
      retval = ( v_Extra_Value == g_default_Extra_Value );
      break;

      case 5:
      retval = ( v_Host_Name == g_default_Host_Name );
      break;

      case 6:
      retval = ( v_Host_Port == g_default_Host_Port );
      break;

      case 7:
      retval = ( v_Local_Port == g_default_Local_Port );
      break;

      case 8:
      retval = ( v_Num_Helpers == g_default_Num_Helpers );
      break;

      case 9:
      retval = ( v_Peer_Type == g_default_Peer_Type );
      break;

      case 10:
      retval = ( v_Port_Numbers == g_default_Port_Numbers );
      break;

      case 11:
      retval = ( v_Shared_Secret == g_default_Shared_Secret );
      break;

      case 12:
      retval = ( v_Status == g_default_Status );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Global_Peerchain_Entry::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600098a
   if( get_obj( ).Status( ) == 0 )
      state |= c_modifier_Is_Halted;
   // [(finish modifier_field_value)] 600098a

   // [(start modifier_field_value)] 600098e
   if( get_obj( ).Status( ) == 2 ) // i.e. Connected
      state |= c_modifier_Is_Connected;
   // [(finish modifier_field_value)] 600098e

   // [(start modifier_field_value)] 600098f
   if( get_obj( ).Status( ) == 3 ) // i.e. Connecting
      state |= c_modifier_Is_Connecting;
   // [(finish modifier_field_value)] 600098f

   // [(start modifier_field_value)] 600098g
   if( get_obj( ).Status( ) == 4 ) // i.e. Disconnecting
      state |= c_modifier_Is_Connecting;
   // [(finish modifier_field_value)] 600098g

   // [(start protect_equal)] 600098h
   if( check_equal( get_obj( ).Status( ), 3 ) )
      state |= ( c_state_uneditable | c_state_undeletable | c_state_is_changing );
   // [(finish protect_equal)] 600098h

   // [(start protect_equal)] 600098i
   if( check_equal( get_obj( ).Status( ), 4 ) )
      state |= ( c_state_uneditable | c_state_undeletable | c_state_is_changing );
   // [(finish protect_equal)] 600098i

   // [(start modifier_field_value)] 600098j
   if( get_obj( ).Status( ) != 0 )
      state |= c_modifier_Not_Is_Halted;
   // [(finish modifier_field_value)] 600098j

   // [(start protect_not_equal)] 600098k
   if( check_not_equal( get_obj( ).Status( ), 0 ) )
      state |= ( c_state_undeletable );
   // [(finish protect_not_equal)] 600098k

   // [(start modifier_field_value)] 600898b
   if( get_obj( ).Peer_Type( ) == 1 )
      state |= c_modifier_Is_Local_Only;
   // [(finish modifier_field_value)] 600898b

   // [(start modifier_field_value)] 600898c
   if( get_obj( ).Peer_Type( ) == 2 )
      state |= c_modifier_Is_Backup_Only;
   // [(finish modifier_field_value)] 600898c

   // [(start modifier_field_value)] 600898d
   if( get_obj( ).Peer_Type( ) == -2 )
      state |= c_modifier_Is_Hub;
   // [(finish modifier_field_value)] 600898d

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Global_Peerchain_Entry::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_Backup_Only )
      state_names += "|" + string( "Is_Backup_Only" );
   if( state & c_modifier_Is_Connected )
      state_names += "|" + string( "Is_Connected" );
   if( state & c_modifier_Is_Connecting )
      state_names += "|" + string( "Is_Connecting" );
   if( state & c_modifier_Is_Disconnecting )
      state_names += "|" + string( "Is_Disconnecting" );
   if( state & c_modifier_Is_Halted )
      state_names += "|" + string( "Is_Halted" );
   if( state & c_modifier_Is_Hub )
      state_names += "|" + string( "Is_Hub" );
   if( state & c_modifier_Is_Local_Only )
      state_names += "|" + string( "Is_Local_Only" );
   if( state & c_modifier_Not_Is_Halted )
      state_names += "|" + string( "Not_Is_Halted" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Global_Peerchain_Entry::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Global_Peerchain_Entry::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Global_Peerchain_Entry::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Global_Peerchain_Entry::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Global_Peerchain_Entry::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   ( void )foreign_key_values;
}

void Meta_Global_Peerchain_Entry::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Global_Peerchain_Entry::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Global_Peerchain_Entry::impl::clear( )
{
   v_Actions = g_default_Actions;
   v_Auto_Start = g_default_Auto_Start;
   v_Chain_Id = g_default_Chain_Id;
   v_Description = g_default_Description;
   v_Extra_Value = g_default_Extra_Value;
   v_Host_Name = g_default_Host_Name;
   v_Host_Port = g_default_Host_Port;
   v_Local_Port = g_default_Local_Port;
   v_Num_Helpers = g_default_Num_Helpers;
   v_Peer_Type = g_default_Peer_Type;
   v_Port_Numbers = g_default_Port_Numbers;
   v_Shared_Secret = g_default_Shared_Secret;
   v_Status = g_default_Status;
}

bool Meta_Global_Peerchain_Entry::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Global_Peerchain_Entry::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( is_null( v_Description ) && !value_will_be_provided( c_field_name_Description ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Description,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Description ) ) ) ) );

   if( is_null( v_Host_Name ) && !value_will_be_provided( c_field_name_Host_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Host_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Host_Name ) ) ) ) );

   if( !is_null( v_Chain_Id )
    && ( v_Chain_Id != g_default_Chain_Id
    || !value_will_be_provided( c_field_name_Chain_Id ) )
    && !g_Chain_Id_domain.is_valid( v_Chain_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Chain_Id,
       get_module_string( c_field_display_name_Chain_Id ) + " " + error_message ) );

   if( !is_null( v_Description )
    && ( v_Description != g_default_Description
    || !value_will_be_provided( c_field_name_Description ) )
    && !g_Description_domain.is_valid( v_Description, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Description,
       get_module_string( c_field_display_name_Description ) + " " + error_message ) );

   if( !is_null( v_Extra_Value )
    && ( v_Extra_Value != g_default_Extra_Value
    || !value_will_be_provided( c_field_name_Extra_Value ) )
    && !g_Extra_Value_domain.is_valid( v_Extra_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Extra_Value,
       get_module_string( c_field_display_name_Extra_Value ) + " " + error_message ) );

   if( !is_null( v_Host_Name )
    && ( v_Host_Name != g_default_Host_Name
    || !value_will_be_provided( c_field_name_Host_Name ) )
    && !g_Host_Name_domain.is_valid( v_Host_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Host_Name,
       get_module_string( c_field_display_name_Host_Name ) + " " + error_message ) );

   if( !is_null( v_Host_Port )
    && ( v_Host_Port != g_default_Host_Port
    || !value_will_be_provided( c_field_name_Host_Port ) )
    && !g_Host_Port_domain.is_valid( v_Host_Port, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Host_Port,
       get_module_string( c_field_display_name_Host_Port ) + " " + error_message ) );

   if( !is_null( v_Local_Port )
    && ( v_Local_Port != g_default_Local_Port
    || !value_will_be_provided( c_field_name_Local_Port ) )
    && !g_Local_Port_domain.is_valid( v_Local_Port, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Local_Port,
       get_module_string( c_field_display_name_Local_Port ) + " " + error_message ) );

   if( !is_null( v_Num_Helpers )
    && ( v_Num_Helpers != g_default_Num_Helpers
    || !value_will_be_provided( c_field_name_Num_Helpers ) )
    && !g_Num_Helpers_domain.is_valid( v_Num_Helpers, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Num_Helpers,
       get_module_string( c_field_display_name_Num_Helpers ) + " " + error_message ) );

   if( !is_null( v_Shared_Secret )
    && ( v_Shared_Secret != g_default_Shared_Secret
    || !value_will_be_provided( c_field_name_Shared_Secret ) )
    && !g_Shared_Secret_domain.is_valid( v_Shared_Secret, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Shared_Secret,
       get_module_string( c_field_display_name_Shared_Secret ) + " " + error_message ) );

   if( !g_peerchain_peer_type_enum.count( v_Peer_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Peer_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Peer_Type ) ) ) ) );

   if( !g_peerchain_status_enum.count( v_Status ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Status,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Status ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Global_Peerchain_Entry::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Chain_Id )
    && ( fields_set.count( c_field_id_Chain_Id ) || fields_set.count( c_field_name_Chain_Id ) )
    && !g_Chain_Id_domain.is_valid( v_Chain_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Chain_Id,
       get_module_string( c_field_display_name_Chain_Id ) + " " + error_message ) );

   if( !is_null( v_Description )
    && ( fields_set.count( c_field_id_Description ) || fields_set.count( c_field_name_Description ) )
    && !g_Description_domain.is_valid( v_Description, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Description,
       get_module_string( c_field_display_name_Description ) + " " + error_message ) );

   if( !is_null( v_Extra_Value )
    && ( fields_set.count( c_field_id_Extra_Value ) || fields_set.count( c_field_name_Extra_Value ) )
    && !g_Extra_Value_domain.is_valid( v_Extra_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Extra_Value,
       get_module_string( c_field_display_name_Extra_Value ) + " " + error_message ) );

   if( !is_null( v_Host_Name )
    && ( fields_set.count( c_field_id_Host_Name ) || fields_set.count( c_field_name_Host_Name ) )
    && !g_Host_Name_domain.is_valid( v_Host_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Host_Name,
       get_module_string( c_field_display_name_Host_Name ) + " " + error_message ) );

   if( !is_null( v_Host_Port )
    && ( fields_set.count( c_field_id_Host_Port ) || fields_set.count( c_field_name_Host_Port ) )
    && !g_Host_Port_domain.is_valid( v_Host_Port, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Host_Port,
       get_module_string( c_field_display_name_Host_Port ) + " " + error_message ) );

   if( !is_null( v_Local_Port )
    && ( fields_set.count( c_field_id_Local_Port ) || fields_set.count( c_field_name_Local_Port ) )
    && !g_Local_Port_domain.is_valid( v_Local_Port, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Local_Port,
       get_module_string( c_field_display_name_Local_Port ) + " " + error_message ) );

   if( !is_null( v_Num_Helpers )
    && ( fields_set.count( c_field_id_Num_Helpers ) || fields_set.count( c_field_name_Num_Helpers ) )
    && !g_Num_Helpers_domain.is_valid( v_Num_Helpers, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Num_Helpers,
       get_module_string( c_field_display_name_Num_Helpers ) + " " + error_message ) );

   if( !is_null( v_Shared_Secret )
    && ( fields_set.count( c_field_id_Shared_Secret ) || fields_set.count( c_field_name_Shared_Secret ) )
    && !g_Shared_Secret_domain.is_valid( v_Shared_Secret, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Shared_Secret,
       get_module_string( c_field_display_name_Shared_Secret ) + " " + error_message ) );
}

void Meta_Global_Peerchain_Entry::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
//nyi
   get_obj( ).Actions( "" );

   get_obj( ).Chain_Id( get_obj( ).get_key( ) );

   if( get_obj( ).Host_Name( ) == c_local_host )
   {
      if( has_registered_listener_id( get_obj( ).Chain_Id( ) ) )
      {
         get_obj( ).Status( c_enum_peerchain_status_Waiting );
         get_obj( ).Actions( '<' + string( c_procedure_id_Finish ) );
      }
      else
         get_obj( ).Actions( '<' + string( c_procedure_id_Start ) );

      get_obj( ).Port_Numbers( to_string( get_obj( ).Local_Port( ) ) );
   }
   else
   {
      if( !get_system_variable( '~' + get_obj( ).Chain_Id( ) ).empty( ) )
         get_obj( ).Status( c_enum_peerchain_status_Disconnecting );
      else if( has_any_session_variable( get_obj( ).Chain_Id( ) ) )
      {
         get_obj( ).Status( c_enum_peerchain_status_Connected );
         get_obj( ).Actions( '<' + string( c_procedure_id_Disconnect ) );
      }
      else if( !get_system_variable( get_obj( ).Chain_Id( ) ).empty( ) )
         get_obj( ).Status( c_enum_peerchain_status_Connecting );
      else
      {
         if( !get_obj( ).Local_Port( ) )
            get_obj( ).Actions( '<' + string( c_procedure_id_Connect ) );
         else
         {
            if( has_registered_listener_id( get_obj( ).Chain_Id( ) ) )
            {
               get_obj( ).Status( c_enum_peerchain_status_Waiting );

               get_obj( ).Actions( '<' + string( c_procedure_id_Finish ) + ",<" + string( c_procedure_id_Connect ) );
            }
            else
               get_obj( ).Actions( '<' + string( c_procedure_id_Start ) );
         }
      }

      get_obj( ).Port_Numbers( to_string( get_obj( ).Host_Port( ) ) + '/' + to_string( get_obj( ).Local_Port( ) ) );
   }
   // [<finish after_fetch>]
}

void Meta_Global_Peerchain_Entry::impl::completed_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start completed_fetch>]
   // [<finish completed_fetch>]
}

void Meta_Global_Peerchain_Entry::impl::at_create( )
{
   // [<start at_create>]
//nyi
   throw runtime_error( "create is not supported for this type" );
   // [<finish at_create>]
}

void Meta_Global_Peerchain_Entry::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Global_Peerchain_Entry::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Global_Peerchain_Entry::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Global_Peerchain_Entry::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Global_Peerchain_Entry::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Global_Peerchain_Entry::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Global_Peerchain_Entry::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Global_Peerchain_Entry::impl::set_default_values( )
{
   clear( );
}

bool Meta_Global_Peerchain_Entry::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Global_Peerchain_Entry::impl::get_required_transients( ) const
{
   required_transients.clear( );

   get_obj( ).add_required_transients( required_transients );

   set< string > dependents( required_transients.begin( ), required_transients.end( ) );

   p_obj->get_required_field_names( required_transients, true, &dependents );

   int iterations = 0;

   // NOTE: It is possible that due to "interdependent" required fields
   // some required fields may not have been added in the first or even
   // later calls to "get_required_field_names" so continue calling the
   // function until no further field names have been added.
   size_t num_required = required_transients.size( );

   while( num_required )
   {
      p_obj->get_required_field_names( required_transients, true, &dependents );

      if( required_transients.size( ) == num_required )
         break;

      if( ++iterations > 100 )
         throw runtime_error( "unexpected excessive get_required_field_names( ) iterations in get_required_transients( )" );

      num_required = required_transients.size( );
   }
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Global_Peerchain_Entry::Meta_Global_Peerchain_Entry( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Global_Peerchain_Entry::~Meta_Global_Peerchain_Entry( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Global_Peerchain_Entry::Actions( ) const
{
   return p_impl->impl_Actions( );
}

void Meta_Global_Peerchain_Entry::Actions( const string& Actions )
{
   p_impl->impl_Actions( Actions );
}

bool Meta_Global_Peerchain_Entry::Auto_Start( ) const
{
   return p_impl->impl_Auto_Start( );
}

void Meta_Global_Peerchain_Entry::Auto_Start( bool Auto_Start )
{
   p_impl->impl_Auto_Start( Auto_Start );
}

const string& Meta_Global_Peerchain_Entry::Chain_Id( ) const
{
   return p_impl->impl_Chain_Id( );
}

void Meta_Global_Peerchain_Entry::Chain_Id( const string& Chain_Id )
{
   p_impl->impl_Chain_Id( Chain_Id );
}

const string& Meta_Global_Peerchain_Entry::Description( ) const
{
   return p_impl->impl_Description( );
}

void Meta_Global_Peerchain_Entry::Description( const string& Description )
{
   p_impl->impl_Description( Description );
}

const string& Meta_Global_Peerchain_Entry::Extra_Value( ) const
{
   return p_impl->impl_Extra_Value( );
}

void Meta_Global_Peerchain_Entry::Extra_Value( const string& Extra_Value )
{
   p_impl->impl_Extra_Value( Extra_Value );
}

const string& Meta_Global_Peerchain_Entry::Host_Name( ) const
{
   return p_impl->impl_Host_Name( );
}

void Meta_Global_Peerchain_Entry::Host_Name( const string& Host_Name )
{
   p_impl->impl_Host_Name( Host_Name );
}

int Meta_Global_Peerchain_Entry::Host_Port( ) const
{
   return p_impl->impl_Host_Port( );
}

void Meta_Global_Peerchain_Entry::Host_Port( int Host_Port )
{
   p_impl->impl_Host_Port( Host_Port );
}

int Meta_Global_Peerchain_Entry::Local_Port( ) const
{
   return p_impl->impl_Local_Port( );
}

void Meta_Global_Peerchain_Entry::Local_Port( int Local_Port )
{
   p_impl->impl_Local_Port( Local_Port );
}

int Meta_Global_Peerchain_Entry::Num_Helpers( ) const
{
   return p_impl->impl_Num_Helpers( );
}

void Meta_Global_Peerchain_Entry::Num_Helpers( int Num_Helpers )
{
   p_impl->impl_Num_Helpers( Num_Helpers );
}

int Meta_Global_Peerchain_Entry::Peer_Type( ) const
{
   return p_impl->impl_Peer_Type( );
}

void Meta_Global_Peerchain_Entry::Peer_Type( int Peer_Type )
{
   p_impl->impl_Peer_Type( Peer_Type );
}

const string& Meta_Global_Peerchain_Entry::Port_Numbers( ) const
{
   return p_impl->impl_Port_Numbers( );
}

void Meta_Global_Peerchain_Entry::Port_Numbers( const string& Port_Numbers )
{
   p_impl->impl_Port_Numbers( Port_Numbers );
}

const string& Meta_Global_Peerchain_Entry::Shared_Secret( ) const
{
   return p_impl->impl_Shared_Secret( );
}

void Meta_Global_Peerchain_Entry::Shared_Secret( const string& Shared_Secret )
{
   p_impl->impl_Shared_Secret( Shared_Secret );
}

int Meta_Global_Peerchain_Entry::Status( ) const
{
   return p_impl->impl_Status( );
}

void Meta_Global_Peerchain_Entry::Status( int Status )
{
   p_impl->impl_Status( Status );
}

void Meta_Global_Peerchain_Entry::Connect( )
{
   p_impl->impl_Connect( );
}

void Meta_Global_Peerchain_Entry::Disconnect( )
{
   p_impl->impl_Disconnect( );
}

void Meta_Global_Peerchain_Entry::Finish( )
{
   p_impl->impl_Finish( );
}

void Meta_Global_Peerchain_Entry::Start( )
{
   p_impl->impl_Start( );
}

string Meta_Global_Peerchain_Entry::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Global_Peerchain_Entry::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Global_Peerchain_Entry::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Global_Peerchain_Entry::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Global_Peerchain_Entry::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Global_Peerchain_Entry::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Global_Peerchain_Entry::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Global_Peerchain_Entry::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Global_Peerchain_Entry::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Global_Peerchain_Entry::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Global_Peerchain_Entry::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Global_Peerchain_Entry::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Global_Peerchain_Entry::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Global_Peerchain_Entry::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Global_Peerchain_Entry::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Global_Peerchain_Entry::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Global_Peerchain_Entry::clear( )
{
   p_impl->clear( );
}

void Meta_Global_Peerchain_Entry::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Global_Peerchain_Entry::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Global_Peerchain_Entry::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Global_Peerchain_Entry::completed_fetch( bool skip_set_original )
{
   p_impl->completed_fetch( skip_set_original );
}

void Meta_Global_Peerchain_Entry::at_create( )
{
   p_impl->at_create( );
}

void Meta_Global_Peerchain_Entry::post_init( )
{
   p_impl->post_init( );
}

void Meta_Global_Peerchain_Entry::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Global_Peerchain_Entry::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Global_Peerchain_Entry::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Global_Peerchain_Entry::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Global_Peerchain_Entry::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Global_Peerchain_Entry::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Global_Peerchain_Entry::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Global_Peerchain_Entry::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Global_Peerchain_Entry::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Actions )
   {
      p_id = c_field_id_Actions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Auto_Start )
   {
      p_id = c_field_id_Auto_Start;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Chain_Id )
   {
      p_id = c_field_id_Chain_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Description )
   {
      p_id = c_field_id_Description;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Extra_Value )
   {
      p_id = c_field_id_Extra_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Host_Name )
   {
      p_id = c_field_id_Host_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Host_Port )
   {
      p_id = c_field_id_Host_Port;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Local_Port )
   {
      p_id = c_field_id_Local_Port;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Num_Helpers )
   {
      p_id = c_field_id_Num_Helpers;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Peer_Type )
   {
      p_id = c_field_id_Peer_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Port_Numbers )
   {
      p_id = c_field_id_Port_Numbers;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Shared_Secret )
   {
      p_id = c_field_id_Shared_Secret;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Status )
   {
      p_id = c_field_id_Status;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Global_Peerchain_Entry::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Actions )
   {
      p_name = c_field_name_Actions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Auto_Start )
   {
      p_name = c_field_name_Auto_Start;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Chain_Id )
   {
      p_name = c_field_name_Chain_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Description )
   {
      p_name = c_field_name_Description;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Extra_Value )
   {
      p_name = c_field_name_Extra_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Host_Name )
   {
      p_name = c_field_name_Host_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Host_Port )
   {
      p_name = c_field_name_Host_Port;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Local_Port )
   {
      p_name = c_field_name_Local_Port;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Num_Helpers )
   {
      p_name = c_field_name_Num_Helpers;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Peer_Type )
   {
      p_name = c_field_name_Peer_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Port_Numbers )
   {
      p_name = c_field_name_Port_Numbers;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Shared_Secret )
   {
      p_name = c_field_name_Shared_Secret;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Status )
   {
      p_name = c_field_name_Status;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Global_Peerchain_Entry::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Global_Peerchain_Entry::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Global_Peerchain_Entry::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Global_Peerchain_Entry::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Global_Peerchain_Entry::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Global_Peerchain_Entry::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Global_Peerchain_Entry::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
   {
      name = string( c_field_display_name_Actions );
      get_module_string( c_field_display_name_Actions, &next );
   }
   if( ( id_or_name == c_field_id_Auto_Start ) || ( id_or_name == c_field_name_Auto_Start ) )
   {
      name = string( c_field_display_name_Auto_Start );
      get_module_string( c_field_display_name_Auto_Start, &next );
   }
   if( ( id_or_name == c_field_id_Chain_Id ) || ( id_or_name == c_field_name_Chain_Id ) )
   {
      name = string( c_field_display_name_Chain_Id );
      get_module_string( c_field_display_name_Chain_Id, &next );
   }
   if( ( id_or_name == c_field_id_Description ) || ( id_or_name == c_field_name_Description ) )
   {
      name = string( c_field_display_name_Description );
      get_module_string( c_field_display_name_Description, &next );
   }
   if( ( id_or_name == c_field_id_Extra_Value ) || ( id_or_name == c_field_name_Extra_Value ) )
   {
      name = string( c_field_display_name_Extra_Value );
      get_module_string( c_field_display_name_Extra_Value, &next );
   }
   if( ( id_or_name == c_field_id_Host_Name ) || ( id_or_name == c_field_name_Host_Name ) )
   {
      name = string( c_field_display_name_Host_Name );
      get_module_string( c_field_display_name_Host_Name, &next );
   }
   if( ( id_or_name == c_field_id_Host_Port ) || ( id_or_name == c_field_name_Host_Port ) )
   {
      name = string( c_field_display_name_Host_Port );
      get_module_string( c_field_display_name_Host_Port, &next );
   }
   if( ( id_or_name == c_field_id_Local_Port ) || ( id_or_name == c_field_name_Local_Port ) )
   {
      name = string( c_field_display_name_Local_Port );
      get_module_string( c_field_display_name_Local_Port, &next );
   }
   if( ( id_or_name == c_field_id_Num_Helpers ) || ( id_or_name == c_field_name_Num_Helpers ) )
   {
      name = string( c_field_display_name_Num_Helpers );
      get_module_string( c_field_display_name_Num_Helpers, &next );
   }
   if( ( id_or_name == c_field_id_Peer_Type ) || ( id_or_name == c_field_name_Peer_Type ) )
   {
      name = string( c_field_display_name_Peer_Type );
      get_module_string( c_field_display_name_Peer_Type, &next );
   }
   if( ( id_or_name == c_field_id_Port_Numbers ) || ( id_or_name == c_field_name_Port_Numbers ) )
   {
      name = string( c_field_display_name_Port_Numbers );
      get_module_string( c_field_display_name_Port_Numbers, &next );
   }
   if( ( id_or_name == c_field_id_Shared_Secret ) || ( id_or_name == c_field_name_Shared_Secret ) )
   {
      name = string( c_field_display_name_Shared_Secret );
      get_module_string( c_field_display_name_Shared_Secret, &next );
   }
   if( ( id_or_name == c_field_id_Status ) || ( id_or_name == c_field_name_Status ) )
   {
      name = string( c_field_display_name_Status );
      get_module_string( c_field_display_name_Status, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Global_Peerchain_Entry::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
      display_name = get_module_string( c_field_display_name_Actions );
   if( ( id_or_name == c_field_id_Auto_Start ) || ( id_or_name == c_field_name_Auto_Start ) )
      display_name = get_module_string( c_field_display_name_Auto_Start );
   if( ( id_or_name == c_field_id_Chain_Id ) || ( id_or_name == c_field_name_Chain_Id ) )
      display_name = get_module_string( c_field_display_name_Chain_Id );
   if( ( id_or_name == c_field_id_Description ) || ( id_or_name == c_field_name_Description ) )
      display_name = get_module_string( c_field_display_name_Description );
   if( ( id_or_name == c_field_id_Extra_Value ) || ( id_or_name == c_field_name_Extra_Value ) )
      display_name = get_module_string( c_field_display_name_Extra_Value );
   if( ( id_or_name == c_field_id_Host_Name ) || ( id_or_name == c_field_name_Host_Name ) )
      display_name = get_module_string( c_field_display_name_Host_Name );
   if( ( id_or_name == c_field_id_Host_Port ) || ( id_or_name == c_field_name_Host_Port ) )
      display_name = get_module_string( c_field_display_name_Host_Port );
   if( ( id_or_name == c_field_id_Local_Port ) || ( id_or_name == c_field_name_Local_Port ) )
      display_name = get_module_string( c_field_display_name_Local_Port );
   if( ( id_or_name == c_field_id_Num_Helpers ) || ( id_or_name == c_field_name_Num_Helpers ) )
      display_name = get_module_string( c_field_display_name_Num_Helpers );
   if( ( id_or_name == c_field_id_Peer_Type ) || ( id_or_name == c_field_name_Peer_Type ) )
      display_name = get_module_string( c_field_display_name_Peer_Type );
   if( ( id_or_name == c_field_id_Port_Numbers ) || ( id_or_name == c_field_name_Port_Numbers ) )
      display_name = get_module_string( c_field_display_name_Port_Numbers );
   if( ( id_or_name == c_field_id_Shared_Secret ) || ( id_or_name == c_field_name_Shared_Secret ) )
      display_name = get_module_string( c_field_display_name_Shared_Secret );
   if( ( id_or_name == c_field_id_Status ) || ( id_or_name == c_field_name_Status ) )
      display_name = get_module_string( c_field_display_name_Status );

   return display_name;
}

void Meta_Global_Peerchain_Entry::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Global_Peerchain_Entry::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Global_Peerchain_Entry::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Global_Peerchain_Entry::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

size_t Meta_Global_Peerchain_Entry::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Global_Peerchain_Entry::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Global_Peerchain_Entry::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 0;

   if( !is_internal )
   {
      g_external_aliases_lookup.clear( );

      for( external_aliases_const_iterator
       eaci = g_external_aliases.begin( ), end = g_external_aliases.end( ); eaci != end; ++eaci )
      {
         size_t num_extra = eaci->second->get_num_foreign_key_children( true );

         if( num_extra )
         {
            eaci->second->set_key( get_key( ) );
            eaci->second->copy_all_field_values( *this );

            g_external_aliases_lookup.insert( make_pair( rc, eaci->second ) );

            rc += num_extra;
         }
      }
   }

   set_total_child_relationships( rc );

   return rc;
}

class_base* Meta_Global_Peerchain_Entry::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   ( void )child_num;
   ( void )next_child_field;
   ( void )op;

   return p_class_base;
}

void Meta_Global_Peerchain_Entry::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Global_Peerchain_Entry::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Global_Peerchain_Entry::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Global_Peerchain_Entry::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Global_Peerchain_Entry::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Global_Peerchain_Entry::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Global_Peerchain_Entry::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Global_Peerchain_Entry::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "global_peerchain_entry";

   return get_module_string( key );
}

string Meta_Global_Peerchain_Entry::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Global_Peerchain_Entry::get_create_instance_info( ) const
{
   return "";
}

string Meta_Global_Peerchain_Entry::get_update_instance_info( ) const
{
   return "";
}

string Meta_Global_Peerchain_Entry::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Global_Peerchain_Entry::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "145410" ) // i.e. Connect
      retval = "";
   else if( procedure_id == "145420" ) // i.e. Disconnect
      retval = "";
   else if( procedure_id == "145440" ) // i.e. Finish
      retval = "";
   else if( procedure_id == "145430" ) // i.e. Start
      retval = "";

   return retval;
}

bool Meta_Global_Peerchain_Entry::get_is_alias( ) const
{
   return false;
}

void Meta_Global_Peerchain_Entry::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Global_Peerchain_Entry::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Global_Peerchain_Entry::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Global_Peerchain_Entry::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   throw runtime_error( "invalid get_sql_column_names call for non-SQL class: " + to_string( static_class_name( ) ) );
}

void Meta_Global_Peerchain_Entry::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   throw runtime_error( "invalid get_sql_column_values call for non-SQL class: " + to_string( static_class_name( ) ) );
}

void Meta_Global_Peerchain_Entry::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Global_Peerchain_Entry::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600098a
   dependents.insert( "Status" ); // (for Is_Halted modifier)

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [(finish modifier_field_value)] 600098a

   // [(start modifier_field_value)] 600098e
   dependents.insert( "Status" ); // (for Is_Connected modifier)

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [(finish modifier_field_value)] 600098e

   // [(start modifier_field_value)] 600098f
   dependents.insert( "Status" ); // (for Is_Connecting modifier)

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [(finish modifier_field_value)] 600098f

   // [(start modifier_field_value)] 600098g
   dependents.insert( "Status" ); // (for Is_Connecting modifier)

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [(finish modifier_field_value)] 600098g

   // [(start protect_equal)] 600098h
   dependents.insert( "Status" );

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [(finish protect_equal)] 600098h

   // [(start protect_equal)] 600098i
   dependents.insert( "Status" );

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [(finish protect_equal)] 600098i

   // [(start modifier_field_value)] 600098j
   dependents.insert( "Status" ); // (for Not_Is_Halted modifier)

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [(finish modifier_field_value)] 600098j

   // [(start protect_not_equal)] 600098k
   dependents.insert( "Status" );

   if( ( use_transients && is_field_transient( e_field_id_Status ) )
    || ( !use_transients && !is_field_transient( e_field_id_Status ) ) )
      names.insert( "Status" );
   // [(finish protect_not_equal)] 600098k

   // [(start modifier_field_value)] 600898b
   dependents.insert( "Peer_Type" ); // (for Is_Local_Only modifier)

   if( ( use_transients && is_field_transient( e_field_id_Peer_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Peer_Type ) ) )
      names.insert( "Peer_Type" );
   // [(finish modifier_field_value)] 600898b

   // [(start modifier_field_value)] 600898c
   dependents.insert( "Peer_Type" ); // (for Is_Backup_Only modifier)

   if( ( use_transients && is_field_transient( e_field_id_Peer_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Peer_Type ) ) )
      names.insert( "Peer_Type" );
   // [(finish modifier_field_value)] 600898c

   // [(start modifier_field_value)] 600898d
   dependents.insert( "Peer_Type" ); // (for Is_Hub modifier)

   if( ( use_transients && is_field_transient( e_field_id_Peer_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Peer_Type ) ) )
      names.insert( "Peer_Type" );
   // [(finish modifier_field_value)] 600898d

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Global_Peerchain_Entry::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Global_Peerchain_Entry::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   throw runtime_error( "invalid do_generate_sql call for non-SQL class: " + to_string( static_class_name( ) ) );
}

const char* Meta_Global_Peerchain_Entry::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Global_Peerchain_Entry::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Global_Peerchain_Entry::static_lock_class_id( )
{
   return "145100";
}

const char* Meta_Global_Peerchain_Entry::static_check_class_name( )
{
   return "Global_Peerchain_Entry";
}

const char* Meta_Global_Peerchain_Entry::static_persistence_extra( )
{
   return "peer!";
}

bool Meta_Global_Peerchain_Entry::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Global_Peerchain_Entry::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.145100" );
}

void Meta_Global_Peerchain_Entry::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "145111", "Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "145107", "Auto_Start", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "145101", "Chain_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "145102", "Description", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "145110", "Extra_Value", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "145104", "Host_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "145105", "Host_Port", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "145106", "Local_Port", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "145109", "Num_Helpers", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "145103", "Peer_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "145112", "Port_Numbers", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "145113", "Shared_Secret", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "145108", "Status", "int", false, "", "" ) );
}

void Meta_Global_Peerchain_Entry::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;
}

int Meta_Global_Peerchain_Entry::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Global_Peerchain_Entry::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Global_Peerchain_Entry::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Global_Peerchain_Entry::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "145111";
      break;

      case 2:
      p_id = "145107";
      break;

      case 3:
      p_id = "145101";
      break;

      case 4:
      p_id = "145102";
      break;

      case 5:
      p_id = "145110";
      break;

      case 6:
      p_id = "145104";
      break;

      case 7:
      p_id = "145105";
      break;

      case 8:
      p_id = "145106";
      break;

      case 9:
      p_id = "145109";
      break;

      case 10:
      p_id = "145103";
      break;

      case 11:
      p_id = "145112";
      break;

      case 12:
      p_id = "145113";
      break;

      case 13:
      p_id = "145108";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Global_Peerchain_Entry" );

   return p_id;
}

const char* Meta_Global_Peerchain_Entry::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Actions";
      break;

      case 2:
      p_id = "Auto_Start";
      break;

      case 3:
      p_id = "Chain_Id";
      break;

      case 4:
      p_id = "Description";
      break;

      case 5:
      p_id = "Extra_Value";
      break;

      case 6:
      p_id = "Host_Name";
      break;

      case 7:
      p_id = "Host_Port";
      break;

      case 8:
      p_id = "Local_Port";
      break;

      case 9:
      p_id = "Num_Helpers";
      break;

      case 10:
      p_id = "Peer_Type";
      break;

      case 11:
      p_id = "Port_Numbers";
      break;

      case 12:
      p_id = "Shared_Secret";
      break;

      case 13:
      p_id = "Status";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Global_Peerchain_Entry" );

   return p_id;
}

int Meta_Global_Peerchain_Entry::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( ( field == c_field_id_Actions ) || ( field == c_field_name_Actions ) )
      rc += 1;
   else if( ( field == c_field_id_Auto_Start ) || ( field == c_field_name_Auto_Start ) )
      rc += 2;
   else if( ( field == c_field_id_Chain_Id ) || ( field == c_field_name_Chain_Id ) )
      rc += 3;
   else if( ( field == c_field_id_Description ) || ( field == c_field_name_Description ) )
      rc += 4;
   else if( ( field == c_field_id_Extra_Value ) || ( field == c_field_name_Extra_Value ) )
      rc += 5;
   else if( ( field == c_field_id_Host_Name ) || ( field == c_field_name_Host_Name ) )
      rc += 6;
   else if( ( field == c_field_id_Host_Port ) || ( field == c_field_name_Host_Port ) )
      rc += 7;
   else if( ( field == c_field_id_Local_Port ) || ( field == c_field_name_Local_Port ) )
      rc += 8;
   else if( ( field == c_field_id_Num_Helpers ) || ( field == c_field_name_Num_Helpers ) )
      rc += 9;
   else if( ( field == c_field_id_Peer_Type ) || ( field == c_field_name_Peer_Type ) )
      rc += 10;
   else if( ( field == c_field_id_Port_Numbers ) || ( field == c_field_name_Port_Numbers ) )
      rc += 11;
   else if( ( field == c_field_id_Shared_Secret ) || ( field == c_field_name_Shared_Secret ) )
      rc += 12;
   else if( ( field == c_field_id_Status ) || ( field == c_field_name_Status ) )
      rc += 13;

   return rc - 1;
}

procedure_info_container& Meta_Global_Peerchain_Entry::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "145410", procedure_info( "Connect" ) ) );
      procedures.insert( make_pair( "145420", procedure_info( "Disconnect" ) ) );
      procedures.insert( make_pair( "145440", procedure_info( "Finish" ) ) );
      procedures.insert( make_pair( "145430", procedure_info( "Start" ) ) );
   }

   return procedures;
}

string Meta_Global_Peerchain_Entry::static_get_sql_columns( )
{
   throw runtime_error( "invalid static_get_sql_columns call for non-SQL class: " + to_string( static_class_name( ) ) );
}

void Meta_Global_Peerchain_Entry::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Global_Peerchain_Entry::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_peerchain_peer_type_-2", get_enum_string_peerchain_peer_type( -2 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_peer_type_-1", get_enum_string_peerchain_peer_type( -1 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_peer_type_0", get_enum_string_peerchain_peer_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_peer_type_1", get_enum_string_peerchain_peer_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_peer_type_2", get_enum_string_peerchain_peer_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_peer_type_3", get_enum_string_peerchain_peer_type( 3 ) ) );

   pairs.push_back( make_pair( "enum_peerchain_status_0", get_enum_string_peerchain_status( 0 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_status_1", get_enum_string_peerchain_status( 1 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_status_2", get_enum_string_peerchain_status( 2 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_status_3", get_enum_string_peerchain_status( 3 ) ) );
   pairs.push_back( make_pair( "enum_peerchain_status_4", get_enum_string_peerchain_status( 4 ) ) );
}

void Meta_Global_Peerchain_Entry::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Global_Peerchain_Entry::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   ( void )unique_indexes;
}

void Meta_Global_Peerchain_Entry::static_get_sql_indexes( vector< string >& indexes )
{
   throw runtime_error( "invalid static_get_sql_indexes call for non-SQL class: " + to_string( static_class_name( ) ) );
}

void Meta_Global_Peerchain_Entry::static_get_sql_unique_indexes( vector< string >& indexes )
{
   throw runtime_error( "invalid static_get_sql_unique_indexes call for non-SQL class: " + to_string( static_class_name( ) ) );
}

void Meta_Global_Peerchain_Entry::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Global_Peerchain_Entry::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Global_Peerchain_Entry::static_insert_external_alias( const string& module_and_class_id, Meta_Global_Peerchain_Entry* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Global_Peerchain_Entry::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Global_Peerchain_Entry::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_peerchain_peer_type_enum.insert( -2 );
   g_peerchain_peer_type_enum.insert( -1 );
   g_peerchain_peer_type_enum.insert( 0 );
   g_peerchain_peer_type_enum.insert( 1 );
   g_peerchain_peer_type_enum.insert( 2 );
   g_peerchain_peer_type_enum.insert( 3 );

   g_peerchain_status_enum.insert( 0 );
   g_peerchain_status_enum.insert( 1 );
   g_peerchain_status_enum.insert( 2 );
   g_peerchain_status_enum.insert( 3 );
   g_peerchain_status_enum.insert( 4 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Global_Peerchain_Entry::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

// Copyright (c) 2008
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
#  include <cstring>
#  include <fstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#define CIYAM_BASE_LIB
#define MODULE_META_IMPL

// [<start macros>]
// [<finish macros>]

#include "Meta_Initial_Record.h"

#include "Meta_Initial_Record_Value.h"
#include "Meta_Class.h"

#include "ciyam_base.h"
#include "class_domains.h"
#include "module_strings.h"
#include "class_utilities.h"
#include "command_handler.h"
#include "module_interface.h"

// [(start copy_child_links_from_fk)]
#include "Meta_Field.h"
// [(finish copy_child_links_from_fk)]

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline int system( const string& cmd ) { return exec_system( cmd ); }

namespace
{

#include "Meta_Initial_Record.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Class = "302410";
const char* const c_field_id_Comments = "129102";
const char* const c_field_id_Key = "129101";
const char* const c_field_id_Order = "129103";

const char* const c_field_name_Class = "Class";
const char* const c_field_name_Comments = "Comments";
const char* const c_field_name_Key = "Key";
const char* const c_field_name_Order = "Order";

const char* const c_field_display_name_Class = "field_initial_record_class";
const char* const c_field_display_name_Comments = "field_initial_record_comments";
const char* const c_field_display_name_Key = "field_initial_record_key";
const char* const c_field_display_name_Order = "field_initial_record_order";

const int c_num_fields = 4;

const char* const c_all_sorted_field_ids[ ] =
{
   "129101",
   "129102",
   "129103",
   "302410"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Class",
   "Comments",
   "Key",
   "Order"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 0;

bool is_transient_field( const string& ) { static bool false_value( false ); return false_value; }

const char* const c_procedure_id_Move_Down = "130420";
const char* const c_procedure_id_Move_Up = "130410";

domain_string_max_size< 100 > g_Comments_domain;
aggregate_domain< string,
 domain_string_label_format,
 domain_string_max_size< 30 > > g_Key_domain;

string g_order_field_name( "Order" );

set< string > g_derivations;

set< string > g_file_field_names;

typedef map< string, Meta_Initial_Record* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Initial_Record* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

string gv_default_Class = string( );
string gv_default_Comments = string( );
string gv_default_Key = string( );
string gv_default_Order = string( );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Initial_Record > Initial_Record_registration( get_class_registry( ), "129100" );

class Meta_Initial_Record_command_functor;

class Meta_Initial_Record_command_handler : public command_handler
{
   friend class Meta_Initial_Record_command_functor;

   public:
   Meta_Initial_Record_command_handler( ) : p_Meta_Initial_Record( 0 ) { }

   void set_Meta_Initial_Record( Meta_Initial_Record* p_new_Meta_Initial_Record ) { p_Meta_Initial_Record = p_new_Meta_Initial_Record; }

   void handle_unknown_command( const string& command )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Initial_Record* p_Meta_Initial_Record;

   protected:
   string retval;
};

class Meta_Initial_Record_command_functor : public command_functor
{
   public:
   Meta_Initial_Record_command_functor( Meta_Initial_Record_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Initial_Record_command_handler& cmd_handler;
};

command_functor* Meta_Initial_Record_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Initial_Record_command_functor( dynamic_cast< Meta_Initial_Record_command_handler& >( handler ) );
}

void Meta_Initial_Record_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Initial_Record_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Initial_Record_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Initial_Record->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Initial_Record->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Initial_Record_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Initial_Record->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Initial_Record->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Initial_Record_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Initial_Record_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Class || field_name == c_field_name_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Initial_Record->Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Comments || field_name == c_field_name_Comments )
         string_getter< string >( cmd_handler.p_Meta_Initial_Record->Comments( ), cmd_handler.retval );
      else if( field_name == c_field_id_Key || field_name == c_field_name_Key )
         string_getter< string >( cmd_handler.p_Meta_Initial_Record->Key( ), cmd_handler.retval );
      else if( field_name == c_field_id_Order || field_name == c_field_name_Order )
         string_getter< string >( cmd_handler.p_Meta_Initial_Record->Order( ), cmd_handler.retval );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Initial_Record_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Initial_Record_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Initial_Record_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Class || field_name == c_field_name_Class )
         func_string_setter< Meta_Initial_Record, Meta_Class >(
          *cmd_handler.p_Meta_Initial_Record, &Meta_Initial_Record::Class, field_value );
      else if( field_name == c_field_id_Comments || field_name == c_field_name_Comments )
         func_string_setter< Meta_Initial_Record, string >(
          *cmd_handler.p_Meta_Initial_Record, &Meta_Initial_Record::Comments, field_value );
      else if( field_name == c_field_id_Key || field_name == c_field_name_Key )
         func_string_setter< Meta_Initial_Record, string >(
          *cmd_handler.p_Meta_Initial_Record, &Meta_Initial_Record::Key, field_value );
      else if( field_name == c_field_id_Order || field_name == c_field_name_Order )
         func_string_setter< Meta_Initial_Record, string >(
          *cmd_handler.p_Meta_Initial_Record, &Meta_Initial_Record::Order, field_value );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Initial_Record_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Initial_Record_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Initial_Record_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Class || field_name == c_field_name_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Initial_Record->Class( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Initial_Record_Move_Down )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_parm_Meta_Initial_Record_Move_Down_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_parm_Meta_Initial_Record_Move_Down_Restrict_Values ) );
      cmd_handler.p_Meta_Initial_Record->Move_Down( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );
   }
   else if( command == c_cmd_Meta_Initial_Record_Move_Up )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_parm_Meta_Initial_Record_Move_Up_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_parm_Meta_Initial_Record_Move_Up_Restrict_Values ) );
      cmd_handler.p_Meta_Initial_Record->Move_Up( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );
   }
}

struct Meta_Initial_Record::impl : public Meta_Initial_Record_command_handler
{
   impl( Meta_Initial_Record& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Initial_Record( &o );

      add_commands( 0, Meta_Initial_Record_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Initial_Record_command_definitions ) );
   }

   Meta_Initial_Record& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Comments( ) const { return lazy_fetch( p_obj ), v_Comments; }
   void impl_Comments( const string& Comments ) { v_Comments = Comments; }

   const string& impl_Key( ) const { return lazy_fetch( p_obj ), v_Key; }
   void impl_Key( const string& Key ) { v_Key = Key; }

   const string& impl_Order( ) const { return lazy_fetch( p_obj ), v_Order; }
   void impl_Order( const string& Order ) { v_Order = Order; }

   Meta_Class& impl_Class( )
   {
      if( !cp_Class )
      {
         cp_Class.init( );

         p_obj->setup_graph_parent( *cp_Class, c_field_id_Class, v_Class );
      }
      return *cp_Class;
   }

   const Meta_Class& impl_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Class )
      {
         cp_Class.init( );

         p_obj->setup_graph_parent( *cp_Class, c_field_id_Class, v_Class );
      }
      return *cp_Class;
   }

   void impl_Class( const string& key )
   {
      class_base_accessor cba( impl_Class( ) );
      cba.set_key( key );
   }

   Meta_Initial_Record_Value& impl_child_Initial_Record_Value( )
   {
      if( !cp_child_Initial_Record_Value )
      {
         cp_child_Initial_Record_Value.init( );

         p_obj->setup_graph_parent( *cp_child_Initial_Record_Value, "302510" );
      }
      return *cp_child_Initial_Record_Value;
   }

   const Meta_Initial_Record_Value& impl_child_Initial_Record_Value( ) const
   {
      if( !cp_child_Initial_Record_Value )
      {
         cp_child_Initial_Record_Value.init( );

         p_obj->setup_graph_parent( *cp_child_Initial_Record_Value, "302510" );
      }
      return *cp_child_Initial_Record_Value;
   }

   void impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values );

   void impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values );

   string get_field_value( int field ) const;
   void set_field_value( int field, const string& value );

   uint64_t get_state( ) const;

   const string& execute( const string& cmd_and_args );

   void clear_foreign_key( const string& field );

   void set_foreign_key_value( const string& field, const string& value );

   const string& get_foreign_key_value( const string& field );

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   void add_extra_paging_info( vector< pair< string, string > >& paging_info ) const;

   void clear( );

   bool value_will_be_provided( const string& field_name );

   void validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors );
   void validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors );

   void after_fetch( );
   void finalise_fetch( );

   void at_create( );

   void to_store( bool is_create, bool is_internal );
   void for_store( bool is_create, bool is_internal );
   void after_store( bool is_create, bool is_internal );

   bool can_destroy( bool is_internal );
   void for_destroy( bool is_internal );
   void after_destroy( bool is_internal );

   void set_default_values( );

   bool is_filtered( ) const;

   void get_required_transients( ) const;

   Meta_Initial_Record* p_obj;
   class_pointer< Meta_Initial_Record > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Comments;
   string v_Key;
   string v_Order;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   mutable class_pointer< Meta_Initial_Record_Value > cp_child_Initial_Record_Value;
};

void Meta_Initial_Record::impl::impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   // [(start move_up_and_down)]
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_Initial_Record > cp_other( e_create_instance );

         int num_fixed = 0;
         if( !Restrict_Fields.empty( ) )
            num_fixed = split_count( Restrict_Fields );

         string key_info( Restrict_Fields );
         if( num_fixed )
            key_info += ',';
         key_info += string( c_field_id_Order );

         if( !num_fixed )
            key_info += ' ' + get_obj( ).Order( );
         else
         {
            key_info += '#' + to_string( num_fixed );
            key_info += ' ' + Restrict_Values + ',' + get_obj( ).Order( );
         }

         // NOTE: Only the first record is required so set the row limit to 1.
         if( cp_other->iterate_forwards( key_info, string( c_field_name_Order ), false, 1 ) )
         {
            string old_val( get_obj( ).Order( ) );
            string new_val( cp_other->Order( ) );

            get_obj( ).Order( gen_key( ) );
            get_obj( ).op_apply( );

            get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

            cp_other->op_update( cp_other->get_key( ), c_field_name_Order );
            cp_other->Order( old_val );
            cp_other->op_apply( );

            get_obj( ).Order( new_val );
            get_obj( ).op_apply( );

            cp_other->iterate_stop( );
         }
         else
            get_obj( ).op_cancel( );
      }

      transaction_commit( );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }
   // [(finish move_up_and_down)]

   // [<start Move_Down_impl>]
   // [<finish Move_Down_impl>]
}

void Meta_Initial_Record::impl::impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   // [(start move_up_and_down)]
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_Initial_Record > cp_other( e_create_instance );

         int num_fixed = 0;
         if( !Restrict_Fields.empty( ) )
            num_fixed = split_count( Restrict_Fields );

         string key_info( Restrict_Fields );
         if( num_fixed )
            key_info += ',';
         key_info += string( c_field_id_Order );

         if( !num_fixed )
            key_info += ' ' + get_obj( ).Order( );
         else
         {
            key_info += '#' + to_string( num_fixed );
            key_info += ' ' + Restrict_Values + ',' + get_obj( ).Order( );
         }

         // NOTE: Only the first record is required so set the row limit to 1.
         if( cp_other->iterate_backwards( key_info, string( c_field_name_Order ), false, 1 ) )
         {
            string old_val( get_obj( ).Order( ) );
            string new_val( cp_other->Order( ) );

            get_obj( ).Order( gen_key( ) );
            get_obj( ).op_apply( );

            get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

            cp_other->op_update( cp_other->get_key( ), c_field_name_Order );
            cp_other->Order( old_val );
            cp_other->op_apply( );

            get_obj( ).Order( new_val );
            get_obj( ).op_apply( );

            cp_other->iterate_stop( );
         }
         else
            get_obj( ).op_cancel( );
      }

      transaction_commit( );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }
   // [(finish move_up_and_down)]

   // [<start Move_Up_impl>]
   // [<finish Move_Up_impl>]
}

string Meta_Initial_Record::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Class( ) );
      break;

      case 1:
      retval = to_string( impl_Comments( ) );
      break;

      case 2:
      retval = to_string( impl_Key( ) );
      break;

      case 3:
      retval = to_string( impl_Order( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Initial_Record::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Initial_Record::impl, Meta_Class >( *this, &Meta_Initial_Record::impl::impl_Class, value );
      break;

      case 1:
      func_string_setter< Meta_Initial_Record::impl, string >( *this, &Meta_Initial_Record::impl::impl_Comments, value );
      break;

      case 2:
      func_string_setter< Meta_Initial_Record::impl, string >( *this, &Meta_Initial_Record::impl::impl_Key, value );
      break;

      case 3:
      func_string_setter< Meta_Initial_Record::impl, string >( *this, &Meta_Initial_Record::impl::impl_Order, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Initial_Record::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_Initial_Record::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Initial_Record::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Class || field == c_field_name_Class )
      impl_Class( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Initial_Record::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Class || field == c_field_name_Class )
      v_Class = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Initial_Record::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Class || field == c_field_name_Class )
      return v_Class;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Initial_Record::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
}

void Meta_Initial_Record::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Initial_Record::impl::clear( )
{
   v_Comments = gv_default_Comments;
   v_Key = gv_default_Key;
   v_Order = gv_default_Order;

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );
}

bool Meta_Initial_Record::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Initial_Record::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( is_null( v_Key ) && !value_will_be_provided( c_field_name_Key ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Key,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Key ) ) ) ) );

   if( is_null( v_Order ) && !value_will_be_provided( c_field_name_Order ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Order,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Order ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   string error_message;
   if( !is_null( v_Comments )
    && ( v_Comments != gv_default_Comments
    || !value_will_be_provided( c_field_name_Comments ) )
    && !g_Comments_domain.is_valid( v_Comments, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Comments,
       get_module_string( c_field_display_name_Comments ) + " " + error_message ) );

   if( !is_null( v_Key )
    && ( v_Key != gv_default_Key
    || !value_will_be_provided( c_field_name_Key ) )
    && !g_Key_domain.is_valid( v_Key, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Key,
       get_module_string( c_field_display_name_Key ) + " " + error_message ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Initial_Record::impl::validate_set_fields( set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;

   if( !is_null( v_Comments )
    && ( fields_set.count( c_field_id_Comments ) || fields_set.count( c_field_name_Comments ) )
    && !g_Comments_domain.is_valid( v_Comments, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Comments,
       get_module_string( c_field_display_name_Comments ) + " " + error_message ) );

   if( !is_null( v_Key )
    && ( fields_set.count( c_field_id_Key ) || fields_set.count( c_field_name_Key ) )
    && !g_Key_domain.is_valid( v_Key, error_message = "" ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Key,
       get_module_string( c_field_display_name_Key ) + " " + error_message ) );
}

void Meta_Initial_Record::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Initial_Record::impl::finalise_fetch( )
{

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Initial_Record::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Initial_Record::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_from_key)]
   if( !get_obj( ).get_clone_key( ).empty( ) || ( is_create && is_null( get_obj( ).Order( ) ) ) )
      get_obj( ).Order( get_obj( ).get_key( ) );
   // [(finish default_from_key)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Initial_Record::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Initial_Record::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [(start copy_child_links_from_fk)]
   if( is_create
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Class( ).child_Field( ).iterate_forwards( ) )
   {
      do
      {
         string child_key( get_obj( ).get_key( ) + "_" + get_obj( ).Class( ).child_Field( ).get_key( ) );

         get_obj( ).child_Initial_Record_Value( ).op_create( child_key );

         get_obj( ).child_Initial_Record_Value( ).Initial_Record( get_obj( ).get_key( ) );
         get_obj( ).child_Initial_Record_Value( ).Field( get_obj( ).Class( ).child_Field( ).get_key( ) );

         copy_field_or_file_and_field( get_obj( ).child_Initial_Record_Value( ), child_key,
          "Value", get_obj( ).Class( ).child_Field( ), "Default", storage_locked_for_admin( ) );

         get_obj( ).child_Initial_Record_Value( ).op_apply( );

      } while( get_obj( ).Class( ).child_Field( ).iterate_next( ) );
   }
   // [(finish copy_child_links_from_fk)]

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Initial_Record::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Initial_Record::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Initial_Record::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Initial_Record::impl::set_default_values( )
{
   clear( );
}

bool Meta_Initial_Record::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Initial_Record::impl::get_required_transients( ) const
{
   required_transients.clear( );

   set< string > dependents;
   p_obj->get_required_field_names( required_transients, true, &dependents );

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

      num_required = required_transients.size( );
   }
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Initial_Record::Meta_Initial_Record( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Initial_Record::~Meta_Initial_Record( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Initial_Record::Comments( ) const
{
   return p_impl->impl_Comments( );
}

void Meta_Initial_Record::Comments( const string& Comments )
{
   p_impl->impl_Comments( Comments );
}

const string& Meta_Initial_Record::Key( ) const
{
   return p_impl->impl_Key( );
}

void Meta_Initial_Record::Key( const string& Key )
{
   p_impl->impl_Key( Key );
}

const string& Meta_Initial_Record::Order( ) const
{
   return p_impl->impl_Order( );
}

void Meta_Initial_Record::Order( const string& Order )
{
   p_impl->impl_Order( Order );
}

Meta_Class& Meta_Initial_Record::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_Initial_Record::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_Initial_Record::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Initial_Record_Value& Meta_Initial_Record::child_Initial_Record_Value( )
{
   return p_impl->impl_child_Initial_Record_Value( );
}

const Meta_Initial_Record_Value& Meta_Initial_Record::child_Initial_Record_Value( ) const
{
   return p_impl->impl_child_Initial_Record_Value( );
}

void Meta_Initial_Record::Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Down( Restrict_Fields, Restrict_Values );
}

void Meta_Initial_Record::Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Up( Restrict_Fields, Restrict_Values );
}

string Meta_Initial_Record::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Initial_Record::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

bool Meta_Initial_Record::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Initial_Record::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Initial_Record::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Initial_Record::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Initial_Record::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Initial_Record::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Initial_Record::clear( )
{
   p_impl->clear( );
}

void Meta_Initial_Record::validate( unsigned state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Initial_Record::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Initial_Record::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Initial_Record::finalise_fetch( )
{
   p_impl->finalise_fetch( );
}

void Meta_Initial_Record::at_create( )
{
   p_impl->at_create( );
}

void Meta_Initial_Record::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Initial_Record::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Initial_Record::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Initial_Record::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Initial_Record::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Initial_Record::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Initial_Record::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Initial_Record::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Initial_Record::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Class )
   {
      p_id = c_field_id_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Comments )
   {
      p_id = c_field_id_Comments;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Key )
   {
      p_id = c_field_id_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Order )
   {
      p_id = c_field_id_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Initial_Record::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Class )
   {
      p_name = c_field_name_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Comments )
   {
      p_name = c_field_name_Comments;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Key )
   {
      p_name = c_field_name_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Order )
   {
      p_name = c_field_name_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Initial_Record::get_order_field_name( ) const
{
   return g_order_field_name;
}

bool Meta_Initial_Record::is_file_field_name( const string& name ) const
{
   return g_file_field_names.count( name );
}

void Meta_Initial_Record::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Initial_Record::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   else if( id_or_name == c_field_id_Class || id_or_name == c_field_name_Class )
      display_name = get_module_string( c_field_display_name_Class );
   else if( id_or_name == c_field_id_Comments || id_or_name == c_field_name_Comments )
      display_name = get_module_string( c_field_display_name_Comments );
   else if( id_or_name == c_field_id_Key || id_or_name == c_field_name_Key )
      display_name = get_module_string( c_field_display_name_Key );
   else if( id_or_name == c_field_id_Order || id_or_name == c_field_name_Order )
      display_name = get_module_string( c_field_display_name_Order );

   return display_name;
}

void Meta_Initial_Record::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Initial_Record::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Initial_Record::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Initial_Record::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Initial_Record::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Initial_Record::setup_graph_parent( Meta_Initial_Record_Value& o, const string& foreign_key_field )
{
   static_cast< Meta_Initial_Record_Value& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Initial_Record::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

size_t Meta_Initial_Record::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Initial_Record::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Initial_Record::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 1;

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

class_base* Meta_Initial_Record::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 1 )
   {
      external_aliases_lookup_const_iterator ealci = g_external_aliases_lookup.lower_bound( child_num );
      if( ealci == g_external_aliases_lookup.end( ) || ealci->first > child_num )
         --ealci;

      p_class_base = ealci->second->get_next_foreign_key_child( child_num - ealci->first, next_child_field, op, true );
   }
   else
   {
      switch( child_num )
      {
         case 0:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302510";
            p_class_base = &child_Initial_Record_Value( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Initial_Record::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

const char* Meta_Initial_Record::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Initial_Record::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Initial_Record::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Initial_Record::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Initial_Record::module_name( ) const
{
   return static_module_name( );
}

string Meta_Initial_Record::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "initial_record";

   return get_module_string( key );
}

bool Meta_Initial_Record::get_is_alias( ) const
{
   return false;
}

void Meta_Initial_Record::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Initial_Record::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Initial_Record::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == "_302510" || sub_context == "child_Initial_Record_Value" )
      p_class_base = &child_Initial_Record_Value( );
   else if( sub_context == c_field_id_Class || sub_context == c_field_name_Class )
      p_class_base = &Class( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Initial_Record::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Class" );
   names.push_back( "C_Comments" );
   names.push_back( "C_Key" );
   names.push_back( "C_Order" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Initial_Record::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( sql_quote( to_string( Comments( ) ) ) );
   values.push_back( sql_quote( to_string( Key( ) ) ) );
   values.push_back( sql_quote( to_string( Order( ) ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Initial_Record::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Initial_Record::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start move_up_and_down)]
   dependents.insert( "Order" );

   if( ( use_transients && is_field_transient( e_field_id_Order ) )
    || ( !use_transients && !is_field_transient( e_field_id_Order ) ) )
      names.insert( "Order" );
   // [(finish move_up_and_down)]

   // [<start get_always_required_field_names>]


   // [<finish get_always_required_field_names>]
}

void Meta_Initial_Record::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Initial_Record::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Initial_Record::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Initial_Record::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Initial_Record::static_lock_class_id( )
{
   return "129100";
}

const char* Meta_Initial_Record::static_check_class_name( )
{
   return "Initial_Record";
}

bool Meta_Initial_Record::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Initial_Record::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.129100" );
}

void Meta_Initial_Record::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "302410", "Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "129102", "Comments", "string", false ) );
   all_field_info.push_back( field_info( "129101", "Key", "string", false ) );
   all_field_info.push_back( field_info( "129103", "Order", "string", false ) );
}

void Meta_Initial_Record::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.129100", "Meta_Class" ) ) );
}

int Meta_Initial_Record::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Initial_Record::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Initial_Record::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "302410";
      break;

      case 2:
      p_id = "129102";
      break;

      case 3:
      p_id = "129101";
      break;

      case 4:
      p_id = "129103";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Initial_Record" );

   return p_id;
}

const char* Meta_Initial_Record::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Class";
      break;

      case 2:
      p_id = "Comments";
      break;

      case 3:
      p_id = "Key";
      break;

      case 4:
      p_id = "Order";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Initial_Record" );

   return p_id;
}

int Meta_Initial_Record::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 1;
   else if( field == c_field_id_Comments || field == c_field_name_Comments )
      rc += 2;
   else if( field == c_field_id_Key || field == c_field_name_Key )
      rc += 3;
   else if( field == c_field_id_Order || field == c_field_name_Order )
      rc += 4;

   return rc - 1;
}

procedure_info_container& Meta_Initial_Record::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "130420", "Move_Down" ) );
      procedures.insert( make_pair( "130410", "Move_Up" ) );
   }

   return procedures;
}

string Meta_Initial_Record::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Class VARCHAR(64) NOT NULL,"
    "C_Comments VARCHAR(200) NOT NULL,"
    "C_Key VARCHAR(200) NOT NULL,"
    "C_Order VARCHAR(200) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Initial_Record::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Initial_Record::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Initial_Record::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Order" );
}

void Meta_Initial_Record::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Order" );
}

void Meta_Initial_Record::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Initial_Record::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Initial_Record::static_insert_external_alias( const string& module_and_class_id, Meta_Initial_Record* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Initial_Record::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Initial_Record::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Initial_Record::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}


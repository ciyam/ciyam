// Copyright (c) 2004
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

#ifndef MODULE_INTERFACE_H
#  define MODULE_INTERFACE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <vector>
#     include <string>
#  endif

#  include "macros.h"
#  include "class_base.h"

#  ifdef MODULE_INTERFACE_IMPL
#     define MODULE_INTERFACE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define MODULE_INTERFACE_DECL_SPEC DYNAMIC_IMPORT
#  endif

#  define GMS( s ) get_module_string( STRINGIZE( s ) )

struct module_details
{
   const char* p_id;
   const char* p_ver;
   const char* p_name;
};

typedef std::map< std::string, std::string > module_strings_container;

typedef module_strings_container::value_type module_strings_value_type;
typedef module_strings_container::const_iterator module_strings_const_iterator;

struct registration_base
{
   virtual void class_init( const char* p_name ) = 0;
   virtual void class_term( const char* p_name ) = 0;

   virtual const char* class_id( ) const = 0;
   virtual const char* class_name( ) const = 0;

   virtual const char* module_id( ) const = 0;
   virtual const char* module_name( ) const = 0;

   virtual const char* resolved_module_id( ) const = 0;
   virtual const char* resolved_module_name( ) const = 0;

   virtual bool has_derivations( ) const = 0;

   virtual void get_class_info( class_info_container& class_info ) = 0;
   virtual void get_field_info( field_info_container& field_info ) = 0;
   virtual void get_foreign_key_info( foreign_key_info_container& foreign_key_info ) = 0;

   virtual const procedure_info_container& get_procedure_info( ) = 0;

   virtual std::string get_sql_columns( ) = 0;
   virtual void get_sql_indexes( std::vector< std::string >& indexes ) = 0;

   virtual class_base* create_instance( ) const = 0;
   virtual void destroy_instance( class_base* ) const = 0;
};

typedef std::map< std::string, registration_base* > class_registry_container;

typedef class_registry_container::value_type class_registry_value_type;
typedef class_registry_container::const_iterator class_registry_const_iterator;

module_strings_container& get_module_strings( );
class_registry_container& get_class_registry( );

std::string get_module_string( const std::string& key );

template< typename T > struct registration : registration_base
{
   registration( class_registry_container& registry, const char* p_name )
   {
      registry.insert( class_registry_value_type( p_name, this ) );
   }

   void class_init( const char* p_name ) { T::static_class_init( p_name ); }
   void class_term( const char* p_name ) { T::static_class_term( p_name ); }

   const char* class_id( ) const { return T::static_class_id( ); }
   const char* class_name( ) const { return T::static_class_name( ); }

   const char* module_id( ) const { return T::static_module_id( ); }
   const char* module_name( ) const { return T::static_module_name( ); }

   const char* resolved_module_id( ) const { return T::static_resolved_module_id( ); }
   const char* resolved_module_name( ) const { return T::static_resolved_module_name( ); }

   bool has_derivations( ) const { return T::static_has_derivations( ); }

   const procedure_info_container& get_procedure_info( ) { return T::static_get_procedure_info( ); }

   void get_class_info( class_info_container& class_info ) { T::static_get_class_info( class_info ); }
   void get_field_info( field_info_container& field_info ) { T::static_get_field_info( field_info ); }
   void get_foreign_key_info( foreign_key_info_container& foreign_key_info ) { T::static_get_foreign_key_info( foreign_key_info ); }

   std::string get_sql_columns( ) { return T::static_get_sql_columns( ); }
   void get_sql_indexes( std::vector< std::string >& indexes ) { T::static_get_sql_indexes( indexes ); }

   class_base* create_instance( ) const { return new T; }
   void destroy_instance( class_base* p_t ) const { delete p_t; }
};

class command_functor;
class command_handler;
struct command_definition;
typedef command_functor* command_functor_creator( const std::string& name, command_handler& handler );

typedef std::vector< std::string > externals_container;

extern "C" bool MODULE_INTERFACE_DECL_SPEC load_strings( const char* p_name );
extern "C" void MODULE_INTERFACE_DECL_SPEC init_classes( const char* p_name );
extern "C" void MODULE_INTERFACE_DECL_SPEC term_classes( const char* p_name );
extern "C" void MODULE_INTERFACE_DECL_SPEC obtain_externals( externals_container*& p_externals );
extern "C" void MODULE_INTERFACE_DECL_SPEC create_class_object( const char* p_name, class_base*& p_object );
extern "C" void MODULE_INTERFACE_DECL_SPEC destroy_class_object( const char* p_name, class_base*& p_object );
extern "C" void MODULE_INTERFACE_DECL_SPEC obtain_class_registry( const class_registry_container*& p_class_registry );
extern "C" void MODULE_INTERFACE_DECL_SPEC obtain_module_details( const module_details*& p_info );
extern "C" void MODULE_INTERFACE_DECL_SPEC obtain_module_strings( const module_strings_container*& p_module_strings );
extern "C" void MODULE_INTERFACE_DECL_SPEC obtain_module_commands(
 command_functor_creator*& p_functor_creator, command_definition*& p_definitions, size_t* p_num_commands );

typedef bool ( *fp_load_strings_func )( const char* p_name );
typedef void ( *fp_init_classes_func )( const char* p_name );
typedef void ( *fp_term_classes_func )( const char* p_name );
typedef void ( *fp_obtain_externals_func )( externals_container*& p_externals );
typedef void ( *fp_create_class_object_func )( const char* p_name, class_base*& p_object );
typedef void ( *fp_destroy_class_object_func )( const char* p_name, class_base*& p_object );
typedef void ( *fp_obtain_class_registry_func )( const class_registry_container*& p_class_registry );
typedef void ( *fp_obtain_module_details_func )( const module_details*& p_info );
typedef void ( *fp_obtain_module_strings_func )( const module_strings_container*& p_module_strings );
typedef void ( *fp_obtain_module_commands_func )(
 command_functor_creator*& p_functor_creator, command_definition*& p_definitions, size_t* p_num_commands );

#  ifndef __BORLANDC__
const char* const c_load_strings_func_name = "load_strings";
const char* const c_init_classes_func_name = "init_classes";
const char* const c_term_classes_func_name = "term_classes";
const char* const c_obtain_externals_func_name = "obtain_externals";
const char* const c_create_class_object_func_name = "create_class_object";
const char* const c_destroy_class_object_func_name = "destroy_class_object";
const char* const c_obtain_class_registry_func_name = "obtain_class_registry";
const char* const c_obtain_module_details_func_name = "obtain_module_details";
const char* const c_obtain_module_strings_func_name = "obtain_module_strings";
const char* const c_obtain_module_commands_func_name = "obtain_module_commands";
#  else
const char* const c_load_strings_func_name = "_load_strings";
const char* const c_init_classes_func_name = "_init_classes";
const char* const c_term_classes_func_name = "_term_classes";
const char* const c_obtain_externals_func_name = "_obtain_externals";
const char* const c_create_class_object_func_name = "_create_class_object";
const char* const c_destroy_class_object_func_name = "_destroy_class_object";
const char* const c_obtain_class_registry_func_name = "_obtain_class_registry";
const char* const c_obtain_module_details_func_name = "_obtain_module_details";
const char* const c_obtain_module_strings_func_name = "_obtain_module_strings";
const char* const c_obtain_module_commands_func_name = "_obtain_module_commands";
#  endif

#endif


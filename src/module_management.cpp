// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <memory>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#ifdef __GNUG__
#  define _access access
#endif

#include "module_management.h"

#include "threads.h"
#include "class_base.h"
#include "dynamic_library.h"
#include "module_interface.h"

using namespace std;

namespace
{

struct module_library_info
{
   module_library_info( const module_details* p_details, dynamic_library* p_dynamic_library,
    fp_term_classes fp_term_classes_func, fp_obtain_externals fp_obtain_externals_func )
    :
    ref_count( 1 ),
    p_details( p_details ),
    p_dynamic_library( p_dynamic_library ),
    fp_term_classes_func( fp_term_classes_func ),
    fp_obtain_externals_func( fp_obtain_externals_func )
   {
   }

   size_t ref_count;
   const module_details* p_details;
   dynamic_library* p_dynamic_library;

   fp_term_classes fp_term_classes_func;
   fp_obtain_externals fp_obtain_externals_func;

   map< string, string > class_ids;
   map< string, string > class_names;

   map< string, map< string, string > > class_field_ids;
   map< string, map< string, string > > class_field_names;
};

typedef map< string, module_library_info > module_container;

typedef module_container::iterator module_iterator;
typedef module_container::const_iterator module_const_iterator;

typedef module_container::value_type module_value_type;

mutex g_mutex;

module_container g_modules;

map< string, string > g_module_ids;
map< string, string > g_module_names;

inline string module_id_from_id_or_name( const string& module_id_or_name )
{
   string module_id( module_id_or_name );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   if( g_module_names.count( directory + module_id_or_name ) )
      module_id = g_module_names[ directory + module_id_or_name ];

   return module_id;
}

inline string module_name_from_id_or_name( const string& module_id_or_name )
{
   string module_name( module_id_or_name );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   if( g_module_ids.count( directory + module_id_or_name ) )
      module_name = g_module_ids[ directory + module_id_or_name ];

   return module_name;
}

module_class_list_error list_module_classes_impl(
 const string& module_id_or_name, ostream* p_os, vector< string >* p_lst,
 map< string, string >* p_map, bool key_by_id, int type = -1, const char* p_pat = 0 )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      return e_module_class_list_error_name_unknown;

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   class_registry_const_iterator crci, end;

   for( crci = p_class_registry->begin( ), end = p_class_registry->end( ); crci != end; ++crci )
   {
      class_info_container class_info;
      get_class_info_for_module_class( module_id_or_name, crci->first, class_info );

      if( type >= 0 && ( crci->second )->class_type( ) != type )
         continue;

      if( p_pat && !wildcard_match( p_pat, ( crci->second )->class_name( ) ) )
         continue;

      string restrictions;

      if( !class_info.empty( ) )
      {
         string::size_type pos = class_info[ 0 ].find( ',' );
         if( pos != string::npos )
         {
            restrictions = class_info[ 0 ].substr( pos );
            replace( restrictions, ",", " " );
         }
      }

      if( p_os )
         *p_os << crci->first << ' ' << ( crci->second )->class_name( )
          << ' ' << ( crci->second )->class_type( ) << restrictions << '\n';

      if( p_lst )
         p_lst->push_back( crci->first );

      if( p_map )
      {
         if( key_by_id )
            p_map->insert( make_pair( crci->first, ( crci->second )->class_name( ) ) );
         else
            p_map->insert( make_pair( ( crci->second )->class_name( ), crci->first ) );
      }
   }

   return e_module_class_list_error_none;
}

}

string module_directory( const string* p_new_directory )
{
   static TLS( string )* gtp_directory;

   if( !gtp_directory )
      gtp_directory = new string;

   if( p_new_directory )
      *gtp_directory = *p_new_directory;

   return *gtp_directory;
}

module_load_error load_module( const string& module_name )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_name ) );

   if( mi != g_modules.end( ) )
   {
      ++( mi->second ).ref_count;

      externals_container* p_externals;
      ( *( mi->second ).fp_obtain_externals_func )( p_externals );

      for( size_t i = 0; i < p_externals->size( ); i++ )
      {
         module_iterator xmi = g_modules.find( directory + module_id_from_id_or_name( ( *p_externals )[ i ] ) );
         if( xmi != g_modules.end( ) )
            ++( xmi->second ).ref_count;
         else
            return e_module_load_error_external_module_failure;
      }

      return e_module_load_error_none;
   }

   string dyn_lib_name( "./" );

   dyn_lib_name += directory + module_name;

   dyn_lib_name += string( ".so" );

   if( _access( dyn_lib_name.c_str( ), 0 ) != 0 )
      return e_module_load_error_file_does_not_exist;

   unique_ptr< dynamic_library > up_dynamic_library( new dynamic_library( dyn_lib_name, module_name ) );

   fp_init_dir fp_init_dir_func;
   fp_init_dir_func = ( fp_init_dir )up_dynamic_library->bind_to_function( c_init_dir_func_name );

   fp_load_strings fp_load_strings_func;
   fp_load_strings_func = ( fp_load_strings )up_dynamic_library->bind_to_function( c_load_strings_func_name );

   fp_init_classes fp_init_classes_func;
   fp_init_classes_func = ( fp_init_classes )up_dynamic_library->bind_to_function( c_init_classes_func_name );

   fp_term_classes fp_term_classes_func;
   fp_term_classes_func = ( fp_term_classes )up_dynamic_library->bind_to_function( c_term_classes_func_name );

   fp_obtain_externals fp_obtain_externals_func;
   fp_obtain_externals_func = ( fp_obtain_externals )up_dynamic_library->bind_to_function( c_obtain_externals_func_name );

   fp_obtain_module_details fp_obtain_module_details_func;
   fp_obtain_module_details_func = ( fp_obtain_module_details )up_dynamic_library->bind_to_function( c_obtain_module_details_func_name );

   const module_details* p_details;

   ( *fp_obtain_module_details_func )( p_details );

   ( *fp_init_dir_func )( directory.c_str( ) );

   if( !( *fp_load_strings_func )( p_details->p_name ) )
      throw runtime_error( "unexpected error loading module strings for '"
       + directory + p_details->p_name + "' (file missing or contains bad formatting?)" );

   ( *fp_init_classes_func )( p_details->p_id );

   externals_container* p_externals;

   ( *fp_obtain_externals_func )( p_externals );

   for( size_t i = 0; i < p_externals->size( ); i++ )
   {
      module_iterator xmi = g_modules.find( directory + module_id_from_id_or_name( ( *p_externals )[ i ] ) );
      if( xmi != g_modules.end( ) )
         ++( xmi->second ).ref_count;
      else
         return e_module_load_error_external_module_failure;
   }

   if( g_module_ids.count( directory + p_details->p_id ) )
   {
      ( *fp_term_classes_func )( p_details->p_id );

      throw runtime_error( "module id '" + string( p_details->p_id )
       + "' is already being used by '" + g_module_ids[ directory + p_details->p_id ] + "'" );
   }

   g_modules.insert( module_value_type( directory + p_details->p_id,
    module_library_info( p_details, up_dynamic_library.get( ), fp_term_classes_func, fp_obtain_externals_func ) ) );

   g_module_ids.insert( make_pair( directory + p_details->p_id, p_details->p_name ) );
   g_module_names.insert( make_pair( directory + p_details->p_name, p_details->p_id ) );

   up_dynamic_library.release( );

   return e_module_load_error_none;
}

module_unload_error unload_module( const string& module_id_or_name )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );
   if( mi == g_modules.end( ) )
      return e_module_unload_error_name_unknown;

   externals_container* p_externals;
   ( *( mi->second ).fp_obtain_externals_func )( p_externals );

   for( size_t i = 0; i < p_externals->size( ); i++ )
   {
      module_iterator xmi = g_modules.find( directory + module_id_from_id_or_name( ( *p_externals )[ i ] ) );
      if( xmi != g_modules.end( ) )
         --( xmi->second ).ref_count;
      else
         return e_module_unload_error_external_module_failure;
   }

   if( --( mi->second ).ref_count == 0 )
   {
      ( *( mi->second ).fp_term_classes_func )( module_id_from_id_or_name( module_id_or_name ).c_str( ) );

      delete ( mi->second ).p_dynamic_library;

      g_modules.erase( mi );

      if( g_module_ids.count( directory + module_id_or_name ) )
      {
         g_module_names.erase( directory + g_module_ids[ directory + module_id_or_name ] );
         g_module_ids.erase( directory + module_id_or_name );
      }
      else if( g_module_names.count( directory + module_id_or_name ) )
      {
         g_module_ids.erase( directory + g_module_names[ directory + module_id_or_name ] );
         g_module_names.erase( directory + module_id_or_name );
      }
   }

   return e_module_unload_error_none;
}

void list_modules( ostream& os )
{
   guard g( g_mutex );

   module_const_iterator mci, end;

   for( mci = g_modules.begin( ), end = g_modules.end( ); mci != end; ++mci )
      os << mci->first << " (" << ( mci->second ).ref_count << ")\n";
}

module_class_list_error list_module_classes( const string& module_id_or_name, ostream& os, int type, const char* p_pat )
{
   return list_module_classes_impl( module_id_or_name, &os, 0, 0, false, type, p_pat );
}

module_class_list_error list_module_classes(
 const string& module_id_or_name, vector< string >& class_list, int type, const char* p_pat )
{
   return list_module_classes_impl( module_id_or_name, 0, &class_list, 0, false, type, p_pat );
}

module_class_list_error list_module_classes(
 const string& module_id_or_name, map< string, string >& class_map, bool key_by_id, int type, const char* p_pat )
{
   return list_module_classes_impl( module_id_or_name, 0, 0, &class_map, key_by_id, type, p_pat );
}

void init_module_class_info( const string& module_id_or_name, module_library_info& module_info )
{
   guard g( g_mutex );

   module_class_list_error rc = list_module_classes( module_id_or_name, module_info.class_ids, true );

   if( rc != e_module_class_list_error_none )
      throw runtime_error( "unexpected module class list error in init_module_class_info" );

   rc = list_module_classes( module_id_or_name, module_info.class_names, false );

   if( rc != e_module_class_list_error_none )
      throw runtime_error( "unexpected module class list error in init_module_class_info" );
}

void init_module_class_field_info( const string& module_id_or_name,
 const string& class_id_or_name, module_library_info& module_info )
{
   guard g( g_mutex );

   string class_id( class_id_or_name );

   if( module_info.class_names.count( class_id_or_name ) )
      class_id = module_info.class_names[ class_id_or_name ];

   if( module_info.class_ids.count( class_id ) )
   {
      if( !module_info.class_field_ids.count( class_id ) )
      {
         map< string, string > field_ids;
         map< string, string > field_names;

         fp_obtain_class_registry obtain_class_registry_func;

         obtain_class_registry_func
          = ( fp_obtain_class_registry )( module_info ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

         const class_registry_container* p_class_registry;

         ( *obtain_class_registry_func )( p_class_registry );

         class_registry_const_iterator crci, end;

         for( crci = p_class_registry->begin( ), end = p_class_registry->end( ); crci != end; ++crci )
         {
            if( class_id == ( crci->second )->class_id( ) )
            {
               field_info_container all_field_info;
               ( crci->second )->get_field_info( all_field_info );

               field_info_const_iterator fici;

               for( fici = all_field_info.begin( ); fici != all_field_info.end( ); ++fici )
               {
                  field_ids.insert( make_pair( fici->id, fici->name ) );
                  field_names.insert( make_pair( fici->name, fici->id ) );
               }

               break;
            }
         }

         module_info.class_field_ids.insert( make_pair( class_id, field_ids ) );
         module_info.class_field_names.insert( make_pair( class_id, field_names ) );
      }
   }
}

module_string_list_error list_module_strings( const string& module_id_or_name, ostream& os )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      return e_module_string_list_error_name_unknown;

   fp_obtain_module_strings obtain_module_strings_func;

   obtain_module_strings_func
    = ( fp_obtain_module_strings )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_module_strings_func_name );

   const module_strings_container* p_module_strings;

   ( *obtain_module_strings_func )( p_module_strings );

   for( module_strings_const_iterator
    msci = p_module_strings->begin( ), end = p_module_strings->end( ); msci != end; ++msci )
      os << msci->first << ' ' << msci->second << '\n';

   return e_module_string_list_error_none;
}

module_class_field_list_error list_module_class_fields(
 const string& module_id_or_name, const string& class_id_or_name, ostream& os )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      return e_module_class_field_list_error_name_unknown;

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   bool found = false;

   class_registry_const_iterator crci, end;

   for( crci = p_class_registry->begin( ), end = p_class_registry->end( ); crci != end; ++crci )
   {
      if( class_id_or_name == ( crci->second )->class_id( ) || class_id_or_name == ( crci->second )->class_name( ) )
      {
         found = true;

         field_info_container all_field_info;

         ( crci->second )->get_field_info( all_field_info );

         field_info_const_iterator fici;

         for( fici = all_field_info.begin( ); fici != all_field_info.end( ); ++fici )
         {
            string type_name( fici->type_name );

            string module_name_prefix( ( crci->second )->resolved_module_name( ) );
            module_name_prefix += '_';

            // NOTE: For fields that are foreign keys output the class id as well as name.
            if( type_name.length( ) > module_name_prefix.length( )
             && type_name.substr( 0, module_name_prefix.length( ) ) == module_name_prefix )
            {
               if( ( mi->second ).class_ids.empty( ) )
                  init_module_class_info( module_id_or_name, mi->second );

               if( ( mi->second ).class_names.count( type_name.substr( module_name_prefix.length( ) ) ) )
                  type_name = ( mi->second ).class_names[ type_name.substr( module_name_prefix.length( ) ) ] + ":" + type_name;
            }

            os << fici->id << ' ' << fici->name << ' ' << type_name;

            if( !fici->is_transient )
               os << " normal";
            else
               os << " transient";

            if( !fici->scope.empty( ) )
               os << ' ' << fici->scope << ' ' << fici->change;

            if( fici->is_owner_fk )
               os << " *";

            os << '\n';
         }

         break;
      }
   }

   if( !found )
      return e_module_class_field_list_error_class_name_unknown;

   return e_module_class_field_list_error_none;
}

module_class_procedure_list_error list_module_class_procedures(
 const string& module_id_or_name, const string& class_id_or_name, ostream& os )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      return e_module_class_procedure_list_error_name_unknown;

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   bool found = false;

   class_registry_const_iterator crci, end;

   for( crci = p_class_registry->begin( ), end = p_class_registry->end( ); crci != end; ++crci )
   {
      if( class_id_or_name == ( crci->second )->class_id( ) || class_id_or_name == ( crci->second )->class_name( ) )
      {
         found = true;

         const procedure_info_container& procedure_info(
          get_procedure_info_for_module_class( module_id_or_name, ( crci->second )->class_id( ) ) );

         for( procedure_info_const_iterator pici = procedure_info.begin( ); pici != procedure_info.end( ); ++pici )
         {
            os << pici->first << ' ' << pici->second.name;

            if( !pici->second.access_info.empty( ) )
               os << ' ' << pici->second.access_info;

            os << '\n';
         }

         break;
      }
   }

   if( !found )
      return e_module_class_procedure_list_error_class_name_unknown;

   return e_module_class_procedure_list_error_none;
}

string get_module_id( const string& module_name )
{
   guard g( g_mutex );

   return module_id_from_id_or_name( module_name );
}

size_t get_module_ref_count( const string& module_id_or_name )
{
   guard g( g_mutex );

   size_t rc = 0;

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi != g_modules.end( ) )
      rc = ( mi->second ).ref_count;

   return rc;
}

dynamic_library* get_module_ptr( const string& module_id_or_name )
{
   guard g( g_mutex );

   dynamic_library* p_dynamic_library( 0 );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi != g_modules.end( ) )
      p_dynamic_library = ( mi->second ).p_dynamic_library;

   return p_dynamic_library;
}

class_base* construct_object( const string& module_id_or_name, const string& class_id )
{
   guard g( g_mutex );

   class_base* p_class_base( 0 );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi != g_modules.end( ) )
   {
      fp_create_class_object create_class_object_func;

      create_class_object_func
       = ( fp_create_class_object )( mi->second ).p_dynamic_library->bind_to_function( c_create_class_object_func_name );

      ( *create_class_object_func )( class_id.c_str( ), p_class_base );
   }

   return p_class_base;
}

void destroy_object( const string& module_id_or_name, const string& class_id, class_base*& p_cb )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi != g_modules.end( ) )
   {
      fp_destroy_class_object destroy_class_object_func;

      destroy_class_object_func
       = ( fp_destroy_class_object )( mi->second ).p_dynamic_library->bind_to_function( c_destroy_class_object_func_name );

      ( *destroy_class_object_func )( class_id.c_str( ), p_cb );

      p_cb = 0;
   }
}

void get_class_info_for_module_class( const string& module_id_or_name, const string& class_id, class_info_container& class_info )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_class_info_for_module_class" );

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   class_registry_const_iterator crci = p_class_registry->find( class_id );

   if( crci == p_class_registry->end( ) )
      throw runtime_error( "unable to locate class '" + class_id + "' in the module's class registry" );

   crci->second->get_class_info( class_info );
}

string get_module_id_for_id_or_name( const string& module_id_or_name )
{
   guard g( g_mutex );

   return module_id_from_id_or_name( module_id_or_name );
}

string get_module_name_for_id_or_name( const string& module_id_or_name )
{
   guard g( g_mutex );

   return module_name_from_id_or_name( module_id_or_name );
}

string get_class_id_for_id_or_name( const string& module_id_or_name, const string& id_or_name )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   string module_id( module_id_from_id_or_name( module_id_or_name ) );

   module_iterator mi = g_modules.find( directory + module_id );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_class_id_for_id_or_name" );

   string class_id( id_or_name );

   if( ( mi->second ).class_ids.empty( ) )
      init_module_class_info( module_id_or_name, mi->second );

   if( !( mi->second ).class_ids.count( id_or_name ) )
   {
      if( ( mi->second ).class_names.count( id_or_name ) )
         class_id = ( mi->second ).class_names[ id_or_name ];
      else if( ( mi->second ).class_ids.count( module_id + id_or_name ) )
         class_id = module_id + id_or_name;
   }

   return class_id;
}

string get_class_name_for_id_or_name( const string& module_id_or_name, const string& id_or_name )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   string module_id( module_id_from_id_or_name( module_id_or_name ) );

   module_iterator mi = g_modules.find( directory + module_id );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_class_id_for_id_or_name" );

   string class_name( id_or_name );

   if( ( mi->second ).class_ids.empty( ) )
      init_module_class_info( module_id_or_name, mi->second );

   if( ( mi->second ).class_ids.count( id_or_name ) )
      class_name = ( mi->second ).class_ids[ id_or_name ];
   else if( ( mi->second ).class_ids.count( module_id + id_or_name ) )
      class_name = ( mi->second ).class_ids[ module_id + id_or_name ];

   return class_name;
}

string get_field_id_for_id_or_name(
 const string& module_id_or_name, const string& class_id_or_name, const string& id_or_name )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   string module_id( module_id_from_id_or_name( module_id_or_name ) );

   module_iterator mi = g_modules.find( directory + module_id );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_class_id_for_id_or_name" );

   string field_id( id_or_name );
   string class_id( class_id_or_name );

   if( ( mi->second ).class_ids.empty( ) )
      init_module_class_info( module_id, mi->second );

   if( ( mi->second ).class_names.count( class_id_or_name ) )
      class_id = ( mi->second ).class_names[ class_id_or_name ];
   else if( ( mi->second ).class_ids.count( module_id + class_id_or_name ) )
      class_id = module_id + class_id_or_name;

   if( !( mi->second ).class_field_ids.count( class_id ) )
      init_module_class_field_info( module_id, class_id, mi->second );

   if( ( mi->second ).class_field_names.count( class_id ) )
   {
      if( ( mi->second ).class_field_names[ class_id ].count( id_or_name ) )
         field_id = ( mi->second ).class_field_names[ class_id ][ id_or_name ];
      else if( ( mi->second ).class_field_ids[ class_id ].count( class_id + id_or_name ) )
         field_id = class_id + id_or_name;
   }

   return field_id;
}

string get_field_name_for_id_or_name(
 const string& module_id_or_name, const string& class_id_or_name, const string& id_or_name )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   string module_id( module_id_from_id_or_name( module_id_or_name ) );

   module_iterator mi = g_modules.find( directory + module_id );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_class_id_for_id_or_name" );

   string field_name( id_or_name );
   string class_id( class_id_or_name );

   if( ( mi->second ).class_ids.empty( ) )
      init_module_class_info( module_id, mi->second );

   if( ( mi->second ).class_names.count( class_id_or_name ) )
      class_id = ( mi->second ).class_names[ class_id_or_name ];
   else if( ( mi->second ).class_ids.count( module_id + class_id_or_name ) )
      class_id = module_id + class_id;

   if( !( mi->second ).class_field_ids.count( class_id ) )
      init_module_class_field_info( module_id, class_id, mi->second );

   if( ( mi->second ).class_field_ids.count( class_id ) )
   {
      if( ( mi->second ).class_field_ids[ class_id ].count( id_or_name ) )
         field_name = ( mi->second ).class_field_ids[ class_id ][ id_or_name ];
      else if( ( mi->second ).class_field_ids[ class_id ].count( class_id + id_or_name ) )
         field_name = ( mi->second ).class_field_ids[ class_id ][ class_id + id_or_name ];
   }

   return field_name;
}

bool get_module_class_has_derivations( const string& module_id_or_name, const string& class_id )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_module_class_has_derivations" );

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   class_registry_const_iterator crci = p_class_registry->find( class_id );

   if( crci == p_class_registry->end( ) )
      throw runtime_error( "unable to locate class '" + class_id + "' in the module's class registry" );

   return crci->second->has_derivations( );
}

void get_foreign_key_info_for_module_class( const string& module_id_or_name,
 const string& class_id, foreign_key_info_container& foreign_key_info )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_foreign_key_info_for_module_class" );

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   class_registry_const_iterator crci = p_class_registry->find( class_id );

   if( crci == p_class_registry->end( ) )
      throw runtime_error( "unable to locate class '" + class_id + "' in the module's class registry" );

   crci->second->get_foreign_key_info( foreign_key_info );
}

const procedure_info_container& get_procedure_info_for_module_class( const string& module_id_or_name, const string& class_id )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_procedure_info_for_module_class" );

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   class_registry_const_iterator crci = p_class_registry->find( class_id );

   if( crci == p_class_registry->end( ) )
      throw runtime_error( "unable to locate class '" + class_id + "' in the module's class registry" );

   return crci->second->get_procedure_info( );
}

string get_sql_columns_for_module_class( const string& module_id_or_name, const string& class_id )
{
   guard g( g_mutex );

   string sql_columns;

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_sql_columns_for_module_class" );

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   class_registry_const_iterator crci = p_class_registry->find( class_id );

   if( crci == p_class_registry->end( ) )
      throw runtime_error( "unable to locate class '" + class_id + "' in the module's class registry" );

   if( crci->second->persistence_type( ) == 0 ) // i.e. SQL persistence
      sql_columns = crci->second->get_sql_columns( );

   return sql_columns;
}

void get_sql_indexes_for_module_class(
 const string& module_id_or_name, const string& class_id, vector< string >& indexes )
{
   guard g( g_mutex );

   string directory( module_directory( ) );

   if( !directory.empty( ) )
      directory += "/";

   module_iterator mi = g_modules.find( directory + module_id_from_id_or_name( module_id_or_name ) );

   if( mi == g_modules.end( ) )
      throw runtime_error( "unknown module '" + module_id_or_name + "' in get_sql_indexes_for_module_class" );

   fp_obtain_class_registry obtain_class_registry_func;

   obtain_class_registry_func
    = ( fp_obtain_class_registry )( mi->second ).p_dynamic_library->bind_to_function( c_obtain_class_registry_func_name );

   const class_registry_container* p_class_registry;

   ( *obtain_class_registry_func )( p_class_registry );

   class_registry_const_iterator crci = p_class_registry->find( class_id );

   if( crci == p_class_registry->end( ) )
      throw runtime_error( "unable to locate class '" + class_id + "' in the module's class registry" );

   if( crci->second->persistence_type( ) == 0 ) // i.e. SQL persistence
      crci->second->get_sql_indexes( indexes );
}

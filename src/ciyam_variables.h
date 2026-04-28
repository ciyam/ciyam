// Copyright (c) 2017-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_VARIABLES_H
#  define CIYAM_VARIABLES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#  endif

#  include "ptypes.h"

#  include "ciyam_variable_names.h"

struct progress;

std::string get_special_var_name( special_var var );

struct var_name
{
   var_name( size_t num )
   {
      name = get_special_var_name( ( special_var )num );
   }

   var_name( const char* p_name )
    :
    name( p_name )
   {
   }

   var_name( const std::string& name )
    :
    name( name )
   {
   }

   std::string name;
};

class system_variable_lock
{
   public:
   system_variable_lock( const std::string& name );
   system_variable_lock( const std::string& name, const std::string& display );

   ~system_variable_lock( );

   private:
   var_name var;

   protected:
   void acquire_lock( const char* p_display_name_str = 0 );
};

bool has_system_variable( const var_name& var );

std::string get_system_variable( const var_name& var, bool is_internal = true );

struct expression
{
   explicit expression( const std::string& expr ) : expr( expr ) { }

   std::string expr;
};

std::string get_system_variable( const expression& expr, bool is_internal = true );

void set_system_variable(
 const var_name& var, const std::string& value,
 bool is_init = false, progress* p_progress = 0 );

bool set_system_variable( const var_name& var,
 const std::string& value, const std::string& current,
 progress* p_progress = 0, const char append_separator = '\0' );

void rename_system_variable( const var_name& old_var, const var_name& new_var );

enum variable_check_type
{
   e_variable_check_type_no_session_has,
   e_variable_check_type_any_session_has,
   e_variable_check_type_has_other_system,
   e_variable_check_type_not_has_other_system
};

struct set_variable_checker
{
   set_variable_checker( variable_check_type check_type,
    const var_name& var, set_variable_checker* p_additional_check = 0 )
    :
    var( var ),
    check_type( check_type ),
    p_additional_check( p_additional_check )
   {
   }

   bool can_set( ) const;

   var_name var;

   variable_check_type check_type;

   set_variable_checker* p_additional_check;
};

bool set_system_variable(
 const var_name& var, const std::string& value,
 set_variable_checker& checker, bool is_init = false, progress* p_progress = 0 );

struct system_variable_eraser
{
   system_variable_eraser( const var_name& var )
    :
    var( var )
   {
   }

   ~system_variable_eraser( )
   {
      set_system_variable( var, "" );
   }

   var_name var;
};

struct temporary_system_variable
{
   temporary_system_variable( const var_name& var );

   temporary_system_variable( const var_name& var, const std::string& value );

   ~temporary_system_variable( );

   struct impl;

   impl* p_impl;
};

void system_variable_expression( const std::string& expr );

std::string variable_name_from_name_and_value( const std::string& name_and_value, std::string* p_value = 0 );

class mutex;

mutex& get_mutex_for_ciyam_variables( );

#endif

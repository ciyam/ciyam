// Copyright (c) 2005
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
#  include <ctime>
#  include <cassert>
#  include <climits>
#  include <map>
#  include <set>
#  include <stack>
#  include <vector>
#  include <fstream>
#  include <sstream>
#  include <iomanip>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_base.h"

#include "md5.h"
#include "ods.h"
#include "sio.h"
#include "salt.h"
#include "sha1.h"
#include "base64.h"
#include "sql_db.h"
#include "config.h"
#include "format.h"
#include "threads.h"
#include "pointers.h"
#include "utilities.h"
#include "date_time.h"
#include "class_base.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#endif
#include "fs_iterator.h"
#include "oid_pointer.h"
#include "crypt_stream.h"
#include "ciyam_strings.h"
#include "ciyam_session.h"
#include "command_handler.h"
#include "dynamic_library.h"
#include "module_interface.h"
#include "module_management.h"
#include "read_write_stream.h"

using namespace std;

namespace
{

const string c_nul_key( 1, '\0' );

const char c_module_prefix_separator = '_';

const int c_identity_burn = 100;
const int c_iteration_row_cache_limit = 500;

const int c_max_lock_attempts = 20;
const int c_lock_attempt_sleep_time = 100;

const char* const c_server_sid_file = "ciyam_server.sid";
const char* const c_server_config_file = "ciyam_server.sio";

const char* const c_manuscript_file = "manuscript.sio";

const char* const c_section_class = "class";
const char* const c_attribute_name = "name";
const char* const c_attribute_fields = "fields";
const char* const c_attribute_record = "record";

const char* const c_section_script = "script";
const char* const c_attribute_filename = "filename";
const char* const c_attribute_arguments = "arguments";

const char* const c_section_mbox = "mbox";
const char* const c_section_pop3 = "pop3";
const char* const c_section_smtp = "smtp";
const char* const c_section_email = "email";
const char* const c_attribute_path = "path";
const char* const c_attribute_domain = "domain";
const char* const c_attribute_server = "server";
const char* const c_attribute_sender = "sender";
const char* const c_attribute_suffix = "suffix";
const char* const c_attribute_license = "license";
const char* const c_attribute_identity = "identity";
const char* const c_attribute_password = "password";
const char* const c_attribute_security = "security";
const char* const c_attribute_timezone = "timezone";
const char* const c_attribute_username = "username";
const char* const c_attribute_web_root = "web_root";
const char* const c_attribute_web_type = "web_type";
const char* const c_attribute_set_trace = "set_trace";
const char* const c_attribute_use_https = "use_https";
const char* const c_attribute_max_sessions = "max_sessions";
const char* const c_attribute_pem_password = "pem_password";
const char* const c_attribute_sql_password = "sql_password";
const char* const c_attribute_default_storage = "default_storage";
const char* const c_attribute_script_reconfig = "script_reconfig";
const char* const c_attribute_session_timeout = "session_timeout";
const char* const c_attribute_max_send_attempts = "max_send_attempts";
const char* const c_attribute_max_attached_data = "max_attached_data";
const char* const c_attribute_max_storage_handlers = "max_storage_handlers";

const char* const c_default_web_root = "%root%/%store%";

const char* const c_expand_root = "%root%";
const char* const c_expand_store = "%store%";

const char* const c_uid_unknown = "<unknown>";

const char* const c_dead_keys_ext = ".dead_keys.lst";

const char* const c_default_pem_password = "password";

const char* const c_script_dummy_filename = "*script*";

const char* const c_system_variable_storage = "@storage";

const char* const c_session_variable_dtm = "@dtm";
const char* const c_session_variable_sec = "@sec";
const char* const c_session_variable_uid = "@uid";
const char* const c_session_variable_none = "@none";
const char* const c_session_variable_class = "@class";
const char* const c_session_variable_module = "@module";
const char* const c_session_variable_storage = "@storage";
const char* const c_session_variable_tz_abbr = "@tz_abbr";

struct instance_info
{
   instance_info( class_base* p_class_base, dynamic_library* p_dynamic_library )
    :
    p_class_base( p_class_base ),
    p_dynamic_library( p_dynamic_library )
   {
   }

   class_base* p_class_base;
   dynamic_library* p_dynamic_library;
};

struct module_commands
{
   module_commands( size_t num_commands,
    command_definition* p_command_definitions, command_functor_creator* p_command_functor_creator )
    :
    num_commands( num_commands ),
    p_command_definitions( p_command_definitions ),
    p_command_functor_creator( p_command_functor_creator )
   {
   }

   size_t num_commands;
   command_definition* p_command_definitions;
   command_functor_creator* p_command_functor_creator;
};

typedef map< size_t, instance_info > object_instance_registry_container;
typedef object_instance_registry_container::iterator object_instance_registry_iterator;
typedef object_instance_registry_container::const_iterator object_instance_registry_const_iterator;
typedef object_instance_registry_container::value_type object_instance_registry_value_type;

typedef map< string, string > module_container;
typedef module_container::iterator module_iterator;
typedef module_container::const_iterator module_const_iterator;
typedef module_container::value_type module_value_type;

typedef map< string, module_commands > module_commands_registry_container;
typedef module_commands_registry_container::iterator module_commands_registry_iterator;
typedef module_commands_registry_container::const_iterator module_commands_registry_const_iterator;
typedef module_commands_registry_container::value_type module_commands_registry_value_type;

class storage_handler;

struct session
{
   session( size_t id, size_t slot, command_handler& cmd_handler, storage_handler* p_storage_handler )
    :
    id( id ),
    slot( slot ),
    sql_count( 0 ),
    next_handle( 0 ),
    is_captured( false ),
    running_script( false ),
    skip_fk_fetches( false ),
    skip_validation( false ),
    skip_is_constrained( false ),
    cmd_handler( cmd_handler ),
    p_storage_handler( p_storage_handler )
   {
      dtm_created = date_time::local( );
      dtm_last_cmd = date_time::local( );

      srand( time( 0 ) );

      variables.insert( make_pair( "@uuid", uuid( ).as_string( ) ) );
   }

   size_t id;
   size_t slot;

   string dtm;
   string uid;
   string sec;

   string tz_abbr;

   string last_cmd;

   set< string > perms;

   string tmp_directory;

   size_t sql_count;
   size_t next_handle;

   bool is_captured;
   bool running_script;

   bool skip_fk_fetches;
   bool skip_validation;
   bool skip_is_constrained;

   date_time dtm_created;
   date_time dtm_last_cmd;

   module_container modules_by_id;
   module_container modules_by_name;

   auto_ptr< sql_db > ap_db;

   command_handler& cmd_handler;

   storage_handler* p_storage_handler;

   vector< string > storage_controlled_modules;

   string transaction_log_command;

   map< string, string > variables;

   stack< ods::transaction* > transactions;

   set< size_t > release_sessions;
   map< size_t, date_time > condemned_sessions;

   module_commands_registry_container commands_registry;
   object_instance_registry_container instance_registry;
};

struct op_lock
{
   enum lock_type
   {
      e_lock_type_none,
      e_lock_type_view,
      e_lock_type_link,
      e_lock_type_review,
      e_lock_type_obtain,
      e_lock_type_update,
      e_lock_type_create,
      e_lock_type_destroy,
      e_lock_type_propagate,
      e_lock_type_dummy_value
   };

   op_lock( )
    :
    handle( 0 ),
    type( e_lock_type_none ),
    tx_type( e_lock_type_none ),
    transaction_id( 0 ),
    transaction_level( 0 ),
    p_session( 0 ),
    p_class_base( 0 ),
    p_root_class( 0 )
   {
   }

   op_lock( size_t handle, lock_type type, int_t transaction_id,
    int_t transaction_level, session* p_session, class_base* p_class_base, class_base* p_root_class )
    :
    handle( handle ),
    type( type ),
    transaction_id( transaction_id ),
    transaction_level( transaction_level ),
    p_session( p_session ),
    p_class_base( p_class_base ),
    p_root_class( p_root_class )
   {
      if( transaction_level )
         tx_type = type;
      else
         tx_type = e_lock_type_none;
   }

   static string lock_type_name( lock_type type );

   size_t handle;

   lock_type type;
   lock_type tx_type;

   int_t transaction_id;
   int_t transaction_level;

   session* p_session;
   class_base* p_class_base;
   class_base* p_root_class;
};

bool operator ==( op_lock& lhs, op_lock& rhs )
{
   return lhs.type == rhs.type && lhs.tx_type == rhs.tx_type
    && lhs.transaction_id == rhs.transaction_id && lhs.transaction_level == rhs.transaction_level && lhs.p_session == rhs.p_session;
}

string op_lock::lock_type_name( lock_type type )
{
   string retval;

   if( type == e_lock_type_none )
      retval = "none";
   else if( type == e_lock_type_view )
      retval = "view";
   else if( type == e_lock_type_link )
      retval = "link";
   else if( type == e_lock_type_review )
      retval = "review";
   else if( type == e_lock_type_obtain )
      retval = "obtain";
   else if( type == e_lock_type_update )
      retval = "update";
   else if( type == e_lock_type_create )
      retval = "create";
   else if( type == e_lock_type_destroy )
      retval = "destroy";
   else if( type == e_lock_type_propagate )
      retval = "propagate";
   else
      retval = "**unknown**";

   return retval;
}

const int c_num_lock_types = op_lock::e_lock_type_dummy_value - 1;

bool locks_can_coexist( op_lock::lock_type lhs, op_lock::lock_type rhs )
{
   struct
   {
      bool values[ c_num_lock_types ];
   }
   g_locks_can_coexist[ ] =
   {
   //   V  L  R  O  U  C  D  P
      { 1, 1, 1, 1, 1, 1, 1, 1 }, // V
      { 1, 1, 1, 1, 1, 0, 0, 0 }, // L
      { 1, 1, 1, 1, 0, 0, 0, 0 }, // R
      { 1, 1, 1, 0, 0, 0, 0, 0 }, // O
      { 1, 1, 0, 0, 0, 0, 0, 0 }, // U
      { 1, 0, 0, 0, 0, 0, 0, 0 }, // C
      { 1, 0, 0, 0, 0, 0, 0, 0 }, // D
      { 1, 0, 0, 0, 0, 0, 0, 1 }, // P
   };

   if( lhs == op_lock::e_lock_type_none || rhs == op_lock::e_lock_type_none )
      return true;

   return g_locks_can_coexist[ lhs - 1 ].values[ rhs - 1 ];
}

typedef multimap< string, op_lock > lock_container;
typedef lock_container::iterator lock_iterator;
typedef lock_container::const_iterator lock_const_iterator;
typedef lock_container::value_type lock_value_type;

typedef map< size_t, lock_iterator > lock_index_container;
typedef lock_index_container::iterator lock_index_iterator;
typedef lock_index_container::const_iterator lock_index_const_iterator;
typedef lock_index_container::value_type lock_index_value_type;

const int c_storage_format_version = 1;

struct storage_module
{
   storage_module( ) : version( 0 ) { }

   int32_t version;
};

typedef storable< storage_module, 256, storable_base > storable_module;

typedef map< string, oid_pointer< storable_module > > storable_module_container;
typedef storable_module_container::iterator storable_module_iterator;
typedef storable_module_container::const_iterator storable_module_const_iterator;
typedef storable_module_container::value_type storable_module_value_type;

// NOTE: Identity values less than 5 are reserved for system purposes.
struct identity
{
   identity( ) : next_id( 4 ), ceiling( 0 ) { }

   int32_t next_id;
   int32_t ceiling;
};

typedef storable< identity, 0, storable_base > storable_identity;

int_t size_of( const identity& i )
{
   return size_determiner( &i.next_id );
}

read_stream& operator >>( read_stream& rs, identity& i )
{
   rs >> i.next_id;
   return rs;
}

write_stream& operator <<( write_stream& ws, const identity& i )
{
   ws << i.next_id;
   return ws;
}

struct storage_root
{
   storage_root( )
    :
    version( c_storage_format_version ),
    identity( uuid( ).as_string( ) ),
    web_root( c_default_web_root ),
    truncation_count( 0 )
   {
   }

   int32_t version;
   string identity;

   string web_root;

   string module_directory;

   int32_t truncation_count;

   vector< string > module_list;
   storable_module_container modules;

   oid_pointer< storable_identity > o_identity;
};

typedef storable< storage_root, 512, storable_base > storable_root;

int_t size_of( const storage_root& sr )
{
   size_t size( 0 );

   size += size_determiner( &sr.version );
   size += size_determiner( &sr.identity );

   size += size_determiner( &sr.web_root );

   size += size_determiner( &sr.module_directory );
   size += size_determiner( &sr.truncation_count );

   size += size_determiner( &sr.module_list );
   size += size_determiner( &sr.modules );

   size += size_determiner( &sr.o_identity );

   return size;
}

read_stream& operator >>( read_stream& rs, storage_root& sr )
{
   rs
    >> sr.version
    >> sr.identity
    >> sr.web_root
    >> sr.module_directory
    >> sr.truncation_count
    >> sr.module_list
    >> sr.modules
    >> sr.o_identity;

   return rs;
}

write_stream& operator <<( write_stream& ws, const storage_root& sr )
{
   ws
    << sr.version
    << sr.identity
    << sr.web_root
    << sr.module_directory
    << sr.truncation_count
    << sr.module_list
    << sr.modules
    << sr.o_identity;

   return ws;
}

int_t size_of( const storage_module& sm )
{
   size_t size( 0 );

   size += size_determiner( &sm.version );

   return size;
}

read_stream& operator >>( read_stream& rs, storage_module& sm )
{
   rs >> sm.version;

   return rs;
}

write_stream& operator <<( write_stream& ws, const storage_module& sm )
{
   ws << sm.version;

   return ws;
}

class storage_handler
{
   public:
   storage_handler( size_t slot, const string& name, ods* p_ods = 0 )
    :
    slot( slot ),
    name( name ),
    p_ods( p_ods ),
    ref_count( 0 ),
    p_bulk_write( 0 ),
    next_lock_handle( 1 ),
    p_alternative_log_file( 0 ),
    is_locked_for_admin( false )
   {
   }

   size_t get_slot( ) const { return slot; }
   const string& get_name( ) const { return name; }

   ods* get_ods( ) const { return p_ods; }

   void release_ods( ) { delete p_ods; p_ods = 0; }

   void obtain_bulk_write( ) { p_bulk_write = new ods::bulk_write( *p_ods ); }
   void release_bulk_write( ) { delete p_bulk_write; p_bulk_write = 0; }

   size_t get_ref_count( ) const { return ref_count; }

   size_t inc_ref_count( ) { return ++ref_count; }
   size_t dec_ref_count( ) { return --ref_count; }

   ofstream& get_log_file( ) { return log_file; }

   ofstream* get_alternative_log_file( ) { return p_alternative_log_file; }
   void set_alternative_log_file( ofstream* p_log_file ) { p_alternative_log_file = p_log_file; }

   storable_root& get_root( ) { return root; }
   const storable_root& get_root( ) const { return root; }

   bool get_is_locked_for_admin( ) const { return is_locked_for_admin; }
   void set_is_locked_for_admin( bool lock_for_admin = true ) { is_locked_for_admin = lock_for_admin; }

   void dump_locks( ostream& os ) const;

   bool obtain_lock( size_t& handle, const string& lock_class, const string& lock_instance,
    op_lock::lock_type type, session* p_session, class_base* p_class_base = 0, class_base* p_root_class_base = 0 );

   void transform_lock( size_t handle, op_lock::lock_type new_type );

   void release_lock( size_t handle, bool force_removal = false );

   op_lock get_lock_info( size_t handle ) const;
   op_lock get_lock_info( const string& lock_class, const string& lock_instance ) const;

   op_lock get_lock_info_for_owner( const string& lock_class, const string& lock_instance, class_base& owner ) const;

   void release_locks_for_owner( class_base& owner, bool force_removal = false );

   void release_locks_for_commit( session* p_session );
   void release_locks_for_rollback( session* p_session );

   set< string >& get_dead_keys( ) { return dead_keys; }

   private:
   size_t slot;
   string name;

   ods* p_ods;
   size_t ref_count;
   ods::bulk_write* p_bulk_write;

   ofstream log_file;
   ofstream* p_alternative_log_file;

   storable_root root;

   size_t next_lock_handle;

   bool is_locked_for_admin;

   mutable mutex lock_mutex;
   mutable mutex cache_mutex;

   lock_container locks;
   lock_index_container lock_index;

   set< size_t > lock_duplicates;

   set< string > dead_keys;

   storage_handler( const storage_handler& );
   storage_handler& operator ==( const storage_handler& );
};

void storage_handler::dump_locks( ostream& os ) const
{
   os << "handle key (lock_class:instance)                     type       tx_type    tran_id    tran_level p_session p_class_base p_root_class\n";
   os << "------ --------------------------------------------- ---------- ---------- ---------- ---------- --------- ------------ ------------\n";

   lock_index_const_iterator lici;
   for( lici = lock_index.begin( ); lici != lock_index.end( ); ++lici )
   {
      op_lock& next_lock( lici->second->second );

      os.setf( ios::left );

      os << setw( 6 ) << lici->first
       << ' ' << setw( 45 ) << lici->second->first
       << ' ' << setw( 10 ) << op_lock::lock_type_name( next_lock.type )
       << ' ' << setw( 10 ) << op_lock::lock_type_name( next_lock.tx_type )
       << ' ' << setw( 10 ) << next_lock.transaction_id
       << ' ' << setw( 10 ) << next_lock.transaction_level << ' ' << next_lock.p_session
       << "  " << next_lock.p_class_base << "     " << next_lock.p_root_class << '\n';
   }
}

bool storage_handler::obtain_lock( size_t& handle,
 const string& lock_class, const string& lock_instance,
 op_lock::lock_type type, session* p_session, class_base* p_class_base, class_base* p_root_class )
{
   int attempts = c_max_lock_attempts;

   TRACE_LOG( TRACE_LOCK_OPS, "[obtain lock] class = " + lock_class
    + ", instance = " + lock_instance + ", type = " + to_string( type ) );

   bool found = false;
   while( attempts-- )
   {
      // NOTE: Empty scope for lock object.
      {
         guard g( lock_mutex );

         ods* p_ods( ods::instance( ) );

         string key( lock_class );
         key += ':';
         key += lock_instance;

         lock_iterator li( locks.lower_bound( lock_class ) );

         // NOTE: Check existing locks of the same class for a conflicting lock
         // (an empty lock instance is treated as a class-wide lock).
         op_lock last_lock;
         bool lock_conflict = false;

         while( li != locks.end( ) )
         {
            string next_lock_class, next_lock_instance;
            string::size_type pos = li->first.find( ':' );
            if( pos == string::npos )
               throw runtime_error( "lock key '" + li->first + "' is missing class/instance separator" );

            next_lock_class = li->first.substr( 0, pos );
            next_lock_instance = li->first.substr( pos + 1 );

            if( lock_class != next_lock_class )
               break;

            if( !lock_instance.empty( ) && !next_lock_instance.empty( ) && lock_instance != next_lock_instance )
            {
               // NOTE: If both locks being compared are instance locks then if greater finish or if less then
               // skip to the first instance that is equal or greater (as only equal instance locks can clash).
               if( next_lock_instance > lock_instance )
                  break;
               else
               {
                  li = locks.lower_bound( key );
                  continue;
               }
            }

            op_lock& next_lock( li->second );

            // NOTE: Locks that are effectively "dead" (i.e. awaiting cleanup when the tx completes) can
            // result in increasingly poorer performance if duplicates are allowed to exist (due to lock
            // clash scanning) therefore remove any duplicates as discovered.
            if( next_lock.type == op_lock::e_lock_type_none && next_lock == last_lock )
            {
               lock_duplicates.insert( next_lock.handle );

               lock_index.erase( next_lock.handle );
               locks.erase( li++ );

               continue;
            }

            // NOTE: Cascade locks will be ignored within the same session as it can sometimes be necessary for
            // update or delete operations to occur on an already cascade locked instance. Locks that are being
            // held for the duration of a transaction are ignored within the same session.
            // FUTURE: The ignoring of cascade locks would need to be reviewed if it became desirable to permit
            // multiple separate operations (belonging to the same session) to occur within a single command.
            if( ( lock_instance.empty( ) || next_lock_instance.empty( ) || lock_instance == next_lock_instance )
             && ( ( !locks_can_coexist( type, next_lock.type )
             && ( p_session != next_lock.p_session || p_root_class || p_root_class == next_lock.p_root_class ) )
             || ( next_lock.tx_type && p_session != next_lock.p_session && !locks_can_coexist( type, next_lock.tx_type ) ) ) )
            {
               // NOTE: Allow "update" locks to be obtained for an already "update" locked instance provided
               // that the instances are separate but owned by the same session and that the existing locked
               // instance is in the "after_store" trigger. Allow "review" locks for separate instances that
               // are owned by the same session (even if class locked).
               if( p_session != next_lock.p_session
                || ( p_class_base == next_lock.p_class_base )
                || ( type != next_lock.type && type != op_lock::e_lock_type_review )
                || ( !next_lock.p_class_base && type != op_lock::e_lock_type_review )
                || ( type != op_lock::e_lock_type_review && type != op_lock::e_lock_type_update )
                || ( type != op_lock::e_lock_type_review && !next_lock.p_class_base->get_is_after_store( ) ) )
               {
                  lock_conflict = true;
                  if( !attempts )
                     TRACE_LOG( TRACE_LOCK_OPS, "*** failed to acquire lock ***" );
                  break;
               }
            }

            last_lock = next_lock;
            ++li;
         }

         if( !lock_conflict )
         {
            int_t tran_id( p_ods->get_transaction_id( ) );
            int_t tran_level( p_ods->get_transaction_level( ) );

            li = locks.insert( lock_value_type( key,
             op_lock( ++next_lock_handle, type, tran_id, tran_level, p_session, p_class_base, p_root_class ) ) );

            lock_index.insert( lock_index_value_type( next_lock_handle, li ) );

            handle = next_lock_handle;
            found = true;
            break;
         }
      }

      msleep( c_lock_attempt_sleep_time );
   }

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dumping locks]\n" + osstr.str( ) );
   }

   return found;
}

void storage_handler::transform_lock( size_t handle, op_lock::lock_type new_type )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[transform lock] handle = "
    + to_string( handle ) + ", new_type = " + to_string( new_type ) );

   if( handle && lock_duplicates.find( handle ) == lock_duplicates.end( ) )
   {
      lock_index_iterator lii = lock_index.find( handle );
      if( lii != lock_index.end( ) )
      {
         lock_iterator li( lii->second );
         li->second.type = new_type;
      }
   }

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_lock( size_t handle, bool force_removal )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[release lock] handle = "
    + to_string( handle ) + ", force_removal = " + to_string( force_removal ) );

   if( handle && lock_duplicates.find( handle ) == lock_duplicates.end( ) )
   {
      lock_index_iterator lii = lock_index.find( handle );
      if( lii != lock_index.end( ) )
      {
         lock_iterator li( lii->second );
         if( !force_removal && li->second.transaction_level > 0 )
            li->second.type = op_lock::e_lock_type_none;
         else
         {
            lock_index.erase( lii );
            locks.erase( li );
         }
      }
   }

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
   }
}

op_lock storage_handler::get_lock_info( size_t handle ) const
{
   guard g( lock_mutex );

   lock_index_const_iterator lici = lock_index.find( handle );
   if( lici == lock_index.end( ) )
      throw runtime_error( "unable to locate lock handle #" + to_string( handle ) );

   return ( lici->second )->second;
}

op_lock storage_handler::get_lock_info( const string& lock_class, const string& lock_instance ) const
{
   guard g( lock_mutex );

   string key( lock_class );
   key += ':';
   key += lock_instance;

   op_lock lock;
   for( lock_const_iterator lci = locks.lower_bound( key ), end = locks.end( ); lci != end; ++lci )
   {
      if( lci->first != key )
         break;

      // NOTE: If more than one matching lock exists then return the strongest lock type.
      if( lci->second.type > lock.type )
         lock = lci->second;
   }

   return lock;
}

op_lock storage_handler::get_lock_info_for_owner( const string& lock_class, const string& lock_instance, class_base& owner ) const
{
   guard g( lock_mutex );

   string key( lock_class );
   key += ':';
   key += lock_instance;

   op_lock lock;
   for( lock_const_iterator lci = locks.lower_bound( key ), end = locks.end( ); lci != end; ++lci )
   {
      if( lci->first != key )
         break;

      if( lci->second.p_root_class == &owner )
      {
         lock = lci->second;
         break;
      }
   }

   return lock;
}

void storage_handler::release_locks_for_owner( class_base& owner, bool force_removal )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[release locks for owner] owner = "
    + to_string( &owner ) + ", force_removal = " + to_string( force_removal ) );

   lock_index_iterator lii;
   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_root_class == &owner )
      {
         if( !force_removal && next_lock.transaction_level > 0 )
         {
            next_lock.type = op_lock::e_lock_type_none;
            next_lock.p_root_class = 0;
         }
         else
         {
            locks.erase( lii->second );
            lock_index.erase( lii++ );
         }
      }
      else
         ++lii;
   }

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_locks_for_commit( session* p_session )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[release locks for commit] p_session = " + to_string( p_session ) );

   ods* p_ods( ods::instance( ) );

   lock_index_iterator lii;
   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_session == p_session
       && next_lock.transaction_id == p_ods->get_transaction_id( )
       && next_lock.transaction_level >= p_ods->get_transaction_level( ) )
      {
         if( p_ods->get_transaction_level( ) > 1 )
         {
            next_lock.transaction_level = p_ods->get_transaction_level( ) - 1;
            next_lock.type = op_lock::e_lock_type_none;
            ++lii;
         }
         else
         {
            locks.erase( lii->second );
            lock_index.erase( lii++ );
         }
      }
      else
         ++lii;
   }

   lock_duplicates.clear( );

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
   }
}

void storage_handler::release_locks_for_rollback( session* p_session )
{
   guard g( lock_mutex );

   TRACE_LOG( TRACE_LOCK_OPS, "[release locks for rollback] p_session = " + to_string( p_session ) );

   ods* p_ods( ods::instance( ) );

   lock_index_iterator lii;
   for( lii = lock_index.begin( ); lii != lock_index.end( ); )
   {
      op_lock& next_lock( lii->second->second );

      if( next_lock.p_session == p_session
       && next_lock.transaction_id == p_ods->get_transaction_id( )
       && next_lock.transaction_level >= p_ods->get_transaction_level( ) )
      {
         locks.erase( lii->second );
         lock_index.erase( lii++ );
      }
      else
         ++lii;
   }

   lock_duplicates.clear( );

   IF_IS_TRACING( TRACE_LOCK_OPS )
   {
      ostringstream osstr;
      dump_locks( osstr );

      TRACE_LOG( TRACE_LOCK_OPS, "[dump_locks]\n" + osstr.str( ) );
   }
}

struct scoped_lock_holder
{
   scoped_lock_holder( storage_handler& handler, size_t lock_handle )
    :
    handler( handler ),
    lock_handle( lock_handle )
   {
   }

   ~scoped_lock_holder( )
   {
      if( lock_handle )
         handler.release_lock( lock_handle, true );
   }

   void release( )
   {
      lock_handle = 0;
   }

   storage_handler& handler;
   size_t lock_handle;
};

// NOTE: If max sessions is set to a value greater than 1000 then would need to change "gen_key".
const size_t c_max_sessions_limit = 1000;
const size_t c_max_sessions_default = 100;
const size_t c_max_storage_handlers_default = 10;

string g_empty_string;

size_t g_max_sessions = c_max_sessions_default;
size_t g_max_storage_handlers = c_max_storage_handlers_default + 1; // i.e. extra for <none>

const char* const c_default_storage_name = "<none>";
const char* const c_default_storage_identity = "<default>";

const char* const c_ignore_field = "@ignore";

mutex g_mutex;
mutex g_trace_mutex;

size_t g_next_session_id;

map< string, string > g_variables;

typedef vector< session* > session_container;

session_container g_sessions;

map< size_t, date_time > g_condemned_sessions;

static TLS( session )* gtp_session;

typedef vector< storage_handler* > storage_handler_container;

typedef map< string, size_t > storage_handler_index_container;
typedef storage_handler_index_container::iterator storage_handler_index_iterator;
typedef storage_handler_index_container::const_iterator storage_handler_index_const_iterator;
typedef storage_handler_index_container::value_type storage_handler_index_value_type;

storage_handler_container g_storage_handlers;
storage_handler_index_container g_storage_handler_index;

enum storage_op
{
   e_storage_op_init,
   e_storage_op_create,
   e_storage_op_attach
};

void perform_storage_op( storage_op op,
 const string& name, const string& directory, command_handler& cmd_handler, bool lock_for_admin )
{
   guard g( g_mutex );

   storage_handler* p_new_handler = 0;

   if( name == c_default_storage_name )
      throw runtime_error( "storage name '" + name + "' cannot be used explicitly" );

   if( !lock_for_admin && file_exists( name + ".log.new" ) )
      throw runtime_error( "storage is under administration" );

   if( !gtp_session->instance_registry.empty( ) )
      throw runtime_error( "all object instances must be destroyed before linking to a storage" );

   if( gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "current storage needs to be terminated before another can be linked to" );

   bool was_constructed = false;
   if( g_storage_handler_index.find( name ) != g_storage_handler_index.end( ) )
   {
      if( op == e_storage_op_create )
         throw runtime_error( "storage '" + name + "' cannot be created as its already in use" );

      if( lock_for_admin )
         throw runtime_error( "storage '" + name + "' cannot be administered as its already in use" );

      p_new_handler = g_storage_handlers[ g_storage_handler_index[ name ] ];
   }
   else
   {
      size_t slot = 0;
      for( slot = 1; slot < g_max_storage_handlers; slot++ )
      {
         if( !g_storage_handlers[ slot ] )
            break;
      }

      if( !slot || slot == g_max_storage_handlers )
         throw runtime_error( "max. permitted concurrent storage handlers already active" );

      ods::open_mode open_mode;

      if( op == e_storage_op_attach )
         open_mode = ods::e_open_mode_exist;
      else if( op == e_storage_op_create )
         open_mode = ods::e_open_mode_not_exist;
      else if( !lock_for_admin )
         open_mode = ods::e_open_mode_exist;
      else
         open_mode = ods::e_open_mode_create_if_not_exist;

      try
      {
         auto_ptr< ods > ap_ods( new ods( name.c_str( ), open_mode, ods::e_share_mode_exclusive ) );
         auto_ptr< storage_handler > ap_handler( new storage_handler( slot, name, ap_ods.get( ) ) );

         ap_handler->obtain_bulk_write( );

         // NOTE: In case a server shutdown had occurred whilst an ODS transaction was still active.
         if( !ap_ods->is_new( ) )
            ap_ods->rollback_dead_transactions( );

         ods::instance( ap_ods.get( ) );

         if( ap_ods->is_new( ) )
         {
            ods::transaction tx( *ap_ods );

            ap_handler->get_root( ).set_new( );
            ap_handler->get_root( ).module_directory = directory;
            *ap_ods << ap_handler->get_root( );

            // NOTE: Create the storage "identity" object then store the root object again (so that
            // the root object is the first and the identity object is the second within the storage).
            ap_handler->get_root( ).o_identity.reset( new storable_identity );
            ap_handler->get_root( ).o_identity.store( );
            *ap_ods << ap_handler->get_root( );

            tx.commit( );
         }
         else
         {
            ap_handler->get_root( ).set_id( 0 );
            *ap_ods >> ap_handler->get_root( );

            if( ap_handler->get_root( ).version != c_storage_format_version )
               throw runtime_error( "found incorrect storage format version " + to_string( ap_handler->get_root( ).version ) );

            // NOTE: Force an identity write to occur when the first transaction is logged.
            ap_handler->get_root( ).o_identity->ceiling = ap_handler->get_root( ).o_identity->next_id;
         }

         string dead_keys_file( ap_handler->get_name( ) + c_dead_keys_ext );
         if( exists_file( dead_keys_file ) )
         {
            vector< string > lines;
            buffer_file_lines( dead_keys_file, lines );

            for( size_t i = 0; i < lines.size( ); i++ )
            {
               if( !lines[ i ].empty( ) )
               {
                  string next( lines[ i ] );

                  string::size_type pos = next.find( ':' );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected invalid dead key format '" + next + "'" );


                  string class_id = next.substr( 0, pos );

                  ap_handler->get_dead_keys( ).insert( next );
               }
            }

            buffer_file_lines( dead_keys_file, ap_handler->get_dead_keys( ) );
         }

         p_new_handler = ap_handler.release( );

         was_constructed = true;

         g_storage_handlers[ slot ] = p_new_handler;
         g_storage_handler_index.insert( make_pair( name, slot ) );

         ap_ods.release( );
      }
      catch( ods_error& err )
      {
         ods::instance( 0, true );
         throw runtime_error( err.what( ) );
      }
   }

   if( p_new_handler && p_new_handler != gtp_session->p_storage_handler )
   {
      if( p_new_handler->get_is_locked_for_admin( ) )
         throw runtime_error( "storage '" + name + "' is currently locked for administration" );

      bool created_ods_instance = false;
      storage_handler* p_old_handler = gtp_session->p_storage_handler;
      try
      {
         gtp_session->ap_db.reset( new sql_db( p_new_handler->get_name( ), p_new_handler->get_name( ) ) );

         ods::instance( new ods( *p_new_handler->get_ods( ) ) );
         created_ods_instance = true;

         gtp_session->p_storage_handler = p_new_handler;
         gtp_session->p_storage_handler->inc_ref_count( );

         if( module_count( ) && !p_new_handler->get_root( ).module_directory.empty( ) )
            throw runtime_error( "storages with a module directory cannot be initialised whilst modules are already loaded" );

         module_directory( &p_new_handler->get_root( ).module_directory );

         // NOTE: Modules that have been registered to this storage are now automatically loaded (if not already present).
         // This is performed in the same order that the modules were registered in as dependencies may exist between them.
         size_t num_modules( p_new_handler->get_root( ).module_list.size( ) );
         for( size_t i = 0; i < num_modules; i++ )
         {
            string next_module( p_new_handler->get_root( ).module_list[ i ] );
            if( gtp_session->modules_by_name.find( next_module ) == gtp_session->modules_by_name.end( ) )
            {
               module_load( next_module, cmd_handler );
               gtp_session->storage_controlled_modules.push_back( next_module );
            }
         }

         if( lock_for_admin )
            gtp_session->p_storage_handler->set_is_locked_for_admin( );

         set_session_variable( c_session_variable_storage, gtp_session->p_storage_handler->get_name( ) );
      }
      catch( ... )
      {
         while( !gtp_session->storage_controlled_modules.empty( ) )
         {
            module_unload( gtp_session->storage_controlled_modules.back( ), cmd_handler );
            gtp_session->storage_controlled_modules.pop_back( );
         }

         p_new_handler->release_bulk_write( );
         p_new_handler->release_ods( );

         if( created_ods_instance )
         {
            delete ods::instance( );
            ods::instance( 0, true );
         }

         g_storage_handler_index.erase( p_new_handler->get_name( ) );
         g_storage_handlers[ p_new_handler->get_slot( ) ] = 0;

         if( was_constructed )
            delete p_new_handler;

         gtp_session->p_storage_handler = p_old_handler;

         throw;
      }
   }
}

bool fetch_instance_from_db( class_base& instance,
 const map< int, int >& fields, const vector< int >& columns, bool skip_after_fetch )
{
   class_base_accessor instance_accessor( instance );
   sql_dataset& ds( *instance_accessor.p_sql_dataset( ) );

   bool found = ds.next( );
   instance_accessor.clear( );

   if( !found )
   {
      delete instance_accessor.p_sql_dataset( );
      instance_accessor.p_sql_dataset( ) = 0;

      instance_accessor.after_fetch_from_db( );

      if( !skip_after_fetch )
         instance_accessor.perform_after_fetch( );
   }
   else
   {
      instance_accessor.set_key( ds.as_string( 0 ), true );
      instance_accessor.set_version( ds.as_int( 1 ) );
      instance_accessor.set_revision( ds.as_int( 2 ) );
      instance_accessor.set_original_identity( ds.as_string( 3 ) );

      for( int i = 4; i < ds.get_fieldcount( ); i++ )
      {
         if( !fields.count( columns[ i - 4 ] ) )
            throw runtime_error( "unexpected field # not found for column #" + to_string( i - 4 ) );

         int fnum( fields.find( columns[ i - 4 ] )->second );

         TRACE_LOG( TRACE_SQLCLSET, "setting field #" + to_string( fnum + 1 ) + " to " + ds.as_string( i ) );
         instance.set_field_value( fnum, ds.as_string( i ) );
      }

      instance_accessor.after_fetch_from_db( );

      if( !skip_after_fetch )
         instance_accessor.perform_after_fetch( );
   }

   return found;
}

bool fetch_instance_from_db( class_base& instance,
 const string& sql, bool check_only = false, bool is_minimal_fetch = false )
{
   bool found = false;
   class_base_accessor instance_accessor( instance );

   if( gtp_session && gtp_session->ap_db.get( ) )
   {
      TRACE_LOG( TRACE_SQLSTMTS, sql );

      sql_dataset ds( *gtp_session->ap_db.get( ), sql );
      found = ds.next( );

      ++gtp_session->sql_count;

      if( !check_only )
      {
         instance_accessor.clear( );

         if( found )
         {
            instance_accessor.set_key( ds.as_string( 0 ), true );
            instance_accessor.set_version( ds.as_int( 1 ) );
            instance_accessor.set_revision( ds.as_int( 2 ) );
            instance_accessor.set_original_identity( ds.as_string( 3 ) );

            int fnum = 4;
            for( int i = fnum; i < ds.get_fieldcount( ); i++, fnum++ )
            {
               while( instance.is_field_transient( fnum - 4 ) )
                  fnum++;

               TRACE_LOG( TRACE_SQLCLSET, "setting field #" + to_string( fnum - 4 + 1 ) + " to " + ds.as_string( i ) );
               instance.set_field_value( fnum - 4, ds.as_string( i ) );
            }
         }

         instance_accessor.after_fetch_from_db( );
         instance_accessor.perform_after_fetch( is_minimal_fetch );
      }
   }

   return found;
}

bool is_child_constrained( class_base& instance,
 class_base& root_instance, string& constraining_class, map< string, set< string > >& instance_keys )
{
   class_base_accessor instance_accessor( instance );

   class_base* p_class_base;
   string sql, next_child_field;

   size_t num_children = instance_accessor.get_num_foreign_key_children( );
   if( num_children && !instance_keys[ instance.class_id( ) ].count( instance.get_key( ) ) )
   {
      for( int pass = 0; pass < 2; ++pass )
      {
         cascade_op next_op;
         if( pass == 0 )
            next_op = e_cascade_op_restrict;
         else
            next_op = e_cascade_op_destroy;

         for( size_t i = 0; i < num_children; i++ )
         {
            p_class_base = instance_accessor.get_next_foreign_key_child( i, next_child_field, next_op );

            auto_ptr< class_cascade > ap_tmp_cascading;
            if( p_class_base )
               ap_tmp_cascading.reset( new class_cascade( *p_class_base ) );

            // FUTURE: The handling of cascades needs to be revisited at some stage to improve performance
            // as currently iteration occurs three times (firstly to lock, secondly to test if constrained
            // and thirdly for the actual deletions).
            if( p_class_base
             && p_class_base->iterate_forwards( "", c_key_field, true, 0, e_sql_optimisation_unordered ) )
            {
               class_base_accessor child_instance_accessor( *p_class_base );

               if( next_op == e_cascade_op_restrict )
               {
                  storage_handler& handler( *gtp_session->p_storage_handler );

                  do
                  {
                     // NOTE: If the instance that would restrict has already been locked for destroy then
                     // it can be safely ignored (i.e. continue iteration to find the next instance). Once
                     // an actual restricting instance is found then no further iteration is required.
                     op_lock lock = handler.get_lock_info_for_owner(
                      p_class_base->lock_class_id( ), p_class_base->get_key( ), root_instance );

                     if( lock.type != op_lock::e_lock_type_destroy )
                     {
                        constraining_class = p_class_base->get_display_name( );
                        p_class_base->iterate_stop( );
                        return true;
                     }
                  } while( p_class_base->iterate_next( ) );
               }
               else
               {
                  do
                  {
                     // NOTE: If the child instance is actually a derived class then get the derived object.
                     sql.erase( );
                     class_base* p_dyn_class_base( child_instance_accessor.fetch_dynamic_instance( sql, false ) );

                     // FUTURE: This fetch should not actually be necessary as long as the key has been set.
                     if( !sql.empty( ) )
                        fetch_instance_from_db( *p_dyn_class_base, sql );

                     if( next_op == e_cascade_op_destroy )
                        instance_keys[ p_class_base->class_id( ) ].insert( p_class_base->get_key( ) );

                     if( is_child_constrained( *p_dyn_class_base, root_instance, constraining_class, instance_keys ) )
                     {
                        p_class_base->iterate_stop( );
                        return true;
                     }

                     child_instance_accessor.destroy_dynamic_instance( );

                  } while( p_class_base->iterate_next( ) );
               }
            }
         }
      }
   }

   return false;
}

bool obtain_cascade_locks_for_destroy( class_base& instance,
 class_base& root_instance, map< string, set< string > >& instance_keys )
{
   class_base_accessor instance_accessor( instance );

   size_t lock_handle;
   class_base* p_class_base;
   string sql, next_child_field;

   storage_handler& handler( *gtp_session->p_storage_handler );

   size_t num_children = instance_accessor.get_num_foreign_key_children( );
   if( num_children && !instance_keys[ instance.class_id( ) ].count( instance.get_key( ) ) )
   {
      for( int pass = 0; pass < 2; ++pass )
      {
         cascade_op next_op;
         if( pass == 0 )
            next_op = e_cascade_op_destroy;
         else
            next_op = e_cascade_op_unlink;

         for( size_t i = 0; i < num_children; i++ )
         {
            p_class_base = instance_accessor.get_next_foreign_key_child( i, next_child_field, next_op );

            auto_ptr< class_cascade > ap_tmp_cascading;
            if( p_class_base )
               ap_tmp_cascading.reset( new class_cascade( *p_class_base ) );

            // FUTURE: The handling of cascades needs to be revisited at some stage to improve performance
            // as currently iteration occurs three times (firstly to lock, secondly to test if constrained
            // and thirdly for the actual deletions).
            if( p_class_base
             && p_class_base->iterate_forwards( "", c_key_field, true, 0, e_sql_optimisation_unordered ) )
            {
               class_base_accessor child_instance_accessor( *p_class_base );

               do
               {
                  // NOTE: A lock may have already been obtained during the recursion due to multiple
                  // relationship paths leading to the same instance - if found then simply continue.
                  op_lock lock = handler.get_lock_info_for_owner(
                   p_class_base->lock_class_id( ), p_class_base->get_key( ), root_instance );

                  if( lock.type != op_lock::e_lock_type_none )
                     continue;

                  // NOTE: It is possible that the original instance being destroyed (for which the lock
                  // is not marked class "owned") may also be subject to unlinking (due to the existence
                  // of a relationship to a child instance) so it's being assumed here that if a destroy
                  // lock belonging to the same session is found for this instance then this is the case.
                  if( pass == 1 )
                  {
                     // FUTURE: Although extremely unlinkely it's possible for the destroy lock to not be
                     // that of the original operation (i.e. instead part of a completely separate op for
                     // the same session). As the key protocol commands only deal with a single operation
                     // the assumption that the lock is part of the same operation is fine, however, were
                     // it to become desirable for multiple operations (belonging to the same session) to
                     // be supported then this assumption could be incorrect and lead to potential issues.
                     lock = handler.get_lock_info( p_class_base->lock_class_id( ), p_class_base->get_key( ) );
                     if( lock.p_session == gtp_session && lock.type == op_lock::e_lock_type_destroy )
                        continue;
                  }

                  if( next_op == e_cascade_op_unlink )
                  {
                     if( !handler.obtain_lock( lock_handle, p_class_base->lock_class_id( ),
                      p_class_base->get_key( ), op_lock::e_lock_type_update, gtp_session, &instance, &root_instance ) )
                     {
                        p_class_base->iterate_stop( );
                        return false;
                     }
                  }
                  else
                  {
                     if( handler.obtain_lock( lock_handle, p_class_base->lock_class_id( ),
                      p_class_base->get_key( ), op_lock::e_lock_type_destroy, gtp_session, &instance, &root_instance ) )
                     {
                        // NOTE: If the child instance is actually a derived class then get the derived object.
                        sql.erase( );
                        class_base* p_dyn_class_base( child_instance_accessor.fetch_dynamic_instance( sql, false ) );

                        // FUTURE: This fetch should not actually be necessary as long as the key has been set.
                        if( !sql.empty( ) )
                           fetch_instance_from_db( *p_dyn_class_base, sql );

                        if( next_op == e_cascade_op_destroy )
                           instance_keys[ p_class_base->class_id( ) ].insert( p_class_base->get_key( ) );

                        if( !obtain_cascade_locks_for_destroy( *p_dyn_class_base, root_instance, instance_keys ) )
                        {
                           p_class_base->iterate_stop( );
                           return false;
                        }

                        child_instance_accessor.destroy_dynamic_instance( );
                     }
                     else
                     {
                        p_class_base->iterate_stop( );
                        return false;
                     }
                  }
               } while( p_class_base->iterate_next( ) );
            }
         }
      }
   }

   return true;
}

enum permit_op_type_value
{
   e_permit_op_type_value_any,
   e_permit_op_type_value_none,
   e_permit_op_type_value_review,
   e_permit_op_type_value_create_update_destroy
};

class_base& get_class_base_from_handle( size_t handle, const string& context )
{
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );
   object_instance_registry_iterator oiri = instance_registry.find( handle );
   if( oiri == instance_registry.end( ) )
      throw runtime_error( "invalid object instance handle #" + to_string( handle ) );

   class_base* p_class_base( ( oiri->second ).p_class_base );
   p_class_base = p_class_base->get_dynamic_instance( );

   if( !context.empty( ) )
      p_class_base = &p_class_base->get_or_create_graph_child( context );

   return *p_class_base;
}

class_base& get_class_base_from_handle_for_op( size_t handle,
 const string& context, permit_op_type_value permit = e_permit_op_type_value_none, bool use_dynamic_context = true )
{
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );
   object_instance_registry_iterator oiri = instance_registry.find( handle );
   if( oiri == instance_registry.end( ) )
      throw runtime_error( "invalid object instance handle #" + to_string( handle ) );

   class_base* p_class_base( ( oiri->second ).p_class_base );

   if( !context.empty( ) )
      p_class_base = &p_class_base->get_or_create_graph_child( context );

   // NOTE: If not wanting to use a dynamic instance then provided it isn't already in use for an
   // operation then discard it (if one exists). This could be used to support "dynamic" instance
   // iteration, however, unless a second SELECT can be issued to get the derived instance's data
   // there is probably no real benefit to supporting it (but perhaps some other future use might
   // benefit from this behaviour).
   class_base* p_dynamic_base = p_class_base->get_dynamic_instance( );
   if( !use_dynamic_context )
   {
      if( p_class_base != p_dynamic_base && p_dynamic_base->get_op( ) == class_base::e_op_type_none )
         class_base_accessor( *p_class_base ).destroy_dynamic_instance( );
      else
         p_class_base = p_dynamic_base;
   }
   else
      p_class_base = p_dynamic_base;

   class_base::op_type op( p_class_base->get_op( ) );

   if( permit == e_permit_op_type_value_none && op != class_base::e_op_type_none )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   if( permit != e_permit_op_type_value_none && op == class_base::e_op_type_none )
      throw runtime_error( "object instance #" + to_string( handle ) + " is not currently involved in an operation" );

   if( permit == e_permit_op_type_value_review && op != class_base::e_op_type_review )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   if( permit == e_permit_op_type_value_create_update_destroy
    && op != class_base::e_op_type_create && op != class_base::e_op_type_update && op != class_base::e_op_type_destroy )
      throw runtime_error( "object instance #" + to_string( handle ) + " is currently involved in another operation" );

   return *p_class_base;
}

size_t obtain_keyed_lock( const string& lock_class, const string& key, op_lock::lock_type lock_type )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   size_t lock_handle( 0 );
   handler.obtain_lock( lock_handle, lock_class, key, lock_type, gtp_session );
   return lock_handle;
}

inline void perform_op_cancel( storage_handler& handler, class_base& instance, class_base::op_type op )
{
   class_base_accessor instance_accessor( instance );

   // FUTURE: It would be more efficient not to call "release_locks_for_owner" unless
   // it is known that cascade locks had actually been obtained when the destroy began.
   if( op == class_base::e_op_type_destroy && !instance.get_is_being_cascaded( ) )
      handler.release_locks_for_owner( instance, true );

   instance_accessor.cancel( );
}

inline string get_field_name( class_base& instance,
 string& field_name, bool* p_sql_numeric = 0, string* p_type_name = 0 )
{
   const char* p_str;
   bool was_name = true;
   class_base_accessor instance_accessor( instance );

   if( field_name == c_key_field )
      field_name = "Key_";

   if( field_name == "Key_" || field_name == "Typ_" )
   {
      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( field_name == "Ver_" || field_name == "Rev_" )
   {
      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else
   {
      p_str = instance_accessor.get_field_name( field_name, p_sql_numeric, p_type_name );

      if( p_str )
         field_name = p_str;
      else
         p_str = instance_accessor.get_field_id( field_name, p_sql_numeric, p_type_name );

      if( !p_str )
         throw runtime_error( "field name/id " + field_name + " is invalid" );
   }

   return field_name;
}

void setup_select_columns( class_base& instance, const vector< string >& field_info )
{
   class_base_accessor instance_accessor( instance );

   vector< int > column_nums;
   vector< string > column_names;

   map< int, int > field_nums;
   map< string, int > field_names;
   int num_fields = instance.get_num_fields( );
   for( int i = 0, c = 0; i < num_fields; i++ )
   {
      if( instance.is_field_transient( i ) )
         continue;

      field_nums.insert( make_pair( c++, i ) );
      field_names.insert( make_pair( instance.get_field_name( i ), i ) );
   }

   instance.get_sql_column_names( column_names );

   if( field_info.empty( ) )
   {
      for( size_t i = 0; i < column_names.size( ); i++ )
         column_nums.push_back( i );
   }
   else
   {
      field_nums.clear( );

      for( size_t i = 0; i < field_info.size( ); i++ )
      {
         string next_field( field_info[ i ] );
         get_field_name( instance, next_field );

         string column_name( "C_" + next_field );

         for( size_t j = 0; j < column_names.size( ); j++ )
         {
            if( column_name == column_names[ j ] )
            {
               column_nums.push_back( j );
               field_nums.insert( make_pair( j, field_names[ next_field ] ) );
            }
         }
      }

      if( column_nums.size( ) != field_info.size( ) )
         throw runtime_error( "unexpected fields/columns size mismatch" );
   }

   instance_accessor.select_fields( ) = field_nums;
   instance_accessor.select_columns( ) = column_nums;
}

void split_key_info( const string& key_info,
 vector< pair< string, string > >& fixed_info,
 vector< pair< string, string > >& paging_info,
 vector< string >& order_info, bool do_not_add_key_to_order )
{
   string::size_type pos = key_info.find( ' ' );
   if( pos == string::npos )
   {
      if( !key_info.empty( ) )
         fixed_info.push_back( make_pair( "Key_", key_info ) );

      // NOTE: The primary key is always being added to the end of the order info as it
      // is not known here if the query will have repeatably ordered results without it.
      if( !do_not_add_key_to_order )
         order_info.push_back( "Key_" );
   }
   else
   {
      int num_fixed = 0;
      string fields( key_info.substr( 0, pos ) );
      string values( key_info.substr( pos + 1 ) );

      pos = fields.find( '#' );
      if( pos != string::npos )
      {
         num_fixed = atoi( fields.substr( pos + 1 ).c_str( ) );
         fields.erase( pos );
      }

      size_t order_provided = order_info.size( );
      split( fields, order_info );

      // NOTE: The primary key is always being added to the end of the order info as it
      // is not known here if the query will have repeatably ordered results without it.
      if( !do_not_add_key_to_order )
         order_info.push_back( "Key_" );

      if( values.empty( ) )
      {
         if( num_fixed > 1 )
            throw runtime_error( "specified # of fixed values were not provided" );
         else if( num_fixed == 1 )
         {
            if( order_info.size( ) <= order_provided + 1 )
               throw runtime_error( "missing expected ordering field" );
            fixed_info.push_back( make_pair( order_info[ order_provided ], string( ) ) );
         }
      }
      else
      {
         vector< string > all_values;
         split( values, all_values );

         if( all_values.size( ) < num_fixed )
            throw runtime_error( "specified # of fixed values were not provided" );

         if( order_info.size( ) < all_values.size( ) + order_provided )
            throw runtime_error( "not enough order info for all provided values" );

         for( size_t i = 0; i < all_values.size( ); i++ )
         {
            if( i < num_fixed )
               fixed_info.push_back( make_pair( order_info[ order_provided + i ], all_values[ i ] ) );
            else
               paging_info.push_back( make_pair( order_info[ order_provided + i ], all_values[ i ] ) );
         }
      }
   }
}

void split_text_search( const string& text_search, vector< string >& words )
{
   string next_word;
   bool in_quotes = false;
   bool had_escape = false;
   for( size_t i = 0; i < text_search.size( ); i++ )
   {
      char ch = text_search[ i ];

      if( had_escape )
      {
         had_escape = false;
         next_word += ch;
      }
      else if( ch == '\\' )
         had_escape = true;
      else
      {
         if( ch == '"' )
         {
            in_quotes = !in_quotes;
            if( !in_quotes )
            {
               if( !next_word.empty( ) )
               {
                  words.push_back( next_word );
                  next_word.erase( );
               }
            }
            else if( i < text_search.size( ) - 1 && text_search[ i + 1 ] == '"' )
            {
               in_quotes = false;
               next_word += ch;
               ++i;
            }
         }
         else
         {
            if( ch == ' ' && !in_quotes )
            {
               if( !next_word.empty( ) )
               {
                  words.push_back( next_word );
                  next_word.erase( );
               }
            }
            else
               next_word += ch;
         }
      }
   }

   if( !next_word.empty( ) )
      words.push_back( next_word );
}

size_t split_csv_values( const string& line,
 vector< string >& values, bool& last_value_incomplete, size_t continuation_offset )
{
   string next_value;

   // NOTE: If the last value was continued into the next line then only process the
   // portion that has not already been processed (i.e. continue where it left off).
   if( continuation_offset )
      next_value = line.substr( 0, continuation_offset );

   bool in_quotes = last_value_incomplete;
   for( size_t i = continuation_offset; i < line.size( ); i++ )
   {
      if( in_quotes )
      {
         if( line[ i ] == '"' )
         {
            if( i < line.size( ) - 1 && line[ i + 1 ] != ',' )
            {
               i++;
               next_value += '"';
            }
            else
               in_quotes = false;
         }
         else
            next_value += line[ i ];
      }
      else if( line[ i ] == '"' )
         in_quotes = true;
      else if( line[ i ] != ',' )
         next_value += line[ i ];
      else
      {
         values.push_back( next_value );
         next_value.erase( );
      }
   }

   values.push_back( next_value );
   last_value_incomplete = in_quotes;

   return values.size( ) - last_value_incomplete;
}

string resolve_class_id( const string& module,
 const string& id_or_name, const string& exception_context )
{
   string class_id( id_or_name );

   class_id = get_class_id_for_id_or_name( module, class_id );

   if( class_id == get_class_name_for_id_or_name( module, class_id ) )
      throw runtime_error( "unknown class '" + class_id + "' " + exception_context );

   return class_id;
}

string resolve_field_id( const string& module,
 const string& mclass, const string& id_or_name, const string& exception_context )
{
   string field_id( id_or_name );

   field_id = get_field_id_for_id_or_name( module, mclass, field_id );

   if( field_id == get_field_name_for_id_or_name( module, mclass, field_id ) )
      throw runtime_error( "unknown field '" + field_id + "' " + exception_context );

   return field_id;
}

string construct_paging_sql( const vector< pair< string, string > >& paging_info, bool reverse, bool inclusive )
{
   string page_exclusive( reverse ? " < " : " > " );
   string page_inclusive( reverse ? " <= " : " >= " );

   string sql( "(" );
   for( size_t i = 0; i < paging_info.size( ); i++ )
   {
      if( i == 0 )
      {
         if( !inclusive || i != paging_info.size( ) - 1 )
            sql += "C_" + paging_info[ i ].first + page_exclusive + paging_info[ i ].second;
         else
            sql += "C_" + paging_info[ i ].first + page_inclusive + paging_info[ i ].second;
      }
      else
      {
         sql += " OR (";
         for( size_t j = 0; j < i; j++ )
         {
            if( j > 0 )
               sql += " AND ";
            sql += "C_" + paging_info[ j ].first + " = " + paging_info[ j ].second;
         }

         sql += " AND ";
         if( !inclusive || i != paging_info.size( ) - 1 )
            sql += "C_" + paging_info[ i ].first + page_exclusive + paging_info[ i ].second;
         else
            sql += "C_" + paging_info[ i ].first + page_inclusive + paging_info[ i ].second;

         sql += ")";
      }
   }
   sql += ")";

   return sql;
}

string formatted_value( const string& value, const string& field_type )
{
   string formatted_value( value );

   if( !formatted_value.empty( ) )
   {
      if( field_type == "udate" || field_type == "tdate" )
      {
         udate today( udate::standard( ) );

         if( formatted_value == c_date_today )
            formatted_value == today.as_string( );
         else if( formatted_value == c_date_tomorrow )
            formatted_value == ( ++today ).as_string( );

         formatted_value = udate( formatted_value ).as_string( true );
      }
      else if( field_type == "mtime" || field_type == "ttime" )
      {
         if( formatted_value == c_dtm_now )
            formatted_value = mtime::standard( ).as_string( );

         formatted_value = mtime( formatted_value ).as_string( true );
      }
      else if( field_type == "datetime" || field_type == "tdatetime"
       || field_type == "date_time" || field_type == "tdate_time" )
      {
         date_time now( date_time::standard( ) );

         if( formatted_value == c_dtm_now || formatted_value == c_date_today )
            formatted_value = now.as_string( );
         else if( formatted_value == c_date_tomorrow )
            formatted_value = ( ++now ).as_string( );
         // NOTE: If a date_time string starts with 'U' then it is considered as already being UTC.
         else if( formatted_value[ 0 ] == 'U' )
            formatted_value.erase( 0, 1 );
         else
         {
            string tz_abbr( get_tz_abbr( ) );
            if( tz_abbr.empty( ) )
               tz_abbr = get_timezone( );

            formatted_value = local_to_utc( date_time( formatted_value ), tz_abbr ).as_string( );
         }


         formatted_value = date_time( formatted_value ).as_string( true );
      }
   }

   return formatted_value;
}

string construct_sql_select(
 class_base& instance,
 const vector< string >& field_info,
 const vector< string >& order_info,
 const vector< pair< string, string > >& query_info,
 const vector< pair< string, string > >& fixed_info,
 const vector< pair< string, string > >& paging_info, const string& security_info,
 bool is_reverse, bool is_inclusive, int row_limit, bool only_sys_fields, const string& text_search )
{
   string sql, sql_fields_and_table( "SELECT " );

   class_base_accessor instance_accessor( instance );

   vector< string > use_index_fields;

   if( field_info.empty( ) )
   {
      if( !only_sys_fields )
         sql_fields_and_table += "*";
      else
         sql_fields_and_table += "C_Key_,C_Ver_,C_Rev_,C_Typ_";
   }
   else
   {
      sql_fields_and_table += "C_Key_,C_Ver_,C_Rev_,C_Typ_";
      for( size_t i = 0; i < field_info.size( ); i++ )
      {
         string next_field( field_info[ i ] );

         if( next_field.empty( ) )
            throw runtime_error( "unexpected empty field found in sql select preparation" );

         get_field_name( instance, next_field );

         sql_fields_and_table += ",C_" + next_field;
      }
   }

   sql_fields_and_table += " FROM T_"
    + string( instance.module_name( ) ) + "_" + string( instance.class_name( ) );

   if( !query_info.empty( ) || !text_search.empty( )
    || !fixed_info.empty( ) || !paging_info.empty( ) || !security_info.empty( ) )
      sql += " WHERE ";

   if( !query_info.empty( ) )
   {
      for( size_t i = 0; i < query_info.size( ); i++ )
      {
         string field_name( query_info[ i ].first );
         string field_values( query_info[ i ].second );

         if( i > 0 )
            sql += " AND ";

         bool is_sub_select = false;
         string sub_select_sql_prefix, sub_select_sql_suffix;
         class_base* p_instance( &instance );

         // NOTE: If a child sub-context & field has been provided then one or more sub-selects
         // (depending upon the operator used) will be included in the generated SQL expression.
         if( !field_name.empty( ) && field_name[ 0 ] == '_' )
         {
            string::size_type pos = field_name.find( '.' );
            if( pos == string::npos )
               throw runtime_error( "unexpected field name format '" + field_name + "'" );

            class_base& child_instance( instance.get_or_create_graph_child( field_name.substr( 0, pos ) ) );

            is_sub_select = true;
            p_instance = &child_instance;
            field_name.erase( 0, pos + 1 );

            string graph_parent_fk_field( p_instance->get_graph_parent_fk_field( ) );
            get_field_name( *p_instance, graph_parent_fk_field );

            sub_select_sql_prefix = "C_Key_ IN (SELECT C_"
             + graph_parent_fk_field + " FROM T_" + to_string( p_instance->module_name( ) )
             + "_" + to_string( p_instance->class_name( ) ) + " WHERE ";

            sub_select_sql_suffix = " GROUP BY C_" + graph_parent_fk_field + ")";

            sql += sub_select_sql_prefix;
         }

         string field_type;
         bool is_sql_numeric;

         get_field_name( *p_instance, field_name, &is_sql_numeric, &field_type );

         bool was_like = false;
         if( field_values.length( ) >= 2 )
         {
            if( field_values[ 0 ] == '*' && field_values[ field_values.length( ) - 1 ] == '*' )
            {
               was_like = true;

               field_values.erase( 0, 1 );
               field_values.erase( field_values.size( ) - 1 );

               field_values = '%' + field_values + '%';

               // NOTE: For end-user convenience LIKE queries are performed case-insensitively.
               sql += "LOWER(C_" + field_name + ") LIKE " + sql_quote( lower( field_values ) );
            }
            else if( field_values[ field_values.length( ) - 1 ] == '*' )
            {
               was_like = true;

               field_values.erase( field_values.size( ) - 1 );

               field_values += '%';

               sql += "C_" + field_name + " LIKE " + sql_quote( field_values );
            }
         }

         if( !was_like )
         {
            string::size_type pos = field_values.find( ".." );
            if( pos != string::npos )
            {
               sql += "C_" + field_name + " BETWEEN ";
               if( is_sql_numeric )
               {
                  sql += field_values.substr( 0, pos );
                  sql += " AND ";
                  sql += field_values.substr( pos + 2 );
               }
               else
               {
                  sql += sql_quote( formatted_value( field_values.substr( 0, pos ), field_type ) );
                  sql += " AND ";
                  sql += sql_quote( formatted_value( field_values.substr( pos + 2 ), field_type ) );
               }
            }
            else
            {
               pos = field_values.find( '|' );
               if( pos == string::npos )
               {
                  pos = field_values.find( '&' );
                  if( pos == string::npos )
                  {
                     sql += "C_" + field_name + " = ";

                     sql += is_sql_numeric ? field_values : sql_quote( formatted_value( field_values, field_type ) );
                  }
                  else
                  {
                     if( !is_sub_select )
                        throw runtime_error( "unexpected '&' operator found in '" + field_values + "'" );

                     bool is_first = true;
                     while( true )
                     {
                        if( is_first )
                           is_first = false;
                        else
                        {
                           sql += sub_select_sql_suffix;
                           sql += " AND ";
                           sql += sub_select_sql_prefix;
                        }

                        sql += "C_" + field_name + " = ";

                        string next_value( field_values.substr( 0, pos ) );
                        sql += is_sql_numeric ? next_value : sql_quote( formatted_value( next_value, field_type ) );

                        if( pos == string::npos )
                           break;

                        field_values.erase( 0, pos + 1 );
                        pos = field_values.find( '&' );
                     }
                  }
               }
               else
               {
                  sql += "C_" + field_name + " IN (";
                  while( true )
                  {
                     string next_value( field_values.substr( 0, pos ) );
                     sql += is_sql_numeric ? next_value : sql_quote( formatted_value( next_value, field_type ) );

                     if( pos == string::npos )
                        break;

                     sql += ",";
                     field_values.erase( 0, pos + 1 );
                     pos = field_values.find( '|' );
                  }

                  sql += ")";
               }
            }
         }
         if( is_sub_select )
            sql += sub_select_sql_suffix;
      }

      if( !text_search.empty( )
       || !fixed_info.empty( ) || !paging_info.empty( ) || !security_info.empty( ) )
         sql += " AND ";
   }

   if( !text_search.empty( ) )
   {
      vector< string > text_search_fields;
      instance.get_text_search_fields( text_search_fields );

      if( text_search_fields.empty( ) )
         throw runtime_error( "no text search fields defined in class '" + string( instance.class_name( ) ) + "'" );

      vector< string > text_search_words;
      split_text_search( text_search, text_search_words );

      if( text_search_words.size( ) > 1 )
         sql += "(";

      for( size_t i = 0; i < text_search_words.size( ); i++ )
      {
         if( i > 0 )
            sql += " AND ";

         sql += "(";

         for( size_t j = 0; j < text_search_fields.size( ); j++ )
         {
            if( j > 0 )
               sql += " OR ";

            string like_expr( escaped( text_search_words[ i ], "'%_" ) );
            like_expr = "%" + like_expr + "%";

            // NOTE: Due to issues with UTF-8 and MySQL the query needs to use both the case
            // sensitive and insensitive tests in order to get the correct results.
            sql += "C_" + text_search_fields[ j ] + " LIKE '" + like_expr + "'";
            sql += " OR LOWER(C_" + text_search_fields[ j ] + ") LIKE LOWER('" + like_expr + "')";
         }

         sql += ")";
      }

      if( text_search_words.size( ) > 1 )
         sql += ")";

      if( !fixed_info.empty( ) || !paging_info.empty( ) || !security_info.empty( ) )
         sql += " AND ";
   }

   if( !fixed_info.empty( ) )
   {
      for( size_t i = 0; i < fixed_info.size( ); i++ )
      {
         string next_field( fixed_info[ i ].first );
         string next_value( fixed_info[ i ].second );

         if( i > 0 )
            sql += " AND ";

         string field_type;
         bool is_sql_numeric;

         get_field_name( instance, next_field, &is_sql_numeric, &field_type );

         if( query_info.empty( ) && text_search.empty( ) )
            use_index_fields.push_back( "C_" + next_field );

         // NOTE: If a fixed key value contains one or more "+val" suffixes then these
         // are stripped off to behave as an though they are in IN list (although "ORs"
         // are actually used in the SQL generated here).
         bool has_multiple_values = false;
         string::size_type pos = next_value.find( '+' );
         if( pos != string::npos )
         {
            has_multiple_values = true;

            sql += "(";

            string::size_type spos( pos + 1 );

            while( true )
            {
               string::size_type npos = next_value.find( '+', spos );

               string next_opt_value;

               if( npos == string::npos )
                  next_opt_value = next_value.substr( spos );
               else
                  next_opt_value = next_value.substr( spos, npos - spos );

               sql += "C_" + next_field + " = " + next_opt_value;

               sql += " OR ";

               if( npos == string::npos )
                  break;

               spos = npos + 1;
            }

            next_value.erase( pos );
         }

         // NOTE: If a fixed key value ends with a '*' then the value preceeding
         // it will become a LIKE prefix and the '*' is changed to a SQL wildcard.
         bool value_is_like_prefix = false;
         if( !next_value.empty( ) && next_value[ next_value.length( ) - 1 ] == '*' )
         {
            value_is_like_prefix = true;
            next_value[ next_value.length( ) - 1 ] = '%';
         }

         if( !is_sql_numeric )
            next_value = sql_quote( formatted_value( next_value, field_type ) );

         if( next_value == "NULL" )
            sql += "C_" + next_field + " IS NULL";
         else if( !value_is_like_prefix )
            sql += "C_" + next_field + " = " + next_value;
         else
            sql += "C_" + next_field + " LIKE " + next_value;

         if( has_multiple_values )
            sql += ")";
      }

      if( !paging_info.empty( ) || !security_info.empty( ) )
         sql += " AND ";
   }

   if( !paging_info.empty( ) )
   {
      vector< pair< string, string > > modified_paging_info;

      for( size_t i = 0; i < paging_info.size( ); i++ )
      {
         string next_field( paging_info[ i ].first );
         string next_value( paging_info[ i ].second );

         string field_type;
         bool is_sql_numeric;

         get_field_name( instance, next_field, &is_sql_numeric, &field_type );

         if( query_info.empty( ) && text_search.empty( ) )
            use_index_fields.push_back( "C_" + next_field );

         if( !is_sql_numeric )
            next_value = sql_quote( formatted_value( next_value, field_type ) );

         modified_paging_info.push_back( make_pair( next_field, next_value ) );
      }

      sql += construct_paging_sql( modified_paging_info, is_reverse, is_inclusive );

      if( !security_info.empty( ) )
         sql += " AND ";
   }

   string security_field;
   if( !security_info.empty( ) )
   {
      string::size_type pos = security_info.find( ':' );
      if( pos == string::npos )
         throw runtime_error( "unexpected format for security_info '" + security_info + "'" );

      security_field = security_info.substr( 0, pos );

      bool is_sql_numeric;
      get_field_name( instance, security_field, &is_sql_numeric );

      sql += "C_" + security_field + " LIKE '" + security_info.substr( pos + 1 ) + "%'";
   }

   if( !order_info.empty( ) )
   {
      sql += " ORDER BY ";

      vector< string > unique_indexes;
      instance.get_sql_unique_indexes( unique_indexes );

      set< string > sorted_unique_indexes( unique_indexes.begin( ), unique_indexes.end( ) );

      string index;
      for( size_t i = 0; i < order_info.size( ); i++ )
      {
         string next_field( order_info[ i ] );
         get_field_name( instance, next_field );

         // NOTE: As the "key" is always appended to the ordering (in case no unique index found)
         // here if a unique index is found on all but the last order field then skip this field.
         if( i == order_info.size( ) - 1 && sorted_unique_indexes.count( index ) )
            break;

         if( i > 0 )
         {
            sql += ',';
            index += ',';
         }

         sql += "C_" + next_field;
         index += "C_" + next_field;

         if( is_reverse )
            sql += " DESC";
      }
   }

   if( row_limit )
      sql += " LIMIT " + to_string( row_limit );

   // NOTE: If no query or text search info was supplied then the fixed and paging fields will be
   // checked against each table index to determine if an index covers all of them. If one is found
   // then force the query to use this index (to ensure the RDBMS optimiser does not get it wrong).
   if( !use_index_fields.empty( ) )
   {
      vector< string > indexes;
      instance.get_sql_indexes( indexes );

      vector< string > unique_indexes;
      instance.get_sql_unique_indexes( unique_indexes );

      set< string > sorted_unique_indexes( unique_indexes.begin( ), unique_indexes.end( ) );

      for( size_t i = use_index_fields.size( ); i < order_info.size( ); i++ )
      {
         string next_field( order_info[ i ] );
         get_field_name( instance, next_field );

         use_index_fields.push_back( "C_" + next_field );
      }

      bool found = false;
      string index_to_use;

      for( size_t i = 0; i < indexes.size( ); i++ )
      {
         vector< string > index_fields;
         split( indexes[ i ], index_fields );

         // NOTE: As the "key" is always appended to the ordering (in case no unique index found)
         // the key is appended to unique indexes so the field matching will operate as expected.
         if( sorted_unique_indexes.count( indexes[ i ] ) )
            index_fields.push_back( "C_Key_" );

         if( index_fields.size( ) == use_index_fields.size( ) )
         {
            for( size_t j = 0; j < use_index_fields.size( ); j++ )
            {
               if( index_fields[ j ] != use_index_fields[ j ] )
                  break;

               if( j == use_index_fields.size( ) - 1 )
                  found = true;
            }
         }

         if( found )
         {
            index_to_use = "I_" + string( instance.module_name( ) ) + "_"
             + string( instance.class_name( ) ) + "_" + ( i < 10 ? "0" : "" ) + to_string( i );
            break;
         }
      }

      if( !index_to_use.empty( ) )
         sql_fields_and_table += " USE INDEX (" + index_to_use + ")";
   }

   return sql_fields_and_table + sql;
}

void append_transaction_log_command(
 storage_handler& handler, bool log_even_when_locked = false, size_t load_module_id = 0 )
{
   if( ( log_even_when_locked || handler.get_alternative_log_file( )
    || !handler.get_is_locked_for_admin( ) ) && !gtp_session->transaction_log_command.empty( ) )
   {
      string log_filename( handler.get_name( ) );
      log_filename += ".log";

      bool is_new = false;
      if( !file_exists( log_filename ) )
         is_new = true;

      ofstream& log_file( handler.get_alternative_log_file( )
       ? *handler.get_alternative_log_file( ) : handler.get_log_file( ) );

      if( !log_file.is_open( ) )
         log_file.open( log_filename.c_str( ), ios::out | ios::app );

      if( is_new )
         log_file << "[0]" << handler.get_root( ).identity << '\n';

      int tx_id;

      // NOTE: When log file is truncated during a backup no transaction is active so
      // change the tx id to 2 to ensure the restore can recognise a partial log file.
      // Any "init" ops will also get the tx id of 2 and the tx id of any module loads
      // will always be 1.
      if( load_module_id )
         tx_id = 1;
      else if( !ods::instance( )->get_transaction_id( ) || handler.get_alternative_log_file( ) )
         tx_id = 2;
      else
      {
         storable_identity& identity( *handler.get_root( ).o_identity );

         if( identity.next_id == identity.ceiling )
         {
            identity.ceiling += c_identity_burn;

            // NOTE: Store the "ceiling" rather than "next_id" to avoid having to
            // perform extra I/O for every transaction. If storage termination is
            // to occur normally then the actual "next_id" will be written and no
            // identity value is lost. If for some reason normal termination does
            // not occur then up to the "burn" number of identities will be lost.
            restorable< int > tmp_identity( identity.next_id, identity.ceiling );
            handler.get_root( ).o_identity.store( e_oid_pointer_opt_force_write );
         }

         tx_id = ++identity.next_id;
      }

      vector< string > lines;
      raw_split( gtp_session->transaction_log_command, lines, '\n' );

      for( size_t i = 0; i < lines.size( ); i++ )
         log_file << '[' << tx_id << ']' << lines[ i ] << '\n';

      log_file.flush( );
      if( !log_file.good( ) )
         throw runtime_error( "*** unexpected error occurred writing to transaction log ***" );
   }

   gtp_session->transaction_log_command.erase( );
}

string_container g_strings;

string g_sid;
string g_domain;
string g_license;
string g_timezone;
string g_web_root;

string g_set_trace;

bool g_use_https = false;
bool g_using_ssl = false;

int g_max_user_limit = 1;

string g_pem_password;
string g_sql_password;

string g_default_storage;

int g_session_timeout = 0;

bool g_script_reconfig = false;

string g_mbox_path;
string g_mbox_username;

string g_pop3_server;
string g_pop3_suffix;
string g_pop3_username;
string g_pop3_password;
string g_pop3_security;

string g_smtp_server;
string g_smtp_sender;
string g_smtp_suffix;
string g_smtp_username;
string g_smtp_password;
string g_smtp_security;
int g_smtp_max_send_attempts = 1;
int64_t g_smtp_max_attached_data = INT64_C( 0 );

struct script_info
{
   string filename;
   string arguments;
};

time_t g_scripts_mod;

map< string, script_info > g_scripts;

bool scripts_file_has_changed( )
{
   bool changed = false;

   time_t t = 0;
   if( file_exists( c_manuscript_file ) )
      t = last_modification_time( c_manuscript_file );

   if( t != g_scripts_mod )
      changed = true;

   g_scripts_mod = t;

   return changed;
}

void read_script_info( )
{
   ifstream inpf( c_manuscript_file );

   g_scripts.clear( );

   if( inpf )
   {
      sio_reader reader( inpf );

      string name;
      while( reader.has_started_section( c_section_script ) )
      {
         script_info info;

         string name = reader.read_attribute( c_attribute_name );

         info.filename = reader.read_attribute( c_attribute_filename );
         info.arguments = reader.read_opt_attribute( c_attribute_arguments );

         g_scripts.insert( make_pair( name, info ) );

         reader.finish_section( c_section_script );
      }

      reader.verify_finished_sections( );
   }
}

void read_server_configuration( )
{
   ifstream inpf( c_server_config_file );

   if( inpf )
   {
      sio_reader reader( inpf );

      g_domain = reader.read_opt_attribute( c_attribute_domain, "localhost" );

      g_license = upper( reader.read_opt_attribute( c_attribute_license ) );

      g_timezone = upper( reader.read_opt_attribute( c_attribute_timezone ) );

      g_web_root = reader.read_attribute( c_attribute_web_root );
      if( !g_web_root.empty( ) )
      {
         if( g_web_root[ 0 ] == '$' )
         {
            const char* p_env = getenv( g_web_root.substr( 1 ).c_str( ) );
            g_web_root = string( p_env ? p_env : "" );
         }
         else if( g_web_root[ 0 ] == '%' && g_web_root.size( ) > 2 && g_web_root[ g_web_root.size( ) - 1 ] == '%' )
         {
            const char* p_env = getenv( g_web_root.substr( 1, g_web_root.size( ) - 2 ).c_str( ) );
            g_web_root = string( p_env ? p_env : "" );
         }
      }

      g_set_trace = reader.read_opt_attribute( c_attribute_set_trace );
      if( !g_set_trace.empty( ) )
      {
         int trace_flags;
         istringstream isstr( g_set_trace );
         isstr >> hex >> trace_flags;

         set_trace_flags( get_trace_flags( ) | trace_flags );
      }

      g_use_https = ( lower( reader.read_opt_attribute( c_attribute_use_https, "false" ) ) == "true" );

      g_max_sessions = atoi( reader.read_opt_attribute(
       c_attribute_max_sessions, to_string( c_max_sessions_default ) ).c_str( ) );

      g_pem_password = reader.read_opt_attribute( c_attribute_pem_password );
      g_sql_password = reader.read_opt_attribute( c_attribute_sql_password );

      g_default_storage = reader.read_opt_attribute( c_attribute_default_storage );
      g_variables.insert( make_pair( c_system_variable_storage, g_default_storage ) );

      g_script_reconfig = ( lower( reader.read_opt_attribute( c_attribute_script_reconfig, "false" ) ) == "true" );

      g_session_timeout = atoi( reader.read_opt_attribute( c_attribute_session_timeout, "0" ).c_str( ) );

      g_max_storage_handlers = atoi( reader.read_opt_attribute(
       c_attribute_max_storage_handlers, to_string( c_max_storage_handlers_default ) ).c_str( ) ) + 1;

      reader.start_section( c_section_email );

      if( reader.has_started_section( c_section_mbox ) )
      {
         g_mbox_path = reader.read_attribute( c_attribute_path );
         g_mbox_username = reader.read_opt_attribute( c_attribute_username );

         reader.finish_section( c_section_mbox );
      }

      if( reader.has_started_section( c_section_pop3 ) )
      {
         g_pop3_server = reader.read_attribute( c_attribute_server );
         g_pop3_suffix = reader.read_attribute( c_attribute_suffix );
         g_pop3_username = reader.read_attribute( c_attribute_username );
         g_pop3_password = reader.read_opt_attribute( c_attribute_password );
         g_pop3_security = reader.read_opt_attribute( c_attribute_security );

         reader.finish_section( c_section_pop3 );
      }

      if( reader.has_started_section( c_section_smtp ) )
      {
         g_smtp_server = reader.read_attribute( c_attribute_server );
         g_smtp_sender = reader.read_opt_attribute( c_attribute_sender );
         g_smtp_suffix = reader.read_attribute( c_attribute_suffix );
         g_smtp_username = reader.read_attribute( c_attribute_username );
         g_smtp_password = reader.read_opt_attribute( c_attribute_password );
         g_smtp_security = reader.read_opt_attribute( c_attribute_security );

         if( g_smtp_sender.empty( ) )
            g_smtp_sender = g_smtp_username;

         string max_send_attempts = reader.read_opt_attribute( c_attribute_max_send_attempts );
         if( !max_send_attempts.empty( ) )
            g_smtp_max_send_attempts = atoi( max_send_attempts.c_str( ) );

         string max_attached_data = reader.read_opt_attribute( c_attribute_max_attached_data );
         if( !max_attached_data.empty( ) )
            g_smtp_max_attached_data = unformat_bytes( max_attached_data );

         reader.finish_section( c_section_smtp );
      }

      reader.finish_section( c_section_email );

      reader.verify_finished_sections( );
   }
}

struct field_data
{
   field_data( const string& id, const string& name,
    const string& value, bool mandatory, bool transient, const string& class_id, const string& type_name )
    :
    id( id ),
    name( name ),
    value( value ),
    mandatory( mandatory ),
    transient( transient ),
    class_id( class_id ),
    type_name( type_name )
   {
   }

   string id;
   string name;
   string value;
   bool mandatory;
   bool transient;
   string class_id;
   string type_name;
};

void get_all_field_data( size_t handle, const string& context,
 const string& key, vector< field_data >& all_field_data, string* p_class_id = 0,
 string* p_class_name = 0, vector< pair< string, string > >* p_base_class_info = 0 )
{
   class_base& cb( get_class_base_from_handle_for_op( handle, context ) );

   if( !key.empty( ) )
      instance_review_begin( handle, context, key );

   class_base& dcb( *cb.get_dynamic_instance( ) );

   if( p_class_id )
      *p_class_id = string( dcb.class_id( ) );

   if( p_class_name )
      *p_class_name = string( dcb.class_name( ) );

   if( p_base_class_info )
      dcb.get_base_class_info( *p_base_class_info );

   foreign_key_info_container foreign_key_info;
   get_foreign_key_info_for_module_class( dcb.module_id( ), dcb.class_id( ), foreign_key_info );

   field_info_container field_info;
   dcb.get_field_info( field_info );

   // FUTURE: It would make things easier (and improve performance) if both the fk class id and
   // the transient indicator were a part of the "field_info" data structure itself.
   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      string class_id;
      if( foreign_key_info.count( field_info[ i ].id ) )
      {
         size_t offset = 0;
         if( foreign_key_info[ field_info[ i ].id ].second.find( dcb.module_name( ) ) == 0 )
            offset = strlen( dcb.module_name( ) ) + 1;

         class_id = get_class_id_for_id_or_name( dcb.module_id( ), foreign_key_info[ field_info[ i ].id ].second.substr( offset ) );
      }

      string field_value;
      if( !key.empty( ) )
         field_value = dcb.get_field_value( i );

      all_field_data.push_back( field_data( field_info[ i ].id, field_info[ i ].name,
       field_value, field_info[ i ].mandatory, dcb.is_field_transient( i ), class_id, field_info[ i ].type_name ) );
   }

   if( !key.empty( ) )
      instance_review_finish( handle, context );
}

void export_data( ostream& outs,
 const string& module, string class_id, const string& key,
 string& last_class_id, bool output_children, command_handler& handler,
 const map< string, int >& all_class_ids, const map< string, set< string > >& excludes,
 const map< string, map< string, string > >& tests, const map< string, set< string > >& includes,
 map< string, set< string > >& exported_records, map< string, set< string > >& exported_children,
 deque< pair< string, string > >& next_pass, map< string, set< string > >& will_be_exported,
 map< string, set< string > >& partial_export, const map< string, int >& rounds,
 int current_round, map< int, deque< pair< string, string > > >& future_rounds, time_t& ts, size_t& total )
{
   size_t handle = create_object_instance( module, class_id );
   class_base& cb( get_class_base_from_handle_for_op( handle, "" ) );

   string class_name;
   vector< pair< string, string > > base_class_info;

   vector< field_data > all_field_data;
   get_all_field_data( handle, "", key, all_field_data, &class_id, &class_name, &base_class_info );

   if( !all_class_ids.count( class_id ) )
      throw runtime_error( "unable to find '" + class_id + "' in all_class_ids for export" );

   string exclude_suffix;
   bool skip_record = false;

   set< string > first_output_ids;

   bool has_seen = false;
   if( partial_export[ class_id ].count( key ) )
   {
      has_seen = true;
      partial_export[ class_id ].erase( key );
      for( size_t i = 0; i < base_class_info.size( ); i++ )
         partial_export[ base_class_info[ i ].first ].erase( key );
   }

   if( will_be_exported[ class_id ].count( key ) )
   {
      has_seen = true;
      will_be_exported[ class_id ].erase( key );
      for( size_t i = 0; i < base_class_info.size( ); i++ )
         will_be_exported[ base_class_info[ i ].first ].erase( key );
   }

   if( tests.count( class_id ) )
   {
      if( tests.find( class_id )->second.count( "*" ) )
         skip_record = true;
      else
      {
         for( size_t i = 0; i < all_field_data.size( ); i++ )
         {
            string next_value( all_field_data[ i ].value );

            string test_expr;

            set< string > key_values;
            if( tests.find( class_id )->second.count( c_key_field ) )
            {
               next_value = key;
               test_expr = tests.find( class_id )->second.find( c_key_field )->second;
            }
            else if( tests.find( class_id )->second.count( all_field_data[ i ].id ) )
               test_expr = tests.find( class_id )->second.find( all_field_data[ i ].id )->second;
            else if( tests.find( class_id )->second.count( all_field_data[ i ].name ) )
               test_expr = tests.find( class_id )->second.find( all_field_data[ i ].name )->second;

            if( !test_expr.empty( ) )
            {
               string suffix;

               string::size_type pos = test_expr.find( ';' );
               if( pos != string::npos )
               {
                  suffix = test_expr.substr( pos );
                  test_expr.erase( pos );
               }

               bool is_negative = false;
               if( !test_expr.empty( ) && test_expr[ 0 ] == '!' )
               {
                  is_negative = true;
                  test_expr.erase( 0, 1 );
               }

               if( !test_expr.empty( ) && test_expr[ 0 ] == '=' )
               {
                  test_expr.erase( 0, 1 );

                  set< string > all_tests;
                  split( test_expr, all_tests );

                  if( ( is_negative && all_tests.count( next_value ) )
                   || ( !is_negative && !all_tests.count( next_value ) ) )
                  {
                     if( suffix.empty( ) )
                        skip_record = true;
                     else
                        exclude_suffix = suffix;

                     break;
                  }
               }
               else if( test_expr[ 0 ] == '#' )
                  first_output_ids.insert( all_field_data[ i ].id );
               else
                  throw runtime_error( "unexpected test_expr '" + test_expr + "'" );
            }
         }
      }
   }

   if( !skip_record )
   {
      bool need_to_repeat = false;
      int position = all_class_ids.find( class_id )->second;

      string all_values( key );
      for( size_t i = 0; i < all_field_data.size( ); i++ )
      {
         if( all_field_data[ i ].transient )
            continue;

         string next_value( all_field_data[ i ].value );

         if( !next_value.empty( ) && !all_field_data[ i ].class_id.empty( )
          && !partial_export[ all_field_data[ i ].class_id ].count( next_value )
          && !exported_records[ all_field_data[ i ].class_id ].count( next_value ) )
         {
            if( !all_class_ids.count( all_field_data[ i ].class_id ) )
               throw runtime_error( "unable to find '"
                + all_field_data[ i ].class_id + "' in all_class_ids for export" );

            bool output_fk_children = false;

            if( includes.count( class_id )
             && ( includes.find( class_id )->second.count( all_field_data[ i ].id )
             || includes.find( class_id )->second.count( all_field_data[ i ].name ) ) )
               output_fk_children = true;

            if( first_output_ids.count( all_field_data[ i ].id )
             || all_class_ids.find( all_field_data[ i ].class_id )->second < position )
               export_data( outs, module, all_field_data[ i ].class_id,
                all_field_data[ i ].value, last_class_id, output_fk_children, handler,
                all_class_ids, excludes, tests, includes, exported_records, exported_children,
                next_pass, will_be_exported, partial_export, rounds, current_round, future_rounds, ts, total );
            else if( !all_field_data[ i ].mandatory )
            {
               // NOTE: If a foreign key cannot be processed first but is not mandatory (as is often
               // the case with self relationships) then the record will be output without the value
               // and the foreign key record will be processed in the next pass. As the full details
               // are missing the record itself will need to be repeated (see after this loop). This
               // approach of allowing partial records to be initially output solves interdependency
               // issues that can occur especially with self relationships.
               if( !will_be_exported[ all_field_data[ i ].class_id ].count( next_value ) )
               {
                  will_be_exported[ all_field_data[ i ].class_id ].insert( next_value );
                  next_pass.push_back( make_pair( all_field_data[ i ].class_id, next_value ) );
               }

               next_value.erase( );
               need_to_repeat = true;
            }
            else
               throw runtime_error( "unable to determine export ordering for "
                + all_field_data[ i ].class_id + " whilst processing " + class_id );
         }

         all_values += ',';
         all_values += escaped( next_value, ",\"", '\\', "rn\r\n" );
      }

      if( need_to_repeat )
      {
         if( !will_be_exported[ class_id ].count( key ) )
         {
            will_be_exported[ class_id ].insert( key );
            for( size_t i = 0; i < base_class_info.size( ); i++ )
               will_be_exported[ base_class_info[ i ].first ].insert( key );

            next_pass.push_back( make_pair( class_id, key ) );

            partial_export[ class_id ].insert( key );
            for( size_t i = 0; i < base_class_info.size( ); i++ )
               partial_export[ base_class_info[ i ].first ].insert( key );
         }
      }

      if( !exported_records[ class_id ].count( key ) )
      {
         if( class_id != last_class_id )
         {
            if( !last_class_id.empty( ) )
               outs << " </class>\n";

            outs << " <class/>\n";
            outs << "  <name>" << class_name << '\n';

            string field_info( "  <fields>" + to_string( c_key_field ) );
            for( size_t i = 0; i < all_field_data.size( ); i++ )
            {
               if( all_field_data[ i ].transient )
                  continue;

               field_info += ',';
               field_info += all_field_data[ i ].name;
            }

            outs << field_info << '\n';
         }
         outs << "  <record>" << all_values << '\n';

         ++total;
         last_class_id = class_id;

         if( !need_to_repeat )
         {
            exported_records[ class_id ].insert( key );
            for( size_t i = 0; i < base_class_info.size( ); i++ )
               exported_records[ base_class_info[ i ].first ].insert( key );
         }

         if( time( 0 ) - ts >= 10 )
         {
            ts = time( 0 );
            // FUTURE: This message should be handled as a server string message.
            handler.output_progress( "Processed " + to_string( total ) + " records..." );
         }
      }

      if( output_children && !exported_children[ class_id ].count( key ) )
      {
         class_base_accessor instance_accessor( cb );

         size_t num_children = instance_accessor.get_num_foreign_key_children( );
         for( int pass = 0; pass < 2; ++pass )
         {
            cascade_op next_op;
            if( pass == 0 )
               next_op = e_cascade_op_restrict;
            else
               next_op = e_cascade_op_destroy;

            for( size_t i = 0; i < num_children; i++ )
            {
               string next_child_field;
               class_base* p_class_base = instance_accessor.get_next_foreign_key_child( i, next_child_field, next_op );

               string child_class_and_field;

               if( p_class_base )
               {
                  child_class_and_field = string( p_class_base->class_id( ) ) + "#" + next_child_field;

                  if( excludes.count( class_id )
                   && ( excludes.find( class_id )->second.count( "*" + exclude_suffix )
                   || excludes.find( class_id )->second.count( p_class_base->class_id( ) + exclude_suffix )
                   || excludes.find( class_id )->second.count( child_class_and_field + exclude_suffix ) ) )
                     continue;

                  string key_info( p_class_base->get_order_field_name( ) );
                  if( !key_info.empty( ) )
                     key_info += ' ';

                  if( ( !key_info.empty( ) && p_class_base->iterate_forwards( key_info ) )
                   || ( key_info.empty( ) && p_class_base->iterate_forwards( true, 0, e_sql_optimisation_unordered ) ) )
                  {
                     do
                     {
                        if( rounds.count( p_class_base->class_id( ) )
                         && rounds.find( p_class_base->class_id( ) )->second > current_round )
                        {
                           int round( rounds.find( p_class_base->class_id( ) )->second );
                           future_rounds[ round ].push_back(
                            make_pair( p_class_base->class_id( ), p_class_base->get_key( ) ) );
                           continue;
                        }

                        export_data( outs, module, p_class_base->class_id( ),
                         p_class_base->get_key( ), last_class_id, true, handler, all_class_ids,
                         excludes, tests, includes, exported_records, exported_children, next_pass,
                         will_be_exported, partial_export, rounds, current_round, future_rounds, ts, total );
                     } while( p_class_base->iterate_next( ) );
                  }
               }
            }
         }

         exported_children[ class_id ].insert( key );
         for( size_t i = 0; i < base_class_info.size( ); i++ )
            exported_children[ base_class_info[ i ].first ].insert( key );
      }
   }

   destroy_object_instance( handle );
}

void fetch_instance_from_row_cache( class_base& instance, bool skip_after_fetch )
{
   class_base_accessor instance_accessor( instance );

   if( instance_accessor.row_cache( ).empty( ) )
      throw runtime_error( "unexpected empty row cache" );

   instance_accessor.clear( );

   instance_accessor.set_key( instance_accessor.row_cache( )[ 0 ][ 0 ], true );
   instance_accessor.set_version( atoi( instance_accessor.row_cache( )[ 0 ][ 1 ].c_str( ) ) );
   instance_accessor.set_revision( atoi( instance_accessor.row_cache( )[ 0 ][ 2 ].c_str( ) ) );
   instance_accessor.set_original_identity( instance_accessor.row_cache( )[ 0 ][ 3 ] );

   const map< int, int >& fields( instance_accessor.select_fields( ) );
   const vector< int >& columns( instance_accessor.select_columns( ) );

   for( int i = 4; i < instance_accessor.row_cache( )[ 0 ].size( ); i++ )
   {
      if( !fields.count( columns[ i - 4 ] ) )
         throw runtime_error( "unexpected field # not found for column #" + to_string( i - 4 ) );

      int fnum( fields.find( columns[ i - 4 ] )->second );

      TRACE_LOG( TRACE_SQLCLSET, "setting field #" + to_string( fnum + 1 ) + " to " + instance_accessor.row_cache( )[ 0 ][ i ] );
      instance.set_field_value( fnum, instance_accessor.row_cache( )[ 0 ][ i ] );
   }

   instance_accessor.row_cache( ).pop_front( );

   instance_accessor.after_fetch_from_db( );

   if( !skip_after_fetch )
      instance_accessor.perform_after_fetch( );
}

}

size_t g_trace_flags;

int get_trace_flags( )
{
   return g_trace_flags;
}

void set_trace_flags( int flags )
{
   g_trace_flags = flags;
}

void list_trace_flags( vector< string >& flag_names )
{
   flag_names.push_back( "commands" ); // TRACE_COMMANDS
   flag_names.push_back( "sqlstmts" ); // TRACE_SQLSTMTS
   flag_names.push_back( "classops" ); // TRACE_CLASSOPS
   flag_names.push_back( "mods_gen" ); // TRACE_MODS_GEN
   flag_names.push_back( "sql_sets" ); // TRACE_SQLCLSET
   flag_names.push_back( "fld_vals" ); // TRACE_FLD_VALS
   flag_names.push_back( "lock_ops" ); // TRACE_LOCK_OPS
   flag_names.push_back( "ctr_dtrs" ); // TRACE_CTR_DTRS
   flag_names.push_back( "sessions" ); // TRACE_SESSIONS
}

void log_trace_message( int flag, const string& message )
{
   static bool is_first = true;

   guard g( g_trace_mutex );

   ofstream outf( "trace.log", is_first ? ios::out | ios::trunc : ios::out | ios::app );

   is_first = false;

   string type( "unknown" );
   switch( flag )
   {
      case TRACE_COMMANDS:
      type = "command";
      break;

      case TRACE_SQLSTMTS:
      type = "sqlstmt";
      break;

      case TRACE_CLASSOPS:
      type = "classop";
      break;

      case TRACE_MODS_GEN:
      type = "mod_gen";
      break;

      case TRACE_SQLCLSET:
      type = "sql_set";
      break;

      case TRACE_FLD_VALS:
      type = "fld_val";
      break;

      case TRACE_LOCK_OPS:
      type = "lock_op";
      break;

      case TRACE_CTR_DTRS:
      type = "ctr_dtr";
      break;

      case TRACE_SESSIONS:
      type = "session";
      break;

      case TRACE_ANYTHING:
      type = "general";
      break;
   }

   outf << '[' << date_time::local( ).as_string( true, false ) << "] [" << setw( 6 )
    << setfill( '0' ) << ( gtp_session ? gtp_session->id : 0 ) << "] [" << type << "] " << message << '\n';
}

void init_globals( )
{
   guard g( g_mutex );

   if( !file_exists( c_server_sid_file ) )
   {
      g_sid = upper( uuid( ).as_string( ) );
      write_file( c_server_sid_file, g_sid );
   }
   else
      g_sid = buffer_file( c_server_sid_file );

   read_server_configuration( );

   setup_timezones( );

   // NOTE: The manuscript info doesn't actually need to be read until a script is attempted
   // to be run, however, it is been read at startup just to ensure that the .sio file isn't
   // initially malformed.
   if( scripts_file_has_changed( ) )
      read_script_info( );

   // NOTE: This "get_tz_info" call is performed to verify that the server timezone is valid.
   float offset = 0.0;
   string tz_abbr( get_timezone( ) );
   date_time now( date_time::standard( ) );
   get_tz_info( now, tz_abbr, offset );

   read_strings( "ciyam_strings.txt", g_strings, "c_str_" );
   read_strings( "module_strings.txt", g_strings, "c_str_" );

   for( size_t i = 0; i < g_max_sessions; i++ )
      g_sessions.push_back( 0 );

   for( size_t i = 0; i < g_max_storage_handlers; i++ )
      g_storage_handlers.push_back( 0 );

   g_storage_handlers[ 0 ] = new storage_handler( 0, c_default_storage_name );
   g_storage_handlers[ 0 ]->get_root( ).identity = c_default_storage_identity;

   g_storage_handler_index.insert( make_pair( c_default_storage_name, 0 ) );

#ifdef SSL_SUPPORT
   if( file_exists( "ciyam_server.pem" ) )
   {
      string password( c_default_pem_password );
      if( !g_pem_password.empty( ) )
         password = get_pem_password( );

      init_ssl( "ciyam_server.pem", password.c_str( ) );
      g_using_ssl = true;
   }
#endif
}

void term_globals( )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] )
      {
         delete g_sessions[ i ];
         g_sessions[ i ] = 0;
      }
   }

   for( size_t i = 0; i < g_max_storage_handlers; i++ )
   {
      if( g_storage_handlers[ i ] )
      {
         delete g_storage_handlers[ i ];
         g_storage_handlers[ i ] = 0;
      }
   }

   g_scripts.clear( );

   g_strings.clear( );

#ifdef SSL_SUPPORT
   if( g_using_ssl )
      term_ssl( );
#endif
}

string get_string( const string& key )
{
   string str( key );

   string_const_iterator sci = g_strings.find( key );
   if( sci != g_strings.end( ) )
      str = sci->second;

   return str;
}

string get_string_message( const string& string_message, const pair< string, string >& parm1 )
{
   string message;

   string::size_type pos = string_message.find( parm1.first );
   if( pos == string::npos )
      throw runtime_error( "parameter '" + parm1.first + "' not found in '" + string_message + "'" );

   message = string_message.substr( 0, pos );
   message += parm1.second;
   message += string_message.substr( pos + parm1.first.length( ) );

   return message;
}

string get_string_message( const string& string_message,
 const pair< string, string >& parm1, const pair< string, string >& parm2 )
{
   string message( get_string_message( string_message, parm1 ) );

   message = get_string_message( message, parm2 );

   return message;
}

string get_string_message( const string& string_message,
 const pair< string, string >& parm1, const pair< string, string >& parm2,
 const pair< string, string >& parm3 )
{
   string message( get_string_message( string_message, parm1, parm2 ) );

   message = get_string_message( message, parm3 );

   return message;
}

string get_string_message( const string& string_message,
 const pair< string, string >& parm1, const pair< string, string >& parm2,
 const pair< string, string >& parm3, const pair< string, string >& parm4 )
{
   string message( get_string_message( string_message, parm1, parm2, parm3 ) );

   message = get_string_message( message, parm4 );

   return message;
}

int get_max_user_limit( )
{
   guard g( g_mutex );
   return g_max_user_limit;
}

void set_max_user_limit( int new_limit )
{
   guard g( g_mutex );
   g_max_user_limit = new_limit;
}

string get_prefix( )
{
   guard g( g_mutex );
   return g_use_https ? "https" : "http";
}

string get_domain( )
{
   guard g( g_mutex );
   return g_domain;
}

string get_app_url( const string& suffix )
{
   guard g( g_mutex );

   string url( get_prefix( ) );
   url += "://";
   url += get_domain( );
   url += "/" + lower( storage_name( ) );

   if( !suffix.empty( ) )
      url += "/" + suffix;

   return url;
}

string get_license( bool prepend_sid, bool append_max_user_limit )
{
   guard g( g_mutex );

   if( !prepend_sid && !append_max_user_limit )
      return g_license;

   string s( g_license );

   if( prepend_sid )
      s = g_sid + "-" + s;

   if( append_max_user_limit )
      s += ":" + to_string( g_max_user_limit );

   return s;
}

string get_checksum( const string& data, bool use_license )
{
   guard g( g_mutex );

   string prefix( !use_license ? g_sid : g_license );

   sha1 hash( prefix + data );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_timezone( )
{
   guard g( g_mutex );

   string tz_abbr( g_timezone );

   if( tz_abbr.empty( ) )
      tz_abbr = "GMT";

   return g_timezone;
}

string get_web_root( )
{
   guard g( g_mutex );
   return g_web_root;
}

bool get_using_ssl( )
{
   guard g( g_mutex );
   return g_using_ssl;
}

string get_mbox_path( )
{
   guard g( g_mutex );
   return g_mbox_path;
}

string get_mbox_username( )
{
   guard g( g_mutex );
   return g_mbox_username;
}

string get_pop3_server( )
{
   guard g( g_mutex );
   return g_pop3_server;
}

string get_pop3_suffix( )
{
   guard g( g_mutex );
   return g_pop3_suffix;
}

string get_pop3_username( )
{
   guard g( g_mutex );
   return g_pop3_username;
}

string get_pop3_password( )
{
   guard g( g_mutex );
   return decrypt_password( g_pop3_password );
}

string get_pop3_security( )
{
   guard g( g_mutex );
   return g_pop3_security;
}

string get_smtp_server( )
{
   guard g( g_mutex );
   return g_smtp_server;
}

string get_smtp_sender( )
{
   guard g( g_mutex );
   return g_smtp_sender;
}

string get_smtp_suffix( )
{
   guard g( g_mutex );
   return g_smtp_suffix;
}

string get_smtp_username( )
{
   guard g( g_mutex );
   return g_smtp_username;
}

string get_smtp_password( )
{
   guard g( g_mutex );
   return decrypt_password( g_smtp_password );
}

string get_smtp_security( )
{
   guard g( g_mutex );
   return g_smtp_security;
}

int get_smtp_max_send_attempts( )
{
   guard g( g_mutex );
   return g_smtp_max_send_attempts;
}

int64_t get_smtp_max_attached_data( )
{
   guard g( g_mutex );
   return g_smtp_max_attached_data;
}

string encrypt_password( const string& password, bool no_ssl, bool no_salt )
{
   string salt;
   if( !no_salt )
      salt = g_sid + c_salt_value;

   return password_encrypt( password, salt, !no_ssl );
}

string decrypt_password( const string& password, bool no_ssl, bool no_salt )
{
   string salt;
   if( !no_salt )
      salt = g_sid + c_salt_value;

   return password_decrypt( password, salt, !no_ssl );
}

int exec_system( const string& cmd, bool async )
{
   string s( cmd );

   // NOTE: For security against potentially malicious module code only permit system calls
   // from the autoscript session, via "run_script", or from either the "Meta" or "default"
   // (or "ciyam") storages.
   if( gtp_session && !gtp_session->running_script
    && gtp_session->p_storage_handler->get_name( ) != "Meta"
    && gtp_session->p_storage_handler->get_name( ) != "ciyam"
    && gtp_session->p_storage_handler->get_name( ) != c_default_storage_name )
      throw runtime_error( "invalid exec_system: " + cmd );

   // NOTE: The session variable @allow_async can be used to force non-async execution.
   if( get_session_variable( "@allow_async" ) == "0" || get_session_variable( "@allow_async" ) == "false" )
      async = false;

   if( async )
   {
#ifdef _WIN32
      s = "start /min " + s;
#else
      s += " &";
#endif
   }

   TRACE_LOG( TRACE_SESSIONS, s );

   return system( s.c_str( ) );
}

int run_script( const string& script_name, bool async )
{
   int rc;

   if( get_script_reconfig( ) && scripts_file_has_changed( ) )
      read_script_info( );

   if( !g_scripts.count( script_name ) )
      throw runtime_error( "unknown script '" + script_name + "'" );

   string filename( g_scripts[ script_name ].filename );
   bool is_script = ( filename == c_script_dummy_filename );

   string arguments( process_script_args( g_scripts[ script_name ].arguments, is_script ) );

   auto_ptr< restorable< bool > > ap_running_script;
   if( gtp_session )
      ap_running_script.reset( new restorable< bool >( gtp_session->running_script, true ) );

   if( is_script )
   {
      string script_args( "~" + uuid( ).as_string( ) );

      ofstream outf( script_args.c_str( ) );
      if( !outf )
         throw runtime_error( "unable to open '" + script_args + "' for output" );

      outf << "<" << arguments << endl;
      outf.close( );

      script_args += " " + script_name;

#ifdef _WIN32
      rc = exec_system( "script " + script_args, async );
#else
      rc = exec_system( "./script " + script_args, async );
#endif
   }
   else
   {
#ifdef _WIN32
      string cmd_and_args( filename );
#else
      string cmd_and_args( "./" + filename );
#endif

      if( !arguments.empty( ) )
         cmd_and_args += " " + arguments;

      rc = exec_system( cmd_and_args, async );
   }

   return rc;
}

string process_script_args( const string& raw_args, bool is_script_arg )
{
   string retval;

   if( !raw_args.empty( ) )
   {
      vector< string > all_args;
      split( raw_args, all_args, ' ' );

      for( size_t i = 0; i < all_args.size( ); i++ )
      {
         string next_arg( all_args[ i ] );

         if( !next_arg.empty( ) && next_arg[ 0 ] == '@' )
            next_arg = get_session_variable( next_arg );

         if( !retval.empty( ) )
            retval += " ";

         if( next_arg.empty( ) )
            next_arg = is_script_arg ? "\\\"\\\"" : "\"\"";
         else if( next_arg.find_first_of( "\" " ) != string::npos )
         {
            if( is_script_arg )
               escape( next_arg, "\"" );

            next_arg = "\"" + escaped( next_arg, "\"" ) + "\"";
         }

         retval += next_arg;
      }
   }

   return retval;
}

void generate_new_manuscript_sio( )
{
   if( file_exists( c_manuscript_file ) )
   {
      vector< string > new_lines;
      vector< string > original_lines;
      buffer_file_lines( c_manuscript_file, original_lines );

      bool found_start = false;
      size_t finish_line = 0;
      for( size_t i = 0; i < original_lines.size( ); i++ )
      {
         string next( original_lines[ i ] );

         if( !found_start )
            new_lines.push_back( next );

         if( next == "### [<start generated>]" )
            found_start = true;
         else if( found_start && next == "### [<finish generated>]" )
         {
            finish_line = i;
            break;
         }
      }

      file_filter ff;
      fs_iterator fs( ".", &ff );

      while( fs.has_next( ) )
      {
         if( wildcard_match( "*.extra.lst", fs.get_name( ) ) )
         {
            vector< string > lines;
            buffer_file_lines( fs.get_name( ), lines );

            for( size_t i = 0; i < lines.size( ); i++ )
            {
               if( file_exists( lines[ i ] ) )
               {
                  vector< string > script_lines;
                  buffer_file_lines( lines[ i ], script_lines );

                  bool is_manuscript = false;
                  for( size_t j = 0; j < script_lines.size( ); j++ )
                  {
                     string next( script_lines[ j ] );

                     // NOTE: Remove comment prefixes for console script files.
                     if( next.find( "#;" ) == 0 )
                        next.erase( 0, 1 );
                     else if( next.find( "REM;" ) == 0 )
                        next.erase( 0, 3 );

                     if( next.empty( ) || next[ 0 ] != ';' )
                     {
                        if( is_manuscript )
                           break;
                     }
                     else if( is_manuscript )
                        new_lines.push_back( next.substr( 1 ) );
                     else if( next.find( "; NOTE: " ) == 0 )
                     {
                        if( next.find( "'" + string( c_manuscript_file ) + "'" ) != string::npos )
                           is_manuscript = true;
                     }
                  }
               }
            }
         }
      }

      if( finish_line > 0 )
      {
         while( finish_line < original_lines.size( ) )
            new_lines.push_back( original_lines[ finish_line++ ] );
      }

      ofstream outf( ( string( c_manuscript_file ) + ".new" ).c_str( ) );
      for( size_t i = 0; i < new_lines.size( ); i++ )
         outf << new_lines[ i ] << '\n';
   }
}

size_t init_session( command_handler& cmd_handler )
{
   guard g( g_mutex );

   size_t sess_id;
   gtp_session = 0;
   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( !g_sessions[ i ] )
      {
         g_sessions[ i ] = new session( ++g_next_session_id, i, cmd_handler, g_storage_handlers[ 0 ] );
         gtp_session = g_sessions[ i ];
         ods::instance( 0, true );
         sess_id = i;
         break;
      }
   }

   if( !gtp_session )
      throw runtime_error( "max. permitted concurrent sessions already active" );

   set_default_session_variables( );

   return sess_id;
}

void term_session( )
{
   guard g( g_mutex );

   if( gtp_session )
   {
      for( size_t i = 0; i < g_max_sessions; i++ )
      {
         if( gtp_session == g_sessions[ i ] )
         {
            set< size_t >::iterator j;
            for( j = gtp_session->release_sessions.begin( ); j != gtp_session->release_sessions.end( ); ++j )
               release_session( *j, false );

            g_condemned_sessions.insert(
             gtp_session->condemned_sessions.begin( ), gtp_session->condemned_sessions.end( ) );

            delete g_sessions[ i ];
            g_sessions[ i ] = 0;
            gtp_session = 0;
            break;
         }
      }
   }

   if( gtp_session )
      throw runtime_error( "unable to terminate session" );
}

size_t session_id( )
{
   size_t rc = 0;

   if( gtp_session )
      rc = gtp_session->id;

   return rc;
}

void list_sessions( ostream& os, bool inc_dtms )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] )
      {
         os << g_sessions[ i ]->id;

         if( gtp_session && gtp_session->id == g_sessions[ i ]->id )
            os << '*';

         if( inc_dtms )
         {
            os << ' ' << g_sessions[ i ]->dtm_created.as_string( true, false )
             << ' ' << g_sessions[ i ]->dtm_last_cmd.as_string( true, false );
         }

         os << ' ' << g_sessions[ i ]->last_cmd;

         os << ' ' << g_sessions[ i ]->p_storage_handler->get_name( );
         if( g_sessions[ i ]->p_storage_handler->get_is_locked_for_admin( ) )
            os << '*';

         string uid( g_sessions[ i ]->uid );

         if( uid.empty( ) )
            uid = c_uid_unknown;
         else
         {
            // NOTE: If the session's "uid" is in the form <key>:<uid> then strip off the key
            // so that just the uid is displayed (with quotes surrounding so it's clearly not
            // a key). This format is not expected to be found for ops that are stored in the
            // transaction log (as only keys are stored in the log) but makes it a bit easier
            // for an administrator to identify "who is logged in" when it's used for some of
            // the more common non-logged operations such as "fetch".
            string::size_type pos = uid.find( ':' );

            if( pos != string::npos )
               uid = '"' + uid.substr( pos + 1 ) + '"';
         }

         os << ' ' << uid;

         os << ' ' << g_sessions[ i ]->instance_registry.size( ) << ':' << g_sessions[ i ]->next_handle;

         os << ' ' << g_sessions[ i ]->sql_count;

         if( g_sessions[ i ]->is_captured )
            os << '*';

         os << '\n';
      }
   }
}

command_handler& get_session_command_handler( )
{
   if( !gtp_session )
      throw runtime_error( "no session to return command handler for" );

   return gtp_session->cmd_handler;
}

void set_last_session_cmd( const string& cmd )
{
   guard g( g_mutex );

   if( gtp_session )
   {
      gtp_session->last_cmd = cmd;
      gtp_session->dtm_last_cmd = date_time::local( );
   }
}

void condemn_session( size_t sess_id, int num_seconds, bool force_uncapture, bool wait_until_term )
{
   guard g( g_mutex );

   // NOTE: This function is not designed to be used to self terminate (use condemn_this_session).
   if( gtp_session && sess_id == gtp_session->id )
      return;

   date_time dtm( date_time::local( ) + ( seconds )num_seconds );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id == sess_id )
      {
         if( force_uncapture )
            g_sessions[ i ]->is_captured = false;

         if( !wait_until_term )
            g_condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
         else
            gtp_session->condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
      }
   }
}

void condemn_this_session( )
{
   guard g( g_mutex );

   if( gtp_session )
      g_condemned_sessions.insert( make_pair( gtp_session->id, date_time::local( ) ) );
}

void condemn_all_other_sessions( int num_seconds, bool force_uncapture, bool wait_until_term )
{
   guard g( g_mutex );

   size_t sess_id = 0;
   if( gtp_session )
      sess_id = gtp_session->id;

   date_time dtm( date_time::local( ) + ( seconds )num_seconds );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id != sess_id )
      {
         if( force_uncapture )
            g_sessions[ i ]->is_captured = false;

         if( !wait_until_term )
            g_condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
         else
            gtp_session->condemned_sessions.insert( make_pair( g_sessions[ i ]->id, dtm ) );
      }
   }
}

bool is_condemned_session( )
{
   guard g( g_mutex );

   return gtp_session
    && ( ( g_condemned_sessions.count( gtp_session->id )
    && g_condemned_sessions[ gtp_session->id ] <= date_time::local( ) )
    || ( g_session_timeout && ( date_time::local( ) - gtp_session->dtm_last_cmd ) > g_session_timeout ) );
}

void capture_session( size_t sess_id )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id == sess_id )
      {
         g_sessions[ i ]->is_captured = true;
         break;
      }
   }
}

void capture_all_other_sessions( )
{
   guard g( g_mutex );

   size_t sess_id = 0;
   if( gtp_session )
      sess_id = gtp_session->id;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id != sess_id )
         g_sessions[ i ]->is_captured = true;
   }
}

bool is_captured_session( )
{
   guard g( g_mutex );
   return gtp_session && gtp_session->is_captured;
}

void release_session( size_t sess_id, bool wait_until_term )
{
   guard g( g_mutex );

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id == sess_id )
      {
         if( !wait_until_term )
            g_sessions[ i ]->is_captured = false;
         else
            gtp_session->release_sessions.insert( sess_id );

         break;
      }
   }
}

void release_all_other_sessions( bool wait_until_term )
{
   guard g( g_mutex );

   size_t sess_id = 0;
   if( gtp_session )
      sess_id = gtp_session->id;

   for( size_t i = 0; i < g_max_sessions; i++ )
   {
      if( g_sessions[ i ] && g_sessions[ i ]->id != sess_id )
      {
         if( !wait_until_term )
            g_sessions[ i ]->is_captured = false;
         else
            gtp_session->release_sessions.insert( sess_id );
      }
   }
}

bool session_skip_fk_fetches( )
{
   guard g( g_mutex );
   return gtp_session && gtp_session->skip_fk_fetches;
}

void session_skip_fk_fetches( bool skip_fk_fetches )
{
   guard g( g_mutex );

   if( gtp_session )
      gtp_session->skip_fk_fetches = skip_fk_fetches;
}

bool session_skip_validation( )
{
   guard g( g_mutex );
   return gtp_session && gtp_session->skip_validation;
}

void session_skip_validation( bool skip_validation )
{
   guard g( g_mutex );

   if( gtp_session )
      gtp_session->skip_validation = skip_validation;
}

bool session_skip_is_constained( )
{
   guard g( g_mutex );
   return gtp_session && gtp_session->skip_is_constrained;
}

void session_skip_is_constained( bool skip_is_constrained )
{
   guard g( g_mutex );

   if( gtp_session )
      gtp_session->skip_is_constrained = skip_is_constrained;
}

bool get_script_reconfig( )
{
   guard g( g_mutex );
   return g_script_reconfig;
}

string get_pem_password( )
{
   guard g( g_mutex );

   return decrypt_password( g_pem_password );
}

string get_sql_password( )
{
   guard g( g_mutex );

   string pwd;

   if( gtp_session
    && gtp_session->p_storage_handler->get_name( ) == "Meta" ) // i.e. only allow the Meta to do this
      pwd = decrypt_password( g_sql_password );

   return pwd;
}

string get_default_storage( )
{
   guard g( g_mutex );
   return g_default_storage;
}

void set_default_storage( const string& name )
{
   guard g( g_mutex );
   g_default_storage = name;
}

int get_session_timeout( )
{
   guard g( g_mutex );
   return g_session_timeout;
}

void set_session_timeout( int seconds )
{
   guard g( g_mutex );
   g_session_timeout = seconds;
}

string get_session_variable( const string& name )
{
   guard g( g_mutex );

   string retval;

   bool found = false;
   if( gtp_session )
   {
      if( gtp_session->variables.count( name ) )
      {
         found = true;
         retval = gtp_session->variables[ name ];
      }
      else if( name.find_first_of( "?*" ) != string::npos )
      {
         found = true;
         map< string, string >::const_iterator ci;
         for( ci = gtp_session->variables.begin( ); ci != gtp_session->variables.end( ); ++ci )
         {
            if( wildcard_match( name, ci->first ) )
            {
               if( !retval.empty( ) )
                  retval += "\n";
               retval += ci->first + ' ' + ci->second;
            }
         }
      }
   }

   if( !found )
   {
      if( name == c_session_variable_none )
         retval = " ";
      else if( name == c_session_variable_storage )
         retval = get_default_storage( );
   }

   return retval;
}

void set_session_variable( const string& name, const string& value )
{
   guard g( g_mutex );

   if( gtp_session )
   {
      if( value.empty( ) )
      {
         if( gtp_session->variables.count( name ) )
            gtp_session->variables.erase( name );
      }
      else
      {
         if( gtp_session->variables.count( name ) )
            gtp_session->variables[ name ] = value;
         else
            gtp_session->variables.insert( make_pair( name, value ) );
      }
   }
}

bool set_session_variable( const string& name, const string& value, const string& current )
{
   guard g( g_mutex );

   bool retval = false;

   if( gtp_session )
   {
      if( !gtp_session->variables.count( name ) )
      {
         if( current.empty( ) )
            retval = true;
      }
      else if( current == gtp_session->variables[ name ] )
      {
         retval = true;
         gtp_session->variables.erase( name );
      }

      if( retval && !value.empty( ) )
      {
         if( gtp_session->variables.count( name ) )
            gtp_session->variables[ name ] = value;
         else
            gtp_session->variables.insert( make_pair( name, value ) );
      }
   }

   return retval;
}

void set_default_session_variables( )
{
   set_session_variable( c_session_variable_storage, get_default_storage( ) );
}

string get_system_variable( const string& name )
{
   guard g( g_mutex );

   string retval;

   if( g_variables.count( name ) )
      retval = g_variables[ name ];
   else if( name.find_first_of( "?*" ) != string::npos )
   {
      map< string, string >::const_iterator ci;
      for( ci = g_variables.begin( ); ci != g_variables.end( ); ++ci )
      {
         if( wildcard_match( name, ci->first ) )
         {
            if( !retval.empty( ) )
               retval += "\n";
            retval += ci->first + ' ' + ci->second;
         }
      }
   }

   return retval;
}

void set_system_variable( const string& name, const string& value )
{
   guard g( g_mutex );

   if( value.empty( ) )
   {
      if( g_variables.count( name ) )
         g_variables.erase( name );
   }
   else
   {
      if( g_variables.count( name ) )
         g_variables[ name ] = value;
      else
         g_variables.insert( make_pair( name, value ) );
   }
}

bool set_system_variable( const string& name, const string& value, const string& current )
{
   guard g( g_mutex );

   bool retval = false;

   if( !g_variables.count( name ) )
   {
      if( current.empty( ) )
         retval = true;
   }
   else if( current == g_variables[ name ] )
   {
      retval = true;
      g_variables.erase( name );
   }

   if( retval && !value.empty( ) )
   {
      if( g_variables.count( name ) )
         g_variables[ name ] = value;
      else
         g_variables.insert( make_pair( name, value ) );
   }

   return retval;
}

void init_storage( const string& name,
 const string& directory, command_handler& cmd_handler, bool lock_for_admin )
{
   perform_storage_op( e_storage_op_init, name, directory, cmd_handler, lock_for_admin );
}

void create_storage( const string& name,
 const string& directory, command_handler& cmd_handler, bool lock_for_admin )
{
   perform_storage_op( e_storage_op_create, name, directory, cmd_handler, lock_for_admin );
}

void attach_storage( const string& name, command_handler& cmd_handler, bool lock_for_admin )
{
   perform_storage_op( e_storage_op_attach, name, "", cmd_handler, lock_for_admin );
}

void backup_storage( command_handler& cmd_handler, int* p_truncation_count )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform a backup whilst a transaction is active" );

      ods* p_ods( ods::instance( ) );
      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot backup a storage unless it has been locked for admin" );

      // NOTE: Create a SQL file (which is the storage name with a ".backup.sql" extension).
      string sql_file_name( handler.get_name( ) );
      sql_file_name += ".backup.sql";

      ofstream outf( sql_file_name.c_str( ) );
      if( !outf )
         throw runtime_error( "unable to open file '" + sql_file_name + "' for output" );

      vector< string >::const_iterator mci;
      for( mci = handler.get_root( ).module_list.begin( ); mci != handler.get_root( ).module_list.end( ); ++mci )
      {
         vector< string > class_list;
         list_module_classes( *mci, class_list );

         map< string, string > class_ids_and_names;
         list_module_classes( *mci, class_ids_and_names, true );

         outf << "BEGIN;\n";
         for( size_t i = 0; i < class_list.size( ); i++ )
         {
            vector< string > columns;
            string sql_columns( get_sql_columns_for_module_class( *mci, class_list[ i ] ) );

            if( !sql_columns.empty( ) )
            {
               split( sql_columns, columns );

               string table_name( "T_" + *mci + "_" + class_ids_and_names[ class_list[ i ] ] );

               // FUTURE: These messages should be handled as a server string messages.
               cmd_handler.output_progress(
                "Processing DDL and row data for " + table_name + "..." );

               outf << "\n#Creating table " << table_name << "...\n";

               outf << "\nDROP TABLE IF EXISTS " << table_name << ";\n";

               outf << "\nCREATE TABLE " << table_name << '\n';
               outf << "(\n";
               for( size_t j = 0; j < columns.size( ); j++ )
               {
                  if( j > 0 )
                     outf << ",\n";
                  outf << " " << columns[ j ];
               }
               outf << "\n);\n\n";

               size_t num_rows = 0;
               if( gtp_session->ap_db.get( ) )
               {
                  string select_sql( "SELECT * FROM " + table_name );

                  size_t handle = create_object_instance( *mci, class_list[ i ], 0, false );
                  class_base& instance( get_class_base_from_handle( handle, "" ) );

                  vector< string > sql_column_names;
                  sql_column_names.push_back( "C_Key_" );
                  sql_column_names.push_back( "C_Ver_" );
                  sql_column_names.push_back( "C_Rev_" );
                  sql_column_names.push_back( "C_Typ_" );
                  instance.get_sql_column_names( sql_column_names );

                  sql_dataset ds( *gtp_session->ap_db.get( ), select_sql );
                  while( ds.next( ) )
                  {
                     if( ds.get_fieldcount( ) != sql_column_names.size( ) )
                        throw runtime_error( "unexpected SQL columns mismatch" );

                     string insert_sql( "INSERT INTO " + table_name );
                     insert_sql += " VALUES (";

                     for( int col = 0; col < ds.get_fieldcount( ); col++ )
                     {
                        bool is_sql_numeric;
                        string field_name( sql_column_names[ col ].substr( 2 ) );
                        get_field_name( instance, field_name, &is_sql_numeric );

                        if( col > 0 )
                           insert_sql += ",";

                        if( is_sql_numeric )
                           insert_sql += ds.as_string( col );
                        else
                        {
                           string data( ds.as_string( col ) );
                           insert_sql += sql_quote( escaped( data, 0, '\\', "rn\r\n" ) );
                        }
                     }

                     outf << insert_sql << ");\n";

                     if( ++num_rows % 1000 == 0 )
                     {
                        // FUTURE: These messages should be handled as a server string messages.
                        if( num_rows % 10000 == 0 )
                           cmd_handler.output_progress( "Processed "
                            + to_string( num_rows ) + " rows for " + table_name + "..." );

                        outf << "\n#Inserted " << num_rows
                         << " rows into table " << table_name << "...\n\n";

                        outf << "COMMIT;\n";
                        outf << "BEGIN;\n";
                     }
                  }

                  // FUTURE: This message should be handled as a server string message.
                  if( num_rows % 1000 != 0 )
                     outf << "\n#Inserted " << num_rows
                      << " rows into table " << table_name << "...\n";

                  destroy_object_instance( handle );
               }

               vector< string > sql_indexes;
               get_sql_indexes_for_module_class( *mci, class_list[ i ], sql_indexes );

               // FUTURE: This message should be handled as a server string message.
               if( num_rows < 1000 )
                  outf << "\n#Creating indexes for table " << table_name << "...\n";

               string index_prefix( "I_" + *mci + "_" + class_ids_and_names[ class_list[ i ] ] );

               for( size_t j = 0; j < sql_indexes.size( ); j++ )
               {
                  vector< string > index_columns;
                  split( sql_indexes[ j ], index_columns );

                  if( num_rows >= 1000 )
                  {
                     // FUTURE: This message should be handled as a server string message.
                     outf << "\n#Creating index #";
                     if( j < 10 )
                        outf << '0';
                     outf << j << " for table " << table_name << "...\n";
                  }

                  outf << "\nCREATE UNIQUE INDEX " << index_prefix << "_";
                  if( j < 10 )
                     outf << '0';
                  outf << j << " ON " << table_name << '\n';
                  outf << "(\n";
                  for( size_t k = 0; k < index_columns.size( ); k++ )
                  {
                     if( k > 0 )
                        outf << ",\n";
                     outf << " " << index_columns[ k ];
                  }
                  outf << "\n);\n";
               }
            }
         }
      }

      outf << "\nCOMMIT;\n";
      outf.flush( );

      if( !outf.good( ) )
         throw runtime_error( "unexpected bad output stream" );

      if( p_truncation_count )
      {
         *p_truncation_count = ++gtp_session->p_storage_handler->get_root( ).truncation_count;
         *p_ods << gtp_session->p_storage_handler->get_root( );

         ostringstream osstr;
         osstr << handler.get_name( ) << ".log." << setw( 3 ) << setfill( '0' ) << *p_truncation_count;

         if( file_exists( osstr.str( ) ) )
            remove( osstr.str( ).c_str( ) );

         rename( ( handler.get_name( ) + ".log" ).c_str( ), osstr.str( ).c_str( ) );

         transaction_log_command( ";truncated at "
          + date_time::local( ).as_string( e_time_format_hhmmss, true ) );

         append_transaction_log_command( handler, true );
      }
   }
}

void restore_storage( command_handler& cmd_handler )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform a restore whilst a transaction is active" );

      ods* p_ods( ods::instance( ) );
      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot restore a storage unless it has been locked for admin" );

      string sql_file_name( handler.get_name( ) );
      sql_file_name += ".backup.sql";

      if( !file_exists( sql_file_name ) )
         throw runtime_error( "did not find backup file '" + sql_file_name + "'" );

      if( gtp_session->ap_db.get( ) )
         exec_sql_from_file( *gtp_session->ap_db, sql_file_name, &cmd_handler, true );
   }
}

void upgrade_storage( command_handler& cmd_handler )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot perform an upgrade whilst a transaction is active" );

      ods* p_ods( ods::instance( ) );
      storage_handler& handler( *gtp_session->p_storage_handler );

      if( !handler.get_is_locked_for_admin( ) )
         throw runtime_error( "cannot upgrade a storage unless it has been locked for admin" );

      string sql_file_name( handler.get_name( ) );
      sql_file_name += ".upgrade.sql";

      if( file_exists( sql_file_name ) && gtp_session->ap_db.get( ) )
         exec_sql_from_file( *gtp_session->ap_db, sql_file_name, &cmd_handler );
   }
}

void term_storage( command_handler& cmd_handler )
{
   guard g( g_mutex );

   if( ods::instance( ) )
   {
      while( !gtp_session->transactions.empty( ) )
         transaction_rollback( );

      // NOTE: Now store the "next_id" (rather than the "ceiling" value). The "force_write" option
      // is to ensure that store occurs regardless of whether the "oid_pointer" has registered any
      // change (as using reference aliases prevents that mechanism from working).
      gtp_session->p_storage_handler->get_root( ).o_identity.store( e_oid_pointer_opt_force_write );

      delete ods::instance( );
      ods::instance( 0, true );

      destroy_all_object_instances( );

      while( !gtp_session->storage_controlled_modules.empty( ) )
      {
         module_unload( gtp_session->storage_controlled_modules.back( ), cmd_handler );
         gtp_session->storage_controlled_modules.pop_back( );
      }

      module_directory( &g_empty_string );

      if( gtp_session->p_storage_handler->dec_ref_count( ) == 0 )
      {
         g_storage_handler_index.erase( gtp_session->p_storage_handler->get_name( ) );
         g_storage_handlers[ gtp_session->p_storage_handler->get_slot( ) ] = 0;

         gtp_session->p_storage_handler->release_bulk_write( );
         gtp_session->p_storage_handler->release_ods( );

         delete gtp_session->p_storage_handler;
      }

      set_session_variable( c_session_variable_storage, "" );
      gtp_session->p_storage_handler = g_storage_handlers[ 0 ];
   }
}

void slice_storage_log( command_handler& cmd_handler, const string& name, const vector< string >& module_list )
{
   perform_storage_op( e_storage_op_attach, name, "", cmd_handler, true );

   try
   {
      for( size_t i = 0; i < module_list.size( ); i++ )
      {
         if( !gtp_session->modules_by_name.count( module_list[ i ] ) )
            throw runtime_error( "unknown module '" + module_list[ i ] + "' for storage '" + name + "'" );
      }

      set< string > sorted_module_list( module_list.begin( ), module_list.end( ) );

      vector< pair< string, string > > modules_for_slice;

      size_t num_modules( gtp_session->p_storage_handler->get_root( ).module_list.size( ) );
      for( size_t i = 0; i < num_modules; i++ )
      {
         string next_module( gtp_session->p_storage_handler->get_root( ).module_list[ i ] );

         if( sorted_module_list.empty( ) || sorted_module_list.count( next_module ) )
            modules_for_slice.push_back( make_pair( next_module, gtp_session->modules_by_name.find( next_module )->second ) );
      }

      map< string, string > module_aliases;

      // NOTE: For each module being extracted create an .ltf file from the storage .ltf file (if exists).
      string ltf_file_name( name + ".ltf" );
      if( file_exists( ltf_file_name ) )
      {
         vector< string > ltf_lines;
         buffer_file_lines( ltf_file_name, ltf_lines );

         for( size_t i = 0; i < modules_for_slice.size( ); i++ )
         {
            string module_ltf_file_name( modules_for_slice[ i ].first + ".ltf.new" );

            ofstream outs( module_ltf_file_name.c_str( ) );
            if( !outs )
               throw runtime_error( "unexpected error opening '" + module_ltf_file_name + "' for output" );

            size_t num_lines = 0;
            for( size_t j = 0; j < ltf_lines.size( ); j++ )
            {
               vector< string > ltf_line_info;
               split( ltf_lines[ j ], ltf_line_info, ' ' );

               if( ltf_line_info.size( ) >= 3 )
               {
                  bool is_alias = false;

                  if( ltf_line_info[ 2 ] == "map_module" && ltf_line_info[ 3 ] == modules_for_slice[ i ].second )
                  {
                     is_alias = true;
                     module_aliases.insert( make_pair( ltf_line_info[ 1 ], ltf_line_info[ 3 ] ) );
                  }

                  if( is_alias || ltf_line_info[ 1 ] == modules_for_slice[ i ].second )
                  {
                     ++num_lines;
                     outs << ltf_lines[ j ] << '\n';
                  }
               }
            }

            outs.flush( );
            if( !outs.good( ) )
               throw runtime_error( "unexpected error occurred writing to '" + module_ltf_file_name + "'" );

            outs.close( );
            if( !num_lines )
               remove_file( module_ltf_file_name );
         }
      }

      vector< ofstream* > output_log_files;

      // NOTE: Iterate through the storage log writing matching log lines for each module being extracted
      // (taking into account any module mapping determined from the .ltf processing above).
      try
      {
         for( size_t i = 0; i < modules_for_slice.size( ); i++ )
         {
            string next_log_file_name( modules_for_slice[ i ].first + ".log.new" );
            output_log_files.push_back( new ofstream( next_log_file_name.c_str( ) ) );
            if( !*output_log_files.back( ) )
               throw runtime_error( "unable to open '" + next_log_file_name + "' for output" );
         }

         string log_file_name( name + ".log" );
         ifstream inpf( log_file_name.c_str( ) );

         if( !inpf )
            throw runtime_error( "unexpected error attempting to open '" + log_file_name + "' for reading" );

         string next;
         size_t line = 0;
         time_t ts( time( 0 ) );
         while( getline( inpf, next ) )
         {
            ++line;
            if( next.empty( ) )
               continue;

            if( next[ 0 ] != '[' )
               throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

            string::size_type pos = next.find( ']' );
            if( pos == string::npos )
               throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

            size_t tran_id = from_string< size_t >( next.substr( 1, pos - 1 ) );

            if( time( 0 ) - ts >= 10 )
            {
               ts = time( 0 );
               cmd_handler.output_progress( "Processed " + to_string( line ) + " lines..." );

               if( is_condemned_session( ) )
                  break;
            }

            if( tran_id == 0 )
            {
               for( size_t i = 0; i < output_log_files.size( ); i++ )
               {
                  *output_log_files[ i ] << "[0]" << uuid( ).as_string( ) << '\n';
                  *output_log_files[ i ] << "[1];module ==> " << modules_for_slice[ i ].first << '\n';
               }
            }
            else if( tran_id > 1 )
            {
               vector< string > next_items;
               split( next, next_items, ' ' );

               if( next_items.size( ) < 5 )
                  throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

               string module( next_items[ 3 ] );
               if( module_aliases.count( module ) )
                  module = module_aliases.find( module )->second;

               for( size_t i = 0; i < output_log_files.size( ); i++ )
               {
                  if( module == modules_for_slice[ i ].second )
                     *output_log_files[ i ] << next << '\n';
               }
            }
         }

         for( size_t i = 0; i < output_log_files.size( ); i++ )
         {
            output_log_files[ i ]->close( );
            delete output_log_files[ i ];
         }
      }
      catch( ... )
      {
         for( size_t i = 0; i < output_log_files.size( ); i++ )
            delete output_log_files[ i ];
         throw;
      }
   }
   catch( ... )
   {
      term_storage( cmd_handler );
      throw;
   }

   term_storage( cmd_handler );
}

void splice_storage_log( command_handler& cmd_handler, const string& name, const vector< string >& module_list )
{
   perform_storage_op( e_storage_op_attach, name, "", cmd_handler, true );

   try
   {
      for( size_t i = 0; i < module_list.size( ); i++ )
      {
         if( gtp_session->modules_by_name.count( module_list[ i ] ) )
            throw runtime_error( "module '" + module_list[ i ] + "' already exists in storage '" + name + "'" );
      }

      string ltf_file_name( name + ".ltf" );
      string new_ltf_file_name( name + ".ltf.new" );

      bool has_ltf_entries = false;
      if( file_exists( ltf_file_name ) )
      {
         has_ltf_entries = true;
         file_copy( ltf_file_name, new_ltf_file_name );
      }

      ofstream ltfs( new_ltf_file_name.c_str( ), ios::out | ios::app );
      if( !ltfs )
         throw runtime_error( "unable to open file '" + new_ltf_file_name + "' for output" );

      // NOTE: Append any .ltf files for the new modules to the main .ltf file.
      for( size_t i = 0; i < module_list.size( ); i++ )
      {
         string next_ltf_file_name( module_list[ i ] + ".ltf" );

         if( file_exists( next_ltf_file_name ) )
         {
            ifstream inpf( next_ltf_file_name.c_str( ) );
            copy_stream( inpf, ltfs );
            has_ltf_entries = true;
         }
      }

      ltfs.flush( );
      if( !ltfs.good( ) )
         throw runtime_error( "unexpected error occurred writing to '" + new_ltf_file_name + "'" );

      ltfs.close( );
      if( !has_ltf_entries )
         remove_file( new_ltf_file_name );

      string log_file_name( name + ".log" );
      string new_log_file_name( name + ".log.new" );

      ifstream logs( log_file_name.c_str( ) );
      if( !logs )
         throw runtime_error( "unable to open file '" + log_file_name + "' for input" );

      ofstream new_logs( new_log_file_name.c_str( ) );
      if( !new_logs )
         throw runtime_error( "unable to open file '" + new_log_file_name + "' for output" );

      new_logs << "[0]" << uuid( ).as_string( ) << '\n';

      vector< pair< ifstream*, string > > module_log_files;

      // NOTE: Splice together a new .log file using the existing .log file and those of each
      // module being added. The lines are ordered according to the transaction id numbering.
      try
      {
         for( size_t i = 0; i < module_list.size( ); i++ )
         {
            string next_log_file_name( module_list[ i ] + ".log" );
            module_log_files.push_back( make_pair( new ifstream( next_log_file_name.c_str( ) ), "" ) );
         }

         string next;
         size_t line = 0;
         time_t ts( time( 0 ) );
         while( getline( logs, next ) )
         {
            ++line;
            if( next.empty( ) )
               continue;

            if( next[ 0 ] != '[' )
               throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

            string::size_type pos = next.find( ']' );
            if( pos == string::npos )
               throw runtime_error( "unexpected formatting in log line #" + to_string( line ) + " ==> " + next );

            size_t tran_id = from_string< size_t >( next.substr( 1, pos - 1 ) );

            if( tran_id == 0 )
               continue;

            if( time( 0 ) - ts >= 10 )
            {
               ts = time( 0 );
               cmd_handler.output_progress( "Processed " + to_string( line ) + " lines..." );

               if( is_condemned_session( ) )
                  break;
            }

            multimap< size_t, string > new_lines;
            for( size_t i = 0; i < module_list.size( ); i++ )
            {
               while( *module_log_files[ i ].first )
               {
                  string next_module_line( module_log_files[ i ].second );

                  size_t module_tran_id = 0;

                  if( !next_module_line.empty( ) )
                  {
                     if( next_module_line[ 0 ] != '[' )
                        throw runtime_error( "unexpected formatting in ==> " + next_module_line );

                     string::size_type pos = next_module_line.find( ']' );
                     if( pos == string::npos )
                        throw runtime_error( "unexpected formatting in log line ==> " + next_module_line );

                     module_tran_id = from_string< size_t >( next_module_line.substr( 1, pos - 1 ) );

                     if( module_tran_id && module_tran_id < tran_id )
                        new_lines.insert( make_pair( module_tran_id, next_module_line ) );
                  }

                  if( module_tran_id >= tran_id )
                     break;

                  getline( *module_log_files[ i ].first, module_log_files[ i ].second );
               }
            }

            for( multimap< size_t, string >::iterator i = new_lines.begin( ); i!= new_lines.end( ); ++i )
               new_logs << i->second << '\n';

            new_logs << next << '\n';
         }

         for( size_t i = 0; i < module_log_files.size( ); i++ )
            delete module_log_files[ i ].first;

         string modules_file_name( name + ".modules.lst" );
         string new_modules_file_name( name + ".modules.lst.new" );

         if( file_exists( modules_file_name ) )
            file_copy( modules_file_name, new_modules_file_name );

         ofstream new_mods( new_modules_file_name.c_str( ), ios::out | ios::app );
         for( size_t i = 0; i < module_list.size( ); i++ )
            new_mods << module_list[ i ] << '\n';
      }
      catch( ... )
      {
         for( size_t i = 0; i < module_log_files.size( ); i++ )
            delete module_log_files[ i ].first;
         throw;
      }
   }
   catch( ... )
   {
      term_storage( cmd_handler );
      throw;
   }

   term_storage( cmd_handler );
}

bool storage_is_dead_key( const string& cid, const string& key )
{
   guard g( g_mutex );

   bool found = false;

   string dead_key( cid + ':' + key );

   const set< string >& dead_keys( gtp_session->p_storage_handler->get_dead_keys( ) );

   if( !dead_keys.empty( ) )
   {
      set< string >::const_iterator ci = dead_keys.lower_bound( dead_key );

      if( ci != dead_keys.end( ) )
         found = wildcard_match( *ci, dead_key );

      if( !found && ci != dead_keys.begin( ) )
      {
         --ci;
         found = wildcard_match( *ci, dead_key );
      }
   }

   return found;
}

void storage_add_dead_key( const string& cid, const string& key, bool is_prefix )
{
   guard g( g_mutex );

   string dead_key( cid + ':' + key );

   if( is_prefix )
      dead_key += '*';

   string dead_keys_file( gtp_session->p_storage_handler->get_name( ) + c_dead_keys_ext );

   ofstream outf( dead_keys_file.c_str( ), ios::out | ios::app );
   if( !outf )
      throw runtime_error( "unable to open '" + dead_keys_file + "' for output" );

   outf << dead_key << '\n';
   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "*** unexpected error occurred writing to '" + dead_keys_file + " ***" );

   gtp_session->p_storage_handler->get_dead_keys( ).insert( dead_key );
}

storage_bulk_write_pause::storage_bulk_write_pause( )
{
   guard g( g_mutex );

   if( ods::instance( ) )
      gtp_session->p_storage_handler->release_bulk_write( );
}

storage_bulk_write_pause::~storage_bulk_write_pause( )
{
   guard g( g_mutex );

   if( ods::instance( ) )
      gtp_session->p_storage_handler->obtain_bulk_write( );
}

string storage_name( )
{
   return gtp_session->p_storage_handler->get_name( );
}

string storage_identity( )
{
   return gtp_session->p_storage_handler->get_root( ).identity;
}

string storage_module_directory( )
{
   return gtp_session->p_storage_handler->get_root( ).module_directory;
}

string storage_web_root( bool expand )
{
   guard g( g_mutex );

   string path( gtp_session->p_storage_handler->get_root( ).web_root );
   if( expand )
   {
      path = search_replace( path, c_expand_root, get_web_root( ) );
      path = search_replace( path, c_expand_store, lower( storage_name( ) ) );
   }

   return path;
}

void storage_web_root( const string& new_root )
{
   guard g( g_mutex );
   ods* p_ods( ods::instance( ) );

   gtp_session->p_storage_handler->get_root( ).web_root = new_root;

   if( p_ods )
   {
      ods::transaction tx( *p_ods );
      *p_ods << gtp_session->p_storage_handler->get_root( );
      tx.commit( );

      gtp_session->transaction_log_command = ";web_root ==> " + new_root;
      append_transaction_log_command( *gtp_session->p_storage_handler );
   }
}

void storage_lock_all_tables( )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
         throw runtime_error( "cannot lock tables whilst a transaction is active" );

      ods* p_ods( ods::instance( ) );
      storage_handler& handler( *gtp_session->p_storage_handler );

      try
      {
         string all_table_info;

         vector< string >::const_iterator mci;
         for( mci = handler.get_root( ).module_list.begin( ); mci != handler.get_root( ).module_list.end( ); ++mci )
         {
            vector< string > class_list;
            list_module_classes( *mci, class_list );

            map< string, string > class_ids_and_names;
            list_module_classes( *mci, class_ids_and_names, true );

            for( size_t i = 0; i < class_list.size( ); i++ )
            {
               vector< string > columns;
               string sql_columns( get_sql_columns_for_module_class( *mci, class_list[ i ] ) );

               if( !sql_columns.empty( ) )
               {
                  string table_name( "T_" + *mci + "_" + class_ids_and_names[ class_list[ i ] ] );

                  if( !all_table_info.empty( ) )
                     all_table_info += ",";
                  all_table_info += table_name + " WRITE";
               }
            }
         }

         if( !all_table_info.empty( ) )
            exec_sql( *gtp_session->ap_db, "LOCK TABLES " + all_table_info );
      }
      catch( ... )
      {
         exec_sql( *gtp_session->ap_db, "UNLOCK TABLES" );
         throw;
      }
   }
}

void storage_unlock_all_tables( )
{
   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
      exec_sql( *gtp_session->ap_db, "UNLOCK TABLES" );
}

bool storage_locked_for_admin( )
{
   return gtp_session->p_storage_handler->get_is_locked_for_admin( );
}

ods& storage_instance( )
{
   if( !ods::instance( ) )
      throw runtime_error( "storage has not been initialised" );

   return *ods::instance( );
}

string gen_key( const char* p_suffix )
{
   string key;

   // NOTE: Automatically generate a key using the session id and current date/time.
   if( gtp_session )
   {
      date_time dtm( date_time::local( ) );
      size_t sess_slot( gtp_session->slot );

      char sss[ ] = "sss";
      sss[ 0 ] = '0' + ( sess_slot / 100 );
      sss[ 1 ] = '0' + ( ( sess_slot % 100 ) / 10 );
      sss[ 2 ] = '0' + ( sess_slot % 10 );

      key = dtm.as_string( ) + string( sss );
      if( p_suffix )
         key += string( p_suffix );

      // KLUDGE: Time granularity restrictions (at least under Win32) prevent more than one key being created
      // every 20 milliseconds within each session so add a delay to ensure duplicates will not be generated.
      msleep( 20 );
   }

   return key;
}

string get_uid( bool remove_display_name )
{
   string uid( gtp_session->uid );

   string::size_type pos = string::npos;
   if( remove_display_name )
      pos = uid.find( ':' );

   return uid.substr( 0, pos );
}

void set_uid( const string& uid )
{
   string s( uid );

   string::size_type pos = uid.find( ':' );
   string::size_type spos = uid.find( '!' );

   gtp_session->sec.erase( );
   set_session_variable( c_session_variable_sec, "" );

   if( spos != string::npos )
   {
      if( pos == string::npos || pos > spos )
      {
         string sec = uid.substr( spos + 1, pos == string::npos ? pos : pos - spos - 1 );

         gtp_session->sec = sec;
         set_session_variable( c_session_variable_sec, sec );

         s = uid.substr( 0, spos );
         if( pos != string::npos )
            s += uid.substr( pos );
      }
   }

   gtp_session->uid = s;
   set_session_variable( c_session_variable_uid, s );
}

bool is_sys_uid( )
{
   return get_uid( ) == "sys";
}

bool is_auto_uid( )
{
   return get_uid( ) == "auto";
}

bool is_init_uid( )
{
   return get_uid( ) == "init";
}

bool is_admin_uid( )
{
   return get_uid( ) == "admin";
}

bool is_system_uid( )
{
   bool rc = false;
   string uid( get_uid( ) );

   if( uid == "sys" || uid == "auto" || uid == "init" )
      rc = true;

   return rc;
}

bool is_admin_uid_key( const string& key )
{
   return key == "admin";
}

bool is_uid_not_self_and_not_in_set( const string& key, const string& key_set )
{
   bool rc = false;

   if( !is_admin_uid_key( key ) )
   {
      if( key_set.empty( ) )
         rc = ( key != get_uid( ) );
      else if( key != get_uid( ) )
      {
         set< string > keys;
         split( key_set, keys );

         rc = !keys.count( key );
      }
   }

   return rc;
}

bool has_sec_level( const string& level )
{
   // NOTE: Security level strings (e.g. XXXX) are shorter for higher.
   if( gtp_session->sec.empty( ) || gtp_session->sec <= level )
      return true;
   else
      return false;
}

string get_dtm( )
{
   return gtp_session->dtm;
}

void set_dtm( const string& dtm )
{
   gtp_session->dtm = dtm;
   set_session_variable( c_session_variable_dtm, dtm );
}

void set_class( const string& mclass )
{
   set_session_variable( c_session_variable_class, mclass );
}

void set_module( const string& module )
{
   set_session_variable( c_session_variable_module, module );
}

string get_tz_abbr( )
{
   return gtp_session->tz_abbr;
}

void set_tz_abbr( const string& tz_abbr )
{
   string tz( tz_abbr );
   if( tz.empty( ) )
      tz = get_timezone( );

   gtp_session->tz_abbr = tz;
   set_session_variable( c_session_variable_tz_abbr, tz );
}

const set< string >& get_perms( )
{
   return gtp_session->perms;
}

void set_perms( const set< string >& perms )
{
   gtp_session->perms = perms;
}

string get_tmp_directory( )
{
   return gtp_session->tmp_directory;
}

void set_tmp_directory( const string& tmp_directory )
{
   gtp_session->tmp_directory = tmp_directory;
}

size_t get_next_handle( )
{
   return ++gtp_session->next_handle;
}

void module_list( ostream& os )
{
   for( module_const_iterator
    mci = gtp_session->modules_by_name.begin( ), end = gtp_session->modules_by_name.end( ); mci != end; ++mci )
      os << mci->second << ' ' << mci->first << '\n';
}

size_t module_count( )
{
   return gtp_session->modules_by_name.size( );
}

void module_class_list( const string& module, ostream& os )
{
   module_const_iterator mci = gtp_session->modules_by_id.find( module );
   if( mci == gtp_session->modules_by_id.end( ) )
   {
      mci = gtp_session->modules_by_name.find( module );

      if( mci == gtp_session->modules_by_name.end( ) )
         throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
          make_pair( c_str_parm_module_not_loaded_module, module ) ) );
   }

   module_class_list_error rc = list_module_classes( module, os );
   if( rc != e_module_class_list_error_none )
      throw runtime_error( "unexpected module class list error #" + to_string( rc ) );
}

void module_strings_list( const string& module, ostream& os )
{
   module_const_iterator mci = gtp_session->modules_by_id.find( module );
   if( mci == gtp_session->modules_by_id.end( ) )
   {
      mci = gtp_session->modules_by_name.find( module );

      if( mci == gtp_session->modules_by_name.end( ) )
         throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
          make_pair( c_str_parm_module_not_loaded_module, module ) ) );
   }

   module_string_list_error rc = list_module_strings( module, os );
   if( rc != e_module_string_list_error_none )
      throw runtime_error( "unexpected module string list error #" + to_string( rc ) );
}

void module_class_fields_list( const string& module, const string& class_id_or_name, ostream& os )
{
   module_const_iterator mci = gtp_session->modules_by_id.find( module );
   if( mci == gtp_session->modules_by_id.end( ) )
   {
      mci = gtp_session->modules_by_name.find( module );

      if( mci == gtp_session->modules_by_name.end( ) )
         throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
          make_pair( c_str_parm_module_not_loaded_module, module ) ) );
   }

   module_class_field_list_error rc = list_module_class_fields( module, class_id_or_name, os );
   if( rc != e_module_class_field_list_error_none )
      throw runtime_error( "unexpected module class field list error #" + to_string( rc ) );
}

void module_load( const string& module_name,
 command_handler& cmd_handler, bool log_tx_comment, bool append_to_module_list )
{
   if( gtp_session->modules_by_name.find( module_name ) != gtp_session->modules_by_name.end( ) )
      throw runtime_error( get_string_message( GS( c_str_module_is_loaded ),
       make_pair( c_str_parm_module_is_loaded_module, module_name ) ) );

   module_load_error rc = load_module( module_name );

   if( rc != e_module_load_error_none )
   {
      if( rc == e_module_load_error_file_does_not_exist )
         throw runtime_error( get_string_message( GS( c_str_module_not_exists ),
          make_pair( c_str_parm_module_not_exists_module, module_name ) ) );
      else if( rc == e_module_load_error_external_module_failure )
         throw runtime_error( get_string_message( GS( c_str_module_depends_reqd ),
          make_pair( c_str_parm_module_depends_reqd_module, module_name ) ) );
      else
         throw runtime_error( "unexpected module load error #" + to_string( rc ) );
   }

   if( ods::instance( ) && gtp_session->p_storage_handler->get_ods( ) )
   {
      if( ods::instance( )->get_transaction_level( ) )
      {
         unload_module( module_name );
         throw runtime_error( "cannot load a module whilst a transaction is active" );
      }

      auto_ptr< guard > ap_guard( new guard( g_mutex ) );

      ods* p_ods( ods::instance( ) );
      storage_handler& handler( *gtp_session->p_storage_handler );

      if( handler.get_root( ).modules.find( module_name ) == handler.get_root( ).modules.end( ) )
      {
         if( !handler.get_is_locked_for_admin( ) )
         {
            unload_module( module_name );
            throw runtime_error( "modules can only be added to a storage during storage administration" );
         }

         gtp_session->storage_controlled_modules.push_back( module_name );

         handler.get_root( ).module_list.push_back( module_name );
         handler.get_root( ).modules.insert( make_pair( module_name, oid_pointer< storable_module >( ) ) );

         try
         {
            storable_module& module( *handler.get_root( ).modules[ module_name ] );

            string temp_sql_file_name;

            if( module.get_id( ).is_new( ) )
            {
               vector< string > class_list;
               list_module_classes( module_name, class_list );

               map< string, string > class_ids_and_names;
               list_module_classes( module_name, class_ids_and_names, true );

               // NOTE: Create/append to a DDL file (which is the storage name with a ".sql" extension).
               string sql_file_name( handler.get_name( ) );
               sql_file_name += ".sql";

               temp_sql_file_name = "~" + sql_file_name;

               bool is_first = true;
               bool file_existed( file_exists( sql_file_name ) );

               ofstream out1( sql_file_name.c_str( ), ios::out | ios::app );
               if( !out1 )
                  throw runtime_error( "unable to open file '" + sql_file_name + "' for output/append" );

               ofstream out2( temp_sql_file_name.c_str( ) );
               if( !out2 )
                  throw runtime_error( "unable to open file '" + temp_sql_file_name + "' for output" );

               if( file_existed )
                  out1 << '\n';

               tee_stream outf( out1, out2 );
               outf << "BEGIN;\n";

               for( size_t i = 0; i < class_list.size( ); i++ )
               {
                  vector< string > columns;
                  string sql_columns( get_sql_columns_for_module_class( module_name, class_list[ i ] ) );

                  if( !sql_columns.empty( ) )
                  {
                     split( sql_columns, columns );

                     string table_name( "T_" + module_name + "_" + class_ids_and_names[ class_list[ i ] ] );

                     outf << "\nDROP TABLE IF EXISTS " << table_name << ";\n";

                     // FUTURE: This message should be handled as a server string message.
                     outf << "\n#Creating table and indexes for " << module_name
                      << "_" << class_ids_and_names[ class_list[ i ] ] << "...\n";

                     outf << "\nCREATE TABLE " << table_name << '\n';
                     outf << "(\n";
                     for( size_t j = 0; j < columns.size( ); j++ )
                     {
                        if( j > 0 )
                           outf << ",\n";
                        outf << " " << columns[ j ];
                     }
                     outf << "\n);\n";

                     vector< string > sql_indexes;
                     get_sql_indexes_for_module_class( module_name, class_list[ i ], sql_indexes );

                     string index_prefix( "I_" + module_name + "_" + class_ids_and_names[ class_list[ i ] ] );

                     for( size_t j = 0; j < sql_indexes.size( ); j++ )
                     {
                        vector< string > index_columns;
                        split( sql_indexes[ j ], index_columns );

                        outf << "\nCREATE UNIQUE INDEX " << index_prefix << "_";
                        if( j < 10 )
                           outf << '0';
                        outf << j << " ON " << table_name << '\n';
                        outf << "(\n";
                        for( size_t k = 0; k < index_columns.size( ); k++ )
                        {
                           if( k > 0 )
                              outf << ",\n";
                           outf << " " << index_columns[ k ];
                        }
                        outf << "\n);\n";
                     }
                  }
               }

               outf << "\nCOMMIT;\n";
               outf.flush( );

               if( gtp_session->ap_db.get( ) )
               {
                  // NOTE: As MySQL DDL operations with InnoDB can be very slow the global
                  // thread lock is released whilst performing the DDL. As the storage has
                  // already been locked for administration this should be of no concern.
                  ap_guard.reset( );
                  try
                  {
                     exec_sql_from_file( *gtp_session->ap_db, temp_sql_file_name, &cmd_handler );
                  }
                  catch( ... )
                  {
                     // NOTE: Restore the storage state (otherwise a SQL error can be lost
                     // due to the fact that the storage is left in an inconsistent state).
                     gtp_session->storage_controlled_modules.pop_back( );
                     *p_ods >> handler.get_root( );
                     throw;
                  }

                  ap_guard.reset( new guard( g_mutex ) );
               }

               ods::transaction tx( *p_ods );

               *p_ods << module;

               // FUTURE: Performing a "set_id" on the module's "oid_pointer" is inefficient
               // as the "storable_module" object itself is destroyed when doing this.
               handler.get_root( ).modules[ module_name ].set_id( module.get_id( ) );
               *p_ods << handler.get_root( );

               tx.commit( );

               if( log_tx_comment )
               {
                  gtp_session->transaction_log_command = ";module ==> " + module_name;
                  append_transaction_log_command( handler, true, handler.get_root( ).module_list.size( ) );
               }

               if( append_to_module_list )
               {
                  string module_list_file( handler.get_name( ) + ".modules.lst" );

                  set< string > existing_modules;

                  if( file_exists( module_list_file ) )
                     buffer_file_lines( module_list_file, existing_modules );

                  if( !existing_modules.count( module_name ) )
                  {
                     ofstream modf( module_list_file.c_str( ), ios::out | ios::app );

                     if( !modf )
                        throw runtime_error( "unexpected error opening '" + module_list_file + "' for output/append" );

                     modf << module_name << endl;
                  }
               }
            }

            if( !temp_sql_file_name.empty( ) )
               remove( temp_sql_file_name.c_str( ) );
         }
         catch( ods_error& err )
         {
            unload_module( module_name );
            throw runtime_error( err.what( ) );
         }
         catch( ... )
         {
            unload_module( module_name );
            throw;
         }
      }
   }

   vector< string > class_list;
   list_module_classes( module_name, class_list );

   gtp_session->modules_by_id.insert( module_value_type( get_module_id( module_name ), module_name ) );
   gtp_session->modules_by_name.insert( module_value_type( module_name, get_module_id( module_name ) ) );

   register_module_commands( module_name, cmd_handler );
}

void module_unload( const string& module_name, command_handler& cmd_handler, bool check_controlled )
{
   if( check_controlled )
   {
      for( size_t i = 0; i < gtp_session->storage_controlled_modules.size( ); i++ )
      {
         if( gtp_session->storage_controlled_modules[ i ] == module_name )
            throw runtime_error( "cannot unload storage controlled module '" + module_name + "'" );
      }
   }

   module_iterator mi = gtp_session->modules_by_name.find( module_name );
   if( mi == gtp_session->modules_by_name.end( ) )
      throw runtime_error( get_string_message( GS( c_str_module_not_loaded ),
       make_pair( c_str_parm_module_not_loaded_module, module_name ) ) );

   destroy_object_instances( module_name );
   unregister_module_commands( module_name, cmd_handler );

   module_unload_error rc = unload_module( module_name );

   if( rc != e_module_unload_error_none )
   {
      if( rc == e_module_unload_error_name_unknown )
         throw runtime_error( "module '" + module_name + "' was not found to be loaded" );
      else
         throw runtime_error( "unexpected module unload error #" + to_string( rc ) );
   }

   gtp_session->modules_by_name.erase( mi );

   mi = gtp_session->modules_by_id.find( get_module_id( module_name ) );
   if( mi != gtp_session->modules_by_id.end( ) )
      gtp_session->modules_by_id.erase( mi );
}

void module_unload_all( command_handler& cmd_handler )
{
   vector< string > module_names;
   for( module_iterator mi = gtp_session->modules_by_name.begin( ); mi != gtp_session->modules_by_name.end( ); ++mi )
      module_names.push_back( mi->first );

   for( size_t i = 0; i < module_names.size( ); i++ )
      module_unload( module_names[ i ], cmd_handler );
}

void register_module_commands( const string& module_name, command_handler& handler )
{
   dynamic_library* p_dynamic_library = get_module_ptr( module_name );

   fp_obtain_module_commands_func obtain_module_commands;
   obtain_module_commands =
    ( fp_obtain_module_commands_func )p_dynamic_library->bind_to_function( c_obtain_module_commands_func_name );

   if( obtain_module_commands )
   {
      size_t num_commands( 0 );
      command_definition* p_command_definitions;
      command_functor_creator* p_command_functor_creator;

      ( *obtain_module_commands )( p_command_functor_creator, p_command_definitions, &num_commands );

      if( num_commands )
      {
         handler.add_commands( 0, module_name + c_module_prefix_separator,
          *p_command_functor_creator, p_command_definitions, num_commands );

         gtp_session->commands_registry.insert( module_commands_registry_value_type( module_name,
          module_commands( num_commands, p_command_definitions, p_command_functor_creator ) ) );
      }
   }
}

void unregister_module_commands( const string& module_name, command_handler& handler )
{
   module_commands_registry_container& commands_registry( gtp_session->commands_registry );

   module_commands_registry_iterator i = commands_registry.find( module_name );
   if( i != commands_registry.end( ) )
   {
      handler.remove_commands( module_name + c_module_prefix_separator,
       i->second.p_command_definitions, i->second.num_commands );

      commands_registry.erase( i );
   }
}

void unregister_all_module_commands( command_handler& handler )
{
   module_commands_registry_container& commands_registry( gtp_session->commands_registry );

   module_commands_registry_iterator i( commands_registry.begin( ) );
   while( i != commands_registry.end( ) )
   {
      handler.remove_commands( i->first + c_module_prefix_separator,
       i->second.p_command_definitions, i->second.num_commands );

      commands_registry.erase( i++ );
   }
}

void list_object_instances( ostream& os )
{
   object_instance_registry_iterator oiri, end;
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );
   for( oiri = instance_registry.begin( ), end = instance_registry.end( ); oiri != end; ++oiri )
   {
      os << oiri->first
       << ' ' << ( oiri->second ).p_dynamic_library->get_module_name( )
       << ' ' << ( oiri->second ).p_class_base->class_name( ) << '\n';
   }
}

size_t create_object_instance( const string& module, const string& class_id, size_t handle, bool dynamic_enabled )
{
   dynamic_library* p_dynamic_library = get_module_ptr( module );
   if( !p_dynamic_library )
      throw runtime_error( "unknown module '" + module + "'" );

   class_base* p_class_base = construct_object( module, class_id );
   if( !p_class_base )
      throw runtime_error( "unknown class id '" + class_id + "'" );

   class_base_accessor cba( *p_class_base );

   cba.set_default_values( );

   if( dynamic_enabled )
      cba.set_is_dynamic_enabled( true );

   object_instance_registry_container& instance_registry( gtp_session->instance_registry );

   if( handle == 0 )
      handle = get_next_handle( );
   else
   {
      if( instance_registry.find( handle ) != instance_registry.end( ) )
         throw runtime_error( "object instance handle #" + to_string( handle ) + " is currently in use" );
   }

   instance_registry.insert(
    object_instance_registry_value_type( handle, instance_info( p_class_base, p_dynamic_library ) ) );

   return handle;
}

string execute_object_command( size_t handle, const string& context, const string& method_name_and_args )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   TRACE_LOG( TRACE_CLASSOPS, "execute_object_command( ) [class: " + to_string( instance.class_name( ) ) + "] " + method_name_and_args );

   return instance.execute( method_name_and_args );
}

void init_object_instance( size_t handle, const string& context, bool for_create )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.init( for_create );
}

void prepare_object_instance( size_t handle, const string& context, bool for_create )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.prepare( for_create );
}

void validate_object_instance( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   if( !instance.is_valid( false ) )
      throw runtime_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );
}

void destroy_object_instance( size_t handle )
{
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );
   object_instance_registry_iterator oiri = instance_registry.find( handle );
   if( oiri == instance_registry.end( ) )
      throw runtime_error( "invalid object instance handle #" + to_string( handle ) );

   class_base* p_class_base( ( oiri->second ).p_class_base );
   dynamic_library* p_dynamic_library( ( oiri->second ).p_dynamic_library );
   destroy_object( p_dynamic_library->get_module_name( ), p_class_base->class_id( ), p_class_base );

   assert( p_class_base == 0 );

   instance_registry.erase( oiri );
}

void destroy_object_instances( const string& module_name )
{
   dynamic_library* p_dynamic_library = get_module_ptr( module_name );
   if( !p_dynamic_library )
      throw runtime_error( "unknown module name '" + module_name + "'" );

   object_instance_registry_iterator oiri, end;
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );
   for( oiri = instance_registry.begin( ), end = instance_registry.end( ); oiri != end; )
   {
      if( ( oiri->second ).p_dynamic_library == p_dynamic_library )
      {
         class_base* p_class_base( ( oiri->second ).p_class_base );
         dynamic_library* p_dynamic_library( ( oiri->second ).p_dynamic_library );
         destroy_object( p_dynamic_library->get_module_name( ), p_class_base->class_id( ), p_class_base );

         assert( p_class_base == 0 );

         instance_registry.erase( oiri++ );
      }
      else
         oiri++;
   }
}

void destroy_all_object_instances( )
{
   object_instance_registry_iterator oiri, end;
   object_instance_registry_container& instance_registry( gtp_session->instance_registry );
   for( oiri = instance_registry.begin( ), end = instance_registry.end( ); oiri != end; ++oiri )
   {
      class_base* p_class_base( ( oiri->second ).p_class_base );

      dynamic_library* p_dynamic_library( ( oiri->second ).p_dynamic_library );
      destroy_object( p_dynamic_library->get_module_name( ), p_class_base->class_id( ), p_class_base );

      assert( p_class_base == 0 );
   }

   instance_registry.clear( );
}

string get_class_id( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.class_id( );
}

string get_class_name( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.class_name( );
}

string get_class_display_name( size_t handle, const string& context, bool plural )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_display_name( plural );
}

void get_all_enum_pairs( size_t handle, const string& context, vector< pair< string, string > >& pairs )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.get_all_enum_pairs( pairs );
}

void get_all_field_names( size_t handle, const string& context, vector< string >& fields )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   for( int i = 0; i < instance.get_num_fields( ); i++ )
      fields.push_back( instance.get_field_name( i ) );
}

string get_field_name_for_id( size_t handle, const string& context, const string& id, bool no_throw )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   map< string, string > ids_to_names;

   field_info_container field_info;
   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      ids_to_names.insert( make_pair( field_info[ i ].id, field_info[ i ].name ) );

   if( !ids_to_names.count( id ) )
   {
      if( no_throw )
         return string( );
      else
         throw runtime_error( "unknown field id '" + id + "' in get_field_name_for_id" );
   }

   return ids_to_names[ id ];
}

string get_field_id_for_name( size_t handle, const string& context, const string& name, bool no_throw )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   map< string, string > names_to_ids;

   field_info_container field_info;
   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      names_to_ids.insert( make_pair( field_info[ i ].name, field_info[ i ].id ) );

   if( !names_to_ids.count( name ) )
   {
      if( no_throw )
         return string( );
      else
         throw runtime_error( "unknown field name '" + name + "' in get_field_id_for_name" );
   }

   return names_to_ids[ name ];
}

string get_field_type_name( size_t handle, const string& context, const string& id_or_name )
{
   string type_name;

   class_base& instance( get_class_base_from_handle( handle, context ) );

   field_info_container field_info;
   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( field_info[ i ].id == id_or_name || field_info[ i ].name == id_or_name )
      {
         type_name = field_info[ i ].type_name;
         break;
      }
   }

   if( type_name.empty( ) )
      throw runtime_error( "unknown field '" + id_or_name + "' in get_field_type_name " );

   return type_name;
}

string get_field_display_name( size_t handle, const string& context, const string& id_or_name )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   string field( id_or_name );
   if( !instance_accessor.get_field_name( field ) )
   {
      const char* p_id = instance_accessor.get_field_id( field );
      if( p_id )
         field = string( p_id );
   }

   return instance.get_field_display_name( field );
}

void inline add_next_value( bool as_csv, const string& next_value, string& field_values )
{
   if( !as_csv )
      field_values += escaped( next_value, ",\"", c_esc, "rn\r\n" );
   else
   {
      string next_csv_value;
      if( next_value.find_first_of( ",\"\r\n" ) == string::npos )
         next_csv_value = next_value;
      else
         next_csv_value = "\"" + escaped( replaced( next_value, "\r\n", "\n" ), "\"", '"' ) + "\"";

      field_values += next_csv_value;
   }
}

string get_field_values( size_t handle,
 const string& parent_context, const vector< string >& field_list,
 const string& tz_abbr, bool is_default, bool as_csv, vector< string >* p_raw_values,
 const map< int, string >* p_inserts, const map< string, string >* p_package_map )
{
   string field_values;
   string key_value( instance_key_info( handle, parent_context, true ) );

   for( size_t i = 0; i < field_list.size( ); i++ )
   {
      string next_field( field_list[ i ] );
      size_t pos = next_field.find_last_of( "." );

      string field, field_context;
      if( pos != string::npos )
      {
         field = next_field.substr( pos + 1 );
         field_context = next_field.substr( 0, pos );
      }
      else
         field = next_field;

      if( i > 0 )
         field_values += ',';

      string context( parent_context );
      if( !field_context.empty( ) )
      {
         if( !context.empty( ) )
            context += ".";
         context += field_context;
      }

      string next_value;

      if( p_inserts && p_inserts->count( i ) )
      {
         if( p_inserts->find( i )->second == c_key_field )
         {
            if( !p_package_map || p_package_map->find( key_value ) == p_package_map->end( ) )
               add_next_value( as_csv, key_value, field_values );
            else
               add_next_value( as_csv, p_package_map->find( key_value )->second, field_values );
         }
         else
            add_next_value( as_csv, p_inserts->find( i )->second, field_values );

         field_values += ',';
      }

      if( field == c_key_field )
         next_value = key_value;
      else if( field != c_ignore_field )
         next_value = execute_object_command( handle, context, "get " + field );

      if( p_package_map )
      {
         for( map< string, string >::const_iterator ci = p_package_map->begin( ); ci != p_package_map->end( ); ++ci )
         {
            while( true )
            {
               string::size_type pos = next_value.find( ci->first );
               if( pos == string::npos )
                  break;

               next_value.replace( pos, ci->first.length( ), ci->second );
            }
         }
      }

      if( p_raw_values )
         p_raw_values->push_back( next_value );

      if( field != c_key_field && !next_value.empty( ) && !tz_abbr.empty( ) )
      {
         string type_name = get_field_type_name( handle, context, field );
         if( type_name == "date_time" || type_name == "tdatetime" )
         {
            date_time dt( next_value );

            // NOTE: For mandatory date_time's the "default" date_time (for default or
            // "new" records) is not adjusted so it can be used in order to identify a
            // "default" value (which may in a UI be displayed as blank instead).
            if( !is_default || dt != date_time( ) )
               next_value = utc_to_local( dt, tz_abbr ).as_string( );
         }
      }

      add_next_value( as_csv, next_value, field_values );
   }

   if( p_inserts && p_inserts->count( field_list.size( ) ) )
   {
      field_values += ',';
      if( p_inserts->find( field_list.size( ) )->second == c_key_field )
      {
         if( !p_package_map || p_package_map->find( key_value ) == p_package_map->end( ) )
            add_next_value( as_csv, key_value, field_values );
         else
            add_next_value( as_csv, p_package_map->find( key_value )->second, field_values );
      }
      else
         add_next_value( as_csv, p_inserts->find( field_list.size( ) )->second, field_values );
   }

   return field_values;
}

void set_any_field_ids_to_names( size_t handle, const string& context, vector< string >& fields )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   map< string, string > ids_to_names;

   field_info_container field_info;
   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      ids_to_names.insert( make_pair( field_info[ i ].id, field_info[ i ].name ) );

   for( size_t i = 0; i < fields.size( ); i++ )
   {
      if( ids_to_names.count( fields[ i ] ) )
         fields[ i ] = ids_to_names[ fields[ i ] ];
   }
}

void set_any_field_names_to_ids( size_t handle, const string& context, vector< string >& fields )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   map< string, string > names_to_ids;

   field_info_container field_info;
   instance.get_field_info( field_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      names_to_ids.insert( make_pair( field_info[ i ].name, field_info[ i ].id ) );

   for( size_t i = 0; i < fields.size( ); i++ )
   {
      if( names_to_ids.count( fields[ i ] ) )
         fields[ i ] = names_to_ids[ fields[ i ] ];
   }
}

bool determine_alternative_key_fields( size_t handle, const string& context,
 const vector< string > available_fields, vector< string >& selected_fields )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   set< string > sorted_available_fields( available_fields.begin( ), available_fields.end( ) );

   vector< key_field_info_container > all_key_field_info;
   instance.get_alternative_key_field_info( all_key_field_info );

   vector< key_field_info_container > candidates_key_field_info;

   for( size_t i = 0; i < all_key_field_info.size( ); i++ )
   {
      bool is_candidate = true;
      for( size_t j = 0; j < all_key_field_info[ i ].size( ); j++ )
      {
         if( !sorted_available_fields.count( all_key_field_info[ i ][ j ].id )
          && !sorted_available_fields.count( all_key_field_info[ i ][ j ].name ) )
         {
            is_candidate = false;
            break;
         }
      }

      if( is_candidate )
         candidates_key_field_info.push_back( all_key_field_info[ i ] );
   }

   if( !candidates_key_field_info.empty( ) )
   {
      size_t best = 0;
      for( size_t i = 0; i < candidates_key_field_info.size( ); i++ )
      {
         if( candidates_key_field_info[ i ].size( ) < candidates_key_field_info[ best ].size( ) )
            best = i;
      }

      for( size_t j = 0; j < candidates_key_field_info[ best ].size( ); j++ )
      {
         if( sorted_available_fields.count( candidates_key_field_info[ best ][ j ].id ) )
            selected_fields.push_back( *sorted_available_fields.find( candidates_key_field_info[ best ][ j ].id ) );
         else if( sorted_available_fields.count( candidates_key_field_info[ best ][ j ].name ) )
            selected_fields.push_back( *sorted_available_fields.find( candidates_key_field_info[ best ][ j ].name ) );
      }

      return true;
   }

   return false;
}

void get_foreign_field_and_class_ids( size_t handle,
 const string& context, map< string, string >& foreign_field_and_class_ids )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   vector< field_data > all_field_data;
   get_all_field_data( handle, "", "", all_field_data );

   for( size_t i = 0; i < all_field_data.size( ); i++ )
   {
      if( !all_field_data[ i ].class_id.empty( ) )
         foreign_field_and_class_ids.insert( make_pair( all_field_data[ i ].id, all_field_data[ i ].class_id ) );
   }
}

void get_base_class_info( size_t handle,
 const string& context, vector< pair< string, string > >& base_class_info )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   class_base& dcb( *instance.get_dynamic_instance( ) );
   dcb.get_base_class_info( base_class_info );
}

size_t obtain_instance_fk_lock( const string& lock_class_id, const string& key, bool review_required )
{
   if( lock_class_id.empty( ) || key.empty( ) )
      throw runtime_error( "lock_class_id and key must both have non-empty values" );

   size_t lock_handle = obtain_keyed_lock( lock_class_id,
    key, !review_required ? op_lock::e_lock_type_link : op_lock::e_lock_type_review );

   return lock_handle;
}

void release_obtained_lock( size_t lock_handle )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   storage_handler& handler( *gtp_session->p_storage_handler );
   handler.release_lock( lock_handle );
}

void dump_storage_locks( ostream& os )
{
   guard g( g_mutex );

   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   os << '\n';
   gtp_session->p_storage_handler->dump_locks( os );
}

string exec_bulk_ops( const string& module,
 const string& uid, const string& dtm, const string& mclass,
 const string& filename, const string& export_fields, const string& tz_abbr, bool destroy_records,
 const string& search_text, const string& search_query, const string& fixed_field_values, command_handler& handler )
{
   string response;

   string module_id( module );
   if( !gtp_session->modules_by_id.count( module ) )
   {
      if( !gtp_session->modules_by_name.count( module ) )
         throw runtime_error( "unable to resolve module id/name '" + module + "'" );

      module_id = gtp_session->modules_by_name.find( module )->second;
   }

   string class_id = get_class_id_for_id_or_name( module_id, mclass );

   ifstream inpf;
   ofstream outf;

   size_t line = 1;
   time_t ts( time( 0 ) );
   bool in_trans = false;
   bool is_export = false;

   if( !export_fields.empty( ) )
   {
      is_export = true;

      outf.open( filename.c_str( ), ios::out );
      if( !outf )
         throw runtime_error( "unable to open '" + filename + "' for output" );
   }
   else
   {
      inpf.open( filename.c_str( ), ios::in );

      if( !inpf )
         throw runtime_error( "unable to open '" + filename + "' for input" );

      string log_file_name( filename.substr( 0, filename.find_last_of( "." ) ) + ".log" );
      outf.open( log_file_name.c_str( ), ios::out );
      if( !outf )
         throw runtime_error( "unable to open '" + log_file_name + "' for output" );
   }

   size_t handle = create_object_instance( module_id, class_id, 0, false );
   try
   {
      vector< string > fields;
      vector< string > original_fields;

      if( is_export )
      {
         if( export_fields == "*" )
         {
            fields.push_back( c_key_field );
            original_fields.push_back( c_key_field );

            get_all_field_names( handle, "", fields );
            get_all_field_names( handle, "", original_fields );
         }
         else
         {
            split( export_fields, fields );
            split( export_fields, original_fields );
         }

         set_any_field_ids_to_names( handle, "", fields );

         string fields_for_iteration;

         string key_info;
         for( size_t i = 0; i < fields.size( ); i++ )
         {
            if( original_fields[ i ] != c_key_field )
            {
               // NOTE: Use the first field to determine the order (if the first
               // field was the key then no need to add any ordering information).
               if( i == 0 )
                  key_info = original_fields[ i ] + " ";

               if( !fields_for_iteration.empty( ) )
                  fields_for_iteration += ",";
               fields_for_iteration += original_fields[ i ];
            }

            if( i > 0 )
               outf << ",";
            outf << fields[ i ];
         }

         outf << "\n";

         if( instance_iterate( handle, "", key_info,
          fields_for_iteration, search_text, search_query, "", e_iter_direction_forwards, true ) )
         {
            do
            {
               outf << get_field_values( handle, "", fields, tz_abbr, false, true ) << "\n";
            } while( instance_iterate_next( handle, "" ) );
         }
      }
      else
      {
         set_uid( uid );
         set_dtm( dtm );
         set_tz_abbr( tz_abbr );

         string log_lines;

         vector< string > key_fields;
         set< string > sorted_key_fields;

         vector< string > fixed_fields;
         vector< string > fixed_values;

         if( !fixed_field_values.empty( ) )
         {
            vector< string > fixed_field_value_pairs;
            split( fixed_field_values, fixed_field_value_pairs );

            string::size_type pos;
            for( size_t i = 0; i < fixed_field_value_pairs.size( ); i++ )
            {
               pos = fixed_field_value_pairs[ i ].find( '=' );
               if( pos == string::npos )
                  throw runtime_error( "unexpected field=value pair format '" + fixed_field_value_pairs[ i ] + "'" );

               fixed_fields.push_back( fixed_field_value_pairs[ i ].substr( 0, pos ) );
               fixed_values.push_back( fixed_field_value_pairs[ i ].substr( pos + 1 ) );
            }
         }

         string next;
         size_t errors = 0;
         bool is_first = true;
         bool can_fetch = false;
         bool has_key_field = false;

         size_t num_created = 0;
         size_t num_updated = 0;
         size_t num_destroyed = 0;
         size_t key_field_num = 0;
         size_t transaction_id = 0;

         while( getline( inpf, next ) )
         {
            remove_trailing_cr_from_text_file_line( next, is_first );

            if( is_first )
            {
               is_first = false;

               // NOTE: UTF-8 text files will often begin with an identifying sequence "EF BB BF" as the
               // first three characters of the file so if the first byte is "EF" assume UTF-8 and strip.
               if( next.size( ) >= 3 && next[ 0 ] == ( char )0xef )
                  next.erase( 0, 3 );

               if( split( next, fields ) < 1 )
                  throw runtime_error( "cannot import without at least one field name" );

               set_any_field_names_to_ids( handle, "", fields );
               set_any_field_names_to_ids( handle, "", fixed_fields );

               // NOTE: Incorrerct field names are detected by attempting to convert
               // all the names that were changed to ids back to names and comparing.
               vector< string > test_fields( fields );
               set_any_field_ids_to_names( handle, "", test_fields );

               for( size_t i = 0; i < fields.size( ); i++ )
               {
                  if( fields[ i ] == c_key_field )
                  {
                     key_field_num = i;
                     has_key_field = true;
                  }
                  else if( fields[ i ] == c_ignore_field )
                     continue;
                  else if( fields[ i ] == test_fields[ i ] )
                     // FUTURE: This needs to be implemented as a string message.
                     throw runtime_error( "Unknown field '"
                      + fields[ i ] + "' for '" + instance_class( handle, "" ) + "' records." );
               }

               vector< string > test_fixed_fields( fixed_fields );
               set_any_field_ids_to_names( handle, "", test_fixed_fields );

               for( size_t i = 0; i < fixed_fields.size( ); i++ )
               {
                  // FUTURE: This needs to be implemented as a string message.
                  if( fixed_fields[ i ] == test_fixed_fields[ i ] )
                     throw runtime_error( "Unknown field '"
                      + fixed_fields[ i ] + "' for " + instance_class( handle, "" ) + " records." );
               }

               if( has_key_field )
                  can_fetch = true;
               else if( determine_alternative_key_fields( handle, "", fields, key_fields ) )
               {
                  can_fetch = true;
                  for( size_t i = 0; i < key_fields.size( ); i++ )
                     sorted_key_fields.insert( key_fields[ i ] );
               }

               continue;
            }

            // FUTURE: This needs to be implemented as a string message.
            if( !can_fetch && destroy_records )
               throw runtime_error( "Cannot destroy records without providing their key fields." );

            ++line;

            if( next.empty( ) )
               continue;

            bool destroy_record( destroy_records );

            size_t num_values = 0;
            vector< string > values;

            size_t continuation_offset = 0;
            bool last_value_incomplete = false;
            while( true )
            {
               num_values = split_csv_values( next, values, last_value_incomplete, continuation_offset );

               if( !last_value_incomplete )
                  break;

               next = values.back( );
               values.pop_back( );

               string continuation;
               if( !getline( inpf, continuation ) )
                  throw runtime_error( "unexpected incomplete record found at line #" + to_string( line ) );

               // NOTE: In case the string file had been treated as binary during an FTP transfer remove trailing CR.
               if( continuation.size( ) && continuation[ continuation.size( ) - 1 ] == '\r' )
                  continuation.erase( continuation.size( ) - 1 );

               continuation_offset = next.size( ) + 2;

               ++line;
               next += "\r\n" + continuation;
            }

            if( num_values != fields.size( ) )
            {
               // FUTURE: This needs to be implemented as a string message.
               string message( "Wrong number of field values (expected "
                + to_string( fields.size( ) ) + " but found " + to_string( num_values )
                + ") in import file line #" + to_string( line ) + "." );

               // NOTE: If the second line is incorrect then is assuming all will fail.
               if( line == 2 )
                  throw runtime_error( message );
               else
               {
                  outf << "Error: " << message << endl;

                  ++errors;
                  continue;
               }
            }

            string key;

            if( !in_trans )
            {
               in_trans = true;
               transaction_start( );
               transaction_id = next_transaction_id( );
            }

            string next_log_line;
            bool found_instance = false;
            if( can_fetch )
            {
               string key_info;

               if( has_key_field )
                  key_info = values[ key_field_num ];
               else
               {
                  size_t num_fixed = 0;
                  string key_info_fields;
                  string key_info_values;
                  for( size_t i = 0; i < num_values; i++ )
                  {
                     if( sorted_key_fields.count( fields[ i ] ) )
                     {
                        ++num_fixed;

                        if( !key_info_fields.empty( ) )
                           key_info_fields += ",";
                        key_info_fields += fields[ i ];

                        if( !key_info_values.empty( ) )
                           key_info_values += ",";
                        key_info_values += escape( values[ i ], "," );
                     }
                  }

                  key_info = key_info_fields + "#" + to_string( num_fixed ) + " " + key_info_values;
               }

               instance_fetch_rc rc;
               instance_fetch( handle, "", key_info, &rc );

               if( rc == e_instance_fetch_rc_okay )
               {
                  found_instance = true;

                  string key_info( instance_key_info( handle, "" ) );
                  key = key_info.substr( 0, key_info.find( ' ' ) );

                  if( !destroy_record )
                  {
                     op_update_rc rc;
                     op_instance_update( handle, "", key, "", false, &rc );

                     if( rc == e_op_update_rc_okay )
                        next_log_line = "perform_update";
                     else
                     {
                        // FUTURE: These need to be implemented as string messages.
                        if( rc == e_op_update_rc_locked )
                           outf << "Error: Processing line #" << line << " - unable to lock for update." << endl;
                        else if( rc == e_op_update_rc_not_found )
                           outf << "Error: Processing line #" << line << " - record not found for update." << endl;
                        else
                           throw runtime_error( "unexpected op_update rc #" + to_string( rc ) );

                        ++errors;
                        continue;
                     }
                  }
                  else
                  {
                     op_destroy_rc rc;
                     op_instance_destroy( handle, "", key, "", false, &rc );

                     if( rc == e_op_destroy_rc_okay )
                        next_log_line = "perform_destroy";
                     else
                     {
                        // FUTURE: These need to be implemented as string messages.
                        if( rc == e_op_destroy_rc_locked )
                           outf << "Error: Processing line #" << line << " - unable to lock for destroy." << endl;
                        else if( rc == e_op_destroy_rc_not_found )
                           outf << "Error: Processing line #" << line << " - record not found for destroy." << endl;
                        else if( rc == e_op_destroy_rc_constrained )
                           outf << "Error: Processing line #" << line << " - record cannot be destroyed due to being used by other records." << endl;
                        else
                           throw runtime_error( "unexpected op_destroy rc #" + to_string( rc ) );

                        ++errors;
                        continue;
                     }
                  }
               }
            }

            if( !destroy_record && !found_instance )
            {
               if( !has_key_field )
                  key = gen_key( );
               else
                  key = values[ key_field_num ];

               op_create_rc rc;
               op_instance_create( handle, "", key, false, &rc );

               if( rc == e_op_create_rc_okay )
                  next_log_line = "perform_create";
               else
               {
                  // FUTURE: These need to be implemented as string messages.
                  if( rc == e_op_create_rc_locked )
                     outf << "Error: Processing line #" << line << " - unable to lock for create." << endl;
                  else if( rc == e_op_create_rc_exists )
                     outf << "Error: Processing line #" << line << " - cannot create duplicate record." << endl;
                  else
                     throw runtime_error( "unexpected op_create rc #" + to_string( rc ) );

                  ++errors;
                  continue;
               }
            }

            next_log_line += " " + uid + " " + dtm + " " + module_id + " " + class_id + " " + key;

            if( !destroy_record )
            {
               next_log_line += " ";

               string log_field_value_pairs;
               for( size_t i = 0; i < num_values; i++ )
               {
                  if( ( has_key_field && i == key_field_num ) || fields[ i ] == c_ignore_field )
                     continue;

                  if( !values[ i ].empty( ) && !tz_abbr.empty( ) )
                  {
                     string type_name = get_field_type_name( handle, "", fields[ i ] );
                     if( !tz_abbr.empty( ) && ( type_name == "date_time" || type_name == "tdatetime" ) )
                        values[ i ] = local_to_utc( date_time( values[ i ] ), tz_abbr ).as_string( );
                  }

                  string method_name_and_args( "set " );
                  method_name_and_args += fields[ i ] + " ";
                  method_name_and_args += "\"" + escaped( escaped( values[ i ] ), "\"", c_nul ) + "\"";

                  if( !log_field_value_pairs.empty( ) )
                     log_field_value_pairs += ",";
                  log_field_value_pairs += fields[ i ] + "="
                   + search_replace( escaped( escaped( values[ i ] ), "\"", c_esc, "rn\r\n" ), ",", "\\\\," );

                  execute_object_command( handle, "", method_name_and_args );
               }

               for( size_t i = 0; i < fixed_fields.size( ); i++ )
               {
                  string method_name_and_args( "set " );
                  method_name_and_args += fixed_fields[ i ] + " ";
                  method_name_and_args += "\"" + escaped( escaped( fixed_values[ i ] ), "\"", c_nul ) + "\"";

                  if( !log_field_value_pairs.empty( ) )
                     log_field_value_pairs += ",";
                  log_field_value_pairs += fixed_fields[ i ] + "="
                   + search_replace( escaped( escaped( fixed_values[ i ] ), "\"", c_esc, "rn\r\n" ), ",", "\\\\," );

                  execute_object_command( handle, "", method_name_and_args );
               }

               next_log_line += "\"" + log_field_value_pairs + "\"";
            }

            op_apply_rc rc;
            op_instance_apply( handle, "", false, &rc );

            if( rc != e_op_apply_rc_okay )
            {
               op_instance_cancel( handle, "", false );

               // FUTURE: These need to be implemented as string messages.
               if( rc == e_op_apply_rc_locked )
                  outf << "Error: Processing line #" << line << " - record was locked." << endl;
               else if( rc == e_op_apply_rc_invalid )
                  outf << "Error: Processing line #" << line << " - record was invalid." << endl;
               else
                  throw runtime_error( "unexpected op_apply rc #" + to_string( rc ) );

               ++errors;
               continue;
            }

            if( !log_lines.empty( ) )
               log_lines += "\n";
            log_lines += next_log_line;

            if( time( 0 ) - ts >= 10 )
            {
               ts = time( 0 );

               transaction_log_command( log_lines );
               transaction_commit( );

               in_trans = false;
               log_lines.clear( );

               // FUTURE: This message should be handled as a server string message.
               handler.output_progress( "Processed " + to_string( line ) + " lines..." );

               if( is_condemned_session( ) )
                  break;
            }

            if( found_instance )
            {
               if( !destroy_record )
                  ++num_updated;
               else
                  ++num_destroyed;
            }
            else
               ++num_created;
         }

         if( in_trans )
         {
            transaction_log_command( log_lines );
            transaction_commit( );
         }

         // FUTURE: These should be handled as string messages.
         if( num_created )
            outf << "Created " << num_created << " new record(s)." << endl;
         if( num_updated )
            outf << "Updated " << num_updated << " existing record(s)." << endl;
         if( num_destroyed )
            outf << "Removed " << num_destroyed << " existing record(s)." << endl;

         response = "Processed " + to_string( line ) + " lines with " + to_string( errors ) + " error(s).";
      }

      destroy_object_instance( handle );
   }
   catch( exception& x )
   {
      if( !is_export )
         // FUTURE: This message should be handled as a server string message.
         outf << "Error: Processing line #" << line << " - " << x.what( ) << endl;

      if( in_trans )
         transaction_rollback( );
      destroy_object_instance( handle );
      throw;
   }
   catch( ... )
   {
      if( !is_export )
         // FUTURE: This message should be handled as a server string message.
         outf << "Error: Processing line #" << line << " - unknown exception caught" << endl;

      if( in_trans )
         transaction_rollback( );
      destroy_object_instance( handle );
      throw;
   }

   return response;
}

void export_package( const string& module,
 const string& mclass, const string& keys, const string& exclude_info,
 const string& test_info, const string& include_info, const string& filename )
{
   string last_class_id;
   map< string, int > all_class_ids;
   deque< pair< string, string > > next_pass;
   map< string, set< string > > partial_export;
   map< string, set< string > > will_be_exported;
   map< string, set< string > > exported_records;
   map< string, set< string > > exported_children;

   string module_id( module );
   if( !gtp_session->modules_by_id.count( module ) )
   {
      if( !gtp_session->modules_by_name.count( module ) )
         throw runtime_error( "unable to resolve module id/name '" + module + "'" );

      module_id = gtp_session->modules_by_name.find( module )->second;
   }

   string class_id = get_class_id_for_id_or_name( module_id, mclass );

   ofstream outf( filename.c_str( ) );
   if( !outf )
      throw runtime_error( "unable to open file '" + filename + "' for output" );

   map< string, map< string, string > > tests;
   if( !test_info.empty( ) )
   {
      vector< string > test_items;

      // NOTE: The test info can alternatively be placed in an external list file for convenience.
      // Each test item can be one of the following formats: <class>:*[;label] or <class>:<field>#
      // or <class>:<field>[!]=[value][;label]. Both "class" and "field" (where applicable) can be
      // specified as either names or id's. The first format will simply prevent any record of the
      // matching class to be included in the package. The second format will handle the field (if
      // it is a foreign key) as though it is mandatory (forcing the parent record to be processed
      // before itself). The third format handles a conditional test (only if true will the record
      // be output). If the optional label is used then instead of omitting the record itself that
      // label (including the semi-colon) will be appended to the exclude child class name/id when
      // processing the child exclusions. The special case <field> being @key is permitted so that
      // specific class records can be omitted and for multiple test values use comma separators.
      if( test_info[ 0 ] != '@' )
         split( test_info, test_items );
      else
         buffer_file_lines( test_info.substr( 1 ), test_items );

      for( size_t i = 0; i < test_items.size( ); i++ )
      {
         string::size_type pos = test_items[ i ].find( ':' );
         if( pos == string::npos )
            throw runtime_error( "invalid test_info item format '" + test_items[ i ] + "'" );

         string pclass = test_items[ i ].substr( 0, pos );
         string cfield = test_items[ i ].substr( pos + 1 );

         string xinfo( "for test_info '" + test_items[ i ] + "'" );
         pclass = resolve_class_id( module_id, pclass, xinfo );

         string test_expr;
         if( cfield != "*" )
         {
            string::size_type cpos = cfield.find( "!=" );
            if( cpos == string::npos )
               cpos = cfield.find( "=" );
            if( cpos == string::npos )
               cpos = cfield.find( "#" );

            if( cpos == string::npos )
               throw runtime_error( "invalid test_info item format '" + test_items[ i ] + "'" );

            test_expr = cfield.substr( cpos );
            cfield.erase( cpos );

            if( cfield != c_key_field )
            {
               string xinfo( "for test_info '" + test_items[ i ] + "'" );
               cfield = resolve_field_id( module_id, pclass, cfield, xinfo );
            }
         }

         tests[ pclass ][ cfield ] = test_expr;
      }
   }

   map< string, set< string > > excludes;
   if( !exclude_info.empty( ) )
   {
      vector< string > exclude_items;

      // NOTE: The exclude info can alternatively be placed in an external list file for convenience.
      // Each exclude is in the format <pclass>:<cclass>[#<field_id>][;label] where both "pclass" and
      // "cclass" able to be specified as either names or id's. The optional "field_id" must be an id
      // and will target a specific child context rather than any of the matching child class. If the
      // "label" is added then only if a test item (see above) identified the record will the exclude
      // actually apply.
      if( exclude_info[ 0 ] != '@' )
         split( exclude_info, exclude_items );
      else
         buffer_file_lines( exclude_info.substr( 1 ), exclude_items );

      for( size_t i = 0; i < exclude_items.size( ); i++ )
      {
         string::size_type pos = exclude_items[ i ].find( ':' );
         if( pos == string::npos )
            throw runtime_error( "invalid exclude_info item format '" + exclude_items[ i ] + "'" );

         string pclass = exclude_items[ i ].substr( 0, pos );
         string cclass = exclude_items[ i ].substr( pos + 1 );

         string xinfo( "for exclude_info '" + exclude_items[ i ] + "'" );
         pclass = resolve_class_id( module_id, pclass, xinfo );

         string field_info;
         pos = cclass.find_first_of( "#;" );
         if( pos != string::npos )
         {
            field_info = cclass.substr( pos );
            cclass.erase( pos );
         }

         if( cclass != "*" )
         {
            cclass = resolve_class_id( module_id, cclass, xinfo );

            if( !field_info.empty( ) && field_info[ 0 ] == '#' )
            {
               pos = field_info.find( ';' );
               resolve_field_id( module_id, cclass,
                field_info.substr( 1, pos == string::npos ? pos : pos - 1 ), xinfo );
            }
         }

         excludes[ pclass ].insert( cclass + field_info );
      }
   }

   map< string, int > rounds;
   map< string, set< string > > includes;
   if( !include_info.empty( ) )
   {
      vector< string > include_items;

      // NOTE: The include info can alternatively be placed in an external list file for convenience.
      // Each include is in the format <class>:<field> or <class>#<round_num> where both "class" and
      // "field" can be specified as either names or id's. The second format allows processing of the
      // nominated class to be delayed to a later round (useful if the pre-determined ordering is not
      // what is wanted).
      if( include_info[ 0 ] != '@' )
         split( include_info, include_items );
      else
         buffer_file_lines( include_info.substr( 1 ), include_items );

      for( size_t i = 0; i < include_items.size( ); i++ )
      {
         string::size_type pos = include_items[ i ].find( '#' );
         if( pos != string::npos )
         {
            string pclass = include_items[ i ].substr( 0, pos );

            string xinfo( "for include_info '" + include_items[ i ] + "'" );
            pclass = resolve_class_id( module_id, pclass, xinfo );

            rounds[ pclass ] = atoi( include_items[ i ].substr( pos + 1 ).c_str( ) );
            continue;
         }

         pos = include_items[ i ].find( ':' );
         if( pos == string::npos )
            throw runtime_error( "invalid include_info item format '" + include_items[ i ] + "'" );

         string pclass = include_items[ i ].substr( 0, pos );
         string cfield = include_items[ i ].substr( pos + 1 );

         string xinfo( "for include_info '" + include_items[ i ] + "'" );
         pclass = resolve_class_id( module_id, pclass, xinfo );

         cfield = resolve_field_id( module_id, pclass, cfield, xinfo );

         includes[ pclass ].insert( cfield );
      }
   }

   int current_round = 0;
   map< int, deque< pair< string, string > > > future_rounds;

   string module_name( module );
   if( !gtp_session->modules_by_name.count( module ) )
   {
      if( !gtp_session->modules_by_id.count( module ) )
         throw runtime_error( "unable to resolve module id/name '" + module + "'" );

      module_name = gtp_session->modules_by_id.find( module )->second;
   }

   vector< string > classes;
   buffer_file_lines( module_name + ".acyclic.lst", classes );

   for( size_t i = 0; i < classes.size( ); i++ )
      all_class_ids.insert( make_pair( get_class_id_for_id_or_name( module_id, classes[ i ] ), i ) );

   vector< string > all_keys;
   split( keys, all_keys );

   if( all_keys.empty( ) )
      throw runtime_error( "cannot perform package export without at least one key" );

   string next_key( all_keys[ 0 ] );
   string next_class_id( class_id );

   for( size_t i = 1; i < all_keys.size( ); i++ )
      next_pass.push_back( make_pair( next_class_id, all_keys[ i ] ) );

   outf << "<sio/>\n";

   bool is_first = true;

   // NOTE: An export "package" contains foreign key and child related records reached
   // from the nominated starting record. In some cases there may be dependencies that
   // require a record to be output twice (the first time with one or more optional fk
   // links set to blank).
   size_t total = 0;
   time_t ts( time( 0 ) );
   while( true )
   {
      export_data( outf, module_name, next_class_id,
       next_key, last_class_id, is_first, get_session_command_handler( ),
       all_class_ids, excludes, tests, includes, exported_records, exported_children,
       next_pass, will_be_exported, partial_export, rounds, current_round, future_rounds, ts, total );

      if( current_round == 0 )
         is_first = false;

      if( next_pass.empty( ) && !future_rounds.empty( ) )
      {
         current_round = future_rounds.begin( )->first;
         next_pass = future_rounds.begin( )->second;

         is_first = true;
         future_rounds.erase( future_rounds.begin( ) );
      }

      if( next_pass.empty( ) )
         break;

      next_class_id = next_pass.front( ).first;
      next_key = next_pass.front( ).second;

      next_pass.pop_front( );
   }

   if( !exported_records.empty( ) )
      outf << " </class>\n";
   outf << "</sio>\n";

   outf.flush( );
   if( !outf.good( ) )
      throw runtime_error( "unexpected bad stream for '" + filename + "'" );
}

void import_package( const string& module,
 const string& uid, const string& dtm, const string& filename, const string& key_prefix,
 const string& replace_info, const string& skip_field_info, bool new_only, bool for_remove )
{
   string module_id( module );
   if( !gtp_session->modules_by_id.count( module ) )
   {
      if( !gtp_session->modules_by_name.count( module ) )
         throw runtime_error( "unable to resolve module id/name '" + module + "'" );

      module_id = gtp_session->modules_by_name.find( module )->second;
   }

   map< string, map< string, string > > skip_fields;
   if( !skip_field_info.empty( ) )
   {
      vector< string > skip_field_items;

      // NOTE: The skip info can alternatively be placed in an external list file for convenience.
      // Each skip entry is in the format <class>:<field>[=<value>] where both "class" and "field"
      // can be specified as either names or id's. If specified as <class>:<field> then all values
      // for the name field are ignored (i.e. as though the field was not in each record). For the
      // use <class>:<field>=[<value>] then the record itself will be skipped if the field's value
      // matches that specified.
      if( skip_field_info[ 0 ] != '@' )
         split( skip_field_info, skip_field_items );
      else
         buffer_file_lines( skip_field_info.substr( 1 ), skip_field_items );

      for( size_t i = 0; i < skip_field_items.size( ); i++ )
      {
         string::size_type pos = skip_field_items[ i ].find( ':' );
         if( pos == string::npos )
            throw runtime_error( "invalid skip_field_info item format '" + skip_field_items[ i ] + "'" );

         string sclass( skip_field_items[ i ].substr( 0, pos ) );

         string xinfo( "for skip_field_info '" + skip_field_items[ i ] + "'" );
         sclass = resolve_class_id( module_id, sclass, xinfo );

         string sfield( skip_field_items[ i ].substr( pos + 1 ) );
         string svalue;

         pos = sfield.find( '=' );
         if( pos != string::npos )
         {
            svalue = sfield.substr( pos );
            sfield.erase( pos );
         }

         sfield = resolve_field_id( module_id, sclass, sfield, xinfo );

         skip_fields[ sclass ][ sfield ] = svalue;
      }
   }

   ifstream inpf( filename.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + filename + "' for input" );

   set_uid( uid );
   set_dtm( dtm );

   sio_reader reader( inpf );

   string log_lines;

   bool has_key_list_file = false;
   map< string, string > search_replaces_map;
   vector< pair< string, string > > search_replaces;

   if( !replace_info.empty( ) )
   {
      vector< string > replace_items;

      if( replace_info[ 0 ] != '@' )
         split( replace_info, replace_items );
      else
      {
         has_key_list_file = true;
         buffer_file_lines( replace_info.substr( 1 ), replace_items );
      }

      for( size_t i = 0; i < replace_items.size( ); i++ )
      {
         if( replace_items[ i ].empty( ) || replace_items[ i ][ 0 ] == ';' )
            continue;

         // NOTE: If the replace info starts with an asterisk then search/replacing
         // will be performed seach string itself (this can be useful when compound
         // keys that contain other package keys need to be optional).
         bool do_replaces_for_find_string = false;
         if( replace_items[ i ][ 0 ] == '*' )
         {
            do_replaces_for_find_string = true;
            replace_items[ i ].erase( 0, 1 );
         }

         string::size_type pos = replace_items[ i ].find( '=' );
         if( pos == string::npos )
            throw runtime_error( "invalid replace_info item format '" + replace_items[ i ] + "'" );

         string find_string( replace_items[ i ].substr( 0, pos ) );
         string replace_with( replace_items[ i ].substr( pos + 1 ) );

         if( replace_with[ 0 ] == '?' || replace_with[ 0 ] == '!' )
         {
            // NOTE: Conditional search replacement is available in two ways:
            //
            // <search>=!<check>=[replace] will change <search> to <replace> if <check>'s replace is empty
            // <search>=?<check>=[replace] will change <search> to <replace> if <check>'s replace is non-empty
            //
            // The <check> value is expected to be the key for an already existing search/replacement pair.
            pos = replace_with.find( "=" );
            if( pos == string::npos )
               throw runtime_error( "invalid replace_info item format '" + replace_items[ i ] + "'" );

            bool check_exists = ( replace_with[ 0 ] == '?' );

            string check_for( replace_with.substr( 1, pos - 1 ) );
            replace_with.erase( 0, pos + 1 );

            if( !search_replaces_map.count( check_for ) )
               throw runtime_error( "conditional search/replace check '" + check_for + "' was not found" );

            if( check_exists && search_replaces_map[ check_for ].empty( ) )
               continue;

            if( !check_exists && !search_replaces_map[ check_for ].empty( ) )
               continue;
         }

         if( replace_with == c_key_field )
            replace_with = gen_key( );
         else
         {
            while( search_replaces_map.count( replace_with ) )
               replace_with = search_replaces_map[ replace_with ];
         }

         if( do_replaces_for_find_string )
         {
            for( size_t i = 0; i < search_replaces.size( ); i++ )
            {
               find_string = search_replace( find_string,
                search_replaces[ i ].first, search_replaces[ i ].second );
            }
         }

         bool exists = ( search_replaces_map.count( find_string ) > 0 );

         search_replaces_map[ find_string ] = replace_with;

         if( !exists )
            search_replaces.push_back( make_pair( find_string, replace_with ) );
         else
         {
            for( size_t i = 0; i < search_replaces.size( ); i++ )
            {
               if( search_replaces[ i ].first == find_string )
               {
                  search_replaces[ i ].second = replace_with;
                  break;
               }
            }
         }
      }

      if( !for_remove && has_key_list_file )
      {
         string map_file_name( replace_info.substr( 1 ) + ".map" );
         ofstream outf( map_file_name.c_str( ) );

         for( size_t i = 0; i < search_replaces.size( ); i++ )
            outf << search_replaces[ i ].first << "=" << search_replaces[ i ].second << endl;
      }
   }

   time_t ts( time( 0 ) );

   transaction_start( );
   size_t transaction_id = next_transaction_id( ) + 1;

   set< string > keys_updating;
   map< string, string > keys_created;
   map< string, set< string > > prefixed_class_keys;

   try
   {
      size_t line_num = 1;
      while( reader.has_started_section( c_section_class ) )
      {
         ++line_num;

         string mclass( reader.read_attribute( c_attribute_name ) );
         string field_list( reader.read_attribute( c_attribute_fields ) );

         line_num += 2;

         mclass = get_class_id_for_id_or_name( module_id, mclass );

         size_t handle = create_object_instance( module_id, mclass, 0, false );
         try
         {
            vector< string > fields;
            split( field_list, fields );

            set_any_field_names_to_ids( handle, "", fields );

            map< string, string > foreign_field_and_class_ids;
            if( !key_prefix.empty( ) )
               get_foreign_field_and_class_ids( handle, "", foreign_field_and_class_ids );

            vector< pair< string, string > > base_class_info;
            get_base_class_info( handle, "", base_class_info );

            if( base_class_info.empty( ) )
               base_class_info.push_back( make_pair( mclass, mclass ) );

            string next_record;
            while( reader.has_read_attribute( c_attribute_record, next_record ) )
            {
               ++line_num;

               if( time( 0 ) - ts >= 10 )
               {
                  ts = time( 0 );
                  // FUTURE: This message should be handled as a server string message.
                  get_session_command_handler( ).output_progress( "Processed " + to_string( line_num ) + " lines..." );
               }

               if( !search_replaces.empty( ) )
               {
                  for( size_t i = 0; i < search_replaces.size( ); i++ )
                  {
                     next_record = search_replace( next_record,
                      search_replaces[ i ].first, search_replaces[ i ].second );
                  }
               }

               vector< string > field_values;
               split( next_record, field_values, ',', '\\', false );

               if( field_values.size( ) != fields.size( ) )
                  throw runtime_error( "found " + to_string( field_values.size( ) )
                   + " field values but was expecting " + to_string( fields.size( ) ) );

               if( fields.size( ) )
               {
                  if( fields[ 0 ] != c_key_field )
                     throw runtime_error( "unexpected missing key field processing line #" + to_string( line_num ) );

                  bool skip_op = false;

                  if( field_values[ 0 ].empty( ) || field_values[ 0 ] == "!" )
                     skip_op = true;
                  else
                  {
                     for( size_t i = 1; i < fields.size( ); i++ )
                     {
                        for( size_t j = 0; j < base_class_info.size( ); j++ )
                        {
                           string next_cid = base_class_info[ j ].first;

                           if( skip_fields.count( next_cid ) && skip_fields[ next_cid ].count( fields[ i ] ) )
                           {
                              if( !skip_fields[ next_cid ][ fields[ i ] ].empty( )
                               && field_values[ i ] == skip_fields[ next_cid ][ fields[ i ] ].substr( 1 ) )
                                 skip_op = true;
                           }
                        }
                     }
                  }

                  string next_key( field_values[ 0 ] );

                  // NOTE: Allow packages being imported with the "new_only" option to
                  // still update specific records by prefixing their keys with a '!'.
                  if( !next_key.empty( ) && next_key[ 0 ] == '!' )
                  {
                     next_key.erase( 0, 1 );
                     keys_updating.insert( next_key );
                  }

                  bool is_remove_op = false;
                  if( !next_key.empty( ) && next_key[ 0 ] == '~' )
                  {
                     next_key.erase( 0, 1 );

                     if( !for_remove )
                        skip_op = true;
                     else
                        is_remove_op = true;
                  }

                  if( is_remove_op && !for_remove )
                     skip_op = true;
                  else if( for_remove && !is_remove_op )
                     skip_op = true;

                  if( !skip_op )
                  {
                     string next_log_line;

                     string key_value( key_prefix + next_key );

                     instance_fetch_rc rc;

                     // NOTE: As a performance optimisation the instance fetch is skipped if the
                     // "new_only" option is being used (assumes the record was not found unless
                     // previously created or specifically flagged as an update).
                     if( new_only )
                     {
                        if( keys_created.count( key_value ) || keys_updating.count( key_value ) )
                           rc = e_instance_fetch_rc_okay;
                        else
                           rc = e_instance_fetch_rc_not_found;
                     }
                     else
                        instance_fetch( handle, "", key_value, &rc );

                     bool is_update = false;
                     if( rc != e_instance_fetch_rc_okay )
                     {
                        next_log_line = "perform_create";
                        op_instance_create( handle, "", key_value, false );
                     }
                     else if( new_only && !keys_created.count( key_value ) && !keys_updating.count( key_value ) )
                        // FUTURE: This message should be handled as a server string message.
                        throw runtime_error( "Package key '" + key_value + "' is already in use." );
                     else
                     {
                        is_update = true;
                        next_log_line = "perform_update";
                        op_instance_update( handle, "", key_value, "", false );
                     }

                     next_log_line += " " + uid + " " + dtm + " "
                      + module_id + " " + mclass + " " + key_value + " \"";

                     string log_field_value_pairs;

                     for( size_t i = 1; i < fields.size( ); i++ )
                     {
                        bool skip_field = false;
                        for( size_t j = 0; j < base_class_info.size( ); j++ )
                        {
                           string next_cid = base_class_info[ j ].first;

                           if( skip_fields.count( next_cid ) && skip_fields[ next_cid ].count( fields[ i ] ) )
                           {
                              if( skip_fields[ next_cid ][ fields[ i ] ].empty( ) )
                              {
                                 skip_field = true;
                                 break;
                              }
                           }
                        }

                        if( skip_field || fields[ i ] == c_ignore_field )
                           continue;

                        if( foreign_field_and_class_ids.count( fields[ i ] )
                         && prefixed_class_keys[ foreign_field_and_class_ids[ fields[ i ] ] ].count( field_values[ i ] ) )
                           field_values[ i ] = key_prefix + field_values[ i ];

                        string method_name_and_args( "set " );
                        method_name_and_args += fields[ i ] + " ";
                        method_name_and_args += "\"" + escaped( unescaped( field_values[ i ], "rn\r\n" ), "\"" ) + "\"";

                        if( !log_field_value_pairs.empty( ) )
                           log_field_value_pairs += ",";

                        log_field_value_pairs += fields[ i ]
                         + "=" + search_replace( field_values[ i ], "\\\\", "\\\\\\\\", ",", "\\\\," );

                        execute_object_command( handle, "", method_name_and_args );
                     }

                     next_log_line += log_field_value_pairs + "\"";

                     if( !log_lines.empty( ) )
                        log_lines += "\n";
                     log_lines += next_log_line;

                     op_instance_apply( handle, "", false );

                     if( !is_update && !for_remove )
                        keys_created.insert( make_pair( key_value, mclass ) );
                  }

                  if( !for_remove && !key_prefix.empty( ) )
                  {
                     prefixed_class_keys[ mclass ].insert( next_key );

                     vector< pair< string, string > > base_class_info;
                     get_base_class_info( handle, "", base_class_info );

                     for( size_t i = 0; i < base_class_info.size( ); i++ )
                        prefixed_class_keys[ base_class_info[ i ].first ].insert( next_key );
                  }
               }
            }

            reader.finish_section( c_section_class );
            destroy_object_instance( handle );
            ++line_num;
         }
         catch( ... )
         {
            destroy_object_instance( handle );
            throw;
         }
      }

      reader.verify_finished_sections( );
   }
   catch( exception& x )
   {
      transaction_rollback( );

      string s( x.what( ) );

      // FUTURE: This message should be handled as a server string message.
      s += " This occurred while processing line #"
       + to_string( reader.get_last_line_num( ) ) + " of '" + filename + "'.";

      throw runtime_error( s );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }

   transaction_log_command( log_lines );
   transaction_commit( );

   if( has_key_list_file )
   {
      string new_file_name( replace_info.substr( 1 ) + ".new" );
      ofstream outf( new_file_name.c_str( ) );

      for( map< string, string >::iterator i = keys_created.begin( ); i != keys_created.end( ); ++i )
         outf << i->second << ':' << i->first << '\n';
   }
}

string instance_class( size_t handle, const string& context )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_display_name( );
}

string instance_key_info( size_t handle, const string& context, bool key_only )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   string retval( instance.get_key( ) );

   if( !key_only )
      retval += " =" + instance.get_version_info( )
       + " " + to_string( instance.get_state( ) ) + " " + instance.get_original_identity( );

   return retval;
}

string instance_get_variable( size_t handle, const string& context, const string& vname )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   return instance.get_variable( vname );
}

void instance_set_variable( size_t handle, const string& context, const string& vname, const string& value )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );

   instance.set_variable( vname, value );
}

void instance_check( class_base& instance, instance_check_rc* p_rc )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   class_base_accessor instance_accessor( instance );

   string sql;
   instance_accessor.fetch( sql, true, true );
   bool found = fetch_instance_from_db( instance, sql, true );

   if( !found )
   {
      if( p_rc )
         *p_rc = e_instance_check_rc_not_found;
      else
         throw runtime_error( get_string_message( GS( c_str_record_not_found ),
          make_pair( c_str_parm_record_not_found_class, instance.class_name( ) ),
          make_pair( c_str_parm_record_not_found_key, instance_accessor.get_lazy_fetch_key( ) ) ) );
   }
}

bool is_change_locked( class_base& instance, bool include_cascades )
{
   op_lock lock = gtp_session->p_storage_handler->get_lock_info( instance.lock_class_id( ), instance.get_key( ) );

   return lock.type >= op_lock::e_lock_type_update && ( include_cascades || !lock.p_root_class );
}

bool is_destroy_locked( class_base& instance, bool include_cascades )
{
   op_lock lock = gtp_session->p_storage_handler->get_lock_info( instance.lock_class_id( ), instance.get_key( ) );

   return lock.type == op_lock::e_lock_type_destroy && ( include_cascades || !lock.p_root_class );
}

bool is_create_locked_by_own_session( class_base& instance, const char* p_key, bool copy_field_values )
{
   op_lock lock;
   bool rc = false;

   lock = gtp_session->p_storage_handler->get_lock_info(
    instance.lock_class_id( ), p_key ? string( p_key ) : instance.get_key( ) );

   if( lock.p_session == gtp_session && lock.type == op_lock::e_lock_type_create )
   {
      rc = true;
      if( copy_field_values )
      {
         if( !lock.p_class_base )
            rc = false;
         else
            instance.copy_all_field_values( *lock.p_class_base );
      }
   }

   return rc;
}

bool is_update_locked_by_own_session( class_base& instance, const char* p_key )
{
   op_lock lock;
   bool rc = false;

   lock = gtp_session->p_storage_handler->get_lock_info(
    instance.lock_class_id( ), p_key ? string( p_key ) : instance.get_key( ) );

   return lock.p_session == gtp_session && lock.type == op_lock::e_lock_type_update;
}

void instance_fetch( size_t handle, const string& context, const string& key_info, instance_fetch_rc* p_rc )
{
   perform_instance_fetch( get_class_base_from_handle_for_op( handle, context ), key_info, p_rc );
}

void instance_set_parent( size_t handle, const string& context, const string& new_key )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   instance_accessor.set_key( new_key );
}

void instance_prepare_execute( size_t handle, const string& context, const string& key, const string& ver_info )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   instance_accessor.set_is_executing( true );

   if( key.empty( ) )
      instance_accessor.clear( );
   else
   {
      instance.perform_fetch( key );
      instance_accessor.set_ver_exp( ver_info );
   }
}

string instance_execute( size_t handle,
 const string& context, const string& key, const string& method_name_and_args )
{
   class_base& instance( get_class_base_from_handle( handle, context ) );
   class_base_accessor instance_accessor( instance );

   TRACE_LOG( TRACE_CLASSOPS, "instance_execute( ) [class: " + to_string( instance.class_name( ) ) + "] " + method_name_and_args );

   return instance.execute( method_name_and_args );
}

bool instance_iterate( size_t handle, const string& context,
 const string& key_info, const string& fields, const string& text,
 const string& query, const string& security_info, iter_direction direction,
 bool inclusive, int row_limit, sql_optimisation optimisation, const set< string >* p_filters )
{
   return perform_instance_iterate(
    get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_none,
    false ), key_info, fields, text, query, security_info, direction, inclusive, row_limit, optimisation, p_filters );
}

bool instance_iterate_next( size_t handle, const string& context )
{
   return perform_instance_iterate_next(
    get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_none, false ) );
}

void instance_iterate_stop( size_t handle, const string& context )
{
   get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_none, false ).iterate_stop( );
}

void instance_review_begin( size_t handle, const string& context, const string& key )
{
   get_class_base_from_handle_for_op( handle, context ).begin_review( key );
}

void instance_review_finish( size_t handle, const string& context )
{
   get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_review ).finish_review( );
}

bool instance_filtered( size_t handle, const string& context )
{
   return get_class_base_from_handle( handle, context ).filtered( );
}

void op_instance_create( size_t handle,
 const string& context, const string& key, bool internal_operation, op_create_rc* p_rc )
{
   get_class_base_from_handle_for_op( handle, context ).op_create( key, p_rc, internal_operation );
}

void op_instance_update( size_t handle,
 const string& context, const string& key, const string& ver_info, bool internal_operation, op_update_rc* p_rc )
{
   class_base& instance( get_class_base_from_handle_for_op( handle, context ) );

   class_base_accessor( instance ).set_ver_exp( ver_info );
   instance.op_update( key, p_rc, internal_operation );
}

void op_instance_destroy( size_t handle,
 const string& context, const string& key, const string& ver_info, bool internal_operation, op_destroy_rc* p_rc )
{
   class_base& instance( get_class_base_from_handle_for_op( handle, context ) );

   class_base_accessor( instance ).set_ver_exp( ver_info );
   instance.op_destroy( key, p_rc, internal_operation );
}

void op_instance_apply( size_t handle, const string& context,
 bool internal_operation, op_apply_rc* p_rc, set< string >* p_fields_set )
{
   class_base& instance( get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_create_update_destroy ) );

   op_apply_rc rc;
   instance.op_apply( &rc, internal_operation, p_fields_set );

   if( p_rc )
      *p_rc = rc;
   else
   {
      if( rc == e_op_apply_rc_invalid )
         throw runtime_error( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );
      else if( rc != e_op_apply_rc_okay )
         throw runtime_error( "unexpected rc value " + to_string( rc ) + " from op_apply" );
   }
}

void op_instance_cancel( size_t handle, const string& context, bool internal_operation )
{
   get_class_base_from_handle_for_op( handle, context, e_permit_op_type_value_any ).op_cancel( internal_operation );
}

void output_progress_message( const string& message )
{
   if( gtp_session )
      gtp_session->cmd_handler.output_progress( message );
}

void transaction_start( )
{
   if( !ods::instance( ) )
      throw runtime_error( "no storage is currently attached" );

   if( gtp_session->ap_db.get( ) && gtp_session->transactions.empty( ) )
   {
      TRACE_LOG( TRACE_SQLSTMTS, "BEGIN" );
      exec_sql( *gtp_session->ap_db, "BEGIN" );
   }

   gtp_session->transactions.push( new ods::transaction( *ods::instance( ) ) );
}

void transaction_commit( )
{
   if( gtp_session->transactions.empty( ) )
      throw runtime_error( "no active transaction exists" );

   storage_handler& handler( *gtp_session->p_storage_handler );

   // NOTE: Scope for guard object.
   {
      guard g( g_mutex );

      gtp_session->transactions.top( )->commit( );

      if( gtp_session->transactions.size( ) == 1 )
      {
         append_transaction_log_command( handler );

         if( gtp_session->ap_db.get( ) )
         {
            TRACE_LOG( TRACE_SQLSTMTS, "COMMIT" );
            exec_sql( *gtp_session->ap_db, "COMMIT" );
         }
      }
   }

   handler.release_locks_for_commit( gtp_session );

   delete gtp_session->transactions.top( );
   gtp_session->transactions.pop( );
}

void transaction_rollback( )
{
   if( gtp_session->transactions.empty( ) )
      throw runtime_error( "no active transaction exists" );

   gtp_session->transactions.top( )->rollback( );

   gtp_session->p_storage_handler->release_locks_for_rollback( gtp_session );

   delete gtp_session->transactions.top( );
   gtp_session->transactions.pop( );

   if( gtp_session->ap_db.get( ) && gtp_session->transactions.empty( ) )
   {
      TRACE_LOG( TRACE_SQLSTMTS, "ROLLBACK" );
      exec_sql( *gtp_session->ap_db, "ROLLBACK" );

      gtp_session->transaction_log_command.erase( );
   }
}

size_t transaction_id( )
{
   return ods::instance( )->get_transaction_id( );
}

size_t transaction_level( )
{
   return gtp_session->transactions.size( );
}

size_t set_transaction_id( size_t tx_id )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );
   storable_identity& identity( *handler.get_root( ).o_identity );

   identity.next_id = tx_id;

   if( identity.next_id >= identity.ceiling )
   {
      while( identity.next_id >= identity.ceiling )
         identity.ceiling += c_identity_burn;

      // NOTE: Store the "ceiling" rather than "next_id" to avoid having to
      // perform extra I/O for every transaction. If storage termination is
      // to occur normally then the actual "next_id" will be written and no
      // identity value is lost. If for some reason normal termination does
      // not occur then up to the "burn" number of identities will be lost.
      restorable< int > tmp_identity( identity.next_id, identity.ceiling );
      handler.get_root( ).o_identity.store( e_oid_pointer_opt_force_write );
   }

   return identity.next_id;
}

size_t next_transaction_id( )
{
   guard g( g_mutex );

   storage_handler& handler( *gtp_session->p_storage_handler );
   storable_identity& identity( *handler.get_root( ).o_identity );

   return identity.next_id;
}

void transaction_log_command( const string& log_command )
{
   if( log_command.empty( ) )
      gtp_session->transaction_log_command.erase( );
   else
   {
      if( !gtp_session->transaction_log_command.empty( ) )
         gtp_session->transaction_log_command += '\n';

      gtp_session->transaction_log_command += log_command;
   }
}

transaction::transaction( bool is_not_dummy )
 :
 is_dummy( !is_not_dummy ),
 can_commit( true )
{
   if( !is_dummy )
      transaction_start( );
}

transaction::~transaction( )
{
   if( !is_dummy && can_commit )
      transaction_rollback( );
}

void transaction::commit( )
{
   if( !is_dummy )
   {
      if( !can_commit )
         throw runtime_error( "cannot commit already completed transaction" );

      transaction_commit( );
      can_commit = false;
   }
}

void transaction::rollback( )
{
   if( !is_dummy )
   {
      if( !can_commit )
         throw runtime_error( "cannot rollback already completed transaction" );

      transaction_rollback( );
      can_commit = false;
   }
}

record_initialiser::record_initialiser( ofstream& log_file )
{
   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.set_alternative_log_file( &log_file );
}

record_initialiser::~record_initialiser( )
{
   storage_handler& handler( *gtp_session->p_storage_handler );

   handler.set_alternative_log_file( 0 );
}

void begin_instance_op( instance_op op, class_base& instance,
 const string& key, bool internal_modification, instance_op_rc* p_rc )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "No storage is currently linked." );

   if( p_rc )
      *p_rc = e_instance_op_rc_okay;

   if( op == e_instance_op_none )
      return;

   TRACE_LOG( TRACE_CLASSOPS, "begin (enter) op = "
    + to_string( op ) + ", class = " + instance.class_name( )
    + ", internal = " + to_string( internal_modification ) + ", key = " + key );

   if( instance.get_is_in_op( ) )
      throw runtime_error( "cannot begin an instance operation whilst already perfoming an instance operation" );

   string sql;
   bool is_cascade_op = false;

   string key_in_use( instance.get_key( ) );

   string key_for_op( key );
   if( key_for_op.empty( ) )
      key_for_op = key_in_use;

   if( key_for_op.empty( ) )
      throw runtime_error( "cannot perform an instance operation without a key or current instance" );

   string clone_key;
   if( op == e_instance_op_create )
   {
      string::size_type pos = key_for_op.find( ' ' );
      if( pos != string::npos )
      {
         clone_key = key.substr( pos + 1 );
         key_for_op = key.substr( 0, pos );
      }

      if( key_for_op.find_first_of( "`~!@#$%^&*<>()[]{}/\\?|-+=.,;:'\"" ) != string::npos )
         throw runtime_error( get_string_message( GS( c_str_key_invalid ),
          make_pair( c_str_parm_key_invalid_key, key_for_op ) ) );
   }

   size_t lock_handle( 0 );
   size_t xlock_handle( 0 );
   class_base_accessor instance_accessor( instance );

   storage_handler& handler( *gtp_session->p_storage_handler );

   const string& class_id( instance.class_id( ) );
   const string& module_name( instance.module_name( ) );
   const string& lock_class_id( instance.lock_class_id( ) );

   bool is_minimal_update( op == e_instance_op_update && !instance_accessor.fetch_field_names( ).empty( ) );

   if( is_minimal_update )
      TRACE_LOG( TRACE_CLASSOPS, "*** minimal update ***" );

   // NOTE: A create op can be started (but not applied) without an instance key (this is to help with record
   // preparation when cloning in order to create a new instance).
   if( op != e_instance_op_create || !key_for_op.empty( ) )
   {
      bool obtained_lock = false;

      if( op == e_instance_op_review
       && handler.obtain_lock( lock_handle, lock_class_id, key_for_op, op_lock::e_lock_type_review, gtp_session, &instance ) )
         obtained_lock = true;

      if( op == e_instance_op_create
       && handler.obtain_lock( lock_handle, lock_class_id, key_for_op, op_lock::e_lock_type_create, gtp_session, &instance ) )
         obtained_lock = true;

      if( op == e_instance_op_update || op == e_instance_op_destroy )
      {
         if( instance.get_is_being_cascaded( ) )
         {
            obtained_lock = true;
            is_cascade_op = true;
         }

         if( !obtained_lock )
         {
            if( op == e_instance_op_update
             && handler.obtain_lock( lock_handle, lock_class_id, key_for_op, op_lock::e_lock_type_update, gtp_session, &instance ) )
               obtained_lock = true;

            if( op == e_instance_op_destroy
             && handler.obtain_lock( lock_handle, lock_class_id, key_for_op, op_lock::e_lock_type_destroy, gtp_session, &instance ) )
               obtained_lock = true;
         }
      }

      if( !obtained_lock )
      {
         if( p_rc )
         {
            *p_rc = e_instance_op_rc_locked;
            return;
         }
         else
            throw runtime_error( "unable to obtain lock for '" + key_for_op + "'" );
      }

      scoped_lock_holder lock_holder( handler, lock_handle );

      if( op == e_instance_op_destroy )
      {
         // NOTE: In order to correctly determine whether an instance is constrained it must be first fetched.
         instance_accessor.set_key( key_for_op, true );

         instance_accessor.fetch( sql, false );
         bool found = fetch_instance_from_db( instance, sql );

         if( !found )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_not_found;
               return;
            }
            else
               throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                make_pair( c_str_parm_record_not_found_class, instance.class_name( ) ),
                make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
         }

         string ver_expected( instance_accessor.get_ver_exp( ) );
         if( !ver_expected.empty( ) && ver_expected != instance.get_version_info( ) )
            throw runtime_error( get_string_message( GS( c_str_version_mismatch ),
             make_pair( c_str_parm_version_mismatch_found, instance.get_version_info( ) ),
             make_pair( c_str_parm_version_mismatch_expected, ver_expected ) ) );

         if( !instance_accessor.can_destroy( internal_modification ) )
            throw runtime_error( get_string_message( GS( c_str_cannot_destroy ),
             make_pair( c_str_parm_cannot_destroy_class, instance.get_display_name( ) ) ) );

         if( instance.get_current_identity( ) != instance.get_original_identity( ) )
            throw runtime_error( "cannot destroy '" + instance.get_original_identity( )
             + "' stored instance using '" + instance.get_current_identity( ) + "' object instance" );

         string constraining_class;
         bool is_constrained = false;

         // NOTE: Unless performing the cascade obtain locks to all children (which are held in
         // each child's graph parent) and then check for the existence of restricting children.
         if( !is_cascade_op )
         {
            map< string, set< string > > instance_keys;
            if( !obtain_cascade_locks_for_destroy( instance, instance, instance_keys ) )
            {
               handler.release_locks_for_owner( instance, true );

               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_child_locked;
                  return;
               }
               else
                  throw runtime_error( "instance '" + key_for_op
                   + "' cannot be destroyed as a lock to a dependent child could not be obtained" );
            }

            instance_keys.clear( );

            if( !session_skip_is_constained( ) )
               is_constrained = is_child_constrained( instance, instance, constraining_class, instance_keys );
         }

         if( is_constrained )
         {
            handler.release_locks_for_owner( instance, true );

            if( p_rc )
            {
               *p_rc = e_instance_op_rc_constrained;
               return;
            }
            else
               throw runtime_error( get_string_message( GS( c_str_record_constrained ),
                make_pair( c_str_parm_record_constrained_class, constraining_class ) ) );
         }
      }

      lock_holder.release( );
   }

   if( op == e_instance_op_create )
   {
      // NOTE: If cloning then retrieve the instance values of the clone source under a review lock. This review lock
      // will be held until the create operation is finished so that link locks to any foreign keys are not required.
      if( !clone_key.empty( ) )
      {
         const op_lock& lock_info = handler.get_lock_info( lock_class_id, clone_key );

         if( ( lock_info.handle && lock_info.type >= op_lock::e_lock_type_review && lock_info.p_session == gtp_session )
          || handler.obtain_lock( xlock_handle, lock_class_id, clone_key, op_lock::e_lock_type_review, gtp_session, &instance ) )
         {
            guard g( g_mutex );

            scoped_lock_holder xlock_holder( handler, xlock_handle );

            instance_accessor.set_key( clone_key, true );

            instance_accessor.fetch( sql, false );
            bool found = fetch_instance_from_db( instance, sql );

            xlock_holder.release( );

            if( !found )
            {
               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_not_found;
                  return;
               }
               else
                  throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                   make_pair( c_str_parm_record_not_found_class, instance.class_name( ) ),
                   make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
            }
         }
         else
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_locked;
               return;
            }
            else
               throw runtime_error( "unable to obtain lock for '" + clone_key + "'" );
         }
      }
      else
         instance_accessor.set_default_values( );
   }

   instance_accessor.set_key( key_for_op, true );
   instance_accessor.set_clone_key( clone_key );

   if( op != e_instance_op_update )
      instance_accessor.fetch_field_names( ).clear( );

   class_base::op_type old_op( instance.get_op( ) );
   try
   {
      scoped_lock_holder lock_holder( handler, lock_handle );
      scoped_lock_holder xlock_holder( handler, xlock_handle );

      if( op == e_instance_op_create )
      {
         string sql;

         // NOTE: Skip this check during a restore as an optimsation to reduce SQL.
         if( !storage_locked_for_admin( ) )
         {
            instance_accessor.fetch( sql, true );
            bool found = fetch_instance_from_db( instance, sql, true );

            if( found )
            {
               if( p_rc )
               {
                  *p_rc = e_instance_op_rc_already_exists;
                  return;
               }
               else
                  throw runtime_error( get_string_message( GS( c_str_record_exists ),
                   make_pair( c_str_parm_record_exists_key, instance.get_key( ) ),
                   make_pair( c_str_parm_record_exists_class, instance.get_display_name( ) ) ) );
            }
         }

         // NOTE: In order for foreign keys to be linked the "create" op must be set.
         instance_accessor.set_op( class_base::e_op_type_create, false );
         instance_accessor.at_create( );
      }
      else if( op == e_instance_op_review || op == e_instance_op_update )
      {
         string sql;

         instance_accessor.fetch( sql, false, false );
         bool found = fetch_instance_from_db( instance, sql, false, is_minimal_update );

         if( !found )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_not_found;
               return;
            }
            else
               throw runtime_error( get_string_message( GS( c_str_record_not_found ),
                make_pair( c_str_parm_record_not_found_class, instance.get_display_name( ) ),
                make_pair( c_str_parm_record_not_found_key, instance.get_key( ) ) ) );
         }

         string ver_expected( instance_accessor.get_ver_exp( ) );
         if( !ver_expected.empty( ) && ver_expected != instance.get_version_info( ) )
            throw runtime_error( get_string_message( GS( c_str_version_mismatch ),
             make_pair( c_str_parm_version_mismatch_found, instance.get_version_info( ) ),
             make_pair( c_str_parm_version_mismatch_expected, ver_expected ) ) );

         if( op == e_instance_op_update
          && ( !internal_modification
          && ( instance.get_state( ) & c_state_uneditable )
          && !( instance.get_state( ) & c_state_ignore_uneditable ) ) )
            throw runtime_error( get_string_message( GS( c_str_cannot_update ),
             make_pair( c_str_parm_cannot_update_class, instance.get_display_name( ) ) ) );

         if( op == e_instance_op_update
          && instance.get_current_identity( ) != instance.get_original_identity( ) )
            throw runtime_error( "cannot update '" + instance.get_original_identity( )
             + "' stored instance using '" + instance.get_current_identity( ) + "' object instance" );
      }

      lock_holder.release( );
      xlock_holder.release( );
   }
   catch( ... )
   {
      instance_accessor.set_op( old_op, false );

      if( op == e_instance_op_destroy && !is_cascade_op )
         handler.release_locks_for_owner( instance, true );

      throw;
   }

   instance_accessor.set_lock_handle( lock_handle );

   if( xlock_handle )
      instance_accessor.set_xlock_handle( xlock_handle );

   if( op == e_instance_op_review )
      instance_accessor.set_op( class_base::e_op_type_review, key_for_op != key_in_use );
   else if( op == e_instance_op_create )
      instance_accessor.set_op( class_base::e_op_type_create, true );
   else if( op == e_instance_op_update )
      instance_accessor.set_op( class_base::e_op_type_update, key_for_op != key_in_use );
   else if( op == e_instance_op_destroy )
      instance_accessor.set_op( class_base::e_op_type_destroy, key_for_op != key_in_use );

   TRACE_LOG( TRACE_CLASSOPS, "begin (leave) op = "
    + to_string( op ) + ", class = " + instance.class_name( )
    + ", internal = " + to_string( internal_modification ) + ", key = " + key );
}

void finish_instance_op( class_base& instance, bool apply_changes,
 bool internal_operation, instance_op_rc* p_rc, set< string >* p_fields_set )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   if( p_rc )
      *p_rc = e_instance_op_rc_okay;

   class_base::op_type op = instance.get_op( );

   if( op == class_base::e_op_type_none )
      return;

   TRACE_LOG( TRACE_CLASSOPS, "finish (enter) op = "
    + to_string( op ) + ", class = " + instance.class_name( )
    + ", internal = " + to_string( internal_operation )
    + ", apply_changes = " + to_string( apply_changes ) + ", key = " + instance.get_key( ) );

   class_base_accessor instance_accessor( instance );
   storage_handler& handler( *gtp_session->p_storage_handler );

   if( !apply_changes || op == class_base::e_op_type_review )
      perform_op_cancel( handler, instance, op );
   else
   {
      // NOTE: In order to make "minimal" updates as minimal as possible both the "to_store" trigger function
      // and validation is being skipped, so such updates must only be used in circumstances when it is known
      // that they will not affect the record's validity (i.e. not to be used lightly).
      if( op == class_base::e_op_type_create
       || ( op == class_base::e_op_type_update && !instance.get_is_minimal_update( ) ) )
      {
         bool valid = true;
         // NOTE: Validation can be switched off as an optimisation during a "storage restore".
         if( p_fields_set && !session_skip_validation( ) && !instance.is_valid( internal_operation, p_fields_set ) )
            valid = false;

         if( valid )
         {
            instance_accessor.perform_to_store( op == class_base::e_op_type_create, internal_operation );

            if( !session_skip_validation( ) && !instance.is_valid( internal_operation ) )
               valid = false;
         }

         if( !valid )
         {
            if( p_rc )
            {
               *p_rc = e_instance_op_rc_invalid;
               return;
            }
            else
            {
               string error_message( instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );
               perform_op_cancel( handler, instance, op );
               throw runtime_error( error_message );
            }
         }
      }

      ods* p_ods( ods::instance( ) );

      bool op_is_in_transaction( !gtp_session->transactions.empty( ) );

      if( instance_accessor.get_lock_handle( )
       && handler.get_lock_info( instance_accessor.get_lock_handle( ) ).transaction_level != p_ods->get_transaction_level( ) )
         throw runtime_error( "attempt to perform apply for operation commenced outside the current transaction scope" );

      bool executing_sql = false;
      try
      {
         // NOTE: The "for_store" or "for_destroy" triggers may result in further create, update
         // or destroy operations so a transaction must be commenced here (unless already in one).
         transaction tx( !op_is_in_transaction );

         if( op == class_base::e_op_type_destroy )
            instance_accessor.for_destroy( internal_operation );
         else if( op == class_base::e_op_type_create
          || ( op == class_base::e_op_type_update && !instance.get_is_minimal_update( ) ) )
         {
            instance_accessor.for_store( op == class_base::e_op_type_create, internal_operation );

            // NOTE: As it's possible that "for_store" might inadvertantly have made the record invalid
            // the validation call is repeated now and the first error (if any is found) will be thrown.
            if( !session_skip_validation( ) && !instance.is_valid( internal_operation ) )
               throw runtime_error(
                instance.get_validation_errors( class_base::e_validation_errors_type_first_only ) );
         }

         vector< string > sql_stmts;
         if( !instance_accessor.get_sql_stmts( sql_stmts ) )
            throw runtime_error( "unexpected get_sql_stmts failure" );

         // NOTE: If updating but no fields apart from the ver/rev ones were changed (by any
         // derivation) then all update statements are discarded to skip the unnecessary SQL.
         if( op == class_base::e_op_type_update && instance_accessor.has_skipped_empty_update( ) )
            sql_stmts.clear( );

         if( gtp_session->ap_db.get( ) )
         {
            executing_sql = true;

            for( size_t i = 0; i < sql_stmts.size( ); i++ )
            {
               if( sql_stmts[ i ].empty( ) )
                  continue;

               TRACE_LOG( TRACE_SQLSTMTS, sql_stmts[ i ] );
               exec_sql( *gtp_session->ap_db, sql_stmts[ i ] );

               ++gtp_session->sql_count;
            }
         }

         executing_sql = false;

         // NOTE: In order to be able to create child records (or to review the just created instance)
         // the "create" lock is downgraded to an "update" lock after the SQL is executed but prior to
         // the transaction being committed. If the SQL engine allows "phantom" reads then it could be
         // possible for data to become corrupted if the transaction does not end up being committed.
         if( op == class_base::e_op_type_create )
            handler.transform_lock( instance_accessor.get_lock_handle( ), op_lock::e_lock_type_update );

         // NOTE: Although "after_store" is normally skipped for "minimal" updates in the case of Meta
         // it must still be called so that "aliased" class artifacts will behave as would be expected.
         if( op == class_base::e_op_type_destroy )
            instance_accessor.after_destroy( internal_operation );
         else if( op == class_base::e_op_type_create || ( op == class_base::e_op_type_update
          && ( handler.get_name( ) == "Meta" || !instance.get_is_minimal_update( ) ) ) )
         {
            class_after_store cas( instance );
            instance_accessor.after_store( op == class_base::e_op_type_create, internal_operation );
         }

         const string& key( instance.get_key( ) );
         storage_handler& handler( *gtp_session->p_storage_handler );

         if( !op_is_in_transaction )
         {
            guard g( g_mutex );

            tx.commit( );
            append_transaction_log_command( handler );
         }

         // FUTURE: It would be more efficient not to call "release_locks_for_owner" unless
         // it is known that cascade locks had actually been obtained when the destroy began.
         // Note that this call also occurs in "perform_op_cancel" (when revisited it should
         // be refactored into "class_base" where the other locks are being released).
         if( op == class_base::e_op_type_destroy && !instance.get_is_being_cascaded( ) )
            handler.release_locks_for_owner( instance );

         instance_accessor.finish( );
      }
      catch( exception& e )
      {
         string field;

         perform_op_cancel( handler, instance, op );

         // NOTE: A failed INSERT or UPDATE could be due to a unique index violation
         // so query all unique indexes to determine if indeed this has occurred and
         // if so then report the error against the last field of the smallest index.
         if( executing_sql
          && ( op == class_base::e_op_type_create || op == class_base::e_op_type_update ) )
         {
            vector< string > all_unique_indexes;
            instance.get_sql_unique_indexes( all_unique_indexes );

            vector< string > all_column_names;
            instance.get_sql_column_names( all_column_names );

            vector< string > all_column_values;
            instance.get_sql_column_values( all_column_values );

            map< string, size_t > column_numbers;
            for( size_t i = 0; i < all_column_names.size( ); i++ )
               column_numbers.insert( make_pair( all_column_names[ i ], i ) );

            size_t num_columns = 0;
            for( size_t i = 0; i < all_unique_indexes.size( ); i++ )
            {
               vector< string > unique_index_columns;
               split( all_unique_indexes[ i ], unique_index_columns );

               string sql( "SELECT C_Key_ FROM T_" + string( instance.module_name( ) )
                + "_" + string( instance.class_name( ) ) + " WHERE " );
               for( size_t j = 0; j < unique_index_columns.size( ); j++ )
               {
                  if( column_numbers.count( unique_index_columns[ j ] ) == 0 )
                     throw runtime_error( "unexpected column name '" + unique_index_columns[ j ] + "'" );

                  if( j > 0 )
                     sql += " AND ";
                  sql += all_column_names[ column_numbers[ unique_index_columns[ j ] ] ];
                  sql += " = " + all_column_values[ column_numbers[ unique_index_columns[ j ] ] ];
               }

               TRACE_LOG( TRACE_SQLSTMTS, sql );

               sql_dataset ds( *gtp_session->ap_db.get( ), sql );
               if( ds.next( ) && ds.as_string( 0 ) != instance.get_key( ) )
               {
                  if( field.empty( ) || unique_index_columns.size( ) < num_columns )
                  {
                     num_columns = unique_index_columns.size( );
                     field = unique_index_columns[ unique_index_columns.size( ) - 1 ].substr( 2 );
                  }
               }
            }
         }

         if( field.empty( ) )
            throw;
         else
         {
            TRACE_LOG( TRACE_SQLSTMTS, e.what( ) );

            throw runtime_error( get_string_message(
             GS( c_str_index_duplicate ), make_pair( c_str_parm_index_duplicate_field,
             instance.get_field_display_name( instance_accessor.get_field_id( field ) ) ),
             make_pair( c_str_parm_index_duplicate_value,
             instance.get_field_value( instance.get_field_num( field ) ) ),
             make_pair( c_str_parm_index_duplicate_class, instance.get_display_name( ) ) ) );
         }
      }
   }

   instance_accessor.set_ver_exp( "" );

   TRACE_LOG( TRACE_CLASSOPS, "finish (leave) op = "
    + to_string( op ) + ", class = " + instance.class_name( )
    + ", internal = " + to_string( internal_operation )
    + ", apply_changes = " + to_string( apply_changes ) + ", key = " + instance.get_key( ) );
}

void perform_instance_fetch( class_base& instance,
 const string& key_info, instance_fetch_rc* p_rc, bool only_sys_fields )
{
   if( !gtp_session->p_storage_handler->get_ods( ) )
      throw runtime_error( "no storage is currently linked" );

   if( key_info.empty( ) )
      throw runtime_error( "cannot fetch "
       + string( instance.class_name( ) ) + " record without key information" );

   if( p_rc )
      *p_rc = e_instance_fetch_rc_okay;

   const string& class_id( instance.class_id( ) );
   const string& module_name( instance.module_name( ) );

   class_base_accessor instance_accessor( instance );
   storage_handler& handler( *gtp_session->p_storage_handler );

   if( instance.get_is_in_op( ) )
      throw runtime_error( "cannot fetch "
       + string( instance.class_name( ) ) + " record whilst currently perfoming an instance operation" );

   string sql;
   vector< string > field_info;
   vector< string > order_info;
   vector< pair< string, string > > query_info;
   vector< pair< string, string > > fixed_info;
   vector< pair< string, string > > paging_info;

   split_key_info( key_info, fixed_info, paging_info, order_info, true );

   sql = construct_sql_select( instance,
    field_info, order_info, query_info, fixed_info, paging_info, "", false, true, 1, only_sys_fields, "" );

   bool found = fetch_instance_from_db( instance, sql );

   if( !found )
   {
      if( p_rc )
         *p_rc = e_instance_fetch_rc_not_found;
      else
         throw runtime_error( get_string_message( GS( c_str_record_not_found ),
          make_pair( c_str_parm_record_not_found_class, instance.class_name( ) ),
          make_pair( c_str_parm_record_not_found_key, key_info ) ) );
   }
}

bool perform_instance_iterate( class_base& instance,
 const string& key_info, const string& fields, const string& text,
 const string& query, const string& security_info, iter_direction direction,
 bool inclusive, int row_limit, sql_optimisation optimisation, const set< string >* p_filters )
{
   bool found = false;

   TRACE_LOG( TRACE_CLASSOPS, "[iterate] class = '"
    + to_string( instance.class_name( ) ) + "', key_info = '" + key_info
    + "', fields = '" + fields + "', direction = " + to_string( direction ) );

   oid id;
   string sql, key, key_value( key_info );

   size_t num_fields = 0;

   const string& class_id( instance.class_id( ) );
   const string& module_name( instance.module_name( ) );

   class_base_accessor instance_accessor( instance );
   storage_handler& handler( *gtp_session->p_storage_handler );

   // NOTE: If row_limit < 0 then iteration is being continued.
   if( row_limit >= 0 && instance.get_is_iterating( ) )
   {
      string class_name( instance.class_name( ) );
      class_base* p_parent = instance.get_graph_parent( );
      while( p_parent )
      {
         class_name += string( " <- " ) + p_parent->class_name( );
         p_parent = p_parent->get_graph_parent( );
      }

      throw runtime_error( "iterate called whilst already iterating (class: " + class_name + ")" );
   }

   // NOTE: Because filtering can exclude records from the DB fetch the limit must be
   // omitted if any filters have been supplied.
   if( p_filters )
   {
      row_limit = 0;
      instance_accessor.filters( ) = *p_filters;
   }

   if( instance.get_is_in_op( ) )
      throw runtime_error( "cannot begin iteration whilst currently perfoming an instance operation" );
   else
   {
      if( row_limit >= 0 && key_info != c_nul_key )
      {
         vector< string > field_info;
         vector< string > order_info;
         vector< pair< string, string > > query_info;
         vector< pair< string, string > > fixed_info;
         vector< pair< string, string > > paging_info;

         set< string > supplied_fields;
         set< string > transient_field_names;

         int num_fields = instance.get_num_fields( );
         for( int i = 0; i < num_fields; i++ )
         {
            if( instance.is_field_transient( i ) )
               transient_field_names.insert( instance.get_field_name( i ) );
         }

         if( !fields.empty( ) && fields != c_key_field )
         {
            set< string > fetch_field_names;

            vector< string > tmp_field_info;
            split( fields, tmp_field_info );
            for( size_t i = 0; i < tmp_field_info.size( ); i++ )
            {
               if( tmp_field_info[ i ].find( '.' ) == string::npos )
               {
                  string next( tmp_field_info[ i ] );

                  if( next != c_ignore_field )
                  {
                     get_field_name( instance, next );

                     fetch_field_names.insert( next );

                     if( !transient_field_names.count( next ) )
                     {
                        field_info.push_back( tmp_field_info[ i ] );
                        supplied_fields.insert( tmp_field_info[ i ] );
                     }
                  }
               }
            }

            instance_accessor.fetch_field_names( ) = fetch_field_names;

            // NOTE: If there are fields that are required in order to determine
            // state correctly then these fields are appended to the field list.
            set< string > required_fields;
            set< string > field_dependents;
            instance.get_required_field_names( required_fields, false, &field_dependents );

            // NOTE: It is possible that due to "interdependent" required fields
            // some required fields may not have been added in the first or even
            // later calls to "get_required_field_names" so continue calling the
            // function until no further field names have been added.
            size_t required_fields_added = required_fields.size( );
            while( required_fields_added )
            {
               instance.get_required_field_names( required_fields, false, &field_dependents );
               if( required_fields.size( ) == required_fields_added )
                  break;

               required_fields_added = required_fields.size( );
            }

            for( set< string >::iterator i = required_fields.begin( ); i != required_fields.end( ); ++i )
            {
               if( !supplied_fields.count( *i ) )
                  field_info.push_back( *i );
            }
         }

         // NOTE: If this class object is its graph parent's "child" then the iteration is
         // restricted to those instances that belong to the parent (via a non-primary index).
         if( instance.get_graph_parent( ) && !instance.get_is_singular( ) )
         {
            order_info.push_back( instance.get_graph_parent_fk_field( ) );
            fixed_info.push_back(
             make_pair( instance.get_graph_parent_fk_field( ), instance.get_graph_parent( )->get_key( ) ) );
         }

         instance_accessor.add_extra_fixed_info( fixed_info );
         instance_accessor.add_extra_paging_info( paging_info );

         // NOTE: If the key info contains any transient field names (for ordering)
         // then these need to be replaced by zero or more persistent field names.
         string keys( key_info );

         string extra_key_info;
         string final_key_info;

         size_t pos = keys.find( '#' );
         if( pos != string::npos )
         {
            extra_key_info = keys.substr( pos );
            keys.erase( pos );
         }
         else
         {
            pos = keys.find( ' ' );
            if( pos != string::npos )
            {
               extra_key_info = keys.substr( pos );
               keys.erase( pos );
            }
         }

         if( pos == string::npos )
            final_key_info = key_info;
         else
         {
            vector< string > all_keys;
            if( !keys.empty( ) )
               split( keys, all_keys );

            vector< string > final_keys;
            for( size_t i = 0; i < all_keys.size( ); i++ )
            {
               string next( all_keys[ i ] );
               get_field_name( instance, next );
   
               if( !transient_field_names.count( next ) )
                  final_keys.push_back( next );
               else
                  instance.get_transient_replacement_field_names( next, final_keys );
            }

            for( size_t i = 0; i < final_keys.size( ); i++ )
            {
               if( !final_key_info.empty( ) )
                  final_key_info += ',';
               final_key_info += final_keys[ i ];
            }

            final_key_info += extra_key_info;
         }

         split_key_info( final_key_info, fixed_info,
          paging_info, order_info, ( optimisation == e_sql_optimisation_unordered ) );

         if( !query.empty( ) )
         {
            vector< string > query_parts;
            split( query, query_parts );

            for( size_t i = 0; i < query_parts.size( ); i++ )
            {
               string::size_type pos = query_parts[ i ].find( ':' );
               if( pos == string::npos )
                  pos = query_parts[ i ].find( '=' ); // i.e. allow for either <field>:<value_info> or <field>=<value_info>

               if( pos == string::npos )
                  throw runtime_error( "unexpected query part format '" + query_parts[ i ] + "'" );

               string field_name( query_parts[ i ].substr( 0, pos ) );
               string field_values( query_parts[ i ].substr( pos + 1 ) );

               query_info.push_back( make_pair( field_name, field_values ) );
            }
         }

         sql = construct_sql_select( instance,
          field_info, order_info, query_info, fixed_info, paging_info, security_info,
          ( direction == e_iter_direction_backwards ), inclusive, row_limit, ( fields == c_key_field ), text );

         TRACE_LOG( TRACE_SQLSTMTS, sql );

         if( instance_accessor.p_sql_dataset( ) )
            delete instance_accessor.p_sql_dataset( );
         instance_accessor.p_sql_dataset( ) = new sql_dataset( *gtp_session->ap_db, sql );

         setup_select_columns( instance, field_info );
      }

      bool skip_after_fetch = false;
      string skip_after_fetch_var( instance.get_variable( "@skip_after_fetch" ) );

      if( skip_after_fetch_var == "1" || skip_after_fetch_var == "true" )
         skip_after_fetch = true;

      if( row_limit < 0 )
         found = true;
      else
      {
         // NOTE: Unless a single row limit was specified (which is an alternate way of performing an
         // instance fetch) then iteration is flagged so that "after_fetch" triggers can detect this.
         if( row_limit != 1 )
         {
            instance_accessor.set_iteration_starting( true );
            instance_accessor.set_is_in_iteration( true, direction == e_iter_direction_forwards );
         }

         found = fetch_instance_from_db( instance,
          instance_accessor.select_fields( ), instance_accessor.select_columns( ), skip_after_fetch );

         // NOTE: It is expected that the "after_fetch" trigger will be being skipped due to a later
         // "prepare" call which will call the trigger and then clear this flag (otherwise dependent
         // fields might be missed when the trigger is actually called).
         if( !skip_after_fetch )
            instance_accessor.set_iteration_starting( false );

         ++gtp_session->sql_count;

         IF_IS_TRACING( TRACE_SQLSTMTS )
         {
            string sql_plan( "EXPLAIN " + sql );
            sql_dataset ds( *gtp_session->ap_db, sql_plan );

            sql_plan = "QUERY PLAN:";
            while( ds.next( ) )
            {
               sql_plan += '\n';

               for( size_t i = 0; i < ds.get_fieldcount( ); i++ )
               {
                  if( i > 0 )
                     sql_plan += " | ";
                  sql_plan += ds.as_string( i );
               }
            }

            TRACE_LOG( TRACE_SQLSTMTS, sql_plan );
         }
      }

      if( found )
      {
         if( !instance_accessor.p_sql_dataset( ) )
            throw runtime_error( "unexpected null dataset in perform_instance_iterate" );

         deque< vector< string > > rows;
         sql_dataset& ds( *instance_accessor.p_sql_dataset( ) );

         bool query_finished = true;
         while( ds.next( ) )
         {
            vector< string > columns;
            for( size_t i = 0; i < ds.get_fieldcount( ); i++ )
               columns.push_back( ds.as_string( i ) );

            rows.push_back( columns );

            if( rows.size( ) == c_iteration_row_cache_limit )
            {
               query_finished = false;
               break;
            }
         }

         // NOTE: Put a dummy row at the end to stop iteration.
         if( query_finished )
            rows.push_back( vector< string >( ) );

         instance_accessor.row_cache( ) = rows;

         if( key_info == c_nul_key )
            fetch_instance_from_row_cache( instance, skip_after_fetch );

         if( query_finished )
         {
            delete instance_accessor.p_sql_dataset( );
            instance_accessor.p_sql_dataset( ) = 0;
         }
      }

      if( !found )
      {
         if( instance_accessor.p_sql_dataset( ) )
            delete instance_accessor.p_sql_dataset( );
         instance_accessor.p_sql_dataset( ) = 0;

         instance_accessor.set_is_in_iteration( false );
      }
   }

   return found;
}

bool perform_instance_iterate_next( class_base& instance )
{
   if( instance.get_is_in_op( ) )
      throw runtime_error( "cannot continue iteration whilst currently perfoming an instance operation" );

   if( !instance.get_is_iterating( ) )
      return false;

   const string& class_id( instance.class_id( ) );
   const string& module_name( instance.module_name( ) );

   class_base_accessor instance_accessor( instance );
   storage_handler& handler( *gtp_session->p_storage_handler );

   bool found = false, cache_depleted = false;
   if( !instance_accessor.row_cache( ).empty( ) )
   {
      if( !instance_accessor.p_sql_dataset( ) && instance_accessor.row_cache( ).size( ) == 1 )
      {
         cache_depleted = true;
         instance_accessor.row_cache( ).clear( );
         instance_accessor.set_is_in_iteration( false );
      }
      else
      {
         found = true;

         bool skip_after_fetch = false;
         string skip_after_fetch_var( instance.get_variable( "@skip_after_fetch" ) );

         if( skip_after_fetch_var == "1" || skip_after_fetch_var == "true" )
            skip_after_fetch = true;

         fetch_instance_from_row_cache( instance, skip_after_fetch );
      }
   }

   if( found || cache_depleted )
      return found;
   else
      return perform_instance_iterate( instance, c_nul_key, "", "", "", "",
       instance.get_is_in_forwards_iteration( ) ? e_iter_direction_forwards : e_iter_direction_backwards, false, -1 );
}

void perform_instance_iterate_stop( class_base& instance )
{
   if( instance.get_is_iterating( ) )
   {
      class_base_accessor instance_accessor( instance );

      if( instance_accessor.p_sql_dataset( ) )
      {
         delete instance_accessor.p_sql_dataset( );
         instance_accessor.p_sql_dataset( ) = 0;
      }
   }
}


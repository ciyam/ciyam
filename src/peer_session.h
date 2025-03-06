// Copyright (c) 2014-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef PEER_SESSION_H
#  define PEER_SESSION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <memory>
#     include <vector>
#     include <string>
#  endif

#  include "config.h"
#  include "macros.h"
#  include "sockets.h"
#  include "threads.h"
#  ifdef SSL_SUPPORT
#     include "ssl_socket.h"
#  endif

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

enum peer_extra
{
   e_peer_extra_none,
   e_peer_extra_primary,
   e_peer_extra_secondary
};

enum peerchain_type
{
   e_peerchain_type_any,
   e_peerchain_type_hub,
   e_peerchain_type_data,
   e_peerchain_type_user,
   e_peerchain_type_backup,
   e_peerchain_type_shared
};

class CIYAM_BASE_DECL_SPEC peer_session : public thread
{
   public:
#  ifdef SSL_SUPPORT
   peer_session( int64_t time_val, bool is_responder,
    std::auto_ptr< ssl_socket >& ap_socket, const std::string& addr_info,
    bool is_for_support = false, peer_extra extra = e_peer_extra_none,
    const char* p_identity = 0, peerchain_type chain_type = e_peerchain_type_any,
    bool has_support_sessions = false, bool must_clear_system_variable = false );
#  else
   peer_session( int64_t time_val, bool is_responder,
    std::auto_ptr< tcp_socket >& ap_socket, const std::string& addr_info,
    bool is_for_support = false, peer_extra extra = e_peer_extra_none,
    const char* p_identity = 0, peerchain_type chain_type = e_peerchain_type_any,
    bool has_support_sessions = false, bool must_clear_system_variable = false );
#  endif

   ~peer_session( );

   void on_start( );

   void process_greeting( );

   void set_num_for_support( size_t new_num ) { num_for_support = new_num; }

   void set_backup_identity( const std::string& identity ) { backup_identity = identity; }

   void set_is_combined_backup( bool new_val ) { is_combined_backup = new_val; }

   static void increment_session_count( );
   static void decrement_session_count( );

   private:
   bool is_hub;
   bool is_data;
   bool is_user;

   bool is_local;
   bool is_owner;

   bool using_tls;

   bool is_responder;
   bool is_for_support;

   bool other_is_owner;
   bool both_are_owners;
   bool is_combined_backup;
   bool needs_key_exchange;
   bool has_support_sessions;
   bool must_clear_system_variable;

   int64_t time_val;

   size_t num_for_support;

   std::string port;
   std::string ip_addr;
   std::string identity;

   std::string blockchain;
   std::string hub_identity;
   std::string backup_identity;

   std::string public_ext;
   std::string public_loc;

   std::string secret_key;
   std::string secret_hash;

   std::string session_secret;

   std::string session_id_owner;

#  ifdef SSL_SUPPORT
   std::auto_ptr< ssl_socket > ap_socket;
#  else
   std::auto_ptr< tcp_socket > ap_socket;
#  endif
};

class CIYAM_BASE_DECL_SPEC peer_listener : public thread
{
   public:
   peer_listener( int port ) : port( port ) { }
   peer_listener( int port, const std::string& blockchains ) : port( port ), blockchains( blockchains ) { }

   void on_start( );

   private:
   int port;

   std::string blockchains;
};

class CIYAM_BASE_DECL_SPEC peer_session_starter : public thread
{
   public:
   peer_session_starter( );
   ~peer_session_starter( );

   void on_start( );

   void start_peer_session( const std::string& peer_info );
};

struct other_session_extras
{
   other_session_extras( size_t num = 0, const std::string* p_backup_identity = 0 )
   {
      num_for_support = num;

      if( p_backup_identity )
         backup_identity = *p_backup_identity;

      is_combined_backup = false;
   }

   void set_extras( peer_session& s )
   {
      if( num_for_support )
         s.set_num_for_support( num_for_support );

      if( !backup_identity.empty( ) )
         s.set_backup_identity( backup_identity );

      s.set_is_combined_backup( is_combined_backup );
   }

   size_t num_for_support;
   std::string backup_identity;

   bool is_combined_backup;
};

class mutex;

mutex& CIYAM_BASE_DECL_SPEC get_mutex_for_peer_session( );

std::string CIYAM_BASE_DECL_SPEC peer_channel_height(
 const std::string& identity, bool minimal = false, bool reversed = false, size_t minimum_height = 0 );

std::string CIYAM_BASE_DECL_SPEC prefixed_blockchains( const std::string& blockchains );
std::string CIYAM_BASE_DECL_SPEC unprefixed_blockchains( const std::string& blockchains );

void CIYAM_BASE_DECL_SPEC create_peer_listener( int port, const std::string& blockchains );

struct other_session_extras;

peer_session* CIYAM_BASE_DECL_SPEC create_peer_initiator( const std::string& blockchain,
 const std::string& host_and_or_port, bool force = false, size_t num_for_support = 0,
 bool is_interactive = true, bool is_secondary = false, bool has_main_session = false,
 peerchain_type chain_type = e_peerchain_type_any, bool has_set_system_variable = false,
 const std::string* p_extra_value = 0, other_session_extras* p_other_session_extras = 0 );

void CIYAM_BASE_DECL_SPEC create_initial_peer_sessions( );

extern "C" void CIYAM_BASE_DECL_SPEC init_peer_sessions( int start_listeners );

typedef void ( *fp_init_peer_sessions )( int );

#endif

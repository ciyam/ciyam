// Copyright (c) 2014-2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CORE_FILES_H
#  define CIYAM_CORE_FILES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#     include <vector>
#  endif

#  include "ptypes.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

struct new_block_info
{
   enum target_range
   {
      e_target_range_optimal = 1,
      e_target_range_sub_optimal = 2,
      e_target_range_less_optimal = 4
   };

   new_block_info( )
    :
    num_txs( 0 ),
    weight( 0 ),
    range( e_target_range_less_optimal ),
    height( 0 ),
    total_weight( 0 ),
    previous_block_weight( 0 )
   {
   }

   std::string acct;

   int num_txs;

   uint64_t weight;
   target_range range;

   uint64_t height;

   uint64_t total_weight;
   uint64_t previous_block_weight;
};

struct blockchain_info
{
   std::string chain_id;
   std::vector< std::string > checkpoint_info;
   std::vector< std::string > blob_hashes_with_sigs;
};

struct checkpoint_info
{
   std::string chain_id;
   std::string checkpoint_hash;
   std::vector< std::string > blob_hashes_with_sigs;
};

struct transactions_info
{
   std::string chain_id;
   std::vector< std::string > transaction_hashes_with_sigs;
};

struct account_key_info
{
   std::string msg_pubkey;
   std::string msg_secret;

   std::string block_hash;
   std::string block_lock;
   std::string block_secret;

   std::string trans_hash;
   std::string trans_lock;
   std::string trans_secret;
};

class trace_mutex;

void CIYAM_BASE_DECL_SPEC list_mutex_lock_ids_for_ciyam_core_files( std::ostream& outs );

trace_mutex CIYAM_BASE_DECL_SPEC& get_core_files_trace_mutex( );

void CIYAM_BASE_DECL_SPEC verify_core_file( const std::string& content,
 bool check_sigs = true, std::vector< std::pair< std::string, std::string > >* p_extras = 0 );

bool CIYAM_BASE_DECL_SPEC is_block( const std::string& core_type );
bool CIYAM_BASE_DECL_SPEC is_checkpoint( const std::string& core_type );
bool CIYAM_BASE_DECL_SPEC is_transaction( const std::string& core_type );
bool CIYAM_BASE_DECL_SPEC is_blockchain_info( const std::string& core_type );
bool CIYAM_BASE_DECL_SPEC is_checkpoint_info( const std::string& core_type );
bool CIYAM_BASE_DECL_SPEC is_checkpoint_blocks( const std::string& core_type );
bool CIYAM_BASE_DECL_SPEC is_checkpoint_transactions( const std::string& core_type );

void CIYAM_BASE_DECL_SPEC get_blockchain_info( const std::string& content, blockchain_info& bc_info );

void CIYAM_BASE_DECL_SPEC get_checkpoint_info(
 const std::string& blockchain, const std::string& conbtent, checkpoint_info& cp_info );

bool CIYAM_BASE_DECL_SPEC has_better_block(
 const std::string& blockchain, uint64_t height, uint64_t weight, std::string* p_acct = 0 );

std::string CIYAM_BASE_DECL_SPEC check_account( const std::string& blockchain, const std::string& password );

std::string CIYAM_BASE_DECL_SPEC construct_new_block(
 const std::string& blockchain, const std::string& password,
 const std::string& account, bool use_core_file_format = true,
 new_block_info* p_new_block_info = 0, bool search_for_proof_of_work_nonce = false );

inline std::string CIYAM_BASE_DECL_SPEC construct_new_block(
 const std::string& blockchain, const std::string& password,
 new_block_info* p_new_block_info = 0, bool search_for_proof_of_work_nonce = false )
{
   return construct_new_block( blockchain,
    password, "", true, p_new_block_info, search_for_proof_of_work_nonce );
}

std::string CIYAM_BASE_DECL_SPEC construct_new_transaction(
 const std::string& blockchain, const std::string& password,
 const std::string& account, const std::string& application,
 const std::string& transaction_log_lines, bool use_core_file_format = true,
 std::string* p_tx_hash = 0, const std::vector< std::string >* p_file_info = 0 );

std::string CIYAM_BASE_DECL_SPEC construct_blob_for_block_content( const std::string& block_content );

std::string CIYAM_BASE_DECL_SPEC construct_blob_for_block_content(
 const std::string& block_content, const std::string& block_signature );

std::string CIYAM_BASE_DECL_SPEC construct_blob_for_transaction_content(
 const std::string& transaction_content, const std::string& transaction_signature );

std::string CIYAM_BASE_DECL_SPEC construct_account_info(
 const std::string& blockchain, const std::string& password,
 unsigned int exp, const std::string& account, account_key_info* p_key_info = 0,
 uint64_t* p_balance = 0, uint64_t* p_num_transactions = 0, std::string* p_last_transaction_id = 0 );

inline std::string CIYAM_BASE_DECL_SPEC construct_account_info(
 const std::string& blockchain, const std::string& password, unsigned int exp )
{
   std::string account;

   return construct_account_info( blockchain, password, exp, account );
}

std::string CIYAM_BASE_DECL_SPEC get_account_msg_secret(
 const std::string& blockchain, const std::string& password, const std::string& account );

std::string CIYAM_BASE_DECL_SPEC create_peer_repository_entry_pull_info( const std::string& hash, bool store_as_blob = true );

std::string CIYAM_BASE_DECL_SPEC create_peer_repository_entry_pull_info(
 const std::string& hash, const std::string& local_hash,
 const std::string& local_public_key, const std::string& master_public_key, bool store_as_blob = true );

std::string CIYAM_BASE_DECL_SPEC create_peer_repository_entry_push_info( const std::string& file_hash,
 const std::string& password, std::string* p_pub_key = 0, bool store_as_blob = true, bool is_for_testing = false );

void CIYAM_BASE_DECL_SPEC decrypt_pulled_peer_file(
 const std::string& dest_hash, const std::string& src_hash, const std::string& password, bool is_for_testing = false );

void CIYAM_BASE_DECL_SPEC decrypt_pulled_peer_file(
 const std::string& dest_hash, const std::string& src_hash,
 const std::string& password, const std::string& public_key_in_hex, bool is_for_testing = false );

void CIYAM_BASE_DECL_SPEC perform_storage_rewind( const std::string& blockchain, uint64_t block_height );

std::string CIYAM_BASE_DECL_SPEC construct_blockchain_info_file( const std::string& blockchain );

uint64_t CIYAM_BASE_DECL_SPEC construct_transaction_scripts_for_blockchain(
 const std::string& blockchain, const std::string& tx_hash, std::vector< std::string >& applications );

#endif


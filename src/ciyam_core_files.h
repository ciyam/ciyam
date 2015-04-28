// Copyright (c) 2014-2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CORE_FILES_H
#  define CIYAM_CORE_FILES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <vector>
#  endif

#  include "ptypes.h"

#  ifdef CIYAM_BASE_IMPL
#     define CLASS_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CLASS_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

struct new_block_info
{
   new_block_info( ) : can_mint( false ), weight( 0 ), is_optimal( false ), height( 0 ) { }

   bool can_mint;

   uint64_t weight;
   bool is_optimal;

   unsigned long height;
};

struct blockchain_info
{
   std::string chain_id;
   std::vector< std::string > checkpoint_info;
   std::vector< std::string > block_hashes_with_sigs;
};

struct checkpoint_info
{
   std::string chain_id;
   std::string checkpoint_hash;
   std::vector< std::string > block_hashes_with_sigs;
};

struct account_key_info
{
   std::string block_hash;
   std::string block_lock;
   std::string block_secret;

   std::string trans_hash;
   std::string trans_lock;
   std::string trans_secret;
};

void CLASS_BASE_DECL_SPEC verify_core_file( const std::string& content,
 bool check_sigs = true, std::vector< std::pair< std::string, std::string > >* p_extras = 0 );

bool CLASS_BASE_DECL_SPEC is_block( const std::string& core_type );
bool CLASS_BASE_DECL_SPEC is_checkpoint( const std::string& core_type );
bool CLASS_BASE_DECL_SPEC is_blockchain_info( const std::string& core_type );
bool CLASS_BASE_DECL_SPEC is_checkpoint_info( const std::string& core_type );
bool CLASS_BASE_DECL_SPEC is_checkpoint_blocks( const std::string& core_type );
bool CLASS_BASE_DECL_SPEC is_checkpoint_transactions( const std::string& core_type );

void CLASS_BASE_DECL_SPEC get_blockchain_info( const std::string& content, blockchain_info& bc_info );

void CLASS_BASE_DECL_SPEC get_checkpoint_info(
 const std::string& blockchain, const std::string& conbtent, checkpoint_info& cp_info );

bool CLASS_BASE_DECL_SPEC has_better_block(
 const std::string& blockchain, unsigned long height, uint64_t weight, bool* p_has_worser = 0 );

std::string CLASS_BASE_DECL_SPEC construct_new_block(
 const std::string& blockchain, const std::string& password,
 const std::string& account, bool use_core_file_format = true, new_block_info* p_new_block_info = 0 );

inline std::string CLASS_BASE_DECL_SPEC construct_new_block(
 const std::string& blockchain, const std::string& password, new_block_info* p_new_block_info = 0 )
{
   return construct_new_block( blockchain, password, "", true, p_new_block_info );
}

std::string CLASS_BASE_DECL_SPEC construct_blob_for_block_content(
 const std::string& block_content, const std::string& block_signature );

std::string CLASS_BASE_DECL_SPEC construct_account_info(
 const std::string& blockchain, const std::string& password,
 unsigned int exp, const std::string& account, account_key_info* p_key_info = 0, uint64_t* p_balance = 0 );

std::string CLASS_BASE_DECL_SPEC construct_blockchain_info_file( const std::string& blockchain );

#endif


// Copyright (c) 2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CRYPTO_KEYS_H
#  define CRYPTO_KEYS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <vector>
#  endif

#  include "ptypes.h"
#  include "pointers.h"

class private_key;

class public_key
{
   friend class private_key;

   public:
   public_key( const std::string& encoded, bool use_base64 = false );

   ~public_key( );

   std::string get_public( bool compressed = true, bool use_base64 = false ) const;
   std::string get_hash160( bool compressed = true ) const;
   std::string get_address( bool compressed = true, bool is_testnet = false ) const;

   bool verify_signature( const std::string& msg, const std::string& sig ) const;
   bool verify_signature( const unsigned char* p_data, const std::string& sig ) const;

   static std::string address_to_hash160( const std::string& address );
   static std::string hash160_to_address( const std::string& hash160, bool is_testnet = false );

   private:
   public_key( const public_key& );
   public_key& operator =( const public_key& );

   protected:
   public_key( );

   struct impl;
   impl* p_impl;
};

class private_key : public public_key
{
   public:
   private_key( );
   private_key( const std::string& secret, bool is_wif_format = false );

   ~private_key( );

   std::string get_secret( bool use_base64 = false ) const;

   std::string get_wif_secret( bool compressed = true, bool is_testnet = false ) const;

   std::string decrypt_message( const public_key& pub,
    const std::string& base64, const char* p_id = 0 ) const;

   std::string encrypt_message( const public_key& pub,
    const std::string& message, const char* p_id = 0, bool add_salt = false ) const;

   std::string construct_shared( const public_key& pub ) const;

   std::string construct_signature( const unsigned char* p_data ) const;
   std::string construct_signature( const std::string& msg ) const;

   private:
   struct impl;
   impl* p_impl;

   private_key( const private_key& );
   private_key& operator =( const private_key& );
};

struct utxo_information
{
   utxo_information( unsigned int index,
    const std::string& reversed_txid, const char* p_script = 0, private_key* p_private_key = 0 )
    :
    index( index ),
    reversed_txid( reversed_txid ),
    original_script( p_script ? p_script : "" ),
    rp_private_key( p_private_key )
   {
   }

   unsigned int index;
   std::string reversed_txid;

   std::string original_script;
   ref_count_ptr< private_key > rp_private_key;
};

struct output_information
{
   output_information( uint64_t amount, const std::string& address )
    :
    amount( amount ),
    address( address )
   {
   }

   uint64_t amount;
   std::string address;
};

std::string construct_raw_transaction(
 const std::vector< utxo_information >& inputs,
 const std::vector< output_information >& outputs );

#endif


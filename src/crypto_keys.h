// // Copyright (c) 2014-2026 CIYAM Developers
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

enum address_prefix
{
   e_address_prefix_use_default = 0x00,
   e_address_prefix_btc_wif_mainnet = 0x80,
   e_address_prefix_btc_wif_testnet = 0xef,
   e_address_prefix_btc_p2sh_mainnet = 0x05,
   e_address_prefix_btc_p2sh_testnet = 0xc4,
   e_address_prefix_btc_p2pkh_mainnet = 0x00,
   e_address_prefix_btc_p2pkh_testnet = 0x6F
};

class public_key
{
   friend class private_key;

   public:
   public_key( const std::string& encoded, bool use_base64 = false );

   ~public_key( );

   std::string get_public( bool compressed = true, bool use_base64 = false ) const;
   std::string get_hash160( bool compressed = true ) const;

   std::string get_address( bool compressed = true,
    bool use_override = false, address_prefix override = e_address_prefix_btc_p2pkh_testnet ) const;

   bool verify_signature( const std::string& msg, const std::string& sig ) const;
   bool verify_signature( const unsigned char* p_data, const std::string& sig ) const;

   static std::string address_to_hash160( const std::string& address );

   static std::string hash160_to_address( const std::string& hash160,
    bool use_override = false, address_prefix override = e_address_prefix_btc_p2pkh_testnet );

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
   private_key( const std::string& secret, bool is_wif_format = false, bool* p_is_compressed = 0 );

   ~private_key( );

   void get_secret( std::string& s, bool use_base64 = false ) const;

   inline std::string get_secret( bool use_base64 = false ) const
   {
      std::string s;

      get_secret( s, use_base64 );

      return s;
   }

   std::string get_wif_secret( bool compressed = true,
    bool use_override = false, address_prefix override = e_address_prefix_btc_wif_testnet ) const;

   void decrypt_message( std::string& s,
    const public_key& pub, const std::string& base64, const char* p_id = 0 ) const;

   inline std::string decrypt_message(
    const public_key& pub, const std::string& base64, const char* p_id = 0 ) const
   {
      std::string s;

      decrypt_message( s, pub, base64, p_id );

      return s;
   }

   void encrypt_message( std::string& s, const public_key& pub,
    const std::string& message, const char* p_id = 0, bool add_salt = false ) const;

   inline std::string encrypt_message( const public_key& pub,
    const std::string& message, const char* p_id = 0, bool add_salt = false ) const
   {
      std::string s;

      encrypt_message( s, pub, message, p_id, add_salt );

      return s;
   }

   void construct_shared( std::string& s, const public_key& pub ) const;

   inline std::string construct_shared( const public_key& pub ) const
   {
      std::string s;
      construct_shared( s, pub );

      return s;
   }

   std::string construct_signature( const std::string& msg, bool use_base64 = false ) const;
   std::string construct_signature( const unsigned char* p_data, bool use_base64 = false ) const;

   private:
   struct impl;
   impl* p_impl;

   private_key( const private_key& );
   private_key& operator =( const private_key& );
};

std::string create_p2sh_address( const std::string& hex_script,
 bool use_override = false, address_prefix override = e_address_prefix_btc_p2sh_testnet );

std::string create_secret_for_address_prefix_with_leading_hash160_bytes( const std::string& prefix, const std::string& bytes );

void generate_secrets_for_leading_byte_encoded_message( const std::string& message, std::vector< std::string >& secrets );

std::string decode_message_from_leading_byte_encoded_addresses( const std::vector< std::string >& addresses );

struct utxo_information
{
   utxo_information( ) : index ( 0 ), is_p2sh_redeem( false ) { }

   utxo_information( unsigned int index, const std::string& reversed_txid,
    const char* p_script = 0, private_key* p_private_key = 0, bool is_p2sh_redeem = false )
    :
    index( index ),
    reversed_txid( reversed_txid ),
    original_script( p_script ? p_script : "" ),
    rp_private_key( p_private_key ),
    is_p2sh_redeem( is_p2sh_redeem )
   {
   }

   uint32_t index;
   std::string reversed_txid;

   std::string original_script;
   ref_count_ptr< private_key > rp_private_key;

   bool is_p2sh_redeem;
};

struct output_information
{
   output_information( ) : amount( 0 ) { }

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
 const std::vector< output_information >& outputs,
 bool* p_is_complete = 0, bool randomly_order_outputs = true, const char* p_message = 0,
 uint32_t lock_time = 0, std::vector< std::string >* p_extra_sig_script_items = 0 );

#endif

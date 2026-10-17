// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.
//
// (built on https://github.com/orlp/ed25519 - zlib licensed, its notice in "ed25519.cpp" and the "ed25519_*" files)

#ifndef ED25519_H
#  define ED25519_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <cstddef>
#     include <string>
#  endif

// NOTE: Ed25519 (RFC 8032) to sign and verify, and X25519 (RFC 7748) for a shared secret between two people from one key
// pair each - "ed25519_key_exchange( )" turns the other's Ed25519 public key into its X25519 form. A private key here is the
// 64 bytes "orlp" keeps: the clamped scalar, then the second half of the seed's SHA-512.

const size_t c_ed25519_seed_size = 32;
const size_t c_ed25519_public_key_size = 32;
const size_t c_ed25519_private_key_size = 64;
const size_t c_ed25519_signature_size = 64;
const size_t c_ed25519_shared_secret_size = 32;

void ed25519_create_seed( unsigned char* p_seed );

void ed25519_create_keypair( unsigned char* p_public_key, unsigned char* p_private_key, const unsigned char* p_seed );

// NOTE: The public key must be the one made with this private key - signing with any other gives away the private key
// (from two signatures of the same message). "ed25519_key_pair" keeps the two together, so prefer it to calling this.
void ed25519_sign( unsigned char* p_signature, const unsigned char* p_message,
 size_t message_len, const unsigned char* p_public_key, const unsigned char* p_private_key );

bool ed25519_verify( const unsigned char* p_signature,
 const unsigned char* p_message, size_t message_len, const unsigned char* p_public_key );

void ed25519_key_exchange( unsigned char* p_shared_secret, const unsigned char* p_public_key, const unsigned char* p_private_key );

// NOTE: A key pair, in hex - from a seed (64 hex digits) or a random one - for signing and for shared secrets. The private key
// is cleared when it goes.
class ed25519_key_pair
{
   public:
   ed25519_key_pair( );
   explicit ed25519_key_pair( const std::string& seed_hex );

   ~ed25519_key_pair( );

   std::string get_public( ) const;

   std::string sign( const std::string& message ) const;

   std::string construct_shared( const std::string& public_key_hex ) const;

   private:
   void init( const unsigned char* p_seed );

   unsigned char public_key[ c_ed25519_public_key_size ];
   unsigned char private_key[ c_ed25519_private_key_size ];

   ed25519_key_pair( const ed25519_key_pair& );
   ed25519_key_pair& operator =( const ed25519_key_pair& );
};

bool ed25519_verify( const std::string& message, const std::string& signature_hex, const std::string& public_key_hex );

#endif

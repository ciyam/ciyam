// Copyright (c) 2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CRYPTO_KEYS_H
#  define CRYPTO_KEYS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

class private_key;

class public_key
{
   friend class private_key;

   public:
   public_key( const std::string& base64_key );

   ~public_key( );

   std::string get_public( ) const;

   bool verify_signature( const std::string& msg, const std::string& sig ) const;

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
   private_key( const std::string& secret );

   ~private_key( );

   std::string get_secret( ) const;

   std::string decrypt_message( const public_key& pub,
    const std::string& base64, const char* p_id = 0 );

   std::string encrypt_message( const public_key& pub,
    const std::string& message, const char* p_id = 0, bool add_salt = false );

   std::string construct_shared( const public_key& pub );

   std::string construct_signature( const std::string& msg ) const;

   private:
   struct impl;
   impl* p_impl;

   private_key( const private_key& );
   private_key& operator =( const private_key& );
};

#endif


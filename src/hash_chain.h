// Copyright (c) 2014-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef HASH_CHAIN_H
#  define HASH_CHAIN_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

class hash_chain
{
   public:
   hash_chain( const std::string& name, bool is_new, unsigned int size = 0, bool use_secret = true );
   ~hash_chain( );

   bool has_been_depleted( ) const;

   bool check_and_update_if_good( const std::string& new_hash, unsigned int check_limit = 1 );

   std::string get_next_hashes_to_publish( const std::string& password, unsigned int num_hashes = 1 );

   private:
   hash_chain( const hash_chain& );
   hash_chain& operator =( const hash_chain& );

   struct impl;
   friend struct impl;
   impl* p_impl;
};

bool check_if_valid_hash_pair( const std::string& current, const std::string& previous, bool base64 = false );

std::string generate_hash_chain( size_t length,
 bool base64 = false, const char* p_secret = 0, char separator = '\n', bool hind_only = false );

#endif

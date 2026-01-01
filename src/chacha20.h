// Copyright (c) 2023-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CHACHA20_H
#  define CHACHA20_H

#  include "ptypes.h"

struct chacha20_ctx
{
   size_t position;

   uint64_t counter;

   uint8_t key[ 32 ];
   uint8_t nonce[ 12 ];

   uint32_t state[ 16 ];
   uint32_t keystream[ 16 ];
};

void chacha20_init_ctx( struct chacha20_ctx* p_ctx,
 uint8_t key[ ], uint8_t nonce[ ], uint64_t counter );

void chacha20_xor_next( struct chacha20_ctx* p_ctx, uint8_t* p_bytes, size_t n_bytes );

#endif

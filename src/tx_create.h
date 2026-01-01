// Copyright (c) 2013-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef TX_CREATE_H
#  define TX_CREATE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#     include <vector>
#  endif

#  include "ptypes.h"
#  include "class_base.h"

std::string reverse_txid( const std::string& txid );

void parse_utxo_info( std::istream& is, std::vector< utxo_info >& utxos );

uint64_t get_utxos_balance_amt( const std::string& file_name );

std::string create_raw_transaction_command(
 const std::string& source_address, const std::string& destination_addresses,
 std::string& change_address, uint64_t amount, quote_style qs, uint64_t& fee,
 std::string& sign_tx_template, const std::string* p_file_name = 0 );

#endif


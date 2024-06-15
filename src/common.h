// Copyright (c) 2024 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef COMMON_H
#  define COMMON_H

enum quote_style
{
   e_quote_style_both_linux,
   e_quote_style_both_windows,
   e_quote_style_windows_linux
};

enum stream_cipher
{
   e_stream_cipher_bd_shift,
   e_stream_cipher_chacha20,
   e_stream_cipher_dbl_hash,
};

const int c_max_key_length = 75;
const int c_max_fields_per_index = 5;
const int c_max_indexes_per_class = 100;
const int c_max_string_length_limit = 5000000;

#endif

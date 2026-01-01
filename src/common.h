// Copyright (c) 2024-2026 CIYAM Developers
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
   e_stream_cipher_unknown,
   e_stream_cipher_chacha20,
   e_stream_cipher_dbl_hash,
};

#endif

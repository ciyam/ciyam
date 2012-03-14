// Copyright (c) 2012
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifndef CONFIG_H
#  define CONFIG_H

//#  define SSL_SUPPORT
#  define ZLIB_SUPPORT
#  define ICONV_SUPPORT

const int c_default_ciyam_port = 12345;

const char* const c_default_ciyam_host = "localhost";

const int c_protocol_major_version = 0;
const int c_protocol_minor_version = 1;

const char* const c_protocol_version = "0.1";

const size_t c_password_hash_rounds = 1000;

#endif


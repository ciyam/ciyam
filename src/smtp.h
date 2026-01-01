// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SMTP_H
#  define SMTP_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <string>
#  endif

#  include "ptypes.h"

class date_time;
struct progress;

enum smtp_auth_type
{
   e_smtp_auth_type_none,
   e_smtp_auth_type_plain,
   e_smtp_auth_type_login,
   e_smtp_auth_type_cram_md5
};

struct smtp_user_info
{
   smtp_user_info( )
    :
    use_ssl( false ),
    use_tls( false ),
    utc_offset( 0.0 ),
    p_dt( 0 ),
    p_tz_abbr( 0 ),
    p_charset( 0 ),
    auth_type( e_smtp_auth_type_none ),
    max_attachment_bytes( INT64_C( 0 ) )
   {
   }

   smtp_user_info( const std::string& address )
    :
    domain( "localhost" ),
    address( address ),
    use_ssl( false ),
    use_tls( false ),
    p_dt( 0 ),
    utc_offset( 0.0 ),
    p_tz_abbr( 0 ),
    p_charset( 0 ),
    auth_type( e_smtp_auth_type_none ),
    max_attachment_bytes( INT64_C( 0 ) )
   {
   }

   smtp_user_info( const std::string& address,
    const std::string& username, const std::string& password, const date_time* p_dt = 0,
    float utc_offset = 0.0, const std::string* p_tz_abbr = 0, const std::string* p_charset = 0,
    bool use_ssl = false, bool use_tls = false, smtp_auth_type auth_type = e_smtp_auth_type_login )
    :
    domain( "localhost" ),
    address( address ),
    username( username ),
    password( password ),
    use_ssl( use_ssl ),
    use_tls( use_tls ),
    p_dt( p_dt ),
    utc_offset( utc_offset ),
    p_tz_abbr( p_tz_abbr ),
    p_charset( p_charset ),
    auth_type( auth_type ),
    max_attachment_bytes( INT64_C( 0 ) )
   {
   }

   smtp_user_info( const std::string& domain, const std::string& address,
    const std::string& username, const std::string& password, const date_time* p_dt,
    float utc_offset = 0.0, const std::string* p_tz_abbr = 0, const std::string* p_charset = 0,
    bool use_ssl = false, bool use_tls = false, smtp_auth_type auth_type = e_smtp_auth_type_login )
    :
    domain( domain ),
    address( address ),
    username( username ),
    password( password ),
    use_ssl( use_ssl ),
    use_tls( use_tls ),
    utc_offset( utc_offset ),
    p_dt( p_dt ),
    p_tz_abbr( p_tz_abbr ),
    p_charset( p_charset ),
    auth_type( auth_type ),
    max_attachment_bytes( INT64_C( 0 ) )
   {
   }

   std::string domain;
   std::string address;
   std::string username;
   std::string password;

   bool use_ssl;
   bool use_tls;

   const date_time* p_dt;

   float utc_offset;
   const std::string* p_tz_abbr;

   const std::string* p_charset;

   smtp_auth_type auth_type;
   int64_t max_attachment_bytes;
};

std::string html_to_text( const std::string& html );

void send_smtp_message( const std::string& host,
 const smtp_user_info& user_info, const std::vector< std::string >& recipients, const std::string& subject );

void send_smtp_message( const std::string& host, const smtp_user_info& user_info,
 const std::vector< std::string >& recipients, const std::string& subject, const std::string& message );

void send_smtp_message( const std::string& host,
 const smtp_user_info& user_info, const std::vector< std::string >& recipients,
 const std::string& subject, const std::string& message, const std::vector< std::string >& file_names );

void send_smtp_message( const std::string& host,
 const smtp_user_info& user_info, const std::vector< std::string >& recipients,
 const std::string& subject, const std::string& message,
 const std::string& html, const std::vector< std::string >* p_extra_headers,
 const std::vector< std::string >* p_file_names, const std::vector< std::string >* p_image_names,
 const std::string* p_image_path_prefix = 0, progress* p_progress = 0, bool* p_had_timeout = 0 );

#endif

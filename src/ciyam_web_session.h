// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_WEB_SESSION_H
#  define CIYAM_WEB_SESSION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <string>
#  endif

struct api_cws_params
{
   api_cws_params( ) : is_json_output( false ) { }

   std::string access;
   std::string device;
   std::string passwd;
   std::string payload;
   std::string request;
   std::string session;

   bool is_json_output;
};

bool process_api_cws_request( const std::string& uri,
 const api_cws_params& cws_params, bool& use_none_response, std::string& response, std::string& error );

#endif

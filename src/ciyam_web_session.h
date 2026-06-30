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

enum http_request_type
{
   e_http_request_type_get,
   e_http_request_type_put,
   e_http_request_type_head,
   e_http_request_type_post,
   e_http_request_type_trace,
   e_http_request_type_delete,
   e_http_request_type_options,
   e_http_request_type_unknown
};

struct cws_paramaters
{
   cws_paramaters( ) : is_json_output( false ) { }

   std::string access;
   std::string device;
   std::string passwd;
   std::string options;
   std::string payload;
   std::string request;
   std::string session;

   bool is_json_output;
};

bool process_cws_request( http_request_type request_type, const std::string& uri_suffix,
 const cws_paramaters& cws_params, bool& use_none_response, std::string& response, std::string& error );

#endif

// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef MIME_H
#  define MIME_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

struct mime_part
{
   std::string id;
   std::string type;
   std::string subtype;
   std::string encoding;

   std::string attribute;

   std::string data;
};

class mime_decoder
{
   public:
   mime_decoder( const std::string& mime_data );
   ~mime_decoder( );

   std::string get_type( ) const;
   std::string get_subtype( ) const;
   std::string get_encoding( ) const;
   std::string get_attribute( ) const;
   std::string get_text_data( ) const;

   bool has_child( );
   mime_decoder& get_child( );

   size_t num_parts( ) const;

   mime_part& get_part( size_t num ) const;

   private:
   mime_decoder( bool is_child, const std::string& mime_data );

   struct impl;
   friend struct impl;

   impl* p_impl;
};

class mime_encoder
{
   public:
   mime_encoder( const char* p_multi_part_subtype = 0,
    const char* p_multi_part_extra_subtype = 0, int max_chars_per_line = 0 );

   ~mime_encoder( );

   void add_text( const std::string& text,
    const char* p_charset = 0, const char* p_encoding = 0, bool is_for_smtp = true );

   void add_html( const std::string& html,
    const char* p_charset = 0, const char* p_encoding = 0, bool is_for_smtp = true );

   void add_file( const std::string& file_name, const char* p_encoding = 0 );

   void add_image( const std::string& file_name, const char* p_path_prefix = 0, const char* p_encoding = 0 );

   void create_child( const char* p_multi_part_subtype = 0, const char* p_multi_part_extra_subtype = 0 );

   mime_encoder& get_child( );

   std::string get_data( );

   private:
   struct impl;
   friend struct impl;

   impl* p_impl;
};

#endif


// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef PDF_H
#  define PDF_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

class pdf_page;
class pdf_chinese_font;
class pdf_standard_font;

enum other_language
{
   e_other_language_chinese
};

enum page_mode
{
   e_page_mode_none,
   e_page_mode_use_outline,
   e_page_mode_use_thumbs,
   e_page_mode_full_screen
};

enum page_size
{
   e_page_size_not_applicable,
   e_page_size_A3,
   e_page_size_A4,
   e_page_size_A5,
   e_page_size_B4,
   e_page_size_B5
};

enum page_layout
{
   e_page_layout_single,
   e_page_layout_one_column,
   e_page_layout_two_column_left,
   e_page_layout_two_column_right
};

enum page_direction
{
   e_page_direction_portrait,
   e_page_direction_landscape
};

enum line_cap
{
   e_line_cap_butt_end,
   e_line_cap_round_end,
   e_line_cap_projecting_end
};

enum text_align
{
   e_text_align_left,
   e_text_align_right,
   e_text_align_center,
   e_text_align_justify
};

const int c_compress_flag_none = 0;
const int c_compress_flag_text = 1;
const int c_compress_flag_image = 2;
const int c_compress_flag_metadata = 4;

const int c_use_full_compression = c_compress_flag_text | c_compress_flag_image | c_compress_flag_metadata;

struct pdf_ref_helper;

class pdf_doc
{
   friend class pdf_page;
   friend class pdf_chinese_font;
   friend class pdf_standard_font;
   friend class pdf_true_type_font;

   friend struct pdf_ref_helper;

   public:
   pdf_doc( );
   virtual ~pdf_doc( );

   void save_to_file( const std::string& file_name );

   void set_page_mode( page_mode pmode );

   void set_page_layout( page_layout playout );

   void set_compression( int compress_flags = c_use_full_compression );

   private:
   struct impl;
   impl* p_impl;

   void use_chinese_fonts( );

   std::string load_true_type_font( const std::string& file_name );

   pdf_doc( const pdf_doc& );
   pdf_doc& operator =( const pdf_doc& );
};

struct pdf_font
{
   pdf_font( );
   virtual ~pdf_font( );

   virtual void init( ) { }

   std::string get_name( ) const;
   std::string get_encoding( ) const;

   int get_ascent( );
   int get_descent( );
   int get_x_height( );
   int get_cap_height( );

   int get_text_width( const std::string& text );

   bool get_uses_wide_chars( ) const;

   struct impl;
   impl* p_impl;

   private:
   pdf_font( const pdf_font& );
   pdf_font& operator =( const pdf_font& );
};

class pdf_standard_font : public pdf_font
{
   public:
   pdf_standard_font( pdf_doc& doc, const std::string& family, const char* p_encoding = 0 );

   private:
   pdf_doc& doc;
};

class pdf_chinese_font : public pdf_font
{
   public:
   pdf_chinese_font( pdf_doc& doc, const std::string& family, const std::string& encoding );

   private:
   pdf_doc& doc;
};

class pdf_true_type_font : public pdf_font
{
   public:
   pdf_true_type_font( pdf_doc& doc, const std::string& file_name, const std::string& encoding );

   private:
   pdf_doc& doc;
};

struct pdf_image_ref_helper;

struct pdf_image
{
   friend struct pdf_image_ref_helper;

   pdf_image( );
   virtual ~pdf_image( );

   int get_width( );
   int get_height( );

   struct impl;
   impl* p_impl;

   private:
   pdf_image( const pdf_image& );
   pdf_image& operator =( const pdf_image& );
};

class pdf_png_image : public pdf_image
{
   public:
   pdf_png_image( pdf_doc& doc, const std::string& file_name );

   private:
   pdf_doc& doc;
};

class pdf_jpeg_image : public pdf_image
{
   public:
   pdf_jpeg_image( pdf_doc& doc, const std::string& file_name );

   private:
   pdf_doc& doc;
};

class pdf_page
{
   friend struct pdf_font;

   public:
   pdf_page( pdf_doc& doc, float width, float height );
   pdf_page( pdf_doc& doc, page_size ps, page_direction pd = e_page_direction_portrait );

   virtual ~pdf_page( );

   void set_to_fit( );

   void set_font_and_size( pdf_font& font, float size );

   void output_text( const std::string& text, float x, float y );

   void output_text( const std::string& text, float top, float left,
    float bottom, float right, text_align alignment = e_text_align_left, unsigned int* p_len = 0 );

   void draw_image( pdf_image& image, float x, float y );
   void draw_image( pdf_image& image, float x, float y, int width, int height );

   void set_grey_fill( float fill );
   void set_grey_stroke( float stroke );
   void set_grey_fill_and_stroke( float fill, float stroke );

   void set_rgb_fill( float red, float green, float blue );
   void set_rgb_stroke( float red, float green, float blue );

   void set_line_cap( line_cap cap );
   void set_line_width( float width );

   void move_to( float x, float y );
   void line_to( float x, float y );

   void rectangle( float x, float y, float width, float height );

   void fill( );
   void stroke( );

   void fill_and_stroke( );

   void clip( );

   void save_state( );
   void restore_state( );

   float get_width( );
   float get_height( );
   float get_line_width( );

   float text_width( const std::string& text );
   int measure_text( const std::string& text, float width, bool wordwrap, float* p_actual_width = 0 );

   private:
   pdf_doc& doc;

   struct impl;
   impl* p_impl;

   pdf_page( const pdf_page& );
   pdf_page& operator =( const pdf_page& );
};

void pdf_print_grid( pdf_doc& doc, pdf_page& page, bool reverse_vertical = false );

#endif


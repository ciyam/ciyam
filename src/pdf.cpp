// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <string>
#  include <iostream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  include <hpdf.h>
#  include <hpdf_version.h>
#else
#  include <haru/hpdf.h>
#  include <haru/hpdf_version.h>
#endif

#include "pdf.h"

#include "utilities.h"

using namespace std;

//#define COMPILE_TESTBED_MAIN

void pdf_error_handler( HPDF_STATUS error_no, HPDF_STATUS detail_no, void* user_data )
{
   throw runtime_error(
    "PDF error_no = " + to_string( error_no ) + ", detail_no = " + to_string( detail_no ) );
}

struct pdf_doc::impl
{
   impl( )
    :
    using_chinese_fonts( false )
   {
      doc = HPDF_New( pdf_error_handler, 0 );
      if( !doc )
         throw runtime_error( "cannot create PDFDoc object" );
   }

   ~impl( )
   {
      HPDF_Free( doc );
   }

   HPDF_Doc doc;
   bool using_chinese_fonts;
   map< string, string > true_type_fonts;
};

pdf_doc::pdf_doc( )
{
   p_impl = new impl;

}

pdf_doc::~pdf_doc( )
{
   delete p_impl;
}

void pdf_doc::save_to_file( const string& file_name )
{
   HPDF_SaveToFile( p_impl->doc, file_name.c_str( ) );
}

void pdf_doc::set_page_mode( page_mode pmode )
{
   HPDF_PageMode mode = HPDF_PAGE_MODE_USE_NONE;

   if( pmode == e_page_mode_use_outline )
      mode = HPDF_PAGE_MODE_USE_OUTLINE;
   else if( pmode == e_page_mode_use_thumbs )
      mode = HPDF_PAGE_MODE_USE_THUMBS;
   else if( pmode == e_page_mode_full_screen )
      mode = HPDF_PAGE_MODE_FULL_SCREEN;

   HPDF_SetPageMode( p_impl->doc, mode );
}

void pdf_doc::set_page_layout( page_layout playout )
{
   HPDF_PageLayout layout;

   if( playout == e_page_layout_single )
      layout = HPDF_PAGE_LAYOUT_SINGLE;
   else if( playout == e_page_layout_one_column )
      layout = HPDF_PAGE_LAYOUT_ONE_COLUMN;
   else if( playout == e_page_layout_two_column_left )
      layout = HPDF_PAGE_LAYOUT_TWO_COLUMN_LEFT;
   else if( playout == e_page_layout_two_column_right )
      layout = HPDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT;

   HPDF_SetPageLayout( p_impl->doc, layout );
}

void pdf_doc::set_compression( int compress_flags )
{
   HPDF_UINT mode = HPDF_COMP_NONE;

   if( compress_flags & c_compress_flag_text )
      mode |= HPDF_COMP_TEXT;

   if( compress_flags & c_compress_flag_image )
      mode |= HPDF_COMP_IMAGE;

   if( compress_flags & c_compress_flag_metadata )
      mode |= HPDF_COMP_METADATA;

   HPDF_SetCompressionMode( p_impl->doc, mode );
}

void pdf_doc::use_chinese_fonts( )
{
   if( !p_impl->using_chinese_fonts )
   {
      HPDF_UseCNSFonts( p_impl->doc );
      HPDF_UseCNSEncodings( p_impl->doc );

      p_impl->using_chinese_fonts = true;
   }
}

string pdf_doc::load_true_type_font( const string& file_name )
{
   if( p_impl->true_type_fonts.count( file_name ) )
      return p_impl->true_type_fonts[ file_name ];
   else
   {
      string name = HPDF_LoadTTFontFromFile( p_impl->doc, file_name.c_str( ), HPDF_TRUE );
      p_impl->true_type_fonts.insert( make_pair( file_name, name ) );
      return name;
   }
}

struct pdf_ref_helper
{
   static HPDF_Doc& get_ref( pdf_doc& doc )
   {
      return doc.p_impl->doc;
   }
};

struct pdf_font::impl
{
   impl( ) : uses_wide_chars ( false ) { }

   string name;
   HPDF_Font font;
   bool uses_wide_chars;
};

pdf_font::pdf_font( )
{
   p_impl = new impl;
}

pdf_font::~pdf_font( )
{
   delete p_impl;
}

string pdf_font::get_name( ) const
{
   return p_impl->name;
}

string pdf_font::get_encoding( ) const
{
   string encoding;
   const char* p_encoding = HPDF_Font_GetEncodingName( p_impl->font );

   if( p_encoding )
      encoding = string( p_encoding );

   return encoding;
}

int pdf_font::get_ascent( )
{
   return HPDF_Font_GetAscent( p_impl->font );
}

int pdf_font::get_descent( )
{
   return HPDF_Font_GetDescent( p_impl->font );
}

int pdf_font::get_x_height( )
{
   return HPDF_Font_GetXHeight( p_impl->font );
}

int pdf_font::get_cap_height( )
{
   return HPDF_Font_GetCapHeight( p_impl->font );
}

int pdf_font::get_text_width( const string& text )
{
   return HPDF_Font_TextWidth( p_impl->font, ( const HPDF_BYTE* )text.c_str( ), text.length( ) ).width;
}

bool pdf_font::get_uses_wide_chars( ) const
{
   return p_impl->uses_wide_chars;
}

pdf_standard_font::pdf_standard_font( pdf_doc& doc, const string& family, const char* p_encoding )
 :
 doc( doc )
{
   p_impl->name = family;
   p_impl->font = HPDF_GetFont( pdf_ref_helper::get_ref( doc ), family.c_str( ), p_encoding );
}

pdf_chinese_font::pdf_chinese_font( pdf_doc& doc, const string& family, const string& encoding )
 :
 doc( doc )
{
   doc.use_chinese_fonts( );

   p_impl->name = family;
   p_impl->font = HPDF_GetFont( pdf_ref_helper::get_ref( doc ), family.c_str( ), encoding.c_str( ) );

   p_impl->uses_wide_chars = true;
}

pdf_true_type_font::pdf_true_type_font( pdf_doc& doc, const string& file_name, const string& encoding )
 :
 doc( doc )
{
   p_impl->name = doc.load_true_type_font( file_name.c_str( ) );

   p_impl->font = HPDF_GetFont( pdf_ref_helper::get_ref( doc ), p_impl->name.c_str( ), encoding.c_str( ) );
}

struct pdf_image::impl
{
   HPDF_Image image;
};

struct pdf_image_ref_helper
{
   static HPDF_Image& get_ref( pdf_image& image )
   {
      return image.p_impl->image;
   }
};

pdf_image::pdf_image( )
{
   p_impl = new impl;
}

pdf_image::~pdf_image( )
{
   delete p_impl;
}

int pdf_image::get_width( )
{
   return HPDF_Image_GetWidth( p_impl->image );
}

int pdf_image::get_height( )
{
   return HPDF_Image_GetHeight( p_impl->image );
}

pdf_png_image::pdf_png_image( pdf_doc& doc, const string& file_name )
 :
 doc( doc )
{
   p_impl->image = HPDF_LoadPngImageFromFile( pdf_ref_helper::get_ref( doc ), file_name.c_str( ) );
}

pdf_jpeg_image::pdf_jpeg_image( pdf_doc& doc, const string& file_name )
 :
 doc( doc )
{
   p_impl->image = HPDF_LoadJpegImageFromFile( pdf_ref_helper::get_ref( doc ), file_name.c_str( ) );
}

struct pdf_page::impl
{
   HPDF_Page page;
};

pdf_page::pdf_page( pdf_doc& doc, float width, float height )
 :
 doc( doc )
{
   p_impl = new impl;
   p_impl->page = HPDF_AddPage( pdf_ref_helper::get_ref( doc ) );

   HPDF_Page_SetWidth( p_impl->page, width );
   HPDF_Page_SetHeight( p_impl->page, height );
}

pdf_page::pdf_page( pdf_doc& doc, page_size ps, page_direction pd )
 :
 doc( doc )
{
   p_impl = new impl;
   p_impl->page = HPDF_AddPage( pdf_ref_helper::get_ref( doc ) );

   HPDF_PageSizes size;

   switch( ps )
   {
      case e_page_size_A3:
      size = HPDF_PAGE_SIZE_A3;
      break;

      case e_page_size_A4:
      size = HPDF_PAGE_SIZE_A4;
      break;

      case e_page_size_A5:
      size = HPDF_PAGE_SIZE_A5;
      break;

      case e_page_size_B4:
      size = HPDF_PAGE_SIZE_B4;
      break;

      case e_page_size_B5:
      size = HPDF_PAGE_SIZE_B5;
      break;

      default:
      throw runtime_error( "unknown page_size value #" + to_string( ps ) );
   }

   HPDF_PageDirection direction;

   switch( pd )
   {
      case e_page_direction_portrait:
      direction = HPDF_PAGE_PORTRAIT;
      break;

      case e_page_direction_landscape:
      direction = HPDF_PAGE_LANDSCAPE;
      break;

      default:
      throw runtime_error( "unknown page_direction value #" + to_string( pd ) );
   }

   HPDF_Page_SetSize( p_impl->page, size, direction );
}

pdf_page::~pdf_page( )
{
   delete p_impl;
}

void pdf_page::set_to_fit( )
{
   HPDF_Destination dst = HPDF_Page_CreateDestination( p_impl->page );

   HPDF_Destination_SetFit( dst );
   HPDF_SetOpenAction( pdf_ref_helper::get_ref( doc ), dst );
}

void pdf_page::set_font_and_size( pdf_font& font, float size )
{
   HPDF_Page_SetFontAndSize( p_impl->page, font.p_impl->font, size );
}

void pdf_page::output_text( const string& text, float x, float y )
{
   HPDF_Page_BeginText( p_impl->page );
   HPDF_Page_TextOut( p_impl->page, x, y, text.c_str( ) );
   HPDF_Page_EndText( p_impl->page );
}

void pdf_page::output_text( const string& text,
 float top, float left, float bottom, float right, text_align alignment, unsigned int* p_len )
{
   HPDF_TextAlignment align;

   switch( alignment )
   {
      case e_text_align_left:
      align = HPDF_TALIGN_LEFT;
      break;

      case e_text_align_right:
      align = HPDF_TALIGN_RIGHT;
      break;

      case e_text_align_center:
      align = HPDF_TALIGN_CENTER;
      break;

      case e_text_align_justify:
      align = HPDF_TALIGN_JUSTIFY;
      break;

      default:
      throw runtime_error( "unexpected alignment value '" + to_string( alignment ) + "' in output_text" );
   }

   HPDF_Page_BeginText( p_impl->page );
   HPDF_Page_TextRect( p_impl->page, left, top, right, bottom, text.c_str( ), align, p_len );
   HPDF_Page_EndText( p_impl->page );
}

void pdf_page::draw_image( pdf_image& image, float x, float y )
{
   HPDF_Page_DrawImage( p_impl->page,
    pdf_image_ref_helper::get_ref( image ), x, y, image.get_width( ), image.get_height( ) );
}

void pdf_page::draw_image( pdf_image& image, float x, float y, int width, int height )
{
   HPDF_Page_DrawImage( p_impl->page,
    pdf_image_ref_helper::get_ref( image ), x, y, width, height );
}

void pdf_page::set_grey_fill( float fill )
{
   HPDF_Page_SetGrayFill( p_impl->page, fill );
}

void pdf_page::set_grey_stroke( float stroke )
{
   HPDF_Page_SetGrayStroke( p_impl->page, stroke );
}

void pdf_page::set_grey_fill_and_stroke( float fill, float stroke )
{
   HPDF_Page_SetGrayFill( p_impl->page, fill );
   HPDF_Page_SetGrayStroke( p_impl->page, stroke );
}

void pdf_page::set_rgb_fill( float red, float green, float blue )
{
   HPDF_Page_SetRGBFill( p_impl->page, red, green, blue );
}

void pdf_page::set_rgb_stroke( float red, float green, float blue )
{
   HPDF_Page_SetRGBStroke( p_impl->page, red, green, blue );
}

void pdf_page::set_line_cap( line_cap cap )
{
   HPDF_LineCap hcap;

   switch( cap )
   {
      case e_line_cap_butt_end:
      hcap = HPDF_BUTT_END;
      break;

      case e_line_cap_round_end:
      hcap = HPDF_ROUND_END;
      break;

      case e_line_cap_projecting_end:
#if HPDF_BUGFIX_VERSION >= 2
      hcap = HPDF_PROJECTING_SQUARE_END;
#else
      hcap = HPDF_PROJECTING_SCUARE_END;
#endif
      break;

      default:
      throw runtime_error( "unexpected line_cap value #" + to_string( cap ) );
   }

   HPDF_Page_SetLineCap( p_impl->page, hcap );
}

void pdf_page::set_line_width( float width )
{
   HPDF_Page_SetLineWidth( p_impl->page, width );
}

void pdf_page::move_to( float x, float y )
{
   HPDF_Page_MoveTo( p_impl->page, x, y );
}

void pdf_page::line_to( float x, float y )
{
   HPDF_Page_LineTo( p_impl->page, x, y );
}

void pdf_page::rectangle( float x, float y, float width, float height )
{
   HPDF_Page_Rectangle( p_impl->page, x, y, width, height );
}

void pdf_page::fill( )
{
   HPDF_Page_Fill( p_impl->page );
}

void pdf_page::stroke( )
{
   HPDF_Page_Stroke( p_impl->page );
}

void pdf_page::fill_and_stroke( )
{
   HPDF_Page_FillStroke( p_impl->page );
}

void pdf_page::clip( )
{
   HPDF_Page_Clip( p_impl->page );
}

void pdf_page::save_state( )
{
   HPDF_Page_GSave( p_impl->page );
}

void pdf_page::restore_state( )
{
   HPDF_Page_GRestore( p_impl->page );
}

float pdf_page::get_width( )
{
   return HPDF_Page_GetWidth( p_impl->page );
}

float pdf_page::get_height( )
{
   return HPDF_Page_GetHeight( p_impl->page );
}

float pdf_page::get_line_width( )
{
   return HPDF_Page_GetLineWidth( p_impl->page );
}

float pdf_page::text_width( const string& text )
{
   return HPDF_Page_TextWidth( p_impl->page, text.c_str( ) );
}

int pdf_page::measure_text( const string& text, float width, bool wordwrap, float* p_actual_width )
{
   return HPDF_Page_MeasureText( p_impl->page, text.c_str( ), width, wordwrap, p_actual_width );
}

void pdf_print_grid( pdf_doc& doc, pdf_page& page, bool reverse_vertical )
{
   float width = page.get_width( );
   float height = page.get_height( );

   pdf_standard_font sfont( doc, "Helvetica" );

   page.set_font_and_size( sfont, 5 );
   page.set_grey_fill_and_stroke( 0.5f, 0.8f );

   float x, y;

   // NOTE: Draw horizontal lines...
   y = 0;
   while( y < height )
   {
      if( ( int )y % 10 == 0 )
         page.set_line_width( 0.5f );
      else if( page.get_line_width( ) != 0.25f )
         page.set_line_width( 0.25f );

      page.move_to( 0, reverse_vertical ? height - y : y );
      page.line_to( width, reverse_vertical ? height - y : y );
      page.stroke( );

      if( ( int )y % 10 == 0 && y > 0 )
      {
         page.set_grey_stroke( 0.5f );

         page.move_to( 0, reverse_vertical ? height - y : y );
         page.line_to( 5, reverse_vertical ? height - y : y );
         page.stroke( );

         page.set_grey_stroke( 0.8f );
      }

      y += 5;
   }

   // NOTE: Draw vertical lines...
   x = 0;
   while( x < width )
   {
      if( ( int )x % 10 == 0 )
         page.set_line_width( 0.5f );
      else if( page.get_line_width( ) != 0.25f )
         page.set_line_width( 0.25f );

      page.move_to( x, 0 );
      page.line_to( x, height );
      page.stroke( );

      if( ( int )x % 50 == 0 && x > 0 )
      {
         page.set_grey_stroke( 0.5f );

         page.move_to( x, 0 );
         page.line_to( x, 5 );
         page.stroke( );

         page.move_to( x, height );
         page.line_to( x, height - 5 );
         page.stroke( );

         page.set_grey_stroke( 0.8f );
      }

      x += 5;
   }

   // NOTE: Draw horizontal text...
   y = 0;
   while( y < height )
   {
      if( ( int )y % 10 == 0 && y > 0 )
         page.output_text( to_string( y ), 5, reverse_vertical ? height - y - 2 : y - 2 );

      y += 5;
   }

   // NOTE: Draw vertical text...
   x = 0;
   while( x < width )
   {
      if( ( int )x % 50 == 0 && x > 0 )
      {
         page.output_text( to_string( x ), x, 5 );
         page.output_text( to_string( x ), x, height - 10 );
      }

      x += 5;
   }

   page.set_grey_fill_and_stroke( 0, 0 );
}

#ifdef COMPILE_TESTBED_MAIN

const char* const c_ni_hao = "\xc4\xe3\xba\xc3"; // i.e. GB2312 (ni=C4E3 hao=BAC3)

int main( )
{
   try
   {
      pdf_doc doc;

      pdf_page page( doc, 300, 400 );

      pdf_print_grid( doc, page );

      pdf_standard_font sfont( doc, "Helvetica" );

      page.set_font_and_size( sfont, 12 );
      page.output_text( "hello", 50, 370 );

      pdf_chinese_font cfont( doc, "SimSun", "GB-EUC-H" );
      page.set_font_and_size( cfont, 14 );
      page.output_text( c_ni_hao, 50, 350 );

      doc.save_to_file( "test.pdf" );

      cout << "created 'test.pdf'" << endl;
   }
   catch( runtime_error& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception caught" << endl;
      return 2;
   }

   return 0;
}

#endif


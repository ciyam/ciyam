// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory.h>
#  include <cerrno>
#  include <map>
#  include <memory>
#  include <string>
#  include <vector>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

//#define DEBUG

#include "pdf_gen.h"

#include "pdf.h"
#include "sio.h"
#include "config.h"
#include "format.h"
#include "numeric.h"
#include "pointers.h"
#include "progress.h"
#include "date_time.h"
#include "utilities.h"

#ifdef ICONV_SUPPORT
#  include <iconv.h>
#endif

using namespace std;

enum pdf_gen_field_type
{
   e_pdf_gen_field_type_box,
   e_pdf_gen_field_type_line,
   e_pdf_gen_field_type_text,
   e_pdf_gen_field_type_image
};

enum pdf_gen_field_size
{
   e_pdf_gen_field_size_fixed,
   e_pdf_gen_field_size_growable,
   e_pdf_gen_field_size_shrinkable,
   e_pdf_gen_field_size_shrinkable_width,
   e_pdf_gen_field_size_shrinkable_height
};

enum pdf_gen_field_alignx
{
   e_pdf_gen_field_alignx_left,
   e_pdf_gen_field_alignx_right,
   e_pdf_gen_field_alignx_center
};

enum pdf_gen_field_aligny
{
   e_pdf_gen_field_aligny_top,
   e_pdf_gen_field_aligny_bottom,
   e_pdf_gen_field_aligny_center
};

enum pdf_gen_field_alignment
{
   e_pdf_gen_field_alignment_left,
   e_pdf_gen_field_alignment_right,
   e_pdf_gen_field_alignment_center,
   e_pdf_gen_field_alignment_justify
};

enum pdf_gen_field_data_type
{
   e_pdf_gen_field_data_type_char,
   e_pdf_gen_field_data_type_date,
   e_pdf_gen_field_data_type_time,
   e_pdf_gen_field_data_type_boolean,
   e_pdf_gen_field_data_type_integer,
   e_pdf_gen_field_data_type_numeric,
   e_pdf_gen_field_data_type_date_time
};

struct pdf_gen_font_info
{
   pdf_gen_font_info( )
    :
    ypos_adjust( 0.0 )
   {
   }

   string font_name;
   string font_encoding;

   float font_size;
   float ypos_adjust;
};

struct pdf_gen_group_info
{
   pdf_gen_group_info( )
    :
    x( 0 ),
    y( 0 ),
    cols( 1 ),
    rows( 1 ),
    border( 0 ),
    bcolor_r( 0 ),
    bcolor_g( 0 ),
    bcolor_b( 0 ),
    border_left( true ),
    border_right( true ),
    border_top( true ),
    border_bottom( true ),
    border_shadow( false ),
    bgcolor_r( -1 ),
    bgcolor_g( -1 ),
    bgcolor_b( -1 ),
    padding_left( 0 ),
    padding_right( 0 ),
    padding_top( 0 ),
    padding_bottom( 0 ),
    paddingx( 0 ),
    paddingy( 0 ),
    x_is_offset( false ),
    y_is_offset( false ),
    is_forced( false ),
    is_header( false ),
    has_repeats( false ),
    is_top_pinned( false ),
    has_child_group( false ),
    page_each_repeat( false ),
    starts_on_new_page( false ),
    link_as_block_only( false ),
    boundaries_can_grow( false ),
    rel_group_align_x( e_pdf_gen_field_alignx_left ),
    rel_group_align_y( e_pdf_gen_field_aligny_bottom )
   {
   }

   float x;
   float y;

   int cols;
   int rows;

   string id;

   string font;
   string perm;
   string rest;

   float border;

   int bcolor_r;
   int bcolor_g;
   int bcolor_b;

   bool border_left;
   bool border_right;
   bool border_top;
   bool border_bottom;

   bool border_shadow;

   int bgcolor_r;
   int bgcolor_g;
   int bgcolor_b;

   float padding_left;
   float padding_right;
   float padding_top;
   float padding_bottom;

   int paddingx;
   int paddingy;

   bool x_is_offset;
   bool y_is_offset;

   bool is_forced;
   bool is_header;
   bool has_repeats;
   bool is_top_pinned;

   string blink;
   string flink;

   string dep_group;
   string par_group;
   string rel_group;

   bool has_child_group;

   bool page_each_repeat;
   bool starts_on_new_page;

   bool link_as_block_only;
   bool boundaries_can_grow;

   pdf_gen_field_alignx rel_group_align_x;
   pdf_gen_field_aligny rel_group_align_y;
};

struct pdf_gen_field_info
{
   pdf_gen_field_info( )
    :
    x( 0 ),
    y( 0 ),
    color_r( 0 ),
    color_g( 0 ),
    color_b( 0 ),
    had_color( false ),
    border( 0 ),
    bcolor_r( 0 ),
    bcolor_g( 0 ),
    bcolor_b( 0 ),
    border_left( true ),
    border_right( true ),
    border_top( true ),
    border_bottom( true ),
    border_shadow( false ),
    bgcolor_r( -1 ),
    bgcolor_g( -1 ),
    bgcolor_b( -1 ),
    is_png( false ),
    is_jpg( false ),
    truncate( -1 ),
    use_padding( false ),
    x_is_offset( false ),
    y_is_offset( false ),
    padding_left( 0 ),
    padding_right( 0 ),
    padding_top( 0 ),
    padding_bottom( 0 ),
    line_spacing( 0.0 ),
    is_top_pinned( false ),
    has_line_spacing( false ),
    is_external_data( false ),
    is_indirect_data( false ),
    blank_zero_numeric( false ),
    line_with_round_ends( false ),
    fill_and_square_top( false ),
    fill_and_square_bottom( false ),
    last_repeat_num( -1 ),
    use_for_totalling( false ),
    use_for_group_overflow( false ),
    type( e_pdf_gen_field_type_text ),
    size( e_pdf_gen_field_size_fixed ),
    alignx( e_pdf_gen_field_alignx_left ),
    aligny( e_pdf_gen_field_aligny_top ),
    alignment( e_pdf_gen_field_alignment_left ),
    data_type( e_pdf_gen_field_data_type_char ),
    rel_group_align_x( e_pdf_gen_field_alignx_left ),
    rel_group_align_y( e_pdf_gen_field_aligny_bottom )
   {
   }

   float x;
   float y;

   int width;
   int height;

   int color_r;
   int color_g;
   int color_b;

   bool had_color;

   float border;

   int bcolor_r;
   int bcolor_g;
   int bcolor_b;

   bool border_left;
   bool border_right;
   bool border_top;
   bool border_bottom;

   bool border_shadow;

   int bgcolor_r;
   int bgcolor_g;
   int bgcolor_b;

   bool is_png;
   bool is_jpg;

   int truncate;

   bool use_padding;
   float padding_left;
   float padding_right;
   float padding_top;
   float padding_bottom;

   bool is_top_pinned;

   float line_spacing;
   bool has_line_spacing;

   bool is_external_data;
   bool is_indirect_data;

   bool blank_zero_numeric;

   bool x_is_offset;
   bool y_is_offset;

   bool line_with_round_ends;
   bool fill_and_square_top;
   bool fill_and_square_bottom;
   bool use_for_group_overflow;

   pdf_gen_field_type type;
   pdf_gen_field_size size;

   pdf_gen_field_alignx alignx;
   pdf_gen_field_aligny aligny;

   pdf_gen_field_alignment alignment;

   string data;

   string font;
   string mask;
   string perm;
   string rest;
   string group;
   string rel_group;

   string prefix;
   string suffix;

   string special;

   string clear_var;
   string assign_var;

   mutable int last_repeat_num;

   bool use_for_totalling;

   pdf_gen_field_data_type data_type;

   pdf_gen_field_alignx rel_group_align_x;
   pdf_gen_field_aligny rel_group_align_y;
};

typedef map< string, pdf_gen_font_info > font_container;
typedef map< string, pdf_gen_font_info >::iterator font_iterator;
typedef map< string, pdf_gen_font_info >::const_iterator font_const_iterator;

typedef map< string, pdf_gen_group_info > group_container;
typedef map< string, pdf_gen_group_info >::iterator group_iterator;
typedef map< string, pdf_gen_group_info >::const_iterator group_const_iterator;

typedef vector< pdf_gen_field_info > field_container;

struct pdf_gen_format
{
   pdf_gen_format( )
    :
    ps( e_page_size_not_applicable ),
    pd( e_page_direction_portrait ),
    page_width( 0.0 ),
    page_height( 0.0 ),
    header_size( 0.0 ),
    footer_size( 0.0 ),
    header_size_growable( false )
   {
   }

   string grid;

   page_size ps;
   page_direction pd;

   float page_width;
   float page_height;

   float header_size;
   float footer_size;

   bool header_size_growable;

   font_container fonts;
   group_container groups;

   field_container fields;
};

struct group_boundary
{
   group_boundary( ) : top( -1.0 ), left( -1.0 ), right( -1.0 ), bottom( -1.0 ) { }

   float top;
   float left;
   float right;
   float bottom;
};

typedef map< string, group_boundary > group_boundary_container;
typedef map< string, group_boundary >::iterator group_boundary_iterator;

typedef map< string, ref_count_ptr< pdf_image > > view_image_container;

struct font_extra
{
   font_extra( ) { }

   font_extra( float size, float yadjust )
    :
    size( size ),
    yadjust( yadjust )
   {
   }

   float size;
   float yadjust;
};

typedef map< string, pair< ref_count_ptr< pdf_font >, font_extra > > view_font_container;


struct text_output_info
{
   text_output_info( )
    :
    top( -1.0 ),
    left( -1.0 ),
    bottom( -1.0 ),
    right( -1.0 ),
    alignment( e_text_align_left )
   {
   }

   float top;
   float left;
   float bottom;
   float right;

   text_align alignment;
};

typedef vector< ref_count_ptr< pdf_page > > page_container;

const size_t c_max_data_chars = 8192;

const int c_default_character_trunc_limit = 15;

const char* const c_grid_variable = "@grid";
const char* const c_page_variable = "@page";
const char* const c_true_variable = "@true";
const char* const c_false_variable = "@false";
const char* const c_footer_size_variable = "@footer_size";
const char* const c_header_size_variable = "@header_size";
const char* const c_permissions_variable = "@permissions";

const char* const c_restrict_admin_only = "admin_only";
const char* const c_restrict_owner_only = "owner_only";
const char* const c_restrict_admin_owner = "admin_owner";

const char* const c_grid_normal = "normal";
const char* const c_grid_reverse = "reverse";

const char* const c_type_box = "box";
const char* const c_type_line = "line";
const char* const c_type_text = "text";
const char* const c_type_image = "image";

const char* const c_size_fixed = "fixed";
const char* const c_size_growable = "growable";
const char* const c_size_shrinkable = "shrinkable";
const char* const c_size_shrinkable_width = "shrinkable_width";
const char* const c_size_shrinkable_height = "shrinkable_height";

const char* const c_extra_forced = "forced";
const char* const c_extra_header = "header";
const char* const c_extra_pinned = "pinned";
const char* const c_extra_repeats = "repeats";
const char* const c_extra_new_page = "new_page";
const char* const c_extra_new_page_each = "new_page_each";

const char* const c_extra_png = "png";
const char* const c_extra_jpg = "jpg";

const char* const c_extra_round = "round";
const char* const c_extra_round_top = "round_top";
const char* const c_extra_round_bottom = "round_bottom";
const char* const c_extra_group_overflow = "group_overflow";

const char* const c_extra_external = "external";
const char* const c_extra_indirect = "indirect";

const char* const c_extra_blank_zero = "blank_zero";

const char* const c_link_extra_grow = "grow";
const char* const c_link_extra_block = "block";

const char* const c_border_extra_shadow = "shadow";

const char* const c_special_format_html = "html";

const char* const c_section_font = "font";
const char* const c_section_field = "field";
const char* const c_section_fonts = "fonts";
const char* const c_section_group = "group";
const char* const c_section_fields = "fields";
const char* const c_section_groups = "groups";

const char* const c_attribute_pos = "pos";
const char* const c_attribute_cols = "cols";
const char* const c_attribute_data = "data";
const char* const c_attribute_font = "font";
const char* const c_attribute_grid = "grid";
const char* const c_attribute_mask = "mask";
const char* const c_attribute_name = "name";
const char* const c_attribute_perm = "perm";
const char* const c_attribute_rest = "rest";
const char* const c_attribute_rows = "rows";
const char* const c_attribute_type = "type";
const char* const c_attribute_size = "size";
const char* const c_attribute_blink = "blink";
const char* const c_attribute_clear = "clear";
const char* const c_attribute_color = "color";
const char* const c_attribute_dtype = "dtype";
const char* const c_attribute_extra = "extra";
const char* const c_attribute_group = "group";
const char* const c_attribute_width = "width";
const char* const c_attribute_alignx = "alignx";
const char* const c_attribute_aligny = "aligny";
const char* const c_attribute_assign = "assign";
const char* const c_attribute_bcolor = "bcolor";
const char* const c_attribute_border = "border";
const char* const c_attribute_groupd = "groupd";
const char* const c_attribute_groupp = "groupp";
const char* const c_attribute_groupr = "groupr";
const char* const c_attribute_height = "height";
const char* const c_attribute_prefix = "prefix";
const char* const c_attribute_suffix = "suffix";
const char* const c_attribute_bgcolor = "bgcolor";
const char* const c_attribute_borderx = "borderx";
const char* const c_attribute_details = "details";
const char* const c_attribute_padding = "padding";
const char* const c_attribute_spacing = "spacing";
const char* const c_attribute_special = "special";
const char* const c_attribute_encoding = "encoding";
const char* const c_attribute_paddingx = "paddingx";
const char* const c_attribute_paddingy = "paddingy";
const char* const c_attribute_pagesize = "pagesize";
const char* const c_attribute_truncate = "truncate";
const char* const c_attribute_alignment = "alignment";
const char* const c_attribute_footer_size = "footer_size";
const char* const c_attribute_header_size = "header_size";
const char* const c_attribute_orientation = "orientation";
const char* const c_attribute_ypos_adjust = "ypos_adjust";

inline string group_repeat_string( int repeat_num )
{
   ostringstream osstr;

   osstr << ifmt( 5 ) << repeat_num;

   return osstr.str( );
}

#ifdef ICONV_SUPPORT
string convert_utf8_to_other( const string& utf8, const string& other )
{
   string str;
   str.reserve( utf8.size( ) );

   string src( utf8 );

   // KLUDGE: The UTF-8 copyright character and dash characters cannot be
   // converted to GB2312 so these are instead replaced by ASCII characters.
   if( other == "gb2312" )
   {
      utf8_replace( src, "\xc2\xa9", "(C)" );
      utf8_replace( src, "\xe2\x80\x94", "-" );
   }

   vector< string > lines;
   split( src, lines, '\n' );
   
   size_t trim_after = 0;
   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string& next_line( lines[ i ] );
      iconv_t cd;

      if( trim_after )
         next_line.erase( trim_after );

      char buf[ c_max_data_chars ];
      memset( buf, '\0', sizeof( buf ) );

      if( next_line.size( ) > c_max_data_chars )
         throw runtime_error( "buffer overflow in convert_utf8_to_other (size = "
          + to_string( next_line.size( ) ) + " but max allowed = " + to_string( c_max_data_chars ) );

      cd = iconv_open( other.c_str( ), "utf-8" );
      if( cd == ( iconv_t )-1 )
      {
         if( errno == EINVAL )
            throw runtime_error( "conversion from UTF-8 to " + other + " not available" );
         else
            throw runtime_error( "unexpected iconv_open error" );
      }

      size_t isize = next_line.length( );
      size_t avail = c_max_data_chars - 1;

      char* p_buf = buf;
      const char* p_src = next_line.c_str( );

      size_t rc = iconv( cd, ( char** )&p_src, &isize, &p_buf, &avail );

      if( rc == -1 )
      {
         if( errno == EILSEQ )
            throw runtime_error( "invalid " + other + " character sequence" );
         else if( errno == E2BIG )
            throw runtime_error( "output buffer too small for conversion" );
         else if( errno == EINVAL )
            throw runtime_error( "incomplete " + other + " character sequence" );
         else if( errno == EBADF )
            throw runtime_error( "invalid conversion descriptor" );

         // NOTE: If error is unknown then will keep trimming the line
         // in order to try and convert as much of it as is possible.
         if( trim_after == 0 )
            trim_after = next_line.length( ) - 1;
         else
            --trim_after;

         if( trim_after != 0 )
            --i;
      }
      else
      {
         trim_after = 0;

         if( !str.empty( ) )
            str += "\n";
         str += string( buf );
      }

      if( iconv_close( cd ) != 0 )
         throw runtime_error( "unexpected iconv_close error" );
   }

   return str;
}

void decode_text( string& text, const pdf_font& text_font, bool is_continuation = false, bool* p_using_wide_chars = 0 )
{
   string encoding( text_font.get_encoding( ) );

   if( !encoding.empty( ) && !is_continuation )
   {
      if( text_font.get_uses_wide_chars( ) )
      {
         // NOTE: It can be possible for a BOM to be present at the start of
         // a UTF-8 data string (due to Windows copy and paste behaviour) so
         // if this appears to be the case then it will need to be skipped.
         string::size_type pos = 0;
         if( text.size( ) >= 3 && text[ 0 ] == ( char )0xef )
            pos = 3;

         if( p_using_wide_chars )
            *p_using_wide_chars = true;

         if( encoding == "GB-EUC-H" || encoding == "GB-EUC-V" )
            text = convert_utf8_to_other( text.substr( pos ), "gb2312" );
         else
            throw runtime_error( "pdf_gen: unsupported wide character encoding '" + encoding + "'" );
      }
      else if( encoding != "StandardEncoding" )
         text = convert_utf8_to_other( text, encoding );
   }
}
#endif

void parse_sides( const string& sides, bool& left, bool& right, bool& top, bool& bottom )
{
   if( !sides.empty( ) )
   {
      vector< string > all_sides;
      split( sides, all_sides );

      left = false;
      right = false;
      top = false;
      bottom = false;

      for( size_t i = 0; i < all_sides.size( ); i++ )
      {
         string next( lower( all_sides[ i ] ) );

         if( next == "left" )
            left = true;
         else if( next == "right" )
            right = true;
         else if( next == "top" )
            top = true;
         else if( next == "bottom" )
            bottom = true;
      }
   }
}

void parse_rgb_values( const string& color, size_t num,
 const string& label, const string& type, const string& name, int& red, int& green, int& blue )
{
   string color_name( lower( color ) );

   red = 0;
   green = 0;
   blue = 0;

   string::size_type pos = color.find( ',' );
   if( pos != string::npos )
   {
      string::size_type npos = color.find( ',', pos + 1 );
      if( npos == string::npos || npos == pos + 1 )
         throw runtime_error( "incorrect format for "
          + label + " in " + type + " '" + color_name + "' at line #" + to_string( num ) );

      red = atoi( color.substr( 0, pos ).c_str( ) );
      green = atoi( color.substr( pos + 1, npos - pos - 1 ).c_str( ) );
      blue = atoi( color.substr( npos + 1 ).c_str( ) );
   }
   else if( color_name == "red" )
      red = 255;
   else if( color_name == "lime" )
      green = 255;
   else if( color_name == "blue" )
      blue = 255;
   else if( color_name == "yellow" )
   {
      red = 255;
      green = 255;
   }
   else if( color_name == "aqua" || color_name == "cyan" )
   {
      green = 255;
      blue = 255;
   }
   else if( color_name == "orange" )
   {
      red = 255;
      green = 165;
   }
   else if( color_name == "fuchsia" || color_name == "magenta" )
   {
      red = 255;
      blue = 255;
   }
   else if( color_name == "maroon" )
      red = 128;
   else if( color_name == "green" )
      green = 128;
   else if( color_name == "navy" )
      blue = 128;
   else if( color_name == "olive" )
   {
      red = 128;
      green = 128;
   }
   else if( color_name == "purple" )
   {
      red = 128;
      blue = 128;
   }
   else if( color_name == "teal" )
   {
      green = 128;
      blue = 128;
   }
   else if( color_name == "gray" || color_name == "grey" )
   {
      red = 128;
      green = 128;
      blue = 128;
   }
   else if( color_name == "white" )
   {
      red = 255;
      green = 255;
      blue = 255;
   }
   else if( color_name != "black" )
      throw runtime_error( "unknown " + label + " '"
       + color + "' in " + type + " '" + name + "' at line #" + to_string( num ) );
}

bool has_permission( const string& restrict, const string& permission, const map< string, string >& permissions )
{
   if( restrict.empty( ) && permission.empty( ) )
      return true;

   bool okay = false;

   if( !restrict.empty( ) )
   {
      if( restrict == c_restrict_admin_only )
      {
         if( permissions.count( "@admin" ) )
            okay = true;
      }
      else if( restrict == c_restrict_owner_only )
      {
         if( permissions.count( "@owner" ) )
            okay = true;
      }
      else if( restrict == c_restrict_admin_owner )
      {
         if( permissions.count( "@admin" ) || permissions.count( "@owner" ) )
            okay = true;
      }
      else
         throw runtime_error( "unexpected group/field restrict value '" + restrict + "'" );
   }

   // FUTURE: Alternate permissions such as "perm_a|perm_b" or permissions whose
   // values are other permissions such as "perm_a_or_b=perm_a|perm_b" also need
   // to handled here.
   if( !okay && !permission.empty( ) )
   {
      if( permissions.count( permission ) )
         okay = true;
   }

   return okay;
}

bool perform_relative_adjustment( const pdf_gen_format& format, const string& group,
 const string& repeat_after, group_boundary_container& group_boundaries, float& x, float& y, float y2,
 float page_width, float normal_group_page_top )
{
   bool okay = true;

   pdf_gen_field_alignx rel_group_alignx;
   pdf_gen_field_aligny rel_group_aligny;

   if( !format.groups.count( group ) )
      throw runtime_error( "invalid group '" + group + "' passed to perform_relative_adjustment" );

   const pdf_gen_group_info& group_info = format.groups.find( group )->second;

   string rel_groupx( group_info.rel_group );
   string rel_groupy( repeat_after.empty( ) ? group_info.rel_group : repeat_after );

   if( rel_groupx.empty( ) )
   {
      if( group_info.x_is_offset )
      {
         if( group_info.rel_group_align_x == e_pdf_gen_field_alignx_right )
            x += page_width;
         else if( group_info.rel_group_align_x == e_pdf_gen_field_alignx_center )
            x += page_width / 2.0;
      }
   }
   else
   {
      if( !group_boundaries.count( rel_groupx ) )
         throw runtime_error( "unable to locate boundary information for group '" + rel_groupx + "'" );

      if( group_info.x_is_offset )
      {
         if( group_info.rel_group_align_x == e_pdf_gen_field_alignx_left )
         {
            if( group_boundaries[ rel_groupx ].left < 0.0 )
               okay = false;
            else
               x += group_boundaries[ group_info.rel_group ].left;
         }
         else if( group_info.rel_group_align_x == e_pdf_gen_field_alignx_right )
         {
            if( group_boundaries[ rel_groupx ].right < 0.0 )
               okay = false;
            else
               x += group_boundaries[ rel_groupx ].right;
         }
         else if( group_info.rel_group_align_x == e_pdf_gen_field_alignx_center )
         {
            if( group_boundaries[ rel_groupx ].left < 0.0 || group_boundaries[ rel_groupx ].right < 0.0 )
               okay = false;
            else
               x += ( group_boundaries[ rel_groupx ].left + group_boundaries[ rel_groupx ].right ) / 2.0;
         }
      }
   }

   if( rel_groupy.empty( ) )
   {
      // NOTE: If no relative group was specified then treat the "header" area as
      // being the "group" it is relative to.
      if( group_info.y_is_offset )
      {
         if( group_info.rel_group_align_y == e_pdf_gen_field_aligny_bottom )
            y += normal_group_page_top;
         else if( group_info.rel_group_align_y == e_pdf_gen_field_aligny_center )
            y += normal_group_page_top / 2.0;
      }
   }
   else
   {
      if( !group_boundaries.count( rel_groupy ) )
         throw runtime_error( "unable to locate boundary information for group '" + rel_groupy + "'" );

      if( !group_info.rel_group.empty( ) )
      {
         // NOTE: If the relative group is part a "block only" link then need to find
         // the lowest boundary across all links (as their boundaries are not grown).
         if( format.groups.find( group_info.rel_group )->second.link_as_block_only )
         {
            string link, next( rel_groupy );
            while( true )
            {
               link = ( format.groups.find( next )->second.flink );
               if( link.empty( ) )
                  break;

               if( group_boundaries[ link ].bottom > group_boundaries[ rel_groupy ].bottom )
                  rel_groupy = link;

               next = link;
            }
         }
      }

      if( group_info.y_is_offset )
      {
         if( group_info.rel_group_align_y == e_pdf_gen_field_aligny_top )
         {
            if( group_boundaries[ rel_groupy ].top < 0.0 )
            {
               if( y2 >= 0.0 )
                  y += y2;
               else
                  okay = false;
            }
            else
               y += group_boundaries[ rel_groupy ].top;
         }
         else if( group_info.rel_group_align_y == e_pdf_gen_field_aligny_bottom )
         {
            if( group_boundaries[ rel_groupy ].bottom < 0.0 )
            {
               if( y2 >= 0.0 )
                  y += y2;
               else
                  okay = false;
            }
            else
               y += group_boundaries[ rel_groupy ].bottom;
         }
         else if( group_info.rel_group_align_y == e_pdf_gen_field_aligny_center )
         {
            if( group_boundaries[ rel_groupy ].top < 0.0 || group_boundaries[ rel_groupy ].bottom < 0.0 )
            {
               if( y2 >= 0.0 )
                  y += y2;
               else
                  okay = false;
            }
            else
               y += ( group_boundaries[ rel_groupy ].top + group_boundaries[ rel_groupy ].bottom ) / 2.0;
         }
      }
   }

   return okay;
}

bool perform_relative_adjustment( const pdf_gen_group_info& group_info,
 const pdf_gen_field_info& field_info, group_boundary_container& group_boundaries,
 float& x, float& y, float y2, float page_width, float page_height )
{
   bool okay = true;

   pdf_gen_field_alignx rel_group_alignx;
   pdf_gen_field_aligny rel_group_aligny;

   string rel_group( group_info.rel_group );
   if( !rel_group.empty( ) )
   {
      rel_group_alignx = group_info.rel_group_align_x;
      rel_group_aligny = group_info.rel_group_align_y;
   }
   else
   {
      rel_group = field_info.rel_group;
      rel_group_alignx = field_info.rel_group_align_x;
      rel_group_aligny = field_info.rel_group_align_y;
   }

   if( rel_group.empty( ) )
   {
      if( field_info.x_is_offset && x < 0.0 )
         x += page_width;

      if( field_info.y_is_offset && y < 0.0 )
         y += page_height;
   }
   else
   {
      if( !group_boundaries.count( rel_group ) )
         throw runtime_error( "unable to locate boundary information for group '" + rel_group + "'" );

      if( field_info.x_is_offset )
      {
         if( field_info.rel_group_align_x == e_pdf_gen_field_alignx_left )
         {
            if( group_boundaries[ rel_group ].left < 0.0 )
               okay = false;
            else
               x += group_boundaries[ rel_group ].left;
         }
         else if( field_info.rel_group_align_x == e_pdf_gen_field_alignx_right )
         {
            if( group_boundaries[ rel_group ].right < 0.0 )
               okay = false;
            else
               x += group_boundaries[ rel_group ].right;
         }
         else if( field_info.rel_group_align_x == e_pdf_gen_field_alignx_center )
         {
            if( group_boundaries[ rel_group ].left < 0.0 || group_boundaries[ rel_group ].right < 0.0 )
               okay = false;
            else
               x += ( group_boundaries[ rel_group ].left + group_boundaries[ rel_group ].right ) / 2.0;
         }
      }

      if( field_info.y_is_offset )
      {
         if( field_info.rel_group_align_y == e_pdf_gen_field_aligny_top )
         {
            if( group_boundaries[ rel_group ].top < 0.0 )
            {
               if( y2 >= 0.0 )
                  y += y2;
               else
                  okay = false;
            }
            else
               y += group_boundaries[ rel_group ].top;
         }
         else if( field_info.rel_group_align_y == e_pdf_gen_field_aligny_bottom )
         {
            if( group_boundaries[ rel_group ].bottom < 0.0 )
            {
               if( y2 >= 0.0 )
                  y += y2;
               else
                  okay = false;
            }
            else
               y += group_boundaries[ rel_group ].bottom;
         }
         else if( field_info.rel_group_align_y == e_pdf_gen_field_aligny_center )
         {
            if( group_boundaries[ rel_group ].top < 0.0 || group_boundaries[ rel_group ].bottom < 0.0 )
            {
               if( y2 >= 0.0 )
                  y += y2;
               else
                  okay = false;
            }
            else
               y += ( group_boundaries[ rel_group ].top + group_boundaries[ rel_group ].bottom ) / 2.0;
         }
      }
   }

   return okay;
}

void get_group_field_info( const string& group, const pdf_gen_format& format,
 const float& group_x, const float& group_y, map< string, int >& group_last_row,
 group_boundary_container& group_boundaries, view_font_container& fonts,
 float normal_group_page_top, float& group_most_top, float& group_most_left,
 float& group_most_right, float& group_most_bottom, float page_width, float page_height )
{
   string default_font( "default" );

   if( !format.groups.find( group )->second.font.empty( ) )
      default_font = format.groups.find( group )->second.font;

   float group_padding_top = format.groups.find( group )->second.padding_top;
   float group_padding_left = format.groups.find( group )->second.padding_left;
   float group_padding_right = format.groups.find( group )->second.padding_right;
   float group_padding_bottom = format.groups.find( group )->second.padding_bottom;

   bool output_every_page = ( group.empty( ) || group[ 0 ] == '@' );

   for( size_t j = 0; j < format.fields.size( ); j++ )
   {
      if( group != format.fields[ j ].group )
         continue;

      float x = format.fields[ j ].x;
      float y = format.fields[ j ].y;

      if( !output_every_page )
      {
         if( format.fields[ j ].x_is_offset )
            x += group_x + group_padding_left;

         if( format.fields[ j ].y_is_offset )
            y += group_y + group_padding_top;
      }
      else
      {
         if( !perform_relative_adjustment( format.groups.find( group )->second,
          format.fields[ j ], group_boundaries, x, y, -1.0, page_width, page_height ) )
            continue;
      }

      float width = format.fields[ j ].width;
      float height = format.fields[ j ].height;

      if( width < 0 )
         width = page_width;

      if( height < 0 )
         height = page_height;

      if( group_most_top < 0 || y < group_most_top )
         group_most_top = y;

      if( group_most_left < 0 || x < group_most_left )
         group_most_left = x;

      if( x + width > group_most_right )
         group_most_right = x + width;

      if( format.fields[ j ].type == e_pdf_gen_field_type_text )
      {
         string font( format.fields[ j ].font );
         if( font.empty( ) )
            font = default_font;

         if( !fonts.count( font ) )
            throw runtime_error( "unknown font '" + font + "' in field #" + to_string( j ) );

         pdf_font& text_font( *fonts[ font ].first );

         int padding_top = 0;
         int padding_bottom = 0;

         if( format.fields[ j ].use_padding )
         {
            padding_top = ( int )format.fields[ j ].padding_top;
            padding_bottom = ( int )format.fields[ j ].padding_bottom;
         }

         float descent = text_font.get_descent( ) / 100.0;
         float cap_height = text_font.get_cap_height( ) / 100.0;

         y += fonts[ font ].second.yadjust;

         float font_size_multiplier = ( fonts[ font ].second.size / 10.0 );

         descent *= font_size_multiplier;
         cap_height *= font_size_multiplier;

         pdf_gen_field_size size = format.fields[ j ].size;

         bool is_hshrinkable
          = ( size == e_pdf_gen_field_size_shrinkable || size == e_pdf_gen_field_size_shrinkable_height );

         float line_height = cap_height - descent;
         float line_spacing = format.fields[ j ].line_spacing * ( height - 1 );

         float total_height = ( line_height * height ) + line_spacing + padding_top + padding_bottom;

         if( y + total_height > group_most_bottom )
            group_most_bottom = y + total_height;
      }
      else
         if( y + height > group_most_bottom )
            group_most_bottom = y + height;
   }

   if( !group.empty( ) && group[ 0 ] != '@' && group_most_bottom > page_height )
      throw runtime_error( "group '" + group + "' exceeds page height " + to_string( page_height ) );
}

void draw_group_border(
 const string& group, const pdf_gen_format& format, pdf_page& page,
 float group_border, float group_border_top, float group_border_left,
 float group_border_right, float group_border_bottom, float red, float green, float blue,
 bool had_overflow, bool is_page_overflow, float normal_group_page_height, float page_height,
 bool is_page_delayed_repeat )
{
   bool group_border_shadow = format.groups.find( group )->second.border_shadow;

   float group_padding_bottom = format.groups.find( group )->second.padding_bottom;

   float width = group_border_right - group_border_left;
   float height = group_border_bottom - group_border_top;

   bool has_top = format.groups.find( group )->second.border_top;
   bool has_left = format.groups.find( group )->second.border_left;
   bool has_right = format.groups.find( group )->second.border_right;
   bool has_bottom = format.groups.find( group )->second.border_bottom;

   // NOTE: If needs a border "shadow" then it is draw first as the border itself will slightly overlap
   // its shadow to make sure that no gap will appear between the two (due to rounding).
   if( group_border_shadow )
   {
      float border_shadow( group_border * 2.0 );
      float border_offset( border_shadow * 0.5 );

      // NOTE: If border doesn't include the top then start the shadow a little higher in case it is being
      // used for a repeating "detail" group (otherwise will see gaps between the shadow for each row).
      float border_shadow_top = group_border_top + ( has_top ? border_shadow : border_offset );

      // NOTE: If this is a "page delayed" group repeat then instead start the shadow at the top.
      if( is_page_delayed_repeat )
         border_shadow_top = group_border_top;

      if( border_shadow_top < normal_group_page_height )
      {
         page.set_grey_stroke( 0 );
         page.set_line_width( border_shadow );

         page.move_to( group_border_left + width + border_offset, page_height - border_shadow_top );

         if( had_overflow )
         {
            if( has_right )
               page.line_to( group_border_left + width + border_offset, page_height - normal_group_page_height );
         }
         else
         {
            float border_shadow_bottom = group_border_top + height + border_offset;

            if( !has_bottom && border_shadow_bottom > normal_group_page_height )
               border_shadow_bottom = normal_group_page_height;

            if( has_right )
               page.line_to( group_border_left + width + border_offset, page_height - border_shadow_bottom );
            else
               page.move_to( group_border_left + width + border_offset, page_height - border_shadow_bottom );
         }

         if( has_bottom && !had_overflow )
            page.line_to( group_border_left + border_shadow, page_height - ( group_border_top + height + border_offset ) );

         page.stroke( );
      }
   }

   page.set_rgb_stroke( red, green, blue );

   page.set_line_width( group_border );

   float group_bgcolor_r = 0;
   float group_bgcolor_g = 0;
   float group_bgcolor_b = 0;

   bool has_group_bg = false;

   if( format.groups.find( group )->second.bgcolor_r >= 0 )
   {
      has_group_bg = true;

      group_bgcolor_r = ( format.groups.find( group )->second.bgcolor_r / 255.0 );
      group_bgcolor_g = ( format.groups.find( group )->second.bgcolor_g / 255.0 );
      group_bgcolor_b = ( format.groups.find( group )->second.bgcolor_b / 255.0 );
   }

   if( had_overflow )
   {
      if( has_left )
      {
         page.move_to( group_border_left, page_height - normal_group_page_height );
         page.line_to( group_border_left, page_height - group_border_top );
      }
      else
         page.move_to( group_border_left, page_height - group_border_top );

      if( has_top && !is_page_overflow )
         page.line_to( group_border_right, page_height - group_border_top );
      else
         page.move_to( group_border_right, page_height - group_border_top );

      if( has_right )
         page.line_to( group_border_right, page_height - normal_group_page_height );
      page.stroke( );
   }
   else
   {
      if( has_group_bg )
      {
         page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

         float height_extension = group_padding_bottom;

         if( height_extension > 0.0 )
         {
            page.rectangle( group_border_left,
             page_height - ( group_border_top + height ), width, height_extension );

            page.fill( );
         }
      }

      if( is_page_overflow || !has_top || !has_left || !has_right || !has_bottom )
      {
         page.move_to( group_border_left, page_height - group_border_top );
         if( has_left )
            page.line_to( group_border_left, page_height - ( group_border_top + height ) );
         else
            page.move_to( group_border_left, page_height - ( group_border_top + height ) );

         if( has_bottom )
            page.line_to( group_border_right, page_height - ( group_border_top + height ) );
         else
            page.move_to( group_border_right, page_height - ( group_border_top + height ) );

         if( has_right )
            page.line_to( group_border_right, page_height - group_border_top );
         else
            page.move_to( group_border_right, page_height - group_border_top );

         if( has_top && !is_page_overflow )
            page.line_to( group_border_left, page_height - group_border_top );

         page.stroke( );
      }
      else
      {
         page.rectangle( group_border_left, page_height - ( group_border_top + height ), width, height );
         page.stroke( );
      }
   }
}

void extend_linked_boundaries( const string& group, const pdf_gen_format& format,
 pdf_page& page, group_boundary_container& group_boundaries, float normal_group_page_top,
 bool allow_extra_extension, float page_height, float normal_group_page_height, float boundary_bottom,
 bool had_overflow, bool use_back_links, map< string, bool >* p_group_still_to_come = 0 )
{
   string next( group );
   while( true )
   {
      string link;

      if( use_back_links )
         link = ( format.groups.find( next )->second.blink );
      else
         link = ( format.groups.find( next )->second.flink );

      if( link.empty( ) )
         break;

      bool grow_bg_and_border = format.groups.find( next )->second.boundaries_can_grow;

      if( format.groups.find( next )->second.link_as_block_only )
      {
         next = link;
         if( !next.empty( ) && !format.groups.find( next )->second.link_as_block_only )
            throw runtime_error( "cannot link block and non-block groups together" );
         continue;
      }

      string linked_group( link );

      if( ( !p_group_still_to_come || !( *p_group_still_to_come )[ linked_group ] )
       && group_boundaries[ linked_group ].bottom < boundary_bottom )
      {
         float linked_group_border = format.groups.find( linked_group )->second.border;

         float top = max( normal_group_page_top, group_boundaries[ linked_group ].top );

         bool link_group_page_overflow = false;

         if( allow_extra_extension && top <= group_boundaries[ linked_group ].top )
            link_group_page_overflow = true;

         float group_bcolor_r = ( format.groups.find( linked_group )->second.bcolor_r / 255.0 );
         float group_bcolor_g = ( format.groups.find( linked_group )->second.bcolor_g / 255.0 );
         float group_bcolor_b = ( format.groups.find( linked_group )->second.bcolor_b / 255.0 );

         if( grow_bg_and_border && format.groups.find( linked_group )->second.bgcolor_r >= 0 )
         {
            float group_bgcolor_r = ( format.groups.find( linked_group )->second.bgcolor_r / 255.0 );
            float group_bgcolor_g = ( format.groups.find( linked_group )->second.bgcolor_g / 255.0 );
            float group_bgcolor_b = ( format.groups.find( linked_group )->second.bgcolor_b / 255.0 );

            float left = group_boundaries[ linked_group ].left;
            float right = group_boundaries[ linked_group ].right;
            float bottom = max( normal_group_page_top, group_boundaries[ linked_group ].bottom );

            float height_extension = boundary_bottom - bottom;
            float extra_height_extension = 0.0;

            // NOTE: If a bottom boundary had already been drawn for the group then need
            // extend the height of the fill in order to cover it up.
            if( link_group_page_overflow )
               extra_height_extension = linked_group_border;

            page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

            page.rectangle( left,
             page_height - ( bottom + height_extension ), right - left, height_extension + extra_height_extension );

            page.fill( );
         }

         group_boundaries[ linked_group ].bottom = boundary_bottom;

         if( grow_bg_and_border && linked_group_border > 0 )
         {
            draw_group_border( linked_group, format, page, linked_group_border,
             top, group_boundaries[ linked_group ].left, group_boundaries[ linked_group ].right,
             group_boundaries[ linked_group ].bottom, group_bcolor_r, group_bcolor_g, group_bcolor_b,
             had_overflow, true, normal_group_page_height, page_height, false );
         }
      }

      next = link;
   }
}

void read_pdf_gen_format( const string& file_name, pdf_gen_format& format )
{
   ifstream inpf( file_name.c_str( ) );

   if( inpf )
   {
      sio_reader reader( inpf );

      format.grid = reader.read_opt_attribute( c_attribute_grid );

      string pagesize = upper( reader.read_attribute( c_attribute_pagesize ) );

      string::size_type pos = pagesize.find( ',' );
      if( pos == string::npos )
      {
         if( pagesize == "A3" )
            format.ps = e_page_size_A3;
         else if( pagesize == "A4" )
            format.ps = e_page_size_A4;
         else if( pagesize == "A5" )
            format.ps = e_page_size_A5;
         else if( pagesize == "B4" )
            format.ps = e_page_size_B4;
         else if( pagesize == "B5" )
            format.ps = e_page_size_B5;
         else
            throw runtime_error( "unexpected pagesize '" + pagesize
             + "' at line #" + to_string( reader.get_last_line_num( ) ) );

         string orientation = lower( reader.read_attribute( c_attribute_orientation ) );

         if( orientation == "portrait" )
            format.pd = e_page_direction_portrait;
         else if( orientation == "landscape" )
            format.pd = e_page_direction_landscape;
         else
            throw runtime_error( "unexpected orientation '" + orientation
             + "' at line #" + to_string( reader.get_last_line_num( ) ) );
      }
      else
      {
         format.page_width = atof( pagesize.substr( 0, pos ).c_str( ) );
         format.page_height = atof( pagesize.substr( pos + 1 ).c_str( ) );

         string orientation;
         if( reader.has_read_attribute( c_attribute_orientation, orientation ) )
            throw runtime_error( "unexpected orientation '" + orientation
             + "' for explicit page size at line #" + to_string( reader.get_last_line_num( ) ) );
      }

      string header_size = reader.read_opt_attribute( c_attribute_header_size );
      if( !header_size.empty( ) )
      {
         if( header_size[ header_size.length( ) - 1 ] == '+' )
            format.header_size_growable = true;

         format.header_size = atof( header_size.c_str( ) );
      }

      format.footer_size = atof( reader.read_opt_attribute( c_attribute_footer_size, "0" ).c_str( ) );

      if( reader.has_started_section( c_section_fonts ) )
      {
         while( reader.has_started_section( c_section_font ) )
         {
            string name( reader.read_attribute( c_attribute_name ) );
            string details( reader.read_attribute( c_attribute_details ) );

            string::size_type pos = details.find_last_of( ',' );
            if( pos == string::npos )
               throw runtime_error(
                "incorrect format for font details at line #" + to_string( reader.get_last_line_num( ) ) );

            pdf_gen_font_info font_info;

            font_info.font_name = details.substr( 0, pos );
            font_info.font_size = atof( details.substr( pos + 1 ).c_str( ) );

            font_info.font_encoding = reader.read_opt_attribute( c_attribute_encoding );

            font_info.ypos_adjust = atof( reader.read_opt_attribute( c_attribute_ypos_adjust ).c_str( ) );

            format.fonts.insert( make_pair( name, font_info ) );

            reader.finish_section( c_section_font );
         }

         reader.finish_section( c_section_fonts );
      }

      format.groups.insert( make_pair( "", pdf_gen_group_info( ) ) );

      if( reader.has_started_section( c_section_groups ) )
      {
         map< string, int > next_child_id;

         while( reader.has_started_section( c_section_group ) )
         {
            pdf_gen_group_info group_info;

            string gpos( reader.read_attribute( c_attribute_pos ) );

            string::size_type pos = gpos.find( ',' );
            if( pos == string::npos || gpos.length( ) < 3 )
               throw runtime_error(
                "incorrect format for group pos at line #" + to_string( reader.get_last_line_num( ) ) );

            string xpos = gpos.substr( 0, pos );
            string ypos = gpos.substr( pos + 1 );

            if( xpos.length( ) < 1 || ypos.length( ) < 1 )
               throw runtime_error(
                "incorrect format for group pos at line #" + to_string( reader.get_last_line_num( ) ) );

            group_info.x = atof( xpos.c_str( ) );
            group_info.y = atof( ypos.c_str( ) );

            if( xpos[ 0 ] == '+' || xpos[ 0 ] == '-' )
               group_info.x_is_offset = true;
            if( ypos[ 0 ] == '+' || ypos[ 0 ] == '-' )
               group_info.y_is_offset = true;

            string name = reader.read_attribute( c_attribute_name );

            group_info.rest = reader.read_opt_attribute( c_attribute_rest );
            group_info.perm = reader.read_opt_attribute( c_attribute_perm );

            group_info.font = reader.read_opt_attribute( c_attribute_font );

            if( !group_info.font.empty( ) && !format.fonts.count( group_info.font ) )
               throw runtime_error( "unknown font '" + group_info.font + "' for group '"
                + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

            group_info.cols = atoi( reader.read_opt_attribute( c_attribute_cols, "1" ).c_str( ) );
            group_info.rows = atoi( reader.read_opt_attribute( c_attribute_rows, "1" ).c_str( ) );

            string extra( reader.read_opt_attribute( c_attribute_extra ) );
            if( !extra.empty( ) )
            {
               vector< string > extras;
               split( extra, extras );

               for( size_t i = 0; i < extras.size( ); i++ )
               {
                  if( extras[ i ] == c_extra_forced )
                     group_info.is_forced = true;
                  else if( extras[ i ] == c_extra_header )
                     group_info.is_header = true;
                  else if( extras[ i ] == c_extra_repeats )
                     group_info.has_repeats = true;
                  else if( extras[ i ] == c_extra_pinned )
                     group_info.is_top_pinned = true;
                  else if( extras[ i ] == c_extra_new_page )
                     group_info.starts_on_new_page = true;
                  else if( extras[ i ] == c_extra_new_page_each )
                  {
                     group_info.page_each_repeat = true;
                     group_info.starts_on_new_page = true;
                  }
                  else
                     throw runtime_error( "invalid group extra '" + extras[ i ]
                      + "' for group '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
               }

               if( group_info.is_header && ( name.empty( ) || name[ 0 ] != '@' ) )
                  throw runtime_error( "invalid use of extra 'header' for normal group '"
                   + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
            }

            string blink_info = reader.read_opt_attribute( c_attribute_blink );
            if( !blink_info.empty( ) )
            {
               string blink( blink_info );
               string::size_type pos = blink.find( ':' );

               string link_extra;
               if( pos != string::npos )
               {
                  link_extra = blink.substr( pos + 1 );
                  blink.erase( pos );

                  if( !link_extra.empty( ) )
                  {
                     if( link_extra == c_link_extra_grow )
                        group_info.boundaries_can_grow = true;
                     else if( link_extra == c_link_extra_block )
                        group_info.link_as_block_only = true;
                  }
               }

               group_info.blink = blink;

               if( !format.groups.count( blink ) )
                  throw runtime_error( "attempt to link to undefined group '"
                   + blink + "' in group '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               if( !format.groups.find( blink )->second.flink.empty( ) )
                  throw runtime_error( "attempt to link to already linked group '"
                   + blink + "' in group '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               format.groups.find( blink )->second.flink = name;
               format.groups.find( blink )->second.link_as_block_only = group_info.link_as_block_only;
               format.groups.find( blink )->second.boundaries_can_grow = group_info.boundaries_can_grow;
            }

            group_info.dep_group = reader.read_opt_attribute( c_attribute_groupd );

            group_info.par_group = reader.read_opt_attribute( c_attribute_groupp );

            if( !group_info.par_group.empty( ) )
            {
               if( !format.groups.count( group_info.par_group ) )
                  throw runtime_error( "unknown parent group '" + group_info.par_group
                   + "' in group '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               format.groups.find( group_info.par_group )->second.has_child_group = true;
            }

            string parent_for_trio( group_info.par_group );

            string rel_group( reader.read_opt_attribute( c_attribute_groupr ) );

            if( parent_for_trio.empty( ) && !rel_group.empty( ) )
            {
               pos = rel_group.find( ':' );

               if( format.groups.count( rel_group.substr( 0, pos ) )
                && !format.groups.find( rel_group.substr( 0, pos ) )->second.par_group.empty( ) )
                  parent_for_trio = format.groups.find( rel_group.substr( 0, pos ) )->second.par_group;
            }

            string id = to_string( ++next_child_id[ parent_for_trio ] );

            if( parent_for_trio.empty( ) && id.length( ) < 2 )
               id = "0" + id;

            if( !parent_for_trio.empty( ) )
               id = format.groups.find( parent_for_trio )->second.id + "." + id;

            group_info.id = id;

            if( rel_group.empty( ) )
               rel_group = group_info.par_group;

            pos = rel_group.find( ':' );
            if( pos != string::npos )
            {
               string align_info( rel_group.substr( pos + 1 ) );
               rel_group.erase( pos );

               pos = align_info.find( ',' );
               if( pos == string::npos )
                  throw runtime_error( "invalid align_info '" + align_info + "' for rel_group '" + rel_group
                   + "' in group '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               string alignx( align_info.substr( 0, pos ) );
               string aligny( align_info.substr( pos + 1 ) );

               if( alignx == "left" )
                  group_info.rel_group_align_x = e_pdf_gen_field_alignx_left;
               else if( alignx == "right" )
                  group_info.rel_group_align_x = e_pdf_gen_field_alignx_right;
               else if( alignx == "center" )
                  group_info.rel_group_align_x = e_pdf_gen_field_alignx_center;
               else
                  throw runtime_error( "invalid alignx '" + alignx + "' for rel_group '" + rel_group
                   + "' in group '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               if( aligny == "top" )
                  group_info.rel_group_align_y = e_pdf_gen_field_aligny_top;
               else if( aligny == "bottom" )
                  group_info.rel_group_align_y = e_pdf_gen_field_aligny_bottom;
               else if( aligny == "center" )
                  group_info.rel_group_align_y = e_pdf_gen_field_aligny_center;
               else
                  throw runtime_error( "invalid alignx '" + alignx + "' for rel_group '" + rel_group
                   + "' in group '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
            }

            group_info.rel_group = rel_group;

            string border = reader.read_opt_attribute( c_attribute_border );
            if( !border.empty( ) )
            {
               string sides;
               pos = border.find( ':' );
               if( pos != string::npos )
               {
                  sides = border.substr( pos + 1 );
                  border.erase( pos );
               }

               parse_sides( sides, group_info.border_left,
                group_info.border_right, group_info.border_top, group_info.border_bottom );

               group_info.border = atof( border.c_str( ) );
            }

            string bcolor = reader.read_opt_attribute( c_attribute_bcolor );
            if( !bcolor.empty( ) )
            {
               int red = 0;
               int green = 0;
               int blue = 0;

               parse_rgb_values( bcolor,
                reader.get_last_line_num( ), "bcolor", "group", name, red, green, blue );

               group_info.bcolor_r = red;
               group_info.bcolor_g = green;
               group_info.bcolor_b = blue;
            }

            string borderx = reader.read_opt_attribute( c_attribute_borderx );
            if( !borderx.empty( ) )
            {
               if( borderx == c_border_extra_shadow )
                  group_info.border_shadow = true;
               else
                  throw runtime_error( "unknown border extra '" + borderx
                   + "' in group '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
            }

            string bgcolor = reader.read_opt_attribute( c_attribute_bgcolor );
            if( !bgcolor.empty( ) )
            {
               int red = 0;
               int green = 0;
               int blue = 0;

               parse_rgb_values( bgcolor,
                reader.get_last_line_num( ), "bgcolor", "group", name, red, green, blue );

               group_info.bgcolor_r = red;
               group_info.bgcolor_g = green;
               group_info.bgcolor_b = blue;
            }

            string padding = reader.read_opt_attribute( c_attribute_padding );
            if( !padding.empty( ) )
            {
               pos = padding.find( ',' );
               if( pos == string::npos )
                  group_info.padding_left = group_info.padding_right
                   = group_info.padding_top = group_info.padding_bottom = atof( padding.c_str( ) );
               else
               {
                  string::size_type npos = padding.find( ',', pos + 1 );
                  if( npos == string::npos )
                  {
                     group_info.padding_left = group_info.padding_right = atof( padding.substr( 0, pos ).c_str( ) );
                     group_info.padding_top = group_info.padding_bottom = atof( padding.substr( pos + 1 ).c_str( ) );
                  }
                  else
                  {
                     string::size_type lpos = padding.find( ',', npos + 1 );
                     if( lpos == string::npos )
                        throw runtime_error( "invalid padding '" + padding + "' in group '"
                         + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

                     group_info.padding_left = atof( padding.substr( 0, pos ).c_str( ) );
                     group_info.padding_right = atof( padding.substr( pos + 1, npos - pos ).c_str( ) );
                     group_info.padding_top = atof( padding.substr( npos + 1, lpos - npos ).c_str( ) );
                     group_info.padding_bottom = atof( padding.substr( lpos + 1 ).c_str( ) );
                  }
               }
            }

            group_info.paddingx = atoi( reader.read_opt_attribute( c_attribute_paddingx, "0" ).c_str( ) );
            group_info.paddingy = atoi( reader.read_opt_attribute( c_attribute_paddingy, "0" ).c_str( ) );

            format.groups.insert( make_pair( name, group_info ) );

            reader.finish_section( c_section_group );
         }

         reader.finish_section( c_section_groups );
      }

      if( reader.has_started_section( c_section_fields ) )
      {
         while( reader.has_started_section( c_section_field ) )
         {
            pdf_gen_field_info field_info;

            string fpos( reader.read_attribute( c_attribute_pos ) );

            string::size_type pos = fpos.find( ',' );
            if( pos == string::npos || fpos.length( ) < 3 )
               throw runtime_error(
                "incorrect format for pos in field at line #" + to_string( reader.get_last_line_num( ) ) );

            string xpos = fpos.substr( 0, pos );
            string ypos = fpos.substr( pos + 1 );

            if( xpos.length( ) < 1 || ypos.length( ) < 1 )
               throw runtime_error(
               "incorrect format for pos in field at line #" + to_string( reader.get_last_line_num( ) ) );

            field_info.x = atof( xpos.c_str( ) );
            field_info.y = atof( ypos.c_str( ) );

            if( xpos[ 0 ] == '+' || xpos[ 0 ] == '-' )
               field_info.x_is_offset = true;
            if( ypos[ 0 ] == '+' || ypos[ 0 ] == '-' )
               field_info.y_is_offset = true;

            string name = reader.read_attribute( c_attribute_name );

            field_info.rest = reader.read_opt_attribute( c_attribute_rest );
            field_info.perm = reader.read_opt_attribute( c_attribute_perm );

            string size( reader.read_opt_attribute( c_attribute_size ) );
            if( !size.empty( ) )
            {
               if( size == c_size_fixed )
                  field_info.size = e_pdf_gen_field_size_fixed;
               else if( size == c_size_growable )
                  field_info.size = e_pdf_gen_field_size_growable;
               else if( size == c_size_shrinkable )
                  field_info.size = e_pdf_gen_field_size_shrinkable;
               else if( size == c_size_shrinkable_width )
                  field_info.size = e_pdf_gen_field_size_shrinkable_width;
               else if( size == c_size_shrinkable_height )
                  field_info.size = e_pdf_gen_field_size_shrinkable_height;
               else
                  throw runtime_error( "unknown field size '" + size + "' in field '"
                   + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
            }

            string type( reader.read_opt_attribute( c_attribute_type ) );
            if( !type.empty( ) )
            {
               if( type == c_type_box )
                  field_info.type = e_pdf_gen_field_type_box;
               else if( type == c_type_line )
                  field_info.type = e_pdf_gen_field_type_line;
               else if( type == c_type_text )
                  field_info.type = e_pdf_gen_field_type_text;
               else if( type == c_type_image )
                  field_info.type = e_pdf_gen_field_type_image;
               else
                  throw runtime_error( "unknown field type '" + type + "' in field '"
                   + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
            }

            string extra( reader.read_opt_attribute( c_attribute_extra ) );
            if( !extra.empty( ) )
            {
               vector< string > all_extras;
               split( extra, all_extras );

               for( size_t i = 0; i < all_extras.size( ); i++ )
               {
                  string next_extra( all_extras[ i ] );

                  if( next_extra == c_extra_png )
                     field_info.is_png = true;
                  else if( next_extra == c_extra_jpg )
                     field_info.is_jpg = true;
                  else if( next_extra == c_extra_round )
                     field_info.line_with_round_ends = true;
                  else if( next_extra == c_extra_pinned )
                     field_info.is_top_pinned = true;
                  else if( next_extra == c_extra_external )
                     field_info.is_external_data = true;
                  else if( next_extra == c_extra_indirect )
                     field_info.is_indirect_data = true;
                  else if( next_extra == c_extra_blank_zero )
                     field_info.blank_zero_numeric = true;
                  else if( next_extra == c_extra_round_top )
                  {
                     field_info.line_with_round_ends = true;
                     field_info.fill_and_square_bottom = true;
                  }
                  else if( next_extra == c_extra_round_bottom )
                  {
                     field_info.line_with_round_ends = true;
                     field_info.fill_and_square_top = true;
                  }
                  else if( next_extra == c_extra_group_overflow )
                     field_info.use_for_group_overflow = true;
                  else
                     throw runtime_error( "unexpected field extra '" + next_extra
                      + "' in field '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
               }
            }

            if( field_info.is_top_pinned && field_info.type != e_pdf_gen_field_type_text )
               throw runtime_error( "currently only 'text' type fields can be 'pinned' in field '"
                + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

            field_info.group = reader.read_opt_attribute( c_attribute_group );

            bool every_page_group = ( field_info.group.empty( ) || field_info.group[ 0 ] == '@' );

            if( !every_page_group && ( !field_info.x_is_offset || !field_info.x_is_offset ) )
               throw runtime_error( "group fields cannot use absolute co-ords in field '"
                + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

            string rel_group( reader.read_opt_attribute( c_attribute_groupr ) );

            if( !rel_group.empty( ) && !every_page_group )
               throw runtime_error( "group relative position not permitted for group field in field '"
                + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

            pos = rel_group.find( ':' );
            if( pos != string::npos )
            {
               string align_info( rel_group.substr( pos + 1 ) );
               rel_group.erase( pos );

               pos = align_info.find( ',' );
               if( pos == string::npos )
                  throw runtime_error( "invalid align_info '" + align_info + "' for rel_group '" + rel_group
                   + "' in field '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               string alignx( align_info.substr( 0, pos ) );
               string aligny( align_info.substr( pos + 1 ) );

               if( alignx == "left" )
                  field_info.rel_group_align_x = e_pdf_gen_field_alignx_left;
               else if( alignx == "right" )
                  field_info.rel_group_align_x = e_pdf_gen_field_alignx_right;
               else if( alignx == "center" )
                  field_info.rel_group_align_x = e_pdf_gen_field_alignx_center;
               else
                  throw runtime_error( "invalid alignx '" + alignx + "' for rel_group '" + rel_group
                   + "' in field '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               if( aligny == "top" )
                  field_info.rel_group_align_y = e_pdf_gen_field_aligny_top;
               else if( aligny == "bottom" )
                  field_info.rel_group_align_y = e_pdf_gen_field_aligny_bottom;
               else if( aligny == "center" )
                  field_info.rel_group_align_y = e_pdf_gen_field_aligny_center;
               else
                  throw runtime_error( "invalid alignx '" + alignx + "' for rel_group '" + rel_group
                   + "' in field '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
            }

            field_info.rel_group = rel_group;

            if( field_info.type == e_pdf_gen_field_type_text )
            {
               field_info.font = reader.read_opt_attribute( c_attribute_font );

               if( !field_info.font.empty( ) && !format.fonts.count( field_info.font ) )
                  throw runtime_error( "unknown font '" + field_info.font + "' for field '"
                   + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
            }

            field_info.mask = reader.read_opt_attribute( c_attribute_mask );

            string color( reader.read_opt_attribute( c_attribute_color ) );
            if( !color.empty( ) )
            {
               int red = 0;
               int green = 0;
               int blue = 0;

               parse_rgb_values( color,
                reader.get_last_line_num( ), "color", "field", name, red, green, blue );

               field_info.color_r = red;
               field_info.color_g = green;
               field_info.color_b = blue;

               field_info.had_color = true;
            }

            field_info.width = atoi( reader.read_attribute( c_attribute_width ).c_str( ) );
            field_info.height = atoi( reader.read_attribute( c_attribute_height ).c_str( ) );

            string border = reader.read_opt_attribute( c_attribute_border );
            if( !border.empty( ) )
            {
               string sides;
               pos = border.find( ':' );
               if( pos != string::npos )
               {
                  sides = border.substr( pos + 1 );
                  border.erase( pos );
               }

               if( !sides.empty( )
                && ( field_info.type == e_pdf_gen_field_type_box || field_info.type == e_pdf_gen_field_type_line ) )
                  throw runtime_error( "border sides not supported for 'box/line' type field '"
                   + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               parse_sides( sides, field_info.border_left,
                field_info.border_right, field_info.border_top, field_info.border_bottom );

               field_info.border = atof( border.c_str( ) );
            }

            string bcolor = reader.read_opt_attribute( c_attribute_bcolor );
            if( !bcolor.empty( ) )
            {
               int red = 0;
               int green = 0;
               int blue = 0;

               parse_rgb_values( bcolor,
                reader.get_last_line_num( ), "bcolor", "field", name, red, green, blue );

               field_info.bcolor_r = red;
               field_info.bcolor_g = green;
               field_info.bcolor_b = blue;
            }

            string borderx = reader.read_opt_attribute( c_attribute_borderx );
            if( !borderx.empty( ) )
            {
               if( borderx == c_border_extra_shadow )
                  field_info.border_shadow = true;
               else
                  throw runtime_error( "unknown border extra '" + borderx + "' in field '"
                   + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
            }

            string bgcolor = reader.read_opt_attribute( c_attribute_bgcolor );
            if( !bgcolor.empty( ) )
            {
               if( field_info.type == e_pdf_gen_field_type_text
                && ( field_info.size == e_pdf_gen_field_size_shrinkable
                || field_info.size == e_pdf_gen_field_size_shrinkable_width ) )
                  throw runtime_error( "'" + size + "' cannot use 'bgcolor' for 'text' field '"
                   + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               if( field_info.type == e_pdf_gen_field_type_box || field_info.type == e_pdf_gen_field_type_line )
                  throw runtime_error( "'" + size + "' cannot use 'bgcolor' for 'box/line' field '"
                   + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

               int red = 0;
               int green = 0;
               int blue = 0;

               parse_rgb_values( bgcolor,
                reader.get_last_line_num( ), "bgcolor", "field", name, red, green, blue );

               field_info.bgcolor_r = red;
               field_info.bgcolor_g = green;
               field_info.bgcolor_b = blue;
            }

            if( field_info.type == e_pdf_gen_field_type_image )
            {
               string alignx( reader.read_opt_attribute( c_attribute_alignx ) );
               if( !alignx.empty( ) )
               {
                  if( alignx == "left" )
                     field_info.alignx = e_pdf_gen_field_alignx_left;
                  else if( alignx == "right" )
                     field_info.alignx = e_pdf_gen_field_alignx_right;
                  else if( alignx == "center" )
                     field_info.alignx = e_pdf_gen_field_alignx_center;
                  else
                     throw runtime_error( "unknown x-alignment '" + alignx + "' in field '"
                      + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
               }

               string aligny( reader.read_opt_attribute( c_attribute_aligny ) );
               if( !aligny.empty( ) )
               {
                  if( aligny == "top" )
                     field_info.aligny = e_pdf_gen_field_aligny_top;
                  else if( aligny == "bottom" )
                     field_info.aligny = e_pdf_gen_field_aligny_bottom;
                  else if( aligny == "center" )
                     field_info.aligny = e_pdf_gen_field_aligny_center;
                  else
                     throw runtime_error( "unknown y-alignment '" + aligny + "' in field '"
                      + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
               }
            }

            string padding = reader.read_opt_attribute( c_attribute_padding );
            if( !padding.empty( ) )
            {
               field_info.use_padding = true;

               pos = padding.find( ',' );
               if( pos == string::npos )
                  field_info.padding_left = field_info.padding_right
                   = field_info.padding_top = field_info.padding_bottom = atof( padding.c_str( ) );
               else
               {
                  string::size_type npos = padding.find( ',', pos + 1 );
                  if( npos == string::npos )
                  {
                     field_info.padding_left = field_info.padding_right = atof( padding.substr( 0, pos ).c_str( ) );
                     field_info.padding_top = field_info.padding_bottom = atof( padding.substr( pos + 1 ).c_str( ) );
                  }
                  else
                  {
                     string::size_type lpos = padding.find( ',', npos + 1 );
                     if( lpos == string::npos )
                        throw runtime_error( "invalid padding '" + padding + "' in field '"
                         + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

                     field_info.padding_left = atof( padding.substr( 0, pos ).c_str( ) );
                     field_info.padding_right = atof( padding.substr( pos + 1, npos - pos ).c_str( ) );
                     field_info.padding_top = atof( padding.substr( npos + 1, lpos - npos ).c_str( ) );
                     field_info.padding_bottom = atof( padding.substr( lpos + 1 ).c_str( ) );
                  }
               }
            }

            if( field_info.type == e_pdf_gen_field_type_text )
            {
               string spacing( reader.read_opt_attribute( c_attribute_spacing ) );
               if( !spacing.empty( ) )
               {
                  field_info.has_line_spacing = true;
                  field_info.line_spacing = atof( spacing.c_str( ) );
               }

               field_info.special = reader.read_opt_attribute( c_attribute_special );

               string truncate = reader.read_opt_attribute( c_attribute_truncate );
               if( !truncate.empty( ) )
                  field_info.truncate = atoi( truncate.c_str( ) );

               string alignment( reader.read_opt_attribute( c_attribute_alignment ) );
               if( !alignment.empty( ) )
               {
                  if( alignment == "left" )
                     field_info.alignment = e_pdf_gen_field_alignment_left;
                  else if( alignment == "right" )
                     field_info.alignment = e_pdf_gen_field_alignment_right;
                  else if( alignment == "center" )
                     field_info.alignment = e_pdf_gen_field_alignment_center;
                  else if( alignment == "justify" )
                     field_info.alignment = e_pdf_gen_field_alignment_justify;
                  else
                     throw runtime_error( "unknown alignment '" + alignment + "' in field '"
                      + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
               }
            }

            if( field_info.type != e_pdf_gen_field_type_box && field_info.type != e_pdf_gen_field_type_line )
            {
               field_info.data = reader.read_attribute( c_attribute_data );

               string dtype( reader.read_opt_attribute( c_attribute_dtype ) );

               if( !dtype.empty( ) )
               {
                  string dtypex;
                  string::size_type pos = dtype.find( ':' );
                  if( pos != string::npos )
                  {
                     dtypex = dtype.substr( pos + 1 );
                     dtype.erase( pos );
                  }

                  if( dtype == "date" )
                     field_info.data_type = e_pdf_gen_field_data_type_date;
                  else if( dtype == "time" )
                     field_info.data_type = e_pdf_gen_field_data_type_time;
                  else if( dtype == "bool" || dtype == "boolean" )
                     field_info.data_type = e_pdf_gen_field_data_type_boolean;
                  else if( dtype == "int" || dtype == "integer" )
                     field_info.data_type = e_pdf_gen_field_data_type_integer;
                  else if( dtype == "numeric" )
                     field_info.data_type = e_pdf_gen_field_data_type_numeric;
                  else if( dtype == "datetime" || dtype == "date_time" )
                     field_info.data_type = e_pdf_gen_field_data_type_date_time;
                  else
                     throw runtime_error( "unknown data type '" + dtype + "' for field '"
                      + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

                  if( !dtypex.empty( ) )
                  {
                     if( dtypex == "total" )
                     {
                        if( field_info.data_type != e_pdf_gen_field_data_type_integer
                         && field_info.data_type != e_pdf_gen_field_data_type_numeric )
                           throw runtime_error( "data type extra 'total' is not permitted for '" + dtype
                             + "' field '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );

                        field_info.use_for_totalling = true;
                     }
                     else
                        throw runtime_error( "unexpected data type extra '" + dtypex
                         + "' for field '" + name + "' at line #" + to_string( reader.get_last_line_num( ) ) );
                  }
               }

               field_info.clear_var = reader.read_opt_attribute( c_attribute_clear );
               field_info.assign_var = reader.read_opt_attribute( c_attribute_assign );

               field_info.prefix = reader.read_opt_attribute( c_attribute_prefix );
               field_info.suffix = reader.read_opt_attribute( c_attribute_suffix );
            }

            format.fields.push_back( field_info );

            reader.finish_section( c_section_field );
         }

         reader.finish_section( c_section_fields );
      }

      reader.verify_finished_sections( );
   }
   else
      throw runtime_error( "unable to open '" + file_name + "' for input" );
}

bool process_group(
 const string& group,
 const pdf_gen_format& format,
 const map< string, string >& variables,
 const map< string, string >& permissions,
 map< string, string >& dynamic_variables, pdf_doc& doc,
 pdf_page& page, bool is_page_overflow, float page_width, float page_height,
 float& normal_group_page_top, float normal_group_page_height,
 group_boundary_container& group_boundaries, view_font_container& fonts,
 view_image_container& images, map< string, int >& group_last_row, bool was_page_delayed,
 bool& page_overflow_occured, text_output_info& page_count_info, vector< string >& temporary_image_files,
 map< string, set< string > >& group_cols_processed, map< string, string >& group_text_overflows,
 const string& repeat_prefix, int repeat_num, const string& repeat_after, bool is_nested_repeat )
{
   bool is_normal_group = true;

   if( group.empty( ) || group[ 0 ] == '@' )
      is_normal_group = false;

   string default_font( "default" );

   if( !format.groups.find( group )->second.font.empty( ) )
      default_font = format.groups.find( group )->second.font;

   int group_cols = format.groups.find( group )->second.cols;
   int group_rows = format.groups.find( group )->second.rows;

   float group_border = format.groups.find( group )->second.border;

   float group_x = format.groups.find( group )->second.x;
   float group_y = format.groups.find( group )->second.y;

   if( repeat_num > 0 )
      group_y = format.groups.find( group )->second.paddingy;

   bool is_block_linked = false;
   if( format.groups.find( group )->second.link_as_block_only )
      is_block_linked = true;

   if( !perform_relative_adjustment( format, group, repeat_after, group_boundaries, group_x, group_y,
    ( is_page_overflow || was_page_delayed || is_block_linked || is_nested_repeat ) ? 0.0 : -1.0,
    page_width, normal_group_page_top ) )
      throw runtime_error( "group '" + group + "' must not be defined before its relative group" );

   bool starts_on_new_page = format.groups.find( group )->second.starts_on_new_page;

   // NOTE: Unless specifically set to start each repeat on a new page only do so for the first record.
   if( repeat_num > 0 && !format.groups.find( group )->second.page_each_repeat )
      starts_on_new_page = false;

   float group_bcolor_r = 0.0;
   float group_bcolor_g = 0.0;
   float group_bcolor_b = 0.0;

   if( group_border > 0 )
   {
      group_bcolor_r = ( format.groups.find( group )->second.bcolor_r / 255.0 );
      group_bcolor_g = ( format.groups.find( group )->second.bcolor_g / 255.0 );
      group_bcolor_b = ( format.groups.find( group )->second.bcolor_b / 255.0 );
   }

   bool has_group_bg = false;

   float group_bgcolor_r = 0.0;
   float group_bgcolor_g = 0.0;
   float group_bgcolor_b = 0.0;

   if( format.groups.find( group )->second.bgcolor_r >= 0 )
   {
      has_group_bg = true;

      group_bgcolor_r = ( format.groups.find( group )->second.bgcolor_r / 255.0 );
      group_bgcolor_g = ( format.groups.find( group )->second.bgcolor_g / 255.0 );
      group_bgcolor_b = ( format.groups.find( group )->second.bgcolor_b / 255.0 );
   }

   float group_padding_top = format.groups.find( group )->second.padding_top;
   float group_padding_left = format.groups.find( group )->second.padding_left;
   float group_padding_right = format.groups.find( group )->second.padding_right;
   float group_padding_bottom = format.groups.find( group )->second.padding_bottom;

   int paddingx = format.groups.find( group )->second.paddingx;
   int paddingy = format.groups.find( group )->second.paddingy;

   bool group_is_header = format.groups.find( group )->second.is_header;
   bool group_is_top_pinned = format.groups.find( group )->second.is_top_pinned;

   float most_top = -1.0;
   float most_left = -1.0;
   float most_right = 0.0;
   float most_bottom = 0.0;

   float next_top = -1.0;
   float next_left = -1.0;
   float next_right = 0.0;
   float next_bottom = 0.0;

   float last_top = -1.0;
   float last_left = -1.0;
   float last_right = 0.0;
   float last_bottom = 0.0;

   // NOTE: If processing overflow then group top is at top of available page area otherwise
   // if there is either not enough room for one row (or if the group needs to be started on
   // a new page and it is not so) then return so it will be treated as "page delayed".
   if( is_page_overflow )
      group_y = normal_group_page_top;
   else if( group_y >= normal_group_page_height
    || ( starts_on_new_page && group_y - format.groups.find( group )->second.y > 0.0 ) )
   {
      page_overflow_occured = false;
      return false;
   }

   if( was_page_delayed )
      group_y = normal_group_page_top;

   if( was_page_delayed || is_page_overflow )
   {
      // NOTE: Need to adjust the starting point if the group is relative to the top of the page.
      if( format.groups.find( group )->second.y_is_offset
       && format.groups.find( group )->second.rel_group.empty( ) )
         group_y += format.groups.find( group )->second.y;
   }

   bool text_overflow_present = false;
   if( is_normal_group )
   {
      for( size_t i = 0; i < format.fields.size( ); i++ )
      {
         if( group != format.fields[ i ].group )
            continue;

         for( size_t j = 0; j < group_cols; j++ )
         {
            if( group_text_overflows.count( group + "_" + to_string( i ) + "_" + to_string( j ) ) )
            {
               text_overflow_present = true;
               break;
            }

            if( text_overflow_present )
               break;
         }
      }
   }

   if( is_page_overflow || text_overflow_present )
      group_padding_top = 0;

   int next = 0;
   int loops = group_rows * group_cols;

   int first = next;

   if( is_normal_group && group_last_row[ group ] >= 0 )
   {
      first = group_last_row[ group ];
      next = group_last_row[ group ] * group_cols;
   }

   float x_offset = 0.0;
   float y_offset = 0.0;

   float group_column_width = 0.0;

   float group_bg_x = 0.0;
   float group_bg_y = 0.0;

   float group_width = 0.0;
   float group_row_height = 0.0;
   float group_standard_row_height = 0.0;

   float group_most_top = -1.0;
   float group_most_left = -1.0;
   float group_most_right = 0.0;
   float group_most_bottom = 0.0;

   get_group_field_info( group, format,
    group_x, group_y, group_last_row, group_boundaries, fonts, normal_group_page_top,
    group_most_top, group_most_left, group_most_right, group_most_bottom, page_width, page_height );

   float x = group_most_left - group_padding_left;
   float y = group_most_bottom + group_padding_top;

   float width = ( group_most_right - group_most_left ) * group_cols;
   float height = ( group_most_bottom - group_most_top );

   width += paddingx * ( group_cols - 1 );

   group_column_width = group_most_right - group_most_left;

   group_bg_x = group_x;
   group_bg_y = group_y + group_padding_top;

   group_width = width + group_padding_left + group_padding_right;
   group_row_height = group_standard_row_height = group_most_bottom - group_most_top;

   // NOTE: If a group does not have its top "pinned" (and it has a border or a background color)
   // then if it can't fit in this page then skip its processing until the following page.
   if( !group_is_top_pinned && ( group_border > 0 || format.groups.find( group )->second.bgcolor_r >= 0 )
    && ( group_y + group_row_height + group_padding_top + group_padding_bottom ) > normal_group_page_height )
   {
      // NOTE: If this group is a "nested repeat" then extend both the background and
      // left and right borders (if present) to the end of the available page space.
      if( has_group_bg && is_nested_repeat )
      {
         page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

         float height_extension = normal_group_page_height - group_y;

         if( height_extension > 0.0 )
         {
            page.rectangle( group_bg_x,
             page_height - ( group_y + height_extension ), group_width, height_extension );

            page.fill( );
         }
      }

      if( group_border > 0 && is_nested_repeat )
      {
         float group_border_top = group_y;
         float group_border_left = group_x;
         float group_border_right = group_border_left + group_width;
         float group_border_bottom = normal_group_page_height;

         draw_group_border( group, format, page, group_border,
          group_border_top, group_border_left, group_border_right, group_border_bottom,
          group_bcolor_r, group_bcolor_g, group_bcolor_b, true,
          is_page_overflow, normal_group_page_height, page_height, false );
      }

      page_overflow_occured = false;
      return false;
   }

   if( has_group_bg )
   {
      if( group_padding_top > 0 )
      {
         page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

         page.rectangle( x, page_height - group_bg_y, group_width, group_padding_top );
         page.fill( );
      }
   }

   int last_row = 0;
   int previous = 0;
   int num_field_cols = 0;
   int num_field_cols_skipped = 0;

   bool had_row_overflow = false;
   bool had_text_overflow = false;

   set< string > columns_processed;

   // NOTE: Iterate throw all group rows and columns making adjustments to the x_offset based
   // upon the group_column_width and to the y_offset by using the most top and bottom fields
   // from the previous row.
   while( next < loops )
   {
      int row = next / group_cols;
      int col = next % group_cols;

      if( col > 0 )
         x_offset += group_column_width + paddingx;
      else
         x_offset = 0;

      next_left = -1.0;
      next_right = 0.0;

      bool is_subsequent_row = false;

      if( ( !is_normal_group || !is_page_overflow ) && row > 0 )
         is_subsequent_row = true;

      if( is_normal_group && ( is_page_overflow && row > group_last_row[ group ] ) )
         is_subsequent_row = true;

      float extra_offset = paddingy;
      if( row == first && text_overflow_present )
      {
         extra_offset = 0.0;
         is_subsequent_row = false;
      }

      if( is_subsequent_row && col == 0 )
         y_offset += ( next_bottom - next_top ) + extra_offset;

      if( !is_subsequent_row && row > 0 && col == 0 )
         y_offset += extra_offset;

      if( col == 0 )
      {
         next_top = -1.0;
         next_bottom = 0.0;

         group_row_height = group_standard_row_height;
      }

      // NOTE: After the first row has been processed we have all the boundary information
      // except the most bottom so store this to allow fields relative to the top to still
      // be processed even if the group processing is not finished.
      if( row == 1 && col == 0 )
      {
         if( is_normal_group )
         {
            group_boundary boundary;

            boundary.top = group_y;
            boundary.left = group_x;
            boundary.right = most_right + group_padding_right;

            group_boundaries[ group ] = boundary;
         }
      }

      string rowstr( to_string( row + 1 ) );
      while( rowstr.length( ) < 3 )
         rowstr = '0' + rowstr;

      string colstr( to_string( col + 1 ) );
      while( colstr.length( ) < 3 )
         colstr = '0' + colstr;

      string group_prefix( group );
      if( group_rows > 1 && group_cols > 1 )
         group_prefix += "_" + rowstr + "_" + colstr + "_";
      else if( group_rows > 1 )
         group_prefix += "_" + rowstr + "_";
      else if( group_cols > 1 )
         group_prefix += "_" + colstr + "_";
      else if( format.groups.find( group )->second.has_repeats )
         group_prefix += "_";

      for( size_t j = 0; j < format.fields.size( ); j++ )
      {
         if( group != format.fields[ j ].group )
            continue;

         if( !has_permission( format.fields[ j ].rest, format.fields[ j ].perm, permissions ) )
            continue;

         if( row == first )
            ++num_field_cols;
         else if( col == 0 && row > previous )
            columns_processed.clear( );

         previous = row;

         float x = format.fields[ j ].x + x_offset;
         float y = format.fields[ j ].y + y_offset;

         if( row == first && text_overflow_present )
            y = y_offset;

         if( is_normal_group )
         {
            if( format.fields[ j ].x_is_offset )
               x += group_x + group_padding_left;

            if( format.fields[ j ].y_is_offset )
               y += group_y + group_padding_top;
         }
         else
         {
            if( !perform_relative_adjustment( format.groups.find( group )->second,
             format.fields[ j ], group_boundaries, x, y, -1.0, page_width, page_height ) )
               continue;
         }

         float width = format.fields[ j ].width;
         float height = format.fields[ j ].height;

         if( width < 0 )
            width = page_width;

         if( height < 0 )
            height = page_height;

         float border = format.fields[ j ].border;

         bool field_border_shadow = format.fields[ j ].border_shadow;

         pdf_gen_field_type type = format.fields[ j ].type;
         pdf_gen_field_size size = format.fields[ j ].size;

         bool is_growable = ( size == e_pdf_gen_field_size_growable );

         bool field_is_top_pinned = format.fields[ j ].is_top_pinned;

         bool is_wshrinkable
          = ( size == e_pdf_gen_field_size_shrinkable || size == e_pdf_gen_field_size_shrinkable_width );

         bool is_hshrinkable
          = ( size == e_pdf_gen_field_size_shrinkable || size == e_pdf_gen_field_size_shrinkable_height );

         string row_prefix;
         if( format.groups.find( group )->second.has_repeats )
            row_prefix = repeat_prefix + group_repeat_string( repeat_num ) + "_";

         string data( row_prefix + group_prefix + format.fields[ j ].data );

         // NOTE: If a field belongs to a group but no variable with the group prefix was
         // found then if a variable without the group prefix exists then use it instead.
         // If a variable without the group prefix exists as a "dynamic variable" (all of
         // which should start with an unambiguous prefix) its value will take precedence.
         if( variables.count( data ) )
            data = variables.find( data )->second;
         else if( dynamic_variables.count( format.fields[ j ].data ) )
            data = dynamic_variables.find( format.fields[ j ].data )->second;
         else if( variables.count( row_prefix + format.fields[ j ].data ) )
            data = variables.find( row_prefix + format.fields[ j ].data )->second;
         else if( variables.count( format.fields[ j ].data ) )
            data = variables.find( format.fields[ j ].data )->second;

         if( format.fields[ j ].data_type == e_pdf_gen_field_data_type_boolean )
         {
            if( atoi( data.c_str( ) ) )
            {
               if( variables.count( c_true_variable ) )
                  data = variables.find( c_true_variable )->second;
            }
            else
            {
               if( variables.count( c_false_variable ) )
                  data = variables.find( c_false_variable )->second;
            }
         }

         if( !data.empty( ) && !format.fields[ j ].mask.empty( ) )
         {
            switch( format.fields[ j ].data_type )
            {
               case e_pdf_gen_field_data_type_date:
               data = format_udate( udate( data ), format.fields[ j ].mask );
               break;

               case e_pdf_gen_field_data_type_time:
               data = format_mtime( mtime( data ), format.fields[ j ].mask );
               break;

               case e_pdf_gen_field_data_type_integer:
               case e_pdf_gen_field_data_type_numeric:
               data = format_numeric( numeric( data.c_str( ) ), format.fields[ j ].mask );
               break;

               case e_pdf_gen_field_data_type_date_time:
               {
                  string::size_type pos = format.fields[ j ].mask.find( ' ' );
                  if( pos == string::npos )
                     pos = format.fields[ j ].mask.find( '_' );

                  if( pos != string::npos )
                     data = format_date_time( date_time( data ),
                      format.fields[ j ].mask.substr( 0, pos ), format.fields[ j ].mask.substr( pos ) );
               }
               break;
            }
         }

         // NOTE: If a field is marked as to be used for totalling then provided that it has not already
         // been processed for this repeat then add its current value to the total value.
         if( format.fields[ j ].use_for_totalling && format.fields[ j ].last_repeat_num < repeat_num )
         {
            format.fields[ j ].last_repeat_num = repeat_num;

            string total_dyn_var( "@@total_" + format.fields[ j ].data );
            if( dynamic_variables.count( total_dyn_var ) )
            {
               string val( dynamic_variables.find( total_dyn_var )->second );
               if( val.empty( ) )
                  val = "0";

               numeric total( val.c_str( ) );
               total += numeric( data.empty( ) ? "0" : data.c_str( ) );

               dynamic_variables[ total_dyn_var ] = to_string( total );
            }

            string next_parent( format.groups.find( group )->second.par_group );
            while( !next_parent.empty( ) )
            {
               string total_dyn_var( "@@total_" + next_parent + "_" + format.fields[ j ].data );

               if( dynamic_variables.count( total_dyn_var ) )
               {
                  string val( dynamic_variables.find( total_dyn_var )->second );
                  if( val.empty( ) )
                     val = "0";

                  numeric total( val.c_str( ) );
                  total += numeric( data.empty( ) ? "0" : data.c_str( ) );

                  dynamic_variables[ total_dyn_var ] = to_string( total );
               }

               next_parent = format.groups.find( next_parent )->second.par_group;
            }
         }

         string group_field_col_key( group + "_" + to_string( j ) + "_" + to_string( col ) );

         if( format.fields[ j ].is_external_data
          && !group_text_overflows.count( group_field_col_key )
          && data.size( ) > 4 && data.substr( 0, 4 ) == "http" )
         {
            string temp_file_name( "~" + uuid( ).as_string( ) );

            string cmd( "curl -s \"" + data + "\" >" + temp_file_name );

            int rc = system( cmd.c_str( ) );
            ( void )rc;

            data = buffer_file( temp_file_name );
            file_remove( temp_file_name );

            if( !data.empty( ) && data[ data.length( ) - 1 ] == '\n' )
               data.erase( data.length( ) - 1 );
         }

         if( format.fields[ j ].is_indirect_data )
         {
            if( variables.count( data ) )
               data = variables.find( data )->second;
         }

         if( format.fields[ j ].blank_zero_numeric )
         {
            if( data.find( '0' ) != string::npos
             && data.find_first_of( "123456789" ) == string::npos )
               data.erase( );
         }

         if( !format.fields[ j ].prefix.empty( ) )
         {
            string prefix( format.fields[ j ].prefix );

            if( variables.count( prefix ) )
               prefix = variables.find( prefix )->second + " ";
            else if( dynamic_variables.count( prefix ) )
               prefix = dynamic_variables.find( prefix )->second + " ";
            else if( variables.count( row_prefix + group_prefix + prefix ) )
               prefix = variables.find( row_prefix + group_prefix + prefix )->second + " ";
            else if( dynamic_variables.count( row_prefix + group_prefix + prefix ) )
               prefix = dynamic_variables.find( row_prefix + group_prefix + prefix )->second + " ";

            data = prefix + data;

            // NOTE: If the prefix begins with @ then it is being
            // assumed that the prefix and data concatenated will
            // be another variable name (for enumerated types).
            if( prefix[ 0 ] == '@' )
            {
               if( variables.count( data ) )
                  data = variables.find( data )->second;
               else if( dynamic_variables.count( data ) )
                  data = dynamic_variables.find( data )->second;
            }
         }

         if( !format.fields[ j ].suffix.empty( ) )
         {
            string suffix( format.fields[ j ].suffix );

            if( variables.count( suffix ) )
               suffix = " " + variables.find( suffix )->second;
            else if( dynamic_variables.count( suffix ) )
               suffix = " " + dynamic_variables.find( suffix )->second;
            else if( variables.count( row_prefix + group_prefix + suffix ) )
               suffix = " " + variables.find( row_prefix + group_prefix + suffix )->second;
            else if( dynamic_variables.count( row_prefix + group_prefix + suffix ) )
               suffix = " " + dynamic_variables.find( row_prefix + group_prefix + suffix )->second;
            else if( suffix == " (uom)" )
            {
               string uom_symbol( format.fields[ j ].data + "_" + suffix.substr( 1 ) );

               if( variables.count( uom_symbol ) )
                  suffix = " " + variables.find( uom_symbol )->second;
            }

            data += suffix;
         }

         string initial_data( data );

         if( type == e_pdf_gen_field_type_box )
         {
            float red = ( format.fields[ j ].color_r / 255.0 );
            float green = ( format.fields[ j ].color_g / 255.0 );
            float blue = ( format.fields[ j ].color_b / 255.0 );

            // NOTE: To support simple box definitions if no color was provided and
            // no border width then will assume a border width of 0.5 for a stroke.
            if( format.fields[ j ].had_color )
               page.set_rgb_fill( red, green, blue );
            else if( border <= 0.0 )
               border = 0.5;

            if( border > 0.0 )
            {
               page.set_line_width( border );
               float red = ( format.fields[ j ].bcolor_r / 255.0 );
               float green = ( format.fields[ j ].bcolor_g / 255.0 );
               float blue = ( format.fields[ j ].bcolor_b / 255.0 );

               page.set_rgb_stroke( red, green, blue );
               page.set_line_width( border );
            }

            page.rectangle( x, page_height - y - height, width, height );

            if( !format.fields[ j ].had_color )
               page.stroke( );
            else
            {
               if( border <= 0.0 )
                  page.fill( );
               else
                  page.fill_and_stroke( );
            }

            if( border >= 0.0 && field_border_shadow )
            {
               page.set_grey_stroke( 0 );
               page.set_line_width( border / 2.0 );

               float border_shadow( border * 2.5 );

               page.set_grey_stroke( 0 );
               page.set_line_width( border_shadow );

               page.move_to( x + border, page_height - ( y + height + border ) );
               page.line_to( x + width + border, page_height - ( y + height + border ) );
               page.line_to( x + width + border, page_height - ( y + border ) );

               page.stroke( );
            }
         }
         else if( type == e_pdf_gen_field_type_line )
         {
            float check_height = y_offset + group_y + group_padding_top + group_standard_row_height;

            if( is_normal_group && ( check_height > normal_group_page_height ) )
            {
               // NOTE: If this line is actually intended to be a group footer then
               // use its border and colors in order to extend a group border to the
               // bottom of the available page space.
               if( format.fields[ j ].use_for_group_overflow )
               {
                  has_group_bg = true;
                  group_border = format.fields[ j ].border;

                  group_bcolor_r = ( format.fields[ j ].bcolor_r / 255.0 );
                  group_bcolor_g = ( format.fields[ j ].bcolor_g / 255.0 );
                  group_bcolor_b = ( format.fields[ j ].bcolor_b / 255.0 );

                  group_bgcolor_r = ( format.fields[ j ].color_r / 255.0 );
                  group_bgcolor_g = ( format.fields[ j ].color_g / 255.0 );
                  group_bgcolor_b = ( format.fields[ j ].color_b / 255.0 );
               }

               ++num_field_cols_skipped;
               had_row_overflow = true;
               continue;
            }

            if( next_top < 0 || y < next_top )
               next_top = y;

            if( next_left < 0 || x < next_left )
               next_left = x;

            if( x + width > next_right )
               next_right = x + width;

            if( y + height > next_bottom )
               next_bottom = y + height;

            float border( format.fields[ j ].border );
            float adjust = ( format.fields[ j ].height / 2.0 ) - ( border / 3.0 );

            if( format.fields[ j ].line_with_round_ends )
            {
               page.set_line_cap( e_line_cap_round_end );
               x += adjust;
               width -= ( adjust * 2.0 );

               y += adjust;
               height -= ( adjust * 2.0 );
            }

            if( border > 0 )
            {
               if( field_border_shadow )
               {
                  page.set_grey_stroke( 0 );
                  page.set_line_width( format.fields[ j ].height );

                  page.move_to( x + border, page_height - ( y + border ) );
                  page.line_to( x + width + border, page_height - ( y + border ) );

                  page.stroke( );
               }

               float red = ( format.fields[ j ].bcolor_r / 255.0 );
               float green = ( format.fields[ j ].bcolor_g / 255.0 );
               float blue = ( format.fields[ j ].bcolor_b / 255.0 );

               page.set_rgb_stroke( red, green, blue );
               page.set_line_width( format.fields[ j ].height );

               page.move_to( x, page_height - y );
               page.line_to( x + width, page_height - y );

               page.stroke( );
            }

            float red = ( format.fields[ j ].color_r / 255.0 );
            float green = ( format.fields[ j ].color_g / 255.0 );
            float blue = ( format.fields[ j ].color_b / 255.0 );

            page.set_rgb_stroke( red, green, blue );
            page.set_line_width( format.fields[ j ].height - ( border * 2.0 ) );

            page.move_to( x, page_height - y );
            page.line_to( x + width, page_height - y );

            page.stroke( );

            x -= adjust;
            width += ( adjust * 2.0 );

            y -= adjust;
            height += ( adjust * 2.0 );

            page.set_line_cap( e_line_cap_butt_end );

            if( format.fields[ j ].fill_and_square_top )
            {
               page.set_rgb_fill( red, green, blue );

               // KLUDGE: For some reason the top part of the rounded line ends up going outside the
               // expected normal area so need to extend the fill a little higher to cover the mess.
               page.rectangle( x + ( border / 2.0 ),
                page_height - ( y + height / 2.0 ), width - border, ( height / 2.0 ) + ( border / 3.0 ) );

               page.fill( );

               if( border > 0 )
               {
                  if( field_border_shadow )
                  {
                     page.set_grey_stroke( 0 );
                     page.set_line_width( border * 1.25 );

                     if( is_page_overflow )
                        page.move_to( x + width + border, page_height - y );
                     else
                        page.move_to( x + width + border, page_height - ( y + border ) );

                     page.line_to( x + width + border, page_height - ( y + ( height / 2.0 ) + border ) );

                     page.stroke( );
                  }

                  float red = ( format.fields[ j ].bcolor_r / 255.0 );
                  float green = ( format.fields[ j ].bcolor_g / 255.0 );
                  float blue = ( format.fields[ j ].bcolor_b / 255.0 );

                  page.set_rgb_stroke( red, green, blue );
                  page.set_line_width( border );

                  page.move_to( x, page_height - y );
                  page.line_to( x, page_height - ( y + height / 2.0 ) );

                  page.move_to( x + width, page_height - y );
                  page.line_to( x + width, page_height - ( y + height / 2.0 ) );

                  page.stroke( );
               }
            }

            if( format.fields[ j ].fill_and_square_bottom )
            {
               page.set_rgb_fill( red, green, blue );
               page.rectangle( x, page_height - ( y + height ), width, ( height / 2.0 ) );

               page.fill( );

               if( border > 0 )
               {
                  if( field_border_shadow )
                  {
                     page.set_grey_stroke( 0 );
                     page.set_line_width( border * 1.5 );

                     page.move_to( x + width + border, page_height - ( y + height + border ) );
                     page.line_to( x + width + border, page_height - ( y + ( height / 2.0 ) + border ) );

                     page.stroke( );
                  }

                  float red = ( format.fields[ j ].bcolor_r / 255.0 );
                  float green = ( format.fields[ j ].bcolor_g / 255.0 );
                  float blue = ( format.fields[ j ].bcolor_b / 255.0 );

                  page.set_rgb_stroke( red, green, blue );
                  page.set_line_width( border );

                  page.move_to( x, page_height - ( y + height ) );
                  page.line_to( x, page_height - ( y + height / 2.0 ) );

                  page.move_to( x + width, page_height - ( y + height ) );
                  page.line_to( x + width, page_height - ( y + height / 2.0 ) );

                  page.stroke( );
               }
            }
         }
         else if( type == e_pdf_gen_field_type_image )
         {
            if( row == first && group_cols_processed[ group ].count( group_field_col_key ) )
               continue;

            float image_width = 0.0;
            float image_height = 0.0;

            float padding_left = 0.0;
            float padding_right = 0.0;

            float padding_top = 0.0;
            float padding_bottom = 0.0;

            if( format.fields[ j ].use_padding )
            {
               padding_left = ( int )format.fields[ j ].padding_left;
               padding_right = ( int )format.fields[ j ].padding_right;
               padding_top = ( int )format.fields[ j ].padding_top;
               padding_bottom = ( int )format.fields[ j ].padding_bottom;
            }

            float available_width = width - padding_left - padding_right;
            float available_height = height - padding_top - padding_bottom;

            float image_x = x;
            float image_y = y;

            pdf_image* p_image = 0;

            if( !data.empty( ) )
            {
               pdf_gen_field_alignx alignx = format.fields[ j ].alignx;
               pdf_gen_field_aligny aligny = format.fields[ j ].aligny;

               if( data.size( ) > 4 && data.substr( 0, 4 ) == "http" )
               {
                  if( images.count( data ) )
                     p_image = images[ data ].get( );
                  else
                  {
                     string temp_file_name = "~" + uuid( ).as_string( );
                     if( format.fields[ j ].is_png )
                        temp_file_name += ".png";
                     else if( format.fields[ j ].is_jpg )
                        temp_file_name += ".jpg";
                     else
                        throw runtime_error( "no file type specified for URL image '" + data + "'" );

                     string cmd( "curl -s \"" + data + "\" >" + temp_file_name );

                     int rc = system( cmd.c_str( ) );
                     ( void )rc;

                     auto_ptr< pdf_image > ap_image;

                     if( format.fields[ j ].is_png )
                        ap_image.reset( new pdf_png_image( doc, temp_file_name ) );
                     else if( format.fields[ j ].is_jpg )
                        ap_image.reset( new pdf_jpeg_image( doc, temp_file_name ) );

                     temporary_image_files.push_back( temp_file_name );

                     images.insert( make_pair( data, p_image = ap_image.release( ) ) );
                  }
               }
               else
               {
                  string::size_type ext_pos = data.find_last_of( "." );
                  if( ext_pos == string::npos )
                     throw runtime_error( "invalid image file name '" + data + "' in field #" + to_string( j ) );

                  string ext = lower( data.substr( ext_pos + 1 ) );

                  if( images.count( data ) )
                     p_image = images[ data ].get( );
                  else
                  {
                     auto_ptr< pdf_image > ap_image;

                     if( ext == "png" )
                        ap_image.reset( new pdf_png_image( doc, data ) );
                     else if( ext == "jpg" || ext == "jpeg" )
                        ap_image.reset( new pdf_jpeg_image( doc, data ) );
                     else
                        throw runtime_error( "unsupported image type '" + ext + "' in field #" + to_string( j ) );

                     images.insert( make_pair( data, p_image = ap_image.release( ) ) );
                  }
               }

               image_width = min( available_width, ( float )p_image->get_width( ) );
               image_height = min( available_height, ( float )p_image->get_height( ) );

               if( is_growable )
               {
                  if( p_image->get_width( ) > available_width )
                  {
                     image_width = ( float )p_image->get_width( );
                     width = image_width + padding_left + padding_right;
                  }

                  if( p_image->get_height( ) > available_height )
                  {
                     image_height = ( float )p_image->get_height( );
                     height = image_height + padding_top + padding_bottom;
                  }
               }

               if( is_wshrinkable && available_width > image_width )
                  available_width = image_width;

               if( is_hshrinkable && available_height > image_height )
                  available_height = image_height;

               if( image_width < available_width && alignx != e_pdf_gen_field_alignx_left )
               {
                  if( alignx == e_pdf_gen_field_alignx_right )
                     image_x += available_width - image_width;
                  else if( alignx == e_pdf_gen_field_alignx_center )
                     image_x += ( available_width - image_width ) / 2;
               }

               if( image_height < available_height && aligny != e_pdf_gen_field_aligny_top )
               {
                  if( aligny == e_pdf_gen_field_aligny_bottom )
                     image_y += available_height - image_height;
                  else if( aligny == e_pdf_gen_field_aligny_center )
                     image_y += ( available_height - image_height ) / 2;
               }
            }

            float actual_width = ( !is_wshrinkable ? width : image_width + padding_left + padding_right );
            float actual_height = ( !is_hshrinkable ? height : image_height + padding_top + padding_bottom );

            float check_height = y_offset + group_y + group_padding_top + group_standard_row_height;

            if( is_normal_group && ( check_height > normal_group_page_height ) )
            {
               ++num_field_cols_skipped;
               had_row_overflow = true;
               continue;
            }

            if( is_normal_group )
               columns_processed.insert( group_field_col_key );

            if( next_top < 0 || y < next_top )
               next_top = y;

            if( next_left < 0 || x < next_left )
               next_left = x;

            if( x + width > next_right )
               next_right = x + width;

            if( y + height > next_bottom )
               next_bottom = y + height;

            float current_height = next_bottom - next_top;

            // NOTE: If the group has a background color and the new height will be greater than the
            // current background fill then extend the background fill to the standard row height.
            if( has_group_bg && y + actual_height > group_bg_y )
            {
               page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

               float height_extension = ( y + actual_height ) - group_bg_y;

               if( row > 0 && col == 0 && next_left < 0 )
                  height_extension += paddingy;

               if( is_normal_group && ( group_bg_y + height_extension > normal_group_page_height ) )
                  height_extension = normal_group_page_height - group_bg_y;

               if( height_extension > 0.0 )
               {
                  page.rectangle( group_bg_x,
                   page_height - ( group_bg_y + height_extension ), group_width, height_extension );

                  page.fill( );

                  group_bg_y += height_extension;
                  group_row_height += height_extension;
               }
            }

            if( format.fields[ j ].bgcolor_r >= 0 )
            {
               float red = ( format.fields[ j ].bgcolor_r / 255.0 );
               float green = ( format.fields[ j ].bgcolor_g / 255.0 );
               float blue = ( format.fields[ j ].bgcolor_b / 255.0 );

               page.set_rgb_fill( red, green, blue );

               float bg_width = actual_width;
               float bg_height = actual_height;

               page.rectangle( x, page_height - ( y + bg_height ), bg_width, bg_height );
               page.fill( );
            }

            if( p_image )
            {
               float adjusted_bottom = image_y + image_height + padding_top;

               page.draw_image( *p_image, image_x + padding_left,
                page_height - ( adjusted_bottom ), ( int )image_width, ( int )image_height );
            }
            else
            {
               // NOTE: If no image then output a black cross instead. For fixed size fields the size of
               // the cross will be the full image size otherwise it will be sized at half the full size.
               image_width = ( is_wshrinkable ? padding_left + padding_right : width );
               image_height = ( is_hshrinkable ? padding_top + padding_bottom : height );

               if( image_width && image_height )
               {
                  page.set_grey_stroke( 0 );
                  page.set_line_width( 0.25 );

                  page.move_to( x, page_height - y );
                  page.line_to( x + image_width, page_height - ( y + image_height ) );

                  page.move_to( x + image_width, page_height - y );
                  page.line_to( x, page_height - ( y + image_height ) );

                  page.stroke( );
               }
            }

            if( border > 0 )
            {
               float red = ( format.fields[ j ].bcolor_r / 255.0 );
               float green = ( format.fields[ j ].bcolor_g / 255.0 );
               float blue = ( format.fields[ j ].bcolor_b / 255.0 );

               page.set_rgb_stroke( red, green, blue );

               float border_width = actual_width;
               float border_height = actual_height;

               bool has_left = format.fields[ j ].border_left;
               bool has_right = format.fields[ j ].border_right;
               bool has_top = format.fields[ j ].border_top;
               bool has_bottom = format.fields[ j ].border_bottom;

               page.set_line_width( border );

               if( has_left && has_right && has_top && has_bottom )
                  page.rectangle( x, page_height - ( y + border_height ), border_width, border_height );
               else
               {
                  page.move_to( x, page_height - ( y + border_height ) );

                  if( has_left )
                     page.line_to( x, page_height - y );
                  else
                     page.move_to( x, page_height - y );

                  if( has_top )
                     page.line_to( x + border_width, page_height - y );
                  else
                     page.move_to( x + border_width, page_height - y );

                  if( has_right )
                     page.line_to( x + border_width, page_height - ( y + border_height ) );
                  else
                     page.move_to( x + border_width, page_height - ( y + border_height ) );

                  if( has_bottom )
                     page.line_to( x, page_height - ( y + border_height ) );
               }

               page.stroke( );

               if( field_border_shadow && ( has_right || has_bottom ) )
               {
                  float border_shadow( border * 2.5 );

                  page.set_grey_stroke( 0 );
                  page.set_line_width( border_shadow );

                  page.move_to( x + border_width + border, page_height - ( y + border_shadow ) );

                  if( has_right )
                     page.line_to( x + border_width + border, page_height - ( y + border_height + border ) );
                  else
                     page.move_to( x + border_width + border, page_height - ( y + border_height + border ) );

                  if( has_bottom )
                     page.line_to( x + border_shadow, page_height - ( y + border_height + border ) );

                  page.stroke( );
               }
            }
         }
         else if( type == e_pdf_gen_field_type_text )
         {
            bool is_continuation = false;
            if( row == first && ( text_overflow_present || !group_cols_processed[ group ].empty( ) ) )
            {
               bool okay = true;
               if( group_cols_processed[ group ].count( group_field_col_key ) )
                  okay = false;
               else
               {
                  string text_overflow_key( group_field_col_key );
                  if( group_text_overflows.count( text_overflow_key ) )
                  {
                     is_hshrinkable = true;
                     is_continuation = true;
                     data = group_text_overflows[ text_overflow_key ];
                     group_text_overflows.erase( text_overflow_key );
                  }
               }

               if( !okay )
                  continue;
            }

            if( !is_continuation && !format.fields[ j ].special.empty( ) )
            {
               // FUTURE: Rather than just extracting the text from HTML it would
               // be much better if some sort of more sophisticated approach that
               // might permit the usage of different font weightings or similar.
               if( format.fields[ j ].special == string( c_special_format_html ) )
                  data = extract_text_from_html( data );
            }

            if( !is_continuation
             && format.fields[ j ].truncate >= 0 && format.fields[ j ].truncate != 1 ) // i.e. none
            {
               int character_trunc_limit = format.fields[ j ].truncate;

               if( character_trunc_limit == 0 ) // i.e. standard
                  character_trunc_limit = c_default_character_trunc_limit;

               utf8_truncate( data, character_trunc_limit, "..." );
            }

            string font( format.fields[ j ].font );
            if( font.empty( ) )
               font = default_font;

            if( !fonts.count( font ) )
               throw runtime_error( "unknown font '" + font + "' in field #" + to_string( j ) );

            pdf_font& text_font( *fonts[ font ].first );

            y += fonts[ font ].second.yadjust;

            page.set_font_and_size( text_font, fonts[ font ].second.size );

            float descent = text_font.get_descent( ) / 100.0;
            float cap_height = text_font.get_cap_height( ) / 100.0;

            float font_size_multiplier = ( fonts[ font ].second.size / 10.0 );

            descent *= font_size_multiplier;
            cap_height *= font_size_multiplier;

            float padding_left = 0.0;
            float padding_right = 0.0;

            float padding_top = font_size_multiplier;
            float padding_bottom = 0.0;

            if( format.fields[ j ].use_padding )
            {
               padding_left = format.fields[ j ].padding_left;
               padding_right = format.fields[ j ].padding_right;
               padding_top = format.fields[ j ].padding_top;
               padding_bottom = format.fields[ j ].padding_bottom;
            }

            // NOTE: If continuing text overflow put a small amount of padding at the top of the page
            // (unless no actual text is left as just the bottom padding and borders are output then).
            if( row == first && text_overflow_present )
               padding_top = data.empty( ) ? 0 : descent * -1;

            float line_height = cap_height - descent;
            float line_spacing = format.fields[ j ].line_spacing;

            float total_height = padding_top + padding_bottom;
            float check_height = y_offset + group_y + group_padding_top;

            // NOTE: Include the full height of the border if pinned (otherwise the padding will end
            // up appearing below the normal group page height.
            if( field_is_top_pinned )
               check_height += ( border * 2.0 );
            else
               check_height += group_standard_row_height;

            if( is_normal_group && ( check_height > normal_group_page_height ) )
            {
               ++num_field_cols_skipped;
               had_row_overflow = true;
               continue;
            }

            if( is_normal_group )
               columns_processed.insert( group_field_col_key );

            bool using_wide_chars = false;

#ifdef ICONV_SUPPORT
            decode_text( data, text_font, is_continuation, &using_wide_chars );
#endif

            float red = ( format.fields[ j ].color_r / 255.0 );
            float green = ( format.fields[ j ].color_g / 255.0 );
            float blue = ( format.fields[ j ].color_b / 255.0 );

            float max_width = 0;
            float max_height = 0;

            text_align align = e_text_align_left;

            switch( format.fields[ j ].alignment )
            {
               case e_pdf_gen_field_alignment_left:
               align = e_text_align_left;
               break;

               case e_pdf_gen_field_alignment_right:
               align = e_text_align_right;
               break;

               case e_pdf_gen_field_alignment_center:
               align = e_text_align_center;
               break;

               case e_pdf_gen_field_alignment_justify:
               align = e_text_align_justify;
               break;
            }

            float text_y = y + padding_top;
            float text_x = x + padding_left;

            bool skip_standard_row_bg = false;

            if( row == first && text_overflow_present )
               skip_standard_row_bg = true;

            // NOTE: If the group has a background color (and this isn't text overflow)
            // then extend the background fill to the standard row height.
            if( has_group_bg && !skip_standard_row_bg && col == 0 && next_left < 0 )
            {
               page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

               float height_extension = group_standard_row_height;

               if( row > 0 )
                  height_extension += paddingy;

               if( is_normal_group && ( group_bg_y + height_extension > normal_group_page_height ) )
                  height_extension = normal_group_page_height - group_bg_y;

               if( height_extension > 0.0 )
               {
                  page.rectangle( group_bg_x,
                   page_height - ( group_bg_y + height_extension ), group_width, height_extension );

                  page.fill( );

                  group_bg_y += height_extension;
                  group_row_height += height_extension;
               }
            }

            float text_overflow_amt = 0.0;
            bool has_text_overflow = false;
            bool has_bottom_padding_only = false;

            for( int i = 0; i < height; i++ )
            {
               float text_width = 0;
               size_t wrap_fit_pos = 0;
               size_t wrap_over_pos = string::npos;

               float available_width = width - padding_left - padding_right;

               // NOTE: If text overflow occurred but was only due to the bottom padding
               // not fitting then no text will be output (just the bottom padding afer).
               if( row == first && text_overflow_present && data.empty( ) )
               {
                  has_bottom_padding_only = true;
                  break;
               }

               // FUTURE: The ellipsis appending should also be supported for wide character
               // sets but this requires using the appropriate wide characters (use iconv?).
               // Also the detection of CR/LF needs to be implemented for wide chars below.
               bool is_last_line = !is_growable && !using_wide_chars && ( i == height - 1 );

               string ellipsis( "..." );

               // NOTE: Before considering the need for an ellipsis first check if the whole
               // of the remaining text will fit in the available width.
               if( is_last_line )
               {
                  float next_width = page.text_width( data );

                  if( next_width <= available_width )
                  {
                     wrap_fit_pos = data.length( );
                     text_width = next_width;
                  }
               }

               // NOTE: Determine positions at and after where text wrapping must occur.
               if( wrap_fit_pos == 0 )
               {
                  for( size_t pos = ( using_wide_chars ? 2 : 1 );
                   pos <= data.length( ); pos += ( using_wide_chars ? 2 : 1 ) )
                  {
                     float next_width = page.text_width(
                      data.substr( 0, pos ) + ( is_last_line ? ellipsis : string( ) ) );

                     // FUTURE: (see above FUTURE note)
                     if( is_last_line && ( data[ pos - 1 ] == '\r' || data[ pos - 1 ] == '\n' ) )
                     {
                        wrap_over_pos = pos;
                        break;
                     }

                     if( next_width <= available_width )
                     {
                        wrap_fit_pos = pos;
                        text_width = next_width;
                     }
                     else
                     {
                        wrap_over_pos = pos;
                        break;
                     }
                  }
               }

               max_height += line_height;
               total_height += line_height;

               if( i > 0 )
               {
                  text_y += line_spacing;
                  max_height += line_spacing;
                  total_height += line_spacing;
               }

               // NOTE: If text has grown beyond the standard row height (or if is text overflow)
               // then extend the background fill to the height of the text.
               if( has_group_bg && y + total_height > group_bg_y )
               {
                  page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

                  float height_extension = ( y + total_height ) - group_bg_y;

                  if( is_normal_group && ( group_bg_y + height_extension > normal_group_page_height ) )
                     height_extension = normal_group_page_height - group_bg_y;

                  if( height_extension > 0.0 )
                  {
                     page.rectangle( group_bg_x,
                      page_height - ( group_bg_y + height_extension ), group_width, height_extension );

                     page.fill( );

                     group_bg_y += height_extension;
                     group_row_height += height_extension;
                  }
               }

               if( i == 0 && format.fields[ j ].bgcolor_r >= 0 && padding_top > 0 )
               {
                  float bg_red = ( format.fields[ j ].bgcolor_r / 255.0 );
                  float bg_green = ( format.fields[ j ].bgcolor_g / 255.0 );
                  float bg_blue = ( format.fields[ j ].bgcolor_b / 255.0 );

                  page.set_rgb_fill( bg_red, bg_green, bg_blue );

                  float border_width = width;
                  float border_height = padding_top;

                  if( is_normal_group && ( y + border_height > normal_group_page_height ) )
                     border_height = normal_group_page_height - y;

                  if( field_border_shadow )
                  {
                     float border_shadow = border * 2.5;

                     border_width -= border_shadow;
                  }

                  page.rectangle( x, page_height - ( y + border_height ), border_width, border_height );

                  page.fill( );
               }

               if( format.fields[ j ].bgcolor_r >= 0 )
               {
                  float bg_red = ( format.fields[ j ].bgcolor_r / 255.0 );
                  float bg_green = ( format.fields[ j ].bgcolor_g / 255.0 );
                  float bg_blue = ( format.fields[ j ].bgcolor_b / 255.0 );

                  page.set_rgb_fill( bg_red, bg_green, bg_blue );

                  float height_extension = line_height;

                  if( is_normal_group && ( text_y + height_extension > normal_group_page_height ) )
                     height_extension = normal_group_page_height - text_y;

                  if( height_extension > 0.0 )
                  {
                     page.rectangle( text_x - padding_left,
                      page_height - ( text_y + height_extension ), width, height_extension );

                     page.fill( );
                  }
               }

               // NOTE: Need to check the size in case a "top-pinned" field with no room for any text
               // is present (as the background and borders still need to be output).
               if( !data.empty( ) && ( !is_normal_group || text_y + line_height < normal_group_page_height ) )
               {
                  page.set_rgb_fill( red, green, blue );

                  string wrap_fit_data( data.substr( 0, wrap_fit_pos ) );
                  string wrap_over_data( data.substr( 0, wrap_over_pos ) );

                  bool force_wrap_fit = false;
                  if( is_last_line && wrap_fit_data.length( ) < wrap_over_data.length( ) )
                  {
                     force_wrap_fit = true;
                     wrap_fit_data += ellipsis;
                  }

                  unsigned int len = 0;

                  // NOTE: If the field is intended to output the total page count then it cannot be
                  // processed until all pages have been processed so instead simply store the state
                  // so that the font and colors are remembered when it is actually processed.
                  if( format.fields[ j ].data == "@@page_count" )
                  {
                     page.save_state( );
                     if( page_count_info.top < 0.0 )
                     {
                        page_count_info.top = page_height - ( text_y + descent );
                        page_count_info.left = text_x;
                        page_count_info.bottom = page_height - ( text_y + line_height );
                        page_count_info.right = text_x + width - padding_left - padding_right;

                        page_count_info.alignment = align;
                     }
                  }
                  else
                  {
                     if( !force_wrap_fit )
                     {
                        page.output_text( wrap_over_data, page_height - ( text_y + descent ), text_x,
                         page_height - ( text_y + line_height ), text_x + width - padding_left - padding_right, align, &len );

                        // NOTE: If an "empty" line is present then remove the line spacing (otherwise it is
                        // effectively doubled which does not produce very desirable output).
                        if( wrap_over_data.find( "\n\n" ) != string::npos )
                        {
                           text_y -= line_spacing;
                           max_height -= line_spacing;
                           total_height -= line_spacing;
                        }
                     }

                     // NOTE: If the text cannot be wrapped then just output as much text as is possible.
                     if( len == 0 )
                        page.output_text( wrap_fit_data, page_height - ( text_y + descent ), text_x,
                         page_height - ( text_y + line_height ), text_x + width - padding_left - padding_right, align, &len );

                     page.measure_text( wrap_over_data.substr( 0, len ), text_x + width - padding_right, align, &text_width );

                     if( text_width > max_width )
                        max_width = text_width;
                  }

                  if( len >= data.size( ) || len == 0 && !data.empty( ) )
                     data.erase( );
                  else
                  {
                     data.erase( 0, len );

                     // KLUDGE: It seems that the width returned by "measure_text" is not quite correct when
                     // text wraps using "justify" so just assume that if text has wrapped it is full width.
                     if( align == e_text_align_justify )
                        max_width = width;
                  }
               }

               // NOTE: If no text left but no room for padding then treat this as though "text overflow"
               // had occurred so that the bottom and padding and borders can be output in the next page.
               if( is_normal_group && data.empty( )
                && text_y + line_height + padding_bottom > normal_group_page_height )
               {
                  last_row = row + 1;
                  padding_bottom = 0;

                  if( columns_processed.count( group_field_col_key ) )
                     columns_processed.erase( group_field_col_key );

                  has_text_overflow = had_text_overflow = true;
                  text_overflow_amt = normal_group_page_height - text_y - line_height;

                  group_text_overflows.insert(
                   make_pair( group + "_" + to_string( j ) + "_" + to_string( col ), "" ) );

                  break;
               }

               if( is_hshrinkable && data.empty( ) )
                  break;

               if( !data.empty( ) && is_growable && i == height - 1 )
                  ++height;

               text_y += line_height;

               // NOTE: If text overflow occurred but one last line of text fitted then store the
               // remaining overflow text here.
               if( has_text_overflow )
               {
                  group_text_overflows[ group + "_" + to_string( j ) + "_" + to_string( col ) ] = data;
                  break;
               }

               if( is_normal_group && !data.empty( )
                && ( text_y + line_height + padding_bottom > normal_group_page_height ) )
               {
                  has_text_overflow = is_growable;

                  if( has_text_overflow )
                  {
                     last_row = row + 1;
                     padding_bottom = 0;

                     had_text_overflow = true;

                     if( columns_processed.count( group_field_col_key ) )
                        columns_processed.erase( group_field_col_key );

                     text_overflow_amt = normal_group_page_height - text_y;

                     group_text_overflows.insert(
                      make_pair( group + "_" + to_string( j ) + "_" + to_string( col ), data ) );
                  }

                  // NOTE: If one last line of text can fit (i.e. the overflow is the padding)
                  // then continue for one more line.
                  if( text_y + line_height > normal_group_page_height )
                     break;
                  else
                     text_overflow_amt -= line_height;
               }
            }

            // NOTE: If the page header size has been designated as being "growable" then
            // if any field belonging to a "header" group is found which has grown beyond
            // the header size then extend it (so that "non header" fields following will
            // see this size as the header size).
            if( group_is_header
             && format.header_size_growable && !format.fields[ j ].group.empty( ) )
            {
               if( text_y > normal_group_page_top )
                  normal_group_page_top = text_y;
            }

            // NOTE: Special case for text overflow when only the bottom padding overflowed.
            if( has_bottom_padding_only && has_group_bg )
            {
               page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

               page.rectangle( group_bg_x,
                page_height - ( group_bg_y + padding_bottom ), group_width, padding_bottom );
               page.fill( );

               group_bg_y += padding_bottom;
            }

            // NOTE: If overflow is due to growable text then need to extend both the group and
            // field backgrounds to the bottom of the available page area.
            if( has_group_bg && has_text_overflow && y + total_height + line_height > group_bg_y )
            {
               page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

               float height_extension = ( y + total_height + line_height ) - group_bg_y;

               if( is_normal_group && ( group_bg_y + height_extension > normal_group_page_height ) )
                  height_extension = normal_group_page_height - group_bg_y;

               if( height_extension > 0.0 )
               {
                  page.rectangle( group_bg_x,
                   page_height - ( group_bg_y + height_extension ), group_width, height_extension );

                  page.fill( );

                  group_bg_y += height_extension;
                  group_row_height += height_extension;
               }
            }

            if( has_text_overflow && text_overflow_amt > 0.0 && format.fields[ j ].bgcolor_r >= 0 )
            {
               float bg_red = ( format.fields[ j ].bgcolor_r / 255.0 );
               float bg_green = ( format.fields[ j ].bgcolor_g / 255.0 );
               float bg_blue = ( format.fields[ j ].bgcolor_b / 255.0 );

               page.set_rgb_fill( bg_red, bg_green, bg_blue );

               page.rectangle( text_x - padding_left,
                page_height - normal_group_page_height, width, text_overflow_amt );

               page.fill( );
            }

            if( format.fields[ j ].bgcolor_r >= 0 && padding_bottom > 0 )
            {
               float bg_red = ( format.fields[ j ].bgcolor_r / 255.0 );
               float bg_green = ( format.fields[ j ].bgcolor_g / 255.0 );
               float bg_blue = ( format.fields[ j ].bgcolor_b / 255.0 );

               page.set_rgb_fill( bg_red, bg_green, bg_blue );

               float bg_width = width;
               float bg_height = padding_bottom;

               // NOTE: As the group background fill for the next row will start precisely after
               // highest fields height the bottom border (due to rounding) may actually end up
               // being very slightly lower (in which case it will get clipped) so for both the
               // field's background fill and its border shrink the height by the thickness of
               // the border to prevent this problem.
               if( format.fields[ j ].border_bottom )
                  bg_height -= border;

               if( field_border_shadow )
               {
                  float border_shadow = border * 2.5;

                  bg_width -= border_shadow;
                  bg_height -= border_shadow;
               }

               page.rectangle( x,
                page_height - ( y + max_height + padding_top + bg_height ), bg_width, bg_height );

               page.fill( );
            }

            float actual_width = ( !is_wshrinkable ? width : max_width + padding_left + padding_right );
            float actual_height = ( !is_hshrinkable ? total_height : max_height + padding_top + padding_bottom );

            if( next_top < 0 || y < next_top )
               next_top = y;

            if( next_left < 0 || x < next_left )
               next_left = x;

            if( x + width > next_right )
               next_right = x + width;

            if( y + total_height > next_bottom )
               next_bottom = y + total_height;

            if( is_wshrinkable && actual_width < width && align == e_text_align_center )
               x += ( width - actual_width ) / 2.0;

            if( border > 0 )
            {
               float red = ( format.fields[ j ].bcolor_r / 255.0 );
               float green = ( format.fields[ j ].bcolor_g / 255.0 );
               float blue = ( format.fields[ j ].bcolor_b / 255.0 );

               page.set_rgb_stroke( red, green, blue );

               float border_width = actual_width;
               float border_height = actual_height;

               // NOTE: Refer to group background clipping NOTE above.
               if( format.fields[ j ].border_bottom )
                  border_height -= border;

               float border_shadow = 0;
               if( field_border_shadow )
               {
                  border_shadow = border * 2.5;

                  border_width -= border_shadow;
                  border_height -= border_shadow;
               }

               page.set_line_width( border );

               bool has_left = format.fields[ j ].border_left;
               bool has_right = format.fields[ j ].border_right;
               bool has_top = format.fields[ j ].border_top;
               bool has_bottom = format.fields[ j ].border_bottom;

               if( has_text_overflow )
               {
                  page.move_to( x, page_height - normal_group_page_height );

                  if( has_left )
                     page.line_to( x, page_height - y );
                  else
                     page.move_to( x, page_height - y );

                  if( has_top && ( row != first || !is_page_overflow ) )
                     page.line_to( x + border_width, page_height - y );
                  else
                     page.move_to( x + border_width, page_height - y );

                  if( has_right )
                     page.line_to( x + border_width, page_height - normal_group_page_height );

                  page.stroke( );
               }
               else
               {
                  if( ( row == first && text_overflow_present )
                   || !has_left || !has_right || !has_top || !has_bottom )
                  {
                     page.move_to( x, page_height - y );

                     if( has_left )
                        page.line_to( x, page_height - ( y + border_height ) );
                     else
                        page.move_to( x, page_height - ( y + border_height ) );

                     if( has_bottom )
                        page.line_to( x + border_width, page_height - ( y + border_height ) );
                     else
                        page.move_to( x + border_width, page_height - ( y + border_height ) );

                     if( has_right )
                        page.line_to( x + border_width, page_height - y );
                     else
                        page.move_to( x + border_width, page_height - y );

                     if( has_top && !text_overflow_present )
                        page.line_to( x, page_height - y );

                     page.stroke( );
                  }
                  else
                  {
                     page.rectangle( x, page_height - ( y + border_height ), border_width, border_height );
                     page.stroke( );
                  }
               }

               if( border_shadow > 0 && ( has_right || has_bottom ) )
               {
                  page.set_grey_stroke( 0 );
                  page.set_line_width( border_shadow );

                  if( row == first && text_overflow_present )
                     page.move_to( x + border_width + border, page_height - y );
                  else
                     page.move_to( x + border_width + border, page_height - ( y + border_shadow ) );

                  if( has_text_overflow )
                  {
                     if( has_right )
                        page.line_to( x + border_width + border, page_height - normal_group_page_height );
                  }
                  else
                  {
                     if( has_right && !has_text_overflow )
                        page.line_to( x + border_width + border, page_height - ( y + border_height + border ) );
                     else
                        page.move_to( x + border_width + border, page_height - ( y + border_height + border ) );

                     if( has_bottom )
                        page.line_to( x + border_shadow, page_height - ( y + border_height + border ) );
                  }

                  page.stroke( );
               }
            }
         }

         if( !format.fields[ j ].clear_var.empty( ) )
            dynamic_variables[ format.fields[ j ].clear_var ] = "";

         if( !format.fields[ j ].assign_var.empty( ) )
            dynamic_variables[ format.fields[ j ].assign_var ] = initial_data;
      }

      if( most_top >= 0 )
         last_top = most_top;

      if( most_left >= 0 )
         last_left = most_left;

      if( most_top < 0 || next_top < most_top )
         most_top = next_top;

      if( most_left < 0 || next_left < most_left )
         most_left = next_left;

      if( next_right > most_right )
         most_right = next_right;

      if( next_bottom > most_bottom )
         most_bottom = next_bottom;

      ++next;

      // NOTE: Record the last row number that was processed (ignoring the page overflow row if present).
      if( is_normal_group && ( had_row_overflow
       || next == loops - 1 || ( had_text_overflow && col == group_cols - 1 ) ) )
      {
         int offset = 0;
         if( !had_row_overflow && !had_text_overflow )
            ++offset;

         last_row = row + offset;

         // NOTE: If we have had overflow and the current row has been completed then stop processing.
         if( ( had_row_overflow || had_text_overflow ) && col == group_cols - 1 )
            break;
      }
   }

   if( had_text_overflow || ( had_row_overflow && num_field_cols_skipped != num_field_cols ) )
      group_cols_processed[ group ].insert( columns_processed.begin( ), columns_processed.end( ) );
   else
      group_cols_processed[ group ].clear( );

   if( is_normal_group )
   {
      if( had_text_overflow || had_row_overflow )
         group_last_row[ group ] = last_row;
      else
         group_last_row[ group ] = -1;
   }

   if( is_normal_group )
   {
      group_boundary boundary;

      boundary.top = group_y;
      boundary.left = group_x;
      boundary.right = most_right + group_padding_right;
      boundary.bottom = most_bottom + group_padding_bottom;

      float extra_bottom = 0.0;

      // NOTE: Unless this is a repeating group extend the bottom boundary by half
      // of the border thickness to prevent the border from being partially erased
      // by the following group.
      if( repeat_num < 0 && format.groups.find( group )->second.border_bottom )
      {
         extra_bottom = group_border / 2.0;
         boundary.bottom += extra_bottom;
      }

      if( had_row_overflow || had_text_overflow )
         boundary.bottom = normal_group_page_height;

      group_boundaries[ group ] = boundary;

      extend_linked_boundaries( group, format, page,
       group_boundaries, normal_group_page_top, !is_nested_repeat,
       page_height, normal_group_page_height, boundary.bottom - extra_bottom,
       ( had_row_overflow || had_text_overflow ), true );
   }

   // NOTE: If the group has overflowed the page then make sure that group backround
   // (if one is present) has been filled to the bottom of the available page space.
   if( has_group_bg && ( had_row_overflow || had_text_overflow ) )
   {
      page.set_rgb_fill( group_bgcolor_r, group_bgcolor_g, group_bgcolor_b );

      float height_extension = normal_group_page_height - group_bg_y;

      if( height_extension > 0.0 )
      {
         page.rectangle( group_bg_x,
          page_height - ( group_bg_y + height_extension ), group_width, height_extension );

         page.fill( );
      }
   }

   if( group_border > 0 )
   {
      float group_border_top = group_y;
      float group_border_left = group_x;
      float group_border_right = group_border_left + group_width;
      float group_border_bottom = max( group_y, most_bottom ) + group_padding_bottom;

      draw_group_border( group, format, page, group_border,
       group_border_top, group_border_left, group_border_right, group_border_bottom,
       group_bcolor_r, group_bcolor_g, group_bcolor_b, ( had_row_overflow || had_text_overflow ),
       is_page_overflow, normal_group_page_height, page_height, ( was_page_delayed && is_nested_repeat ) );
   }

   page_overflow_occured = ( had_row_overflow || had_text_overflow );

   return !had_row_overflow && !had_text_overflow;
}

bool check_for_processed_linked( const string& group,
 const pdf_gen_format& format, map< string, bool >& group_has_processed )
{
   bool all_processed = false;

   if( format.groups.find( group )->second.has_repeats && !format.groups.find( group )->second.flink.empty( ) )
   {
      string next( group );
      all_processed = true;
      while( true )
      {
         if( format.groups.find( next )->second.flink.empty( ) )
            break;

         next = format.groups.find( next )->second.flink;
         if( !group_has_processed[ next ] )
         {
            all_processed = false;
            break;
         }
      }
   }

   return all_processed;
}

bool check_relative_still_coming(
 const string& group, const string& rel_group,
 const pdf_gen_format& format, map< string, bool >& group_has_overflow,
 map< string, bool >& group_page_delayed, map< string, bool >& group_has_processed,
 map< string, bool >& group_repeating_suspended, map< string, string >& group_child_start_point )
{
   if( rel_group.empty( ) || group_repeating_suspended[ rel_group ] )
      return false;

   string dep_group( format.groups.find( group )->second.dep_group );

   // NOTE: If processing of a relative "detail" group has been stopped due to
   // some child group's page overflow/delay then prevent processing.
   if( !dep_group.empty( ) )
   {
      string dep_rel_group( format.groups.find( dep_group )->second.rel_group );
      if( dep_rel_group.empty( ) )
         dep_rel_group = dep_group;

      if( !group_child_start_point[ dep_rel_group ].empty( ) )
         return true;
   }

   if( group_has_overflow[ rel_group ]
    || group_page_delayed[ rel_group ] || !group_has_processed[ rel_group ] )
      return true;

   string next( rel_group );
   while( true )
   {
      if( format.groups.find( next )->second.flink.empty( ) )
         break;

      next = format.groups.find( next )->second.flink;
      if( group_has_overflow[ next ] || group_page_delayed[ next ] || !group_has_processed[ next ] )
         return true;
   }

   // NOTE: For a repeating group that is linked need prevent processing
   // if one of the links has page overflow/delay.
   if( format.groups.find( group )->second.has_repeats
    && !format.groups.find( group )->second.link_as_block_only )
   {
      next = group;
      while( true )
      {
         if( format.groups.find( next )->second.flink.empty( ) )
            break;

         next = format.groups.find( next )->second.flink;
         if( group_has_overflow[ next ] || group_page_delayed[ next ] )
            return true;
      }
   }

   return check_relative_still_coming( rel_group,
    format.groups.find( rel_group )->second.rel_group, format, group_has_overflow,
    group_page_delayed, group_has_processed, group_repeating_suspended, group_child_start_point );
}

void adjust_parent_boundaries( const string& group,
 const pdf_gen_format& format, const vector< string >& groups,
 map< string, string >& group_detail_footer, group_boundary_container& group_boundaries )
{
   string par_group( format.groups.find( group )->second.par_group );

   float border = format.groups.find( group )->second.border;

   float new_bottom = group_boundaries[ group ].bottom;

   // NOTE: If this group is not a footer then add half the border width to the boundary
   // to make sure gaps won't appear between detail records.
   if( !par_group.empty( ) )
      new_bottom += ( border * 0.5 );
   else
      par_group = format.groups.find( group )->second.dep_group;

   if( !par_group.empty( ) )
   {
      if( group_boundaries[ par_group ].top < new_bottom )
         group_boundaries[ par_group ].top = new_bottom;

      if( group_boundaries[ par_group ].bottom < new_bottom )
         group_boundaries[ par_group ].bottom = new_bottom;

      if( !group_detail_footer[ par_group ].empty( ) )
      {
         if( group_boundaries[ group_detail_footer[ par_group ] ].top < new_bottom )
            group_boundaries[ group_detail_footer[ par_group ] ].top = new_bottom;

         if( group_boundaries[ group_detail_footer[ par_group ] ].bottom < new_bottom )
            group_boundaries[ group_detail_footer[ par_group ] ].bottom = new_bottom;
      }

      adjust_parent_boundaries( par_group, format, groups, group_detail_footer, group_boundaries );
   }
}

void adjust_relative_boundaries( const string& group,
 const pdf_gen_format& format, const vector< string >& groups, group_boundary_container& group_boundaries )
{
   for( size_t i = 0; i < groups.size( ); i++ )
   {
      string next_group( groups[ i ] );

      if( format.groups.find( next_group )->second.rel_group == group )
      {
         if( group_boundaries[ next_group ].top < group_boundaries[ group ].bottom )
            group_boundaries[ next_group ].top = group_boundaries[ group ].bottom;

         if( group_boundaries[ next_group ].bottom < group_boundaries[ group ].bottom )
            group_boundaries[ next_group ].bottom = group_boundaries[ group ].bottom;

         adjust_relative_boundaries( next_group, format, groups, group_boundaries );
      }
   }
}

void adjust_boundaries_for_ancestors( const string& group,
 const string& parent, const string& stop_parent, pdf_page& page,
 const pdf_gen_format& format, group_boundary_container& group_boundaries,
 const vector< string >& groups, map< string, string >& group_detail_footer,
 float page_height, float normal_group_page_top, float normal_group_page_height )
{
   string next_parent_to_adjust( parent );
   while( !next_parent_to_adjust.empty( ) )
   {
      if( next_parent_to_adjust == stop_parent )
         break;

      if( format.groups.find( next_parent_to_adjust )->second.has_repeats )
      {
         group_boundaries[ next_parent_to_adjust ].bottom = group_boundaries[ group ].bottom;
         adjust_relative_boundaries( next_parent_to_adjust, format, groups, group_boundaries );

         extend_linked_boundaries( next_parent_to_adjust, format,
          page, group_boundaries, normal_group_page_top, false, page_height,
          normal_group_page_height, normal_group_page_height, true, false );

         string next_footer( group_detail_footer[ next_parent_to_adjust ] );
         if( !next_footer.empty( ) )
         {
            group_boundaries[ next_footer ].bottom = group_boundaries[ group ].bottom;
            adjust_relative_boundaries( next_footer, format, groups, group_boundaries );

            extend_linked_boundaries( next_footer, format,
             page, group_boundaries, normal_group_page_top, false,
             page_height, normal_group_page_height, normal_group_page_height, true, true );
         }
      }
      else
         break;

      next_parent_to_adjust = format.groups.find( next_parent_to_adjust )->second.par_group;
   }
}

float get_left_boundary( const string& group,
 const pdf_gen_format& format, group_boundary_container& group_boundaries, float page_width )
{
   float x = format.groups.find( group )->second.x;

   if( !format.groups.find( group )->second.x_is_offset )
      return x;

   if( format.groups.find( group )->second.rel_group_align_x == e_pdf_gen_field_alignx_left )
   {
      if( !format.groups.find( group )->second.rel_group.empty( ) )
         x += group_boundaries[ format.groups.find( group )->second.rel_group ].left;
   }
   else if( format.groups.find( group )->second.rel_group_align_x == e_pdf_gen_field_alignx_right )
   {
      if( format.groups.find( group )->second.rel_group.empty( ) )
         x += page_width;
      else
         x += group_boundaries[ format.groups.find( group )->second.rel_group ].right;
   }
   else if( format.groups.find( group )->second.rel_group_align_x == e_pdf_gen_field_alignx_center )
   {
      if( format.groups.find( group )->second.rel_group.empty( ) )
         x += page_width / 2.0;
      else
         x += group_boundaries[ format.groups.find( group )->second.rel_group ].right / 2.0;
   }

   return x;
}

void determine_left_and_right_boundaries(
 const string& group, const pdf_gen_format& format,
 group_boundary_container& group_boundaries, map< string, int >& group_last_row,
 view_font_container& fonts, float page_width, float page_height, float normal_group_page_top )
{
   float most_top = 0.0;
   float most_left = 0.0;
   float most_right = 0.0;
   float most_bottom = 0.0;

   get_group_field_info( group, format, 0.0, 0.0, group_last_row, group_boundaries,
    fonts, normal_group_page_top, most_top, most_left, most_right, most_bottom, page_width, page_height );

   group_boundaries[ group ].left = get_left_boundary( group, format, group_boundaries, page_width );

   float width = ( most_right - most_left ) * format.groups.find( group )->second.cols;

   float total_width = width + format.groups.find( group )->second.padding_right;

   group_boundaries[ group ].right = group_boundaries[ group ].left + total_width;
}

void generate_pdf_output( pdf_doc& doc, pdf_gen_format& format,
 const map< string, string >& variables, vector< string >& temp_image_files )
{
   page_container pages;

   view_font_container view_fonts;
   view_image_container view_images;

   for( font_const_iterator fci = format.fonts.begin( ); fci != format.fonts.end( ); ++fci )
   {
      auto_ptr< pdf_font > ap_font;

      string::size_type pos = fci->second.font_name.find( ',' );
      string font_family( lower( fci->second.font_name.substr( 0, pos ) ) );

      pos = fci->second.font_name.find( '.' );

      if( pos != string::npos )
      {
         if( fci->second.font_encoding.empty( ) )
            throw runtime_error( "cannot use '" + fci->second.font_name + "' without specifying encoding" );

         ap_font.reset( new pdf_true_type_font( doc, fci->second.font_name, fci->second.font_encoding ) );
      }
      else if( font_family == "simsun" || font_family == "simhei" )
      {
         if( fci->second.font_encoding.empty( ) )
            throw runtime_error( "cannot use '" + fci->second.font_name + "' without specifying encoding" );

         ap_font.reset( new pdf_chinese_font( doc, fci->second.font_name, fci->second.font_encoding ) );
      }
      else
      {
         if( fci->second.font_encoding.empty( ) )
            ap_font.reset( new pdf_standard_font( doc, fci->second.font_name ) );
         else
            ap_font.reset( new pdf_standard_font( doc, fci->second.font_name, fci->second.font_encoding.c_str( ) ) );
      }

      view_fonts.insert( make_pair( fci->first,
       make_pair( ap_font.release( ), font_extra( fci->second.font_size, fci->second.ypos_adjust ) ) ) );
   }

   // NOTE: The processing order for nested groups in particular needs to occur as a directed graph of
   // children within parents so use the generated "id" to ensure that the correct processing order is
   // followed.
   map< string, string > group_ids_and_names;
   for( group_const_iterator gci = format.groups.begin( ); gci != format.groups.end( ); ++gci )
      group_ids_and_names.insert( make_pair( gci->second.id, gci->first ) );

   vector< string > groups;
   for( map< string, string >::iterator i = group_ids_and_names.begin( ); i != group_ids_and_names.end( ); ++i )
   {
      // NOTE: The "empty" group is placed at the end to ensure fields than don't belong to a
      // group can still be relative to a group (i.e. all "group" fields are processed first).
      if( !i->second.empty( ) )
         groups.push_back( i->second );
   }

   map< string, string > permissions;
   map< string, string > dynamic_variables;

   map< string, int > group_last_row;
   for( size_t i = 0; i < groups.size( ); i++ )
      group_last_row.insert( make_pair( groups[ i ], -1 ) );

   group_boundary_container group_boundaries;
   for( size_t i = 0; i < groups.size( ); i++ )
      group_boundaries.insert( make_pair( groups[ i ], group_boundary( ) ) );

   group_boundary_container group_new_boundaries;
   for( size_t i = 0; i < groups.size( ); i++ )
      group_new_boundaries.insert( make_pair( groups[ i ], group_boundary( ) ) );

   map< string, int > group_last_page;

   map< string, bool > group_has_overflow;
   map< string, bool > group_page_delayed;
   map< string, bool > group_still_to_come;
   map< string, bool > group_has_processed;
   map< string, bool > group_child_processed;
   map< string, bool > group_already_processed;
   map< string, bool > group_still_has_repeats;
   map< string, bool > group_was_header_delayed;
   map< string, bool > group_repeating_suspended;

   map< string, bool > group_is_nested_footer;
   map< string, bool > group_has_footer_to_come;

   set< string > overflow_or_page_delay_suspended;

   map< string, int > group_repeats;
   map< string, int > parent_group_num;
   map< string, int > parent_repeat_num;
   map< string, int > group_overflow_page;
   map< string, int > group_child_start_page;
   map< string, int > group_page_delayed_page;

   map< string, int > count_parent_repeat_num;
   map< string, int > total_parent_repeat_num;

   map< string, string > group_detail_header;
   map< string, string > group_detail_footer;
   map< string, string > group_child_start_point;

   map< string, string > group_text_overflows;
   map< string, set< string > > group_cols_processed;

   for( size_t i = 0; i < groups.size( ); i++ )
   {
      group_still_to_come[ groups[ i ] ] = true;
      dynamic_variables[ "@@count_" + groups[ i ] ] = to_string( 0 );
   }

   for( size_t j = 0; j < format.fields.size( ); j++ )
   {
      if( format.fields[ j ].use_for_totalling )
         dynamic_variables[ "@@total_" + format.fields[ j ].data ] = "0";

      if( !format.fields[ j ].assign_var.empty( ) )
         dynamic_variables[ format.fields[ j ].assign_var ] = "";
   }

   groups.push_back( "" );

   if( variables.count( c_permissions_variable ) )
   {
      vector< string > perms;
      split( variables.find( c_permissions_variable )->second, perms );

      for( size_t i = 0; i < perms.size( ); i++ )
      {
         string::size_type pos = perms[ i ].find( '=' );

         string name( perms[ i ].substr( 0, pos ) );
         string value;
         if( pos != string::npos )
            value = perms[ i ].substr( pos + 1 );

         permissions.insert( make_pair( name, value ) );
      }
   }

   // NOTE: Determine "footer" groups and prevent groups that require a "permission"
   // that is not present from being processed.
   for( size_t i = 0; i < groups.size( ); i++ )
   {
      string next_group( groups[ i ] );

      string next_par_group( format.groups.find( next_group )->second.par_group );
      string next_dep_group( format.groups.find( next_group )->second.dep_group );
      string next_rel_group( format.groups.find( next_group )->second.rel_group );

      string rest = format.groups.find( next_group )->second.rest;
      string perm = format.groups.find( next_group )->second.perm;

      // NOTE: If user does not have permission to access any field in the group then
      // the group will be treated as though its permission was denied.
      bool has_one_or_more_fields = false;
      for( size_t j = 0; j < format.fields.size( ); j++ )
      {
         if( format.fields[ j ].group == next_group
          && has_permission( format.fields[ j ].rest, format.fields[ j ].perm, permissions ) )
         {
            has_one_or_more_fields = true;
            break;
         }
      }

      if( !perm.empty( ) || !has_one_or_more_fields )
      {
         if( !has_one_or_more_fields || !has_permission( rest, perm, permissions ) )
         {
            group_has_processed[ next_group ] = true;
            group_still_to_come[ next_group ] = false;

            float next_group_x = format.groups.find( next_group )->second.x;

            for( size_t j = i + 1; j < groups.size( ); j++ )
            {
               string next_after( groups[ j ] );

               // NOTE: The "x" position (or offset) will be added to the relative group's
               // own, however, the "y" position is not considered as it is not considered
               // likely that extra vertical gaps would be desirable.
               if( format.groups.find( next_after )->second.rel_group == next_group )
               {
                  format.groups.find( next_after )->second.x += next_group_x;
                  format.groups.find( next_after )->second.rel_group = next_rel_group;
               }
            }
         }
      }

      if( next_par_group.empty( ) && !next_dep_group.empty( ) && ( next_dep_group == next_rel_group ) )
      {
         if( !format.groups.find( next_dep_group )->second.par_group.empty( ) )
            group_is_nested_footer[ next_group ] = true;
         group_detail_footer.insert( make_pair( next_dep_group, next_group ) );
      }
   }

   int num_pages = 0;
   bool finished = false;
   text_output_info page_count_info;
   bool has_left_and_right_boundaries = false;
   while( !finished )
   {
      auto_ptr< pdf_page > ap_page;

      if( format.ps != e_page_size_not_applicable )
         ap_page.reset( new pdf_page( doc, format.ps, format.pd ) );
      else
         ap_page.reset( new pdf_page( doc, format.page_width, format.page_height ) );

      ++num_pages;

      // NOTE: This will ensure that the PDF reader displays the whole page when opening
      // (this perhaps should be optional down the track).
      if( num_pages == 1 )
         ap_page->set_to_fit( );

      string grid( format.grid );
      if( variables.count( c_grid_variable ) )
         grid = variables.find( c_grid_variable )->second;

      if( grid == c_grid_normal || grid == c_grid_reverse )
      {
         bool is_reverse = ( grid == string( c_grid_reverse ) );
         pdf_print_grid( doc, *ap_page, !is_reverse );
      }

      float page_width = ap_page->get_width( );
      float page_height = ap_page->get_height( );

      float header_size = format.header_size;
      if( variables.count( c_header_size_variable ) )
         header_size = atof( variables.find( c_header_size_variable )->second.c_str( ) );

      float footer_size = format.footer_size;
      if( variables.count( c_footer_size_variable ) )
         footer_size = atof( variables.find( c_footer_size_variable )->second.c_str( ) );

      float normal_group_page_top = header_size;
      float normal_group_page_height = page_height - footer_size;

      // NOTE: For the first pass determine all the left and right boundaries for all
      // groups as this information does not rely upon data being present and will be
      // necessary to have for boundaries to be extended for child groups that do not
      // contain data.
      if( !has_left_and_right_boundaries )
      {
         has_left_and_right_boundaries = true;

         for( size_t i = 0; i < groups.size( ); i++ )
         {
            string next_group( groups[ i ] );

            determine_left_and_right_boundaries( next_group, format,
             group_boundaries, group_last_row, view_fonts, page_width, page_height, normal_group_page_top );
         }
      }

      bool had_overflow = false;
      bool is_repeating = false;
      bool has_group_to_come = false;
      bool has_processed_any = false;

#ifdef DEBUG
      int last_page = 0;
#endif
      while( true )
      {
#ifdef DEBUG
         if( last_page < num_pages )
         {
            last_page = num_pages;
            cout << "page #" << num_pages << endl;
         }
#endif
         bool has_further_repeats = false;
         for( size_t i = 0; i < groups.size( ); i++ )
         {
            string group = groups[ i ];

            // NOTE: Fields that belong to the anonymous group must not be output
            // until all repeating groups have first been completed for each page.
            if( group.empty( ) && has_further_repeats )
               continue;

            bool output_every_page = ( group.empty( ) || group[ 0 ] == '@' );

            // NOTE: Fields that belong to an "every page" group must only be
            // output once per page.
            if( output_every_page && group_last_page[ group ] == num_pages )
               continue;

            if( output_every_page
             || group_has_overflow[ group ] || group_page_delayed[ group ]
             || group_still_to_come[ group ] || !group_has_processed[ group ] )
            {
               string dep_group( format.groups.find( group )->second.dep_group );
               string par_group( format.groups.find( group )->second.par_group );
               string rel_group( format.groups.find( group )->second.rel_group );

               bool is_ready = group_still_has_repeats[ group ];

               if( !is_ready )
               {
                  if( !par_group.empty( ) && group_has_processed[ par_group ]
                   && !group_has_overflow[ group ] && !group_repeats[ group ] )
                     continue;

                  if( !par_group.empty( )
                   && ( group_has_overflow[ par_group ] || group_page_delayed[ par_group ] ) )
                     continue;

                  if( !par_group.empty( ) && group_still_to_come[ par_group ] )
                     continue;

                  if( !rel_group.empty( ) && group_still_to_come[ rel_group ] )
                     continue;
               }

               if( !output_every_page && group_page_delayed_page[ group ] == num_pages )
                  continue;

               if( !output_every_page
                && group_has_overflow[ group ] && group_overflow_page[ group ] == num_pages )
                  continue;

               // NOTE: If this group is a parent (or grandparent, etc.) of another group that
               // has either overflowed or been page delayed then do not process it as we need
               // to continue processing from the first child (or grandchild, etc.) where that
               // actually occurred.
               if( !group_child_start_point[ group ].empty( ) )
                  continue;

               string parent_for_trio;

               // NOTE: If has a parent group or is a nested "footer" then remember the parent.
               if( !par_group.empty( ) || ( !dep_group.empty( )
                && !format.groups.find( dep_group )->second.par_group.empty( ) ) )
               {
                  parent_for_trio = par_group;
                  if( parent_for_trio.empty( ) )
                     parent_for_trio = format.groups.find( dep_group )->second.par_group;
               }

               // NOTE: If this a "non-nested footer" then if its "detail" group has page
               // overflow/delay then do not process it now.
               if( par_group.empty( ) && !dep_group.empty( ) && !rel_group.empty( )
                && ( group_has_overflow[ dep_group ] || group_page_delayed[ dep_group ] ) )
                  continue;

               // NOTE: If this is a nested "footer" for a "detail" group that has page overflow
               // or delay or still has children being processed then do not process it until all
               // children have been processed.
               if( par_group.empty( ) && !parent_for_trio.empty( ) )
               {
                  if( group_has_overflow[ dep_group ]
                   || group_page_delayed[ dep_group ] || group_repeating_suspended[ dep_group ] )
                     continue;

                  if( !group_detail_header[ dep_group ].empty( ) )
                  {
                     if( group_has_overflow[ group_detail_header[ dep_group ] ]
                      || group_page_delayed[ group_detail_header[ dep_group ] ] )
                        continue;
                  }
               }

               // NOTE: If this is a "detail" group that has a "header" which is "still to come" or is
               // itself relative to a group that is "still to come" (but not currently "suspended" for
               // child processing), then do not process it now.
               if( !par_group.empty( ) && dep_group.empty( ) )
               {
                  if( !group_detail_header[ group ].empty( ) )
                  {
                     string header( group_detail_header[ group ] );
                     string header_rel_group( format.groups.find( header )->second.rel_group );

                     if( group_still_to_come[ header ] )
                        continue;

                     if( !header_rel_group.empty( )
                      && group_still_to_come[ header_rel_group ]
                      && !group_repeating_suspended[ header_rel_group ] )
                        continue;
                  }
               }

               // NOTE: If this is a "detail" group that already has a "header" or "footer" group
               // that has page overflow or delay then do not process it now.
               if( !par_group.empty( ) && dep_group.empty( ) )
               {
                  if( !group_detail_header[ group ].empty( )
                   && ( group_has_overflow[ group_detail_header[ group ] ]
                   || group_page_delayed[ group_detail_header[ group ] ] ) )
                     continue;

                  if( !group_detail_footer[ group ].empty( )
                   && ( group_has_overflow[ group_detail_footer[ group ] ]
                   || group_page_delayed[ group_detail_footer[ group ] ] ) )
                     continue;
               }

               // NOTE: If is a nested child then need to find the correct point of continuation
               // after overflow/page delay.
               if( !parent_for_trio.empty( ) )
               {
                  string next_parent( parent_for_trio );

                  if( !next_parent.empty( )
                   && !group_child_start_point[ next_parent ].empty( )
                   && group_child_start_page[ next_parent ] == num_pages )
                  {
                     if( group != group_child_start_point[ next_parent ] )
                        continue;

                     while( !next_parent.empty( ) )
                     {
                        group_child_start_point.erase( next_parent );
                        next_parent = format.groups.find( next_parent )->second.par_group;
                     }
                  }
               }

               bool has_repeats = format.groups.find( group )->second.has_repeats;

               // NOTE: If group has been identified as a nested "header" then if has already
               // been processed for the current child then skip it.
               if( !has_repeats
                && !par_group.empty( ) && !dep_group.empty( ) && group_child_processed[ group ] )
                  continue;

               bool has_child_group = format.groups.find( group )->second.has_child_group;

               pdf_gen_field_aligny rel_aligny = format.groups.find( group )->second.rel_group_align_y;

               if( !dep_group.empty( ) && !format.groups.count( dep_group ) )
                  throw runtime_error( "unknown group '" + dep_group
                   + "' specified for '" + group + "' as being dependent upon" );

               bool check_child_processed = false;

               // NOTE: If this is either a "detail" or "footer" then will use "group_child_processed"
               // rather than "group_has_processed" for the relative "still to come" tests.
               if( ( dep_group.empty( ) && !par_group.empty( ) ) || ( par_group.empty( )
                && !dep_group.empty( ) && !format.groups.find( dep_group )->second.par_group.empty( ) ) )
                  check_child_processed = true;

               if( !rel_group.empty( ) && !format.groups.count( rel_group ) )
                  throw runtime_error( "unknown group '" + rel_group
                   + "' specified for '" + group + "' as being relative to" );

               if( group_page_delayed[ group ] && num_pages > group_page_delayed_page[ group ] )
                  is_ready = true;

               // NOTE: If a group is relative to the bottom of another group (or any of its linked groups)
               // that has page overflow/delay or has not yet been processed then do not process it now but
               // flag it so that it will be processed in a further pass.
               if( rel_aligny == e_pdf_gen_field_aligny_bottom
                && !is_ready && !rel_group.empty( ) && !group_has_overflow[ group ]
                && ( par_group.empty( )
                || !group_repeating_suspended[ par_group ]
                || !group_child_start_point[ par_group ].empty( ) )
                && check_relative_still_coming( group, rel_group, format, group_has_overflow,
                group_page_delayed, ( check_child_processed ? group_child_processed : group_has_processed ),
                group_repeating_suspended, group_child_start_point ) )
               {
#ifdef DEBUG
                  cout << "group still to come: " << group << endl;
#endif
                  has_group_to_come = true;
                  group_still_to_come[ group ] = true;

                  // NOTE: Any groups that are linked to a group yet to be processed may need to have their
                  // boundaries extended to the bottom of the normal available page area.
                  if( parent_for_trio.empty( )
                   || parent_repeat_num[ group ] == group_repeats[ parent_for_trio ] )
                  {
                     extend_linked_boundaries( group, format,
                      *ap_page, group_boundaries, normal_group_page_top, true,
                      page_height, normal_group_page_height, normal_group_page_height, true, true );
                  }
               }
               else
               {
                  bool has_overflow = false;

                  int repeat_num = -1;

                  string repeat_prefix;

                  string start_parent( par_group );
                  if( par_group.empty( ) && !dep_group.empty( )
                   && !format.groups.find( dep_group )->second.par_group.empty( ) )
                     start_parent = format.groups.find( dep_group )->second.par_group;

                  bool has_delayed_ancestor = false;
                  string next_parent( start_parent );
                  while( !next_parent.empty( ) )
                  {
                     if( group_page_delayed_page[ next_parent ] == num_pages )
                        has_delayed_ancestor = true;

                     repeat_prefix
                      = group_repeat_string( group_repeats[ next_parent ] - 1 ) + "_" + repeat_prefix;

                     next_parent = format.groups.find( next_parent )->second.par_group;
                  }

                  if( has_delayed_ancestor )
                     continue;

                  if( has_child_group )
                     parent_group_num[ group ] = i;

                  string parent_with_further_children;

                  bool has_further_children = false;
                  bool ancestor_has_further_children = false;

                  bool is_still_processing_children = false;
                  bool is_still_processing_later_sibling = false;
                  bool is_still_processing_earlier_sibling = false;

                  string detail_group;

                  if( has_child_group )
                     detail_group = group;
                  else if( !par_group.empty( ) && !dep_group.empty( ) )
                     detail_group = dep_group;

                  if( !detail_group.empty( ) )
                  {
                     for( size_t j = 0; j < groups.size( ); j++ )
                     {
                        string next_group = groups[ j ];

                        bool is_descendent = false;
                        string next_par_group( format.groups.find( next_group )->second.par_group );

                        while( !next_par_group.empty( ) )
                        {
                           if( detail_group == next_par_group )
                           {
                              is_descendent = true;
                              break;
                           }

                           next_par_group = format.groups.find( next_par_group )->second.par_group;
                        }

                        if( is_descendent && ( group_has_overflow[ next_group ]
                         || group_page_delayed[ next_group ] || group_still_has_repeats[ next_group ] ) )
                           is_still_processing_children = true;
                     }
                  }
                  else if( par_group.empty( ) && !rel_group.empty( ) )
                  {
                     for( size_t j = 0; j < groups.size( ); j++ )
                     {
                        string next_group = groups[ j ];

                        // NOTE: Check if the group child's (or grandchild's, etc) "footer" is still to come.
                        if( group_has_footer_to_come[ next_group ] )
                        {
                           string next_parent( format.groups.find( next_group )->second.par_group );

                           while( !next_parent.empty( ) )
                           {
                              if( rel_group == next_parent )
                              {
                                 is_still_processing_children = true;
                                 break;
                              }

                              next_parent = format.groups.find( next_parent )->second.par_group;
                           }
                        }

                        if( is_still_processing_children )
                           break;

                        if( rel_group == format.groups.find( next_group )->second.par_group
                         && ( group_has_overflow[ next_group ]
                         || group_page_delayed[ next_group ] || group_still_has_repeats[ next_group ] ) )
                           is_still_processing_children = true;
                     }
                  }

                  if( !par_group.empty( ) )
                  {
                     string rel_group_for_trio( rel_group );

                     if( !group_detail_header[ group ].empty( ) )
                        rel_group_for_trio = format.groups.find( group_detail_header[ group ] )->second.rel_group;

                     for( size_t j = 0; j < i; j++ )
                     {
                        string next_group = groups[ j ];
                        string next_rel_group( format.groups.find( next_group )->second.rel_group );

                        // NOTE: Check if any earlier siblings are still being processed.
                        if( group_still_has_repeats[ next_group ] || group_has_footer_to_come[ next_group ] )
                        {
                           string next_parent( format.groups.find( next_group )->second.par_group );
                           while( !next_parent.empty( ) )
                           {
                              if( par_group == next_parent && par_group != next_group
                               && group_child_start_point[ next_parent ].empty( )
                               && !group_child_processed[ next_group ] && !group_still_to_come[ next_group ] )
                              {
                                 is_still_processing_earlier_sibling = true;
                                 break;
                              }

                              next_parent = format.groups.find( next_parent )->second.par_group;
                           }
                        }

                        if( is_still_processing_earlier_sibling )
                           break;
                     }

                     for( size_t j = i + 1; j < groups.size( ); j++ )
                     {
                        string next_group = groups[ j ];

                        // NOTE: Check if a later sibling's (or its child, etc) "footer" is still to come.
                        if( group_has_footer_to_come[ next_group ] )
                        {
                           string next_parent( format.groups.find( next_group )->second.par_group );
                           while( !next_parent.empty( ) )
                           {
                              if( par_group == next_parent
                               && group_still_has_repeats[ next_group ] && !group_still_to_come[ next_group ] )
                              {
                                 if( !group_has_footer_to_come[ next_group ]
                                  || !group_still_to_come[ group_detail_footer[ next_group ] ] )
                                 {
                                    is_still_processing_later_sibling = true;
                                    break;
                                 }
                              }

                              next_parent = format.groups.find( next_parent )->second.par_group;
                           }

                           if( is_still_processing_later_sibling )
                              break;
                        }
                     }
                  }

                  if( !parent_for_trio.empty( ) )
                  {
                     for( size_t j = i + 1; j < groups.size( ); j++ )
                     {
                        string next_group = groups[ j ];

                        if( parent_for_trio == format.groups.find( next_group )->second.par_group )
                        {
                           has_further_children = true;
                           break;
                        }
                     }

                     string next_parent( format.groups.find( parent_for_trio )->second.par_group );
                     while( !next_parent.empty( ) )
                     {
                        for( size_t j = i + 1; j < groups.size( ); j++ )
                        {
                           string next_group = groups[ j ];
                           if( next_parent == format.groups.find( next_group )->second.par_group )
                           {
                              parent_with_further_children = next_parent;
                              ancestor_has_further_children = true;
                              break;
                           }
                        }

                        next_parent = format.groups.find( next_parent )->second.par_group;
                     }
                  }
                  
                  if( is_still_processing_children
                   || is_still_processing_later_sibling || is_still_processing_earlier_sibling )
                     continue;

                  if( has_repeats )
                  {
                     if( group_repeating_suspended[ group ] )
                        continue;

                     string key( repeat_prefix );
                     key += group_repeat_string( group_repeats[ group ] );
                     key += "_" + group;

                     // FUTURE: The "row" value itself will be used to hold record state
                     // information (i.e. for modifiers).
                     if( !variables.count( key ) )
                     {
#ifdef DEBUG
                        cout << "*** no data found for group: " << group << endl;
#endif
                        group_has_overflow[ group ] = false;
                        group_page_delayed[ group ] = false;
                        group_still_to_come[ group ] = false;

                        if( !par_group.empty( ) )
                        {
                           string count_var_name( "@@count_" + par_group + "_" + group );

                           dynamic_variables[ count_var_name ] = to_string( 0 );
                        }

                        if( par_group.empty( )
                         || ( !group_still_has_repeats[ par_group ]
                         && group_child_start_point[ par_group ].empty( ) ) )
                           group_has_processed[ group ] = true;

                        if( !group_detail_footer[ group ].empty( )
                         && format.groups.find( group_detail_footer[ group ] )->second.is_forced )
                           group_has_processed[ group_detail_footer[ group ] ] = false;

                        // NOTE: If the group is relative to another then set its top and bottom
                        // boundaries to the other's bottom (in case background needs to be grown).
                        if( !rel_group.empty( ) )
                        {
                           if( group_boundaries[ group ].top < group_boundaries[ rel_group ].bottom )
                              group_boundaries[ group ].top = group_boundaries[ rel_group ].bottom;

                           if( group_boundaries[ group ].bottom < group_boundaries[ rel_group ].bottom )
                              group_boundaries[ group ].bottom = group_boundaries[ rel_group ].bottom;
                        }
                        else
                        {
                           // NOTE: If not relative to any group then make sure top and boundaries
                           // are set to the page top (otherwise will get an error when attempting
                           // to process a group relative to this one).
                           if( group_boundaries[ group ].top < 0.0 )
                              group_boundaries[ group ].top = normal_group_page_top;

                           if( group_boundaries[ group ].bottom < 0.0 )
                              group_boundaries[ group ].bottom = normal_group_page_top;
                        }

                        adjust_relative_boundaries( group, format, groups, group_boundaries );

                        // NOTE: If the group contains any total fields make sure that these are set
                        // to zero (otherwise could get errors as they may not have been defined).
                        for( size_t j = 0; j < format.fields.size( ); j++ )
                        {
                           if( format.fields[ j ].group == group && format.fields[ j ].use_for_totalling )
                           {
                              string next_parent( par_group );
                              while( !next_parent.empty( ) )
                              {
                                 string dyn_var_name( "@@total_" + next_parent + "_" + format.fields[ j ].data );

                                 dynamic_variables[ dyn_var_name ] = "0";
                                 total_parent_repeat_num[ next_parent + "_" + format.fields[ j ].data ] = 0;

                                 string count_var_name( "@@count_" + next_parent + "_" + group );
                                 dynamic_variables[ count_var_name ] = "0";

                                 next_parent = format.groups.find( next_parent )->second.par_group;
                              }
                           }
                        }

                        if( !par_group.empty( ) && group_child_start_point[ par_group ].empty( ) )
                        {
                           bool had_details = false;
                           if( group_repeats[ group ] > 0 )
                              had_details = true;

                           group_repeats[ group ] = 0;

                           // NOTE: If there are other children for the same parent then suspend
                           // processing for this child and continue otherwise clear the suspend
                           // processing for the parent and any previous suspended children then
                           // if no footer return to the parent.
                           if( has_further_children )
                           {
                              has_further_repeats = true;

                              if( !group_detail_footer[ group ].empty( )
                               && !format.groups.find( group_detail_footer[ group ] )->second.is_forced )
                                 group_repeating_suspended[ group ] = true;
                              else
                              {
                                 // NOTE: If there is a "footer" to follow that is "forced" then make sure
                                 // it will be processed after this.
                                 group_repeating_suspended[ group ] = false;

                                 for( size_t j = 0; j < groups.size( ); j++ )
                                 {
                                    string next_group = groups[ j ];
                                    string next_par_group( format.groups.find( next_group )->second.par_group );

                                    if( par_group == next_par_group )
                                    {
                                       group_has_processed[ next_group ] = ( j <= i ? true : false );
                                       group_child_processed[ next_group ] = ( j <= i ? true : false );
                                    }
                                 }
                              }
                           }
                           else
                           {
                              bool has_further_dependents = false;

                              for( size_t j = 0; j < i; j++ )
                              {
                                 string next_group = groups[ j ];

                                 if( par_group == format.groups.find( next_group )->second.par_group )
                                    group_repeating_suspended[ next_group ] = false;
                              }

                              // NOTE: If any data was output for the "detail" group then will need to
                              // continue processing for the case where there is a further "dependent"
                              // (or "footer").
                              for( size_t j = i + 1; j < groups.size( ); j++ )
                              {
                                 string next_group = groups[ j ];

                                 string next_par_group( format.groups.find( next_group )->second.par_group );

                                 if( had_details
                                  && format.groups.find( next_group )->second.dep_group == group )
                                    has_further_dependents = true;
                              }

                              if( !has_further_dependents )
                              {
                                 if( group_boundaries[ par_group ].top < group_new_boundaries[ par_group ].top )
                                    group_boundaries[ par_group ].top = group_new_boundaries[ par_group ].top;

                                 if( group_boundaries[ par_group ].bottom < group_new_boundaries[ par_group ].bottom )
                                    group_boundaries[ par_group ].bottom = group_new_boundaries[ par_group ].bottom;

                                 // NOTE: When returning to process a parent group set the "processed" flag for
                                 // all dependent child groups according to whether further parent data exists.
                                 for( size_t j = 0; j < groups.size( ); j++ )
                                 {
                                    string next_group = groups[ j ];
                                    string next_par_group( format.groups.find( next_group )->second.par_group );

                                    if( par_group == next_par_group )
                                    {
                                       if( !group_child_start_point[ next_group ].empty( )
                                        || group_has_overflow[ next_group ] || group_page_delayed[ next_group ] )
                                          break;

                                       if( !group_detail_header[ next_group ].empty( ) &&
                                        ( group_has_overflow[ group_detail_header[ next_group ] ]
                                        || group_page_delayed[ group_detail_header[ next_group ] ] ) )
                                          break;

                                       group_has_processed[ next_group ] = !group_still_has_repeats[ par_group ];
                                       group_child_processed[ next_group ] = !group_still_has_repeats[ par_group ];
                                    }
                                 }

                                 string parent_prefix;
                                 string initial_parent( par_group );

                                 if( !group_still_has_repeats[ par_group ] )
                                 {
                                    group_has_processed[ par_group ] = true;

                                    // NOTE: If the group's parent has no further repeats and no footer
                                    // then return to the grandparent (or great-grandparent, etc.).
                                    while( group_detail_footer[ par_group ].empty( ) )
                                    {
                                       group_repeating_suspended[ par_group ] = false;

                                       if( format.groups.find( par_group )->second.par_group.empty( ) )
                                          break;

                                       par_group = format.groups.find( par_group )->second.par_group;
                                       group_boundaries[ par_group ] = group_new_boundaries[ initial_parent ];

                                       if( parent_prefix.empty( ) )
                                          parent_prefix = "grand";
                                       else
                                          parent_prefix = "great-" + parent_prefix;

                                       if( group_still_has_repeats[ par_group ] )
                                          break;
                                    }
                                 }

                                 has_further_repeats = true;
                                 i = parent_group_num[ par_group ] - 1;
                                 group_repeating_suspended[ groups[ i + 1 ] ] = false;

                                 if( !group_child_start_point[ groups[ i + 1 ] ].empty( ) )
                                    group_child_processed[ groups[ i + 1 ] ] = true;

                                 string grand_parent( format.groups.find( groups[ i + 1 ] )->second.par_group );

                                 if( !grand_parent.empty( ) )
                                    group_repeating_suspended[ grand_parent ] = true;
#ifdef DEBUG
                                 cout << "==> returning to " << parent_prefix << "parent group: " << groups[ i + 1 ] << endl;
#endif
                              }
                           }
                        }

                        continue;
                     }
                     else
                     {
                        // NOTE: Make sure all children can be processed again.
                        if( format.groups.find( group )->second.has_child_group )
                        {
                           for( size_t j = 0; j < groups.size( ); j++ )
                           {
                              string next_group = groups[ j ];

                              if( format.groups.find( next_group )->second.par_group == group )
                                 group_has_processed[ next_group ] = false;
                           }
                        }

                        if( !par_group.empty( ) )
                        {
                           // NOTE: If any nested children are identified as being "footers" then make
                           // sure they will be processed again.
                           for( size_t j = 0; j < groups.size( ); j++ )
                           {
                              string next_group = groups[ j ];

                              string rel_group( format.groups.find( next_group )->second.rel_group );

                              if( group == rel_group )
                                 group_has_processed[ next_group ] = false;
                           }
                        }
                     }

                     repeat_num = group_repeats[ group ];

                     if( !group_has_overflow[ group ] && !group_page_delayed[ group ] )
                     {
                        string count_var_name( "@@count_" + group );
                        int val = atoi( dynamic_variables[ count_var_name ].c_str( ) );

                        dynamic_variables[ count_var_name ] = to_string( ++val );

                        string next_parent( par_group );
                        while( !next_parent.empty( ) )
                        {
                           string count_var_name( "@@count_" + next_parent + "_" + group );

                           int val = atoi( dynamic_variables[ count_var_name ].c_str( ) );
                           dynamic_variables[ count_var_name ] = to_string( ++val );

                           next_parent = format.groups.find( next_parent )->second.par_group;
                        }
                     }

                     // NOTE: If this is the first repeat reset the "last_repeat_num" for any total
                     // fields that belong to this group (and make sure the dynamic variable for it
                     // and all parent groups has been created).
                     if( repeat_num == 0 && !group_has_overflow[ group ] && !group_page_delayed[ group ] )
                     {
                        for( size_t j = 0; j < format.fields.size( ); j++ )
                        {
                           if( format.fields[ j ].group == group && format.fields[ j ].use_for_totalling )
                           {
                              format.fields[ j ].last_repeat_num = -1;

                              string dyn_var_name( "@@total_" + format.fields[ j ].data );

                              if( !dynamic_variables.count( dyn_var_name ) )
                                 dynamic_variables[ dyn_var_name ] = "0";

                              string next_parent( par_group );
                              while( !next_parent.empty( ) )
                              {
                                 string dyn_var_name( "@@total_" + next_parent + "_" + format.fields[ j ].data );

                                 if( !dynamic_variables.count( dyn_var_name ) )
                                    dynamic_variables[ dyn_var_name ] = "0";

                                 next_parent = format.groups.find( next_parent )->second.par_group;
                              }
                           }
                        }
                     }

                     // NOTE: When processing a parent record make sure that any count or total
                     // variables that might belong to it have been reset to zero.
                     if( format.groups.find( group )->second.has_child_group )
                     {
                        for( size_t j = 0; j < groups.size( ); j++ )
                        {
                           string next_group( groups[ j ] );
                           string count_var_name( "@@count_" + group + "_" + next_group );

                           if( dynamic_variables.count( count_var_name ) )
                              dynamic_variables[ count_var_name ] = "0";
                        }

                        for( size_t j = 0; j < format.fields.size( ); j++ )
                        {
                           if( format.fields[ j ].use_for_totalling )
                           {
                              string dyn_var_name( "@@total_" + group + "_" + format.fields[ j ].data );

                              if( dynamic_variables.count( dyn_var_name ) )
                                 dynamic_variables[ dyn_var_name ] = "0";
                           }
                        }
                     }

                     key = repeat_prefix;
                     key += group_repeat_string( ++group_repeats[ group ] );
                     key += "_" + group;

                     group_still_has_repeats[ group ] = variables.count( key );

                     if( group_still_has_repeats[ group ] )
                        has_further_repeats = true;
                  }

                  if( !dep_group.empty( ) )
                  {
                     string key( repeat_prefix );
                     key += group_repeat_string( 0 );
                     key += "_" + dep_group;

                     if( !variables.count( key ) )
                     {
#ifdef DEBUG
                        cout << "*** no data found for dependent group '" << dep_group << "' in: " << group << endl;
#endif
                        // NOTE: Identify and ensure that no processing will be attempted for groups which
                        // are "nested" between the current group and its dependency.
                        set< string > dep_candidates;
                        string inner_group( dep_group );
                        for( size_t i = 0; i < groups.size( ); i++ )
                        {
                           string next_group( groups[ i ] );
                           if( format.groups.find( next_group )->second.par_group == inner_group )
                           {
                              group_has_overflow[ next_group ] = false;
                              group_page_delayed[ next_group ] = false;
                              group_still_to_come[ next_group ] = false;

                              inner_group = next_group;
                              if( !format.groups.find( next_group )->second.par_group.empty( )
                               && format.groups.find( next_group )->second.par_group != dep_group )
                                 dep_candidates.insert( format.groups.find( next_group )->second.par_group );
                           }

                           if( !inner_group.empty( )
                            && format.groups.find( next_group )->second.dep_group == inner_group )
                           {
                              group_has_overflow[ next_group ] = false;
                              group_page_delayed[ next_group ] = false;
                              group_still_to_come[ next_group ] = false;
                           }

                           if( dep_candidates.count( format.groups.find( next_group )->second.dep_group ) )
                           {
                              group_has_overflow[ next_group ] = false;
                              group_page_delayed[ next_group ] = false;
                              group_still_to_come[ next_group ] = false;
                           }
                        }

                        group_has_overflow[ group ] = false;
                        group_page_delayed[ group ] = false;
                        group_still_to_come[ group ] = false;

                        // NOTE: If the group is relative to another then set its top and bottom
                        // boundaries to that group (in case background needs to be grown).
                        if( !rel_group.empty( ) && !group_child_processed[ dep_group ] )
                        {
                           group_boundaries[ group ].top = group_boundaries[ rel_group ].bottom;
                           group_boundaries[ group ].bottom = group_boundaries[ rel_group ].bottom;

                           adjust_relative_boundaries( group, format, groups, group_boundaries );
                        }

                        // NOTE: If the dependent group has a parent then don't mark it as
                        // processed (so that nested groups can also have a "footer" group).
                        if( format.groups.find( dep_group )->second.par_group.empty( ) )
                           group_has_processed[ group ] = true;

                        // NOTE: If this is a "header" then mark the "detail" (and if exists
                        // the "footer") as being "child processed".
                        if( !par_group.empty( ) && !group_still_has_repeats[ dep_group ] )
                        {
                           group_child_processed[ dep_group ] = true;
                           if( !group_detail_footer[ dep_group ].empty( ) )
                              group_child_processed[ group_detail_footer[ dep_group ] ] = true;
                        }

                        if( !format.groups.find( group )->second.is_forced )
                           continue;

                        // NOTE: If there is a "header" for a "detail" group that is "forced" then
                        // make sure it will be processed after this (and likewise for the "footer"
                        // if it is also "forced").
                        if( !par_group.empty( ) )
                        {
                           if( !group_detail_footer[ dep_group ].empty( )
                            && format.groups.find( group_detail_footer[ dep_group ] )->second.is_forced )
                           {
                              group_still_to_come[ group_detail_footer[ dep_group ] ] = true;
                              group_has_processed[ group_detail_footer[ dep_group ] ] = false;
                           }

                           group_has_overflow[ dep_group ] = false;
                           group_page_delayed[ dep_group ] = false;
                           group_repeating_suspended[ dep_group ] = false;
                        }

                     }
                  }

                  string repeat_after;
                  if( repeat_num > 0 )
                     repeat_after = group;
                  else
                     repeat_after = group_detail_header[ group ];

                  // NOTE: If a "detail" group was marked as "page delayed" due to its "header"
                  // group then clear that here so that it will be output after the "header" by
                  // the assignment of "repeat_after" above.
                  if( group_was_header_delayed[ group ] )
                  {
                     group_page_delayed[ group ] = false;
                     group_was_header_delayed[ group ] = false;
                  }

                  group_last_page[ group ] = num_pages;

                  // NOTE: If group is a nested child and its parent has repeated then pass this onto
                  // "process_group" so that if a "page delay" occurs the background and borders will
                  // be extended to the bottom of the available page space.
                  bool is_nested_repeat = false;
                  if( !parent_for_trio.empty( ) && group_repeats[ parent_for_trio ] > 0 )
                     is_nested_repeat = true;

                  bool had_overflow_or_page_delay = false;

                  if( process_group( group, format, variables, permissions, dynamic_variables, doc, *ap_page,
                   group_has_overflow[ group ], page_width, page_height, normal_group_page_top, normal_group_page_height,
                   group_boundaries, view_fonts, view_images, group_last_row, group_page_delayed[ group ], has_overflow, page_count_info,
                   temp_image_files, group_cols_processed, group_text_overflows, repeat_prefix, repeat_num, repeat_after, is_nested_repeat ) )
                  {
#ifdef DEBUG
                     cout << " --- " << group << " ---" << endl;
#endif
                     if( !output_every_page )
                        has_processed_any = true;

                     group_has_overflow[ group ] = false;
                     group_page_delayed[ group ] = false;
                     group_still_to_come[ group ] = false;

                     group_already_processed[ group ] = false;

                     group_new_boundaries[ group ] = group_boundaries[ group ];

                     // NOTE: If this is a repeating group with a "footer" then flag that footer
                     // is yet to come so other related groups are not processed before it.
                     if( has_repeats )
                     {
                        for( size_t j = i + 1; j < groups.size( ); j++ )
                        {
                           string next_group = groups[ j ];

                           if( format.groups.find( next_group )->second.par_group.empty( )
                            && group == format.groups.find( next_group )->second.rel_group )
                           {
                              parent_repeat_num[ next_group ] = -1;
                              group_has_footer_to_come[ group ] = true;
                              break;
                           }
                        }
                     }

                     if( !par_group.empty( ) )
                        parent_repeat_num[ group ] = group_repeats[ par_group ];

                     // NOTE: If is a nested "footer" then ensure that the parent's new boundary
                     // bottom has been extended enough.
                     if( par_group.empty( ) && !dep_group.empty( )
                      && !format.groups.find( dep_group )->second.par_group.empty( ) )
                     {
                        string dep_par_group( format.groups.find( rel_group )->second.par_group );

                        parent_repeat_num[ group ] = group_repeats[ dep_par_group ];

                        if( group_new_boundaries[ dep_par_group ].bottom < group_boundaries[ group ].bottom )
                           group_new_boundaries[ dep_par_group ].bottom = group_boundaries[ group ].bottom;
                     }

                     if( !parent_for_trio.empty( ) )
                        adjust_parent_boundaries( group, format, groups, group_detail_footer, group_boundaries );

                     bool is_last_detail_and_has_no_footer = false;

                     if( !par_group.empty( ) && ( !has_repeats || !group_still_has_repeats[ group ] ) )
                     {
#ifdef DEBUG
                        cout << "finished processing child group: " << group << endl;
#endif
                        group_child_processed[ group ] = true;

                        if( group_new_boundaries[ par_group ].bottom < group_boundaries[ group ].bottom )
                           group_new_boundaries[ par_group ].bottom = group_boundaries[ group ].bottom;

                        if( !dep_group.empty( ) )
                           group_detail_header[ dep_group ] = group;

                        if( !has_further_children && !group_still_has_repeats[ par_group ] )
                           group_has_processed[ par_group ] = true;

                        // NOTE: If this is a repeating group but there is no footer to come (and
                        // it has no children) then unsuspend the parent group so that processing
                        // can continue from it.
                        if( has_repeats && !group_has_footer_to_come[ group ]
                         && !format.groups.find( group )->second.has_child_group )
                        {
                           is_last_detail_and_has_no_footer = true;
                           group_repeating_suspended[ par_group ] = false;
                        }
                     }

                     // NOTE: Make sure any children (and to ensure that secondary nested children that
                     // are relative to an earlier nested group's "footer" their relative group as well)
                     // get their parents top and bottom boundary so that they will not be output above
                     // their parent due to a nested child not being present.
                     if( format.groups.find( group )->second.has_child_group )
                     {
                        for( size_t j = i + 1; j < groups.size( ); j++ )
                        {
                           string next_group = groups[ j ];

                           if( group == format.groups.find( next_group )->second.par_group )
                           {
                              group_repeats[ next_group ] = 0;

                              group_boundaries[ next_group ].top = group_boundaries[ group ].top;
                              group_boundaries[ next_group ].bottom = group_boundaries[ group ].bottom;

                              // NOTE: Reset the "child processed" and "repeating suspended" flags
                              // for all the children of each new parent record.
                              group_child_processed[ next_group ] = false;
                              group_repeating_suspended[ next_group ] = false;

                              string child_rel_group( format.groups.find( next_group )->second.rel_group );
                              if( !child_rel_group.empty( ) && group != child_rel_group )
                              {
                                 group_boundaries[ child_rel_group ].top = group_boundaries[ group ].top;
                                 group_boundaries[ child_rel_group ].bottom = group_boundaries[ group ].bottom;
                              }
                           }
                        }
                     }

                     if( is_last_detail_and_has_no_footer )
                     {
                        if( !has_further_children && group_child_start_point[ par_group ].empty( ) )
                        {
                           group_boundaries[ par_group ] = group_new_boundaries[ par_group ];

                           adjust_relative_boundaries( group, format, groups, group_boundaries );

                           extend_linked_boundaries( group, format,
                            *ap_page, group_boundaries, normal_group_page_top, false,
                            page_height, normal_group_page_height, group_boundaries[ group ].bottom, false, false );

                           // NOTE: When returning to process a parent group set the "processed" flag for
                           // all dependent child groups according to whether further parent data exists.
                           for( size_t j = 0; j < groups.size( ); j++ )
                           {
                              string next_group = groups[ j ];
                              string next_par_group( format.groups.find( next_group )->second.par_group );

                              if( par_group == next_par_group )
                              {
                                 if( !group_child_start_point[ next_group ].empty( )
                                  || group_has_overflow[ next_group ] || group_page_delayed[ next_group ] )
                                    break;

                                 if( !group_detail_header[ next_group ].empty( ) &&
                                  ( group_has_overflow[ group_detail_header[ next_group ] ]
                                  || group_page_delayed[ group_detail_header[ next_group ] ] ) )
                                    break;

                                 group_has_processed[ next_group ] = !group_still_has_repeats[ par_group ];
                                 group_child_processed[ next_group ] = !group_still_has_repeats[ par_group ];
                              }
                           }

                           string parent_prefix;
                           string initial_parent( par_group );

                           if( !group_still_has_repeats[ par_group ] )
                           {
                              group_has_processed[ par_group ] = true;

                              // NOTE: If the group's parent has no further repeats and no footer
                              // then return to the grandparent (or great-grandparent, etc.).
                              while( group_detail_footer[ par_group ].empty( ) )
                              {
                                 group_repeating_suspended[ par_group ] = false;
                                 if( format.groups.find( par_group )->second.par_group.empty( ) )
                                    break;

                                 par_group = format.groups.find( par_group )->second.par_group;
                                 group_boundaries[ par_group ] = group_new_boundaries[ initial_parent ];

                                 if( parent_prefix.empty( ) )
                                    parent_prefix = "grand";
                                 else
                                    parent_prefix = "great-" + parent_prefix;

                                 if( group_still_has_repeats[ par_group ] )
                                    break;
                              }
                           }

                           has_further_repeats = true;
                           i = parent_group_num[ par_group ] - 1;
                           group_repeating_suspended[ groups[ i + 1 ] ] = false;

                           if( !group_child_start_point[ groups[ i + 1 ] ].empty( ) )
                              group_child_processed[ groups[ i + 1 ] ] = true;

                           string grand_parent( format.groups.find( groups[ i + 1 ] )->second.par_group );
                           if( !grand_parent.empty( ) )
                              group_repeating_suspended[ grand_parent ] = true;
#ifdef DEBUG
                           cout << "==> returning to " << parent_prefix << "parent group (after detail): " << groups[ i + 1 ] << endl;
#endif
                           continue;
                        }
                     }

                     if( par_group.empty( ) && !group_still_has_repeats[ group ] )
                     {
#ifdef DEBUG
                        if( !output_every_page )
                           cout << "finished processing group: " << group << endl;
#endif
                        // NOTE: In order to ensure that children will be processed for the last parent
                        // record delay setting the "processed" flag until child processing is complete.
                        if( !has_child_group )
                           group_has_processed[ group ] = true;

                        if( !dep_group.empty( ) )
                           group_has_footer_to_come[ dep_group ] = false;

                        if( !dep_group.empty( ) && !group_detail_header[ dep_group ].empty( ) )
                           group_has_processed[ group_detail_header[ dep_group ] ] = true;

                        // NOTE: If we have processed a "non-nested header" group then ensure that it's
                        // remembered as being the "header" group for the "detail" group.
                        if( !dep_group.empty( ) && !group_is_nested_footer[ group ] )
                           group_detail_header[ dep_group ] = group;

                        // NOTE: If this group has been identified as a nested "footer" and there are no
                        // further children to process then return to the parent group to continue further
                        // processing.
                        if( !dep_group.empty( ) && !format.groups.find( dep_group )->second.par_group.empty( ) )
                        {
                           group_boundaries[ dep_group ].top = group_boundaries[ group ].top;
                           group_boundaries[ dep_group ].bottom = group_boundaries[ group ].bottom;

                           adjust_relative_boundaries( dep_group, format, groups, group_boundaries );

                           extend_linked_boundaries( group, format,
                            *ap_page, group_boundaries, normal_group_page_top, false,
                            page_height, normal_group_page_height, group_boundaries[ group ].bottom, false, false );

                           string dep_par_group( format.groups.find( dep_group )->second.par_group );

                           // NOTE: Although there may be further children to process the boundaries for each
                           // parent (and their "header" if present) will be extended here in case processing
                           // returns later to a grandparent (or great-grandparent, etc.).
                           string next_dep_par_group( dep_par_group );
                           while( !next_dep_par_group.empty( ) )
                           {
                              group_boundaries[ next_dep_par_group ].top = group_boundaries[ group ].bottom;
                              group_boundaries[ next_dep_par_group ].bottom = group_boundaries[ group ].bottom;

                              string next_detail_header( group_detail_header[ next_dep_par_group ] );
                              if( !next_detail_header.empty( ) )
                              {
                                 group_boundaries[ next_detail_header ].top = group_boundaries[ group ].bottom;
                                 group_boundaries[ next_detail_header ].bottom = group_boundaries[ group ].bottom;
                              }

                              next_dep_par_group = format.groups.find( next_dep_par_group )->second.par_group;
                           }

                           bool has_another_child = false;
                           bool has_another_child_with_data = false;
                           for( size_t j = i + 1; j < groups.size( ); j++ )
                           {
                              string next_group( groups[ j ] );

                              if( format.groups.find( next_group )->second.par_group == dep_par_group )
                              {
                                 has_another_child = true;

                                 if( group_still_has_repeats[ next_group ] )
                                    has_another_child_with_data = true;
                              }
                           }

                           // NOTE: If no other child has data then clear the "footer to come" flag.
                           if( !has_another_child_with_data )
                           {
                              group_has_footer_to_come[ dep_group ] = false;

                              group_boundaries[ dep_par_group ].top = group_boundaries[ group ].top;
                              group_boundaries[ dep_par_group ].bottom = group_boundaries[ group ].bottom;
                           }

                           if( !has_another_child )
                           {
                              group_boundaries[ dep_par_group ] = group_new_boundaries[ dep_par_group ];

                              // NOTE: When returning to process a parent group set the "processed" flag for
                              // all dependent child groups according to whether further parent data exists.
                              for( size_t j = 0; j < groups.size( ); j++ )
                              {
                                 string next_group = groups[ j ];
                                 string next_par_group( format.groups.find( next_group )->second.par_group );

                                 if( dep_par_group == next_par_group )
                                 {
                                    group_has_processed[ next_group ] = !group_still_has_repeats[ dep_par_group ];
                                    group_child_processed[ next_group ] = !group_still_has_repeats[ dep_par_group ];
                                 }
                              }

                              if( !group_still_has_repeats[ dep_par_group ] )
                                 group_has_processed[ dep_par_group ] = true;

                              has_further_repeats = true;
                              i = parent_group_num[ format.groups.find( dep_group )->second.par_group ] - 1;

                              group_repeating_suspended[ groups[ i + 1 ] ] = false;

                              string grand_parent( format.groups.find( groups[ i + 1 ] )->second.par_group );
                              if( !grand_parent.empty( ) )
                                 group_repeating_suspended[ grand_parent ] = true;
#ifdef DEBUG
                              cout << "==> returning to parent group (after footer): " << groups[ i + 1 ] << endl;
#endif
                           }

                           continue;
                        }
                     }

                     if( format.groups.find( group )->second.has_child_group )
                        group_repeating_suspended[ group ] = true;
                  }
                  else if( has_overflow )
                  {
                     had_overflow = true;
                     had_overflow_or_page_delay = true;

                     if( !output_every_page )
                        has_processed_any = true;

                     group_has_overflow[ group ] = true;
                     group_page_delayed[ group ] = false;
                     group_still_to_come[ group ] = false;
                     group_overflow_page[ group ] = num_pages;
#ifdef DEBUG
                     cout << "overflow has occurred for group: " << group << endl;
#endif
                     if( has_repeats )
                     {
                        --group_repeats[ group ];

                        group_repeating_suspended[ group ] = true;
                        overflow_or_page_delay_suspended.insert( group );
                     }
                  }
                  else
                  {
                     has_group_to_come = true;
                     had_overflow_or_page_delay = true;

                     group_page_delayed[ group ] = true;
                     group_page_delayed_page[ group ] = num_pages;

                     // NOTE: If the group is a "header" then make sure the "detail" group
                     // will be processed immediately after it in the following page.
                     if( !dep_group.empty( ) && !par_group.empty( )
                      && par_group == format.groups.find( dep_group )->second.par_group )
                     {
                        group_was_header_delayed[ dep_group ] = true;

                        group_page_delayed[ dep_group ] = true;
                        group_page_delayed_page[ dep_group ] = num_pages;
                     }

#ifdef DEBUG
                     cout << "page delay has occurred for group: " << group << endl;
#endif
                     if( has_repeats )
                     {
                        --group_repeats[ group ];

                        group_repeating_suspended[ group ] = true;
                        overflow_or_page_delay_suspended.insert( group );
                     }
                  }

                  if( had_overflow_or_page_delay )
                  {
                     if( !parent_for_trio.empty( ) )
                     {
                        string next_parent( parent_for_trio );
                        if( next_parent.empty( ) )
                           next_parent = format.groups.find( dep_group )->second.par_group;

                        // NOTE: If this is a "header" and a "footer" also exists then ensure
                        // the "footer" is not processed to prevent further processing of any
                        // groups relative to it until this group has been processed.
                        if( !par_group.empty( ) && !dep_group.empty( ) )
                        {
                           if( !group_detail_footer[ dep_group ].empty( ) )
                              group_has_processed[ group_detail_footer[ dep_group ] ] = false;
                        }

                        string check_rel_group( rel_group );

                        // NOTE: If this is a "detail" group and there is a "header" then
                        // always check the "header" as the relative group.
                        if( !group_detail_header[ group ].empty( ) )
                           check_rel_group = group_detail_header[ group ];

                        if( !check_rel_group.empty( ) )
                        {
                           if( group_boundaries[ group ].top < group_boundaries[ check_rel_group ].top )
                              group_boundaries[ group ].top = group_boundaries[ check_rel_group ].top;

                           if( group_boundaries[ group ].bottom < group_boundaries[ check_rel_group ].bottom )
                              group_boundaries[ group ].bottom = group_boundaries[ check_rel_group ].bottom;

                           adjust_relative_boundaries( group, format, groups, group_boundaries );
                        }

                        // NOTE: If this is a "header" and the group has a "footer" then attempt to
                        // extend links from the "footer".
                        string extend_from( group );
                        if( !par_group.empty( ) && !dep_group.empty( ) )
                        {
                           if( !group_detail_footer[ dep_group ].empty( ) )
                              extend_from = group_detail_footer[ dep_group ];
                        }

                        extend_linked_boundaries( extend_from, format,
                         *ap_page, group_boundaries, normal_group_page_top, false,
                         page_height, normal_group_page_height, normal_group_page_height, true, true );

                        // NOTE: If this is a "detail" group that has a "footer" then set the footer's
                        // boundary to the normal page height in case it is linked to other groups and
                        // their borders and backgrounds need to be extended.
                        if( !group_detail_footer[ group ].empty( ) )
                        {
                           group_boundaries[ group_detail_footer[ group ] ].top = normal_group_page_height;
                           group_boundaries[ group_detail_footer[ group ] ].bottom = normal_group_page_height;
                        }

                        // NOTE: For multi-level nested groups boundary adjustment and the extension of
                        // linked boundaries needs to include parent "detail" and "footer" for each and
                        // every parent level as the page overflow/delay could occur at any depth.
                        if( !has_further_children )
                           adjust_boundaries_for_ancestors( group, next_parent,
                            parent_with_further_children, *ap_page, format, group_boundaries, groups,
                            group_detail_footer, page_height, normal_group_page_top, normal_group_page_height );

                        // NOTE: If this group is a child then make sure all parent "detail" or
                        // "footer" (if exists) boundaries are set to the normal page height so
                        // linking between other sibling groups will not incorrectly extend the
                        // backgrounds.
                        if( !parent_for_trio.empty( ) )
                        {
                           string next_parent( parent_for_trio );
                           while( !next_parent.empty( ) )
                           {
                              string detail_or_footer( next_parent );
                              if( !group_detail_footer[ next_parent ].empty( ) )
                                 detail_or_footer = group_detail_footer[ next_parent ];

                              group_boundaries[ detail_or_footer ].top = normal_group_page_height;
                              group_boundaries[ detail_or_footer ].bottom = normal_group_page_height;

                              next_parent = format.groups.find( next_parent )->second.par_group;
                           }
                        }

                        vector< string > start_point_parents;

                        // NOTE: If page overflow or delay has occurred for a nested child (including
                        // a nested "footer") then set up the start point for all ancestors so in the
                        // next page (for this group's "family") processing will continue at the same
                        // point that it had been stopped at.
                        while( !next_parent.empty( ) )
                        {
                           if( !group_child_start_point[ next_parent ].empty( ) )
                           {
                              start_point_parents.clear( );
                              break;
                           }

                           start_point_parents.push_back( next_parent );
                           next_parent = format.groups.find( next_parent )->second.par_group;
                        }

                        for( size_t j = 0; j < start_point_parents.size( ); j++ )
                        {
                           group_child_start_point[ start_point_parents[ j ] ] = group;
                           group_child_start_page[ start_point_parents[ j ] ] = num_pages + 1;
                        }
                     }
                  }
               }
            }
         }

         // NOTE: If finished handling repeats then check if there is still a group to come.
         if( !has_further_repeats )
         {
            has_group_to_come = false;

            for( size_t i = 0; i < groups.size( ); i++ )
            {
               string group = groups[ i ];

               // NOTE: If a group with repeats was not processed due to being linked to other
               // groups then repeating can continue if all linked groups have been processed.
               if( group_still_has_repeats[ group ] && !group_page_delayed[ group ]
                && check_for_processed_linked( group, format, group_has_processed ) )
               {
                  has_further_repeats = true;
                  break;
               }

               // NOTE: If a child group's parent has been processed then need to
               // make sure that it (and any header for footer it has) are marked
               // as not still to come.
               string par_group = format.groups.find( group )->second.par_group;
               if( !par_group.empty( ) && group_has_processed[ par_group ] )
               {
                  group_still_to_come[ group ] = false;

                  if( group_detail_header.count( group ) )
                     group_still_to_come[ group_detail_header[ group ] ] = false;

                  if( group_detail_footer.count( group ) )
                     group_still_to_come[ group_detail_footer[ group ] ] = false;
               }

               if( group_still_to_come[ group ] || group_page_delayed[ group ] )
                  has_group_to_come = true;
            }

            if( !has_further_repeats )
               break;
         }

         is_repeating = true;
      }

      // NOTE: Any repeating suspensions that were just due to page overflow or delay are now removed.
      for( set< string >::iterator
       si = overflow_or_page_delay_suspended.begin( ); si != overflow_or_page_delay_suspended.end( ); ++ si )
      {
         group_repeating_suspended.erase( *si );
      }

      overflow_or_page_delay_suspended.clear( );

      // NOTE: The boundary extensions used prior to this point were tracing the back links to
      // find shorter boundaries, however, this will not help if the earlier groups are taller
      // then the later ones. To solve this we trace forward links for any group that has page
      // overflowed or just been processed.
      for( size_t i = 0; i < groups.size( ); i++ )
      {
         string group( groups[ i ] );

         // NOTE: For repeating groups (i.e. child and "footer" groups) extend linked
         // boundaries if a "group child start point" for the repeating parent exists
         // (as that will be the case for either overflow or page delay).
         if( group_is_nested_footer[ group ]
          || !format.groups.find( group )->second.par_group.empty( ) )
         {
            string ancestor( format.groups.find( group )->second.par_group );
            if( ancestor.empty( ) )
               ancestor = format.groups.find( group )->second.rel_group;

            bool header_or_detail_still_to_come = false;

            // NOTE: If the first "link" is "still to come" then do not attempt to
            // extend any of the other linked groups boundaries.
            if( group_still_to_come[ ancestor ] )
               header_or_detail_still_to_come = true;

            if( !group_detail_header[ ancestor ].empty( )
             && group_still_to_come[ group_detail_header[ ancestor ] ] )
               header_or_detail_still_to_come = true;

            // NOTE: The "group_child_start_point" is actually placed on the parent of
            // the repeating group so set the ancestor here accordingly.
            if( !format.groups.find( ancestor )->second.par_group.empty( ) )
               ancestor = format.groups.find( ancestor )->second.par_group;

            if( !header_or_detail_still_to_come && !group_child_start_point[ ancestor ].empty( ) )
               extend_linked_boundaries( group,
                format, *ap_page, group_boundaries, normal_group_page_top, false,
                page_height, normal_group_page_height, normal_group_page_height, true, false );
         }
         else if( group_has_overflow[ group ] )
         {
            extend_linked_boundaries( group,
             format, *ap_page, group_boundaries, normal_group_page_top, true, page_height,
             normal_group_page_height, normal_group_page_height, true, false, &group_still_to_come );
         }
         else if( group_has_processed[ group ] && !group_already_processed[ group ] )
         {
            group_already_processed[ group ] = true;

            // NOTE: Refer to the note about "extra_bottom" in process page.
            float extra_bottom = 0.0;
            if( group_repeats[ group ] < 0 && format.groups.find( group )->second.border_bottom )
               extra_bottom = format.groups.find( group )->second.border / 2.0;

            extend_linked_boundaries( group,
             format, *ap_page, group_boundaries, normal_group_page_top, true, page_height,
             normal_group_page_height, group_boundaries[ group ].bottom - extra_bottom, false, false );
         }
      }

      if( ( had_overflow || has_group_to_come ) && !has_processed_any && groups.size( ) > 1 )
         throw runtime_error( "unexpected pass completed without processing any groups" );

#ifdef DEBUG
      cout << "[[ pass completed ]]" << endl;
#endif
      for( size_t i = 0; i < groups.size( ); i++ )
      {
         string group = groups[ i ];
         bool output_every_page = ( group.empty( ) || group[ 0 ] == '@' );

         if( !output_every_page )
         {
            group_boundaries[ group ].top = -1;
            group_new_boundaries[ group ].top = -1;

            if( !group_has_overflow[ group ] )
            {
               group_boundaries[ group ].bottom = -1;
               group_new_boundaries[ group ].bottom = -1;
            }
         }
      }

      if( !had_overflow && !has_group_to_come )
         finished = true;

      pages.push_back( ref_count_ptr< pdf_page >( ap_page.release( ) ) );
   }

   // NOTE: If a total page count field was found and there were multiple pages
   // then it is assumed that the total page count field was used once in every
   // page and the page state would have been stored once per page. So now loop
   // through each page restoring the state and outputting the total page count.
   if( num_pages > 1 && page_count_info.top >= 0.0 )
   {
      for( size_t i = 0; i < pages.size( ); i++ )
      {
         pages[ i ]->restore_state( );

         string page( c_page_variable );
         if( variables.count( page ) )
            page = variables.find( page )->second;

         string text( page );
         text += "  " + to_string( i + 1 );
         text += " / " + to_string( pages.size( ) );

         pages[ i ]->output_text( text, page_count_info.top, page_count_info.left,
          page_count_info.bottom, page_count_info.right, page_count_info.alignment );
      }
   }
}

void generate_pdf_doc( const string& format_filename,
 const string& output_filename, const map< string, string >& variables, progress* p_progress )
{
   if( p_progress )
   {
      for( map< string, string >::const_iterator ci = variables.begin( ); ci != variables.end( ); ++ci )
         p_progress->output_progress( ci->first + " \"" + ci->second + "\"" );
   }

   pdf_gen_format format;
   read_pdf_gen_format( format_filename, format );

   pdf_doc doc;
   doc.set_compression( );

   vector< string > temp_image_files;
   generate_pdf_output( doc, format, variables, temp_image_files );

   doc.save_to_file( output_filename );

   for( size_t i = 0; i < temp_image_files.size( ); i++ )
      file_remove( temp_image_files[ i ] );
}

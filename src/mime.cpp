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
#  include <memory>
#  include <iostream>
#  include <stdexcept>
#endif

#include "mime.h"

#include "base64.h"
#include "utilities.h"

//#define DEBUG

using namespace std;

namespace
{

// MIME: RFC 2045 (http://www.faqs.org/rfcs/rfc2045.html)
// MIME: RFC 2046 (http://www.faqs.org/rfcs/rfc2046.html)
// MIME: RFC 2047 (http://www.faqs.org/rfcs/rfc2047.html)
// MIME: RFC 4289 (http://www.faqs.org/rfcs/rfc4289.html)
// MIME: RFC 2049 (http://www.faqs.org/rfcs/rfc2049.html)
// MIME: RFC 4288 (http://www.faqs.org/rfcs/rfc4288.html)

const size_t c_max_chars_per_line = 96;

const string g_new_line( "\r\n" );

string escape_fullstops_if_required( const string& src )
{
   string str;

   // NOTE: For an SMTP message a full stop followed by a CR/LF pair indicates the end of the message so
   // in accordance with the "transparency" section (4.5.2) of RFC 821 any text lines that commence with
   // a full stop are escaped by adding an extra full stop (these should be automatically removed by the
   // message receiver).
   bool is_start_of_line = true;
   for( size_t i = 0; i < src.length( ); i++ )
   {
      if( is_start_of_line && src[ i ] == '.' )
         str += ".";

      str += src[ i ];

      is_start_of_line = ( src[ i ] == '\n' );
   }

   return str;
}

string split_input_into_lines( const string& input, size_t chars_per_line )
{
   string str;
   string::size_type pos = 0;

   while( true )
   {
      if( input.length( ) - pos > chars_per_line )
      {
         str += input.substr( pos, chars_per_line ) + g_new_line;
         pos += chars_per_line;
      }
      else
      {
         str += input.substr( pos ) + g_new_line;
         break;
      }
   }

   return str;
}

string clean_whitespace( const string& input )
{
   string s;

   bool had_whitespace = false;
   for( size_t i = 0; i < input.size( ); i++ )
   {
      if( input[ i ] == 9 || input[ i ] == 32 )
      {
         if( !had_whitespace )
            s += ' ';

         had_whitespace = true;
      }
      else
      {
         had_whitespace = false;
         s += input[ i ];
      }
   }

   return s;
}

void get_attributes( const string& input, vector< string >& attributes )
{
   string next;
   bool in_quotes = false;
   for( size_t i = 0; i < input.size( ); i++ )
   {
      if( input[ i ] == '"' )
         in_quotes = !in_quotes;
      else if( !in_quotes )
      {
         if( input[ i ] == ' ' )
         {
            attributes.push_back( next );
            next.erase( );
         }
         else
            next += input[ i ];
      }
      else
         next += input[ i ];
   }

   if( !next.empty( ) )
      attributes.push_back( next );
}

string process_header( const string& input, vector< string >& attributes )
{
   string s( clean_whitespace( input ) );

   string::size_type pos = s.find( ':' );
   if( pos == string::npos )
      throw runtime_error( "unexpected MIME header format '" + input + "'" );

   string name( s.substr( 0, pos ) );

   if( pos < s.length( ) - 2 )
   {
      get_attributes( s.substr( pos + 2 ), attributes );

      for( size_t i = 0; i < attributes.size( ); i++ )
      {
         if( attributes[ i ].length( ) && attributes[ i ][ attributes[ i ].length( ) - 1 ] == ';' )
            attributes[ i ].erase( attributes[ i ].length( ) - 1 );
      }
   }

   return name;
}

bool is_multipart_content( const vector< string >& attributes )
{
   bool retval = false;

   if( attributes.size( ) )
   {
      string attr( attributes[ 0 ] );

      string::size_type pos = attr.find( '/' );
      if( pos == string::npos )
         throw runtime_error( "unexpected initial MIME Content-Type attribute '" + attr + "'" );

      string type = attr.substr( 0, pos );

      if( type == "multipart" )
         retval = true;
   }

   return retval;
}

string process_content_type( const vector< string >& attributes, string& type, string& subtype )
{
   string retval;

   bool is_text = false;
   bool is_image = false;
   bool is_multipart = false;
   bool is_application = false;

   for( size_t i = 0; i < attributes.size( ); i++ )
   {
      string attr( attributes[ i ] );

      if( attr.length( ) && attr[ attr.length( ) - 1 ] == ';' )
         attr.erase( attr.length( ) - 1 );

      if( i == 0 )
      {
         string::size_type pos = attr.find( '/' );
         if( pos == string::npos )
            throw runtime_error( "unexpected initial MIME Content-Type attribute '" + attr + "'" );

         type = attr.substr( 0, pos );
         subtype = attr.substr( pos + 1 );

         if( type == "text" )
            is_text = true;
         else if( type == "image" )
            is_image = true;
         else if( type == "multipart" )
            is_multipart = true;
         else if( type == "application" )
            is_application = true;
      }
      else
      {
         string::size_type pos = attr.find( '=' );
         if( pos == string::npos )
            throw runtime_error( "unexpected subsequent MIME Content-Type attribute '" + attr + "'" );

         string name( attr.substr( 0, pos ) );

         string value( attr.substr( pos + 1 ) );
         if( value.length( ) > 2 && value[ 0 ] == '"' )
            value = value.substr( 1, value.length( ) - 2 );

         if( is_text && name == "charset" )
            retval = value;

         if( is_image && name == "name" )
            retval = value;

         if( is_multipart && name == "boundary" )
            retval = value;

         if( is_application && name == "name" )
            retval = value;
      }
   }

   return retval;
}

string process_boundary( const string& input )
{
   string s( "--" );

   for( size_t i = 0; i < input.size( ); i++ )
   {
      bool ignore = false;
      if( i == 0 && input[ i ] == '"' )
         ignore = true;

      if( i == input.length( ) - 2 && input[ i ] == '"' )
         ignore = true;

      if( i == input.length( ) - 1 && ( input[ i ] == ';' || input[ i ] == '"' ) )
         ignore = true;

      if( !ignore )
         s += input[ i ];
   }

   return s;
}

}

struct mime_decoder::impl
{
   impl( )
   {
      is_child = false;
   }

   void process_data( );

   string data;

   string type;
   string subtype;
   string encoding;
   string boundary;

   string attribute;

   string text_data;

   bool is_child;

   vector< mime_part* > parts;

   unique_ptr< mime_decoder > up_child;
};

void mime_decoder::impl::process_data( )
{
   vector< string > lines;
   split( data, lines, '\n' );

   string last_line, next_id, next_data;
   string next_type, next_subtype, next_value, next_encoding, next_charset;

   bool needs_boundary = true;
   bool processing_data = false;
   bool finished_headers = false;
   for( size_t i = 0; i < lines.size( ); i++ )
   {
      last_line = lines[ i ];

      if( !last_line.empty( ) && !finished_headers )
      {
         while( i < lines.size( ) - 1 && lines[ i + 1 ].length( )
          && ( lines[ i + 1 ][ 0 ] == 9 || lines[ i + 1 ][ 0 ] == 32 ) )
            last_line += lines[ ++i ];
      }

      if( !finished_headers )
      {
         if( processing_data )
         {
            if( !next_data.empty( ) )
               next_data += '\n';

            next_data += last_line;

            if( !lines[ i ].length( ) )
               next_data += '\n';
         }

         if( !lines[ i ].length( ) )
         {
            finished_headers = true;
            if( next_type != "multipart" )
               next_data.erase( );

            if( !needs_boundary )
               processing_data = true;
         }

         if( !last_line.empty( ) )
         {
            vector< string > attributes;
            string header( process_header( last_line, attributes ) );

            if( lower( header ) == "content-id" )
            {
               if( !attributes.empty( ) )
               {
                  next_id = attributes[ 0 ];
                  if( next_id.length( ) > 2 && next_id[ 0 ] == '<' )
                     next_id = next_id.substr( 1, next_id.length( ) - 2 );
               }
            }
            else if( lower( header ) == "content-type" )
            {
               if( boundary.empty( ) )
                  needs_boundary = is_multipart_content( attributes );

               if( needs_boundary && !processing_data )
                  boundary = "--" + process_content_type( attributes, type, subtype );
               else
               {
                  next_value = process_content_type( attributes, next_type, next_subtype );
                  if( type == "text" )
                     attribute = next_value;
               }
            }
            else if( lower( header ) == "content-transfer-encoding" )
            {
               if( !attributes.empty( ) )
               {
                  next_encoding = attributes[ 0 ];
                  if( type == "text" )
                     encoding = next_encoding;
               }
            }
         }
      }
      else if( type == "text" )
      {
         if( !text_data.empty( ) )
            text_data += "\n";

         text_data += last_line;
      }

      if( !boundary.empty( ) && last_line.find( boundary ) == 0 )
      {
         if( processing_data )
         {
            if( next_type == "multipart" )
               up_child.reset( new mime_decoder( true, next_data ) );
            else
            {
               mime_part* p_next_part = new mime_part;

               p_next_part->id = next_id;
               p_next_part->type = next_type;
               p_next_part->subtype = next_subtype;
               p_next_part->encoding = next_encoding;
               p_next_part->attribute = next_value;
               p_next_part->data = next_data;

               parts.push_back( p_next_part );
            }
         }

         if( last_line.find( boundary + "--" ) == 0 )
            break;

         processing_data = true;
         finished_headers = false;

         next_id.erase( );
         next_data.erase( );
         next_value.erase( );
         next_encoding.erase( );
      }
      else if( processing_data )
      {
         if( !next_data.empty( ) )
            next_data += '\n';

         next_data += last_line;
      }
   }

   if( !needs_boundary && processing_data )
   {
      mime_part* p_next_part = new mime_part;

      p_next_part->id = next_id;
      p_next_part->type = next_type;
      p_next_part->subtype = next_subtype;
      p_next_part->encoding = next_encoding;
      p_next_part->attribute = next_value;
      p_next_part->data = next_data;

      parts.push_back( p_next_part );
   }
}

mime_decoder::mime_decoder( const string& mime_data )
{
   p_impl = new impl;

   p_impl->data = mime_data;

   p_impl->process_data( );
}

mime_decoder::mime_decoder( bool is_child, const string& mime_data )
{
   p_impl = new impl;

   p_impl->data = mime_data;
   p_impl->is_child = is_child;

   p_impl->process_data( );
}

mime_decoder::~mime_decoder( )
{
   for( size_t i = 0; i < p_impl->parts.size( ); i++ )
      delete p_impl->parts[ i ];

   delete p_impl;
   p_impl = 0;
}

string mime_decoder::get_type( ) const
{
   return p_impl->type;
}

string mime_decoder::get_subtype( ) const
{
   return p_impl->subtype;
}

string mime_decoder::get_encoding( ) const
{
   return p_impl->encoding;
}

string mime_decoder::get_attribute( ) const
{
   return p_impl->attribute;
}

string mime_decoder::get_text_data( ) const
{
   return p_impl->text_data;
}

bool mime_decoder::has_child( )
{
   return p_impl->up_child.get( ) != 0;
}

mime_decoder& mime_decoder::get_child( )
{
   if( !p_impl->up_child.get( ) )
      throw runtime_error( "mime_decoder: child has not been created" );

   return *p_impl->up_child;
}

size_t mime_decoder::num_parts( ) const
{
   return p_impl->parts.size( );
}

mime_part& mime_decoder::get_part( size_t num ) const
{
   if( num >= p_impl->parts.size( ) )
      throw runtime_error( "MIME part #" + to_string( num ) + " is out of range" );

   return *p_impl->parts[ num ];
}

struct mime_encoder::impl
{
   impl( )
   {
      num_parts = 0;

      is_child = false;
      finished = false;

      max_chars_per_line = c_max_chars_per_line;

      uuid guid;

      multi_part_subtype = "mixed";

      mime_boundary = string( "MIME-Boundary-" );
      mime_boundary += guid.as_string( );
   }

   string data;
   string mime_boundary;
   string multi_part_subtype;
   string multi_part_extra_subtype;

   unique_ptr< mime_encoder > up_child;

   bool is_child;
   bool finished;
   int num_parts;
   int max_chars_per_line;
};

mime_encoder::mime_encoder( const char* p_multi_part_subtype,
 const char* p_multi_part_extra_subtype, int max_chars_per_line )
{
   p_impl = new impl;

   if( p_multi_part_subtype )
      p_impl->multi_part_subtype = string( p_multi_part_subtype );

   if( p_multi_part_extra_subtype )
      p_impl->multi_part_extra_subtype = string( p_multi_part_extra_subtype );

   if( max_chars_per_line )
      p_impl->max_chars_per_line = max_chars_per_line;
}

mime_encoder::~mime_encoder( )
{
   delete p_impl;
   p_impl = 0;
}

void mime_encoder::add_text( const string& text,
 const char* p_charset, const char* p_encoding, bool is_for_smtp )
{
   if( p_impl->finished )
      throw runtime_error( "mime_encoder: cannot add text as MIME encoding already finished" );

   ++p_impl->num_parts;

   string charset( "us-ascii" );
   if( p_charset )
      charset = string( p_charset );

   string encoding( "7bit" );
   if( p_encoding )
      encoding = string( p_encoding );

   p_impl->data += string( "--" ) + p_impl->mime_boundary + g_new_line;
   p_impl->data += string( "Content-ID: <" ) + uuid( ).as_string( ) + ">" + g_new_line;
   p_impl->data += string( "Content-Type: text/plain; charset=\"" ) + charset + "\"" + g_new_line;
   p_impl->data += string( "Content-Transfer-Encoding: " ) + encoding + g_new_line + g_new_line;

   int max_chars_per_line = p_impl->max_chars_per_line;

   // NOTE: Need to make room for any leading fullstops that need to be doubled.
   if( is_for_smtp )
      --max_chars_per_line;

   string text_data;

   if( encoding == "7bit" )
      text_data = split_input_into_lines( text, max_chars_per_line );
   else if( encoding == "quoted_printable" )
      text_data = encode_quoted_printable( text, max_chars_per_line );
   else
      throw runtime_error( "unexpected encoding type '" + encoding + "' for mime_encoder::add_html" );

   if( !is_for_smtp )
      p_impl->data += text_data;
   else
      p_impl->data += escape_fullstops_if_required( text_data );

   p_impl->data += g_new_line;
}

void mime_encoder::add_html( const string& html,
 const char* p_charset, const char* p_encoding, bool is_for_smtp )
{
   if( p_impl->finished )
      throw runtime_error( "mime_encoder: cannot add HTML as MIME encoding already finished" );

   ++p_impl->num_parts;

   string charset( "iso-8859-1" );
   if( p_charset )
      charset = string( p_charset );

   string encoding( "quoted-printable" );
   if( p_encoding )
      encoding = string( p_encoding );

   p_impl->data += string( "--" ) + p_impl->mime_boundary + g_new_line;
   p_impl->data += string( "Content-ID: <" ) + uuid( ).as_string( ) + ">" + g_new_line;
   p_impl->data += string( "Content-Type: text/html; charset=\"" ) + charset + "\"" + g_new_line;
   p_impl->data += string( "Content-Transfer-Encoding: " ) + encoding + g_new_line + g_new_line;

   int max_chars_per_line = p_impl->max_chars_per_line;

   // NOTE: Need to make room for any leading fullstops that need to be doubled.
   if( is_for_smtp )
      --max_chars_per_line;

   string text_data;

   if( encoding == "7bit" )
      text_data = split_input_into_lines( html, max_chars_per_line );
   else if( encoding == "quoted-printable" )
      text_data = encode_quoted_printable( html, max_chars_per_line );
   else
      throw runtime_error( "unexpected encoding type '" + encoding + "' for mime_encoder::add_html" );

   if( !is_for_smtp )
      p_impl->data += text_data;
   else
      p_impl->data += escape_fullstops_if_required( text_data );

   p_impl->data += g_new_line;
}

void mime_encoder::add_file( const string& file_name, const char* p_encoding )
{
   if( p_impl->finished )
      throw runtime_error( "mime_encoder: cannot add a file as MIME encoding already finished" );

   ++p_impl->num_parts;

   string file( file_name );
   string name( file_name );

   string::size_type pos = file.find( '?' );

   if( pos != string::npos )
   {
      name = file_name.substr( 0, pos );
      file = file_name.substr( pos + 1 );
   }

   pos = file.find_last_of( "." );
   if( pos != string::npos )
   {
      string ext( file.substr( pos ) );

      pos = name.find( "." );
      if( pos == string::npos )
         name += ext;
   }

   string encoding( "base64" );
   if( p_encoding )
      encoding = string( p_encoding );

   p_impl->data += string( "--" ) + p_impl->mime_boundary + g_new_line;
   p_impl->data += string( "Content-ID: <" ) + uuid( ).as_string( ) + ">" + g_new_line;
   p_impl->data += string( "Content-Type: application/octet-stream;" );
   p_impl->data += string( " name=\"" ) + name + "\"" + g_new_line;
   p_impl->data += string( "Content-Disposition: attachment; filename=\"" ) + name + "\"" + g_new_line;
   p_impl->data += string( "Content-Transfer-Encoding: " ) + encoding + g_new_line + g_new_line;

   string file_data = buffer_file( file );
   if( encoding == "base64" )
   {
      file_data = base64::encode( file_data );
      p_impl->data += split_input_into_lines( file_data, p_impl->max_chars_per_line );
   }
   else if( encoding == "quoted-printable" )
      p_impl->data += encode_quoted_printable( file_data, p_impl->max_chars_per_line ) + g_new_line;
   else
      throw runtime_error( "unexpected encoding type '" + encoding + "' for mime_encoder::add_file" );
}

void mime_encoder::add_image( const string& file_name, const char* p_path_prefix, const char* p_encoding )
{
   if( p_impl->finished )
      throw runtime_error( "mime_encoder: cannot add an image as MIME encoding already finished" );

   ++p_impl->num_parts;

   string file( file_name );
   string name( file_name );

   string::size_type pos = file.find( '?' );

   if( pos != string::npos )
   {
      name = file_name.substr( 0, pos );
      file = file_name.substr( pos + 1 );
   }

   string path_prefix;
   if( p_path_prefix )
      path_prefix = string( p_path_prefix );

   string full_path( file );

   if( !path_prefix.empty( ) )
   {
      if( path_prefix[ path_prefix.length( ) - 1 ] != '\\' && path_prefix[ path_prefix.length( ) - 1 ] != '/' )
         path_prefix += '/';

      full_path = path_prefix + file;
   }

   pos = file.find_last_of( "." );
   if( pos == string::npos )
      throw runtime_error( "mime_encoder::add_image no file extension in '" + file + "'" );

   string file_type( lower( file.substr( pos + 1 ) ) );

   pos = name.find( "." );
   if( pos == string::npos )
      name += "." + file_type;

   if( file_type == "jpg" )
      file_type = "jpeg";

   string encoding( "base64" );
   if( p_encoding )
      encoding = string( p_encoding );

   p_impl->data += string( "--" ) + p_impl->mime_boundary + g_new_line;
   p_impl->data += string( "Content-ID: <" ) + "image-" + name + ">" + g_new_line;
   p_impl->data += string( "Content-Type: image/" + file_type + ";" );
   p_impl->data += string( " name=\"" ) + name + "\"" + g_new_line;
   p_impl->data += string( "Content-Disposition: inline; filename=\"" ) + name + "\"" + g_new_line;
   p_impl->data += string( "Content-Transfer-Encoding: " ) + encoding + g_new_line + g_new_line;

   string file_data = buffer_file( full_path );
   if( encoding == "base64" )
   {
      file_data = base64::encode( file_data );
      p_impl->data += split_input_into_lines( file_data, p_impl->max_chars_per_line );
   }
   else if( encoding == "quoted-printable" )
      p_impl->data += encode_quoted_printable( file_data, p_impl->max_chars_per_line ) + g_new_line;
   else
      throw runtime_error( "unexpected encoding type '" + encoding + "' for mime_encoder::add_image" );
}

void mime_encoder::create_child( const char* p_multi_part_subtype, const char* p_multi_part_extra_subtype )
{
   p_impl->up_child.reset(
    new mime_encoder( p_multi_part_subtype, p_multi_part_extra_subtype, p_impl->max_chars_per_line ) );

   p_impl->up_child->p_impl->is_child = true;
}

mime_encoder& mime_encoder::get_child( )
{
   if( !p_impl->up_child.get( ) )
      throw runtime_error( "mime_encoder: child has not been created" );

   return *p_impl->up_child;
}

string mime_encoder::get_data( )
{
   if( !p_impl->finished )
   {
      p_impl->finished = true;

      string final_data;

      if( !p_impl->is_child )
         final_data += "MIME-Version: 1.0" + g_new_line;

      bool is_multi = false;

      if( ( p_impl->num_parts > 1 ) || p_impl->up_child.get( ) )
         is_multi = true;

      if( is_multi )
      {
         final_data += "Content-Type: multipart/" + p_impl->multi_part_subtype + ";";
         if( !p_impl->multi_part_extra_subtype.empty( ) )
            final_data += " type=\"multipart/" + p_impl->multi_part_extra_subtype + "\";";
         final_data += string( " boundary=\"" ) + p_impl->mime_boundary + "\"" + g_new_line + g_new_line;

         if( !p_impl->is_child )
            final_data += string( "This is a multipart MIME message." ) + g_new_line;

         if( p_impl->up_child.get( ) )
         {
            final_data += string( "--" ) + p_impl->mime_boundary + g_new_line;

            final_data += p_impl->up_child->get_data( );
         }
      }

      final_data += p_impl->data;

      final_data += string( "--" ) + p_impl->mime_boundary + string( "--" ) + g_new_line;

      p_impl->data = final_data;
   }

   return p_impl->data;
}


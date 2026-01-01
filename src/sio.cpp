// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "sio.h"

#include "utilities.h"

//#define DEBUG

using namespace std;

namespace
{

/* Sample "sio" file format...

# An initial comment
<sio/>
 <section/>
  <attr1>value1
  <attr2>value2
  # This is a comment
  <subsection/>
   <attr1>value1
   <attr2>value2
   <attr3>value3
  </subsection>
 </section>
</sio>

It should be noted that comments are not permitted after </sio> (but can occur anywhere else).

*/

const char c_basic_prefix = '<';
const char c_basic_suffix = '>';
const char c_extra_modifier = '/';

const char* const c_root_section = "sio";

const int c_min_size_for_section = 4; //i.e. <x/> or </x>
const int c_min_size_for_section_name = 2; //i.e. x/ or /x

void write_section_attributes( sio_writer& writer, const section_node& node )
{
   for( size_t i = 0; i < node.get_num_attributes( ); i++ )
      writer.write_attribute( node.get_attribute( i ).get_name( ), node.get_attribute( i ).get_value( ) );

   for( size_t i = 0; i < node.get_num_child_nodes( ); i++ )
   {
      const section_node& child_node( node.get_child_node( i ) );
      writer.start_section( child_node.get_name( ) );
      write_section_attributes( writer, child_node );
      writer.finish_section( child_node.get_name( ) );
   }
}

string compress( const string& json )
{
   string out;

   bool is_escape = false;
   bool is_in_quotes = false;

   for( size_t i = 0; i < json.size( ); i++ )
   {
      char ch = json[ i ];

      bool skip = false;

      if( is_escape )
         is_escape = false;
      else
      {
         char ch = json[ i ];

         if( ch == '\\' )
            is_escape = true;
         else if( ch == '"' )
            is_in_quotes = !is_in_quotes;

         if( !is_in_quotes
          && ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f' ) )
            skip = true;
      }

      if( !skip )
         out += ch;
   }

   return out;
}

string extract_value( string& json )
{
   string value;

   if( json.empty( ) || json[ 0 ] != '"' )
      throw runtime_error( "unexpected json value '" + json + "'" );

   bool in_escape = false;
   for( size_t i = 1; i < json.size( ); i++ )
   {
      char ch = json[ i ];

      if( in_escape )
      {
         value += ch;
         in_escape = false;
      }
      else if( ch == '\\' )
         in_escape = true;
      else
      {
         if( ch == '"' )
         {
            json.erase( 0, i + 1 );
            return value;
         }
         else
            value += ch;
      }
   }

   throw runtime_error( "unexpected json value '" + json + "'" );
}

void process_sio( const section_node& node, ostream& outs, json_format format )
{
   string ws, sep, indent;

   switch( format )
   {
      case e_json_format_multi:
      ws = '\n';
      sep = ' ';
      indent = string( node.get_child_depth( ), ' ' );
      break;

      case e_json_format_single:
      ws = ' ';
      sep = ' ';
      break;
   }

   if( node.is_root_node( ) )
      outs << "[" << ws;
   else
      outs << indent << "{" << sep << "\"" << node.get_name( ) << "\":" << ws << indent << "[" << ws;

   size_t num_attributes = node.get_num_attributes( );
   for( size_t i = 0; i < num_attributes; i++ )
   {
      if( i > 0 )
         outs << "," << ws;

      if( format != e_json_format_single )
         outs << indent << sep;

      outs << "{" << sep << "\"" << node.get_attribute( i ).get_name( )
       << "\":" << sep << quote( node.get_attribute( i ).get_value( ) ) << sep << "}";
   }

   size_t num_children = node.get_num_child_nodes( );

   if( num_attributes && num_children )
      outs << "," << ws;

   for( size_t i = 0; i < num_children; i++ )
   {
      if( i > 0 )
         outs << "," << ws;
      process_sio( node.get_child_node( i ), outs, format );
   }

   if( num_attributes || num_children )
      outs << ws;

   if( node.is_root_node( ) )
      outs << "]";
   else
      outs << indent << "]";

   if( !node.is_root_node( ) )
      outs << ws << indent << "}";
}

bool process_json( string& json, sio_writer& sio, size_t depth = 0 )
{
   string name, value;

   while( !json.empty( ) )
   {
      if( json.length( ) < 5 )
         return false;

      if( json[ 0 ] != '{' || json[ 1 ] != '"' )
         return false;

      json.erase( 0, 2 );

      string::size_type pos = json.find( '"' );
      if( pos == string::npos )
         return false;

      name = json.substr( 0, pos );
      json.erase( 0, pos + 1 );

      if( json.empty( ) || json[ 0 ] != ':' )
         return false;

      json.erase( 0, 1 );

      if( json.empty( ) || ( json[ 0 ] != '"' && json[ 0 ] != '[' ) )
         return false;

      if( json[ 0 ] == '[' )
      {
         sio.start_section( name );

         json.erase( 0, 1 );

         if( !json.empty( ) && json[ 0 ] != ']' )
         {
            if( !process_json( json, sio, depth + 1 ) )
               return false;
         }

         if( !json.empty( ) && json[ 0 ] == ']' )
            json.erase( 0, 1 );

         bool is_complete = false;
         bool is_continuing = false;

         if( json.length( ) >= 2 )
         {
            if( json.substr( 0, 2 ) == "}]" )
               is_complete = true;
            else if( json.substr( 0, 2 ) == "}," )
               is_continuing = true;
         }

         if( json.length( ) >= 2 && ( is_complete || is_continuing ) )
         {
            json.erase( 0, 2 );

            if( !sio.is_root_section( ) )
               sio.finish_section( sio.get_current_section( ) );

            if( is_complete )
               return true;
         }
      }
      else
      {
         value = extract_value( json );

         if( json.empty( ) || json[ 0 ] != '}' )
            return false;

         sio.write_attribute( name, value );

         json.erase( 0, 1 );

         if( json.empty( ) || ( json[ 0 ] != ',' && json[ 0 ] != ']' ) )
            return false;

         if( json[ 0 ] == ']' )
            return true;
         else
            json.erase( 0, 1 );
      }
   }

   return true;
}

}

sio_reader::sio_reader( istream& is, bool include_comments, vector< string >* p_initial_comments )
 :
 is( is ),
 last_num( 0 ),
 line_num( 0 ),
 start_pos( 0 ),
 finish_pos( 0 ),
 include_comments( include_comments )
{
   if( !is.good( ) )
      throw runtime_error( "input stream is bad" );

   if( !is.eof( ) )
   {
      read_line( );

      if( p_initial_comments )
      {
         string comment;

         while( true )
         {
            if( has_read_comment( comment ) )
               p_initial_comments->push_back( comment );
            else
               break;
         }
      }

      last_num = line_num;

      if( !has_started_section( c_root_section ) )
         throw runtime_error( "invalid root section" );
   }
}

sio_reader::operator sio_reader::bool_type( ) const
{
   if( !is || is.eof( ) )
      return 0;
   else
      return &sio_reader::this_type_does_not_support_comparisons;
}

void sio_reader::start_section( const string& name )
{
   if( !has_started_section( name ) )
      throw runtime_error( "section '" + name + "' start was not found at line #" + to_string( line_num ) );
}

void sio_reader::finish_section( const string& name )
{
   if( !has_finished_section( name ) )
      throw runtime_error( "section '" + name + "' finish was not found at line #" + to_string( line_num ) );
}

string sio_reader::read_attribute( const string& name )
{
   string str;
   if( !has_read_attribute( name, str ) )
      throw runtime_error( "attribute '" + name + "' was not found at line #" + to_string( line_num ) );

   return str;
}

string sio_reader::read_opt_attribute( const string& name, const string& default_value )
{
   string str( default_value );
   if( !has_read_attribute( name, str ) )
      str = default_value;

   return str;
}

bool sio_reader::has_read_comment( string& comment )
{
   string::size_type pos = line.find_first_not_of( " \t" );

   if( pos != string::npos && line[ pos ] == '#' ) // i.e. is a comment
   {
      comment = line;
      read_line( );

      return true;
   }
   else
      return false;
}

bool sio_reader::has_started_section( string& name )
{
   if( !is_sio_identifier( name, e_extra_type_start ) )
      return false;

   if( name.empty( ) )
      name = line.substr( start_pos, finish_pos - start_pos + 1 );

   read_line( );
   sections.push( name );

   return true;
}

bool sio_reader::has_started_section( const string& name )
{
   string str( name );
   return has_started_section( str );
}

bool sio_reader::has_finished_section( string& name )
{
   if( !is_sio_identifier( name, e_extra_type_finish ) )
      return false;

   if( sections.empty( ) )
      throw runtime_error( "unexpected empty section stack at line #" + to_string( line_num ) );

   if( name.empty( ) )
      name = sections.top( );

   read_line( );
   sections.pop( );

   return true;
}

bool sio_reader::has_finished_section( const string& name )
{
   string str( name );
   return has_finished_section( str );
}

bool sio_reader::has_read_attribute( string& name, string& value )
{
   if( !is_sio_identifier( name, e_extra_type_neither ) )
      return false;

   if( name.empty( ) )
      name = line.substr( start_pos, finish_pos - start_pos + 1 );

   value = line.substr( value_pos );
   read_line( );

   return true;
}

bool sio_reader::has_read_attribute( const string& name, string& value )
{
   string str( name );
   return has_read_attribute( str, value );
}

string sio_reader::get_current_section( ) const
{
   string str;

   if( !sections.empty( ) )
      str = sections.top( );

   return str;
}

bool sio_reader::is_root_section( ) const
{
   if( !sections.empty( ) && sections.top( ) == c_root_section )
      return true;
   else
      return false;
}

void sio_reader::verify_finished_sections( )
{
   if( !has_finished_section( c_root_section ) )
      throw runtime_error( "root section finish not found at line #" + to_string( line_num ) );
}

void sio_reader::read_line( )
{
   last_num = line_num;

   while( true )
   {
      if( is.eof( ) )
         throw runtime_error( "unexpected end of file" );

      ++line_num;
#ifdef DEBUG
      cout << "==> reading line #" << line_num << endl;
#endif
      if( getline( is, line ) && line.empty( ) )
         throw runtime_error( "unexpected empty line #" + to_string( line_num ) );

      remove_trailing_cr_from_text_file_line( line, line_num == 1 );

      string::size_type pos = line.find_first_not_of( " \t" );
      if( is.eof( ) || include_comments || ( pos != string::npos && line[ pos ] != '#' ) ) // i.e. continue if is a comment
         break;
   }
}

string sio_reader::get_line( )
{
   read_line( );
   return line;
}

bool sio_reader::is_sio_identifier( const string& name, extra_type xtype ) const
{
   string::size_type spos, fpos;

   if( line.size( ) < c_min_size_for_section )
      return false;

   spos = line.find( c_basic_prefix );
   if( line.size( ) - spos < c_min_size_for_section )
      return false;

   fpos = line.find( c_basic_suffix, spos + 1 );
   if( fpos - spos < c_min_size_for_section_name )
      return false;

   for( string::size_type i = 0; i < spos; i++ )
   {
      if( line[ i ] != ' ' && line[ i ] != '\t' )
         return false;
   }

   int extra = 0;
   if( line[ spos + 1 ] == c_extra_modifier )
   {
      if( xtype == e_extra_type_start || xtype == e_extra_type_neither )
         return false;
      ++spos;
      ++extra;
   }

   if( line[ fpos - 1 ] == c_extra_modifier )
   {
      if( xtype == e_extra_type_finish || xtype == e_extra_type_neither )
         return false;
      --fpos;
      ++extra;
   }

#ifdef DEBUG
   cout << "==> xtype is: " << xtype << ", extra is: " << extra << endl;
#endif
   if( extra == ( xtype == e_extra_type_neither ) )
      return false;

   ++spos;
   --fpos;

   if( xtype != e_extra_type_neither )
   {
      for( string::size_type i = spos; i < fpos; i++ )
      {
         char ch( line[ i ] );

         if( ch >= '0' && ch <= '9' )
            continue;

         if( ch >= 'A' && ch <= 'Z' )
            continue;

         if( ch >= 'a' && ch <= 'z' )
            continue;

         if( ch == '_' )
            continue;

         return false;
      }
   }

   value_pos = fpos + 2;

   start_pos = spos;
   finish_pos = fpos;

   if( name.empty( ) )
      return true;

   if( name.size( ) != fpos - spos + 1 )
      return false;

   return name == line.substr( spos, fpos - spos + 1 );
}

void dump_sio( sio_reader& reader, ostream* p_ostream )
{
   if( !p_ostream )
      p_ostream = &cout;

   sio_writer( *p_ostream, reader );
}

void dump_sio_file( const string& filename, ostream* p_ostream )
{
   if( !p_ostream )
      p_ostream = &cout;

   ifstream inpf( filename.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + filename + "' for input in dump_sio_file" );

   sio_reader reader( inpf );
   dump_sio( reader, p_ostream );
}

void write_graph( const sio_graph& graph, ostream* p_ostream )
{
   if( !p_ostream )
      p_ostream = &cout;
   sio_writer( *p_ostream, graph );
}

sio_writer::sio_writer( ostream& os, vector< string >* p_initial_comments )
 :
 os( os ),
 can_write_attribute( false )
{
   if( !os.good( ) )
      throw runtime_error( "output stream is bad" );

   if( p_initial_comments )
   {
      for( size_t i = 0; i < p_initial_comments->size( ); i++ )
         write_comment( ( *p_initial_comments )[ i ] );
   }

   start_section( c_root_section );
}

sio_writer::sio_writer( ostream& os, sio_reader& reader )
 :
 os( os )
{
   while( reader )
      put_line( reader.get_line( ) );
}

sio_writer::sio_writer( ostream& os, const sio_graph& graph )
 :
 os( os )
{
   if( graph.empty( ) )
      throw runtime_error( "unexpected empty graph" );

   start_section( c_root_section );
   write_section_attributes( *this, graph.get_root_node( ) );
   finish_sections( );
}

void sio_writer::write_comment( const std::string& comment )
{
   string s( comment );

   string::size_type pos = s.find_first_not_of( " \t" );
   if( pos != string::npos && s[ pos ] != '#' )
      s = '#' + s;

   os << s << '\n';

   if( !os.good( ) )
      throw runtime_error( "output stream is bad" );
}

void sio_writer::start_section( const string& name )
{
   os << string( sections.size( ), ' ' )
    << c_basic_prefix << name << c_extra_modifier << c_basic_suffix << '\n';

   if( !os.good( ) )
      throw runtime_error( "output stream is bad" );

   sections.push( name );

   section = name;
   can_write_attribute = true;
}

void sio_writer::finish_section( const string& name )
{
   if( sections.empty( ) )
      throw runtime_error( "unexpected empty section stack" );

   if( name != section )
      throw runtime_error( "attempt to finish section '" + name + "' whilst in section '" + section + "'" );

   os << string( sections.size( ) - 1, ' ' )
    << c_basic_prefix << c_extra_modifier << sections.top( ) << c_basic_suffix << '\n';

   sections.pop( );

   if( sections.empty( ) )
      section.erase( );
   else
      section = sections.top( );

   can_write_attribute = false;

   if( !os.good( ) )
      throw runtime_error( "output stream is bad" );
}

void sio_writer::write_attribute( const string& name, const string& value )
{
   if( !can_write_attribute )
      throw runtime_error( "attributes are only permitted within a section" );

   if( name.size( ) < c_min_size_for_section_name )
      throw runtime_error( "attribute name cannot be shorter than the shortest permitted section name" );

   os << string( sections.size( ), ' ' )
    << c_basic_prefix << name << c_basic_suffix << value << '\n';

   if( !os.good( ) )
      throw runtime_error( "output stream is bad" );
}

void sio_writer::write_opt_attribute( const string& name, const string& value, const string& default_value )
{
   if( value != default_value )
      write_attribute( name, value );
}

void sio_writer::finish_sections( )
{
   if( !sections.empty( ) )
      finish_section( c_root_section );

   os.flush( );
   if( !os.good( ) )
      throw runtime_error( "unexpected bad output stream" );

   if( !sections.empty( ) )
      throw runtime_error( "unexpected non-empty section stack" );
}

bool sio_writer::is_root_section( ) const
{
   return section == c_root_section;
}

void sio_writer::put_line( const string& line )
{
   os << line << '\n';
}

bool section_node::has_attribute( const string& name ) const
{
   bool retval( false );
   for( vector< attribute >::size_type i = 0; i < attributes.size( ); i++ )
   {
      if( attributes[ i ].get_name( ) == name )
      {
         retval = true;
         break;
      }
   }

   return retval;
}

const attribute& section_node::get_attribute( const string& name ) const
{
   vector< attribute >::size_type i;
   for( i = 0; i < attributes.size( ); i++ )
   {
      if( attributes[ i ].get_name( ) == name )
         break;
   }

   if( i == attributes.size( ) )
      throw runtime_error( "attribute '" + name + "' not found for section node '" + get_name( ) + "'" );

   return attributes[ i ];
}

const string& section_node::get_attribute_value( const string& name ) const
{
   return get_attribute( name ).get_value( );
}

size_t section_node::get_child_depth( ) const
{
   size_t depth = 0;
   const section_node* p_node( this );

   while( true )
   {
      if( !p_node->get_parent_node( ) )
         break;

      ++depth;
      p_node = p_node->get_parent_node( );
   }

   return depth;
}

sio_graph::sio_graph( sio_reader& reader )
 :
 p_root_node( 0 )
{
   stack< section_node* > nodes;
   nodes.push( new section_node( "" ) );

   while( reader )
   {
      string name, value;
      while( reader.has_started_section( name ) )
      {
#ifdef DEBUG
         cout << "==> started section: " << name << endl;
#endif
         section_node* p_new_node
          = new section_node( name, nodes.top( )->get_num_child_nodes( ), nodes.top( ) );
         nodes.top( )->add_child_node( p_new_node );
         nodes.push( p_new_node );
         name.erase( );
      }

      while( reader.has_read_attribute( name, value ) )
      {
#ifdef DEBUG
         cout << "==> process attribute: " << name << " value = " << value << endl;
#endif
         nodes.top( )->add_attribute( attribute( name, value ) );
         name.erase( );
      }

      while( reader.has_finished_section( name ) )
      {
#ifdef DEBUG
         cout << "==> finished section: " << name << endl;
#endif
         if( nodes.size( ) > 1 )
            nodes.pop( );
         name.erase( );
      }
   }

   if( !nodes.empty( ) )
      p_root_node = nodes.top( );
}

sio_graph::~sio_graph( )
{
   if( p_root_node )
   {
      destroy_nodes( p_root_node );
      delete p_root_node;
   }
}

void sio_graph::destroy_nodes( section_node* p_node )
{
   for( size_t i = 0; i < p_node->get_num_child_nodes( ); i++ )
      destroy_nodes( p_node->child_nodes[ i ] );

   for( size_t i = 0; i < p_node->get_num_child_nodes( ); i++ )
      delete p_node->child_nodes[ i ];

   p_node->child_nodes.clear( );
}

void convert_sio_to_json( const sio_graph& sio, ostream& outs, json_format format )
{
   process_sio( sio.get_root_node( ), outs, format );
}

void convert_json_to_sio( const string& json, ostream& outs )
{
   string compressed( compress( json ) );

   sio_writer sio( outs );

   if( !compressed.empty( ) )
   {
      if( compressed.size( ) < 2 )
         throw runtime_error( "invalid json '" + json + "'" );

      if( compressed[ 0 ] != '[' || compressed[ compressed.size( ) - 1 ] != ']' )
         throw runtime_error( "invalid json '" + json + "'" );

      compressed.erase( 0, 1 );

      if( !process_json( compressed, sio ) )
         throw runtime_error( "invalid json '" + json + "'" );
   }

   sio.finish_sections( );
}

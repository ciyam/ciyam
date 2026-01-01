// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SIO_H
#  define SIO_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <stack>
#     include <vector>
#     include <string>
#     include <iosfwd>
#  endif

#  include "macros.h"

class sio_writer;

class sio_reader
{
   friend class sio_writer;

   typedef void ( sio_reader::*bool_type )( ) const;
   void this_type_does_not_support_comparisons( ) const { }

   public:
   sio_reader( std::istream& is, bool include_comments = false, std::vector< std::string >* p_initial_comments = 0 );

   operator bool_type( ) const;

   template< typename T > bool operator ==( const T& rhs ) const
   {
      this_type_does_not_support_comparisons( );
      return false;
   }

   template< typename T > bool operator !=( const T& rhs ) const
   {
      this_type_does_not_support_comparisons( );
      return false;
   }

   void start_section( const std::string& name );
   void finish_section( const std::string& name );

   std::string read_attribute( const std::string& name );
   std::string read_opt_attribute( const std::string& name, const std::string& default_value = "" );

   bool has_read_comment( std::string& comment );

   bool has_started_section( std::string& name );
   bool has_started_section( const std::string& name );

   bool has_finished_section( std::string& name );
   bool has_finished_section( const std::string& name );

   bool has_read_attribute( std::string& name, std::string& value );
   bool has_read_attribute( const std::string& name, std::string& value );

   void verify_finished_sections( );

   std::string get_current_section( ) const;

   bool is_root_section( ) const;

   size_t get_last_line_num( ) const { return last_num; }
   size_t get_next_line_num( ) const { return line_num; }

   size_t get_current_section_level( ) const { return sections.size( ); }

   private:
   enum extra_type
   {
      e_extra_type_start,
      e_extra_type_finish,
      e_extra_type_neither
   };

   void read_line( );

   std::string get_line( );

   bool is_sio_identifier( const std::string& name, extra_type xtype ) const;

   std::istream& is;

   bool include_comments;

   size_t last_num;
   size_t line_num;

   std::string line;
   std::stack< std::string > sections;

   mutable size_t value_pos, start_pos, finish_pos;
};

void dump_sio( sio_reader& reader, std::ostream* p_ostream = 0 );
void dump_sio_file( const std::string& filename, std::ostream* p_ostream = 0 );

class sio_graph;

void write_graph( const sio_graph& graph, std::ostream* p_ostream = 0 );

class sio_writer
{
   friend void dump_sio( sio_reader& reader, std::ostream* p_ostream );
   friend void dump_sio_file( const std::string& filename, std::ostream* p_ostream );
   friend void write_graph( const sio_graph& graph, std::ostream* p_ostream );

   public:
   sio_writer( std::ostream& os, std::vector< std::string >* p_initial_comments = 0 );

   void write_comment( const std::string& comment );

   void start_section( const std::string& name );
   void finish_section( const std::string& name );

   void write_attribute( const std::string& name, const std::string& value );
   void write_opt_attribute( const std::string& name, const std::string& value, const std::string& default_value = "" );

   std::string get_current_section( ) const { return section; }

   bool is_root_section( ) const;

   void finish_sections( );

   private:
   sio_writer( std::ostream& os, sio_reader& reader );
   sio_writer( std::ostream& os, const sio_graph& graph );

   void put_line( const std::string& line );

   std::ostream& os;
   std::string section;
   bool can_write_attribute;

   std::stack< std::string > sections;
};

class attribute
{
   public:
   attribute( ) { }

   attribute( const std::string& name, const std::string& value )
    :
    name( name ),
    value( value )
   {
   }

   const std::string& get_name( ) const { return name; }
   const std::string& get_value( ) const { return value; }

   private:
   std::string name;
   std::string value;
};

class section_node
{
   public:
   section_node( ) : child_num( 0 ), p_parent_node( 0 ) { }

   section_node( const std::string& name, size_t child_num = 0, section_node* p_parent_node = 0 )
    :
    name( name ),
    child_num( child_num ),
    p_parent_node( p_parent_node )
   {
   }

   bool is_root_node( ) const { return p_parent_node == 0; }

   const std::string& get_name( ) const { return name; }
   size_t get_child_num( ) const { return child_num; }

   bool has_attribute( const std::string& name ) const;

   const attribute& get_attribute( const std::string& name ) const;
   const std::string& get_attribute_value( const std::string& name ) const;

   size_t get_num_attributes( ) const { return attributes.size( ); }
   const attribute& get_attribute( size_t num ) const { return attributes.at( num ); }

   const section_node* get_parent_node( ) const { return p_parent_node; }

   size_t get_child_depth( ) const;

   size_t get_num_child_nodes( ) const { return child_nodes.size( ); }
   const section_node& get_child_node( size_t num ) const { return *child_nodes.at( num ); }

   void add_attribute( const attribute& attr ) { attributes.push_back( attr ); }

   void add_child_node( section_node* p_node ) { child_nodes.push_back( p_node ); }

   private:
   std::string name;
   std::vector< attribute > attributes;

   size_t child_num;

   const section_node* p_parent_node;
   std::vector< section_node* > child_nodes;

   friend class sio_graph;
};

class sio_graph
{
   public:
   sio_graph( sio_reader& reader );
   ~sio_graph( );

   section_node& get_root_node( ) { return *p_root_node; }
   const section_node& get_root_node( ) const { return *p_root_node; }

   bool empty( ) const { return p_root_node == 0; }

   private:
   section_node* p_root_node;

   void destroy_nodes( section_node* p_node );
};

enum json_format
{
   e_json_format_multi,
   e_json_format_single,
   e_json_format_compressed
};

void convert_sio_to_json( const sio_graph& sio, std::ostream& outs, json_format format = e_json_format_multi );

void convert_json_to_sio( const std::string& json, std::ostream& outs );

#endif


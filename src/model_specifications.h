// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef MODEL_SPECIFICATIONS_H
#  define MODEL_SPECIFICATIONS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <vector>
#  endif

#  include "class_factory.h"

class model;
class sio_reader;
class sio_writer;
struct specification_data;

enum model_element_type
{
   e_model_element_type_class,
   e_model_element_type_field,
   e_model_element_type_procedure,
   e_model_element_type_procedure_arg,
   e_model_element_type_relationship,
   e_model_element_type_specification
};

enum specification_data_source
{
   e_specification_data_source_model,
   e_specification_data_source_non_model
};

struct specification_detail
{
   specification_detail( ) { }

   specification_detail( const std::string& id, const std::string& item, model_element_type element_type )
    :
    id( id ),
    item( item ),
    element_type( element_type )
   {
   }

   std::string id;
   std::string item;
   model_element_type element_type;
};

struct specification
{
   virtual ~specification( ) { }

   virtual void add( model& m, const std::vector< std::string >& args, std::vector< specification_detail >& details ) = 0;

   virtual specification_data_source get_specification_data_source( ) const = 0;

   virtual bool has_specification_data_for_class( const std::string& id ) const { return true; }

   virtual bool is_facet_specification( ) const { return false; }

   virtual bool is_interface_specification( ) const { return false; }

   virtual void read_data( sio_reader& reader ) { }
   virtual void write_data( sio_writer& writer ) const { }

   virtual void add_specification_data( model& m, specification_data& data ) const { }

   virtual void get_specification_strings( model& m, std::vector< std::pair< std::string, std::string > >& strings ) const { }
};

DECLARE_CLASS_FACTORY( std::string, specification );

#endif


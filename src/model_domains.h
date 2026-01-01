// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef MODEL_DOMAINS_H
#  define MODEL_DOMAINS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <vector>
#  endif

#  include "class_factory.h"

class sio_reader;
class sio_writer;

struct domain
{
   virtual ~domain( ) { }

   virtual void add( const std::vector< std::string >& args ) = 0;

   virtual std::string get_extra( ) const = 0;

   virtual std::string get_type_name( ) const = 0;

   virtual std::string get_list_display( ) const = 0;

   virtual void read_data( sio_reader& reader ) { }
   virtual void write_data( sio_writer& writer ) const { }
};

DECLARE_CLASS_FACTORY( std::string, domain );

#endif


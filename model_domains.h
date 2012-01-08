// Copyright (c) 2006
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

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


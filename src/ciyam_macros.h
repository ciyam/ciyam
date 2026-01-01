// Copyright (c) 2009-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_MACROS_H
#  define CIYAM_MACROS_H

#  include "macros.h"

#  define MODULE_AND_CLASS( module, class )\
CONCATIZE( CONCATIZE( module, _ ), class )

#  define CLASS_ID( module, class )\
MODULE_AND_CLASS( module, class )::static_class_id( )

#  define FIELD_ID( module, class, field )\
MODULE_AND_CLASS( module, class )::static_get_field_id( \
CONCATIZE( MODULE_AND_CLASS( module, class )::e_field_id_, field ) )

#  define FIELD_NAME( module, class, field )\
MODULE_AND_CLASS( module, class )::static_get_field_name( \
CONCATIZE( MODULE_AND_CLASS( module, class )::e_field_id_, field ) )

#endif

// Copyright (c) 2009
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

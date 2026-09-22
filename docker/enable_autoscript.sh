#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

# NOTE: Uncomments named "autoscript.sio" entries. The "config" script copies
# "autoscript.sio.default", which deliberately keeps most entries commented out so
# that a fresh development environment starts minimal. The "install" script handles
# this with "sed -i '/<sio\/>/,/<\/sio>/{s/^# / /g}'", but that enables every entry
# including "auto_update" and "ciyam_ddns_ip_updates" - neither of which belongs in
# a container, where the image is the unit of update and there is no DDNS name to
# maintain. This enables only the entries it is given.
#
# Usage: enable_autoscript.sh [file] [name]...

if [ $# -lt 2 ]; then
 echo "Usage: enable_autoscript.sh [file] [name]..."

 exit 1
fi

file=$1
shift

if [ ! -f "$file" ]; then
 echo "Error: '$file' was not found."

 exit 1
fi

for name in "$@"; do

 # NOTE: An entry is six commented lines - the "<script/>" that opens it sits one
 # line above the "<name>", so the start is found by looking back from the name.
 # The substitution matches the one the installer uses, turning "# " into " " so
 # the result lines up with the entries that were never commented out.
 awk -v name="$name" '
 {
  line[ NR ] = $0

  if( $0 == "#  <name>" name )
   start = NR - 1

  if( start && !finish && ( NR > start ) && ( $0 == "# </script>" ) )
   finish = NR
 }
 END {
  if( !finish )
  {
   print "Warning: no commented entry named \"" name "\" was found." > "/dev/stderr"
  }

  for( i = 1; i <= NR; i++ )
  {
   if( finish && ( i >= start ) && ( i <= finish ) )
    sub( /^# /, " ", line[ i ] )

   print line[ i ]
  }
 }' "$file" > "$file.tmp"

 mv "$file.tmp" "$file"

 echo "Enabled autoscript entry '$name'."
done

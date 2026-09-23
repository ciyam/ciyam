#!/bin/bash
# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

# NOTE: On a container's first run there is no web access token, so an "admin" one is
# created here using the same seeded bootstrap the regression suite performs in
# "tests/test_node_1_a.sh". The image ships with no system identity (it is erased
# during the build) because the seeded bootstrap expects to create one.
#
# The bootstrap runs in the background *against the server that goes on running*, rather
# than against a temporary one that is then stopped. This matters: the identity is
# encrypted with the admin password, so only the process that created it holds it
# unlocked. Restarting the server leaves the on-disk identity locked, and autoscripts do
# not run while it is locked - see "auto_script.cpp", which skips any entry whose name is
# not "*" prefixed. That in turn means "irc_store_or_restore" never runs and every room
# and message call fails with "IRC usage is not currently available."
#
# Consequence: use "docker rm" and "docker run" to get a clean container. A "docker
# restart" or "docker start" keeps the accounts but comes back with the identity locked
# and IRC unavailable, because nothing here re-supplies the entropy.

cd /ciyam/src || exit 1

show_banner( )
{
   if [ -f .web_access_admin ]; then
      echo ""
      echo "=================================================="
      echo " CIYAM chat prototype"
      echo ""
      echo "   chat     http://localhost:13031/chat.html"
      echo "   harness  http://localhost:13031/test_web_session.html"
      echo ""
      echo "   PIN      $(cat .web_access_admin)"
      echo "   password none"
      echo "=================================================="
      echo ""
   else
      echo "Error: Bootstrap did not produce an access token."
   fi
}

wait_for_server( )
{
   for i in $(seq 1 30); do
      if node -e "fetch('http://localhost:13031/cws').then(()=>process.exit(0)).catch(()=>process.exit(1))" 2>/dev/null; then
         return 0
      fi

      sleep 1
   done

   return 1
}

if [ ! -f .web_access_admin ]; then
   echo "(no access token found - bootstrapping 'admin')"

   (
      if ! wait_for_server; then
         echo "Error: Server did not start, so no bootstrap was attempted."

         exit 1
      fi

      seed=$(node -e 'console.log( require( "crypto" ).randomBytes( 67 ).toString( "hex" ) )')

      node /ciyam/webui/ciyam.js -test "" admin "$seed" "" none >/dev/null 2>&1

      unset seed

      show_banner
   ) &
else
   echo "(existing access token found - the system identity will be locked, so IRC is"
   echo " unavailable until the container is recreated rather than restarted)"

   ( show_banner ) &
fi

# NOTE: Exec so the server is PID 1 and receives "docker stop" directly.
exec ./ciyam_server -quiet -no_udp -no_peers

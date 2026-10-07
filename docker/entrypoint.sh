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

# NOTE: An ntfy server, when one is named (CIYAM_NTFY_SERVER - see the vault's "ntfy-container.sh").
# The server sends to "https://<ntfy_server>/<topic>" with curl, which checks the certificate, so a
# self-signed one given at /etc/ciyam-ntfy is trusted first. "ntfy_server" is set in the config
# before the server starts, as it is read only then.
#
# The name is a host, with a port if need be - "ntfy", not "https://ntfy" - as the server puts "https://"
# in front itself. Anything else is refused rather than written into the config (found by review).
case "${CIYAM_NTFY_SERVER:-}" in
   *[!A-Za-z0-9.:-]*)
      echo "(ntfy server '$CIYAM_NTFY_SERVER' ignored - a host name with a port if need be, not a URL)"
      CIYAM_NTFY_SERVER= ;;
esac

if [ -n "${CIYAM_NTFY_SERVER:-}" ]; then
   if [ -f /etc/ciyam-ntfy/ntfy.crt ]; then
      cp /etc/ciyam-ntfy/ntfy.crt /usr/local/share/ca-certificates/ciyam-ntfy.crt
      update-ca-certificates >/dev/null 2>&1
   fi

   sed -i "s|^#* *<ntfy_server>.*| <ntfy_server>$CIYAM_NTFY_SERVER|" ciyam_server.sio

   echo "(ntfy server: $CIYAM_NTFY_SERVER)"
fi

# NOTE: Admin's alerts - a stand-in, for the ntfy proof of concept, for what the server's own "at_init"
# could send. The topic comes with the certificate ("admin_topic") rather than from "ntfy_topic", which
# needs the identity - and while locked there is none to read; admin has no server topic anyway. Sent
# with curl directly, to try what "send_ntfy_message( )" does not send yet: a title, a priority, tags and
# a link opened when the notification is tapped (CIYAM_PUBLIC_URL - this node as the phone reaches it).
ntfy_alert( )
{
   local topic

   [ -n "${CIYAM_NTFY_SERVER:-}" ] && [ -f /etc/ciyam-ntfy/admin_topic ] || return 0

   topic=$(tr -d '\r\n' < /etc/ciyam-ntfy/admin_topic)

   curl -s -m 10 -o /dev/null -w "(ntfy alert: %{http_code})\n" \
    -H "Title: $1" -H "Priority: $2" -H "Tags: $3" \
    ${CIYAM_PUBLIC_URL:+-H "Click: $CIYAM_PUBLIC_URL/chat.html"} \
    -d "$4" "https://$CIYAM_NTFY_SERVER/$topic"
}

system_state( )
{
   node -e "fetch('http://localhost:13031/system',{signal:AbortSignal.timeout(5000)}).then(r=>r.text()).then(t=>process.stdout.write(t)).catch(()=>{})" 2>/dev/null
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

      # NOTE: Only when the bootstrap worked - otherwise "Set up and ready" would be untrue (found by review).
      if [ -f .web_access_admin ]; then
         ntfy_alert "Home node started" "low" "white_check_mark" "Set up and ready - admin's PIN is in the container's log."
      fi
   ) &
else
   echo "(existing access token found - the system identity will be locked, so IRC is"
   echo " unavailable until the container is recreated rather than restarted)"

   (
      show_banner

      # NOTE: The alert the Home design leads with - "the node restarted and needs unlocking". Sent only when
      # "/system" says so (":CIYAM:"), so a restart that somehow comes back unlocked stays quiet.
      if wait_for_server && [ "$(system_state | cut -c1-7)" = ":CIYAM:" ]; then
         ntfy_alert "Home node needs unlocking" "high" "lock" \
          "It restarted, and nobody can sign in until it's unlocked. Use one of your unlock keys."
      fi
   ) &
fi

# NOTE: A crash leaves a core dump where the host keeps them - Docker Desktop's WSL saves them under Windows'
# "%TEMP%\wsl-crashes"; a Linux host by its "core_pattern". Read one with gdb in a "CIYAM_CPP_OPTS=-g" build.
ulimit -c unlimited 2>/dev/null || true

# NOTE: Exec so the server is PID 1 and receives "docker stop" directly.
exec ./ciyam_server -quiet -no_udp -no_peers

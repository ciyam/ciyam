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

# NOTE: An image built with Meta ("CIYAM_META=1" in the Dockerfile) keeps that storage's records in MariaDB, which has to be
# answering before "ciyam_server" starts.
if [ -f Meta.dat ]; then
   /usr/local/bin/start_mariadb.sh || exit 1
fi

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
   # NOTE: The certificate authority that signed ntfy's certificate, where there is one (a local test CA, 2026-11-17) - OpenSSL
   # trusts only a certificate that signs itself as a root, so a signed one alone would not do; else ntfy's own, self-signed.
   if [ -f /etc/ciyam-ntfy/ca.crt ]; then
      cp /etc/ciyam-ntfy/ca.crt /usr/local/share/ca-certificates/ciyam-ntfy-ca.crt
      update-ca-certificates >/dev/null 2>&1
   elif [ -f /etc/ciyam-ntfy/ntfy.crt ]; then
      cp /etc/ciyam-ntfy/ntfy.crt /usr/local/share/ca-certificates/ciyam-ntfy.crt
      update-ca-certificates >/dev/null 2>&1
   fi

   sed -i "s|^#* *<ntfy_server>.*| <ntfy_server>$CIYAM_NTFY_SERVER|" ciyam_server.sio

   echo "(ntfy server: $CIYAM_NTFY_SERVER)"
fi

# NOTE: The node's own certificate, when one is given at /etc/ciyam-tls (key and certificate in one file, as "create_ciyam_pem"
# makes it) - one a phone can trust, signed by a test CA it has installed, rather than the self-signed one made on first start
# (2026-11-17). In place before the server starts, which makes its own only when there is none.
if [ -f /etc/ciyam-tls/ciyam.pem ]; then
   # NOTE: A certificate that reads as one, and the key its own - the two public keys alike (found by review, 2026-11-17).
   cert_key=$( openssl x509 -in /etc/ciyam-tls/ciyam.pem -noout -pubkey 2>/dev/null )
   own_key=$( openssl pkey -in /etc/ciyam-tls/ciyam.pem -pubout 2>/dev/null )
   if [ -n "$cert_key" ] && [ "$cert_key" = "$own_key" ]; then
      cp /etc/ciyam-tls/ciyam.pem ciyam.pem
      chmod o-r ciyam.pem
      echo "(TLS certificate: given - $( openssl x509 -in ciyam.pem -noout -subject 2>/dev/null ))"
   else
      echo "(TLS certificate given at /etc/ciyam-tls ignored - not a certificate with its own key)"
   fi
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

# NOTE: A node left new - "CIYAM_NO_SETUP=1" - so setting one up from Home can be tried (Damon, 2026-10-12): "/system"
# answers "*CIYAM*" until someone does. Whoever is first sets it up, so only on a node nobody else can reach.
if [ ! -f .web_access_admin ] && [ "${CIYAM_NO_SETUP:-}" = "1" ]; then
   echo "(no access token found - left NEW, to be set up from Home: http://localhost:13031/home.html)"
elif [ ! -f .web_access_admin ]; then
   echo "(no access token found - bootstrapping 'admin')"

   (
      if ! wait_for_server; then
         echo "Error: Server did not start, so no bootstrap was attempted."

         exit 1
      fi

      seed=$(node -e 'console.log( require( "crypto" ).randomBytes( 67 ).toString( "hex" ) )')

      # NOTE: A test node only - set up from the entropy given, so its twelve words are known and recovering the node
      # with them can be tried (Damon, 2026-10-09). Thirty-two hex characters are twelve words. Never for a real node:
      # whoever has the words can take the node over.
      if [[ "${CIYAM_TEST_ENTROPY:-}" =~ ^[0-9a-f]{32}$ ]]; then
         seed="$CIYAM_TEST_ENTROPY"

         echo "(a TEST node - set up from known entropy, so its twelve words are known)"
      fi

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

# NOTE: The relay for calls between networks (coturn, 2026-11-15) - only with "CIYAM_TURN_SECRET". Its shared secret
# ("use-auth-secret"): the node is to hand a signed-in person short-lived credentials made with the same secret, so no
# password is kept for anyone. "CIYAM_TURN_EXTERNAL_IP" - the address phones reach this node by, needed behind NAT (a
# container always is); "CIYAM_TURN_REALM" - its name. Peers on private networks are refused, so the relay cannot be used
# to reach anything on the host's own network - though not the node's own address, which is the peer when both use it. Plain
# TURN on 3478 for now - TLS (5349) once the node has a certificate; the media itself is encrypted end to end (DTLS-SRTP).
# Eight allocations to one set of credentials - a browser makes one for each of its network adapters, and the node hands out
# new credentials for each call - and forty in all, as the forty-one ports allow (a closed call's are counted until they lapse);
# only UDP relayed, as WebRTC uses; run as its own user. (Reviewed 2026-11-15.)
if [ -n "${CIYAM_TURN_SECRET:-}" ]; then
   # NOTE: Letters and digits only - a line break would add lines to the file, and spaces are trimmed ("openssl rand -hex 32").
   # No more than 64, as the node hands out credentials made with it ("ciyam_web_session.cpp") and refuses a longer one.
   if ! printf '%s' "$CIYAM_TURN_SECRET" | grep -qE '^[A-Za-z0-9]{16,64}$'; then
      echo "Error: CIYAM_TURN_SECRET is to be 16 to 64 letters and digits (openssl rand -hex 32) - the call relay is not started."
   else
      # NOTE: An address, not a name (coturn takes only an address here) - anything else is left out rather than written in. A "."
      # or a ":" too, so a name all of hex letters isn't taken for one; the realm a name (found by review, 2026-11-15).
      bad=0
      case "${CIYAM_TURN_EXTERNAL_IP:-}" in
         *[!0-9A-Fa-f.:]*) bad=1 ;;
      esac
      case "${CIYAM_TURN_EXTERNAL_IP:-}" in
         *[.:]*) ;;
         *) bad=1 ;;
      esac
      if [ -n "${CIYAM_TURN_EXTERNAL_IP:-}" ] && [ "$bad" = "1" ]; then
         echo "(call relay address '$CIYAM_TURN_EXTERNAL_IP' ignored - an IP address, not a name)"
         CIYAM_TURN_EXTERNAL_IP=
      fi

      case "${CIYAM_TURN_REALM:-}" in
         *[!A-Za-z0-9.-]*)
            echo "(call relay realm '$CIYAM_TURN_REALM' ignored - letters, digits, dots and dashes)"
            CIYAM_TURN_REALM= ;;
      esac

      # NOTE: The port it listens on - "CIYAM_TURN_PORT", which the node also hands out with the credentials, or 3478 (2026-11-17).
      turn_port=3478
      if [ -n "${CIYAM_TURN_PORT:-}" ]; then
         # NOTE: As a number - "03478" as 3478, as the node hands it out (found by review, 2026-11-17).
         if printf '%s' "$CIYAM_TURN_PORT" | grep -qE '^[0-9]{1,5}$' && [ $(( 10#$CIYAM_TURN_PORT )) -ge 1 ] && [ $(( 10#$CIYAM_TURN_PORT )) -le 65535 ]; then
            turn_port=$(( 10#$CIYAM_TURN_PORT ))
         else
            echo "(call relay port '$CIYAM_TURN_PORT' ignored - a number from 1 to 65535)"
         fi
      fi

      # NOTE: "public/private" - the address phones reach, and this container's own that it stands for.
      private_ip=$(hostname -i | awk '{ print $1 }')

      (
         umask 077

         {
            echo "listening-port=$turn_port"
            echo "min-port=49160"
            echo "max-port=49200"
            echo "fingerprint"
            echo "use-auth-secret"
            echo "static-auth-secret=$CIYAM_TURN_SECRET"
            echo "realm=${CIYAM_TURN_REALM:-ciyam}"
            if [ -n "${CIYAM_TURN_EXTERNAL_IP:-}" ]; then
               echo "external-ip=$CIYAM_TURN_EXTERNAL_IP${private_ip:+/$private_ip}"
            fi
            echo "user-quota=8"
            echo "total-quota=40"
            echo "stale-nonce=600"
            echo "no-tcp-relay"
            echo "no-tls"
            echo "no-dtls"
            echo "no-cli"
            echo "no-software-attribute"
            echo "no-multicast-peers"
            for range in 0.0.0.0-0.255.255.255 10.0.0.0-10.255.255.255 100.64.0.0-100.127.255.255 127.0.0.0-127.255.255.255 \
             169.254.0.0-169.254.255.255 172.16.0.0-172.31.255.255 192.0.0.0-192.0.0.255 192.168.0.0-192.168.255.255 \
             198.18.0.0-198.19.255.255 240.0.0.0-255.255.255.255 ::1 ::ffff:0.0.0.0-::ffff:255.255.255.255 \
             64:ff9b::-64:ff9b::ffff:ffff fc00::-fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff fe80::-febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff; do
               echo "denied-peer-ip=$range"
            done
            # NOTE: Except the relay's own address - the peer when both people are relayed - should it be in one of those ranges (a
            # test node reached at its LAN address).
            if [ -n "${CIYAM_TURN_EXTERNAL_IP:-}" ]; then
               echo "allowed-peer-ip=$CIYAM_TURN_EXTERNAL_IP"
            fi
            echo "proc-user=turnserver"
            echo "proc-group=turnserver"
            echo "simple-log"
            echo "log-file=/var/log/turnserver.log"
         } > /etc/turnserver.conf
      )

      touch /var/log/turnserver.log
      chown turnserver:turnserver /var/log/turnserver.log

      if turnserver -c /etc/turnserver.conf --daemon; then
         echo "(call relay: coturn on $turn_port${CIYAM_TURN_EXTERNAL_IP:+, reached at $CIYAM_TURN_EXTERNAL_IP})"
      else
         echo "Error: the call relay (coturn) did not start - see /var/log/turnserver.log."
      fi
   fi
fi

# NOTE: A crash leaves a core dump where the host keeps them - Docker Desktop's WSL saves them under Windows'
# "%TEMP%\wsl-crashes"; a Linux host by its "core_pattern". Read one with gdb in a "CIYAM_CPP_OPTS=-g" build.
ulimit -c unlimited 2>/dev/null || true

# NOTE: Exec so the server is PID 1 and receives "docker stop" directly.
exec ./ciyam_server -quiet -no_udp -no_peers

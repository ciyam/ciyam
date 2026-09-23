# Copyright (c) 2026 CIYAM Developers
#
# Distributed under the MIT/X11 software license, please refer to the file license.txt
# in the root project directory or http://www.opensource.org/licenses/mit-license.php.

# NOTE: Builds just enough CIYAM to serve the web session (CWS) API and the chat
# prototype. The full development guide in "linux_lubuntu.txt" also sets up Apache,
# MariaDB and the Meta application - none of that is needed here, because
# "ciyam_server" carries its own HTTP listener on port 13031.
#
# The target definition in "makefile.sio.xrep" declares "ciyam_server" with
# <libfcgi>false and <mysqldb>false, so neither the CIYAM fork of fcgi nor any
# MySQL client library is required. Only "test_fcgi" needs fcgi, and nothing here
# needs it at all.
#
# Build from the repository root:
#   docker build -f docker/Dockerfile -t ciyam-server .

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV WEBDIR=/var/www/html
ENV CIYAM_USER=root
ENV CIYAM_GROUP=root

# NOTE: The MySQL headers are needed even though "ciyam_server" declares
# <mysqldb>false - that flag only governs whether the MySQL libraries are linked.
# The "base" library it depends on still compiles "sql_db.cpp", which includes
# <mysql/mysql.h>, so the development headers have to be present to build at all.
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential make ca-certificates openssl dos2unix \
      libssl-dev zlib1g-dev libpng-dev libhpdf-dev \
      libreadline-dev libncurses-dev libtinfo-dev \
      libmysqlclient-dev libmysqlcppconn-dev \
      nodejs \
 && rm -rf /var/lib/apt/lists/*

# NOTE: The build links with "-ltermcap", which no longer ships as its own library
# on Ubuntu - the symbols live in libtinfo.
RUN if [ ! -e /usr/lib/x86_64-linux-gnu/libtermcap.so ]; then \
      ln -s /usr/lib/x86_64-linux-gnu/libtinfo.so /usr/lib/x86_64-linux-gnu/libtermcap.so; \
    fi

# NOTE: Only these two trees are needed to build and serve. Copying them rather
# than the whole repository keeps ".git" and the test corpus out of the image.
COPY src/ /ciyam/src/
COPY webui/ /ciyam/webui/

# NOTE: A Windows checkout with core.autocrlf=true leaves CRLF terminators, which
# make every shell script fail with "bad interpreter". dos2unix skips binaries.
RUN find /ciyam -type f -not -path "*/fonts/*" -exec dos2unix -q {} + 2>/dev/null || true

WORKDIR /ciyam/src

# NOTE: "config" bootstraps itself - it copies the ".default" files, builds the
# "xrep" and "xvars" tools using "makefile.tools", then generates the real makefile.
RUN mkdir -p /tmp/ciyam "$WEBDIR" /ciyam/src/.files \
 && ./config

# NOTE: Stops "set_password" being run when the admin password is first set. That
# script uses "sudo chpasswd" to set a *Linux* account password, which is neither
# possible nor meaningful in a container - there is no sudo, and the container user
# is not a login account. Without this the bootstrap fails part way through and the
# system identity is left locked, which blocks every non-admin session and the
# "system_variable" tool along with it.
#
# The file name is read both in "class_base.cpp" and in the "set_password" script,
# and is keyed on CIYAM_USER. Confirmed with Ian on 2026-09-20 as the supported way
# to run where the Linux account password cannot be set.
RUN touch "/ciyam/src/.${CIYAM_USER}_password_protected"

# NOTE: "config" creates a system identity, but the seeded "admin" bootstrap that the
# entrypoint performs expects to create one itself - so it is erased here, exactly as
# "tests/test_node_init.sh" does before running its own bootstrap.
RUN ./save_identity && ./erase_identity

# NOTE: IRC only runs if the "irc_store_or_restore" autoscript entry is active, and
# "autoscript.sio.default" ships it commented out so that a new development
# environment starts minimal. The "install" script uncomments every entry; only this
# one is enabled here, because "auto_update" and "ciyam_ddns_ip_updates" have no
# place in a container. Confirmed with Ian on 2026-09-20.
COPY docker/enable_autoscript.sh /usr/local/bin/enable_autoscript.sh
RUN chmod +x /usr/local/bin/enable_autoscript.sh && dos2unix -q /usr/local/bin/enable_autoscript.sh && /usr/local/bin/enable_autoscript.sh /ciyam/src/autoscript.sio irc_store_or_restore

# NOTE: Building named targets rather than "make all" for two reasons - the rest of
# the tree is the IDE, the storage modules and the test tools, none of which serve
# CWS; and "all" includes "test_fcgi", which would need the CIYAM fork of fcgi that
# is deliberately not installed here.
#
# "ciyam_server" loads "ciyam_base.so" dynamically at run time, so the shared library
# is a separate target and has to be named explicitly alongside the executable.
#
# "ciyam_client" is needed too - starting a web session spawns it as a child process
# to run "web_session.cin" (see the system( ) call in "ciyam_web_session.cpp"), so
# without it every session start fails with "Was unable to start a web session".
RUN make ciyam_base.so ciyam_client ciyam_server

# NOTE: Static files are served from "g_html_dir" in "http_handler.cpp", which is
# the fixed relative path "html" - not <web_root>, which belongs to the Meta
# application's storage. The source comment expects "html" to be a soft link, so
# that is what it is made here.
RUN ln -sfn "$WEBDIR" /ciyam/src/html

# NOTE: Both the harness and the chat prototype are published into the web root.
RUN set -e; \
    for ext in js css html form; do \
       cp -f /ciyam/webui/*."$ext" "$WEBDIR"/ 2>/dev/null || true; \
    done; \
    if [ -d /ciyam/webui/fonts ]; then cp -a /ciyam/webui/fonts "$WEBDIR"/; fi; \
    ls -1 "$WEBDIR" | head -40

# 12021 CIYAM peer protocol, 13031 web session (CWS) and static files
EXPOSE 12021 13031

# NOTE: No VOLUME is declared deliberately. A "docker restart" keeps the container's
# filesystem and therefore the accounts; a "docker rm" followed by "docker run" starts
# clean and bootstraps a new PIN. Persisting only ".files" would leave server state
# behind while the access token was regenerated, which is worse than either.

COPY docker/entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh && dos2unix -q /usr/local/bin/entrypoint.sh

ENTRYPOINT [ "/usr/local/bin/entrypoint.sh" ]

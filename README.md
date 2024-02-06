CIYAM Software Manufacturing
============================

The following environment variables are required by various scripts:

Name   | Description                 | Sample Value
------ | --------------------------- | ---------------
WEBDIR | Web Document Root path**    | /srv/www/htdocs

** This path must *not* contain spaces and each directory in the path must have "rwx" permissions set so that
both the Apache2 user (wwwrun) and the user account being used for development can create, update, append and
delete files in and below the path (for a development only environment it is quickest to just `chmod a+rwx`).

An optional MARKER environment variable can be used as a way of preventing accidental source code commits due
to some temporary tracing code (as it is used in the pre-commit hook script's call to 'check_source').

The development packages for `libpng` (and optionally `readline`) will also need to be installed.

NOTE: More specific OS dependency installation instructions can be found in additional "<os_name>.txt" files.

MySQL
-----
http://dev.mysql.com/downloads/mysql/

Is normally standard but you may need to install the appropriate developer package (e.g. mysql-devel).

FastCGI
-------

```
git clone https://github.com/ciyam/fcgi.git
cd fcgi

./configure
make
su
make install
```

NOTE: If you get error messages about undefined EOF then modify include/fcgio.h as follows:

```
 #include <iostream>
+#include <stdio.h>

 #include "fcgiapp.h"

```

Haru PDF Library
----------------
http://libharu.org

Is normally standard but you may need to install the appropriate developer package (e.g. libharu-devel).

```
unzip libharu-RELEASE_2_2_0.zip
cd libharu-2.2.0

./configure
make
su
make install

cp /usr/local/lib/libhpdf-2.2.0.so /usr/lib (depending upon distro)
```

OpenSSL
-------
http://www.openssl.org/

Is normally standard but if necessary then:

```
tar xzf openssl-1.0.1h.tar.gz
cd openssl-1.0.1h

./config shared
make
make test
su
make install

cp /usr/local/ssl/lib/libssl.so.1.0.1 /usr/lib (depending upon distro)
cp /usr/local/ssl/lib/libcrypto.so.1.0.1 /usr/lib (depending upon distro)
```

Other General
-------------

Note that "ciyam_server.pem" and "ciyam_client.pem" are identical public/private test only SSL keys encrypted
with the password "password".

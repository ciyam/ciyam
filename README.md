CIYAM Software Manufacturing
============================

The following environment variables are required by various scripts:

Name   | Description                 | Sample Value
------ | --------------------------- | -------------
WEBDIR | Web Document Root path**    | /var/www/html

** This path must *not* contain spaces and each directory in the path must have "rwx" permissions set so that
both the Apache2 user (wwwrun) and the user account being used for development can create, update, append and
delete files in and below this path.

An optional CIYAM_CPP_OPTS environment variable can be used to include extra compiler options (which is handy
for development environments where debug information is desirable):

export CIYAM_CPP_OPTS=-g

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

// Copyright (c) 2000
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

#define HAS_PRECOMPILED_STD_HEADERS

#ifdef _WIN32
#  ifndef STRICT
#     define STRICT // Needed for "windows.h" by various Borland headers.
#  endif
#  define NO_MINMAX
#  define _WINSOCKAPI_
#  include <windows.h>
#endif

#define __STDC_CONSTANT_MACROS

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <csignal>

#include <cmath>

#include <ctime>
#include <cerrno>
#include <cfcntl>

// NOTE: This is being used rather than "cstddef" for "size_t" (rather than "std::size_t") to help compatibility.
#include <stddef.h>

#include <share.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/locking.h>

#ifdef __BORLANDC__
#  include <cio>
#  include <dos.h>
#  include <dir.h>
#endif
#ifdef _MSC_VER
#  include <io.h>
#  include <direct.h>
#endif

#include <limits>
#include <memory>

#include <set>
#include <map>
#include <list>
#include <deque>
#include <vector>

#include <stack>

#include <utility>

#include <algorithm>
#include <functional>

#include <string>
#include <sstream>

#include <fstream>

#include <iomanip>
#include <iostream>

#include <typeinfo>

#include <exception>
#include <stdexcept>


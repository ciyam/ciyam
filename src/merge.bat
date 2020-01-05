@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set error_level=0

if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage

if not exist %1 goto error1
if not exist %2 goto error2
if not exist %3 goto error3

diff -q -min -rcs %1 %2 > ~merge1.tmp
if not exist ~merge1.tmp goto end
if not errorlevel 1 goto next1
goto end

:next1
diff -q -min -rcs %1 %3 > ~merge2.tmp
if not exist ~merge2.tmp goto end
if not errorlevel 1 goto next2
goto end

:next2
diffp -q -min %1 ~merge1.tmp ~merge2.tmp
if errorlevel 2 goto error4
if errorlevel 1 goto error5
goto end

:usage
echo Usage: merge [base file] [first variation] [second variation]
goto end

:error1
echo Error: File '%1' does not exist.
set error_level=1
goto end

:error2
echo Error: File '%2' does not exist.
set error_level=1
goto end

:error3
echo Error: File '%3' does not exist.
set error_level=1
goto end

:error4
echo Error: Unexpected error merging '%2' and '%3' from '%1'.
set error_level=%errorlevel%
goto end

:error5
echo Error: Conflicts occurred whilst merging '%2' and '%3' from '%1'.>con

:end
if exist ~merge1.tmp del ~merge1.tmp
if exist ~merge2.tmp del ~merge2.tmp
exit /B %error_level%
endlocal


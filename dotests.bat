@echo off
REM Copyright (c) 2012 CIYAM Open Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto next

test -q tests.sio test_%1
goto end

:next
test -q tests.sio

:end


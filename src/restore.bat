@echo off
REM Copyright (c) 2017-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal
if '%1' == '' goto usage

if not exist packages.lst goto skip
construct @packages.lst ciyam_class.cpp.xrep

:skip
if '%1' == '-quicker' goto next
set CIYAM_STORAGE=%1
ciyam_client -quiet -no_prompt < restore.cin
goto end

:next
if '%2' == '' goto usage
set CIYAM_STORAGE=%2
ciyam_client -quiet -no_prompt < restore_quicker.cin
goto end

:usage
echo Usage: restore [[-quicker]] [app name]

:end
endlocal


@echo off
REM Copyright (c) 2022-2023 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set RESTORED=

if "%WEBDIR%" == "" goto error1

if not exist "ciyam_server.sid.sav" goto skip1
set RESTORED=1
move "ciyam_server.sid.sav" "ciyam_server.sid"

:skip1
if not exist "%WEBDIR%\meta\identity.txt.sav" goto skip2
set RESTORED=1
move "%WEBDIR%\meta\identity.txt.sav" "%WEBDIR%\meta\identity.txt"

:skip2
if not exist "%WEBDIR%\meta\encrypted.txt.sav" goto next
set RESTORED=1
move "%WEBDIR%\meta\encrypted.txt.sav" "%WEBDIR%\meta\encrypted.txt"

:next2
if "%RESTORED%" == "" goto end
echo NOTE: Will need to restart both the application and HTTP server.
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.

:end
endlocal

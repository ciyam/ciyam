@echo off
REM Copyright (c) 2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if "%WEBDIR%" == "" goto error1
if not exist "ciyam_server.sid.sav" goto error2

if exist ciyam_server.sid del ciyam_server.sid

if exist "%WEBDIR%\meta\identity.txt" del "%WEBDIR%\meta\identity.txt"
if exist "%WEBDIR%\meta\encrypted.txt" del "%WEBDIR%\meta\encrypted.txt"

echo NOTE: Will need to restart both the application and HTTP server.

goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:error2
echo Error: Saved identity not found (try running 'save_identity' first).

:end

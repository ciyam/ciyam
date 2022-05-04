@echo off
REM Copyright (c) 2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if "%WEBDIR%" == "" goto error1

if not exist "ciyam_server.sid" goto skip
copy /y "ciyam_server.sid" "ciyam_server.sid.sav" >nul

:skip
if not exist "%WEBDIR%\meta\identity.txt" goto next
copy /y "%WEBDIR%\meta\identity.txt" "%WEBDIR%\meta\identity.txt.sav" >nul

:next
if not exist "%WEBDIR%\meta\encrypted.txt" goto end
copy /y "%WEBDIR%\meta\encrypted.txt" "%WEBDIR%\meta\encrypted.txt.sav" >nul
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.

:end

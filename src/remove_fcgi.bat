@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

if '%2' == '' goto skip
set WEBDIR=%2

:skip
if "%WEBDIR%" == "" goto error1
if not exist "%WEBDIR%\%1\fcgi.sio" goto error2

if exist "%WEBDIR%\%1\upload.kill.bat" del "%WEBDIR%\%1\upload.kill.bat"
if exist "%WEBDIR%\%1\ciyam_interface.kill.bat" del "%WEBDIR%\%1\ciyam_interface.kill.bat"

REM Use PING to approximate a 2 second pause...
PING 127.0.0.1 -n 3 >nul

if exist "%WEBDIR%\%1\upload.kill.bat" call "%WEBDIR%\%1\upload.kill.bat"
if exist "%WEBDIR%\%1\ciyam_interface.kill.bat" call "%WEBDIR%\%1\ciyam_interface.kill.bat"
rmdir /s /q "%WEBDIR%\%1"
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:error1
echo Error: File '%WEBDIR%\%1\fcgi.sio' does not exist.
goto end

:usage
echo Usage: remove_fcgi [app dir] [[web dir]]

:end


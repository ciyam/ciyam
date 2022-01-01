@echo off
REM Copyright (c) 2012-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage

if "%WEBDIR%" == "" goto error1

set directory=%2
set install_path=%WEBDIR%\%directory%

if not exist %1.exe goto error2
if not exist %install_path%\fcgi.sio goto error3

if exist "%WEBDIR%\%directory%\%1.kill.bat" del "%WEBDIR%\%directory%\%1.kill.bat"
REM Use PING to approximate a 2 second pause...
PING 127.0.0.1 -n 3 >nul
if exist "%WEBDIR%\%directory%\%1.kill.bat" call "%WEBDIR%\%directory%\%1.kill.bat"

:next
echo Installing %1 to %install_path%
copy %1.exe "%install_path%\%1.fcgi"

:extra
shift
if '%2' == '' goto done
copy %2 "%install_path%"
goto extra

:done
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:error2
echo Error: File '%1.exe' was not found.
goto end

:error3
echo Error: File '%install_path%\fcgi.sio' was not found.
goto end

:usage
echo Usage: install_fcgi [FCGI w/o ext] [app dir] [[extra files]]

:end
endlocal


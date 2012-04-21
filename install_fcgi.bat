@echo off
setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage

if "%WEBDIR%" == "" goto error1

set directory=%2
set install_path=%WEBDIR%\%directory%

if not exist %1.exe goto error2
if not exist %install_path%\fcgi.sio goto error3

if not '%WEBSVC%' == '' net stop %WEBSVC%
ping 127.0.0.1 -n 2 > nul
echo Installing %1 to %install_path%
copy %1.exe "%install_path%\%1.fcgi"

:extra
shift
if '%2' == '' goto done
copy %2 "%install_path%"
goto extra

:done
if not '%WEBSVC%' == '' net start %WEBSVC%
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


@echo off
setlocal
if '%1' == '' goto usage

if '%2' == 'ciyam' goto skip
if '%WEBDIR%' == '' goto error1

:skip
set CIYAM_MODEL=%1
set CIYAM_STORAGE=%1
if not '%2' == '' set CIYAM_STORAGE=%2
if '%2' == 'ciyam' set CIYAM_TEST_STORAGE=1
del %CIYAM_STORAGE%.log %CIYAM_STORAGE%.dat %CIYAM_STORAGE%.idx %CIYAM_STORAGE%.hdr >nul
start /min ciyam_server -quiet
ping 127.0.0.1 -n 2 > nul
ciyam_client -quiet -no_prompt < init.cin
touch ciyam_server.stop
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:usage
echo Usage: init [model name] [[app name]]

:end
endlocal


@echo off
setlocal
if '%1' == '' goto usage

set CIYAM_MODEL=%1
set CIYAM_STORAGE=%1
if not '%2' == '' set CIYAM_STORAGE=%2
del %CIYAM_STORAGE%.log %CIYAM_STORAGE%.dat %CIYAM_STORAGE%.idx %CIYAM_STORAGE%.hdr >nul
ping 127.0.0.1 -n 2 > nul
start /min ciyam_server -quiet
ciyam_client -quiet -no_prompt < init.cin
touch ciyam_server.stop
goto end

:usage
echo Usage: init [model name] [[app name]]

:end
endlocal


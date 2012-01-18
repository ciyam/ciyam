@echo off
setlocal
if '%1' == '' goto usage

set CIYAM_MODEL=%1
set CIYAM_STORAGE=%1
if not '%2' == '' set CIYAM_STORAGE=%2
del %CIYAM_STORAGE%.log %CIYAM_STORAGE%.dat %CIYAM_STORAGE%.idx %CIYAM_STORAGE%.hdr >nul
start ciyam_server 12345
ciyam_client -quiet -no_prompt 12345 < init.cin
touch ciyam_server.stop
goto end

:usage
echo Usage: init [model name] [[app name]]

:end
endlocal


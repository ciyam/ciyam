@echo off
setlocal
if '%1' == '' goto usage

del %1.modules.lst %1.log %1.dat %1.idx %1.hdr >nul
set CIYAM_MODEL=%1
set CIYAM_STORAGE=%1
if not '%2' == '' set CIYAM_STORAGE=%2
cat_client -quiet -no_prompt < init.cin
goto end

:usage
echo Usage: init [model name] [[app name]]

:end
endlocal


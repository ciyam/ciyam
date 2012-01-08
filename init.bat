@echo off
setlocal
if '%1' == '' goto usage

del %1.modules.lst %1.log %1.dat %1.idx %1.hdr >nul
set CAT_APPLICATION=%1
cat_client -quiet -no_prompt < init.cin
goto end

:usage
echo Usage: init [app name]

:end
endlocal


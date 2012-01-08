@echo off

if '%1' == '' goto usage

if not exist %1.manifest goto end
echo Manifesting %1...............
mt /nologo /manifest %1.manifest /outputresource:%1
del %1.manifest
goto end

:usage
echo Usage: manifest [target]

:end


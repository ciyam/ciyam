@echo off
REM Copyright (c) 2012-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

if not exist %1.manifest goto end
echo Manifesting %1...............
mt /nologo /manifest %1.manifest /outputresource:%1
del %1.manifest
goto end

:usage
echo Usage: manifest [target]

:end


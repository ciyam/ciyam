@echo off
REM Copyright (c) 2012-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

if exist %1 goto check_content
echo Created %2
goto end

:check_content
echo n|comp %1 %2 >nul 2>&1
if not errorlevel 1 goto remove_temp_file
echo Created %2
goto end

:remove_temp_file
del %2
goto end

:usage
echo Usage: remove_identical [old file] [new file]

:end


@echo off
REM Copyright (c) 2012-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

if exist %1 goto check_content
echo Created %1
goto file_copy

:check_content
echo n|comp %1 %2 >nul 2>&1
if not errorlevel 1 goto remove_temp_file
echo Updated %1

:file_copy
copy %2 %1 >nul

:remove_temp_file
del %2
goto end

:usage
echo Usage: update [dest file] [temp file]

:end


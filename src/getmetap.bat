@echo off
REM Copyright (c) 2012-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage

if '%CIYAM_PACKAGE%' == '' goto error1

set mtype=%1
set keyval=%2
set fields=%3

:loop
if '%4' == '' goto next
set fields=%fields%,%4
shift
goto loop

:next
call getmeta.bat %mtype% %keyval% "%fields%" %CIYAM_PACKAGE% >~getmetap

VER | FINDSTR /IL "5.1." > NUL
IF %ERRORLEVEL% EQU 0 clipset @~getmetap

VER | FINDSTR /IL "6." > NUL
IF %ERRORLEVEL% EQU 0 type ~getmetap | clip

del ~getmetap
echo (result is on the clipboard)
goto end

:error1
echo Error: Missing environment variable 'CIYAM_PACKAGE'.
goto end

:usage
echo Usage: getmetap [meta type] [key val] [field1,[field2,[...]]]

:end


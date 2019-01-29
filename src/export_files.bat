@echo off
REM Copyright (c) 2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

echo ^<^<export_files.cin %1 %2 >~export_files
ciyam_client -echo -quiet -no_prompt -no_stderr < ~export_files

del ~export_files
goto end

:usage
echo Usage: export_files [list tag] [start path]

:end

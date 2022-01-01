@echo off
REM Copyright (c) 2019-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

echo ^<^<export_app_file.cin %1 %2 %3 >~export_app_file
ciyam_client -echo -quiet -no_prompt -no_stderr < ~export_app_file

del ~export_app_file
goto end

:usage
echo Usage: export_app_file [app dir] [list file hash] [[web root]]

:end

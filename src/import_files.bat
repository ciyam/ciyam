@echo off
REM Copyright (c) 2019-2023 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<import_files.cin %1 %2 %3 %4 %5 >~import_files
ciyam_client -echo -quiet -no_prompt -no_stderr < ~import_files

del ~import_files
goto end

:usage
echo Usage: import_files [start path] [[tag prefix] [[sub-directory] [[file includes/excludes]] [[path includes/excludes]]]]

:end

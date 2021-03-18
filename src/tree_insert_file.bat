@echo off
REM Copyright (c) 2019-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage
if '%4' == '' goto usage
if '%5' == '' goto usage
if '%6' == '' goto usage

echo ^<^<tree_insert_file.cin %1 %2 %3 %4 %5 %6 %7 >~tree_insert_file
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_insert_file

del ~tree_insert_file
goto end

:usage
echo Usage: tree_insert_file [tree tag] [branch type] [branch tree] [sub-directory path] [file name] [base path] [[tag prefix]]

:end

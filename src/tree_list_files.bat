@echo off
REM Copyright (c) 2019-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

echo ^<^<tree_list_files.cin %1 %2 %3 >~tree_list_files
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_list_files

del ~tree_list_files
goto end

:usage
echo Usage: tree_list_files [tree tag] [branch type] [[branch tree expression]]

:end

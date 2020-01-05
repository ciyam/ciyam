@echo off
REM Copyright (c) 2019-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage
if '%4' == '' goto usage

echo ^<^<tree_list_tagged_files.cin %1 %2 %3 %4 >~tree_list_tagged_files
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_list_tagged_files

del ~tree_list_tagged_files
goto end

:usage
echo Usage: tree_list_tagged_files [tree tag] [branch type] [tag list,] [branch tree expression]

:end

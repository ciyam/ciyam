@echo off
REM Copyright (c) 2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

echo ^<^<tree_insert_branch.cin %1 %2 >~tree_insert_branch
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_insert_branch

del ~tree_insert_branch
goto end

:usage
echo Usage: tree_insert_branch [tree tag] [branch tag]

:end

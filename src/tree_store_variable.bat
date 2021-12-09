@echo off
REM Copyright (c) 2019-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage

echo ^<^<tree_store_variable.cin %1 %2 %3 %4 >~tree_store_variable
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_store_variable

del ~tree_store_variable
goto end

:usage
echo Usage: tree_store_variable [tree tag] [branch expr] [variable name] [[variable data]]

:end

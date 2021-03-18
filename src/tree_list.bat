@echo off
REM Copyright (c) 2019-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<tree_list.cin %1 %2 %3 >~tree_list
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_list

del ~tree_list
goto end

:usage
echo Usage: tree_list [tag name] [[depth [prefix]]]

:end

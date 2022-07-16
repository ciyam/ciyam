@echo off
REM Copyright (c) 2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

echo ^<^<tree_remove_trunk.cin %1 %2 >~tree_remove_trunk
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_remove_trunk

del ~tree_remove_trunk
goto end

:usage
echo Usage: tree_remove_trunk [tree tag] [trunk item]

:end

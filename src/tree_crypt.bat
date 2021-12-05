@echo off
REM Copyright (c) 2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<tree_crypt.cin %1 %2 >~tree_crypt
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_crypt

del ~tree_crypt
goto end

:usage
echo Usage: tree_crypt [[!]tag name] [[password]]]

:end

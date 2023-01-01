@echo off
REM Copyright (c) 2022-2023 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<tree_touch.cin %1 %2 >~tree_touch
ciyam_client -echo -quiet -no_prompt -no_stderr < ~tree_touch

del ~tree_touch
goto end

:usage
echo Usage: tree_touch [tag or hash] [[prefix]]

:end

@echo off
REM Copyright (c) 2022-2023 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<block_tree_relegate.cin %1 %2 >~block_tree_relegate
ciyam_client -echo -quiet -no_prompt -no_stderr < ~block_tree_relegate

del ~block_tree_relegate
goto end

:usage
echo Usage: block_tree_relegate [<identity> else "-backup" or "-shared" [tree tag]]

:end

@echo off
REM Copyright (c) 2019-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

echo ^<^<init_ciyam_tree.cin >~init_ciyam_tree
ciyam_client -echo -quiet -no_prompt -no_stderr < ~init_ciyam_tree
del ~init_ciyam_tree

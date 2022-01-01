@echo off
REM Copyright (c) 2021-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

echo ^<^<init_identity.cin >~init_identity
ciyam_client -echo -quiet -no_prompt -no_stderr < ~init_identity
del ~init_identity

@echo off
REM Copyright (c) 2022-2023 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

echo ^<^<generate_next_block.cin %1 >~generate_next_block
ciyam_client -echo -quiet -no_prompt -no_stderr < ~generate_next_block
del ~generate_next_block

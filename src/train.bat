@echo off
REM Copyright (c) 2017 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

ciyam_client -quiet -no_prompt < train_%1_algos.cin
goto end

:usage
echo Usage: train [name]

:end


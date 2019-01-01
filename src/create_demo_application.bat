@echo off
REM Copyright (c) 2017-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal
if '%1' == '' goto usage
if '%2' == '' goto usage

if not exist create_demo_%2_application.cin goto error1

set CIYAM_MODEL=%1

ciyam_client -quiet -no_prompt < create_demo_%2_application.cin
goto end

:error1
echo File 'create_demo_%2_application.cin' does not exist.
goto end

:usage
echo Usage: create_demo_application [model name] [demo name]

:end
endlocal


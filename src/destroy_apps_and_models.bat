@echo off
REM Copyright (c) 2017-2023 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not '%1' == 'okay' goto usage

ciyam_client -quiet -no_prompt -vars=ERROR_FILE=%2 < destroy_apps_and_models.cin
goto end

:usage
echo Usage: destroy_apps_and_models [okay] [[error file]]

:end

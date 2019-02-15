@echo off
REM Copyright (c) 2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<merge_tags.cin %1 %2 >~merge_tags
ciyam_client -echo -quiet -no_prompt -no_stderr < ~merge_tags

del ~merge_tags
goto end

:usage
echo Usage: merge_tags [tag name(s) and/or prefix] [[tag name]]

:end

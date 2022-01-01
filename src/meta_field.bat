@echo off
REM Copyright (c) 2012-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

findstr /R "^field_add.*Specification_Type.*%1" Meta.cin
goto end

:usage
echo Usage: meta_field [name]

:end

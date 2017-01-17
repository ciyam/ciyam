@echo off
REM Copyright (c) 2017 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not '%1' == 'okay' goto usage

REM Retreive the lines from the log file which were package installations (this is being
REM done in order to make sure the packages will be reinstalled in the exact same order).
findstr /R " 135410$" Meta.log >package_lines.lst

if not exist package_lines.lst goto error1

ciyam_client -quiet -no_prompt < destroy_meta.cin

xrep @reinstall_packages.cin.xrep package_lines=@package_lines.lst >reinstall_packages.cin

ciyam_client -quiet -no_prompt < reinstall_packages.cin

del reinstall_packages.cin
del package_lines.lst
goto end

:error1
echo Error: File 'package_lines.lst' does not exist.
goto end

:usage
echo Usage: reinstall_packages [okay]

:end


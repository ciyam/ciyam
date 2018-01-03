@echo off
REM Copyright (c) 2017-2018 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not '%1' == 'okay' goto usage

ciyam_client -quiet -no_prompt < get_acyclic_package_types.cin

if not exist packages.acyclic.lst goto error1

ciyam_client -quiet -no_prompt < recreate_meta.cin

xrep @reinstall_packages.cin.xrep packages=@packages.acyclic.lst >reinstall_packages.cin

ciyam_client -quiet -no_prompt < reinstall_packages.cin

del reinstall_packages.cin
del packages.acyclic.lst
goto end

:error1
echo Error: File 'packages.acyclic.lst' does not exist.
goto end

:usage
echo Usage: reinstall_packages [okay]

:end


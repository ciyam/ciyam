@echo off
REM Copyright (c) 2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

echo ^<^<install_packages_list.cin %1 >~install_packages_list
ciyam_client -echo -quiet -no_prompt -no_stderr < ~install_packages_list
del ~install_packages_list

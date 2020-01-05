@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%CPPENV%' == 'bcb' goto bcb
if '%CPPENV%' == 'BCB' goto bcb
if '%CPPENV%' == 'mvc' goto mvc
if '%CPPENV%' == 'MVC' goto mvc
goto error1

:bcb
make.exe -fmakefile.bcb -DPRECOMPILE_H %CPPDBG% %* | findstr /V "MAKE Version"
goto end

:mvc
if not exist gendeps.exe call cl.bat gendeps.cpp
nmake.exe /nologo /f makefile.mvc NO_INC_FILES=1 touch_incs
nmake.exe /nologo /f makefile.mvc %CPPDBG% %*
goto end

:error1
echo Error: Unexpected CPPENV value: %CPPENV%
goto end

:end


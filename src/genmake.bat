@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not exist xrep.exe goto error2
if not exist xvars.exe goto error3

xrep @makefile.sio.xrep modules=@modules.lst >makefile.sio

if '%CPPENV%' == 'bcb' goto bcb
if '%CPPENV%' == 'BCB' goto bcb
if '%CPPENV%' == 'mvc' goto mvc
if '%CPPENV%' == 'MVC' goto mvc
goto error1

:bcb
xvars >makefile.vars.xrep
if errorlevel 1 goto end
xrep @makefile.bcb.xrep >makefile.bcb.new
call update makefile.bcb makefile.bcb.new
goto end

:mvc
xvars >makefile.vars.xrep
if errorlevel 1 goto end
xrep @makefile.mvc.xrep >makefile.mvc.new
call update makefile.mvc makefile.mvc.new
goto end

:error1
echo Error: Unexpected CPPENV value: %CPPENV%
goto end

:error2
echo Error: File 'xrep.exe' does not exist.
goto end

:error3
echo Error: File 'xvars.exe' does not exist.
goto end

:end


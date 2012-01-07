@echo off

if not exist xrep.exe goto error2
if not exist xvars.exe goto error3

xrep @makefile.sio.xrep modules=@modules.lst >makefile.sio

if '%CPPENV%' == 'bcb6' goto bcb
if '%CPPENV%' == 'BCB6' goto bcb
if '%CPPENV%' == 'vc71' goto mvc
if '%CPPENV%' == 'VC71' goto mvc
if '%CPPENV%' == 'vc80' goto mvc
if '%CPPENV%' == 'VC80' goto mvc
goto error1

:mvc
xvars >makefile.vars.xrep
if errorlevel 1 goto end
xrep @makefile.mvc.xrep >makefile.mvc.new
call update makefile.mvc makefile.mvc.new
goto end

:bcb
xvars >makefile.vars.xrep
if errorlevel 1 goto end
xrep @makefile.bcb.xrep >makefile.bcb.new
call update makefile.bcb makefile.bcb.new
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


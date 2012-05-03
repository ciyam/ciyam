@echo off

if '%CPPENV%' == 'bcb' goto bcb
if '%CPPENV%' == 'BCB' goto bcb
if '%CPPENV%' == 'mvc' goto mvc
if '%CPPENV%' == 'MVC' goto mvc
goto error1

:bcb
make.exe -fmakefile.bcb %CPPDBG% %* | findstr /V "MAKE Version"
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


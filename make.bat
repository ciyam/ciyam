@echo off

if '%CPPENV%' == 'bcb6' goto bcb
if '%CPPENV%' == 'BCB6' goto bcb

:mvc
if not exist gendeps.exe call cl.bat gendeps.cpp
nmake.exe /nologo /f makefile.mvc NO_INC_FILES=1 touch_incs
nmake.exe /nologo /f makefile.mvc %*
goto end

:bcb
make.exe -fmakefile.bcb %* | findstr /V "MAKE Version"

:end


@echo off

if '%1' == '' goto usage

xrep @gendeps.xrep "all_files=%*" >~gendeps.bat
call ~gendeps.bat
del ~gendeps.bat
goto end

:usage
echo usage: gendeps [file_1.cpp [file_2.cpp [...]]]

:end


@echo off

setlocal

set install_path=%PROGRAMFILES%\ciyam

if exist "%install_path%" goto check_git
echo Creating %install_path%...
mkdir "%install_path%"

:check_git
if exist ".git\config" goto install_copy
echo "ERROR: this isnt a ciyam git"
exit 1

:install_copy
xcopy src/* "%install_path%" /E /Q /H /R /K /X /Y

exit 0

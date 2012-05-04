@echo off

if not '%1' == '' goto show

echo encrypt> ~password.cin
goto run_script

:show
echo encrypt %1> ~password.cin

:run_script
ciyam_client -quiet -no_prompt <~password.cin
del ~password.cin

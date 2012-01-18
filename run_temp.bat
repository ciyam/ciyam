@echo off

if '%1' == '' exit

call %1
del %1
exit


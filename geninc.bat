@echo off

for %%n in ( %* ) do call genincx.bat check %%n
echo.
for %%n in ( %* ) do call genincx.bat define %%n

:end

@echo off

if not exist packages.lst echo Standard>packages.lst
call construct @packages.lst ciyam_class.cpp.xrep

if exist .git\hooks\pre-commit del .git\hooks\pre-commit
if exist pre-commit copy pre-commit .git\hooks >nul


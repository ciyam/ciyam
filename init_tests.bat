@echo off

if not exist packages.lst echo Standard>packages.lst
call construct @packages.lst ciyam_class.cpp.xrep

if not exist .git/hooks/pre-commit copy pre-commit .git/hooks >nul


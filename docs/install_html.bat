@echo off
REM Copyright (c) 2017-2023 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

if not exist "%1\" goto error

copy document.css "%1"  >nul

copy name_small.png "%1"  >nul
copy logo-gryscl-128.png "%1"  >nul
copy background_texture.png "%1"  >nul

if exist "%1\fonts" goto skip_fonts
mkdir "%1\fonts"
..\src\unbundle -qq ..\src\fonts -d "%1\fonts"

:skip_fonts
copy *.html "%1"  >nul

goto end

:error
echo Error: Directory '%1' does not exist.
goto end

:usage
echo Usage: install_html [directory]

:end


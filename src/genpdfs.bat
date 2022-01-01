@echo off
REM Copyright (c) 2012-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage

set full_name=%1_%2

if exist %full_name%.pdf.bat (
 call %full_name%.pdf.bat
 del %full_name%.pdf.bat
)

if exist %full_name%.list.pdf.bat (
 call %full_name%.list.pdf.bat
 del %full_name%.list.pdf.bat
)

for %%n in (1 2 3 4 5 6 7 8 9) do (
 if exist %full_name%.list%%n.pdf.bat (
  call %full_name%.list%%n.pdf.bat
  del %full_name%.list%%n.pdf.bat
 )
)

if exist %full_name%.glist.pdf.bat (
 call %full_name%.glist.pdf.bat
 del %full_name%.glist.pdf.bat
)

for %%n in (1 2 3 4 5 6 7 8 9) do (
 if exist %full_name%.glist%%n.pdf.bat (
  call %full_name%.glist%%n.pdf.bat
  del %full_name%.glist%%n.pdf.bat
 )
)

if exist %full_name%.ulist.pdf.bat (
 call %full_name%.ulist.pdf.bat
 del %full_name%.ulist.pdf.bat
)

for %%n in (1 2 3 4 5 6 7 8 9) do (
 if exist %full_name%.ulist%%n.pdf.bat (
  call %full_name%.ulist%%n.pdf.bat
  del %full_name%.ulist%%n.pdf.bat
 )
)

if exist %full_name%.nglist.pdf.bat (
 call %full_name%.nglist.pdf.bat
 del %full_name%.nglist.pdf.bat
)

for %%n in (1 2 3 4 5 6 7 8 9) do (
 if exist %full_name%.nglist%%n.pdf.bat (
  call %full_name%.nglist%%n.pdf.bat
  del %full_name%.nglist%%n.pdf.bat
 )
)

if exist %full_name%.nulist.pdf.bat (
 call %full_name%.nulist.pdf.bat
 del %full_name%.nulist.pdf.bat
)

for %%n in (1 2 3 4 5 6 7 8 9) do (
 if exist %full_name%.nulist%%n.pdf.bat (
  call %full_name%.nulist%%n.pdf.bat
  del %full_name%.nulist%%n.pdf.bat
 )
)

goto end

:usage
echo Usage: genpdfs [module name] [class name]

:end
endlocal


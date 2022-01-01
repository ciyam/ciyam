@echo off
REM Copyright (c) 2012-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

if exist %1_%2.cmh del %1_%2.cmh
if exist %1_%2.cms del %1_%2.cms
if exist %1_%2.cpp del %1_%2.cpp
if exist %1_%2.dep del %1_%2.dep
if exist %1_%2.h del %1_%2.h
if exist %1_%2.obj del %1_%2.obj
if exist %1_%2.csv del %1_%2.csv
if exist %1_%2.log del %1_%2.log

if exist %1_%2.pdf.sio del %1_%2.pdf.sio
if exist %1_%2.pdf.vars.xrep del %1_%2.pdf.vars.xrep

if exist %1_%2.list.pdf.sio del %1_%2.list.pdf.sio
if exist %1_%2.list.pdf.vars.xrep del %1_%2.list.pdf.vars.xrep
for %%n in (1 2 3 4 5 6 7 8 9) do call delifexist %1_%2.list%%n.pdf.sio %1_%2.list%%n.pdf.vars.xrep

if exist %1_%2.glist.pdf.sio del %1_%2.glist.pdf.sio
if exist %1_%2.glist.pdf.vars.xrep del %1_%2.glist.pdf.vars.xrep
for %%n in (1 2 3 4 5 6 7 8 9) do call delifexist %1_%2.glist%%n.pdf.sio %1_%2.glist%%n.pdf.vars.xrep

if exist %1_%2.ulist.pdf.sio del %1_%2.ulist.pdf.sio
if exist %1_%2.ulist.pdf.vars.xrep del %1_%2.ulist.pdf.vars.xrep
for %%n in (1 2 3 4 5 6 7 8 9) do call delifexist %1_%2.ulist%%n.pdf.sio %1_%2.ulist%%n.pdf.vars.xrep

if exist %1_%2.nglist.pdf.sio del %1_%2.nglist.pdf.sio
if exist %1_%2.nglist.pdf.vars.xrep del %1_%2.nglist.pdf.vars.xrep
for %%n in (1 2 3 4 5 6 7 8 9) do call delifexist %1_%2.nglist%%n.pdf.sio %1_%2.nglist%%n.pdf.vars.xrep

if exist %1_%2.nulist.pdf.sio del %1_%2.nulist.pdf.sio
if exist %1_%2.nulist.pdf.vars.xrep del %1_%2.nulist.pdf.vars.xrep
for %%n in (1 2 3 4 5 6 7 8 9) do call delifexist %1_%2.nulist%%n.pdf.sio %1_%2.nulist%%n.pdf.vars.xrep

if exist %1_%2.vars.xrep.sav del %1_%2.vars.xrep.sav

if not exist %1_%2.extra.lst goto skip
xrep @del_files.bat.xrep files=@%1_%2.extra.lst >~remove_class.bat
call ~remove_class.bat
del ~remove_class.bat
del %1_%2.extra.lst

:skip
goto end

:usage
echo Usage: remove_class [module_name] [class_name]

:end


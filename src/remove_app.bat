@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

if exist %1.log del %1.log
if exist %1.sql del %1.sql

if exist %1.txs.log del %1.txs.log
if exist %1.undo.sql del %1.undo.sql

if exist %1.tlg del %1.tlg
if exist %1.dat del %1.dat
if exist %1.dat.lck del %1.dat.lck
if exist %1.hdr del %1.hdr
if exist %1.hdr.lck del %1.hdr.lck
if exist %1.idx del %1.idx
if exist %1.idx.lck del %1.idx.lck

if exist %1.algos.lst del %1.algos.lst

if exist %1.dead_keys.lst del %1.dead_keys.lst

if exist %1.modules.lst del %1.modules.lst
if exist %1.app.vars.xrep del %1.app.vars.xrep

if exist %1.upgrade.cin del %1.upgrade.cin
if exist %1.upgrade.sql del %1.upgrade.sql
if exist %1.obsolete.sql del %1.obsolete.sql

if exist %1.generate.bat del %1.generate.bat
if exist %1.generate.log del %1.generate.log
if exist %1.generate.*.cin del %1.generate.*.cin

goto end

:usage
echo Usage: remove_app [app name]

:end


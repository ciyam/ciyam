@echo off
REM Copyright (c) 2013-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

dir /b *.txt >~genhtml.lst

..\src\xrep @index.html.xrep docs=@~genhtml.lst>index.html
..\src\xrep @gendocs.xrep prefix=..\\src\\ docs=@~genhtml.lst>~genhtml.bat

call ~genhtml.bat

del ~genhtml.bat
del ~genhtml.lst


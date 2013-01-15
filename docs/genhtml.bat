@echo off
dir /b *.txt >~genhtml.lst

..\src\xrep @index.html.xrep docs=@~genhtml.lst>index.html
..\src\xrep @gendocs.xrep prefix=..\\src\\ docs=@~genhtml.lst>~genhtml.bat

call ~genhtml.bat

del ~genhtml.bat
del ~genhtml.lst

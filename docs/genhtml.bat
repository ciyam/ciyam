@echo off
dir /b *.txt >~genhtml.lst

xrep @index.html.xrep docs=@~genhtml.lst>index.html
xrep @gendocs.bat.xrep docs=@~genhtml.lst>~genhtml.bat

call ~genhtml.bat

del ~genhtml.bat
del ~genhtml.lst

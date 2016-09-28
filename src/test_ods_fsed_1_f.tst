
/> cd
/

/> cd abc

/abc> folder_add aaa

/abc> folder_add zzz

/abc> folder_add nnn

/abc> branch objects
aaa/
nnn/
zzz/

/abc> cd /

/> branch folders
abc
abc/aaa
abc/nnn
abc/zzz
def
ghi
xyz

/> branch objects
abc/
abc/aaa/
abc/nnn/
abc/zzz/
def/
ghi/
xyz/

/> dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 14

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /abc, link = -1
Item #1, data = /abc/aaa, link = -1
Item #2, data = /abc/nnn, link = -1
Item #3, data = /abc/zzz, link = -1
Item #4, data = /def, link = -1
Item #5, data = /ghi, link = -1
Item #6, data = /xyz, link = -1
Item #7, data = :/abc, link = -1
Item #8, data = :/def, link = -1
Item #9, data = :/ghi, link = -1
Item #10, data = :/xyz, link = -1
Item #11, data = :abc/aaa, link = -1
Item #12, data = :abc/nnn, link = -1
Item #13, data = :abc/zzz, link = -1

/> exit

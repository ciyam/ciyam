cd
/
folder_add abc
folder_add xyz
branch objects
abc/
xyz/
folder_add ghi
folder_add def
folder_add "abc\ndef"
error: invalid folder name 'abc
def'
folder_add "abc\tdef"
error: invalid folder name 'abc	def'
branch objects
abc/
def/
ghi/
xyz/
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 8

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /abc, link = -1
Item #1, data = /def, link = -1
Item #2, data = /ghi, link = -1
Item #3, data = /xyz, link = -1
Item #4, data = :/abc, link = -1
Item #5, data = :/def, link = -1
Item #6, data = :/ghi, link = -1
Item #7, data = :/xyz, link = -1
exit

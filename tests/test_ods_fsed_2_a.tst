file_add xxx *
files
xxx (0 B)
file_replace xxx test.jpg
files
xxx (6.7 kB)
file_move xxx yyy
files
yyy (6.7 kB)
file_move yyy xxx
files
xxx (6.7 kB)
file_add yyy test1.jpg
file_add zzz test2.jpg
branch objects
xxx (6.7 kB)
yyy (5.7 kB)
zzz (3.1 kB)
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 3

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = |/xxx, link = -1
Item #1, data = |/yyy, link = -1
Item #2, data = |/zzz, link = -1
trans
begin transaction (level = 1)
file_add aaa modules.lst.default
list
aaa (5 B)
xxx (6.7 kB)
yyy (5.7 kB)
zzz (3.1 kB)
file_get aaa -c
Meta

rollback
transaction rolled back (level = 1)
exit

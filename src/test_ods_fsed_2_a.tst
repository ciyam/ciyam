
/> file_add xxx test.jpg

/> file_add yyy user1.jpg

/> file_add zzz user2.jpg

/> branch objects
xxx (6.7 kB)
yyy (5.7 kB)
zzz (3.1 kB)

/> dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 3

Dumping level #0
[Node 2] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = |/xxx, link = -1
Item #1, data = |/yyy, link = -1
Item #2, data = |/zzz, link = -1

/> exit

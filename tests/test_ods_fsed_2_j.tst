file_remove xxx
branch objects
111/
111/222/
111/222/333/
111/222/333/hhh* (5.7 kB)
111/222/eee* (5.7 kB)
111/bbb* (5.7 kB)
yyy (5.7 kB)
zzz (3.1 kB)
file_remove /yyy
branch objects
111/
111/222/
111/222/333/
zzz (3.1 kB)
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 7

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /111, link = -1
Item #1, data = /111/222, link = -1
Item #2, data = /111/222/333, link = -1
Item #3, data = :/111, link = -1
Item #4, data = :111/222, link = -1
Item #5, data = :111:222/333, link = -1
Item #6, data = |/zzz, link = -1
cd 111
file_remove ../zzz
cd ..
branch objects
111/
111/222/
111/222/333/
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 6

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /111, link = -1
Item #1, data = /111/222, link = -1
Item #2, data = /111/222/333, link = -1
Item #3, data = :/111, link = -1
Item #4, data = :111/222, link = -1
Item #5, data = :111:222/333, link = -1
folder_add 111/222/333/.444
cd 111/222/333/.444
file_add xxx modules.lst.default
cd /
branch -f objects
/111/
/111/222/
/111/222/333/
/111/222/333/.444/
/111/222/333/.444/xxx (5) rw-r--r--
cd 111/222
folder_remove -r 333
cd /
branch -f objects
/111/
/111/222/
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 4

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /111, link = -1
Item #1, data = /111/222, link = -1
Item #2, data = :/111, link = -1
Item #3, data = :111/222, link = -1
exit

folder_add 111
cd 111
folder_add 222
cd 222
folder_add 333
cd /
file_move aaa 111
file_move bbb 111
file_move ccc 111
file_move ddd 111/222
file_move eee 111/222
file_move fff 111/222
file_move ggg 111/222/333
file_move hhh 111/222/333
file_move iii 111/222/333
branch objects
111/
111/222/
111/222/333/
111/222/333/ggg* (3.1 kB)
111/222/333/hhh* (5.7 kB)
111/222/333/iii* (6.7 kB)
111/222/ddd* (3.1 kB)
111/222/eee* (5.7 kB)
111/222/fff* (6.7 kB)
111/aaa* (3.1 kB)
111/bbb* (5.7 kB)
111/ccc* (6.7 kB)
xxx (6.7 kB)
yyy (5.7 kB)
zzz (3.1 kB)
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 30

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /111, link = -1
Item #1, data = /111/222, link = -1
Item #2, data = /111/222/333, link = -1
Item #3, data = 2!/xxx, link = -1
Item #4, data = 2|111/ccc, link = -1
Item #5, data = 2|111|222/fff, link = -1
Item #6, data = 2|111|222|333/iii, link = -1
Item #7, data = 3!/yyy, link = -1
Item #8, data = 3|111/bbb, link = -1
Item #9, data = 3|111|222/eee, link = -1
Item #10, data = 3|111|222|333/hhh, link = -1
Item #11, data = 4!/zzz, link = -1
Item #12, data = 4|111/aaa, link = -1
Item #13, data = 4|111|222/ddd, link = -1
Item #14, data = 4|111|222|333/ggg, link = -1
Item #15, data = :/111, link = -1
Item #16, data = :111/222, link = -1
Item #17, data = :111:222/333, link = -1
Item #18, data = |/xxx, link = -1
Item #19, data = |/yyy, link = -1
Item #20, data = |/zzz, link = -1
Item #21, data = |111/aaa, link = -1
Item #22, data = |111/bbb, link = -1
Item #23, data = |111/ccc, link = -1
Item #24, data = |111|222/ddd, link = -1
Item #25, data = |111|222/eee, link = -1
Item #26, data = |111|222/fff, link = -1
Item #27, data = |111|222|333/ggg, link = -1
Item #28, data = |111|222|333/hhh, link = -1
Item #29, data = |111|222|333/iii, link = -1
file_move 111/aaa .
file_move 111/ccc .
file_move 111/222/ddd .
file_move 111/222/fff .
file_move 111/222/333/ggg .
file_move 111/222/333/iii .
branch objects
111/
111/222/
111/222/333/
111/222/333/hhh* (5.7 kB)
111/222/eee* (5.7 kB)
111/bbb* (5.7 kB)
aaa* (3.1 kB)
ccc* (6.7 kB)
ddd* (3.1 kB)
fff* (6.7 kB)
ggg* (3.1 kB)
iii* (6.7 kB)
xxx (6.7 kB)
yyy (5.7 kB)
zzz (3.1 kB)
file_move xxx 111
file_move yyy 111/222
file_move zzz 111/222/333
branch objects
111/
111/222/
111/222/333/
111/222/333/hhh* (5.7 kB)
111/222/333/zzz (3.1 kB)
111/222/eee* (5.7 kB)
111/222/yyy (5.7 kB)
111/bbb* (5.7 kB)
111/xxx (6.7 kB)
aaa* (3.1 kB)
ccc* (6.7 kB)
ddd* (3.1 kB)
fff* (6.7 kB)
ggg* (3.1 kB)
iii* (6.7 kB)
file_move aaa 111
file_move ccc 111
file_move ddd 111/222
file_move fff 111/222
file_move ggg 111/222/333
file_move iii 111/222/333
branch objects
111/
111/222/
111/222/333/
111/222/333/ggg* (3.1 kB)
111/222/333/hhh* (5.7 kB)
111/222/333/iii* (6.7 kB)
111/222/333/zzz (3.1 kB)
111/222/ddd* (3.1 kB)
111/222/eee* (5.7 kB)
111/222/fff* (6.7 kB)
111/222/yyy (5.7 kB)
111/aaa* (3.1 kB)
111/bbb* (5.7 kB)
111/ccc* (6.7 kB)
111/xxx (6.7 kB)
file_move 111/xxx .
file_move 111/222/yyy .
file_move 111/222/333/zzz .
branch objects
111/
111/222/
111/222/333/
111/222/333/ggg* (3.1 kB)
111/222/333/hhh* (5.7 kB)
111/222/333/iii* (6.7 kB)
111/222/ddd* (3.1 kB)
111/222/eee* (5.7 kB)
111/222/fff* (6.7 kB)
111/aaa* (3.1 kB)
111/bbb* (5.7 kB)
111/ccc* (6.7 kB)
xxx (6.7 kB)
yyy (5.7 kB)
zzz (3.1 kB)
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 30

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /111, link = -1
Item #1, data = /111/222, link = -1
Item #2, data = /111/222/333, link = -1
Item #3, data = 2!/xxx, link = -1
Item #4, data = 2|111/ccc, link = -1
Item #5, data = 2|111|222/fff, link = -1
Item #6, data = 2|111|222|333/iii, link = -1
Item #7, data = 3!/yyy, link = -1
Item #8, data = 3|111/bbb, link = -1
Item #9, data = 3|111|222/eee, link = -1
Item #10, data = 3|111|222|333/hhh, link = -1
Item #11, data = 4!/zzz, link = -1
Item #12, data = 4|111/aaa, link = -1
Item #13, data = 4|111|222/ddd, link = -1
Item #14, data = 4|111|222|333/ggg, link = -1
Item #15, data = :/111, link = -1
Item #16, data = :111/222, link = -1
Item #17, data = :111:222/333, link = -1
Item #18, data = |/xxx, link = -1
Item #19, data = |/yyy, link = -1
Item #20, data = |/zzz, link = -1
Item #21, data = |111/aaa, link = -1
Item #22, data = |111/bbb, link = -1
Item #23, data = |111/ccc, link = -1
Item #24, data = |111|222/ddd, link = -1
Item #25, data = |111|222/eee, link = -1
Item #26, data = |111|222/fff, link = -1
Item #27, data = |111|222|333/ggg, link = -1
Item #28, data = |111|222|333/hhh, link = -1
Item #29, data = |111|222|333/iii, link = -1
exit

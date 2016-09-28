
/> cd 111/222/333

/111/222/333> file_remove ggg

/111/222/333> cd /

/> file_remove 111/222/ddd

/> file_remove 111/aaa

/> branch objects
111/
111/222/
111/222/333/
111/222/333/hhh* (5.6 KiB)
111/222/333/iii* (6.6 KiB)
111/222/eee* (5.6 KiB)
111/222/fff* (6.6 KiB)
111/bbb* (5.6 KiB)
111/ccc* (6.6 KiB)
xxx (6.6 KiB)
yyy (5.6 KiB)
zzz (3.1 KiB)

/> dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 21

Dumping level #0
[Node 2] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /111, link = -1
Item #1, data = /111/222, link = -1
Item #2, data = /111/222/333, link = -1
Item #3, data = 1|111/ccc, link = -1
Item #4, data = 1|111|222/fff, link = -1
Item #5, data = 1|111|222|333/iii, link = -1
Item #6, data = 3|111/bbb, link = -1
Item #7, data = 3|111|222/eee, link = -1
Item #8, data = 3|111|222|333/hhh, link = -1
Item #9, data = :/111, link = -1
Item #10, data = :111/222, link = -1
Item #11, data = :111:222/333, link = -1
Item #12, data = |/xxx, link = -1
Item #13, data = |/yyy, link = -1
Item #14, data = |/zzz, link = -1
Item #15, data = |111/bbb, link = -1
Item #16, data = |111/ccc, link = -1
Item #17, data = |111|222/eee, link = -1
Item #18, data = |111|222/fff, link = -1
Item #19, data = |111|222|333/hhh, link = -1
Item #20, data = |111|222|333/iii, link = -1

/> exit

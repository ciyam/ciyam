cd
/
cd abc/aaa/nnn
folder_move zzz /
cd /
branch folders
abc
abc/aaa
abc/aaa/nnn
abc/def
abc/def/test_1
abc/def/test_2
abc/def/test_3
abc/xyz
abc/xyz/test_A
abc/xyz/test_B
abc/xyz/test_C
ghi
ghi/111
ghi/555
ghi/999
zzz
cd abc/aaa
folder_remove nnn
cd /
branch folders
abc
abc/aaa
abc/def
abc/def/test_1
abc/def/test_2
abc/def/test_3
abc/xyz
abc/xyz/test_A
abc/xyz/test_B
abc/xyz/test_C
ghi
ghi/111
ghi/555
ghi/999
zzz
branch objects
abc/
abc/aaa/
abc/def/
abc/def/test_1/
abc/def/test_2/
abc/def/test_3/
abc/xyz/
abc/xyz/test_A/
abc/xyz/test_B/
abc/xyz/test_C/
ghi/
ghi/111/
ghi/555/
ghi/999/
zzz/
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 30

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /abc, link = -1
Item #1, data = /abc/aaa, link = -1
Item #2, data = /abc/def, link = -1
Item #3, data = /abc/def/test_1, link = -1
Item #4, data = /abc/def/test_2, link = -1
Item #5, data = /abc/def/test_3, link = -1
Item #6, data = /abc/xyz, link = -1
Item #7, data = /abc/xyz/test_A, link = -1
Item #8, data = /abc/xyz/test_B, link = -1
Item #9, data = /abc/xyz/test_C, link = -1
Item #10, data = /ghi, link = -1
Item #11, data = /ghi/111, link = -1
Item #12, data = /ghi/555, link = -1
Item #13, data = /ghi/999, link = -1
Item #14, data = /zzz, link = -1
Item #15, data = :/abc, link = -1
Item #16, data = :/ghi, link = -1
Item #17, data = :/zzz, link = -1
Item #18, data = :abc/aaa, link = -1
Item #19, data = :abc/def, link = -1
Item #20, data = :abc/xyz, link = -1
Item #21, data = :abc:def/test_1, link = -1
Item #22, data = :abc:def/test_2, link = -1
Item #23, data = :abc:def/test_3, link = -1
Item #24, data = :abc:xyz/test_A, link = -1
Item #25, data = :abc:xyz/test_B, link = -1
Item #26, data = :abc:xyz/test_C, link = -1
Item #27, data = :ghi/111, link = -1
Item #28, data = :ghi/555, link = -1
Item #29, data = :ghi/999, link = -1
exit

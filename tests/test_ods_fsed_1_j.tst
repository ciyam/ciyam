cd
/
folder_move ghi def/test_1
branch folders
abc
abc/aaa
abc/nnn
abc/zzz
def
def/test_1
def/test_1/ghi
def/test_1/ghi/111
def/test_1/ghi/555
def/test_1/ghi/999
def/test_1/test_2
def/test_1/test_2/test_3
xyz
xyz/test_A
xyz/test_B
xyz/test_C
folder_move xyz abc
branch folders
abc
abc/aaa
abc/nnn
abc/xyz
abc/xyz/test_A
abc/xyz/test_B
abc/xyz/test_C
abc/zzz
def
def/test_1
def/test_1/ghi
def/test_1/ghi/111
def/test_1/ghi/555
def/test_1/ghi/999
def/test_1/test_2
def/test_1/test_2/test_3
folder_move def abc
branch folders
abc
abc/aaa
abc/def
abc/def/test_1
abc/def/test_1/ghi
abc/def/test_1/ghi/111
abc/def/test_1/ghi/555
abc/def/test_1/ghi/999
abc/def/test_1/test_2
abc/def/test_1/test_2/test_3
abc/nnn
abc/xyz
abc/xyz/test_A
abc/xyz/test_B
abc/xyz/test_C
abc/zzz
branch objects
abc/
abc/aaa/
abc/def/
abc/def/test_1/
abc/def/test_1/ghi/
abc/def/test_1/ghi/111/
abc/def/test_1/ghi/555/
abc/def/test_1/ghi/999/
abc/def/test_1/test_2/
abc/def/test_1/test_2/test_3/
abc/nnn/
abc/xyz/
abc/xyz/test_A/
abc/xyz/test_B/
abc/xyz/test_C/
abc/zzz/
branch folders /
abc
abc/aaa
abc/def
abc/def/test_1
abc/def/test_1/ghi
abc/def/test_1/ghi/111
abc/def/test_1/ghi/555
abc/def/test_1/ghi/999
abc/def/test_1/test_2
abc/def/test_1/test_2/test_3
abc/nnn
abc/xyz
abc/xyz/test_A
abc/xyz/test_B
abc/xyz/test_C
abc/zzz
branch objects /
abc/
abc/aaa/
abc/def/
abc/def/test_1/
abc/def/test_1/ghi/
abc/def/test_1/ghi/111/
abc/def/test_1/ghi/555/
abc/def/test_1/ghi/999/
abc/def/test_1/test_2/
abc/def/test_1/test_2/test_3/
abc/nnn/
abc/xyz/
abc/xyz/test_A/
abc/xyz/test_B/
abc/xyz/test_C/
abc/zzz/
branch folders abc
aaa
def
def/test_1
def/test_1/ghi
def/test_1/ghi/111
def/test_1/ghi/555
def/test_1/ghi/999
def/test_1/test_2
def/test_1/test_2/test_3
nnn
xyz
xyz/test_A
xyz/test_B
xyz/test_C
zzz
branch folders /abc
aaa
def
def/test_1
def/test_1/ghi
def/test_1/ghi/111
def/test_1/ghi/555
def/test_1/ghi/999
def/test_1/test_2
def/test_1/test_2/test_3
nnn
xyz
xyz/test_A
xyz/test_B
xyz/test_C
zzz
branch objects /abc
aaa/
def/
def/test_1/
def/test_1/ghi/
def/test_1/ghi/111/
def/test_1/ghi/555/
def/test_1/ghi/999/
def/test_1/test_2/
def/test_1/test_2/test_3/
nnn/
xyz/
xyz/test_A/
xyz/test_B/
xyz/test_C/
zzz/
branch folders abc/def
test_1
test_1/ghi
test_1/ghi/111
test_1/ghi/555
test_1/ghi/999
test_1/test_2
test_1/test_2/test_3
branch folders /abc/def
test_1
test_1/ghi
test_1/ghi/111
test_1/ghi/555
test_1/ghi/999
test_1/test_2
test_1/test_2/test_3
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 32

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /abc, link = -1
Item #1, data = /abc/aaa, link = -1
Item #2, data = /abc/def, link = -1
Item #3, data = /abc/def/test_1, link = -1
Item #4, data = /abc/def/test_1/ghi, link = -1
Item #5, data = /abc/def/test_1/ghi/111, link = -1
Item #6, data = /abc/def/test_1/ghi/555, link = -1
Item #7, data = /abc/def/test_1/ghi/999, link = -1
Item #8, data = /abc/def/test_1/test_2, link = -1
Item #9, data = /abc/def/test_1/test_2/test_3, link = -1
Item #10, data = /abc/nnn, link = -1
Item #11, data = /abc/xyz, link = -1
Item #12, data = /abc/xyz/test_A, link = -1
Item #13, data = /abc/xyz/test_B, link = -1
Item #14, data = /abc/xyz/test_C, link = -1
Item #15, data = /abc/zzz, link = -1
Item #16, data = :/abc, link = -1
Item #17, data = :abc/aaa, link = -1
Item #18, data = :abc/def, link = -1
Item #19, data = :abc/nnn, link = -1
Item #20, data = :abc/xyz, link = -1
Item #21, data = :abc/zzz, link = -1
Item #22, data = :abc:def/test_1, link = -1
Item #23, data = :abc:def:test_1/ghi, link = -1
Item #24, data = :abc:def:test_1/test_2, link = -1
Item #25, data = :abc:def:test_1:ghi/111, link = -1
Item #26, data = :abc:def:test_1:ghi/555, link = -1
Item #27, data = :abc:def:test_1:ghi/999, link = -1
Item #28, data = :abc:def:test_1:test_2/test_3, link = -1
Item #29, data = :abc:xyz/test_A, link = -1
Item #30, data = :abc:xyz/test_B, link = -1
Item #31, data = :abc:xyz/test_C, link = -1
exit

cd
/
cd abc
folder_remove def
*** cannot remove 'def' as it has one or more child folders ***
folder_remove -r def
cd /
branch folders
abc
abc/aaa
abc/xyz
abc/xyz/test_A
abc/xyz/test_B
abc/xyz/test_C
ghi
ghi/111
ghi/555
ghi/999
zzz
folder_remove abc
*** cannot remove 'abc' as it has one or more child folders ***
folder_remove -r abc
branch folders
ghi
ghi/111
ghi/555
ghi/999
zzz
folder_remove ghi
*** cannot remove 'ghi' as it has one or more child folders ***
folder_remove -r ghi
branch folders
zzz
branch objects
zzz/
dump
Total index levels = 0
Total number of nodes = 1
Total number of items = 2

Dumping level #0
[Node 1] flags = 3, dge_link = -1
         lft_link = -1, rgt_link = -1
Item #0, data = /zzz, link = -1
Item #1, data = :/zzz, link = -1
exit

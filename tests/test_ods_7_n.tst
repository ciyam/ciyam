** File Info
Version: 1.0
Num Logs = 1
Num Trans = 0
Num Writers = 0
Total Entries = 10
Tranlog Offset = 1092
Transaction Id = 5
Index Free List = 7
Total Size of Data = 388
Data Transformation Id = 4
Index Transformation Id = 23

** Entry Info for: all
num: 0000000000000000          pos: 000000000000008a          len: 000000000000004c
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
000000000000008a  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
000000000000009a  ff ff ff ff 06 00 00 00 00 00 00 00 01 00 00 00  ................
00000000000000aa  00 00 00 00 02 00 00 00 00 00 00 00 03 00 00 00  ................
00000000000000ba  00 00 00 00 04 00 00 00 00 00 00 00 05 00 00 00  ................
00000000000000ca  00 00 00 00 06 00 00 00 00 00 00 00              ............

num: 0000000000000001          pos: 0000000000000167          len: 000000000000001d
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
0000000000000167  05 00 00 00 00 00 00 00 78 78 78 23 30 ff ff ff  ........xxx#0...
0000000000000177  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

num: 0000000000000002          pos: 000000000000014a          len: 000000000000001d
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
000000000000014a  05 00 00 00 00 00 00 00 78 78 78 23 31 ff ff ff  ........xxx#1...
000000000000015a  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

num: 0000000000000003          pos: 000000000000012d          len: 000000000000001d
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
000000000000012d  05 00 00 00 00 00 00 00 78 78 78 23 32 ff ff ff  ........xxx#2...
000000000000013d  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

num: 0000000000000004          pos: 0000000000000110          len: 000000000000001d
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
0000000000000110  05 00 00 00 00 00 00 00 79 79 79 23 30 ff ff ff  ........yyy#0...
0000000000000120  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

num: 0000000000000005          pos: 00000000000000f3          len: 000000000000001d
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
00000000000000f3  05 00 00 00 00 00 00 00 79 79 79 23 31 ff ff ff  ........yyy#1...
0000000000000103  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

num: 0000000000000006          pos: 00000000000000d6          len: 000000000000001d
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
00000000000000d6  05 00 00 00 00 00 00 00 79 79 79 23 32 ff ff ff  ........yyy#2...
00000000000000e6  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

num: 0000000000000007          pos: 0000000000000009          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: 0000000000000008

num: 0000000000000008          pos: 000000000000000a          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: 0000000000000009

num: 0000000000000009          pos: 0000000000000000          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: <at end>

** Freelist Info
First freelist entry = 7
Iterating over freelist...(OK)
Final freelist entry = 9
Total freelist entries = 3

** Transaction Log Info
version = 1.0
sequence = 1
entry_offs = 1092
append_offs = 1887

** Transaction Log Info for: all
tx_id = 1 (offs = 56)
commit_offs = 161
commit_items = 1
next_entry_offs = 214
prior_entry_offs = 0
total_data_bytes = 0
data_transform_id = 0
index_transform_id = 0

flags = 1 (create) offs = 144
tx_oid = 0
index_entry_id = 0

flags = 16 (store) offs = 161
data_pos = 0
data_size = 28
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 00 00 00 00 00 00 00 00              ............

tx_id = 2 (offs = 214)
commit_offs = 414
commit_items = 4
next_entry_offs = 653
prior_entry_offs = 56
total_data_bytes = 28
data_transform_id = 1
index_transform_id = 2

flags = 1 (create) offs = 302
tx_oid = 0
index_entry_id = 1

flags = 1 (create) offs = 319
tx_oid = 0
index_entry_id = 2

flags = 1 (create) offs = 336
tx_oid = 0
index_entry_id = 3

flags = 2 (update) offs = 353
tx_oid = 1
data_pos = 0
data_size = 28
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 00 00 00 00 00 00 00 00              ............

flags = 16 (store) offs = 414
data_pos = 28
data_size = 52
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 03 00 00 00 00 00 00 00 01 00 00 00  ................
0000000000000020  00 00 00 00 02 00 00 00 00 00 00 00 03 00 00 00  ................
0000000000000030  00 00 00 00                                      ....

flags = 16 (store) offs = 491
data_pos = 80
data_size = 29
index_entry_id = 3
0000000000000000  05 00 00 00 00 00 00 00 77 77 77 23 32 ff ff ff  ........www#2...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 16 (store) offs = 545
data_pos = 109
data_size = 29
index_entry_id = 2
0000000000000000  05 00 00 00 00 00 00 00 77 77 77 23 31 ff ff ff  ........www#1...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 16 (store) offs = 599
data_pos = 138
data_size = 29
index_entry_id = 1
0000000000000000  05 00 00 00 00 00 00 00 77 77 77 23 30 ff ff ff  ........www#0...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

tx_id = 3 (offs = 653)
commit_offs = 1012
commit_items = 4
next_entry_offs = 1092
prior_entry_offs = 214
total_data_bytes = 167
data_transform_id = 2
index_transform_id = 7

flags = 3 (destroy) offs = 741
tx_oid = 2
data_pos = 138
data_size = 29
index_entry_id = 1
0000000000000000  05 00 00 00 00 00 00 00 77 77 77 23 30 ff ff ff  ........www#0...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 3 (destroy) offs = 803
tx_oid = 2
data_pos = 109
data_size = 29
index_entry_id = 2
0000000000000000  05 00 00 00 00 00 00 00 77 77 77 23 31 ff ff ff  ........www#1...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 3 (destroy) offs = 865
tx_oid = 2
data_pos = 80
data_size = 29
index_entry_id = 3
0000000000000000  05 00 00 00 00 00 00 00 77 77 77 23 32 ff ff ff  ........www#2...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 2 (update) offs = 927
tx_oid = 2
data_pos = 28
data_size = 52
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 03 00 00 00 00 00 00 00 01 00 00 00  ................
0000000000000020  00 00 00 00 02 00 00 00 00 00 00 00 03 00 00 00  ................
0000000000000030  00 00 00 00                                      ....

flags = 16 (store) offs = 1012
data_pos = 28
data_size = 28
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 00 00 00 00 00 00 00 00              ............

flags = 19 (freelist) offs = 1065
index_entry_id = 3

flags = 19 (freelist) offs = 1074
index_entry_id = 2

flags = 19 (freelist) offs = 1083
index_entry_id = 1

tx_id = 4 (offs = 1092)
commit_offs = 1462
commit_items = 7
next_entry_offs = 0
prior_entry_offs = 653
total_data_bytes = 138
data_transform_id = 3
index_transform_id = 12

flags = 33 (create) offs = 1180
tx_oid = 3
data_opos = 3
index_entry_id = 1

flags = 33 (create) offs = 1205
tx_oid = 3
data_opos = 4
index_entry_id = 2

flags = 1 (create) offs = 1230
tx_oid = 3
index_entry_id = 3

flags = 2 (update) offs = 1247
tx_oid = 3
data_pos = 28
data_size = 28
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 00 00 00 00 00 00 00 00              ............

flags = 4 (nested start) offs = 1308
nested tx level = 2

flags = 1 (create) offs = 1317
tx_oid = 0
index_entry_id = 4

flags = 1 (create) offs = 1334
tx_oid = 0
index_entry_id = 5

flags = 1 (create) offs = 1351
tx_oid = 0
index_entry_id = 6

flags = 5 (nested commit) offs = 1368
nested tx level = 2

flags = 4 (nested start) offs = 1377
nested tx level = 2

flags = 1 (create) offs = 1386
tx_oid = 0
index_entry_id = 7

flags = 1 (create) offs = 1403
tx_oid = 0
index_entry_id = 8

flags = 1 (create) offs = 1420
tx_oid = 0
index_entry_id = 9

flags = 7 (nested rollback) offs = 1437
nested tx level = 2

flags = 16 (store) offs = 1462
data_pos = 138
data_size = 76
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 06 00 00 00 00 00 00 00 01 00 00 00  ................
0000000000000020  00 00 00 00 02 00 00 00 00 00 00 00 03 00 00 00  ................
0000000000000030  00 00 00 00 04 00 00 00 00 00 00 00 05 00 00 00  ................
0000000000000040  00 00 00 00 06 00 00 00 00 00 00 00              ............

flags = 16 (store) offs = 1563
data_pos = 214
data_size = 29
index_entry_id = 6
0000000000000000  05 00 00 00 00 00 00 00 79 79 79 23 32 ff ff ff  ........yyy#2...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 16 (store) offs = 1617
data_pos = 243
data_size = 29
index_entry_id = 5
0000000000000000  05 00 00 00 00 00 00 00 79 79 79 23 31 ff ff ff  ........yyy#1...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 16 (store) offs = 1671
data_pos = 272
data_size = 29
index_entry_id = 4
0000000000000000  05 00 00 00 00 00 00 00 79 79 79 23 30 ff ff ff  ........yyy#0...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 16 (store) offs = 1725
data_pos = 301
data_size = 29
index_entry_id = 3
0000000000000000  05 00 00 00 00 00 00 00 78 78 78 23 32 ff ff ff  ........xxx#2...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 16 (store) offs = 1779
data_pos = 330
data_size = 29
index_entry_id = 2
0000000000000000  05 00 00 00 00 00 00 00 78 78 78 23 31 ff ff ff  ........xxx#1...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

flags = 16 (store) offs = 1833
data_pos = 359
data_size = 29
index_entry_id = 1
0000000000000000  05 00 00 00 00 00 00 00 78 78 78 23 30 ff ff ff  ........xxx#0...
0000000000000010  ff ff ff ff ff 00 00 00 00 00 00 00 00           .............

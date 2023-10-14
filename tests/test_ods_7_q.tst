** File Info
Version: 1.1
Num Logs = 1
Num Trans = 0
Num Writers = 0
Total Entries = 7
Tranlog Offset = 1092
Transaction Id = 5
Index Free List = 1
Total Size of Data = 80
Data Transformation Id = 3
Index Transformation Id = 23

** Entry Info for: all
num: 0000000000000000          pos: 000000000000001c          len: 000000000000001c
txn: 0000000000000003          txo: 0000000000000000               flags: lk=0 tx=0
000000000000001c  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
000000000000002c  ff ff ff ff 00 00 00 00 00 00 00 00              ............

num: 0000000000000001          pos: 0000000000000003          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: 0000000000000002

num: 0000000000000002          pos: 0000000000000004          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: 0000000000000003

num: 0000000000000003          pos: 0000000000000005          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: 0000000000000004

num: 0000000000000004          pos: 0000000000000006          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: 0000000000000005

num: 0000000000000005          pos: 0000000000000007          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: 0000000000000006

num: 0000000000000006          pos: 0000000000000000          len: 0000000000000000
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=3
(freelist entry) link: <at end>

** Freelist Info
First freelist entry = 1
Iterating over freelist...(OK)
Final freelist entry = 6
Total freelist entries = 6

** Transaction Log Info
version = 1.1
sequence = 1
entry_offs = 1092
append_offs = 1494

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
commit_offs = 1494
commit_items = 0
next_entry_offs = 0
prior_entry_offs = 653
total_data_bytes = 80
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

flags = 7 (nested rollback) offs = 1368
nested tx level = 2

flags = 4 (nested start) offs = 1393
nested tx level = 2

flags = 33 (create) offs = 1402
tx_oid = 4
data_opos = 6
index_entry_id = 4

flags = 33 (create) offs = 1427
tx_oid = 4
data_opos = 7
index_entry_id = 5

flags = 1 (create) offs = 1452
tx_oid = 4
index_entry_id = 6

flags = 7 (nested rollback) offs = 1469
nested tx level = 2

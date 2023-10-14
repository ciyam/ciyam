** File Info
Version: 1.1
Num Logs = 1
Num Trans = 0
Num Writers = 0
Total Entries = 6
Tranlog Offset = 1051
Transaction Id = 5
Index Free List = n/a
Total Size of Data = 203
Data Transformation Id = 4
Index Transformation Id = 18

** Entry Info for: all
num: 0000000000000000          pos: 0000000000000051          len: 0000000000000044
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
0000000000000051  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000061  ff ff ff ff 05 00 00 00 00 00 00 00 01 00 00 00  ................
0000000000000071  00 00 00 00 02 00 00 00 00 00 00 00 03 00 00 00  ................
0000000000000081  00 00 00 00 04 00 00 00 00 00 00 00 05 00 00 00  ................
0000000000000091  00 00 00 00                                      ....

num: 0000000000000001          pos: 0000000000000036          len: 000000000000001b
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
0000000000000036  03 00 00 00 00 00 00 00 61 61 61 ff ff ff ff ff  ........aaa.....
0000000000000046  ff ff ff 00 00 00 00 00 00 00 00                 ...........

num: 0000000000000002          pos: 000000000000001b          len: 000000000000001b
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
000000000000001b  03 00 00 00 00 00 00 00 62 62 62 ff ff ff ff ff  ........bbb.....
000000000000002b  ff ff ff 00 00 00 00 00 00 00 00                 ...........

num: 0000000000000003          pos: 0000000000000000          len: 000000000000001b
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
0000000000000000  03 00 00 00 00 00 00 00 63 63 63 ff ff ff ff ff  ........ccc.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

num: 0000000000000004          pos: 00000000000000b0          len: 000000000000001b
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
00000000000000b0  03 00 00 00 00 00 00 00 64 64 64 ff ff ff ff ff  ........ddd.....
00000000000000c0  ff ff ff 00 00 00 00 00 00 00 00                 ...........

num: 0000000000000005          pos: 0000000000000095          len: 000000000000001b
txn: 0000000000000004          txo: 0000000000000000               flags: lk=0 tx=0
0000000000000095  03 00 00 00 00 00 00 00 65 65 65 ff ff ff ff ff  ........eee.....
00000000000000a5  ff ff ff 00 00 00 00 00 00 00 00                 ...........

** Freelist Info
No freelist entries.

** Transaction Log Info
version = 1.1
sequence = 1
entry_offs = 1051
append_offs = 1588

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

[xxx]
tx_id = 2 (offs = 214)
commit_offs = 414
commit_items = 4
next_entry_offs = 647
prior_entry_offs = 56
total_data_bytes = 28
data_transform_id = 1
index_transform_id = 2

flags = 1 (create) offs = 302
tx_oid = 0
index_entry_id = 1

flags = 2 (update) offs = 319
tx_oid = 1
data_pos = 0
data_size = 28
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 00 00 00 00 00 00 00 00              ............

flags = 1 (create) offs = 380
tx_oid = 0
index_entry_id = 2

flags = 1 (create) offs = 397
tx_oid = 0
index_entry_id = 3

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
data_size = 27
index_entry_id = 3
0000000000000000  03 00 00 00 00 00 00 00 63 63 63 ff ff ff ff ff  ........ccc.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

flags = 16 (store) offs = 543
data_pos = 107
data_size = 27
index_entry_id = 2
0000000000000000  03 00 00 00 00 00 00 00 62 62 62 ff ff ff ff ff  ........bbb.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

flags = 16 (store) offs = 595
data_pos = 134
data_size = 27
index_entry_id = 1
0000000000000000  03 00 00 00 00 00 00 00 61 61 61 ff ff ff ff ff  ........aaa.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

[yyy]
tx_id = 3 (offs = 647)
commit_offs = 854
commit_items = 3
next_entry_offs = 1051
prior_entry_offs = 214
total_data_bytes = 161
data_transform_id = 2
index_transform_id = 7

flags = 1 (create) offs = 735
tx_oid = 0
index_entry_id = 4

flags = 2 (update) offs = 752
tx_oid = 2
data_pos = 28
data_size = 52
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 03 00 00 00 00 00 00 00 01 00 00 00  ................
0000000000000020  00 00 00 00 02 00 00 00 00 00 00 00 03 00 00 00  ................
0000000000000030  00 00 00 00                                      ....

flags = 1 (create) offs = 837
tx_oid = 0
index_entry_id = 5

flags = 16 (store) offs = 854
data_pos = 161
data_size = 68
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 05 00 00 00 00 00 00 00 01 00 00 00  ................
0000000000000020  00 00 00 00 02 00 00 00 00 00 00 00 03 00 00 00  ................
0000000000000030  00 00 00 00 04 00 00 00 00 00 00 00 05 00 00 00  ................
0000000000000040  00 00 00 00                                      ....

flags = 16 (store) offs = 947
data_pos = 229
data_size = 27
index_entry_id = 5
0000000000000000  03 00 00 00 00 00 00 00 65 65 65 ff ff ff ff ff  ........eee.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

flags = 16 (store) offs = 999
data_pos = 256
data_size = 27
index_entry_id = 4
0000000000000000  03 00 00 00 00 00 00 00 64 64 64 ff ff ff ff ff  ........ddd.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

tx_id = 4 (offs = 1051)
commit_offs = 1139
commit_items = 6
next_entry_offs = 0
prior_entry_offs = 647
total_data_bytes = 283
data_transform_id = 3
index_transform_id = 11

flags = 32 (store) offs = 1139
tx_oid = 2
data_pos = 0
data_opos = 80
data_size = 27
index_entry_id = 3
0000000000000000  03 00 00 00 00 00 00 00 63 63 63 ff ff ff ff ff  ........ccc.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

flags = 32 (store) offs = 1207
tx_oid = 2
data_pos = 27
data_opos = 107
data_size = 27
index_entry_id = 2
0000000000000000  03 00 00 00 00 00 00 00 62 62 62 ff ff ff ff ff  ........bbb.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

flags = 32 (store) offs = 1275
tx_oid = 2
data_pos = 54
data_opos = 134
data_size = 27
index_entry_id = 1
0000000000000000  03 00 00 00 00 00 00 00 61 61 61 ff ff ff ff ff  ........aaa.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

flags = 32 (store) offs = 1343
tx_oid = 3
data_pos = 81
data_opos = 161
data_size = 68
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 05 00 00 00 00 00 00 00 01 00 00 00  ................
0000000000000020  00 00 00 00 02 00 00 00 00 00 00 00 03 00 00 00  ................
0000000000000030  00 00 00 00 04 00 00 00 00 00 00 00 05 00 00 00  ................
0000000000000040  00 00 00 00                                      ....

flags = 32 (store) offs = 1452
tx_oid = 3
data_pos = 149
data_opos = 229
data_size = 27
index_entry_id = 5
0000000000000000  03 00 00 00 00 00 00 00 65 65 65 ff ff ff ff ff  ........eee.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

flags = 32 (store) offs = 1520
tx_oid = 3
data_pos = 176
data_opos = 256
data_size = 27
index_entry_id = 4
0000000000000000  03 00 00 00 00 00 00 00 64 64 64 ff ff ff ff ff  ........ddd.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

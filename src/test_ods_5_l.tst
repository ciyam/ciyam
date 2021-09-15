** File Info
Version: 1.0
Num Logs = 1
Num Trans = 0
Num Writers = 0
Total Entries = 3
Tranlog Offset = 214
Transaction Id = 3
Index Free List = n/a
Total Size of Data = 126
Data Transformation Id = 2
Index Transformation Id = 5

** Entry Info for: all
num: 0000000000000000          pos: 000000000000001c          len: 000000000000002c
txn: 0000000000000002          txo: 0000000000000000               flags: lk=0 tx=0
000000000000001c  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
000000000000002c  ff ff ff ff 02 00 00 00 00 00 00 00 01 00 00 00  ................
000000000000003c  00 00 00 00 02 00 00 00 00 00 00 00              ............

num: 0000000000000001          pos: 0000000000000063          len: 000000000000001b
txn: 0000000000000002          txo: 0000000000000000               flags: lk=0 tx=0
0000000000000063  03 00 00 00 00 00 00 00 78 78 78 ff ff ff ff ff  ........xxx.....
0000000000000073  ff ff ff 00 00 00 00 00 00 00 00                 ...........

num: 0000000000000002          pos: 0000000000000048          len: 000000000000001b
txn: 0000000000000002          txo: 0000000000000000               flags: lk=0 tx=0
0000000000000048  03 00 00 00 00 00 00 00 79 79 79 ff ff ff ff ff  ........yyy.....
0000000000000058  ff ff ff 00 00 00 00 00 00 00 00                 ...........

** Freelist Info
No freelist entries.

** Transaction Log Info
version = 1.0
sequence = 1
entry_offs = 214
append_offs = 602

** Transaction Log Info for: all
tx_id = 1 (offs = 56)
commit_offs = 161
commit_items = 1
next_entry_offs = 214
prior_entry_offs = 0
total_data_bytes = 0
data_transform_id = 0
index_transform_id = 0

flags = 9 (create) offs = 144
tx_oid = 0
index_entry_id = 0

flags = 24 (store) offs = 161
data_pos = 0
data_size = 28
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 00 00 00 00 00 00 00 00              ............

tx_id = 2 (offs = 214)
commit_offs = 421
commit_items = 3
next_entry_offs = 0
prior_entry_offs = 56
total_data_bytes = 28
data_transform_id = 1
index_transform_id = 1

flags = 1 (create) offs = 302
tx_id = 2
tx_oid = 0
index_entry_id = 1

flags = 2 (update) offs = 327
tx_id = 2
tx_oid = 1
data_pos = 0
data_size = 28
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 00 00 00 00 00 00 00 00              ............

flags = 1 (create) offs = 396
tx_id = 2
tx_oid = 0
index_entry_id = 2

flags = 16 (store) offs = 421
tx_id = 2
data_pos = 28
data_size = 44
index_entry_id = 0
0000000000000000  04 00 00 00 00 00 00 00 72 6f 6f 74 ff ff ff ff  ........root....
0000000000000010  ff ff ff ff 02 00 00 00 00 00 00 00 01 00 00 00  ................
0000000000000020  00 00 00 00 02 00 00 00 00 00 00 00              ............

flags = 20 (store) offs = 498
data_pos = 72
data_size = 27
index_entry_id = 2
0000000000000000  03 00 00 00 00 00 00 00 79 79 79 ff ff ff ff ff  ........yyy.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

flags = 20 (store) offs = 550
data_pos = 99
data_size = 27
index_entry_id = 1
0000000000000000  03 00 00 00 00 00 00 00 78 78 78 ff ff ff ff ff  ........xxx.....
0000000000000010  ff ff ff 00 00 00 00 00 00 00 00                 ...........

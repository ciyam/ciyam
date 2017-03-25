
> file_raw -text blob "Hello World!"
2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8

> file_raw -text blob "This is a test."
90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3

> file_raw -text list "2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 hello\n90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 test" root
c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1

> file_raw -text blob "at 0..."
fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25

> file_raw -text blob "at 1..."
055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08

> file_raw -text list "fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25 0\n055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08 1"
f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32

> file_raw -text list "c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 first\nf0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 second" root
35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473

> file_hash root
35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473

> file_info -content root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 first
f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 second

> file_info -recurse -d=1 root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
first
 [list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
 ...
second
 [list] f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 (134 B)
 ...

> file_info -recurse -d=2 root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
first
 [list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
 hello
  [blob] 2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 (13 B)
  ...
 test
  [blob] 90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 (16 B)
  ...
second
 [list] f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 (134 B)
 0
  [blob] fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25 (8 B)
  ...
 1
  [blob] 055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08 (8 B)
  ...

> file_info -recurse -d=0 root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
first
 [list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
 hello
  [blob] 2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 (13 B) [utf8]
Hello World!
 test
  [blob] 90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 (16 B) [utf8]
This is a test.
second
 [list] f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 (134 B)
 0
  [blob] fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25 (8 B) [utf8]
at 0...
 1
  [blob] 055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08 (8 B) [utf8]
at 1...

> file_kill -recurse 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473

> ~mkdir test1

> 
> ~mkdir test2

> 
> file_archive -add test1 10MiB test1

> file_archive -add test2 10MiB test2

> file_archive -add test3 10MiB test3

> file_archives
test1 [okay      ] (0 B/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [bad access] (0 B/10.5 MB) test3

> ~mkdir test3

> 
> file_archives -status_update
test1 [okay      ] (0 B/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3

> session_variable @dummy_timestamp 20170313080001

> file_put user1.jpg
file_put b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9

> session_variable @dummy_timestamp 20170313080002

> file_put user2.jpg
file_put efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f

> session_variable @dummy_timestamp 20170313080000

> file_put test.jpg
file_put a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753

> file_tags ts.*
ts.20170313080000
ts.20170313080001
ts.20170313080002

> file_relegate -n=1
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test1

> file_tags ts.*
ts.20170313080001
ts.20170313080002

> file_archives
test1 [okay      ] (6.7 kB/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3

> file_archive -remove test1
(removing file archive)

> file_archives
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3

> session_variable @dummy_timestamp 20170313080003

> file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
Error: unable to retrieve file a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 from archival

> file_archive -add test1 10MiB test1

> file_archives
test1 [okay      ] (0 B/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3

> file_archive -repair test1
(repairing file archive)

> file_archives
test1 [okay      ] (6.7 kB/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3

> session_variable @dummy_timestamp 20170313080003

> file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
test1

> file_tags ts.*
ts.20170313080001
ts.20170313080002
ts.20170313080003

> file_info ts.*
[blob] b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 (5.7 kB)
[blob] efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f (3.1 kB)
[blob] a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 (6.7 kB)

> file_relegate -s=9KiB test2
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test2
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2

> file_tags ts.*
ts.20170313080003

> file_relegate -n=1 test3
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test3

> file_tags ts.*

> file_archives
test1 [okay      ] (6.7 kB/10.5 MB) test1
test2 [okay      ] (8.8 kB/10.5 MB) test2
test3 [okay      ] (6.7 kB/10.5 MB) test3

> session_variable @dummy_timestamp 20170313080000

> file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
test1

> session_variable @dummy_timestamp 20170313080001

> file_retrieve b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9
test2

> file_tags ts.*
ts.20170313080000
ts.20170313080001

> file_kill -p=ts.*

> file_tags ts.*

> file_archive -destroy test1
(destroying file archive)

> file_archive -destroy test2
(destroying file archive)

> file_archive -destroy test3
(destroying file archive)

> file_archives

> ~rmdir test1

> 
> ~rmdir test2

> 
> ~rmdir test3

> 
> 

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

> file_kill -recurse root

> ~mkdir test1

> 
> file_put 1K*test.jpg test

> file_info -recurse -d=999 test
[list] 0f2ea74eb8ff3b179716384dbfb4c9376458edb7747ba0dd850ef8bcc2954f87 (307 B)
test.jpg.00000
 [blob] fa7fe19a948a3e07b0107ab581c09500be6b837e2e90f8eb7f2ba632354993cd (1.0 kB)
test.jpg.00001
 [blob] 629cb7f34aa4a0f60643a6a8deec5ba26fbc7af51da4bac6daa8a9a69b630e29 (1.0 kB)
test.jpg.00002
 [blob] e858be066360a12b6ee02c7f51781b57e844b50a7ebf2513b392cd1e65fefa4e (1.0 kB)
test.jpg.00003
 [blob] 2b34aae56c8ffe3a74b815a6c6f49b9bf7cb049695021b021f4360749281698b (1.0 kB)
test.jpg.00004
 [blob] 0af58aa60a76fcea76d5918a1f37da511e881fffb2dcda3c00ce7c244c8a9510 (1.0 kB)
test.jpg.00005
 [blob] ff9a4b6004c5f37083d4c2555a364d5e2b61375984516a64c171314834d414cf (1.0 kB)
test.jpg.00006
 [blob] 9c729f25c1dd59fd4ca5f140159c57d502248f171892697e05872dee655982b6 (715 B)

> file_get test *~test.jpg

> file_kill -p=test

> file_put 1K*~test.jpg test

> 
> file_info -recurse -d=999 test
[list] 2c7f28963c4893c5c2a209af806ebf1e18618ad64a23595d1df6bacbd137aeae (309 B)
~test.jpg.00000
 [blob] fa7fe19a948a3e07b0107ab581c09500be6b837e2e90f8eb7f2ba632354993cd (1.0 kB)
~test.jpg.00001
 [blob] 629cb7f34aa4a0f60643a6a8deec5ba26fbc7af51da4bac6daa8a9a69b630e29 (1.0 kB)
~test.jpg.00002
 [blob] e858be066360a12b6ee02c7f51781b57e844b50a7ebf2513b392cd1e65fefa4e (1.0 kB)
~test.jpg.00003
 [blob] 2b34aae56c8ffe3a74b815a6c6f49b9bf7cb049695021b021f4360749281698b (1.0 kB)
~test.jpg.00004
 [blob] 0af58aa60a76fcea76d5918a1f37da511e881fffb2dcda3c00ce7c244c8a9510 (1.0 kB)
~test.jpg.00005
 [blob] ff9a4b6004c5f37083d4c2555a364d5e2b61375984516a64c171314834d414cf (1.0 kB)
~test.jpg.00006
 [blob] 9c729f25c1dd59fd4ca5f140159c57d502248f171892697e05872dee655982b6 (715 B)

> file_crypt test abc

> file_info -recurse -d=999 test
[list] 2c7f28963c4893c5c2a209af806ebf1e18618ad64a23595d1df6bacbd137aeae (309 B) [***]

> file_info -content fa7fe19a948a3e07b0107ab581c09500be6b837e2e90f8eb7f2ba632354993cd
[blob] fa7fe19a948a3e07b0107ab581c09500be6b837e2e90f8eb7f2ba632354993cd (1.0 kB) [***]

> file_info -content 2b34aae56c8ffe3a74b815a6c6f49b9bf7cb049695021b021f4360749281698b
[blob] 2b34aae56c8ffe3a74b815a6c6f49b9bf7cb049695021b021f4360749281698b (1.0 kB) [***]

> file_info -content 9c729f25c1dd59fd4ca5f140159c57d502248f171892697e05872dee655982b6
[blob] 9c729f25c1dd59fd4ca5f140159c57d502248f171892697e05872dee655982b6 (715 B) [***]

> file_crypt test xxx
Error: invalid password to decrypt file 'test'

> file_crypt test abc

> file_get test *test1/
test1/~test.jpg

> file_kill -recurse test

> file_put 1K*test1/~test.jpg test

> 
> file_info -recurse -d=999 test
[list] 6059f821b67bb158c702646f0ea760557b078dc57cd5e685149ac9918f3909a9 (314 B)
test1/~test.jpg.00000
 [blob] fa7fe19a948a3e07b0107ab581c09500be6b837e2e90f8eb7f2ba632354993cd (1.0 kB)
test1/~test.jpg.00001
 [blob] 629cb7f34aa4a0f60643a6a8deec5ba26fbc7af51da4bac6daa8a9a69b630e29 (1.0 kB)
test1/~test.jpg.00002
 [blob] e858be066360a12b6ee02c7f51781b57e844b50a7ebf2513b392cd1e65fefa4e (1.0 kB)
test1/~test.jpg.00003
 [blob] 2b34aae56c8ffe3a74b815a6c6f49b9bf7cb049695021b021f4360749281698b (1.0 kB)
test1/~test.jpg.00004
 [blob] 0af58aa60a76fcea76d5918a1f37da511e881fffb2dcda3c00ce7c244c8a9510 (1.0 kB)
test1/~test.jpg.00005
 [blob] ff9a4b6004c5f37083d4c2555a364d5e2b61375984516a64c171314834d414cf (1.0 kB)
test1/~test.jpg.00006
 [blob] 9c729f25c1dd59fd4ca5f140159c57d502248f171892697e05872dee655982b6 (715 B)

> file_info -recurse -d=999 test
[list] 6059f821b67bb158c702646f0ea760557b078dc57cd5e685149ac9918f3909a9 (314 B)
test1/~test.jpg.00000
 [blob] fa7fe19a948a3e07b0107ab581c09500be6b837e2e90f8eb7f2ba632354993cd (1.0 kB)
test1/~test.jpg.00001
 [blob] 629cb7f34aa4a0f60643a6a8deec5ba26fbc7af51da4bac6daa8a9a69b630e29 (1.0 kB)
test1/~test.jpg.00002
 [blob] e858be066360a12b6ee02c7f51781b57e844b50a7ebf2513b392cd1e65fefa4e (1.0 kB)
test1/~test.jpg.00003
 [blob] 2b34aae56c8ffe3a74b815a6c6f49b9bf7cb049695021b021f4360749281698b (1.0 kB)
test1/~test.jpg.00004
 [blob] 0af58aa60a76fcea76d5918a1f37da511e881fffb2dcda3c00ce7c244c8a9510 (1.0 kB)
test1/~test.jpg.00005
 [blob] ff9a4b6004c5f37083d4c2555a364d5e2b61375984516a64c171314834d414cf (1.0 kB)
test1/~test.jpg.00006
 [blob] 9c729f25c1dd59fd4ca5f140159c57d502248f171892697e05872dee655982b6 (715 B)

> file_kill -recurse test

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

> file_put test1.jpg

> session_variable @dummy_timestamp 20170313080002

> file_put test2.jpg

> session_variable @dummy_timestamp 20170313080000

> file_put test.jpg

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
test1 [okay      ] (6.5 kB/10.5 MB) test1
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
test1 [okay      ] (6.5 kB/10.5 MB) test1
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
[blob] b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 (5.3 kB)
[blob] efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f (2.8 kB)
[blob] a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 (6.5 kB)

> file_relegate -s=9KiB test2
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test2
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2

> file_tags ts.*
ts.20170313080003

> file_relegate -n=1 test3
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test3

> file_tags ts.*

> file_archives
test1 [okay      ] (6.5 kB/10.5 MB) test1
test2 [okay      ] (8.1 kB/10.5 MB) test2
test3 [okay      ] (6.5 kB/10.5 MB) test3

> session_variable @dummy_timestamp 20170313080000

> file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
test1

> session_variable @dummy_timestamp 20170313080001

> file_retrieve b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9
test2

> file_get a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 ~test.jpg

> file_put ~test.jpg

> 
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
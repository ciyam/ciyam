file_raw -text blob "Hello World!"
2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8
file_raw -text blob "This is a test."
90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3
file_raw -text list "2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 hello\n90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 test" root
c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1
file_raw -text blob "at 0..."
fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25
file_raw -text blob "at 1..."
055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08
file_raw -text list "fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25 0\n055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08 1"
f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32
file_raw -text list "c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 first\nf0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 second" root
35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473
file_hash root
35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473
file_info -content root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 first
f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 second
file_info -recurse -d=1 root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
first
 [list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
 ...
second
 [list] f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 (134 B)
 ...
file_info -recurse -d=2 root
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
file_info -recurse -d=0 root
35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473
c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1
2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8
90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3
f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32
fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25
055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08
file_info -recurse -d=-1 root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
000000 c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 first (141 B)
000001 f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 second (134 B)
file_info -recurse -d=-2 root
[list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
000000 2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 hello (13 B)
000001 90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 test (16 B)
[list] f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32 (134 B)
000000 fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25 0 (8 B)
000001 055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08 1 (8 B)
file_tag fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25 test0
file_tag 055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08 test1
file_raw list test0,test1 testx
167359887b16cabc3e8293fd11e2cb3a8a9f18145ef52847f5c704819f897033
file_info -content testx
[list] 167359887b16cabc3e8293fd11e2cb3a8a9f18145ef52847f5c704819f897033 (106 B)
fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25 test0
055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08 test1
file_tags test*
test0
test1
testx
file_tags -i=te*0,te*1
test0
test1
file_tags -i=test* -x=*x
test0
test1
file_tags -i=test* -x=*0,*x
test1
file_kill testx
file_tags test*
test0
test1
file_tag -remove test0,test1
file_tags test*
file_kill -recurse root
~mkdir test1
file_put 1K*test.jpg test
file_info -recurse -d=999 test
[list] 2d3c89f8f5301604234589e08a695e3ab0bdaa5f99ec21cf148b99d13020cb85 (307 B)
test.jpg.000000
 [blob] 8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d (1.0 kB)
test.jpg.000001
 [blob] cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 (1.0 kB)
test.jpg.000002
 [blob] e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 (1.0 kB)
test.jpg.000003
 [blob] 54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 (1.0 kB)
test.jpg.000004
 [blob] 579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 (1.0 kB)
test.jpg.000005
 [blob] 096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 (1.0 kB)
test.jpg.000006
 [blob] 15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 (715 B)
file_get test.xxx
Error: file 'test.xxx' not found
file_get test.xxx
Error: file 'test.xxx' not found
file_get modules.lst.default
Error: local file 'modules.lst.default' already exists
file_get modules.lst.default
Error: local file 'modules.lst.default' already exists
file_get test *~test.jpg
file_kill -p=test
file_put 1K*~test.jpg test
file_stats
[8/100000]7.0 kB/100.0 GB:8 tag(s)
system_variable @files_area_dir test1
file_stats
[0/100000]0 B/100.0 GB:0 tag(s)
system_variable @files_area_dir ""
file_stats
[8/100000]7.0 kB/100.0 GB:8 tag(s)
file_info -recurse -d=999 test
[list] cabb71470df889b67d18bf7eaf562e2519168d7235d3aa1ed9a126ec3299e6dc (309 B)
~test.jpg.000000
 [blob] 8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d (1.0 kB)
~test.jpg.000001
 [blob] cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 (1.0 kB)
~test.jpg.000002
 [blob] e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 (1.0 kB)
~test.jpg.000003
 [blob] 54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 (1.0 kB)
~test.jpg.000004
 [blob] 579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 (1.0 kB)
~test.jpg.000005
 [blob] 096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 (1.0 kB)
~test.jpg.000006
 [blob] 15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 (715 B)
file_crypt 8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d abc
file_crypt cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 abc
file_crypt e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 abc
file_crypt 54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 abc
file_crypt 579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 abc
file_crypt 096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 abc
file_crypt 15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 abc
file_crypt test abc
file_info -recurse -d=999 test
[list] cabb71470df889b67d18bf7eaf562e2519168d7235d3aa1ed9a126ec3299e6dc (309 B) [***]
file_info -content 8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d
[blob] 8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d (1.0 kB) [***]
file_info -content 54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5
[blob] 54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 (1.0 kB) [***]
file_info -content 15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7
[blob] 15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 (715 B) [***]
file_crypt -recurse test xxx
Error: Invalid password to decrypt file 'test'.
file_crypt -recurse test abc
file_get test *test1/
test1/~test.jpg
file_kill -recurse test
file_put 1K*test1/~test.jpg test
session_variable @last_file_put
f674a700cc830e419574523d8388d6ed41e3c13351f80a17b9f9eb308b1410e0
file_info -recurse -d=999 test
[list] f674a700cc830e419574523d8388d6ed41e3c13351f80a17b9f9eb308b1410e0 (315 B)
test1/~test.jpg.000000
 [blob] 8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d (1.0 kB)
test1/~test.jpg.000001
 [blob] cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 (1.0 kB)
test1/~test.jpg.000002
 [blob] e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 (1.0 kB)
test1/~test.jpg.000003
 [blob] 54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 (1.0 kB)
test1/~test.jpg.000004
 [blob] 579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 (1.0 kB)
test1/~test.jpg.000005
 [blob] 096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 (1.0 kB)
test1/~test.jpg.000006
 [blob] 15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 (715 B)
file_info -recurse -d=999 test
[list] f674a700cc830e419574523d8388d6ed41e3c13351f80a17b9f9eb308b1410e0 (315 B)
test1/~test.jpg.000000
 [blob] 8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d (1.0 kB)
test1/~test.jpg.000001
 [blob] cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 (1.0 kB)
test1/~test.jpg.000002
 [blob] e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 (1.0 kB)
test1/~test.jpg.000003
 [blob] 54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 (1.0 kB)
test1/~test.jpg.000004
 [blob] 579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 (1.0 kB)
test1/~test.jpg.000005
 [blob] 096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 (1.0 kB)
test1/~test.jpg.000006
 [blob] 15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 (715 B)
file_kill -recurse test
~mkdir test2
file_archive -add test1 10MiB test1
file_archive -add test2 10MiB test2
file_archive -add test3 10MiB test3
file_archives
test1 [okay      ] (0 B/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [bad access] (0 B/10.5 MB) test3
~mkdir test3
file_archives -status_update
test1 [okay      ] (0 B/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
session_variable @dummy_timestamp 20170313080001
file_put test1.jpg
session_variable @dummy_timestamp 20170313080002
session_variable @last_file_put
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9
file_put test2.jpg
session_variable @dummy_timestamp 20170313080000
session_variable @last_file_put
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f
file_put test.jpg
file_tags ts.*
ts.20170313080000
ts.20170313080001
ts.20170313080002
file_relegate -n=1
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test1
file_tags ts.*
ts.20170313080001
ts.20170313080002
file_archives
test1 [okay      ] (6.5 kB/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
file_archive -remove test1
(removing file archive)
file_archives
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
session_variable @dummy_timestamp 20170313080003
file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
Error: Unable to retrieve file a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 from archival.
file_archive -add test1 10MiB test1
file_archives
test1 [okay      ] (0 B/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
file_archive -repair test1
(repairing file archive)
file_archives
test1 [okay      ] (6.5 kB/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
session_variable @dummy_timestamp 20170313080003
file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
test1
file_tags ts.*
ts.20170313080001
ts.20170313080002
ts.20170313080003
file_info ts.*
[blob] b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 (5.3 kB)
[blob] efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f (2.8 kB)
[blob] a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 (6.5 kB)
file_relegate -s=9KiB test2
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test2
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2
file_tags ts.*
ts.20170313080003
file_relegate -n=1 test3
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test3
file_tags ts.*
file_archives
test1 [okay      ] (6.5 kB/10.5 MB) test1
test2 [okay      ] (8.1 kB/10.5 MB) test2
test3 [okay      ] (6.5 kB/10.5 MB) test3
session_variable @dummy_timestamp 20170313080000
file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
test1
session_variable @dummy_timestamp 20170313080001
file_retrieve b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9
test2
file_get a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 ~test.jpg
file_put ~test.jpg
file_tags ts.*
ts.20170313080000
ts.20170313080001
file_kill -p=ts.*
file_tags ts.*
file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 ts.20170313080001
test1
file_retrieve b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 ts.20170313080002
test2
file_raw list ts.20170313080001,ts.20170313080002 tst
d9a6301b0a1bfe36b3898dd78697616db6207004ddcb5dca903fb5b25f158f0c
file_relegate -n=2
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test1
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test1
file_tags ts*
tst
file_info -recurse -d=999 tst
[list] d9a6301b0a1bfe36b3898dd78697616db6207004ddcb5dca903fb5b25f158f0c (114 B)
ts.20170313080001
 [blob] a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 (6.5 kB)
ts.20170313080002
 [blob] b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 (5.3 kB)
file_stats
[3/100000]12.0 kB/100.0 GB:3 tag(s)
file_kill -p=ts*
file_archive -destroy test1
(destroying file archive)
file_archive -destroy test2
(destroying file archive)
file_archive -destroy test3
(destroying file archive)
file_archives
~rmdir test1
~rmdir test2
~rmdir test3
file_put test1.jpg test1.jpg
file_put test2.jpg test2.jpg
file_raw list test1.jpg,test2.jpg test.1
048bf68a8b49e8679a54154986a9a98bcced3687dbda5ce565531b39b5c21b33
file_info -content test.1
[list] 048bf68a8b49e8679a54154986a9a98bcced3687dbda5ce565531b39b5c21b33 (116 B)
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test1.jpg
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2.jpg
file_put test.jpg test.jpg
file_list *
Error: file '*' not found
file_list -a=test.jpg test.1 test.2
326c9c4eb765fbebe5ecc274e25089319a0eee03c4dae4047dc84e18da46347a
file_info -content test.1
[list] 048bf68a8b49e8679a54154986a9a98bcced3687dbda5ce565531b39b5c21b33 (116 B)
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test1.jpg
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2.jpg
file_info -content test.2
[list] 326c9c4eb765fbebe5ecc274e25089319a0eee03c4dae4047dc84e18da46347a (153 B)
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test1.jpg
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2.jpg
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test.jpg
file_list -sort test.2 test.3
e1d98de36694951cd4c6d12e94787f99487065ab5ab68a159450102c7a3995ce
file_info -content test.1
[list] 048bf68a8b49e8679a54154986a9a98bcced3687dbda5ce565531b39b5c21b33 (116 B)
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test1.jpg
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2.jpg
file_info -content test.2
[list] 326c9c4eb765fbebe5ecc274e25089319a0eee03c4dae4047dc84e18da46347a (153 B)
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test1.jpg
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2.jpg
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test.jpg
file_info -content test.3
[list] e1d98de36694951cd4c6d12e94787f99487065ab5ab68a159450102c7a3995ce (153 B)
a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 test.jpg
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test1.jpg
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2.jpg
file_kill -recurse test.3
file_tag -unlink test.1,test.2
file_tags

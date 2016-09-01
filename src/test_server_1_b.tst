
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

> 
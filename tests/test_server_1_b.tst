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
file_hash !35ddd
35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473
file_hash !f*
f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32
fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25
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
file_info -recurse -d=1 -p=first root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
first
 [list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
 ...
file_info -recurse -d=1 -p=^first root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
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
file_info -recurse -d=2 -p=first: root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
first
 [list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
 hello
  [blob] 2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 (13 B)
  ...
 test
  [blob] 90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 (16 B)
  ...
file_info -recurse -d=2 -p=^first: root
[list] 35dddd1f6a57c18adddca0b99478114fdef5a97cf5b5d0c2474dc777fe029473 (143 B)
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
file_info -total_items root
6
file_info -total_items root:!
2
file_info -total_items root:xxx
0/6
file_info -total_items root:c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1
1/6
file_info -total_items root:2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8
2/6
file_info -total_items root:90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3
3/6
file_info -total_items root:f0e0bbbf3321c7e483e3f7b4072e87791e1ec3cb74c3d4ac0db4faa765f12e32
4/6
file_info -total_items root:fb9677b46fbcd4bb532d10d305a5d8ebe90c9f252d655747a406ba1e7a859e25
5/6
file_info -total_items root:055ab3dc27be99b17779d4e5087c559f0f8743d5ac8575c5e340936b6d34ab08
6/6
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
notifier -start test1/xxx
system_variable *test1*
@:test1/xxx [watching]
test1/xxx none
~touch test1/xxx
system_variable *test1*
@:test1/xxx [watching]
test1/xxx modified
~rm test1/xxx
system_variable *test1*
notifier -start test1
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
test1/ [000000]none
~touch test1/x
~touch test1/y
~touch test1/z
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/x
@:test1/000002 test1/y
@:test1/000003 test1/z
test1/ [000000]none
test1/x [000001]#created
test1/y [000002]#created
test1/z [000003]#created
~touch test1/x
~touch test1/y
~touch test1/z
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/x
@:test1/000002 test1/y
@:test1/000003 test1/z
test1/ [000000]none
test1/x [000001]#created
test1/y [000002]#created
test1/z [000003]#created
~rm test1/x
~rm test1/y
~rm test1/z
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
test1/ [000000]none
~touch test1/x
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000004 test1/x
test1/ [000000]none
test1/x [000004]#created
~mv test1/x test1/xx
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000004 test1/xx
test1/ [000000]none
test1/xx [000004]#created
~mv test1/xx test1/xxx
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000004 test1/xxx
test1/ [000000]none
test1/xxx [000004]#created
~mv test1/xxx test1/x
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000004 test1/x
test1/ [000000]none
test1/x [000004]#created
notifier -finish test1
system_variable *test1*
~mkdir test1/xxx
notifier -start test1/xxx
system_variable *test1*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
test1/xxx/ [000000]none
~rmdir test1/xxx
system_variable *test1*
notifier -start test1
~mkdir test1/xxx
~mkdir test1/zzz
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/xxx/
@:test1/000002 test1/zzz/
test1/ [000000]none
test1/xxx/ [000001]created
test1/zzz/ [000002]created
~mv test1/zzz test1/yyy
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/xxx/
@:test1/000002 test1/yyy/
test1/ [000000]none
test1/xxx/ [000001]created
test1/yyy/ [000002]created
~mkdir test1/yyy/zzz
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/xxx/
@:test1/000002 test1/yyy/
@:test1/000003 test1/yyy/zzz/
test1/ [000000]none
test1/xxx/ [000001]created
test1/yyy/ [000002]created
test1/yyy/zzz/ [000003]created
~touch test1/yyy/zzz/abc
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/xxx/
@:test1/000002 test1/yyy/
@:test1/000003 test1/yyy/zzz/
@:test1/000004 test1/yyy/zzz/abc
test1/ [000000]none
test1/xxx/ [000001]created
test1/yyy/ [000002]created
test1/yyy/zzz/ [000003]created
test1/yyy/zzz/abc [000004]#created
~mv test1/yyy test1/xxx
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/xxx/
@:test1/000002 test1/xxx/yyy/
@:test1/000003 test1/xxx/yyy/zzz/
@:test1/000004 test1/xxx/yyy/zzz/abc
test1/ [000000]none
test1/xxx/ [000001]created
test1/xxx/yyy/ [000002]created
test1/xxx/yyy/zzz/ [000003]created
test1/xxx/yyy/zzz/abc [000004]#created
~touch test1/xxx/yyy/zzz/def
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/xxx/
@:test1/000002 test1/xxx/yyy/
@:test1/000003 test1/xxx/yyy/zzz/
@:test1/000004 test1/xxx/yyy/zzz/abc
@:test1/000005 test1/xxx/yyy/zzz/def
test1/ [000000]none
test1/xxx/ [000001]created
test1/xxx/yyy/ [000002]created
test1/xxx/yyy/zzz/ [000003]created
test1/xxx/yyy/zzz/abc [000004]#created
test1/xxx/yyy/zzz/def [000005]#created
notifier -finish test1
~rm -r test1/xxx
~touch test1/x
notifier -start test1
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/x
test1/ [000000]none
test1/x [000001]none
~mv test1/x test1/y
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/y
test1/ [000000]none
test1/y [000001]#moved_from|test1/x
~touch test1/z
~mv test1/z test1/y
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/y
test1/ [000000]none
test1/y [000001]#modified_from|test1/x
~rm test1/y
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/x
test1/ [000000]none
test1/x [000001]deleted
notifier -finish test1
~mkdir test1/xxx
~touch test1/xxx/111
~touch test1/xxx/222
notifier -start test1
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/xxx/
@:test1/000002 test1/xxx/111
@:test1/000003 test1/xxx/222
test1/ [000000]none
test1/xxx/ [000001]none
test1/xxx/111 [000002]none
test1/xxx/222 [000003]none
~mv test1/xxx test1/yyy
system_variable *test1*
@:test1/ [watching]
@:test1/000000 test1/
@:test1/000001 test1/yyy/
@:test1/000002 test1/yyy/111
@:test1/000003 test1/yyy/222
test1/ [000000]none
test1/yyy/ [000001]moved_from|test1/xxx/
test1/yyy/111 [000002]none
test1/yyy/222 [000003]none
notifier -finish test1
~rm -r test1/yyy
~mkdir test1/xxx
~touch test1/xxx/111 test1/xxx/222
notifier -start test1/xxx
system_variable *test1*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
@:test1/xxx/000002 test1/xxx/222
test1/xxx/ [000000]none
test1/xxx/111 [000001]none
test1/xxx/222 [000002]none
~touch test1/xxx/333
~mv test1/xxx/111 test1/xxx/222 test1
system_variable *test1*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
@:test1/xxx/000002 test1/xxx/222
@:test1/xxx/000003 test1/xxx/333
test1/xxx/ [000000]none
test1/xxx/111 [000001]deleted
test1/xxx/222 [000002]deleted
test1/xxx/333 [000003]#created
~rm test1/xxx/333
system_variable *test1*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
@:test1/xxx/000002 test1/xxx/222
test1/xxx/ [000000]none
test1/xxx/111 [000001]deleted
test1/xxx/222 [000002]deleted
notifier -finish test1/xxx
~rm test1/111 test1/222
~touch test1/xxx/111
notifier -start test1/xxx
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]none
system_variable test1/xxx/111 "[000001]++none"
~touch test1/xxx/111
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]++modified
~mv test1/xxx/111 test1/xxx/222
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/222
test1/xxx/ [000000]none
test1/xxx/222 [000001]++modified_from|test1/xxx/111
~rm test1/xxx/222
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]deleted
notifier -finish test1/xxx
~touch test1/xxx/111
notifier -start test1/xxx
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]none
~mv test1/xxx/111 test1/xxx/222
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/222
test1/xxx/ [000000]none
test1/xxx/222 [000001]#moved_from|test1/xxx/111
~touch test1/xxx/111
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/222
@:test1/xxx/000002 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000002]#created
test1/xxx/222 [000001]#moved_from|test1/xxx/111
~rm test1/xxx/222
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]#modified
~rm test1/xxx/111
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]deleted
notifier -finish test1/xxx
~touch test1/xxx/111
notifier -start test1/xxx
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]none
notifier -suspend test1/xxx
~touch test1/xxx/111
system_variable *test1/xxx*
@:test1/xxx/ [suspended]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]none
notifier -unsuspend test1/xxx
~touch test1/xxx/111
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]#modified
~rm test1/xxx/111
system_variable *test1/xxx*
@:test1/xxx/ [watching]
@:test1/xxx/000000 test1/xxx/
@:test1/xxx/000001 test1/xxx/111
test1/xxx/ [000000]none
test1/xxx/111 [000001]deleted
notifier -finish test1
~touch test1/xxx/0
file_put test1/xxx/0 0
file_info -content 0
[blob] 4bf5122f344554c53bde2ebb8cd2b7e3d1600ad631c385a5d7cce23c7785459a (1 B) [n/a]
file_get 0 test1/xxx/1
file_kill 0
~rmdir test1/xxx
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
file_info -total_items test:5*
4/7
file_info -total_items test:57*
5/7
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
file_info -total_blobs test
7
file_info -total_items test
7
file_info -total_encrypted test
0
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
Error: Invalid cipher/password to decrypt file 'test'.
file_crypt -recrypt -recurse test xxx
Error: Attempt to recrypt recursively when not 'blobs only'.
file_crypt -recrypt -recurse -blobs_only test xxx
Error: Invalid recrypt attempted on encrypted list file 'test'
file_crypt -recurse test abc
file_crypt -recurse -blobs_only test abc
file_info -content test
[list] cabb71470df889b67d18bf7eaf562e2519168d7235d3aa1ed9a126ec3299e6dc (309 B)
8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d ~test.jpg.000000
cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 ~test.jpg.000001
e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 ~test.jpg.000002
54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 ~test.jpg.000003
579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 ~test.jpg.000004
096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 ~test.jpg.000005
15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 ~test.jpg.000006
file_info -total_blobs test
7
file_info -total_items test
7
file_info -total_encrypted test
7
file_crypt -recrypt -recurse -blobs_only test xxx
file_crypt -recrypt -recurse -blobs_only test xxx
file_crypt -recurse -blobs_only test abc
file_info -content test
[list] cabb71470df889b67d18bf7eaf562e2519168d7235d3aa1ed9a126ec3299e6dc (309 B)
8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d ~test.jpg.000000
cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 ~test.jpg.000001
e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 ~test.jpg.000002
54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 ~test.jpg.000003
579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 ~test.jpg.000004
096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 ~test.jpg.000005
15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 ~test.jpg.000006
file_crypt test abc
file_crypt -recrypt test abc
Error: unexpected recrypt has decrypted (same password as when encrypted?)
file_crypt test abc
file_info -content test
[list] cabb71470df889b67d18bf7eaf562e2519168d7235d3aa1ed9a126ec3299e6dc (309 B)
8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d ~test.jpg.000000
cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 ~test.jpg.000001
e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 ~test.jpg.000002
54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 ~test.jpg.000003
579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 ~test.jpg.000004
096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 ~test.jpg.000005
15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 ~test.jpg.000006
file_crypt -encrypt -cipher=cc test abc
file_crypt -decrypt -cipher=xx test abc
Error: unknown stream cipher 'xx'
file_crypt -decrypt -cipher=dh test abc
Error: Invalid cipher/password to decrypt file 'test'.
file_crypt -decrypt -cipher=cc test abc
file_info -content test
[list] cabb71470df889b67d18bf7eaf562e2519168d7235d3aa1ed9a126ec3299e6dc (309 B)
8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d ~test.jpg.000000
cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 ~test.jpg.000001
e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 ~test.jpg.000002
54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 ~test.jpg.000003
579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 ~test.jpg.000004
096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 ~test.jpg.000005
15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 ~test.jpg.000006
file_crypt -encrypt -cipher=dh test abc
file_crypt -decrypt -cipher=cc test abc
Error: Invalid cipher/password to decrypt file 'test'.
file_crypt -decrypt -cipher=dh test abc
file_info -content test
[list] cabb71470df889b67d18bf7eaf562e2519168d7235d3aa1ed9a126ec3299e6dc (309 B)
8f23a8e586d7975095e740da1d43f95cfa057816e1cabddd9e627541a0b6b59d ~test.jpg.000000
cd60cc9598f0831062978f58f3b2f04582c2a2b7efa7876dd03dcd058f2d8b74 ~test.jpg.000001
e60982ce0b124d64f4f8c8cd2ab2b8fd9bd46c1f022aa43f4afd4618bdd056e7 ~test.jpg.000002
54749b4da930e6db6938a0f6393f5fa1c4dba0a148e9c23da10bed11c081b6f5 ~test.jpg.000003
579ad8961e4dc03ebf3965de840ff2eac2b500fde9a144d4f8d67c77ae01e686 ~test.jpg.000004
096b48069f1b3d0dc3a2b650661e6bbf94a5afe3a1c6694745c4505fcee8e1c2 ~test.jpg.000005
15515fff444eb94e0d3e0074f4c772a8bed8ec9a01bc40c691122e812adedea7 ~test.jpg.000006
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
session_variable @dummy_time_stamp 20170313080001
file_put test1.jpg
session_variable @dummy_time_stamp 20170313080002
session_variable @last_file_put
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9
file_put test2.jpg
session_variable @dummy_time_stamp 20170313080000
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
file_info ts.20170313080001
[blob] b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 (5.3 kB)
file_info ts.20170313080002
[blob] efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f (2.8 kB)
file_archives
test1 [okay      ] (6.5 kB/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
file_archive -remove test1
file_archives
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
session_variable @dummy_time_stamp 20170313080003
file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
Error: Unable to retrieve file a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 from archival.
file_archive -add test1 10MiB test1
file_archives
test1 [okay      ] (0 B/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
file_archive -repair test1
file_archives
test1 [okay      ] (6.5 kB/10.5 MB) test1
test2 [okay      ] (0 B/10.5 MB) test2
test3 [okay      ] (0 B/10.5 MB) test3
session_variable @dummy_time_stamp 20170313080003
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
session_variable @dummy_time_stamp 20170313080000
file_retrieve a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753
test1
session_variable @dummy_time_stamp 20170313080001
file_retrieve b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9
test2
file_get a5ab1c26e5253fb7316b51e7f40687183714e0d683034954e1e8fc67bca42753 ~test.jpg
session_variable @dummy_time_stamp 20170313080002
file_put ~test.jpg
file_tags ts.*
ts.20170313080001
ts.20170313080002
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
file_archive -destroy test2
file_archive -destroy test3
file_archive -add test1 10MiB test1
file_archives
test1 [okay      ] (0 B/10.5 MB) test1
file_raw -text blob "Hello World!"
2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8
file_raw -text blob "This is a test."
90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3
file_raw -text list "2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 hello\n90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 test" root
c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1
file_info -recurse -d=2 root
[list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
hello
 [blob] 2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 (13 B) [utf8]
Hello World!
test
 [blob] 90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 (16 B) [utf8]
This is a test.
file_crypt -recurse -blobs_only root password
file_info -recurse -d=2 root
[list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
hello
 [blob] 2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 (13 B) [***]
test
 [blob] 90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 (16 B) [***]
file_relegate -n=2
2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 test1
90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 test1
file_stats
[1/100000]141 B/100.0 GB:1 tag(s)
file_archives
test1 [okay      ] (29 B/10.5 MB) test1
session_variable @blockchain_archive_path test1
file_crypt -recurse -blobs_only root password
file_info -recurse -d=2 root
[list] c158947de2088bcacd73ee2d6c5ca30200f1b4d47d409ea015c13777427a9eb1 (141 B)
hello
 [blob] 2ccdb4c72e6c263e1dc3e5c6617bad479d267546ced55f88d6b6e4527d2e8da8 (13 B) [utf8]
Hello World!
test
 [blob] 90a1a46903f42ddf0386a9c12fd67a6c109285bb8b3117ee83ed222fd0040ad3 (16 B) [utf8]
This is a test.
file_stats
[1/100000]141 B/100.0 GB:1 tag(s)
file_kill -recurse root
file_archive -destroy test1
file_archives
file_archive -add test1 1MB test1
file_archives
test1 [okay      ] (0 B/1.0 MB) test1
file_raw -text blob 999999 test1:xxxx
ce1c6591009b0d60009de10e9b0da944897ea5dd1bc6edb1a930aa0065631c30
file_archives
test1 [okay      ] (1.0 MB/1.0 MB) test1
file_kill xxxx
file_retrieve $FILE_HASH xxxx
test1
file_info xxxx
[blob] ce1c6591009b0d60009de10e9b0da944897ea5dd1bc6edb1a930aa0065631c30 (1.0 MB)
file_kill xxxx
file_raw -text blob "This is a test..." test1:xxxx
ca0fd64a906ba4fe821ca0c290b4777e26ac8f7afc9fa47ba332e97d2cc7654f
file_archives
test1 [okay      ] (18 B/1.0 MB) test1
file_info xxxx
[blob] ca0fd64a906ba4fe821ca0c290b4777e26ac8f7afc9fa47ba332e97d2cc7654f (18 B)
file_kill xxxx
file_raw -text blob 499999 test1:xxxx
107cd8b8d036231f7c3b7bb8df745bd0d98d248a4bfdfba49db9ae1d46398b83
file_archives
test1 [okay      ] (500.0 kB/1.0 MB) test1
file_kill xxxx
file_raw -text blob 499998 test1:xxxx
258831a6dfd1d0b3558763e338cecf65c7b2a5402230c746e4dbf9e5fc84fc46
file_archives
test1 [okay      ] (1.0 MB/1.0 MB) test1
file_kill xxxx
file_retrieve $FILE_HASH
Error: Unable to retrieve file ca0fd64a906ba4fe821ca0c290b4777e26ac8f7afc9fa47ba332e97d2cc7654f from archival.
file_put test.jpg
file_put test1.jpg
file_put test2.jpg
file_archives
test1 [okay      ] (514.6 kB/1.0 MB) test1
file_archive -resize 2MB test1
file_raw -text blob 999999 test1:xyz
ce1c6591009b0d60009de10e9b0da944897ea5dd1bc6edb1a930aa0065631c30
file_kill xyz
file_archives
test1 [okay      ] (1.5 MB/2.0 MB) test1
file_archive -resize 3MB test1
file_archives
test1 [okay      ] (1.5 MB/3.0 MB) test1
file_raw -text blob 999994 test1:xyz
c9f6698797b2bff76c2f74ea38ef63151f759ceddb4be56591c7bea3820f6ae0
file_kill xyz
~ls -tr1 test1/c*
test1/ce1c6591009b0d60009de10e9b0da944897ea5dd1bc6edb1a930aa0065631c30
test1/c9f6698797b2bff76c2f74ea38ef63151f759ceddb4be56591c7bea3820f6ae0
wait 100
file_raw -text blob 999999 test1:xyz
ce1c6591009b0d60009de10e9b0da944897ea5dd1bc6edb1a930aa0065631c30
file_kill xyz
~ls -tr1 test1/c*
test1/c9f6698797b2bff76c2f74ea38ef63151f759ceddb4be56591c7bea3820f6ae0
test1/ce1c6591009b0d60009de10e9b0da944897ea5dd1bc6edb1a930aa0065631c30
file_archive -resize 1MB test1
file_archives
test1 [okay      ] (1.0 MB/1.0 MB) test1
file_archive -destroy test1
session_variable @blockchain_archive_path ""
~rmdir test1
~rmdir test2
~rmdir test3
session_variable @dummy_time_stamp 20170313080010
file_put test1.jpg
session_variable @dummy_time_stamp 20170313080011
file_hash ts.20170313080010
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9
file_touch $OUTPUT
file_hash ts.20170313080011
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9
file_kill $OUTPUT
file_put test1.jpg test1.jpg
file_put test2.jpg test2.jpg
file_raw list test1.jpg,test2.jpg test.1
048bf68a8b49e8679a54154986a9a98bcced3687dbda5ce565531b39b5c21b33
file_info -content test.1
[list] 048bf68a8b49e8679a54154986a9a98bcced3687dbda5ce565531b39b5c21b33 (116 B)
b789eb5b80f6a8fbe9659c8d6ed04222280aa790efb7fe9e972ef8f1ede08cc9 test1.jpg
efeee26ad65084462385b362e873f64fa22cd11b7f1e3d21ba0c3b5e4db8d92f test2.jpg
file_put test.jpg test.jpg
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
file_put 10*test.jpg test
file_stats
[742/100000]38.1 kB/100.0 GB:742 tag(s)
file_kill -recurse test
file_stats
[0/100000]0 B/100.0 GB:0 tag(s)
file_raw -text blob hello hello
cceeb7a985ecc3dabcb4c8f666cd637f16f008e3c963db6aa6f83a7b288c54ef
file_raw -text blob goodbye goodbye
21578622bd4e3ef48ae79c5c6ed2f4e86968ad2fcf06f82a4b1cdbf476152264
file_raw -text list "cceeb7a985ecc3dabcb4c8f666cd637f16f008e3c963db6aa6f83a7b288c54ef hello\ncceeb7a985ecc3dabcb4c8f666cd637f16f008e3c963db6aa6f83a7b288c54ef hello" test
2d6b1cdc3136db447df3b2873b223090ad5952f90153c53da1f92c459ad296f4
file_list -a=goodbye test test old_test
3d9d4f6bc07a8aaf61004949848cd8742e0d5ec9cee5dd9a9289e8de16be89e3
file_info -total_items test
3
file_info -recurse -d=2 test
[list] 3d9d4f6bc07a8aaf61004949848cd8742e0d5ec9cee5dd9a9289e8de16be89e3 (117 B)
hello
 [blob] cceeb7a985ecc3dabcb4c8f666cd637f16f008e3c963db6aa6f83a7b288c54ef (6 B) [utf8]
hello
hello
 [blob] cceeb7a985ecc3dabcb4c8f666cd637f16f008e3c963db6aa6f83a7b288c54ef (6 B) [utf8]
hello
goodbye
 [blob] 21578622bd4e3ef48ae79c5c6ed2f4e86968ad2fcf06f82a4b1cdbf476152264 (8 B) [utf8]
goodbye
file_crypt -recurse test abc
file_info -content test
[list] 3d9d4f6bc07a8aaf61004949848cd8742e0d5ec9cee5dd9a9289e8de16be89e3 (117 B) [***]
file_info -content hello
[blob] cceeb7a985ecc3dabcb4c8f666cd637f16f008e3c963db6aa6f83a7b288c54ef (6 B) [***]
file_info -content goodbye
[blob] 21578622bd4e3ef48ae79c5c6ed2f4e86968ad2fcf06f82a4b1cdbf476152264 (8 B) [***]
file_info -total_items test
0
file_crypt -recurse test abc
file_info -total_items test
3
file_info -recurse -d=2 test
[list] 3d9d4f6bc07a8aaf61004949848cd8742e0d5ec9cee5dd9a9289e8de16be89e3 (117 B)
hello
 [blob] cceeb7a985ecc3dabcb4c8f666cd637f16f008e3c963db6aa6f83a7b288c54ef (6 B) [utf8]
hello
hello
 [blob] cceeb7a985ecc3dabcb4c8f666cd637f16f008e3c963db6aa6f83a7b288c54ef (6 B) [utf8]
hello
goodbye
 [blob] 21578622bd4e3ef48ae79c5c6ed2f4e86968ad2fcf06f82a4b1cdbf476152264 (8 B) [utf8]
goodbye
file_kill test
file_kill old_test
file_kill goodbye
file_kill hello
file_put 88B*pdf_icon.gif xxx
file_info -recurse -d=2 xxx
[list] 54b86b564b22e56ec085e701a70002b9190ec264d486d2544c8e2b693fb14549 (234 B)
pdf_icon.gif.000000
 [blob] b69ff7f3e051d9043be9208828100996cc71c6264d70ce8f221be3d629ed68e2 (88 B) [base64]
R0lGODlhIAAgAMQTAN3d3QAAAN0AAHd3dzMzM/////8zM/9mZszMzLu7u2ZmZhEREf+ZmURERP8zZqqqqpmZmcwzM//MzP///wAAAAAAAAAAAAAAAAAA
pdf_icon.gif.000001
 [blob] 8f5789261725146693475c9a588728d90cdc2e2e7418ae6519f6fe1b5340fd49 (88 B) [base64]
AAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAABMALAAAAAAgACAAAAX/4CSOU2CeaJqS7BgAcCzP8CIEpKrvZi3crp9wSCzefD9caVho/pqFJ1QYGABs
pdf_icon.gif.000002
 [blob] c611034408228e6f9e263a0c32748d0b0fb566ad2584860f3780ea378b8435f0 (88 B) [base64]
1KUwKohyvcXqdUFe4AJMZ/fZVSetgIF8UEVvmWzuUDyr47dqelRwMn5Gh0R8hXQ8jSeEMVUtJXOVlRERczSSLS80MgYHnzCcLJ6jAA4MqKU5qDChrAOT
pdf_icon.gif.000003
 [blob] 2fd27c26d1919b9ec897ff34bd7f3c2d482aab2d7017e5b1c04d39ca6987b3c5 (88 B) [base64]
p6MGBrK0rwyqop+tLq+4ALGbs52oB77Eocq+wCK1MwYMt6HV16LQJaPVyzLeANvSABIOw6gS4semMwgH1K+R7K5xBMQOCfcxCPt99MEANCCQANcDfwD6
pdf_icon.gif.000004
 [blob] ef86dd9946ed6a67b44de7d9cc51eb244c353c0d9c624030e1aee54e7e76785a (87 B) [base64]
/QIYLSEBBRAQKCBwbwCBAfoeAlDQQME6XRAIPMBIoJ++figJL967N24jgYEnVZosSVCBggcfkREEUHJixYsIBnqc2CBnO3mvxjlaynCEpadQLYUAADs=
file_info -total_items xxx
5
file_info -total_encrypted xxx
0
file_crypt b69ff7f3e051d9043be9208828100996cc71c6264d70ce8f221be3d629ed68e2 abc
file_crypt ef86dd9946ed6a67b44de7d9cc51eb244c353c0d9c624030e1aee54e7e76785a abc
file_info -recurse -d=2 xxx
[list] 54b86b564b22e56ec085e701a70002b9190ec264d486d2544c8e2b693fb14549 (234 B)
pdf_icon.gif.000000
 [blob] b69ff7f3e051d9043be9208828100996cc71c6264d70ce8f221be3d629ed68e2 (88 B) [***]
pdf_icon.gif.000001
 [blob] 8f5789261725146693475c9a588728d90cdc2e2e7418ae6519f6fe1b5340fd49 (88 B) [base64]
AAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAABMALAAAAAAgACAAAAX/4CSOU2CeaJqS7BgAcCzP8CIEpKrvZi3crp9wSCzefD9caVho/pqFJ1QYGABs
pdf_icon.gif.000002
 [blob] c611034408228e6f9e263a0c32748d0b0fb566ad2584860f3780ea378b8435f0 (88 B) [base64]
1KUwKohyvcXqdUFe4AJMZ/fZVSetgIF8UEVvmWzuUDyr47dqelRwMn5Gh0R8hXQ8jSeEMVUtJXOVlRERczSSLS80MgYHnzCcLJ6jAA4MqKU5qDChrAOT
pdf_icon.gif.000003
 [blob] 2fd27c26d1919b9ec897ff34bd7f3c2d482aab2d7017e5b1c04d39ca6987b3c5 (88 B) [base64]
p6MGBrK0rwyqop+tLq+4ALGbs52oB77Eocq+wCK1MwYMt6HV16LQJaPVyzLeANvSABIOw6gS4semMwgH1K+R7K5xBMQOCfcxCPt99MEANCCQANcDfwD6
pdf_icon.gif.000004
 [blob] ef86dd9946ed6a67b44de7d9cc51eb244c353c0d9c624030e1aee54e7e76785a (87 B) [***]
file_info -total_items xxx
5
file_info -total_encrypted xxx
2
file_crypt -recurse -blobs_only xxx abc
file_info -recurse -d=2 xxx
[list] 54b86b564b22e56ec085e701a70002b9190ec264d486d2544c8e2b693fb14549 (234 B)
pdf_icon.gif.000000
 [blob] b69ff7f3e051d9043be9208828100996cc71c6264d70ce8f221be3d629ed68e2 (88 B) [base64]
R0lGODlhIAAgAMQTAN3d3QAAAN0AAHd3dzMzM/////8zM/9mZszMzLu7u2ZmZhEREf+ZmURERP8zZqqqqpmZmcwzM//MzP///wAAAAAAAAAAAAAAAAAA
pdf_icon.gif.000001
 [blob] 8f5789261725146693475c9a588728d90cdc2e2e7418ae6519f6fe1b5340fd49 (88 B) [***]
pdf_icon.gif.000002
 [blob] c611034408228e6f9e263a0c32748d0b0fb566ad2584860f3780ea378b8435f0 (88 B) [***]
pdf_icon.gif.000003
 [blob] 2fd27c26d1919b9ec897ff34bd7f3c2d482aab2d7017e5b1c04d39ca6987b3c5 (88 B) [***]
pdf_icon.gif.000004
 [blob] ef86dd9946ed6a67b44de7d9cc51eb244c353c0d9c624030e1aee54e7e76785a (87 B) [base64]
/QIYLSEBBRAQKCBwbwCBAfoeAlDQQME6XRAIPMBIoJ++figJL967N24jgYEnVZosSVCBggcfkREEUHJixYsIBnqc2CBnO3mvxjlaynCEpadQLYUAADs=
file_info -total_items xxx
5
file_info -total_encrypted xxx
3
file_crypt -encrypt -recurse -blobs_only xxx abc
file_info -recurse -d=2 xxx
[list] 54b86b564b22e56ec085e701a70002b9190ec264d486d2544c8e2b693fb14549 (234 B)
pdf_icon.gif.000000
 [blob] b69ff7f3e051d9043be9208828100996cc71c6264d70ce8f221be3d629ed68e2 (88 B) [***]
pdf_icon.gif.000001
 [blob] 8f5789261725146693475c9a588728d90cdc2e2e7418ae6519f6fe1b5340fd49 (88 B) [***]
pdf_icon.gif.000002
 [blob] c611034408228e6f9e263a0c32748d0b0fb566ad2584860f3780ea378b8435f0 (88 B) [***]
pdf_icon.gif.000003
 [blob] 2fd27c26d1919b9ec897ff34bd7f3c2d482aab2d7017e5b1c04d39ca6987b3c5 (88 B) [***]
pdf_icon.gif.000004
 [blob] ef86dd9946ed6a67b44de7d9cc51eb244c353c0d9c624030e1aee54e7e76785a (87 B) [***]
file_info -total_items xxx
5
file_info -total_encrypted xxx
5
file_crypt c611034408228e6f9e263a0c32748d0b0fb566ad2584860f3780ea378b8435f0 abc
file_info -recurse -d=2 xxx
[list] 54b86b564b22e56ec085e701a70002b9190ec264d486d2544c8e2b693fb14549 (234 B)
pdf_icon.gif.000000
 [blob] b69ff7f3e051d9043be9208828100996cc71c6264d70ce8f221be3d629ed68e2 (88 B) [***]
pdf_icon.gif.000001
 [blob] 8f5789261725146693475c9a588728d90cdc2e2e7418ae6519f6fe1b5340fd49 (88 B) [***]
pdf_icon.gif.000002
 [blob] c611034408228e6f9e263a0c32748d0b0fb566ad2584860f3780ea378b8435f0 (88 B) [base64]
1KUwKohyvcXqdUFe4AJMZ/fZVSetgIF8UEVvmWzuUDyr47dqelRwMn5Gh0R8hXQ8jSeEMVUtJXOVlRERczSSLS80MgYHnzCcLJ6jAA4MqKU5qDChrAOT
pdf_icon.gif.000003
 [blob] 2fd27c26d1919b9ec897ff34bd7f3c2d482aab2d7017e5b1c04d39ca6987b3c5 (88 B) [***]
pdf_icon.gif.000004
 [blob] ef86dd9946ed6a67b44de7d9cc51eb244c353c0d9c624030e1aee54e7e76785a (87 B) [***]
file_info -total_items xxx
5
file_info -total_encrypted xxx
4
file_crypt -decrypt -recurse -blobs_only xxx abc
file_info -recurse -d=2 xxx
[list] 54b86b564b22e56ec085e701a70002b9190ec264d486d2544c8e2b693fb14549 (234 B)
pdf_icon.gif.000000
 [blob] b69ff7f3e051d9043be9208828100996cc71c6264d70ce8f221be3d629ed68e2 (88 B) [base64]
R0lGODlhIAAgAMQTAN3d3QAAAN0AAHd3dzMzM/////8zM/9mZszMzLu7u2ZmZhEREf+ZmURERP8zZqqqqpmZmcwzM//MzP///wAAAAAAAAAAAAAAAAAA
pdf_icon.gif.000001
 [blob] 8f5789261725146693475c9a588728d90cdc2e2e7418ae6519f6fe1b5340fd49 (88 B) [base64]
AAAAAAAAAAAAAAAAAAAAAAAAAAAAACH5BAEAABMALAAAAAAgACAAAAX/4CSOU2CeaJqS7BgAcCzP8CIEpKrvZi3crp9wSCzefD9caVho/pqFJ1QYGABs
pdf_icon.gif.000002
 [blob] c611034408228e6f9e263a0c32748d0b0fb566ad2584860f3780ea378b8435f0 (88 B) [base64]
1KUwKohyvcXqdUFe4AJMZ/fZVSetgIF8UEVvmWzuUDyr47dqelRwMn5Gh0R8hXQ8jSeEMVUtJXOVlRERczSSLS80MgYHnzCcLJ6jAA4MqKU5qDChrAOT
pdf_icon.gif.000003
 [blob] 2fd27c26d1919b9ec897ff34bd7f3c2d482aab2d7017e5b1c04d39ca6987b3c5 (88 B) [base64]
p6MGBrK0rwyqop+tLq+4ALGbs52oB77Eocq+wCK1MwYMt6HV16LQJaPVyzLeANvSABIOw6gS4semMwgH1K+R7K5xBMQOCfcxCPt99MEANCCQANcDfwD6
pdf_icon.gif.000004
 [blob] ef86dd9946ed6a67b44de7d9cc51eb244c353c0d9c624030e1aee54e7e76785a (87 B) [base64]
/QIYLSEBBRAQKCBwbwCBAfoeAlDQQME6XRAIPMBIoJ++figJL967N24jgYEnVZosSVCBggcfkREEUHJixYsIBnqc2CBnO3mvxjlaynCEpadQLYUAADs=
file_info -total_items xxx
5
file_info -total_encrypted xxx
0
file_kill -recurse xxx
file_stats
[0/100000]0 B/100.0 GB:0 tag(s)
file_raw -text blob aaa aaa
eea991ff5a78687de5f72eb04ca41d874f25041b3375fcbba3ade8419134e2fa
file_raw -text blob bbb bbb
88bf6d4ba0c23108cd1b83dd3b340d58e0bb2b051a93ec8005d860ff1aef4ed4
file_raw -text blob ccc ccc
82c63533c98e0a4fc3e2ac610841c79cfb3c7cc7e357656b8e6bf2c3af7c1d6d
file_raw -text blob ddd ddd
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6
file_raw -text blob eee eee
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515
file_list -a=aaa,bbb,ccc list0 !list0
ab20291ef2abe6fd181f489b4fcc39170ef97eb028bc776167c8bfa3b34e6539
file_info -content list0
[list] ab20291ef2abe6fd181f489b4fcc39170ef97eb028bc776167c8bfa3b34e6539 (137 B)
eea991ff5a78687de5f72eb04ca41d874f25041b3375fcbba3ade8419134e2fa aaa
88bf6d4ba0c23108cd1b83dd3b340d58e0bb2b051a93ec8005d860ff1aef4ed4 bbb
82c63533c98e0a4fc3e2ac610841c79cfb3c7cc7e357656b8e6bf2c3af7c1d6d ccc
file_list -a=ddd list0 list1
22f2d6ff8c534efdb95bea5a47a296a6fb274e2f28c1e8b40af719b7b3584ede
file_list -a=ddd list1 list2
16289ce092b1d6278521cc4894a77edb42df68aa07c4bfc5e9f4ea3e3e902e6a
file_list -a=ddd list2 list3
58957eaca5b4347d73cd5a87ba7bbb5d1addca4f661f856fbc0e14ea305d123a
file_info -content list3
[list] 58957eaca5b4347d73cd5a87ba7bbb5d1addca4f661f856fbc0e14ea305d123a (179 B)
eea991ff5a78687de5f72eb04ca41d874f25041b3375fcbba3ade8419134e2fa aaa
88bf6d4ba0c23108cd1b83dd3b340d58e0bb2b051a93ec8005d860ff1aef4ed4 bbb
82c63533c98e0a4fc3e2ac610841c79cfb3c7cc7e357656b8e6bf2c3af7c1d6d ccc
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
file_list -a=eee list3 list4
e1bb74504278f2d44608501faffe613136128078e61d6bbcfa1bfb5867ae2f4f
file_list -a=eee list4 list5
dbd441d6231fc98a8ce556b40cbaed5506d07771666f9b249144f74ba853ee24
file_info -content list5
[list] dbd441d6231fc98a8ce556b40cbaed5506d07771666f9b249144f74ba853ee24 (220 B)
eea991ff5a78687de5f72eb04ca41d874f25041b3375fcbba3ade8419134e2fa aaa
88bf6d4ba0c23108cd1b83dd3b340d58e0bb2b051a93ec8005d860ff1aef4ed4 bbb
82c63533c98e0a4fc3e2ac610841c79cfb3c7cc7e357656b8e6bf2c3af7c1d6d ccc
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
file_list -d=ddd list5 list6
80991e18f4b09ed7d62a70a9b5764e1a55a5d49b1ff7ac5078caed89898db479
file_info -content list6
[list] 80991e18f4b09ed7d62a70a9b5764e1a55a5d49b1ff7ac5078caed89898db479 (218 B)
eea991ff5a78687de5f72eb04ca41d874f25041b3375fcbba3ade8419134e2fa aaa
88bf6d4ba0c23108cd1b83dd3b340d58e0bb2b051a93ec8005d860ff1aef4ed4 bbb
82c63533c98e0a4fc3e2ac610841c79cfb3c7cc7e357656b8e6bf2c3af7c1d6d ccc
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
file_list -d=ddd* list5 list7
f36dfc5650ce3533ce46abc7c2b9999db82c8f32c20687cb0f584bdfdb5ee238
file_info -content list7
[list] f36dfc5650ce3533ce46abc7c2b9999db82c8f32c20687cb0f584bdfdb5ee238 (179 B)
eea991ff5a78687de5f72eb04ca41d874f25041b3375fcbba3ade8419134e2fa aaa
88bf6d4ba0c23108cd1b83dd3b340d58e0bb2b051a93ec8005d860ff1aef4ed4 bbb
82c63533c98e0a4fc3e2ac610841c79cfb3c7cc7e357656b8e6bf2c3af7c1d6d ccc
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
file_list -d=ddd*2 list5 list8
3297909a23bc47de0561d8df2843493f9ca468bb6075db740211a06be7077572
file_info -content list8
[list] 3297909a23bc47de0561d8df2843493f9ca468bb6075db740211a06be7077572 (216 B)
eea991ff5a78687de5f72eb04ca41d874f25041b3375fcbba3ade8419134e2fa aaa
88bf6d4ba0c23108cd1b83dd3b340d58e0bb2b051a93ec8005d860ff1aef4ed4 bbb
82c63533c98e0a4fc3e2ac610841c79cfb3c7cc7e357656b8e6bf2c3af7c1d6d ccc
47fc37908a60008788c783fd941b8997f586ec01e659c1381c10d515c2d426f6 ddd
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
file_list -d=bbb,ddd*,eee list5 list9
6f9ef7c7ad1b247fde44e55f039451af5b5c6687255a49e868bbc7d82801d7e7
file_info -content list9
[list] 6f9ef7c7ad1b247fde44e55f039451af5b5c6687255a49e868bbc7d82801d7e7 (139 B)
eea991ff5a78687de5f72eb04ca41d874f25041b3375fcbba3ade8419134e2fa aaa
82c63533c98e0a4fc3e2ac610841c79cfb3c7cc7e357656b8e6bf2c3af7c1d6d ccc
7623bf435855f90d41755d83e5fc61c4ad4f95da4d2d665ebfd0c50c92779515 eee
file_tags
aaa
bbb
ccc
ddd
eee
list0
list1
list2
list3
list4
list5
list6
list7
list8
list9
file_kill -p=list? -x=list1,list3,list5
file_tags
aaa
bbb
ccc
ddd
eee
list1
list3
list5
file_kill -p=l*
file_tags
aaa
bbb
ccc
ddd
eee
file_kill -p=??? -x=ccc
file_tags
ccc
file_kill ccc
file_stats
[0/100000]0 B/100.0 GB:0 tag(s)
file_put 1K*mnemonics.txt x0
file_info -content x0
[list] 52650ffdb011dd2795929ef6a5708d9bae24954f08314759adb609b7e3fa8ac8 (585 B)
51217904b6c322c3c1e194c801deb1f5a44846ff3ca1e19d5abc5628d23576f6 mnemonics.txt.000000
9a9229fe23295a9c84245abbce36dd399d1ed02c844b8b5ac60c2b3f657d55bd mnemonics.txt.000001
39015f5863944b6b7f8110027040efe1216c77740565df132853c6e950f405f2 mnemonics.txt.000002
5004fbebea5d0279ffa57ea0e419d3fc4975170238b11abb79f4e4b3cf7d9fbe mnemonics.txt.000003
3a414659db2c01c3e172fbe8ca5750b24ce432b9a618fc740080ddb5b81e8b90 mnemonics.txt.000004
73a7241a21b14409454a758889771ff50bb72b47ba1d28f9b9e3ff04f1f0b77b mnemonics.txt.000005
e746be32f27f098964c10eff3b45674ddc5721ccd25d36ca439f0e5a7ee36fc1 mnemonics.txt.000006
a19a6671dde69b29a2bfb229f722f8048429d41378726b5c000879358a3a7c59 mnemonics.txt.000007
1d648e4201e4b5c7a8adf6637c321f1c06fd1070885d9b76180e71470ac92278 mnemonics.txt.000008
33d8af39c40b7d504da1b0e0aef90dafb98f61e3c75618a2fe47dcad05e2cbd2 mnemonics.txt.000009
4c05900931dc001b7e5b65fd76ddb1e7179c07e7c9861b046500dc482fa4111b mnemonics.txt.000010
63999badc22bacd803b66c4d555e0129e054e5ea1c4cef8387974aaa51a30698 mnemonics.txt.000011
2d563cb65298840fc4758315e32bf82cabd12a63a905e3731c5c809e9b44290e mnemonics.txt.000012
d24fa1e404a8a0b79e2a641541db5d302472453d9412cdcd57308a57a87d8a20 mnemonics.txt.000013
file_kill -recurse x0
file_put 1K*mnemonics.txt:999 x1
file_info -content x1
[list] d10515ca353b52edbedee388685ac3b88211adeeb7110d37141e06c56f4d45fa (545 B)
9a9229fe23295a9c84245abbce36dd399d1ed02c844b8b5ac60c2b3f657d55bd mnemonics.txt.000000
39015f5863944b6b7f8110027040efe1216c77740565df132853c6e950f405f2 mnemonics.txt.000001
5004fbebea5d0279ffa57ea0e419d3fc4975170238b11abb79f4e4b3cf7d9fbe mnemonics.txt.000002
3a414659db2c01c3e172fbe8ca5750b24ce432b9a618fc740080ddb5b81e8b90 mnemonics.txt.000003
73a7241a21b14409454a758889771ff50bb72b47ba1d28f9b9e3ff04f1f0b77b mnemonics.txt.000004
e746be32f27f098964c10eff3b45674ddc5721ccd25d36ca439f0e5a7ee36fc1 mnemonics.txt.000005
a19a6671dde69b29a2bfb229f722f8048429d41378726b5c000879358a3a7c59 mnemonics.txt.000006
1d648e4201e4b5c7a8adf6637c321f1c06fd1070885d9b76180e71470ac92278 mnemonics.txt.000007
33d8af39c40b7d504da1b0e0aef90dafb98f61e3c75618a2fe47dcad05e2cbd2 mnemonics.txt.000008
4c05900931dc001b7e5b65fd76ddb1e7179c07e7c9861b046500dc482fa4111b mnemonics.txt.000009
63999badc22bacd803b66c4d555e0129e054e5ea1c4cef8387974aaa51a30698 mnemonics.txt.000010
2d563cb65298840fc4758315e32bf82cabd12a63a905e3731c5c809e9b44290e mnemonics.txt.000011
d24fa1e404a8a0b79e2a641541db5d302472453d9412cdcd57308a57a87d8a20 mnemonics.txt.000012
file_kill -recurse x1
file_put 1K*mnemonics.txt:1998 x2
file_info -content x2
[list] a10dcbb9486efa8956147553fe4b0e91f85d123b57af75b50772259aef7c9097 (505 B)
39015f5863944b6b7f8110027040efe1216c77740565df132853c6e950f405f2 mnemonics.txt.000000
5004fbebea5d0279ffa57ea0e419d3fc4975170238b11abb79f4e4b3cf7d9fbe mnemonics.txt.000001
3a414659db2c01c3e172fbe8ca5750b24ce432b9a618fc740080ddb5b81e8b90 mnemonics.txt.000002
73a7241a21b14409454a758889771ff50bb72b47ba1d28f9b9e3ff04f1f0b77b mnemonics.txt.000003
e746be32f27f098964c10eff3b45674ddc5721ccd25d36ca439f0e5a7ee36fc1 mnemonics.txt.000004
a19a6671dde69b29a2bfb229f722f8048429d41378726b5c000879358a3a7c59 mnemonics.txt.000005
1d648e4201e4b5c7a8adf6637c321f1c06fd1070885d9b76180e71470ac92278 mnemonics.txt.000006
33d8af39c40b7d504da1b0e0aef90dafb98f61e3c75618a2fe47dcad05e2cbd2 mnemonics.txt.000007
4c05900931dc001b7e5b65fd76ddb1e7179c07e7c9861b046500dc482fa4111b mnemonics.txt.000008
63999badc22bacd803b66c4d555e0129e054e5ea1c4cef8387974aaa51a30698 mnemonics.txt.000009
2d563cb65298840fc4758315e32bf82cabd12a63a905e3731c5c809e9b44290e mnemonics.txt.000010
d24fa1e404a8a0b79e2a641541db5d302472453d9412cdcd57308a57a87d8a20 mnemonics.txt.000011
file_kill -recurse x2
file_put 1K*mnemonics.txt:999+999 x3
file_info -content x3
[list] 4fe33c235b81dc94c44c904d1afa0ef2a2a029e7fbfc5b8333428e31b6fafcb0 (81 B)
9a9229fe23295a9c84245abbce36dd399d1ed02c844b8b5ac60c2b3f657d55bd mnemonics.txt.000000
file_kill -recurse x3
file_put 1K*mnemonics.txt:999+1998 x4
file_info -content x4
[list] 2d2aa533339f80471447ac72cf748762437eeeed59c14f0019730bd3953d514d (124 B)
9a9229fe23295a9c84245abbce36dd399d1ed02c844b8b5ac60c2b3f657d55bd mnemonics.txt.000000
39015f5863944b6b7f8110027040efe1216c77740565df132853c6e950f405f2 mnemonics.txt.000001
file_kill -recurse x4
file_put 1K*mnemonics.txt:1998+1998 x5
file_info -content x5
[list] f812c8d294f2b9471331fb556793117c1402d7105b29cd4d2d324d93230dc985 (125 B)
39015f5863944b6b7f8110027040efe1216c77740565df132853c6e950f405f2 mnemonics.txt.000000
5004fbebea5d0279ffa57ea0e419d3fc4975170238b11abb79f4e4b3cf7d9fbe mnemonics.txt.000001
file_kill -recurse x5
file_stats
[0/100000]0 B/100.0 GB:0 tag(s)
file_raw blob "test" xxx
938725ef2c126a889e25313269edf12e6efa48d6d11c1bdb3b78110146004858
file_raw blob "test" yyy
938725ef2c126a889e25313269edf12e6efa48d6d11c1bdb3b78110146004858
file_tags $HASH
xxx
yyy
session_variable @skip_tagging_if_exists 1
file_raw blob "test" zzz
938725ef2c126a889e25313269edf12e6efa48d6d11c1bdb3b78110146004858
file_tags $HASH
xxx
yyy
file_kill $HASH
file_stats
[0/100000]0 B/100.0 GB:0 tag(s)

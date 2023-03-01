version
0.1
1MB
session_list -min
1* session_list <none> 127.0.0.1 0:0 0:0 2
encrypt -no_ssl -no_salt for_testing_password
Zm9yX3Rlc3RpbmdfcGFzc3dvcmQ=
X
X.
.X
X
X.
.X
XY
XY.
.XY
X.Y.
XY
.XY
XY.
X.Y.
XYZ
XYZ.
.XYZ.
X.Y.Z.
XYZ
XYZ.
.XYZ.
X.Y.Z.
xxx
yyy
$TSX
$TSY
xxxyyy
$TSXyyy
xxx$yyy
$TSX$TSY
xxx
yyy
%TSX
%TSY
xxxyyy
%TSXyyy
%TSX%TSY
xxx%yyy
%xxx%yyy%
crypto_keys XBT test
1Hri98tpTekszQQTBnKbBrMsecrjik4PX8
02b0dbb9b8c580f2cc7b45aaf10d8353cea7880903e1f07ce996614260c55db4a1
0f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08
addr is: 1Hri98tpTekszQQTBnKbBrMsecrjik4PX8
pubkey is 02b0dbb9b8c580f2cc7b45aaf10d8353cea7880903e1f07ce996614260c55db4a1
privkey is 0f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08
PWD is: abc
PWD now: ***
abcXXX
***XXX
@abc
Meta
Meta
abcdefghi
abcdefgh
abcdefg
defghi
abcdef
def
abc<def>ghi
def
abc
def
ghi
ff00000000000000
00000000000000ff
ff000000
000000ff
4080
4100
4000
1
0
1
2
8
9
0
1
2
8
9
0
0
1
2
ifdef true
ifndef false
ifdef false
ifndef true
1234567890
31323334353637383930
1234567890
c775e7b757ede630cd0aa1113bd102661ab38829ca52a6422ab782862f268646
@ifeq x y false
@ifneq x y true
@ifeq y y true
@ifneq y y false
xxyyzz
xxyyxx
ab/cd/ef
ab:cd:ef
ab/cd/ef
(no history is available)
a
b
c
d
e
1	#a
2	#b
3	#c
4	#d
5	#e
1	#b
2	#c
3	#d
4	#e
1	#b
2	#c
3	#d
b
d
c
b
c
e
c
d
c
d
e
f
g
1	#b
2	#c
3	#d
4	#e
5	#f
6	#g
1	#b
2	#c
3	#d
b
c
d
e
f
g
1	#b
2	#c
3	#d
4	#e
5	#f
6	#g
1	#b
2	#f
3	#g
h
i
j
1	#b
2	#f
3	#g
4	#h
5	#i
6	#j
1	#b
2	#f
depth is 0 at line #250
Looping with aaa
Looping with bbb
Looping with ccc
Looping with ddd
NEXT
XXX YYY
CONT
CONT
CONT
DONE
NEXT
XX YYYY
CONT
CONT
CONT
CONT
DONE
NEXT
X YYYYY
CONT
CONT
CONT
CONT
CONT
DONE
depth is 1 at line #289
END
depth is 1 at line #316
depth is 2 at line #318
depth is 3 at line #320 TRUE
docs
src
src/haru
tests
test.jpg
test1.jpg
test2.jpg
encode -text a
YQ==
decode -text $OUTPUT
a
encode -text ab
YWI=
decode -text $OUTPUT
ab
encode -text abc
YWJj
decode -text $OUTPUT
abc
encode -text abcd
YWJjZA==
decode -text $OUTPUT
abcd
encode -text abcde
YWJjZGU=
decode -text $OUTPUT
abcde
encode -text abcdef
YWJjZGVm
decode -text $OUTPUT
abcdef
encode -text abcdefg
YWJjZGVmZw==
decode -text $OUTPUT
abcdefg
encode -text abcdefg
YWJjZGVmZw==
decode -text $OUTPUT
abcdefg
encode -text abcdefgh
YWJjZGVmZ2g=
decode -text $OUTPUT
abcdefgh
encode -url -text a
YQ
decode -url -text $OUTPUT
a
encode -url -text ab
YWI
decode -url -text $OUTPUT
ab
encode -url -text abc
YWJj
decode -url -text $OUTPUT
abc
encode -url -text abcd
YWJjZA
decode -url -text $OUTPUT
abcd
encode -url -text abcde
YWJjZGU
decode -url -text $OUTPUT
abcde
encode -url -text abcdef
YWJjZGVm
decode -url -text $OUTPUT
abcdef
encode -url -text abcdefg
YWJjZGVmZw
decode -url -text $OUTPUT
abcdefg
encode -url -text abcdefg
YWJjZGVmZw
decode -url -text $OUTPUT
abcdefg
encode -url -text abcdefgh
YWJjZGVmZ2g
decode -url -text $OUTPUT
abcdefgh
crypto_hash hello
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
encode $OUTPUT
LPJNul+wow4m6DsqxbninhsWHlwfp0JecwQzYpOLmCQ=
decode $OUTPUT
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
crypto_hash hello
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
encode -url $OUTPUT
LPJNul-wow4m6DsqxbninhsWHlwfp0JecwQzYpOLmCQ
decode -url $OUTPUT
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
crypto_hash -512 hello
9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043
encode $OUTPUT
m3HSJL1i83hdltRq0+o9czGb+8KJDKra4t/3JRlnPKcjI8PZm6XBHXx6zG4UuMXaDEZjR1wuXDre9G9zvN7AQw==
decode $OUTPUT
9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043
crypto_hash -512 hello
9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043
encode -url $OUTPUT
m3HSJL1i83hdltRq0-o9czGb-8KJDKra4t_3JRlnPKcjI8PZm6XBHXx6zG4UuMXaDEZjR1wuXDre9G9zvN7AQw
decode -url $OUTPUT
9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043
crypto_hash -x=0 hello
2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824
crypto_hash -x=1 hello
ae3967e4aa96162198fb8b1f706cc8565388a7fe0a09d9ef9bae38fd6e77d34d
crypto_hash -x=2 hello
18efea3357c1d3a6e33e3bbe21343d99a42f495ffc25ddd0b71f1f3b2c639108
crypto_hash -x=3 hello
1dfcd445a59f40dedace74f327233b57e28930367a3946fbee06bf2c1c3b3cd9
crypto_hash -x=4 hello
88f860146206257be3d881b4c416ddcb859d7c1b0dde13f2b37cc0edf9d9177d
crypto_hash -x=5 hello
07781cb7e5585194923fe009856c4e5d12a9feb95ae61ace8f0eb8a514c6b896
crypto_hash -x=6 hello
3aeb3e00fb362d0cd8fccea1bfcf0ba1166f1464e08523a7fd7779ed5d25c1db
crypto_hash -x=7 hello
946a604e25667b21d26385a105530fc306bdc2f801b272ab7b6e9a1581548a04
crypto_hash -x=8 hello
e36135196abbc93049e6eacf2f9073ddadb6cb9c4e1e5adcab5c60ed1e40a8f0
crypto_hash -x=9 hello
ba9bd021f5af1c02b93f40cd31949855b080f9cf2ff8110cde95865fdf16d292
crypto_hash -x=9 hello -s=0
3d452bdd2b9bdb760acb5576495e84f3d907b9fcfa451240cbf7a7f03d3c434d
crypto_hash -x=9 hello -s=1
4a5874ab6fc048a0ba69a05814384f5f426e7902e7fa90b8ec51df7e1f93a82f
crypto_hash -x=9 hello -s=2
3989b359e25b503e18e4fc3b49e2099c5c3cf6c980bac3d8b1fa4b200139a6af
crypto_hash -x=9 hello -s=3
559b84a18f1b9e31bf71485a5ed8cba49d20042d950692c7d518cfaedaa63a2f
crypto_hash -x=9 hello -s=4
62aa30bc3e176557d12fb53839a149397928af912a9477f07a8b5b2eb8353bcb
crypto_hash -x=9 hello -s=5
ade14a0f9610a7cea12f1b549679e96fb51f21d7a305c902eb3d142643607a19
session_variable test (okay)
Error: invalid value '(okay)' (could be confused with a protocol response)
session_variable @set aaa
session_variable @set aaa
aaa
session_variable @set bbb
session_variable @set bbb
bbb
session_variable @set ccc
session_variable @set ccc
ccc
session_variable @set aaa
aaa
session_variable @set bbb
bbb
session_variable @set ccc
ccc
session_variable @set
aaa
bbb
ccc
session_variable @deque @set
session_variable @deque
aaa
bbb
ccc
session_variable @deque @set
session_variable @deque
aaa
bbb
ccc
aaa
bbb
ccc
session_variable @deque "push_back yyy"
session_variable @deque "push_front xxx"
session_variable @deque
xxx
aaa
bbb
ccc
aaa
bbb
ccc
yyy
session_variable @deque pop_back
session_variable @deque pop_front
session_variable @deque
aaa
bbb
ccc
aaa
bbb
ccc
session_variable @deque "push_front eee"
session_variable @deque "push_front ddd"
session_variable @deque
ddd
eee
aaa
bbb
ccc
aaa
bbb
ccc
session_variable @set @deque
session_variable @set
aaa
bbb
ccc
ddd
eee
session_variable @queue_test abc
session_variable @queue_test def
session_variable @queue_test ghi
session_variable @queue_test
abc
session_variable @queue_test
def
session_variable @queue_test
ghi
session_variable @queue_test
session_variable @mapped_test:abc xxx
session_variable @mapped_test:def yyy
session_variable @mapped_test:ghi zzz
session_variable @mapped_test:abc
xxx
session_variable @mapped_test:def
yyy
session_variable @mapped_test:ghi
zzz
session_variable @mapped_test:abc 111
session_variable @mapped_test:def 222
session_variable @mapped_test:ghi 333
session_variable @mapped_test
abc 111
def 222
ghi 333
session_variable @mapped_test:abc
111
session_variable @mapped_test:def
222
session_variable @mapped_test:ghi
333
session_variable @mapped_test:abc ""
session_variable @mapped_test:abc
session_variable @mapped_test:def
222
session_variable @mapped_test:ghi
333
session_variable @mapped_test
def 222
ghi 333
session_variable @mapped_test:def ""
session_variable @mapped_test:abc
session_variable @mapped_test:def
session_variable @mapped_test:ghi
333
session_variable @mapped_test
ghi 333
session_variable @mapped_test ""
session_variable @mapped_test:abc
session_variable @mapped_test:def
session_variable @mapped_test:ghi
arg $0 is "test_args.cin"
arg $1 is "XXX"
arg $2 is ""
arg $3 is ""
arg $0 is "test_args.cin"
arg $1 is "XXX"
arg $2 is "YYY"
arg $3 is ""
arg $0 is "test_args.cin"
arg $1 is "XXX"
arg $2 is "YYY"
arg $3 is "ZZZ"
arg $0 is "test_args.cin"
arg $1 is "XXX"
arg $2 is ""
arg $3 is "ZZZ"
arg $0 is "test_args.cin"
arg $1 is ""
arg $2 is ""
arg $3 is "ZZZ"
utc_to_local AEST "2013-10-05 15:00"
2013-10-06 01:00 AEST
utc_to_local AEST+ "2013-10-05 15:00"
2013-10-06 01:00 AEST
utc_from_local AEST "2013-10-06 01:00"
2013-10-05 15:00
utc_to_local AEST "2013-10-06 16:00"
2013-10-07 02:00 AEST
utc_to_local AEST+ "2013-10-06 16:00"
2013-10-07 03:00 AEDT
utc_from_local AEST "2013-10-07 03:00"
2013-10-06 17:00
utc_from_local AEDT "2013-10-07 03:00"
2013-10-06 16:00
utc_to_local AEST "2014-04-05 15:00"
2014-04-06 01:00 AEST
utc_to_local AEST+ "2014-04-05 15:00"
2014-04-06 02:00 AEDT
utc_from_local AEST "2014-04-06 02:00"
2014-04-05 16:00
utc_from_local AEDT "2014-04-06 02:00"
2014-04-05 15:00
utc_to_local AEST "2014-04-05 16:00"
2014-04-06 02:00 AEST
utc_to_local AEST+ "2014-04-05 16:00"
2014-04-06 02:00 AEST
utc_from_local AEST "2014-04-06 02:00"
2014-04-05 16:00
utc_from_unix_time 0
1970-01-01 00:00:00
utc_to_unix_time "1970-01-01 00:00:00"
0
utc_from_unix_time 631152000
1990-01-01 00:00:00
utc_to_unix_time "1990-01-01 00:00:00"
631152000
utc_from_unix_time 946684800
2000-01-01 00:00:00
utc_to_unix_time "2000-01-01 00:00:00"
946684800
utc_from_unix_time 1000000000
2001-09-09 01:46:40
utc_to_unix_time "2001-09-09 01:46:40"
1000000000
utc_from_unix_time 1320195610
2011-11-02 01:00:10
utc_to_unix_time "2011-11-02 01:00:10"
1320195610

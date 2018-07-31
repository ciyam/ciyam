
> version
0.1

> session_list -min
1* session_list <none> <unknown> 0:0 0:0

> encrypt -no_ssl -no_salt for_testing_password
Zm9yX3Rlc3RpbmdfcGFzc3dvcmQ=

> 
> 
> 
> X

> X.

> .X

> X

> X.

> .X

> XY

> XY.

> .XY

> X.Y.

> XY

> .XY

> XY.

> X.Y.

> XYZ

> XYZ.

> .XYZ.

> X.Y.Z.

> XYZ

> XYZ.

> .XYZ.

> X.Y.Z.

> crypto_keys XBT test
1Hri98tpTekszQQTBnKbBrMsecrjik4PX8
02b0dbb9b8c580f2cc7b45aaf10d8353cea7880903e1f07ce996614260c55db4a1
0f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08

> 
> 
> 
> addr is: 1Hri98tpTekszQQTBnKbBrMsecrjik4PX8

> pubkey is 02b0dbb9b8c580f2cc7b45aaf10d8353cea7880903e1f07ce996614260c55db4a1

> privkey is 0f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08

> 
> abcdefghi

> 
> defghi

> 
> abcdef

> 
> def

> 
> abc<def>ghi

> 
> def

> 
> 
> ff00000000000000

> 
> 00000000000000ff

> 
> ff000000

> 
> 000000ff

> 
> 4080

> 
> 4100

> 
> 4000

> 
> 1

> 
> ifdef true

> 
> 
> 
> 
> 
> 
> ifndef false

> 
> 
> 
> 
> 
> ifdef false

> 
> 
> ifndef true

> 
> 
> 
> 
> 1234567890

> 
> 31323334353637383930

> 
> 1234567890

> 
> c775e7b757ede630cd0aa1113bd102661ab38829ca52a6422ab782862f268646

> 
> 
> 
> 
> 
> @ifeq x y false

> 
> 
> @ifneq x y true

> 
> 
> 
> 
> 
> @ifeq y y true

> 
> 
> 
> 
> 
> 
> @ifneq y y false

> 
> 
> xxyyzz

> 
> xxyyxx

> 
> (no history is available)

> a

> b

> c

> d

> e

> 1	#a
2	#b
3	#c
4	#d
5	#e

> 
> 1	#b
2	#c
3	#d
4	#e

> 
> 1	#b
2	#c
3	#d

> b

> d

> c

> b
c

> e

> c
d

> c
d
e

> f

> g

> 1	#b
2	#c
3	#d
4	#e
5	#f
6	#g

> 
> 1	#b
2	#c
3	#d

> b
c
d

> e

> f

> g

> 1	#b
2	#c
3	#d
4	#e
5	#f
6	#g

> 
> 1	#b
2	#f
3	#g

> h

> i

> j

> 1	#b
2	#f
3	#g
4	#h
5	#i
6	#j

> 
> 1	#b
2	#f

> 
> 
> 
> 
> 
> 
> Looping with aaa

> 
> 
> 
> 
> 
> Looping with bbb

> 
> 
> 
> 
> Looping with ccc

> 
> 
> 
> 
> Looping with ddd

> 
> 
> 
> session_variable @set aaa

> session_variable @set aaa
aaa

> session_variable @set bbb

> session_variable @set bbb
bbb

> session_variable @set ccc

> session_variable @set ccc
ccc

> session_variable @set aaa
aaa

> session_variable @set bbb
bbb

> session_variable @set ccc
ccc

> session_variable @set
aaa
bbb
ccc

> 
> utc_to_local AEST "2013-10-05 15:00"
2013-10-06 01:00 AEST

> utc_to_local AEST+ "2013-10-05 15:00"
2013-10-06 01:00 AEST

> utc_from_local AEST "2013-10-06 01:00"
2013-10-05 15:00

> utc_to_local AEST "2013-10-06 16:00"
2013-10-07 02:00 AEST

> utc_to_local AEST+ "2013-10-06 16:00"
2013-10-07 03:00 AEDT

> utc_from_local AEST "2013-10-07 03:00"
2013-10-06 17:00

> utc_from_local AEDT "2013-10-07 03:00"
2013-10-06 16:00

> utc_to_local AEST "2014-04-05 15:00"
2014-04-06 01:00 AEST

> utc_to_local AEST+ "2014-04-05 15:00"
2014-04-06 02:00 AEDT

> utc_from_local AEST "2014-04-06 02:00"
2014-04-05 16:00

> utc_from_local AEDT "2014-04-06 02:00"
2014-04-05 15:00

> utc_to_local AEST "2014-04-05 16:00"
2014-04-06 02:00 AEST

> utc_to_local AEST+ "2014-04-05 16:00"
2014-04-06 02:00 AEST

> utc_from_local AEST "2014-04-06 02:00"
2014-04-05 16:00

> 
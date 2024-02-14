session_variable @progress_total 100000
session_variable @progress_count 0
session_variable @progress_value
0%
session_variable @progress_count 1
session_variable @progress_value
0.001%
session_variable @progress_count 5
session_variable @progress_value
0.005%
session_variable @progress_count 10
session_variable @progress_value
0.010%
session_variable @progress_count 15
session_variable @progress_value
0.015%
session_variable @progress_count 55
session_variable @progress_value
0.05%
session_variable @progress_count 100
session_variable @progress_value
0.10%
session_variable @progress_count 200
session_variable @progress_value
0.2%
session_variable @progress_count 250
session_variable @progress_value
0.25%
session_variable @progress_count 275
session_variable @progress_value
0.27%
session_variable @progress_count 276
session_variable @progress_value
0.276%
session_variable @progress_count 277
session_variable @progress_value
0.277%
session_variable @progress_count 280
session_variable @progress_value
0.280%
session_variable @progress_count 290
session_variable @progress_value
0.29%
session_variable @progress_count 300
session_variable @progress_value
0.30%
session_variable @progress_count 350
session_variable @progress_value
0.35%
session_variable @progress_count 400
session_variable @progress_value
0.40%
session_variable @progress_count 500
session_variable @progress_value
0.5%
session_variable @progress_count 1000
session_variable @progress_value
1.0%
session_variable @progress_count 2000
session_variable @progress_value
2%
session_variable @progress_count 3000
session_variable @progress_value
3%
session_variable @progress_count 5000
session_variable @progress_value
5%
session_variable @progress_count 99000
session_variable @progress_value
99%
session_variable @progress_count 99010
session_variable @progress_value
99.0%
session_variable @progress_count 99020
session_variable @progress_value
99.02%
session_variable @progress_count 99025
session_variable @progress_value
99.025%
session_variable @progress_count 99050
session_variable @progress_value
99.05%
session_variable @progress_count 99100
session_variable @progress_value
99.10%
session_variable @progress_count 99200
session_variable @progress_value
99.2%
session_variable @progress_count 99500
session_variable @progress_value
99.5%
session_variable @progress_count 99900
session_variable @progress_value
99.9%
session_variable @progress_count 99990
session_variable @progress_value
99.99%
session_variable @progress_count 99999
session_variable @progress_value
99.999%
session_variable @progress_count 100000
session_variable @progress_value
100%
session_variable @progress_*
@progress_count 100000
@progress_fracs 3
@progress_prior 100000
@progress_total 100000
@progress_value 100%
session_variable @progress_clear
session_variable @progress_*
@progress_count 0
@progress_fracs 0
@progress_prior 0
@progress_total 0
@progress_value 100%
system_variable @queue_test abc
system_variable @queue_test def
system_variable @queue_test ghi
system_variable @queue_test
abc
system_variable @queue_test
def
system_variable @queue_test
ghi
system_variable @queue_test
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
system_variable @sys_var_prefix test
system_variable >test_a aaa
system_variable >test_b bbb
system_variable >test_c ccc
system_variable ?test_a
1
system_variable !test_a
system_variable ?test_x
system_variable !test_x
1
system_variable ?test_a|?test_b
1
system_variable ?test_a|?test_x
1
system_variable ?test_a&?test_b
1
system_variable ?test_a&?test_x
system_variable ?test_a&!test_x
1
system_variable !test_a|!test_b
system_variable !test_a|!test_x
1
system_variable !test_a&!test_x
system_variable *
test_a aaa
test_b bbb
test_c ccc
system_variable **
test_c ccc
test_b bbb
test_a aaa
system_variable >
test_a aaa
test_b bbb
test_c ccc
system_variable <
system_variable test_a ddd
system_variable *
test_a ddd
test_b bbb
test_c ccc
system_variable **
test_c ccc
test_b bbb
test_a ddd
system_variable >
test_a aaa
test_b bbb
test_c ccc
system_variable <
test_a aaa
system_variable test_b eee
system_variable *
test_a ddd
test_b eee
test_c ccc
system_variable >
test_a aaa
test_b bbb
test_c ccc
system_variable <
test_a aaa
test_b bbb
system_variable test_c fff
system_variable *
test_a ddd
test_b eee
test_c fff
system_variable **
test_c fff
test_b eee
test_a ddd
system_variable >
test_a aaa
test_b bbb
test_c ccc
system_variable <
test_a aaa
test_b bbb
test_c ccc
system_variable <test_a
system_variable *
test_a aaa
test_b eee
test_c fff
system_variable >
test_a aaa
test_b bbb
test_c ccc
system_variable <
test_b bbb
test_c ccc
system_variable <*
system_variable *
test_a aaa
test_b bbb
test_c ccc
system_variable >
test_a aaa
test_b bbb
test_c ccc
system_variable <
system_variable test_a ""
system_variable test_b ""
system_variable test_c ""
system_variable *
system_variable >
test_a aaa
test_b bbb
test_c ccc
system_variable <
test_a aaa
test_b bbb
test_c ccc
system_variable >*
system_variable *
system_variable >
system_variable <
system_variable @sys_var_prefix ""
system_variable test_a aaa
system_variable "test a" aaa
system_variable test_b bbb
system_variable "test b" bbb
system_variable test_c ccc
system_variable "test c" ccc
system_variable test*
"test a" aaa
"test b" bbb
"test c" ccc
test_a aaa
test_b bbb
test_c ccc
system_variable "\"abc\"" abc
Error: invalid system variable name '"abc"'
system_variable xxx yyy ""
1
system_variable xxx
yyy
system_variable xxx zzz ""
0
system_variable xxx
yyy
system_variable xxx zzz yyy
1
system_variable xxx
zzz
system_variable xxx "" yyy
0
system_variable xxx
zzz
system_variable xxx "" zzz
1
system_variable xxx
session_restrict abc "session_variable"
system_variable xxx
Error: command 'system_variable' is not currently permitted
session_variable xxx
session_restrict def ""
Error: incorrect restriction key value
system_variable xxx
Error: command 'system_variable' is not currently permitted
session_variable xxx
session_restrict abc ""
session_variable xxx
system_variable xxx

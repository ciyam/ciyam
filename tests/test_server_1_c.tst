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


> system_variable @sys_var_prefix test

> system_variable >test_a aaa

> system_variable >test_b bbb

> system_variable >test_c ccc

> system_variable ?test_a
1

> system_variable !test_a

> system_variable ?test_x

> system_variable !test_x
1

> system_variable ?test_a|?test_b
1

> system_variable ?test_a|?test_x
1

> system_variable ?test_a&?test_b
1

> system_variable ?test_a&?test_x

> system_variable ?test_a&!test_x
1

> system_variable !test_a|!test_b

> system_variable !test_a|!test_x
1

> system_variable !test_a&!test_x

> system_variable *
test_a aaa
test_b bbb
test_c ccc

> system_variable >
test_a aaa
test_b bbb
test_c ccc

> system_variable <

> system_variable test_a ddd

> system_variable *
test_a ddd
test_b bbb
test_c ccc

> system_variable >
test_a aaa
test_b bbb
test_c ccc

> system_variable <
test_a aaa

> system_variable test_b eee

> system_variable *
test_a ddd
test_b eee
test_c ccc

> system_variable >
test_a aaa
test_b bbb
test_c ccc

> system_variable <
test_a aaa
test_b bbb

> system_variable test_c fff

> system_variable *
test_a ddd
test_b eee
test_c fff

> system_variable >
test_a aaa
test_b bbb
test_c ccc

> system_variable <
test_a aaa
test_b bbb
test_c ccc

> system_variable <test_a

> system_variable *
test_a aaa
test_b eee
test_c fff

> system_variable >
test_a aaa
test_b bbb
test_c ccc

> system_variable <
test_b bbb
test_c ccc

> system_variable <*

> system_variable *
test_a aaa
test_b bbb
test_c ccc

> system_variable >
test_a aaa
test_b bbb
test_c ccc

> system_variable <

> system_variable test_a ""

> system_variable test_b ""

> system_variable test_c ""

> system_variable *

> system_variable >
test_a aaa
test_b bbb
test_c ccc

> system_variable <
test_a aaa
test_b bbb
test_c ccc

> system_variable >*

> system_variable *

> system_variable >

> system_variable <

> system_variable @sys_var_prefix ""

> 
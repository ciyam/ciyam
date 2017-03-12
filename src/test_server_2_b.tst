
> module_load Meta

> object_create Meta User
1

> object_execute 1 set "Description \"Test Description\""
okay

> object_validate 1
Error: Password must not be empty.

> object_execute 1 set "Password \"Test Password\""
okay

> object_validate 1
Error: User Id must not be empty.

> object_execute 1 set "User_Id \"Test User_Id\""
okay

> object_validate 1

> object_execute 1 get Description
Test Description

> object_execute 1 get User_Id
Test User_Id

> object_execute 1 get Password
Test Password

> object_variable 1 test1 "first value"

> object_variable 1 test2 "second value"

> object_variable 1 test1
first value

> object_variable 1 test2
second value

> object_create Meta Workgroup
2

> object_create Meta User
3

> object_create Meta Application
4

> object_create Meta Workgroup
5

> object_execute 4 set "Name \"Test App Name\""
okay

> object_variable 4 test_var "One"

> object_execute 4 Test_Proc_1
One Test App Name

> object_variable 4 test_var "Two"

> session_variable test_var "Here"

> object_execute 4 "Test_Proc_2 \"Is Over\""
Two Test App Name Is Over Here

> session_variable test_var "Found"

> object_execute 4 "Test_Proc_2 \"Is Soon\""
Two Test App Name Is Soon Found

> session_list -min
5* session_list <none> <unknown> 5:5 0:0

> object_list
1 Meta User
2 Meta Workgroup
3 Meta User
4 Meta Application
5 Meta Workgroup

> object_destroy 1

> object_list
2 Meta Workgroup
3 Meta User
4 Meta Application
5 Meta Workgroup

> object_destroy 2

> object_destroy 3

> object_destroy 4

> object_destroy 5

> object_list

> module_unload Meta

> 
session_variable @errors_only 1
run_script !test "@one=aaa"
Error: Script 'test' missing argument '@two'.
run_script !test "@one=aaa,@two=bbb"
system_variable @test
aaa bbb
run_script !test "@one=aaa,@two=bbb,@opt1=ccc"
system_variable @test
aaa bbb ccc
run_script !test "@one=aaa,@two=bbb,@opt1=ccc,@opt2=ddd"
system_variable @test
aaa bbb ccc ddd
run_script !test "@arg1=aaa"
Error: Script 'test' missing argument '@two'.
run_script !test "@arg1=aaa,@arg2=bbb"
system_variable @test
aaa bbb
run_script !test "@arg1=aaa,@arg2=bbb,@arg3=ccc"
system_variable @test
aaa bbb ccc
run_script !test "@arg1=aaa,@arg2=bbb,@arg3=ccc,@arg4=ddd"
system_variable @test
aaa bbb ccc ddd
run_script !test "@one=aaa,@arg2=bbb"
system_variable @test
aaa bbb
run_script !test "@arg1=aaa,@two=bbb"
system_variable @test
aaa bbb
run_script !test "@one=aaa,@arg2=bbb,@opt1=ccc"
system_variable @test
aaa bbb ccc
run_script !test "@arg1=aaa,@two=bbb,@arg3=ccc"
system_variable @test
aaa bbb ccc
run_script !test "@one=aaa,@arg2=bbb,@opt1=ccc,@arg4=ddd"
system_variable @test
aaa bbb ccc ddd
run_script !test "@arg1=aaa,@two=bbb,@arg3=ccc,@opt2=ddd"
system_variable @test
aaa bbb ccc ddd
run_script test*
test @one @two opt @opt1 @opt2
test1 [ *** busy *** ]
test2 [ *** busy *** ]
test3 [ *** busy *** ]
test4
run_script test1
run_script test2
Error: Script 'test2' appears to be busy.
wait 250
run_script test3
run_script test4
wait 250
(started test1)
(finished test1)
(started test3)
(started test4)
(finished test4)
(finished test3)
run_script test2
run_script test1
Error: Script 'test1' appears to be busy.
wait 250
run_script test3
run_script test4
wait 250
(started test1)
(finished test1)
(started test3)
(started test4)
(finished test4)
(finished test3)
(started test2)
(finished test2)
(started test3)
(started test4)
(finished test4)
(finished test3)
run_script test3
Error: Script 'test3' appears to be busy.
run_script test4
wait 250
(started test1)
(finished test1)
(started test3)
(started test4)
(finished test4)
(finished test3)
(started test2)
(finished test2)
(started test3)
(started test4)
(finished test4)
(finished test3)
(started test4)
(finished test4)
run_script test3
run_script test4
wait 250
(started test1)
(finished test1)
(started test3)
(started test4)
(finished test4)
(finished test3)
(started test2)
(finished test2)
(started test3)
(started test4)
(finished test4)
(finished test3)
(started test4)
(finished test4)
(started test3)
(started test4)
(finished test4)
(finished test3)
quit

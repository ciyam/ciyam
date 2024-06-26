run_script test*
test1 [ *** busy *** ]
test2 [ *** busy *** ]
test3 [ *** busy *** ]
test4
run_script test1
run_script test2
Error: script 'test2' appears to be busy
run_script test3
Error: script 'test3' appears to be busy
run_script test4
wait 350
(started test1)
(started test4)
(finished test4)
(finished test1)
run_script test2
run_script test1
Error: script 'test1' appears to be busy
run_script test3
Error: script 'test3' appears to be busy
run_script test4
wait 350
(started test1)
(started test4)
(finished test4)
(finished test1)
(started test2)
(started test4)
(finished test4)
(finished test2)
run_script test3
Error: script 'test3' appears to be busy
run_script test4
wait 350
(started test1)
(started test4)
(finished test4)
(finished test1)
(started test2)
(started test4)
(finished test4)
(finished test2)
(started test4)
(finished test4)
run_script test3
run_script test4
wait 350
(started test1)
(started test4)
(finished test4)
(finished test1)
(started test2)
(started test4)
(finished test4)
(finished test2)
(started test4)
(finished test4)
(started test3)
(started test4)
(finished test4)
(finished test3)
quit

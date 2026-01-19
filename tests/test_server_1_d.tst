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
system_variable @test xxx
system_variable @test
xxx
system_variable @test @null
system_variable @test
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
system_variable @trace_filters test1,test2,test3
system_variable @trace_filters
test1,test2,test3
system_variable @trace_filters -test2
system_variable @trace_filters
test1,test3
system_variable @trace_filters -test1
system_variable @trace_filters
test3
system_variable @trace_filters +test1,+test2
system_variable @trace_filters
test1,test2,test3
system_variable @trace_filters ""
system_variable @trace_filters
system_variable @trace_session_ids 1,2,3,4,5
system_variable @trace_session_ids
1,2,3,4,5
system_variable @trace_session_ids -
system_variable @trace_session_ids
1,2,3,5
system_variable @trace_session_ids +
system_variable @trace_session_ids
1,2,3,4,5
system_variable @trace_session_ids 2
system_variable @trace_session_ids
2
system_variable @trace_session_ids +1,+3
system_variable @trace_session_ids
1,2,3
system_variable @trace_session_ids -1,-3
system_variable @trace_session_ids
2
system_variable @trace_session_ids ""
system_variable @trace_session_ids
system_variable *needed*
system_variable @backup_needed 1
system_variable *needed*
@backup_needed 1
system_variable @restore_needed 1
system_variable *needed*
@restore_needed 1
system_variable @prepare_backup_needed 1
system_variable *needed*
@restore_needed 1
system_variable @complete_restore_needed 1
system_variable *needed*
@complete_restore_needed 1
system_variable @backup_needed 1
system_variable *needed*
@complete_restore_needed 1
system_variable @restore_needed 1
system_variable *needed*
@complete_restore_needed 1
system_variable @prepare_backup_needed 1
system_variable *needed*
@complete_restore_needed 1
system_variable @complete_restore_needed ""
system_variable @prepare_backup_needed 1
system_variable *needed*
@prepare_backup_needed 1
system_variable @backup_needed 1
system_variable *needed*
@prepare_backup_needed 1
system_variable @restore_needed 1
system_variable *needed*
@restore_needed 1
system_variable @complete_restore_needed 1
system_variable *needed*
@complete_restore_needed 1
system_variable @complete_restore_needed ""
system_variable *needed*
system_peerchain_info -list
system_peerchain_create -auto=0 abc111cba 111.domain.com "Peerchain 1"
system_peerchain_info -list
abc111cba
system_peerchain_info abc111cba
<sio/>
 <auto_start>0
 <description>Peerchain 1
 <extra_value>
 <host_name>111.domain.com
 <host_port>12021
 <local_port>12021
 <num_helpers>5
 <peer_type>0
 <shared_secret>
</sio>
system_peerchain_info abc111cba host_name
111.domain.com
system_peerchain_info abc111cba description
Peerchain 1
system_peerchain_update abc111cba "Peerchain 111"
system_peerchain_info abc111cba host_name
111.domain.com
system_peerchain_info abc111cba description
Peerchain 111
system_peerchain_create -auto=0 abc222cba 222.domain.com "Peerchain 2"
system_peerchain_create -auto=0 abc333cba 333.domain.com "Peerchain 3"
system_peerchain_info -list
abc111cba
abc222cba
abc333cba
system_peerchain_info abc222cba
<sio/>
 <auto_start>0
 <description>Peerchain 2
 <extra_value>
 <host_name>222.domain.com
 <host_port>12021
 <local_port>12021
 <num_helpers>5
 <peer_type>0
 <shared_secret>
</sio>
system_peerchain_update -num=9 abc222cba
system_peerchain_info abc222cba host_name
222.domain.com
system_peerchain_info abc222cba num_helpers
9
system_peerchain_info abc333cba
<sio/>
 <auto_start>0
 <description>Peerchain 3
 <extra_value>
 <host_name>333.domain.com
 <host_port>12021
 <local_port>12021
 <num_helpers>5
 <peer_type>0
 <shared_secret>
</sio>
system_peerchain_update abc333cba "Peerchain 333"
system_peerchain_info abc333cba
<sio/>
 <auto_start>0
 <description>Peerchain 333
 <extra_value>
 <host_name>333.domain.com
 <host_port>12021
 <local_port>12021
 <num_helpers>5
 <peer_type>0
 <shared_secret>
</sio>
system_peerchain_destroy abc222cba
system_peerchain_info -list
abc111cba
abc333cba
system_peerchain_destroy abc111cba
system_peerchain_info -list
abc333cba
system_peerchain_destroy abc333cba
system_peerchain_info -list
quit

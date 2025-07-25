storage_init ciyam
session_variable @dump_minimal 1
storage_bulk_start read
storage_bulk_start read
Error: storage is already bulk locked for read by this session
storage_bulk_start write
Error: storage is already bulk locked for read by this session
storage_bulk_finish
storage_bulk_start write
storage_bulk_start write
Error: storage is already bulk locked for write by this session
storage_bulk_start read
Error: storage is already bulk locked for write by this session
storage_bulk_finish
storage_lock_create view test
1
storage_lock_create view test
2
storage_lock_create view test
3
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            view      
storage_lock_modify 2 review
storage_lock_modify 3 review
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            review    
3        test:                                            review    
storage_lock_modify 3 obtain
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            review    
3        test:                                            obtain    
storage_lock_modify 2 obtain
Error: unable to transform lock due to locking conflict
storage_lock_modify 3 update
Error: unable to transform lock due to locking conflict
storage_lock_modify 2 view
storage_lock_modify 3 update
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            update    
storage_lock_modify 2 review
Error: unable to transform lock due to locking conflict
storage_lock_modify 3 obtain
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            obtain    
storage_lock_modify 2 review
storage_lock_modify 3 destroy
Error: unable to transform lock due to locking conflict
storage_lock_modify 2 view
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            obtain    
storage_lock_modify 3 destroy
storage_lock_modify 2 review
Error: unable to transform lock due to locking conflict
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            destroy   
storage_lock_modify 3 obtain
storage_lock_create review test abc
4
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            obtain    
4        test:abc                                         review    
storage_lock_modify 3 update
Error: unable to transform lock due to locking conflict
storage_lock_modify 4 view
storage_lock_modify 3 update
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            update    
4        test:abc                                         view      
storage_lock_modify 4 review
Error: unable to transform lock due to locking conflict
storage_lock_create review test def
Error: unable to obtain lock due to locking conflict
storage_lock_modify 3 obtain
storage_lock_create review test def
5
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            obtain    
4        test:abc                                         view      
5        test:def                                         review    
storage_lock_modify 4 review
storage_lock_modify 4 update
Error: unable to transform lock due to locking conflict
storage_lock_modify 3 view
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            view      
4        test:abc                                         review    
5        test:def                                         review    
storage_lock_modify 4 update
storage_lock_modify 5 update
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            view      
4        test:abc                                         update    
5        test:def                                         update    
storage_lock_modify 3 obtain
Error: unable to transform lock due to locking conflict
storage_lock_modify 4 review
storage_lock_modify 3 obtain
Error: unable to transform lock due to locking conflict
storage_lock_modify 5 review
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            view      
4        test:abc                                         review    
5        test:def                                         review    
storage_lock_modify 3 obtain
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            obtain    
4        test:abc                                         review    
5        test:def                                         review    
storage_lock_modify 3 update
Error: unable to transform lock due to locking conflict
storage_lock_remove 5
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            obtain    
4        test:abc                                         review    
storage_lock_modify 3 update
Error: unable to transform lock due to locking conflict
storage_lock_remove 4
storage_lock_modify 3 update
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            view      
3        test:                                            update    
storage_lock_modify 2 update
Error: unable to transform lock due to locking conflict
storage_lock_remove 3
storage_lock_modify 2 update
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            view      
2        test:                                            update    
storage_lock_modify 1 obtain
Error: unable to transform lock due to locking conflict
storage_lock_modify 2 review
storage_lock_modify 1 obtain
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
1        test:                                            obtain    
2        test:                                            review    
storage_lock_remove 1
storage_lock_modify 2 destroy
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
2        test:                                            destroy   
storage_lock_modify 2 create
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
2        test:                                            create    
storage_lock_create link test
Error: unable to obtain lock due to locking conflict
storage_lock_modify 2 destroy
storage_lock_create link test
Error: unable to obtain lock due to locking conflict
storage_lock_modify 2 update
storage_lock_create link test
6
storage_lock_create link test
7
storage_lock_modify 2 destroy
Error: unable to transform lock due to locking conflict
storage_lock_remove 7
storage_lock_modify 2 destroy
Error: unable to transform lock due to locking conflict
storage_lock_remove 6
storage_lock_modify 2 destroy
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
2        test:                                            destroy   
storage_lock_modify 2 view
storage_dump_locks
handle   key (class_id:instance)                          type
-------- ------------------------------------------------ ----------
2        test:                                            view      
storage_lock_remove 2
storage_variable >test1 aaa
storage_variable test2 bbb
storage_variable >test3 ccc
storage_variable test4 ddd
storage_variable >test5 eee
storage_variable >
test1 aaa
test3 ccc
test5 eee
storage_variable <
storage_variable test1 xxx
storage_variable >
test1 aaa
test3 ccc
test5 eee
storage_variable <
test1 aaa
storage_variable test3 yyy
storage_variable >
test1 aaa
test3 ccc
test5 eee
storage_variable <
test1 aaa
test3 ccc
storage_variable test5 zzz
storage_variable >
test1 aaa
test3 ccc
test5 eee
storage_variable <
test1 aaa
test3 ccc
test5 eee
storage_variable >test?
test1 xxx
test3 yyy
test5 zzz
storage_variable >
test1 xxx
test3 yyy
test5 zzz
storage_variable <
storage_variable *
test1 xxx
test2 bbb
test3 yyy
test4 ddd
test5 zzz
storage_variable test1 aaa
storage_variable test3 ccc
storage_variable test5 eee
storage_variable *
test1 aaa
test2 bbb
test3 ccc
test4 ddd
test5 eee
storage_variable <
test1 xxx
test3 yyy
test5 zzz
storage_variable >test?
test1 aaa
test3 ccc
test5 eee
storage_variable >
test1 aaa
test3 ccc
test5 eee
storage_variable <
storage_variable *
test1 aaa
test2 bbb
test3 ccc
test4 ddd
test5 eee
session_variable @attached_file_path .
object_create 100 100100
1
object_op_update 1 guest
object_execute 1 get Workgroup
guests
object_execute 1 set "Workgroup crap"
Error: Workgroup record 'crap' was not found.
object_execute 1 set "Workgroup ?crap"
okay
object_execute 1 get Workgroup
guests
object_op_cancel 1
object_destroy 1
pf Meta Enum_Item "" Label,@order,@level,Enum,Enum.Name,Enum.@group,@owner
[guests_primitive00001 =1.0 259 100:104100] string,guests_primitive00001,,guests_primitive,primitive,guests,
[guests_primitive00002 =1.0 259 100:104100] datetime,guests_primitive00002,,guests_primitive,primitive,guests,
[guests_primitive00003 =1.0 259 100:104100] date,guests_primitive00003,,guests_primitive,primitive,guests,
[guests_primitive00004 =1.0 259 100:104100] time,guests_primitive00004,,guests_primitive,primitive,guests,
[guests_primitive00005 =1.0 259 100:104100] numeric,guests_primitive00005,,guests_primitive,primitive,guests,
[guests_primitive00006 =1.0 259 100:104100] int,guests_primitive00006,,guests_primitive,primitive,guests,
[guests_primitive00007 =1.0 259 100:104100] bool,guests_primitive00007,,guests_primitive,primitive,guests,
[primitive0000000001 =1.0 259 100:104100] string,primitive0000000001,,primitive,primitive,,
[primitive0000000002 =1.0 259 100:104100] datetime,primitive0000000002,,primitive,primitive,,
[primitive0000000003 =1.0 259 100:104100] date,primitive0000000003,,primitive,primitive,,
[primitive0000000004 =1.0 259 100:104100] time,primitive0000000004,,primitive,primitive,,
[primitive0000000005 =1.0 259 100:104100] numeric,primitive0000000005,,primitive,primitive,,
[primitive0000000006 =1.0 259 100:104100] int,primitive0000000006,,primitive,primitive,,
[primitive0000000007 =1.0 259 100:104100] bool,primitive0000000007,,primitive,primitive,,
pc guest 20120102 100 105100 guest_model "105101=Sample,300500=guests,105102=0.1,105103=2012,105104=M001,105112=0,105118=0"
guest_model
pf 100 105100 guest_model "105101,105102,105103,105104,105105,105106,105107,105108,105109,105110,105111,105112,105113,105114,105118"
[guest_model =1.0 768 100:105100] Sample,0.1,2012,M001,M001C100,M001S1000,M001L100,M001V100,,,,2,0,,0
pc guest 20120102 100 136100 guest_standard "136101=Standard,136107=Standard,302810=Standard,302800=guest_model"
guest_standard
pe guest 20120102 100 136100 "-v=@async=false,@message=Installing Standard Package..." guest_standard =1.0 -136410
pc guest 20120102 100 136100 guest_user "136101=User,136107=User,302810=User,302800=guest_model"
guest_user
pe guest 20120102 100 136100 "-v=@async=false,@message=Installing User Package..." guest_user =1.0 -136410
pf 100 106100 "106104 " 106101,106102,106104 -min
System_Information,System_Information,M001C100
User,User,M001C101
Peer_Node,Peer_Nodes,M001C102
Peer_Node_Item,Peer_Node_Items,M001C103
Peer_Node_User,Peer_Node_User,M001C104
Peer_Node_Data,Peer_Node_Data,M001C105
Contact,Contacts,M001C106
Contact_Group_Member,Contact_Group_Members,M001C107
Core_Peer,Core_Peers,M001C108
pf 100 107100 "107110 " 107101,107110 -min
Name,M001C100F100
Not_Backup_Or_Restore_Needed,M001C100F101
Not_Executing_Procedure,M001C100F102
Prepare_Backup_Needed,M001C100F103
Prepare_in_Progress,M001C100F104
Variable_Expression,M001C100F105
Expression_Results,M001C100F106
Restore_In_Progress,M001C100F107
Restore_Needed,M001C100F108
Status,M001C100F109
Raw_Sync_Status,M001C100F110
System_Checksum,M001C100F111
Complete_Restore_Needed,M001C100F112
Raw_Sync_Status_Has_Ellipsis,M001C100F113
Sync_Status,M001C100F114
Vendor,M001C100F115
Identity,M001C100F116
Message,M001C100F117
Actions,M001C100F118
Backup,M001C100F119
Backup_Details,M001C100F120
Backup_File_Name,M001C100F121
Backup_In_Progress,M001C100F122
Backup_Needed,M001C100F123
Backup_Or_Restore_Needed,M001C100F124
Blockchain_Backup_Height,M001C100F125
Blockchain_Hashes,M001C100F126
Check_Is_Connected,M001C100F127
Check_Is_Connecting,M001C100F128
Check_Is_Waiting,M001C100F129
Check_Preparing_Backup,M001C100F130
Check_Preparing_Restore,M001C100F131
Check_Is_Disconnecting,M001C100F132
Dummy_0,M001C100F133
Dummy_1,M001C100F134
Dummy_Asterisk,M001C100F135
Dummy_Connected,M001C100F136
Dummy_Connecting,M001C100F137
Dummy_Disconnecting,M001C100F138
Dummy_Hash,M001C100F139
Dummy_Preparing_Backup,M001C100F140
Dummy_Preparing_Restore,M001C100F141
Dummy_Tilda,M001C100F142
Dummy_Waiting,M001C100F143
Ellipsis,M001C100F144
Error,M001C100F145
Executing_Procedure,M001C100F146
Exists,M001C100F147
Hash_Prefixed_Identity,M001C100F148
Initial_Backup_Hash,M001C100F149
Initial_Hub_Hash,M001C100F150
Initial_Shared_Hash,M001C100F151
Tilda_Prefixed_Identity,M001C100F152
Dummy_Self,M001C101F100
Name,M001C101F101
Username,M001C101F102
User_Password_Change,M001C101F103
Actions,M001C101F104
Active,M001C101F105
Change_Password_Time,M001C101F106
Storage,M001C101F107
Submit_File_Prefix,M001C101F108
Submit_File_Suffix,M001C101F109
Unique,M001C101F110
Unix_Timestamp,M001C101F111
User_Hash,M001C101F112
Password,M001C101F113
Password_Change_After,M001C101F114
Password_Hash,M001C101F115
Class_Id,M001C101F116
Created_On,M001C101F117
Handle,M001C101F118
Dummy_0,M001C101F119
Dummy_1,M001C101F120
Dummy_Bool_1,M001C101F121
Email,M001C101F122
Email_Failure,M001C101F123
Email_Address,M001C101F124
Has_Changed_Handle,M001C101F125
Has_Password,M001C101F126
Hide_Email_Address,M001C101F127
Is_In_Restore,M001C101F128
Is_Initial_Record,M001C101F129
Not_Is_In_Restore,M001C101F130
Not_Is_Initial_Record,M001C101F131
Ntfy_Topic_Name,M001C101F132
Unix_Now,M001C101F133
Permissions,M001C101F134
Reset_Password,M001C101F135
Rescue_Password,M001C101F136
Is_Self_Or_Admin,M001C101F137
Key,M001C101F138
Must_Change_Password,M001C101F139
No_Email,M001C101F140
Read_Access_Only,M001C101F141
Primary_External_Contact,M001C102F100
Primary_External_User,M001C102F101
Current_User,M001C102F102
Owner,M001C102F103
Actions,M001C102F104
Active,M001C102F105
Active_And_New,M001C102F106
Active_And_Not_Activated,M001C102F107
Active_And_Not_New,M001C102F108
Active_Not_New_And_Ext_Contact,M001C102F109
Active_Not_New_And_Verified,M001C102F110
Alias,M001C102F111
Alias_And_Identity,M001C102F112
Backup_Identity,M001C102F113
Channel_Information,M001C102F114
Closed,M001C102F115
Owner_Active_And_New,M001C102F116
Owner_Active_And_Received,M001C102F117
Owner_Active_Not_Received,M001C102F118
Owner_And_Verified_Checksums,M001C102F119
Owner_Is_Current_User,M001C102F120
Owner_Key,M001C102F121
Owner_Needs_To_Check_Peer,M001C102F122
Peer_Information,M001C102F123
Peer_Node_Files_Area,M001C102F124
Peer_Node_Items,M001C102F125
Closing,M001C102F126
Identity,M001C102F127
Marked,M001C102F128
Not_Connecting,M001C102F129
Not_Dummy_Domain,M001C102F130
Not_Dummy_Owner_Active_And_New,M001C102F131
Null,M001C102F132
Opened,M001C102F133
Opened_Files,M001C102F134
Opened_User_Name,M001C102F135
Opening,M001C102F136
Description,M001C102F137
Disconnecting,M001C102F138
Disconnecting_Chain_Id,M001C102F139
Dummy_1,M001C102F140
Dummy_Domain,M001C102F141
Dummy_Domain_Name,M001C102F142
Dummy_Int_1,M001C102F143
Dummy_tilda,M001C102F144
EC_Public_Key,M001C102F145
Dummy_Owner_Active_Not_Received,M001C102F146
Dummy_Owner_Active_Received,M001C102F147
Dummy_null,M001C102F148
Error,M001C102F149
Files_Approved,M001C102F150
Files_To_Submit,M001C102F151
Files_Waiting,M001C102F152
Unverified_Checksums,M001C102F153
Unverified_Checksums_And_Not_New,M001C102F154
User_Is_Primary_External,M001C102F155
Verified_Checksums,M001C102F156
Key,M001C102F157
Lock_Files_Pending,M001C102F158
Height_Fetched,M001C102F159
Height_Received,M001C102F160
Height_Submitted,M001C102F161
Host_And_Port_Info,M001C102F162
Files_Pending,M001C102F163
Files_Pending_Locked,M001C102F164
Files_Received,M001C102F165
Files_Reviewing,M001C102F166
Host_Name,M001C102F167
Hub_Identity,M001C102F168
Core_Peer_Type,M001C102F169
Port_Number,M001C102F170
Primary_External_Contact_Found,M001C102F171
Protocol_Handler,M001C102F172
QR_Code,M001C102F173
QR_Code_Scanned,M001C102F174
QR_Code_Text,M001C102F175
Receive_Chain_Id,M001C102F176
Receive_Error,M001C102F177
Receive_Checksum,M001C102F178
Receive_Initial_Block_Hash,M001C102F179
Receive_Syncing,M001C102F180
Shared_Identity,M001C102F181
Shared_Secret,M001C102F182
Status,M001C102F183
Submit_Chain_Id,M001C102F184
Submit_Checksum,M001C102F185
Submit_Error,M001C102F186
Submit_File_Name,M001C102F187
Submit_File_Name_Exists,M001C102F188
Submit_File_Prefix,M001C102F189
Submit_File_Suffix,M001C102F190
Connect_Info_1,M001C102F191
Connect_Info_2,M001C102F192
Connect_Information,M001C102F193
Connected,M001C102F194
Connecting,M001C102F195
Connection_Busy,M001C102F196
Connected_Or_Busy,M001C102F197
Submit_Initial_Block_Hash,M001C102F198
Submit_Syncing,M001C102F199
Syncing,M001C102F200
Unlock_Files_Pending,M001C102F201
Current_User_Name,M001C102F202
Current_Username,M001C102F203
Demo_Blockchain,M001C102F204
Demo_Blockchain_0,M001C102F205
Demo_Blockchain_1,M001C102F206
Current_Uid,M001C102F207
Peer_Node,M001C103F100
Original_Name,M001C103F101
Submitting,M001C103F102
Item_Name,M001C103F103
Item_Size_Info,M001C103F104
Name_And_Size,M001C103F105
Opened_Directory,M001C103F106
Identity_Directory,M001C103F107
Identity_Path,M001C103F108
Ignoring,M001C103F109
A0,M001C103F110
A1,M001C103F111
A2,M001C103F112
Actions,M001C103F113
Change,M001C103F114
File,M001C103F115
Folder,M001C103F116
Peer_Node,M001C104F100
User,M001C104F101
Active,M001C104F102
Address,M001C104F103
Peer_Node_Alias,M001C104F104
Dummy_1,M001C104F105
External,M001C104F106
Identity,M001C104F107
Is_Not_Primary_External_User,M001C104F108
Is_Not_Primary_Peer_User,M001C104F109
Is_Primary_External_User,M001C104F110
Is_Primary_Peer_User,M001C104F111
Key,M001C104F112
Local,M001C104F113
Name,M001C104F114
Peer_Node_Identity,M001C104F115
Primary_External_User_Key,M001C104F116
Primary_Peer_User_Key,M001C104F117
Submit_File_Name,M001C104F118
Peer_Node,M001C105F100
Identity,M001C105F101
Identity_And_Type,M001C105F102
Identity_Null,M001C105F103
Included_Datachain,M001C105F104
Storage,M001C105F105
Actions,M001C105F106
Current_Uid,M001C105F107
Type,M001C105F108
Description,M001C105F109
Has_Data_Key_Variable,M001C105F110
Name,M001C105F111
Owner_Key,M001C105F112
Peer_Node_Not_Dummy,M001C105F113
Member,M001C106F100
Peer_Node,M001C106F101
Peer_Node_User,M001C106F102
Local_User,M001C106F103
Owner,M001C106F104
Active,M001C106F105
Address,M001C106F106
Creating_For_Local_User,M001C106F107
Creating_For_Peerchain_User,M001C106F108
Dummy_1,M001C106F109
Group,M001C106F110
Local,M001C106F111
Local_Identity,M001C106F112
Type,M001C106F113
Local_User_Name,M001C106F114
Member_Keys,M001C106F115
Members,M001C106F116
Group_Contact_Address,M001C106F117
Group_Contact_Key,M001C106F118
Group_Member_Keys,M001C106F119
Name,M001C106F120
Non_Group,M001C106F121
Identity,M001C106F122
Key,M001C106F123
Not_Local,M001C106F124
Peer_Node_Alias,M001C106F125
Peer_Node_Identity,M001C106F126
Peer_Node_User_Key,M001C106F127
Contact,M001C107F100
Group,M001C107F101
Actions,M001C108F100
Active,M001C108F101
Auto_Reconnect,M001C108F102
Auto_Reconnect_Off,M001C108F103
Auto_Reconnect_Secs,M001C108F104
Auto_Reconnect_Value,M001C108F105
Backup_Checksum,M001C108F106
Peer_Node_Checksum,M001C108F107
Blockchain_Hashes,M001C108F108
Chain_Id,M001C108F109
Combined_Checksum,M001C108F110
Import_Needed,M001C108F111
Shared_Details,M001C108F112
Remote_Identity,M001C108F113
Importing_For_Chain_Id,M001C108F114
Inactive,M001C108F115
Description,M001C108F116
Dummy_0,M001C108F117
Dummy_1,M001C108F118
Dummy_Host_Name,M001C108F119
Error,M001C108F120
Export_In_Progress,M001C108F121
Export_Needed,M001C108F122
Exporting_For_Chain_Id,M001C108F123
Host_And_Port_Info,M001C108F124
Core_Peer_Lot,M001C108F125
Initial_Backup_Hash,M001C108F126
Initial_Hub_Hash,M001C108F127
Initial_Shared_Hash,M001C108F128
Local_Backup_Checksum,M001C108F129
Local_Combined_Checksum,M001C108F130
Local_Hub_Checksum,M001C108F131
Local_Shared_Checksum,M001C108F132
Next_Peer_Lot,M001C108F133
Host_Name,M001C108F134
Hub_Checksum,M001C108F135
Identity,M001C108F136
Identity_For_Peer,M001C108F137
Identity_From_Peer,M001C108F138
Identity_And_Lot,M001C108F139
Core_Peer_Type,M001C108F140
Port_Number,M001C108F141
Private_Identity,M001C108F142
QR_Code,M001C108F143
QR_Code_Scanned,M001C108F144
QR_Code_Text,M001C108F145
Reconnect_Pending,M001C108F146
Shared_Checksum,M001C108F147
Shared_Secret,M001C108F148
Shared_Secret_Hash,M001C108F149
Connection_Type,M001C108F150
Status,M001C108F151
Status_,M001C108F152
Sync_Status,M001C108F153
System_Secret,M001C108F154
pf 100 103100 "103104 " 103101,103104 -min
primitive,E000
primitive,E000
Peer_Status,E100
System_Status,E101
Currency,E102
Day_Type,E103
Customary_Event,E104
Month,E105
Day_Of_Month,E106
Day_Number,E107
Day_Set,E108
Date_Cycle,E109
Day_Of_Week,E110
Minutes,E111
Occurrence,E112
Orientation,E113
Peer_Connection,E114
Core_Peer_Type,E115
Datachain_Type,E116
Peer_Node_Status,E117
Change_Password_Seconds,E118
Change_Type,E119
Contact_Type,E120
pf 100 102100 "102111 " 102101,102111 -min
string,T000
string,T000
datetime,T001
datetime,T001
date,T002
date,T002
time,T003
time,T003
numeric,T004
numeric,T004
int,T005
int,T005
bool,T006
bool,T006
foreign_key,T007
foreign_key,T007
bytes,T100
str001,T101
str002,T102
str003,T103
str004,T104
str005,T105
str010,T106
str015,T107
str020,T108
str025,T109
str030,T110
str040,T111
str050,T112
str060,T113
str080,T114
str100,T115
str125,T116
str150,T117
str200,T118
year,T119
duration,T120
filename,T121
0..10,T122
1..10,T123
0..100,T124
1..100,T125
0..1000,T126
1..1000,T127
0..10000,T128
1..10000,T129
0..100000,T130
1..100000,T131
std_date,T132
std_time,T133
std_datetime,T134
std_timestamp,T135
std_numeric,T136
std_percentage,T137
std_money,T138
std_money_amount,T139
std_crypto,T140
std_crypto_amount,T141

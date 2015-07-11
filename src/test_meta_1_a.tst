
> storage_init ciyam

> session_variable @attached_file_path .

> perform_create guest 20120102 100 105100 guest_model "105101=Sample,300500=guests,105102=0.1,105103=2012,105104=M001,105112=0"
guest_model

> perform_fetch 100 105100 guest_model "105101,105102,105103,105104,105105,105106,105107,105108,105109,105110,105111,105112,105113,105114"
[guest_model =1.0 512 100:105100] Sample,0.1,2012,M001,M001C100,M001S1000,M001L100,M001V100,,,,2,0,

> perform_create guest 20120102 100 136100 guest_standard "136101=Standard,136107=Standard,302810=standard,302800=guest_model"
guest_standard

> perform_execute guest 20120102 100 136100 "-v=@async=false,@message=Installing Standard Package..." guest_standard =1.0 -136410
Installing Standard Package...

> perform_create guest 20120102 100 136100 guest_user "136101=User,136107=User,302810=user,302800=guest_model"
guest_user

> perform_execute guest 20120102 100 136100 "-v=@async=false,@message=Installing User Package..." guest_user =1.0 -136410
Installing User Package...

> perform_fetch Meta Class "Id " Name,Plural,Id -min
System_Information,System_Information,M001C100
User,User,M001C101

> perform_fetch Meta Field "Id " Name,Id -min
Name,M001C100F100
Vendor,M001C100F101
Reference,M001C100F102
Message,M001C100F103
Actions,M001C100F104
User_Id,M001C101F100
User_Id_or_Name,M001C101F101
User_Name,M001C101F102
Actions,M001C101F103
Active,M001C101F104
Template,M001C101F105
Unique,M001C101F106
User_Hash,M001C101F107
Password,M001C101F108
Password_Hash,M001C101F109
Class_Id,M001C101F110
Command_Hash,M001C101F111
Created_On,M001C101F112
Description,M001C101F113
Email,M001C101F114
Email_Failure,M001C101F115
Email_Address,M001C101F116
Has_Password,M001C101F117
Hide_Email_Address,M001C101F118
Is_In_Restore,M001C101F119
Not_Is_In_Restore,M001C101F120
Permissions,M001C101F121
Is_Self_Or_Admin,M001C101F122
Key,M001C101F123
No_Email,M001C101F124
No_Email_Padding,M001C101F125
Dummy_Self,M001C101F126

> perform_fetch Meta Enum "Id " Name,Id -min
primitive,E000
primitive,E000
Currency,E100
Day_Type,E101
Customary_Event,E102
Month,E103
Day_Of_Month,E104
Day_Number,E105
Day_Set,E106
Date_Cycle,E107
Day_Of_Week,E108
Minutes,E109
Occurrence,E110
Orientation,E111

> perform_fetch Meta Type "Id " Name,Id -min
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
std_cash_amount,T139
std_crypto_amount,T140

> 
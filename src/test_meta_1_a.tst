
> storage_init ciyam

> session_variable @attached_file_path .
.

> perform_create guest 20120102 100 105100 guest_model "105101=Sample,300500=guests,105102=0.1,105103=2012,105104=M001,105112=0"
guest_model

> perform_fetch 100 105100 guest_model "105101,105102,105103,105104,105105,105106,105107,105108,105109,105110,105111,105112,105113,105114"
[guest_model =1.0 256 100:105100] Sample,0.1,2012,M001,M001C100,M001S1000,M001L100,M001V100,,,,2,0,

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
User_Name,M001C101F101
Actions,M001C101F102
Active,M001C101F103
Template,M001C101F104
User_Hash,M001C101F105
Password,M001C101F106
Password_Hash,M001C101F107
Class_Id,M001C101F108
Created_On,M001C101F109
Description,M001C101F110
Email,M001C101F111
Email_Failure,M001C101F112
Email_Address,M001C101F113
Has_Password,M001C101F114
Hide_Email_Address,M001C101F115
Permissions,M001C101F116
Is_Self_Or_Admin,M001C101F117
Key,M001C101F118
Dummy_Self,M001C101F119

> perform_fetch Meta Enum "Id " Name,Id -min
primitive,E000
primitive,E000
Day_Type,E100
Customary_Event,E101
Month,E102
Day_Of_Month,E103
Day_Number,E104
Day_Set,E105
Date_Cycle,E106
Day_Of_Week,E107
Occurrence,E108
Orientation,E109

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
str010,T101
str030,T102
str100,T103
str200,T104
year,T105
duration,T106
filename,T107
0..100,T108
std_date,T109
std_time,T110
std_datetime,T111
std_timestamp,T112
std_numeric,T113
std_percentage,T114

> 

> storage_init ciyam

> session_variable @attached_file_path .
.

> perform_create guest 20120102 100 105100 guest_model "105101=Sample,300500=guests,105102=0.1,105103=2012,105104=M001,105112=0"
guest_model

> perform_fetch 100 105100 guest_model "105101,105102,105103,105104,105105,105106,105107,105108,105109,105110,105111,105112,105113,105114"
[guest_model =1.0 256 100:105100] Sample,0.1,2012,M001,M001C100,M001S100,M001L100,M001V100,,,,2,0,

> perform_create guest 20120102 100 136100 guest_standard "136101=Standard,136107=Standard,302810=standard,302800=guest_model"
guest_standard

> perform_execute guest 20120102 100 136100 "-v=@async=false,@message=Installing Standard Package..." guest_standard =1.0 -136410
Installing Standard Package...

> perform_create guest 20120102 100 136100 guest_user "136101=User,136107=User,302810=user,302800=guest_model"
guest_user

> perform_execute guest 20120102 100 136100 "-v=@async=false,@message=Installing User Package..." guest_user =1.0 -136410
Installing User Package...

> perform_fetch Meta Class "" Name,Plural,Id -min
System,System,M001C100
User,User,M001C101

> perform_fetch Meta Field "" Name,Id -min
Name,M001C100F100
Vendor,M001C100F101
Reference,M001C100F102
Message,M001C100F103
Actions,M001C100F104
User_Id,M001C101F100
Active,M001C101F101
Template,M001C101F102
Password,M001C101F103
Description,M001C101F104
Email,M001C101F105
Permissions,M001C101F106

> perform_fetch Meta Enum "" Name,Id -min
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
primitive,E000
primitive,E000

> perform_fetch Meta Type "" Name,Id -min
bytes,T100
str010,T101
str030,T102
str100,T103
year,T104
duration,T105
filename,T106
0..100,T107
std_date,T108
std_datetime,T110
std_time,T109
bool,T006
date,T002
datetime,T001
foreign_key,T007
bool,T006
date,T002
datetime,T001
foreign_key,T007
int,T005
numeric,T004
string,T000
time,T003
int,T005
numeric,T004
string,T000
time,T003

> 
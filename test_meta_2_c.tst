
> storage_init ciyam

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
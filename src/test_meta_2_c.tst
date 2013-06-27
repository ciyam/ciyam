
> storage_init ciyam

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
No_Email,M001C101F119
No_Email_Padding,M001C101F120
Dummy_Self,M001C101F121

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
Minutes,E108
Occurrence,E109
Orientation,E110

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
str020,T102
str030,T103
str050,T104
str100,T105
str200,T106
year,T107
duration,T108
filename,T109
0..10,T110
0..100,T111
std_date,T112
std_time,T113
std_datetime,T114
std_timestamp,T115
std_numeric,T116
std_percentage,T117

> 
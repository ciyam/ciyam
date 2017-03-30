
> module_load Meta

> module_list
100 Meta

> module_class_list Meta
100100 User 0
101100 Workgroup 0
102100 Type 0
103100 Enum 0
104100 Enum_Item 0
105100 Model 0
106100 Class 0
107100 Field 0
108100 Index 0
109100 Modifier 0
110100 Modifier_Affect 0
111100 Procedure 0
112100 Procedure_Arg 0
113100 Relationship 0
114100 Specification_Type 0
115100 Specification 0
116100 Permission 0
117100 View_Type 0
118100 View 0
119100 View_Field_Type 0
120100 View_Field 0
121100 List_Type 0
122100 List 0
123100 List_Field_Type 0
124100 List_Field 0
125100 Auto_Code 0
126100 Module 0
127100 Application 0
128100 Specification_Field_Action 0
129100 Initial_Record 0
130100 Initial_Record_Value 0
135100 Package_Type 0
136100 Package 0
137100 Package_Option 0
138100 Application_Script 0
139100 Global_Archive 0
140100 Global_Blacklist_Entry 0

> module_class_field_list Meta User
100102 Active bool normal
100104 Description string normal
100105 Email string normal
100103 Password string normal
100108 Password_Hash string normal
100106 Permissions string normal
100107 User_Hash string normal
100101 User_Id string normal
300100 Workgroup 101100:Meta_Workgroup normal

> module_class_field_list Meta Workgroup
101102 Id string normal
101101 Name string normal
101104 Next_Enum_Id string normal
101103 Next_Model_Id string normal
101106 Next_Permission_Id string normal
101105 Next_Type_Id string normal
302835 Standard_Package 136100:Meta_Package normal

> module_unload Meta

> module_list

> 
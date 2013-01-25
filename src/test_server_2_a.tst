
> module_load Meta

> module_list
100 Meta

> module_class_list Meta
100100 User
101100 Workgroup
102100 Type
103100 Enum
104100 Enum_Item
105100 Model
106100 Class
107100 Field
108100 Index
109100 Modifier
110100 Modifier_Affect
111100 Procedure
112100 Procedure_Arg
113100 Relationship
114100 Specification_Type
115100 Specification
116100 Permission
117100 View_Type
118100 View
119100 View_Field_Type
120100 View_Field
121100 List_Type
122100 List
123100 List_Field_Type
124100 List_Field
125100 Auto_Code
126100 Module
127100 Application
128100 Specification_Field_Action
129100 Initial_Record
130100 Initial_Record_Value
135100 Package_Type
136100 Package
137100 Package_Option

> module_class_field_list Meta User
100102 Active bool
100104 Description string
100105 Email string
100103 Password string
100108 Password_Hash string
100106 Permissions string
100107 User_Hash string
100101 User_Id string
300100 Workgroup 101100:Meta_Workgroup

> module_class_field_list Meta Workgroup
101102 Id string
101101 Name string
101104 Next_Enum_Id string
101103 Next_Model_Id string
101106 Next_Permission_Id string
101105 Next_Type_Id string
302835 Standard_Package 136100:Meta_Package

> module_unload Meta

> module_list

> 
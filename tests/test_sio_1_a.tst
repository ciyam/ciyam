
> load Meta.fcgi.sio

> sections
00000 enums
00001 views
00002 lists

> sections enums.enum
00000 values

> attribute enums.enum.values.*.data,name
data=0,name=enum_add_packages_none
data=1,name=enum_add_packages_standard
data=2,name=enum_add_packages_standard_user
data=3,name=enum_add_packages_standard_user_group

> output enums.enum.values
<value/>
 <data>0
 <name>enum_add_packages_none
</value>
<value/>
 <data>1
 <name>enum_add_packages_standard
</value>
<value/>
 <data>2
 <name>enum_add_packages_standard_user
</value>
<value/>
 <data>3
 <name>enum_add_packages_standard_user_group
</value>

> output -json enums.enum.values
{ "values":
[
 { "value":
 [
  { "data": "0" },
  { "name": "enum_add_packages_none" }
 ]
 },
 { "value":
 [
  { "data": "1" },
  { "name": "enum_add_packages_standard" }
 ]
 },
 { "value":
 [
  { "data": "2" },
  { "name": "enum_add_packages_standard_user" }
 ]
 },
 { "value":
 [
  { "data": "3" },
  { "name": "enum_add_packages_standard_user_group" }
 ]
 }
]
}
> output lists.id=602900
<id>602900
<cid>127100
<name>602900_name
<type>group
<extra>no_limit
<actions>
<modifiers>
<view>602910
<class>Application
<bclass>
<order>forward
<pclass>101100
<pfield>302220
<module>Meta
<columns/>
 <column/>
  <name>field_application_name
  <field>127101
  <ftype>string
  <extra>max_size=30
  <modifiers>
  <unique>true
  <indexed>true
 </column>
 <column/>
  <name>field_application_version
  <field>127102
  <ftype>string
  <extra>max_size=5
  <modifiers>8000:protect
  <unique>false
  <indexed>false
 </column>
 <column/>
  <name>field_application_generate_status
  <field>127105
  <ftype>string
  <extra>
  <modifiers>
  <unique>false
  <indexed>false
 </column>
</columns>
<parents/>
</parents>
<restricts/>
</restricts>

> output -json lists.id=602900
{ "list":
[
 { "id": "602900" },
 { "cid": "127100" },
 { "name": "602900_name" },
 { "type": "group" },
 { "extra": "no_limit" },
 { "actions": "" },
 { "modifiers": "" },
 { "view": "602910" },
 { "class": "Application" },
 { "bclass": "" },
 { "order": "forward" },
 { "pclass": "101100" },
 { "pfield": "302220" },
 { "module": "Meta" },
 { "columns":
 [
  { "column":
  [
   { "name": "field_application_name" },
   { "field": "127101" },
   { "ftype": "string" },
   { "extra": "max_size=30" },
   { "modifiers": "" },
   { "unique": "true" },
   { "indexed": "true" }
  ]
  },
  { "column":
  [
   { "name": "field_application_version" },
   { "field": "127102" },
   { "ftype": "string" },
   { "extra": "max_size=5" },
   { "modifiers": "8000:protect" },
   { "unique": "false" },
   { "indexed": "false" }
  ]
  },
  { "column":
  [
   { "name": "field_application_generate_status" },
   { "field": "127105" },
   { "ftype": "string" },
   { "extra": "" },
   { "modifiers": "" },
   { "unique": "false" },
   { "indexed": "false" }
  ]
  }
 ]
 },
 { "parents":
 [
 ]
 },
 { "restricts":
 [
 ]
 }
]
}
> output views.id=601020
<id>601020
<cid>100100
<name>601020_name
<type>admin
<extra>
<modifiers>
<class>User
<bclass>
<module>Meta
<fields/>
 <field/>
  <name>field_user_username
  <field>100101
  <ftype>string
  <extra>hpassword_salt+max_size=30
  <mandatory>true
  <modifiers>
 </field>
 <field/>
  <name>field_user_active
  <field>100102
  <ftype>bool
  <extra>
  <mandatory>true
  <modifiers>
 </field>
 <field/>
  <name>field_user_password
  <field>100103
  <ftype>string
  <extra>new_only+hpassword+encrypted+max_size=200
  <mandatory>true
  <modifiers>
 </field>
 <field/>
  <name>field_user_new_password
  <field>100109
  <ftype>string
  <extra>non_new+admin_only+hpassword+encrypted+max_size=200
  <mandatory>false
  <modifiers>100:relegate
 </field>
 <field/>
  <name>field_user_description
  <field>100104
  <ftype>string
  <extra>max_size=100
  <mandatory>true
  <modifiers>
 </field>
 <field/>
  <name>field_user_email
  <field>100105
  <ftype>string
  <extra>mailto
  <mandatory>false
  <modifiers>100:relegate
 </field>
 <field/>
  <name>field_user_workgroup
  <field>300100
  <ftype>@class
  <extra>max_size=100+user_group
  <mandatory>false
  <modifiers>
  <pclass>101100
  <pfield>101101
  <pfname>field_workgroup_name
  <pextra>
 </field>
</fields>

> output -json views.id=601020
{ "view":
[
 { "id": "601020" },
 { "cid": "100100" },
 { "name": "601020_name" },
 { "type": "admin" },
 { "extra": "" },
 { "modifiers": "" },
 { "class": "User" },
 { "bclass": "" },
 { "module": "Meta" },
 { "fields":
 [
  { "field":
  [
   { "name": "field_user_username" },
   { "field": "100101" },
   { "ftype": "string" },
   { "extra": "hpassword_salt+max_size=30" },
   { "mandatory": "true" },
   { "modifiers": "" }
  ]
  },
  { "field":
  [
   { "name": "field_user_active" },
   { "field": "100102" },
   { "ftype": "bool" },
   { "extra": "" },
   { "mandatory": "true" },
   { "modifiers": "" }
  ]
  },
  { "field":
  [
   { "name": "field_user_password" },
   { "field": "100103" },
   { "ftype": "string" },
   { "extra": "new_only+hpassword+encrypted+max_size=200" },
   { "mandatory": "true" },
   { "modifiers": "" }
  ]
  },
  { "field":
  [
   { "name": "field_user_new_password" },
   { "field": "100109" },
   { "ftype": "string" },
   { "extra": "non_new+admin_only+hpassword+encrypted+max_size=200" },
   { "mandatory": "false" },
   { "modifiers": "100:relegate" }
  ]
  },
  { "field":
  [
   { "name": "field_user_description" },
   { "field": "100104" },
   { "ftype": "string" },
   { "extra": "max_size=100" },
   { "mandatory": "true" },
   { "modifiers": "" }
  ]
  },
  { "field":
  [
   { "name": "field_user_email" },
   { "field": "100105" },
   { "ftype": "string" },
   { "extra": "mailto" },
   { "mandatory": "false" },
   { "modifiers": "100:relegate" }
  ]
  },
  { "field":
  [
   { "name": "field_user_workgroup" },
   { "field": "300100" },
   { "ftype": "@class" },
   { "extra": "max_size=100+user_group" },
   { "mandatory": "false" },
   { "modifiers": "" },
   { "pclass": "101100" },
   { "pfield": "101101" },
   { "pfname": "field_workgroup_name" },
   { "pextra": "" }
  ]
  }
 ]
 }
]
}
> attributes
id
name
perm
title
user_info
user_perm
user_extra
user_group
user_other
user_parent
user_active
user_select
user_slevel
user_unique
user_tz_name
user_has_auth
user_pin_value
user_gpg_install
home_info

> attribute enums.*.id,name
id=575,name=enum_add_packages
id=906,name=enum_app_auto_days
id=905,name=enum_app_generate_type
id=900,name=enum_app_list_row_limit
id=903,name=enum_app_print_row_limit
id=902,name=enum_app_text_limit
id=901,name=enum_app_text_rows
id=904,name=enum_app_text_trunc_limit
id=907,name=enum_app_type
id=950,name=enum_archive_standard_size
id=570,name=enum_arg_type
id=580,name=enum_cascade_op
id=520,name=enum_class_extra
id=515,name=enum_class_type
id=511,name=enum_date_precision
id=801,name=enum_field_action_create_access_restriction
id=800,name=enum_field_action_create_type
id=802,name=enum_field_action_type
id=522,name=enum_field_alignment
id=530,name=enum_field_extra
id=600,name=enum_field_type
id=635,name=enum_font_size
id=514,name=enum_fraction_limit
id=505,name=enum_int_type
id=639,name=enum_label_source_child
id=688,name=enum_list_direction
id=687,name=enum_list_display_row_limit
id=698,name=enum_list_field_label_class
id=692,name=enum_list_field_label_source
id=691,name=enum_list_field_link_restrict
id=697,name=enum_list_field_link_type
id=696,name=enum_list_field_notes_truncation
id=695,name=enum_list_field_print_type
id=690,name=enum_list_field_restrict
id=694,name=enum_list_field_switch_type
id=693,name=enum_list_field_trigger_option
id=699,name=enum_list_field_view_parent_extra
id=682,name=enum_list_pdf_list_type
id=681,name=enum_list_print_restrict
id=680,name=enum_list_restrict
id=686,name=enum_list_search_opt_limit
id=685,name=enum_list_style
id=689,name=enum_list_text_match_highlight
id=850,name=enum_model_type
id=560,name=enum_modifier_affect_extra
id=540,name=enum_modifier_affect_scope
id=550,name=enum_modifier_affect_type
id=506,name=enum_numeric_type
id=638,name=enum_orientation
id=654,name=enum_pdf_font_type
id=960,name=enum_peerchain_peer_type
id=955,name=enum_peerchain_status
id=970,name=enum_persistence_type
id=500,name=enum_primitive
id=650,name=enum_procedure_arg_primitive
id=640,name=enum_procedure_arg_type
id=585,name=enum_relationship_extra
id=507,name=enum_round_type
id=610,name=enum_source_parent_type
id=620,name=enum_source_type
id=510,name=enum_string_domain
id=630,name=enum_test_field_type
id=512,name=enum_time_precision
id=535,name=enum_uom
id=655,name=enum_view_access_restrict
id=657,name=enum_view_change_restrict
id=661,name=enum_view_field_access_scope
id=662,name=enum_view_field_change_scope
id=664,name=enum_view_field_child_list_extra_option
id=678,name=enum_view_field_date_precision_option
id=674,name=enum_view_field_enum_at
id=679,name=enum_view_field_enum_option
id=676,name=enum_view_field_fk_trigger_behaviour
id=675,name=enum_view_field_fk_trigger_option
id=663,name=enum_view_field_link_restrict
id=677,name=enum_view_field_mandatory_option
id=670,name=enum_view_field_new_source
id=660,name=enum_view_field_restrict
id=672,name=enum_view_field_trigger_behaviour
id=673,name=enum_view_field_trigger_for_state
id=671,name=enum_view_field_trigger_option
id=658,name=enum_view_pdf_view_type
id=513,name=enum_zero_padding

> attribute lists.*.id,name
id=602900,name=602900_name
id=601620,name=601620_name
id=601300,name=601300_name
id=601420,name=601420_name
id=601720,name=601720_name
id=603800,name=603800_name
id=603900,name=603900_name
id=603960,name=603960_name
id=603980,name=603980_name
id=601820,name=601820_name
id=603210,name=603210_name
id=603310,name=603310_name
id=602810,name=602810_name
id=602860,name=602860_name
id=601500,name=601500_name
id=601920,name=601920_name
id=602020,name=602020_name
id=602950,name=602950_name
id=601520,name=601520_name
id=601550,name=601550_name
id=603500,name=603500_name
id=602260,name=602260_name
id=602120,name=602120_name
id=602220,name=602220_name
id=602320,name=602320_name
id=602321,name=602321_name
id=602530,name=602530_name
id=602540,name=602540_name
id=602550,name=602550_name
id=602410,name=602410_name
id=602420,name=602420_name
id=601220,name=601220_name
id=601000,name=601000_name
id=601010,name=601010_name
id=602620,name=602620_name
id=602720,name=602720_name
id=601100,name=601100_name

> attribute views.*.id,name
id=602910,name=602910_name
id=601610,name=601610_name
id=601310,name=601310_name
id=601410,name=601410_name
id=601710,name=601710_name
id=603850,name=603850_name
id=603950,name=603950_name
id=603970,name=603970_name
id=603990,name=603990_name
id=601810,name=601810_name
id=603200,name=603200_name
id=603300,name=603300_name
id=602800,name=602800_name
id=602850,name=602850_name
id=601510,name=601510_name
id=601910,name=601910_name
id=602010,name=602010_name
id=602960,name=602960_name
id=601530,name=601530_name
id=601560,name=601560_name
id=603550,name=603550_name
id=602250,name=602250_name
id=602100,name=602100_name
id=602200,name=602200_name
id=602300,name=602300_name
id=602500,name=602500_name
id=603100,name=603100_name
id=602400,name=602400_name
id=601210,name=601210_name
id=601020,name=601020_name
id=602610,name=602610_name
id=602710,name=602710_name
id=601110,name=601110_name

> exit

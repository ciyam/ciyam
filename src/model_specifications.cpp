// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#endif

#include "model_specifications.h"

#include "sio.h"
#include "model.h"
#include "utilities.h"

using namespace std;

namespace
{

const char* const c_true = "true";
const char* const c_false = "false";

const char* const c_arg_major = "-major";
const char* const c_arg_annual = "-annual";
const char* const c_arg_future = "-future";
const char* const c_arg_reverse = "-reverse";
const char* const c_arg_changing = "-changing";
const char* const c_arg_negative = "-negative";
const char* const c_arg_no_clear = "-no_clear";
const char* const c_arg_new_only = "-new_only";
const char* const c_arg_not_equal = "-not_equal";
const char* const c_arg_for_store = "-for_store";
const char* const c_arg_non_empty = "-non_empty";
const char* const c_arg_transient = "-transient";
const char* const c_arg_opt_prefix = "-opt_prefix";
const char* const c_arg_self_child = "-self_child";
const char* const c_arg_skip_nulls = "-skip_nulls";
const char* const c_arg_check_null = "-check_null";
const char* const c_arg_not_create = "-not_create";
const char* const c_arg_embed_links = "-embed_links";
const char* const c_arg_exact_match = "-exact_match";
const char* const c_arg_not_default = "-not_default";
const char* const c_arg_create_only = "-create_only";
const char* const c_arg_combine_keys = "-combine_keys";
const char* const c_arg_grand_parent = "-grand_parent";
const char* const c_arg_allow_internal = "-allow_internal";
const char* const c_arg_include_clones = "-include_clones";
const char* const c_arg_test_not_equal = "-test_not_equal";
const char* const c_arg_include_default = "-include_default";

const char* const c_arg_add_prefix = "-add=";
const char* const c_arg_sep_prefix = "-sep=";
const char* const c_arg_enum_prefix = "-enum=";
const char* const c_arg_func_prefix = "-func=";
const char* const c_arg_test_prefix = "-test=";
const char* const c_arg_type_prefix = "-type=";
const char* const c_arg_check_prefix = "-check=";
const char* const c_arg_order_prefix = "-order=";
const char* const c_arg_child_prefix = "-child=";
const char* const c_arg_scope_prefix = "-scope=";
const char* const c_arg_append_prefix = "-append=";
const char* const c_arg_status_prefix = "-status=";
const char* const c_arg_memfunc_prefix = "-memfunc=";
const char* const c_arg_options_prefix = "-options=";
const char* const c_arg_reverse_prefix = "-reverse=";
const char* const c_arg_special_prefix = "-special=";
const char* const c_arg_convertor_prefix = "-convertor=";
const char* const c_arg_child_field_prefix = "-child_field=";
const char* const c_arg_append_to_ext_prefix = "-append_to_ext=";

const char* const c_attribute_amt = "amt";
const char* const c_attribute_ext = "ext";
const char* const c_attribute_name = "name";
const char* const c_attribute_type = "type";
const char* const c_attribute_extra = "extra";
const char* const c_attribute_limit = "limit";
const char* const c_attribute_value = "value";
const char* const c_attribute_period = "period";
const char* const c_attribute_suffix = "suffix";
const char* const c_attribute_add_amt = "add_amt";
const char* const c_attribute_enum_id = "enum_id";
const char* const c_attribute_add_type = "add_type";
const char* const c_attribute_class_id = "class_id";
const char* const c_attribute_field_id = "field_id";
const char* const c_attribute_function = "function";
const char* const c_attribute_mem_func = "mem_func";
const char* const c_attribute_new_only = "new_only";
const char* const c_attribute_not_null = "not_null";
const char* const c_attribute_cclass_id = "cclass_id";
const char* const c_attribute_is_annual = "is_annual";
const char* const c_attribute_is_months = "is_months";
const char* const c_attribute_cfield_id = "cfield_id";
const char* const c_attribute_efield_id = "efield_id";
const char* const c_attribute_fclass_id = "fclass_id";
const char* const c_attribute_ffield_id = "ffield_id";
const char* const c_attribute_gclass_id = "gclass_id";
const char* const c_attribute_gfield_id = "gfield_id";
const char* const c_attribute_mclass_id = "mclass_id";
const char* const c_attribute_mfield_id = "mfield_id";
const char* const c_attribute_pclass_id = "pclass_id";
const char* const c_attribute_pfield_id = "pfield_id";
const char* const c_attribute_nfield_id = "nfield_id";
const char* const c_attribute_class1_id = "class1_id";
const char* const c_attribute_class2_id = "class2_id";
const char* const c_attribute_dfield_id = "dfield_id";
const char* const c_attribute_field1_id = "field1_id";
const char* const c_attribute_field2_id = "field2_id";
const char* const c_attribute_for_store = "for_store";
const char* const c_attribute_lfield_id = "lfield_id";
const char* const c_attribute_ofield_id = "ofield_id";
const char* const c_attribute_rfield_id = "rfield_id";
const char* const c_attribute_sclass_id = "sclass_id";
const char* const c_attribute_sfield_id = "sfield_id";
const char* const c_attribute_tclass_id = "tclass_id";
const char* const c_attribute_tfield_id = "tfield_id";
const char* const c_attribute_uclass_id = "uclass_id";
const char* const c_attribute_ufield_id = "ufield_id";
const char* const c_attribute_xfield_id = "xfield_id";
const char* const c_attribute_data_type = "data_type";
const char* const c_attribute_new_value = "new_value";
const char* const c_attribute_non_empty = "non_empty";
const char* const c_attribute_not_equal = "not_equal";
const char* const c_attribute_separator = "separator";
const char* const c_attribute_convertor = "convertor";
const char* const c_attribute_is_future = "is_future";
const char* const c_attribute_check_null = "check_null";
const char* const c_attribute_cpfield_id = "cpfield_id";
const char* const c_attribute_data_value = "data_value";
const char* const c_attribute_gpfield_id = "gpfield_id";
const char* const c_attribute_is_reverse = "is_reverse";
const char* const c_attribute_paclass_id = "paclass_id";
const char* const c_attribute_pafield_id = "pafield_id";
const char* const c_attribute_pfield1_id = "pfield1_id";
const char* const c_attribute_pfield2_id = "pfield2_id";
const char* const c_attribute_prfield_id = "prfield_id";
const char* const c_attribute_scfield_id = "scfield_id";
const char* const c_attribute_sfield1_id = "sfield1_id";
const char* const c_attribute_sfield2_id = "sfield2_id";
const char* const c_attribute_sfield3_id = "sfield3_id";
const char* const c_attribute_spfield_id = "spfield_id";
const char* const c_attribute_tcfield_id = "tcfield_id";
const char* const c_attribute_tpfield_id = "tpfield_id";
const char* const c_attribute_tvfield_id = "tvfield_id";
const char* const c_attribute_vdfield_id = "vdfield_id";
const char* const c_attribute_vsfield_id = "vsfield_id";
const char* const c_attribute_dest_value = "dest_value";
const char* const c_attribute_null_value = "null_value";
const char* const c_attribute_self_child = "self_child";
const char* const c_attribute_skip_nulls = "skip_nulls";
const char* const c_attribute_test_value = "test_value";
const char* const c_attribute_field_info = "field_info";
const char* const c_attribute_mpfield_id = "mpfield_id";
const char* const c_attribute_pnfield_id = "pnfield_id";
const char* const c_attribute_fkclass_id = "fkclass_id";
const char* const c_attribute_fkfield_id = "fkfield_id";
const char* const c_attribute_cffield_id = "cffield_id";
const char* const c_attribute_not_create = "not_create";
const char* const c_attribute_create_only = "create_only";
const char* const c_attribute_crmfield_id = "crmfield_id";
const char* const c_attribute_embed_links = "embed_links";
const char* const c_attribute_field_id_id = "field_id_id";
const char* const c_attribute_fkcclass_id = "fkcclass_id";
const char* const c_attribute_fkcfield_id = "fkcfield_id";
const char* const c_attribute_gcpfield_id = "gcpfield_id";
const char* const c_attribute_is_changing = "is_changing";
const char* const c_attribute_is_negative = "is_negative";
const char* const c_attribute_mcrclass_id = "mcrclass_id";
const char* const c_attribute_mcrfield_id = "mcrfield_id";
const char* const c_attribute_mttfield_id = "mttfield_id";
const char* const c_attribute_mtsfield_id = "mtsfield_id";
const char* const c_attribute_pacfield_id = "pacfield_id";
const char* const c_attribute_patfield_id = "patfield_id";
const char* const c_attribute_pcfield1_id = "pcfield1_id";
const char* const c_attribute_pcfield2_id = "pcfield2_id";
const char* const c_attribute_pfkfield_id = "pfkfield_id";
const char* const c_attribute_not_default = "not_default";
const char* const c_attribute_field_value = "field_value";
const char* const c_attribute_dfield_prim = "dfield_prim";
const char* const c_attribute_dfield_type = "dfield_type";
const char* const c_attribute_modifier_id = "modifier_id";
const char* const c_attribute_sftfield_id = "sftfield_id";
const char* const c_attribute_act_field_id = "act_field_id";
const char* const c_attribute_append_value = "append_value";
const char* const c_attribute_combine_keys = "combine_keys";
const char* const c_attribute_crpmfield_id = "crpmfield_id";
const char* const c_attribute_field_key_id = "field_key_id";
const char* const c_attribute_fkcffield_id = "fkcffield_id";
const char* const c_attribute_is_transient = "is_transient";
const char* const c_attribute_mcrcfield_id = "mcrcfield_id";
const char* const c_attribute_tfield_value = "tfield_value";
const char* const c_attribute_uid_field_id = "uid_field_id";
const char* const c_attribute_pw_enum_name = "pw_enum_name";
const char* const c_attribute_ug_enum_name = "ug_enum_name";
const char* const c_attribute_append_to_ext = "append_to_ext";
const char* const c_attribute_crgpmfield_id = "crgpmfield_id";
const char* const c_attribute_link_class_id = "link_class_id";
const char* const c_attribute_opt_field2_id = "opt_field2_id";
const char* const c_attribute_src_field2_id = "src_field2_id";
const char* const c_attribute_type_field_id = "type_field_id";
const char* const c_attribute_uid2_field_id = "uid2_field_id";
const char* const c_attribute_mask_field_id = "mask_field_id";
const char* const c_attribute_next_field_id = "next_field_id";
const char* const c_attribute_auto_class_id = "auto_class_id";
const char* const c_attribute_field_field_id = "field_field_id";
const char* const c_attribute_opt_prefix2_id = "opt_prefix2_id";
const char* const c_attribute_pw_enum_2_name = "pw_enum_2_name";
const char* const c_attribute_pw_enum_value = "pw_enum_value";
const char* const c_attribute_ug_enum_value = "ug_enum_value";
const char* const c_attribute_action_value = "action_value";
const char* const c_attribute_status_value = "status_value";
const char* const c_attribute_options_left = "options_left";
const char* const c_attribute_protect_spec = "protect_spec";
const char* const c_attribute_options_right = "options_right";
const char* const c_attribute_protect_value = "protect_value";
const char* const c_attribute_protect_scope = "protect_scope";
const char* const c_attribute_procedure_id = "procedure_id";
const char* const c_attribute_dtm_field_id = "dtm_field_id";
const char* const c_attribute_dtm2_field_id = "dtm2_field_id";
const char* const c_attribute_test_field_id = "test_field_id";
const char* const c_attribute_task_class_id = "task_class_id";
const char* const c_attribute_default_value = "default_value";
const char* const c_attribute_special_value = "special_value";
const char* const c_attribute_user_info_key = "user_info_key";
const char* const c_attribute_allow_internal = "allow_internal";
const char* const c_attribute_is_exact_match = "is_exact_match";
const char* const c_attribute_test2_field_id = "test2_field_id";
const char* const c_attribute_restrict_value = "restrict_value";
const char* const c_attribute_child_class_id = "child_class_id";
const char* const c_attribute_child_field_id = "child_field_id";
const char* const c_attribute_extra_field_id = "extra_field_id";
const char* const c_attribute_has_opt_prefix = "has_opt_prefix";
const char* const c_attribute_include_clones = "include_clones";
const char* const c_attribute_order_field_id = "order_field_id";
const char* const c_attribute_test_not_equal = "test_not_equal";
const char* const c_attribute_child_pclass_id = "child_pclass_id";
const char* const c_attribute_child_pfield_id = "child_pfield_id";
const char* const c_attribute_enum_value_name = "enum_value_name";
const char* const c_attribute_extra_sfield_id = "extra_sfield_id";
const char* const c_attribute_include_default = "include_default";
const char* const c_attribute_is_grand_parent = "is_grand_parent";
const char* const c_attribute_pw_enum_2_value = "pw_enum_2_value";
const char* const c_attribute_prefix_class_id = "prefix_class_id";
const char* const c_attribute_prefix_field_id = "prefix_field_id";
const char* const c_attribute_status_field_id = "status_field_id";
const char* const c_attribute_suffix_class_id = "suffix_class_id";
const char* const c_attribute_suffix_field_id = "suffix_field_id";
const char* const c_attribute_relationship_id = "relationship_id";
const char* const c_attribute_source_class_id = "source_class_id";
const char* const c_attribute_source_field_cid = "source_field_cid";
const char* const c_attribute_neq_status_value = "neq_status_value";
const char* const c_attribute_version_field_id = "version_field_id";
const char* const c_attribute_special_field_id = "special_field_id";
const char* const c_attribute_special_class_id = "special_class_id";
const char* const c_attribute_cfk_from_field_id = "cfk_from_field_id";
const char* const c_attribute_internal_field_id = "internal_field_id";
const char* const c_attribute_special_cfield_id = "special_cfield_id";
const char* const c_attribute_source_field_value = "source_field_value";
const char* const c_attribute_exhausted_field_id = "exhausted_field_id";
const char* const c_attribute_neq_status_field_id = "neq_status_field_id";
const char* const c_attribute_increment_procedure_id = "increment_procedure_id";
const char* const c_attribute_special_extra_class_id = "special_extra_class_id";
const char* const c_attribute_special_extra_source_id = "special_extra_source_id";
const char* const c_attribute_special_extra_field_info = "special_extra_field_info";
const char* const c_attribute_increment_next_value_arg_id = "increment_next_value_arg_id";
const char* const c_attribute_move_up_procedure_id = "move_up_procedure_id";
const char* const c_attribute_move_up_fields_arg_id = "move_up_fields_args_id";
const char* const c_attribute_move_up_values_arg_id = "move_up_values_args_id";
const char* const c_attribute_move_down_procedure_id = "move_down_procedure_id";
const char* const c_attribute_move_down_fields_arg_id = "move_down_fields_args_id";
const char* const c_attribute_move_down_values_arg_id = "move_down_values_args_id";
const char* const c_attribute_class1_fld_id = "class1_fld_id";
const char* const c_attribute_class1_fk_id = "class1_fk_id";
const char* const c_attribute_class2_fld_id = "class2_fld_id";
const char* const c_attribute_class2_fk_id = "class2_fk_id";
const char* const c_attribute_class1_fk2_id = "class1_fk2_id";
const char* const c_attribute_class2_fk2_id = "class2_fk2_id";
const char* const c_attribute_class1_fk_cid = "class1_fk_cid";
const char* const c_attribute_class2_fk_cid = "class2_fk_cid";
const char* const c_attribute_class1_fk2_cid = "class1_fk2_cid";
const char* const c_attribute_class2_fk2_cid = "class2_fk2_cid";
const char* const c_attribute_class1_fk_value = "class1_fk_value";
const char* const c_attribute_class1_fk_title = "class1_fk_title";
const char* const c_attribute_class2_fk_value = "class2_fk_value";
const char* const c_attribute_class2_fk_title = "class2_fk_title";
const char* const c_attribute_class1_fk2_value = "class1_fk2_value";
const char* const c_attribute_class1_fk2_title = "class1_fk2_title";
const char* const c_attribute_class2_fk2_value = "class2_fk2_value";
const char* const c_attribute_class2_fk2_title = "class2_fk2_title";
const char* const c_attribute_test_field_value = "test_field_value";
const char* const c_attribute_task_pv_field_id = "task_pv_field_id";
const char* const c_attribute_test2_field_value = "test2_field_value";
const char* const c_attribute_major_version_only = "major_version_only";

const char* const c_data_amt = "amt";
const char* const c_data_cid = "cid";
const char* const c_data_ext = "ext";
const char* const c_data_key = "key";
const char* const c_data_func = "func";
const char* const c_data_name = "name";
const char* const c_data_type = "type";
const char* const c_data_child = "child";
const char* const c_data_class = "class";
const char* const c_data_dtype = "dtype";
const char* const c_data_field = "field";
const char* const c_data_label = "label";
const char* const c_data_limit = "limit";
const char* const c_data_mfunc = "mfunc";
const char* const c_data_value = "value";
const char* const c_data_uikey = "uikey";
const char* const c_data_annual = "annual";
const char* const c_data_append = "append";
const char* const c_data_cclass = "cclass";
const char* const c_data_evalue = "evalue";
const char* const c_data_fclass = "fclass";
const char* const c_data_gchild = "gchild";
const char* const c_data_gclass = "gclass";
const char* const c_data_mclass = "mclass";
const char* const c_data_oclass = "oclass";
const char* const c_data_months = "months";
const char* const c_data_period = "period";
const char* const c_data_cfield = "cfield";
const char* const c_data_dfield = "dfield";
const char* const c_data_dfprim = "dfprim";
const char* const c_data_dftype = "dftype";
const char* const c_data_dvalue = "dvalue";
const char* const c_data_efield = "efield";
const char* const c_data_ffield = "ffield";
const char* const c_data_field1 = "field1";
const char* const c_data_field2 = "field2";
const char* const c_data_fields = "fields";
const char* const c_data_future = "future";
const char* const c_data_gfield = "gfield";
const char* const c_data_ifield = "ifield";
const char* const c_data_is_neg = "is_neg";
const char* const c_data_mfield = "mfield";
const char* const c_data_nvalue = "nvalue";
const char* const c_data_parent = "parent";
const char* const c_data_pclass = "pclass";
const char* const c_data_nfield = "nfield";
const char* const c_data_not_eq = "not_eq";
const char* const c_data_lfield = "lfield";
const char* const c_data_ofield = "ofield";
const char* const c_data_oleftb = "oleftb";
const char* const c_data_pfield = "pfield";
const char* const c_data_prefix = "prefix";
const char* const c_data_pwname = "pwname";
const char* const c_data_rfield = "rfield";
const char* const c_data_sclass = "sclass";
const char* const c_data_sfield = "sfield";
const char* const c_data_static = "static";
const char* const c_data_suffix = "suffix";
const char* const c_data_tfield = "tfield";
const char* const c_data_tvalue = "tvalue";
const char* const c_data_uclass = "uclass";
const char* const c_data_ufield = "ufield";
const char* const c_data_ugname = "ugname";
const char* const c_data_xfield = "xfield";
const char* const c_data_add_amt = "add_amt";
const char* const c_data_cffield = "cffield";
const char* const c_data_command = "command";
const char* const c_data_cpclass = "cpclass";
const char* const c_data_cpfield = "cpfield";
const char* const c_data_dn_proc = "dn_proc";
const char* const c_data_fkchild = "fkchild";
const char* const c_data_fkclass = "fkclass";
const char* const c_data_fkfield = "fkfield";
const char* const c_data_gpfield = "gpfield";
const char* const c_data_idfield = "idfield";
const char* const c_data_mpfield = "mpfield";
const char* const c_data_new_val = "new_val";
const char* const c_data_not_new = "not_new";
const char* const c_data_ofield2 = "ofield2";
const char* const c_data_orightb = "orightb";
const char* const c_data_pafield = "pafield";
const char* const c_data_pfield1 = "pfield1";
const char* const c_data_pfield2 = "pfield2";
const char* const c_data_pnfield = "pnfield";
const char* const c_data_prefix2 = "prefix2";
const char* const c_data_prfield = "prfield";
const char* const c_data_pvchild = "pvchild";
const char* const c_data_pwname2 = "pwname2";
const char* const c_data_pwvalue = "pwvalue";
const char* const c_data_reverse = "reverse";
const char* const c_data_scclass = "scclass";
const char* const c_data_scfield = "scfield";
const char* const c_data_sfield1 = "sfield1";
const char* const c_data_sfield2 = "sfield2";
const char* const c_data_sfield3 = "sfield3";
const char* const c_data_sfclass = "sfclass";
const char* const c_data_sfvalue = "sfvalue";
const char* const c_data_spfield = "spfield";
const char* const c_data_tcfield = "tcfield";
const char* const c_data_tpfield = "tpfield";
const char* const c_data_tvfield = "tvfield";
const char* const c_data_ugvalue = "ugvalue";
const char* const c_data_up_proc = "up_proc";
const char* const c_data_vdfield = "vdfield";
const char* const c_data_vsfield = "vsfield";
const char* const c_data_xsfield = "xsfield";
const char* const c_data_add_type = "add_type";
const char* const c_data_changing = "changing";
const char* const c_data_chk_null = "chk_null";
const char* const c_data_cmd_args = "cmd_args";
const char* const c_data_crmfield = "crmfield";
const char* const c_data_fidfield = "fidfield";
const char* const c_data_fkcclass = "fkcclass";
const char* const c_data_fkcfield = "fkcfield";
const char* const c_data_mcrfield = "mcrfield";
const char* const c_data_mttfield = "mttfield";
const char* const c_data_mtsfield = "mtsfield";
const char* const c_data_pcfield1 = "pcfield1";
const char* const c_data_pcfield2 = "pcfield2";
const char* const c_data_pfkfield = "pfkfield";
const char* const c_data_pwvalue2 = "pwvalue2";
const char* const c_data_test_neq = "test_neq";
const char* const c_data_cvt_func = "cvt_func";
const char* const c_data_inc_dflt = "inc_dflt";
const char* const c_data_inc_proc = "inc_proc";
const char* const c_data_keyfield = "keyfield";
const char* const c_data_modifier = "modifier";
const char* const c_data_new_only = "new_only";
const char* const c_data_not_null = "not_null";
const char* const c_data_pacfield = "pacfield";
const char* const c_data_patfield = "patfield";
const char* const c_data_root_key = "root_key";
const char* const c_data_sftfield = "sftfield";
const char* const c_data_not_dflt = "not_dflt";
const char* const c_data_act_field = "act_field";
const char* const c_data_crpmfield = "crpmfield";
const char* const c_data_dn_fields = "dn_fields";
const char* const c_data_dn_values = "dn_values";
const char* const c_data_fkcffield = "fkcffield";
const char* const c_data_for_store = "for_store";
const char* const c_data_mcrcfield = "mcrcfield";
const char* const c_data_non_empty = "non_empty";
const char* const c_data_separator = "separator";
const char* const c_data_transient = "transient";
const char* const c_data_inc_value = "inc_value";
const char* const c_data_up_fields = "up_fields";
const char* const c_data_up_values = "up_values";
const char* const c_data_ver_field = "ver_field";
const char* const c_data_procedure = "procedure";
const char* const c_data_spec_file = "spec_file";
const char* const c_data_crgpmfield = "crgpmfield";
const char* const c_data_fmandatory = "fmandatory";
const char* const c_data_ftransient = "ftransient";
const char* const c_data_is_testval = "is_testval";
const char* const c_data_link_class = "link_class";
const char* const c_data_mask_field = "mask_field";
const char* const c_data_next_field = "next_field";
const char* const c_data_not_create = "not_create";
const char* const c_data_opt_prefix = "opt_prefix";
const char* const c_data_self_child = "self_child";
const char* const c_data_spec_value = "spec_value";
const char* const c_data_spec_scope = "spec_scope";
const char* const c_data_auto_class = "auto_class";
const char* const c_data_inc_clones = "inc_clones";
const char* const c_data_after_store = "after_store";
const char* const c_data_child_class = "child_class";
const char* const c_data_child_field = "child_field";
const char* const c_data_create_only = "create_only";
const char* const c_data_embed_links = "embed_links";
const char* const c_data_exact_match = "exact_match";
const char* const c_data_fistexttype = "fistexttype";
const char* const c_data_order_field = "order_field";
const char* const c_data_combine_keys = "combine_keys";
const char* const c_data_grand_parent = "grand_parent";
const char* const c_data_status_field = "status_field";
const char* const c_data_status_value = "status_value";
const char* const c_data_append_to_ext = "append_to_ext";
const char* const c_data_sfistexttype = "sfistexttype";
const char* const c_data_tfistexttype = "tfistexttype";
const char* const c_data_fk_from_child = "fk_from_child";
const char* const c_data_primary_class = "primary_class";
const char* const c_data_scfistexttype = "scfistexttype";
const char* const c_data_special_value = "special_value";
const char* const c_data_special_field = "special_field";
const char* const c_data_tcfistexttype = "tcfistexttype";
const char* const c_data_special_cfield = "special_cfield";
const char* const c_data_exhausted_field = "exhausted_field";
const char* const c_data_secondary_class = "secondary_class";
const char* const c_data_neq_status_field = "neq_status_field";
const char* const c_data_neq_status_value = "neq_status_value";
const char* const c_data_dtm_field = "dtm_field";
const char* const c_data_dtm2_field = "dtm2_field";
const char* const c_data_uid_field = "uid_field";
const char* const c_data_uid2_field = "uid2_field";
const char* const c_data_file_field = "file_field";
const char* const c_data_test_field = "test_field";
const char* const c_data_test_value = "test_value";
const char* const c_data_task_class = "task_class";
const char* const c_data_field_list = "field_list";
const char* const c_data_major_only = "major_only";
const char* const c_data_skip_nulls = "skip_nulls";
const char* const c_data_allow_internal = "allow_internal";
const char* const c_data_special_extra_source = "special_extra_source";
const char* const c_data_primary_stats1_cclass = "primary_stats1_cclass";
const char* const c_data_primary_stats2_cclass = "primary_stats2_cclass";
const char* const c_data_primary_stats1_cfield = "primary_stats1_cfield";
const char* const c_data_primary_stats2_cfield = "primary_stats2_cfield";
const char* const c_data_primary_stats1_class1 = "primary_stats1_class1";
const char* const c_data_primary_stats1_field1 = "primary_stats1_field1";
const char* const c_data_primary_stats1_value1 = "primary_stats1_value1";
const char* const c_data_primary_stats2_class1 = "primary_stats2_class1";
const char* const c_data_primary_stats2_field1 = "primary_stats2_field1";
const char* const c_data_primary_stats2_value1 = "primary_stats2_value1";
const char* const c_data_primary_stats1_class2 = "primary_stats1_class2";
const char* const c_data_primary_stats1_field2 = "primary_stats1_field2";
const char* const c_data_primary_stats1_value2 = "primary_stats1_value2";
const char* const c_data_primary_stats2_class2 = "primary_stats2_class2";
const char* const c_data_primary_stats2_field2 = "primary_stats2_field2";
const char* const c_data_primary_stats2_value2 = "primary_stats2_value2";
const char* const c_data_secondary_stats1_cclass = "secondary_stats1_cclass";
const char* const c_data_secondary_stats2_cclass = "secondary_stats2_cclass";
const char* const c_data_secondary_stats1_cfield = "secondary_stats1_cfield";
const char* const c_data_secondary_stats2_cfield = "secondary_stats2_cfield";
const char* const c_data_secondary_stats1_class1 = "secondary_stats1_class1";
const char* const c_data_secondary_stats1_field1 = "secondary_stats1_field1";
const char* const c_data_secondary_stats1_value1 = "secondary_stats1_value1";
const char* const c_data_secondary_stats2_class1 = "secondary_stats2_class1";
const char* const c_data_secondary_stats2_field1 = "secondary_stats2_field1";
const char* const c_data_secondary_stats2_value1 = "secondary_stats2_value1";
const char* const c_data_secondary_stats1_class2 = "secondary_stats1_class2";
const char* const c_data_secondary_stats1_field2 = "secondary_stats1_field2";
const char* const c_data_secondary_stats1_value2 = "secondary_stats1_value2";
const char* const c_data_secondary_stats2_class2 = "secondary_stats2_class2";
const char* const c_data_secondary_stats2_field2 = "secondary_stats2_field2";
const char* const c_data_secondary_stats2_value2 = "secondary_stats2_value2";
const char* const c_data_special_extra_field_info = "special_extra_field_info";

bool is_non_string_type( const string& field_type )
{
   bool retval = false;

   // FUTURE: These types should not listed both here and in the "modeller".
   if( field_type == "int" || field_type == "bool" || field_type == "numeric" )
      retval = true;

   return retval;
}

// FUTURE: This should probably be put into "model.cpp".
string decorated_class_name( const string& class_name )
{
   return "{" + class_name + "}";
}

string get_id_for_enum_name( model& m, const string& enum_name )
{
   string enum_id;

   vector< user_defined_enum_data > all_ude_data;
   m.get_user_defined_enum_data( all_ude_data );

   for( size_t i = 0; i < all_ude_data.size( ); i++ )
   {
      if( all_ude_data[ i ].name == enum_name )
      {
         enum_id = all_ude_data[ i ].id;
         break;
      }
   }

   return enum_id;
}

string get_key_for_enum_value( model& m, const string& enum_id, const string& enum_value )
{
   string enum_key;

   vector< user_defined_enum_data > all_ude_data;
   m.get_user_defined_enum_data( all_ude_data );

   for( size_t i = 0; i < all_ude_data.size( ); i++ )
   {
      if( all_ude_data[ i ].id == enum_id )
      {
         vector< string > key_values;
         split( all_ude_data[ i ].key_values, key_values );

         for( size_t j = 0; j < key_values.size( ); j++ )
         {
            string::size_type pos = key_values[ j ].find( '=' );

            string value( key_values[ j ].substr( pos + 1 ) );

            string::size_type rpos = value.find( '=' );
            if( rpos != string::npos )
               value.erase( rpos );

            if( unescaped( value ) == enum_value )
            {
               enum_key = key_values[ j ].substr( 0, pos );
               break;
            }
         }

         break;
      }
   }

   return enum_key;
}

string get_class_id_for_name( model& m, const string& class_name, bool* p_is_external_alias = 0 )
{
   class_data cls_data;
   m.get_class_data( class_name, cls_data );

   if( p_is_external_alias )
      *p_is_external_alias = cls_data.is_external_alias;

   return cls_data.id;
}

string get_class_name_for_id( const vector< class_data >& all_class_data, const string& class_id )
{
   string class_name;
   for( size_t i = 0; i < all_class_data.size( ); i++ )
   {
      if( all_class_data[ i ].id == class_id )
      {
         class_name = all_class_data[ i ].name;
         break;
      }
   }

   return class_name;
}

string get_class_name_for_id( model& m, const string& class_id )
{
   vector< class_data > all_class_data;
   m.get_class_data( all_class_data );

   return get_class_name_for_id( all_class_data, class_id );
}

string get_class_name_from_field_type( model& m, const string& class_name, const string& field_name, const string& field_type )
{
   string::size_type pos = field_type.find( m.get_name( ) );
   if( pos == string::npos || field_type.length( ) < m.get_name( ).length( ) + 2 )
      return string( );
   else
      return field_type.substr( m.get_name( ).length( ) + 1 );
}

string get_field_id_for_name( const vector< field_data >& all_field_data,
 const string& field_name, string* p_field_type = 0, bool check_decorated_name = false,
 bool get_sys_type = false, bool* p_is_mandatory = 0 )
{
   string field_id;
   for( size_t i = 0; i < all_field_data.size( ); i++ )
   {
      if( ( !check_decorated_name && field_name == all_field_data[ i ].name )
       || ( check_decorated_name && field_name == all_field_data[ i ].decorated_name ) )
      {
         field_id = all_field_data[ i ].id;

         if( p_field_type )
            *p_field_type = get_sys_type ? all_field_data[ i ].sys_type : all_field_data[ i ].type;

         if( p_is_mandatory )
            *p_is_mandatory = all_field_data[ i ].is_mandatory;

         break;
      }
   }

   return field_id;
}

string get_field_id_for_name( model& m,
 const string& class_name, const string& field_name, string* p_field_type = 0,
 bool check_decorated_name = false, bool get_sys_type = false, bool include_base_fields = false, bool* p_is_mandatory = 0 )
{
   vector< field_data > all_field_data;
   m.get_field_data( class_name, all_field_data, e_get_field_type_any, include_base_fields );

   return get_field_id_for_name( all_field_data, field_name, p_field_type, check_decorated_name, get_sys_type, p_is_mandatory );
}

string get_field_name_for_id( const vector< field_data >& all_field_data,
 const string& field_id, string* p_field_type = 0, bool get_decorated_name = false, bool* p_is_mandatory = 0 )
{
   string field_name;
   for( size_t i = 0; i < all_field_data.size( ); i++ )
   {
      if( field_id == all_field_data[ i ].id )
      {
         if( !get_decorated_name )
            field_name = all_field_data[ i ].name;
         else
            field_name = all_field_data[ i ].decorated_name;

         if( p_field_type )
            *p_field_type = all_field_data[ i ].type;

         if( p_is_mandatory )
            *p_is_mandatory = all_field_data[ i ].is_mandatory;

         break;
      }
   }

   return field_name;
}

string get_field_name_for_id( model& m, const string& class_name, const string& field_id,
 string* p_field_type = 0, bool include_base_fields = false, bool get_decorated_name = false, bool* p_is_mandatory = 0 )
{
   vector< field_data > all_field_data;
   m.get_field_data( class_name, all_field_data, e_get_field_type_any, include_base_fields );

   return get_field_name_for_id( all_field_data, field_id, p_field_type, get_decorated_name, p_is_mandatory );
}

string get_modifier_id_for_name( model& m, const string& class_name, const string& modifier_name )
{
   string modifier_id;

   vector< modifier_data > all_modifier_data;
   m.get_modifier_data( class_name, all_modifier_data );

   for( size_t i = 0; i < all_modifier_data.size( ); i++ )
   {
      if( all_modifier_data[ i ].name == modifier_name )
      {
         modifier_id = all_modifier_data[ i ].id;
         break;
      }
   }

   return modifier_id;
}

string get_modifier_name_for_id( model& m, const string& class_name, const string& modifier_id )
{
   string modifier_name;

   vector< modifier_data > all_modifier_data;
   m.get_modifier_data( class_name, all_modifier_data );

   for( size_t i = 0; i < all_modifier_data.size( ); i++ )
   {
      if( all_modifier_data[ i ].id == modifier_id )
      {
         modifier_name = all_modifier_data[ i ].name;
         break;
      }
   }

   return modifier_name;
}

string get_modifier_value_for_id( model& m, const string& class_name, const string& modifier_id )
{
   string modifier_value;

   vector< modifier_data > all_modifier_data;
   m.get_modifier_data( class_name, all_modifier_data );

   for( size_t i = 0; i < all_modifier_data.size( ); i++ )
   {
      if( all_modifier_data[ i ].id == modifier_id )
      {
         modifier_value = all_modifier_data[ i ].value;
         break;
      }
   }

   return modifier_value;
}

string get_procedure_id_for_name( model& m, const string& class_name, const string& procedure_name )
{
   string procedure_id;

   vector< procedure_data > all_procedure_data;
   m.get_procedure_data( class_name, all_procedure_data, true );

   for( size_t i = 0; i < all_procedure_data.size( ); i++ )
   {
      if( all_procedure_data[ i ].name == procedure_name )
      {
         procedure_id = all_procedure_data[ i ].id;
         break;
      }
   }

   return procedure_id;
}

string get_procedure_name_for_id( model& m, const string& class_name, const string& procedure_id )
{
   string procedure_name;

   vector< procedure_data > all_procedure_data;
   m.get_procedure_data( class_name, all_procedure_data, true );

   for( size_t i = 0; i < all_procedure_data.size( ); i++ )
   {
      if( all_procedure_data[ i ].id == procedure_id )
      {
         procedure_name = all_procedure_data[ i ].name;
         break;
      }
   }

   return procedure_name;
}

string get_procedure_arg_id_for_name( model& m,
 const string& class_name, const string& procedure_name, const string& procedure_arg_name )
{
   string procedure_arg_id;

   vector< procedure_arg_data > all_procedure_args;
   m.get_procedure_arg_data( class_name, procedure_name, all_procedure_args );

   for( size_t i = 0; i < all_procedure_args.size( ); i++ )
   {
      if( all_procedure_args[ i ].name == procedure_arg_name )
      {
         procedure_arg_id = all_procedure_args[ i ].id;
         break;
      }
   }

   return procedure_arg_id;
}

string get_procedure_arg_name_for_id( model& m,
 const string& class_name, const string& procedure_name, const string& procedure_arg_id )
{
   string procedure_arg_name;

   vector< procedure_arg_data > all_procedure_args;
   m.get_procedure_arg_data( class_name, procedure_name, all_procedure_args );

   for( size_t i = 0; i < all_procedure_args.size( ); i++ )
   {
      if( all_procedure_args[ i ].id == procedure_arg_id )
      {
         procedure_arg_name = all_procedure_args[ i ].name;
         break;
      }
   }

   return procedure_arg_name;
}

string internal_type_for_field_type( const string& field_type )
{
   string internal_type;

   // FUTURE: This mapping should not be being done here.
   if( field_type == "datetime" )
      internal_type = "date_time";
   else if( field_type == "date" )
      internal_type = "udate";
   else if( field_type == "time" )
      internal_type = "mtime";
   else
      throw runtime_error( "unexpected type '" + field_type + "' for internal type mapping" );

   return internal_type;
}

int internal_type_enum_value( const string& internal_type )
{
   int rc = 0;

   if( internal_type == "date_time" )
      rc = 1;
   else if( internal_type == "udate" )
      rc = 2;
   else if( internal_type == "mtime" )
      rc = 3;

   if( rc == 0 )
      throw runtime_error( "unknown internal type '" + internal_type + "' in internal_type_enum_value( )" );

   return rc;
}

}

DEFINE_CLASS_FACTORY( std::string, specification );

struct acyclic_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   static string static_class_name( );
};

void acyclic_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) > 2 )
      throw runtime_error( "unexpected number of args > 2 for 'acyclic' specification" );

   if( args.size( ) < 1 )
      throw runtime_error( "'class name' argument is required for the 'acyclic' specification" );

   string class_id;
   string field_id;
   string relationship_id;

   string arg_class_name( args[ 0 ] );

   vector< specification_data > all_spec_data;
   m.get_specification_data( arg_class_name, all_spec_data );
   for( size_t i = 0; i < all_spec_data.size( ); i++ )
   {
      if( all_spec_data[ i ].type == static_class_name( ) )
         throw runtime_error( "specification type '" + static_class_name( ) + "' can only be used once per class" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   if( args.size( ) < 2 )
      throw runtime_error( "'relationship name' argument is required for the 'acyclic' specification" );

   string arg_relationship_name( args[ 1 ] );

   relationship_data rel_data;
   m.get_relationship_data( arg_relationship_name, rel_data );

   if( rel_data.child_class_name != rel_data.parent_class_name )
      throw runtime_error( "relationship '" + arg_relationship_name + "' is not a self-relationship" );

   if( arg_class_name != rel_data.child_class_name )
      throw runtime_error( "relationship '"
       + arg_relationship_name + "' is not a self-relationship for class '" + arg_class_name + "'" );

   relationship_id = rel_data.id;

   vector< field_data > rel_field_data;
   m.get_field_data( arg_class_name, rel_field_data, e_get_field_type_relationships );

   for( size_t i = 0; i < rel_field_data.size( ); i++ )
   {
      if( rel_field_data[ i ].decorated_name == arg_relationship_name )
      {
         field_id = rel_field_data[ i ].id;
         break;
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( relationship_id, "relationship", e_model_element_type_relationship ) );
}

string acyclic_specification::static_class_name( ) { return "acyclic"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, acyclic_specification, static_class_name );

struct check_cond_non_null_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;
   string tclass_id;

   string field_id;
   string cfield_id;
   string pfield_id;
   string tfield_id;
   string tpfield_id;

   bool is_negative;

   string test_value;
   bool test_not_equal;
};

void check_cond_non_null_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'check_cond_non_null' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_cfield_name( args[ 2 ] );

   is_negative = false;
   test_not_equal = false;

   string test_field_info;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_negative ) )
         is_negative = true;
      else if( next_arg == string( c_arg_test_not_equal ) )
         test_not_equal = true;
      else if( next_arg.find( c_arg_test_prefix ) == 0 )
         test_field_info = next_arg.substr( strlen( c_arg_test_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'check_cond_non_null' specification" );
   }

   string test_field_name;
   if( !test_field_info.empty( ) )
   {
      string::size_type pos = test_field_info.find( '=' );
      test_field_name = test_field_info.substr( 0, pos );
      if( pos != string::npos )
         test_value = test_field_info.substr( pos + 1 );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, 0, true, false, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   string pfield_name;
   string::size_type pos = arg_cfield_name.find( ':' );
   if( pos != string::npos )
   {
      pfield_name = arg_cfield_name.substr( 0, pos );
      arg_cfield_name.erase( 0, pos + 1 );
   }

   if( pfield_name.empty( ) )
   {
      cfield_id = get_field_id_for_name( m, arg_class_name, arg_cfield_name, 0, true, false, true );

      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_cfield_name + "' for class '" + arg_class_name + "'" );
   }
   else
   {
      string pfield_type;
      pfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, &pfield_type, true, false, true );

      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      string pclass_name( get_class_name_from_field_type( m, arg_class_name, pfield_name, pfield_type ) );

      pclass_id = get_class_id_for_name( m, pclass_name );
      cfield_id = get_field_id_for_name( m, pclass_name, arg_cfield_name );

      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_cfield_name + "' for class '" + pclass_name + "'" );
   }

   if( !test_field_name.empty( ) )
   {
      string test_class_name( arg_class_name );

      string test_pfield_name;
      string::size_type pos = test_field_name.find( ':' );
      if( pos != string::npos )
      {
         test_pfield_name = test_field_name.substr( 0, pos );
         test_field_name.erase( 0, pos + 1 );

         string test_pfield_type;
         tpfield_id = get_field_id_for_name( m, arg_class_name, test_pfield_name, &test_pfield_type, true, false, true );
         if( tpfield_id.empty( ) )
            throw runtime_error( "unknown field '" + test_pfield_name + "' for class '" + arg_class_name + "'" );

         string test_pclass_name( get_class_name_from_field_type( m, arg_class_name, test_pfield_name, test_pfield_type ) );

         tclass_id = get_class_id_for_name( m, test_pclass_name );
         test_class_name = test_pclass_name;
      }

      string test_field_type;
      tfield_id = get_field_id_for_name( m, test_class_name, test_field_name, &test_field_type, true, false, true );
      if( tfield_id.empty( ) )
         throw runtime_error( "unknown field '" + test_field_name + "' for class '" + test_class_name + "'" );

      if( !test_value.empty( ) )
      {
         bool is_non_string( is_non_string_type( test_field_type ) );

         if( !is_non_string )
            test_value = '"' + test_value + '"';
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !pclass_id.empty( ) )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );
   if( !tclass_id.empty( ) )
      details.push_back( specification_detail( tclass_id, "tclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );

   if( !pfield_id.empty( ) )
      details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );

   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );

   if( !tpfield_id.empty( ) )
      details.push_back( specification_detail( tpfield_id, "tpfield", e_model_element_type_field ) );
}

void check_cond_non_null_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_opt_attribute( c_attribute_pclass_id );
   tclass_id = reader.read_opt_attribute( c_attribute_tclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   cfield_id = reader.read_attribute( c_attribute_cfield_id );

   pfield_id = reader.read_opt_attribute( c_attribute_pfield_id );
   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );
   tpfield_id = reader.read_opt_attribute( c_attribute_tpfield_id );
   test_value = reader.read_opt_attribute( c_attribute_test_value );
   is_negative = ( reader.read_opt_attribute( c_attribute_is_negative ) == c_true );
   test_not_equal = ( reader.read_opt_attribute( c_attribute_test_not_equal ) == c_true );
}

void check_cond_non_null_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_opt_attribute( c_attribute_tclass_id, tclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_cfield_id, cfield_id );

   writer.write_opt_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );
   writer.write_opt_attribute( c_attribute_tpfield_id, tpfield_id );
   writer.write_opt_attribute( c_attribute_test_value, test_value );
   writer.write_opt_attribute( c_attribute_is_negative, is_negative ? c_true : "" );
   writer.write_opt_attribute( c_attribute_test_not_equal, test_not_equal ? c_true : "" );
}

void check_cond_non_null_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id, 0, true );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string pclass_name, pfield_name;
   if( pclass_id.empty( ) )
      pclass_name = class_name;
   else
   {
      pclass_name = get_class_name_for_id( m, pclass_id );
      pfield_name = get_field_name_for_id( m, class_name, pfield_id, 0, true );
   }
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string cfield_name = get_field_name_for_id( m, pclass_name, cfield_id, 0, true );
   spec_data.data_pairs.push_back( make_pair( c_data_cfield, cfield_name ) );

   string tclass_name( class_name );
   if( !tclass_id.empty( ) )
      tclass_name = get_class_name_for_id( m, tclass_id );

   string tpfield_name;
   if( !tpfield_id.empty( ) )
      tpfield_name = get_field_name_for_id( m, class_name, tpfield_id, 0, true );
   spec_data.data_pairs.push_back( make_pair( c_data_tpfield, tpfield_name ) );

   string tfield_name;
   if( !tfield_id.empty( ) )
      tfield_name = get_field_name_for_id( m, tclass_name, tfield_id, 0, true );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_is_neg ), is_negative ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_test_neq ), test_not_equal ? c_true : "" ) );
}

string check_cond_non_null_specification::static_class_name( ) { return "check_cond_non_null"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, check_cond_non_null_specification, static_class_name );

struct check_no_repeats_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;

   vector< string > field_info;

   bool skip_nulls;
};

void check_no_repeats_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 2 || args.size( ) > 3 )
      throw runtime_error( "unexpected number of args for 'check_no_repeats' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_info( args[ 1 ] );

   skip_nulls = false;
   for( size_t arg = 2; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_skip_nulls ) && !skip_nulls )
         skip_nulls = true;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'check_no_repeats' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   while( true )
   {
      string::size_type pos = arg_field_info.find( ',' );
      string next_field_name( arg_field_info.substr( 0, pos ) );

      string next_field_id = get_field_id_for_name( m, arg_class_name, next_field_name, 0, true );
      if( next_field_id.empty( ) )
         throw runtime_error( "unknown field '" + next_field_name + "' for class '" + arg_class_name + "'" );

      field_info.push_back( next_field_id );

      if( pos == string::npos )
         break;

      arg_field_info.erase( 0, pos + 1 );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   for( size_t i = 0; i < field_info.size( ); i++ )
      details.push_back( specification_detail( field_info[ i ], "field", e_model_element_type_field ) );
}

void check_no_repeats_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );

   string next;
   while( reader.has_read_attribute( c_attribute_field_info, next ) )
      field_info.push_back( next );

   skip_nulls = ( reader.read_opt_attribute( c_attribute_skip_nulls ) == c_true );
}

void check_no_repeats_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );

   for( size_t i = 0; i < field_info.size( ); i++ )
      writer.write_attribute( c_attribute_field_info, field_info[ i ] );

   writer.write_opt_attribute( c_attribute_skip_nulls, skip_nulls ? c_true : "" );
}

void check_no_repeats_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field_list;
   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_list.empty( ) )
         field_list += " ";

      string field_name = get_field_name_for_id( m, class_name, field_info[ i ] );

      field_list += field_name;
   }

   spec_data.data_pairs.push_back( make_pair( string( c_data_field_list ), field_list ) );

   spec_data.data_pairs.push_back( make_pair( c_data_skip_nulls, skip_nulls ? c_true : "" ) );
}

string check_no_repeats_specification::static_class_name( ) { return "check_no_repeats"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, check_no_repeats_specification, static_class_name );

struct check_null_match_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   static string static_class_name( );
};

void check_null_match_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 3 )
      throw runtime_error( "unexpected number of args != 3 for 'check_null_match' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field1_name( args[ 1 ] );
   string arg_field2_name( args[ 2 ] );

   string class_id;
   string field1_id;
   string field2_id;

   class_id = get_class_id_for_name( m, arg_class_name );

   field1_id = get_field_id_for_name( m, arg_class_name, arg_field1_name, 0, true );
   if( field1_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field1_name + "' for class '" + arg_class_name + "'" );

   field2_id = get_field_id_for_name( m, arg_class_name, arg_field2_name, 0, true );
   if( field2_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field2_name + "' for class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field1_id, "field1", e_model_element_type_field ) );
   details.push_back( specification_detail( field2_id, "field2", e_model_element_type_field ) );
}

string check_null_match_specification::static_class_name( ) { return "check_null_match"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, check_null_match_specification, static_class_name );

struct child_field_cascade_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string child_class_id;

   string cpfield_id;

   vector< string > field_info;
};

void child_field_cascade_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 3 )
      throw runtime_error( "unexpected number of args != 3 for 'child_field_cascade' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_info( args[ 1 ] );
   string arg_cclass_info( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   string::size_type pos = arg_cclass_info.find( ':' );
   string child_class_name( arg_cclass_info.substr( 0, pos ) );

   string cpfield_name;

   if( pos != string::npos )
      cpfield_name = arg_cclass_info.substr( pos + 1 );

   child_class_id = get_class_id_for_name( m, child_class_name );

   if( !cpfield_name.empty( ) )
   {
      string cfield_type;
      cpfield_id = get_field_id_for_name( m, child_class_name, cpfield_name, &cfield_type, true );

      if( cpfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cpfield_name + "' for class '" + child_class_name + "'" );

      string cpfield_class_name( get_class_name_from_field_type( m, child_class_name, cpfield_name, cfield_type ) );

      if( cpfield_class_name != arg_class_name )
         throw runtime_error( "child field '" + cpfield_name + "' is not class '" + arg_class_name + "'" );
   }

   vector< string > field_ids;

   while( true )
   {
      string::size_type pos = arg_field_info.find( ',' );
      string next_info( arg_field_info.substr( 0, pos ) );

      string::size_type cpos = next_info.find( '-' );
      string cfield_name( next_info.substr( 0, cpos ) );

      string pfield_name( cfield_name );
      if( cpos != string::npos )
         pfield_name = next_info.substr( cpos + 1 );

      string cfield_id = get_field_id_for_name( m, child_class_name, cfield_name, 0, true );
      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cfield_name + "' for class '" + child_class_name + "'" );

      string pfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, 0, true );
      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      field_ids.push_back( cfield_id );
      field_ids.push_back( pfield_id );

      field_info.push_back( cfield_id + "," + pfield_id );

      if( pos == string::npos )
         break;

      arg_field_info.erase( 0, pos + 1 );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   if( class_id != child_class_id )
      details.push_back( specification_detail( child_class_id, "child_class", e_model_element_type_class ) );

   if( !cpfield_id.empty( ) )
      details.push_back( specification_detail( cpfield_id, "cpfield", e_model_element_type_field ) );

   for( size_t i = 0; i < field_ids.size( ); i++ )
      details.push_back( specification_detail( field_ids[ i ], "field", e_model_element_type_field ) );
}

void child_field_cascade_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   child_class_id = reader.read_attribute( c_attribute_child_class_id );

   cpfield_id = reader.read_opt_attribute( c_attribute_cpfield_id );

   string next;

   while( reader.has_read_attribute( c_attribute_field_info, next ) )
      field_info.push_back( next );
}

void child_field_cascade_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_child_class_id, child_class_id );

   writer.write_opt_attribute( c_attribute_cpfield_id, cpfield_id );

   for( size_t i = 0; i < field_info.size( ); i++ )
      writer.write_attribute( c_attribute_field_info, field_info[ i ] );
}

void child_field_cascade_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string child_class_name = get_class_name_for_id( m, child_class_id );

   string cpfield_name;

   if( !cpfield_id.empty( ) )
   {
      cpfield_name = get_field_name_for_id( m, child_class_name, cpfield_id, 0, false, true );
      if( child_class_id != class_id )
         cpfield_name += "_{" + child_class_name + "}";
      cpfield_name = m.determine_child_rel_suffix( cpfield_name );
   }

   // KLUDGE: Assumes that class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_child, cpfield_name.empty( ) ? child_class_name : cpfield_name ) );

   string field_list;

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_list.empty( ) )
         field_list += " ";

      string::size_type pos = field_info[ i ].find( ',' );

      string cfield_name = get_field_name_for_id( m, child_class_name, field_info[ i ].substr( 0, pos ) );
      string pfield_name = get_field_name_for_id( m, class_name, field_info[ i ].substr( pos + 1 ) );

      field_list += cfield_name + "," + pfield_name;
   }

   spec_data.data_pairs.push_back( make_pair( string( c_data_field_list ), field_list ) );
   spec_data.data_pairs.push_back( make_pair( "testop", "" ) );
   spec_data.data_pairs.push_back( make_pair( "tfield", "" ) );
   spec_data.data_pairs.push_back( make_pair( "ctfield", "" ) );
   spec_data.data_pairs.push_back( make_pair( "check_orig", "" ) );
   spec_data.data_pairs.push_back( make_pair( "any_change", "" ) );
}

string child_field_cascade_specification::static_class_name( ) { return "child_field_cascade"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, child_field_cascade_specification, static_class_name );

struct child_field_change_cascade_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;

   string field_id;
   string cfield_id;
   string cpfield_id;

   bool is_exact_match;
};

void child_field_change_cascade_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'child_field_change_cascade' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_cclass_info( args[ 2 ] );

   string cfield_name;
   is_exact_match = false;

   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_exact_match )
         is_exact_match = true;
      else if( cfield_name.empty( ) )
         cfield_name = next_arg;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'child_field_change_cascade' specification" );
   }

   if( cfield_name.empty( ) )
      cfield_name = arg_field_name;

   string::size_type pos = arg_cclass_info.find( ':' );
   string cclass_name( arg_cclass_info.substr( 0, pos ) );

   string cpfield_name;
   if( pos != string::npos )
      cpfield_name = arg_cclass_info.substr( pos + 1 );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   cclass_id = get_class_id_for_name( m, cclass_name );

   cfield_id = get_field_id_for_name( m, cclass_name, cfield_name );
   if( cfield_id.empty( ) )
      throw runtime_error( "unknown field '" + cfield_name + "' for class '" + cclass_name + "'" );

   if( !cpfield_name.empty( ) )
   {
      string cfield_type;
      cpfield_id = get_field_id_for_name( m, cclass_name, cpfield_name, &cfield_type, true );

      if( cpfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cpfield_name + "' for class '" + cclass_name + "'" );

      string cpfield_class_name( get_class_name_from_field_type( m, cclass_name, cpfield_name, cfield_type ) );

      if( cpfield_class_name != arg_class_name )
         throw runtime_error( "child field '" + cpfield_name + "' is not class '" + arg_class_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( cclass_id != class_id )
      details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );
   if( !cpfield_id.empty( ) )
      details.push_back( specification_detail( cpfield_id, "cpfield", e_model_element_type_field ) );
}

void child_field_change_cascade_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   cfield_id = reader.read_attribute( c_attribute_cfield_id );
   cpfield_id = reader.read_opt_attribute( c_attribute_cpfield_id );

   is_exact_match = ( reader.read_opt_attribute( c_attribute_is_exact_match ) == c_true );
}

void child_field_change_cascade_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_cfield_id, cfield_id );
   writer.write_attribute( c_attribute_cpfield_id, cpfield_id );

   writer.write_opt_attribute( c_attribute_is_exact_match, is_exact_match ? c_true : "" );
}

void child_field_change_cascade_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   string cclass_name = get_class_name_for_id( m, cclass_id );

   string cpfield_name;
   if( !cpfield_id.empty( ) )
   {
      cpfield_name = get_field_name_for_id( m, cclass_name, cpfield_id );
      if( cclass_id != class_id )
         cpfield_name += "_{" + cclass_name + "}";
      cpfield_name = m.determine_child_rel_suffix( cpfield_name );
   }

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_child, cpfield_name.empty( ) ? cclass_name : cpfield_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string cfield_name = get_field_name_for_id( m, cclass_name, cfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_cfield, cfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_exact_match, is_exact_match ? c_true : "" ) );
}

string child_field_change_cascade_specification::static_class_name( ) { return "child_field_change_cascade"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, child_field_change_cascade_specification, static_class_name );

struct clone_children_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;
   string pfield_id;
};

void clone_children_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'clone_children' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_cclass_name( args[ 1 ] );
   string arg_pfield_name( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );
   cclass_id = get_class_id_for_name( m, arg_cclass_name );

   pfield_id = get_field_id_for_name( m, arg_cclass_name, arg_pfield_name, 0, true );
   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pfield_name + "' for class '" + arg_cclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );
   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
}

void clone_children_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );
   pfield_id = reader.read_attribute( c_attribute_pfield_id );
}

void clone_children_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );
   writer.write_attribute( c_attribute_pfield_id, pfield_id );
}

void clone_children_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   string cclass_name = get_class_name_for_id( m, cclass_id );

   string pfield_name = get_field_name_for_id( m, cclass_name, pfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_child, cclass_name ) ); // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   spec_data.data_pairs.push_back( make_pair( "ctfield", "" ) );
   spec_data.data_pairs.push_back( make_pair( "ctvalue", "" ) );
   spec_data.data_pairs.push_back( make_pair( "not_peer", "" ) );
   spec_data.data_pairs.push_back( make_pair( "copy_files", "" ) );
   spec_data.data_pairs.push_back( make_pair( "ctfistexttype", "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_ofield, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_cpfield, "" ) );
   spec_data.data_pairs.push_back( make_pair( "skip_child_updates", "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tfistexttype, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_child_class, cclass_name ) );
}

string clone_children_specification::static_class_name( ) { return "clone_children"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, clone_children_specification, static_class_name );

struct clone_children_for_create_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;
   string pclass_id;
   string gclass_id;

   string pfield_id;
   string sfield_id;
   string vsfield_id;
   string vdfield_id;
   string cpfield_id;
   string gpfield_id;
   string gcpfield_id;

   bool combine_keys;
};

void clone_children_for_create_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 5 )
      throw runtime_error( "unexpected number of args < 5 for 'clone_children_for_create' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_pfield_name( args[ 1 ] );
   string arg_sfield_name( args[ 2 ] );
   string arg_cclass_info( args[ 3 ] );
   string arg_gcclass_info( args[ 4 ] );

   string arg_gpfield_name;
   if( args.size( ) > 5 )
      arg_gpfield_name = args[ 5 ];

   string arg_value_info;
   if( args.size( ) > 6 )
      arg_value_info = args[ 6 ];

   combine_keys = false;
   for( size_t arg = 7; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_combine_keys ) )
         combine_keys = true;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'clone_children_for_create' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string::size_type pos = arg_gcclass_info.find( ':' );
   string gcclass_name( arg_gcclass_info.substr( 0, pos ) );

   gclass_id = get_class_id_for_name( m, gcclass_name );

   string gcpfield_name;
   if( pos != string::npos )
      gcpfield_name = arg_gcclass_info.substr( pos + 1 );

   string pfield_type;
   pfield_id = get_field_id_for_name( m, arg_class_name, arg_pfield_name, &pfield_type, true );
   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pfield_name + "' for class '" + arg_class_name + "'" );

   string pfield_class_name( get_class_name_from_field_type( m, arg_class_name, arg_pfield_name, pfield_type ) );

   string sfield_type;
   sfield_id = get_field_id_for_name( m, gcclass_name, arg_sfield_name, &sfield_type, true );
   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + gcclass_name + "'" );

   string sfield_class_name( get_class_name_from_field_type( m, arg_class_name, arg_sfield_name, sfield_type ) );

   if( sfield_class_name != arg_class_name )
      throw runtime_error( "sfield class '" + sfield_class_name + "' is not class '" + arg_class_name + "'" );

   pclass_id = get_class_id_for_name( m, pfield_class_name );

   pos = arg_cclass_info.find( ':' );
   string cclass_name( arg_cclass_info.substr( 0, pos ) );

   cclass_id = get_class_id_for_name( m, cclass_name );

   string cpfield_name;
   if( pos != string::npos )
      cpfield_name = arg_cclass_info.substr( pos + 1 );

   if( !cpfield_name.empty( ) )
   {
      string cfield_type;
      cpfield_id = get_field_id_for_name( m, cclass_name, cpfield_name, &cfield_type, true );

      if( cpfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cpfield_name + "' for class '" + cclass_name + "'" );

      string cpfield_class_name( get_class_name_from_field_type( m, cclass_name, cpfield_name, cfield_type ) );

      if( cpfield_class_name != cclass_name )
         throw runtime_error( "child field '" + cpfield_name + "' is not class '" + cclass_name + "'" );
   }

   if( !gcpfield_name.empty( ) )
   {
      gcpfield_id = get_field_id_for_name( m, gcclass_name, gcpfield_name, 0, true );

      if( gcpfield_id.empty( ) )
         throw runtime_error( "unknown field '" + gcpfield_name + "' for class '" + gcclass_name + "'" );
   }

   if( !arg_gpfield_name.empty( ) )
   {
      gpfield_id = get_field_id_for_name( m, gcclass_name, arg_gpfield_name, 0, true );

      if( gpfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_gpfield_name + "' for class '" + gcclass_name + "'" );
   }

   if( !arg_value_info.empty( ) )
   {
      pos = arg_value_info.find( '-' );
      string vsfield_name( arg_value_info.substr( 0, pos ) );

      vsfield_id = get_field_id_for_name( m, arg_class_name, vsfield_name );

      if( vsfield_id.empty( ) )
         throw runtime_error( "unknown field '" + vsfield_name + "' for class '" + arg_class_name + "'" );

      string vdfield_name;
      if( pos == string::npos )
         vdfield_name = vsfield_name;
      else
         vdfield_name = arg_value_info.substr( pos + 1 );

      vdfield_id = get_field_id_for_name( m, gcclass_name, vdfield_name );

      if( vdfield_id.empty( ) )
         throw runtime_error( "unknown field '" + vdfield_name + "' for class '" + gcclass_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( cclass_id != class_id )
      details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );
   details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );
   if( gclass_id != class_id )
      details.push_back( specification_detail( gclass_id, "gclass", e_model_element_type_class ) );

   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );

   if( !cpfield_id.empty( ) )
      details.push_back( specification_detail( cpfield_id, "cpfield", e_model_element_type_field ) );

   if( !gpfield_id.empty( ) )
      details.push_back( specification_detail( gpfield_id, "gpfield", e_model_element_type_field ) );

   if( !gcpfield_id.empty( ) )
      details.push_back( specification_detail( gcpfield_id, "gcpfield", e_model_element_type_field ) );

   if( !vsfield_id.empty( ) )
      details.push_back( specification_detail( vsfield_id, "vsfield", e_model_element_type_field ) );

   if( !vdfield_id.empty( ) )
      details.push_back( specification_detail( vdfield_id, "vdfield", e_model_element_type_field ) );
}

void clone_children_for_create_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );
   pclass_id = reader.read_attribute( c_attribute_pclass_id );
   gclass_id = reader.read_attribute( c_attribute_gclass_id );

   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   sfield_id = reader.read_attribute( c_attribute_sfield_id );

   vsfield_id = reader.read_opt_attribute( c_attribute_vsfield_id );
   vdfield_id = reader.read_opt_attribute( c_attribute_vdfield_id );
   cpfield_id = reader.read_opt_attribute( c_attribute_cpfield_id );
   gpfield_id = reader.read_opt_attribute( c_attribute_gpfield_id );
   gcpfield_id = reader.read_opt_attribute( c_attribute_gcpfield_id );

   combine_keys = ( reader.read_opt_attribute( c_attribute_combine_keys ) == c_true );
}

void clone_children_for_create_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );
   writer.write_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_attribute( c_attribute_gclass_id, gclass_id );

   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_sfield_id, sfield_id );

   writer.write_opt_attribute( c_attribute_vsfield_id, vsfield_id );
   writer.write_opt_attribute( c_attribute_vdfield_id, vdfield_id );
   writer.write_opt_attribute( c_attribute_cpfield_id, cpfield_id );
   writer.write_opt_attribute( c_attribute_gpfield_id, gpfield_id );
   writer.write_opt_attribute( c_attribute_gcpfield_id, gcpfield_id );

   writer.write_opt_attribute( c_attribute_combine_keys, combine_keys ? c_true : "" );
}

void clone_children_for_create_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pfield_name = get_field_name_for_id( m, class_name, pfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string gclass_name = get_class_name_for_id( m, gclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_gclass, gclass_name ) );

   string sfield_name = get_field_name_for_id( m, gclass_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   string cclass_name = get_class_name_for_id( m, cclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_cclass, cclass_name ) );

   string pclass_name = get_class_name_for_id( m, pclass_id );

   string cpfield_name;
   if( !cpfield_id.empty( ) )
   {
      cpfield_name = get_field_name_for_id( m, pclass_name, cpfield_id, 0, false, true );
      cpfield_name = m.determine_child_rel_suffix( cpfield_name );
   }

   // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_child, cpfield_name.empty( ) ? cclass_name : cpfield_name ) );

   string gpfield_name;
   if( !gpfield_id.empty( ) )
      gpfield_name = get_field_name_for_id( m, gclass_name, gpfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_gpfield, gpfield_name.empty( ) ? pfield_name : gpfield_name ) );

   string gcpfield_name;
   if( !gcpfield_id.empty( ) )
   {
      gcpfield_name = get_field_name_for_id( m, gclass_name, gcpfield_id );
      gcpfield_name = m.determine_child_rel_suffix( gcpfield_name );
   }

   spec_data.data_pairs.push_back( make_pair( c_data_gchild, gcpfield_name.empty( ) ? gclass_name : gcpfield_name ) );

   string vsfield_name;
   if( !vsfield_id.empty( ) )
      vsfield_name = get_field_name_for_id( m, class_name, vsfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_vsfield, vsfield_name ) );

   string vdfield_name;
   if( !vdfield_id.empty( ) )
      vdfield_name = get_field_name_for_id( m, gclass_name, vdfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_vdfield, vdfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_combine_keys, combine_keys ? c_true : "" ) );
}

string clone_children_for_create_specification::static_class_name( ) { return "clone_children_for_create"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, clone_children_for_create_specification, static_class_name );

struct clone_children_for_update_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;

   string pfield_id;
   string sfield_id;
   string cfield_id;
   string xfield_id;
};

void clone_children_for_update_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'clone_children_for_update' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_pfield_name( args[ 1 ] );
   string arg_sfield_name( args[ 2 ] );
   string arg_cclass_info( args[ 3 ] );

   string arg_xfield_name;
   if( args.size( ) > 4 )
      arg_xfield_name = args[ 4 ];

   class_id = get_class_id_for_name( m, arg_class_name );

   pfield_id = get_field_id_for_name( m, arg_class_name, arg_pfield_name, 0, true );
   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pfield_name + "' for class '" + arg_class_name + "'" );

   string sfield_type;
   sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_name, &sfield_type, true );
   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + arg_class_name + "'" );

   string sfield_class_name( get_class_name_from_field_type( m, arg_class_name, arg_sfield_name, sfield_type ) );

   if( sfield_class_name != arg_class_name )
      throw runtime_error( "sfield class '" + sfield_class_name + "' is not class '" + arg_class_name + "'" );

   string::size_type pos = arg_cclass_info.find( ':' );
   string cclass_name( arg_cclass_info.substr( 0, pos ) );

   if( cclass_name != arg_class_name )
      throw runtime_error( "child class '" + cclass_name + "' is not class '" + arg_class_name + "'" );

   string cfield_name;
   if( pos != string::npos )
      cfield_name = arg_cclass_info.substr( pos + 1 );

   if( !cfield_name.empty( ) )
   {
      string cfield_type;
      cfield_id = get_field_id_for_name( m, cclass_name, cfield_name, &cfield_type, true );

      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cfield_name + "' for class '" + cclass_name + "'" );

      string cfield_class_name( get_class_name_from_field_type( m, cclass_name, cfield_name, cfield_type ) );

      if( cfield_class_name != cclass_name )
         throw runtime_error( "child field '" + cfield_name + "' is not class '" + cclass_name + "'" );
   }

   if( !arg_xfield_name.empty( ) )
   {
      xfield_id = get_field_id_for_name( m, arg_class_name, arg_xfield_name, 0, true );
      if( xfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_xfield_name + "' for class '" + arg_class_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );

   if( !cfield_id.empty( ) )
      details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );

   if( !xfield_id.empty( ) )
      details.push_back( specification_detail( sfield_id, "xfield", e_model_element_type_field ) );
}

void clone_children_for_update_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   sfield_id = reader.read_attribute( c_attribute_sfield_id );

   cfield_id = reader.read_opt_attribute( c_attribute_cfield_id );
   xfield_id = reader.read_opt_attribute( c_attribute_xfield_id );
}

void clone_children_for_update_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_sfield_id, sfield_id );

   writer.write_opt_attribute( c_attribute_cfield_id, cfield_id );
   writer.write_opt_attribute( c_attribute_xfield_id, xfield_id );
}

void clone_children_for_update_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pfield_name = get_field_name_for_id( m, class_name, pfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string sfield_name = get_field_name_for_id( m, class_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   string cfield_name;
   if( !cfield_id.empty( ) )
   {
      cfield_name = get_field_name_for_id( m, class_name, cfield_id, 0, false, true );
      cfield_name = m.determine_child_rel_suffix( cfield_name );
   }

   // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_child, cfield_name.empty( ) ? class_name : cfield_name ) );

   string xfield_name;
   if( !xfield_id.empty( ) )
      xfield_name = get_field_name_for_id( m, class_name, xfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_xfield, xfield_name ) );
}

string clone_children_for_update_specification::static_class_name( ) { return "clone_children_for_update"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, clone_children_for_update_specification, static_class_name );

struct clone_children_from_fk_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;

   string pfield_id;
   string fkfield_id;
   string cpfield_id;
};

void clone_children_from_fk_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'clone_children_from_fk' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_cclass_name( args[ 1 ] );
   string arg_pfield_name( args[ 2 ] );
   string arg_fkfield_name( args[ 3 ] );
   string arg_cpfield_name;

   if( args.size( ) > 4 )
      arg_cpfield_name = args[ 4 ];

   class_id = get_class_id_for_name( m, arg_class_name );
   cclass_id = get_class_id_for_name( m, arg_cclass_name );

   pfield_id = get_field_id_for_name( m, arg_cclass_name, arg_pfield_name, 0, true );
   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pfield_name + "' for class '" + arg_cclass_name + "'" );

   fkfield_id = get_field_id_for_name( m, arg_class_name, arg_fkfield_name, 0, true );
   if( fkfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_fkfield_name + "' for class '" + arg_class_name + "'" );

   if( !arg_cpfield_name.empty( ) )
   {
      cpfield_id = get_field_id_for_name( m, arg_cclass_name, arg_cpfield_name, 0, true );
      if( cpfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_cpfield_name + "' for class '" + arg_cclass_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( cclass_id != class_id )
      details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );

   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( fkfield_id, "fkfield", e_model_element_type_field ) );
   if( !cpfield_id.empty( ) )
      details.push_back( specification_detail( cpfield_id, "cpfield", e_model_element_type_field ) );
}

void clone_children_from_fk_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );
   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   fkfield_id = reader.read_attribute( c_attribute_fkfield_id );
   cpfield_id = reader.read_opt_attribute( c_attribute_cpfield_id );
}

void clone_children_from_fk_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );
   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_fkfield_id, fkfield_id );
   writer.write_opt_attribute( c_attribute_cpfield_id, cpfield_id );
}

void clone_children_from_fk_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   string cclass_name = get_class_name_for_id( m, cclass_id );

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_child, cclass_name ) ); // KLUDGE: Assumes class name is the child name.

   string pfield_name = get_field_name_for_id( m, cclass_name, pfield_id );
   string fkfield_name = get_field_name_for_id( m, class_name, fkfield_id );

   string cpfield_name;
   if( !cpfield_id.empty( ) )
      cpfield_name = get_field_name_for_id( m, cclass_name, cpfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fkchild, cclass_name ) ); // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_fkfield, fkfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_cpfield, cpfield_name ) );

   spec_data.data_pairs.push_back( make_pair( "cclass", cclass_name ) ); // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( "dfield", "" ) );
   spec_data.data_pairs.push_back( make_pair( "procedure", "" ) );
}

string clone_children_from_fk_specification::static_class_name( ) { return "clone_children_from_fk"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, clone_children_from_fk_specification, static_class_name );

struct clone_default_for_group_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;
   string gfield_id;
};

void clone_default_for_group_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 3 )
      throw runtime_error( "unexpected args != 3 for 'clone_default_for_group' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_cclass_name( args[ 1 ] );
   string arg_gfield_name( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );
   cclass_id = get_class_id_for_name( m, arg_cclass_name );

   gfield_id = get_field_id_for_name( m, arg_cclass_name, arg_gfield_name, 0, true );
   if( gfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_gfield_name + "' for class '" + arg_cclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );

   details.push_back( specification_detail( gfield_id, "gfield", e_model_element_type_field ) );
}

void clone_default_for_group_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );
   gfield_id = reader.read_attribute( c_attribute_gfield_id );
}

void clone_default_for_group_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );
   writer.write_attribute( c_attribute_gfield_id, gfield_id );
}

void clone_default_for_group_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   string cclass_name = get_class_name_for_id( m, cclass_id );
   string gfield_name = get_field_name_for_id( m, cclass_name, gfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_cclass, cclass_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_gfield, gfield_name ) );
}

string clone_default_for_group_specification::static_class_name( ) { return "clone_default_for_group"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, clone_default_for_group_specification, static_class_name );

struct clone_default_group_children_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;
   string gfield_id;
};

void clone_default_group_children_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 3 )
      throw runtime_error( "unexpected args != 3 for 'clone_default_group_children' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_cclass_name( args[ 1 ] );
   string arg_gfield_name( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );
   cclass_id = get_class_id_for_name( m, arg_cclass_name );

   gfield_id = get_field_id_for_name( m, arg_cclass_name, arg_gfield_name, 0, true );
   if( gfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_gfield_name + "' for class '" + arg_cclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );

   details.push_back( specification_detail( gfield_id, "gfield", e_model_element_type_field ) );
}

void clone_default_group_children_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );
   gfield_id = reader.read_attribute( c_attribute_gfield_id );
}

void clone_default_group_children_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );
   writer.write_attribute( c_attribute_gfield_id, gfield_id );
}

void clone_default_group_children_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   string cclass_name = get_class_name_for_id( m, cclass_id );
   string gfield_name = get_field_name_for_id( m, cclass_name, gfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_cclass, cclass_name ) );

   // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_child, cclass_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_gfield, gfield_name ) );
}

string clone_default_group_children_specification::static_class_name( ) { return "clone_default_group_children"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, clone_default_group_children_specification, static_class_name );

struct copy_child_links_from_fk_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;
   string cfield_id;

   string fkclass_id;
   string fkcclass_id;
   string fkcfield_id;

   vector< string > field_info;

   bool not_create;
   bool self_child;
   bool combine_keys;

   string cfk_from_field_id;
};

void copy_child_links_from_fk_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 5 )
      throw runtime_error( "unexpected number of args < 5 for 'copy_child_links_from_fk' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_cclass_name( args[ 1 ] );
   string arg_fkclass_name( args[ 2 ] );
   string arg_fkcclass_name( args[ 3 ] );
   string arg_field_info( args[ 4 ] );

   not_create = false;
   self_child = false;
   combine_keys = false;

   string arg_fk_child_field;

   for( size_t arg = 5; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_not_create )
         not_create = true;
      else if( next_arg == c_arg_self_child )
         self_child = true;
      else if( next_arg == string( c_arg_combine_keys ) )
         combine_keys = true;
      else if( next_arg.find( c_arg_child_field_prefix ) == 0 )
         arg_fk_child_field = next_arg.substr( strlen( c_arg_child_field_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'copy_child_links_from_fk' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   if( self_child )
   {
      cclass_id = class_id;

      cfield_id = get_field_id_for_name( m, arg_class_name, arg_cclass_name, 0, true );
      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_cclass_name + "' for class '" + arg_class_name + "'" );

      arg_cclass_name = arg_class_name;
   }
   else
      cclass_id = get_class_id_for_name( m, arg_cclass_name );

   fkclass_id = get_class_id_for_name( m, arg_fkclass_name );

   if( self_child )
   {
      fkcclass_id = fkclass_id;

      fkcfield_id = get_field_id_for_name( m, arg_fkclass_name, arg_fkcclass_name, 0, true );
      if( fkcfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_fkcclass_name + "' for class '" + arg_fkclass_name + "'" );

      arg_fkcclass_name = arg_fkclass_name;
   }
   else
      fkcclass_id = get_class_id_for_name( m, arg_fkcclass_name );

   vector< string > field_ids;
   while( !arg_field_info.empty( ) )
   {
      string::size_type pos = arg_field_info.find( ',' );
      string next_info( arg_field_info.substr( 0, pos ) );

      string::size_type fkcpos = next_info.find( '-' );
      string cfield_name( next_info.substr( 0, fkcpos ) );

      string fkcfield_name( cfield_name );
      if( fkcpos != string::npos )
         fkcfield_name = next_info.substr( fkcpos + 1 );

      string cfield_id = get_field_id_for_name( m, arg_cclass_name, cfield_name, 0, true );
      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cfield_name + "' for class '" + arg_cclass_name + "'" );

      string fkcfield_id = get_field_id_for_name( m, arg_fkcclass_name, fkcfield_name, 0, true );
      if( fkcfield_id.empty( ) )
         throw runtime_error( "unknown field '" + fkcfield_name + "' for class '" + arg_fkcclass_name + "'" );

      field_ids.push_back( cfield_id );
      field_ids.push_back( fkcfield_id );

      field_info.push_back( cfield_id + "," + fkcfield_id );

      if( pos == string::npos )
         break;

      arg_field_info.erase( 0, pos + 1 );
   }

   if( !arg_fk_child_field.empty( ) )
   {
      cfk_from_field_id = get_field_id_for_name( m, arg_cclass_name, arg_fk_child_field, 0, true );
      if( cfk_from_field_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_fk_child_field + "' for class '" + arg_cclass_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( cclass_id != class_id )
      details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );

   details.push_back( specification_detail( fkclass_id, "fkclass", e_model_element_type_class ) );
   if( fkcclass_id != fkclass_id )
      details.push_back( specification_detail( fkcclass_id, "fkcclass", e_model_element_type_class ) );

   for( size_t i = 0; i < field_ids.size( ); i++ )
      details.push_back( specification_detail( field_ids[ i ], "field", e_model_element_type_field ) );

   if( !cfield_id.empty( ) )
      details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );

   if( !fkcfield_id.empty( ) )
      details.push_back( specification_detail( fkcfield_id, "fkcfield", e_model_element_type_field ) );

   if( !arg_fk_child_field.empty( ) )
      details.push_back( specification_detail( cfk_from_field_id, "cfk_from_field", e_model_element_type_field ) );
}

void copy_child_links_from_fk_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );
   fkclass_id = reader.read_attribute( c_attribute_fkclass_id );
   fkcclass_id = reader.read_attribute( c_attribute_fkcclass_id );

   string next;
   while( reader.has_read_attribute( c_attribute_field_info, next ) )
      field_info.push_back( next );

   not_create = ( reader.read_opt_attribute( c_attribute_not_create ) == c_true );
   self_child = ( reader.read_opt_attribute( c_attribute_self_child ) == c_true );
   combine_keys = ( reader.read_opt_attribute( c_attribute_combine_keys ) == c_true );

   cfield_id = reader.read_opt_attribute( c_attribute_cfield_id );
   fkcfield_id = reader.read_opt_attribute( c_attribute_fkcfield_id );
   cfk_from_field_id = reader.read_opt_attribute( c_attribute_cfk_from_field_id );
}

void copy_child_links_from_fk_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );
   writer.write_attribute( c_attribute_fkclass_id, fkclass_id );
   writer.write_attribute( c_attribute_fkcclass_id, fkcclass_id );

   for( size_t i = 0; i < field_info.size( ); i++ )
      writer.write_attribute( c_attribute_field_info, field_info[ i ] );

   writer.write_opt_attribute( c_attribute_not_create, not_create ? c_true : "" );
   writer.write_opt_attribute( c_attribute_self_child, self_child ? c_true : "" );
   writer.write_opt_attribute( c_attribute_combine_keys, combine_keys ? c_true : "" );

   writer.write_opt_attribute( c_attribute_cfield_id, cfield_id );
   writer.write_opt_attribute( c_attribute_fkcfield_id, fkcfield_id );
   writer.write_opt_attribute( c_attribute_cfk_from_field_id, cfk_from_field_id );
}

void copy_child_links_from_fk_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string cclass_name = get_class_name_for_id( m, cclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_cclass, cclass_name ) );

   string fkclass_name = get_class_name_for_id( m, fkclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_fkclass, fkclass_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fkfield, fkclass_name ) ); // KLUDGE: Assumes same name as class.

   string fkcclass_name = get_class_name_for_id( m, fkcclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_fkcclass, fkcclass_name ) );

   string field_list;
   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_list.empty( ) )
         field_list += " ";

      string::size_type pos = field_info[ i ].find( ',' );

      string cfield_name = get_field_name_for_id( m, cclass_name, field_info[ i ].substr( 0, pos ) );
      string fkcfield_name = get_field_name_for_id( m, fkcclass_name, field_info[ i ].substr( pos + 1 ) );

      field_list += cfield_name + "," + fkcfield_name;
   }

   spec_data.data_pairs.push_back( make_pair( string( c_data_fields ), field_list ) );

   spec_data.data_pairs.push_back( make_pair( c_data_not_create, not_create ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_self_child, self_child ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_combine_keys, combine_keys ? c_true : "" ) );

   string cfield_name, cpfield_name;
   if( !cfield_id.empty( ) )
   {
      cfield_name = get_field_name_for_id( m, cclass_name, cfield_id, 0, false, true );
      if( cclass_id != class_id )
         cfield_name += "_{" + class_name + "}";
      cfield_name = m.determine_child_rel_suffix( cfield_name );

      cpfield_name = get_field_name_for_id( m, cclass_name, cfield_id );
   }
   spec_data.data_pairs.push_back( make_pair( c_data_cfield, cfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_cpfield, cpfield_name ) );

   string fkcfield_name;
   if( !fkcfield_id.empty( ) )
   {
      fkcfield_name = get_field_name_for_id( m, fkclass_name, fkcfield_id, 0, false, true );
      if( fkcclass_id != fkclass_id )
         fkcfield_name += "_{" + fkclass_name + "}";
      fkcfield_name = m.determine_child_rel_suffix( fkcfield_name );
   }
   spec_data.data_pairs.push_back( make_pair( c_data_fkcfield, fkcfield_name ) );

   string ckf_from_field_name;
   if( !cfk_from_field_id.empty( ) )
      ckf_from_field_name = get_field_name_for_id( m, cclass_name, cfk_from_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_fk_from_child, ckf_from_field_name ) );

   spec_data.data_pairs.push_back( make_pair( "skip_child_updates", "" ) );
}

string copy_child_links_from_fk_specification::static_class_name( ) { return "copy_child_links_from_fk"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, copy_child_links_from_fk_specification, static_class_name );

struct default_from_key_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string field_id;

   string append_value;
   string transform_func;

   bool for_store;
   bool check_null;
   bool not_default;
   bool include_clones;
};

void default_from_key_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 2 )
      throw runtime_error( "unexpected number of args < 2 for 'default_from_key' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );

   for_store = false;
   check_null = false;
   not_default = false;
   include_clones = false;

   for( size_t arg = 2; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_for_store )
         for_store = true;
      else if( next_arg == c_arg_check_null )
         check_null = true;
      else if( next_arg == c_arg_not_default )
         not_default = true;
      else if( next_arg == c_arg_include_clones )
         include_clones = true;
      else if( next_arg.find( c_arg_append_prefix ) == 0 )
         append_value = next_arg.substr( strlen( c_arg_append_prefix ) );
      else if( next_arg.find( c_arg_func_prefix ) == 0 )
         transform_func = next_arg.substr( strlen( c_arg_func_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'default_from_key' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string field_type;
   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, &field_type, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   if( !append_value.empty( ) )
   {
      bool is_non_string( is_non_string_type( field_type ) );
      if( !is_non_string )
         append_value = '"' + append_value + '"';
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

void default_from_key_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );

   for_store = ( reader.read_opt_attribute( c_attribute_for_store ) == c_true );
   check_null = ( reader.read_opt_attribute( c_attribute_check_null ) == c_true );
   not_default = ( reader.read_opt_attribute( c_attribute_not_default ) == c_true );
   include_clones = ( reader.read_opt_attribute( c_attribute_include_clones ) == c_true );

   append_value = reader.read_opt_attribute( c_attribute_append_value );
   transform_func = reader.read_opt_attribute( c_attribute_function );
}

void default_from_key_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );

   writer.write_opt_attribute( c_attribute_for_store, for_store ? c_true : "" );
   writer.write_opt_attribute( c_attribute_check_null, check_null ? c_true : "" );
   writer.write_opt_attribute( c_attribute_not_default, not_default ? c_true : "" );
   writer.write_opt_attribute( c_attribute_include_clones, include_clones ? c_true : "" );

   writer.write_opt_attribute( c_attribute_append_value, append_value );
   writer.write_opt_attribute( c_attribute_function, transform_func );
}

void default_from_key_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   bool is_mandatory;
   string field_type;
   string field_name = get_field_name_for_id( m, class_name, field_id, &field_type, false, false, &is_mandatory );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_for_store, for_store ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_chk_null, check_null ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_not_dflt, not_default ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_inc_clones, include_clones ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_fmandatory, is_mandatory ? "1" : "0" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_append, append_value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_func, transform_func ) );
   spec_data.data_pairs.push_back( make_pair( "skip_fk_handling", "" ) );
}

string default_from_key_specification::static_class_name( ) { return "default_from_key"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, default_from_key_specification, static_class_name );

struct default_group_fk_for_user_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string uclass_id;

   string pfield_id;
   string ufield_id;
   string gfield_id;
};

void default_group_fk_for_user_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 4 )
      throw runtime_error( "unexpected number of args != 4 for 'default_group_fk_for_user' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_pfield_name( args[ 1 ] );
   string arg_ufield_name( args[ 2 ] );
   string arg_gfield_name( args[ 3 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   pfield_id = get_field_id_for_name( m, arg_class_name, arg_pfield_name, 0, true );

   string ufield_type;
   ufield_id = get_field_id_for_name( m, arg_class_name, arg_ufield_name, &ufield_type, true );
   if( ufield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_ufield_name + "' for class '" + arg_class_name + "'" );

   string user_class_name( get_class_name_from_field_type( m, arg_class_name, arg_ufield_name, ufield_type ) );

   if( user_class_name.empty( ) )
      throw runtime_error( "unexpected non-parent field '"
       + arg_ufield_name + "' was provided to 'default_group_fk_for_user' specification" );

   uclass_id = get_class_id_for_name( m, user_class_name );

   gfield_id = get_field_id_for_name( m, user_class_name, arg_gfield_name, 0, true );
   if( gfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_gfield_name + "' for class '" + user_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( uclass_id, "uclass", e_model_element_type_class ) );

   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( ufield_id, "ufield", e_model_element_type_field ) );
   details.push_back( specification_detail( gfield_id, "gfield", e_model_element_type_field ) );
}

void default_group_fk_for_user_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   uclass_id = reader.read_attribute( c_attribute_uclass_id );

   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   ufield_id = reader.read_attribute( c_attribute_ufield_id );
   gfield_id = reader.read_attribute( c_attribute_gfield_id );
}

void default_group_fk_for_user_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_uclass_id, uclass_id );

   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_ufield_id, ufield_id );
   writer.write_attribute( c_attribute_gfield_id, gfield_id );
}

void default_group_fk_for_user_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   string uclass_name = get_class_name_for_id( m, uclass_id );

   string pfield_name = get_field_name_for_id( m, class_name, pfield_id );
   string ufield_name = get_field_name_for_id( m, class_name, ufield_id );
   string gfield_name = get_field_name_for_id( m, uclass_name, gfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_uclass, uclass_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_ufield, ufield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_gfield, gfield_name ) );
}

string default_group_fk_for_user_specification::static_class_name( ) { return "default_group_fk_for_user"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, default_group_fk_for_user_specification, static_class_name );

struct default_to_field_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string field_id;

   string pclass_id;
   string pfield_id;
   string sfield_id;

   string tclass_id;
   string tfield_id;
   string tpfield_id;
   string test_value;

   string append_value;

   string convertor;

   bool for_store;
   bool not_default;
};

void default_to_field_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'default_to_field' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_sfield_name( args[ 2 ] );

   for_store = false;
   not_default = false;
   string test_field_info;

   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_for_store )
         for_store = true;
      else if( next_arg == c_arg_not_default )
         not_default = true;
      else if( next_arg.find( c_arg_test_prefix ) == 0 )
         test_field_info = next_arg.substr( strlen( c_arg_test_prefix ) );
      else if( next_arg.find( c_arg_append_prefix ) == 0 )
         append_value = next_arg.substr( strlen( c_arg_append_prefix ) );
      else if( next_arg.find( c_arg_convertor_prefix ) == 0 )
         convertor = next_arg.substr( strlen( c_arg_convertor_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'default_to_field' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string field_type;
   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, &field_type, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   string pfield_name;
   bool is_external_parent = false;
   string::size_type pos = arg_sfield_name.find( ':' );
   if( pos != string::npos )
   {
      pfield_name = arg_sfield_name.substr( 0, pos );
      arg_sfield_name.erase( 0, pos + 1 );

      string pfield_type;
      pfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, &pfield_type, true );

      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      string parent_class_name( get_class_name_from_field_type( m, arg_class_name, pfield_name, pfield_type ) );

      if( parent_class_name.empty( ) )
         throw runtime_error( "unexpected non-parent field '"
          + pfield_name + "' was provided to 'default_to_field' specification" );

      pclass_id = get_class_id_for_name( m, parent_class_name, &is_external_parent );

      sfield_id = get_field_id_for_name( m, parent_class_name, arg_sfield_name, 0, true, false, true );
      if( sfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + parent_class_name + "'" );
   }
   else
   {
      sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_name, 0, true, false, true );
      if( sfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + arg_class_name + "'" );
   }

   if( !test_field_info.empty( ) )
   {
      string::size_type pos = test_field_info.find( '=' );
      string test_field_name( test_field_info.substr( 0, pos ) );

      string test_field_class( arg_class_name );

      string test_pfield_name;
      string::size_type ppos = test_field_name.find( ':' );
      if( ppos != string::npos )
      {
         test_pfield_name = test_field_name.substr( 0, ppos );
         test_field_name.erase( 0, ppos + 1 );

         string test_pfield_type;
         tpfield_id = get_field_id_for_name( m, arg_class_name, test_pfield_name, &test_pfield_type, true );
         if( tpfield_id.empty( ) )
            throw runtime_error( "unknown field '" + test_pfield_name + "' for class '" + arg_class_name + "'" );

         string test_pclass_name( get_class_name_from_field_type( m, arg_class_name, test_pfield_name, test_pfield_type ) );

         tclass_id = get_class_id_for_name( m, test_pclass_name );
         test_field_class = test_pclass_name;
      }

      string test_field_type;
      tfield_id = get_field_id_for_name( m, test_field_class, test_field_name, &test_field_type, true );
      if( tfield_id.empty( ) )
         throw runtime_error( "unknown field '" + test_field_name + "' for class '" + arg_class_name + "'" );

      if( pos != string::npos )
      {
         test_value = test_field_info.substr( pos + 1 );

         bool is_non_string( is_non_string_type( test_field_type ) );

         if( !is_non_string )
            test_value = '"' + test_value + '"';
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !pclass_id.empty( ) && pclass_id != class_id )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );
   if( !tclass_id.empty( ) )
      details.push_back( specification_detail( tclass_id, "tclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   if( !pfield_id.empty( ) )
      details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );

   if( !is_external_parent )
      details.push_back( specification_detail( sfield_id, "src_field", e_model_element_type_field ) );

   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );
   if( !tpfield_id.empty( ) )
      details.push_back( specification_detail( tpfield_id, "tpfield", e_model_element_type_field ) );
}

void default_to_field_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   pclass_id = reader.read_attribute( c_attribute_pclass_id );
   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   sfield_id = reader.read_attribute( c_attribute_sfield_id );

   tclass_id = reader.read_opt_attribute( c_attribute_tclass_id );
   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );
   tpfield_id = reader.read_opt_attribute( c_attribute_tpfield_id );
   test_value = reader.read_opt_attribute( c_attribute_test_value );

   append_value = reader.read_opt_attribute( c_attribute_append_value );

   for_store = ( reader.read_opt_attribute( c_attribute_for_store ) == c_true );
   not_default = ( reader.read_opt_attribute( c_attribute_not_default ) == c_true );

   convertor = reader.read_opt_attribute( c_attribute_convertor );
}

void default_to_field_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_sfield_id, sfield_id );

   writer.write_opt_attribute( c_attribute_tclass_id, tclass_id );
   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );
   writer.write_opt_attribute( c_attribute_tpfield_id, tpfield_id );
   writer.write_opt_attribute( c_attribute_test_value, test_value );

   writer.write_opt_attribute( c_attribute_append_value, append_value );

   writer.write_opt_attribute( c_attribute_for_store, for_store ? c_true : "" );
   writer.write_opt_attribute( c_attribute_not_default, not_default ? c_true : "" );

   writer.write_opt_attribute( c_attribute_convertor, convertor );
}

void default_to_field_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   bool is_mandatory;
   string field_type;
   string field_name = get_field_name_for_id( m, class_name, field_id, &field_type, false, false, &is_mandatory );

   bool is_text_type( !is_non_string_type( field_type ) );

   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fmandatory, is_mandatory ? "1" : "0" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fistexttype, is_text_type ? "1" : "0" ) );

   string pclass_name, pfield_name, sfield_name;

   if( !pclass_id.empty( ) )
   {
      pclass_name = get_class_name_for_id( m, pclass_id );
      pfield_name = get_field_name_for_id( m, class_name, pfield_id );
      sfield_name = get_field_name_for_id( m, pclass_name, sfield_id, 0, true );
   }
   else
      sfield_name = get_field_name_for_id( m, class_name, sfield_id, 0, true );

   spec_data.data_pairs.push_back( make_pair( c_data_pclass, pclass_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   string tpfield_name;
   if( !tpfield_id.empty( ) )
      tpfield_name = get_field_name_for_id( m, class_name, tpfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tpfield, tpfield_name ) );

   string tclass_name( class_name );
   if( !tclass_id.empty( ) )
      tclass_name = get_class_name_for_id( m, tclass_id );

   string tfield_name, tfield_type;
   if( !tfield_id.empty( ) )
      tfield_name = get_field_name_for_id( m, tclass_name, tfield_id, &tfield_type );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   bool is_ttext_type( !is_non_string_type( tfield_type ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tfistexttype, is_ttext_type ? "1" : "0" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_append, append_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_for_store, for_store ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_not_dflt, not_default ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_cvt_func, convertor ) );

   spec_data.data_pairs.push_back( make_pair( "not_peer", "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_new_only, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_inc_clones, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tcfistexttype, "0" ) );
}

string default_to_field_specification::static_class_name( ) { return "default_to_field"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, default_to_field_specification, static_class_name );

struct default_to_global_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string field_id;
   string field_value;

   string cfield_id;

   string tclass_id;
   string tfield_id;
   string tpfield_id;
   string test_value;

   bool new_only;
};

void default_to_global_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'default_to_global' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_field_value( args[ 2 ] );

   string test_field_info;
   string check_field_name;

   new_only = false;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg.find( c_arg_test_prefix ) == 0 )
         test_field_info = next_arg.substr( strlen( c_arg_test_prefix ) );
      else if( next_arg.find( c_arg_check_prefix ) == 0 )
         check_field_name = next_arg.substr( strlen( c_arg_check_prefix ) );
      else if( next_arg == string( c_arg_new_only ) )
         new_only = true;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'default_to_global' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string field_type;
   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, &field_type, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   if( !check_field_name.empty( ) )
   {
      cfield_id = get_field_id_for_name( m, arg_class_name, check_field_name, 0, true );
      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + check_field_name + "' for class '" + arg_class_name + "'" );
   }

   bool is_non_string( is_non_string_type( field_type ) );
   if( is_non_string )
      field_value = arg_field_value;
   else
      field_value = '"' + arg_field_value + '"';

   if( !test_field_info.empty( ) )
   {
      string::size_type pos = test_field_info.find( '=' );
      string test_field_name( test_field_info.substr( 0, pos ) );

      string test_field_class( arg_class_name );

      string test_pfield_name;
      string::size_type ppos = test_field_name.find( ':' );
      if( ppos != string::npos )
      {
         test_pfield_name = test_field_name.substr( 0, ppos );
         test_field_name.erase( 0, ppos + 1 );

         string test_pfield_type;
         tpfield_id = get_field_id_for_name( m, arg_class_name, test_pfield_name, &test_pfield_type, true );
         if( tpfield_id.empty( ) )
            throw runtime_error( "unknown field '" + test_pfield_name + "' for class '" + arg_class_name + "'" );

         string test_pclass_name( get_class_name_from_field_type( m, arg_class_name, test_pfield_name, test_pfield_type ) );

         tclass_id = get_class_id_for_name( m, test_pclass_name );
         test_field_class = test_pclass_name;
      }

      string test_field_type;
      tfield_id = get_field_id_for_name( m, test_field_class, test_field_name, &test_field_type, true );
      if( tfield_id.empty( ) )
         throw runtime_error( "unknown field '" + test_field_name + "' for class '" + arg_class_name + "'" );

      if( pos != string::npos )
      {
         test_value = test_field_info.substr( pos + 1 );

         bool is_non_string( is_non_string_type( test_field_type ) );

         if( !is_non_string )
            test_value = '"' + test_value + '"';
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !tclass_id.empty( ) )
      details.push_back( specification_detail( tclass_id, "tclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );

   if( !cfield_id.empty( ) )
      details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );

   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );
   if( !tpfield_id.empty( ) )
      details.push_back( specification_detail( tpfield_id, "tpfield", e_model_element_type_field ) );
}

void default_to_global_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   field_value = reader.read_attribute( c_attribute_field_value );

   cfield_id = reader.read_opt_attribute( c_attribute_cfield_id );

   tclass_id = reader.read_opt_attribute( c_attribute_tclass_id );
   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );
   tpfield_id = reader.read_opt_attribute( c_attribute_tpfield_id );
   test_value = reader.read_opt_attribute( c_attribute_test_value );

   new_only = ( reader.read_opt_attribute( c_attribute_new_only ) == c_true );
}

void default_to_global_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_field_value, field_value );

   writer.write_opt_attribute( c_attribute_cfield_id, cfield_id );

   writer.write_opt_attribute( c_attribute_tclass_id, tclass_id );
   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );
   writer.write_opt_attribute( c_attribute_tpfield_id, tpfield_id );
   writer.write_opt_attribute( c_attribute_test_value, test_value );

   writer.write_opt_attribute( c_attribute_new_only, new_only ? c_true : "" );
}

void default_to_global_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );

   bool is_mandatory;
   string field_type;
   string field_name = get_field_name_for_id( m, class_name, field_id, &field_type, false, false, &is_mandatory );

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_value, field_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_fmandatory, is_mandatory ? "1" : "0" ) );

   string cfield_name;
   if( !cfield_id.empty( ) )
      cfield_name = get_field_name_for_id( m, class_name, cfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_cfield, cfield_name ) );

   string tpfield_name;
   if( !tpfield_id.empty( ) )
      tpfield_name = get_field_name_for_id( m, class_name, tpfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tpfield, tpfield_name ) );

   string tclass_name( class_name );
   if( !tclass_id.empty( ) )
      tclass_name = get_class_name_for_id( m, tclass_id );

   string tfield_name, tfield_type;
   if( !tfield_id.empty( ) )
      tfield_name = get_field_name_for_id( m, tclass_name, tfield_id, &tfield_type );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   bool is_text_type( !is_non_string_type( tfield_type ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tfistexttype, is_text_type ? "1" : "0" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_new_only, new_only ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( "ename", "" ) );
   spec_data.data_pairs.push_back( make_pair( "modifier", "" ) );
   spec_data.data_pairs.push_back( make_pair( "inc_exist", "" ) );
   spec_data.data_pairs.push_back( make_pair( "for_store", "" ) );
}

string default_to_global_specification::static_class_name( ) { return "default_to_global"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, default_to_global_specification, static_class_name );

struct destroy_restrict_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;

   string field_id;
   string pfield_id;

   string restrict_value;

   bool allow_internal;
};

void destroy_restrict_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'destroy_restrict' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_info( args[ 1 ] );
   string arg_field_value( args[ 2 ] );

   allow_internal = false;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_allow_internal ) )
         allow_internal = true;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'destroy_restrict' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string field_name, field_class_name( arg_class_name );
   string::size_type pos = arg_field_info.find( ':' );
   if( pos == string::npos )
      field_name = arg_field_info;
   else
   {
      string pfield_name( arg_field_info.substr( 0, pos ) );
      field_name = arg_field_info.substr( pos + 1 );

      string pfield_type;
      pfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, &pfield_type, true );
      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      string pfield_class_name( get_class_name_from_field_type( m, arg_class_name, pfield_name, pfield_type ) );

      pclass_id = get_class_id_for_name( m, pfield_class_name );
      field_class_name = pfield_class_name;
   }

   string field_type;
   field_id = get_field_id_for_name( m, field_class_name, field_name, &field_type );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field name '" + field_name + "' for class '" + field_class_name + "'" );

   bool is_non_string( is_non_string_type( field_type ) );

   if( is_non_string )
      restrict_value = arg_field_value;
   else
      restrict_value = '"' + arg_field_value + '"';

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !pclass_id.empty( ) )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   if( !pfield_id.empty( ) )
      details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
}

void destroy_restrict_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_opt_attribute( c_attribute_pclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   pfield_id = reader.read_opt_attribute( c_attribute_pfield_id );

   restrict_value = reader.read_attribute( c_attribute_restrict_value );

   allow_internal = ( reader.read_opt_attribute( c_attribute_allow_internal ) == c_true );
}

void destroy_restrict_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_pclass_id, pclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_pfield_id, pfield_id );

   writer.write_attribute( c_attribute_restrict_value, restrict_value );

   writer.write_opt_attribute( c_attribute_allow_internal, allow_internal ? c_true : "" );
}

void destroy_restrict_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pclass_name( class_name );
   if( !pclass_id.empty( ) )
      pclass_name = get_class_name_for_id( m, pclass_id );

   string pfield_name;
   if( !pfield_id.empty( ) )
      pfield_name = get_field_name_for_id( m, class_name, pfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string field_type;
   string field_name = get_field_name_for_id( m, pclass_name, field_id, &field_type );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   bool is_text_type( !is_non_string_type( field_type ) );

   spec_data.data_pairs.push_back( make_pair( c_data_value, restrict_value ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_allow_internal ), allow_internal ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_sfistexttype, is_text_type ? "1" : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_scfistexttype, "" ) );
}

string destroy_restrict_specification::static_class_name( ) { return "destroy_restrict"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, destroy_restrict_specification, static_class_name );

struct field_clear_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string sclass_id;

   string field_id;
   string sfield_id;
   string spfield_id;
   string tfield_id;

   bool is_negative;
};

void field_clear_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 2 )
      throw runtime_error( "unexpected number of args < 2 for 'field_clear' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );

   string sfield_info, arg_tfield_name;
   if( args.size( ) > 2 )
      sfield_info = args[ 2 ];

   is_negative = false;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_negative ) )
         is_negative = true;
      else if( arg_tfield_name.empty( ) )
         arg_tfield_name = next_arg;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'field_clear' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   if( !sfield_info.empty( ) )
   {
      string sfield_name, sfield_class_name( arg_class_name );

      string::size_type pos = sfield_info.find( ':' );
      if( pos == string::npos )
         sfield_name = sfield_info;
      else
      {
         string spfield_name( sfield_info.substr( 0, pos ) );
         sfield_name = sfield_info.substr( pos + 1 );

         string spfield_type;
         spfield_id = get_field_id_for_name( m, arg_class_name, spfield_name, &spfield_type, true );
         if( spfield_id.empty( ) )
            throw runtime_error( "unknown field '" + spfield_name + "' for class '" + arg_class_name + "'" );

         string spfield_class_name( get_class_name_from_field_type( m, arg_class_name, spfield_name, spfield_type ) );

         sclass_id = get_class_id_for_name( m, spfield_class_name );
         sfield_class_name = spfield_class_name;
      }

      sfield_id = get_field_id_for_name( m, sfield_class_name, sfield_name );
      if( sfield_id.empty( ) )
         throw runtime_error( "unknown field '" + sfield_name + "' for class '" + sfield_class_name + "'" );

      if( !arg_tfield_name.empty( ) )
      {
         tfield_id = get_field_id_for_name( m, arg_class_name, arg_tfield_name );
         if( tfield_id.empty( ) )
            throw runtime_error( "unknown field '" + arg_tfield_name + "' for class '" + arg_class_name + "'" );
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !sclass_id.empty( ) )
      details.push_back( specification_detail( sclass_id, "sclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   if( !sfield_id.empty( ) )
      details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );
   if( !spfield_id.empty( ) )
      details.push_back( specification_detail( spfield_id, "spfield", e_model_element_type_field ) );
   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );
}

void field_clear_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   sclass_id = reader.read_opt_attribute( c_attribute_sclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );

   sfield_id = reader.read_opt_attribute( c_attribute_sfield_id );
   spfield_id = reader.read_opt_attribute( c_attribute_spfield_id );

   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );

   is_negative = ( reader.read_opt_attribute( c_attribute_is_negative ) == c_true );
}

void field_clear_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_sclass_id, sclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );

   writer.write_opt_attribute( c_attribute_sfield_id, sfield_id );
   writer.write_opt_attribute( c_attribute_spfield_id, spfield_id );

   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );

   writer.write_opt_attribute( c_attribute_is_negative, is_negative ? c_true : "" );
}

void field_clear_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field_type;
   string field_name = get_field_name_for_id( m, class_name, field_id, &field_type, false, false );

   bool is_text_type( !is_non_string_type( field_type ) );

   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string sclass_name( class_name );
   if( !sclass_id.empty( ) )
      sclass_name = get_class_name_for_id( m, sclass_id );

   string spfield_name;
   if( !spfield_id.empty( ) )
      spfield_name = get_field_name_for_id( m, class_name, spfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_spfield, spfield_name ) );

   string sfield_name;
   if( !sfield_id.empty( ) )
      sfield_name = get_field_name_for_id( m, sclass_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   string tfield_name;
   if( !tfield_id.empty( ) )
      tfield_name = get_field_name_for_id( m, class_name, tfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_is_neg ), is_negative ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_for_store, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fistexttype, is_text_type ? "1" : "0" ) );
   spec_data.data_pairs.push_back( make_pair( "modifier", "" ) );
   spec_data.data_pairs.push_back( make_pair( "for_store", "" ) );
   spec_data.data_pairs.push_back( make_pair( "set_to_default", "" ) );
   spec_data.data_pairs.push_back( make_pair( "create_only", "" ) );
   spec_data.data_pairs.push_back( make_pair( "update_only", "" ) );
}

string field_clear_specification::static_class_name( ) { return "field_clear"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_clear_specification, static_class_name );

struct field_clear_on_changed_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string tclass_id;

   string field_id;
   string sfield_id;

   string tfield_id;
   string tpfield_id;
   string test_value;

   bool not_create;
   bool not_default;
};

void field_clear_on_changed_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'field_clear_on_changed' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_sfield_name( args[ 1 ] );
   string arg_dfield_name( args[ 2 ] );

   not_create = false;
   not_default = false;

   string test_field_info;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_not_create )
         not_create = true;
      else if( next_arg == c_arg_not_default )
         not_default = true;
      else if( next_arg.find( c_arg_test_prefix ) == 0 )
         test_field_info = next_arg.substr( strlen( c_arg_test_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'field_clear_on_changed' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string sfield_type;
   sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_name, &sfield_type, true );

   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + arg_class_name + "'" );

   string spclass_name( get_class_name_from_field_type( m, arg_class_name, arg_sfield_name, sfield_type ) );

   if( spclass_name.empty( ) )
      throw runtime_error( "unexpected non-parent field '" + arg_sfield_name + "' was provided to field_clear_on_changed" );

   field_id = get_field_id_for_name( m, arg_class_name, arg_dfield_name, 0, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_dfield_name + "' for class '" + arg_class_name + "'" );

   if( !test_field_info.empty( ) )
   {
      string::size_type pos = test_field_info.find( '=' );
      string test_field_name( test_field_info.substr( 0, pos ) );

      string test_field_class( arg_class_name );

      string test_pfield_name;
      string::size_type ppos = test_field_name.find( ':' );
      if( ppos != string::npos )
      {
         test_pfield_name = test_field_name.substr( 0, ppos );
         test_field_name.erase( 0, ppos + 1 );

         string test_pfield_type;
         tpfield_id = get_field_id_for_name( m, arg_class_name, test_pfield_name, &test_pfield_type, true );
         if( tpfield_id.empty( ) )
            throw runtime_error( "unknown field '" + test_pfield_name + "' for class '" + arg_class_name + "'" );

         string test_pclass_name( get_class_name_from_field_type( m, arg_class_name, test_pfield_name, test_pfield_type ) );

         tclass_id = get_class_id_for_name( m, test_pclass_name );
         test_field_class = test_pclass_name;
      }

      string test_field_type;
      tfield_id = get_field_id_for_name( m, test_field_class, test_field_name, &test_field_type, true );
      if( tfield_id.empty( ) )
         throw runtime_error( "unknown field '" + test_field_name + "' for class '" + arg_class_name + "'" );

      if( pos != string::npos )
      {
         test_value = test_field_info.substr( pos + 1 );

         bool is_non_string( is_non_string_type( test_field_type ) );

         if( !is_non_string )
            test_value = '"' + test_value + '"';
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !tclass_id.empty( ) && tclass_id != class_id )
      details.push_back( specification_detail( tclass_id, "tclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );

   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );

   if( !tpfield_id.empty( ) )
      details.push_back( specification_detail( tpfield_id, "tpfield", e_model_element_type_field ) );
}

void field_clear_on_changed_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   tclass_id = reader.read_opt_attribute( c_attribute_tclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   sfield_id = reader.read_attribute( c_attribute_sfield_id );

   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );
   tpfield_id = reader.read_opt_attribute( c_attribute_tpfield_id );
   test_value = reader.read_opt_attribute( c_attribute_test_value );

   not_create = ( reader.read_opt_attribute( c_attribute_not_create ) == c_true );
   not_default = ( reader.read_opt_attribute( c_attribute_not_default ) == c_true );
}

void field_clear_on_changed_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_tclass_id, tclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_sfield_id, sfield_id );

   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );
   writer.write_opt_attribute( c_attribute_tpfield_id, tpfield_id );
   writer.write_opt_attribute( c_attribute_test_value, test_value );

   writer.write_opt_attribute( c_attribute_not_create, not_create ? c_true : "" );
   writer.write_opt_attribute( c_attribute_not_default, not_default ? c_true : "" );
}

void field_clear_on_changed_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string sfield_name = get_field_name_for_id( m, class_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   string field_type;
   string field_name = get_field_name_for_id( m, class_name, field_id, &field_type );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   bool is_text_type( !is_non_string_type( field_type ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fistexttype, is_text_type ? "1" : "0" ) );

   string tclass_name( class_name );
   if( !tclass_id.empty( ) )
      tclass_name = get_class_name_for_id( m, tclass_id );

   string tpfield_name;
   if( !tpfield_id.empty( ) )
      tpfield_name = get_field_name_for_id( m, class_name, tpfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tpfield, tpfield_name ) );

   string tfield_name;
   if( !tfield_id.empty( ) )
      tfield_name = get_field_name_for_id( m, tclass_name, tfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_not_dflt, not_default ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_not_create, not_create ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( "sfisfk", "1" ) );
   spec_data.data_pairs.push_back( make_pair( "modifier", "" ) );
   spec_data.data_pairs.push_back( make_pair( "for_store", "" ) );
   spec_data.data_pairs.push_back( make_pair( "set_to_default", "" ) );
}

string field_clear_on_changed_specification::static_class_name( ) { return "field_clear_on_changed"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_clear_on_changed_specification, static_class_name );

struct field_dependency_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   static string static_class_name( );
};

void field_dependency_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected args < 3 for 'field_dependency' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_sfield_name( args[ 2 ] );

   string class_id = get_class_id_for_name( m, arg_class_name );
   string field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, 0, true );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   string sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_name, 0, true );

   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_sfield_name + "' for class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );
}

string field_dependency_specification::static_class_name( ) { return "field_dependency"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_dependency_specification, static_class_name );

struct field_empty_action_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;

   string field_id;
   string pfield_id;
   string act_field_id;

   string null_value;
   string non_null_value;

   bool is_negative;
   bool create_only;
};

void field_empty_action_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected args < 4 for 'field_empty_action' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_info( args[ 1 ] );
   string arg_act_field_name( args[ 2 ] );

   non_null_value = args[ 3 ];

   is_negative = false;
   create_only = false;
   bool no_clear = false;
   for( size_t i = 4; i < args.size( ); i++ )
   {
      if( args[ i ] == c_arg_negative )
         is_negative = true;
      else if( args[ i ] == c_arg_no_clear )
         no_clear = true;
      else if( args[ i ] == c_arg_create_only )
         create_only = true;
      else
         throw runtime_error( "unexpected arg '" + args[ i ] + "' for 'field_empty_action' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string field_name, field_class_name( arg_class_name );

   string::size_type pos = arg_field_info.find( ':' );
   if( pos == string::npos )
      field_name = arg_field_info;
   else
   {
      string pfield_name( arg_field_info.substr( 0, pos ) );
      field_name = arg_field_info.substr( pos + 1 );

      string pfield_type;
      pfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, &pfield_type, true );
      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      string pfield_class_name( get_class_name_from_field_type( m, arg_class_name, pfield_name, pfield_type ) );

      pclass_id = get_class_id_for_name( m, pfield_class_name );
      field_class_name = pfield_class_name;
   }

   field_id = get_field_id_for_name( m, field_class_name, field_name, 0, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + field_name + "' for class '" + field_class_name + "'" );

   string act_field_type;
   act_field_id = get_field_id_for_name( m, arg_class_name, arg_act_field_name, &act_field_type );
   if( act_field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_act_field_name + "' for class '" + arg_class_name + "'" );

   bool is_non_string( is_non_string_type( act_field_type ) );

   if( !is_non_string )
   {
      null_value = "\"\"";
      non_null_value = '"' + non_null_value + '"';
   }

   if( no_clear )
      null_value.erase( );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !pclass_id.empty( ) )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   if( !pfield_id.empty( ) )
      details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( act_field_id, "act_field", e_model_element_type_field ) );
}

void field_empty_action_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   act_field_id = reader.read_attribute( c_attribute_act_field_id );

   pclass_id = reader.read_opt_attribute( c_attribute_pclass_id );
   pfield_id = reader.read_opt_attribute( c_attribute_pfield_id );
   null_value = reader.read_opt_attribute( c_attribute_null_value );
   non_null_value = reader.read_opt_attribute( c_attribute_action_value );

   is_negative = ( reader.read_opt_attribute( c_attribute_is_negative ) == c_true );
   create_only = ( reader.read_opt_attribute( c_attribute_create_only ) == c_true );
}

void field_empty_action_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_act_field_id, act_field_id );

   writer.write_opt_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_opt_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_opt_attribute( c_attribute_null_value, null_value );
   writer.write_opt_attribute( c_attribute_action_value, non_null_value );

   writer.write_opt_attribute( c_attribute_is_negative, is_negative ? c_true : "" );
   writer.write_opt_attribute( c_attribute_create_only, create_only ? c_true : "" );
}

void field_empty_action_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pclass_name;
   string field_class_name( class_name );
   if( !pclass_id.empty( ) )
   {
      pclass_name = get_class_name_for_id( m, pclass_id );
      field_class_name = pclass_name;
   }

   string field_name = get_field_name_for_id( m, field_class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string pfield_name;
   if( !pfield_id.empty( ) )
      pfield_name = get_field_name_for_id( m, class_name, pfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string act_field_name = get_field_name_for_id( m, class_name, act_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_act_field, act_field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_nvalue, null_value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_value, non_null_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_is_neg, is_negative ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_create_only, create_only ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_is_testval, "" ) );
}

string field_empty_action_specification::static_class_name( ) { return "field_empty_action"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_empty_action_specification, static_class_name );

struct field_from_acyclic_path_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string separator;
   bool embed_links;
};

void field_from_acyclic_path_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'field_from_acyclic_path' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_pfield_name( args[ 1 ] );
   string arg_src_field_name( args[ 2 ] );
   string arg_dest_field_name( args[ 3 ] );

   separator = string( "." );

   embed_links = false;
   for( size_t i = 4; i < args.size( ); i++ )
   {
      string next_arg( args[ i ] );

      if( next_arg == c_arg_embed_links )
         embed_links = true;
      else if( next_arg.find( c_arg_sep_prefix ) == 0 )
         separator = next_arg.substr( strlen( c_arg_sep_prefix ) );
      else
         throw runtime_error( "unexpected arg '" + next_arg + "' for 'field_from_acyclic_path' specification" );
   }

   string class_id = get_class_id_for_name( m, arg_class_name );

   string pfield_id = get_field_id_for_name( m, arg_class_name, arg_pfield_name, 0, true );
   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_pfield_name + "' for class '" + arg_class_name + "'" );

   string src_field_id = get_field_id_for_name( m, arg_class_name, arg_src_field_name, 0, true );
   if( src_field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_src_field_name + "' for class '" + arg_class_name + "'" );

   string dest_field_id = get_field_id_for_name( m, arg_class_name, arg_dest_field_name, 0, true );
   if( dest_field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_dest_field_name + "' for class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( src_field_id, "src_field", e_model_element_type_field ) );
   details.push_back( specification_detail( dest_field_id, "dest_field", e_model_element_type_field ) );
}

void field_from_acyclic_path_specification::read_data( sio_reader& reader )
{
   separator = reader.read_attribute( c_attribute_separator );

   embed_links = ( reader.read_opt_attribute( c_attribute_embed_links ) == c_true );
}

void field_from_acyclic_path_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_separator, separator );

   writer.write_opt_attribute( c_attribute_embed_links, embed_links ? c_true : "" );
}

void field_from_acyclic_path_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   spec_data.data_pairs.push_back( make_pair( c_data_separator, separator ) );
   spec_data.data_pairs.push_back( make_pair( c_data_embed_links, embed_links ? c_true : "" ) );
}

string field_from_acyclic_path_specification::static_class_name( ) { return "field_from_acyclic_path"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_from_acyclic_path_specification, static_class_name );

struct field_from_changed_fk_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string parent_class_id;
   string parent_field_id;
   string destination_field_id;
   string parent_source_field_id;

   bool new_only;
   bool not_null;
   bool for_store;
   bool not_create;
   bool include_default;
};

void field_from_changed_fk_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 || args.size( ) > 5 )
      throw runtime_error( "unexpected number of args for 'field_from_changed_fk' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_parent_field_name( args[ 1 ] );
   string arg_src_dest_field_info( args[ 2 ] );

   new_only = false;
   not_null = false;
   for_store = false;
   not_create = false;
   include_default = false;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_new_only )
         new_only = true;
      else if( next_arg == c_arg_check_null )
         not_null = true;
      else if( next_arg == c_arg_for_store )
         for_store = true;
      else if( next_arg == c_arg_not_create )
         not_create = true;
      else if( next_arg == c_arg_include_default )
         include_default = true;
      else
         throw runtime_error( "unexpected arg '" + next_arg + "' for 'field_from_changed_fk' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string parent_field_type;
   parent_field_id = get_field_id_for_name( m, arg_class_name, arg_parent_field_name, &parent_field_type, true );

   if( parent_field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_parent_field_name + "' for class '" + arg_class_name + "'" );

   string parent_class_name( get_class_name_from_field_type( m, arg_class_name, arg_parent_field_name, parent_field_type ) );

   if( parent_class_name.empty( ) )
      throw runtime_error( "unexpected non-parent field '" + arg_parent_field_name + "' was provided to field_from_changed_fk" );

   parent_class_id = get_class_id_for_name( m, parent_class_name );

   string dest_field_name, source_field_name;
   string::size_type pos = arg_src_dest_field_info.find( ':' );
   if( pos == string::npos )
      dest_field_name = source_field_name = arg_src_dest_field_info;
   else
   {
      dest_field_name = arg_src_dest_field_info.substr( 0, pos );
      source_field_name = arg_src_dest_field_info.substr( pos + 1 );
   }

   destination_field_id = get_field_id_for_name( m, arg_class_name, dest_field_name, 0, true );
   if( destination_field_id.empty( ) )
      throw runtime_error( "unknown field '" + dest_field_name + "' for class '" + arg_class_name + "'" );

   parent_source_field_id = get_field_id_for_name( m, parent_class_name, source_field_name, 0, true );
   if( parent_source_field_id.empty( ) )
      throw runtime_error( "unknown field '" + source_field_name + "' for class '" + parent_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( class_id != parent_class_id )
      details.push_back( specification_detail( parent_class_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( parent_field_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( destination_field_id, "dfield", e_model_element_type_field ) );
   details.push_back( specification_detail( parent_source_field_id, "sfield", e_model_element_type_field ) );
}

void field_from_changed_fk_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   parent_class_id = reader.read_attribute( c_attribute_pclass_id );
   parent_field_id = reader.read_attribute( c_attribute_pfield_id );
   destination_field_id = reader.read_attribute( c_attribute_dfield_id );
   parent_source_field_id = reader.read_attribute( c_attribute_sfield_id );

   new_only = ( reader.read_opt_attribute( c_attribute_new_only ) == c_true );
   not_null = ( reader.read_opt_attribute( c_attribute_not_null ) == c_true );
   for_store = ( reader.read_opt_attribute( c_attribute_for_store ) == c_true );
   not_create = ( reader.read_opt_attribute( c_attribute_not_create ) == c_true );
   include_default = ( reader.read_opt_attribute( c_attribute_include_default ) == c_true );
}

void field_from_changed_fk_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_pclass_id, parent_class_id );
   writer.write_attribute( c_attribute_pfield_id, parent_field_id );
   writer.write_attribute( c_attribute_dfield_id, destination_field_id );
   writer.write_attribute( c_attribute_sfield_id, parent_source_field_id );

   writer.write_opt_attribute( c_attribute_new_only, new_only ? c_true : "" );
   writer.write_opt_attribute( c_attribute_not_null, not_null ? c_true : "" );
   writer.write_opt_attribute( c_attribute_for_store, for_store ? c_true : "" );
   writer.write_opt_attribute( c_attribute_not_create, not_create ? c_true : "" );
   writer.write_opt_attribute( c_attribute_include_default, include_default ? c_true : "" );
}

void field_from_changed_fk_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string parent_class_name = get_class_name_for_id( m, parent_class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pclass, parent_class_name ) );

   string parent_field_name = get_field_name_for_id( m, class_name, parent_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, parent_field_name ) );

   bool is_mandatory;
   string destination_field_name = get_field_name_for_id( m, class_name, destination_field_id, 0, false, false, &is_mandatory );
   spec_data.data_pairs.push_back( make_pair( c_data_dfield, destination_field_name ) );

   string parent_source_field_name = get_field_name_for_id( m, parent_class_name, parent_source_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, parent_source_field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_new_only, new_only ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_not_null, not_null ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_for_store, for_store ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_not_create, not_create ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fmandatory, is_mandatory ? "1" : "0" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_inc_dflt, include_default ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( "trigger_only", "" ) );
   spec_data.data_pairs.push_back( make_pair( "append", "" ) );
}

string field_from_changed_fk_specification::static_class_name( ) { return "field_from_changed_fk"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_from_changed_fk_specification, static_class_name );

struct field_from_concatenation_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string dest_field_id;

   vector< string > field_info;

   string separator;
};

void field_from_concatenation_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'field_from_concatenation' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_dest_field_name( args[ 1 ] );
   string arg_src_field_info( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   dest_field_id = get_field_id_for_name( m, arg_class_name, arg_dest_field_name );
   if( dest_field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_dest_field_name + "' for class '" + arg_class_name + "'" );

   string arg_sep;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg.find( c_arg_sep_prefix ) == 0 )
      {
         arg_sep = next_arg.substr( strlen( c_arg_sep_prefix ) );

         // NOTE: If the separator argument has been specified with no value then a single space will be used.
         if( arg_sep.empty( ) )
            arg_sep = " ";
      }
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'field_from_concatenation' specification" );
   }

   while( true )
   {
      string::size_type pos = arg_src_field_info.find( ',' );
      string next_field_name( arg_src_field_info.substr( 0, pos ) );

      string next_field_id = get_field_id_for_name( m, arg_class_name, next_field_name, 0, true );
      if( next_field_id.empty( ) )
         throw runtime_error( "unknown field '" + next_field_name + "' for class '" + arg_class_name + "'" );

      field_info.push_back( next_field_id );

      if( pos == string::npos )
         break;

      arg_src_field_info.erase( 0, pos + 1 );
   }

   if( field_info.size( ) < 2 )
      throw runtime_error( "must have at least two source fields for 'field_from_concatenation' specification" );

   separator = arg_sep;

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   details.push_back( specification_detail( dest_field_id, "dest_field", e_model_element_type_field ) );

   for( size_t i = 0; i < field_info.size( ); i++ )
      details.push_back( specification_detail( field_info[ i ], "field", e_model_element_type_field ) );
}

void field_from_concatenation_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   dest_field_id = reader.read_attribute( c_attribute_dfield_id );

   string next;
   while( reader.has_read_attribute( c_attribute_field_info, next ) )
      field_info.push_back( next );

   separator = reader.read_opt_attribute( c_attribute_separator );
}

void field_from_concatenation_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_dfield_id, dest_field_id );

   for( size_t i = 0; i < field_info.size( ); i++ )
      writer.write_attribute( c_attribute_field_info, field_info[ i ] );

   writer.write_opt_attribute( c_attribute_separator, separator );
}

void field_from_concatenation_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string dest_field_name = get_field_name_for_id( m, class_name, dest_field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_dfield ), dest_field_name ) );

   string field_list;
   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_list.empty( ) )
         field_list += " ";

      string field_name = get_field_name_for_id( m, class_name, field_info[ i ] );

      field_list += field_name;
   }
   spec_data.data_pairs.push_back( make_pair( string( c_data_field_list ), field_list ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_separator ), separator ) );
}

string field_from_concatenation_specification::static_class_name( ) { return "field_from_concatenation"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_from_concatenation_specification, static_class_name );

struct field_from_other_field_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string sclass_id;
   string tclass_id;

   string src_field_id;
   string src_pfield_id;
   string dest_field_id;
   string test_field_id;
   string test_pfield_id;

   string function;
   string mem_func;
   string null_value;
   string test_value;

   bool for_store;
   bool check_null;
   bool create_only;
};

void field_from_other_field_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 'field_from_other_field' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_src_field_name( args[ 1 ] );
   string arg_dest_field_name( args[ 2 ] );

   bool has_null_value = false;
   string arg_null_value, test_info;

   for_store = false;
   check_null = false;
   create_only = false;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_for_store )
         for_store = true;
      else if( next_arg == c_arg_check_null )
         check_null = true;
      else if( next_arg == c_arg_create_only )
         create_only = true;
      else if( next_arg.find( c_arg_func_prefix ) == 0 )
         function = next_arg.substr( strlen( c_arg_func_prefix ) );
      else if( next_arg.find( c_arg_test_prefix ) == 0 )
         test_info = next_arg.substr( strlen( c_arg_test_prefix ) );
      else if( next_arg.find( c_arg_memfunc_prefix ) == 0 )
         mem_func = next_arg.substr( strlen( c_arg_memfunc_prefix ) );
      else if( !has_null_value )
      {
         arg_null_value = next_arg;
         has_null_value = true;
      }
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'field_from_other_field' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string src_class_name( arg_class_name );
   string::size_type pos = arg_src_field_name.find( ':' );
   if( pos != string::npos )
   {
      string src_pfield_name( arg_src_field_name.substr( 0, pos ) );

      string src_pfield_type;
      src_pfield_id = get_field_id_for_name( m, arg_class_name, src_pfield_name, &src_pfield_type, true );

      if( src_pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + src_pfield_name + "' for class '" + arg_class_name + "'" );

      src_class_name = get_class_name_from_field_type( m, arg_class_name, src_pfield_name, src_pfield_type );
      sclass_id = get_class_id_for_name( m, src_class_name );

      arg_src_field_name.erase( 0, pos + 1 );
   }

   src_field_id = get_field_id_for_name( m, src_class_name, arg_src_field_name, 0, true );

   if( src_field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_src_field_name + "' for class '" + src_class_name + "'" );

   string dest_field_type;
   dest_field_id = get_field_id_for_name( m, arg_class_name, arg_dest_field_name, &dest_field_type, true );

   if( dest_field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_dest_field_name + "' for class '" + arg_class_name + "'" );

   bool is_non_string( is_non_string_type( dest_field_type ) );

   if( has_null_value )
   {
      if( is_non_string )
         null_value = arg_null_value;
      else
         null_value = '"' + arg_null_value + '"';
   }

   if( !test_info.empty( ) )
   {
      string::size_type pos = test_info.find( '=' );
      string test_field_name = test_info.substr( 0, pos );

      if( pos != string::npos )
         test_value = test_info.substr( pos + 1 );

      string test_class_name( arg_class_name );
      string test_pfield_name;

      pos = test_field_name.find( ':' );
      if( pos != string::npos )
      {
         test_pfield_name = test_field_name.substr( 0, pos );
         test_field_name.erase( 0, pos + 1 );

         string test_pfield_type;
         test_pfield_id = get_field_id_for_name( m, arg_class_name, test_pfield_name, &test_pfield_type, true );
         if( test_pfield_id.empty( ) )
            throw runtime_error( "unknown field '" + test_pfield_name + "' for class '" + arg_class_name + "'" );

         string test_pclass_name( get_class_name_from_field_type( m, arg_class_name, test_pfield_name, test_pfield_type ) );

         tclass_id = get_class_id_for_name( m, test_pclass_name );
         test_class_name = test_pclass_name;
      }

      test_field_id = get_field_id_for_name( m, test_class_name, test_field_name );
      if( test_field_id.empty( ) )
         throw runtime_error( "unknown field '" + test_field_name + "' for class '" + test_class_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !sclass_id.empty( ) && sclass_id != class_id )
      details.push_back( specification_detail( sclass_id, "sclass", e_model_element_type_class ) );
   if( !tclass_id.empty( ) && tclass_id != class_id )
      details.push_back( specification_detail( tclass_id, "tclass", e_model_element_type_class ) );

   details.push_back( specification_detail( src_field_id, "src_field", e_model_element_type_field ) );
   if( !src_pfield_id.empty( ) )
      details.push_back( specification_detail( src_pfield_id, "src_pfield", e_model_element_type_field ) );

   details.push_back( specification_detail( dest_field_id, "dest_field", e_model_element_type_field ) );
   if( !test_field_id.empty( ) )
      details.push_back( specification_detail( test_field_id, "test_field", e_model_element_type_field ) );
   if( !test_pfield_id.empty( ) )
      details.push_back( specification_detail( test_pfield_id, "test_pfield", e_model_element_type_field ) );
}

void field_from_other_field_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   sclass_id = reader.read_attribute( c_attribute_sclass_id );
   tclass_id = reader.read_opt_attribute( c_attribute_tclass_id );

   src_field_id = reader.read_attribute( c_attribute_sfield_id );
   src_pfield_id = reader.read_opt_attribute( c_attribute_pfield_id );

   dest_field_id = reader.read_attribute( c_attribute_dfield_id );

   test_field_id = reader.read_opt_attribute( c_attribute_tfield_id );
   test_pfield_id = reader.read_opt_attribute( c_attribute_tpfield_id );
   test_value = reader.read_opt_attribute( c_attribute_test_value );

   for_store = ( reader.read_opt_attribute( c_attribute_for_store ) == c_true );
   check_null = ( reader.read_opt_attribute( c_attribute_check_null ) == c_true );
   create_only = ( reader.read_opt_attribute( c_attribute_create_only ) == c_true );

   function = reader.read_opt_attribute( c_attribute_function );
   mem_func = reader.read_opt_attribute( c_attribute_mem_func );
   null_value = reader.read_opt_attribute( c_attribute_null_value );
}

void field_from_other_field_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_sclass_id, sclass_id );
   writer.write_opt_attribute( c_attribute_tclass_id, tclass_id );

   writer.write_attribute( c_attribute_sfield_id, src_field_id );
   writer.write_opt_attribute( c_attribute_pfield_id, src_pfield_id );

   writer.write_attribute( c_attribute_dfield_id, dest_field_id );

   writer.write_opt_attribute( c_attribute_tfield_id, test_field_id );
   writer.write_opt_attribute( c_attribute_tpfield_id, test_pfield_id );
   writer.write_opt_attribute( c_attribute_test_value, test_value );

   writer.write_opt_attribute( c_attribute_for_store, for_store ? c_true : "" );
   writer.write_opt_attribute( c_attribute_check_null, check_null ? c_true : "" );
   writer.write_opt_attribute( c_attribute_create_only, create_only ? c_true : "" );

   writer.write_opt_attribute( c_attribute_function, function );
   writer.write_opt_attribute( c_attribute_mem_func, mem_func );
   writer.write_opt_attribute( c_attribute_null_value, null_value );
}

void field_from_other_field_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pfield_name, sclass_name( class_name );
   if( !sclass_id.empty( ) )
   {
      sclass_name = get_class_name_for_id( m, sclass_id );
      pfield_name = get_field_name_for_id( m, class_name, src_pfield_id );
   }
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string sfield_name = get_field_name_for_id( m, sclass_name, src_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   bool is_mandatory;
   string dfield_name = get_field_name_for_id( m, class_name, dest_field_id, 0, false, false, &is_mandatory );

   spec_data.data_pairs.push_back( make_pair( c_data_dfield, dfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fmandatory, is_mandatory ? "1" : "0" ) );

   string tclass_name( class_name );
   if( !tclass_id.empty( ) )
      tclass_name = get_class_name_for_id( m, tclass_id );

   string test_pfield_name;
   if( !test_pfield_id.empty( ) )
      test_pfield_name = get_field_name_for_id( m, class_name, test_pfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tpfield, test_pfield_name ) );

   string test_field_name, test_field_type;
   if( !test_field_id.empty( ) )
      test_field_name = get_field_name_for_id( m, tclass_name, test_field_id, &test_field_type );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, test_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   bool is_text_type( !is_non_string_type( test_field_type ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tfistexttype, is_text_type ? "1" : "0" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tcfistexttype, is_text_type ? "1" : "0" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_for_store, for_store ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_chk_null, check_null ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_create_only, create_only ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_func, function ) );
   spec_data.data_pairs.push_back( make_pair( c_data_mfunc, mem_func ) );
   spec_data.data_pairs.push_back( make_pair( c_data_nvalue, null_value ) );

   spec_data.data_pairs.push_back( make_pair( "check", "" ) );
   spec_data.data_pairs.push_back( make_pair( "append", "" ) );
   spec_data.data_pairs.push_back( make_pair( "field1", "" ) );
   spec_data.data_pairs.push_back( make_pair( "field2", "" ) );
   spec_data.data_pairs.push_back( make_pair( "update", "" ) );
   spec_data.data_pairs.push_back( make_pair( "modifier", "" ) );
   spec_data.data_pairs.push_back( make_pair( "new_only", "" ) );
   spec_data.data_pairs.push_back( make_pair( "procedure", "" ) );
   spec_data.data_pairs.push_back( make_pair( "func_arg1", "" ) );
   spec_data.data_pairs.push_back( make_pair( "func_arg2", "" ) );
   spec_data.data_pairs.push_back( make_pair( "non_trigger", "" ) );
   spec_data.data_pairs.push_back( make_pair( "internal_only", "" ) );
   spec_data.data_pairs.push_back( make_pair( "include_clones", "" ) );
}

string field_from_other_field_specification::static_class_name( ) { return "field_from_other_field"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_from_other_field_specification, static_class_name );

struct field_from_procedure_call_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;

   string field_id;
   string procedure_id;

   string value;

   bool is_transient;
};

void field_from_procedure_call_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'field_from_procedure_call' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_procedure_name( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   procedure_id = get_procedure_id_for_name( m, arg_class_name, arg_procedure_name );

   if( procedure_id.empty( ) )
      throw runtime_error( "unknown procedure '" + arg_procedure_name + "' for class '" + arg_class_name + "'" );

   is_transient = false;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_transient ) )
         is_transient = true;
      else if( value.empty( ) )
      {
         value = next_arg;
         if( value.empty( ) )
            value = "\"" + value + "\"";
      }
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'field_from_procedure_call' specification" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( procedure_id, "procedure", e_model_element_type_procedure ) );
}

void field_from_procedure_call_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   procedure_id = reader.read_attribute( c_attribute_procedure_id );

   value = reader.read_opt_attribute( c_attribute_value );
   is_transient = ( reader.read_opt_attribute( c_attribute_is_transient ) == c_true );
}

void field_from_procedure_call_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_procedure_id, procedure_id );

   writer.write_opt_attribute( c_attribute_value, value );
   writer.write_opt_attribute( c_attribute_is_transient, is_transient ? c_true : "" );
}

void field_from_procedure_call_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string procedure_name = get_procedure_name_for_id( m, class_name, procedure_id );
   spec_data.data_pairs.push_back( make_pair( c_data_procedure, procedure_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_value, value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_transient, is_transient ? c_true : "" ) );
}

string field_from_procedure_call_specification::static_class_name( ) { return "field_from_procedure_call"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_from_procedure_call_specification, static_class_name );

struct field_from_search_replace_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string sclass_id;

   string dest_field_id;

   string src_field_id;
   string src_cfield_id;

   vector< string > field_info;

   string separator;

   bool is_transient;
   bool has_opt_prefix;

   string options_field_id;
   string options_left_bracket;
   string options_right_bracket;
};

void field_from_search_replace_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'field_from_search_replace' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_dest_field_name( args[ 1 ] );
   string arg_src_field_name( args[ 2 ] );
   string arg_field_info( args[ 3 ] );

   string arg_sep;

   class_id = get_class_id_for_name( m, arg_class_name );

   dest_field_id = get_field_id_for_name( m, arg_class_name, arg_dest_field_name, 0, false, false, true );

   if( dest_field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_dest_field_name + "' for class '" + arg_class_name + "'" );

   string src_pfield_name;
   string::size_type pos = arg_src_field_name.find( ':' );
   if( pos != string::npos )
   {
      src_pfield_name = arg_src_field_name.substr( 0, pos );
      arg_src_field_name.erase( 0, pos + 1 );
   }

   is_transient = false;
   has_opt_prefix = false;

   string options_info;
   for( size_t arg = 4; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg.find( c_arg_sep_prefix ) == 0 )
      {
         arg_sep = next_arg.substr( strlen( c_arg_sep_prefix ) );

         // NOTE: If the separator argument has been specified with no value then a single space will be used.
         if( arg_sep.empty( ) )
            arg_sep = " ";
      }
      else if( next_arg == string( c_arg_transient ) )
         is_transient = true;
      else if( next_arg == string( c_arg_opt_prefix ) )
         has_opt_prefix = true;
      else if( next_arg.find( c_arg_options_prefix ) == 0 )
         options_info = next_arg.substr( strlen( c_arg_options_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'field_from_search_replace' specification" );
   }

   separator = arg_sep;

   if( separator.size( ) > 1 )
      throw runtime_error( "separator must be a single character for 'field_from_search_replace' specification" );

   if( src_pfield_name.empty( ) )
   {
      src_field_id = get_field_id_for_name( m, arg_class_name, arg_src_field_name, 0, false, false, true );

      if( src_field_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_src_field_name + "' for class '" + arg_class_name + "'" );
   }
   else
   {
      string src_pfield_type;
      src_field_id = get_field_id_for_name( m, arg_class_name, src_pfield_name, &src_pfield_type, true, false, true );

      if( src_field_id.empty( ) )
         throw runtime_error( "unknown field '" + src_pfield_name + "' for class '" + arg_class_name + "'" );

      string src_pclass_name( get_class_name_from_field_type( m, arg_class_name, src_pfield_name, src_pfield_type ) );

      sclass_id = get_class_id_for_name( m, src_pclass_name );
      src_cfield_id = get_field_id_for_name( m, src_pclass_name, arg_src_field_name, 0, false, false, true );

      if( src_cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_src_field_name + "' for class '" + src_pclass_name + "'" );
   }

   vector< string > field_ids;
   while( true )
   {
      string::size_type pos = arg_field_info.find( ',' );
      string next_info( arg_field_info.substr( 0, pos ) );

      string::size_type spos = next_info.find( '=' );
      if( spos == string::npos )
         throw runtime_error( "unexpected format '" + next_info + "' for field_info" );

      string search_string( next_info.substr( spos + 1 ) );
      next_info.erase( spos );

      string::size_type cpos = next_info.find( ':' );
      string pfield_name( next_info.substr( 0, cpos ) );

      string pfield_type;
      string pfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, &pfield_type, true );
      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      field_ids.push_back( pfield_id );

      string next_field_info( pfield_id );
      if( cpos != string::npos )
      {
         string pclass_name( get_class_name_from_field_type( m, arg_class_name, pfield_name, pfield_type ) );
         string pclass_id = get_class_id_for_name( m, pclass_name );

         string cfield_name( next_info.substr( cpos + 1 ) );

         string cfield_id = get_field_id_for_name( m, pclass_name, cfield_name, 0, true );
         if( cfield_id.empty( ) )
            throw runtime_error( "unknown field '" + cfield_name + "' for class '" + pclass_name + "'" );

         next_field_info += ":" + cfield_id + ";" + pclass_id;
         field_ids.push_back( cfield_id );
      }

      next_field_info += "=" + search_string;

      field_info.push_back( next_field_info );

      if( pos == string::npos )
         break;

      arg_field_info.erase( 0, pos + 1 );
   }

   if( !options_info.empty( ) )
   {
      string::size_type pos = options_info.find( ',' );
      string options_field_name( options_info.substr( 0, pos ) );

      if( pos != string::npos )
      {
         string brackets( options_info.substr( pos + 1 ) );
         if( brackets.size( ) != 2 )
            throw runtime_error( "unexpected options brackets size != 2 for 'field_from_search_replace' specification" );

         options_left_bracket = brackets[ 0 ];
         options_right_bracket = brackets[ 1 ];
      }

      options_field_id = get_field_id_for_name( m, arg_class_name, options_field_name, 0, false, false, true );
      if( options_field_id.empty( ) )
         throw runtime_error( "unknown options field '" + options_field_name + "' for class '" + arg_class_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !sclass_id.empty( ) )
      details.push_back( specification_detail( sclass_id, "sclass", e_model_element_type_class ) );

   details.push_back( specification_detail( dest_field_id, "dest_field", e_model_element_type_field ) );
   details.push_back( specification_detail( src_field_id, "src_field", e_model_element_type_field ) );
   if( !src_cfield_id.empty( ) )
      details.push_back( specification_detail( src_cfield_id, "src_cfield", e_model_element_type_field ) );

   for( size_t i = 0; i < field_ids.size( ); i++ )
      details.push_back( specification_detail( field_ids[ i ], "field", e_model_element_type_field ) );

   if( !options_field_id.empty( ) )
      details.push_back( specification_detail( options_field_id, "options", e_model_element_type_field ) );
}

void field_from_search_replace_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   sclass_id = reader.read_opt_attribute( c_attribute_sclass_id );

   dest_field_id = reader.read_attribute( c_attribute_dfield_id );

   src_field_id = reader.read_attribute( c_attribute_sfield_id );
   src_cfield_id = reader.read_opt_attribute( c_attribute_cfield_id );

   string next;
   while( reader.has_read_attribute( c_attribute_field_info, next ) )
      field_info.push_back( next );

   separator = reader.read_opt_attribute( c_attribute_separator );

   is_transient = ( reader.read_opt_attribute( c_attribute_is_transient ) == c_true );
   has_opt_prefix = ( reader.read_opt_attribute( c_attribute_has_opt_prefix ) == c_true );

   options_field_id = reader.read_opt_attribute( c_attribute_ofield_id );
   options_left_bracket = reader.read_opt_attribute( c_attribute_options_left );
   options_right_bracket = reader.read_opt_attribute( c_attribute_options_right );
}

void field_from_search_replace_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_sclass_id, sclass_id );

   writer.write_attribute( c_attribute_dfield_id, dest_field_id );

   writer.write_attribute( c_attribute_sfield_id, src_field_id );
   writer.write_opt_attribute( c_attribute_cfield_id, src_cfield_id );

   for( size_t i = 0; i < field_info.size( ); i++ )
      writer.write_attribute( c_attribute_field_info, field_info[ i ] );

   writer.write_opt_attribute( c_attribute_separator, separator );

   writer.write_opt_attribute( c_attribute_is_transient, is_transient ? c_true : "" );
   writer.write_opt_attribute( c_attribute_has_opt_prefix, has_opt_prefix ? c_true : "" );

   writer.write_opt_attribute( c_attribute_ofield_id, options_field_id );
   writer.write_opt_attribute( c_attribute_options_left, options_left_bracket );
   writer.write_opt_attribute( c_attribute_options_right, options_right_bracket );
}

void field_from_search_replace_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string dfield_name = get_field_name_for_id( m, class_name, dest_field_id, 0, true );
   spec_data.data_pairs.push_back( make_pair( c_data_dfield, dfield_name ) );

   string sfield_name = get_field_name_for_id( m, class_name, src_field_id, 0, true );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   string cfield_name, sclass_name;
   if( !sclass_id.empty( ) )
   {
      sclass_name = get_class_name_for_id( m, sclass_id );
      cfield_name = get_field_name_for_id( m, sclass_name, src_cfield_id );
   }
   spec_data.data_pairs.push_back( make_pair( c_data_sclass, sclass_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_cfield, cfield_name ) );

   string field_list;
   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_list.empty( ) )
         field_list += " ";

      string next_info( field_info[ i ] );

      string::size_type pos = next_info.find( '=' );
      string search_string( next_info.substr( pos + 1 ) );
      next_info.erase( pos );

      string child_field_name;
      string next_class_name( class_name );

      pos = next_info.find( ';' );
      if( pos != string::npos )
      {
         next_class_name = get_class_name_for_id( m, next_info.substr( pos + 1 ) );
         next_info.erase( pos );

         pos = next_info.find( ':' );
         child_field_name = get_field_name_for_id( m, next_class_name, next_info.substr( pos + 1 ) );

         next_info.erase( pos );
      }

      string field_name = get_field_name_for_id( m, class_name, next_info );

      field_list += field_name + "," + child_field_name + "," + search_string;
   }

   spec_data.data_pairs.push_back( make_pair( string( c_data_field_list ), field_list ) );

   spec_data.data_pairs.push_back( make_pair( c_data_separator, separator ) );

   spec_data.data_pairs.push_back( make_pair( c_data_ftransient, is_transient ? "1" : "0" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_opt_prefix, has_opt_prefix ? c_true : "" ) );

   string options_field_name;
   if( !options_field_id.empty( ) )
      options_field_name = get_field_name_for_id( m, class_name, options_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_ofield, options_field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_oleftb, options_left_bracket ) );
   spec_data.data_pairs.push_back( make_pair( c_data_orightb, options_right_bracket ) );

   spec_data.data_pairs.push_back( make_pair( "tfield", "" ) );
   spec_data.data_pairs.push_back( make_pair( "keyrep", "" ) );
   spec_data.data_pairs.push_back( make_pair( "classrep", "" ) );
   spec_data.data_pairs.push_back( make_pair( "modifier", "" ) );
   spec_data.data_pairs.push_back( make_pair( "top_level", "" ) );
   spec_data.data_pairs.push_back( make_pair( "inc_labels", "" ) );
   spec_data.data_pairs.push_back( make_pair( "key_suffix", "" ) );
}

string field_from_search_replace_specification::static_class_name( ) { return "field_from_search_replace"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_from_search_replace_specification, static_class_name );

struct field_inc_in_proc_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string value;
};

void field_inc_in_proc_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 || args.size( ) > 5 )
      throw runtime_error( "unexpected number of args for 'field_inc_in_proc' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_proc_name( args[ 1 ] );
   string arg_field_name( args[ 2 ] );
   string arg_inc_amount( args[ 3 ] );

   value = arg_inc_amount;

   if( args.size( ) > 4 )
      value += " " + args[ 4 ];

   string class_id = get_class_id_for_name( m, arg_class_name );

   string proc_id = get_procedure_id_for_name( m, arg_class_name, arg_proc_name );
   if( proc_id.empty( ) )
      throw runtime_error( "procedure '" + arg_proc_name + "' not found in class '" + arg_class_name + "'" );

   string field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( proc_id, "proc", e_model_element_type_procedure ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

void field_inc_in_proc_specification::read_data( sio_reader& reader )
{
   value = reader.read_attribute( c_attribute_value );
}

void field_inc_in_proc_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_value, value );
}

void field_inc_in_proc_specification::add_specification_data( model& /*m*/, specification_data& spec_data ) const
{
   spec_data.data_pairs.push_back( make_pair( c_data_value, value ) );
}

string field_inc_in_proc_specification::static_class_name( ) { return "field_inc_in_proc"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_inc_in_proc_specification, static_class_name );

struct field_required_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   static string static_class_name( );
};

void field_required_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 2 )
      throw runtime_error( "unexpected args < 2 for 'field_required' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );

   string class_id = get_class_id_for_name( m, arg_class_name );
   string field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, 0, true );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

string field_required_specification::static_class_name( ) { return "field_required"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, field_required_specification, static_class_name );

struct file_attachment_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   bool is_facet_specification( ) const { return true; }

   static string static_class_name( );
};

void file_attachment_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 2 )
      throw runtime_error( "unexpected number of args < 2 for 'file_attachment' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );

   string arg_sfield_name;
   if( args.size( ) > 2 )
      arg_sfield_name = args[ 2 ];

   string arg_ofield_name;
   if( args.size( ) > 3 )
      arg_ofield_name = args[ 3 ];

   vector< specification_data > all_spec_data;
   m.get_specification_data( arg_class_name, all_spec_data );
   for( size_t i = 0; i < all_spec_data.size( ); i++ )
   {
      if( all_spec_data[ i ].type == static_class_name( ) )
         throw runtime_error( "specification type '" + static_class_name( ) + "' can only be used once per class" );
   }

   string class_id = get_class_id_for_name( m, arg_class_name );
   string field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   string sfield_id;

   if( !arg_sfield_name.empty( ) )
   {
      sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_name );

      if( sfield_id.empty( ) )
         throw runtime_error( "unknown field name '" + arg_sfield_name + "' for class '" + arg_class_name + "'" );
   }

   string ofield_id;

   if( !arg_ofield_name.empty( ) )
   {
      ofield_id = get_field_id_for_name( m, arg_class_name, arg_ofield_name );

      if( ofield_id.empty( ) )
         throw runtime_error( "unknown field name '" + arg_ofield_name + "' for class '" + arg_class_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );

   if( !sfield_id.empty( ) )
      details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );

   if( !ofield_id.empty( ) )
      details.push_back( specification_detail( ofield_id, "ofield", e_model_element_type_field ) );
}

string file_attachment_specification::static_class_name( ) { return "file_attachment"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, file_attachment_specification, static_class_name );

struct file_link_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;

   string field_id;
   string field1_id;
   string field2_id;
   string pfield2_id;

   string ext;
   string append_to_ext;
};

void file_link_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'file_link' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field1_name( args[ 1 ] );
   string arg_field_name( args[ 2 ] );

   ext = args[ 3 ];

   string arg_field2_info;

   for( size_t arg = 4; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg.find( c_arg_append_to_ext_prefix ) == 0 )
         append_to_ext = next_arg.substr( strlen( c_arg_append_to_ext_prefix ) );
      else if( arg_field2_info.empty( ) )
         arg_field2_info = next_arg;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'file_link' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   if( !arg_field2_info.empty( ) )
   {
      string field2_name( arg_field2_info );
      string field2_class_name( arg_class_name );

      string::size_type pos = arg_field2_info.find( ':' );
      if( pos != string::npos )
      {
         string pfield2_name( arg_field2_info.substr( 0, pos ) );
         field2_name.erase( 0, pos + 1 );

         string pfield2_type;
         pfield2_id = get_field_id_for_name( m, field2_class_name, pfield2_name, &pfield2_type, true );

         if( pfield2_id.empty( ) )
            throw runtime_error( "unknown field '" + pfield2_name + "' for class '" + field2_class_name + "'" );

         string pclass_name( get_class_name_from_field_type( m, field2_class_name, pfield2_name, pfield2_type ) );

         field2_class_name = pclass_name;
         pclass_id = get_class_id_for_name( m, field2_class_name );
      }

      field2_id = get_field_id_for_name( m, field2_class_name, field2_name );
      if( field2_id.empty( ) )
         throw runtime_error( "unknown field '" + field2_name + "' for class '" + field2_class_name + "'" );
   }

   field1_id = get_field_id_for_name( m, arg_class_name, arg_field1_name );
   if( field1_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field1_name + "' for class '" + arg_class_name + "'" );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !pclass_id.empty( ) )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( field1_id, "field1", e_model_element_type_field ) );
   if( !field2_id.empty( ) )
      details.push_back( specification_detail( field2_id, "field2", e_model_element_type_field ) );
}

void file_link_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_opt_attribute( c_attribute_pclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   field1_id = reader.read_attribute( c_attribute_field1_id );

   field2_id = reader.read_opt_attribute( c_attribute_field2_id );
   pfield2_id = reader.read_opt_attribute( c_attribute_pfield2_id );

   ext = reader.read_attribute( c_attribute_ext );

   append_to_ext = reader.read_opt_attribute( c_attribute_append_to_ext );
}

void file_link_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_pclass_id, pclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_field1_id, field1_id );

   writer.write_opt_attribute( c_attribute_field2_id, field2_id );
   writer.write_opt_attribute( c_attribute_pfield2_id, pfield2_id );

   writer.write_attribute( c_attribute_ext, ext );

   writer.write_opt_attribute( c_attribute_append_to_ext, append_to_ext );
}

void file_link_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field2_class_name( class_name );
   if( !pclass_id.empty( ) )
      field2_class_name = get_class_name_for_id( m, pclass_id );

   string pfield2_name;
   if( !pfield2_id.empty( ) )
      pfield2_name = get_field_name_for_id( m, class_name, pfield2_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield2, pfield2_name ) );

   string field2_name;
   if( !field2_id.empty( ) )
      field2_name = get_field_name_for_id( m, field2_class_name, field2_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field2, field2_name ) );

   string field1_name = get_field_name_for_id( m, class_name, field1_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field1, field1_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_ext, ext ) );

   spec_data.data_pairs.push_back( make_pair( c_data_append_to_ext, append_to_ext ) );
}

string file_link_specification::static_class_name( ) { return "file_link"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, file_link_specification, static_class_name );

struct fk_default_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string default_value;
};

void fk_default_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 3 )
      throw runtime_error( "unexpected number of args != 3 for 'fk_default' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_default_value( args[ 2 ] );

   string class_id = get_class_id_for_name( m, arg_class_name );
   string field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, 0, true );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   default_value = arg_default_value;

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

void fk_default_specification::read_data( sio_reader& reader )
{
   default_value = reader.read_attribute( c_attribute_default_value );
}

void fk_default_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_default_value, default_value );
}

void fk_default_specification::add_specification_data( model& /*m*/, specification_data& spec_data ) const
{
   spec_data.data_pairs.push_back( make_pair( c_data_value, default_value ) );
}

string fk_default_specification::static_class_name( ) { return "fk_default"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, fk_default_specification, static_class_name );

struct fk_link_first_child_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string field_id;
   string pfield_id;
   string relationship_id;
   string child_class_id;
   string child_field_id;
   string order_field_id;
   string status_field_id;
   string status_value;

   bool reverse;
   bool new_only;
   bool not_create;
};

void fk_link_first_child_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'fk_link_first_child' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_child_name( args[ 2 ] );
   string arg_child_field_name;

   string arg_order_info, arg_status_info;

   new_only = false;
   not_create = false;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg.find( c_arg_order_prefix ) == 0 )
         arg_order_info = next_arg.substr( strlen( c_arg_order_prefix ) );
      else if( next_arg.find( c_arg_status_prefix ) == 0 )
         arg_status_info = next_arg.substr( strlen( c_arg_status_prefix ) );
      else if( next_arg == c_arg_reverse )
         reverse = true;
      else if( next_arg == c_arg_new_only )
         new_only = true;
      else if( next_arg == c_arg_not_create )
         not_create = true;
      else if( arg_child_field_name.empty( ) )
         arg_child_field_name = next_arg;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'fk_link_first_child' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, 0, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   string parent_class_name( arg_class_name );

   string::size_type pos = arg_child_name.find( "." );
   if( pos != string::npos )
   {
      string pfield_name( arg_child_name.substr( 0, pos ) );
      arg_child_name.erase( 0, pos + 1 );

      string pfield_type;
      pfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, &pfield_type, true );
      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      parent_class_name = get_class_name_from_field_type( m, arg_class_name, pfield_name, pfield_type );
   }

   string relationship_name( decorated_class_name( parent_class_name ) );
   relationship_name += '_' + arg_child_name;

   relationship_data rel_data;
   m.get_relationship_data( relationship_name, rel_data );

   relationship_id = rel_data.id;

   child_class_id = get_class_id_for_name( m, rel_data.child_class_name );

   if( !arg_child_field_name.empty( ) )
   {
      child_field_id = get_field_id_for_name( m, rel_data.child_class_name, arg_child_field_name, 0, true );
      if( child_field_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_child_field_name + "' for class '" + rel_data.child_class_name + "'" );
   }

   if( !arg_order_info.empty( ) )
   {
      order_field_id = get_field_id_for_name( m, rel_data.child_class_name, arg_order_info );
      if( order_field_id.empty( ) )
         throw runtime_error( "unknown order field '" + arg_order_info + "' for child class '" + rel_data.child_class_name + "'" );
   }

   if( !arg_status_info.empty( ) )
   {
      string::size_type pos = arg_status_info.find( ',' );
      if( pos == string::npos )
         throw runtime_error( "unexpected format for status info '" + arg_status_info + "'" );

      status_field_id = get_field_id_for_name( m, arg_class_name, arg_status_info.substr( 0, pos ), 0, true );
      if( status_field_id.empty( ) )
         throw runtime_error( "unknown status field '" + arg_status_info.substr( 0, pos ) + "' for class '" + arg_class_name + "'" );

      status_value = arg_status_info.substr( pos + 1 );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( child_class_id, "child_class", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );

   if( !pfield_id.empty( ) )
      details.push_back( specification_detail( pfield_id, "parent_field", e_model_element_type_field ) );

   if( !child_field_id.empty( ) )
      details.push_back( specification_detail( child_field_id, "child_field", e_model_element_type_field ) );

   if( !order_field_id.empty( ) )
      details.push_back( specification_detail( order_field_id, "order_field", e_model_element_type_field ) );

   if( !status_field_id.empty( ) )
      details.push_back( specification_detail( status_field_id, "status_field", e_model_element_type_field ) );

   details.push_back( specification_detail( relationship_id, "relationship", e_model_element_type_relationship ) );
}

void fk_link_first_child_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   relationship_id = reader.read_attribute( c_attribute_relationship_id );
   child_class_id = reader.read_attribute( c_attribute_child_class_id );

   pfield_id = reader.read_opt_attribute( c_attribute_pfield_id );
   child_field_id = reader.read_opt_attribute( c_attribute_child_field_id );
   order_field_id = reader.read_opt_attribute( c_attribute_order_field_id );
   status_field_id = reader.read_opt_attribute( c_attribute_status_field_id );
   status_value = reader.read_opt_attribute( c_attribute_status_value );

   reverse = ( reader.read_opt_attribute( c_attribute_is_reverse ) == c_true );
   new_only = ( reader.read_opt_attribute( c_attribute_new_only ) == c_true );
   not_create = ( reader.read_opt_attribute( c_attribute_not_create ) == c_true );
}

void fk_link_first_child_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_relationship_id, relationship_id );
   writer.write_attribute( c_attribute_child_class_id, child_class_id );

   writer.write_opt_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_opt_attribute( c_attribute_child_field_id, child_field_id );
   writer.write_opt_attribute( c_attribute_order_field_id, order_field_id );
   writer.write_opt_attribute( c_attribute_status_field_id, status_field_id );
   writer.write_opt_attribute( c_attribute_status_value, status_value );

   writer.write_opt_attribute( c_attribute_is_reverse, reverse ? c_true : "" );
   writer.write_opt_attribute( c_attribute_new_only, new_only ? c_true : "" );
   writer.write_opt_attribute( c_attribute_not_create, not_create ? c_true : "" );
}

void fk_link_first_child_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string pfield_name;
   if( !pfield_id.empty( ) )
      pfield_name = get_field_name_for_id( m, class_name, pfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string child_class_name = get_class_name_for_id( m, child_class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_child_class, child_class_name ) );

   string child_field_name;
   if( !child_field_id.empty( ) )
      child_field_name = get_field_name_for_id( m, child_class_name, child_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_child_field, child_field_name ) );

   string order_field_name;
   if( !order_field_id.empty( ) )
      order_field_name = get_field_name_for_id( m, child_class_name, order_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_order_field, order_field_name ) );

   string status_field_name;
   if( !status_field_id.empty( ) )
      status_field_name = get_field_name_for_id( m, class_name, status_field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_status_field, status_field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_status_value, status_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_reverse, reverse ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_new_only, new_only ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_not_create, not_create ? c_true : "" ) );
}

string fk_link_first_child_specification::static_class_name( ) { return "fk_link_first_child"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, fk_link_first_child_specification, static_class_name );

struct folder_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   bool is_facet_specification( ) const { return true; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
};

void folder_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) > 2 )
      throw runtime_error( "unexpected number of args > 2 for 'folder' specification" );

   if( args.size( ) < 1 )
      throw runtime_error( "'class name' argument is required for the 'folder' specification" );

   string field_id;
   string relationship_id;

   string arg_class_name( args[ 0 ] );

   bool found_acyclic = false;
   vector< specification_data > all_spec_data;
   m.get_specification_data( arg_class_name, all_spec_data );
   for( size_t i = 0; i < all_spec_data.size( ); i++ )
   {
      if( all_spec_data[ i ].type == static_class_name( ) )
         throw runtime_error( "specification type '" + static_class_name( ) + "' can only be used once per class" );

      if( all_spec_data[ i ].type == acyclic_specification::static_class_name( ) )
         found_acyclic = true;
   }

   if( !found_acyclic )
      throw runtime_error( "cannot use the 'folder' specification without first providing an 'acyclic' specification" );

   class_data cls_data;
   m.get_class_data( arg_class_name, cls_data );

   if( cls_data.fixed_key_val.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' must have a non-blank fixed key value" );

   class_id = cls_data.id;

   if( args.size( ) < 2 )
      throw runtime_error( "'relationship name' argument is required for the 'folder' specification" );

   string arg_relationship_name( args[ 1 ] );

   relationship_data rel_data;
   m.get_relationship_data( arg_relationship_name, rel_data );

   if( rel_data.child_class_name != rel_data.parent_class_name )
      throw runtime_error( "relationship '" + arg_relationship_name + "' is not a self-relationship" );

   if( arg_class_name != rel_data.child_class_name )
      throw runtime_error( "relationship '"
       + arg_relationship_name + "' is not a self-relationship for class '" + arg_class_name + "'" );

   relationship_id = rel_data.id;

   vector< field_data > rel_field_data;
   m.get_field_data( arg_class_name, rel_field_data, e_get_field_type_relationships );

   for( size_t i = 0; i < rel_field_data.size( ); i++ )
   {
      if( rel_field_data[ i ].decorated_name == arg_relationship_name )
      {
         field_id = rel_field_data[ i ].id;
         break;
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( relationship_id, "relationship", e_model_element_type_relationship ) );
}

void folder_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
}

void folder_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
}

void folder_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   vector< class_data > all_class_data;
   m.get_class_data( all_class_data );

   string root_key;
   for( size_t i = 0; i < all_class_data.size( ); i++ )
   {
      if( all_class_data[ i ].id == class_id )
      {
         root_key = all_class_data[ i ].fixed_key_val;
         break;
      }
   }

   spec_data.data_pairs.push_back( make_pair( c_data_root_key, root_key ) );
}

string folder_specification::static_class_name( ) { return "folder"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, folder_specification, static_class_name );

struct for_auto_code_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string mask_field_id;
   string next_field_id;
   string exhausted_field_id;
   string increment_procedure_id;
   string increment_next_value_arg_id;
};

void for_auto_code_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 6 )
      throw runtime_error( "unexpected number of args != 6 for 'for_auto_code' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_increment_procedure_name( args[ 1 ] );
   string arg_increment_procedure_arg_name( args[ 2 ] );
   string arg_mask_field_name( args[ 3 ] );
   string arg_next_field_name( args[ 4 ] );
   string arg_exhausted_field_name( args[ 5 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   mask_field_id = get_field_id_for_name( m, arg_class_name, arg_mask_field_name );
   if( mask_field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_mask_field_name + "' for class '" + arg_class_name + "'" );

   next_field_id = get_field_id_for_name( m, arg_class_name, arg_next_field_name );
   if( next_field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_next_field_name + "' for class '" + arg_class_name + "'" );

   exhausted_field_id = get_field_id_for_name( m, arg_class_name, arg_exhausted_field_name );
   if( exhausted_field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_exhausted_field_name + "' for class '" + arg_class_name + "'" );

   increment_procedure_id = get_procedure_id_for_name( m, arg_class_name, arg_increment_procedure_name );
   if( increment_procedure_id.empty( ) )
      throw runtime_error( "increment procedure '" + arg_increment_procedure_name + "' not found in class '" + arg_class_name + "'" );

   increment_next_value_arg_id = get_procedure_arg_id_for_name( m,
    arg_class_name, arg_increment_procedure_name, arg_increment_procedure_arg_name );
   if( increment_next_value_arg_id.empty( ) )
      throw runtime_error( "increment procedure arg '" + arg_increment_procedure_arg_name + "' not found in class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   details.push_back( specification_detail( mask_field_id, "mask_field", e_model_element_type_field ) );
   details.push_back( specification_detail( next_field_id, "next_field", e_model_element_type_field ) );
   details.push_back( specification_detail( exhausted_field_id, "exhausted_field", e_model_element_type_field ) );

   details.push_back( specification_detail( increment_procedure_id, "increment_procedure", e_model_element_type_procedure ) );
   details.push_back( specification_detail( increment_next_value_arg_id, "increment_next_value_arg", e_model_element_type_procedure_arg ) );
}

void for_auto_code_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );

   mask_field_id = reader.read_attribute( c_attribute_mask_field_id );
   next_field_id = reader.read_attribute( c_attribute_next_field_id );
   exhausted_field_id = reader.read_attribute( c_attribute_exhausted_field_id );

   increment_procedure_id = reader.read_attribute( c_attribute_increment_procedure_id );
   increment_next_value_arg_id = reader.read_attribute( c_attribute_increment_next_value_arg_id );
}

void for_auto_code_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );

   writer.write_attribute( c_attribute_mask_field_id, mask_field_id );
   writer.write_attribute( c_attribute_next_field_id, next_field_id );
   writer.write_attribute( c_attribute_exhausted_field_id, exhausted_field_id );

   writer.write_attribute( c_attribute_increment_procedure_id, increment_procedure_id );
   writer.write_attribute( c_attribute_increment_next_value_arg_id, increment_next_value_arg_id );
}

void for_auto_code_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string mask_field_name = get_field_name_for_id( m, class_name, mask_field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_mask_field ), mask_field_name ) );

   string next_field_name = get_field_name_for_id( m, class_name, next_field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_next_field ), next_field_name ) );

   string exhausted_field_name = get_field_name_for_id( m, class_name, exhausted_field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_exhausted_field ), exhausted_field_name ) );

   string increment_procedure_name = get_procedure_name_for_id( m, class_name, increment_procedure_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_inc_proc ), increment_procedure_name ) );

   string increment_next_value_arg_name
    = get_procedure_arg_name_for_id( m, class_name, increment_procedure_name, increment_next_value_arg_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_inc_value ), increment_next_value_arg_name ) );
}

string for_auto_code_specification::static_class_name( ) { return "for_auto_code"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, for_auto_code_specification, static_class_name );

struct for_auto_code_user_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string field_id;
   string auto_class_id;
   string increment_procedure_id;
};

void for_auto_code_user_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 4 )
      throw runtime_error( "unexpected number of args != 4 for 'for_auto_class_user' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_auto_class_name( args[ 2 ] );
   string arg_increment_procedure_name( args[ 3 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   bool is_external_alias;
   auto_class_id = get_class_id_for_name( m, arg_auto_class_name, &is_external_alias );

   increment_procedure_id = get_procedure_id_for_name( m, arg_auto_class_name, arg_increment_procedure_name );
   if( increment_procedure_id.empty( ) )
      throw runtime_error( "increment procedure '" + arg_increment_procedure_name + "' not found in class '" + arg_auto_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( auto_class_id, "auto_class", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   if( !is_external_alias )
      details.push_back( specification_detail( increment_procedure_id, "increment_procedure", e_model_element_type_procedure ) );
}

void for_auto_code_user_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   auto_class_id = reader.read_attribute( c_attribute_auto_class_id );
   increment_procedure_id = reader.read_attribute( c_attribute_increment_procedure_id );
}

void for_auto_code_user_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_auto_class_id, auto_class_id );
   writer.write_attribute( c_attribute_increment_procedure_id, increment_procedure_id );
}

void for_auto_code_user_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   bool is_mandatory;
   string field_name = get_field_name_for_id( m, class_name, field_id, 0, false, false, &is_mandatory );

   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fmandatory, is_mandatory ? "1" : "0" ) );

   string auto_class_name = get_class_name_for_id( m, auto_class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_auto_class, auto_class_name ) );

   string increment_procedure_name = get_procedure_name_for_id( m, auto_class_name, increment_procedure_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_inc_proc ), increment_procedure_name ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_inc_clones ), "" ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_key ), "" ) );
}

string for_auto_code_user_specification::static_class_name( ) { return "for_auto_code_user"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, for_auto_code_user_specification, static_class_name );

struct home_message_stats_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == user_class_id; }

   bool is_facet_specification( ) const { return true; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   void get_specification_strings( model& m, vector< pair< string, string > >& strings ) const;

   static string static_class_name( );

   string user_class_id;
   string group_class_id;

   string stats_class_1_id;
   string stats_class_1_fld_id;
   string stats_class_1_fk_id;
   string stats_class_1_fk_cid;
   string stats_class_1_fk_value;
   string stats_class_1_fk_title;
   string stats_class_1_fk_2_id;
   string stats_class_1_fk_2_cid;
   string stats_class_1_fk_2_value;
   string stats_class_1_fk_2_title;

   string stats_class_2_id;
   string stats_class_2_fld_id;
   string stats_class_2_fk_id;
   string stats_class_2_fk_cid;
   string stats_class_2_fk_value;
   string stats_class_2_fk_title;
   string stats_class_2_fk_2_id;
   string stats_class_2_fk_2_cid;
   string stats_class_2_fk_2_value;
   string stats_class_2_fk_2_title;
};

void home_message_stats_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 1 )
      throw runtime_error( "home_message_stats specification requires 'user class'" );

   string arg_class_name( args[ 0 ] );
   string arg_stats_class_1_info;
   string arg_stats_class_2_info;

   if( args.size( ) > 1 )
      arg_stats_class_1_info = args[ 1 ];

   if( args.size( ) > 2 )
      arg_stats_class_2_info = args[ 2 ];

   string group_class_name;
   string::size_type pos = arg_class_name.find( ',' );
   if( pos != string::npos )
   {
      group_class_name = arg_class_name.substr( pos + 1 );
      arg_class_name.erase( pos );
   }

   user_class_id = get_class_id_for_name( m, arg_class_name );

   if( !group_class_name.empty( ) )
      group_class_id = get_class_id_for_name( m, group_class_name );

   bool class_1_is_external;
   bool class_2_is_external;

   if( !arg_stats_class_1_info.empty( ) )
   {
      string cname, fk_name, fk_type, fk2_name, fk2_type;

      string::size_type pos = arg_stats_class_1_info.find( ',' );
      cname = arg_stats_class_1_info.substr( 0, pos );

      if( pos != string::npos )
      {
         arg_stats_class_1_info.erase( 0, pos + 1 );
         pos = arg_stats_class_1_info.find( ',' );

         fk_name = arg_stats_class_1_info.substr( 0, pos );

         if( pos == string::npos )
            arg_stats_class_1_info.erase( );
         else
            arg_stats_class_1_info.erase( 0, pos + 1 );

         pos = arg_stats_class_1_info.find( ',' );
         stats_class_1_fk_value = arg_stats_class_1_info.substr( 0, pos );

         if( pos != string::npos )
         {
            arg_stats_class_1_info.erase( 0, pos + 1 );
            pos = arg_stats_class_1_info.find( ',' );

            fk2_name = arg_stats_class_1_info.substr( 0, pos );

            if( pos == string::npos )
               arg_stats_class_1_info.erase( );
            else
               arg_stats_class_1_info.erase( 0, pos + 1 );

            stats_class_1_fk_2_value = arg_stats_class_1_info;

            pos = stats_class_1_fk_2_value.find( ':' );
            if( pos != string::npos )
            {
               stats_class_1_fk_2_title = stats_class_1_fk_2_value.substr( pos + 1 );
               stats_class_1_fk_2_value.erase( pos );
            }
         }

         pos = stats_class_1_fk_value.find( ':' );
         if( pos != string::npos )
         {
            stats_class_1_fk_title = stats_class_1_fk_value.substr( pos + 1 );
            stats_class_1_fk_value.erase( pos );
         }
      }

      string fname;
      pos = cname.find( ':' );
      if( pos != string::npos )
      {
         fname = cname.substr( pos + 1 );
         cname.erase( pos );
      }

      stats_class_1_id = get_class_id_for_name( m, cname, &class_1_is_external );

      if( !fname.empty( ) )
         stats_class_1_fld_id = get_field_id_for_name( m, cname, fname, 0, false, false, true );

      if( !fk_name.empty( ) )
      {
         stats_class_1_fk_id = get_field_id_for_name( m, cname, fk_name, &fk_type, true, false, true );

         string parent_class_name( get_class_name_from_field_type( m, cname, fk_name, fk_type ) );
         if( !parent_class_name.empty( ) )
            stats_class_1_fk_cid = get_class_id_for_name( m, parent_class_name );
      }

      if( !fk2_name.empty( ) )
      {
         stats_class_1_fk_2_id = get_field_id_for_name( m, cname, fk2_name, &fk2_type, true, false, true );

         string parent_class_name( get_class_name_from_field_type( m, cname, fk2_name, fk2_type ) );
         if( !parent_class_name.empty( ) )
            stats_class_1_fk_2_cid = get_class_id_for_name( m, parent_class_name );
      }
   }

   if( !arg_stats_class_2_info.empty( ) )
   {
      string cname, fk_name, fk_type, fk2_name, fk2_type;

      string::size_type pos = arg_stats_class_2_info.find( ',' );
      cname = arg_stats_class_2_info.substr( 0, pos );

      if( pos != string::npos )
      {
         arg_stats_class_2_info.erase( 0, pos + 1 );
         pos = arg_stats_class_2_info.find( ',' );

         fk_name = arg_stats_class_2_info.substr( 0, pos );

         if( pos == string::npos )
            arg_stats_class_2_info.erase( );
         else
            arg_stats_class_2_info.erase( 0, pos + 1 );

         pos = arg_stats_class_2_info.find( ',' );
         stats_class_2_fk_value = arg_stats_class_2_info.substr( 0, pos );

         if( pos != string::npos )
         {
            arg_stats_class_2_info.erase( 0, pos + 1 );
            pos = arg_stats_class_2_info.find( ',' );

            fk2_name = arg_stats_class_2_info.substr( 0, pos );

            if( pos == string::npos )
               arg_stats_class_2_info.erase( );
            else
               arg_stats_class_2_info.erase( 0, pos + 1 );

            stats_class_2_fk_2_value = arg_stats_class_2_info;

            pos = stats_class_2_fk_2_value.find( ':' );
            if( pos != string::npos )
            {
               stats_class_2_fk_2_title = stats_class_2_fk_2_value.substr( pos + 1 );
               stats_class_2_fk_2_value.erase( pos );
            }
         }

         pos = stats_class_2_fk_value.find( ':' );
         if( pos != string::npos )
         {
            stats_class_2_fk_title = stats_class_2_fk_value.substr( pos + 1 );
            stats_class_2_fk_value.erase( pos );
         }
      }

      string fname;
      pos = cname.find( ':' );
      if( pos != string::npos )
      {
         fname = cname.substr( pos + 1 );
         cname.erase( pos );
      }

      stats_class_2_id = get_class_id_for_name( m, cname, &class_2_is_external );

      if( !fname.empty( ) )
         stats_class_2_fld_id = get_field_id_for_name( m, cname, fname, 0, false, false, true );

      if( !fk_name.empty( ) )
      {
         stats_class_2_fk_id = get_field_id_for_name( m, cname, fk_name, &fk_type, true, false, true );

         string parent_class_name( get_class_name_from_field_type( m, cname, fk_name, fk_type ) );
         if( !parent_class_name.empty( ) )
            stats_class_2_fk_cid = get_class_id_for_name( m, parent_class_name );
      }

      if( !fk2_name.empty( ) )
      {
         stats_class_2_fk_2_id = get_field_id_for_name( m, cname, fk2_name, &fk2_type, true, false, true );

         string parent_class_name( get_class_name_from_field_type( m, cname, fk2_name, fk2_type ) );
         if( !parent_class_name.empty( ) )
            stats_class_2_fk_2_cid = get_class_id_for_name( m, parent_class_name );
      }
   }

   details.push_back( specification_detail( user_class_id, "user_class", e_model_element_type_class ) );

   if( !group_class_id.empty( ) )
      details.push_back( specification_detail( group_class_id, "group_class", e_model_element_type_class ) );

   if( !stats_class_1_id.empty( ) )
      details.push_back( specification_detail( stats_class_1_id, "stats_class_1", e_model_element_type_class ) );

   if( !stats_class_2_id.empty( ) )
      details.push_back( specification_detail( stats_class_2_id, "stats_class_2", e_model_element_type_class ) );

   if( !stats_class_1_fk_cid.empty( ) )
      details.push_back( specification_detail( stats_class_1_fk_cid, "stats_class_1_fk", e_model_element_type_class ) );

   if( !stats_class_1_fk_2_cid.empty( ) )
      details.push_back( specification_detail( stats_class_1_fk_2_cid, "stats_class_1_fk_2", e_model_element_type_class ) );

   if( !stats_class_2_fk_cid.empty( ) )
      details.push_back( specification_detail( stats_class_2_fk_cid, "stats_class_2_fk", e_model_element_type_class ) );

   if( !stats_class_2_fk_2_cid.empty( ) )
      details.push_back( specification_detail( stats_class_2_fk_2_cid, "stats_class_2_fk_2", e_model_element_type_class ) );

   if( !class_1_is_external && !stats_class_1_fld_id.empty( ) )
      details.push_back( specification_detail( stats_class_1_fld_id, "stats_class_1_fld", e_model_element_type_field ) );

   if( !class_1_is_external && !stats_class_1_fk_id.empty( ) )
      details.push_back( specification_detail( stats_class_1_fk_id, "stats_class_1_fk_field", e_model_element_type_field ) );

   if( !class_1_is_external && !stats_class_1_fk_2_id.empty( ) )
      details.push_back( specification_detail( stats_class_1_fk_2_id, "stats_class_1_fk_2_field", e_model_element_type_field ) );

   if( !class_2_is_external && !stats_class_2_fld_id.empty( ) )
      details.push_back( specification_detail( stats_class_2_fld_id, "stats_class_2_fld", e_model_element_type_field ) );

   if( !class_2_is_external && !stats_class_2_fk_id.empty( ) )
      details.push_back( specification_detail( stats_class_2_fk_id, "stats_class_2_fk_field", e_model_element_type_field ) );

   if( !class_2_is_external && !stats_class_2_fk_2_id.empty( ) )
      details.push_back( specification_detail( stats_class_2_fk_2_id, "stats_class_2_fk_2_field", e_model_element_type_field ) );
}

void home_message_stats_specification::read_data( sio_reader& reader )
{
   user_class_id = reader.read_attribute( c_attribute_class_id );
   group_class_id = reader.read_opt_attribute( c_attribute_gclass_id );

   stats_class_1_id = reader.read_opt_attribute( c_attribute_class1_id );
   stats_class_1_fld_id = reader.read_opt_attribute( c_attribute_class1_fld_id );
   stats_class_1_fk_id = reader.read_opt_attribute( c_attribute_class1_fk_id );
   stats_class_1_fk_cid = reader.read_opt_attribute( c_attribute_class1_fk_cid );
   stats_class_1_fk_value = reader.read_opt_attribute( c_attribute_class1_fk_value );
   stats_class_1_fk_title = reader.read_opt_attribute( c_attribute_class1_fk_title );
   stats_class_1_fk_2_id = reader.read_opt_attribute( c_attribute_class1_fk2_id );
   stats_class_1_fk_2_cid = reader.read_opt_attribute( c_attribute_class1_fk2_cid );
   stats_class_1_fk_2_value = reader.read_opt_attribute( c_attribute_class1_fk2_value );
   stats_class_1_fk_2_title = reader.read_opt_attribute( c_attribute_class1_fk2_title );

   stats_class_2_id = reader.read_opt_attribute( c_attribute_class2_id );
   stats_class_2_fld_id = reader.read_opt_attribute( c_attribute_class2_fld_id );
   stats_class_2_fk_id = reader.read_opt_attribute( c_attribute_class2_fk_id );
   stats_class_2_fk_cid = reader.read_opt_attribute( c_attribute_class2_fk_cid );
   stats_class_2_fk_value = reader.read_opt_attribute( c_attribute_class2_fk_value );
   stats_class_2_fk_title = reader.read_opt_attribute( c_attribute_class2_fk_title );
   stats_class_2_fk_2_id = reader.read_opt_attribute( c_attribute_class2_fk2_id );
   stats_class_2_fk_2_cid = reader.read_opt_attribute( c_attribute_class2_fk2_cid );
   stats_class_2_fk_2_value = reader.read_opt_attribute( c_attribute_class2_fk2_value );
   stats_class_2_fk_2_title = reader.read_opt_attribute( c_attribute_class2_fk2_title );
}

void home_message_stats_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, user_class_id );
   writer.write_opt_attribute( c_attribute_gclass_id, group_class_id );

   writer.write_opt_attribute( c_attribute_class1_id, stats_class_1_id );
   writer.write_opt_attribute( c_attribute_class1_fld_id, stats_class_1_fld_id );
   writer.write_opt_attribute( c_attribute_class1_fk_id, stats_class_1_fk_id );
   writer.write_opt_attribute( c_attribute_class1_fk_cid, stats_class_1_fk_cid );
   writer.write_opt_attribute( c_attribute_class1_fk_value, stats_class_1_fk_value );
   writer.write_opt_attribute( c_attribute_class1_fk_title, stats_class_1_fk_title );
   writer.write_opt_attribute( c_attribute_class1_fk2_id, stats_class_1_fk_2_id );
   writer.write_opt_attribute( c_attribute_class1_fk2_cid, stats_class_1_fk_2_cid );
   writer.write_opt_attribute( c_attribute_class1_fk2_value, stats_class_1_fk_2_value );
   writer.write_opt_attribute( c_attribute_class1_fk2_title, stats_class_1_fk_2_title );

   writer.write_opt_attribute( c_attribute_class2_id, stats_class_2_id );
   writer.write_opt_attribute( c_attribute_class2_fld_id, stats_class_2_fld_id );
   writer.write_opt_attribute( c_attribute_class2_fk_id, stats_class_2_fk_id );
   writer.write_opt_attribute( c_attribute_class2_fk_cid, stats_class_2_fk_cid );
   writer.write_opt_attribute( c_attribute_class2_fk_value, stats_class_2_fk_value );
   writer.write_opt_attribute( c_attribute_class2_fk_title, stats_class_2_fk_title );
   writer.write_opt_attribute( c_attribute_class2_fk2_id, stats_class_2_fk_2_id );
   writer.write_opt_attribute( c_attribute_class2_fk2_cid, stats_class_2_fk_2_cid );
   writer.write_opt_attribute( c_attribute_class2_fk2_value, stats_class_2_fk_2_value );
   writer.write_opt_attribute( c_attribute_class2_fk2_title, stats_class_2_fk_2_title );
}

void home_message_stats_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string user_class_name = get_class_name_for_id( m, user_class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_class, user_class_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_type, "stats" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_command, m.get_name( ) + "_Home_Message" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_cmd_args, "$user" ) );

   string group_class_name;
   if( !group_class_id.empty( ) )
   {
      group_class_name = get_class_name_for_id( m, group_class_id );
      spec_data.data_pairs.push_back( make_pair( c_data_secondary_class, group_class_name ) );
   }
   else
      spec_data.data_pairs.push_back( make_pair( c_data_secondary_class, "" ) );

   string stats_1_class_name;
   string stats_1_field_name;
   string stats_1_fk_class_name;
   string stats_1_fk_field_name;
   string stats_1_fk_2_class_name;
   string stats_1_fk_2_field_name;

   string stats_2_class_name;
   string stats_2_field_name;
   string stats_2_fk_class_name;
   string stats_2_fk_field_name;
   string stats_2_fk_2_class_name;
   string stats_2_fk_2_field_name;

   if( !stats_class_1_id.empty( ) )
      stats_1_class_name = get_class_name_for_id( m, stats_class_1_id );

   if( !stats_class_1_fld_id.empty( ) )
      stats_1_field_name = get_field_name_for_id( m, stats_1_class_name, stats_class_1_fld_id, 0, true );

   if( !stats_class_1_fk_cid.empty( ) )
      stats_1_fk_class_name = get_class_name_for_id( m, stats_class_1_fk_cid );

   if( !stats_class_1_fk_id.empty( ) )
      stats_1_fk_field_name = get_field_name_for_id( m, stats_1_class_name, stats_class_1_fk_id, 0, true );

   if( !stats_class_1_fk_2_cid.empty( ) )
      stats_1_fk_2_class_name = get_class_name_for_id( m, stats_class_1_fk_2_cid );

   if( !stats_class_1_fk_2_id.empty( ) )
      stats_1_fk_2_field_name = get_field_name_for_id( m, stats_1_class_name, stats_class_1_fk_2_id, 0, true );

   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats1_cclass, stats_1_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats1_cfield, stats_1_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats1_class1, stats_1_fk_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats1_field1, stats_1_fk_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats1_value1, stats_class_1_fk_value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats1_class2, stats_1_fk_2_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats1_field2, stats_1_fk_2_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats1_value2, stats_class_1_fk_2_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats1_cclass, stats_1_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats1_cfield, stats_1_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats1_class1, stats_1_fk_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats1_field1, stats_1_fk_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats1_value1, stats_class_1_fk_value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats1_class2, stats_1_fk_2_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats1_field2, stats_1_fk_2_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats1_value2, stats_class_1_fk_2_value ) );

   if( !stats_class_2_id.empty( ) )
      stats_2_class_name = get_class_name_for_id( m, stats_class_2_id );

   if( !stats_class_2_fld_id.empty( ) )
      stats_2_field_name = get_field_name_for_id( m, stats_2_class_name, stats_class_2_fld_id, 0, true );

   if( !stats_class_2_fk_cid.empty( ) )
      stats_2_fk_class_name = get_class_name_for_id( m, stats_class_2_fk_cid );

   if( !stats_class_2_fk_id.empty( ) )
      stats_2_fk_field_name = get_field_name_for_id( m, stats_2_class_name, stats_class_2_fk_id, 0, true );

   if( !stats_class_2_fk_2_cid.empty( ) )
      stats_2_fk_2_class_name = get_class_name_for_id( m, stats_class_2_fk_2_cid );

   if( !stats_class_2_fk_2_id.empty( ) )
      stats_2_fk_2_field_name = get_field_name_for_id( m, stats_2_class_name, stats_class_2_fk_2_id, 0, true );

   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats2_cclass, stats_2_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats2_cfield, stats_2_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats2_class1, stats_2_fk_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats2_field1, stats_2_fk_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats2_value1, stats_class_2_fk_value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats2_class2, stats_2_fk_2_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats2_field2, stats_2_fk_2_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_primary_stats2_value2, stats_class_2_fk_2_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats2_cclass, stats_2_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats2_cfield, stats_2_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats2_class1, stats_2_fk_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats2_field1, stats_2_fk_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats2_value1, stats_class_2_fk_value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats2_class2, stats_2_fk_2_class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats2_field2, stats_2_fk_2_field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_secondary_stats2_value2, stats_class_2_fk_2_value ) );

   spec_data.data_pairs.push_back( make_pair( "secondary_perm", "" ) );
   spec_data.data_pairs.push_back( make_pair( "primary_stats2_perm", "" ) );
   spec_data.data_pairs.push_back( make_pair( "secondary_stats2_perm", "" ) );
}

void home_message_stats_specification::get_specification_strings( model& m, vector< pair< string, string > >& strings ) const
{
   if( !stats_class_1_fk_id.empty( ) )
   {
      strings.push_back( make_pair( "primary_stats1_title",
       stats_class_1_fk_title.empty( ) ? stats_class_1_fk_value : stats_class_1_fk_title ) );

      if( !group_class_id.empty( ) )
         strings.push_back( make_pair( "secondary_stats1_title",
          stats_class_1_fk_title.empty( ) ? stats_class_1_fk_value : stats_class_1_fk_title ) );
   }

   if( !stats_class_2_fk_id.empty( ) )
   {
      strings.push_back( make_pair( "primary_stats2_title",
       stats_class_2_fk_title.empty( ) ? stats_class_2_fk_value : stats_class_2_fk_title ) );

      if( !group_class_id.empty( ) )
         strings.push_back( make_pair( "secondary_stats2_title",
          stats_class_2_fk_title.empty( ) ? stats_class_2_fk_value : stats_class_2_fk_title ) );
   }
}

string home_message_stats_specification::static_class_name( ) { return "home_message_stats"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, home_message_stats_specification, static_class_name );

struct home_message_top_n_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   bool is_facet_specification( ) const { return true; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;

   string field_id;
   string ofield_id;

   int limit;
};

void home_message_top_n_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "home_message_top_n specification requires 'class', 'field', 'ofield' and 'limit'" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_ofield_name( args[ 2 ] );
   string arg_limit( args[ 3 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   ofield_id = get_field_id_for_name( m, arg_class_name, arg_ofield_name );
   if( ofield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_ofield_name + "' for class '" + arg_class_name + "'" );

   limit = atoi( arg_limit.c_str( ) );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( ofield_id, "ofield", e_model_element_type_field ) );
}

void home_message_top_n_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   ofield_id = reader.read_attribute( c_attribute_ofield_id );
   limit = atoi( reader.read_attribute( c_attribute_limit ).c_str( ) );
}

void home_message_top_n_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_ofield_id, ofield_id );
   writer.write_attribute( c_attribute_limit, to_string( limit ) );
}

void home_message_top_n_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_type, "top_n" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_command, m.get_name( ) + "_Home_Message" ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string ofield_name = get_field_name_for_id( m, class_name, ofield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_ofield, ofield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_limit, to_string( limit ) ) );
}

string home_message_top_n_specification::static_class_name( ) { return "home_message_top_n"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, home_message_top_n_specification, static_class_name );

struct interest_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   static string static_class_name( );
};

void interest_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) > 6 )
      throw runtime_error( "unexpected number of args > 6 for 'interest' specification" );

   if( args.size( ) < 1 )
      throw runtime_error( "'class name' argument is required for the 'interest' specification" );

   string class_id;
   string rate_field_id;
   string balance_field_id;
   string calc_start_field_id;
   string calc_procedure_id;
   string calc_finish_arg_id;

   string arg_class_name( args[ 0 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   if( args.size( ) < 2 )
      throw runtime_error( "'rate field' argument is required for the 'interest' specification" );

   string arg_rate_field_name( args[ 1 ] );

   if( args.size( ) < 3 )
      throw runtime_error( "'balance field' argument is required for the 'interest' specification" );

   string arg_balance_field_name( args[ 2 ] );

   if( args.size( ) < 4 )
      throw runtime_error( "'calc start field' argument is required for the 'interest' specification" );

   string arg_calc_start_field_name( args[ 3 ] );

   vector< field_data > all_field_data;
   m.get_field_data( arg_class_name, all_field_data, e_get_field_type_any );

   for( size_t i = 0; i < all_field_data.size( ); i++ )
   {
      if( all_field_data[ i ].decorated_name == arg_rate_field_name )
      {
         if( all_field_data[ i ].sys_type != "numeric" )
            throw runtime_error( "invalid type '"
             + all_field_data[ i ].sys_type + "' for rate field '" + arg_rate_field_name + "'" );

         rate_field_id = all_field_data[ i ].id;
      }
      else if( all_field_data[ i ].decorated_name == arg_balance_field_name )
      {
         if( all_field_data[ i ].sys_type != "numeric" )
            throw runtime_error( "invalid type '"
             + all_field_data[ i ].sys_type + "' for balance field '" + arg_balance_field_name + "'" );

         balance_field_id = all_field_data[ i ].id;
      }
      else if( all_field_data[ i ].decorated_name == arg_calc_start_field_name )
      {
         if( all_field_data[ i ].sys_type != "datetime" )
            throw runtime_error( "invalid type '"
             + all_field_data[ i ].sys_type + "' for balance field '" + arg_balance_field_name + "'" );

         calc_start_field_id = all_field_data[ i ].id;
      }
   }

   if( rate_field_id.empty( ) )
      throw runtime_error( "rate field '" + arg_rate_field_name + "' not found in class '" + arg_class_name + "'" );

   if( balance_field_id.empty( ) )
      throw runtime_error( "balance field '" + arg_balance_field_name + "' not found in class '" + arg_class_name + "'" );

   if( calc_start_field_id.empty( ) )
      throw runtime_error( "calc start field '" + arg_calc_start_field_name + "' not found in class '" + arg_class_name + "'" );

   if( args.size( ) < 5 )
      throw runtime_error( "'calc procedure' argument is required for the 'interest' specification" );

   string arg_calc_procedure_name( args[ 4 ] );

   calc_procedure_id = get_procedure_id_for_name( m, arg_class_name, arg_calc_procedure_name );
   if( calc_procedure_id.empty( ) )
      throw runtime_error( "calc procedure '" + arg_calc_procedure_name + "' not found in class '" + arg_class_name + "'" );

   if( args.size( ) < 6 )
      throw runtime_error( "'calc finish' argument is required for the 'interest' specification" );

   string arg_calc_finish_name( args[ 5 ] );

   vector< procedure_arg_data > all_procedure_args;
   m.get_procedure_arg_data( arg_class_name, arg_calc_procedure_name, all_procedure_args );

   for( size_t i = 0; i < all_procedure_args.size( ); i++ )
   {
      if( all_procedure_args[ i ].name == arg_calc_finish_name )
      {
         if( all_procedure_args[ i ].type != "datetime" )
            throw runtime_error( "invalid type '"
             + all_procedure_args[ i ].type + "' for calc finish field '" + arg_calc_finish_name + "'" );

         calc_finish_arg_id = all_procedure_args[ i ].id;
         break;
      }
   }

   if( calc_finish_arg_id.empty( ) )
      throw runtime_error( "calc finish field '" + arg_calc_finish_name + "' not found in class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( rate_field_id, "rate", e_model_element_type_field ) );
   details.push_back( specification_detail( balance_field_id, "balance", e_model_element_type_field ) );
   details.push_back( specification_detail( calc_start_field_id, "calc_start", e_model_element_type_field ) );
   details.push_back( specification_detail( calc_procedure_id, "calc_procedure", e_model_element_type_procedure ) );
   details.push_back( specification_detail( calc_finish_arg_id, "calc_finish", e_model_element_type_procedure_arg ) );
}

string interest_specification::static_class_name( ) { return "interest"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, interest_specification, static_class_name );

struct key_generator_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   static string static_class_name( );
};

void key_generator_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "key_generator specification requires 'class', 'field', 'procedure' and 'return arg'" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_procedure_name( args[ 2 ] );
   string arg_procedure_arg_name( args[ 3 ] );

   string class_id = get_class_id_for_name( m, arg_class_name );
   string field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   string procedure_id = get_procedure_id_for_name( m, arg_class_name, arg_procedure_name );
   string procedure_arg_id = get_procedure_arg_id_for_name( m, arg_class_name, arg_procedure_name, arg_procedure_arg_name );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   if( procedure_id.empty( ) )
      throw runtime_error( "unknown procedure '" + arg_procedure_name + "' for class '" + arg_class_name + "'" );

   if( procedure_arg_id.empty( ) )
      throw runtime_error( "unknown procedure arg '" + arg_procedure_arg_name + "' for procedure '" + arg_procedure_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( procedure_id, "procedure", e_model_element_type_procedure ) );
   details.push_back( specification_detail( procedure_arg_id, "procedure_arg", e_model_element_type_procedure_arg ) );
}

string key_generator_specification::static_class_name( ) { return "key_generator"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, key_generator_specification, static_class_name );

struct meta_relationship_child_name_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;

   string field_id;
   string sfield_id;
   string pfield1_id;
   string pfield2_id;
   string pcfield1_id;
   string pcfield2_id;
};

void meta_relationship_child_name_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 5 )
      throw runtime_error( "unexpected number of args != 5 for 'meta_relationship_child_name' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_sfield_name( args[ 2 ] );
   string arg_parent_info1( args[ 3 ] );
   string arg_parent_info2( args[ 4 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_name );
   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + arg_class_name + "'" );

   string::size_type pos = arg_parent_info1.find( ':' );
   if( pos == string::npos )
      throw runtime_error( "unexpected child info missing for '" + arg_parent_info1 + "'" );

   string pfield1_name = arg_parent_info1.substr( 0, pos );
   string pchild1_name = arg_parent_info1.substr( pos + 1 );

   string pfield1_type;
   pfield1_id = get_field_id_for_name( m, arg_class_name, pfield1_name, &pfield1_type, true );

   if( pfield1_id.empty( ) )
      throw runtime_error( "unknown field '" + pfield1_name + "' for class '" + arg_class_name + "'" );

   string pclass_name( get_class_name_from_field_type( m, arg_class_name, pfield1_name, pfield1_type ) );

   pclass_id = get_class_id_for_name( m, pclass_name );

   pcfield1_id = get_field_id_for_name( m, pclass_name, pchild1_name );
   if( pcfield1_id.empty( ) )
      throw runtime_error( "unknown field '" + pchild1_name + "' for class '" + pclass_name + "'" );

   pos = arg_parent_info2.find( ':' );
   if( pos == string::npos )
      throw runtime_error( "unexpected child info missing for '" + arg_parent_info2 + "'" );

   string pfield2_name = arg_parent_info2.substr( 0, pos );
   string pchild2_name = arg_parent_info2.substr( pos + 1 );

   string pfield2_type;
   pfield2_id = get_field_id_for_name( m, arg_class_name, pfield2_name, &pfield2_type, true );

   if( pfield2_id.empty( ) )
      throw runtime_error( "unknown field '" + pfield2_name + "' for class '" + arg_class_name + "'" );

   string pclass2_name( get_class_name_from_field_type( m, arg_class_name, pfield2_name, pfield2_type ) );

   if( pclass2_name != pclass_name )
      throw runtime_error( "unexpected parent classes do not match for 'meta_relationship_child_name' specification" );

   pcfield2_id = get_field_id_for_name( m, pclass_name, pchild2_name );
   if( pcfield2_id.empty( ) )
      throw runtime_error( "unknown field '" + pchild2_name + "' for class '" + pclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );

   details.push_back( specification_detail( pfield1_id, "pfield1", e_model_element_type_field ) );
   details.push_back( specification_detail( pfield2_id, "pfield2", e_model_element_type_field ) );

   details.push_back( specification_detail( pcfield1_id, "pcfield1", e_model_element_type_field ) );
   details.push_back( specification_detail( pcfield2_id, "pcfield2", e_model_element_type_field ) );
}

void meta_relationship_child_name_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_attribute( c_attribute_pclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   sfield_id = reader.read_attribute( c_attribute_sfield_id );

   pfield1_id = reader.read_attribute( c_attribute_pfield1_id );
   pfield2_id = reader.read_attribute( c_attribute_pfield2_id );

   pcfield1_id = reader.read_attribute( c_attribute_pcfield1_id );
   pcfield2_id = reader.read_attribute( c_attribute_pcfield2_id );
}

void meta_relationship_child_name_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_pclass_id, pclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_sfield_id, sfield_id );

   writer.write_attribute( c_attribute_pfield1_id, pfield1_id );
   writer.write_attribute( c_attribute_pfield2_id, pfield2_id );

   writer.write_attribute( c_attribute_pcfield1_id, pcfield1_id );
   writer.write_attribute( c_attribute_pcfield2_id, pcfield2_id );
}

void meta_relationship_child_name_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pclass_name = get_class_name_for_id( m, pclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pclass, pclass_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_field ), field_name ) );

   string sfield_name = get_field_name_for_id( m, class_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_sfield ), sfield_name ) );

   string pfield1_name = get_field_name_for_id( m, class_name, pfield1_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pfield1 ), pfield1_name ) );

   string pfield2_name = get_field_name_for_id( m, class_name, pfield2_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pfield2 ), pfield2_name ) );

   string pcfield1_name = get_field_name_for_id( m, pclass_name, pcfield1_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pcfield1 ), pcfield1_name ) );

   string pcfield2_name = get_field_name_for_id( m, pclass_name, pcfield2_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pcfield2 ), pcfield2_name ) );
}

string meta_relationship_child_name_specification::static_class_name( ) { return "meta_relationship_child_name"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, meta_relationship_child_name_specification, static_class_name );

struct meta_relationship_field_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string fclass_id;
   string mclass_id;
   string gclass_id;

   vector< string > field_info;

   string type_field_id;
   string extra_field_id;
   string extra_sfield_id;
   string internal_field_id;

   string field_id_id;
   string field_key_id;
   string field_field_id;
};

void meta_relationship_field_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 10 )
      throw runtime_error( "unexpected number of args != 10 for 'meta_relationship_field' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_fclass_name( args[ 1 ] );
   string arg_mclass_name( args[ 2 ] );
   string arg_gclass_name( args[ 3 ] );
   string arg_field_info( args[ 4 ] );
   string arg_tfield_name( args[ 5 ] );
   string arg_xfield_name( args[ 6 ] );
   string arg_ifield_name( args[ 7 ] );
   string arg_ffield_info( args[ 8 ] );
   string arg_kfield_name( args[ 9 ] );

   class_id = get_class_id_for_name( m, arg_class_name );
   fclass_id = get_class_id_for_name( m, arg_fclass_name );
   mclass_id = get_class_id_for_name( m, arg_mclass_name );
   gclass_id = get_class_id_for_name( m, arg_gclass_name );

   vector< string > field_ids;
   while( true )
   {
      string::size_type pos = arg_field_info.find( ',' );
      string next_info( arg_field_info.substr( 0, pos ) );

      string::size_type cpos = next_info.find( '-' );
      string cfield_name( next_info.substr( 0, cpos ) );

      string pfield_name( cfield_name );
      if( cpos != string::npos )
         pfield_name = next_info.substr( cpos + 1 );

      string cfield_id = get_field_id_for_name( m, arg_fclass_name, cfield_name, 0, true );
      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cfield_name + "' for class '" + arg_fclass_name + "'" );

      string pfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, 0, true );
      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      field_ids.push_back( cfield_id );
      field_ids.push_back( pfield_id );

      field_info.push_back( cfield_id + "," + pfield_id );

      if( pos == string::npos )
         break;

      arg_field_info.erase( 0, pos + 1 );
   }

   type_field_id = get_field_id_for_name( m, arg_fclass_name, arg_tfield_name, 0, true );
   if( type_field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_tfield_name + "' for class '" + arg_fclass_name + "'" );

   extra_field_id = get_field_id_for_name( m, arg_fclass_name, arg_xfield_name );
   if( extra_field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_xfield_name + "' for class '" + arg_fclass_name + "'" );

   extra_sfield_id = get_field_id_for_name( m, arg_class_name, arg_xfield_name );
   if( extra_sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_xfield_name + "' for class '" + arg_class_name + "'" );

   internal_field_id = get_field_id_for_name( m, arg_fclass_name, arg_ifield_name, 0, true );
   if( internal_field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_ifield_name + "' for class '" + arg_fclass_name + "'" );

   string::size_type pos = arg_ffield_info.find( ':' );
   if( pos == string::npos )
      throw runtime_error( "unexpected format for ffield_info" );

   string field_id_name( arg_ffield_info.substr( 0, pos ) );
   field_id_id = get_field_id_for_name( m, arg_class_name, field_id_name );
   if( field_id_id.empty( ) )
      throw runtime_error( "unknown field '" + field_id_name + "' for class '" + arg_class_name + "'" );

   field_key_id = get_field_id_for_name( m, arg_class_name, arg_kfield_name );
   if( field_key_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_kfield_name + "' for class '" + arg_class_name + "'" );

   string field_field_id_name( arg_ffield_info.substr( pos + 1 ) );
   field_field_id = get_field_id_for_name( m, arg_fclass_name, field_field_id_name );
   if( field_field_id.empty( ) )
      throw runtime_error( "unknown field '" + field_field_id_name + "' for class '" + arg_fclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( fclass_id, "fclass", e_model_element_type_class ) );
   details.push_back( specification_detail( mclass_id, "mclass", e_model_element_type_class ) );
   details.push_back( specification_detail( gclass_id, "gclass", e_model_element_type_class ) );

   for( size_t i = 0; i < field_ids.size( ); i++ )
      details.push_back( specification_detail( field_ids[ i ], "field", e_model_element_type_field ) );

   details.push_back( specification_detail( type_field_id, "tfield", e_model_element_type_field ) );
   details.push_back( specification_detail( extra_field_id, "xfield", e_model_element_type_field ) );
   details.push_back( specification_detail( extra_sfield_id, "xsfield", e_model_element_type_field ) );
   details.push_back( specification_detail( internal_field_id, "ifield", e_model_element_type_field ) );

   details.push_back( specification_detail( field_id_id, "field_id", e_model_element_type_field ) );
   details.push_back( specification_detail( field_key_id, "field_key_id", e_model_element_type_field ) );
   details.push_back( specification_detail( field_field_id, "field_field_id", e_model_element_type_field ) );
}

void meta_relationship_field_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   fclass_id = reader.read_attribute( c_attribute_fclass_id );
   mclass_id = reader.read_attribute( c_attribute_mclass_id );
   gclass_id = reader.read_attribute( c_attribute_gclass_id );

   string next;
   while( reader.has_read_attribute( c_attribute_field_info, next ) )
      field_info.push_back( next );

   type_field_id = reader.read_attribute( c_attribute_type_field_id );
   extra_field_id = reader.read_attribute( c_attribute_extra_field_id );
   extra_sfield_id = reader.read_attribute( c_attribute_extra_sfield_id );
   internal_field_id = reader.read_attribute( c_attribute_internal_field_id );

   field_id_id = reader.read_attribute( c_attribute_field_id_id );
   field_key_id = reader.read_attribute( c_attribute_field_key_id );
   field_field_id = reader.read_attribute( c_attribute_field_field_id );
}

void meta_relationship_field_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_fclass_id, fclass_id );
   writer.write_attribute( c_attribute_mclass_id, mclass_id );
   writer.write_attribute( c_attribute_gclass_id, gclass_id );

   for( size_t i = 0; i < field_info.size( ); i++ )
      writer.write_attribute( c_attribute_field_info, field_info[ i ] );

   writer.write_attribute( c_attribute_type_field_id, type_field_id );
   writer.write_attribute( c_attribute_extra_field_id, extra_field_id );
   writer.write_attribute( c_attribute_extra_sfield_id, extra_sfield_id );
   writer.write_attribute( c_attribute_internal_field_id, internal_field_id );

   writer.write_attribute( c_attribute_field_id_id, field_id_id );
   writer.write_attribute( c_attribute_field_key_id, field_key_id );
   writer.write_attribute( c_attribute_field_field_id, field_field_id );
}

void meta_relationship_field_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string fclass_name = get_class_name_for_id( m, fclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_fclass, fclass_name ) );

   string mclass_name = get_class_name_for_id( m, mclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_mclass, mclass_name ) );

   string gclass_name = get_class_name_for_id( m, gclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_gclass, gclass_name ) );

   string field_list;
   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( !field_list.empty( ) )
         field_list += " ";

      string::size_type pos = field_info[ i ].find( ',' );

      string cfield_name = get_field_name_for_id( m, fclass_name, field_info[ i ].substr( 0, pos ) );
      string pfield_name = get_field_name_for_id( m, class_name, field_info[ i ].substr( pos + 1 ) );

      field_list += cfield_name + "," + pfield_name;
   }

   spec_data.data_pairs.push_back( make_pair( string( c_data_field_list ), field_list ) );

   string type_field_name = get_field_name_for_id( m, fclass_name, type_field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_tfield ), type_field_name ) );

   string extra_field_name = get_field_name_for_id( m, fclass_name, extra_field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_xfield ), extra_field_name ) );

   string extra_sfield_name = get_field_name_for_id( m, class_name, extra_sfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_xsfield ), extra_sfield_name ) );

   string internal_field_name = get_field_name_for_id( m, fclass_name, internal_field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_ifield ), internal_field_name ) );

   string field_id_id_name = get_field_name_for_id( m, class_name, field_id_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_idfield ), field_id_id_name ) );

   string field_key_id_name = get_field_name_for_id( m, class_name, field_key_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_keyfield ), field_key_id_name ) );

   string field_field_id_name = get_field_name_for_id( m, fclass_name, field_field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_fidfield ), field_field_id_name ) );
}

string meta_relationship_field_specification::static_class_name( ) { return "meta_relationship_field"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, meta_relationship_field_specification, static_class_name );

struct meta_spec_field_pairs_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;

   string field_id;
   string ofield_id;

   string cfield_id;
   string sfield_id;

   string test_value;
};

void meta_spec_field_pairs_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 5 )
      throw runtime_error( "unexpected number of args != 5 for 'meta_spec_field_pairs' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_ofield_name( args[ 2 ] );
   string arg_child_info( args[ 3 ] );
   string arg_sfield_info( args[ 4 ] );

   string::size_type pos = arg_child_info.find( ':' );
   string cclass_name( arg_child_info.substr( 0, pos ) );

   string cfield_name;
   if( pos != string::npos )
      cfield_name = arg_child_info.substr( pos + 1 );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   ofield_id = get_field_id_for_name( m, arg_class_name, arg_ofield_name );
   if( ofield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_ofield_name + "' for class '" + arg_class_name + "'" );

   cclass_id = get_class_id_for_name( m, cclass_name );

   if( !cfield_name.empty( ) )
   {
      string cfield_type;
      cfield_id = get_field_id_for_name( m, cclass_name, cfield_name, &cfield_type, true );

      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cfield_name + "' for class '" + cclass_name + "'" );

      string cfield_class_name( get_class_name_from_field_type( m, cclass_name, cfield_name, cfield_type ) );

      if( cfield_class_name != arg_class_name )
         throw runtime_error( "child field '" + cfield_name + "' is not class '" + arg_class_name + "'" );
   }

   pos = arg_sfield_info.find( '=' );
   if( pos == string::npos )
      throw runtime_error( "unexpected missing '=' is sfield_info" );

   string sfield_name( arg_sfield_info.substr( 0, pos ) );
   test_value = arg_sfield_info.substr( pos + 1 );

   sfield_id = get_field_id_for_name( m, cclass_name, sfield_name, 0, true );

   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + sfield_name + "' for class '" + cclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( cclass_id != class_id )
      details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );

   if( !cfield_id.empty( ) )
      details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );

   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );
}

void meta_spec_field_pairs_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   ofield_id = reader.read_attribute( c_attribute_ofield_id );
   sfield_id = reader.read_attribute( c_attribute_sfield_id );
   test_value = reader.read_attribute( c_attribute_test_value );

   cfield_id = reader.read_opt_attribute( c_attribute_cfield_id );
}

void meta_spec_field_pairs_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_ofield_id, ofield_id );
   writer.write_attribute( c_attribute_sfield_id, sfield_id );
   writer.write_attribute( c_attribute_test_value, test_value );

   writer.write_opt_attribute( c_attribute_cfield_id, cfield_id );
}

void meta_spec_field_pairs_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string ofield_name = get_field_name_for_id( m, class_name, ofield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_ofield, ofield_name ) );

   string cclass_name = get_class_name_for_id( m, cclass_id );

   string sfield_name = get_field_name_for_id( m, cclass_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   string cfield_name;
   if( !cfield_id.empty( ) )
   {
      cfield_name = get_field_name_for_id( m, cclass_name, cfield_id, 0, false, true );
      if( cclass_id != class_id )
         cfield_name += "_{" + cclass_name + "}";
      cfield_name = m.determine_child_rel_suffix( cfield_name );
   }

   // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_child, cfield_name.empty( ) ? cclass_name : cfield_name ) );
}

string meta_spec_field_pairs_specification::static_class_name( ) { return "meta_spec_field_pairs"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, meta_spec_field_pairs_specification, static_class_name );

struct meta_spec_field_type_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string mclass_id;
   string fclass_id;
   string paclass_id;
   string mcrclass_id;

   string field_id;
   string cfield_id;
   string mfield_id;
   string prfield_id;

   string tfield_id;
   string tvfield_id;
   string mttfield_id;

   string tpfield_id;
   string tcfield_id;

   string sfield_id;
   string mtsfield_id;

   string mpfield_id;
   string mcrfield_id;
   string mcrcfield_id;

   string crmfield_id;
   string crpmfield_id;
   string crgpmfield_id;

   string spfield_id;
   string scfield_id;
   string sftfield_id;

   string efield_id;
   string pfield_id;

   string pafield_id;
   string patfield_id;
   string pacfield_id;

   string procedure_id;

   string user_info_key;

   string password_enum_name;
   string password_enum_name_2;
   string password_enum_value;
   string password_enum_value_2;

   string user_group_enum_name;
   string user_group_enum_value;
};

void meta_spec_field_type_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 27 )
      throw runtime_error( "unexpected number of args != 27 for 'meta_spec_field_type' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_mfield_name( args[ 1 ] );
   string arg_tfield_name( args[ 2 ] );
   string arg_tpfield_name( args[ 3 ] );
   string arg_tcfield_name( args[ 4 ] );
   string arg_tvfield_name( args[ 5 ] );
   string arg_mttfield_name( args[ 6 ] );
   string arg_sfield_name( args[ 7 ] );
   string arg_spfield_name( args[ 8 ] );
   string arg_scfield_name( args[ 9 ] );
   string arg_mtsfield_name( args[ 10 ] );
   string arg_mpfield_name( args[ 11 ] );
   string arg_mcfield_info( args[ 12 ] );
   string arg_crmatch_name( args[ 13 ] );
   string arg_crpmatch_name( args[ 14 ] );
   string arg_crgpmatch_name( args[ 15 ] );
   string arg_meta_proc_name( args[ 16 ] );
   string arg_cfield_name( args[ 17 ] );
   string arg_field_info( args[ 18 ] );
   string arg_type_primitive( args[ 19 ] );
   string arg_sftfield_name( args[ 20 ] );
   string arg_proc_arg_field_name( args[ 21 ] );
   string arg_proc_arg_type_field_name( args[ 22 ] );
   string arg_proc_arg_class_field_name( args[ 23 ] );
   string arg_efield_name( args[ 24 ] );
   string arg_user_info_key( args[ 25 ] );
   string arg_field_extra_enum_info( args[ 26 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   string mfield_type;
   mfield_id = get_field_id_for_name( m, arg_class_name, arg_mfield_name, &mfield_type, true );

   if( mfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_mfield_name + "' for class '" + arg_class_name + "'" );

   string mfield_class_name( get_class_name_from_field_type( m, arg_class_name, arg_mfield_name, mfield_type ) );

   mclass_id = get_class_id_for_name( m, mfield_class_name );

   string tfield_type;
   tfield_id = get_field_id_for_name( m, arg_class_name, arg_tfield_name, &tfield_type, true );
   if( tfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_tfield_name + "' for class '" + arg_class_name + "'" );

   tvfield_id = get_field_id_for_name( m, arg_class_name, arg_tvfield_name );
   if( tvfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_tvfield_name + "' for class '" + arg_class_name + "'" );

   tpfield_id = get_field_id_for_name( m, arg_class_name, arg_tpfield_name, 0, true );
   if( tpfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_tpfield_name + "' for class '" + arg_class_name + "'" );

   tcfield_id = get_field_id_for_name( m, arg_class_name, arg_tcfield_name, 0, true );
   if( tcfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_tcfield_name + "' for class '" + arg_class_name + "'" );

   mttfield_id = get_field_id_for_name( m, mfield_class_name, arg_mttfield_name );
   if( mttfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_mttfield_name + "' for class '" + mfield_class_name + "'" );

   string sfield_type;
   sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_name, &sfield_type, true );
   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + arg_class_name + "'" );

   spfield_id = get_field_id_for_name( m, arg_class_name, arg_spfield_name, 0, true );
   if( spfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_spfield_name + "' for class '" + arg_class_name + "'" );

   mtsfield_id = get_field_id_for_name( m, mfield_class_name, arg_mtsfield_name );
   if( mtsfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_mtsfield_name + "' for class '" + mfield_class_name + "'" );

   mpfield_id = get_field_id_for_name( m, arg_class_name, arg_mpfield_name, 0, true );
   if( mpfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_mpfield_name + "' for class '" + arg_class_name + "'" );

   string::size_type pos = arg_mcfield_info.find( ':' );
   if( pos == string::npos )
      throw runtime_error( "missing child class field for child_relationship info in 'meta_spec_field_type' specification" );

   string child_rel_name( arg_mcfield_info.substr( 0, pos ) );
   string child_rel_child_name( arg_mcfield_info.substr( pos + 1 ) );

   string mcrfield_type;
   mcrfield_id = get_field_id_for_name( m, arg_class_name, child_rel_name, &mcrfield_type, true );
   if( mcrfield_id.empty( ) )
      throw runtime_error( "unknown field '" + child_rel_name + "' for class '" + arg_class_name + "'" );

   string mcrfield_class_name( get_class_name_from_field_type( m, arg_class_name, child_rel_name, mcrfield_type ) );

   mcrclass_id = get_class_id_for_name( m, mcrfield_class_name );

   mcrcfield_id = get_field_id_for_name( m, mcrfield_class_name, child_rel_child_name, 0, true );

   crmfield_id = get_field_id_for_name( m, mfield_class_name, arg_crmatch_name );
   if( crmfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_crmatch_name + "' for class '" + mfield_class_name + "'" );

   crpmfield_id = get_field_id_for_name( m, mfield_class_name, arg_crpmatch_name );
   if( crpmfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_crpmatch_name + "' for class '" + mfield_class_name + "'" );

   crgpmfield_id = get_field_id_for_name( m, mfield_class_name, arg_crgpmatch_name );
   if( crgpmfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_crgpmatch_name + "' for class '" + mfield_class_name + "'" );

   procedure_id = get_procedure_id_for_name( m, mfield_class_name, arg_meta_proc_name );
   if( procedure_id.empty( ) )
      throw runtime_error( "unknown procedure '" + arg_meta_proc_name + "' for class '" + mfield_class_name + "'" );

   cfield_id = get_field_id_for_name( m, arg_class_name, arg_cfield_name, 0, true );
   if( cfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_cfield_name + "' for class '" + arg_class_name + "'" );

   pos = arg_field_info.find( ':' );
   if( pos == string::npos )
      throw runtime_error( "missing parent class field for field info in 'meta_spec_field_type' specification" );

   string field_name( arg_field_info.substr( 0, pos ) );
   string pfield_name( arg_field_info.substr( pos + 1 ) );

   string field_type;
   field_id = get_field_id_for_name( m, arg_class_name, field_name, &field_type, true );

   if( field_type != tfield_type )
      throw runtime_error( "field and tfield types must match for 'meta_spec_field_type' specification" );

   if( field_type != sfield_type )
      throw runtime_error( "field and sfield types must match for 'meta_spec_field_type' specification" );

   string field_class_name( get_class_name_from_field_type( m, arg_class_name, field_name, field_type ) );

   fclass_id = get_class_id_for_name( m, field_class_name );

   efield_id = get_field_id_for_name( m, field_class_name, arg_efield_name );
   if( efield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_efield_name + "' for class '" + field_class_name + "'" );

   pfield_id = get_field_id_for_name( m, field_class_name, pfield_name, 0, true );
   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + pfield_name + "' for class '" + field_class_name + "'" );

   prfield_id = get_field_id_for_name( m, field_class_name, arg_type_primitive, 0, true );
   if( prfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_type_primitive + "' for class '" + field_class_name + "'" );

   scfield_id = get_field_id_for_name( m, arg_class_name, arg_scfield_name );
   if( scfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_scfield_name + "' for class '" + arg_class_name + "'" );

   sftfield_id = get_field_id_for_name( m, mfield_class_name, arg_sftfield_name );
   if( sftfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_sftfield_name + "' for class '" + mfield_class_name + "'" );

   string proc_arg_field_type;
   pafield_id = get_field_id_for_name( m, arg_class_name, arg_proc_arg_field_name, &proc_arg_field_type, true );

   if( pafield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_proc_arg_field_name + "' for class '" + arg_class_name + "'" );

   string proc_arg_class_name( get_class_name_from_field_type( m, arg_class_name, arg_proc_arg_field_name, proc_arg_field_type ) );

   paclass_id = get_class_id_for_name( m, proc_arg_class_name );

   patfield_id = get_field_id_for_name( m, proc_arg_class_name, arg_proc_arg_type_field_name );
   if( patfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_proc_arg_type_field_name + "' for class '" + proc_arg_class_name + "'" );

   pacfield_id = get_field_id_for_name( m, proc_arg_class_name, arg_proc_arg_class_field_name );
   if( pacfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_proc_arg_class_field_name + "' for class '" + proc_arg_class_name + "'" );

   user_info_key = arg_user_info_key;

   pos = arg_field_extra_enum_info.find( ':' );
   if( pos == string::npos )
      throw runtime_error( "unexpected format for arg_field_extra_enum_info in 'meta_spec_field_type' specification" );

   string field_extra_enum_name( arg_field_extra_enum_info.substr( 0, pos ) );
   arg_field_extra_enum_info.erase( 0, pos + 1 );

   string field_extra_enum_id = get_id_for_enum_name( m, field_extra_enum_name );
   if( field_extra_enum_id.empty( ) )
      throw runtime_error( "unknown enum '" + field_extra_enum_name + "'" );

   pos = arg_field_extra_enum_info.find( ',' );
   if( pos == string::npos )
      throw runtime_error( "unexpected format for arg_field_extra_enum_info in 'meta_spec_field_type' specification" );

   password_enum_name = arg_field_extra_enum_info.substr( 0, pos );
   user_group_enum_name = arg_field_extra_enum_info.substr( pos + 1 );

   pos = password_enum_name.find( '+' );
   if( pos != string::npos )
   {
      password_enum_name_2 = password_enum_name.substr( pos + 1 );
      password_enum_value_2 = get_key_for_enum_value( m, field_extra_enum_id, password_enum_name_2 );

      password_enum_name.erase( pos );
   }

   password_enum_value = get_key_for_enum_value( m, field_extra_enum_id, password_enum_name );
   user_group_enum_value = get_key_for_enum_value( m, field_extra_enum_id, user_group_enum_name );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( mclass_id, "mclass", e_model_element_type_class ) );
   details.push_back( specification_detail( fclass_id, "fclass", e_model_element_type_class ) );
   details.push_back( specification_detail( paclass_id, "paclass", e_model_element_type_class ) );
   details.push_back( specification_detail( mcrclass_id, "mcrclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );
   details.push_back( specification_detail( efield_id, "efield", e_model_element_type_field ) );
   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( prfield_id, "prfield", e_model_element_type_field ) );

   details.push_back( specification_detail( mpfield_id, "mpfield", e_model_element_type_field ) );
   details.push_back( specification_detail( mcrfield_id, "mcrfield", e_model_element_type_field ) );
   details.push_back( specification_detail( mcrcfield_id, "mcrcfield", e_model_element_type_field ) );

   details.push_back( specification_detail( crmfield_id, "crmfield", e_model_element_type_field ) );
   details.push_back( specification_detail( crpmfield_id, "crpmfield", e_model_element_type_field ) );
   details.push_back( specification_detail( crgpmfield_id, "crgpmfield", e_model_element_type_field ) );

   details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );
   details.push_back( specification_detail( tvfield_id, "tvfield", e_model_element_type_field ) );
   details.push_back( specification_detail( mttfield_id, "mttfield_id", e_model_element_type_field ) );

   details.push_back( specification_detail( tpfield_id, "tpfield", e_model_element_type_field ) );
   details.push_back( specification_detail( tcfield_id, "tcfield", e_model_element_type_field ) );

   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );
   details.push_back( specification_detail( mtsfield_id, "mtsfield", e_model_element_type_field ) );

   details.push_back( specification_detail( spfield_id, "spfield", e_model_element_type_field ) );
   details.push_back( specification_detail( scfield_id, "scfield", e_model_element_type_field ) );
   details.push_back( specification_detail( sftfield_id, "sftfield", e_model_element_type_field ) );

   details.push_back( specification_detail( pafield_id, "pafield", e_model_element_type_field ) );
   details.push_back( specification_detail( patfield_id, "patfield", e_model_element_type_field ) );
   details.push_back( specification_detail( pacfield_id, "pacfield", e_model_element_type_field ) );

   details.push_back( specification_detail( procedure_id, "procedure", e_model_element_type_procedure ) );
}

void meta_spec_field_type_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   mclass_id = reader.read_attribute( c_attribute_mclass_id );
   fclass_id = reader.read_attribute( c_attribute_fclass_id );
   paclass_id = reader.read_attribute( c_attribute_paclass_id );
   mcrclass_id = reader.read_attribute( c_attribute_mcrclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   mfield_id = reader.read_attribute( c_attribute_mfield_id );
   cfield_id = reader.read_attribute( c_attribute_cfield_id );
   efield_id = reader.read_attribute( c_attribute_efield_id );
   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   prfield_id = reader.read_attribute( c_attribute_prfield_id );

   mpfield_id = reader.read_attribute( c_attribute_mpfield_id );
   mcrfield_id = reader.read_attribute( c_attribute_mcrfield_id );
   mcrcfield_id = reader.read_attribute( c_attribute_mcrcfield_id );

   crmfield_id = reader.read_attribute( c_attribute_crmfield_id );
   crpmfield_id = reader.read_attribute( c_attribute_crpmfield_id );
   crgpmfield_id = reader.read_attribute( c_attribute_crgpmfield_id );

   tfield_id = reader.read_attribute( c_attribute_tfield_id );
   tvfield_id = reader.read_attribute( c_attribute_tvfield_id );
   mttfield_id = reader.read_attribute( c_attribute_mttfield_id );

   tpfield_id = reader.read_attribute( c_attribute_tpfield_id );
   tcfield_id = reader.read_attribute( c_attribute_tcfield_id );

   sfield_id = reader.read_attribute( c_attribute_sfield_id );
   mtsfield_id = reader.read_attribute( c_attribute_mtsfield_id );

   spfield_id = reader.read_attribute( c_attribute_spfield_id );
   scfield_id = reader.read_attribute( c_attribute_scfield_id );
   sftfield_id = reader.read_attribute( c_attribute_sftfield_id );

   procedure_id = reader.read_attribute( c_attribute_procedure_id );

   pafield_id = reader.read_attribute( c_attribute_pafield_id );
   patfield_id = reader.read_attribute( c_attribute_patfield_id );
   pacfield_id = reader.read_attribute( c_attribute_pacfield_id );

   user_info_key = reader.read_attribute( c_attribute_user_info_key );

   password_enum_name = reader.read_attribute( c_attribute_pw_enum_name );
   password_enum_name_2 = reader.read_opt_attribute( c_attribute_pw_enum_2_name );
   password_enum_value = reader.read_attribute( c_attribute_pw_enum_value );
   password_enum_value_2 = reader.read_opt_attribute( c_attribute_pw_enum_2_value );

   user_group_enum_name = reader.read_attribute( c_attribute_ug_enum_name );
   user_group_enum_value = reader.read_attribute( c_attribute_ug_enum_value );
}

void meta_spec_field_type_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_mclass_id, mclass_id );
   writer.write_attribute( c_attribute_fclass_id, fclass_id );
   writer.write_attribute( c_attribute_paclass_id, paclass_id );
   writer.write_attribute( c_attribute_mcrclass_id, mcrclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_mfield_id, mfield_id );
   writer.write_attribute( c_attribute_cfield_id, cfield_id );
   writer.write_attribute( c_attribute_efield_id, efield_id );
   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_prfield_id, prfield_id );

   writer.write_attribute( c_attribute_mpfield_id, mpfield_id );
   writer.write_attribute( c_attribute_mcrfield_id, mcrfield_id );
   writer.write_attribute( c_attribute_mcrcfield_id, mcrcfield_id );

   writer.write_attribute( c_attribute_crmfield_id, crmfield_id );
   writer.write_attribute( c_attribute_crpmfield_id, crpmfield_id );
   writer.write_attribute( c_attribute_crgpmfield_id, crgpmfield_id );

   writer.write_attribute( c_attribute_tfield_id, tfield_id );
   writer.write_attribute( c_attribute_tvfield_id, tvfield_id );
   writer.write_attribute( c_attribute_mttfield_id, mttfield_id );

   writer.write_attribute( c_attribute_tpfield_id, tpfield_id );
   writer.write_attribute( c_attribute_tcfield_id, tcfield_id );

   writer.write_attribute( c_attribute_sfield_id, sfield_id );
   writer.write_attribute( c_attribute_mtsfield_id, mtsfield_id );

   writer.write_attribute( c_attribute_spfield_id, spfield_id );
   writer.write_attribute( c_attribute_scfield_id, scfield_id );
   writer.write_attribute( c_attribute_sftfield_id, sftfield_id );

   writer.write_attribute( c_attribute_procedure_id, procedure_id );

   writer.write_attribute( c_attribute_pafield_id, pafield_id );
   writer.write_attribute( c_attribute_patfield_id, patfield_id );
   writer.write_attribute( c_attribute_pacfield_id, pacfield_id );

   writer.write_attribute( c_attribute_user_info_key, user_info_key );

   writer.write_attribute( c_attribute_pw_enum_name, password_enum_name );
   writer.write_opt_attribute( c_attribute_pw_enum_2_name, password_enum_name_2 );
   writer.write_attribute( c_attribute_pw_enum_value, password_enum_value );
   writer.write_opt_attribute( c_attribute_pw_enum_2_value, password_enum_value_2 );

   writer.write_attribute( c_attribute_ug_enum_name, user_group_enum_name );
   writer.write_attribute( c_attribute_ug_enum_value, user_group_enum_value );
}

void meta_spec_field_type_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string mclass_name = get_class_name_for_id( m, mclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_mclass, mclass_name ) );

   string fclass_name = get_class_name_for_id( m, fclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_fclass, fclass_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_field ), field_name ) );

   string mfield_name = get_field_name_for_id( m, class_name, mfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_mfield ), mfield_name ) );

   string cfield_name = get_field_name_for_id( m, class_name, cfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_cfield ), cfield_name ) );

   string efield_name = get_field_name_for_id( m, fclass_name, efield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_efield ), efield_name ) );

   string pfield_name = get_field_name_for_id( m, fclass_name, pfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pfield ), pfield_name ) );

   string prfield_name = get_field_name_for_id( m, fclass_name, prfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_prfield ), prfield_name ) );

   string tfield_name = get_field_name_for_id( m, class_name, tfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_tfield ), tfield_name ) );

   string tvfield_name = get_field_name_for_id( m, class_name, tvfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_tvfield ), tvfield_name ) );

   string tpfield_name = get_field_name_for_id( m, class_name, tpfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_tpfield ), tpfield_name ) );

   string tcfield_name = get_field_name_for_id( m, class_name, tcfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_tcfield ), tcfield_name ) );

   string mttfield_name = get_field_name_for_id( m, mclass_name, mttfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_mttfield ), mttfield_name ) );

   string sfield_name = get_field_name_for_id( m, class_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_sfield ), sfield_name ) );

   string mpfield_name = get_field_name_for_id( m, class_name, mpfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_mpfield ), mpfield_name ) );

   string mcrfield_name = get_field_name_for_id( m, class_name, mcrfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_mcrfield ), mcrfield_name ) );

   string crmfield_name = get_field_name_for_id( m, mclass_name, crmfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_crmfield ), crmfield_name ) );

   string crpmfield_name = get_field_name_for_id( m, mclass_name, crpmfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_crpmfield ), crpmfield_name ) );

   string crgpmfield_name = get_field_name_for_id( m, mclass_name, crgpmfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_crgpmfield ), crgpmfield_name ) );

   string mtsfield_name = get_field_name_for_id( m, mclass_name, mtsfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_mtsfield ), mtsfield_name ) );

   string spfield_name = get_field_name_for_id( m, class_name, spfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_spfield ), spfield_name ) );

   string scfield_name = get_field_name_for_id( m, class_name, scfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_scfield ), scfield_name ) );

   string sftfield_name = get_field_name_for_id( m, mclass_name, sftfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_sftfield ), sftfield_name ) );

   string procedure_name = get_procedure_name_for_id( m, mclass_name, procedure_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_procedure ), procedure_name ) );

   string pafield_name = get_field_name_for_id( m, class_name, pafield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pafield ), pafield_name ) );

   string paclass_name = get_class_name_for_id( m, paclass_id );

   string patfield_name = get_field_name_for_id( m, paclass_name, patfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_patfield ), patfield_name ) );

   string pacfield_name = get_field_name_for_id( m, paclass_name, pacfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pacfield ), pacfield_name ) );

   string mcrclass_name = get_class_name_for_id( m, mcrclass_id );

   string mcrcfield_name = get_field_name_for_id( m, mcrclass_name, mcrcfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_mcrcfield ), mcrcfield_name ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_uikey ), user_info_key ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_pwname ), unescaped( password_enum_name ) ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pwname2 ), unescaped( password_enum_name_2 ) ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pwvalue ), password_enum_value ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_pwvalue2 ), password_enum_value_2 ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_ugname ), unescaped( user_group_enum_name ) ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_ugvalue ), user_group_enum_value ) );
}

string meta_spec_field_type_specification::static_class_name( ) { return "meta_spec_field_type"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, meta_spec_field_type_specification, static_class_name );

struct meta_spec_field_values_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;

   string field_id;
   string rfield_id;
   string ofield_id;

   string cfield_id;
   string sfield_id;
   string fkfield_id;

   string test_value;
};

void meta_spec_field_values_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 7 )
      throw runtime_error( "unexpected number of args != 7 for 'meta_spec_field_values' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_rfield_name( args[ 2 ] );
   string arg_ofield_name( args[ 3 ] );
   string arg_child_info( args[ 4 ] );
   string arg_fk_info( args[ 5 ] );
   string arg_sfield_name( args[ 6 ] );

   string::size_type pos = arg_child_info.find( ':' );
   string cclass_name( arg_child_info.substr( 0, pos ) );

   string cfield_name;
   if( pos != string::npos )
      cfield_name = arg_child_info.substr( pos + 1 );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   rfield_id = get_field_id_for_name( m, arg_class_name, arg_rfield_name );
   if( rfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_rfield_name + "' for class '" + arg_class_name + "'" );

   ofield_id = get_field_id_for_name( m, arg_class_name, arg_ofield_name );
   if( ofield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_ofield_name + "' for class '" + arg_class_name + "'" );

   cclass_id = get_class_id_for_name( m, cclass_name );

   if( !cfield_name.empty( ) )
   {
      string cfield_type;
      cfield_id = get_field_id_for_name( m, cclass_name, cfield_name, &cfield_type, true );

      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cfield_name + "' for class '" + cclass_name + "'" );

      string cfield_class_name( get_class_name_from_field_type( m, cclass_name, cfield_name, cfield_type ) );

      if( cfield_class_name != arg_class_name )
         throw runtime_error( "child field '" + cfield_name + "' is not class '" + arg_class_name + "'" );
   }

   pos = arg_fk_info.find( '=' );
   if( pos == string::npos )
      throw runtime_error( "unexpected missing '=' is fk_info" );

   string fk_field_name( arg_fk_info.substr( 0, pos ) );
   test_value = arg_fk_info.substr( pos + 1 );

   sfield_id = get_field_id_for_name( m, cclass_name, arg_sfield_name );

   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + cclass_name + "'" );

   fkfield_id = get_field_id_for_name( m, cclass_name, fk_field_name, 0, true );

   if( fkfield_id.empty( ) )
      throw runtime_error( "unknown field '" + fk_field_name + "' for class '" + cclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( cclass_id != class_id )
      details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( rfield_id, "rfield", e_model_element_type_field ) );
   details.push_back( specification_detail( ofield_id, "ofield", e_model_element_type_field ) );

   if( !cfield_id.empty( ) )
      details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );

   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );
   details.push_back( specification_detail( fkfield_id, "fkfield", e_model_element_type_field ) );
}

void meta_spec_field_values_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   rfield_id = reader.read_attribute( c_attribute_rfield_id );
   ofield_id = reader.read_attribute( c_attribute_ofield_id );

   sfield_id = reader.read_attribute( c_attribute_sfield_id );
   fkfield_id = reader.read_attribute( c_attribute_fkfield_id );
   test_value = reader.read_attribute( c_attribute_test_value );

   cfield_id = reader.read_opt_attribute( c_attribute_cfield_id );
}

void meta_spec_field_values_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_rfield_id, rfield_id );
   writer.write_attribute( c_attribute_ofield_id, ofield_id );

   writer.write_attribute( c_attribute_sfield_id, sfield_id );
   writer.write_attribute( c_attribute_fkfield_id, fkfield_id );
   writer.write_attribute( c_attribute_test_value, test_value );

   writer.write_opt_attribute( c_attribute_cfield_id, cfield_id );
}

void meta_spec_field_values_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   string rfield_name = get_field_name_for_id( m, class_name, rfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_rfield, rfield_name ) );

   string ofield_name = get_field_name_for_id( m, class_name, ofield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_ofield, ofield_name ) );

   string cclass_name = get_class_name_for_id( m, cclass_id );

   string sfield_name = get_field_name_for_id( m, cclass_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   string fkfield_name = get_field_name_for_id( m, cclass_name, fkfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_fkfield, fkfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   string cfield_name;
   if( !cfield_id.empty( ) )
   {
      cfield_name = get_field_name_for_id( m, cclass_name, cfield_id, 0, false, true );
      if( cclass_id != class_id )
         cfield_name += "_{" + cclass_name + "}";
      cfield_name = m.determine_child_rel_suffix( cfield_name );
   }

   // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_child, cfield_name.empty( ) ? cclass_name : cfield_name ) );
}

string meta_spec_field_values_specification::static_class_name( ) { return "meta_spec_field_values"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, meta_spec_field_values_specification, static_class_name );

struct meta_trait_val_proc_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   static string static_class_name( );
};

void meta_trait_val_proc_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 10 )
      throw runtime_error( "unexpected number of args != 10 for 'meta_trait_val_proc' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_procedure_name( args[ 2 ] );
   string arg_class_arg_name( args[ 3 ] );
   string arg_parent_arg_name( args[ 4 ] );
   string arg_primitive_arg_name( args[ 5 ] );
   string arg_error_arg_name( args[ 6 ] );
   string arg_test_value_arg_name( args[ 7 ] );
   string arg_proc_arg_type_arg_name( args[ 8 ] );
   string arg_proc_arg_class_arg_name( args[ 9 ] );

   string class_id = get_class_id_for_name( m, arg_class_name );

   string field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   string procedure_id = get_procedure_id_for_name( m, arg_class_name, arg_procedure_name );
   if( procedure_id.empty( ) )
      throw runtime_error( "unknown procedure '" + arg_procedure_name + "' for class '" + arg_class_name + "'" );

   string class_arg_id = get_procedure_arg_id_for_name( m, arg_class_name, arg_procedure_name, arg_class_arg_name );
   string parent_arg_id = get_procedure_arg_id_for_name( m, arg_class_name, arg_procedure_name, arg_parent_arg_name );
   string primitive_arg_id = get_procedure_arg_id_for_name( m, arg_class_name, arg_procedure_name, arg_primitive_arg_name );
   string error_arg_id = get_procedure_arg_id_for_name( m, arg_class_name, arg_procedure_name, arg_error_arg_name );
   string test_value_arg_id = get_procedure_arg_id_for_name( m, arg_class_name, arg_procedure_name, arg_test_value_arg_name );
   string proc_arg_type_arg_id = get_procedure_arg_id_for_name( m, arg_class_name, arg_procedure_name, arg_proc_arg_type_arg_name );
   string proc_arg_class_arg_id = get_procedure_arg_id_for_name( m, arg_class_name, arg_procedure_name, arg_proc_arg_class_arg_name );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( procedure_id, "procedure", e_model_element_type_procedure ) );
   details.push_back( specification_detail( class_arg_id, "class_arg", e_model_element_type_procedure_arg ) );
   details.push_back( specification_detail( parent_arg_id, "parent_arg", e_model_element_type_procedure_arg ) );
   details.push_back( specification_detail( primitive_arg_id, "primitive_arg", e_model_element_type_procedure_arg ) );
   details.push_back( specification_detail( error_arg_id, "error_arg", e_model_element_type_procedure_arg ) );
   details.push_back( specification_detail( test_value_arg_id, "test_value_arg", e_model_element_type_procedure_arg ) );
   details.push_back( specification_detail( proc_arg_type_arg_id, "proc_arg_type_arg", e_model_element_type_procedure_arg ) );
   details.push_back( specification_detail( proc_arg_class_arg_id, "proc_arg_class_arg", e_model_element_type_procedure_arg ) );
}

string meta_trait_val_proc_specification::static_class_name( ) { return "meta_trait_val_proc"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, meta_trait_val_proc_specification, static_class_name );

struct modifier_date_notification_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string modifier_id;
   string field_id;
   string period;
   string type;
   bool is_annual;
};

void modifier_date_notification_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'modifier_date_notification' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_modifier_name( args[ 1 ] );
   string arg_field_name( args[ 2 ] );
   string arg_period( args[ 3 ] );

   string::size_type pos = arg_period.find( 'm' );
   if( pos != string::npos )
      type = "months";
   else
   {
      type = "days";
      pos = arg_period.find( 'd' );
   }

   if( pos != string::npos )
      arg_period.erase( pos );

   is_annual = false;
   for( size_t arg = 4; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_annual ) )
         is_annual = true;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'modifier_date_notification' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   modifier_id = get_modifier_id_for_name( m, arg_class_name, arg_modifier_name );
   if( modifier_id.empty( ) )
      throw runtime_error( "unknown modifier '" + arg_modifier_name + "' for class '" + arg_class_name + "'" );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, 0, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   period = arg_period;

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

void modifier_date_notification_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   modifier_id = reader.read_attribute( c_attribute_modifier_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   period = reader.read_attribute( c_attribute_period );
   type = reader.read_attribute( c_attribute_type );

   is_annual = ( reader.read_opt_attribute( c_attribute_is_annual ) == c_true );
}

void modifier_date_notification_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_modifier_id, modifier_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_period, period );
   writer.write_attribute( c_attribute_type, type );

   writer.write_opt_attribute( c_attribute_is_annual, is_annual ? c_true : "" );
}

void modifier_date_notification_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string modifier_name = get_modifier_name_for_id( m, class_name, modifier_id );
   spec_data.data_pairs.push_back( make_pair( c_data_modifier, modifier_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_period, period ) );
   spec_data.data_pairs.push_back( make_pair( c_data_type, type ) );

   spec_data.data_pairs.push_back( make_pair( c_data_annual, is_annual ? c_true : "" ) );
}

string modifier_date_notification_specification::static_class_name( ) { return "modifier_date_notification"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, modifier_date_notification_specification, static_class_name );

struct modifier_field_null_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;

   string modifier_id;

   string field_id;
   string pfield_id;

   bool is_negative;
   bool not_default;
};

void modifier_field_null_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'modifier_field_null' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_modifier_name( args[ 1 ] );
   string arg_field_info( args[ 2 ] );

   is_negative = false;
   not_default = false;
   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_negative ) )
         is_negative = true;
      else if( next_arg == string( c_arg_not_default ) )
         not_default = true;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'modifier_field_null' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   modifier_id = get_modifier_id_for_name( m, arg_class_name, arg_modifier_name );
   if( modifier_id.empty( ) )
      throw runtime_error( "unknown modifier '" + arg_modifier_name + "' for class '" + arg_class_name + "'" );

   string field_name, pfield_name;
   string::size_type pos = arg_field_info.find( ':' );
   if( pos == string::npos )
      field_name = arg_field_info;
   else
   {
      field_name = arg_field_info.substr( 0, pos );
      pfield_name = arg_field_info.substr( pos + 1 );
   }

   string field_type;
   field_id = get_field_id_for_name( m, arg_class_name, field_name, &field_type, true, false, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + field_name + "' for class '" + arg_class_name + "'" );

   if( !pfield_name.empty( ) )
   {
      string pclass_name( get_class_name_from_field_type( m, arg_class_name, field_name, field_type ) );

      string pfield_type;
      pclass_id = get_class_id_for_name( m, pclass_name );
      pfield_id = get_field_id_for_name( m, pclass_name, pfield_name, &pfield_type );

      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + pclass_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   if( !pclass_id.empty( ) )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );

   if( !pfield_id.empty( ) )
      details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
}

void modifier_field_null_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_opt_attribute( c_attribute_pclass_id );
   modifier_id = reader.read_attribute( c_attribute_modifier_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   pfield_id = reader.read_opt_attribute( c_attribute_pfield_id );
   is_negative = ( reader.read_opt_attribute( c_attribute_is_negative ) == c_true );
   not_default = ( reader.read_opt_attribute( c_attribute_not_default ) == c_true );
}

void modifier_field_null_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_attribute( c_attribute_modifier_id, modifier_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_opt_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_opt_attribute( c_attribute_is_negative, is_negative ? c_true : "" );
   writer.write_opt_attribute( c_attribute_not_default, not_default ? c_true : "" );
}

void modifier_field_null_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string modifier_name = get_modifier_name_for_id( m, class_name, modifier_id );
   spec_data.data_pairs.push_back( make_pair( c_data_modifier, modifier_name ) );

   string scclass_name;

   if( !pclass_id.empty( ) )
   {
      string field_name = get_field_name_for_id( m, class_name, field_id, 0, true );
      spec_data.data_pairs.push_back( make_pair( c_data_pfield, field_name ) );

      string pclass_name = get_class_name_for_id( m, pclass_id );

      string field_type;
      string pfield_name = get_field_name_for_id( m, pclass_name, pfield_id, &field_type );

      spec_data.data_pairs.push_back( make_pair( c_data_field, pfield_name ) );

      scclass_name = get_class_name_from_field_type( m, pclass_name, pfield_name, field_type );
   }
   else
   {
      spec_data.data_pairs.push_back( make_pair( c_data_pfield, "" ) );

      string field_name = get_field_name_for_id( m, class_name, field_id, 0, true );
      spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );
   }

   spec_data.data_pairs.push_back( make_pair( string( c_data_is_neg ), is_negative ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_not_dflt ), not_default ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_scclass ), scclass_name ) );

   spec_data.data_pairs.push_back( make_pair( string( c_data_tfield ), "" ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_tpfield ), "" ) );
   spec_data.data_pairs.push_back( make_pair( string( c_data_tvalue ), "" ) );
}

string modifier_field_null_specification::static_class_name( ) { return "modifier_field_null"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, modifier_field_null_specification, static_class_name );

struct modifier_field_required_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string modifier_id;
   string field_id;
};

void modifier_field_required_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'modifier_field_required' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_modifier_name( args[ 1 ] );
   string arg_field_name( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   modifier_id = get_modifier_id_for_name( m, arg_class_name, arg_modifier_name );
   if( modifier_id.empty( ) )
      throw runtime_error( "unknown modifier '" + arg_modifier_name + "' for class '" + arg_class_name + "'" );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, 0, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

void modifier_field_required_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   modifier_id = reader.read_attribute( c_attribute_modifier_id );
   field_id = reader.read_attribute( c_attribute_field_id );
}

void modifier_field_required_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_modifier_id, modifier_id );
   writer.write_attribute( c_attribute_field_id, field_id );
}

void modifier_field_required_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string modifier_name = get_modifier_name_for_id( m, class_name, modifier_id );
   spec_data.data_pairs.push_back( make_pair( c_data_modifier, modifier_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_tfield, "" ) );
}

string modifier_field_required_specification::static_class_name( ) { return "modifier_field_required"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, modifier_field_required_specification, static_class_name );

struct modifier_field_value_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;
   string tclass_id;

   string modifier_id;

   string field_id;
   string pfield_id;
   string field_value;

   string tfield_id;
   string tpfield_id;
   string test_value;

   string enum_id;
   string enum_value_name;

   bool not_equal;
   bool not_default;
};

void modifier_field_value_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'modifier_field_value' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_modifier_name( args[ 1 ] );
   string arg_field_info( args[ 2 ] );
   string arg_field_value( args[ 3 ] );

   not_equal = false;
   not_default = false;

   string enum_name, test_field_info;
   for( size_t arg = 4; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg.find( c_arg_enum_prefix ) == 0 )
         enum_name = next_arg.substr( strlen( c_arg_enum_prefix ) );
      else if( next_arg.find( c_arg_test_prefix ) == 0 )
         test_field_info = next_arg.substr( strlen( c_arg_test_prefix ) );
      else if( next_arg == c_arg_not_equal )
         not_equal = true;
      else if( next_arg == c_arg_not_default )
         not_default = true;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'modifier_field_value' specification" );
   }

   string test_field_name;
   if( !test_field_info.empty( ) )
   {
      string::size_type pos = test_field_info.find( '=' );
      test_field_name = test_field_info.substr( 0, pos );
      if( pos != string::npos )
         test_value = test_field_info.substr( pos + 1 );
   }

   if( !enum_name.empty( ) )
      enum_value_name = arg_field_value;

   class_id = get_class_id_for_name( m, arg_class_name );

   modifier_id = get_modifier_id_for_name( m, arg_class_name, arg_modifier_name );
   if( modifier_id.empty( ) )
      throw runtime_error( "unknown modifier '" + arg_modifier_name + "' for class '" + arg_class_name + "'" );

   string field_name, pfield_name;
   string::size_type pos = arg_field_info.find( ':' );
   if( pos == string::npos )
      field_name = arg_field_info;
   else
   {
      field_name = arg_field_info.substr( 0, pos );
      pfield_name = arg_field_info.substr( pos + 1 );
   }

   string field_type;
   field_id = get_field_id_for_name( m, arg_class_name, field_name, &field_type, true, false, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + field_name + "' for class '" + arg_class_name + "'" );

   if( !pfield_name.empty( ) )
   {
      string pclass_name( get_class_name_from_field_type( m, arg_class_name, field_name, field_type ) );

      string pfield_type;
      pclass_id = get_class_id_for_name( m, pclass_name );
      pfield_id = get_field_id_for_name( m, pclass_name, pfield_name, &pfield_type );

      if( pfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + pclass_name + "'" );
   }

   if( !arg_field_value.empty( ) )
      field_value = arg_field_value;

   if( !enum_name.empty( ) )
   {
      enum_id = get_id_for_enum_name( m, enum_name );

      if( enum_id.empty( ) )
         throw runtime_error( "unknown enum '" + enum_name + "'" );
   }

   if( !test_field_name.empty( ) )
   {
      string test_class_name( arg_class_name );

      string test_pfield_name;
      string::size_type pos = test_field_name.find( ':' );
      if( pos != string::npos )
      {
         test_pfield_name = test_field_name.substr( 0, pos );
         test_field_name.erase( 0, pos + 1 );

         string test_pfield_type;
         tpfield_id = get_field_id_for_name( m, arg_class_name, test_pfield_name, &test_pfield_type, true );
         if( tpfield_id.empty( ) )
            throw runtime_error( "unknown field '" + test_pfield_name + "' for class '" + arg_class_name + "'" );

         string test_pclass_name( get_class_name_from_field_type( m, arg_class_name, test_pfield_name, test_pfield_type ) );

         tclass_id = get_class_id_for_name( m, test_pclass_name );
         test_class_name = test_pclass_name;
      }

      tfield_id = get_field_id_for_name( m, test_class_name, test_field_name );
      if( tfield_id.empty( ) )
         throw runtime_error( "unknown field '" + test_field_name + "' for class '" + test_class_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   if( !pclass_id.empty( ) )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   if( !tclass_id.empty( ) )
      details.push_back( specification_detail( tclass_id, "tclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );

   if( !pfield_id.empty( ) )
      details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );

   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );

   if( !tpfield_id.empty( ) )
      details.push_back( specification_detail( tpfield_id, "tpfield", e_model_element_type_field ) );
}

void modifier_field_value_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_opt_attribute( c_attribute_pclass_id );
   tclass_id = reader.read_opt_attribute( c_attribute_tclass_id );

   modifier_id = reader.read_attribute( c_attribute_modifier_id );
   enum_id = reader.read_opt_attribute( c_attribute_enum_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   pfield_id = reader.read_opt_attribute( c_attribute_pfield_id );
   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );
   tpfield_id = reader.read_opt_attribute( c_attribute_tpfield_id );
   test_value = reader.read_opt_attribute( c_attribute_test_value );

   field_value = reader.read_attribute( c_attribute_field_value );
   enum_value_name = reader.read_opt_attribute( c_attribute_enum_value_name );

   not_equal = ( reader.read_opt_attribute( c_attribute_not_equal ) == c_true );
   not_default = ( reader.read_opt_attribute( c_attribute_not_default ) == c_true );
}

void modifier_field_value_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_opt_attribute( c_attribute_tclass_id, tclass_id );

   writer.write_attribute( c_attribute_modifier_id, modifier_id );
   writer.write_opt_attribute( c_attribute_enum_id, enum_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_opt_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );
   writer.write_opt_attribute( c_attribute_tpfield_id, tpfield_id );
   writer.write_opt_attribute( c_attribute_test_value, test_value );

   writer.write_attribute( c_attribute_field_value, field_value );
   writer.write_opt_attribute( c_attribute_enum_value_name, enum_value_name );

   writer.write_opt_attribute( c_attribute_not_equal, not_equal ? c_true : "" );
   writer.write_opt_attribute( c_attribute_not_default, not_default ? c_true : "" );
}

void modifier_field_value_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string modifier_name = get_modifier_name_for_id( m, class_name, modifier_id );
   spec_data.data_pairs.push_back( make_pair( c_data_modifier, modifier_name ) );

   string field_type;
   if( !pclass_id.empty( ) )
   {
      string field_name = get_field_name_for_id( m, class_name, field_id, 0, true );
      spec_data.data_pairs.push_back( make_pair( c_data_pfield, field_name ) );

      string pclass_name = get_class_name_for_id( m, pclass_id );
      string pfield_name = get_field_name_for_id( m, pclass_name, pfield_id, &field_type );

      spec_data.data_pairs.push_back( make_pair( c_data_field, pfield_name ) );
   }
   else
   {
      spec_data.data_pairs.push_back( make_pair( c_data_pfield, "" ) );

      string field_name = get_field_name_for_id( m, class_name, field_id, &field_type, true );
      spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );
   }

   string tclass_name( class_name );
   if( !tclass_id.empty( ) )
      tclass_name = get_class_name_for_id( m, tclass_id );

   string tpfield_name;
   if( !tpfield_id.empty( ) )
      tpfield_name = get_field_name_for_id( m, class_name, tpfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tpfield, tpfield_name ) );

   string tfield_name;
   if( !tfield_id.empty( ) )
      tfield_name = get_field_name_for_id( m, tclass_name, tfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   if( enum_id.empty( ) )
      spec_data.data_pairs.push_back( make_pair( c_data_value, field_value ) );
   else
      spec_data.data_pairs.push_back( make_pair( c_data_value, get_key_for_enum_value( m, enum_id, field_value ) ) );

   spec_data.data_pairs.push_back( make_pair( c_data_evalue, enum_value_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_not_eq, not_equal ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_not_dflt, not_default ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_cvt_func, "" ) );

   bool is_text_type( !is_non_string_type( field_type ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fistexttype, is_text_type ? "1" : "0" ) );

   spec_data.data_pairs.push_back( make_pair( "testop", "" ) );
   spec_data.data_pairs.push_back( make_pair( "new_only", "" ) );
}

string modifier_field_value_specification::static_class_name( ) { return "modifier_field_value"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, modifier_field_value_specification, static_class_name );

struct modifier_past_date_time_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string modifier_id;

   string dfield_id;
   string dfield_prim;

   string tfield_id;
   string tfield_value;

   bool is_future;

   string value;
};

void modifier_past_date_time_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'modifier_past_date_time' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_modifier_name( args[ 1 ] );
   string arg_dfield_name( args[ 2 ] );

   string arg_add_info, arg_test_info, arg_tfield_name, arg_tfield_value, arg_dfield_type( "datetime" );

   is_future = false;

   for( size_t arg = 3; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == string( c_arg_future ) && !is_future )
         is_future = true;
      else if( next_arg.find( c_arg_add_prefix ) == 0 )
         arg_add_info = next_arg.substr( strlen( c_arg_add_prefix ) );
      else if( next_arg.find( c_arg_test_prefix ) == 0 )
         arg_test_info = next_arg.substr( strlen( c_arg_test_prefix ) );
      else if( next_arg.find( c_arg_type_prefix ) == 0 )
         arg_dfield_type = next_arg.substr( strlen( c_arg_type_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'past_date_time' specification" );
   }

   string add_amt;
   string add_type;
   if( !arg_add_info.empty( ) )
   {
      string::size_type pos = arg_add_info.find( ';' );

      add_amt = arg_add_info.substr( 0, pos );
      if( pos != string::npos )
         add_type = arg_add_info.substr( pos + 1 );

      value = add_amt;
      if( !add_type.empty( ) )
         value += " " + add_type;
   }

   if( !arg_test_info.empty( ) )
   {
      string::size_type pos = arg_test_info.find( ':' );

      arg_tfield_name = arg_test_info.substr( 0, pos );
      if( pos != string::npos )
         arg_tfield_value = arg_test_info.substr( pos + 1 );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   modifier_id = get_modifier_id_for_name( m, arg_class_name, arg_modifier_name );
   if( modifier_id.empty( ) )
      throw runtime_error( "unknown modifier '" + arg_modifier_name + "' for class '" + arg_class_name + "'" );

   string field_type;
   dfield_id = get_field_id_for_name( m, arg_class_name, arg_dfield_name, &field_type, true, true );
   if( dfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_dfield_name + "' for class '" + arg_class_name + "'" );

   string dfield_type = arg_dfield_type;

   if( field_type != dfield_type )
      throw runtime_error( "field '" + arg_dfield_name + "' is of type '" + field_type + "' not '" + dfield_type + "'" );

   dfield_type = internal_type_for_field_type( dfield_type );
   dfield_prim = to_string( internal_type_enum_value( dfield_type ) );

   if( !arg_tfield_name.empty( ) )
   {
      tfield_id = get_field_id_for_name( m, arg_class_name, arg_tfield_name, &field_type, true );
      if( tfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_tfield_name + "' for class '" + arg_class_name + "'" );

      bool is_non_string( is_non_string_type( field_type ) );

      if( !tfield_value.empty( ) )
      {
         if( is_non_string )
            tfield_value = arg_tfield_value;
         else
            tfield_value = '"' + arg_tfield_value + '"';
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   details.push_back( specification_detail( dfield_id, "dfield", e_model_element_type_field ) );
   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );
}

void modifier_past_date_time_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   modifier_id = reader.read_attribute( c_attribute_modifier_id );
   dfield_id = reader.read_attribute( c_attribute_dfield_id );
   dfield_prim = reader.read_attribute( c_attribute_dfield_prim );

   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );
   tfield_value = reader.read_opt_attribute( c_attribute_tfield_value );

   is_future = ( reader.read_opt_attribute( c_attribute_is_future ) == c_true );

   value = reader.read_opt_attribute( c_attribute_value );
}

void modifier_past_date_time_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_modifier_id, modifier_id );
   writer.write_attribute( c_attribute_dfield_id, dfield_id );
   writer.write_attribute( c_attribute_dfield_prim, dfield_prim );

   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );
   writer.write_opt_attribute( c_attribute_tfield_value, tfield_value );

   writer.write_opt_attribute( c_attribute_is_future, is_future ? c_true : "" );

   writer.write_opt_attribute( c_attribute_value, value );
}

void modifier_past_date_time_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string modifier_name = get_modifier_name_for_id( m, class_name, modifier_id );
   spec_data.data_pairs.push_back( make_pair( c_data_modifier, modifier_name ) );

   string dfield_name = get_field_name_for_id( m, class_name, dfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_dfield, dfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_dfprim, dfield_prim ) );

   string tfield_name = get_field_name_for_id( m, class_name, tfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, tfield_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_future, is_future ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_value, value ) );
}

string modifier_past_date_time_specification::static_class_name( ) { return "modifier_past_date_time"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, modifier_past_date_time_specification, static_class_name );

struct modifier_set_field_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string modifier_id;

   string field_id;
   string field_value;
};

void modifier_set_field_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'modifier_set_field' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_modifier_name( args[ 1 ] );
   string arg_field_name( args[ 2 ] );
   string arg_field_value( args[ 3 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   modifier_id = get_modifier_id_for_name( m, arg_class_name, arg_modifier_name );
   if( modifier_id.empty( ) )
      throw runtime_error( "unknown modifier '" + arg_modifier_name + "' for class '" + arg_class_name + "'" );

   string field_type;
   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, &field_type, true );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   bool is_non_string( is_non_string_type( field_type ) );

   if( is_non_string )
      field_value = arg_field_value;
   else
      field_value = '"' + arg_field_value + '"';

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

void modifier_set_field_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   modifier_id = reader.read_attribute( c_attribute_modifier_id );
   field_id = reader.read_attribute( c_attribute_field_id );
   field_value = reader.read_attribute( c_attribute_field_value );
}

void modifier_set_field_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_modifier_id, modifier_id );
   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_field_value, field_value );
}

void modifier_set_field_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string modifier_name = get_modifier_name_for_id( m, class_name, modifier_id );
   spec_data.data_pairs.push_back( make_pair( c_data_modifier, modifier_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_value, field_value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_evalue, "" ) );

   spec_data.data_pairs.push_back( make_pair( "for_store", "" ) );
   spec_data.data_pairs.push_back( make_pair( "non_trigger", "" ) );
   spec_data.data_pairs.push_back( make_pair( "create_only", "" ) );
   spec_data.data_pairs.push_back( make_pair( "update_only", "" ) );
}

string modifier_set_field_specification::static_class_name( ) { return "modifier_set_field"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, modifier_set_field_specification, static_class_name );

struct move_up_and_down_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string field_id;

   string move_up_procedure_id;
   string move_up_fields_arg_id;
   string move_up_values_arg_id;

   string move_down_procedure_id;
   string move_down_fields_arg_id;
   string move_down_values_arg_id;
};

void move_up_and_down_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 6 )
      throw runtime_error( "unexpected number of args != 6 for 'move_up_and_down' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_move_up_procedure_name( args[ 1 ] );
   string arg_move_down_procedure_name( args[ 2 ] );
   string arg_fields_arg_name( args[ 3 ] );
   string arg_values_arg_name( args[ 4 ] );
   string arg_field_name( args[ 5 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   move_up_procedure_id = get_procedure_id_for_name( m, arg_class_name, arg_move_up_procedure_name );

   if( move_up_procedure_id.empty( ) )
      throw runtime_error( "move_up procedure '" + arg_move_up_procedure_name + "' not found in class '" + arg_class_name + "'" );

   move_up_fields_arg_id = get_procedure_arg_id_for_name( m,
    arg_class_name, arg_move_up_procedure_name, arg_fields_arg_name );
   if( move_up_fields_arg_id.empty( ) )
      throw runtime_error( "procedure arg '"
       + arg_fields_arg_name + "' not found in procedure '" + arg_move_up_procedure_name + "'" );

   move_up_values_arg_id = get_procedure_arg_id_for_name( m,
    arg_class_name, arg_move_up_procedure_name, arg_values_arg_name );
   if( move_up_values_arg_id.empty( ) )
      throw runtime_error( "procedure arg '"
       + arg_values_arg_name + "' not found in procedure '" + arg_move_up_procedure_name + "'" );

   move_down_procedure_id = get_procedure_id_for_name( m, arg_class_name, arg_move_down_procedure_name );

   if( move_down_procedure_id.empty( ) )
      throw runtime_error( "move_down procedure '" + arg_move_down_procedure_name + "' not found in class '" + arg_class_name + "'" );

   move_down_fields_arg_id = get_procedure_arg_id_for_name( m,
    arg_class_name, arg_move_down_procedure_name, arg_fields_arg_name );
   if( move_down_fields_arg_id.empty( ) )
      throw runtime_error( "procedure arg '"
       + arg_fields_arg_name + "' not found in procedure '" + arg_move_down_procedure_name + "'" );

   move_down_values_arg_id = get_procedure_arg_id_for_name( m,
    arg_class_name, arg_move_down_procedure_name, arg_values_arg_name );
   if( move_down_values_arg_id.empty( ) )
      throw runtime_error( "procedure arg '"
       + arg_values_arg_name + "' not found in procedure '" + arg_move_down_procedure_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );

   details.push_back( specification_detail( move_up_procedure_id, "up_procedure", e_model_element_type_procedure ) );
   details.push_back( specification_detail( move_up_fields_arg_id, "up_fields_arg", e_model_element_type_procedure_arg ) );
   details.push_back( specification_detail( move_up_values_arg_id, "up_values_arg", e_model_element_type_procedure_arg ) );

   details.push_back( specification_detail( move_down_procedure_id, "down_procedure", e_model_element_type_procedure ) );
   details.push_back( specification_detail( move_down_fields_arg_id, "down_fields_arg", e_model_element_type_procedure_arg ) );
   details.push_back( specification_detail( move_down_values_arg_id, "down_values_arg", e_model_element_type_procedure_arg ) );
}

void move_up_and_down_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   field_id = reader.read_attribute( c_attribute_field_id );

   move_up_procedure_id = reader.read_attribute( c_attribute_move_up_procedure_id );
   move_up_fields_arg_id = reader.read_attribute( c_attribute_move_up_fields_arg_id );
   move_up_values_arg_id = reader.read_attribute( c_attribute_move_up_values_arg_id );

   move_down_procedure_id = reader.read_attribute( c_attribute_move_down_procedure_id );
   move_down_fields_arg_id = reader.read_attribute( c_attribute_move_down_fields_arg_id );
   move_down_values_arg_id = reader.read_attribute( c_attribute_move_down_values_arg_id );
}

void move_up_and_down_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_field_id, field_id );

   writer.write_attribute( c_attribute_move_up_procedure_id, move_up_procedure_id );
   writer.write_attribute( c_attribute_move_up_fields_arg_id, move_up_fields_arg_id );
   writer.write_attribute( c_attribute_move_up_values_arg_id, move_up_values_arg_id );

   writer.write_attribute( c_attribute_move_down_procedure_id, move_down_procedure_id );
   writer.write_attribute( c_attribute_move_down_fields_arg_id, move_down_fields_arg_id );
   writer.write_attribute( c_attribute_move_down_values_arg_id, move_down_values_arg_id );
}

void move_up_and_down_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_field ), field_name ) );

   string move_up_procedure_name = get_procedure_name_for_id( m, class_name, move_up_procedure_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_up_proc ), move_up_procedure_name ) );

   string move_up_fields_arg_name
    = get_procedure_arg_name_for_id( m, class_name, move_up_procedure_name, move_up_fields_arg_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_up_fields ), move_up_fields_arg_name ) );

   string move_up_values_arg_name
    = get_procedure_arg_name_for_id( m, class_name, move_up_procedure_name, move_up_values_arg_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_up_values ), move_up_values_arg_name ) );

   string move_down_procedure_name = get_procedure_name_for_id( m, class_name, move_down_procedure_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_dn_proc ), move_down_procedure_name ) );

   string move_down_fields_arg_name
    = get_procedure_arg_name_for_id( m, class_name, move_down_procedure_name, move_down_fields_arg_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_dn_fields ), move_down_fields_arg_name ) );

   string move_down_values_arg_name
    = get_procedure_arg_name_for_id( m, class_name, move_down_procedure_name, move_down_values_arg_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_dn_values ), move_down_values_arg_name ) );

   spec_data.data_pairs.push_back( make_pair( "minimal", "true" ) );
   spec_data.data_pairs.push_back( make_pair( "update_parent", "" ) );
}

string move_up_and_down_specification::static_class_name( ) { return "move_up_and_down"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, move_up_and_down_specification, static_class_name );

struct parent_auto_int_inc_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;

   string field_id;
   string pfield_id;
   string cfield_id;
};

void parent_auto_int_inc_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 4 )
      throw runtime_error( "unexpected number of args != 4 for 'parent_auto_int_inc' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_pfield_name( args[ 2 ] );
   string arg_cfield_name( args[ 3 ] );

   class_id = get_class_id_for_name( m, arg_class_name );
   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   string pfield_type;
   pfield_id = get_field_id_for_name( m, arg_class_name, arg_pfield_name, &pfield_type, true );

   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pfield_name + "' for class '" + arg_class_name + "'" );

   string pclass_name( get_class_name_from_field_type( m, arg_class_name, arg_pfield_name, pfield_type ) );

   pclass_id = get_class_id_for_name( m, pclass_name );

   cfield_id = get_field_id_for_name( m, pclass_name, arg_cfield_name );
   if( cfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_cfield_name + "' for class '" + pclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !pclass_id.empty( ) && pclass_id != class_id )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );
}

void parent_auto_int_inc_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_attribute( c_attribute_pclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   cfield_id = reader.read_attribute( c_attribute_cfield_id );
}

void parent_auto_int_inc_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_pclass_id, pclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_cfield_id, cfield_id );
}

void parent_auto_int_inc_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pclass_name = get_class_name_for_id( m, pclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pclass, pclass_name ) );

   bool is_mandatory;
   string field_name = get_field_name_for_id( m, class_name, field_id, 0, false, false, &is_mandatory );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_fmandatory, is_mandatory ? "1" : "0" ) );

   string pfield_name = get_field_name_for_id( m, class_name, pfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string cfield_name = get_field_name_for_id( m, pclass_name, cfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_cfield, cfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_static, "" ) );
}

string parent_auto_int_inc_specification::static_class_name( ) { return "parent_auto_int_inc"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, parent_auto_int_inc_specification, static_class_name );

struct parent_field_from_other_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;
   string sclass_id;

   string pfield_id;
   string dfield_id;
   string sfield_id;
   string spfield_id;

   string tfield_id;
   string test_value;
};

void parent_field_from_other_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'parent_field_from_other' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_pfield_name( args[ 1 ] );
   string arg_dfield_name( args[ 2 ] );
   string arg_sfield_info( args[ 3 ] );

   string tfield_info;
   for( size_t arg = 4; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg.find( c_arg_test_prefix ) == 0 )
         tfield_info = next_arg.substr( strlen( c_arg_test_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'parent_field_from_other' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string pfield_type;
   pfield_id = get_field_id_for_name( m, arg_class_name, arg_pfield_name, &pfield_type, true );

   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pfield_name + "' for class '" + arg_class_name + "'" );

   string pclass_name( get_class_name_from_field_type( m, arg_class_name, arg_pfield_name, pfield_type ) );

   pclass_id = get_class_id_for_name( m, pclass_name );
   dfield_id = get_field_id_for_name( m, pclass_name, arg_dfield_name, 0, true );

   if( dfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_dfield_name + "' for class '" + pclass_name + "'" );

   string tfield_name( tfield_info );
   string::size_type pos = tfield_info.find( '=' );
   if( pos != string::npos )
   {
      tfield_name.erase( pos );
      test_value = tfield_info.substr( pos + 1 );
   }

   if( !arg_sfield_info.empty( ) )
   {
      string sfield_name, sfield_class_name( arg_class_name );

      string::size_type pos = arg_sfield_info.find( ':' );
      if( pos == string::npos )
         sfield_name = arg_sfield_info;
      else
      {
         string spfield_name( arg_sfield_info.substr( 0, pos ) );
         sfield_name = arg_sfield_info.substr( pos + 1 );

         string spfield_type;
         spfield_id = get_field_id_for_name( m, arg_class_name, spfield_name, &spfield_type, true );
         if( spfield_id.empty( ) )
            throw runtime_error( "unknown field '" + spfield_name + "' for class '" + arg_class_name + "'" );

         string spfield_class_name( get_class_name_from_field_type( m, arg_class_name, spfield_name, spfield_type ) );

         sclass_id = get_class_id_for_name( m, spfield_class_name );
         sfield_class_name = spfield_class_name;
      }

      sfield_id = get_field_id_for_name( m, sfield_class_name, sfield_name, 0, true );
      if( sfield_id.empty( ) )
         throw runtime_error( "unknown field '" + sfield_name + "' for class '" + sfield_class_name + "'" );

      if( !tfield_name.empty( ) )
      {
         tfield_id = get_field_id_for_name( m, sfield_class_name, tfield_name );
         if( tfield_id.empty( ) )
            throw runtime_error( "unknown field '" + tfield_name + "' for class '" + sfield_class_name + "'" );
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !pclass_id.empty( ) && pclass_id != class_id )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );
   if( !sclass_id.empty( ) )
      details.push_back( specification_detail( sclass_id, "sclass", e_model_element_type_class ) );

   details.push_back( specification_detail( dfield_id, "dfield", e_model_element_type_field ) );
   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );
   if( !spfield_id.empty( ) )
      details.push_back( specification_detail( spfield_id, "spfield", e_model_element_type_field ) );

   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );
}

void parent_field_from_other_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_attribute( c_attribute_pclass_id );
   sclass_id = reader.read_opt_attribute( c_attribute_sclass_id );

   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   dfield_id = reader.read_attribute( c_attribute_dfield_id );

   sfield_id = reader.read_attribute( c_attribute_sfield_id );
   spfield_id = reader.read_opt_attribute( c_attribute_spfield_id );

   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );
   test_value = reader.read_opt_attribute( c_attribute_test_value );
}

void parent_field_from_other_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_opt_attribute( c_attribute_sclass_id, sclass_id );

   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_dfield_id, dfield_id );

   writer.write_attribute( c_attribute_sfield_id, sfield_id );
   writer.write_opt_attribute( c_attribute_spfield_id, spfield_id );

   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );
   writer.write_opt_attribute( c_attribute_test_value, test_value );
}

void parent_field_from_other_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pclass_name = get_class_name_for_id( m, pclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pclass, pclass_name ) );

   string pfield_name = get_field_name_for_id( m, class_name, pfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string dfield_name = get_field_name_for_id( m, pclass_name, dfield_id );
   spec_data.data_pairs.push_back( make_pair( string( c_data_dfield ), dfield_name ) );

   string sclass_name( class_name );
   if( !sclass_id.empty( ) )
      sclass_name = get_class_name_for_id( m, sclass_id );

   string spfield_name;
   if( !spfield_id.empty( ) )
      spfield_name = get_field_name_for_id( m, class_name, spfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_spfield, spfield_name ) );

   string sfield_name;
   if( !sfield_id.empty( ) )
      sfield_name = get_field_name_for_id( m, sclass_name, sfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );

   string tfield_name;
   if( !tfield_id.empty( ) )
      tfield_name = get_field_name_for_id( m, sclass_name, tfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_test_value, test_value ) );
}

string parent_field_from_other_specification::static_class_name( ) { return "parent_field_from_other"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, parent_field_from_other_specification, static_class_name );

struct state_protect_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string protect_spec;
   string protect_value;
   string protect_scope;

   bool is_changing;
};

void state_protect_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args < 4 for 'state_protect' specification" );

   string arg_type_name( args[ 0 ] );
   string arg_class_name( args[ 1 ] );
   string arg_field_name( args[ 2 ] );
   string arg_field_value( args[ 3 ] );
   string arg_protect_scope;

   is_changing = false;
   for( size_t arg = 4; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_changing )
         is_changing = true;
      else if( next_arg.find( c_arg_scope_prefix ) == 0 )
         arg_protect_scope = next_arg.substr( strlen( c_arg_scope_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'state_protect' specification" );
   }

   string class_id = get_class_id_for_name( m, arg_class_name );

   if( arg_type_name == "equal" )
      protect_spec = "protect_equal.spec.xrep";
   else if( arg_type_name == "not_equal" )
      protect_spec = "protect_not_equal.spec.xrep";
   else
      throw runtime_error( "unknown protect type '" + arg_type_name + "'" );

   string field_id, field_type;
   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name, &field_type );

   if( field_id.empty( ) )
      throw runtime_error( "unknown field name '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   bool is_non_string( is_non_string_type( field_type ) );

   if( is_non_string )
      protect_value = args[ 3 ];
   else
      protect_value = '"' + args[ 3 ] + '"';

   protect_scope = arg_protect_scope;

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

void state_protect_specification::read_data( sio_reader& reader )
{
   protect_spec = reader.read_attribute( c_attribute_protect_spec );
   protect_value = reader.read_opt_attribute( c_attribute_protect_value );
   protect_scope = reader.read_opt_attribute( c_attribute_protect_scope );
   is_changing = ( reader.read_opt_attribute( c_attribute_is_changing ) == c_true );
}

void state_protect_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_protect_spec, protect_spec );
   writer.write_opt_attribute( c_attribute_protect_value, protect_value );
   writer.write_opt_attribute( c_attribute_protect_scope, protect_scope );
   writer.write_opt_attribute( c_attribute_is_changing, is_changing ? c_true : "" );
}

void state_protect_specification::add_specification_data( model& /*m*/, specification_data& spec_data ) const
{
   spec_data.data_pairs.push_back( make_pair( c_data_spec_file, protect_spec ) );
   spec_data.data_pairs.push_back( make_pair( c_data_spec_value, protect_value ) );
   spec_data.data_pairs.push_back( make_pair( c_data_spec_scope, protect_scope ) );
   spec_data.data_pairs.push_back( make_pair( c_data_changing, is_changing ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_label, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_modifier, "" ) );
   spec_data.data_pairs.push_back( make_pair( "could_change", "" ) );
   spec_data.data_pairs.push_back( make_pair( "admin_always", "" ) );
   spec_data.data_pairs.push_back( make_pair( "scripts_always", "" ) );
}

string state_protect_specification::static_class_name( ) { return "state_protect"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, state_protect_specification, static_class_name );

struct total_child_field_in_parent_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;
   string nfield_id;
   string pfield_id;
   string pnfield_id;
};

void total_child_field_in_parent_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 || args.size( ) > 4 )
      throw runtime_error( "unexpected number of args for 'total_child_field_in_parent' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_nfield_name( args[ 1 ] );
   string arg_pfield_name( args[ 2 ] );

   string arg_pnfield_name;
   if( args.size( ) > 3 )
      arg_pnfield_name = args[ 3 ];
   else
      arg_pnfield_name = arg_nfield_name;

   class_id = get_class_id_for_name( m, arg_class_name );

   nfield_id = get_field_id_for_name( m, arg_class_name, arg_nfield_name );
   if( nfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_nfield_name + "' for class '" + arg_class_name + "'" );

   string pfield_type;
   pfield_id = get_field_id_for_name( m, arg_class_name, arg_pfield_name, &pfield_type, true );

   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pfield_name + "' for class '" + arg_class_name + "'" );

   string pclass_name( get_class_name_from_field_type( m, arg_class_name, arg_pfield_name, pfield_type ) );

   pclass_id = get_class_id_for_name( m, pclass_name );

   pnfield_id = get_field_id_for_name( m, pclass_name, arg_pnfield_name );
   if( pnfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pnfield_name + "' for class '" + pclass_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( nfield_id, "nfield", e_model_element_type_field ) );
   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   details.push_back( specification_detail( pnfield_id, "pnfield", e_model_element_type_field ) );
}

void total_child_field_in_parent_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_attribute( c_attribute_pclass_id );
   nfield_id = reader.read_attribute( c_attribute_nfield_id );
   pfield_id = reader.read_attribute( c_attribute_pfield_id );
   pnfield_id = reader.read_attribute( c_attribute_pnfield_id );
}

void total_child_field_in_parent_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_attribute( c_attribute_nfield_id, nfield_id );
   writer.write_attribute( c_attribute_pfield_id, pfield_id );
   writer.write_attribute( c_attribute_pnfield_id, pnfield_id );
}

void total_child_field_in_parent_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   string pclass_name = get_class_name_for_id( m, pclass_id );

   string nfield_name = get_field_name_for_id( m, class_name, nfield_id );
   string pfield_name = get_field_name_for_id( m, class_name, pfield_id );

   string pnfield_name = get_field_name_for_id( m, pclass_name, pnfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_pclass, pclass_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_nfield, nfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_pnfield, pnfield_name ) );
   spec_data.data_pairs.push_back( make_pair( "pfkfield", "" ) );
   spec_data.data_pairs.push_back( make_pair( "tfield", "" ) );
}

string total_child_field_in_parent_specification::static_class_name( ) { return "total_child_field_in_parent"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, total_child_field_in_parent_specification, static_class_name );

struct transient_field_alias_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string sclass_id;

   string field_id;

   string sfield_id;
   string scfield_id;
};

void transient_field_alias_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 3 )
      throw runtime_error( "unexpected number of args for 'transient_field_alias_specification' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_sfield_info( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   string::size_type pos = arg_sfield_info.find( ':' );
   if( pos == string::npos )
   {
      sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_info );
      if( sfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_sfield_info + "' for class '" + arg_class_name + "'" );
   }
   else
   {
      string pfield_name, cfield_name;

      pfield_name = arg_sfield_info.substr( 0, pos );
      cfield_name = arg_sfield_info.substr( pos + 1 );

      string pfield_type;
      sfield_id = get_field_id_for_name( m, arg_class_name, pfield_name, &pfield_type, true );

      if( sfield_id.empty( ) )
         throw runtime_error( "unknown field '" + pfield_name + "' for class '" + arg_class_name + "'" );

      string pclass_name( get_class_name_from_field_type( m, arg_class_name, pfield_name, pfield_type ) );

      sclass_id = get_class_id_for_name( m, pclass_name );
      scfield_id = get_field_id_for_name( m, pclass_name, cfield_name );

      if( scfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cfield_name + "' for class '" + pclass_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !sclass_id.empty( ) )
      details.push_back( specification_detail( sclass_id, "sclass", e_model_element_type_class ) );

   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );

   if( !scfield_id.empty( ) )
      details.push_back( specification_detail( scfield_id, "scfield", e_model_element_type_field ) );
}

void transient_field_alias_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   sclass_id = reader.read_opt_attribute( c_attribute_sclass_id );

   field_id = reader.read_attribute( c_attribute_field_id );
   sfield_id = reader.read_attribute( c_attribute_sfield_id );

   scfield_id = reader.read_opt_attribute( c_attribute_scfield_id );
}

void transient_field_alias_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_sclass_id, sclass_id );

   writer.write_attribute( c_attribute_field_id, field_id );
   writer.write_attribute( c_attribute_sfield_id, sfield_id );

   writer.write_opt_attribute( c_attribute_scfield_id, scfield_id );
}

void transient_field_alias_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );

   string field_name = get_field_name_for_id( m, class_name, field_id );
   string sfield_name = get_field_name_for_id( m, class_name, sfield_id );

   string sclass_name, scfield_name;
   if( !sclass_id.empty( ) )
   {
      sclass_name = get_class_name_for_id( m, sclass_id );
      scfield_name = get_field_name_for_id( m, sclass_name, scfield_id );
   }

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_field, field_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_sfield, sfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_scfield, scfield_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_not_dflt, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_func, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_mfunc, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_field1, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_field2, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tfield, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tfistexttype, "" ) );
   spec_data.data_pairs.push_back( make_pair( "not_new", "" ) );
   spec_data.data_pairs.push_back( make_pair( "def_value", "" ) );
   spec_data.data_pairs.push_back( make_pair( "iter_only", "" ) );
   spec_data.data_pairs.push_back( make_pair( "check_mod", "" ) );
   spec_data.data_pairs.push_back( make_pair( "func_arg1", "" ) );
   spec_data.data_pairs.push_back( make_pair( "func_arg2", "" ) );
   spec_data.data_pairs.push_back( make_pair( "top_level", "" ) );
   spec_data.data_pairs.push_back( make_pair( "top_or_iter", "" ) );
   spec_data.data_pairs.push_back( make_pair( "post_init", "" ) );
   spec_data.data_pairs.push_back( make_pair( "completed_fetch", "" ) );
}

string transient_field_alias_specification::static_class_name( ) { return "transient_field_alias"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, transient_field_alias_specification, static_class_name );

struct transient_field_concat_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string dfield_id;
   string field1_id;
   string field2_id;

   string pclass_id;
   string cfield_id;
   string rfield_id;
   string separator;

   bool is_reverse;
   bool both_non_empty;
};

void transient_field_concat_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "unexpected number of args for 'transient_field_concat_specification' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_dfield_name( args[ 1 ] );
   string arg_field1_name( args[ 2 ] );
   string arg_field2_name( args[ 3 ] );

   string arg_cfield_name, arg_rfield_name;

   is_reverse = false;
   both_non_empty = false;
   for( size_t arg = 4; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( !is_reverse && next_arg == string( c_arg_reverse ) )
         is_reverse = true;
      else if( !both_non_empty && next_arg == string( c_arg_non_empty ) )
         both_non_empty = true;
      else if( next_arg.find( c_arg_sep_prefix ) == 0 )
         separator = next_arg.substr( strlen( c_arg_sep_prefix ) );
      else if( next_arg.find( c_arg_child_prefix ) == 0 )
         arg_cfield_name = next_arg.substr( strlen( c_arg_child_prefix ) );
      else if( next_arg.find( c_arg_reverse_prefix ) == 0 )
         arg_rfield_name = next_arg.substr( strlen( c_arg_reverse_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'transient_field_concat' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   dfield_id = get_field_id_for_name( m, arg_class_name, arg_dfield_name );
   if( dfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_dfield_name + "' for class '" + arg_class_name + "'" );

   field1_id = get_field_id_for_name( m, arg_class_name, arg_field1_name, 0, true );
   if( field1_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field1_name + "' for class '" + arg_class_name + "'" );

   string field2_type;
   field2_id = get_field_id_for_name( m, arg_class_name, arg_field2_name, &field2_type, true );

   if( field2_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field2_name + "' for class '" + arg_class_name + "'" );

   if( !arg_cfield_name.empty( ) )
   {
      string field2_class_name( get_class_name_from_field_type( m, arg_class_name, arg_field2_name, field2_type ) );

      pclass_id = get_class_id_for_name( m, field2_class_name );
      cfield_id = get_field_id_for_name( m, field2_class_name, arg_cfield_name );

      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_cfield_name + "' for class '" + field2_class_name + "'" );

      if( !arg_rfield_name.empty( ) )
      {
         rfield_id = get_field_id_for_name( m, field2_class_name, arg_rfield_name );

         if( rfield_id.empty( ) )
            throw runtime_error( "unknown field '" + arg_rfield_name + "' for class '" + field2_class_name + "'" );
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( !pclass_id.empty( ) )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( dfield_id, "dfield", e_model_element_type_field ) );
   details.push_back( specification_detail( field1_id, "field1", e_model_element_type_field ) );
   details.push_back( specification_detail( field2_id, "field2", e_model_element_type_field ) );

   if( !cfield_id.empty( ) )
      details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );
   if( !rfield_id.empty( ) )
      details.push_back( specification_detail( rfield_id, "rfield", e_model_element_type_field ) );
}

void transient_field_concat_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   dfield_id = reader.read_attribute( c_attribute_dfield_id );
   field1_id = reader.read_attribute( c_attribute_field1_id );
   field2_id = reader.read_attribute( c_attribute_field2_id );

   pclass_id = reader.read_opt_attribute( c_attribute_pclass_id );
   cfield_id = reader.read_opt_attribute( c_attribute_cfield_id );
   rfield_id = reader.read_opt_attribute( c_attribute_rfield_id );
   separator = reader.read_opt_attribute( c_attribute_separator );
   is_reverse = ( reader.read_opt_attribute( c_attribute_is_reverse ) == c_true );
   both_non_empty = ( reader.read_opt_attribute( c_attribute_non_empty ) == c_true );
}

void transient_field_concat_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_dfield_id, dfield_id );
   writer.write_attribute( c_attribute_field1_id, field1_id );
   writer.write_attribute( c_attribute_field2_id, field2_id );

   writer.write_opt_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_opt_attribute( c_attribute_cfield_id, cfield_id );
   writer.write_opt_attribute( c_attribute_rfield_id, rfield_id );
   writer.write_opt_attribute( c_attribute_separator, separator );
   writer.write_opt_attribute( c_attribute_is_reverse, is_reverse ? c_true : "" );
   writer.write_opt_attribute( c_attribute_non_empty, both_non_empty ? c_true : "" );
}

void transient_field_concat_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );

   string dfield_name = get_field_name_for_id( m, class_name, dfield_id );
   string field1_name = get_field_name_for_id( m, class_name, field1_id );
   string field2_name = get_field_name_for_id( m, class_name, field2_id );

   string pclass_name, cfield_name, rfield_name;
   if( !pclass_id.empty( ) )
   {
      pclass_name = get_class_name_for_id( m, pclass_id );
      cfield_name = get_field_name_for_id( m, pclass_name, cfield_id );

      if( !rfield_id.empty( ) )
         rfield_name = get_field_name_for_id( m, pclass_name, rfield_id );
   }

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_dfield, dfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_field1, field1_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_field2, field2_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_cfield, cfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_rfield, rfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_separator, separator ) );
   spec_data.data_pairs.push_back( make_pair( c_data_reverse, is_reverse ? c_true : "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_non_empty, both_non_empty ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( "top_level", "" ) );
}

string transient_field_concat_specification::static_class_name( ) { return "transient_field_concat"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, transient_field_concat_specification, static_class_name );

struct transient_field_from_file_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string suffix;
};

void transient_field_from_file_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'transient_field_from_file' specification" );

   string class_id;
   string field_id;
   string sfield_id;

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_sfield_name( args[ 2 ] );

   if( args.size( ) > 3 )
      suffix = args[ 3 ];

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   sfield_id = get_field_id_for_name( m, arg_class_name, arg_sfield_name );
   if( sfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_sfield_name + "' for class '" + arg_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
   details.push_back( specification_detail( sfield_id, "sfield", e_model_element_type_field ) );
}

void transient_field_from_file_specification::read_data( sio_reader& reader )
{
   suffix = reader.read_opt_attribute( c_attribute_suffix );
}

void transient_field_from_file_specification::write_data( sio_writer& writer ) const
{
   writer.write_opt_attribute( c_attribute_suffix, suffix );
}

void transient_field_from_file_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   spec_data.data_pairs.push_back( make_pair( c_data_suffix, suffix ) );
}

string transient_field_from_file_specification::static_class_name( ) { return "transient_field_from_file"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, transient_field_from_file_specification, static_class_name );

struct unactionable_field_value_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string value;
};

void unactionable_field_value_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "unexpected number of args < 3 for 'unactionable_field_value' specification" );

   string class_id;
   string field_id;

   string arg_class_name( args[ 0 ] );
   string arg_field_name( args[ 1 ] );
   string arg_value( args[ 2 ] );

   class_id = get_class_id_for_name( m, arg_class_name );

   field_id = get_field_id_for_name( m, arg_class_name, arg_field_name );
   if( field_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_field_name + "' for class '" + arg_class_name + "'" );

   value = arg_value;

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( field_id, "field", e_model_element_type_field ) );
}

void unactionable_field_value_specification::read_data( sio_reader& reader )
{
   value = reader.read_opt_attribute( c_attribute_value );
}

void unactionable_field_value_specification::write_data( sio_writer& writer ) const
{
   writer.write_opt_attribute( c_attribute_value, value );
}

void unactionable_field_value_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   spec_data.data_pairs.push_back( make_pair( c_data_value, value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_pfield, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_evalue, "" ) );
   spec_data.data_pairs.push_back( make_pair( c_data_not_eq, "" ) );
}

string unactionable_field_value_specification::static_class_name( ) { return "unactionable_field_value"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, unactionable_field_value_specification, static_class_name );

struct update_children_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string cclass_id;

   string cfield_id;
   string tfield_id;
};

void update_children_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 2 )
      throw runtime_error( "unexpected number of args < 2 for 'update_children' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_cclass_info( args[ 1 ] );

   string arg_tfield_name;
   if( args.size( ) > 2 )
      arg_tfield_name = args[ 2 ];

   string::size_type pos = arg_cclass_info.find( ':' );
   string cclass_name( arg_cclass_info.substr( 0, pos ) );

   string cfield_name;
   if( pos != string::npos )
      cfield_name = arg_cclass_info.substr( pos + 1 );

   class_id = get_class_id_for_name( m, arg_class_name );
   cclass_id = get_class_id_for_name( m, cclass_name );

   if( !cfield_name.empty( ) )
   {
      string cfield_type;
      cfield_id = get_field_id_for_name( m, cclass_name, cfield_name, &cfield_type, true );

      if( cfield_id.empty( ) )
         throw runtime_error( "unknown field '" + cfield_name + "' for class '" + cclass_name + "'" );

      string cfield_class_name( get_class_name_from_field_type( m, cclass_name, cfield_name, cfield_type ) );

      if( cfield_class_name != arg_class_name )
         throw runtime_error( "child field '" + cfield_name + "' is not class '" + arg_class_name + "'" );
   }

   if( !arg_tfield_name.empty( ) )
   {
      tfield_id = get_field_id_for_name( m, arg_class_name, arg_tfield_name );

      if( tfield_id.empty( ) )
         throw runtime_error( "unknown field '" + arg_tfield_name + "' for class '" + arg_class_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( cclass_id != class_id )
      details.push_back( specification_detail( cclass_id, "cclass", e_model_element_type_class ) );

   if( !cfield_id.empty( ) )
      details.push_back( specification_detail( cfield_id, "cfield", e_model_element_type_field ) );

   if( !tfield_id.empty( ) )
      details.push_back( specification_detail( tfield_id, "tfield", e_model_element_type_field ) );
}

void update_children_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   cclass_id = reader.read_attribute( c_attribute_cclass_id );

   cfield_id = reader.read_opt_attribute( c_attribute_cfield_id );
   tfield_id = reader.read_opt_attribute( c_attribute_tfield_id );
}

void update_children_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_cclass_id, cclass_id );

   writer.write_opt_attribute( c_attribute_cfield_id, cfield_id );
   writer.write_opt_attribute( c_attribute_tfield_id, tfield_id );
}

void update_children_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   string cclass_name = get_class_name_for_id( m, cclass_id );

   string cfield_name;
   if( !cfield_id.empty( ) )
   {
      cfield_name = get_field_name_for_id( m, cclass_name, cfield_id );
      if( cclass_id != class_id )
         cfield_name += "_{" + cclass_name + "}";
      else
      {
         string::size_type pos = cfield_name.find( cclass_name );
         if( pos != string::npos )
            cfield_name = cfield_name.substr( 0, pos ) + "{" + cclass_name + "}";
      }
      cfield_name = m.determine_child_rel_suffix( cfield_name );
   }

   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );
   // KLUDGE: Assumes class name is the child name.
   spec_data.data_pairs.push_back( make_pair( c_data_child, cfield_name.empty( ) ? cclass_name : cfield_name ) );

   string tfield_name;
   if( !tfield_id.empty( ) )
      tfield_name = get_field_name_for_id( m, class_name, tfield_id );

   spec_data.data_pairs.push_back( make_pair( c_data_tfield, tfield_name ) );

   spec_data.data_pairs.push_back( make_pair( "tvalue", "" ) );
   spec_data.data_pairs.push_back( make_pair( "not_var", "" ) );
   spec_data.data_pairs.push_back( make_pair( "procedure", "" ) );
   spec_data.data_pairs.push_back( make_pair( "set_loopvar", "" ) );
}

string update_children_specification::static_class_name( ) { return "update_children"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, update_children_specification, static_class_name );

struct update_parent_cascade_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string pclass_id;

   string pfield_id;

   string fkfield_id;
   string test_value;

   bool is_grand_parent;
};

void update_parent_cascade_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 2 )
      throw runtime_error( "unexpected number of args < 2 for 'update_parent_cascade' specification" );

   string arg_class_name( args[ 0 ] );
   string arg_pfield_name( args[ 1 ] );

   string fk_field_info;
   is_grand_parent = false;

   for( size_t arg = 2; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg == c_arg_grand_parent )
         is_grand_parent = true;
      else if( fk_field_info.empty( ) )
         fk_field_info = next_arg;
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for 'update_parent_cascade' specification" );
   }

   class_id = get_class_id_for_name( m, arg_class_name );

   string pfield_type;
   pfield_id = get_field_id_for_name( m, arg_class_name, arg_pfield_name, &pfield_type, true );
   if( pfield_id.empty( ) )
      throw runtime_error( "unknown field '" + arg_pfield_name + "' for class '" + arg_class_name + "'" );

   string pclass_name( get_class_name_from_field_type( m, arg_class_name, arg_pfield_name, pfield_type ) );

   pclass_id = get_class_id_for_name( m, pclass_name );

   if( !fk_field_info.empty( ) )
   {
      string::size_type pos = fk_field_info.find( '=' );
      string fk_field_name( fk_field_info.substr( 0, pos ) );

      if( pos != string::npos )
         test_value = fk_field_info.substr( pos + 1 );

      fkfield_id = get_field_id_for_name( m, arg_class_name, fk_field_name, 0, true );
      if( fkfield_id.empty( ) )
         throw runtime_error( "unknown field '" + fk_field_name + "' for class '" + arg_class_name + "'" );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   if( pclass_id != class_id )
      details.push_back( specification_detail( pclass_id, "pclass", e_model_element_type_class ) );

   details.push_back( specification_detail( pfield_id, "pfield", e_model_element_type_field ) );
   if( !fkfield_id.empty( ) )
      details.push_back( specification_detail( fkfield_id, "fkfield", e_model_element_type_field ) );
}

void update_parent_cascade_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   pclass_id = reader.read_attribute( c_attribute_pclass_id );

   pfield_id = reader.read_attribute( c_attribute_pfield_id );

   fkfield_id = reader.read_opt_attribute( c_attribute_fkfield_id );
   test_value = reader.read_opt_attribute( c_attribute_test_value );
   is_grand_parent = ( reader.read_opt_attribute( c_attribute_is_grand_parent ) == c_true );
}

void update_parent_cascade_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_pclass_id, pclass_id );

   writer.write_attribute( c_attribute_pfield_id, pfield_id );

   writer.write_opt_attribute( c_attribute_fkfield_id, fkfield_id );
   writer.write_opt_attribute( c_attribute_test_value, test_value );
   writer.write_opt_attribute( c_attribute_is_grand_parent, is_grand_parent ? c_true : "" );
}

void update_parent_cascade_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   string pclass_name = get_class_name_for_id( m, pclass_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pclass, pclass_name ) );

   string pfield_name = get_field_name_for_id( m, class_name, pfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_name ) );

   string fkfield_name;
   if( !fkfield_id.empty( ) )
      fkfield_name = get_field_name_for_id( m, class_name, fkfield_id );
   spec_data.data_pairs.push_back( make_pair( c_data_fkfield, fkfield_name ) );
   spec_data.data_pairs.push_back( make_pair( c_data_tvalue, test_value ) );

   spec_data.data_pairs.push_back( make_pair( c_data_grand_parent, is_grand_parent ? c_true : "" ) );

   spec_data.data_pairs.push_back( make_pair( c_data_after_store, "" ) );
}

string update_parent_cascade_specification::static_class_name( ) { return "update_parent_cascade"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, update_parent_cascade_specification, static_class_name );

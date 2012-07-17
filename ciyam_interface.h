// Copyright (c) 2011
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifndef CIYAM_INTERFACE_H
#  define CIYAM_INTERFACE_H

const int c_timeout_seconds = 600;

const int c_default_row_limit = 10;
const int c_default_login_days = 0;
const int c_default_notes_cols = 100;
const int c_default_notes_rmax = 40;
const int c_default_notes_rmin = 6;
const int c_default_notes_limit = 1000;
const int c_default_notes_trunc = 15;
const int c_default_print_limit = 4000;
const int c_default_image_width = 100;
const int c_default_image_height = 75;

const bool c_default_encrypt_data = false;
const bool c_default_checkbox_bools = false;

const size_t c_default_filesize_limit = 0;

const char* const c_default_int_mask = "-##########";
const char* const c_default_numeric_mask = "-##############.#####";

const char* const c_extkey = "@extkey";

const char* const c_fcgi_sio = "fcgi.sio";
const char* const c_fcgi_sio_ext = ".fcgi.sio";

const char* const c_tmp_directory = "tmp";
const char* const c_files_directory = "files";

const char* const c_true = "true";
const char* const c_false = "false";

const char* const c_true_value = "1";
const char* const c_false_value = "0";

const char* const c_key_field = "@key";
const char* const c_row_field = "@row";

const char* const c_new_record = " ";
const char* const c_new_session = "new_session";

const char* const c_anon_user_key = "anon";
const char* const c_admin_user_key = "admin";

const char* const c_list_prefix = "list";
const char* const c_hash_suffix = "hash";
const char* const c_info_suffix = "info";
const char* const c_prnt_suffix = "prnt";
const char* const c_rest_suffix = "rest";
const char* const c_show_prefix = "show";
const char* const c_sort_suffix = "sort";
const char* const c_srch_suffix = "srch";

const char* const c_act_del = "del";
const char* const c_act_cont = "cont";
const char* const c_act_edit = "edit";
const char* const c_act_exec = "exec";
const char* const c_act_link = "link";
const char* const c_act_save = "save";
const char* const c_act_view = "view";
const char* const c_act_undo = "undo";
const char* const c_act_qlink = "qlink";

const char* const c_cmd_pwd = "pwd";
const char* const c_cmd_home = "home";
const char* const c_cmd_view = "view";
const char* const c_cmd_list = "list";
const char* const c_cmd_quit = "quit";
const char* const c_cmd_login = "login";
const char* const c_cmd_plist = "plist";
const char* const c_cmd_pview = "pview";
const char* const c_cmd_status = "status";
const char* const c_cmd_activate = "activate";

const char* const c_param_act = "act";
const char* const c_param_app = "app";
const char* const c_param_chk = "chk";
const char* const c_param_cls = "cls";
const char* const c_param_cmd = "cmd";
const char* const c_param_back = "back";
const char* const c_param_cont = "cont";
const char* const c_param_data = "data";
const char* const c_param_exec = "exec";
const char* const c_param_hash = "hash";
const char* const c_param_keep = "keep";
const char* const c_param_user = "user";
const char* const c_param_vtab = "vtab";
const char* const c_param_vtabc = "vtabc";
const char* const c_param_extra = "extra";
const char* const c_param_field = "field";
const char* const c_param_flags = "flags";
const char* const c_param_ident = "ident";
const char* const c_param_qlink = "qlink";
const char* const c_param_base64 = "base64";
const char* const c_param_bcount = "bcount";
const char* const c_param_chksum = "chksum";
const char* const c_param_dtmoff = "dtmoff";
const char* const c_param_gmtoff = "gmtoff";
const char* const c_param_newpwd = "newpwd";
const char* const c_param_module = "module";
const char* const c_param_utcdtm = "utcdtm";
const char* const c_param_hashval = "hashval";
const char* const c_param_newflds = "newflds";
const char* const c_param_newvals = "newvals";
const char* const c_param_scrollx = "scrollx";
const char* const c_param_scrolly = "scrolly";
const char* const c_param_session = "session";
const char* const c_param_special = "special";
const char* const c_param_listarg = "listarg";
const char* const c_param_listvar = "listvar";
const char* const c_param_uselect = "uselect";
const char* const c_param_findinfo = "findinfo";
const char* const c_param_listinfo = "listinfo";
const char* const c_param_listsort = "listsort";
const char* const c_param_listsrch = "listsrch";
const char* const c_param_tzoffcur = "tzoffcur";
const char* const c_param_username = "username";
const char* const c_param_password = "password";
const char* const c_param_fieldlist = "fieldlist";
const char* const c_param_listextra = "listextra";
const char* const c_param_uselextra = "uselextra";
const char* const c_param_userfetch = "userfetch";
const char* const c_param_quicklink = "quicklink";
const char* const c_param_keepchecks = "keepchecks";
const char* const c_param_persistent = "persistent";
const char* const c_param_extrafields = "extrafields";
const char* const c_param_extravalues = "extravalues";

const char* const c_response_okay = "(okay)";
const char* const c_response_error = "(error)";
const char* const c_response_okay_more = "(okay more)";

const char* const c_operation_link = "link";
const char* const c_operation_search = "search";
const char* const c_operation_select = "select";
const char* const c_operation_checked = "checked";
const char* const c_operation_rchecked = "rchecked";
const char* const c_operation_unchecked = "unchecked";
const char* const c_operation_restricted = "restricted";
const char* const c_operation_runchecked = "runchecked";
const char* const c_operation_select_child = "select_child";

const char* const c_field_type_bool = "bool";
const char* const c_field_type_date = "date";
const char* const c_field_type_tdate = "tdate";
const char* const c_field_type_udate = "udate";
const char* const c_field_type_time = "time";
const char* const c_field_type_mtime = "mtime";
const char* const c_field_type_ttime = "ttime";
const char* const c_field_type_datetime = "datetime";
const char* const c_field_type_date_time = "date_time";
const char* const c_field_type_tdatetime = "tdatetime";
const char* const c_field_type_int = "int";
const char* const c_field_type_numeric = "numeric";

const char* const c_field_extra_url = "url";
const char* const c_field_extra_uom = "uom";
const char* const c_field_extra_enum = "enum";
const char* const c_field_extra_file = "file";
const char* const c_field_extra_href = "href";
const char* const c_field_extra_html = "html";
const char* const c_field_extra_link = "link";
const char* const c_field_extra_mask = "mask";
const char* const c_field_extra_perm = "perm";
const char* const c_field_extra_text = "text";
const char* const c_field_extra_flink = "flink";
const char* const c_field_extra_image = "image";
const char* const c_field_extra_large = "large";
const char* const c_field_extra_notes = "notes";
const char* const c_field_extra_pmask = "pmask";
const char* const c_field_extra_range = "range";
const char* const c_field_extra_small = "small";
const char* const c_field_extra_hidden = "hidden";
const char* const c_field_extra_larger = "larger";
const char* const c_field_extra_mailto = "mailto";
const char* const c_field_extra_smaller = "smaller";
const char* const c_field_extra_filename = "filename";
const char* const c_field_extra_int_type = "int_type";
const char* const c_field_extra_non_link = "non_link";
const char* const c_field_extra_non_print = "non_print";
const char* const c_field_extra_transient = "transient";
const char* const c_field_extra_admin_link = "admin_link";
const char* const c_field_extra_admin_only = "admin_only";
const char* const c_field_extra_defcurrent = "defcurrent";
const char* const c_field_extra_full_width = "full_width";
const char* const c_field_extra_owner_link = "owner_link";
const char* const c_field_extra_owner_only = "owner_only";
const char* const c_field_extra_permission = "permission";
const char* const c_field_extra_print_only = "print_only";
const char* const c_field_extra_manual_link = "manual_link";
const char* const c_field_extra_non_fk_link = "non_fk_link";
const char* const c_field_extra_orientation = "orientation";
const char* const c_field_extra_print_total = "print_total";
const char* const c_field_extra_ignore_links = "ignore_links";
const char* const c_field_extra_non_prefixed = "non_prefixed";
const char* const c_field_extra_numeric_type = "numeric_type";
const char* const c_field_extra_print_summary = "print_summary";
const char* const c_field_extra_date_precision = "date_precision";
const char* const c_field_extra_defcurrentyear = "defcurrentyear";
const char* const c_field_extra_security_level = "security_level";
const char* const c_field_extra_time_precision = "time_precision";
const char* const c_field_extra_admin_owner_link = "admin_owner_link";
const char* const c_field_extra_admin_owner_only = "admin_owner_only";
const char* const c_field_extra_print_only_total = "print_only_total";
const char* const c_field_extra_text_search_title = "text_search_title";
const char* const c_field_extra_replace_underbars = "replace_underbars";
const char* const c_field_extra_is_effective_owner = "is_effective_owner";
const char* const c_field_extra_print_only_summary = "print_only_summary";

const char* const c_parent_extra_key = "@key";
const char* const c_parent_extra_key0 = "@key0";
const char* const c_parent_extra_key1 = "@key1";
const char* const c_parent_extra_key2 = "@key2";
const char* const c_parent_extra_key3 = "@key3";
const char* const c_parent_extra_key4 = "@key4";
const char* const c_parent_extra_key5 = "@key5";
const char* const c_parent_extra_key6 = "@key6";
const char* const c_parent_extra_key7 = "@key7";
const char* const c_parent_extra_key8 = "@key8";
const char* const c_parent_extra_key9 = "@key9";
const char* const c_parent_extra_sort = "@sort";
const char* const c_parent_extra_user = "@user";
const char* const c_parent_extra_view = "@view";
const char* const c_parent_extra_fkey0 = "@fkey0";
const char* const c_parent_extra_fkey1 = "@fkey1";
const char* const c_parent_extra_fkey2 = "@fkey2";
const char* const c_parent_extra_fkey3 = "@fkey3";
const char* const c_parent_extra_fkey4 = "@fkey4";
const char* const c_parent_extra_fkey5 = "@fkey5";
const char* const c_parent_extra_fkey6 = "@fkey6";
const char* const c_parent_extra_fkey7 = "@fkey7";
const char* const c_parent_extra_fkey8 = "@fkey8";
const char* const c_parent_extra_fkey9 = "@fkey9";
const char* const c_parent_extra_group = "@group";
const char* const c_parent_extra_other = "@other";
const char* const c_parent_extra_skey0 = "@skey0";
const char* const c_parent_extra_skey1 = "@skey1";
const char* const c_parent_extra_skey2 = "@skey2";
const char* const c_parent_extra_skey3 = "@skey3";
const char* const c_parent_extra_skey4 = "@skey4";
const char* const c_parent_extra_skey5 = "@skey5";
const char* const c_parent_extra_skey6 = "@skey6";
const char* const c_parent_extra_skey7 = "@skey7";
const char* const c_parent_extra_skey8 = "@skey8";
const char* const c_parent_extra_skey9 = "@skey9";
const char* const c_parent_extra_xself = "@xself";
const char* const c_parent_extra_parent = "@parent";
const char* const c_parent_extra_slevel = "@slevel";
const char* const c_parent_extra_xfkey0 = "@xfkey0";
const char* const c_parent_extra_xfkey1 = "@xfkey1";
const char* const c_parent_extra_xfkey2 = "@xfkey2";
const char* const c_parent_extra_xfkey3 = "@xfkey3";
const char* const c_parent_extra_xfkey4 = "@xfkey4";
const char* const c_parent_extra_xfkey5 = "@xfkey5";
const char* const c_parent_extra_xfkey6 = "@xfkey6";
const char* const c_parent_extra_xfkey7 = "@xfkey7";
const char* const c_parent_extra_xfkey8 = "@xfkey8";
const char* const c_parent_extra_xfkey9 = "@xfkey9";
const char* const c_parent_extra_xparent = "@xparent";
const char* const c_parent_extra_manuallink = "@manuallink";
const char* const c_parent_extra_permission = "@permission";
const char* const c_parent_extra_xselfadmin = "@xselfadmin";

const char* const c_modifier_effect_protect = "protect";
const char* const c_modifier_effect_relegate = "relegate";
const char* const c_modifier_effect_lowlight = "lowlight";
const char* const c_modifier_effect_lowlight1 = "lowlight1";
const char* const c_modifier_effect_highlight = "highlight";
const char* const c_modifier_effect_extralight = "extralight";
const char* const c_modifier_effect_highlight1 = "highlight1";
const char* const c_modifier_effect_extralight1 = "extralight1";

const char* const c_list_extra_find = "find";

const char* const c_list_type_home = "home";
const char* const c_list_type_user = "user";
const char* const c_list_type_admin = "admin";
const char* const c_list_type_child = "child";
const char* const c_list_type_group = "group";
const char* const c_list_type_nonuser = "nonuser";
const char* const c_list_type_nongroup = "nongroup";
const char* const c_list_type_standard = "standard";
const char* const c_list_type_no_access = "no_access";
const char* const c_list_type_user_child = "user_child";
const char* const c_list_type_child_admin = "child_admin";
const char* const c_list_type_child_owner = "child_owner";
const char* const c_list_type_child_admin_owner = "child_admin_owner";

const char* const c_list_type_extra_limit = "limit";
const char* const c_list_type_extra_fixed = "fixed";
const char* const c_list_type_extra_no_new = "no_new";
const char* const c_list_type_extra_search = "search";
const char* const c_list_type_extra_actions = "actions";
const char* const c_list_type_extra_no_erase = "no_erase";
const char* const c_list_type_extra_no_limit = "no_limit";
const char* const c_list_type_extra_no_print = "no_print";
const char* const c_list_type_extra_admin_new = "admin_new";
const char* const c_list_type_extra_owner_new = "owner_new";
const char* const c_list_type_extra_quick_link = "quick_link";
const char* const c_list_type_extra_admin_erase = "admin_erase";
const char* const c_list_type_extra_admin_print = "admin_print";
const char* const c_list_type_extra_owner_erase = "owner_erase";
const char* const c_list_type_extra_text_search = "text_search";
const char* const c_list_type_extra_show_security = "show_security";
const char* const c_list_type_extra_sort_no_limit = "sort_no_limit";
const char* const c_list_type_extra_print_no_trunc = "print_no_trunc";
const char* const c_list_type_extra_text_highlight = "text_highlight";
const char* const c_list_type_extra_admin_owner_new = "admin_owner_new";
const char* const c_list_type_extra_text_highlight1 = "text_highlight1";
const char* const c_list_type_extra_search_opt_limit = "search_opt_limit";
const char* const c_list_type_extra_admin_owner_erase = "admin_owner_erase";
const char* const c_list_type_extra_print_no_highlight = "print_no_highlight";
const char* const c_list_type_extra_ignore_parent_state = "ignore_parent_state";
const char* const c_list_type_extra_ignore_unactionable = "ignore_unactionable";
const char* const c_list_type_extra_new_if_default_other = "new_if_default_other";
const char* const c_list_type_extra_show_if_default_other = "show_if_default_other";
const char* const c_list_type_extra_erase_if_default_other = "erase_if_default_other";

const char* const c_list_field_extra_left = "left";
const char* const c_list_field_extra_right = "right";
const char* const c_list_field_extra_trunc = "trunc";
const char* const c_list_field_extra_center = "center";
const char* const c_list_field_extra_pstate = "pstate";
const char* const c_list_field_extra_exact_match = "exact_match";
const char* const c_list_field_extra_label_class = "label_class";
const char* const c_list_field_extra_omit_label = "omit_label";
const char* const c_list_field_extra_use_list_title = "use_list_title";

const char* const c_view_type_admin = "admin";
const char* const c_view_type_print = "print";
const char* const c_view_type_standard = "standard";
const char* const c_view_type_admin_print = "admin_print";

const char* const c_view_type_extra_vtabc = "vtabc";
const char* const c_view_type_extra_no_edit = "no_edit";
const char* const c_view_type_extra_can_copy = "can_copy";
const char* const c_view_type_extra_no_print = "no_print";
const char* const c_view_type_extra_auto_back = "auto_back";
const char* const c_view_type_extra_user_info = "user_info";
const char* const c_view_type_extra_owner_edit = "owner_edit";
const char* const c_view_type_extra_admin_edit = "admin_edit";
const char* const c_view_type_extra_admin_owner_edit = "admin_owner_edit";
const char* const c_view_type_extra_print_no_highlight = "print_no_highlight";
const char* const c_view_type_extra_static_instance_key = "static_instance_key";
const char* const c_view_type_extra_use_first_row_as_header = "first_row_as_header";

const char* const c_view_field_extra_key0 = "key0";
const char* const c_view_field_extra_key1 = "key1";
const char* const c_view_field_extra_key2 = "key2";
const char* const c_view_field_extra_key3 = "key3";
const char* const c_view_field_extra_key4 = "key4";
const char* const c_view_field_extra_key5 = "key5";
const char* const c_view_field_extra_key6 = "key6";
const char* const c_view_field_extra_key7 = "key7";
const char* const c_view_field_extra_key8 = "key8";
const char* const c_view_field_extra_key9 = "key9";
const char* const c_view_field_extra_show = "show";
const char* const c_view_field_extra_fkey0 = "fkey0";
const char* const c_view_field_extra_fkey1 = "fkey1";
const char* const c_view_field_extra_fkey2 = "fkey2";
const char* const c_view_field_extra_fkey3 = "fkey3";
const char* const c_view_field_extra_fkey4 = "fkey4";
const char* const c_view_field_extra_fkey5 = "fkey5";
const char* const c_view_field_extra_fkey6 = "fkey6";
const char* const c_view_field_extra_fkey7 = "fkey7";
const char* const c_view_field_extra_fkey8 = "fkey8";
const char* const c_view_field_extra_fkey9 = "fkey9";
const char* const c_view_field_extra_force = "force";
const char* const c_view_field_extra_upper = "upper";
const char* const c_view_field_extra_estart = "estart";
const char* const c_view_field_extra_forced = "forced";
const char* const c_view_field_extra_actions = "actions";
const char* const c_view_field_extra_efinish = "efinish";
const char* const c_view_field_extra_no_edit = "no_edit";
const char* const c_view_field_extra_non_new = "non_new";
const char* const c_view_field_extra_vextra1 = "vextra1";
const char* const c_view_field_extra_vextra2 = "vextra2";
const char* const c_view_field_extra_new_edit = "new_edit";
const char* const c_view_field_extra_new_only = "new_only";
const char* const c_view_field_extra_non_view = "non_view";
const char* const c_view_field_extra_password = "password";
const char* const c_view_field_extra_edit_edit = "edit_edit";
const char* const c_view_field_extra_edit_only = "edit_only";
const char* const c_view_field_extra_epassword = "epassword";
const char* const c_view_field_extra_hpassword = "hpassword";
const char* const c_view_field_extra_new_value = "new_value";
const char* const c_view_field_extra_view_only = "view_only";
const char* const c_view_field_extra_admin_edit = "admin_edit";
const char* const c_view_field_extra_owner_edit = "owner_edit";
const char* const c_view_field_extra_owner_only = "owner_only";
const char* const c_view_field_extra_quick_link = "quick_link";
const char* const c_view_field_extra_owning_user = "owning_user";
const char* const c_view_field_extra_hpassword_salt = "hpassword_salt";
const char* const c_view_field_extra_always_editable = "always_editable";
const char* const c_view_field_extra_create_datetime = "create_datetime";
const char* const c_view_field_extra_create_user_key = "create_user_key";
const char* const c_view_field_extra_modify_datetime = "modify_datetime";
const char* const c_view_field_extra_modify_user_key = "modify_user_key";
const char* const c_view_field_extra_admin_owner_edit = "admin_owner_edit";
const char* const c_view_field_extra_admin_owner_only = "admin_owner_only";
const char* const c_view_field_extra_trigger_protected = "trigger_protected";
const char* const c_view_field_extra_trigger_relegated = "trigger_relegated";

const char* const c_url_opt_use_checksum = "use_checksum";

const char* const c_menu_opt_show_inaccessible_modules = "show_inaccessible_modules";

const char* const c_list_print_opt_show_checks = "show_checks";
const char* const c_list_print_opt_show_numbers = "show_numbers";

const char* const c_login_opt_allow_multiple = "allow_multiple";
const char* const c_login_opt_allow_switching = "allow_switching";

// NOTE: The following need to appear in "ciyam_interface.txt" as translatable strings.
const char* const c_display_or = "Or";
const char* const c_display_and = "And";
const char* const c_display_any = "Any";
const char* const c_display_new = "New";
const char* const c_display_back = "Back";
const char* const c_display_only = "only";
const char* const c_display_none = "None";
const char* const c_display_true = "True";
const char* const c_display_home = "Home";
const char* const c_display_help = "Help";
const char* const c_display_next = "Next";
const char* const c_display_prev = "Previous";
const char* const c_display_error = "Error";
const char* const c_display_false = "False";
const char* const c_display_print = "Print";
const char* const c_display_reset = "Reset";
const char* const c_display_image = "Image";
const char* const c_display_login = "Login";
const char* const c_display_total = "Total";
const char* const c_display_cancel = "Cancel";
const char* const c_display_choose = "Choose";
const char* const c_display_search = "Search";
const char* const c_display_attach = "Attach";
const char* const c_display_delete = "Delete";
const char* const c_display_logout = "Logout";
const char* const c_display_records = "records";
const char* const c_display_unknown = "Unknown";
const char* const c_display_confirm = "Confirm";
const char* const c_display_user_id = "User Id";
const char* const c_display_contains = "Contains";
const char* const c_display_has_file = "Has file ({@ext}).";
const char* const c_display_has_file_parm_ext = "{@ext}";
const char* const c_display_includes = "Includes";
const char* const c_display_password = "Password";
const char* const c_display_standard = "Standard";
const char* const c_display_subtotal = "Subtotal";
const char* const c_display_assign_to = "Assign to";
const char* const c_display_see_below = "(see below)";
const char* const c_display_field_name = "Field Name";
const char* const c_display_is_between = "Is Between";
const char* const c_display_login_info = "Login Info";
const char* const c_display_new_record = "New Record";
const char* const c_display_welcome_to = "Welcome to";
const char* const c_display_cancel_edit = "Cancel Edit";
const char* const c_display_create_copy = "Create Copy";
const char* const c_display_field_value = "Field Value";
const char* const c_display_is_equal_to = "Is Equal To";
const char* const c_display_pdf_version = "PDF Version";
const char* const c_display_pick_a_date = "Pick a date";
const char* const c_display_search_type = "Search Type";
const char* const c_display_edit_details = "Edit Details";
const char* const c_display_logged_in_as = "Logged in as";
const char* const c_display_new_password = "New Password";
const char* const c_display_old_password = "Old Password";
const char* const c_display_save_details = "Save Details";
const char* const c_display_search_field = "Search Field";
const char* const c_display_search_values = "Search Values";
const char* const c_display_add_quick_link = "Add Quick Link";
const char* const c_display_any_text_field = "Any Text Field";
const char* const c_display_change_password = "Change Password";
const char* const c_display_characters_left = "characters left";
const char* const c_display_verify_password = "Verify Password";
const char* const c_display_record_not_found = "Record not found";
const char* const c_display_switch_to_module = "Switch to module";
const char* const c_display_permission_denied = "Permission denied";
const char* const c_display_maximum_characters = "Maximum characters";
const char* const c_display_automatically_login = "Automatically login";
const char* const c_display_click_here_to_login = "Click {@href}here{#href} to login.";
const char* const c_display_click_here_to_login_parm_href = "{@href}";
const char* const c_display_proceed_with_action = "Proceed with {@label}?";
const char* const c_display_proceed_with_action_parm_label = "{@label}";
const char* const c_display_verify_new_password = "Verify New Password";
const char* const c_display_remove_attached_file = "Remove Attached File";
const char* const c_display_show_search_criteria = "Show Search Criteria";
const char* const c_display_click_here_to_go_back = "Click {@href}here{#href} to go back.";
const char* const c_display_click_here_to_go_back_parm_href = "{@href}";
const char* const c_display_click_here_to_view_file = "Click {@href}here{#href} to view file ({@ext}).";
const char* const c_display_click_here_to_view_file_parm_ext = "{@ext}";
const char* const c_display_click_here_to_view_file_parm_href = "{@href}";
const char* const c_display_click_here_to_view_html = "Click {@href}here{#href} to view HTML.";
const char* const c_display_click_here_to_view_html_parm_href = "{@href}";
const char* const c_display_create_multiple_records = "Create Multiple Records";
const char* const c_display_output_has_been_limited = "output has been limited";
const char* const c_display_unexpected_server_error = "Unexpected Server Error";
const char* const c_display_select_local_upload_file = "Select a local file and then click {@attach} to upload it.";
const char* const c_display_select_local_upload_file_parm_attach = "{@attach}";
const char* const c_display_you_have_been_logged_out = "You have been logged out.";
const char* const c_display_password_has_been_changed = "Password has been changed!";
const char* const c_display_unknown_or_invalid_user_id = "Unknown or invalid user credentials.";
const char* const c_display_upload_max_length_exceeded = "Maximum upload size ({@size}) exceeded.";
const char* const c_display_upload_max_length_exceeded_parm_size = "{@size}";
const char* const c_display_you_are_currently_logged_in = "You are already currently logged in.";
const char* const c_display_application_server_unavailable = "Application server is not currently available.";
const char* const c_display_max_concurrent_users_logged_in = "Maximum concurrent users already logged in. Please try again later.";
const char* const c_display_click_the_printer_icon_to_print = "Click the printer icon to print then click {@href}here{#href} to go back.";
const char* const c_display_click_the_printer_icon_to_print_parm_href = "{@href}";
const char* const c_display_session_logged_into_other_module = "Cannot access this module from this session. Click {@href}here{#href} to go back.";
const char* const c_display_session_logged_into_other_module_parm_href = "{@href}";
const char* const c_display_your_session_has_been_terminated = "Your session has been terminated by the server";
const char* const c_display_account_has_already_been_activated = "Account has already been activated - please login with your user id and password.";
const char* const c_display_password_must_not_be_the_same_as_your_user_id = "For security reasons your password must not be the same as your user id.";

// NOTE: The following strings are used by "ciyam_interface.js".
const char* const c_display_timeout = "Timeout occurred.";
const char* const c_display_invalid_date = "Invalid date.";
const char* const c_display_invalid_time = "Invalid time.";
const char* const c_display_incorrect_date_format = "Incorrect date format.";
const char* const c_display_incorrect_time_format = "Incorrect time format.";
const char* const c_display_enter_required_values = "Enter required values.";
const char* const c_display_incorrect_bytes_format = "Incorrect bytes format.";
const char* const c_display_select_one_or_more_items = "Select one or more items to perform this operation on.";
const char* const c_display_incorrect_integer_format = "Incorrect integer format.";
const char* const c_display_incorrect_numeric_format = "Incorrect numeric format.";
const char* const c_display_incorrect_duration_format = "Incorrect duration format.";
const char* const c_display_maximum_text_limit_reached = "Maximum text limit reached.";
const char* const c_display_password_verification_incorrect = "Password verification incorrect.";
const char* const c_display_your_session_is_about_to_expire = "Your session is about to expire. Click OK to continue.";
const char* const c_display_selected_records_will_be_removed = "Selected record(s) will be permanently removed. Click OK to confirm.";

#endif


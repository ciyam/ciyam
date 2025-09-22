// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_STRINGS_H
#  define CIYAM_STRINGS_H

const char* const c_str_or = "or";
const char* const c_str_and = "and";
const char* const c_str_page = "Page";
const char* const c_str_true = "True";
const char* const c_str_false = "False";
const char* const c_str_total = "Total";
const char* const c_str_footer = "Copyright (c) 2012-2025 CIYAM Developers";
const char* const c_str_records = "record(s)";
const char* const c_str_syncing = "Syncing";
const char* const c_str_subtotal = "Subtotal";
const char* const c_str_preparing = "Preparing";
const char* const c_str_decrypting = "Decrypting";
const char* const c_str_encrypting = "Encrypting";
const char* const c_str_at_height_suffix = " at height";
const char* const c_str_module_is_loaded = "Module '{@module}' is already loaded.";
const char* const c_str_module_is_loaded_module = "{@module}";
const char* const c_str_module_not_loaded = "Module '{@module}' is not currently loaded.";
const char* const c_str_module_not_loaded_module = "{@module}";
const char* const c_str_module_not_exists = "Module '{@module}' does not appear to exist.";
const char* const c_str_module_not_exists_module = "{@module}";
const char* const c_str_module_depends_reqd = "Module '{@module}' requires external dependencies to be loaded first.";
const char* const c_str_module_depends_reqd_module = "{@module}";
const char* const c_str_key_invalid = "Key '{@key}' contains invalid characters.";
const char* const c_str_key_invalid_key = "{@key}";
const char* const c_str_record_exists = "This {@class} already exists or is not able to be created.";
const char* const c_str_record_exists_class = "{@class}";
const char* const c_str_cannot_update = "Update not permitted for this {@class} record.";
const char* const c_str_cannot_update_class = "{@class}";
const char* const c_str_cannot_destroy = "Destroy not permitted for this record.";
const char* const c_str_cannot_destroy_class = "{@class}";
const char* const c_str_index_duplicate = "This {@field} [val: {@value}] is already being used by another {@class} record.";
const char* const c_str_index_duplicate_field = "{@field}";
const char* const c_str_index_duplicate_value = "{@value}";
const char* const c_str_index_duplicate_class = "{@class}";
const char* const c_str_processed_items = "Processed {@num} items";
const char* const c_str_processed_items_num = "{@num}";
const char* const c_str_currently_locked = "Currently locked (please try again later).";
const char* const c_str_record_not_found = "{@class} record '{@key}' was not found.";
const char* const c_str_record_not_found_class = "{@class}";
const char* const c_str_record_not_found_key = "{@key}";
const char* const c_str_version_mismatch = "Version mismatch (found {@found} but expected {@expected}).";
const char* const c_str_version_mismatch_found = "{@found}";
const char* const c_str_version_mismatch_expected = "{@expected}";
const char* const c_str_permission_denied = "Permission denied.";
const char* const c_str_processing_prefix = "Processing: ";
const char* const c_str_field_is_incorrect = "{@field} is incorrect.";
const char* const c_str_field_is_incorrect_field = "{@field}";
const char* const c_str_record_constrained = "This record cannot be destroyed as it is being used by one or more {@class} records.";
const char* const c_str_record_constrained_class = "{@class}";
const char* const c_str_current_height_prefix = "Currently at height ";
const char* const c_str_failed_connect_to_peer = "Failed trying to connect to '{@host}' using port number {@port}.";
const char* const c_str_failed_connect_to_peer_host = "{@host}";
const char* const c_str_failed_connect_to_peer_port = "{@port}";
const char* const c_str_invalid_peer_handshake = "Invalid peer handshake.";
const char* const c_str_incorrect_shared_secret = "Found invalid/corrupt peer data (incorrect shared secret?).";
const char* const c_str_invalid_time_difference = "Invalid time difference (check system clocks).";
const char* const c_str_mapping_at_height_prefix = "Mapping at height ";
const char* const c_str_polling_at_height_prefix = "Polling at height ";
const char* const c_str_syncing_at_height_prefix = "Syncing at height ";
const char* const c_str_waiting_for_verification = "Waiting for peer verification.";
const char* const c_str_checking_at_height_prefix = "Checking at height ";
const char* const c_str_mapping_for_height_prefix = "Mapping for height ";
const char* const c_str_syncing_for_height_prefix = "Syncing for height ";
const char* const c_str_unable_to_start_peerchain = "Unable to start peerchain using port number {@port}.";
const char* const c_str_unable_to_start_peerchain_port = "{@port}";
const char* const c_str_waiting_for_peer_hub_sync = "Waiting for peer hub sync";
const char* const c_str_host_address_not_permitted = "Host '{@host}' address {@address} is not permitted.";
const char* const c_str_host_address_not_permitted_host = "{@host}";
const char* const c_str_host_address_not_permitted_address = "{@address}";
const char* const c_str_copy_password_to_clipboard = "Copy password to clipboard: {@value}";
const char* const c_str_copy_password_to_clipboard_value = "{@value}";
const char* const c_str_peer_terminated_connection = "Peer has unexpectedly terminated this connection.";
const char* const c_str_timeout_connecting_to_peer = "Timeout occurred trying to connect to peer.";
const char* const c_str_unexpected_blockchain_fork = "Unexpected blockchain fork detected.";
const char* const c_str_unable_to_determine_address = "Unable to determine address for domain name '{@domain}'.";
const char* const c_str_unable_to_determine_address_domain = "{@domain}";
const char* const c_str_external_service_unavailable = "External service {@symbol} does not appear to be currently available.";
const char* const c_str_external_service_unavailable_symbol = "{@symbol}";
const char* const c_str_found_incorrect_hub_identity = "Found incorrect hub identity.";
const char* const c_str_timed_out_connecting_to_peer = "Timed out trying to connect to '{@host}' using port number {@port}.";
const char* const c_str_timed_out_connecting_to_peer_host = "{@host}";
const char* const c_str_timed_out_connecting_to_peer_port = "{@port}";
const char* const c_str_files_area_item_size_mismatch = "Unexpected files area item size mismatch.";
const char* const c_str_requested_peerchain_unavailable = "Requested peerchain is not available.";
const char* const c_str_incorrect_or_unsupported_peer_type = "Incorrect or unsupported Peer Type.";
const char* const c_str_timed_out_waiting_for_verification = "Timed out waiting for peer verification.";

#endif


// Copyright (c) 2023-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CHANNELS_H
#  define CIYAM_CHANNELS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#  endif

#  include "ptypes.h"
#  include "ciyam_core.h"

class mutex;

mutex& get_mutex_for_ciyam_channels( );

class class_base;

struct channel_lock
{
   channel_lock( const class_base& cb, const std::string& identity );
   ~channel_lock( );

   std::string identity;
};

int64_t channel_height_fetched( const std::string& identity );

void storage_channel_document_submit( const std::string& file_path );
void storage_channel_document_unsubmit( const std::string& file_path );

void storage_channel_document_restore( const std::string& identity_path );

bool storage_channel_document_ignoring( const std::string& file_path );
bool storage_channel_document_submitting( const std::string& file_path );

int64_t storage_channel_received_height( const std::string& identity );
int64_t storage_channel_submitted_height( const std::string& identity );

void storage_channel_list( std::ostream& os );

void storage_channel_create( const char* p_identity = 0, const char* p_channel_information = 0 );
void storage_channel_destroy( const char* p_identity = 0 );

void storage_channel_update( const char* p_identity = 0, const char* p_channel_information = 0 );

std::string storage_channel_documents(
 const std::string& identity, bool get_height = false, bool for_fetched = false );

void storage_channel_documents_open( const char* p_identity = 0 );
void storage_channel_documents_close( const char* p_identity = 0 );

bool storage_channel_documents_marked( const std::string& identity );
bool storage_channel_documents_opened( const std::string& identity );

std::string storage_channel_documents_update( const std::string& identity, bool submitted = false );
std::string storage_channel_documents_prepare( const std::string& identity );

void storage_channel_documents_cancel_pending( const char* p_identity = 0 );

inline void storage_channel_documents_cancel_pending( const std::string& identity )
{
   storage_channel_documents_cancel_pending( identity.c_str( ) );
}

void storage_channel_documents_reject_waiting(
 const char* p_identity = 0, const char* p_file_list_for_rejection = 0 );

void storage_channel_documents_reject_reviewing(
 const char* p_identity = 0, const char* p_file_list_for_rejection = 0 );

void storage_channel_documents_approve_waiting(
 const char* p_identity = 0, const char* p_file_list_for_approval = 0 );

void storage_channel_documents_approve_reviewing(
 const char* p_identity = 0, const char* p_file_list_for_approval = 0 );

enum channel_documents_type
{
   e_channel_documents_type_pending,
   e_channel_documents_type_waiting,
   e_channel_documents_type_retrieved,
   e_channel_documents_type_reviewing,
   e_channel_documents_type_submitting
};

std::string storage_channel_documents_specific( const std::string& identity,
 channel_documents_type type = e_channel_documents_type_submitting, bool include_sizes = true );

inline std::string storage_channel_documents_fetched( const std::string& identity )
{
   return storage_channel_documents( identity, true, true );
}

inline std::string storage_channel_documents_submitted( const std::string& identity )
{
   return storage_channel_documents( identity, true, false );
}

inline std::string storage_channel_documents_pending( const std::string& identity )
{
   return storage_channel_documents_specific( identity, e_channel_documents_type_pending );
}

inline std::string storage_channel_documents_waiting( const std::string& identity )
{
   return storage_channel_documents_specific( identity, e_channel_documents_type_waiting );
}

inline std::string storage_channel_documents_retrieved( const std::string& identity )
{
   return storage_channel_documents_specific( identity, e_channel_documents_type_retrieved );
}

inline std::string storage_channel_documents_reviewing( const std::string& identity )
{
   return storage_channel_documents_specific( identity, e_channel_documents_type_reviewing );
}

inline std::string storage_channel_documents_submitting( const std::string& identity )
{
   return storage_channel_documents_specific( identity, e_channel_documents_type_submitting );
}

void list_datachains( std::ostream& os );
void list_datachains( std::vector< std::string >& datachains );

bool has_datachain( const std::string& identity );

std::string get_datachain_info( const std::string& identity, size_t* p_data_type = 0, size_t* p_height = 0 );

void create_datachain_info( const std::string& identity, size_t data_type );
void remove_datachain_info( const std::string& identity );

void set_datachain_height( const std::string& identity, size_t new_height );

bool is_linked_to_datachain( const std::string& channel, const std::string& identity );

void link_channel_to_datachain( const std::string& channel, const std::string& identity );
void unlink_channel_from_datachain( const std::string& channel, const std::string& identity );

#endif

// Copyright (c) 2023 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CHANNELS_H
#  define CIYAM_CHANNELS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "macros.h"
#  include "ptypes.h"
#  include "ciyam_common.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class class_base;

void CIYAM_BASE_DECL_SPEC lock_peer_channel( const class_base& cb, const std::string& identity );

int64_t CIYAM_BASE_DECL_SPEC channel_height_fetched( const std::string& identity );

void CIYAM_BASE_DECL_SPEC storage_channel_document_submit( const std::string& file_path );
void CIYAM_BASE_DECL_SPEC storage_channel_document_unsubmit( const std::string& file_path );

void CIYAM_BASE_DECL_SPEC storage_channel_document_restore( const std::string& identity_path );

bool CIYAM_BASE_DECL_SPEC storage_channel_document_ignoring( const std::string& file_path );
bool CIYAM_BASE_DECL_SPEC storage_channel_document_submitting( const std::string& file_path );

int64_t CIYAM_BASE_DECL_SPEC storage_channel_received_height( const std::string& identity );
int64_t CIYAM_BASE_DECL_SPEC storage_channel_submitted_height( const std::string& identity );

void CIYAM_BASE_DECL_SPEC storage_channel_create( const char* p_identity = 0, const char* p_channel_information = 0 );
void CIYAM_BASE_DECL_SPEC storage_channel_destroy( const char* p_identity = 0 );

void CIYAM_BASE_DECL_SPEC storage_channel_update( );

std::string CIYAM_BASE_DECL_SPEC storage_channel_documents(
 const std::string& identity, bool height = false, bool fetched = false );

void CIYAM_BASE_DECL_SPEC storage_channel_documents_open( const char* p_identity = 0 );
void CIYAM_BASE_DECL_SPEC storage_channel_documents_close( const char* p_identity = 0 );

bool CIYAM_BASE_DECL_SPEC storage_channel_documents_marked( const std::string& identity );
bool CIYAM_BASE_DECL_SPEC storage_channel_documents_opened( const std::string& identity );

std::string CIYAM_BASE_DECL_SPEC storage_channel_documents_update( const std::string& identity, bool submitted = false );
std::string CIYAM_BASE_DECL_SPEC storage_channel_documents_prepare( const std::string& identity );

void CIYAM_BASE_DECL_SPEC storage_channel_documents_cancel_pending( const std::string& identity );

void CIYAM_BASE_DECL_SPEC storage_channel_documents_reject_waiting(
 const char* p_identity = 0, const char* p_file_list_for_rejection = 0 );

void CIYAM_BASE_DECL_SPEC storage_channel_documents_reject_reviewing(
 const char* p_identity = 0, const char* p_file_list_for_rejection = 0 );

void CIYAM_BASE_DECL_SPEC storage_channel_documents_approve_waiting(
 const char* p_identity = 0, const char* p_file_list_for_approval = 0 );

void CIYAM_BASE_DECL_SPEC storage_channel_documents_approve_reviewing(
 const char* p_identity = 0, const char* p_file_list_for_approval = 0 );

enum channel_documents_type
{
   e_channel_documents_type_pending,
   e_channel_documents_type_waiting,
   e_channel_documents_type_retrieved,
   e_channel_documents_type_reviewing,
   e_channel_documents_type_submitting
};

std::string CIYAM_BASE_DECL_SPEC storage_channel_documents_specific( const std::string& identity,
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

#endif

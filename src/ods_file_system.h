// Copyright (c) 2016 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef ODS_FILE_SYSTEM_H
#  define ODS_FILE_SYSTEM_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <deque>
#     include <vector>
#     include <string>
#  endif

#  include "macros.h"
#  include "ptypes.h"

#  ifdef CIYAM_BASE_LIB
#     ifdef CIYAM_BASE_IMPL
#        define ODS_FILE_SYSTEM_DECL_SPEC DYNAMIC_EXPORT
#     else
#        define ODS_FILE_SYSTEM_DECL_SPEC DYNAMIC_IMPORT
#     endif
#  else
#     define ODS_FILE_SYSTEM_DECL_SPEC
#  endif

class ods;

class ODS_FILE_SYSTEM_DECL_SPEC ods_file_system
{
   public:
   ods_file_system( ods& o, int64_t i = 0 );
   ~ods_file_system( );

   void set_folder( const std::string& new_folder, bool* p_rc = 0 );

   inline std::string get_folder( ) const { return current_folder; }

   std::string determine_folder( const std::string& folder,
    std::ostream* p_os = 0, bool explicit_child_only = false );

   std::string determine_strip_and_change_folder( std::string& name, std::ostream* p_os = 0 );

   enum list_style
   {
      e_list_style_brief,
      e_list_style_default,
      e_list_style_extended
   };

   inline void list_files( std::ostream& os, list_style style = e_list_style_default )
   {
      list_files_or_objects( "", os, false, style );
   }

   inline void list_files(
    const std::string& expr, std::ostream& os, list_style style = e_list_style_default )
   {
      list_files_or_objects( expr, os, false, style );
   }

   void list_files( const std::string& expr, std::vector< std::string >& list, bool include_links = true );

   inline void list_files( std::vector< std::string >& list, bool include_links = true )
   {
      list_files( "", list, include_links );
   }

   void list_folders( const std::string& expr, std::vector< std::string >& list );

   inline void list_folders( std::vector< std::string >& list ) { list_folders( "", list ); }

   void list_folders( const std::string& expr, std::ostream& os, bool full_path = false );

   inline void list_folders( std::ostream& os, bool full_path = false ) { list_folders( "", os, full_path ); }

   inline void list_objects( std::ostream& os, list_style style = e_list_style_default )
   {
      list_files_or_objects( "", os, true, style );
   }

   inline void list_objects(
    const std::string& expr, std::ostream& os, list_style style = e_list_style_default )
   {
      list_files_or_objects( expr, os, true, style );
   }

   enum branch_style
   {
      e_branch_style_brief,
      e_branch_style_default,
      e_branch_style_extended
   };

   inline void branch_files( std::ostream& os, branch_style style = e_branch_style_default )
   {
      branch_files_or_objects( os, current_folder, "", style );
   }

   inline void branch_files(
    const std::string& expr, std::ostream& os, branch_style style = e_branch_style_default )
   {
      branch_files_or_objects( os, current_folder, expr, style );
   }

   void branch_files( const std::string& expr, std::vector< std::string >& files );

   void branch_folders( const std::string& expr, std::vector< std::string >& folders );

   void branch_folders(
    const std::string& expr, std::ostream& os, branch_style style = e_branch_style_default );

   inline void branch_folders( std::ostream& os, branch_style style = e_branch_style_default )
   {
      branch_folders( "", os, style );
   }

   inline void branch_objects( std::ostream& os, branch_style style = e_branch_style_default )
   {
      branch_files_or_objects( os, current_folder, "", style, true );
   }

   inline void branch_objects(
    const std::string& expr, std::ostream& os, branch_style style = e_branch_style_default )
   {
      branch_files_or_objects( os, current_folder, expr, style, true );
   }

   void add_file(
    const std::string& name, const std::string& source, std::ostream* p_os = 0, std::istream* p_is = 0 );

   inline void add_file( const std::string& name, std::ostream* p_os = 0, std::istream* p_is = 0 )
   {
      add_file( name, "", p_os, p_is );
   }

   inline void add_file( const std::string& name, std::istream* p_is )
   {
      add_file( name, "", 0, p_is );
   }

   void get_file( const std::string& name,
    const std::string& destination, std::ostream* p_os = 0, bool output_to_stream = false );

   inline void get_file( const std::string& name, std::ostream* p_os = 0, bool output_to_stream = false )
   {
      get_file( name, "", p_os, output_to_stream );
   }

   inline void get_file( const std::string& name, std::ostream& os )
   {
      get_file( name, "", &os, true );
   }

   bool has_file( const std::string& name );

   void link_file( const std::string& name, const std::string& source, std::ostream* p_os = 0 );

   void move_file( const std::string& name, const std::string& destination, std::ostream* p_os = 0 );

   void store_file( const std::string& name,
    const std::string& source, std::ostream* p_os = 0, std::istream* p_is = 0 );

   inline void store_file( const std::string& name, std::istream* p_is = 0 )
   {
      store_file( name, "", 0, p_is );
   }

   inline void store_file( const std::string& name, std::ostream* p_os, std::istream* p_is = 0 )
   {
      store_file( name, "", p_os, p_is );
   }

   void remove_file( const std::string& name, std::ostream* p_os = 0 );

   void replace_file( const std::string& name,
    const std::string& source, std::ostream* p_os = 0, std::istream* p_is = 0 );

   inline void replace_file( const std::string& name, std::ostream* p_os = 0, std::istream* p_is = 0 )
   {
      replace_file( name, "", p_os, p_is );
   }

   void store_as_text_file( const std::string& name, int32_t val );
   void store_as_text_file( const std::string& name, int64_t val );
   void store_as_text_file( const std::string& name, const std::string& val, size_t pad_to_len = 0 );

   void fetch_from_text_file( const std::string& name, int32_t& val );
   void fetch_from_text_file( const std::string& name, int64_t& val );
   void fetch_from_text_file( const std::string& name, std::string& val, bool remove_padding = false );

   void add_folder( const std::string& name, std::ostream* p_os = 0 );

   bool has_folder( const std::string& name );

   void move_folder( const std::string& name,
    const std::string& destination, bool overwrite = false, std::ostream* p_os = 0 );

   void remove_folder( const std::string& name, std::ostream* p_os = 0, bool remove_branch = false );

   void rebuild_index( );

   void dump_node_data( const std::string& file_name, std::ostream* p_os = 0 );

   protected:
   enum file_size_output_type
   {
      e_file_size_output_type_none,
      e_file_size_output_type_scaled,
      e_file_size_output_type_num_bytes
   };

   void perform_match( std::ostream& os,
    const std::string& expr, const std::string& regexpr, size_t* p_count = 0,
    std::vector< std::pair< std::string, std::string > >* p_search_replaces = 0,
    const char* p_prefix_1 = 0, const char* p_prefix_2 = 0,
    const char erase_all_before_and_including = '\0',
    file_size_output_type file_size_output = e_file_size_output_type_none,
    const char* p_ignore_with_prefix = 0, std::deque< std::string >* p_extra_items = 0,
    std::pair< std::string, std::string >* p_range = 0 );

   void expand_entity_expression( const std::string& expr,
    bool had_wildcard, std::string& entity_expr, const char* p_suffix = 0 );

   void get_child_folders( const std::string& expr,
    bool full, std::deque< std::string >& folders, bool append_separator = true );

   void list_files_or_objects( const std::string& expr,
    std::ostream& os, bool objects = true, list_style style = e_list_style_default );

   void branch_files_or_objects(
    std::ostream& os, const std::string& folder, const std::string& expr,
    branch_style style, bool show_folders = false, const std::string* p_start_folder = 0 );

   bool move_files_and_folders( const std::string& source,
    const std::string& destination, bool src_is_root, bool dest_is_root, bool replace_existing = false );

   bool remove_items_for_file( const std::string& name, std::ostream* p_os = 0 );
   bool remove_items_for_folder( const std::string& name, std::ostream* p_os = 0 );

   private:
   ods& o;

   struct impl;
   impl* p_impl;

   std::string current_folder;
};

struct temporary_set_folder
{
   temporary_set_folder( ods_file_system& ofs, const std::string& tmp_folder )
    :
    ofs( ofs )
   {
      old_folder = ofs.get_folder( );
      ofs.set_folder( tmp_folder );
   }

   ~temporary_set_folder( )
   {
      ofs.set_folder( old_folder );
   }

   ods_file_system& ofs;
   std::string old_folder;
};

#endif


// Copyright (c) 2016-2025 CIYAM Developers
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

struct progress;

struct temporary_force_write;
struct temporary_allow_specials;
struct temporary_include_hidden;

class ODS_FILE_SYSTEM_DECL_SPEC ods_file_system
{
   public:
   ods_file_system( ods& o, int64_t i = 0, bool for_regression_tests = false );
   ~ods_file_system( );

   friend struct temporary_force_write;
   friend struct temporary_allow_specials;
   friend struct temporary_include_hidden;

   inline std::string get_folder( ) const { return current_folder; }

   std::string get_folder( bool /* force reload */ );

   void set_folder( const std::string& new_folder, bool* p_rc = 0 );
   void set_root_folder( const std::string& new_folder, bool* p_rc = 0 );

   std::string determine_folder( const std::string& folder,
    bool explicit_child_only = false, bool ignore_not_found = false );

   std::string determine_strip_and_change_folder( std::string& name );

   enum list_style
   {
      e_list_style_brief,
      e_list_style_default,
      e_list_style_extended
   };

   inline void list_files( std::ostream& os, list_style style = e_list_style_default )
   {
      list_files_or_objects( "", os, "", false, style );
   }

   inline void list_files(
    const std::string& expr, std::ostream& os, list_style style = e_list_style_default )
   {
      list_files_or_objects( expr, os, "", false, style );
   }

   inline void list_files( const std::string& expr, std::ostream& os,
    const std::string& start_from, list_style style = e_list_style_default,
    bool inclusive = true, size_t limit = 0, bool in_reverse_order = false )
   {
      list_files_or_objects( expr, os, start_from, false, style, inclusive, limit, in_reverse_order );
   }

   void list_files( const std::string& expr, std::vector< std::string >& list, bool include_links,
    const std::string& start_from, bool inclusive = true, size_t limit = 0, bool in_reverse_order = false );

   inline void list_files( const std::string& expr,
    std::vector< std::string >& list, bool include_links = true )
   {
      list_files( expr, list, include_links, "" );
   }

   inline void list_files( std::vector< std::string >& list, bool include_links = true )
   {
      list_files( "", list, include_links, "" );
   }

   inline void list_files( std::vector< std::string >& list, const std::string& start_from,
    bool inclusive = true, size_t limit = 0, bool in_reverse_order = false, bool include_links = true )
   {
      list_files( "", list, include_links, start_from, inclusive, limit, in_reverse_order );
   }

   void list_links( const std::string& name, std::ostream& os );

   void list_folders( const std::string& expr,
    std::vector< std::string >& list, const std::string& start_from,
    bool inclusive = true, size_t limit = 0, bool in_reverse_order = false );

   inline void list_folders( std::vector< std::string >& list ) { list_folders( "", list, "" ); }

   inline void list_folders( std::vector< std::string >& list,
    const std::string& start_from, bool inclusive = true, size_t limit = 0, bool in_reverse_order = false )
   {
      list_folders( "", list, start_from, inclusive, limit, in_reverse_order );
   }

   void list_folders(
    const std::string& expr,
    std::ostream& os, bool full_path = false,
    std::pair< std::string, std::string >* p_range = 0,
    bool inclusive = true, size_t limit = 0, bool in_reverse_order = false );

   inline void list_folders( std::ostream& os, bool full_path = false ) { list_folders( "", os, full_path ); }

   inline void list_objects( std::ostream& os, list_style style = e_list_style_default )
   {
      list_files_or_objects( "", os, "", true, style );
   }

   inline void list_objects(
    const std::string& expr, std::ostream& os, list_style style = e_list_style_default )
   {
      list_files_or_objects( expr, os, "", true, style );
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

   void branch_files( const std::string& expr,
    std::vector< std::string >& files, branch_style style = e_branch_style_default );

   void branch_folders( const std::string& expr,
    std::vector< std::string >& folders, branch_style style = e_branch_style_default );

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

   void add_file( const std::string& name, const std::string& source,
    std::ostream* p_os = 0, std::istream* p_is = 0, progress* p_progress = 0, const std::string* p_perms = 0 );

   inline void add_file( const std::string& name,
    std::ostream* p_os = 0, std::istream* p_is = 0, progress* p_progress = 0, const std::string* p_perms = 0 )
   {
      add_file( name, "", p_os, p_is, p_progress, p_perms );
   }

   inline void add_file( const std::string& name,
    std::istream* p_is, progress* p_progress = 0, const std::string* p_perms = 0 )
   {
      add_file( name, "", 0, p_is, p_progress, p_perms );
   }

   void get_file( const std::string& name,
    const std::string& destination, std::ostream* p_os = 0, progress* p_progress = 0 );

   inline void get_file( const std::string& name, std::ostream* p_os = 0, progress* p_progress = 0 )
   {
      get_file( name, "", p_os, p_progress );
   }

   inline void get_file( const std::string& name, std::ostream& os, progress* p_progress = 0 )
   {
      get_file( name, "", &os, p_progress );
   }

   bool is_link( const std::string& name );

   bool has_file( const std::string& name, bool is_prefix = false,
    std::string* p_suffix = 0, std::string* p_perms = 0, int64_t* p_tm_val = 0, int64_t* p_num_bytes = 0 );

   std::string last_file_name_with_prefix( const std::string& prefix );

   std::string link_source( const std::string& name );

   void link_file( const std::string& name, const std::string& source );

   void move_file( const std::string& source, const std::string& destination );

   bool store_file( const std::string& name, const std::string& source,
    std::ostream* p_os = 0, std::istream* p_is = 0, progress* p_progress = 0, bool force_write = false );

   inline bool store_file( const std::string& name,
    std::istream* p_is = 0, progress* p_progress = 0, bool force_write = false )
   {
      return store_file( name, "", 0, p_is, p_progress, force_write );
   }

   inline bool store_file( const std::string& name,
    std::ostream* p_os, std::istream* p_is = 0, progress* p_progress = 0, bool force_write = false )
   {
      return store_file( name, "", p_os, p_is, p_progress, force_write );
   }

   void remove_file( const std::string& name,
    std::ostream* p_os = 0, progress* p_progress = 0, bool is_prefix = false );

   void replace_file( const std::string& name,
    const std::string& source, std::ostream* p_os = 0, std::istream* p_is = 0, progress* p_progress = 0, bool force_write = false );

   inline void replace_file( const std::string& name,
    std::ostream* p_os = 0, std::istream* p_is = 0, progress* p_progress = 0, bool force_write = false )
   {
      replace_file( name, "", p_os, p_is, p_progress, force_write );
   }

   void permissions_file( const std::string& name, std::ostream* p_os );
   void permissions_file( const std::string& name, const std::string& perms, std::ostream* p_os = 0 );

   void store_as_text_file( const std::string& name, int32_t val );
   void store_as_text_file( const std::string& name, int64_t val );
   void store_as_text_file( const std::string& name, const std::string& val, size_t pad_to_len = 0 );

   void fetch_from_text_file( const std::string& name, int32_t& val );
   void fetch_from_text_file( const std::string& name, int64_t& val );
   void fetch_from_text_file( const std::string& name, std::string& val, bool remove_padding = false );

   void add_folder( const std::string& name,
    std::ostream* p_os = 0, std::string* p_perms = 0, int64_t* p_tm_val = 0 );

   bool has_folder( const std::string& name, std::string* p_perms = 0, int64_t* p_tm_val = 0 );

   void move_folder( const std::string& name,
    const std::string& destination, bool overwrite = false, std::ostream* p_os = 0 );

   void remove_folder( const std::string& name, std::ostream* p_os = 0, bool remove_branch = false );

   void replace_folder( const std::string& name,
    std::ostream* p_os = 0, std::string* p_perms = 0, int64_t* p_tm_val = 0 );

   void rebuild_index( );

   void dump_node_data( const std::string& file_name, std::ostream* p_os = 0 );

   protected:
   enum file_size_output_type
   {
      e_file_size_output_type_none,
      e_file_size_output_type_scaled,
      e_file_size_output_type_num_bytes
   };

   void force_reload( );

   std::string value_folder_and_file_name( const std::string& provided,
    std::string* p_folder = 0, std::string* p_file_name = 0 );

   inline std::string key_value( const std::string& provided ) { return value_folder_and_file_name( provided ); }

   void perform_match( std::ostream& os,
    const std::string& expr, const std::string& regexpr, size_t* p_count = 0,
    std::vector< std::pair< std::string, std::string > >* p_search_replaces = 0,
    const char* p_prefix_1 = 0, const char* p_prefix_2 = 0,
    const char erase_all_before_and_including = '\0',
    file_size_output_type file_size_output = e_file_size_output_type_none,
    const char* p_ignore_with_prefix = 0, std::deque< std::string >* p_extra_items = 0,
    std::pair< std::string, std::string >* p_range = 0, bool inclusive = true,
    size_t limit = 0, bool in_reverse_order = false );

   void expand_entity_expression( const std::string& expr, std::string& entity_expr, const char* p_suffix = 0 );

   void get_child_folders( const std::string& expr,
    bool full, std::deque< std::string >& folders, bool append_separator = true );

   void list_files_or_objects( const std::string& expr,
    std::ostream& os, const std::string& start_from,
    bool objects = true, list_style style = e_list_style_default,
    bool inclusive = true, size_t limit = 0, bool in_reverse_order = false, bool branch = false );

   void branch_files_or_objects( std::ostream& os,
    const std::string& folder, const std::string& expr, branch_style style,
    bool show_folders = false, const std::string* p_start_folder = 0, size_t depth = 0 );

   bool move_files_and_folders( const std::string& source,
    const std::string& destination, bool src_is_root, bool dest_is_root, bool replace_existing = false );

   bool remove_items_for_file( const std::string& name,
    bool is_prefix = false, bool ignore_not_found = false );

   bool remove_items_for_folder( const std::string& name, bool ignore_not_found = false );

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

struct temporary_force_write
{
   temporary_force_write( ods_file_system& ofs );
   ~temporary_force_write( );

   ods_file_system& ofs;

   bool old_force_write;
};

struct temporary_allow_specials
{
   temporary_allow_specials( ods_file_system& ofs );
   ~temporary_allow_specials( );

   ods_file_system& ofs;

   bool old_allow_specials;
};

struct temporary_include_hidden
{
   temporary_include_hidden( ods_file_system& ofs, bool include_hidden = true );
   ~temporary_include_hidden( );

   ods_file_system& ofs;

   bool old_skip_hidden;
};

void ODS_FILE_SYSTEM_DECL_SPEC export_objects( ods_file_system& ofs,
 const std::string& directory, std::vector< std::string >* p_rename_expressions = 0,
 std::ostream* p_os = 0, progress* p_progress = 0, int level = 0, int start_pos = 0 );

void ODS_FILE_SYSTEM_DECL_SPEC import_objects( ods_file_system& ofs,
 const std::string& directory, std::vector< std::string >* p_rename_expressions = 0,
 std::ostream* p_os = 0, progress* p_progress = 0, bool force_write = false );

#endif

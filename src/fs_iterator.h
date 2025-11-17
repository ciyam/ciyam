// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FS_ITERATOR_H
#  define FS_ITERATOR_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <stack>
#     include <string>
#     include <sys/stat.h>
#  endif

const char c_dir_char = '/';

const char* const c_default_directory = "/";

struct directory_info;

struct fs_filter
{
   virtual bool operator ( )( const struct stat& statbuf ) = 0;
};

struct file_filter : fs_filter
{
   bool operator ( )( const struct stat& statbuf )
   {
      return ( statbuf.st_mode & S_IFDIR ) == 0;
   }
};

struct directory_filter : fs_filter
{
   bool operator ( )( const struct stat& statbuf )
   {
      return ( statbuf.st_mode & S_IFDIR ) != 0;
   }
};

class fs_iterator
{
   public:
   fs_iterator( const std::string& root = c_default_directory,
    fs_filter* p_filter = 0, bool treat_symbolic_links_as_files = false );

   ~fs_iterator( );

   void set_root( const std::string& new_root );

   const std::string& get_root( ) const { return root.empty( ) ? true_root : root; }
   const std::string& get_name( ) const { return name.empty( ) ? true_root : name; }

   const std::string& get_full_name( ) const { return full_name.empty( ) ? true_root : full_name; }
   const std::string& get_path_name( ) const { return path_name.empty( ) ? true_root : path_name; }

   bool get_is_directory( ) const { return is_directory; }

   size_t get_level( ) const { return directories.size( ) - 1; }

   bool has_next( );

   private:
   void reset( );

   bool is_first;
   bool use_lstat;

   bool is_true_root;
   bool is_directory;

   std::string root;
   std::string name;
   std::string full_name;
   std::string path_name;

   std::string true_root;

   fs_filter* p_filter;

   std::stack< directory_info* > directories;
};

#endif

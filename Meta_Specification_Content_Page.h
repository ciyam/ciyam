// Copyright (c) 2008
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

#ifndef META_SPECIFICATION_CONTENT_PAGE_H
#  define META_SPECIFICATION_CONTENT_PAGE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"

#  include "Meta_Specification.h"

#  ifdef MODULE_META_IMPL
#     define META_SPECIFICATION_CONTENT_PAGE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define META_SPECIFICATION_CONTENT_PAGE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class Meta_Field;
class Meta_Relationship;
class Meta_Procedure_Arg;
class Meta_Procedure;
class Meta_Class;

class META_SPECIFICATION_CONTENT_PAGE_DECL_SPEC Meta_Specification_Content_Page : public Meta_Specification
{
   friend class Meta_Field;
   friend class Meta_Relationship;
   friend class Meta_Procedure_Arg;
   friend class Meta_Procedure;
   friend class Meta_Class;

   public:
   typedef Meta_Specification_Content_Page class_type;
   typedef Meta_Specification parent_class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Actions_Field = 1,
      e_field_id_Child_Self_Relationship = 2,
      e_field_id_Clone_Permitted_Field = 3,
      e_field_id_Code_Field = 4,
      e_field_id_Content_Type = 5,
      e_field_id_Create_Copy_Output_Arg = 6,
      e_field_id_Create_Copy_Procedure = 7,
      e_field_id_File_Name_Field = 8,
      e_field_id_File_Name_Gen_Field = 9,
      e_field_id_Frag_Code_Field = 10,
      e_field_id_Frag_Content_Field = 11,
      e_field_id_Frag_Group_Field = 12,
      e_field_id_Fragment_Class = 13,
      e_field_id_Generate_Children_Field = 14,
      e_field_id_Generate_Output_Arg = 15,
      e_field_id_Generate_Procedure = 16,
      e_field_id_Group_Base_Path_Field = 17,
      e_field_id_Group_Class = 18,
      e_field_id_Group_Extra_Field = 19,
      e_field_id_Group_Field = 20,
      e_field_id_Group_Name_Field = 21,
      e_field_id_Hyperlink_Title_Field = 22,
      e_field_id_Link_Class = 23,
      e_field_id_Link_Content_Field = 24,
      e_field_id_Link_Group_Field = 25,
      e_field_id_Link_Name_Field = 26,
      e_field_id_Link_Page_Link_Child = 27,
      e_field_id_Meta_Content_Field = 28,
      e_field_id_Next_Child_Num_Field = 29,
      e_field_id_Order_Field = 30,
      e_field_id_Owned_Links_Field = 31,
      e_field_id_Page_Field = 32,
      e_field_id_Page_File_Class = 33,
      e_field_id_Page_File_Field = 34,
      e_field_id_Page_File_Name_Field = 35,
      e_field_id_Page_File_Name_Gen_Field = 36,
      e_field_id_Page_Link_Class = 37,
      e_field_id_Page_Page_Link_Child = 38,
      e_field_id_Parent_Page_Field = 39,
      e_field_id_Sub_Title_Field = 40,
      e_field_id_Text_Content_Field = 41,
      e_field_id_Title_Field = 42,
      e_field_id_Variable_Class = 43,
      e_field_id_Variable_Name = 44,
      e_field_id_Variable_Value = 45
   };

   Meta_Specification_Content_Page( );
   ~Meta_Specification_Content_Page( );

   Meta_Field& Actions_Field( );
   const Meta_Field& Actions_Field( ) const;
   void Actions_Field( const std::string& key );

   Meta_Relationship& Child_Self_Relationship( );
   const Meta_Relationship& Child_Self_Relationship( ) const;
   void Child_Self_Relationship( const std::string& key );

   Meta_Field& Clone_Permitted_Field( );
   const Meta_Field& Clone_Permitted_Field( ) const;
   void Clone_Permitted_Field( const std::string& key );

   Meta_Field& Code_Field( );
   const Meta_Field& Code_Field( ) const;
   void Code_Field( const std::string& key );

   Meta_Field& Content_Type( );
   const Meta_Field& Content_Type( ) const;
   void Content_Type( const std::string& key );

   Meta_Procedure_Arg& Create_Copy_Output_Arg( );
   const Meta_Procedure_Arg& Create_Copy_Output_Arg( ) const;
   void Create_Copy_Output_Arg( const std::string& key );

   Meta_Procedure& Create_Copy_Procedure( );
   const Meta_Procedure& Create_Copy_Procedure( ) const;
   void Create_Copy_Procedure( const std::string& key );

   Meta_Field& File_Name_Field( );
   const Meta_Field& File_Name_Field( ) const;
   void File_Name_Field( const std::string& key );

   Meta_Field& File_Name_Gen_Field( );
   const Meta_Field& File_Name_Gen_Field( ) const;
   void File_Name_Gen_Field( const std::string& key );

   Meta_Field& Frag_Code_Field( );
   const Meta_Field& Frag_Code_Field( ) const;
   void Frag_Code_Field( const std::string& key );

   Meta_Field& Frag_Content_Field( );
   const Meta_Field& Frag_Content_Field( ) const;
   void Frag_Content_Field( const std::string& key );

   Meta_Field& Frag_Group_Field( );
   const Meta_Field& Frag_Group_Field( ) const;
   void Frag_Group_Field( const std::string& key );

   Meta_Class& Fragment_Class( );
   const Meta_Class& Fragment_Class( ) const;
   void Fragment_Class( const std::string& key );

   Meta_Field& Generate_Children_Field( );
   const Meta_Field& Generate_Children_Field( ) const;
   void Generate_Children_Field( const std::string& key );

   Meta_Procedure_Arg& Generate_Output_Arg( );
   const Meta_Procedure_Arg& Generate_Output_Arg( ) const;
   void Generate_Output_Arg( const std::string& key );

   Meta_Procedure& Generate_Procedure( );
   const Meta_Procedure& Generate_Procedure( ) const;
   void Generate_Procedure( const std::string& key );

   Meta_Field& Group_Base_Path_Field( );
   const Meta_Field& Group_Base_Path_Field( ) const;
   void Group_Base_Path_Field( const std::string& key );

   Meta_Class& Group_Class( );
   const Meta_Class& Group_Class( ) const;
   void Group_Class( const std::string& key );

   Meta_Field& Group_Extra_Field( );
   const Meta_Field& Group_Extra_Field( ) const;
   void Group_Extra_Field( const std::string& key );

   Meta_Field& Group_Field( );
   const Meta_Field& Group_Field( ) const;
   void Group_Field( const std::string& key );

   Meta_Field& Group_Name_Field( );
   const Meta_Field& Group_Name_Field( ) const;
   void Group_Name_Field( const std::string& key );

   Meta_Field& Hyperlink_Title_Field( );
   const Meta_Field& Hyperlink_Title_Field( ) const;
   void Hyperlink_Title_Field( const std::string& key );

   Meta_Class& Link_Class( );
   const Meta_Class& Link_Class( ) const;
   void Link_Class( const std::string& key );

   Meta_Field& Link_Content_Field( );
   const Meta_Field& Link_Content_Field( ) const;
   void Link_Content_Field( const std::string& key );

   Meta_Field& Link_Group_Field( );
   const Meta_Field& Link_Group_Field( ) const;
   void Link_Group_Field( const std::string& key );

   Meta_Field& Link_Name_Field( );
   const Meta_Field& Link_Name_Field( ) const;
   void Link_Name_Field( const std::string& key );

   Meta_Relationship& Link_Page_Link_Child( );
   const Meta_Relationship& Link_Page_Link_Child( ) const;
   void Link_Page_Link_Child( const std::string& key );

   Meta_Field& Meta_Content_Field( );
   const Meta_Field& Meta_Content_Field( ) const;
   void Meta_Content_Field( const std::string& key );

   Meta_Field& Next_Child_Num_Field( );
   const Meta_Field& Next_Child_Num_Field( ) const;
   void Next_Child_Num_Field( const std::string& key );

   Meta_Field& Order_Field( );
   const Meta_Field& Order_Field( ) const;
   void Order_Field( const std::string& key );

   Meta_Field& Owned_Links_Field( );
   const Meta_Field& Owned_Links_Field( ) const;
   void Owned_Links_Field( const std::string& key );

   Meta_Field& Page_Field( );
   const Meta_Field& Page_Field( ) const;
   void Page_Field( const std::string& key );

   Meta_Class& Page_File_Class( );
   const Meta_Class& Page_File_Class( ) const;
   void Page_File_Class( const std::string& key );

   Meta_Field& Page_File_Field( );
   const Meta_Field& Page_File_Field( ) const;
   void Page_File_Field( const std::string& key );

   Meta_Field& Page_File_Name_Field( );
   const Meta_Field& Page_File_Name_Field( ) const;
   void Page_File_Name_Field( const std::string& key );

   Meta_Field& Page_File_Name_Gen_Field( );
   const Meta_Field& Page_File_Name_Gen_Field( ) const;
   void Page_File_Name_Gen_Field( const std::string& key );

   Meta_Class& Page_Link_Class( );
   const Meta_Class& Page_Link_Class( ) const;
   void Page_Link_Class( const std::string& key );

   Meta_Relationship& Page_Page_Link_Child( );
   const Meta_Relationship& Page_Page_Link_Child( ) const;
   void Page_Page_Link_Child( const std::string& key );

   Meta_Field& Parent_Page_Field( );
   const Meta_Field& Parent_Page_Field( ) const;
   void Parent_Page_Field( const std::string& key );

   Meta_Field& Sub_Title_Field( );
   const Meta_Field& Sub_Title_Field( ) const;
   void Sub_Title_Field( const std::string& key );

   Meta_Field& Text_Content_Field( );
   const Meta_Field& Text_Content_Field( ) const;
   void Text_Content_Field( const std::string& key );

   Meta_Field& Title_Field( );
   const Meta_Field& Title_Field( ) const;
   void Title_Field( const std::string& key );

   Meta_Class& Variable_Class( );
   const Meta_Class& Variable_Class( ) const;
   void Variable_Class( const std::string& key );

   Meta_Field& Variable_Name( );
   const Meta_Field& Variable_Name( ) const;
   void Variable_Name( const std::string& key );

   Meta_Field& Variable_Value( );
   const Meta_Field& Variable_Value( ) const;
   void Variable_Value( const std::string& key );

   std::string get_field_value( int field ) const;
   void set_field_value( int field, const std::string& value );

   bool has_field_changed( const std::string& field ) const;

   bool is_field_transient( int field ) const;
   bool is_field_transient( field_id id ) const { return static_is_field_transient( id ); }

   std::string get_field_name( int field ) const;

   int get_field_num( const std::string& field ) const;

   uint64_t get_state( ) const;

   const std::string& execute( const std::string& cmd_and_args );

   void clear( );

   void validate( unsigned state, bool is_internal );

   void after_fetch( );
   void finalise_fetch( );

   void at_create( );

   void to_store( bool is_create, bool is_internal );
   void for_store( bool is_create, bool is_internal );
   void after_store( bool is_create, bool is_internal );

   bool can_destroy( bool is_internal );
   void for_destroy( bool is_internal );
   void after_destroy( bool is_internal );

   void set_default_values( );

   bool is_filtered( const std::set< std::string >& filters ) const;

   const char* class_id( ) const;
   const char* class_name( ) const;
   const char* plural_name( ) const;

   const char* module_id( ) const;
   const char* module_name( ) const;

   const char* resolved_module_id( ) const { return static_resolved_module_id( ); }
   const char* resolved_module_name( ) const { return static_resolved_module_name( ); }

   const char* lock_class_id( ) const { return static_lock_class_id( ); }
   const char* check_class_name( ) const { return static_check_class_name( ); }

   std::string get_display_name( bool plural = false ) const;

   bool get_is_alias( ) const;
   void get_alias_base_info( std::pair< std::string, std::string >& alias_base_info ) const;

   void get_base_class_info( std::vector< std::pair< std::string, std::string > >& base_class_info ) const;

   class_base& get_or_create_graph_child( const std::string& context );

   void clear_foreign_key( const std::string& field );
   void set_foreign_key_value( const std::string& field, const std::string& value );

   const std::string& get_foreign_key_value( const std::string& field );

   void get_class_info( class_info_container& class_info ) const { static_get_class_info( class_info ); }
   void get_field_info( field_info_container& field_info ) const { return static_get_field_info( field_info ); }
   void get_foreign_key_info( foreign_key_info_container& foreign_key_info ) const { static_get_foreign_key_info( foreign_key_info ); }

   int get_num_fields( bool* p_done = 0, const std::string* p_class_name = 0 ) const { return static_get_num_fields( p_done, p_class_name ); }

   const procedure_info_container& get_procedure_info( ) const { return static_get_procedure_info( ); }

   void get_text_search_fields( std::vector< std::string >& fields ) const { return static_get_text_search_fields( fields ); }

   void get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs ) const { return static_get_all_enum_pairs( pairs ); }

   std::string get_sql_columns( ) const { return static_get_sql_columns( ); }

   void get_sql_indexes( std::vector< std::string >& indexes ) const { static_get_sql_indexes( indexes ); }
   void get_sql_unique_indexes( std::vector< std::string >& indexes ) const { static_get_sql_unique_indexes( indexes ); }

   void get_sql_column_names( std::vector< std::string >& names, bool* p_done, const std::string* p_class_name ) const;
   void get_sql_column_values( std::vector< std::string >& values, bool* p_done, const std::string* p_class_name ) const;

   void get_required_field_names( std::set< std::string >& names,
    bool required_transients, std::set< std::string >* p_dependents = 0 ) const;

   void get_always_required_field_names( std::set< std::string >& names,
    bool required_transients, std::set< std::string >& dependents ) const;

   void get_transient_replacement_field_names( const std::string& name, std::vector< std::string >& names ) const;

   void do_generate_sql( generate_sql_type type, std::vector< std::string >& sql_stmts ) const;

   static void static_class_init( const char* p_module_name );
   static void static_class_term( const char* p_module_name );

   static const char* static_class_id( ) { return "131100"; }
   static const char* static_class_name( ) { return "Specification_Content_Page"; }
   static const char* static_plural_name( ) { return "Specification_Content_Pages"; }

   static const char* static_module_id( ) { return "100"; }
   static const char* static_module_name( ) { return "Meta"; }

   static const char* static_resolved_module_id( );
   static const char* static_resolved_module_name( );

   static const char* static_lock_class_id( );
   static const char* static_check_class_name( );

   static bool static_has_derivations( );

   static void static_get_class_info( class_info_container& class_info );
   static void static_get_field_info( field_info_container& all_field_info );
   static void static_get_foreign_key_info( foreign_key_info_container& foreign_key_info );

   static int static_get_num_fields( bool* p_done = 0, const std::string* p_class_name = 0 );

   static bool static_is_field_transient( field_id id );

   static const char* static_get_field_id( field_id id );
   static const char* static_get_field_name( field_id id );

   static int static_get_field_num( const std::string& field );

   static procedure_info_container& static_get_procedure_info( );

   static void static_get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs );

   static void static_get_text_search_fields( std::vector< std::string >& fields );

   static std::string static_get_sql_columns( );

   static void static_get_sql_indexes( std::vector< std::string >& indexes );
   static void static_get_sql_unique_indexes( std::vector< std::string >& indexes );

   static void static_insert_derivation( const std::string& module_and_class_id );
   static void static_remove_derivation( const std::string& module_and_class_id );

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_Specification_Content_Page* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_Specification_Content_Page( const Meta_Specification_Content_Page& );
   Meta_Specification_Content_Page& operator =( const Meta_Specification_Content_Page& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   std::string get_field_display_name( const std::string& id ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_foreign_key( Meta_Field& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Relationship& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Procedure_Arg& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Procedure& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Class& o, const std::string& value );

   virtual void setup_graph_parent( Meta_Field& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Relationship& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Procedure_Arg& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Procedure& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Class& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );
};

inline std::string to_string( const Meta_Specification_Content_Page& c ) { return c.get_key( ); }

inline bool is_null( const Meta_Specification_Content_Page& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_Specification_Content_Page& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_Specification_Content_Page& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif


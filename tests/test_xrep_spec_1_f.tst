   // [(start field_from_variable)] 12345
   if( !is_create && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).Field_Name( from_string< typeof(
       get_obj( ).Field_Name( ) ) >( get_system_variable( "variable_name" ) ) );
   // [(finish field_from_variable)] 12345


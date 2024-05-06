   // [(start field_from_variable)] 12345
   if( !get_obj( ).get_is_for_peer( )
    && is_create && !get_obj( ).get_key( ).empty( )
    && ( state & c_modifier_Modifier ) )
      get_obj( ).Field_Name( from_string< typeof(
       get_obj( ).Field_Name( ) ) >( get_obj( ).get_variable( "variable_name" + get_obj( ).Other_Field( ) ) ) );
   // [(finish field_from_variable)] 12345


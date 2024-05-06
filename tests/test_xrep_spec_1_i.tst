   // [(start field_from_variable)] 12345
   if( !get_obj( ).get_is_for_peer( )
    && get_obj( ).Test_Field( )
    && ( state & c_modifier_Modifier ) )
      get_obj( ).Field_Name( from_string< typeof(
       get_obj( ).Field_Name( ) ) >( get_system_variable( "variable_name" + to_string( get_obj( ).Other_Field( ) ) ) ) );
   // [(finish field_from_variable)] 12345


   // [(start field_from_variable)] 12345
   if( get_obj( ).get_key( ).empty( )
    && get_obj( ).Test_Field( ) )
      get_obj( ).Field_Name( from_string< typeof(
       get_obj( ).Field_Name( ) ) >( get_session_variable( "variable_name" + to_string( get_obj( ).Other_Field( ) ) ) ) );
   // [(finish field_from_variable)] 12345


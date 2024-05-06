   // [(start field_from_variable)] 12345
   if( !get_obj( ).get_is_for_peer( )
    && get_obj( ).get_key( ).empty( ) )
      get_obj( ).Field_Name( from_string< typeof(
       get_obj( ).Field_Name( ) ) >( get_obj( ).get_variable( "variable_name" ) ) );
   // [(finish field_from_variable)] 12345


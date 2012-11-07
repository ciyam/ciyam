   // [(start test_spec)]
   if( !get_obj( ).get_key( ).empty( ) )
   {
      string value;

      if( !is_null( get_obj( ).Source( ) ) )
      {
         if( !value.empty( ) )
            value += ",";
         value += to_string( get_obj( ).Source( ) );
      }

      get_obj( ).Field( value );
   }   
   // [(finish test_spec)]


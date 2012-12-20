   // [(start test_spec)]
   if( !get_obj( ).get_key( ).empty( ) )
   {
      string value;

      if( !is_null( get_obj( ).First( ) ) )
      {
         if( !value.empty( ) )
            value += ",";
         value += to_string( get_obj( ).First( ) );
      }

      if( !is_null( get_obj( ).Second( ) ) )
      {
         if( !value.empty( ) )
            value += ",";
         value += to_string( get_obj( ).Second( ) );
      }

      if( !is_null( get_obj( ).Third( ) ) )
      {
         if( !value.empty( ) )
            value += ",";
         value += to_string( get_obj( ).Third( ) );
      }

      get_obj( ).Field( value );
   }   
   // [(finish test_spec)]


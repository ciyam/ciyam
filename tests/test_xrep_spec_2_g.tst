   // [(start destroy_restrict)] 12345
   dependents.insert( "Field" );

   if( ( use_transients && is_field_transient( e_field_id_Field ) )
    || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
      names.insert( "Field" );
   // [(finish destroy_restrict)] 12345


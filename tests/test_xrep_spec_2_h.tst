   // [(start destroy_restrict)] 12345
   dependents.insert( "Parent" );

   if( ( use_transients && is_field_transient( e_field_id_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent ) ) )
      names.insert( "Parent" );
   // [(finish destroy_restrict)] 12345


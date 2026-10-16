// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Hashes its input round after round - hardening an unlock key's entropy or a password - a module ("Modules" in
// "ciyam.js"). In steps, as before:
//
//    an input, after loading or "(ready)"   primes it - one hash of it, "(hash primed)"
//    "<rounds>"                              that many more rounds - each hashes the result with the input after it
//    "<rounds>:<repeats>"                    that many rounds, repeated, the progress shown; then ready for a new input
//    "+<rounds>:<repeats>"                   the same, carrying on with this input after
//    nothing                                 "(ready)" - the next input primes again
//
// "reset" - ready for a new input - is kept where a list can "wait ciyam_harden_reset" ("expose" in its context), and so
// is the result as it goes. The SHA-256 is the page's faster one, decided once a page ("ciyam_sha256_fastest( )") - it was
// timed here on every load.
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_harden",
      version: "2",
      description: "hashes its input round after round - hardening entropy or a password",
      example: "<input>",
      needs: [ "sha256" ]
   };

   // NOTE: "num" rounds - each hashes what there is with "input" after it - from "initial", or from "input" itself; with nought
   // rounds, one hash of "input". The page's in-step SHA-256 runs without waiting between rounds.
   static async rounds( hasher, num, input, initial )
   {
      var s = ( ( initial !== null ) && ( initial !== undefined ) ) ? initial : input;
      var count = Number( num );

      if( count === 0 )
         s = await hasher.hash( input );

      for( var i = 0; i < count; i++ )
         s = hasher.native ? await hasher.hash( s + input ) : hasher.hash( s + input );

      return s;
   }

   constructor( context )
   {
      super( context );

      this.result = null;
      this.text = "";
      this.need_reset = false;

      this.set_reset( true );
   }

   set_reset( value )
   {
      this.reset = value;

      if( this.context.expose )
         this.context.expose( "reset", value );
   }

   hints( )
   {
      return !!( this.context.hints && this.context.hints( ) );
   }

   async hasher( )
   {
      return this.context.sha256_fastest ? await this.context.sha256_fastest( ) : { native: true, hash: this.context.sha256 };
   }

   async loaded( )
   {
      return ( await this.hasher( ) ).native ? "(using async version)" : "(using non-async version)";
   }

   async run( input )
   {
      var hasher = await this.hasher( );

      if( ( input === null ) || ( input === "" ) )
      {
         this.set_reset( true );
         this.need_reset = false;

         return { output: "(ready)" + ( this.hints( ) ? "\nemploy javascript harden <input>" : "" ) };
      }

      if( this.reset )
      {
         this.set_reset( false );
         this.need_reset = false;

         this.text = input;
         this.result = await this.constructor.rounds( hasher, 0, input );

         return { result: this.result, output: "(hash primed)" + ( this.hints( ) ? "\nemploy javascript harden <rounds>[:<repeats>])" : "" ) };
      }

      if( input.indexOf( "+" ) === 0 )
         input = input.substring( 1 );
      else
         this.need_reset = true;

      var colon = input.indexOf( ":" );

      if( colon <= 0 )
      {
         this.result = await this.constructor.rounds( hasher, input, this.text, this.result );

         return this.result;
      }

      var chunk = input.substr( 0, colon );
      var repeats = Number( input.substring( colon + 1 ) );

      if( this.context.progress )
         this.context.progress( 0 );

      for( var repeat = 1; repeat <= repeats; repeat++ )
      {
         // NOTE: Let go by the page - unloaded, or the session ended - so it ends here, and keeps nothing.
         if( this.context.stopped && this.context.stopped( ) )
         {
            this.result = null;
            this.text = "";

            return null;
         }

         this.result = await this.constructor.rounds( hasher, chunk, this.text, this.result );

         if( this.context.expose )
            this.context.expose( "result", this.result );

         if( this.context.progress )
            this.context.progress( repeat / repeats );

         await new Promise( function( resolve ) { setTimeout( resolve, 10 ); } );
      }

      if( this.context.progress )
         this.context.progress( null );

      this.set_reset( this.need_reset );

      return this.result;
   }
} );

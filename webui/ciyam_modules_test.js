// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Exercises the modules ("Modules" in "ciyam.js") and the modules themselves against fixed inputs, and prints a
// deterministic transcript. No server and no browser are needed. The answers through the old globals are those Ian's
// original modules gave, compared step by step when they were converted (2026-10-16).
//
// Run with:  node ciyam_modules_test.js
// Compare against "ciyam_modules_test.tst" - see "run_chat_tests.sh".

const m = require( "./ciyam.js" );

// NOTE: A module file calls "ciyam_register_module( )" and extends "ciyam_module" as globals, as in a page.
global.ciyam_register_module = m.ciyam_register_module;
global.ciyam_module = m.ciyam_module;

var failures = 0;

function check( label, actual, expected )
{
   var ok = ( JSON.stringify( actual ) === JSON.stringify( expected ) );

   if( !ok )
      ++failures;

   console.log( ( ok ? "ok   " : "FAIL " ) + label );

   if( !ok )
   {
      console.log( "       expected: " + JSON.stringify( expected ) );
      console.log( "       actual:   " + JSON.stringify( actual ) );
   }
}

function heading( text )
{
   console.log( "" );
   console.log( "== " + text + " " + "=".repeat( Math.max( 0, 60 - text.length ) ) );
}

// NOTE: A page, as far as the old globals go - a module's legacy functions are made on it, and called as the console and
// the harness call them, by name with a callback.
function page( )
{
   var host = { init_script_value: null, include_script_usage_hints: false };

   host.call = function( fn, input )
   {
      return new Promise( function( done ) { host[ fn ]( function( value ) { done( value ); }, input ); } );
   };

   return host;
}

async function steps( module_class, list )
{
   var host = page( );
   var name = module_class.about.name;
   var out = [ ];

   m.ciyam_module_legacy( module_class, host );

   for( var step of list )
   {
      host.include_script_usage_hints = !!step.hints;

      var said;

      if( step.load !== undefined )
      {
         host.init_script_value = step.load;
         said = await host.call( name + "_at_load" );
      }
      else
         said = await host.call( name + "_execute", step.exec );

      out.push( [ said, host[ name + "_result" ] ] );
   }

   return out;
}

( async function( )
{
   heading( "the registry" );

   require( "./ciyam_xor_hex_data.js" );
   require( "./ciyam_xxxxx.js" );

   var xor_module = m.ciyam_module_named( "xor_hex_data" );

   check( "a module registers itself by loading", [ xor_module !== null, m.ciyam_module_named( "xxxxx" ) !== null, m.ciyam_module_named( "nothing" ) ],
    [ true, true, null ] );
   check( "- says what it is, for the console's help", m.ciyam_module_about( "xor_hex_data" ),
    [ "xor_hex_data 2 - XORs hex with the result before it", "employ javascript xor_hex_data 01020304" ] );
   check( "- a name that could not be a file's refused", ( function( )
   {
      try { m.ciyam_register_module( class extends m.ciyam_module { static about = { name: "bad name" }; } ); return "taken"; }
      catch( e ) { return "refused"; }
   } )( ), "refused" );
   check( "- named as its file is, so the server's renaming for an account's own reaches it", m.ciyam_register_module( class extends m.ciyam_module
    { static about = { name: "ciyam_20401" }; } ) === m.ciyam_module_named( "20401" ), true );
   m.ciyam_unregister_module( "20401" );
   check( "- loading it again replaces it - no second 'class' to fail", ( function( )
   {
      delete require.cache[ require.resolve( "./ciyam_xxxxx.js" ) ];
      require( "./ciyam_xxxxx.js" );
      return m.ciyam_module_named( "xxxxx" ) !== null;
   } )( ), true );
   check( "- unloaded, out of the registry", [ m.ciyam_unregister_module( "xxxxx" ), m.ciyam_module_named( "xxxxx" ), m.ciyam_unregister_module( "xxxxx" ) ],
    [ true, null, false ] );

   delete require.cache[ require.resolve( "./ciyam_xxxxx.js" ) ];
   require( "./ciyam_xxxxx.js" );

   heading( "xor_hex_data" );

   check( "its core - a byte at a time, the shorter padded at its start", [ xor_module.xor( "0f0f", "ff" ), xor_module.xor( "01020304", "ff" ),
    xor_module.xor( null, "abc" ), xor_module.xor( "", "" ) ], [ "0ff0", "010203fb", "abc0", "00" ] );

   var instance = new xor_module( { } );

   check( "an instance keeps its running result", [ await instance.run( "01020304" ), await instance.run( "ff" ), instance.result ],
    [ "01020304", "010203fb", "010203fb" ] );

   // NOTE: As Ian's original answered - the same steps, the same answers (compared 2026-10-16).
   check( "through the old globals - as the original answered", await steps( xor_module, [ { load: null, hints: true }, { exec: "01020304", hints: true },
    { exec: "ff", hints: true }, { exec: "abc" }, { exec: "" }, { load: "0f0f" }, { exec: "f0f0f0" } ] ),
    [ [ "(xor_hex_data loaded)\nemploy javascript xor_hex_data 01020304", null ], [ "01020304\n(i.e. XORs the prior result with hex input)", "01020304" ],
     [ "010203fb", "010203fb" ], [ "0102a83b", "0102a83b" ], [ "0102a83b", "0102a83b" ], [ "0f0f", "0f0f" ], [ "f0ffff", "f0ffff" ] ] );

   heading( "the template, xxxxx" );

   check( "through the old globals - as the original answered", await steps( m.ciyam_module_named( "xxxxx" ), [ { load: null, hints: true },
    { exec: "hello", hints: true }, { exec: "again", hints: true }, { exec: null }, { load: "init" } ] ),
    [ [ "(xxxxx loaded)\nemploy javascript xxxxx test", null ], [ "hello\n(i.e. will echo the provided argument)", "hello" ], [ "again", "again" ],
     [ "", "" ], [ "init", "init" ] ] );

   heading( "what a module is given" );

   check( "SHA-256 - the browser's own where there is one", await m.ciyam_sha256_hex( "abc" ),
    "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad" );

   var calls = [ ];
   var context = m.ciyam_module_page_context( { show_progress: function( ) { calls.push( "show" ); },
    update_progress: function( f ) { calls.push( f ); }, hide_progress: function( ) { calls.push( "hide" ); } } );

   context.progress( 0 );
   context.progress( 0.5 );
   context.progress( null );

   check( "progress through the page's own functions - shown at nought, hidden at the end", calls, [ "show", 0, 0.5, "hide" ] );
   check( "- and other modules", context.module( "xor_hex_data" ) === xor_module, true );

   var thrown = await steps( class extends m.ciyam_module { static about = { name: "ciyam_fails" }; async run( ) { throw new Error( "broken" ); } },
    [ { load: null } ] );

   check( "a module that runs nothing says it loaded", thrown, [ [ "(fails loaded)", null ] ] );

   console.log( "" );
   console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

   process.exit( failures === 0 ? 0 : 1 );
} )( );

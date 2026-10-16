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
const vm = require( "vm" );
const fs = require( "fs" );

// NOTE: "bip39.min.js" as a page loads it - in a context of its own, as in a browser; under Node's own globals it takes itself
// for Node and fails.
var bip39_page = { crypto: globalThis.crypto };

bip39_page.window = bip39_page;
vm.createContext( bip39_page );
vm.runInContext( fs.readFileSync( __dirname + "/bip39.min.js", "utf-8" ), bip39_page );

global.BIP39 = bip39_page.BIP39;

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

   check( "progress through the page's own functions - shown at nought, hidden at the end", calls, [ "show", 0.5, "hide" ] );
   check( "- and other modules", context.module( "xor_hex_data" ) === xor_module, true );

   var thrown = await steps( class extends m.ciyam_module { static about = { name: "ciyam_fails" }; async run( ) { throw new Error( "broken" ); } },
    [ { load: null } ] );

   check( "a module that runs nothing says it loaded", thrown, [ [ "(fails loaded)", null ] ] );

   heading( "bip39" );

   require( "./ciyam_bip39.js" );

   var bip39 = m.ciyam_module_named( "bip39" );
   var entropy = "3c6eede8b0c9717370546cd446427499";
   var words = "detect jazz kick ghost novel rifle scout mirror stairs craft outdoor crisp";

   check( "entropy to its words, and back - either case", [ bip39.convert( entropy ), bip39.convert( words ), bip39.convert( entropy.toUpperCase( ) ) ],
    [ words, entropy, words ] );
   check( "- nothing: twelve new words", bip39.convert( "" ).split( " " ).length, 12 );
   check( "- wrong words or entropy answer with themselves and what was wrong", [ bip39.convert( "abandon abandon abandon" ), bip39.convert( "xyz" ),
    bip39.convert( " 3c6e" ) ], [ "abandon abandon abandon\n(incorrect mnemonics)", "xyz\n(invalid entropy)", " 3c6e\n(invalid entropy)" ] );

   var loaded_words = await steps( bip39, [ { load: null, hints: true }, { exec: entropy, hints: true } ] );

   var first_load = loaded_words[ 0 ][ 0 ].split( "\n" );

   check( "through the old globals - loading answers with new words, and keeps them; no second usage line",
    [ first_load[ 0 ].split( " " ).length, first_load[ 1 ], loaded_words[ 0 ][ 1 ] === first_load[ 0 ], loaded_words[ 1 ] ],
    [ 12, "employ javascript bip39 {@1}", true, [ words, words ] ] );

   heading( "rpc_unlock" );

   require( "./ciyam_rpc_unlock.js" );

   var shown = [ ];
   var unlock = new ( m.ciyam_module_named( "rpc_unlock" ) )( { sha256: m.ciyam_sha256_hex, show_qr: function( text ) { shown.push( text ); } } );
   var answer = await unlock.run( "test" );
   var stamp = answer.split( "-" )[ 0 ];

   check( "the time, then the hash of the time and the password's hash - drawn as a QR code",
    [ /^[0-9]{10}-[0-9a-f]{64}$/.test( answer ), answer.split( "-" )[ 1 ] === await m.ciyam_sha256_hex( stamp + await m.ciyam_sha256_hex( "test" ) ),
     shown ], [ true, true, [ answer ] ] );

   var hinted = await steps( m.ciyam_module_named( "rpc_unlock" ), [ { exec: "test", hints: true } ] );

   check( "- its usage hint the command with the answer in it", hinted[ 0 ][ 0 ].split( "\n" )[ 1 ], "./ciyam_client -tls -rpc_unlock=" + hinted[ 0 ][ 1 ]
    + " localhost:6666" );

   heading( "harden" );

   require( "./ciyam_harden.js" );

   var harden = m.ciyam_module_named( "harden" );

   // NOTE: As Ian's original answered, with the browser's own SHA-256 and with "sha2.min.js" alike (compared 2026-10-16).
   var host_steps = await steps( harden, [ { load: null }, { exec: "abc", hints: true }, { exec: "3" }, { exec: "2:3" }, { exec: "+1:2" },
    { exec: "", hints: true } ] );

   check( "in steps - primed, rounds, rounds repeated, then ready again", host_steps.map( function( step ) { return step[ 0 ].split( "\n" ).join( " | " ); } ),
    [ "(using async version)", "(hash primed) | employ javascript harden <rounds>[:<repeats>])",
     "80f4af3280c667d4b821fc45104d78aaf5a6bd066cc7478e693c34074433820c", "0186af14ba4a6cdfb708917da48637d19f84d9c93e0f06dc4741dea1ffe14faf",
     "(hash primed)", "(ready) | employ javascript harden <input>" ] );
   check( "- '(ready)' leaves the result kept before", host_steps[ 5 ][ 1 ], host_steps[ 4 ][ 1 ] );

   var exposed = { };
   var hardening = new harden( { sha256: m.ciyam_sha256_hex, expose: function( key, value ) { exposed[ key ] = value; } } );

   await hardening.run( "abc" );

   var reset_after_prime = exposed.reset;

   await hardening.run( "1:2" );

   check( "'reset' kept where a list waits on it - not ready once primed, ready after a repeat", [ reset_after_prime, exposed.reset, !!exposed.result ],
    [ false, true, true ] );

   // NOTE: Let go while running - the page took its functions away, as a sign out does (found 2026-10-16: it kept running
   // and answered into the page after the session had ended).
   var page_of = page( );
   var answers = [ ];

   m.ciyam_module_legacy( harden, page_of );

   page_of.init_script_value = "secret";

   await page_of.call( "ciyam_harden_at_load" );

   var running = new Promise( function( done )
   {
      page_of.ciyam_harden_execute( function( value ) { answers.push( value ); done( ); }, "1:50" );
      setTimeout( done, 400 );
   } );

   await new Promise( function( resolve ) { setTimeout( resolve, 30 ); } );

   page_of.ciyam_harden_at_load = undefined;
   page_of.ciyam_harden_result = undefined;

   await running;
   await new Promise( function( resolve ) { setTimeout( resolve, 100 ); } );

   check( "let go while running - it stops, says nothing and keeps nothing on the page", [ answers.length, page_of.ciyam_harden_result,
    page_of.ciyam_harden_reset ], [ 0, undefined, false ] );

   console.log( "" );
   console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

   process.exit( failures === 0 ? 0 : 1 );
} )( );

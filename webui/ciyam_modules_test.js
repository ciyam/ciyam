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

   // NOTE: "sha2.min.js", which a page hashes with where the browser has no SHA-256 of its own (plain http), or where it is the
   // faster ("ciyam_sha256_fastest( )") - text not plain ASCII given it as its UTF-8 bytes, so it hashes as the browser's own and
   // the server's do, where it took each character's low byte before (2026-10-21).
   var sha2_page = { };

   vm.createContext( sha2_page );
   vm.runInContext( fs.readFileSync( __dirname + "/sha2.min.js", "utf-8" ), sha2_page );

   check( "- and through sha2.min.js, text as its UTF-8", [ "café", "naïve ☕ test", "plain" ].map( function( s )
   {
      return sha2_page.hex_sha256( m.ciyam_utf8_binary( s ) ).toLowerCase( ) === require( "crypto" ).createHash( "sha256" ).update( s, "utf8" ).digest( "hex" );
   } ), [ true, true, true ] );

   // NOTE: The sign in's hash of PIN and password - in a page through "sha2.min.js", from the command line through Node - the
   // same for a password not plain ASCII (Ian, 2026-10-22). Version 0 is the one SHA-256 sent until 2026-10-23.
   var node_sha = require( "crypto" );
   var sha_hex = function( text ) { return node_sha.createHash( "sha256" ).update( text, "utf8" ).digest( "hex" ); };

   // NOTE: Here in the test's own scope - called from its own context, each round would cross into it, and 300,000 rounds take
   // minutes rather than seconds (as a page's own does).
   global.hex_sha256 = new Function( fs.readFileSync( __dirname + "/sha2.min.js", "utf-8" ) + "; return hex_sha256;" )( );

   var in_page = new m.CIYAM( "localhost:13031", false, true );
   var passwords = [ "pässwörd", "パスワード", "plain-pass" ];
   var signin_hashes = [ ];

   for( var pw = 0; pw < passwords.length; pw++ )
      signin_hashes.push( await in_page.hash_combined( passwords[ pw ], "12345", 0 ) );

   check( "the sign in's hash, version 0 - a page's as the command line's, as UTF-8", signin_hashes,
    passwords.map( function( p ) { return sha_hex( "12345" + p ); } ) );

   // NOTE: Version 1 (Ian, 2026-10-23: hardened before it is sent) - worked out here without "ciyam.js": one SHA-256 of the PIN
   // and password, then each round after the SHA-256 of what there is with the two after it - as the server's "crypto_hash -x"
   // does; the floor for twelve or more UTF-8 bytes, twice it for nine to eleven, three times for fewer. In a page and through
   // Node alike - and as the node's own "crypto_hash -x=<rounds - 1>" gives it (2026-10-23), so "init_web_access" can make it.
   var hardened = function( access, password )
   {
      var rounds = m.password_hash_rounds( password );
      var s = sha_hex( access + password );

      for( var r = 1; r < rounds; r++ )
         s = sha_hex( s + access + password );

      return s;
   };

   check( "version 1's rounds - by the password's length in UTF-8 bytes", [ m.password_hash_rounds( "seven77" ), m.password_hash_rounds( "eight888" ),
    m.password_hash_rounds( "nine99999" ), m.password_hash_rounds( "elevenchars" ), m.password_hash_rounds( "twelve-chars" ),
    m.password_hash_rounds( "pässwörd" ), m.password_hash_rounds( "pässwörd-läng" ) ], [ 300000, 300000, 200000, 200000, 100000, 200000, 100000 ] );

   // NOTE: 40 or more UTF-8 bytes, no extra rounds (Ian, 2026-10-25) - so version 1 is version 0 for it.
   var long_password = "a-password-of-forty-bytes-from-a-manager";

   check( "- one round for 40 or more UTF-8 bytes", [ m.password_hash_rounds( long_password.slice( 0, 39 ) ),
    m.password_hash_rounds( long_password ), m.password_hash_rounds( "ä".repeat( 19 ) + "x" ), m.password_hash_rounds( "ä".repeat( 20 ) ) ],
    [ 100000, 1, 100000, 1 ] );
   check( "- and hashed the same either way", ( await in_page.hash_combined( long_password, "12345", 1 ) ),
    await in_page.hash_combined( long_password, "12345", 0 ) );

   var page_v1 = await in_page.hash_combined( "pässwörd", "12345" );

   delete global.hex_sha256;

   var node_v1 = await m.password_hash( "12345", "pässwörd", 1, node_sha );

   check( "the sign in's hash, version 1 - the current one: hardened, a page's as Node's, as worked out here", [ page_v1 === node_v1,
    node_v1 === hardened( "12345", "pässwörd" ), node_v1 !== sha_hex( "12345pässwörd" ) ], [ true, true, true ] );
   check( "- as the node's own crypto_hash -x gives it (-x=99999 for twelve bytes, -x=199999 for ten)", [
    await m.password_hash( "12345", "twelve-chars", 1, node_sha ), node_v1 ],
    [ "b2b29b24221ff5062bfe831eb0d08956ca1ef5b7b50423e633685d2d1b2c6274", "9feba07f0a288065b45986611ac756f7a364a523c0656d4912829c5f87d0b73f" ] );
   check( "- an unknown version refused", await m.password_hash( "12345", "p", 2, node_sha ).catch( function( e ) { return e.message; } ),
    "There is no password hash version 2." );

   // NOTE: The version a browser guesses (Ian, 2026-10-23) - as kept for the PIN; none kept, the first if the browser keeps
   // things for the node but no versions; the current one otherwise, and on the command line.
   // NOTE: In a page, through "window" - the command line never touches Node's own "localStorage", which warns (Ian, 2026-10-25).
   var fake_storage = function( entries )
   {
      return { get length( ) { return Object.keys( entries ).length; }, key: function( i ) { return Object.keys( entries )[ i ]; },
       getItem: function( k ) { return entries.hasOwnProperty( k ) ? entries[ k ] : null; },
       setItem: function( k, v ) { entries[ k ] = String( v ); }, removeItem: function( k ) { delete entries[ k ]; } };
   };

   var in_window = function( entries, act )
   {
      global.window = { localStorage: fake_storage( entries ) };

      try { return act( ); } finally { delete global.window; }
   };

   var guessed = [ in_page.hash_version_for( "20401" ) ];

   [ { }, { "cws.hashed_20401": "x" }, { "cws.hashed_20401": "x", "cws.hash_version_20101": "1" }, { "cws.hash_version_20401": "0" },
    { "cws.hash_version_20401": "7" }, { "cws.hash_version_20401": "one" } ].forEach( function( entries )
   {
      guessed.push( in_window( entries, function( ) { return in_page.hash_version_for( "20401" ); } ) );
   } );

   check( "the version guessed - none kept: current; kept for the node but no versions: 0; another's kept: current; as kept",
    guessed, [ 1, 1, 0, 1, 0, 1, 1 ] );

   // NOTE: "_" before a PIN, as "cws.hashed_<pin>" (Ian, 2026-10-25) - one kept the first days' way, with "@", read and moved.
   var moved = { "cws.hashed_20401": "x", "cws.hash_version@20401": "0" };

   var moved_version = in_window( moved, function( ) { return in_page.hash_version_for( "20401" ); } );

   check( "- kept the old way (\"@\" before the PIN): read, and moved to \"_\"", [ moved_version,
    moved[ "cws.hash_version_20401" ], moved.hasOwnProperty( "cws.hash_version@20401" ) ], [ 0, "0", false ] );

   var kept_by = { };

   in_window( kept_by, function( ) { in_page.keep_hash_version( "20401", 1 ); in_page.keep_hash_version( "admin", 1 ); } );

   check( "- kept by PIN alone, as \"cws.hash_version_<pin>\"", kept_by, { "cws.hash_version_20401": "1" } );

   // NOTE: Found by review (2026-10-23) - a PIN with a username after it is looked up by its PIN; and only "[okay]", as text or
   // JSON, is taken as done - an error in JSON had been taken for success.
   var suffixed = in_window( { "cws.hash_version_20401": "0" }, function( ) { return in_page.hash_version_for( "20401:verify-a" ); } );

   check( "- a PIN with a username after it, by its PIN", suffixed, 0 );
   check( "- and with no page, the command line: the current one, Node's own localStorage never touched",
    in_page.hash_version_for( "20401" ), 1 );
   check( "a reply that says done - only \"[okay]\", as text or JSON", [ "[okay]", " [okay]\n", "{\"response\":\"[okay]\"}",
    "Error: Unknown access pin '41281' for user update.", "{\"error\":\"Unknown access pin\"}", "{\"response\":\"[none]\"}", "", null, "{bad" ]
    .map( m.is_okay_response ), [ true, true, true, false, false, false, false, false, false ] );

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

   // NOTE: Stopped by the page - the console's Stop ("ciyam_module_stop( )"): the run let go, nothing kept, the next call afresh.
   var stopped_page = page( );
   var stopped_answers = [ ];

   m.ciyam_module_legacy( harden, stopped_page );

   stopped_page.init_script_value = "secret";

   await stopped_page.call( "ciyam_harden_at_load" );

   stopped_page.ciyam_harden_execute( function( value ) { stopped_answers.push( value ); }, "1:50" );

   await new Promise( function( resolve ) { setTimeout( resolve, 30 ); } );

   var stopped_now = m.ciyam_module_stop( "harden" );

   await new Promise( function( resolve ) { setTimeout( resolve, 100 ); } );

   check( "stopped by the page - nothing said, nothing kept, and the next call afresh", [ stopped_now, stopped_answers.length,
    stopped_page.ciyam_harden_result, ( await stopped_page.call( "ciyam_harden_execute", "" ) ).split( "\n" )[ 0 ], m.ciyam_module_stop( "nothing" ) ],
    [ true, 0, null, "(ready)", false ] );

   // NOTE: Unloaded - out of the registry, and its last instance let go with whatever it held (found by review, 2026-10-16:
   // a primed "harden" kept its password in memory until loaded again).
   var unloaded_page = page( );

   m.ciyam_module_legacy( m.ciyam_module_named( "xxxxx" ), unloaded_page );
   unloaded_page.init_script_value = "kept?";

   await unloaded_page.call( "ciyam_xxxxx_at_load" );

   check( "unloaded - out of the registry, nothing left to stop", [ m.ciyam_unregister_module( "xxxxx" ), m.ciyam_module_stop( "xxxxx" ),
    unloaded_page.ciyam_xxxxx_result ], [ true, false, null ] );

   delete require.cache[ require.resolve( "./ciyam_xxxxx.js" ) ];
   require( "./ciyam_xxxxx.js" );

   heading( "chacha20" );

   global.ciyam_sha256_hex = m.ciyam_sha256_hex;

   require( "./ciyam_chacha20.js" );

   var chacha = m.ciyam_module_named( "chacha20" );
   var digest = require( "crypto" );

   // NOTE: From Ian's own "crypt -cc" (built in the test node, 2026-10-16) - a file of zeros under "test key" comes back as
   // the keystream itself; a message under "pass"; 70000 bytes under "k", past "crypt_stream.cpp"'s 32768-byte buffer.
   check( "as crypt -cc - 200 zero bytes, the keystream itself", chacha.hex_of( await chacha.crypt( "test key", new Uint8Array( 200 ) ) ),
    "f0a96087d5413adfe34ca592275ff992e9cc47512976820a5a3d0668a50a83ea976fc51e738d8f93329768481b7ca7ed2856d04b6f89c9548274e0ea76a1"
    + "4866d267cdc1a38af4090cdc5665304661a1dcb54bad6b6d9bdf37e637a07ccf1d980f3818f06ec8934bce9949d468f67266a38cb95c82758de86a16db"
    + "764bdb4e38a9303c3cc7e6dc1ad41c62a61e52667ebfd922325d0ff3c45cefac18ee7ecfc42df29e8023af5402e0d00e644de41ff0bdb1d2adf7074b98"
    + "1a5137ff47726855a7f520318e70dc21" );
   check( "- a message, and back with the same call", [ chacha.hex_of( await chacha.crypt( "pass",
    new TextEncoder( ).encode( "Hello, CIYAM - a short secure message." ) ) ), new TextDecoder( ).decode( await chacha.crypt( "pass",
    chacha.bytes_of( "a3b47d0962550942d63978a689509a63bbd382fb87bed0f7a965488576ab0b0c58d89e8d8894" ) ) ) ],
    [ "a3b47d0962550942d63978a689509a63bbd382fb87bed0f7a965488576ab0b0c58d89e8d8894", "Hello, CIYAM - a short secure message." ] );
   check( "- 70000 bytes, past its buffer", digest.createHash( "sha256" ).update( await chacha.crypt( "k", new Uint8Array( 70000 ) ) ).digest( "hex" ),
    "9616147e42ec6d3d91000c0ae3a8fa19d8697c6d60ffe79029f7a32a9611de72" );

   // NOTE: RFC 8439, 2.4.2 - its key, nonce and counter, "Ladies and Gentlemen of the class of '99...".
   check( "the standard's own test - RFC 8439", chacha.hex_of( chacha.xor( Uint8Array.from( { length: 32 }, function( v, i ) { return i; } ),
    chacha.bytes_of( "000000000000004a00000000" ), 1, new TextEncoder( ).encode( "Ladies and Gentlemen of the class of '99: If I could offer you "
    + "only one tip for the future, sunscreen would be it." ) ) ), "6e2e359a2568f98041ba0728dd0d6981e97e7aec1d4360c20a27afccfd9fae0bf91b65c5524733ab"
    + "8f593dabcd62b3571639d624e65152ab8f530c359f0861d807ca0dbf500d6a6156a38e088a22b65e52bc514d16ccf806818ce91ab77937365af90bbf74a35be6b40b8eedf2"
    + "785e42874d" );
   check( "through run - '<password> <hex>', and again to decrypt", [ await new chacha( { } ).run( "pass 48656c6c6f" ),
    await new chacha( { } ).run( "pass a3b47d0962" ) ], [ "a3b47d0962", "48656c6c6f" ] );
   check( "- a password alone, or data not hex, said", [ await new chacha( { } ).run( "pass" ).catch( function( e ) { return e.message; } ),
    await new chacha( { } ).run( "pass xyz" ).catch( function( e ) { return e.message; } ) ],
    [ "Give a password and then hex - <password> <hex>", "Not hex - an even number of the digits 0 to 9 and a to f." ] );

   heading( "ed25519" );

   // NOTE: "ed25519.min.js" as a page loads it - in a context of its own, giving the global "ED25519".
   var curves_page = { crypto: globalThis.crypto };

   curves_page.globalThis = curves_page;
   vm.createContext( curves_page );
   vm.runInContext( fs.readFileSync( __dirname + "/ed25519.min.js", "utf-8" ), curves_page );

   global.ED25519 = curves_page.ED25519;

   require( "./ciyam_ed25519.js" );

   var ed = m.ciyam_module_named( "ed25519" );

   // NOTE: RFC 8032, 7.1 - tests 1 and 2: the seed, the message, the public key and the signature.
   var rfc = [ [ "9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60", "",
    "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a",
    "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b" ],
    [ "4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb", "r",
    "3d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c",
    "92a009a9f0d4cab8720e820b5f642540a2b27b5416503f8fb3762223ebdb69da085ac1e43e15996e458f3613d0f11d8c387b2eaeb4302aeeb00d291612bb0c00" ] ];

   check( "the standard's public keys - RFC 8032", rfc.map( function( t ) { return ed.public_key( ed.seed_of( t[ 0 ] ) ); } ),
    [ rfc[ 0 ][ 2 ], rfc[ 1 ][ 2 ] ] );
   check( "- its signatures, verified", rfc.map( function( t )
   {
      return [ ed.sign( ed.seed_of( t[ 0 ] ), t[ 1 ] ) === t[ 3 ], ed.verify( t[ 2 ], t[ 3 ], t[ 1 ] ) ];
   } ), [ [ true, true ], [ true, true ] ] );
   check( "- another message not verified", ed.verify( rfc[ 1 ][ 2 ], rfc[ 1 ][ 3 ], "s" ), false );

   // NOTE: The X25519 secret from tests 1 and 2, as "test_crypto_keys" and the console's "@shared" give it.
   check( "the shared secret - the same from either side, as the console's", [ ed.shared_secret( ed.seed_of( rfc[ 0 ][ 0 ] ), rfc[ 1 ][ 2 ] ),
    ed.shared_secret( ed.seed_of( rfc[ 1 ][ 0 ] ), rfc[ 0 ][ 2 ] ) ],
    [ "5166f24a6918368e2af831a4affadd97af0ac326bdf143596c045967cc00230e", "5166f24a6918368e2af831a4affadd97af0ac326bdf143596c045967cc00230e" ] );
   check( "- a public key of small order refused", ( function( ) { try { return ed.shared_secret( ed.seed_of( rfc[ 0 ][ 0 ] ),
    "0100000000000000000000000000000000000000000000000000000000000000" ); } catch( e ) { return e.message; } } )( ),
    "That public key cannot give a shared secret." );

   // NOTE: A secret read as "private_key" reads it, so the same words give the same key - each from the console's "@secret:ed"
   // and "@public" (2026-10-20): 32 hex digits (as Ian's ".secret" holds - read as base64), a short test secret, base64.
   check( "secrets read as the console reads them", [ ed.public_key( ed.seed_of( "0123456789abcdef0123456789abcdef" ) ),
    ed.public_key( ed.seed_of( "hello" ) ), ed.public_key( ed.seed_of( "nWGxne/9WmC6hEr0kuwsxERJxWl7MmkZcDusAxyuf2A=" ) ) ],
    [ "0028b35f26edb6ffe44d3d2cc597906f4065bd550d5009c4d62c422c789493e0", "06bd950857006e5cf4610e6f9569881bf52c34aa21f57a03940c3d7ff2040849",
    "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a" ] );
   check( "- and a shared secret between two of them, as the console's", ed.shared_secret( ed.seed_of( "0123456789abcdef0123456789abcdef" ),
    "06bd950857006e5cf4610e6f9569881bf52c34aa21f57a03940c3d7ff2040849" ), "812788dd5c3c15a6470e406315ec6e49551767cd9b11af795ec48bf93caa407c" );
   // NOTE: Base64 as "base64.cpp" reads it since Ian's rework (2026-10-20, ISS-049) - a length a multiple of four, only the last
   // two "=" filler - and non-ASCII refused there too (found by review). Each answer, a key or a refusal, the console's own.
   check( "- what the console refuses, refused", [ "", "x".repeat( 52 ), "ab".repeat( 33 ), "A".repeat( 45 ), "é".repeat( 16 ),
    "ABCDEFGH" + "=".repeat( 22 ), "0123456789abcdef0123456789abcdef01", "QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVpBQkNERUY====",
    "A".repeat( 44 ) + "====" ].map( function( s )
   {
      try { ed.seed_of( s ); return "taken"; } catch( e ) { return e.message; }
   } ), [ "That secret gives no key - it comes to nothing.", "A secret of over 50 letters is hex - the digits 0 to 9 and a to f.",
    "A secret in hex is at most 64 digits.", "Not base64 - its length is not a multiple of four.",
    "Not base64 - only its letters, digits, '+', '/' and '='.", "Not base64 - its length is not a multiple of four.",
    "Not base64 - its length is not a multiple of four.", "Not base64 - its length is not a multiple of four.",
    "A secret in base64 is at most 32 bytes." ] );
   check( "- and what it takes with filler, the same keys", [ ed.public_key( ed.seed_of( "ABCDEFGHABCDEFGHABCDEFGHABCDEFGHABCDEFGH====" ) ),
    ed.public_key( ed.seed_of( "ABCDEFGHABCDEFGHABCDEFGHABCDEF==" ) ) ],
    [ "104ec09f88a31693b9d36fcdea3007355334f14c1397ddc4e01d86e8e9844489", "01c96ddd8aa748228fe218198e070388aca95f45b972ca334d7adf1ba6d79efd" ] );

   check( "through run - public, shared, sign and verify", [ await new ed( { } ).run( "public " + rfc[ 0 ][ 0 ] ),
    await new ed( { } ).run( "shared " + rfc[ 0 ][ 0 ] + " " + rfc[ 1 ][ 2 ] ), await new ed( { } ).run( "sign " + rfc[ 1 ][ 0 ] + " r" ),
    await new ed( { } ).run( "verify " + rfc[ 1 ][ 2 ] + " " + rfc[ 1 ][ 3 ] + " r" ),
    await new ed( { } ).run( "verify " + rfc[ 1 ][ 2 ] + " " + rfc[ 1 ][ 3 ] + " s" ) ],
    [ rfc[ 0 ][ 2 ], "5166f24a6918368e2af831a4affadd97af0ac326bdf143596c045967cc00230e", rfc[ 1 ][ 3 ], "valid", "not valid" ] );
   check( "- anything else said", await new ed( { } ).run( "keys" ).catch( function( e ) { return e.message; } ),
    "Use one of - " + ed.about.example );

   heading( "secure_message" );

   // NOTE: It finds "ed25519" and "chacha20" in the registry, as a page's modules do.
   global.ciyam_module_named = m.ciyam_module_named;

   require( "./ciyam_secure_message.js" );

   var sm = m.ciyam_module_named( "secure_message" );
   var hex_bytes = function( hex ) { return ed.bytes_of( hex ); };
   var hmac_hex = async function( key, message ) { return sm.hex_of( await sm.hmac_sha256_bytes( key, message ) ); };

   // NOTE: RFC 4231 - tests 1 and 2, and 6 and 7 with a key longer than a block (the case "hmac_sha256" in "sha256.cpp" got
   // wrong, ISS-051).
   check( "HMAC-SHA256 - the standard's tests, RFC 4231", [ await hmac_hex( new Uint8Array( 20 ).fill( 0x0b ), new TextEncoder( ).encode( "Hi There" ) ),
    await sm.hmac_sha256( "Jefe", "what do ya want for nothing?" ),
    await hmac_hex( new Uint8Array( 131 ).fill( 0xaa ), new TextEncoder( ).encode( "Test Using Larger Than Block-Size Key - Hash Key First" ) ),
    await hmac_hex( new Uint8Array( 131 ).fill( 0xaa ), new TextEncoder( ).encode( "This is a test using a larger than block-size key and a larger "
    + "than block-size data. The key needs to be hashed before being used by the HMAC algorithm." ) ) ],
    [ "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7", "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843",
    "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54", "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2" ] );

   // NOTE: "sha2.min.js" for SHA-256 where the browser has none of its own (plain http) - the same answers, as it hashes a
   // string of character codes as the bytes they are.
   var native_crypto = Object.getOwnPropertyDescriptor( globalThis, "crypto" );

   global.hex_sha256 = sha2_page.hex_sha256;
   var random_only = { getRandomValues: function( b ) { return native_crypto.get.call( globalThis ).getRandomValues( b ); } };

   Object.defineProperty( globalThis, "crypto", { value: random_only, configurable: true, writable: true } );

   var without_native = [ typeof crypto.subtle, await sm.hmac_sha256( "Jefe", "what do ya want for nothing?" ),
    await hmac_hex( new Uint8Array( 131 ).fill( 0xaa ), new TextEncoder( ).encode( "Test Using Larger Than Block-Size Key - Hash Key First" ) ) ];

   Object.defineProperty( globalThis, "crypto", native_crypto );
   delete global.hex_sha256;

   check( "- the same through sha2.min.js, with no SHA-256 of the browser's", without_native,
    [ "undefined", "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843",
    "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54" ] );

   // NOTE: From the console (2026-10-21) - "irc.cin"'s steps for this entropy, salt, padding and message: the own secret, the
   // ciphertext "@crypt:cc" gave, and the HMAC as Node's "crypto.createHmac" gives it (the console's own varies, ISS-051).
   var entropy = "dc4c67f1ab93e4a6bd2ab8f24b9ad6a1";
   var own = "1089ddc7d1607b357870227c5af0d894c487f2aa4cbd54cb8a0920d371ce4d28";
   var known = "X\r00112233445566778899aabbccddeeff\r93d750eaebb1a8c0e4c5288d39e2b2753c204627a2\r1041000cf2485e3a3bb546107a74ddd4";

   check( "the own secret - as the console's, from the entropy and it reversed", sm.own_secret( entropy ), own );
   check( "- entropy not 32 to 64 hex digits, said", [ "abc", "x".repeat( 32 ), "a".repeat( 66 ) ].map( function( s )
   {
      try { return sm.own_secret( s ); } catch( e ) { return e.message; }
   } ), [ "Secret entropy is 32 to 64 hex digits.", "Secret entropy is 32 to 64 hex digits.", "Secret entropy is 32 to 64 hex digits." ] );
   check( "sealed - as the console seals it, given its salt and padding", await sm.seal( own, "Hello Ian",
    { salt: "00112233445566778899aabbccddeeff", before: "Ab3dE", after: "zz9YY" } ), known );
   check( "- and opened", await sm.open( own, known ), "Hello Ian" );

   var sealed = await sm.seal( own, "A line\r\nand another, café" );
   var sealed_parts = sealed.substring( 2 ).split( "\r" );
   var padded = new TextDecoder( ).decode( await m.ciyam_module_named( "chacha20" ).crypt( own + sealed_parts[ 0 ], hex_bytes( sealed_parts[ 1 ] ) ) );

   check( "sealed afresh - salt, ciphertext and HMAC, padded to 502 bytes, opened again with CR as a line break", [ sealed.startsWith( "X\r" ),
    /^[0-9a-f]{32}$/.test( sealed_parts[ 0 ] ), sealed_parts[ 1 ].length, /^[0-9a-f]{32}$/.test( sealed_parts[ 2 ] ),
    /^[0-9A-Za-z]{5,10}\rA line\nand another, café\r[0-9A-Za-z]+$/.test( padded ), await sm.open( own, sealed ) ],
    [ true, true, 1004, true, true, "A line\nand another, café" ] );
   check( "- each time a new salt and padding", ( await sm.seal( own, "same" ) ) !== ( await sm.seal( own, "same" ) ), true );

   var refused = async function( promise ) { try { await promise; return "opened"; } catch( e ) { return e.message; } };
   var changed = known.replace( "\r93d7", "\r93d6" );

   check( "not opened - changed, another secret, not sealed, or too long to seal", [ await refused( sm.open( own, changed ) ),
    await refused( sm.open( sm.own_secret( "0123456789abcdef0123456789abcdef" ), known ) ), await refused( sm.open( own, "Hello" ) ),
    await refused( sm.open( own, known + "\r00" ) ), await refused( sm.seal( own, "x".repeat( 491 ) ) ), await refused( sm.seal( "abc", "hi" ) ),
    await refused( sm.seal( own, "hi", { salt: "XYZ" } ) ) ],
    [ "That message has been changed, or was sealed with another secret.", "That message has been changed, or was sealed with another secret.",
    "That is not a sealed message.", "That is not a sealed message.", "That message is too long to seal - at most 490 bytes.",
    "A secret is 64 hex digits - the own secret, or one with another person.", "A salt is 32 lower-case hex digits, and the padding has no CR." ] );
   check( "- 490 bytes sealed, the most", await sm.open( own, await sm.seal( own, "y".repeat( 490 ) ) ), "y".repeat( 490 ) );

   // NOTE: To another person, as "x" now does (Ian, 2026-10-22) - from the console (2026-10-22): each one's public key, as
   // "irc.cin" shows it to scan, and "@shared" between one's key and the other's public key, the same from either side.
   var other = "0123456789abcdef0123456789abcdef";
   var mine_public = "c14540b4da0eff751f41a87ed680a843efe46d35dd13e84caef04f0ce526ac6b";
   var other_public = "0028b35f26edb6ffe44d3d2cc597906f4065bd550d5009c4d62c422c789493e0";
   var between = "f9d8bbdac733c58643b1db586545d8a049f64da83fc4dd6754d860158e92bb6b";

   check( "to another person - the public keys and the secret between them, as the console's, from either side", [ sm.public_key( entropy ),
    sm.public_key( other ), sm.shared_secret( entropy, other_public ), sm.shared_secret( other, mine_public ) ],
    [ mine_public, other_public, between, between ] );

   var to_other = await sm.seal( sm.shared_secret( entropy, other_public ), "Just for you" );

   check( "- sealed by one, opened by the other, and not under either's own secret", [ await sm.open( sm.shared_secret( other, mine_public ), to_other ),
    await refused( sm.open( own, to_other ) ), await refused( sm.open( sm.own_secret( other ), to_other ) ) ],
    [ "Just for you", "That message has been changed, or was sealed with another secret.",
    "That message has been changed, or was sealed with another secret." ] );
   check( "- a public key not a key, said", await refused( Promise.resolve( ).then( function( ) { return sm.shared_secret( entropy, "abc" ); } ) ),
    "Not hex - an even number of the digits 0 to 9 and a to f." );

   var through_run = await new sm( { } ).run( "seal " + own + " Hello through run" );

   check( "through run - own, seal and open, CRs shown as \\r", [ await new sm( { } ).run( "own " + entropy ),
    /^X\\r[0-9a-f]{32}\\r[0-9a-f]+\\r[0-9a-f]{32}$/.test( through_run ), await new sm( { } ).run( "open " + own + " " + through_run ),
    await new sm( { } ).run( "open " + own + " " + known.replace( /\r/g, "\\r" ) ) ], [ own, true, "Hello through run", "Hello Ian" ] );
   check( "- public and shared", [ await new sm( { } ).run( "public " + entropy ), await new sm( { } ).run( "shared " + entropy + " " + other_public ) ],
    [ mine_public, between ] );
   check( "- anything else said", await new sm( { } ).run( "keys" ).catch( function( e ) { return e.message; } ),
    "Use one of - " + sm.about.example );

   heading( "user_keys" );

   require( "./ciyam_user_keys.js" );

   var uk = m.ciyam_module_named( "user_keys" );
   var node_crypto = require( "crypto" );

   // NOTE: The node's test words (Ian's, public - "rebuild-container.sh") and their entropy, both ways; BIP39's all-zero vector.
   var test_entropy = "de604cee0755a3d81944ea96aed12681";
   var test_words = "taxi act describe attitude focus ugly govern outer note isolate endless act";

   check( "the twelve words for entropy and back - BIP39, as a node's", [ uk.words_of_entropy( test_entropy ), uk.entropy_of_words( test_words ),
    uk.entropy_of_words( "  Taxi act describe attitude focus ugly govern outer note isolate endless   ACT " ),
    uk.words_of_entropy( "00000000000000000000000000000000" ) ], [ test_words, test_entropy, test_entropy,
    "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about" ] );
   check( "- words not right, said", [ "taxi act", test_words.replace( "ugly", "ugli" ), test_words.replace( "taxi", "act" ) ].map( function( w )
   {
      try { return uk.entropy_of_words( w ); } catch( e ) { return e.message; }
   } ), [ "Those are not the words - check each is spelt right and in its place.", "Those are not the words - check each is spelt right and in its place.",
    "Those are not the words - check each is spelt right and in its place." ] );

   // NOTE: Worked out here without the module - "harden"'s rounds (each the SHA-256 of what there is with the salt and password
   // after it), ChaCha20 under the key's SHA-256 with a zero nonce and counter ("crypt -cc"), HMAC-SHA256 keyed "mac" and the key.
   var known_salt = "000102030405060708090a0b0c0d0e0f";
   var known_contents = { entropy: test_entropy, contacts: { "verify-a": "0028b35f26edb6ffe44d3d2cc597906f4065bd550d5009c4d62c422c789493e0" } };
   var known_key = known_salt + "pässwörd";

   for( var r = 0, input = known_salt + "pässwörd"; r < 1000; r++ )
      known_key = node_crypto.createHash( "sha256" ).update( known_key + input, "utf8" ).digest( "hex" );

   var known_stream = node_crypto.createCipheriv( "chacha20", node_crypto.createHash( "sha256" ).update( known_key ).digest( ), Buffer.alloc( 16 ) );
   var known_cipher = known_stream.update( Buffer.from( JSON.stringify( known_contents ), "utf8" ) ).toString( "hex" );
   var known_locked = "UK1:1000:" + known_salt + ":" + known_cipher + ":"
    + node_crypto.createHmac( "sha256", "mac" + known_key ).update( known_cipher ).digest( "hex" );

   check( "locked - as worked out without the module, given a salt and rounds", await uk.lock( "pässwörd", known_contents,
    { salt: known_salt, rounds: 1000 } ), known_locked );
   check( "- and unlocked", await uk.unlock( "pässwörd", known_locked ), known_contents );

   var locked_afresh = await uk.lock( "pässwörd", known_contents );

   check( "locked afresh - its own salt each time, the default rounds, unlocked again", [ locked_afresh.split( ":" )[ 1 ],
    locked_afresh !== ( await uk.lock( "pässwörd", known_contents ) ), await uk.unlock( "pässwörd", locked_afresh ) ],
    [ "100000", true, known_contents ] );

   var not_opened = async function( password, locked ) { try { await uk.unlock( password, locked ); return "opened"; } catch( e ) { return e.message; } };

   var changed_locked = known_locked.replace( ":" + known_cipher.substring( 0, 4 ),
    ":" + ( known_cipher[ 0 ] === "0" ? "1" : "0" ) + known_cipher.substring( 1, 4 ) );

   check( "not unlocked - another password, changed, or not locked keys", [ await not_opened( "passwörd", known_locked ),
    await not_opened( "pässwörd", changed_locked ), await not_opened( "pässwörd", "UK1:1000:" + known_salt ),
    await not_opened( "pässwörd", "UK2" + known_locked.substring( 3 ) ) ],
    [ "That password does not open these keys.", "That password does not open these keys.", "Those are not locked keys.", "Those are not locked keys." ] );

   var not_locked = async function( password, contents )
   {
      try { await uk.lock( password, contents, { rounds: 10 } ); return "locked"; } catch( e ) { return e.message; }
   };

   check( "not locked - no password, entropy not hex, a public key not one", [ await not_locked( "", known_contents ),
    await not_locked( "p", { entropy: "xyz" } ), await not_locked( "p", { entropy: test_entropy, contacts: { someone: "abc" } } ) ],
    [ "A password is needed to lock the keys.", "Secret entropy is 32 to 64 hex digits - whole bytes.", "A public key is 64 hex digits, kept by a name." ] );

   // NOTE: Found by review (2026-10-23) - keys come back from the node, so it cannot ask for hours of hashing; entropy is whole
   // bytes; a password the same in its two Unicode forms ("ä" as one character, or "a" and its two dots).
   var too_many = "UK1:1000001:" + known_salt + ":" + known_cipher + ":" + known_locked.split( ":" )[ 4 ];

   check( "- more rounds than ten times the default refused, before any hashing; odd entropy refused", [ await not_opened( "pässwörd", too_many ),
    await not_locked( "p", { entropy: test_entropy + "a" } ),
    await uk.lock( "p", known_contents, { rounds: 1000001 } ).catch( function( e ) { return e.message; } ) ],
    [ "Those are not locked keys.", "Secret entropy is 32 to 64 hex digits - whole bytes.", "The rounds are a whole number from 1 to 1000000." ] );
   check( "- a password the same whichever Unicode form it was typed in", await uk.unlock( "pässwörd".normalize( "NFD" ), known_locked ), known_contents );

   var locked_by_run = await new uk( { } ).run( "lock pw " + test_entropy );

   check( "through run - lock, unlock, words and entropy", [ /^UK1:100000:[0-9a-f]{32}:[0-9a-f]+:[0-9a-f]{64}$/.test( locked_by_run ),
    await new uk( { } ).run( "unlock pw " + locked_by_run ), await new uk( { } ).run( "words " + test_entropy ),
    await new uk( { } ).run( "entropy " + test_words ) ], [ true, JSON.stringify( { entropy: test_entropy, contacts: { } } ), test_words, test_entropy ] );
   check( "- anything else said", await new uk( { } ).run( "keys" ).catch( function( e ) { return e.message; } ), "Use one of - " + uk.about.example );

   console.log( "" );
   console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

   process.exit( failures === 0 ? 0 : 1 );
} )( );

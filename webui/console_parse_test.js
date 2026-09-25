// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Exercises "console_parse.js" against fixed inputs and prints a deterministic
// transcript. No server and no browser are needed. The "run_script *" fixture is real
// output from the development container.
//
// Run with:  node console_parse_test.js
// Compare against "console_parse_test.tst" - see "run_chat_tests.sh".

const cp = require( "./console_parse.js" );

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

function pick( spec, keys )
{
   var out = { };

   keys.forEach( function( k ) { out[ k ] = spec[ k ]; } );

   return out;
}

// --------------------------------------------------------------------
heading( "session handover messages" );

check( "announce", cp.parse_channel_message( "1790000000001:1790000000002" ),
 { kind: "announce", id: "1790000000001", rest: "1790000000002" } );

check( "owner", cp.parse_channel_message( "1790000000002-1790000000001" ),
 { kind: "owner", id: "1790000000002", rest: "1790000000001" } );

check( "credentials", cp.parse_channel_message( "17900=45679,c5588b,8dd2f1,ddd52e,6e29de,admin,1" ).kind, "credentials" );

check( "session ended", cp.parse_channel_message( "1790000000001" ), { kind: "ended", id: "1790000000001", rest: "" } );

check( "not ours", cp.parse_channel_message( "hello" ), null );
check( "JSON is not ours", cp.parse_channel_message( "{\"kind\":\"entry\"}" ), null );

// NOTE: The ISS-016 shape - a hyphen inside a field must not change the kind.
check( "hyphen after the separator stays credentials",
 cp.parse_channel_message( "17900=45679,c5588b,8dd2f1,ddd52e,6e29de,test-1,0" ).kind, "credentials" );

var creds = cp.parse_credentials( "45679,c5588b,8dd2f1,ddd52e,6e29de,test%2D1,1" );

check( "credential fields", creds,
 { access: "45679", device: "c5588b", hashed: "8dd2f1", sessid: "ddd52e", unique: "6e29de", username: "test-1", is_admin: true } );

check( "harness form without username", cp.parse_credentials( "45679,c5588b,8dd2f1,ddd52e,6e29de" ).username, "" );
check( "too few fields", cp.parse_credentials( "45679,c5588b" ), null );
check( "junk in a fixed field", cp.parse_credentials( "45679,c5<b>,8dd2f1,ddd52e,6e29de" ), null );
check( "undecodable username", cp.parse_credentials( "1,2,3,4,5,%E0%A4%A,0" ).username, "" );

// --------------------------------------------------------------------
heading( "resolving commands" );

var keys = [ "kind", "method", "path", "options" ];

check( "noun then verb", pick( cp.resolve_command( "messages review 0000001" ), keys ),
 { kind: "cws", method: "GET", path: "/messages/0000001", options: "" } );

check( "verb then noun", pick( cp.resolve_command( "review messages 0000001 from=0" ), keys ),
 { kind: "cws", method: "GET", path: "/messages/0000001", options: "from=0" } );

check( "joined form", pick( cp.resolve_command( "messages|review 0000001 from=0" ), keys ),
 { kind: "cws", method: "GET", path: "/messages/0000001", options: "from=0" } );

check( "synonym verb", cp.resolve_command( "list users" ).key, "users|review" );
check( "singular noun", cp.resolve_command( "user delete 20101" ).path, "/users/20101" );

check( "options keep their spaces", cp.resolve_command( "messages create 0000000 text=Design  Review" ).options,
 "text=Design  Review" );

check( "invite", pick( cp.resolve_command( "messages update 0000004 for=tester-1,tester-2" ), keys ),
 { kind: "cws", method: "PUT", path: "/messages/0000004", options: "for=tester-1,tester-2" } );

check( "users create secret", pick( cp.resolve_command( "users create secret" ), keys ),
 { kind: "cws", method: "POST", path: "/users", options: "secret" } );

check( "users create nominated", cp.resolve_command( "users create nominated=20401:verify-a" ).options,
 "nominated=20401:verify-a" );

check( "unlock keys", cp.resolve_command( "unlock-keys create" ).method, "POST" );
check( "status", cp.resolve_command( "status" ).path, "/status" );

check( "missing name", cp.resolve_command( "messages review" ).reason, "'messages review' needs a name" );
check( "options where none are taken", cp.resolve_command( "users review extra" ).reason, "'users review' takes no options" );

check( "raw", cp.resolve_command( "~run_script *" ), { kind: "raw", request: "run_script *" } );
check( "raw with space", cp.resolve_command( "~ variable @irc_allow" ), { kind: "raw", request: "variable @irc_allow" } );
check( "bare tilde", cp.resolve_command( "~" ).kind, "unknown" );

check( "local var", cp.resolve_command( "var room 0000004" ), { kind: "local", name: "var", args: "room 0000004" } );
check( "local help", cp.resolve_command( "help" ), { kind: "local", name: "help", args: "" } );
check( "local is case blind", cp.resolve_command( "CLEAR" ).name, "clear" );

check( "quit", cp.resolve_command( "quit" ), { kind: "quit" } );
check( "exit means quit", cp.resolve_command( "exit" ), { kind: "quit" } );

check( "blank", cp.resolve_command( "   " ), { kind: "none" } );
check( "comment", cp.resolve_command( "# a note" ), { kind: "none" } );

// NOTE: Unlike the harness, an unrecognised command is not sent as raw protocol - "~" is
// required, so a typo cannot become a server command.
check( "unknown is not sent raw", cp.resolve_command( "variable @irc_allow" ), { kind: "unknown", word: "variable" } );

// --------------------------------------------------------------------
heading( "building request URLs" );

var session = { access: "45679", device: "c5588b", sessid: "ddd52e" };

check( "fetch", cp.build_cws_url( "http://h/cws", cp.resolve_command( "messages review 0000001 from=0" ), session ),
 "http://h/cws/messages/0000001?access=45679&device=c5588b&format=text&options=from%3D0&session=ddd52e" );

check( "raw", cp.build_cws_url( "http://h/cws", { path: "", request: "run_script *" }, session ),
 "http://h/cws?access=45679&device=c5588b&format=text&request=run_script%20*&session=ddd52e" );

check( "own name", cp.build_cws_url( "http://h/cws", cp.resolve_command( "users delete ***" ), session ),
 "http://h/cws/users/45679?access=45679&device=c5588b&format=text&session=ddd52e" );

check( "no device yet", cp.build_cws_url( "http://h/cws", { path: "/status" }, { access: "1", device: "", sessid: "s" } ),
 "http://h/cws/status?access=1&format=text&session=s" );

// --------------------------------------------------------------------
heading( "variable names" );

check( "lower case", cp.is_valid_variable_name( "room" ), true );
check( "mixed", cp.is_valid_variable_name( "Room_2" ), true );
check( "upper case is reserved", cp.is_valid_variable_name( "ACCESS" ), false );
check( "upper with digit is allowed", cp.is_valid_variable_name( "ROOM2" ), true );
check( "leading digit", cp.is_valid_variable_name( "2room" ), false );
check( "punctuation", cp.is_valid_variable_name( "ro-om" ), false );
check( "empty", cp.is_valid_variable_name( "" ), false );

// --------------------------------------------------------------------
heading( "scripts" );

check( "steps skip blanks and comments", cp.split_script( "status\n\n# note\n  messages review 0000001  \r\n" ),
 [ { line: 1, text: "status" }, { line: 4, text: "messages review 0000001" } ] );

var listing = [
 "add_archive @path @size @name",
 "backup_export",
 "close_channel @storage @identity",
 "destroy_peerchain_entry @identity",
 "export_app_file @app_directory @list_hash @web_root"
].join( "\n" );

var scripts = cp.parse_script_list( listing );

check( "five scripts", scripts.length, 5 );
check( "arguments", scripts[ 0 ], { name: "add_archive", args: [ "@path", "@size", "@name" ] } );
check( "no arguments", scripts[ 1 ], { name: "backup_export", args: [ ] } );
check( "sorted", scripts.map( function( s ) { return s.name; } ),
 [ "add_archive", "backup_export", "close_channel", "destroy_peerchain_entry", "export_app_file" ] );

check( "bad means none", cp.parse_script_list( "[bad]" ), [ ] );
check( "error means none", cp.parse_script_list( "Error: Script 'x' not found." ), [ ] );

check( "command", cp.build_script_command( "add_archive", { "@path": "/tmp/a", "@size": "10", "@name": "x" } ),
 { command: "run_script !add_archive @path=/tmp/a,@size=10,@name=x" } );

check( "command without arguments", cp.build_script_command( "backup_export", { } ), { command: "run_script !backup_export" } );

check( "comma refused", cp.build_script_command( "add_archive", { "@name": "a,b" } ),
 { error: "The value for '@name' cannot contain a comma." } );

// NOTE: The case that failed in use - unquoted, the server saw four words and answered
// "invalid command usage".
check( "a space quotes the whole list", cp.build_script_command( "irc_add_room", { "@name": "Test Room 2" } ),
 { command: "run_script !irc_add_room \"@name=Test Room 2\"" } );

check( "the whole list, not just the value", cp.build_script_command( "add_archive", { "@path": "/tmp/a", "@name": "my file" } ),
 { command: "run_script !add_archive \"@path=/tmp/a,@name=my file\"" } );

check( "quote refused", cp.build_script_command( "irc_add_room", { "@name": "say \"hi\"" } ),
 { error: "The value for '@name' cannot contain a double quote or a backslash." } );

check( "backslash refused", cp.build_script_command( "add_archive", { "@path": "C:\\tmp" } ),
 { error: "The value for '@path' cannot contain a double quote or a backslash." } );

check( "destroy is destructive", cp.is_destructive_script( "destroy_peerchain_entry" ), true );
check( "backup import is destructive", cp.is_destructive_script( "backup_import" ), true );
check( "backup export is not", cp.is_destructive_script( "backup_export" ), false );

// --------------------------------------------------------------------
heading( "output" );

check( "error", cp.is_error_output( "Error: Unknown access pin." ), true );
check( "bad", cp.is_error_output( "[bad]" ), true );
check( "okay", cp.is_error_output( "[okay]" ), false );

var long = [ ];

for( var i = 1; i <= 45; i++ )
   long.push( "line " + i );

var cut = cp.truncate_lines( long.join( "\n" ), 40 );

check( "head kept", cut.shown.length, 40 );
check( "rest held back", cut.hidden, [ "line 41", "line 42", "line 43", "line 44", "line 45" ] );
check( "short output untouched", cp.truncate_lines( "a\nb", 40 ), { shown: [ "a", "b" ], hidden: [ ] } );

// --------------------------------------------------------------------
heading( "history" );

var history = [ ];

history = cp.push_history( history, "status" );
history = cp.push_history( history, "status" );
history = cp.push_history( history, "  " );
history = cp.push_history( history, "vars" );

check( "repeats and blanks dropped", history, [ "status", "vars" ] );
check( "oldest dropped at the limit", cp.push_history( [ "a", "b", "c" ], "d", 3 ), [ "b", "c", "d" ] );

// --------------------------------------------------------------------
heading( "palette" );

var items = [
 { command: "messages review 0000000", description: "list rooms" },
 { command: "users review", description: "list every account" },
 { command: "messages create 0000000 text=<name>", description: "create a room" }
];

check( "empty query keeps all", cp.filter_palette( items, "" ).length, 3 );

check( "every word must match", cp.filter_palette( items, "messages create" ).map( function( i ) { return i.command; } ),
 [ "messages create 0000000 text=<name>" ] );

check( "command matches rank first", cp.filter_palette( items, "list" ).map( function( i ) { return i.command; } ),
 [ "messages review 0000000", "users review" ] );

check( "description only", cp.filter_palette( items, "rooms" ).length, 1 );

check( "placeholder", cp.first_placeholder( "messages create 0000000 text=<name>" ), { start: 29, end: 35 } );
check( "no placeholder", cp.first_placeholder( "users review" ), null );

// --------------------------------------------------------------------
heading( "storage" );

check( "hashed password shortened", cp.summarise_storage_value( "cws.hashed_45679", "8dd2f1c70a4cf9a1f9be166e604aebdf" ),
 "8dd2f1c70a4cf9a..." );

check( "other values whole", cp.summarise_storage_value( "cws.access", "45679,20101" ), "45679,20101" );

// --------------------------------------------------------------------
heading( "request log entries" );

var t0 = new Date( 2026, 8, 25, 23, 14, 4, 0 );
var t1 = new Date( 2026, 8, 25, 23, 14, 4, 38 );

var entry = cp.make_log_entry( "console", "post",
 "http://localhost:13031/cws/messages/0000000?access=45679&device=c5588b&format=text&options=text%3DDesign%20Review&session=ddd52e",
 null, "0000004-8d88bf9c1b679df29aea5a719ca33fe4", t0, t1 );

check( "entry", entry, {
 source: "console", time: "23:14:04", method: "POST", endpoint: "/cws/messages/0000000",
 request: "text=Design Review", response: "0000004-8d88bf9c1b679df29aea5a719ca33fe4", ok: true, ms: 38 } );

var secret = cp.make_log_entry( "chat", "POST",
 "http://h/cws/devices?access=45679&format=text&passwd=YWRtaW46NDg0&request=%40none", null, "c5588b", t0, t1 );

check( "credentials never kept", /45679|passwd|YWRt|device=/.test( JSON.stringify( secret ) ), false );
check( "request kept", secret.request, "@none" );

check( "error marked", cp.make_log_entry( "console", "GET", "http://h/cws?request=x", null, "[bad]", t0, t1 ).ok, false );

var failed = cp.make_log_entry( "console", "GET", "http://h/cws/status", null, null, t0, t1 );

check( "network failure", [ failed.ok, failed.response ], [ false, "(no response - the request failed)" ] );

check( "posted body kept", cp.make_log_entry( "chat", "POST", "http://h/cws/messages/0000004", "hello", "[okay]", t0, t1 ).request,
 "hello" );

check( "long response cut", /truncated at 20000/.test(
 cp.make_log_entry( "chat", "GET", "http://h/cws/status", null, "x".repeat( 20100 ), t0, t1 ).response ), true );

// --------------------------------------------------------------------
heading( "log capture" );

( async function( )
{
   var quiet = false;

   var logged = [ ];

   var stamps = [ t0, t1, t0, t1 ];

   var fake = {
      fetch: async function( url, type, callback ) { callback( "[okay]" ); },
      post: async function( url, text, callback ) { callback( "posted" ); }
   };

   cp.install_log_capture( fake, "chat", function( ) { return quiet; },
    function( e ) { logged.push( e ); }, function( ) { return stamps.shift( ) || t1; } );

   var seen = "";

   await fake.fetch( "http://h/cws/status?access=1&session=2", "GET", function( r ) { seen = r; } );

   check( "caller still gets the response", seen, "[okay]" );
   check( "request logged", logged.map( function( e ) { return e.endpoint; } ), [ "/cws/status" ] );

   quiet = true;

   await fake.fetch( "http://h/cws/messages/0000001?options=from%3D0", "GET", function( ) { } );

   check( "quiet request not logged", logged.length, 1 );

   quiet = false;

   await fake.post( "http://h/cws/messages/0000004", "hi", function( r ) { seen = r; } );

   check( "post logged with body", [ logged.length, logged[ 1 ].request, seen ], [ 2, "hi", "posted" ] );

   var silent = { fetch: async function( ) { }, post: async function( ) { } };

   var dropped = [ ];

   cp.install_log_capture( silent, "console", function( ) { return false; }, function( e ) { dropped.push( e ); } );

   await silent.fetch( "http://h/cws/status", "GET", function( ) { } );

   check( "a request that never answers is still logged", [ dropped.length, dropped[ 0 ].ok ], [ 1, false ] );

   console.log( "" );
   console.log( failures === 0 ? "All checks passed." : ( failures + " check(s) FAILED." ) );

   process.exit( failures === 0 ? 0 : 1 );
} )( );

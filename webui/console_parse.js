// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Pure functions for the developer console - resolving a typed command to a
// request, the session handover protocol, the request log, scripts, history and the
// palette. Loads in the browser as plain globals and in Node through "module.exports",
// so all of it is covered by "console_parse_test.js" without a server or a browser.

const c_console_raw_prefix = "~";

const c_console_own_name = "***";

const c_console_max_history = 200;

const c_console_log_capacity = 200;

const c_console_max_response_chars = 20000;

const c_console_short_key_length = 17;

const c_console_destructive_prefixes = [ "destroy_", "remove_", "close_", "backup_import" ];

// NOTE: Query parameters that carry credentials. They are never kept in a log entry -
// the log is broadcast between tabs and shown on screen, and none of it is needed to
// read what a request did.
const c_console_secret_params = [ "access", "device", "session", "passwd", "format" ];

// NOTE: Verb synonyms are the ones the harness accepts ("reformatted( )" in
// "test_web_session.js"), so a command that works there works here.
const c_console_verbs =
{
   get: "review", list: "review", read: "review", view: "review", review: "review",
   new: "create", create: "create",
   access: "attach", attach: "attach",
   eval: "employ", exec: "employ", employ: "employ", execute: "employ",
   save: "retain", store: "retain", retain: "retain",
   del: "delete", kill: "delete", erase: "delete", delete: "delete", remove: "delete", destroy: "delete",
   update: "update"
};

const c_console_nouns =
{
   user: "users", users: "users",
   message: "messages", messages: "messages",
   storage: "storages", storages: "storages",
   "unlock-key": "unlock-keys", "unlock-keys": "unlock-keys",
   "storage-module": "storage-modules", "storage-modules": "storage-modules",
   "storage-instance": "storage-instances", "storage-instances": "storage-instances",
   script: "javascripts", scripts: "javascripts", javascript: "javascripts", javascripts: "javascripts",
   style: "stylesheets", styles: "stylesheets", stylesheet: "stylesheets", stylesheets: "stylesheets",
   list: "webcmdlists", lists: "webcmdlists", webcmdlist: "webcmdlists", webcmdlists: "webcmdlists"
};

// NOTE: The CWS routes, as "do_fetch( )" in "test_web_session.js" maps them. "name" is a
// path segment after the noun; "options" is everything after that, sent as "options=".
const c_console_routes =
{
   "users|review": { method: "GET", name: false, options: false },
   "users|create": { method: "POST", name: false, options: true },
   "users|delete": { method: "DELETE", name: true, options: false },
   "users|update": { method: "PUT", name: true, options: true },

   "messages|review": { method: "GET", name: true, options: true },
   "messages|create": { method: "POST", name: true, options: true },
   "messages|update": { method: "PUT", name: true, options: true },
   "messages|delete": { method: "DELETE", name: true, options: false },

   "storages|attach": { method: "POST", name: true, options: false },
   "storages|review": { method: "GET", name: true, options: false },

   "unlock-keys|create": { method: "POST", name: false, options: true },
   "unlock-keys|employ": { method: "POST", name: true, options: false },

   "javascripts|review": { method: "GET", name: true, options: false },
   "stylesheets|review": { method: "GET", name: true, options: false },
   "webcmdlists|review": { method: "GET", name: true, options: false },

   "javascripts|delete": { method: "DELETE", name: false, options: false },
   "stylesheets|delete": { method: "DELETE", name: false, options: false },
   "webcmdlists|delete": { method: "DELETE", name: false, options: false },

   "storage-modules|review": { method: "GET", name: true, options: false },
   "storage-instances|review": { method: "GET", name: true, options: true }
};

const c_console_local_commands = [ "help", "clear", "vars", "var", "unset", "echo", "seed", "history", "wait", "run" ];

const c_console_quit_names = [ "quit", "exit", "finish" ];

// ====================================================================
// Session handover
// ====================================================================

// NOTE: The protocol spoken on the "test_web_channel" BroadcastChannel:
//
//   "<target>:<viewer>"   viewer asks target for its session
//   "<viewer>-<owner>"    target names itself as the owner
//   "<viewer>=<fields>"   target hands over its credentials
//   "<id>"                session ended - unlink
//
// The ids are always digits, so the separator is simply the first character that is
// not one. That is sturdier than the harness's approach of searching for ":", then "-",
// then "=" anywhere in the message, which let a hyphen inside a credential field divert
// the whole message (ISS-016).
function parse_channel_message( data )
{
   var text = String( data );

   if( /^\d+$/.test( text ) )
      return { kind: "ended", id: text, rest: "" };

   var match = /^(\d+)([:=-])([\s\S]*)$/.exec( text );

   if( match === null )
      return null;

   var kinds = { ":": "announce", "-": "owner", "=": "credentials" };

   return { kind: kinds[ match[ 2 ] ], id: match[ 1 ], rest: match[ 3 ] };
}

function decode_field( value )
{
   try
   {
      return decodeURIComponent( String( value || "" ) );
   }
   catch( e )
   {
      return "";
   }
}

function parse_credentials( rest )
{
   var fields = String( rest || "" ).split( "," );

   if( fields.length < 5 )
      return null;

   for( var i = 0; i < 5; i++ )
   {
      if( !/^[0-9A-Za-z]*$/.test( fields[ i ] ) )
         return null;
   }

   return {
      access: fields[ 0 ],
      device: fields[ 1 ],
      hashed: fields[ 2 ],
      sessid: fields[ 3 ],
      unique: fields[ 4 ],
      username: ( fields.length > 5 ) ? decode_field( fields[ 5 ] ) : "",
      is_admin: ( fields.length > 6 ) ? ( fields[ 6 ] === "1" ) : false
   };
}

// ====================================================================
// Commands
// ====================================================================

function split_words( text )
{
   return String( text || "" ).trim( ).split( /\s+/ ).filter( function( w ) { return w !== ""; } );
}

// NOTE: Accepts the noun first ("messages review 0000001", as the Node CLI in "ciyam.js"
// does), the verb first ("review messages 0000001", as the harness does) or the joined
// form ("messages|review 0000001"). Returns "" when the words are not a CWS route.
function route_key( first, second )
{
   var pos = first.indexOf( "|" );

   if( pos > 0 )
   {
      var joined_noun = c_console_nouns[ first.substr( 0, pos ).toLowerCase( ) ];
      var joined_verb = c_console_verbs[ first.substring( pos + 1 ).toLowerCase( ) ];

      if( joined_noun && joined_verb && c_console_routes[ joined_noun + "|" + joined_verb ] )
         return joined_noun + "|" + joined_verb;

      return "";
   }

   var a = String( first || "" ).toLowerCase( );
   var b = String( second || "" ).toLowerCase( );

   var candidates = [ ];

   if( c_console_nouns[ a ] && c_console_verbs[ b ] )
      candidates.push( c_console_nouns[ a ] + "|" + c_console_verbs[ b ] );

   if( c_console_verbs[ a ] && c_console_nouns[ b ] )
      candidates.push( c_console_nouns[ b ] + "|" + c_console_verbs[ a ] );

   for( var i = 0; i < candidates.length; i++ )
   {
      if( c_console_routes[ candidates[ i ] ] )
         return candidates[ i ];
   }

   return "";
}

// NOTE: Turns one typed line into what to do with it. Nothing here touches the network;
// the caller builds the URL with "build_cws_url( )" once credentials are known.
//
//   { kind: "none" }                          blank line or a comment
//   { kind: "local", name, args }             handled by the console itself
//   { kind: "raw", request }                  "~<command>" - raw CIYAM protocol
//   { kind: "cws", key, method, path, name, options }
//   { kind: "quit" }                          end the session
//   { kind: "unknown", word }                 nothing recognised it
function resolve_command( line )
{
   var text = String( line || "" ).trim( );

   if( ( text === "" ) || ( text.charAt( 0 ) === "#" ) )
      return { kind: "none" };

   if( text.charAt( 0 ) === c_console_raw_prefix )
   {
      var raw = text.substring( 1 ).trim( );

      if( raw === "" )
         return { kind: "unknown", word: c_console_raw_prefix };

      return { kind: "raw", request: raw };
   }

   var words = split_words( text );

   var first = words[ 0 ].toLowerCase( );

   if( c_console_local_commands.indexOf( first ) >= 0 )
   {
      var space = text.search( /\s/ );

      return { kind: "local", name: first, args: ( space > 0 ) ? text.substring( space ).trim( ) : "" };
   }

   if( c_console_quit_names.indexOf( first ) >= 0 )
      return { kind: "quit" };

   if( ( first === "status" ) && ( words.length === 1 ) )
      return { kind: "cws", key: "status", method: "GET", path: "/status", name: "", options: "" };

   var joined = ( words[ 0 ].indexOf( "|" ) > 0 );

   var key = route_key( words[ 0 ], joined ? "" : ( words[ 1 ] || "" ) );

   if( key === "" )
      return { kind: "unknown", word: words[ 0 ] };

   var route = c_console_routes[ key ];

   var rest = words.slice( joined ? 1 : 2 );

   var name = "";

   if( route.name && ( rest.length > 0 ) )
      name = rest.shift( );

   // NOTE: Options keep their internal spaces - "text=Design Review" is one option.
   var options = "";

   if( route.options && ( rest.length > 0 ) )
   {
      var skip = ( joined ? 1 : 2 ) + ( name !== "" ? 1 : 0 );

      var remainder = text;

      for( var i = 0; i < skip; i++ )
         remainder = remainder.replace( /^\s*\S+/, "" );

      options = remainder.trim( );
   }
   else if( rest.length > 0 )
      return { kind: "unknown", word: words[ 0 ], reason: "'" + key.replace( "|", " " ) + "' takes no options" };

   if( route.name && ( name === "" ) )
      return { kind: "unknown", word: words[ 0 ], reason: "'" + key.replace( "|", " " ) + "' needs a name" };

   var noun = key.substr( 0, key.indexOf( "|" ) );

   return {
      kind: "cws",
      key: key,
      method: route.method,
      path: "/" + noun + ( name !== "" ? "/" + name : "" ),
      name: name,
      options: options
   };
}

// NOTE: "own" stands in for the caller's access where the harness used "***".
function build_cws_url( base, spec, session )
{
   var path = spec.path;

   if( spec.name === c_console_own_name )
      path = path.substr( 0, path.length - c_console_own_name.length ) + session.access;

   var url = base + path + "?access=" + session.access;

   if( session.device !== "" )
      url += "&device=" + session.device;

   url += "&format=text";

   if( spec.options )
      url += "&options=" + encodeURIComponent( spec.options );

   if( spec.request )
      url += "&request=" + encodeURIComponent( spec.request );

   if( !spec.no_session )
      url += "&session=" + session.sessid;

   return url;
}

// NOTE: The rule the harness applies in "do_fetch( )": a name starts with a letter and
// contains only letters, digits and underscores - and all upper case is reserved for the
// session values ("ACCESS", "DEVICE" and so on).
function is_valid_variable_name( name )
{
   var text = String( name || "" );

   if( !/^[A-Za-z][A-Za-z0-9_]*$/.test( text ) )
      return false;

   return /[a-z0-9_]/.test( text );
}

// ====================================================================
// Scripts
// ====================================================================

// NOTE: A saved script is run line by line. Blank lines and "#" comments are skipped but
// the line numbers kept, so an error can say which line of the body it came from.
function split_script( body )
{
   var lines = String( body || "" ).split( /\r?\n/ );

   var steps = [ ];

   for( var i = 0; i < lines.length; i++ )
   {
      var text = lines[ i ].trim( );

      if( ( text === "" ) || ( text.charAt( 0 ) === "#" ) )
         continue;

      steps.push( { line: i + 1, text: text } );
   }

   return steps;
}

// NOTE: "run_script *" answers one script per line as "name @arg1 @arg2". Anything that
// does not look like that - an error, or "[bad]" when raw protocol is refused - yields an
// empty list rather than a list of nonsense.
function parse_script_list( response )
{
   var text = String( response || "" );

   if( ( text.indexOf( "Error: " ) === 0 ) || ( text.trim( ) === "[bad]" ) )
      return [ ];

   var scripts = [ ];

   text.split( /\r?\n/ ).forEach( function( line )
   {
      var words = split_words( line );

      if( ( words.length === 0 ) || !/^[a-z][a-z0-9_]*$/.test( words[ 0 ] ) )
         return;

      var args = words.slice( 1 ).filter( function( w ) { return /^@[a-z0-9_]+$/.test( w ); } );

      if( args.length !== ( words.length - 1 ) )
         return;

      scripts.push( { name: words[ 0 ], args: args } );
   } );

   scripts.sort( function( lhs, rhs ) { return ( lhs.name < rhs.name ) ? -1 : ( lhs.name > rhs.name ) ? 1 : 0; } );

   return scripts;
}

// NOTE: "!" runs the script synchronously and returns its result. Arguments are comma
// separated on the server, so a value containing a comma cannot be passed - that is
// refused here rather than silently split into two arguments.
//
// "run_script" takes exactly two words, the name and the list, and the server splits the
// line on whitespace - so a list with a space in it must be one double-quoted word. Quotes
// only count at the start of a word ("setup_arguments( )" in "utilities.cpp"): written as
// @name="Test Room" they would stay in the value. A quote or backslash inside a value would
// need escaping, so those are refused too.
function build_script_command( name, values )
{
   var pairs = [ ];

   var keys = Object.keys( values || { } );

   for( var i = 0; i < keys.length; i++ )
   {
      var value = String( values[ keys[ i ] ] );

      if( value.indexOf( "," ) >= 0 )
         return { error: "The value for '" + keys[ i ] + "' cannot contain a comma." };

      if( /["\\]/.test( value ) )
         return { error: "The value for '" + keys[ i ] + "' cannot contain a double quote or a backslash." };

      pairs.push( keys[ i ] + "=" + value );
   }

   var list = pairs.join( "," );

   if( /\s/.test( list ) )
      list = "\"" + list + "\"";

   return { command: "run_script !" + name + ( list !== "" ? " " + list : "" ) };
}

function is_destructive_script( name )
{
   var text = String( name || "" );

   return c_console_destructive_prefixes.some( function( prefix ) { return text.indexOf( prefix ) === 0; } );
}

// ====================================================================
// Output
// ====================================================================

function is_error_output( text )
{
   var value = String( text || "" );

   return ( value.indexOf( "Error: " ) === 0 ) || ( value.trim( ) === "[bad]" );
}

// NOTE: A response of hundreds of lines would bury the scrollback it was just written to,
// so only the head is shown and the rest sits behind an expander.
function truncate_lines( text, limit )
{
   var lines = String( text || "" ).split( /\r?\n/ );

   if( lines.length <= limit )
      return { shown: lines, hidden: [ ] };

   return { shown: lines.slice( 0, limit ), hidden: lines.slice( limit ) };
}

function push_history( history, line, max )
{
   var text = String( line || "" ).trim( );

   var list = history.slice( );

   if( ( text === "" ) || ( list[ list.length - 1 ] === text ) )
      return list;

   list.push( text );

   var limit = max || c_console_max_history;

   if( list.length > limit )
      list = list.slice( list.length - limit );

   return list;
}

// NOTE: Every word typed must appear in the command or its description. Matches on the
// command itself rank ahead of matches only in the description.
function filter_palette( items, query )
{
   var words = split_words( String( query || "" ).toLowerCase( ) );

   if( words.length === 0 )
      return items.slice( );

   var scored = [ ];

   items.forEach( function( item, index )
   {
      var command = String( item.command ).toLowerCase( );
      var haystack = command + " " + String( item.description || "" ).toLowerCase( );

      for( var i = 0; i < words.length; i++ )
      {
         if( haystack.indexOf( words[ i ] ) < 0 )
            return;
      }

      var in_command = words.every( function( w ) { return command.indexOf( w ) >= 0; } );

      scored.push( { item: item, rank: ( in_command ? 0 : 1 ), index: index } );
   } );

   scored.sort( function( lhs, rhs ) { return ( lhs.rank - rhs.rank ) || ( lhs.index - rhs.index ); } );

   return scored.map( function( s ) { return s.item; } );
}

// NOTE: The first "<placeholder>" in a palette command, so it can be selected on insert
// and typed straight over.
function first_placeholder( command )
{
   var match = /<[^>]*>/.exec( String( command || "" ) );

   if( match === null )
      return null;

   return { start: match.index, end: match.index + match[ 0 ].length };
}

// NOTE: A saved hashed password is shown truncated, as "get_all_variables( )" in
// "ciyam.js" does - it is a credential for this device.
function summarise_storage_value( key, value )
{
   var text = String( value === null ? "" : value );

   if( ( String( key ).indexOf( "cws.hashed_" ) === 0 ) && ( text.length > 15 ) )
      return text.substr( 0, 15 ) + "...";

   return text;
}

// ====================================================================
// Request log
// ====================================================================

function two_digits( n )
{
   return ( n < 10 ? "0" : "" ) + n;
}

function format_clock( date )
{
   return two_digits( date.getHours( ) ) + ":" + two_digits( date.getMinutes( ) ) + ":" + two_digits( date.getSeconds( ) );
}

// NOTE: What the log keeps of one request. The credentials in the query are dropped here,
// where the entry is made, rather than hidden when it is shown - entries are passed
// between tabs, and nothing downstream should ever have held them.
function make_log_entry( source, method, url, body, response, started, finished )
{
   var endpoint = String( url || "" );
   var request = "";

   var pos = endpoint.indexOf( "/cws" );

   if( pos < 0 )
      pos = endpoint.search( /\/[a-z-]+(\?|$)/ );

   if( pos > 0 )
      endpoint = endpoint.substring( pos );

   var query = "";

   var qpos = endpoint.indexOf( "?" );

   if( qpos >= 0 )
   {
      query = endpoint.substring( qpos + 1 );
      endpoint = endpoint.substr( 0, qpos );
   }

   var kept = [ ];

   query.split( "&" ).forEach( function( pair )
   {
      if( pair === "" )
         return;

      var eq = pair.indexOf( "=" );

      var name = ( eq < 0 ) ? pair : pair.substr( 0, eq );
      var value = ( eq < 0 ) ? "" : decode_field( pair.substring( eq + 1 ) );

      if( c_console_secret_params.indexOf( name ) >= 0 )
         return;

      if( ( name === "options" ) || ( name === "request" ) )
         request = value;
      else
         kept.push( name + "=" + value );
   } );

   if( kept.length > 0 )
      endpoint += "?" + kept.join( "&" );

   if( ( body !== null ) && ( body !== undefined ) && ( String( body ) !== "" ) )
      request = ( request !== "" ? request + "\n" : "" ) + String( body );

   var text = ( response === null ) ? "" : String( response );

   var failed = ( response === null ) || is_error_output( text );

   if( text.length > c_console_max_response_chars )
      text = text.substr( 0, c_console_max_response_chars ) + "\n(truncated at " + c_console_max_response_chars + " characters)";

   return {
      source: source,
      time: format_clock( started ),
      method: String( method || "GET" ).toUpperCase( ),
      endpoint: endpoint,
      request: request,
      response: ( response === null ) ? "(no response - the request failed)" : text,
      ok: !failed,
      ms: Math.max( 0, finished.getTime( ) - started.getTime( ) )
   };
}

// NOTE: Every request goes through "CIYAM.fetch" or "CIYAM.post", so wrapping those two on
// an instance captures all of its traffic - connect, polling, everything. "is_quiet" is
// asked at the moment each request starts, which is how polling is kept out: excluded where
// the entry is made, not filtered afterwards.
function install_log_capture( instance, source, is_quiet, on_entry, p_now )
{
   var now = p_now || function( ) { return new Date( ); };

   var original_fetch = instance.fetch.bind( instance );
   var original_post = instance.post.bind( instance );

   function capture( method, url, body, run, callback )
   {
      if( is_quiet( ) )
         return run( callback );

      var started = now( );

      var answered = false;

      return run( function( response )
      {
         answered = true;

         on_entry( make_log_entry( source, method, url, body, response, started, now( ) ) );

         if( callback )
            callback( response );
      } ).then( function( value )
      {
         // NOTE: "CIYAM.fetch" swallows a network failure and never calls back, so the
         // entry would otherwise simply be missing.
         if( !answered )
            on_entry( make_log_entry( source, method, url, body, null, started, now( ) ) );

         return value;
      } );
   }

   instance.fetch = function( url, request_type, callback )
   {
      return capture( request_type, url, null, function( cb ) { return original_fetch( url, request_type, cb ); }, callback );
   };

   instance.post = function( url, text, callback )
   {
      return capture( "POST", url, text, function( cb ) { return original_post( url, text, cb ); }, callback );
   };
}

// NOTE: Exported for Node.js; in the browser these are plain globals.
if( typeof module !== "undefined" )
{
   module.exports = {
      parse_channel_message: parse_channel_message,
      parse_credentials: parse_credentials,
      resolve_command: resolve_command,
      build_cws_url: build_cws_url,
      is_valid_variable_name: is_valid_variable_name,
      split_script: split_script,
      parse_script_list: parse_script_list,
      build_script_command: build_script_command,
      is_destructive_script: is_destructive_script,
      is_error_output: is_error_output,
      truncate_lines: truncate_lines,
      push_history: push_history,
      filter_palette: filter_palette,
      first_placeholder: first_placeholder,
      summarise_storage_value: summarise_storage_value,
      format_clock: format_clock,
      make_log_entry: make_log_entry,
      install_log_capture: install_log_capture
   };
}

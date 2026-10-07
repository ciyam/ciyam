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

// NOTE: Query parameters that carry credentials. They are never kept in a log entry -
// the log is broadcast between tabs and shown on screen, and none of it is needed to
// read what a request did.
const c_console_secret_params = [ "access", "device", "session", "passwd", "format" ];

// NOTE: Of those, the ones that are credentials - "format" is only left out of what is shown.
const c_console_credential_params = [ "access", "device", "session", "passwd" ];

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
   device: "devices", devices: "devices",
   "storage-module": "storage-modules", "storage-modules": "storage-modules",
   "storage-instance": "storage-instances", "storage-instances": "storage-instances",
   script: "javascripts", scripts: "javascripts", javascript: "javascripts", javascripts: "javascripts",
   style: "stylesheets", styles: "stylesheets", stylesheet: "stylesheets", stylesheets: "stylesheets",
   list: "webcmdlists", lists: "webcmdlists", webcmdlist: "webcmdlists", webcmdlists: "webcmdlists",
   log: "logs", logs: "logs"
};

// NOTE: The CWS routes, as "do_fetch( )" in "test_web_session.js" maps them. "name" is a
// path segment after the noun - required, or "optional" where the server lists what is
// available without one ("review javascript[s] [<name>]" in its help); "options" is
// everything after that, sent as "options=".
const c_console_routes =
{
   "users|review": { method: "GET", name: false, options: false },
   "users|create": { method: "POST", name: false, options: true },
   "users|delete": { method: "DELETE", name: true, options: false },
   "users|update": { method: "PUT", name: true, options: true },

   // NOTE: Ian, 2026-10-06 - this PIN's own devices, with a session: the list, or one removed (and its
   // session ended) - never the one asking.
   "devices|review": { method: "GET", name: false, options: false },
   "devices|delete": { method: "DELETE", name: true, options: false },

   "messages|review": { method: "GET", name: true, options: true },
   "messages|create": { method: "POST", name: true, options: true },
   "messages|update": { method: "PUT", name: true, options: true },
   "messages|delete": { method: "DELETE", name: true, options: false },

   "storages|attach": { method: "POST", name: true, options: false },
   "storages|review": { method: "GET", name: "optional", options: false },

   "unlock-keys|create": { method: "POST", name: false, options: true },
   "unlock-keys|employ": { method: "POST", name: true, options: false },

   "javascripts|review": { method: "GET", name: "optional", options: false },
   "stylesheets|review": { method: "GET", name: "optional", options: false },
   "webcmdlists|review": { method: "GET", name: "optional", options: false },

   // NOTE: Ian, 2026-10-03 - admin only on the server: the log names, or one whole log.
   "logs|review": { method: "GET", name: "optional", options: false },

   "javascripts|delete": { method: "DELETE", name: false, options: false },
   "stylesheets|delete": { method: "DELETE", name: false, options: false },
   "webcmdlists|delete": { method: "DELETE", name: false, options: false },

   "storage-modules|review": { method: "GET", name: "optional", options: false },
   "storage-instances|review": { method: "GET", name: true, options: true }
};

const c_console_local_commands = [ "help", "clear", "vars", "var", "unset", "echo", "seed", "history", "wait", "run", "exec", "ntfy" ];

// NOTE: The ntfy server as the phones reach it - kept in this browser, as only the person setting up
// knows it: the node sends to its own name for ntfy (https://ntfy in the container), which no phone can use.
const c_ntfy_server_key = "cws.ntfy_server";

const c_console_quit_names = [ "quit", "exit", "finish" ];

// NOTE: The harness's "remove creds" and "retain creds", in any of its word orders - "remove
// creds", "creds remove", "creds|remove" - with "delete" meaning "remove".
const c_creds_nouns = [ "creds", "credentials" ];
const c_creds_verbs = { remove: "remove", delete: "remove", retain: "retain" };

const c_creds_pin_length = 5;

const c_creds_hashed_prefix = "cws.hashed_";

// NOTE: Every key that belongs to one account on this browser - "cws.<name>_<pin>", the
// harness's "arbitrary" value included. Removing an account completely takes all of them.
const c_creds_account_prefixes = [ c_creds_hashed_prefix, "cws.dismissed_", "cws.emoji_recent_", "cws.arbitrary_" ];

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

// NOTE: Which page a linked console shares its session with - "from=accounts" in its address when
// the accounts page opened it, nothing when the chat did. "name" and "subject" (to start a sentence)
// are for messages, "source" and "label" for the Log's filter: each page tags what it logs with its
// own source.
function linked_owner( from )
{
   if( from === "accounts" )
      return { name: "the accounts page", subject: "The accounts page", source: "account", label: "Accounts" };

   return { name: "the chat", subject: "The chat", source: "chat", label: "Chat" };
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

   var creds = parse_creds_command( text );

   if( creds !== null )
   {
      if( creds.error !== "" )
         return { kind: "unknown", word: creds.verb, reason: creds.error };

      return { kind: "local", name: "creds", args: "", creds: creds };
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

   if( ( route.name === true ) && ( name === "" ) )
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

// ====================================================================
// Saved credentials
// ====================================================================

function creds_verb( word )
{
   var key = String( word || "" ).toLowerCase( );

   return Object.prototype.hasOwnProperty.call( c_creds_verbs, key ) ? c_creds_verbs[ key ] : "";
}

function is_creds_noun( word )
{
   return c_creds_nouns.indexOf( String( word || "" ).toLowerCase( ) ) >= 0;
}

// NOTE: "remove creds [<pin>] [partial]" or "retain creds [partial]" - null for any other
// line. "partial" keeps the PIN in the saved list and deals only with the password hash.
function parse_creds_command( text )
{
   var words = split_words( text );

   if( words.length === 0 )
      return null;

   var verb = "";
   var rest = [ ];

   var pipe = words[ 0 ].indexOf( "|" );

   if( pipe > 0 )
   {
      if( is_creds_noun( words[ 0 ].substr( 0, pipe ) ) )
         verb = creds_verb( words[ 0 ].substring( pipe + 1 ) );

      rest = words.slice( 1 );
   }
   else if( words.length > 1 )
   {
      if( creds_verb( words[ 0 ] ) && is_creds_noun( words[ 1 ] ) )
         verb = creds_verb( words[ 0 ] );
      else if( is_creds_noun( words[ 0 ] ) && creds_verb( words[ 1 ] ) )
         verb = creds_verb( words[ 1 ] );

      rest = words.slice( 2 );
   }

   if( verb === "" )
      return null;

   var result = { verb: verb, pin: "", partial: false, error: "" };

   for( var i = 0; i < rest.length; i++ )
   {
      var word = rest[ i ].toLowerCase( );

      if( ( word === "partial" ) && !result.partial )
         result.partial = true;
      else if( ( verb === "remove" ) && ( result.pin === "" )
       && /^\d+$/.test( word ) && ( word.length === c_creds_pin_length ) )
         result.pin = word;
      else
      {
         result.error = ( verb === "remove" )
          ? "'remove creds' takes a " + c_creds_pin_length + "-digit PIN and 'partial', both optional"
          : "'retain creds' takes only 'partial'";

         break;
      }
   }

   return result;
}

// NOTE: What "remove creds" does to this browser's saved data, worked out without touching
// it. "keys" are the localStorage keys present and "list" the stored "cws.access" value.
// Returns the new list (null to remove the key, undefined to leave it alone), the keys to
// remove and the line to print - or an error. Relies on "parse_access_list( )" and
// "format_access_list( )" from "chat_parse.js", which the console loads.
function plan_creds_removal( keys, list, access, partial )
{
   if( access === "" )
      return { error: "Error: Not signed in - name the account: remove creds <pin>" };

   var hashed_key = c_creds_hashed_prefix + access;

   if( partial )
   {
      if( keys.indexOf( hashed_key ) < 0 )
         return { error: "Error: No saved password for '" + access + "'." };

      return { list: undefined, remove: [ hashed_key ], set: { }, message: "(removed credentials partially for " + access + ")" };
   }

   var remove = c_creds_account_prefixes
    .map( function( prefix ) { return prefix + access; } )
    .filter( function( key ) { return keys.indexOf( key ) >= 0; } );

   var entries = parse_access_list( list );

   var pos = entries.indexOf( access );

   if( ( pos < 0 ) && ( remove.length === 0 ) )
      return { error: "Error: Unable to find credentials for '" + access + "'." };

   var new_list = undefined;

   if( pos >= 0 )
   {
      entries.splice( pos, 1 );

      new_list = format_access_list( entries );
   }

   return { list: new_list, remove: remove, set: { }, message: "(removed credentials completely for " + access + ")" };
}

// NOTE: What "retain creds" does - saves the signed in account's PIN, and its password hash
// unless "partial" (which also drops a hash saved before). A tab that holds no hash - one
// signed in some other way - can save only the PIN, and says so.
function plan_creds_retain( list, access, hashed, partial )
{
   if( access === "" )
      return { error: "Error: Not signed in." };

   var entries = parse_access_list( list );

   if( entries.indexOf( access ) < 0 )
      entries.push( access );

   var hashed_key = c_creds_hashed_prefix + access;

   if( partial || ( hashed === "" ) )
   {
      return {
         list: format_access_list( entries ),
         remove: [ hashed_key ],
         set: { },
         message: "(retained credentials partially for " + access + ( partial ? "" : " - no password hash is held here" ) + ")"
      };
   }

   var set = { };

   set[ hashed_key ] = hashed;

   return { list: format_access_list( entries ), remove: [ ], set: set, message: "(retained credentials completely for " + access + ")" };
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

   // NOTE: What is saved - "retain javascript" and "retain webcmdlist" take it as "payload=", as the
   // harness sends it.
   if( spec.payload )
      url += "&payload=" + encodeURIComponent( spec.payload );

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

// ====================================================================
// The list language - Ian's ".list" files, as "test_web_session.js" runs them
// ====================================================================

// NOTE: A line is substituted first ("ciyam.replace_variables"), so "?{name} ..." has become
// "?<value> ..." - or a bare "? ..." when "name" is unset. "?" runs the rest of the line only
// if the value was there, "!" only if it was not. As in the harness, a "?" guard is applied
// and then a "!" guard, each at most once.
function apply_line_guard( text )
{
   var line = String( text || "" );

   var guards = [ "?", "!" ];

   for( var i = 0; i < guards.length; i++ )
   {
      if( line.charAt( 0 ) !== guards[ i ] )
         continue;

      var space = line.indexOf( " " );

      if( space <= 0 )
         continue;

      var bare = ( space === 1 );

      var run = ( guards[ i ] === "?" ) ? !bare : bare;

      if( !run )
         return { run: false, text: "" };

      line = line.substring( space + 1 );
   }

   return { run: true, text: line };
}

// NOTE: The forms of "var", from the harness:
//
//   var <name>                      show it - and add it to the output
//   var <name> <text>               set it; an empty text removes it
//   var !<name> <text>              set it only if it is not already set
//   var #<name> substr:<start>[,<length>]
//                                   add part of it to the output; the variable is unchanged
//   var @<name> null                remove it
//   var @<name> <global>            set it from a server javascript's result
//
// Returns { kind, ... } for the console to act on, or { kind: "error", message }.
function parse_var_command( args )
{
   var text = String( args || "" ).trim( );

   if( text === "" )
      return { kind: "error", message: "Usage is var <name> [<text>]" };

   var space = text.search( /\s/ );

   if( space < 0 )
      return { kind: "show", name: text };

   var name = text.substr( 0, space );
   var value = text.substring( space ).trim( );

   var only_if_unset = false;

   if( name.charAt( 0 ) === "!" )
   {
      only_if_unset = true;
      name = name.substring( 1 );
   }

   if( name.charAt( 0 ) === "#" )
   {
      name = name.substring( 1 );

      var match = /^substr:(-?\d+)(?:,(\d+))?$/.exec( value );

      if( match === null )
         return { kind: "error", message: "Invalid or unknown variable function information '" + value + "'" };

      return { kind: "substr", name: name, start: parseInt( match[ 1 ], 10 ),
       length: ( match[ 2 ] === undefined ) ? null : parseInt( match[ 2 ], 10 ) };
   }

   if( name.charAt( 0 ) === "@" )
   {
      name = name.substring( 1 );

      if( value !== "null" )
         return { kind: "from_script", name: name, source: value, only_if_unset: only_if_unset };

      value = "";
   }

   if( !is_valid_variable_name( name ) )
      return { kind: "error", message: "Invalid variable name '" + name + "' - start with a letter, use letters, digits and _, and not all upper case" };

   return { kind: ( value === "" ) ? "remove" : "set", name: name, value: value, only_if_unset: only_if_unset };
}

// NOTE: "substr:<start>,<length>" as the harness does it - JavaScript's own "substring" and
// "substr" on the value.
function substr_of( value, start, length )
{
   var text = String( value || "" );

   return ( length === null ) ? text.substring( start ) : text.substr( start, length );
}

// NOTE: The output buffer is what "{@1}", "{@2}" and "{@}" read. A server response replaces
// it; "echo", "seed" and "var" add a line to it; "clear" empties it.
function append_output( buffer, text )
{
   var before = String( buffer || "" );

   return ( before === "" ) ? String( text ) : before + "\n" + text;
}

// NOTE: "view lists" and "view scripts" answer one name per line - this account's own as "***", as
// the server shows it. An error, or "[none]" when there are none, is an empty list.
function parse_name_list( response )
{
   var text = String( response || "" ).trim( );

   if( ( text === "" ) || ( text === "[none]" ) || ( text.indexOf( "Error: " ) === 0 ) )
      return [ ];

   return text.split( /\r?\n/ ).map( function( line ) { return line.trim( ); } )
    .filter( function( line ) { return ( line === c_console_own_name ) || /^[A-Za-z0-9_\-.]+$/.test( line ); } );
}

// NOTE: Lines that need a server javascript run - "load script", "eval script", "exec
// script" and the rest of the harness's "javascripts" verbs. Run in the page as the harness runs
// them - another account's by admin alone ("script_allowed( )") - see "parse_script_line( )".
function is_javascript_line( text )
{
   return /^~?(load|reload|eval|exec|employ|execute|result|unload)\s+(script|scripts|javascript|javascripts)(\s|$)/i.test( String( text || "" ).trim( ) );
}

// NOTE: The harness's verbs for a server javascript, by what each does: "load" puts the script
// in the page and calls its "_at_load", "eval" calls its "_execute", "result" shows its
// "_result" and "unload" takes it out again.
const c_console_script_verbs = {
   load: "load", reload: "load",
   eval: "eval", exec: "eval", employ: "eval", execute: "eval",
   result: "result", unload: "unload"
};

// NOTE: "load script bip39 3c6e..." split into the verb, the script's name and the rest as its
// argument, spaces and all - "eval script bip39 <twelve words>" passes all twelve. No argument is
// null, as the harness has it. The name becomes part of a file name and of the functions called,
// so it is only letters, digits and "_" - or "***", this account's own. "load" and "eval" need a
// name; "result" and "unload" without one mean this account's own, as in the harness.
//
// Returns { kind: "script", verb, name, arg }, { kind: "error", message }, or null for a line
// that is not about a server javascript at all.
function parse_script_line( text )
{
   var match = /^~?(\w+)\s+(?:script|scripts|javascript|javascripts)(?:\s+(\S+))?(?:\s+([\s\S]*))?$/i.exec( String( text || "" ).trim( ) );

   if( match === null )
      return null;

   var verb = c_console_script_verbs[ match[ 1 ].toLowerCase( ) ];

   if( !verb )
      return null;

   var name = match[ 2 ] || "";

   if( ( name === "" ) && ( ( verb === "load" ) || ( verb === "eval" ) ) )
      return { kind: "error", message: "Name the script - " + match[ 1 ].toLowerCase( ) + " script <name> [<input>]" };

   if( ( name !== "" ) && ( name !== c_console_own_name ) && !/^[A-Za-z0-9_]+$/.test( name ) )
      return { kind: "error", message: "Invalid script name '" + name + "' - letters, digits and _ only" };

   return { kind: "script", verb: verb, name: ( name === "" ) ? c_console_own_name : name,
    arg: ( match[ 3 ] === undefined ) ? null : match[ 3 ] };
}

// NOTE: Whether this account may run a server javascript (Ian, 2026-10-05): anyone runs those not named
// after a PIN, and their own; another account's - all digits, as the server tells a PIN - is admin's
// alone. The server already keeps them from the listing and "review javascript"; this keeps "load
// script", which fetches the file itself, to the same rule.
function script_allowed( name, access, is_admin )
{
   var value = String( name || "" );

   if( is_admin || ( value === c_console_own_name ) || ( value === String( access || "" ) ) )
      return true;

   return !/^[0-9]+$/.test( value );
}

// ====================================================================
// ntfy - subscribing a phone (the ntfy proof of concept, 2026-10-06)
// ====================================================================

// NOTE: A server address as the phones reach it - "http(s)://<host>[:<port>]", with nothing after it.
// "" when it is not one.
function normalise_ntfy_server( text )
{
   var match = /^(https?):\/\/([A-Za-z0-9.-]+(?::\d{1,5})?)\/?$/i.exec( String( text || "" ).trim( ) );

   return ( match === null ) ? "" : match[ 1 ].toLowerCase( ) + "://" + match[ 2 ];
}

// NOTE: ntfy's own rule for a topic - letters, digits, "-" and "_", at most 64.
function is_ntfy_topic( text )
{
   return /^[-_A-Za-z0-9]{1,64}$/.test( String( text || "" ) );
}

// NOTE: What a phone's camera opens to subscribe - the apps' own "ntfy://<host>/<topic>" link, with
// "secure=false" when the server is plain HTTP. Scanned, it saves typing a topic: both phones once
// subscribed to an O for a D.
function ntfy_subscribe_link( server, topic )
{
   var normal = normalise_ntfy_server( server );

   if( ( normal === "" ) || !is_ntfy_topic( topic ) )
      return "";

   var http = ( normal.indexOf( "http://" ) === 0 );

   return "ntfy://" + normal.replace( /^https?:\/\//, "" ) + "/" + topic + ( http ? "?secure=false" : "" );
}

// NOTE: The same topic as an ordinary web address - ntfy's own page for it. Every phone camera opens one,
// where the iPhone's would not open "ntfy://" ("No usable data", 2026-10-06); the page shows the topic, so
// it can be copied into the app rather than typed.
function ntfy_web_link( server, topic )
{
   var normal = normalise_ntfy_server( server );

   return ( ( normal === "" ) || !is_ntfy_topic( topic ) ) ? "" : normal + "/" + topic;
}

// NOTE: "ntfy server [<url>]" - set, or show, the server the phones use; "ntfy qr [<topic>] [web]" - a topic
// as a QR code, the node's own when none is named: the apps' "ntfy://" link, or with "web" ntfy's web page
// for it. Returns { verb, server, topic, web } or { verb: "error", message }.
function parse_ntfy_command( args )
{
   var words = String( args || "" ).trim( ).split( /\s+/ ).filter( function( w ) { return w !== ""; } );

   var verb = ( words[ 0 ] || "" ).toLowerCase( );

   var usage = "Usage is ntfy server [<url>], or ntfy qr [<topic>] [web]";

   if( ( verb === "server" ) && ( words.length <= 2 ) )
   {
      if( words.length === 1 )
         return { verb: "server", server: "", topic: "", web: false };

      var server = normalise_ntfy_server( words[ 1 ] );

      return ( server === "" ) ? { verb: "error", message: "Not a server address - http://<address>:<port>, say http://192.168.0.15:8090" }
       : { verb: "server", server: server, topic: "", web: false };
   }

   // NOTE: "web" last - "ntfy qr web" is the node's own topic as a web page, not a topic called "web".
   var web = ( words.length > 1 ) && ( words[ words.length - 1 ].toLowerCase( ) === "web" );

   if( web )
      words.pop( );

   if( ( verb === "qr" ) && ( words.length <= 2 ) )
   {
      var topic = words[ 1 ] || "";

      if( ( topic !== "" ) && !is_ntfy_topic( topic ) )
         return { verb: "error", message: "Not a topic - letters, digits, - and _ only" };

      return { verb: "qr", server: "", topic: topic, web: web };
   }

   return { verb: "error", message: usage };
}

// NOTE: A global a server javascript sets - "wait ciyam_harden_result", "var @x ciyam_bip39_result".
// Only a plain identifier, so nothing but a name is ever looked up on the page.
function is_global_name( name )
{
   return /^[A-Za-z_$][A-Za-z0-9_$]*$/.test( String( name || "" ) );
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

// NOTE: Every word typed must appear in the command or its description. Matches at the start
// of a word of the command rank first, then anywhere in the command, then only in the
// description.
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

      // NOTE: A word of the command that starts with what was typed beats a match inside a
      // word - "view" should find "view lists" before "messages review".
      var command_words = split_words( command );

      var at_word_start = words.every( function( w )
      {
         return command_words.some( function( cw ) { return cw.indexOf( w ) === 0; } );
      } );

      var rank = at_word_start ? 0 : ( in_command ? 1 : 2 );

      scored.push( { item: item, rank: rank, index: index } );
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
// Preferences
// ====================================================================

// NOTE: One "localStorage" key holding a small JSON object, read by the chat and the console
// alike and shared by every account on the browser. More can be added to "c_console_prefs"
// as they are needed; a per-account set would be a second key ending in the PIN.
//
// "log_polling" also logs the quiet requests - polling and other background reads - which are
// otherwise left out so they do not bury what was asked for. For diagnostics; off by default.
//
// "echo_list_lines" prints each line of a list as it runs, before what it answers. Off by default, as
// in the harness - the output is much cleaner without (Ian, 2026-10-07).
const c_console_prefs_key = "cws.prefs";

const c_console_prefs = { log_session_only: false, log_polling: false, echo_list_lines: false };

// NOTE: Only known names, and only values of the default's type, are taken from what is
// stored - anything else, including text that is not JSON at all, falls back to the default.
function parse_prefs( stored )
{
   var prefs = { };

   var parsed = null;

   try
   {
      parsed = JSON.parse( String( stored ) );
   }
   catch( e )
   {
      parsed = null;
   }

   Object.keys( c_console_prefs ).forEach( function( name )
   {
      var fallback = c_console_prefs[ name ];

      if( ( parsed !== null ) && ( typeof parsed === "object" ) && ( typeof parsed[ name ] === typeof fallback ) )
         prefs[ name ] = parsed[ name ];
      else
         prefs[ name ] = fallback;
   } );

   return prefs;
}

// NOTE: In the browser - whether "Log polling" is ticked. Read afresh on each request, so ticking
// it in the console takes effect in the chat and the accounts page at their next poll.
function is_logging_polling( )
{
   try
   {
      return parse_prefs( localStorage.getItem( c_console_prefs_key ) ).log_polling;
   }
   catch( e )
   {
      return false;
   }
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

   // NOTE: For "full_query( )" - the query as sent, but for the credentials, and which of them it had.
   var sent = [ ];
   var had = { };

   query.split( "&" ).forEach( function( pair )
   {
      if( pair === "" )
         return;

      var eq = pair.indexOf( "=" );

      var name = ( eq < 0 ) ? pair : pair.substr( 0, eq );
      var value = ( eq < 0 ) ? "" : decode_field( pair.substring( eq + 1 ) );

      if( c_console_credential_params.indexOf( name ) >= 0 )
         had[ name ] = true;
      else
         sent.push( pair );

      if( c_console_secret_params.indexOf( name ) >= 0 )
         return;

      if( ( name === "options" ) || ( name === "request" ) )
         request = value;
      else
         kept.push( name + "=" + value );
   } );

   var path = endpoint;

   if( kept.length > 0 )
      endpoint += "?" + kept.join( "&" );

   if( ( body !== null ) && ( body !== undefined ) && ( String( body ) !== "" ) )
      request = ( request !== "" ? request + "\n" : "" ) + String( body );

   var text = ( response === null ) ? "" : String( response );

   // NOTE: "GET /cws/devices" answers each device's live session id - a credential, as "session=" is - so
   // they are masked here with the rest (found by review, 2026-10-07).
   if( /^\/cws\/devices\/?$/.test( endpoint.split( "?" )[ 0 ] ) )
      text = mask_device_sessions( text );

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
      ms: Math.max( 0, finished.getTime( ) - started.getTime( ) ),
      path: path,
      sent: sent,
      had: had
   };
}

// NOTE: A devices list - "<device> <session>", "*" after an active one - with each session id hidden.
function mask_device_sessions( text )
{
   return String( text || "" ).replace( /^(\s*[A-Za-z0-9]+\s+)[A-Za-z0-9]+(\*?)\s*$/gm, "$1(session)$2" );
}

// NOTE: The whole query for a logged request, to copy and try again (Ian, 2026-10-06 - he still opened
// "test_web_session.html" for it). An entry keeps no credentials, so they are put back from "session" -
// this tab's own, which a linked console shares with the chat - in the harness's order, where the
// request had them. A password hash is never put back: a sign in's comes back as "passwd=PASSWORD_HASH".
// "origin" is the server's address, before "/cws". Returns "" for an entry from before this was kept.
function full_query( origin, entry, session )
{
   if( !entry || !entry.path || !entry.sent )
      return "";

   var had = entry.had || { };

   var parts = [ ];

   if( had.access )
      parts.push( "access=" + session.access );

   if( had.device && session.device )
      parts.push( "device=" + session.device );

   if( had.passwd )
      parts.push( "passwd=PASSWORD_HASH" );

   parts = parts.concat( entry.sent );

   if( had.session )
      parts.push( "session=" + session.sessid );

   return String( origin || "" ) + entry.path + ( ( parts.length > 0 ) ? "?" + parts.join( "&" ) : "" );
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
      linked_owner: linked_owner,
      resolve_command: resolve_command,
      parse_creds_command: parse_creds_command,
      plan_creds_removal: plan_creds_removal,
      plan_creds_retain: plan_creds_retain,
      build_cws_url: build_cws_url,
      is_valid_variable_name: is_valid_variable_name,
      split_script: split_script,
      apply_line_guard: apply_line_guard,
      parse_var_command: parse_var_command,
      substr_of: substr_of,
      append_output: append_output,
      is_javascript_line: is_javascript_line,
      parse_script_line: parse_script_line,
      script_allowed: script_allowed,
      full_query: full_query,
      mask_device_sessions: mask_device_sessions,
      normalise_ntfy_server: normalise_ntfy_server,
      is_ntfy_topic: is_ntfy_topic,
      ntfy_subscribe_link: ntfy_subscribe_link,
      ntfy_web_link: ntfy_web_link,
      parse_ntfy_command: parse_ntfy_command,
      c_ntfy_server_key: c_ntfy_server_key,
      is_global_name: is_global_name,
      parse_name_list: parse_name_list,
      is_error_output: is_error_output,
      truncate_lines: truncate_lines,
      push_history: push_history,
      filter_palette: filter_palette,
      first_placeholder: first_placeholder,
      summarise_storage_value: summarise_storage_value,
      parse_prefs: parse_prefs,
      format_clock: format_clock,
      make_log_entry: make_log_entry,
      install_log_capture: install_log_capture
   };
}

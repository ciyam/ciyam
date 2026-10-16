// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The template for a module - copy it as "ciyam_<name>.js" and change "ciyam_xxxxx" to match. One call, registering a
// class: "about" says what it is (the console's help, the usage hints), and "run( input )" answers - the result, or an error
// thrown. See "Modules" in "ciyam.js" for the rest - "loaded( )", { result, output }, and the "context" (progress, a QR code,
// hashing). Opened in the console to become an account's own, the server renames "ciyam_xxxxx" to "ciyam_<PIN>".
ciyam_register_module( class extends ciyam_module
{
   static about =
   {
      name: "ciyam_xxxxx",
      version: "2",
      description: "echoes its input - the template for a module",
      example: "test",
      hint: "(i.e. will echo the provided argument)",
      needs: [ ]
   };

   async run( input )
   {
      return ( input === null ) ? "" : input;
   }
} );

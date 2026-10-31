// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The twelve words typed with help (Ian, 2026-10-31: none when restoring keys - "especially preferable for phone usage"),
// as "test_bip39.html" does it: the rest of the word offered, in grey, after what is typed; Tab or a space takes it; a word not
// in the list underlined. A ghost over the field draws its text - the field's own is transparent, its caret and selection still
// shown - so the ghost takes the field's font, padding and border, and the field grows rather than scrolls, so the two wrap
// alike. The pure parts are "word_first_match( )", "words_entry_state( )" and "words_entry_complete( )" in "chat_parse.js"; the
// look is "chat.css"'s.
//
//    words_entry( textarea, { wordlist: function( ) { return [ ... ]; }, on_enter: f } );
const c_words_entry_copied = [ "fontFamily", "fontSize", "fontWeight", "lineHeight", "letterSpacing", "paddingTop", "paddingRight",
 "paddingBottom", "paddingLeft", "borderTopWidth", "borderRightWidth", "borderBottomWidth", "borderLeftWidth", "boxSizing" ];

function words_entry( field, options )
{
   var opts = options || { };

   var wrap = document.createElement( "div" );
   var ghost = document.createElement( "div" );

   wrap.className = "words-entry";
   ghost.className = "words-entry-ghost";
   ghost.setAttribute( "aria-hidden", "true" );

   field.parentNode.insertBefore( wrap, field );

   wrap.appendChild( field );
   wrap.appendChild( ghost );

   field.classList.add( "words-entry-field" );

   function wordlist( )
   {
      return opts.wordlist ? ( opts.wordlist( ) || [ ] ) : [ ];
   }

   function at_end( )
   {
      return ( field.selectionStart === field.value.length ) && ( field.selectionEnd === field.value.length );
   }

   function draw( )
   {
      var looks = getComputedStyle( field );

      c_words_entry_copied.forEach( function( name ) { ghost.style[ name ] = looks[ name ]; } );

      var state = words_entry_state( field.value, wordlist( ) );

      ghost.replaceChildren( );

      state.pieces.forEach( function( piece )
      {
         var span = document.createElement( "span" );

         span.textContent = piece.text;

         if( piece.kind === "unknown" )
            span.className = "words-entry-unknown";

         ghost.appendChild( span );
      } );

      // NOTE: Only while typing at the end - in the middle of the words it would sit over those after.
      if( ( state.rest !== "" ) && at_end( ) && ( document.activeElement === field ) )
      {
         var rest = document.createElement( "span" );

         rest.className = "words-entry-rest";
         rest.textContent = state.rest;

         ghost.appendChild( rest );
      }

      // NOTE: Grown to its words - not while hidden (a step not shown yet), which would measure it as nothing.
      if( field.offsetParent === null )
      {
         field.style.height = "";

         return;
      }

      field.style.height = "auto";
      field.style.height = ( field.scrollHeight + parseFloat( looks.borderTopWidth ) + parseFloat( looks.borderBottomWidth ) ) + "px";
   }

   // NOTE: The page empties the field by setting its value - the ghost has to follow, or the words would stay on show.
   var value = Object.getOwnPropertyDescriptor( HTMLTextAreaElement.prototype, "value" );

   Object.defineProperty( field, "value", {
      configurable: true,
      get: function( ) { return value.get.call( field ); },
      set: function( text ) { value.set.call( field, text ); draw( ); } } );

   field.addEventListener( "input", function( )
   {
      if( at_end( ) )
      {
         var completed = words_entry_complete( field.value, wordlist( ) );

         if( completed !== field.value )
         {
            value.set.call( field, completed );

            field.setSelectionRange( completed.length, completed.length );
         }
      }

      draw( );
   } );

   field.addEventListener( "keydown", function( event )
   {
      if( event.key === "Enter" )
      {
         event.preventDefault( );

         if( opts.on_enter )
            opts.on_enter( );

         return;
      }

      // NOTE: Tab takes the word offered - and with none offered, moves on as usual.
      if( ( event.key === "Tab" ) && !event.shiftKey && at_end( ) )
      {
         var rest = words_entry_state( field.value, wordlist( ) ).rest;

         if( rest !== "" )
         {
            event.preventDefault( );

            value.set.call( field, field.value + rest + " " );

            field.setSelectionRange( field.value.length, field.value.length );

            draw( );
         }
      }
   } );

   [ "focus", "blur", "click", "keyup" ].forEach( function( name ) { field.addEventListener( name, draw ); } );

   window.addEventListener( "resize", draw );

   draw( );

   return { draw: draw };
}

// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Styled dialogs in place of the browser's "confirm( )" and "alert( )" (Damon, 2026-10-27) - drawn as the page's own
// dialogs are ("chat.css"'s ".chat-scrim" and ".chat-dialog"), for every page of the node. "ask_dialog( )" resolves true for its
// yes, and false for its no, Escape or a click outside it; "tell_dialog( )" resolves once it is dismissed. One at a time - a second
// asked while one is open answers the first no. Its text is set with "textContent", so a name from the server can be in it.
//
//    if( await ask_dialog( { title: "Remove the key?", text: "...", yes: "Remove", no: "Keep it", danger: true } ) ) ...
//    await tell_dialog( { title: "Saved", text: "..." } );
var g_dialog = null;

function dialog_element( tag, class_name, id )
{
   var node = document.createElement( tag );

   if( class_name )
      node.className = class_name;

   if( id )
      node.id = id;

   return node;
}

// NOTE: Made once, the first time it is asked for, at the end of the body so it is drawn over any other dialog.
function dialog_parts( )
{
   var scrim = document.getElementById( "dialog_scrim" );

   if( scrim !== null )
      return scrim;

   scrim = dialog_element( "div", "chat-scrim chat-ask-scrim", "dialog_scrim" );
   scrim.hidden = true;

   var box = dialog_element( "div", "chat-dialog chat-ask", "dialog_box" );

   box.setAttribute( "role", "alertdialog" );
   box.setAttribute( "aria-modal", "true" );
   box.setAttribute( "aria-labelledby", "dialog_title" );
   box.setAttribute( "aria-describedby", "dialog_text" );

   var actions = dialog_element( "div", "chat-dialog-actions" );

   var no = dialog_element( "button", "chat-btn", "dialog_no" );
   var yes = dialog_element( "button", "chat-btn chat-btn--primary", "dialog_yes" );

   no.type = "button";
   yes.type = "button";

   no.addEventListener( "click", function( ) { dialog_end( false ); } );
   yes.addEventListener( "click", function( ) { dialog_end( true ); } );

   actions.appendChild( no );
   actions.appendChild( yes );

   box.appendChild( dialog_element( "h2", "", "dialog_title" ) );
   box.appendChild( dialog_element( "p", "chat-dialog-note chat-ask-text", "dialog_text" ) );
   box.appendChild( actions );

   scrim.appendChild( box );

   // NOTE: A click on the scrim itself, outside the dialog, is a no.
   scrim.addEventListener( "click", function( event )
   {
      if( event.target === scrim )
         dialog_end( false );
   } );

   document.body.appendChild( scrim );

   return scrim;
}

// NOTE: Bound while a dialog is open, ahead of the page's own keys (the capture phase) - Escape here would otherwise also close
// whatever dialog of the page's is under it. Tab goes round the dialog's buttons only.
function dialog_keydown( event )
{
   if( g_dialog === null )
      return;

   if( event.key === "Escape" )
   {
      event.preventDefault( );
      event.stopPropagation( );

      dialog_end( false );
   }
   else if( event.key === "Tab" )
   {
      var buttons = Array.prototype.slice.call( document.querySelectorAll( "#dialog_box button" ) ).filter( function( button )
      {
         return !button.hidden;
      } );

      var at = buttons.indexOf( document.activeElement );

      event.preventDefault( );

      buttons[ ( at + ( event.shiftKey ? buttons.length - 1 : 1 ) + buttons.length ) % buttons.length ].focus( );
   }
}

function dialog_end( answer )
{
   if( g_dialog === null )
      return;

   var open = g_dialog;

   g_dialog = null;

   document.removeEventListener( "keydown", dialog_keydown, true );

   document.getElementById( "dialog_scrim" ).hidden = true;

   if( open.focused && open.focused.isConnected )
      open.focused.focus( );

   open.resolve( answer );
}

// NOTE: "options" - { title, text, yes, no, danger }; "no" null for one button only (as "tell_dialog( )" has it). A danger's yes
// is red, and its no has the focus, so Enter straight away does not do it.
function show_dialog( options )
{
   var opts = options || { };

   dialog_end( false );

   var scrim = dialog_parts( );

   document.getElementById( "dialog_title" ).textContent = opts.title || "";
   document.getElementById( "dialog_text" ).textContent = opts.text || "";
   document.getElementById( "dialog_text" ).hidden = !opts.text;

   var yes = document.getElementById( "dialog_yes" );
   var no = document.getElementById( "dialog_no" );

   yes.textContent = opts.yes || "OK";
   yes.className = "chat-btn " + ( opts.danger ? "chat-btn--danger" : "chat-btn--primary" );

   no.textContent = opts.no || "Cancel";
   no.hidden = ( opts.no === null );

   return new Promise( function( resolve )
   {
      g_dialog = { resolve: resolve, focused: document.activeElement };

      document.addEventListener( "keydown", dialog_keydown, true );

      scrim.hidden = false;

      ( ( opts.danger && !no.hidden ) ? no : yes ).focus( );
   } );
}

function ask_dialog( options )
{
   return show_dialog( options );
}

function tell_dialog( options )
{
   return show_dialog( Object.assign( { yes: "OK" }, options || { }, { no: null } ) ).then( function( ) { } );
}

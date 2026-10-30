// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The people picker (Damon, 2026-10-27: the invitee list had grown cluttered) - one chooser of people for any page: those
// chosen shown as chips in a field, and a popup list of everyone below it, a search at the top and a tick for each. In the
// search, Enter with the list narrowed to one person chooses them and closes it; the arrow keys move down into the list and
// through it; Space ticks; Enter on the list, Done or Escape close it. Each row is a "label.chat-recipient" holding an
// "input.chat-invitee-check" valued with the name, so what is chosen is read from the boxes, and a page may mark a row as it
// learns more of someone ("mark_unavailable( )"). Names are set with "textContent" - they come from the server. The pure parts
// are "picker_matches( )", "picker_enter_choice( )" and "picker_count_text( )" in "chat_parse.js"; the look is "chat.css"'s.
//
//    var picker = people_picker( host, { list_id: "...", add_id: "...", none_text: "...", done_id: "...", on_change: f } );
//    picker.set_people( [ { name: "verify-a", note: "conversation open" } ] );  picker.chosen( );  picker.reset( );
//
// "picker.set_single( true )" makes it a choice of one person (Ian, 2026-10-30: a direct message is with one person - "otherwise
// it's a normal room"): choosing someone replaces whoever was chosen, and closes the list.
const c_picker_hint = "↑ ↓ move · Space ticks · Enter done";
const c_picker_single_hint = "↑ ↓ move · Space or Enter chooses";

function picker_element( tag, class_name, text )
{
   var node = document.createElement( tag );

   if( class_name )
      node.className = class_name;

   if( text !== undefined )
      node.textContent = text;

   return node;
}

function people_picker( host, options )
{
   var opts = options || { };

   var picker = { host: host, opts: opts, query: "", people: 0, single: false };

   host.textContent = "";
   host.classList.add( "chat-picker" );

   picker.field = picker_element( "div", "chat-picker-field" );
   picker.chips = picker_element( "span", "chat-picker-chips" );

   picker.add = picker_element( "button", "chat-picker-add" );
   picker.add.type = "button";
   picker.add.setAttribute( "aria-expanded", "false" );

   if( opts.add_id )
      picker.add.id = opts.add_id;

   picker.field.appendChild( picker.chips );
   picker.field.appendChild( picker.add );

   picker.popup = picker_element( "div", "chat-picker-popup" );
   picker.popup.hidden = true;

   picker.search = picker_element( "input", "chat-field chat-picker-search" );
   picker.search.type = "search";
   picker.search.placeholder = "Search people";
   picker.search.autocomplete = "off";
   picker.search.setAttribute( "aria-label", "Search people" );

   var meta = picker_element( "div", "chat-picker-meta" );

   picker.count = picker_element( "span", "" );

   picker.clear = picker_element( "button", "chat-picker-clear", "Clear" );
   picker.clear.type = "button";

   meta.appendChild( picker.count );
   meta.appendChild( picker.clear );

   picker.list = picker_element( "div", "chat-picker-list" );

   if( opts.list_id )
      picker.list.id = opts.list_id;

   picker.empty = picker_element( "p", "chat-picker-empty" );
   picker.empty.hidden = true;

   var foot = picker_element( "div", "chat-picker-foot" );

   var done = picker_element( "button", "chat-btn chat-btn--accent chat-btn--small", "Done" );
   done.type = "button";

   picker.hint = picker_element( "span", "chat-picker-hint", c_picker_hint );

   foot.appendChild( picker.hint );
   foot.appendChild( done );

   picker.popup.appendChild( picker.search );
   picker.popup.appendChild( meta );
   picker.popup.appendChild( picker.list );
   picker.popup.appendChild( picker.empty );
   picker.popup.appendChild( foot );

   host.appendChild( picker.field );
   host.appendChild( picker.popup );

   // NOTE: What is ticked, and every box - the rows hidden by the search included.
   picker.boxes = function( )
   {
      return Array.prototype.slice.call( picker.list.querySelectorAll( ".chat-invitee-check" ) );
   };

   picker.chosen = function( )
   {
      return picker.boxes( ).filter( function( box ) { return box.checked; } ).map( function( box ) { return box.value; } );
   };

   // NOTE: One person only, or any number.
   picker.set_single = function( single )
   {
      picker.single = !!single;

      picker.hint.textContent = picker.single ? c_picker_single_hint : c_picker_hint;

      picker.refresh( );
   };

   // NOTE: "name" chosen - in a choice of one, in place of whoever was.
   picker.choose = function( name )
   {
      picker.boxes( ).forEach( function( box )
      {
         if( box.value === name )
            box.checked = true;
         else if( picker.single )
            box.checked = false;
      } );
   };

   picker.changed = function( )
   {
      picker.refresh( );

      if( opts.on_change )
         opts.on_change( picker.chosen( ) );
   };

   // NOTE: Everyone to choose from - "people" each { name, note }. Anyone ticked is kept, as the list may be drawn again
   // underneath (the poll brings the user list). None - "none_text" said instead (the option's, unless given here).
   picker.set_people = function( people, none_text )
   {
      var ticked = { };

      picker.chosen( ).forEach( function( name ) { ticked[ name ] = true; } );

      picker.list.textContent = "";

      var list = people || [ ];

      picker.people = list.length;

      if( list.length === 0 )
         picker.list.appendChild( picker_element( "p", "chat-picker-none", none_text || opts.none_text || "No one to choose." ) );

      list.forEach( function( person )
      {
         var row = picker_element( "label", "chat-recipient chat-picker-row" );
         var box = picker_element( "input", "chat-invitee-check" );

         box.type = "checkbox";
         box.value = person.name;
         box.checked = ( ticked[ person.name ] === true );

         var initial = picker_element( "span", "chat-picker-initial is-tone-" + ( person.name.charCodeAt( 0 ) % 5 ),
          person.name.charAt( 0 ).toUpperCase( ) );

         initial.setAttribute( "aria-hidden", "true" );

         row.appendChild( box );
         row.appendChild( initial );
         row.appendChild( picker_element( "span", "chat-invitee-name", person.name ) );

         if( person.note )
            row.appendChild( picker_element( "span", "chat-invitee-note", person.note ) );

         picker.list.appendChild( row );
      } );

      picker.refresh( );
   };

   // NOTE: Someone who cannot be chosen - shown, so the list is honest about who is missing, but greyed and not tickable.
   picker.mark_unavailable = function( name, tag, title )
   {
      picker.boxes( ).forEach( function( box )
      {
         if( box.value !== name )
            return;

         var row = box.closest( ".chat-recipient" );

         box.checked = false;
         box.disabled = true;

         row.classList.add( "is-invited" );
         row.title = title || "";

         if( row.querySelector( ".chat-invitee-tag" ) === null )
            row.appendChild( picker_element( "span", "chat-invitee-tag", tag ) );
      } );

      picker.refresh( );
   };

   // NOTE: The chips, the button's words, the count and which rows the search shows.
   picker.refresh = function( )
   {
      var chosen = picker.chosen( );

      picker.chips.textContent = "";

      chosen.forEach( function( name )
      {
         var chip = picker_element( "span", "chat-picker-chip" );
         var remove = picker_element( "button", "", "×" );

         remove.type = "button";
         remove.setAttribute( "aria-label", "Remove " + name );

         remove.onclick = function( )
         {
            picker.boxes( ).forEach( function( box ) { if( box.value === name ) box.checked = false; } );

            picker.changed( );

            picker.add.focus( );
         };

         chip.appendChild( picker_element( "span", "", name ) );
         chip.appendChild( remove );

         picker.chips.appendChild( chip );
      } );

      if( picker.single )
         picker.add.textContent = ( chosen.length === 0 ) ? "Choose a person" : "Change";
      else
         picker.add.textContent = ( chosen.length === 0 ) ? ( opts.add_text || "Choose people" ) : "Add more";

      picker.count.textContent = picker_count_text( picker.single ? 0 : chosen.length, picker.people );
      picker.clear.hidden = ( chosen.length === 0 );

      var shown = 0;

      picker.boxes( ).forEach( function( box )
      {
         var row = box.closest( ".chat-recipient" );

         row.hidden = !picker_matches( box.value, picker.query );

         if( !row.hidden )
            shown++;
      } );

      picker.empty.textContent = "No one matches “" + picker.query.trim( ) + "”.";
      picker.empty.hidden = ( shown > 0 ) || ( picker.people === 0 );
   };

   picker.shown_boxes = function( )
   {
      return picker.boxes( ).filter( function( box ) { return !box.closest( ".chat-recipient" ).hidden; } );
   };

   picker.is_open = function( )
   {
      return !picker.popup.hidden;
   };

   // NOTE: The dialog it is in scrolls, which would cut the popup off - so it does not while the popup is open.
   picker.open = function( )
   {
      picker.popup.hidden = false;
      picker.add.setAttribute( "aria-expanded", "true" );

      var dialog = host.closest( ".chat-dialog" );

      if( dialog !== null )
         dialog.classList.add( "has-picker-open" );

      picker.refresh( );
      picker.search.focus( );
   };

   // NOTE: "then" - where the focus goes: the field's button (Escape), what the page named for after choosing ("done_id"),
   // or nowhere (a click elsewhere took it).
   picker.close = function( then )
   {
      if( !picker.is_open( ) )
         return;

      picker.popup.hidden = true;
      picker.add.setAttribute( "aria-expanded", "false" );

      var dialog = host.closest( ".chat-dialog" );

      if( dialog !== null )
         dialog.classList.remove( "has-picker-open" );

      picker.query = "";
      picker.search.value = "";

      picker.refresh( );

      var next = ( then === "done" ) && opts.done_id ? document.getElementById( opts.done_id ) : ( ( then === "field" ) ? picker.add : null );

      if( next !== null )
         next.focus( );
   };

   // NOTE: For a dialog opened afresh - no one chosen, nothing typed, closed.
   picker.reset = function( )
   {
      picker.close( null );

      picker.list.textContent = "";
      picker.people = 0;

      picker.refresh( );
   };

   picker.add.onclick = function( )
   {
      if( picker.is_open( ) )
         picker.close( "field" );
      else
         picker.open( );
   };

   done.onclick = function( )
   {
      picker.close( "done" );
   };

   picker.clear.onclick = function( )
   {
      picker.boxes( ).forEach( function( box ) { if( !box.disabled ) box.checked = false; } );

      picker.changed( );

      picker.search.focus( );
   };

   // NOTE: In a choice of one, ticking someone chooses them in place of whoever was, and that is the choice made; unticking
   // leaves no one.
   picker.list.addEventListener( "change", function( event )
   {
      var box = event.target;

      var chose = picker.single && box.classList.contains( "chat-invitee-check" ) && box.checked;

      if( chose )
         picker.choose( box.value );

      picker.changed( );

      if( chose )
         picker.close( "done" );
   } );

   picker.search.addEventListener( "input", function( )
   {
      picker.query = picker.search.value;

      picker.refresh( );
   } );

   picker.search.addEventListener( "keydown", function( event )
   {
      if( event.key === "Enter" )
      {
         // NOTE: Never the form's own Enter - the dialog would be sent with the list half chosen.
         event.preventDefault( );

         var name = picker_enter_choice( picker.shown_boxes( ).map( function( box )
         {
            return { name: box.value, available: !box.disabled };
         } ) );

         if( name === "" )
            return;

         picker.choose( name );

         picker.changed( );

         picker.close( "done" );
      }
      else if( event.key === "ArrowDown" )
      {
         var first = picker.shown_boxes( ).filter( function( box ) { return !box.disabled; } )[ 0 ];

         if( first !== undefined )
         {
            event.preventDefault( );

            first.focus( );
         }
      }
   } );

   picker.list.addEventListener( "keydown", function( event )
   {
      var boxes = picker.shown_boxes( ).filter( function( box ) { return !box.disabled; } );
      var at = boxes.indexOf( event.target );

      if( at < 0 )
         return;

      if( event.key === "Enter" )
      {
         event.preventDefault( );

         // NOTE: In a choice of one, Enter chooses the one with the focus.
         if( picker.single )
         {
            picker.choose( event.target.value );

            picker.changed( );
         }

         picker.close( "done" );
      }
      else if( event.key === "ArrowDown" )
      {
         event.preventDefault( );

         if( at < boxes.length - 1 )
            boxes[ at + 1 ].focus( );
      }
      else if( event.key === "ArrowUp" )
      {
         event.preventDefault( );

         if( at > 0 )
            boxes[ at - 1 ].focus( );
         else
            picker.search.focus( );
      }
   } );

   // NOTE: Escape closes the popup alone - kept from the page's own handler, which would close the dialog as well.
   picker.popup.addEventListener( "keydown", function( event )
   {
      if( event.key !== "Escape" )
         return;

      event.preventDefault( );
      event.stopPropagation( );

      picker.close( "field" );
   } );

   document.addEventListener( "mousedown", function( event )
   {
      if( picker.is_open( ) && !host.contains( event.target ) )
         picker.close( null );
   } );

   picker.refresh( );

   return picker;
}

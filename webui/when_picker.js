// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: The day and time picker (Damon, 2026-10-31) - in place of the browser's own, which differs from one browser to the next
// and could not take what Ian asked for (2026-10-31: a Select beside Clear, the day not set by clicking it). A field showing the
// day and time chosen (yyyy-mm-dd, as Ian prefers) opens a month below it: a click or the arrow keys mark a day, an hour and a
// minute are chosen under it, and Select sets them - Clear empties the field, Cancel or Escape leave it as it was. Days outside
// the limits cannot be marked, and a time outside them is said and cannot be selected. The value is kept in the input it is
// given, as a "datetime-local" field's ("2026-11-06T17:00"), so the page reads it as before. The pure parts are the "when_"
// functions in "chat_parse.js"; the look is "chat.css"'s.
//
//    var when = when_picker( input, { field_id: "...", placeholder: "...", label: "...", before_text: "...", after_text: "...",
//     on_change: f } );
//    when.set_range( "2026-10-31T14:37", "" );  when.set_disabled( true );  when.is_open( );  when.close( );
//
// Keys in the month: the arrows move a day or a week, Home and End to the week's start and end, Page Up and Page Down a month,
// Enter or Space marks the day.
const c_when_minute_step = 5;

function when_element( tag, class_name, text )
{
   var node = document.createElement( tag );

   if( class_name )
      node.className = class_name;

   if( text !== undefined )
      node.textContent = text;

   return node;
}

// NOTE: A small calendar for the field.
function when_icon( )
{
   var svg = document.createElementNS( "http://www.w3.org/2000/svg", "svg" );
   var path = document.createElementNS( "http://www.w3.org/2000/svg", "path" );

   svg.setAttribute( "viewBox", "0 0 20 20" );
   svg.setAttribute( "width", "16" );
   svg.setAttribute( "height", "16" );
   svg.setAttribute( "aria-hidden", "true" );
   svg.setAttribute( "class", "chat-when-icon" );

   path.setAttribute( "d", "M4 4.5h12a1 1 0 0 1 1 1V16a1 1 0 0 1-1 1H4a1 1 0 0 1-1-1V5.5a1 1 0 0 1 1-1zM3 8.5h14M7 3v3M13 3v3" );
   path.setAttribute( "fill", "none" );
   path.setAttribute( "stroke", "currentColor" );
   path.setAttribute( "stroke-width", "1.5" );
   path.setAttribute( "stroke-linecap", "round" );

   svg.appendChild( path );

   return svg;
}

function when_picker( input, options )
{
   var opts = options || { };

   var when = { min: "", max: "", day: "", focus: "", shown: { year: 0, month: 0 } };

   var wrap = when_element( "div", "chat-when" );

   var field = when_element( "button", "chat-when-field" );
   var field_text = when_element( "span", "chat-when-text" );

   field.type = "button";
   field.setAttribute( "aria-haspopup", "dialog" );
   field.setAttribute( "aria-expanded", "false" );

   if( opts.field_id )
      field.id = opts.field_id;

   field.appendChild( field_text );
   field.appendChild( when_icon( ) );

   var popup = when_element( "div", "chat-when-popup" );

   popup.hidden = true;
   popup.setAttribute( "role", "dialog" );
   popup.setAttribute( "aria-label", opts.label || "Choose a day and time" );

   var head = when_element( "div", "chat-when-head" );
   var prev = when_element( "button", "chat-when-step", "‹" );
   var month = when_element( "div", "chat-when-month" );
   var next = when_element( "button", "chat-when-step", "›" );

   prev.type = "button";
   prev.setAttribute( "aria-label", "Previous month" );
   next.type = "button";
   next.setAttribute( "aria-label", "Next month" );
   month.setAttribute( "aria-live", "polite" );

   head.appendChild( prev );
   head.appendChild( month );
   head.appendChild( next );

   var grid = when_element( "div", "chat-when-grid" );

   grid.setAttribute( "role", "group" );
   grid.setAttribute( "aria-label", "Days" );

   c_when_weekdays.forEach( function( name )
   {
      grid.appendChild( when_element( "span", "chat-when-weekday", name ) );
   } );

   var days = [ ];

   for( var i = 0; i < 42; i++ )
   {
      var day = when_element( "button", "chat-when-day" );

      day.type = "button";

      grid.appendChild( day );
      days.push( day );
   }

   var time = when_element( "div", "chat-when-time" );
   var hour = when_element( "select", "chat-when-hour" );
   var minute = when_element( "select", "chat-when-minute" );

   hour.setAttribute( "aria-label", "Hour" );
   minute.setAttribute( "aria-label", "Minute" );

   for( var h = 0; h < 24; h++ )
      hour.appendChild( new Option( when_pad( h ), String( h ) ) );

   for( var m = 0; m < 60; m += c_when_minute_step )
      minute.appendChild( new Option( when_pad( m ), String( m ) ) );

   time.appendChild( when_element( "span", "chat-when-time-label", "Time" ) );
   time.appendChild( hour );
   time.appendChild( when_element( "span", "chat-when-colon", ":" ) );
   time.appendChild( minute );

   var problem = when_element( "p", "chat-when-problem" );

   problem.hidden = true;
   problem.setAttribute( "aria-live", "polite" );

   var foot = when_element( "div", "chat-when-foot" );
   var clear = when_element( "button", "chat-when-clear", "Clear" );
   var buttons = when_element( "span", "chat-when-buttons" );
   var cancel = when_element( "button", "chat-btn chat-btn--small", "Cancel" );
   var select = when_element( "button", "chat-btn chat-btn--small chat-btn--accent chat-when-select", "Select" );

   clear.type = "button";
   cancel.type = "button";
   select.type = "button";

   buttons.appendChild( cancel );
   buttons.appendChild( select );

   foot.appendChild( clear );
   foot.appendChild( buttons );

   popup.appendChild( head );
   popup.appendChild( grid );
   popup.appendChild( time );
   popup.appendChild( problem );
   popup.appendChild( foot );

   input.parentNode.insertBefore( wrap, input );

   wrap.appendChild( input );
   wrap.appendChild( field );
   wrap.appendChild( popup );

   function today( )
   {
      var now = new Date( );

      return when_day_text( now.getFullYear( ), now.getMonth( ) + 1, now.getDate( ) );
   }

   // NOTE: A minute not on the step (a value set elsewhere) is added rather than moved.
   function set_minute( value )
   {
      if( !Array.from( minute.options ).some( function( option ) { return Number( option.value ) === value; } ) )
      {
         var added = new Option( when_pad( value ), String( value ) );
         var after = Array.from( minute.options ).find( function( option ) { return Number( option.value ) > value; } );

         minute.insertBefore( added, after || null );
      }

      minute.value = String( value );
   }

   function chosen_value( )
   {
      if( when.day === "" )
         return "";

      var parts = when.day.split( "-" ).map( Number );

      return when_value( parts[ 0 ], parts[ 1 ], parts[ 2 ], Number( hour.value ), Number( minute.value ) );
   }

   function draw_field( )
   {
      var label = when_label( value.get.call( input ) );

      field_text.textContent = label || opts.placeholder || "Choose a day and time";

      field.classList.toggle( "is-empty", label === "" );
   }

   function month_allowed( year, month_number )
   {
      var first = when_day_text( year, month_number, 1 );
      var last = when_day_text( year, month_number, when_month_length( year, month_number ) );

      return ( !when.max || ( first <= when.max.substring( 0, 10 ) ) ) && ( !when.min || ( last >= when.min.substring( 0, 10 ) ) );
   }

   function draw( )
   {
      var shown = when.shown;
      var before = when_add_months( shown.year, shown.month, -1 );
      var after = when_add_months( shown.year, shown.month, 1 );
      var now = today( );

      month.textContent = when_month_label( shown.year, shown.month );

      prev.disabled = !month_allowed( before.year, before.month );
      next.disabled = !month_allowed( after.year, after.month );

      when_month_grid( shown.year, shown.month ).forEach( function( cell, index )
      {
         var button = days[ index ];

         button.textContent = String( cell.day );
         button.dataset.day = cell.text;
         button.disabled = !when_day_allowed( cell.text, when.min, when.max );
         button.tabIndex = ( cell.text === when.focus ) ? 0 : -1;

         button.classList.toggle( "is-outside", cell.outside );
         button.classList.toggle( "is-today", cell.text === now );
         button.classList.toggle( "is-chosen", cell.text === when.day );

         button.setAttribute( "aria-label", cell.text + ( ( cell.text === now ) ? ", today" : "" ) );
         button.setAttribute( "aria-pressed", ( cell.text === when.day ) ? "true" : "false" );
      } );

      var chosen = chosen_value( );
      var range = ( chosen === "" ) ? "" : when_range_problem( chosen, when.min, when.max );

      problem.textContent = ( range === c_when_before ) ? ( opts.before_text || "That time has passed." )
       : ( ( range === c_when_after ) ? ( opts.after_text || "That time is too late." ) : "" );
      problem.hidden = ( range === "" );

      select.disabled = ( chosen === "" ) || ( range !== "" );
   }

   function focus_day( )
   {
      var button = days.find( function( b ) { return b.dataset.day === when.focus; } );

      if( button && !button.disabled )
         button.focus( );
   }

   // NOTE: Kept within the limits - a day that cannot be marked cannot take the focus either.
   function move_focus( text )
   {
      var target = text;

      if( when.min && ( target < when.min.substring( 0, 10 ) ) )
         target = when.min.substring( 0, 10 );

      if( when.max && ( target > when.max.substring( 0, 10 ) ) )
         target = when.max.substring( 0, 10 );

      var parts = target.split( "-" ).map( Number );

      when.focus = target;
      when.shown = { year: parts[ 0 ], month: parts[ 1 ] };

      draw( );
      focus_day( );
   }

   function is_open( )
   {
      return !popup.hidden;
   }

   function open( )
   {
      var parts = when_parts( value.get.call( input ) );
      var offered = when_parts( when_first_offered( ) );

      if( parts !== null )
         when.day = when_day_text( parts.year, parts.month, parts.day );
      else
      {
         when.day = "";
         parts = offered;
      }

      hour.value = String( parts.hour );
      set_minute( parts.minute );

      popup.hidden = false;
      field.setAttribute( "aria-expanded", "true" );

      move_focus( ( when.day !== "" ) ? when.day : when_day_text( offered.year, offered.month, offered.day ) );
   }

   function close( restore_focus )
   {
      if( !is_open( ) )
         return;

      popup.hidden = true;
      field.setAttribute( "aria-expanded", "false" );

      if( restore_focus )
         field.focus( );
   }

   function set( text )
   {
      value.set.call( input, text );

      draw_field( );
      close( true );

      if( opts.on_change )
         opts.on_change( text );
   }

   // NOTE: The page empties the input by setting its value - the field has to follow, as in "words_entry.js".
   var value = Object.getOwnPropertyDescriptor( HTMLInputElement.prototype, "value" );

   Object.defineProperty( input, "value", {
      configurable: true,
      get: function( ) { return value.get.call( input ); },
      set: function( text ) { value.set.call( input, text ); draw_field( ); } } );

   field.addEventListener( "click", function( )
   {
      if( is_open( ) )
         close( false );
      else
         open( );
   } );

   prev.addEventListener( "click", function( )
   {
      when.shown = when_add_months( when.shown.year, when.shown.month, -1 );

      draw( );
   } );

   next.addEventListener( "click", function( )
   {
      when.shown = when_add_months( when.shown.year, when.shown.month, 1 );

      draw( );
   } );

   days.forEach( function( button )
   {
      button.addEventListener( "click", function( )
      {
         if( button.disabled )
            return;

         when.day = button.dataset.day;

         move_focus( when.day );
      } );
   } );

   grid.addEventListener( "keydown", function( event )
   {
      if( !event.target.classList.contains( "chat-when-day" ) )
         return;

      var from = event.target.dataset.day;
      var parts = from.split( "-" ).map( Number );
      var weekday = ( new Date( parts[ 0 ], parts[ 1 ] - 1, parts[ 2 ] ).getDay( ) + 6 ) % 7;

      var to = null;

      if( event.key === "ArrowLeft" )
         to = when_add_days( from, -1 );
      else if( event.key === "ArrowRight" )
         to = when_add_days( from, 1 );
      else if( event.key === "ArrowUp" )
         to = when_add_days( from, -7 );
      else if( event.key === "ArrowDown" )
         to = when_add_days( from, 7 );
      else if( event.key === "Home" )
         to = when_add_days( from, -weekday );
      else if( event.key === "End" )
         to = when_add_days( from, 6 - weekday );
      else if( ( event.key === "PageUp" ) || ( event.key === "PageDown" ) )
      {
         var other = when_add_months( parts[ 0 ], parts[ 1 ], ( event.key === "PageUp" ) ? -1 : 1 );

         to = when_day_text( other.year, other.month, Math.min( parts[ 2 ], when_month_length( other.year, other.month ) ) );
      }

      if( to === null )
         return;

      event.preventDefault( );

      move_focus( to );
   } );

   hour.addEventListener( "change", draw );
   minute.addEventListener( "change", draw );

   clear.addEventListener( "click", function( ) { set( "" ); } );
   cancel.addEventListener( "click", function( ) { close( true ); } );

   select.addEventListener( "click", function( )
   {
      var chosen = chosen_value( );

      if( ( chosen !== "" ) && ( when_range_problem( chosen, when.min, when.max ) === "" ) )
         set( chosen );
   } );

   // NOTE: Escape is the picker's - not the panel's or dialog's around it.
   wrap.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Escape" ) && is_open( ) )
      {
         event.preventDefault( );
         event.stopPropagation( );

         close( true );
      }
   } );

   // NOTE: A press anywhere else leaves it as it was.
   document.addEventListener( "pointerdown", function( event )
   {
      if( is_open( ) && !wrap.contains( event.target ) )
         close( false );
   } );

   draw_field( );

   return {
      is_open: is_open,
      close: function( ) { close( false ); },
      set_range: function( min, max )
      {
         when.min = min || "";
         when.max = max || "";

         if( is_open( ) )
            draw( );
      },
      set_disabled: function( disabled )
      {
         input.disabled = disabled;
         field.disabled = disabled;

         if( disabled )
            close( false );
      } };
}

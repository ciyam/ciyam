// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file
// license.txt in the root project directory or
// http://www.opensource.org/licenses/mit-license.php.

/*
   Minimal vanilla-JS behavior for shared components.
   No build step, no dependencies — plain functions + data attributes.

   Modal:
     <button data-modal-open="my-modal">Open</button>
     <div class="c-modal-backdrop" id="my-modal" data-open="false">
        <div class="c-modal" role="dialog" aria-modal="true" aria-labelledby="my-modal-title">
           ...
           <button data-modal-close>Cancel</button>
        </div>
     </div>

   Nav:
     <button class="c-nav-item" data-nav-item aria-current="page">...</button>
     Clicking a [data-nav-item] sets aria-current="page" on it and clears
     it from its siblings within the same .c-sidebar.

   Sidebar drawer (mobile, <=768px):
     <div class="c-app" data-sidebar-open="false">
        <header class="c-topbar"><button data-sidebar-toggle>...</button></header>
        <nav class="c-sidebar">...</nav>
        <div class="c-sidebar-backdrop" data-sidebar-close></div>
        <main class="c-content">...</main>
     </div>
     Toggled via [data-sidebar-toggle], closed via [data-sidebar-close],
     Escape, or selecting a [data-nav-item] while the drawer is open.

   Banner:
     <div class="c-banner"><button data-banner-dismiss>&times;</button></div>
     Clicking [data-banner-dismiss] adds "is-dismissed" to the nearest .c-banner.

   Search clear button:
     <div class="c-search">
        <input class="c-input c-search__input" data-search-input>
        <button class="c-search__clear" data-search-clear>&times;</button>
     </div>
     The clear button is shown via .c-search[data-has-value="true"], kept in
     sync with the input's value automatically. Clicking it empties the
     input, fires a native "input" event (so any listener bound to the
     input still runs), and refocuses it. Filtering logic itself is
     page-specific and not part of this file.

   Filter chip group:
     <div class="c-chip-group" data-filter-group>
        <button class="c-chip is-active" data-filter-value="all">All</button>
        <button class="c-chip" data-filter-value="x">X</button>
     </div>
     Clicking a chip marks it (and only it) "is-active" within its group and
     dispatches a "filterchange" CustomEvent (detail: { value }) on the
     group element — page-specific code listens for that to apply filtering.

   Dropzone:
     <div class="c-dropzone" data-dropzone>
        <input type="file" class="c-dropzone__input" data-dropzone-input multiple>
        <label class="c-dropzone__label" for="...">...</label>
        <ul class="c-dropzone__files" data-dropzone-files></ul>
     </div>
     Handles drag-over highlighting and renders dropped/selected file names
     into [data-dropzone-files] with a remove button. No actual upload is
     performed — this is local file selection only.
*/

function open_modal( id )
{
   var backdrop = document.getElementById( id );

   if( backdrop == null )
      return;

   backdrop.dataset.open = "true";

   var dialog = backdrop.querySelector( ".c-modal" );

   if( dialog != null )
      dialog.focus( );

   document.addEventListener( "keydown", on_modal_keydown );
}

function close_modal( backdrop )
{
   if( backdrop == null )
      return;

   backdrop.dataset.open = "false";

   document.removeEventListener( "keydown", on_modal_keydown );
}

function on_modal_keydown( event )
{
   if( event.key != "Escape" )
      return;

   var open_backdrop = document.querySelector( '.c-modal-backdrop[data-open="true"]' );

   close_modal( open_backdrop );
}

document.addEventListener( "click", function( event )
{
   var opener = event.target.closest( "[data-modal-open]" );

   if( opener != null )
   {
      open_modal( opener.dataset.modalOpen );
      return;
   }

   var closer = event.target.closest( "[data-modal-close]" );

   if( closer != null )
   {
      close_modal( closer.closest( ".c-modal-backdrop" ) );
      return;
   }

   if( event.target.classList.contains( "c-modal-backdrop" ) )
      close_modal( event.target );
} );

document.addEventListener( "click", function( event )
{
   var item = event.target.closest( "[data-nav-item]" );

   if( item == null )
      return;

   var sidebar = item.closest( ".c-sidebar" );

   if( sidebar == null )
      return;

   sidebar.querySelectorAll( "[data-nav-item]" ).forEach( function( other )
   {
      other.removeAttribute( "aria-current" );
   } );

   item.setAttribute( "aria-current", "page" );

   close_sidebar( item.closest( ".c-app" ) );
} );

function open_sidebar( app )
{
   if( app == null )
      return;

   app.dataset.sidebarOpen = "true";

   document.addEventListener( "keydown", on_sidebar_keydown );
}

function close_sidebar( app )
{
   if( app == null )
      return;

   app.dataset.sidebarOpen = "false";

   document.removeEventListener( "keydown", on_sidebar_keydown );
}

function on_sidebar_keydown( event )
{
   if( event.key != "Escape" )
      return;

   close_sidebar( document.querySelector( '.c-app[data-sidebar-open="true"]' ) );
}

document.addEventListener( "click", function( event )
{
   var toggle = event.target.closest( "[data-sidebar-toggle]" );

   if( toggle != null )
   {
      var app = toggle.closest( ".c-app" );

      if( app != null )
      {
         if( app.dataset.sidebarOpen == "true" )
            close_sidebar( app );
         else
            open_sidebar( app );
      }

      return;
   }

   var closer = event.target.closest( "[data-sidebar-close]" );

   if( closer != null )
      close_sidebar( closer.closest( ".c-app" ) );
} );

document.addEventListener( "click", function( event )
{
   var dismiss = event.target.closest( "[data-banner-dismiss]" );

   if( dismiss == null )
      return;

   var banner = dismiss.closest( ".c-banner" );

   if( banner != null )
      banner.classList.add( "is-dismissed" );
} );

function sync_search_clear( input )
{
   var wrapper = input.closest( ".c-search" );

   if( wrapper != null )
      wrapper.dataset.hasValue = input.value.length > 0 ? "true" : "false";
}

document.addEventListener( "input", function( event )
{
   if( event.target.matches( "[data-search-input]" ) )
      sync_search_clear( event.target );
} );

document.addEventListener( "click", function( event )
{
   var clear = event.target.closest( "[data-search-clear]" );

   if( clear == null )
      return;

   var wrapper = clear.closest( ".c-search" );
   var input = wrapper == null ? null : wrapper.querySelector( "[data-search-input]" );

   if( input == null )
      return;

   input.value = "";
   input.dispatchEvent( new Event( "input", { bubbles: true } ) );
   input.focus( );
} );

document.addEventListener( "click", function( event )
{
   var chip = event.target.closest( "[data-filter-value]" );

   if( chip == null )
      return;

   var group = chip.closest( "[data-filter-group]" );

   if( group == null )
      return;

   group.querySelectorAll( "[data-filter-value]" ).forEach( function( other )
   {
      other.classList.remove( "is-active" );
   } );

   chip.classList.add( "is-active" );

   group.dispatchEvent( new CustomEvent( "filterchange", { detail: { value: chip.dataset.filterValue } } ) );
} );

function format_file_size( bytes )
{
   if( bytes < 1024 )
      return bytes + " B";

   if( bytes < 1024 * 1024 )
      return ( bytes / 1024 ).toFixed( 1 ) + " KB";

   return ( bytes / ( 1024 * 1024 ) ).toFixed( 1 ) + " MB";
}

function render_dropzone_files( dropzone, files )
{
   var list = dropzone.querySelector( "[data-dropzone-files]" );

   if( list == null )
      return;

   for( var i = 0; i < files.length; i++ )
   {
      var file = files[ i ];

      var item = document.createElement( "li" );
      item.className = "c-dropzone__file";

      var name = document.createElement( "span" );
      name.className = "c-dropzone__file-name";
      name.textContent = file.name;

      var size = document.createElement( "span" );
      size.className = "c-dropzone__file-size";
      size.textContent = format_file_size( file.size );

      var remove = document.createElement( "button" );
      remove.type = "button";
      remove.className = "c-dropzone__file-remove";
      remove.setAttribute( "aria-label", "Remove " + file.name );
      remove.textContent = "×";

      item.appendChild( name );
      item.appendChild( size );
      item.appendChild( remove );

      list.appendChild( item );
   }
}

document.addEventListener( "click", function( event )
{
   var remove = event.target.closest( ".c-dropzone__file-remove" );

   if( remove != null )
      remove.closest( ".c-dropzone__file" ).remove( );
} );

document.addEventListener( "change", function( event )
{
   var input = event.target.closest( "[data-dropzone-input]" );

   if( input == null )
      return;

   render_dropzone_files( input.closest( ".c-dropzone" ), input.files );
} );

[ "dragenter", "dragover" ].forEach( function( type )
{
   document.addEventListener( type, function( event )
   {
      var zone = event.target.closest( "[data-dropzone]" );

      if( zone == null )
         return;

      event.preventDefault( );
      zone.classList.add( "is-dragover" );
   } );
} );

document.addEventListener( "dragleave", function( event )
{
   var zone = event.target.closest( "[data-dropzone]" );

   if( zone != null )
      zone.classList.remove( "is-dragover" );
} );

document.addEventListener( "drop", function( event )
{
   var zone = event.target.closest( "[data-dropzone]" );

   if( zone == null )
      return;

   event.preventDefault( );
   zone.classList.remove( "is-dragover" );

   render_dropzone_files( zone, event.dataTransfer.files );
} );

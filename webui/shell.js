// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file
// license.txt in the root project directory or
// http://www.opensource.org/licenses/mit-license.php.

// NOTE: The shell an app's pages share (Damon, 2026-10-26: consistency across the apps) - "shell.css". Each page builds its own
// into the same markup: a side menu ("shell_rail") of the apps and the page's sections, with who is signed in; a top bar with the
// menu's button ("rail_open"); on a phone the menu a drawer over a scrim ("rail_scrim"), the sections chips under the top bar
// ("shell_chips") and the apps a bar along the bottom ("shell_bottombar"). Moved out of Home for My account.

const c_shell_icons = {
   home: "M3 10.5 12 3l9 7.5V20a1 1 0 0 1-1 1h-5v-6h-6v6H4a1 1 0 0 1-1-1z",
   chat: "M4 5h16v11H8l-4 4z",
   account: "M19 20v-1a4 4 0 0 0-4-4H9a4 4 0 0 0-4 4v1M12 11a4 4 0 1 0 0-8 4 4 0 0 0 0 8",
   people: "M16 19v-1a4 4 0 0 0-4-4H7a4 4 0 0 0-4 4v1M10 11a3 3 0 1 0 0-6 3 3 0 0 0 0 6M21 19v-1a4 4 0 0 0-3-3.9M16 5.1a3 3 0 0 1 0 5.8",
   console: "M4 6l5 5-5 5M12 18h8",
   overview: "M12 14l4-4M3.5 18a9 9 0 1 1 17 0",
   keys: "M15 7a4 4 0 1 1-3.5 6L4 20.5H2v-3l7.5-7.5A4 4 0 0 1 15 7z",
   logs: "M5 4h14v16H5zM8 8h8M8 12h8M8 16h5",
   shield: "M12 3l8 3v6c0 4.5-3.4 8.3-8 9-4.6-.7-8-4.5-8-9V6zM9 12l2 2 4-4",
   health: "M3 12h4l3-8 4 16 3-8h4",
   alerts: "M6 8a6 6 0 1 1 12 0c0 7 3 9 3 9H3s3-2 3-9M10.3 21a1.94 1.94 0 0 0 3.4 0",
   profile: "M12 12a4 4 0 1 0 0-8 4 4 0 0 0 0 8M4 21a8 8 0 0 1 16 0",
   contacts: "M5 4h12a2 2 0 0 1 2 2v14H7a2 2 0 0 1-2-2zM5 18a2 2 0 0 1 2-2h12M12 11a2.5 2.5 0 1 0 0-5 2.5 2.5 0 0 0 0 5",
   devices: "M4 5h12v9H4zM2 18h16M19 9h3v10h-3z",
   notify: "M6 8a6 6 0 1 1 12 0c0 7 3 9 3 9H3s3-2 3-9M10.3 21a1.94 1.94 0 0 0 3.4 0",
   password: "M6 11h12v10H6zM8 11V7a4 4 0 0 1 8 0v4M12 15v2"
};

function shell_icon( name, size )
{
   var svg = document.createElementNS( "http://www.w3.org/2000/svg", "svg" );

   svg.setAttribute( "width", String( size || 18 ) );
   svg.setAttribute( "height", String( size || 18 ) );
   svg.setAttribute( "viewBox", "0 0 24 24" );
   svg.setAttribute( "fill", "none" );
   svg.setAttribute( "stroke", "currentColor" );
   svg.setAttribute( "stroke-width", "1.8" );
   svg.setAttribute( "stroke-linecap", "round" );
   svg.setAttribute( "stroke-linejoin", "round" );
   svg.setAttribute( "aria-hidden", "true" );

   var path = document.createElementNS( "http://www.w3.org/2000/svg", "path" );

   path.setAttribute( "d", c_shell_icons[ name ] || "" );

   svg.appendChild( path );

   return svg;
}

// NOTE: An app opens in this tab, or a new one with ctrl or cmd and click or the middle button ("apps_go( )").
function shell_bind_link( item, on_click )
{
   item.addEventListener( "click", function( event )
   {
      event.preventDefault( );

      on_click( event );
   } );

   item.addEventListener( "auxclick", function( event )
   {
      if( event.button !== 1 )
         return;

      event.preventDefault( );

      on_click( event );
   } );
}

// NOTE: One of the page's own sections - in this tab always, its "#..." the address a reload comes back to.
function shell_bind_section( item, on_click )
{
   item.addEventListener( "click", function( event )
   {
      event.preventDefault( );

      on_click( event );
   } );
}

// NOTE: An item of the side menu - an app ("is_app") or one of the page's sections.
function shell_rail_item( key, title, is_app, on_click )
{
   var item = document.createElement( "a" );

   item.className = "shell-rail-item";
   item.href = "#" + key;
   item.dataset.key = key;

   item.appendChild( shell_icon( key ) );

   var label = document.createElement( "span" );

   label.className = "shell-rail-label";
   label.textContent = title;

   item.appendChild( label );

   if( is_app )
      shell_bind_link( item, on_click );
   else
      shell_bind_section( item, on_click );

   return item;
}

// NOTE: An app on a phone's bar along the bottom.
function shell_bottom_item( key, title, on_click )
{
   var item = document.createElement( "a" );

   item.className = "shell-bottom-item";
   item.href = "#";
   item.dataset.key = key;

   item.appendChild( shell_icon( key, 22 ) );

   var label = document.createElement( "span" );

   label.textContent = title;

   item.appendChild( label );

   shell_bind_link( item, on_click );

   return item;
}

// NOTE: One of the page's sections as a chip, under a phone's top bar.
function shell_chip( key, title, on_click )
{
   var chip = document.createElement( "a" );

   chip.className = "shell-chip";
   chip.href = "#" + key;
   chip.dataset.key = key;
   chip.textContent = title;

   shell_bind_section( chip, on_click );

   return chip;
}

// NOTE: The items for "keys" marked as where the page is - the menu's, the chips and the bottom bar's alike.
function shell_mark( keys )
{
   document.querySelectorAll( ".shell-rail-item, .shell-chip, .shell-bottom-item" ).forEach( function( item )
   {
      var current = ( keys.indexOf( item.dataset.key ) >= 0 );

      item.classList.toggle( "is-current", current );

      if( current )
         item.setAttribute( "aria-current", "page" );
      else
         item.removeAttribute( "aria-current" );
   } );
}

// NOTE: Who is signed in, at the foot of the side menu - the same initial and colour as the chat gives them.
function shell_show_user( name, role )
{
   var avatar = document.getElementById( "rail_avatar" );

   avatar.textContent = user_initial( name );
   avatar.style.background = "var(--color-sender-" + sender_colour_index( name ) + ")";

   document.getElementById( "rail_name" ).textContent = name;
   document.getElementById( "rail_role" ).textContent = role;
}

function shell_open_rail( )
{
   document.getElementById( "shell_rail" ).classList.add( "is-open" );
   document.getElementById( "rail_scrim" ).hidden = false;
   document.getElementById( "rail_open" ).setAttribute( "aria-expanded", "true" );
}

function shell_close_rail( )
{
   document.getElementById( "shell_rail" ).classList.remove( "is-open" );
   document.getElementById( "rail_scrim" ).hidden = true;
   document.getElementById( "rail_open" ).setAttribute( "aria-expanded", "false" );
}

// NOTE: Once, as the page starts - the menu's button, the scrim and Escape.
function shell_install( )
{
   document.getElementById( "rail_open" ).addEventListener( "click", shell_open_rail );
   document.getElementById( "rail_scrim" ).addEventListener( "click", shell_close_rail );

   document.addEventListener( "keydown", function( event )
   {
      if( ( event.key === "Escape" ) && document.getElementById( "shell_rail" ).classList.contains( "is-open" ) )
         shell_close_rail( );
   } );
}

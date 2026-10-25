// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: Bubbles to pop, for a person's own randomness - every pop's time, place and colour, and each miss's moment, taken as
// samples ("randomness_hex( )" in "home_parse.js" makes hex of them). First in Home's set up (2026-10-13), and since
// 2026-10-25 in setting up an account's keys too, so here for both. "bubble_field( )" takes the box and:
//
//    state     - a function giving where the samples and the count are kept ({ samples: [ ], pops: 0 })
//    pops      - how many to pop (twelve)
//    most      - the most in the box at once
//    on_count  - called after each pop
//    on_done   - called once the last is popped
//
// and gives "start( )", "stop( )" and "clear( )". The look is in "account.css" (".home-bubble"), which both pages load.
function bubble_field( field, options )
{
   var bubbles = [ ];
   var frame = 0;
   var last = 0;
   var next_id = 0;

   var state = function( ) { return options.state( ) || { samples: [ ], pops: 0 }; };

   // NOTE: Less motion asked for - they rise slowly.
   var speed_now = function( )
   {
      return window.matchMedia( "(prefers-reduced-motion: reduce)" ).matches ? 0.35 : 1;
   };

   // NOTE: Where and when - a key press has no pointer, so the bubble's own place.
   var sample = function( event, bubble )
   {
      var box = field.getBoundingClientRect( );
      var from_pointer = event && ( event.clientX || event.clientY );

      return { t: performance.now( ), hit: !!bubble, c: bubble ? bubble.colour : "",
       x: from_pointer ? event.clientX - box.left : ( bubble ? bubble.left + bubble.size / 2 : 0 ),
       y: from_pointer ? event.clientY - box.top : ( bubble ? bubble.y + bubble.size / 2 : 0 ) };
   };

   var place = function( bubble, now )
   {
      bubble.left = bubble.x + Math.sin( now * bubble.wobble + bubble.phase ) * bubble.sway;

      bubble.element.style.transform = "translate(" + bubble.left.toFixed( 1 ) + "px, " + bubble.y.toFixed( 1 ) + "px)";
   };

   // NOTE: A bubble's burst where it was - for a pop, and for those left when the last is popped.
   var burst = function( bubble )
   {
      var shown = document.createElement( "span" );

      shown.className = "home-bubble-burst";
      shown.setAttribute( "aria-hidden", "true" );
      shown.style.setProperty( "--bubble-colour", bubble.rgb );
      shown.style.width = bubble.size + "px";
      shown.style.height = bubble.size + "px";
      shown.style.transform = "translate(" + bubble.left.toFixed( 1 ) + "px, " + bubble.y.toFixed( 1 ) + "px)";

      field.appendChild( shown );

      window.setTimeout( function( ) { shown.remove( ); }, 450 );

      bubble.element.remove( );
   };

   var stop = function( )
   {
      if( frame !== 0 )
         window.cancelAnimationFrame( frame );

      frame = 0;
   };

   var pop = function( bubble, event )
   {
      var kept = state( );

      if( kept.pops >= options.pops )
         return;

      kept.samples.push( sample( event, bubble ) );
      kept.pops++;

      burst( bubble );

      bubbles = bubbles.filter( function( other ) { return other !== bubble; } );

      if( options.on_count )
         options.on_count( );

      // NOTE: The last - the rest burst too (Ian, 2026-10-13), not counted: the randomness is the twelve. The box stops taking
      // clicks, and the page moves the focus on (Damon).
      if( kept.pops === options.pops )
      {
         bubbles.forEach( burst );
         bubbles = [ ];

         stop( );

         field.classList.add( "is-done" );

         if( options.on_done )
            options.on_done( );
      }
   };

   var add = function( anywhere )
   {
      var width = field.clientWidth || 430;
      var height = field.clientHeight || 300;
      var size = 26 + Math.random( ) * 22;

      // NOTE: Each its own colour, from the browser's secure random numbers - kept with its pop (Ian, 2026-10-13).
      var bytes = new Uint8Array( 3 );

      window.crypto.getRandomValues( bytes );

      var colour = bubble_colour( bytes );

      var button = document.createElement( "button" );

      button.type = "button";
      button.className = "home-bubble";
      button.setAttribute( "aria-label", "Pop this bubble" );
      button.style.width = size + "px";
      button.style.height = size + "px";
      button.style.setProperty( "--bubble-colour", colour.rgb );

      // NOTE: Livelier since 2026-10-13 (Ian: "move around a more and a little faster") - a drift sideways that wanders, as
      // well as a wider, quicker sway.
      var bubble = { id: ++next_id, element: button, size: size, width: width,
       x: 8 + Math.random( ) * Math.max( 1, width - size - 16 ),
       y: anywhere ? 24 + Math.random( ) * Math.max( 1, height - size - 40 ) : height + 4,
       rise: 1 + Math.random( ) * 1.4, drift: ( Math.random( ) - 0.5 ) * 0.06,
       sway: 8 + Math.random( ) * 20, wobble: 0.0015 + Math.random( ) * 0.002, phase: Math.random( ) * 6.283, left: 0,
       colour: colour.hex, rgb: colour.rgb };

      button.addEventListener( "click", function( event )
      {
         event.stopPropagation( );

         pop( bubble, event );
      } );

      field.appendChild( button );
      bubbles.push( bubble );

      place( bubble, performance.now( ) );
   };

   // NOTE: Each rises at its own pace, which wanders, and sways on its own rhythm - as bubbles do. New ones come at random.
   var step = function( now )
   {
      var elapsed = Math.min( 64, now - last );
      var speed = speed_now( );

      last = now;

      bubbles = bubbles.filter( function( bubble )
      {
         bubble.y -= bubble.rise * 0.035 * elapsed * speed;
         bubble.rise = Math.max( 0.8, Math.min( 3, bubble.rise + ( Math.random( ) - 0.5 ) * 0.04 ) );

         // NOTE: The drift wanders, and turns back off the sides.
         bubble.drift = Math.max( -0.09, Math.min( 0.09, bubble.drift + ( Math.random( ) - 0.5 ) * 0.008 ) );
         bubble.x += bubble.drift * elapsed * speed;

         var room = Math.max( 8, bubble.width - bubble.size - 8 - bubble.sway );

         if( ( bubble.x < bubble.sway ) || ( bubble.x > room ) )
         {
            bubble.x = Math.max( bubble.sway, Math.min( room, bubble.x ) );
            bubble.drift = -bubble.drift;
         }

         if( bubble.y + bubble.size < -6 )
         {
            bubble.element.remove( );

            return false;
         }

         place( bubble, now );

         return true;
      } );

      // NOTE: More of them since 2026-10-13 (Ian) - up to sixteen, so popping is picking some out here and there, not every
      // one that rises - and sooner still with only a few in the box.
      var chance = ( 0.07 * speed + ( bubbles.length < 6 ? 0.08 : 0 ) ) * elapsed / 40;

      if( ( state( ).pops < options.pops ) && ( bubbles.length < options.most ) && ( Math.random( ) < chance ) )
         add( false );

      frame = window.requestAnimationFrame( step );
   };

   var start = function( )
   {
      if( frame !== 0 )
         return;

      // NOTE: All popped - none to pop, and the box no longer a click target, which took the focus off Next (Damon).
      field.classList.toggle( "is-done", state( ).pops >= options.pops );

      if( ( bubbles.length === 0 ) && ( state( ).pops < options.pops ) )
      {
         for( var i = 0; i < 8; i++ )
            add( true );
      }

      last = performance.now( );
      frame = window.requestAnimationFrame( step );
   };

   var clear = function( )
   {
      stop( );

      bubbles = [ ];

      field.replaceChildren( );
      field.classList.remove( "is-done" );
   };

   // NOTE: A tap that misses still adds its moment - only a pop counts.
   field.addEventListener( "pointerdown", function( event )
   {
      var kept = options.state( );

      if( ( event.target === field ) && kept && ( kept.pops < options.pops ) )
         kept.samples.push( sample( event, null ) );
   } );

   return { start: start, stop: stop, clear: clear };
}

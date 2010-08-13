/*
 * ===========================================================================
 *
 *       Filename:  OptionMenu.cpp
 *
 *    Description:  Options menu
 *
 *        Version:  1.0
 *        Created:  08/12/2010 08:53:33 PM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

#include "OptionMenu.h"
#include "GLUtil.h"
#include "VBA.h"

#include <SDL.h>
#include <SDL_ttf.h>

void drawOptionsMenu( SDL_Surface * surface );

//Call this to display the options menu...
void optionsMenu()
{
  SDL_Surface * surface = SDL_GetVideoSurface();
  SDL_Surface * options_screen = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, 24, 
      0x0000ff, 0x00ff00, 0xff0000, 0);

  if (!options_screen)
  {
    fprintf( stderr, "Error creating options menu!\n" );
    exit( 1 );
  }

  TTF_Font * font_normal = TTF_OpenFont( FONT, 18 );

  SDL_Event event;
  bool done = false;
  while (!done)
  {
    drawOptionsMenu( options_screen );
    SDL_DrawSurfaceAsGLTexture( options_screen, portrait_vertexCoords );

    while ( SDL_PollEvent( &event ) )
    {
      if ( event.type == SDL_MOUSEBUTTONDOWN )
      {
        done = true;
      }

    }
    SDL_Delay( 100 );
  }

  SDL_FreeSurface( options_screen );
}

void drawOptionsMenu( SDL_Surface * surface )
{
  //Same as border color of rom selector
  int color = SDL_MapRGB( surface->format, 85, 0, 0);
  SDL_FillRect( surface, NULL, color );
}

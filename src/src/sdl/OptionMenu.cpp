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

enum menuState
{
  MENU_MAIN,
  MENU_SAVES,
  MENU_OPTIONS,
  MENU_HELP
};

enum optionType {
  MENU_TOGGLE,
  MENU_SAVE,
  MENU_BUTTON,
};

typedef struct
{
  char * on_text;
  char * off_text;
  void (*change)(bool);
} toggle_data;

typedef struct
{
  int save_num;
  void (*change)(int,bool);
} save_data;

typedef struct
{
  char * button_text;
  void (*action)(void);
} button_data;

typedef struct
{
  char * text;
  enum optionType type;
  union
  {
    toggle_data toggle;
    save_data save;
    button_data button;
  };
} menuOption;


enum menuState menuState;
bool menuDone;

void drawOptionsMenu( SDL_Surface * surface );
void handleOptionsEvent( SDL_Event * event );

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

  //Start out at top-level menu
  menuState = MENU_MAIN;
  menuDone = false;
  while (!menuDone)
  {

    drawOptionsMenu( options_screen );

    while ( SDL_PollEvent( &event ) )
    {
      handleOptionsEvent( &event );
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

  switch( menuState )
  {
    case MENU_MAIN:
      //Top-level menu.
      //Show list of other menus
      break;
    case MENU_SAVES:
      break;
    case MENU_OPTIONS:
      break;
    case MENU_HELP:
      break;
    default:
      fprintf( stderr, "Unexpected menu state?!\n");
      exit(1);
      break;
  }

  SDL_DrawSurfaceAsGLTexture( surface, portrait_vertexCoords );
}

void handleOptionsEvent( SDL_Event * event )
{

}

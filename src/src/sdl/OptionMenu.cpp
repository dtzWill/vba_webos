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

#define OPTION_SIZE 40

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

bool menuInitialized = false;
menuOption * topMenu = NULL;
menuOption * saveMenu = NULL;
menuOption * optionMenu = NULL;
menuOption * helpMenu = NULL;

enum menuState menuState;
bool menuDone;

void initializeMenu();
void drawOptionsMenu( SDL_Surface * surface );
void handleOptionsEvent( SDL_Event * event );
void drawMenu( menuOption * options, SDL_Surface * surface, int numOptions );


/*-----------------------------------------------------------------------------
 *  Functors for menu options...
 *-----------------------------------------------------------------------------*/
void changeToMainState(void)     { menuState = MENU_MAIN;    }
void changeToSaveState(void)     { menuState = MENU_SAVES;   }
void changeToOptionsState(void)  { menuState = MENU_OPTIONS; }
void changeToHelpState(void)     { menuState = MENU_HELP;    }

void exitMenu(void)              { menuDone = true;          }
void moveToRomSelector(void);
void handleMenuSaveState(int,bool);

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

  initializeMenu();

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

void initializeMenu()
{
  if (menuInitialized) return;
  menuInitialized = true;

  //Static initializers for all this is a PITA, so do it dynamically.
  topMenu = (menuOption*)malloc(5*sizeof(menuOption));
  
  //Top-level menu
  topMenu[0].text = "Save states";
  topMenu[0].type = MENU_BUTTON;
  topMenu[0].button.action = changeToSaveState;

  topMenu[1].text = "Options";
  topMenu[1].type = MENU_BUTTON;
  topMenu[1].button.action = changeToOptionsState;

  topMenu[2].text = "Help";
  topMenu[2].type = MENU_BUTTON;
  topMenu[2].button.action = changeToHelpState;

  topMenu[3].text = "Rom Selector";
  topMenu[3].type = MENU_BUTTON;
  topMenu[3].button.action = moveToRomSelector;

  topMenu[4].text = "Return";
  topMenu[4].type = MENU_BUTTON;
  topMenu[4].button.action = exitMenu;

  //Save menu
  saveMenu = (menuOption*)malloc(4*sizeof(menuOption));
  saveMenu[0].text = "Save 1";
  saveMenu[0].type = MENU_SAVE;
  saveMenu[0].save.save_num = 1;
  saveMenu[0].save.change = handleMenuSaveState;

  saveMenu[1].text = "Save 2";
  saveMenu[1].type = MENU_SAVE;
  saveMenu[1].save.save_num = 2;
  saveMenu[1].save.change = handleMenuSaveState;

  saveMenu[2].text = "Save 3";
  saveMenu[2].type = MENU_SAVE;
  saveMenu[2].save.save_num = 3;
  saveMenu[2].save.change = handleMenuSaveState;

  saveMenu[3].text = "Return";
  saveMenu[3].type = MENU_BUTTON;
  saveMenu[3].button.action = changeToMainState;
  
  //Options menu
  //FIXME: implement!
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
      drawMenu(topMenu, surface, 5);
      break;
    case MENU_SAVES:
      drawMenu(topMenu, surface, 4);
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

void moveToRomSelector(void)
{
  //XXX IMPLEMENT ME!

}

void handleMenuSaveState( int save_num, bool load )
{
  //XXX IMPLEMENT ME!

  menuDone = true;
}

//Calculate y-coord for given option number
int y_coord( int optionNum, int numOptions, int height )
{
  int half = numOptions/2;
  return height/2 + (optionNum-half)*OPTION_SIZE;

}

void drawMenu( menuOption * options, SDL_Surface * surface, int numOptions )
{
  //For each option, draw it!
  TTF_Font * font_normal = TTF_OpenFont( FONT, 18 );
  for ( int i = 0; i < numOptions; ++i )
  {
    int y = y_coord(i, numOptions, surface->h);
    SDL_Surface * option;

    switch( options[i].type )
    {
      case MENU_TOGGLE:
        //Label, then on/off
        break;
      case MENU_SAVE:
        //Label, then load/store
        break;
      case MENU_BUTTON:
        //Label
        break;
    }

  }

}

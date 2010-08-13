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
  SDL_Surface * surface;
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
TTF_Font * menu_font = NULL;

enum menuState menuState;
bool menuDone;

void initializeMenu();
void doMenu( SDL_Surface * s );

SDL_Color textColor = { 255, 255, 255 };
SDL_Color itemColor = { 0, 0, 0 }

/*-----------------------------------------------------------------------------
 *  Constructors for menu items
 *-----------------------------------------------------------------------------*/
menuOption createButton( char * text, void (*action)(void) )
{
  menuOption opt;
  opt.text = text;
  opt.type = MENU_BUTTON;
  opt.action = action;
  opt.surface = TTF_RenderText_Blended( menu_font, text, textColor );
}

#if 0
menuOption createSave( char * text, void (*action)(void) )
{
  menuOption opt;
  opt.text = text;
  opt.type = MENU_BUTTON;
  opt.action = action;
  opt.surface = TTF_RenderText_Blended( menu_font, text, textColor );
}
#endif

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
    doMenu( options_screen );
  }

  SDL_FreeSurface( options_screen );
}

void initializeMenu()
{
  if (menuInitialized) return;
  menuInitialized = true;

  menu_font = TTF_OpenFont( FONT, 18 );

  //Static initializers for all this is a PITA, so do it dynamically.
  topMenu = (menuOption*)malloc(5*sizeof(menuOption));
  
  //Top-level menu
  topMenu[0] = createButton( "Save states", changeToSaveState );
  topMenu[1] = createButton( "Options", changeToOptionsState );
  topMenu[2] = createButton( "Help", changeToHelpState );
  topMenu[3] = createButton( "Rom Selector", moveToRomSelector );
  topMenu[4] = createButton( "Return", exitMenu );

  //Save menu
#if 0
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
#endif
  
  //Options menu
  //FIXME: implement!
}

void doMenu( SDL_Surface * s )
{
  switch( menuState )
  {
    case MENU_MAIN:
      doMainMenu( s );
      break;
    case MENU_SAVES:
    case MENU_OPTIONS:
    case MENU_HELP:
    default:
      break;
  }
}

void doMainMenu( SDL_Surface * s )
{
  bool done = false;
  while(!done)
  {
    for ( int i = 0; i < 5; ++i )
    {
      int y = 100 + 
      //Draw topMenu[i]
      topMenu[i]
    }
}

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
#include "RomSelector.h"
#include "Options.h"

#include <SDL.h>
#include <SDL_ttf.h>

#define OPTION_SIZE 40
#define OPTION_SPACING 50
#define OPTION_WIDTH 300

//Toggle stuff
#define TOGGLE_TXT_X 10
#define TOGGLE_ON_X 160
#define TOGGLE_OFF_X 240
#define TOGGLE_Y 5

//Colors (BGR format)
static SDL_Color textColor = { 255, 255, 255 };
static SDL_Color onColor   = { 255, 200, 200 };
static SDL_Color offColor  = {  50,  50,  50 };
static SDL_Color itemColor = {   0,   0,   0 };
static SDL_Color linkColor = { 255, 200, 200 };
static SDL_Color hiColor   = { 200, 200, 255 };

//Help screen stuff

typedef struct
{
  char * msg;
  SDL_Color color;
} line;

line helpScreen1[] =
{
    {"Welcome to VisualBoyAdvance (VBA)!",          textColor },
    {" ",                                           textColor },
    {"VBA is a Gameboy, Gameboy Color,",            textColor },
    {"and Gameboy Advance emulator.",               textColor },
    {" ",                                           textColor },
    {"What that means is VBA allows you",           textColor },
    {"to play games made for those systems--",      textColor },
    {"however, much like your gameboy needs",       textColor },
    {"separate games to play, VBA needs games",     textColor },
    {"too.  These games are generally called",      textColor },
    {"'ROM's, which are computer copies of",        textColor },
    {"games for those devices.",                    textColor },
    {" ",                                           textColor },
    {"(Click to go to next screen)",                linkColor }
};
line helpScreen2[] =
{
    {"Where do I get ROMs?",                        textColor },
    {" ",                                           textColor },
    {"There are many great ROMs freely available",  textColor },
    {"all over the internet.  Examples of such",    textColor },
    {"include \"Anguna\" and \"Another World\".",   textColor },
    {"VBA can also play many commercial games",     textColor },
    {"made for the Gameboy (Color/Advance)",        textColor },
    {"but how to get those ROMs is something",      textColor },
    {"we don't cover, and you are responsible",     textColor },
    {"for checking the legality of them",           textColor },
    {"in your country.",                            textColor },
    {" ",                                           textColor },
    {" ",                                           textColor },
    {"(Click to go to next screen)",                linkColor }
};
line helpScreen3[] =
{
    {"Okay I got the ROMs, what now?",              textColor },
    {" ",                                           textColor },
    {"To play your ROMs, connect your device",      textColor },
    {"to your computer and put it in USB mode.",    textColor },
    {"You'll want to put your ROMs in",             textColor },
    {"a folder called",                             textColor },
    {"/vba/roms",                                   hiColor },
    {"which you might have to create.",             textColor },
    {"First create a 'vba' folder then create",     textColor },
    {"a 'roms' folder inside of that.",             textColor },
    {"Watch the capitalization, all lower case.",   textColor },
    {"Once you have the ROMs there, restart VBA",   textColor },
    {"and then just click them to play.",           textColor },
    {"(Click to go to next screen)",                linkColor }
};
line helpScreen4[] =
{
    {"How do I play?",                              textColor },
    {" ",                                           textColor },
    {"VBA initially gives you onscreen controls.",  textColor },
    {"You can also use the physical keyboard.",     textColor },
    {"W/A/S/D --- up/down/left/right (d-pad)",      textColor },
    {"K/L --- B/A (action buttons)",                textColor },
    {"Q/P --- L/R (trigger buttons) folder called", textColor },
    {"(Click to go to next screen)",                linkColor }
};

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
  void (*set)(bool);
  bool (*get)(void);
} toggle_data;

typedef struct
{
  int save_num;
} save_data;

typedef struct
{
  void (*action)(void);
} button_data;

typedef struct
{
  char * text;
  int y;
  enum optionType type;
  SDL_Surface * surface;
  union
  {
    toggle_data toggle;
    save_data save;
    button_data button;
  };
} menuOption;

static menuOption * topMenu = NULL;
static menuOption * saveMenu = NULL;
static menuOption * optionMenu = NULL;
static menuOption * helpMenu = NULL;
static TTF_Font * menu_font = NULL;

static enum menuState menuState;
static bool menuDone;
static eMenuResponse menuResponse;

void initializeMenu();
void doMenu( SDL_Surface * s, menuOption * options, int numOptions );
bool optionHitCheck( menuOption * opt, int x, int y );
void freeMenu();
void showLines( SDL_Surface * s, line * lines, int numlines, bool center );

/*-----------------------------------------------------------------------------
 *  Constructors for menu items
 *-----------------------------------------------------------------------------*/
menuOption createButton( char * text, void (*action)(void), int y )
{
  menuOption opt;
  opt.text = strdup( text );
  opt.type = MENU_BUTTON;
  opt.button.action = action;
  opt.y = y;
  opt.surface = NULL;

  //Black rectangle
  opt.surface = SDL_CreateRGBSurface( SDL_SWSURFACE, OPTION_WIDTH, OPTION_SIZE, 24, 
      0x0000ff, 0x00ff00, 0xff0000, 0);
  int black = SDL_MapRGB( opt.surface->format, 0, 0, 0);
  SDL_FillRect( opt.surface, NULL, black );

  //With text on it
  SDL_Surface * s_txt = TTF_RenderText_Blended( menu_font, text, textColor );
  int xx = opt.surface->w/2 - s_txt->w/2;
  int yy = opt.surface->h/2 - s_txt->h/2;
  apply_surface( xx, yy, s_txt, opt.surface );

  SDL_FreeSurface( s_txt );


  return opt;
}

menuOption createToggle( char * text, char * on, char * off, int y, void (*set)(bool), bool (*get)(void) )
{
  menuOption opt;
  opt.text = strdup( text );
  opt.type = MENU_TOGGLE;
  opt.toggle.on_text = strdup( on );
  opt.toggle.off_text = strdup( off );
  opt.toggle.set = set;
  opt.toggle.get = get;
  opt.y = y;
  opt.surface = NULL;

  return opt;
}

menuOption createSave( int num, int y )
{
  char buf[1024];
  sprintf( buf, "Save %d", num + 1 );
  menuOption opt;
  opt.text = strdup( buf );
  opt.type = MENU_SAVE;
  opt.save.save_num = num;
  opt.y = y;

  //Black rectangle
  opt.surface = SDL_CreateRGBSurface( SDL_SWSURFACE, OPTION_WIDTH, OPTION_SIZE, 24, 
      0xff0000, 0x00ff00, 0x0000ff, 0);
  int black = SDL_MapRGB( opt.surface->format, 0, 0, 0);
  SDL_FillRect( opt.surface, NULL, black );

  //Create "Save 1:  load  save"-style display
  SDL_Surface * s_txt  = TTF_RenderText_Blended( menu_font, opt.text, textColor );
  SDL_Surface * s_load = TTF_RenderText_Blended( menu_font, "Load",   textColor );
  SDL_Surface * s_save = TTF_RenderText_Blended( menu_font, "Save",   textColor );

  apply_surface( TOGGLE_TXT_X, TOGGLE_Y, s_txt,  opt.surface );
  apply_surface( TOGGLE_ON_X,  TOGGLE_Y, s_load, opt.surface );
  apply_surface( TOGGLE_OFF_X, TOGGLE_Y, s_save, opt.surface );

  SDL_FreeSurface( s_txt  );
  SDL_FreeSurface( s_load );
  SDL_FreeSurface( s_save );

  return opt;
}


/*-----------------------------------------------------------------------------
 *  Menu option rendering routines
 *-----------------------------------------------------------------------------*/
void updateToggleSurface( menuOption * opt )
{
  if ( opt->surface )
    SDL_FreeSurface( opt->surface );

  //Black rectangle
  opt->surface = SDL_CreateRGBSurface( SDL_SWSURFACE, OPTION_WIDTH, OPTION_SIZE, 24, 
      0xff0000, 0x00ff00, 0x0000ff, 0);
  int black = SDL_MapRGB( opt->surface->format, 0, 0, 0);
  SDL_FillRect( opt->surface, NULL, black );

  //Render each piece...
  SDL_Color on_color = opt->toggle.get() ? onColor : offColor;
  SDL_Color off_color = opt->toggle.get() ? offColor : onColor;
  SDL_Surface * s_txt = TTF_RenderText_Blended( menu_font, opt->text,            textColor );
  SDL_Surface * s_on  = TTF_RenderText_Blended( menu_font, opt->toggle.on_text,  on_color );
  SDL_Surface * s_off = TTF_RenderText_Blended( menu_font, opt->toggle.off_text, off_color );

  apply_surface( TOGGLE_TXT_X, TOGGLE_Y, s_txt, opt->surface );
  apply_surface( TOGGLE_ON_X,  TOGGLE_Y, s_on,  opt->surface );
  apply_surface( TOGGLE_OFF_X, TOGGLE_Y, s_off, opt->surface );

  SDL_FreeSurface( s_txt );
  SDL_FreeSurface( s_on  );
  SDL_FreeSurface( s_off );
}

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

void menuSetOrientation( bool portrait )
{
  orientation = portrait ? ORIENTATION_PORTRAIT : ORIENTATION_LANDSCAPE_R;
  updateOrientation();
}

void menuSetSound( bool sound )   { soundMute = !sound;                          }
void menuSetFilter( bool smooth ) { gl_filter = smooth ? GL_LINEAR : GL_NEAREST;
                                    GL_InitTexture();                            }
void menuSetSpeed( bool show )    { showSpeed = show ? 1 : 0;                    }
void menuSetAutoSave( bool on )   { autosave = on;                               }
void menuSetAutoSkip( bool on )   { autoFrameSkip = on;                          }
void menuSetOnscreen( bool on )   { use_on_screen = on;                          }

bool menuGetOrientation() { return orientation == ORIENTATION_PORTRAIT; }
bool menuGetSound()       { return !soundMute;                          }
bool menuGetFilter()      { return gl_filter == GL_LINEAR;              }
bool menuGetSpeed()       { return showSpeed != 0;                      }
bool menuGetAutoSave()    { return autosave;                            }
bool menuGetAutoSkip()    { return autoFrameSkip;                       }
bool menuGetOnscreen()    { return use_on_screen;                       }

//Call this to display the options menu...
eMenuResponse optionsMenu()
{
  SDL_Surface * surface = SDL_GetVideoSurface();
  SDL_Surface * options_screen = SDL_CreateRGBSurface( SDL_SWSURFACE, surface->w, surface->h, 24, 
      0x0000ff, 0x00ff00, 0xff0000, 0);

  if (!options_screen)
  {
    fprintf( stderr, "Error creating options menu!\n" );
    exit( 1 );
  }

  //Make sure we have the proper values...
  readOptions();

  initializeMenu();

  //Start out at top-level menu
  menuState = MENU_MAIN;
  menuDone = false;
  menuResponse = MENU_RESPONSE_RESUME;
  while (!menuDone)
  {
    switch( menuState )
    {
      case MENU_MAIN:
        doMenu( options_screen, topMenu, emulating? 5 : 3 );
        break;
      case MENU_OPTIONS:
        doMenu( options_screen, optionMenu, 8 );
        break;
      case MENU_SAVES:
        doMenu( options_screen, saveMenu, 4 );
        break;
      case MENU_HELP:
        doHelp( options_screen );
        break;
      default:
        break;
    }
  }

  SDL_FreeSurface( options_screen );

  freeMenu();

  return menuResponse;
}

void initializeMenu()
{
  menu_font = TTF_OpenFont( FONT, 18 );

  //Static initializers for all this is a PITA, so do it dynamically.
  
  //Top-level menu
  int x = 0;
  topMenu = (menuOption*)malloc( ( emulating ? 5 : 3 )*sizeof(menuOption));
  if (emulating)
    topMenu[x++] = createButton( "Save states",           changeToSaveState,   100+x*OPTION_SPACING);
  topMenu[x++] =   createButton( "Options",               changeToOptionsState,100+x*OPTION_SPACING);
  topMenu[x++] =   createButton( "Help",                  changeToHelpState,   100+x*OPTION_SPACING);
  if (emulating)
    topMenu[x++] = createButton( "Choose different game", moveToRomSelector,   100+x*OPTION_SPACING);
  topMenu[x++] =   createButton( "Return",                exitMenu,            100+x*OPTION_SPACING);

  //Save menu
  saveMenu = (menuOption*)malloc(4*sizeof(menuOption));
  x = 0;
  saveMenu[x++] = createSave( x, 100+x*OPTION_SPACING );
  saveMenu[x++] = createSave( x, 100+x*OPTION_SPACING );
  saveMenu[x++] = createSave( x, 100+x*OPTION_SPACING );
  saveMenu[x++] = createButton( "Return", changeToMainState, 100+x*OPTION_SPACING );
  
  //Options menu
  optionMenu = (menuOption*)malloc(8*sizeof(menuOption));
  x = 0;
  optionMenu[x++] = createToggle( "Orientation",   "Port",   "Land",  50+x*OPTION_SPACING,
      menuSetOrientation, menuGetOrientation );
  optionMenu[x++] = createToggle( "Sound",         "On",     "Off",   50+x*OPTION_SPACING,
      menuSetSound, menuGetSound );
  optionMenu[x++] = createToggle( "Filter",        "Smooth", "Sharp", 50+x*OPTION_SPACING,
      menuSetFilter, menuGetFilter );
  optionMenu[x++] = createToggle( "Show Speed",    "On",     "Off",   50+x*OPTION_SPACING,
      menuSetSpeed, menuGetSpeed );
  optionMenu[x++] = createToggle( "Autosave",      "On",     "Off",   50+x*OPTION_SPACING,
      menuSetAutoSave, menuGetAutoSave );
  optionMenu[x++] = createToggle( "Autoframeskip", "On",     "Off",   50+x*OPTION_SPACING,
      menuSetAutoSkip, menuGetAutoSkip );
  optionMenu[x++] = createToggle( "Touchscreen",   "On",     "Off",   50+x*OPTION_SPACING,
      menuSetOnscreen, menuGetOnscreen );
  optionMenu[x++] = createButton( "Return", changeToMainState, 50+x*OPTION_SPACING );
}

void freeMenu( menuOption ** opt, int numOptions )
{
  for ( int i = 0; i < numOptions; ++i )
  {
    menuOption o = (*opt)[i];
    free( o.text );
    SDL_FreeSurface( o.surface );
    switch( o.type )
    {
      case MENU_TOGGLE:
        free( o.toggle.on_text );
        free( o.toggle.off_text );
        break;
      case MENU_SAVE:
        break;
      case MENU_BUTTON:
        break;
    }
  }

  //Free the array...
  free( *opt );
  *opt = NULL;
}

void freeMenu()
{
  freeMenu( &topMenu, emulating ? 5 : 3 );
  freeMenu( &saveMenu, 4 );
  freeMenu( &optionMenu, 8 );
}

void doMenu( SDL_Surface * s, menuOption * options, int numOptions )
{
  // Menu background, same as rom selector
  int menuBGColor = SDL_MapRGB( s->format, 85, 0, 0 );//BGR
  SDL_FillRect( s, NULL, menuBGColor );

  //Draw the menu we were given...
  for ( int i = 0; i < numOptions; ++i )
  {
    if ( options[i].type == MENU_TOGGLE )
      updateToggleSurface( &options[i] );

    //Draw option on top of the box we drew
    int x = s->w / 2 - options[i].surface->w/2;
    int y = options[i].y;
    apply_surface( x, y, options[i].surface, s );
  }

  // Loop, redrawing menu (in case card gets invalidated, etc)
  // until something interesting happens...
  bool done = false;
  SDL_Event event;
  while( !done )
  {
    SDL_DrawSurfaceAsGLTexture( s, portrait_vertexCoords );
    while( SDL_PollEvent( &event ) )
    {
      switch ( event.type )
      {
        case SDL_MOUSEBUTTONUP:
          //Find which option this was, if it's somehow multiple we just take the first such option
          for ( int i = 0; i < numOptions; ++i )
          {
            if ( optionHitCheck( &options[i], event.button.x, event.button.y ) )
            {
              printf( "Chose: %s\n", options[i].text );
              done = true;
              //Make sure any changes to options are saved.
              writeOptions();
              break;
            }
          }
          break;
        case SDL_KEYUP:
          //Back-gesture /swipe back
          if ( event.key.keysym.sym == SDLK_ESCAPE )
          {
            //If top-level, exit menu, else go to main menu
            if ( menuState == MENU_MAIN )
              menuDone = true;
            else
              menuState = MENU_MAIN;
            done = true;
          }
          break;
        default:
          break;
      }
    }
  }
}

void doHelp( SDL_Surface * s )
{
  //Show help text, taps advance to next until done.
  showLines( s, helpScreen1, sizeof(helpScreen1)/sizeof(helpScreen1[0]), false );
  showLines( s, helpScreen2, sizeof(helpScreen2)/sizeof(helpScreen2[0]), false );
  showLines( s, helpScreen3, sizeof(helpScreen3)/sizeof(helpScreen3[0]), false );
  changeToMainState();
}

//Determine if this click hits this option... if so, take the right action!
bool optionHitCheck( menuOption * opt, int x, int y )
{
  bool hit = false;

  if ( y >= opt->y && y <= opt->y + OPTION_SIZE )
  {
    switch( opt->type )
    {
      case MENU_BUTTON:
        //Buttons are easy :)
        hit = true;
        opt->button.action();
        break;
      case MENU_SAVE:
        if ( x >= TOGGLE_ON_X && x < TOGGLE_OFF_X )
        {
          hit = true;
          sdlReadState(opt->save.save_num);
          menuDone = true;
        } else if ( x >= TOGGLE_OFF_X )
        {
          hit = true;
          sdlWriteState(opt->save.save_num);
          menuDone = true;
        }
        break;
      case MENU_TOGGLE:
#if 0
        if ( x >= TOGGLE_ON_X && x < TOGGLE_OFF_X )
        {
          hit = true;
          opt->toggle.set(true);
        } else if ( x >= TOGGLE_OFF_X )
        {
          hit = true;
          opt->toggle.set(false);
        }
#else
        //Instead of having the user CHOOSE which, tapping this option
        //toggles between the two available options.
        hit = true;
        opt->toggle.set(!opt->toggle.get());
#endif
        break;
    }
  }

  return hit;

}

void moveToRomSelector()
{
  menuDone = true;
  menuResponse = MENU_RESPONSE_ROMSELECTOR;
}

void showLines( SDL_Surface * s, line * lines, int numlines, bool center )
{
    // Menu background, same as rom selector
    int menuBGColor = SDL_MapRGB( s->format, 85, 0, 0 );//BGR
    SDL_FillRect( s, NULL, menuBGColor );

    //Black rectangle behind text...
    SDL_Rect drawRect;
    drawRect.x = 10;
    drawRect.y = 20;
    drawRect.h = s->h - 30;
    drawRect.w = s->w-20;
    int black = SDL_MapRGB(s->format, 0, 0, 0);
    SDL_FillRect(s, &drawRect, black);
    
    //Draw the lines specified...
    SDL_Surface * nr[numlines];
    int offset = 30;//arbitrary offset, centering all this isn't worth it.
    TTF_Font * font_normal = TTF_OpenFont( FONT, 16 );
    for ( int i = 0; i < numlines; ++i )
    {
        nr[i] = TTF_RenderText_Blended( font_normal, lines[i].msg, lines[i].color );
        int x;
        if (center)
            x = s->w/2-nr[i]->w/2;
        else
            x = 20;
        apply_surface( x, offset, nr[i], s );
        offset += nr[i]->h + 10;
    }
    TTF_CloseFont( font_normal );

    SDL_Event event;
    bool done = false;
    while (!done)
    {
        SDL_DrawSurfaceAsGLTexture( s, portrait_vertexCoords );
        while ( SDL_PollEvent( &event ) )
        {
            if ( event.type == SDL_MOUSEBUTTONUP )
            {
                //User clicked, we're done.
                done = true;
                break;
            }

        }
    }

    //Free the surfaces we created...
    for ( int i = 0; i < numlines; ++i )
    {
        SDL_FreeSurface( nr[i] );
    }
}

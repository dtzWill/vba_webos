// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "AutoBuild.h"

#include "SDL.h"
#include "GBA.h"
#include "agbprint.h"
#include "Flash.h"
#include "Port.h"
#include "debugger.h"
#include "RTC.h"
#include "Sound.h"
#include "Text.h"
#include "unzip.h"
#include "Util.h"
#include "gb/GB.h"
#include "gb/gbGlobals.h"
#include "controller.h"

#include <SDL_opengles.h>
#include <SDL_video.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <assert.h>
#include <dirent.h>
#include "esFunc.h"

#define VERSION "1.1.0"

#define VBA_HOME "/media/internal/vba"
#define ROM_PATH VBA_HOME "/roms/"
#define FONT "/usr/share/fonts/PreludeCondensed-Medium.ttf"
#define TITLE "VisualBoyAdvance for WebOS (" VERSION ")"
#define AUTHOR_TAG "brought to you by Will Dietz (dtzWill) webos@wdtz.org"
#define NO_ROMS1 "Welcome to VBA!  Looks like you don't have any ROMs yet."
#define NO_ROMS2 "To play games, put the roms in "
#define NO_ROMS3 "/vba/roms"
#define NO_ROMS4 "using USB mode, and then launch VBA again"
#define NO_ROMS5 "For more information, see the wiki"
#define NO_ROMS6 "http://www.webos-internals.org/wiki/Application:VBA"

#define CONTROLLER_IMG VBA_HOME "/controller.png"
#define OPTIONS_CFG VBA_HOME "/options.cfg"

#define SCROLL_FACTOR 20
#define AUTOSAVE_STATE 100

#define DEBUG_CONTROLLER

//#define DEBUG_GL

#ifdef DEBUG_GL
void checkError()
{
    /* Check for error conditions. */
    GLenum gl_error = glGetError( );

    if( gl_error != GL_NO_ERROR ) {
        fprintf( stderr, "VBA: OpenGL error: %x\n", gl_error );
        while(1);
        exit( 1 );
    }

    char * sdl_error = SDL_GetError( );

    if( sdl_error[0] != '\0' ) {
        fprintf(stderr, "VBA: SDL error '%s'\n", sdl_error);
        while(1);
        exit( 2 );
    }
}
#else
#define checkError()
#endif

void GL_Init();
void GL_InitTexture();
void updateOrientation();

char * romSelector();

#ifndef WIN32
# include <unistd.h>
# define GETCWD getcwd
#else // WIN32
# include <direct.h>
# define GETCWD _getcwd
#endif // WIN32

#ifndef __GNUC__
# define HAVE_DECL_GETOPT 0
# define __STDC__ 1
# include "getopt.h"
#else // ! __GNUC__
# define HAVE_DECL_GETOPT 1
# include "getopt.h"
#endif // ! __GNUC__

#ifdef MMX
extern "C" bool cpu_mmx;
#endif
extern bool soundEcho;
extern bool soundLowPass;
extern bool soundReverse;

extern void remoteInit();
extern void remoteCleanUp();
extern void remoteStubMain();
extern void remoteStubSignal(int,int);
extern void remoteOutput(char *, u32);
extern void remoteSetProtocol(int);
extern void remoteSetPort(int);
extern void debuggerOutput(char *, u32);

extern void CPUUpdateRenderBuffers(bool);

struct EmulatedSystem emulator = {
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  false,
  0
};

SDL_Surface *surface = NULL;
SDL_Overlay *overlay = NULL;
SDL_Rect overlay_rect;


/*-----------------------------------------------------------------------------
 *  GL variables
 *-----------------------------------------------------------------------------*/
GLuint texture = 0;
GLuint controller_tex = 0;

// Handle to a program object
GLuint programObject;

// Attribute locations
GLint  positionLoc;
GLint  texCoordLoc;

// Sampler location
GLint samplerLoc;


/*-----------------------------------------------------------------------------
 *  State variables (outside the GB emulation core)
 *-----------------------------------------------------------------------------*/
enum orientation
{
    ORIENTATION_PORTRAIT,    // default mode, portrait
    ORIENTATION_LANDSCAPE_R, // landscape, keyboard on right
    ORIENTATION_LANDSCAPE_L  // landscape, keyboard on left
};

int orientation = ORIENTATION_LANDSCAPE_R;

int gl_filter = GL_LINEAR;

int combo_down = false;

int use_on_screen = true;

int autosave = false;

/*-----------------------------------------------------------------------------
 *  Vertex coordinates for various orientations.
 *-----------------------------------------------------------------------------*/

//Current coords;
float vertexCoords[8];

//Landscape, keyboard on left.
float land_l_vertexCoords[] =
{
    -1, -1,
    1, -1,
    -1, 1,
    1, 1
};

//Landscape, keyboard on right.
float land_r_vertexCoords[] =
{
    1, 1,
    -1, 1,
    1, -1,
    -1, -1
};
//Portrait
float portrait_vertexCoords[] =
{
    -1, 1,
    -1, -1,
    1, 1,
    1, -1
};

float * controller_coords = land_r_vertexCoords;

float texCoords[] =
{
    0.0, 0.0,
    0.0, 1.0,
    1.0, 0.0,
    1.0, 1.0
};

GLushort indices[] = { 0, 1, 2, 1, 2, 3 };

int systemSpeed = 0;
int systemRedShift = 0;
int systemBlueShift = 0;
int systemGreenShift = 0;
int systemColorDepth = 0;
int systemDebug = 0;
int systemVerbose = 0;
int systemFrameSkip = 0;
int systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

int srcPitch = 0;
int srcWidth = 0;
int srcHeight = 0;
int destWidth = 0;
int destHeight = 0;

int sensorX = 2047;
int sensorY = 2047;

int filter = 0;
u8 *delta = NULL;

int sdlPrintUsage = 0;
int disableMMX = 0;

int cartridgeType = 3;
int sizeOption = 0;
int captureFormat = 0;

int pauseWhenInactive = 1;
int active = 1;
int emulating = 0;
int RGB_LOW_BITS_MASK=0x821;
u32 systemColorMap32[0x10000];
u16 systemColorMap16[0x10000];
u16 systemGbPalette[24];
void (*filterFunction)(u8*,u32,u8*,u8*,u32,int,int) = NULL;
void (*ifbFunction)(u8*,u32,int,int) = NULL;
int ifbType = 0;
char filename[2048];
char ipsname[2048];
char biosFileName[2048];
char captureDir[2048];
char saveDir[2048];
char batteryDir[2048];

static char *rewindMemory = NULL;
static int rewindPos = 0;
static int rewindTopPos = 0;
static int rewindCounter = 0;
static int rewindCount = 0;
static bool rewindSaveNeeded = false;
static int rewindTimer = 0;

//User-friendly names while walking them through the config process.
char * bindingNames[]= 
{
    "Press key for Left",
    "Press key for Right",
    "Press key for Up",
    "Press key for Down",
    "Press key for A",
    "Press key for B",
    "Press key for Start",
    "Press key for Select",
    "Press key for L",
    "Press key for R",
    "Done binding keys"
};
//Config-file names
char * bindingCfgNames [] = 
{
    "Joy0_Left",
    "Joy0_Right",
    "Joy0_Up",
    "Joy0_Down",
    "Joy0_A",
    "Joy0_B",
    "Joy0_Start",
    "Joy0_Select",
    "Joy0_L",
    "Joy0_R"
};
#define NOT_BINDING -1
#define BINDING_DONE ( KEY_BUTTON_R + 1 )
static int keyBindingMode = NOT_BINDING;
u16 bindingJoypad[12];


#define REWIND_SIZE 400000

#define _stricmp strcasecmp

bool sdlButtons[4][12] = {
  { false, false, false, false, false, false, 
    false, false, false, false, false, false },
  { false, false, false, false, false, false,
    false, false, false, false, false, false },
  { false, false, false, false, false, false,
    false, false, false, false, false, false },
  { false, false, false, false, false, false,
    false, false, false, false, false, false }
};

bool sdlMotionButtons[4] = { false, false, false, false };

int sdlNumDevices = 0;
SDL_Joystick **sdlDevices = NULL;

bool wasPaused = false;
int autoFrameSkip = 0;
int frameskipadjust = 0;
int showRenderedFrames = 0;
int renderedFrames = 0;

int throttle = 0;
u32 throttleLastTime = 0;
u32 autoFrameSkipLastTime = 0;

int showSpeed = 0;
int showSpeedTransparent = 1;
bool disableStatusMessages = false;
bool paused = false;
bool pauseNextFrame = false;
bool debugger = false;
bool debuggerStub = false;
int fullscreen = 0;
int soundMute = false;
bool systemSoundOn = false;
bool yuv = false;
int yuvType = 0;
bool removeIntros = false;
int sdlFlashSize = 0;
int sdlAutoIPS = 1;
int sdlRtcEnable = 0;
int sdlAgbPrint = 0;

typedef struct
{
    char * name;
    int * value;
} vba_option;

vba_option options[] =
{
    { "orientation", &orientation },
    { "sound", &soundMute },
    { "filter", &gl_filter },
    { "speed", &showSpeed },
    { "onscreen", &use_on_screen },
    { "autosave", &autosave }
};

int sdlDefaultJoypad = 0;

extern void debuggerSignal(int,int);

void (*dbgMain)() = debuggerMain;
void (*dbgSignal)(int,int) = debuggerSignal;
void (*dbgOutput)(char *, u32) = debuggerOutput;

int  mouseCounter = 0;
int autoFire = 0;
bool autoFireToggle = false;

bool screenMessage = false;
char screenMessageBuffer[21];
u32  screenMessageTime = 0;

// Patch #1382692 by deathpudding.
// (pulled from vba 1.8.0)
SDL_sem *sdlBufferLock  = NULL;
SDL_sem *sdlBufferFull  = NULL;
SDL_sem *sdlBufferEmpty = NULL;
u8 sdlBuffer[4096];
int sdlSoundLen = 0;

char *arg0;

#ifndef C_CORE
u8 sdlStretcher[16384];
int sdlStretcherPos;
#else
void (*sdlStretcher)(u8 *, u8*) = NULL;
#endif

enum {
  KEY_LEFT, KEY_RIGHT,
  KEY_UP, KEY_DOWN,
  KEY_BUTTON_A, KEY_BUTTON_B,
  KEY_BUTTON_START, KEY_BUTTON_SELECT,
  KEY_BUTTON_L, KEY_BUTTON_R,
  KEY_BUTTON_SPEED, KEY_BUTTON_CAPTURE
};

u16 joypad[4][12] = {
  { SDLK_a,  SDLK_d,
    SDLK_w,    SDLK_s,
    SDLK_l,     SDLK_k,
    SDLK_RETURN,SDLK_SPACE,
    SDLK_q,     SDLK_p,
    SDLK_AT, SDLK_PERIOD
  },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

u16 defaultJoypad[12] = {
  SDLK_LEFT,  SDLK_RIGHT,
  SDLK_UP,    SDLK_DOWN,
  SDLK_z,     SDLK_x,
  SDLK_RETURN,SDLK_BACKSPACE,
  SDLK_a,     SDLK_s,
  SDLK_SPACE, SDLK_F12
};

u16 motion[4] = {
  SDLK_KP4, SDLK_KP6, SDLK_KP8, SDLK_KP2
};

u16 defaultMotion[4] = {
  SDLK_KP4, SDLK_KP6, SDLK_KP8, SDLK_KP2
};

struct option sdlOptions[] = {
  { "agb-print", no_argument, &sdlAgbPrint, 1 },
  { "auto-frameskip", no_argument, &autoFrameSkip, 1 },  
  { "bios", required_argument, 0, 'b' },
  { "config", required_argument, 0, 'c' },
  { "debug", no_argument, 0, 'd' },
  { "filter", required_argument, 0, 'f' },
  { "filter-normal", no_argument, &filter, 0 },
  { "filter-tv-mode", no_argument, &filter, 1 },
  { "filter-2xsai", no_argument, &filter, 2 },
  { "filter-super-2xsai", no_argument, &filter, 3 },
  { "filter-super-eagle", no_argument, &filter, 4 },
  { "filter-pixelate", no_argument, &filter, 5 },
  { "filter-motion-blur", no_argument, &filter, 6 },
  { "filter-advmame", no_argument, &filter, 7 },
  { "filter-simple2x", no_argument, &filter, 8 },
  { "filter-bilinear", no_argument, &filter, 9 },
  { "filter-bilinear+", no_argument, &filter, 10 },
  { "filter-scanlines", no_argument, &filter, 11 },
  { "filter-hq2x", no_argument, &filter, 12 },
  { "filter-lq2x", no_argument, &filter, 13 },
  { "flash-size", required_argument, 0, 'S' },
  { "flash-64k", no_argument, &sdlFlashSize, 0 },
  { "flash-128k", no_argument, &sdlFlashSize, 1 },
  { "frameskip", required_argument, 0, 's' },
  { "fullscreen", no_argument, &fullscreen, 1 },
  { "gdb", required_argument, 0, 'G' },
  { "help", no_argument, &sdlPrintUsage, 1 },
  { "ifb-none", no_argument, &ifbType, 0 },
  { "ifb-motion-blur", no_argument, &ifbType, 1 },
  { "ifb-smart", no_argument, &ifbType, 2 },
  { "ips", required_argument, 0, 'i' },
  { "no-agb-print", no_argument, &sdlAgbPrint, 0 },
  { "no-auto-frameskip", no_argument, &autoFrameSkip, 0 },
  { "no-debug", no_argument, 0, 'N' },
  { "no-ips", no_argument, &sdlAutoIPS, 0 },
  { "no-mmx", no_argument, &disableMMX, 1 },
  { "no-pause-when-inactive", no_argument, &pauseWhenInactive, 0 },
  { "no-rtc", no_argument, &sdlRtcEnable, 0 },
  { "no-show-speed", no_argument, &showSpeed, 0 },
  { "no-throttle", no_argument, &throttle, 0 },
  { "pause-when-inactive", no_argument, &pauseWhenInactive, 1 },
  { "profile", optional_argument, 0, 'p' },
  { "rtc", no_argument, &sdlRtcEnable, 1 },
  { "save-type", required_argument, 0, 't' },
  { "save-auto", no_argument, &cpuSaveType, 0 },
  { "save-eeprom", no_argument, &cpuSaveType, 1 },
  { "save-sram", no_argument, &cpuSaveType, 2 },
  { "save-flash", no_argument, &cpuSaveType, 3 },
  { "save-sensor", no_argument, &cpuSaveType, 4 },
  { "save-none", no_argument, &cpuSaveType, 5 },
  { "show-speed-normal", no_argument, &showSpeed, 1 },
  { "show-speed-detailed", no_argument, &showSpeed, 2 },
  { "throttle", required_argument, 0, 'T' },
  { "verbose", required_argument, 0, 'v' },  
  { "video-1x", no_argument, &sizeOption, 0 },
  { "video-2x", no_argument, &sizeOption, 1 },
  { "video-3x", no_argument, &sizeOption, 2 },
  { "video-4x", no_argument, &sizeOption, 3 },
  { "yuv", required_argument, 0, 'Y' },
  { NULL, no_argument, NULL, 0 }
};

extern bool CPUIsGBAImage(char *);
extern bool gbIsGameboyRom(char *);

typedef struct
{
    int button1;
    int button2;
    char valid;
} controllerEvent;

/* ===========================================================================
 * controllerHitCheck
 *   
 *  Description:  Determines which on-screen controls were hit for the given x,y
 * =========================================================================*/
controllerEvent controllerHitCheck( int x, int y )
{
    controllerEvent event;
    event.valid = false;
    event.button1 = -1;
    event.button2 = -1;

    if ( HIT_A( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_BUTTON_A;
    }
    else if ( HIT_B( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_BUTTON_B;
    }
    else if ( HIT_AB( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_BUTTON_A;
        event.button2 = KEY_BUTTON_B;
    }
    else if ( HIT_L( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_BUTTON_L;
    }
    else if ( HIT_R( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_BUTTON_R;
    }
    else if ( HIT_START( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_BUTTON_START;
    }
    else if ( HIT_SELECT( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_BUTTON_SELECT;
    }
    //We assign up/down to button '1', and
    //left/right to button '2'.
    //(You can't hit u/d or l/r at same time
    if ( HIT_UP( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_UP;
    }
    if ( HIT_DOWN( x, y ) )
    {
        event.valid = true;
        event.button1 = KEY_DOWN;
    }
    if ( HIT_LEFT( x, y ) )
    {
        event.valid = true;
        event.button2 = KEY_LEFT;
    }
    if ( HIT_RIGHT( x, y ) )
    {
        event.valid = true;
        event.button2 = KEY_RIGHT;
    }

    return event;
}

void applyControllerEvent( controllerEvent ev, char state )
{
    if ( ev.valid )
    {
        if ( ev.button1 != -1 )
        {
            sdlButtons[0][ev.button1] = state;
        }
        if ( ev.button2 != -1 )
        {
            sdlButtons[0][ev.button2] = state;
        }
    }
}

u32 sdlFromHex(char *s)
{
  u32 value;
  sscanf(s, "%x", &value);
  return value;
}

#ifdef __MSC__
#define stat _stat
#define S_IFDIR _S_IFDIR
#endif

void sdlCheckDirectory(char *dir)
{
  struct stat buf;

  int len = strlen(dir);

  char *p = dir + len - 1;

  if(*p == '/' ||
     *p == '\\')
    *p = 0;
  
  if(stat(dir, &buf) == 0) {
    if(!(buf.st_mode & S_IFDIR)) {
      fprintf(stderr, "Error: %s is not a directory\n", dir);
      dir[0] = 0;
    }
  } else {
    fprintf(stderr, "Error: %s does not exist\n", dir);
    dir[0] = 0;
  }
}

char *sdlGetFilename(char *name)
{
  static char filebuffer[2048];

  int len = strlen(name);
  
  char *p = name + len - 1;
  
  while(true) {
    if(*p == '/' ||
       *p == '\\') {
      p++;
      break;
    }
    len--;
    p--;
    if(len == 0)
      break;
  }
  
  if(len == 0)
    strcpy(filebuffer, name);
  else
    strcpy(filebuffer, p);
  return filebuffer;
}

FILE *sdlFindFile(const char *name)
{
  char buffer[4096];
  char path[2048];

#ifdef WIN32
#define PATH_SEP ";"
#define FILE_SEP '\\'
#define EXE_NAME "VisualBoyAdvance-SDL.exe"
#else // ! WIN32
#define PATH_SEP ":"
#define FILE_SEP '/'
#define EXE_NAME "VisualBoyAdvance"
#endif // ! WIN32

  fprintf(stderr, "Searching for file %s\n", name);
  
  if(GETCWD(buffer, 2048)) {
    fprintf(stderr, "Searching current directory: %s\n", buffer);
  }
  
  FILE *f = fopen(name, "r");
  if(f != NULL) {
    return f;
  }

  char *home = getenv("HOME");

  if(home != NULL) {
    fprintf(stderr, "Searching home directory: %s\n", home);
    sprintf(path, "%s%c%s", home, FILE_SEP, name);
    f = fopen(path, "r");
    if(f != NULL)
      return f;
  }

#ifdef WIN32
  home = getenv("USERPROFILE");
  if(home != NULL) {
    fprintf(stderr, "Searching user profile directory: %s\n", home);
    sprintf(path, "%s%c%s", home, FILE_SEP, name);
    f = fopen(path, "r");
    if(f != NULL)
      return f;
  }
#else // ! WIN32
    fprintf(stderr, "Searching system config directory: %s\n", SYSCONFDIR);
    sprintf(path, "%s%c%s", SYSCONFDIR, FILE_SEP, name);
    f = fopen(path, "r");
    if(f != NULL)
      return f;
#endif // ! WIN32
    fprintf( stderr, "Searching %s", VBA_HOME );
    sprintf(path, "%s%c%s", VBA_HOME, FILE_SEP, name);
    f = fopen(path, "r");
    if( f != NULL)
        return f;

  if(!strchr(arg0, '/') &&
     !strchr(arg0, '\\')) {
    char *path = getenv("PATH");

    if(path != NULL) {
      fprintf(stderr, "Searching PATH\n");
      strncpy(buffer, path, 4096);
      buffer[4095] = 0;
      char *tok = strtok(buffer, PATH_SEP);
      
      while(tok) {
        sprintf(path, "%s%c%s", tok, FILE_SEP, EXE_NAME);
        f = fopen(path, "r");
        if(f != NULL) {
          char path2[2048];
          fclose(f);
          sprintf(path2, "%s%c%s", tok, FILE_SEP, name);
          f = fopen(path2, "r");
          if(f != NULL) {
            fprintf(stderr, "Found at %s\n", path2);
            return f;
          }
        }
        tok = strtok(NULL, PATH_SEP);
      }
    }
  } else {
    // executable is relative to some directory
    fprintf(stderr, "Searching executable directory\n");
    strcpy(buffer, arg0);
    char *p = strrchr(buffer, FILE_SEP);
    if(p) {
      *p = 0;
      sprintf(path, "%s%c%s", buffer, FILE_SEP, name);
      f = fopen(path, "r");
      if(f != NULL)
        return f;
    }
  }
  return NULL;
}

void writeOptions()
{
   FILE * f = fopen( OPTIONS_CFG, "w" );
   if ( !f )
   {
       perror( "Failed to read options!" );
       return;
   }

   int count = sizeof( options ) / sizeof( vba_option );
   for ( int i = 0; i < count; i++ )
   {
       fprintf( f, "%s=%x\n", options[i].name, *options[i].value );
   }

   fclose( f );
}

void readOptions()
{
   FILE * f = fopen( OPTIONS_CFG, "r" );
   if ( !f )
   {
       perror( "Failed to read options!" );
       return;
   }

   char buffer[2048];

   while ( 1 )
   {
       char * s = fgets( buffer, 2048, f );

       //More to the file?
       if( s == NULL )
       {
           break;
       }

       //Ignore parts from '#' onwards
       char * p  = strchr(s, '#');

       if ( p )
       {
           *p = '\0';
       }

       char * token = strtok( s, " \t\n\r=" );

       if ( !token || strlen( token ) == 0 )
       {
           continue;
       }

       char * key = token;
       char * value = strtok( NULL, "\t\n\r" );

       if( value == NULL )
       {
           fprintf( stderr, "Empty value for key %s\n", key );
           continue;
       }

       //Okay now we have key/value pair.
       //match it to one of the ones we know about...
       int count = sizeof( options ) / sizeof( vba_option );
       char known = false;
       for ( int i = 0; i < count; i++ )
       {
           if ( !strcasecmp( key, options[i].name ) )
           {
                *options[i].value = sdlFromHex( value );
                known = true;
                break;
           }
       }
       if ( !known )
       {
           fprintf( stderr, "Unrecognized option %s\n", key );
       }
   }
}

void sdlReadPreferences(FILE *f)
{
  char buffer[2048];
  
  while(1) {
    char *s = fgets(buffer, 2048, f);

    if(s == NULL)
      break;

    char *p  = strchr(s, '#');
    
    if(p)
      *p = 0;
    
    char *token = strtok(s, " \t\n\r=");

    if(!token)
      continue;

    if(strlen(token) == 0)
      continue;

    char *key = token;
    char *value = strtok(NULL, "\t\n\r");

    if(value == NULL) {
      fprintf(stderr, "Empty value for key %s\n", key);
      continue;
    }

    if(!strcmp(key,"Joy0_Left")) {
      joypad[0][KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Right")) {
      joypad[0][KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Up")) {
      joypad[0][KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Down")) {
      joypad[0][KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_A")) {
      joypad[0][KEY_BUTTON_A] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_B")) {
      joypad[0][KEY_BUTTON_B] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_L")) {
      joypad[0][KEY_BUTTON_L] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_R")) {
      joypad[0][KEY_BUTTON_R] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Start")) {
      joypad[0][KEY_BUTTON_START] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Select")) {
      joypad[0][KEY_BUTTON_SELECT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Speed")) {
      joypad[0][KEY_BUTTON_SPEED] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy0_Capture")) {
      joypad[0][KEY_BUTTON_CAPTURE] = sdlFromHex(value);
    } else if(!strcmp(key,"Joy1_Left")) {
      joypad[1][KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Right")) {
      joypad[1][KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Up")) {
      joypad[1][KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Down")) {
      joypad[1][KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_A")) {
      joypad[1][KEY_BUTTON_A] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_B")) {
      joypad[1][KEY_BUTTON_B] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_L")) {
      joypad[1][KEY_BUTTON_L] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_R")) {
      joypad[1][KEY_BUTTON_R] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Start")) {
      joypad[1][KEY_BUTTON_START] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Select")) {
      joypad[1][KEY_BUTTON_SELECT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Speed")) {
      joypad[1][KEY_BUTTON_SPEED] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy1_Capture")) {
      joypad[1][KEY_BUTTON_CAPTURE] = sdlFromHex(value);
    } else if(!strcmp(key,"Joy2_Left")) {
      joypad[2][KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Right")) {
      joypad[2][KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Up")) {
      joypad[2][KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Down")) {
      joypad[2][KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_A")) {
      joypad[2][KEY_BUTTON_A] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_B")) {
      joypad[2][KEY_BUTTON_B] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_L")) {
      joypad[2][KEY_BUTTON_L] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_R")) {
      joypad[2][KEY_BUTTON_R] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Start")) {
      joypad[2][KEY_BUTTON_START] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Select")) {
      joypad[2][KEY_BUTTON_SELECT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Speed")) {
      joypad[2][KEY_BUTTON_SPEED] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy2_Capture")) {
      joypad[2][KEY_BUTTON_CAPTURE] = sdlFromHex(value);
    } else if(!strcmp(key,"Joy4_Left")) {
      joypad[4][KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Right")) {
      joypad[4][KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Up")) {
      joypad[4][KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Down")) {
      joypad[4][KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_A")) {
      joypad[4][KEY_BUTTON_A] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_B")) {
      joypad[4][KEY_BUTTON_B] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_L")) {
      joypad[4][KEY_BUTTON_L] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_R")) {
      joypad[4][KEY_BUTTON_R] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Start")) {
      joypad[4][KEY_BUTTON_START] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Select")) {
      joypad[4][KEY_BUTTON_SELECT] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Speed")) {
      joypad[4][KEY_BUTTON_SPEED] = sdlFromHex(value);
    } else if(!strcmp(key, "Joy4_Capture")) {
      joypad[4][KEY_BUTTON_CAPTURE] = sdlFromHex(value);
    } else if(!strcmp(key, "Motion_Left")) {
      motion[KEY_LEFT] = sdlFromHex(value);
    } else if(!strcmp(key, "Motion_Right")) {
      motion[KEY_RIGHT] = sdlFromHex(value);
    } else if(!strcmp(key, "Motion_Up")) {
      motion[KEY_UP] = sdlFromHex(value);
    } else if(!strcmp(key, "Motion_Down")) {
      motion[KEY_DOWN] = sdlFromHex(value);
    } else if(!strcmp(key, "frameSkip")) {
      frameSkip = sdlFromHex(value);
      if(frameSkip < 0 || frameSkip > 9)
        frameSkip = 2;
    } else if(!strcmp(key, "gbFrameSkip")) {
      gbFrameSkip = sdlFromHex(value);
      if(gbFrameSkip < 0 || gbFrameSkip > 9)
        gbFrameSkip = 0;      
    } else if(!strcmp(key, "video")) {
      sizeOption = sdlFromHex(value);
      if(sizeOption < 0 || sizeOption > 3)
        sizeOption = 1;
    } else if(!strcmp(key, "fullScreen")) {
      fullscreen = sdlFromHex(value) ? 1 : 0;
    } else if(!strcmp(key, "useBios")) {
      useBios = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "skipBios")) {
      skipBios = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "biosFile")) {
      strcpy(biosFileName, value);
    } else if(!strcmp(key, "filter")) {
      filter = sdlFromHex(value);
      if(filter < 0 || filter > 13)
        filter = 0;
    } else if(!strcmp(key, "disableStatus")) {
      disableStatusMessages = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "borderOn")) {
      gbBorderOn = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "borderAutomatic")) {
      gbBorderAutomatic = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "emulatorType")) {
      gbEmulatorType = sdlFromHex(value);
      if(gbEmulatorType < 0 || gbEmulatorType > 5)
        gbEmulatorType = 1;
    } else if(!strcmp(key, "colorOption")) {
      gbColorOption = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "captureDir")) {
      sdlCheckDirectory(value);
      strcpy(captureDir, value);
    } else if(!strcmp(key, "saveDir")) {
      sdlCheckDirectory(value);
      strcpy(saveDir, value);
    } else if(!strcmp(key, "batteryDir")) {
      sdlCheckDirectory(value);
      strcpy(batteryDir, value);
    } else if(!strcmp(key, "captureFormat")) {
      captureFormat = sdlFromHex(value);
    } else if(!strcmp(key, "soundQuality")) {
      soundQuality = sdlFromHex(value);
      switch(soundQuality) {
      case 1:
      case 2:
      case 4:
        break;
      default:
        fprintf(stderr, "Unknown sound quality %d. Defaulting to 22Khz\n", 
                soundQuality);
        soundQuality = 2;
        break;
      }
    } else if(!strcmp(key, "soundOff")) {
      soundOffFlag = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "soundEnable")) {
      int res = sdlFromHex(value) & 0x30f;
      soundEnable(res);
      soundDisable(~res);
    } else if(!strcmp(key, "soundEcho")) {
      soundEcho = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "soundLowPass")) {
      soundLowPass = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "soundReverse")) {
      soundReverse = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "soundVolume")) {
      soundVolume = sdlFromHex(value);
      if(soundVolume < 0 || soundVolume > 3)
        soundVolume = 0;
    } else if(!strcmp(key, "removeIntros")) {
      removeIntros = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "saveType")) {
      cpuSaveType = sdlFromHex(value);
      if(cpuSaveType < 0 || cpuSaveType > 5)
        cpuSaveType = 0;
    } else if(!strcmp(key, "flashSize")) {
      sdlFlashSize = sdlFromHex(value);
      if(sdlFlashSize != 0 && sdlFlashSize != 1)
        sdlFlashSize = 0;
    } else if(!strcmp(key, "ifbType")) {
      ifbType = sdlFromHex(value);
      if(ifbType < 0 || ifbType > 2)
        ifbType = 0;
    } else if(!strcmp(key, "showSpeed")) {
      showSpeed = sdlFromHex(value);
      if(showSpeed < 0 || showSpeed > 2)
        showSpeed = 1;
    } else if(!strcmp(key, "showSpeedTransparent")) {
      showSpeedTransparent = sdlFromHex(value);
    } else if(!strcmp(key, "autoFrameSkip")) {
      autoFrameSkip = sdlFromHex(value);
    } else if(!strcmp(key, "throttle")) {
      throttle = sdlFromHex(value);
      if(throttle != 0 && (throttle < 5 || throttle > 1000))
        throttle = 0;
    } else if(!strcmp(key, "disableMMX")) {
#ifdef MMX
      cpu_mmx = sdlFromHex(value) ? false : true;
#endif
    } else if(!strcmp(key, "pauseWhenInactive")) {
      pauseWhenInactive = sdlFromHex(value) ? true : false;
    } else if(!strcmp(key, "agbPrint")) {
      sdlAgbPrint = sdlFromHex(value);
    } else if(!strcmp(key, "rtcEnabled")) {
      sdlRtcEnable = sdlFromHex(value);
    } else if(!strcmp(key, "rewindTimer")) {
      rewindTimer = sdlFromHex(value);
      if(rewindTimer < 0 || rewindTimer > 600)
        rewindTimer = 0;
      rewindTimer *= 6;  // convert value to 10 frames multiple
    } else if(!strcmp(key, "enhancedDetection")) {
      cpuEnhancedDetection = sdlFromHex(value) ? true : false;
    } else {
      fprintf(stderr, "Unknown configuration key %s\n", key);
    }
  }
}

void sdlReadPreferences()
{
  FILE *f = sdlFindFile("VisualBoyAdvance.cfg");

  if(f == NULL) {
    fprintf(stderr, "Configuration file NOT FOUND (using defaults)\n");
    return;
  } else
    fprintf(stderr, "Reading configuration file.\n");

  sdlReadPreferences(f);

  fclose(f);
}

static void sdlApplyPerImagePreferences()
{
  FILE *f = sdlFindFile("vba-over.ini");
  if(!f) {
    fprintf(stderr, "vba-over.ini NOT FOUND (using emulator settings)\n");
    return;
  } else
    fprintf(stderr, "Reading vba-over.ini\n");

  char buffer[7];
  buffer[0] = '[';
  buffer[1] = rom[0xac];
  buffer[2] = rom[0xad];
  buffer[3] = rom[0xae];
  buffer[4] = rom[0xaf];
  buffer[5] = ']';
  buffer[6] = 0;

  char readBuffer[2048];

  bool found = false;
  
  while(1) {
    char *s = fgets(readBuffer, 2048, f);

    if(s == NULL)
      break;

    char *p  = strchr(s, ';');
    
    if(p)
      *p = 0;
    
    char *token = strtok(s, " \t\n\r=");

    if(!token)
      continue;
    if(strlen(token) == 0)
      continue;

    if(!strcmp(token, buffer)) {
      found = true;
      break;
    }
  }

  if(found) {
    while(1) {
      char *s = fgets(readBuffer, 2048, f);

      if(s == NULL)
        break;

      char *p = strchr(s, ';');
      if(p)
        *p = 0;

      char *token = strtok(s, " \t\n\r=");
      if(!token)
        continue;
      if(strlen(token) == 0)
        continue;

      if(token[0] == '[') // starting another image settings
        break;
      char *value = strtok(NULL, "\t\n\r=");
      if(value == NULL)
        continue;
      
      if(!strcmp(token, "rtcEnabled"))
        rtcEnable(atoi(value) == 0 ? false : true);
      else if(!strcmp(token, "flashSize")) {
        int size = atoi(value);
        if(size == 0x10000 || size == 0x20000)
          flashSetSize(size);
      } else if(!strcmp(token, "saveType")) {
        int save = atoi(value);
        if(save >= 0 && save <= 5)
          cpuSaveType = save;
      }
    }
  }
  fclose(f);
}

static int sdlCalculateShift(u32 mask)
{
  int m = 0;
  
  while(mask) {
    m++;
    mask >>= 1;
  }

  return m-5;
}

static int sdlCalculateMaskWidth(u32 mask)
{
  int m = 0;
  int mask2 = mask;

  while(mask2) {
    m++;
    mask2 >>= 1;
  }

  int m2 = 0;
  mask2 = mask;
  while(!(mask2 & 1)) {
    m2++;
    mask2 >>= 1;
  }

  return m - m2;
}

void sdlWriteState(int num)
{
  char stateName[2048];

  if(saveDir[0])
    sprintf(stateName, "%s/%s%d.sgm", saveDir, sdlGetFilename(filename),
            num+1);
  else
    sprintf(stateName,"%s%d.sgm", filename, num+1);
  if(emulator.emuWriteState)
    emulator.emuWriteState(stateName);

  if ( autosave && num == AUTOSAVE_STATE )
  {
      //Nothing
  }
  else
  {
      sprintf(stateName, "Wrote state %d", num+1);
      systemScreenMessage(stateName);
  }
}

void sdlReadState(int num)
{
  char stateName[2048];

  if(saveDir[0])
    sprintf(stateName, "%s/%s%d.sgm", saveDir, sdlGetFilename(filename),
            num+1);
  else
    sprintf(stateName,"%s%d.sgm", filename, num+1);

  if(emulator.emuReadState)
    emulator.emuReadState(stateName);

  if ( autosave && num == AUTOSAVE_STATE )
  {
      systemScreenMessage( "Resuming auto state save...\n" );
  }
  else
  {
      sprintf(stateName, "Loaded state %d", num+1);
      systemScreenMessage(stateName);
  }
}

void sdlWriteBattery()
{
  char buffer[1048];

  if(batteryDir[0])
    sprintf(buffer, "%s/%s.sav", batteryDir, sdlGetFilename(filename));
  else  
    sprintf(buffer, "%s.sav", filename);

  emulator.emuWriteBattery(buffer);

  //No one wants to see this; they assume it saves.
  //systemScreenMessage("Wrote battery");

  //Write the current options.
  writeOptions();

  //Write the autosave!
  if ( autosave )
  {
      sdlWriteState( AUTOSAVE_STATE );
  }
}

void sdlReadBattery()
{
  char buffer[1048];
  
  if(batteryDir[0])
    sprintf(buffer, "%s/%s.sav", batteryDir, sdlGetFilename(filename));
  else 
    sprintf(buffer, "%s.sav", filename);
  
  bool res = false;

  res = emulator.emuReadBattery(buffer);

  //Less annoying than 'wrote battery' since only appears once,
  //but still not the best.
  //if(res)
  //  systemScreenMessage("Loaded battery");
}

#define MOD_KEYS    (KMOD_CTRL|KMOD_SHIFT|KMOD_ALT|KMOD_META)
#define MOD_NOCTRL  (KMOD_SHIFT|KMOD_ALT|KMOD_META)
#define MOD_NOALT   (KMOD_CTRL|KMOD_SHIFT|KMOD_META)
#define MOD_NOSHIFT (KMOD_CTRL|KMOD_ALT|KMOD_META)

void sdlUpdateKey(int key, bool down)
{
  int i;
  for(int j = 0; j < 4; j++) {
    for(i = 0 ; i < 12; i++) {
      if((joypad[j][i] & 0xf000) == 0) {
        if(key == joypad[j][i])
          sdlButtons[j][i] = down;
      }
    }
  }
  for(i = 0 ; i < 4; i++) {
    if((motion[i] & 0xf000) == 0) {
      if(key == motion[i])
        sdlMotionButtons[i] = down;
    }
  }

  if ( key == SDLK_t )
  {
      sdlButtons[0][KEY_BUTTON_A] = down;
      sdlButtons[0][KEY_BUTTON_B] = down;
      sdlButtons[0][KEY_BUTTON_START] = down;
      sdlButtons[0][KEY_BUTTON_SELECT] = down;
  }
}

void sdlUpdateJoyButton(int which,
                        int button,
                        bool pressed)
{
  int i;
  for(int j = 0; j < 4; j++) {
    for(i = 0; i < 12; i++) {
      int dev = (joypad[j][i] >> 12);
      int b = joypad[j][i] & 0xfff;
      if(dev) {
        dev--;
        
        if((dev == which) && (b >= 128) && (b == (button+128))) {
          sdlButtons[j][i] = pressed;
        }
      }
    }
  }
  for(i = 0; i < 4; i++) {
    int dev = (motion[i] >> 12);
    int b = motion[i] & 0xfff;
    if(dev) {
      dev--;

      if((dev == which) && (b >= 128) && (b == (button+128))) {
        sdlMotionButtons[i] = pressed;
      }
    }
  }  
}

void sdlUpdateJoyHat(int which,
                     int hat,
                     int value)
{
  int i;
  for(int j = 0; j < 4; j++) {
    for(i = 0; i < 12; i++) {
      int dev = (joypad[j][i] >> 12);
      int a = joypad[j][i] & 0xfff;
      if(dev) {
        dev--;
        
        if((dev == which) && (a>=32) && (a < 48) && (((a&15)>>2) == hat)) {
          int dir = a & 3;
          int v = 0;
          switch(dir) {
          case 0:
            v = value & SDL_HAT_UP;
            break;
          case 1:
            v = value & SDL_HAT_DOWN;
            break;
          case 2:
            v = value & SDL_HAT_RIGHT;
            break;
          case 3:
            v = value & SDL_HAT_LEFT;
            break;
          }
          sdlButtons[j][i] = (v ? true : false);
        }
      }
    }
  }
  for(i = 0; i < 4; i++) {
    int dev = (motion[i] >> 12);
    int a = motion[i] & 0xfff;
    if(dev) {
      dev--;

      if((dev == which) && (a>=32) && (a < 48) && (((a&15)>>2) == hat)) {
        int dir = a & 3;
        int v = 0;
        switch(dir) {
        case 0:
          v = value & SDL_HAT_UP;
          break;
        case 1:
          v = value & SDL_HAT_DOWN;
          break;
        case 2:
          v = value & SDL_HAT_RIGHT;
          break;
        case 3:
          v = value & SDL_HAT_LEFT;
          break;
        }
        sdlMotionButtons[i] = (v ? true : false);
      }
    }
  }      
}

void sdlUpdateJoyAxis(int which,
                      int axis,
                      int value)
{
  int i;
  for(int j = 0; j < 4; j++) {
    for(i = 0; i < 12; i++) {
      int dev = (joypad[j][i] >> 12);
      int a = joypad[j][i] & 0xfff;
      if(dev) {
        dev--;
        
        if((dev == which) && (a < 32) && ((a>>1) == axis)) {
          sdlButtons[j][i] = (a & 1) ? (value > 16384) : (value < -16384);
        }
      }
    }
  }
  for(i = 0; i < 4; i++) {
    int dev = (motion[i] >> 12);
    int a = motion[i] & 0xfff;
    if(dev) {
      dev--;

      if((dev == which) && (a < 32) && ((a>>1) == axis)) {
        sdlMotionButtons[i] = (a & 1) ? (value > 16384) : (value < -16384);
      }
    }
  }  
}

bool sdlCheckJoyKey(int key)
{
  int dev = (key >> 12) - 1;
  int what = key & 0xfff;

  if(what >= 128) {
    // joystick button
    int button = what - 128;

    if(button >= SDL_JoystickNumButtons(sdlDevices[dev]))
      return false;
  } else if (what < 0x20) {
    // joystick axis    
    what >>= 1;
    if(what >= SDL_JoystickNumAxes(sdlDevices[dev]))
      return false;
  } else if (what < 0x30) {
    // joystick hat
    what = (what & 15);
    what >>= 2;
    if(what >= SDL_JoystickNumHats(sdlDevices[dev]))
      return false;
  }

  // no problem found
  return true;
}

void sdlCheckKeys()
{
  sdlNumDevices = SDL_NumJoysticks();

  if(sdlNumDevices)
    sdlDevices = (SDL_Joystick **)calloc(1,sdlNumDevices *
                                         sizeof(SDL_Joystick **));
  int i;

  bool usesJoy = false;

  for(int j = 0; j < 4; j++) {
    for(i = 0; i < 12; i++) {
      int dev = joypad[j][i] >> 12;
      if(dev) {
        dev--;
        bool ok = false;
        
        if(sdlDevices) {
          if(dev < sdlNumDevices) {
            if(sdlDevices[dev] == NULL) {
              sdlDevices[dev] = SDL_JoystickOpen(dev);
            }
            
            ok = sdlCheckJoyKey(joypad[j][i]);
          } else
            ok = false;
        }
        
        if(!ok)
          joypad[j][i] = defaultJoypad[i];
        else
          usesJoy = true;
      }
    }
  }

  for(i = 0; i < 4; i++) {
    int dev = motion[i] >> 12;
    if(dev) {
      dev--;
      bool ok = false;
      
      if(sdlDevices) {
        if(dev < sdlNumDevices) {
          if(sdlDevices[dev] == NULL) {
            sdlDevices[dev] = SDL_JoystickOpen(dev);
          }
          
          ok = sdlCheckJoyKey(motion[i]);
        } else
          ok = false;
      }
      
      if(!ok)
        motion[i] = defaultMotion[i];
      else
        usesJoy = true;
    }
  }

  if(usesJoy)
    SDL_JoystickEventState(SDL_ENABLE);
}

void sdlPollEvents()
{
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_QUIT:
      if( emulating )
      {
          //FIRST thing we do when request to save--make sure we write the battery!
          sdlWriteBattery();
      }
      emulating = 0;
      break;
    case SDL_ACTIVEEVENT:
      if(pauseWhenInactive && (event.active.state & SDL_APPINPUTFOCUS)) {
        active = event.active.gain;
        if(active) {
          if(!paused) {
            if(emulating)
              soundResume();
          }
        } else {
          wasPaused = true;
          if(pauseWhenInactive) {
            if(emulating)
            {
              soundPause();
              //write battery when pausing.
              //Doesn't hurt, and guarantees we get a good save in.
              sdlWriteBattery();
            }
          }
          
          memset(delta,255,sizeof(delta));
        }
      }
      break;
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
    {
      if ( use_on_screen && orientation != ORIENTATION_LANDSCAPE_R )
      {
          return;
      }
      int x = event.button.x;
      int y = event.button.y;
      int state = event.button.state;

      controllerEvent ev = controllerHitCheck( x, y );
      applyControllerEvent( ev, state );

      break;
    }
    case SDL_MOUSEMOTION:
    {
      if ( use_on_screen && orientation != ORIENTATION_LANDSCAPE_R )
      {
          return;
      }
      int x = event.motion.x;
      int y = event.motion.y;
      int xrel = event.motion.xrel;
      int yrel = event.motion.yrel;

      //We make this work by considering a motion event
      //as releasing where we came FROM
      //and a down event where it is now.
      //XXX: Note that if from==now no harm, it'll end up down still
      controllerEvent ev = controllerHitCheck( x, y );
      controllerEvent old_ev = controllerHitCheck( x - xrel, y - yrel );

      //Where the mouse is now
      applyControllerEvent( old_ev, false );
      applyControllerEvent( ev, true );

      break;
    }
    case SDL_JOYHATMOTION:
      sdlUpdateJoyHat(event.jhat.which,
                      event.jhat.hat,
                      event.jhat.value);
      break;
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
      sdlUpdateJoyButton(event.jbutton.which,
                         event.jbutton.button,
                         event.jbutton.state == SDL_PRESSED);
      break;
    case SDL_JOYAXISMOTION:
      sdlUpdateJoyAxis(event.jaxis.which,
                       event.jaxis.axis,
                       event.jaxis.value);
      break;
    case SDL_KEYDOWN:
      if ( keyBindingMode == NOT_BINDING )
      {
          sdlUpdateKey(event.key.keysym.sym, true);
      }
      break;
    case SDL_KEYUP:
      if ( keyBindingMode != NOT_BINDING )
      {
          int key = event.key.keysym.sym;
          if ( key == SDLK_EQUALS )
          {
              //cancel;
              keyBindingMode = NOT_BINDING;
              systemScreenMessage( "Cancelled binding!" );
              break;
          }

          //Check that this is a valid key.
          //XXX: right now we don't support
          //orange, shift, or sym as keys b/c they are meta keys.
          int valid = 0
              || ( key >= SDLK_a && key <= SDLK_z ) //Alpha
              || key == SDLK_BACKSPACE
              || key == SDLK_RETURN
              || key == SDLK_COMMA
              || key == SDLK_PERIOD
              || key == SDLK_SPACE
              || key == SDLK_AT;

          //If so, bind it.
          if ( valid )
          {
              bindingJoypad[keyBindingMode] = key;
              keyBindingMode++;
          }

          //Display message for next key.
          systemScreenMessage( bindingNames[keyBindingMode] );

          if ( keyBindingMode == BINDING_DONE )
          {
              //write to file.
              //XXX: Write to alternate file? Don't overwrite this existing one?
              FILE * f  = fopen( VBA_HOME "/VisualBoyAdvance.cfg", "w" );

              for ( int i = 0; i < BINDING_DONE; i++ )
              {
                  fprintf( f, "%s=%04x\n", bindingCfgNames[i], bindingJoypad[i] );
              }

              fclose( f );

              //make this the current joy
              memcpy( &joypad[0][0], bindingJoypad, 10*sizeof( u16 ) );
              
              //we're done here!
              keyBindingMode = NOT_BINDING;

          }
                   
          break;
      }
      switch(event.key.keysym.sym) {
          //      XXX: bind these to something useful
//      case SDLK_r:
//        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//           (event.key.keysym.mod & KMOD_CTRL)) {
//          if(emulating) {
//            emulator.emuReset();
//
//            systemScreenMessage("Reset");
//          }
//        }
//        break;
//      case SDLK_b:
//        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//           (event.key.keysym.mod & KMOD_CTRL)) {
//          if(emulating && emulator.emuReadMemState && rewindMemory 
//             && rewindCount) {
//            rewindPos = --rewindPos & 7;
//            emulator.emuReadMemState(&rewindMemory[REWIND_SIZE*rewindPos], 
//                                     REWIND_SIZE);
//            rewindCount--;
//            rewindCounter = 0;
//            systemScreenMessage("Rewind");
//          }
//        }
//        break;
//      case SDLK_p:
//        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//           (event.key.keysym.mod & KMOD_CTRL)) {
//          paused = !paused;
//          SDL_PauseAudio(paused);
//          if(paused)
//            wasPaused = true;
//        }
//        break;
//      case SDLK_ESCAPE:
//        emulating = 0;
//        break;
//      case SDLK_1:
//      case SDLK_2:
//      case SDLK_3:
//      case SDLK_4:
//        if(!(event.key.keysym.mod & MOD_NOALT) &&
//           (event.key.keysym.mod & KMOD_ALT)) {
//          char *disableMessages[4] = 
//            { "autofire A disabled",
//              "autofire B disabled",
//              "autofire R disabled",
//              "autofire L disabled"};
//          char *enableMessages[4] = 
//            { "autofire A",
//              "autofire B",
//              "autofire R",
//              "autofire L"};
//          int mask = 1 << (event.key.keysym.sym - SDLK_1);
//    if(event.key.keysym.sym > SDLK_2)
//      mask <<= 6;
//          if(autoFire & mask) {
//            autoFire &= ~mask;
//            systemScreenMessage(disableMessages[event.key.keysym.sym - SDLK_1]);
//          } else {
//            autoFire |= mask;
//            systemScreenMessage(enableMessages[event.key.keysym.sym - SDLK_1]);
//          }
//        } if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//             (event.key.keysym.mod & KMOD_CTRL)) {
//          int mask = 0x0100 << (event.key.keysym.sym - SDLK_1);
//          layerSettings ^= mask;
//          layerEnable = DISPCNT & layerSettings;
//          CPUUpdateRenderBuffers(false);
//        }
//        break;
//      case SDLK_5:
//      case SDLK_6:
//      case SDLK_7:
//      case SDLK_8:
//        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//           (event.key.keysym.mod & KMOD_CTRL)) {
//          int mask = 0x0100 << (event.key.keysym.sym - SDLK_1);
//          layerSettings ^= mask;
//          layerEnable = DISPCNT & layerSettings;
//        }
//        break;
//      case SDLK_n:
//        if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//           (event.key.keysym.mod & KMOD_CTRL)) {
//          if(paused)
//            paused = false;
//          pauseNextFrame = true;
//        }
//        break;
      case SDLK_0:
        //Toggle orientation
        orientation = ( orientation + 1 ) % 3;
        updateOrientation();
        break;
      case SDLK_ASTERISK:
        //Toggle sound
        soundMute = !soundMute;
        break;
      case SDLK_PLUS:
        //toggle on-screen controls...
        use_on_screen = !use_on_screen;
        updateOrientation();
        break;
      case SDLK_QUOTE:
        //toggle filters...
        if ( gl_filter == GL_LINEAR )
        {
            gl_filter = GL_NEAREST;
        }
        else if ( gl_filter == GL_NEAREST )
        {
            gl_filter = GL_LINEAR;
        }

        GL_InitTexture();
        break;
      case SDLK_MINUS:
        //toggle show speed
        showSpeed = !showSpeed;
        break;
      case SDLK_1:
      case SDLK_2:
      case SDLK_3:
        {
            //save states 1-3
            int state = event.key.keysym.sym - SDLK_1;
            sdlWriteState( state );
            break;
        }
      case SDLK_4:
      case SDLK_5:
      case SDLK_6:
        {
            //load states 1-3
            int state = event.key.keysym.sym - SDLK_4;
            sdlReadState( state );
            break;
        }
      case SDLK_AMPERSAND:
        autosave = !autosave;
        if ( autosave )
        {
            systemScreenMessage( "Auto save enabled" );
        }
        else
        {
            systemScreenMessage( "Auto save disabled" );
        }
        break;
      case SDLK_EQUALS:
        //Enter key-binding mode.
        keyBindingMode = NOT_BINDING;
        keyBindingMode++;
      default:
        break;
      }
      sdlUpdateKey(event.key.keysym.sym, false);
      break;
    }
  }
}

void usage(char *cmd)
{
  printf("%s [option ...] file\n", cmd);
  printf("\
\n\
Options:\n\
  -1, --video-1x               1x\n\
  -2, --video-2x               2x\n\
  -3, --video-3x               3x\n\
  -4, --video-4x               4x\n\
  -F, --fullscreen             Full screen\n\
  -G, --gdb=PROTOCOL           GNU Remote Stub mode:\n\
                                tcp      - use TCP at port 55555\n\
                                tcp:PORT - use TCP at port PORT\n\
                                pipe     - use pipe transport\n\
  -N, --no-debug               Don't parse debug information\n\
  -S, --flash-size=SIZE        Set the Flash size\n\
      --flash-64k               0 -  64K Flash\n\
      --flash-128k              1 - 128K Flash\n\
  -T, --throttle=THROTTLE      Set the desired throttle (5...1000)\n\
  -Y, --yuv=TYPE               Use YUV overlay for drawing:\n\
                                0 - YV12\n\
                                1 - UYVY\n\
                                2 - YVYU\n\
                                3 - YUY2\n\
                                4 - IYUV\n\
  -b, --bios=BIOS              Use given bios file\n\
  -c, --config=FILE            Read the given configuration file\n\
  -d, --debug                  Enter debugger\n\
  -f, --filter=FILTER          Select filter:\n\
      --filter-normal            0 - normal mode\n\
      --filter-tv-mode           1 - TV Mode\n\
      --filter-2xsai             2 - 2xSaI\n\
      --filter-super-2xsai       3 - Super 2xSaI\n\
      --filter-super-eagle       4 - Super Eagle\n\
      --filter-pixelate          5 - Pixelate\n\
      --filter-motion-blur       6 - Motion Blur\n\
      --filter-advmame           7 - AdvanceMAME Scale2x\n\
      --filter-simple2x          8 - Simple2x\n\
      --filter-bilinear          9 - Bilinear\n\
      --filter-bilinear+        10 - Bilinear Plus\n\
      --filter-scanlines        11 - Scanlines\n\
      --filter-hq2x             12 - hq2x\n\
      --filter-lq2x             13 - lq2x\n\
  -h, --help                   Print this help\n\
  -i, --ips=PATCH              Apply given IPS patch\n\
  -p, --profile=[HERTZ]        Enable profiling\n\
  -s, --frameskip=FRAMESKIP    Set frame skip (0...9)\n\
");
  printf("\
  -t, --save-type=TYPE         Set the available save type\n\
      --save-auto               0 - Automatic (EEPROM, SRAM, FLASH)\n\
      --save-eeprom             1 - EEPROM\n\
      --save-sram               2 - SRAM\n\
      --save-flash              3 - FLASH\n\
      --save-sensor             4 - EEPROM+Sensor\n\
      --save-none               5 - NONE\n\
  -v, --verbose=VERBOSE        Set verbose logging (trace.log)\n\
                                  1 - SWI\n\
                                  2 - Unaligned memory access\n\
                                  4 - Illegal memory write\n\
                                  8 - Illegal memory read\n\
                                 16 - DMA 0\n\
                                 32 - DMA 1\n\
                                 64 - DMA 2\n\
                                128 - DMA 3\n\
                                256 - Undefined instruction\n\
                                512 - AGBPrint messages\n\
\n\
Long options only:\n\
      --agb-print              Enable AGBPrint support\n\
      --auto-frameskip         Enable auto frameskipping\n\
      --ifb-none               No interframe blending\n\
      --ifb-motion-blur        Interframe motion blur\n\
      --ifb-smart              Smart interframe blending\n\
      --no-agb-print           Disable AGBPrint support\n\
      --no-auto-frameskip      Disable auto frameskipping\n\
      --no-ips                 Do not apply IPS patch\n\
      --no-mmx                 Disable MMX support\n\
      --no-pause-when-inactive Don't pause when inactive\n\
      --no-rtc                 Disable RTC support\n\
      --no-show-speed          Don't show emulation speed\n\
      --no-throttle            Disable thrrotle\n\
      --pause-when-inactive    Pause when inactive\n\
      --rtc                    Enable RTC support\n\
      --show-speed-normal      Show emulation speed\n\
      --show-speed-detailed    Show detailed speed data\n\
");
}

int romFilter( const struct dirent * file )
{
    const char * curPtr = file->d_name;
    const char * extPtr = NULL;
    //Don't show 'hidden' files (that start with a '.')
    if ( *curPtr == '.' )
    {
        return false;
    }

    //Find the last period
    while ( *curPtr )
    {
        if( *curPtr == '.' )
        {
            extPtr = curPtr;
        }
        curPtr++;
    }
    if ( !extPtr )
    {
        //No extension, not allowed.
        return 0;
    }
    //We don't want the period...
    extPtr++;

    return !(
            strcasecmp( extPtr, "gb" ) &&
            strcasecmp( extPtr, "gbc" ) &&
            strcasecmp( extPtr, "gba" ) &&
            strcasecmp( extPtr, "zip" ) );
}

void apply_surface( int x, int y, int w, SDL_Surface* source, SDL_Surface* destination )
{
    //Holds offsets
    SDL_Rect offset;
    
    //Source rect
    SDL_Rect src;

    //Get offsets
    offset.x = x;
    offset.y = y;

    src.x = 0;
    src.y = 0;
    src.w = w;
    src.h = source->h;

    //Blit
    SDL_BlitSurface( source, &src, destination, &offset );
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination )
{
    apply_surface( x, y, source->w, source, destination );
}

int sortCompar( const struct dirent ** a, const struct dirent ** b )
{
    return strcasecmp( (*a)->d_name, (*b)->d_name );
}

char * romSelector()
{
    //Init SDL for non-gl interaction...
    surface = SDL_SetVideoMode( 480, 320, 32, SDL_FULLSCREEN | SDL_RESIZABLE );
    if (!surface )
    {
        fprintf( stderr, "Error setting video mode!\n" );
        exit( 1 );
    }

    //Init SDL_TTF to print text to the screen...
    if ( TTF_Init() )
    {
        fprintf( stderr, "Error initializing SDL_ttf!\n" );
        exit ( 1 );
    }

    TTF_Font * font_small = TTF_OpenFont( FONT, 12 );
    TTF_Font * font_normal = TTF_OpenFont( FONT, 18 );
    if ( !font_small || !font_normal )
    {
        fprintf( stderr, "Failed to open font: %s\n", FONT );
        exit( 1 );
    }

    //Make sure rom dir exists
    //XXX: This assumes /media/internal (parent directory) already exists
    int mode = S_IRWXU | S_IRWXG | S_IRWXO;
    int result = mkdir( VBA_HOME, mode );
    if ( result && ( errno != EEXIST ) )
    {
        fprintf( stderr, "Error creating directory %s!\n", VBA_HOME );
        exit( 1 );
    }
    result = mkdir( ROM_PATH, mode );
    if ( result && ( errno != EEXIST ) )
    {
        fprintf( stderr, "Error creating directory %s for roms!\n", ROM_PATH );
        exit( 1 );
    }


    struct dirent ** roms;
    int filecount = scandir( ROM_PATH, &roms, romFilter, sortCompar );
    printf( "Rom count: %d\n", filecount );

    //Display general information
    int top, bottom;
    SDL_Color textColor = { 255, 255, 255 };
    int borderColor = SDL_MapRGB( surface->format, 0, 0, 50 );
    SDL_Surface * title = TTF_RenderText_Blended( font_normal, TITLE, textColor );
    top = 10+title->h+10;

    SDL_Surface * author = TTF_RenderText_Blended( font_small, AUTHOR_TAG, textColor );
    bottom = surface->h - author->h - 10;

    //Draw border/text
    SDL_FillRect( surface, NULL, borderColor );
    apply_surface( surface->w - author->w - 10, surface->h - author->h - 10, author, surface );
    apply_surface( 10, 10, title, surface );

    SDL_UpdateRect( surface, 0, 0, 0, 0 );
    SDL_Rect drawRect;
    drawRect.x = 10;
    drawRect.y = top;
    drawRect.h = bottom-top;
    drawRect.w = surface->w-20;
    int black = SDL_MapRGB(surface->format, 0, 0, 0);
    SDL_FillRect(surface, &drawRect, black);

    if ( filecount < 1 )
    {
        //No roms found! Tell the user with a nice screen.
        //(Note this is where first-time users most likely end up);
        SDL_Color hiColor = { 255, 200, 200 };
        //XXX: This code has gone too far--really should make use of some engine or loop or something :(
        SDL_Surface * nr1 = TTF_RenderText_Blended( font_normal, NO_ROMS1, textColor );
        SDL_Surface * nr2 = TTF_RenderText_Blended( font_normal, NO_ROMS2, textColor );
        SDL_Surface * nr3 = TTF_RenderText_Blended( font_normal, NO_ROMS3, hiColor );
        SDL_Surface * nr4 = TTF_RenderText_Blended( font_normal, NO_ROMS4, textColor );
        SDL_Surface * nr5 = TTF_RenderText_Blended( font_normal, NO_ROMS5, textColor );
        SDL_Surface * nr6 = TTF_RenderText_Blended( font_normal, NO_ROMS6, textColor );
        apply_surface( surface->w/2-nr1->w/2, (top + bottom)/2 - nr1->h - nr2->h - 45, nr1, surface );
        apply_surface( surface->w/2-nr2->w/2, (top + bottom)/2 - nr2->h - 35, nr2, surface );
        apply_surface( surface->w/2-nr3->w/2, (top + bottom)/2 - 25, nr3, surface );
        apply_surface( surface->w/2-nr4->w/2, (top + bottom)/2 + nr3->h + -15, nr4, surface );
        apply_surface( surface->w/2-nr5->w/2, (top + bottom)/2 + nr3->h + nr4->h - 5, nr5, surface );
        apply_surface( surface->w/2-nr6->w/2, (top + bottom)/2 + nr3->h + nr4->h + nr5->h + 5, nr6, surface );
        SDL_UpdateRect( surface, 0, 0, 0, 0 );
        while( 1 );
    }

    //Generate text for each rom...
    SDL_Surface * roms_surface[filecount];
    for ( int i = 0; i < filecount; i++ )
    {
        //Here we remove everything in '()'s or '[]'s
        //which is usually annoying release information, etc
        char buffer[100];
        char * src = roms[i]->d_name;
        char * dst = buffer;
        int inParen = 0;
        while ( *src && dst < buffer+sizeof(buffer) - 1 )
        {
            char c = *src;
            if ( c == '(' || c == '[' )
            {
                inParen++;
            }
            if ( !inParen )
            {
                *dst++ = *src;
            }
            if ( c == ')' || c == ']' )
            {
                inParen--;
            }

            src++;
        }
        *dst = '\0';

        //now remove the extension..
        char * extPtr = NULL;
        dst = buffer;
        while ( *dst )
        {
            if( *dst == '.' )
            {
                extPtr = dst;
            }
            dst++;
        }
        //If we found an extension, end the string at that period
        if ( extPtr )
        {
            *extPtr = '\0';
        }

        roms_surface[i] = TTF_RenderText_Blended( font_normal, buffer, textColor );
    }

    int scroll_offset = 0;
    SDL_Event event;
    bool tap = false;
    bool down = false;
    int romSelected = -1;
    SDL_EnableUNICODE( 1 );
    while( romSelected == -1 )
    {
        //Calculate scroll, etc
        int num_roms_display = ( bottom - top + 10 ) / ( roms_surface[0]->h + 10 );
        //Get key input, process.
        while ( SDL_PollEvent( &event ) )
        {
            switch( event.type )
            {
                case SDL_MOUSEBUTTONDOWN:
                    down = tap = true;
                    break;
                case SDL_MOUSEBUTTONUP:
                    down = false;
                    if ( tap )
                    {
                        int rom_index = ( event.button.y - top ) / ( roms_surface[0]->h + 10 );
                        if ( rom_index >= 0 && rom_index < num_roms_display )
                        {
                            romSelected = rom_index+scroll_offset;
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    //If the mouse moves before going up, it's not a tap
                    tap = false;

                    //scroll accordingly..
                    if ( down )
                    {
                        scroll_offset -= event.motion.yrel / SCROLL_FACTOR;
                        if ( scroll_offset > filecount - num_roms_display ) scroll_offset = filecount - num_roms_display;
                        if ( scroll_offset < 0 ) scroll_offset = 0;
                    }

                    break;
                case SDL_KEYDOWN:
                {
                    //Filter based on letter presses.
                    //For now, just jump to the first thing that starts at or after that letter.
                    char c = (char)event.key.keysym.unicode;
                    if ( 'A' <= c && c <= 'Z' )
                    {
                        //lowercase...
                        c -= ( 'A' - 'a' );
                    }
                    if ( 'a' <= c && c <= 'z' )
                    {
                        //find this letter in the roms...
                        int offset = 0;
                        while( offset < filecount )
                        {
                            char c_file = *roms[offset]->d_name;
                            if ( 'A' <= c_file && c_file <= 'Z' )
                            {
                                //lowercase..
                                c_file -= ( 'A' - 'a' );
                            }
                            if ( c_file >= c )
                            {
                                break;
                            }
                            offset++;
                        }
                        scroll_offset = offset;
                        if ( scroll_offset > filecount - num_roms_display ) scroll_offset = filecount - num_roms_display;
                        if ( scroll_offset < 0 ) scroll_offset = 0;
                    }
                }
                default:
                    break;
            }
        }
        if ( scroll_offset + num_roms_display > filecount )
        {
            num_roms_display = filecount - scroll_offset;
        }

        //Draw border/text
        SDL_FillRect( surface, NULL, borderColor );
        apply_surface( surface->w - author->w - 10, surface->h - author->h - 10, author, surface );
        apply_surface( 10, 10, title, surface );

        //Clear middle
        SDL_FillRect(surface, &drawRect, black);

        //Draw roms...

        for ( int i = 0; i < num_roms_display; i++ )
        {
           int index = scroll_offset + i;
           if ( index == romSelected )
           {
               int hiColor = SDL_MapRGB( surface->format, 128, 128, 0 );
               SDL_Rect hiRect;
               hiRect.x = 10;
               hiRect.y = top+(10+roms_surface[0]->h)*i - 5;
               hiRect.h = roms_surface[index]->h+5;
               hiRect.w = surface->w - 20;
               SDL_FillRect( surface, &hiRect, hiColor );
           }
           apply_surface( 20, top + (10+roms_surface[0]->h)*i, surface->w - 40, roms_surface[index], surface );
        }

        //Update screen.
        SDL_UpdateRect( surface, 0, 0, 0, 0 );
        if ( romSelected != -1 )
        {
            SDL_Delay( 20 );
        }
    }
    SDL_FreeSurface( title );
    SDL_FreeSurface( author );

    char * rom_base = roms[romSelected]->d_name;
    char * rom_full_path = (char *)malloc( strlen( ROM_PATH ) + strlen( rom_base ) + 2 );
    strcpy( rom_full_path, ROM_PATH );
    rom_full_path[strlen(ROM_PATH)] = '/';
    strcpy( rom_full_path + strlen( ROM_PATH ) + 1, rom_base );
    return rom_full_path;
}

void GL_Init()
{
    // setup 2D gl environment
    checkError();
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );//black background
    checkError();

    glDisable(GL_DEPTH_TEST);
    glDepthFunc( GL_ALWAYS );
    checkError();
    glDisable(GL_CULL_FACE);
    checkError();

    GLbyte vShaderStr[] =  
        "attribute vec4 a_position;   \n"
        "attribute vec2 a_texCoord;   \n"
        "varying vec2 v_texCoord;     \n"
        "void main()                  \n"
        "{                            \n"
        "   gl_Position = a_position; \n"
        "   v_texCoord = a_texCoord;  \n"
        "}                            \n";

    GLbyte fShaderStr[] =  
        "precision mediump float;                            \n"
        "varying vec2 v_texCoord;                            \n"
        "uniform sampler2D s_texture;                        \n"
        "void main()                                         \n"
        "{                                                   \n"
        "  gl_FragColor = texture2D( s_texture, v_texCoord );\n"
        "}                                                   \n";

    // Load the shaders and get a linked program object
    programObject = esLoadProgram ( ( char *)vShaderStr, (char *)fShaderStr );
    checkError();

    // Get the attribute locations
    positionLoc = glGetAttribLocation ( programObject, "a_position" );
    checkError();
    texCoordLoc = glGetAttribLocation ( programObject, "a_texCoord" );
    checkError();

    // Get the sampler location
    samplerLoc = glGetUniformLocation ( programObject, "s_texture" );
    checkError();
}


void GL_InitTexture()
{
    //delete it if we already have one
    if ( texture )
    {
        glDeleteTextures( 1, &texture );
        texture = 0;
    }
    if ( controller_tex )
    {
        glDeleteTextures( 1, &controller_tex );
        controller_tex = 0;
    }

    glGenTextures(1, &texture);
    checkError();
    glBindTexture(GL_TEXTURE_2D, texture);
    checkError();
    
    //sanity check
    int num;
    glGetIntegerv( GL_TEXTURE_BINDING_2D, &num );
    assert( num == texture );
    glGetIntegerv( GL_ACTIVE_TEXTURE, &num );
    assert( num == GL_TEXTURE0 );
    checkError();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter );
    checkError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter );
    checkError();

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    checkError();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    checkError();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, srcWidth, srcHeight, 0, GL_RGB,
            GL_UNSIGNED_BYTE, NULL );
    checkError();

    //Load controller
    SDL_Surface * initial_surface = IMG_Load( CONTROLLER_IMG );
    if ( !initial_surface )
    {
        printf( "No controller image found!  Running without one...\n" );
        return;
    }
    //Create RGB surface and copy controller into it
    SDL_Surface * controller_surface = SDL_CreateRGBSurface( SDL_SWSURFACE, initial_surface->w, initial_surface->h, 24,
            0x0000ff, 0x00ff00, 0xff0000, 0);
    SDL_BlitSurface( initial_surface, NULL, controller_surface, NULL );

    glGenTextures(1, &controller_tex );
    glBindTexture( GL_TEXTURE_2D, controller_tex );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter );
    checkError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter );
    checkError();

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    checkError();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    checkError();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, controller_surface->w, controller_surface->h, 0, GL_RGB,
            GL_UNSIGNED_BYTE, controller_surface->pixels );
    checkError();

    SDL_FreeSurface( initial_surface );
    SDL_FreeSurface( controller_surface );
}

void updateOrientation()
{
    //XXX: This function is a beast, make it less crazy.
    //
    float screenAspect = (float)destWidth/(float)destHeight;
    float emulatedAspect = (float)srcWidth/(float)srcHeight;
    
    //XXX: 'orientation' is invariant as far the rendering loop goes; move
    //the corresponding invariant results (vertexCoords, etc)
    //to be calculated outside this method
    switch( orientation )
    {
        case ORIENTATION_LANDSCAPE_R:
            memcpy( vertexCoords, land_r_vertexCoords, 8*sizeof(float) );
            break;
        case ORIENTATION_LANDSCAPE_L:
            memcpy( vertexCoords, land_l_vertexCoords, 8*sizeof(float) );
            break;
        default:
            printf( "Unsupported orientation: %d!\n", orientation );
            printf( "Defaulting to portrait orientation\n" );
            //fall through
            orientation = ORIENTATION_PORTRAIT;
        case ORIENTATION_PORTRAIT:
            memcpy( vertexCoords, portrait_vertexCoords, 8*sizeof(float) );
            break;
    }
    if ( orientation != ORIENTATION_PORTRAIT )
    {
        emulatedAspect = 1/emulatedAspect;//landscape has reversed aspect ratio
    }

    for ( int i = 0; i < 4; i++ )
    {
        vertexCoords[2*i+1] *= screenAspect / emulatedAspect;
    }

    if ( use_on_screen && orientation == ORIENTATION_LANDSCAPE_R )
    {
        float controller_aspect = CONTROLLER_SCREEN_WIDTH / CONTROLLER_SCREEN_HEIGHT;
        float scale_factor;
        if ( srcHeight * controller_aspect  > CONTROLLER_SCREEN_WIDTH )
        {
            //width is limiting factor
            scale_factor = ( CONTROLLER_SCREEN_HEIGHT / (float)destWidth );
        }
        else
        {
            //height is limiting factor
            //'effectiveWidth' b/c we already scaled previously
            //and we don't fill the screen due to aspect ratio
            float effectiveWidth = (float)destWidth / emulatedAspect;
            scale_factor = ( CONTROLLER_SCREEN_WIDTH / effectiveWidth );
        }

        for ( int i = 0; i < 4; i++ )
        {
            //scale
            vertexCoords[2*i] *= scale_factor;
            vertexCoords[2*i+1] *= scale_factor;
        }

        float y_offset = 1.0 - vertexCoords[0];
        float x_offset = 1.0 - vertexCoords[1];

        //push the screen to the coordinates indicated
        y_offset -= ( CONTROLLER_SCREEN_Y_OFFSET / (float)destWidth ) * 2;
        x_offset -= ( CONTROLLER_SCREEN_X_OFFSET / (float)destHeight ) * 2;

        for ( int i = 0; i < 4; i++ )
        {
            //translate
            vertexCoords[2*i] += y_offset;
            vertexCoords[2*i+1] += x_offset;
        }
    }
}

int main(int argc, char **argv)
{
  fprintf(stderr, "VisualBoyAdvance version %s [SDL]\n", VERSION);

  arg0 = argv[0];
  
  captureDir[0] = 0;
  saveDir[0] = 0;
  batteryDir[0] = 0;
  ipsname[0] = 0;
  
  int op = -1;

  frameSkip = 2;
  gbBorderOn = 0;

  parseDebug = true;

  sdlReadPreferences();
  readOptions();

  sdlPrintUsage = 0;
  
  while((op = getopt_long(argc,
                          argv,
                          "FNT:Y:G:D:b:c:df:hi:p::s:t:v:1234",
                          sdlOptions,
                          NULL)) != -1) {
    switch(op) {
    case 0:
      // long option already processed by getopt_long
      break;
    case 'b':
      useBios = true;
      if(optarg == NULL) {
        fprintf(stderr, "Missing BIOS file name\n");
        exit(-1);
      }
      strcpy(biosFileName, optarg);
      break;
    case 'c':
      {
        if(optarg == NULL) {
          fprintf(stderr, "Missing config file name\n");
          exit(-1);
        }
        FILE *f = fopen(optarg, "r");
        if(f == NULL) {
          fprintf(stderr, "File not found %s\n", optarg);
          exit(-1);
        }
        sdlReadPreferences(f);
        fclose(f);
      }
      break;
    case 'd':
      debugger = true;
      break;
    case 'h':
      sdlPrintUsage = 1;
      break;
    case 'i':
      if(optarg == NULL) {
        fprintf(stderr, "Missing IPS name\n");
        exit(-1);
        strcpy(ipsname, optarg);
      }
      break;
    case 'Y':
      yuv = true;
      if(optarg) {
        yuvType = atoi(optarg);
        switch(yuvType) {
        case 0:
          yuvType = SDL_YV12_OVERLAY;
          break;
        case 1:
          yuvType = SDL_UYVY_OVERLAY;
          break;
        case 2:
          yuvType = SDL_YVYU_OVERLAY;
          break;
        case 3:
          yuvType = SDL_YUY2_OVERLAY;
          break;
        case 4:
          yuvType = SDL_IYUV_OVERLAY;
          break;
        default:
          yuvType = SDL_YV12_OVERLAY;
        }
      } else
        yuvType = SDL_YV12_OVERLAY;
      break;
    case 'G':
      dbgMain = remoteStubMain;
      dbgSignal = remoteStubSignal;
      dbgOutput = remoteOutput;
      debugger = true;
      debuggerStub = true;
      if(optarg) {
        char *s = optarg;
        if(strncmp(s,"tcp:", 4) == 0) {
          s+=4;
          int port = atoi(s);
          remoteSetProtocol(0);
          remoteSetPort(port);
        } else if(strcmp(s,"tcp") == 0) {
          remoteSetProtocol(0);
        } else if(strcmp(s, "pipe") == 0) {
          remoteSetProtocol(1);
        } else {
          fprintf(stderr, "Unknown protocol %s\n", s);
          exit(-1);
        }
      } else {
        remoteSetProtocol(0);
      }
      break;
    case 'N':
      parseDebug = false;
      break;
    case 'D':
      if(optarg) {
        systemDebug = atoi(optarg);
      } else {
        systemDebug = 1;
      }
      break;
    case 'F':
      fullscreen = 1;
      mouseCounter = 120;
      break;
    case 'f':
      if(optarg) {
        filter = atoi(optarg);
      } else {
        filter = 0;
      }
      break;
    case 'p':
#ifdef PROFILING
      if(optarg) {
        cpuEnableProfiling(atoi(optarg));
      } else
        cpuEnableProfiling(100);
#endif
      break;
    case 'S':
      sdlFlashSize = atoi(optarg);
      if(sdlFlashSize < 0 || sdlFlashSize > 1)
        sdlFlashSize = 0;
      break;
    case 's':
      if(optarg) {
        int a = atoi(optarg);
        if(a >= 0 && a <= 9) {
          gbFrameSkip = a;
          frameSkip = a;
        }
      } else {
        frameSkip = 2;
        gbFrameSkip = 0;
      }
      break;
    case 't':
      if(optarg) {
        int a = atoi(optarg);
        if(a < 0 || a > 5)
          a = 0;
        cpuSaveType = a;
      }
      break;
    case 'T':
      if(optarg) {
        int t = atoi(optarg);
        if(t < 5 || t > 1000)
          t = 0;
        throttle = t;
      }
      break;
    case 'v':
      if(optarg) {
        systemVerbose = atoi(optarg);
      } else 
        systemVerbose = 0;
      break;
    case '1':
      sizeOption = 0;
      break;
    case '2':
      sizeOption = 1;
      break;
    case '3':
      sizeOption = 2;
      break;
    case '4':
      sizeOption = 3;
      break;
    case '?':
      sdlPrintUsage = 1;
      break;
    }
  }

  if(sdlPrintUsage) {
    usage(argv[0]);
    exit(-1);
  }

#ifdef MMX
  if(disableMMX)
    cpu_mmx = 0;
#endif

  if(rewindTimer)
    rewindMemory = (char *)malloc(8*REWIND_SIZE);

  //force higher flash size.
  flashSetSize(0x20000);

  //force RTC --doesn't hurt, and some games need it.
  rtcEnable( true );

  agbPrintEnable(sdlAgbPrint ? true : false);

  if(filter) {
    sizeOption = 1;
  }

  for(int i = 0; i < 24;) {
    systemGbPalette[i++] = (0x1f) | (0x1f << 5) | (0x1f << 10);
    systemGbPalette[i++] = (0x15) | (0x15 << 5) | (0x15 << 10);
    systemGbPalette[i++] = (0x0c) | (0x0c << 5) | (0x0c << 10);
    systemGbPalette[i++] = 0;
  }

  systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

  printf( "Selecting rom...\n" );
  char * szFile = romSelector();

  utilGetBaseName(szFile, filename);
  char *p = strrchr(filename, '.');

  if(p)
      *p = 0;

  if(ipsname[0] == 0)
      sprintf(ipsname, "%s.ips", filename);

  bool failed = false;

  IMAGE_TYPE type = utilFindType(szFile);

  if(type == IMAGE_UNKNOWN) {
      systemMessage(0, "Unknown file type %s", szFile);
      exit(-1);
  }
  cartridgeType = (int)type;

  if(type == IMAGE_GB) {
      failed = !gbLoadRom(szFile);
      if(!failed) {
          cartridgeType = 1;
          emulator = GBSystem;
          if(sdlAutoIPS) {
              int size = gbRomSize;
              utilApplyIPS(ipsname, &gbRom, &size);
              if(size != gbRomSize) {
                  extern bool gbUpdateSizes();
                  gbUpdateSizes();
                  gbReset();
              }
          }
      }
  } else if(type == IMAGE_GBA) {
      int size = CPULoadRom(szFile);
      failed = (size == 0);
      if(!failed) {
          //        if(cpuEnhancedDetection && cpuSaveType == 0) {
          //          utilGBAFindSave(rom, size);
          //        }

          sdlApplyPerImagePreferences();

          cartridgeType = 0;
          emulator = GBASystem;

          /* disabled due to problems
             if(removeIntros && rom != NULL) {
             WRITE32LE(&rom[0], 0xea00002e);
             }
             */

          CPUInit(biosFileName, useBios);
          CPUReset();
          if(sdlAutoIPS) {
              int size = 0x2000000;
              utilApplyIPS(ipsname, &rom, &size);
              if(size != 0x2000000) {
                  CPUReset();
              }
          }
      }
  }

  if(failed) {
      systemMessage(0, "Failed to load file %s", szFile);
      exit(-1);
  }
//} else {
//    cartridgeType = 0;
//    strcpy(filename, "gnu_stub");
//    rom = (u8 *)malloc(0x2000000);
//    workRAM = (u8 *)calloc(1, 0x40000);
//    bios = (u8 *)calloc(1,0x4000);
//    internalRAM = (u8 *)calloc(1,0x8000);
//    paletteRAM = (u8 *)calloc(1,0x400);
//    vram = (u8 *)calloc(1, 0x20000);
//    oam = (u8 *)calloc(1, 0x400);
//    pix = (u8 *)calloc(1, 4 * 240 * 160);
//    ioMem = (u8 *)calloc(1, 0x400);
//
//    emulator = GBASystem;
//
//    CPUInit(biosFileName, useBios);
//    CPUReset();    
//}

  sdlReadBattery();

  if ( autosave )
  {
    sdlReadState( AUTOSAVE_STATE );
  }

  
  if(debuggerStub) 
    remoteInit();
  
  int flags = SDL_INIT_VIDEO|SDL_INIT_AUDIO|
    SDL_INIT_TIMER|SDL_INIT_NOPARACHUTE;

  if(soundOffFlag)
    flags ^= SDL_INIT_AUDIO;
  
  if(SDL_Init(flags)) {
    systemMessage(0, "Failed to init SDL: %s", SDL_GetError());
    exit(-1);
  }

  if(SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
    systemMessage(0, "Failed to init joystick support: %s", SDL_GetError());
  }
  
  sdlCheckKeys();
  
  if(cartridgeType == 0) {
    srcWidth = 240;
    srcHeight = 160;
    systemFrameSkip = frameSkip;
  } else if (cartridgeType == 1) {
    if(gbBorderOn) {
      srcWidth = 256;
      srcHeight = 224;
      gbBorderLineSkip = 256;
      gbBorderColumnSkip = 48;
      gbBorderRowSkip = 40;
    } else {      
      srcWidth = 160;
      srcHeight = 144;
      gbBorderLineSkip = 160;
      gbBorderColumnSkip = 0;
      gbBorderRowSkip = 0;
    }
    systemFrameSkip = gbFrameSkip;
  } else {
    srcWidth = 320;
    srcHeight = 240;
  }
  
  destWidth = 320;
  destHeight = 480;
  
  assert( !SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 ) );
  assert( !SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 ) );
  assert( !SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 ) );
  assert( !SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 ) );
  assert( !SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 ) );
  assert( !SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 ) );
  assert( !SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 ) );
  //assert( !SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 1 ) );
  //assert( !SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 ) );
 //assert( !SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, -1 ) );

  SDL_SetVideoMode( 480, 320, 32, SDL_FULLSCREEN | SDL_RESIZABLE );
  surface = SDL_SetVideoMode( 320, 480, 32,
                             SDL_OPENGLES|
                             (fullscreen ? SDL_FULLSCREEN : 0));
  
  if(surface == NULL) {
    systemMessage(0, "Failed to set video mode");
    SDL_Quit();
    exit(-1);
  }

  GL_Init();
  GL_InitTexture();
  updateOrientation();
  
  // Here we are forcing the bitdepth and format to use.
  // I chose 16 instead of 32 because it will be faster to work with cpu-wise (and sending to gpu)
  // and let the gpu up-convert it. Also GBA doesn't have 32-bit color anyway.
 
  // Furthermore I chose 5-6-5 RGB encoding because
  // a)we have no use for alpha in the main texture.
  // b)I like blue. :P
  //
  // But really, of the GL_SHORT pixel formats I don't know that it matters.
  // Small note: since the alpha bit is 'inverted' ('1' means opaque),
  // the colormaps would have to be changed accordingly.  This is entirely untested.

  //See above for the format, these are the shifts for each component
  //RGB format
  //
  //Note that 5-6-5 has green at offset '5' not '6', but
  //we only have 2^5 blue values to represent, so we shift it.
  systemRedShift = 11;
  systemGreenShift = 6;
  systemBlueShift = 0;

  systemColorDepth = 16;

  //I'm not sure that this matters anymore. XXX Find out and remove.
  RGB_LOW_BITS_MASK = 0x821;

  //Set the colormap using the shifts.
  if(cartridgeType == 2) {
      for(int i = 0; i < 0x10000; i++) {
          systemColorMap16[i] = (((i >> 1) & 0x1f) << systemBlueShift) |
              (((i & 0x7c0) >> 6) << systemGreenShift) |
              (((i & 0xf800) >> 11) << systemRedShift);  
      }      
  } else {
      for(int i = 0; i < 0x10000; i++) {
          systemColorMap16[i] = ((i & 0x1f) << systemRedShift) |
              (((i & 0x3e0) >> 5) << systemGreenShift) |
              (((i & 0x7c00) >> 10) << systemBlueShift);  
      }
  }

  srcPitch = srcWidth * 2;

  //No filter support (should be implemented as part of the GL shader)
  ifbFunction = NULL;
  filterFunction = NULL;

  if(delta == NULL) {
      delta = (u8*)malloc(322*242*4);
      memset(delta, 255, 322*242*4);
  }

  emulating = 1;
  renderedFrames = 0;

  if(!soundOffFlag)
      soundInit();

  autoFrameSkipLastTime = throttleLastTime = systemGetClock();

  SDL_WM_SetCaption("VisualBoyAdvance", NULL);

  while(emulating) {
    if(!paused && active) {
      if(debugger && emulator.emuHasDebugger)
        dbgMain();
      else {
        emulator.emuMain(emulator.emuCount);
        if(rewindSaveNeeded && rewindMemory && emulator.emuWriteMemState) {
          rewindCount++;
          if(rewindCount > 8)
            rewindCount = 8;
          if(emulator.emuWriteMemState &&
             emulator.emuWriteMemState(&rewindMemory[rewindPos*REWIND_SIZE], 
                                       REWIND_SIZE)) {
            rewindPos = ++rewindPos & 7;
            if(rewindCount == 8)
              rewindTopPos = ++rewindTopPos & 7;
          }
        }

        rewindSaveNeeded = false;
      }
    } else {
      SDL_Delay(500);
    }
    sdlPollEvents();
    if ( keyBindingMode != NOT_BINDING )
    {
        //make sure the message stays up until binding is over
        systemScreenMessage( bindingNames[keyBindingMode] );
    }
  }
  
  emulating = 0;
  fprintf(stderr,"Shutting down\n");
  remoteCleanUp();
  soundShutdown();

  if(gbRom != NULL || rom != NULL) {
    sdlWriteBattery();
    emulator.emuCleanUp();
  }

  if(delta) {
    free(delta);
    delta = NULL;
  }
  
  SDL_Quit();
  return 0;
}

void systemMessage(int num, const char *msg, ...)
{
  char buffer[2048];
  va_list valist;
  
  va_start(valist, msg);
  vsprintf(buffer, msg, valist);
  
  fprintf(stderr, "%s\n", buffer);
  va_end(valist);
}

void drawScreenText()
{
  if(screenMessage) {
    if(((systemGetClock() - screenMessageTime) < 3000) &&
       !disableStatusMessages) {
      drawText(pix, srcPitch, 10, srcHeight - 20,
               screenMessageBuffer); 
    } else {
      screenMessage = false;
    }
  }

  if(showSpeed) {
    char buffer[40];
    if(showSpeed == 1)
      sprintf(buffer, "%d%%", systemSpeed);
    else
      sprintf(buffer, "%3d%%(%d, %d fps)", systemSpeed,
              systemFrameSkip,
              showRenderedFrames);
      drawText(pix, srcPitch, 10, srcHeight - 20,
               buffer); 
      static int counter;
      if ( counter++ > 10 )
      {
          printf( "SPEED: %s\n", buffer );
          counter = 0;
      }
  }  

}

void systemDrawScreen()
{
    renderedFrames++;

    glClear( GL_COLOR_BUFFER_BIT );
    checkError();

    /*-----------------------------------------------------------------------------
     *  Overlay
     *-----------------------------------------------------------------------------*/
    if ( use_on_screen && orientation == ORIENTATION_LANDSCAPE_R )
    {
        // Use the program object
        glUseProgram ( programObject );
        checkError();

        glVertexAttribPointer( positionLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), controller_coords );
        checkError();
        glVertexAttribPointer( texCoordLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), texCoords );

        checkError();

        glEnableVertexAttribArray( positionLoc );
        checkError();
        glEnableVertexAttribArray( texCoordLoc );
        checkError();

        checkError();

        //sampler texture unit to 0
        glBindTexture(GL_TEXTURE_2D, controller_tex);
        glUniform1i( samplerLoc, 0 );
        checkError();

        glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
        checkError();
    }


    /*-----------------------------------------------------------------------------
     *  Draw the frame of the gb(c/a)
     *-----------------------------------------------------------------------------*/

    // Use the program object
    glUseProgram ( programObject );
    checkError();

    glVertexAttribPointer( positionLoc, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), vertexCoords );
    checkError();
    glVertexAttribPointer( texCoordLoc, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), texCoords );

    checkError();

    glEnableVertexAttribArray( positionLoc );
    checkError();
    glEnableVertexAttribArray( texCoordLoc );
    checkError();

    drawScreenText();

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D( GL_TEXTURE_2D,0,
            0,0, srcWidth,srcHeight,
            GL_RGB,GL_UNSIGNED_SHORT_5_6_5,pix);

    checkError();

    //sampler texture unit to 0
    glUniform1i( samplerLoc, 0 );
    checkError();

    glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
    checkError();

    
    //Push to screen
    SDL_GL_SwapBuffers();
    checkError();

    return;
}

bool systemReadJoypads()
{
  return true;
}

u32 systemReadJoypad(int which)
{
  if(which < 0 || which > 3)
    which = sdlDefaultJoypad;
  
  u32 res = 0;
  
  if(sdlButtons[which][KEY_BUTTON_A])
    res |= 1;
  if(sdlButtons[which][KEY_BUTTON_B])
    res |= 2;
  if(sdlButtons[which][KEY_BUTTON_SELECT])
    res |= 4;
  if(sdlButtons[which][KEY_BUTTON_START])
    res |= 8;
  if(sdlButtons[which][KEY_RIGHT])
    res |= 16;
  if(sdlButtons[which][KEY_LEFT])
    res |= 32;
  if(sdlButtons[which][KEY_UP])
    res |= 64;
  if(sdlButtons[which][KEY_DOWN])
    res |= 128;
  if(sdlButtons[which][KEY_BUTTON_R])
    res |= 256;
  if(sdlButtons[which][KEY_BUTTON_L])
    res |= 512;

  // disallow L+R or U+D of being pressed at the same time
  if((res & 48) == 48)
    res &= ~16;
  if((res & 192) == 192)
    res &= ~128;

  if(sdlButtons[which][KEY_BUTTON_SPEED])
    res |= 1024;
  if(sdlButtons[which][KEY_BUTTON_CAPTURE])
    res |= 2048;

  if(autoFire) {
    res &= (~autoFire);
    if(autoFireToggle)
      res |= autoFire;
    autoFireToggle = !autoFireToggle;
  }
  
  return res;
}

void systemSetTitle(const char *title)
{
  SDL_WM_SetCaption(title, NULL);
}

void systemShowSpeed(int speed)
{
  systemSpeed = speed;

  showRenderedFrames = renderedFrames;
  renderedFrames = 0;  

  if(!fullscreen && showSpeed) {
    char buffer[80];
    if(showSpeed == 1)
      sprintf(buffer, "VisualBoyAdvance-%3d%%", systemSpeed);
    else
      sprintf(buffer, "VisualBoyAdvance-%3d%%(%d, %d fps)", systemSpeed,
              systemFrameSkip,
              showRenderedFrames);

    systemSetTitle(buffer);
  }
}

void systemFrame()
{
}

void system10Frames(int rate)
{
  u32 time = systemGetClock();  
  if(!wasPaused && autoFrameSkip && !throttle) {
    u32 diff = time - autoFrameSkipLastTime;
    int speed = 100;

    if(diff)
      speed = (1000000/rate)/diff;
    
    if(speed >= 98) {
      frameskipadjust++;

      if(frameskipadjust >= 3) {
        frameskipadjust=0;
        if(systemFrameSkip > 0)
          systemFrameSkip--;
      }
    } else {
      if(speed  < 80)
        frameskipadjust -= (90 - speed)/5;
      else if(systemFrameSkip < 9)
        frameskipadjust--;

      if(frameskipadjust <= -2) {
        frameskipadjust += 2;
        if(systemFrameSkip < 9)
          systemFrameSkip++;
      }
    }    
  }
  if(!wasPaused && throttle) {
    if(!speedup) {
      u32 diff = time - throttleLastTime;
      
      int target = (1000000/(rate*throttle));
      int d = (target - diff);
      
      if(d > 0) {
        SDL_Delay(d);
      }
    }
    throttleLastTime = systemGetClock();
  }
  if(rewindMemory) {
    if(++rewindCounter >= rewindTimer) {
      rewindSaveNeeded = true;
      rewindCounter = 0;
    }
  }

  if(systemSaveUpdateCounter) {
    if(--systemSaveUpdateCounter <= SYSTEM_SAVE_NOT_UPDATED) {
      sdlWriteBattery();
      systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
    }
  }

  wasPaused = false;
  autoFrameSkipLastTime = time;
}

void systemScreenCapture(int a)
{
  char buffer[2048];

  if(captureFormat) {
    if(captureDir[0])
      sprintf(buffer, "%s/%s%02d.bmp", captureDir, sdlGetFilename(filename), a);
    else
      sprintf(buffer, "%s%02d.bmp", filename, a);

    emulator.emuWriteBMP(buffer);
  } else {
    if(captureDir[0])
      sprintf(buffer, "%s/%s%02d.png", captureDir, sdlGetFilename(filename), a);
    else
      sprintf(buffer, "%s%02d.png", filename, a);
    emulator.emuWritePNG(buffer);
  }

  systemScreenMessage("Screen capture");
}

void soundCallback(void *,u8 *stream,int len)
{
  if(!emulating)
    return;

  // Patch #1382692 by deathpudding.
  // (ported from vba 1.8.0)
  /* since this is running in a different thread, speedup and
   * throttle can change at any time; save the value so locks
   * stay in sync */
  bool lock = (!speedup && !throttle) ? true : false;

  if (lock)
    SDL_SemWait (sdlBufferFull);

  SDL_SemWait (sdlBufferLock);
  memcpy (stream, sdlBuffer, len);
  sdlSoundLen = 0;
  SDL_SemPost (sdlBufferLock);

  if (lock)
    SDL_SemPost (sdlBufferEmpty);
}

void systemWriteDataToSoundBuffer()
{
  // Patch #1382692 by deathpudding.
  // (ported from vba 1.8.0)
  if (SDL_GetAudioStatus () != SDL_AUDIO_PLAYING)
    SDL_PauseAudio (0);

  if ((sdlSoundLen + soundBufferLen) >= 2048*2) {
    bool lock = (!speedup && !throttle) ? true : false;

    if (lock)
      SDL_SemWait (sdlBufferEmpty);

    SDL_SemWait (sdlBufferLock);
    int copied = 2048*2 - sdlSoundLen;
    memcpy (sdlBuffer + sdlSoundLen, soundFinalWave, copied);
    sdlSoundLen = 2048*2;
    SDL_SemPost (sdlBufferLock);

    if (lock) {
      SDL_SemPost (sdlBufferFull);

      /* wait for buffer to be dumped by soundCallback() */
      SDL_SemWait (sdlBufferEmpty);
      SDL_SemPost (sdlBufferEmpty);

      SDL_SemWait (sdlBufferLock);
      memcpy (sdlBuffer, ((u8 *)soundFinalWave) + copied,
          soundBufferLen - copied);
      sdlSoundLen = soundBufferLen - copied;
      SDL_SemPost (sdlBufferLock);
    }
    else {
      SDL_SemWait (sdlBufferLock);
      memcpy (sdlBuffer, ((u8 *) soundFinalWave) + copied, soundBufferLen);
      SDL_SemPost (sdlBufferLock);
    }
  }
  else {
    SDL_SemWait (sdlBufferLock);
    memcpy (sdlBuffer + sdlSoundLen, soundFinalWave, soundBufferLen);
     sdlSoundLen += soundBufferLen;
    SDL_SemPost (sdlBufferLock);
  }
}

bool systemSoundInit()
{
  SDL_AudioSpec audio;

  switch(soundQuality) {
  case 1:
    audio.freq = 44100;
    soundBufferLen = 1470*2;
    break;
  case 2:
    audio.freq = 22050;
    soundBufferLen = 736*2;
    break;
  case 4:
    audio.freq = 11025;
    soundBufferLen = 368*2;
    break;
  }
  audio.format=AUDIO_S16SYS;
  audio.channels = 2;
  audio.samples = 1024;
  audio.callback = soundCallback;
  audio.userdata = NULL;
  if(SDL_OpenAudio(&audio, NULL)) {
    fprintf(stderr,"Failed to open audio: %s\n", SDL_GetError());
    return false;
  }
  soundBufferTotalLen = soundBufferLen*10;
  // Patch #1382692 by deathpudding.
  // (ported from vba 1.8.0)
  sdlBufferLock  = SDL_CreateSemaphore (1);
  sdlBufferFull  = SDL_CreateSemaphore (0);
  sdlBufferEmpty = SDL_CreateSemaphore (1);
  sdlSoundLen = 0;
  systemSoundOn = true;
  return true;
}

void systemSoundShutdown()
{
  // Patch #1382692 by deathpudding.
  SDL_CloseAudio (); //TODO: fix freeze
  SDL_DestroySemaphore (sdlBufferLock);
  SDL_DestroySemaphore (sdlBufferFull);
  SDL_DestroySemaphore (sdlBufferEmpty);
  sdlBufferLock  = NULL;
  sdlBufferFull  = NULL;
  sdlBufferEmpty = NULL;
}

void systemSoundPause()
{
  SDL_PauseAudio(1);
}

void systemSoundResume()
{
  SDL_PauseAudio(0);
}

void systemSoundReset()
{
}

u32 systemGetClock()
{
  return SDL_GetTicks();
}

void systemUpdateMotionSensor()
{
  if(sdlMotionButtons[KEY_LEFT]) {
    sensorX += 3;
    if(sensorX > 2197)
      sensorX = 2197;
    if(sensorX < 2047)
      sensorX = 2057;
  } else if(sdlMotionButtons[KEY_RIGHT]) {
    sensorX -= 3;
    if(sensorX < 1897)
      sensorX = 1897;
    if(sensorX > 2047)
      sensorX = 2037;
  } else if(sensorX > 2047) {
    sensorX -= 2;
    if(sensorX < 2047)
      sensorX = 2047;
  } else {
    sensorX += 2;
    if(sensorX > 2047)
      sensorX = 2047;
  }

  if(sdlMotionButtons[KEY_UP]) {
    sensorY += 3;
    if(sensorY > 2197)
      sensorY = 2197;
    if(sensorY < 2047)
      sensorY = 2057;
  } else if(sdlMotionButtons[KEY_DOWN]) {
    sensorY -= 3;
    if(sensorY < 1897)
      sensorY = 1897;
    if(sensorY > 2047)
      sensorY = 2037;
  } else if(sensorY > 2047) {
    sensorY -= 2;
    if(sensorY < 2047)
      sensorY = 2047;
  } else {
    sensorY += 2;
    if(sensorY > 2047)
      sensorY = 2047;
  }    
}

int systemGetSensorX()
{
  return sensorX;
}

int systemGetSensorY()
{
  return sensorY;
}

void systemGbPrint(u8 *data,int pages,int feed,int palette, int contrast)
{
}

void systemScreenMessage(const char *msg)
{
  screenMessage = true;
  screenMessageTime = systemGetClock();
  if(strlen(msg) > 20) {
    strncpy(screenMessageBuffer, msg, 20);
    screenMessageBuffer[20] = 0;
  } else
    strcpy(screenMessageBuffer, msg);  
}

bool systemCanChangeSoundQuality()
{
  return false;
}

bool systemPauseOnFrame()
{
  if(pauseNextFrame) {
    paused = true;
    pauseNextFrame = false;
    return true;
  }
  return false;
}

void systemGbBorderOn()
{
  printf( "Not supported!\n" );
  exit( -1 );
}

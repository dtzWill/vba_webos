/*
 * ===========================================================================
 *
 *       Filename:  VBA.h
 *
 *    Description:  VBA master header
 *
 *        Version:  1.0
 *        Created:  08/12/2010 04:55:30 PM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

#ifndef _VBA_H_
#define _VBA_H_

#include <SDL_video.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <assert.h>
#include "GBA.h"

#define VERSION "1.3.4"

#define VBA_WIKI "http://www.webos-internals.org/wiki/Application:VBA"
#define VBA_HOME "/media/internal/vba"
#define ROM_PATH VBA_HOME "/roms/"
#define SAV_PATH VBA_HOME "/sav/"
#define SKIN_PATH VBA_HOME "/skins/"
#define SKIN_CFG_NAME "controller.cfg"
#define SKIN_IMG_NAME "controller.png"
#define FONT "/usr/share/fonts/PreludeCondensed-Medium.ttf"
#define TITLE "VisualBoyAdvance for WebOS (" VERSION ")"
#define AUTHOR_TAG "Support: webos@wdtz.org"
#define OPTIONS_TEXT "MENU"

#ifdef PALM_PRE
#define NATIVE_RES_WIDTH 320
#define NATIVE_RES_HEIGHT 480
#endif

#ifdef PALM_PIXI
#define NATIVE_RES_WIDTH 320
#define NATIVE_RES_HEIGHT 400
#endif

#define OPTIONS_CFG "options.cfg"

#define SCROLL_FACTOR 20
#define AUTOSAVE_STATE 100


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

enum orientation
{
    ORIENTATION_PORTRAIT,    // default mode, portrait
    ORIENTATION_LANDSCAPE_R, // landscape, keyboard on right
    ORIENTATION_LANDSCAPE_L  // landscape, keyboard on left
};

extern int systemSpeed;
extern int systemRedShift;
extern int systemBlueShift;
extern int systemGreenShift;
extern int systemColorDepth;
extern int systemDebug;
extern int systemVerbose;
extern int systemFrameSkip;
extern int systemSaveUpdateCounter;

extern int srcPitch;
extern int srcWidth;
extern int srcHeight;
extern int destWidth;
extern int destHeight;

extern int sensorX;
extern int sensorY;

extern int filter;

extern int sdlPrintUsage;
extern int disableMMX;

extern int cartridgeType;
extern int sizeOption;
extern int captureFormat;

extern int pauseWhenInactive;
extern int active;
extern int emulating;
extern int RGB_LOW_BITS_MAS;
extern u16 systemGbPalette[24];
extern void (*filterFunction)(u8*,u32,u8*,u8*,u32,int,int);
extern void (*ifbFunction)(u8*,u32,int,int);
extern int ifbType;
extern char filename[2048];
extern char ipsname[2048];
extern char biosFileName[2048];
extern char captureDir[2048];
extern char saveDir[2048];
extern char batteryDir[2048];

#define _stricmp strcasecmp

extern int orientation;
extern int gl_filter;
extern int use_on_screen;
extern int autosave;
extern int running;
extern int turbo_toggle;
extern int stretch;

extern int turbo_on;

/*-----------------------------------------------------------------------------
 *  Game state
 *-----------------------------------------------------------------------------*/
extern bool wasPaused;
extern int autoFrameSkip;
extern int showRenderedFrames;
extern int renderedFrames;

extern int throttle;
extern u32 throttleLastTime;
extern u32 autoFrameSkipLastTime;

extern int showSpeed;
extern int showSpeedTransparent;
extern bool disableStatusMessages;
extern bool paused;
extern bool pauseNextFrame;
extern bool debugger;
extern bool debuggerStub;
extern int fullscreen;
extern int soundMute;
extern bool systemSoundOn;
extern bool yuv;
extern int yuvType;
extern bool removeIntros;
extern int sdlFlashSize;
extern int sdlAutoIPS;
extern int sdlRtcEnable;
extern int sdlAgbPrint;

extern void debuggerSignal(int,int);

extern bool CPUIsGBAImage(char *);
extern bool gbIsGameboyRom(char *);

void pickRom();
void runRom();

extern void remoteInit();
extern void remoteCleanUp();
extern void remoteStubMain();
extern void remoteStubSignal(int,int);
extern void remoteOutput(char *, u32);
extern void remoteSetProtocol(int);
extern void remoteSetPort(int);
extern void debuggerOutput(char *, u32);

extern void CPUUpdateRenderBuffers(bool);

extern void sdlWriteBattery();
extern void sdlReadBattery();

extern void sdlWriteState(int num);
extern void sdlReadState(int num);

extern void sdlRestart(void);

#endif // _VBA_H_

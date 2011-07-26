/*
 * ===========================================================================
 *
 *       Filename:  Event.cpp
 *
 *    Description:  Event-handling code
 *
 *        Version:  1.0
 *        Created:  08/12/2010 05:00:19 PM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL.h>

#include "Controller.h"
#include "Event.h"
#include "VBA.h"
#include "Types.h"
#include "Sound.h"
#include "GLUtil.h"
#include "OptionMenu.h"

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
    "Press key for turbo",
    "Press key for capture",
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
    "Joy0_R",
    "Joy0_Speed",
    "Joy0_Capture"
};
#define NOT_BINDING -1
#define BINDING_DONE ( KEY_BUTTON_CAPTURE + 1 )
static int keyBindingMode = NOT_BINDING;
u16 bindingJoypad[12];

int sdlDefaultJoypad = 0;

int autoFire = 0;
bool autoFireToggle = false;

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

  if ( turbo_toggle )
  {
    if ( down && key == joypad[0][KEY_BUTTON_SPEED] )
      turbo_on = !turbo_on;
  }
  else
    turbo_on = false;
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

void sdlHandleEvent(const SDL_Event& event)
{
  switch(event.type) {
  case SDL_QUIT:
    if( emulating )
    {
        //FIRST thing we do when request to save--make sure we write the battery!
        sdlWriteBattery();
    }
    emulating = false;
    running = false;
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
      }
    }
    break;
  case SDL_MOUSEBUTTONUP:
  case SDL_MOUSEBUTTONDOWN:
  {
    //These are switched and transformed because we're in landscape
    int x = event.button.y;
    int y = destWidth -event.button.x;
    int state = event.button.state;

    controllerEvent ev = controllerHitCheck( x, y );
    applyControllerEvent( ev, state );

    break;
  }
  case SDL_MOUSEMOTION:
  {
    //These are switched and transformed because we're in landscape
    int x = event.motion.y;
    int y = destWidth - event.motion.x;
    int xrel = event.motion.yrel;
    int yrel = -event.motion.xrel;

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
        if ( key == SDLK_EQUALS || key == SDLK_QUESTION )
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
            FILE * f  = fopen( "VisualBoyAdvance.cfg", "w" );

            for ( int i = 0; i < BINDING_DONE; i++ )
            {
                fprintf( f, "%s=%04x\n", bindingCfgNames[i], bindingJoypad[i] );
            }

            fclose( f );

            //make this the current joy
            memcpy( &joypad[0][0], bindingJoypad, 12*sizeof( u16 ) );

            //we're done here!
            keyBindingMode = NOT_BINDING;

        }

        break;
    }
    switch(event.key.keysym.sym) {
//    XXX: bind these to something useful
//    case SDLK_r:
//      if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//         (event.key.keysym.mod & KMOD_CTRL)) {
//        if(emulating) {
//          emulator.emuReset();
//
//          systemScreenMessage("Reset");
//        }
//      }
//      break;
//    case SDLK_b:
//      if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//         (event.key.keysym.mod & KMOD_CTRL)) {
//        if(emulating && emulator.emuReadMemState && rewindMemory
//           && rewindCount) {
//          rewindPos = --rewindPos & 7;
//          emulator.emuReadMemState(&rewindMemory[REWIND_SIZE*rewindPos],
//                                   REWIND_SIZE);
//          rewindCount--;
//          rewindCounter = 0;
//          systemScreenMessage("Rewind");
//        }
//      }
//      break;
//    case SDLK_p:
//      if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//         (event.key.keysym.mod & KMOD_CTRL)) {
//        paused = !paused;
//        SDL_PauseAudio(paused);
//        if(paused)
//          wasPaused = true;
//      }
//      break;
    case SDLK_ESCAPE:
    {
      //make sure we have a save...
      sdlWriteBattery();

      eMenuResponse r = optionsMenu();
      switch ( r )
      {
        default:
          fprintf( stderr, "Unhandled menu response!\n" );
        case MENU_RESPONSE_RESUME:
          break;
        case MENU_RESPONSE_ROMSELECTOR:
          emulating = 0;
          break;
      }
      break;
    }
//    case SDLK_1:
//    case SDLK_2:
//    case SDLK_3:
//    case SDLK_4:
//      if(!(event.key.keysym.mod & MOD_NOALT) &&
//         (event.key.keysym.mod & KMOD_ALT)) {
//        char *disableMessages[4] =
//          { "autofire A disabled",
//            "autofire B disabled",
//            "autofire R disabled",
//            "autofire L disabled"};
//        char *enableMessages[4] =
//          { "autofire A",
//            "autofire B",
//            "autofire R",
//            "autofire L"};
//        int mask = 1 << (event.key.keysym.sym - SDLK_1);
//  if(event.key.keysym.sym > SDLK_2)
//    mask <<= 6;
//        if(autoFire & mask) {
//          autoFire &= ~mask;
//          systemScreenMessage(disableMessages[event.key.keysym.sym - SDLK_1]);
//        } else {
//          autoFire |= mask;
//          systemScreenMessage(enableMessages[event.key.keysym.sym - SDLK_1]);
//        }
//      } if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//           (event.key.keysym.mod & KMOD_CTRL)) {
//        int mask = 0x0100 << (event.key.keysym.sym - SDLK_1);
//        layerSettings ^= mask;
//        layerEnable = DISPCNT & layerSettings;
//        CPUUpdateRenderBuffers(false);
//      }
//      break;
//    case SDLK_5:
//    case SDLK_6:
//    case SDLK_7:
//    case SDLK_8:
//      if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//         (event.key.keysym.mod & KMOD_CTRL)) {
//        int mask = 0x0100 << (event.key.keysym.sym - SDLK_1);
//        layerSettings ^= mask;
//        layerEnable = DISPCNT & layerSettings;
//      }
//      break;
//    case SDLK_n:
//      if(!(event.key.keysym.mod & MOD_NOCTRL) &&
//         (event.key.keysym.mod & KMOD_CTRL)) {
//        if(paused)
//          paused = false;
//        pauseNextFrame = true;
//      }
//      break;
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
    case SDLK_SLASH:
      //toggle skins..
      nextSkin();
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
    case SDLK_QUESTION:
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

void sdlPollEvents()
{
  SDL_Event event;
  while(SDL_PollEvent(&event)) {
    sdlHandleEvent(event);
  }
}

void sdlWaitEvent()
{
  SDL_Event event;
  int result = SDL_WaitEvent(&event);
  if (!result)
    fprintf(stderr, "Ignoring error from SDL_WaitEvent!\n");
  else
    sdlHandleEvent(event);
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

  // If user has 'turbo_toggle' option, turbo is enabled based on the
  // toggle value, not the state of a button.
  if ( turbo_on )
    res |= 1024;
  else if(sdlButtons[which][KEY_BUTTON_SPEED])
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

void displayBindingMessage()
{
  if ( keyBindingMode != NOT_BINDING )
  {
    //make sure the message stays up until binding is over
    systemScreenMessage( bindingNames[keyBindingMode] );
  }
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

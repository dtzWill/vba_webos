/*
 * ===========================================================================
 *
 *       Filename:  Event.h
 *
 *    Description:  Event-handling code
 *
 *        Version:  1.0
 *        Created:  08/12/2010 05:06:08 PM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

#ifndef _EVENT_H_
#define _EVENT_H_

#include "../Types.h"

enum {
  KEY_LEFT, KEY_RIGHT,
  KEY_UP, KEY_DOWN,
  KEY_BUTTON_A, KEY_BUTTON_B,
  KEY_BUTTON_START, KEY_BUTTON_SELECT,
  KEY_BUTTON_L, KEY_BUTTON_R,
  KEY_BUTTON_SPEED, KEY_BUTTON_CAPTURE
};


void sdlPollEvents();
void sdlCheckKeys();
bool systemReadJoypads();
u32 systemReadJoypad(int which);
void displayBindingMessage();
void systemUpdateMotionSensor();

extern u16 joypad[4][12];
extern u16 motion[4];
#endif //_EVENT_H_

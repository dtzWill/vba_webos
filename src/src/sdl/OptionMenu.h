/*
 * ===========================================================================
 *
 *       Filename:  OptionMenu.h
 *
 *    Description:  Options menu
 *
 *        Version:  1.0
 *        Created:  08/12/2010 08:53:10 PM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

#ifndef _OPTIONS_MENU_H_
#define _OPTIONS_MENU_H_

typedef enum menuResponse
{
  MENU_RESPONSE_RESUME,
  MENU_RESPONSE_ROMSELECTOR
} eMenuResponse;

eMenuResponse optionsMenu();

#endif //_OPTIONS_MENU_H_

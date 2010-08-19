/*
 * ===========================================================================
 *
 *       Filename:  convertColor.h
 *
 *    Description:  Convert color
 *
 *        Version:  1.0
 *        Created:  08/13/2010 03:43:52 PM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

#ifndef _CONVERT_COLOR_H_
#define _CONVERT_COLOR_H_

inline u16 convertColor( u16 color )
{
    return color << 1;
}

#endif // _CONVERT_COLOR_H_

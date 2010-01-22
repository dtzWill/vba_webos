/*
 * ===========================================================================
 *
 *       Filename:  controller.h
 *
 *    Description:  Contains constants that are specific to a particular
 *                  controller layout.
 *
 *        Version:  1.0
 *        Created:  01/22/2010 03:46:06 AM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

//Here are x/y/radius for each control element.

#define CONTROLLER_SCREEN_X_OFFSET 124.0f
#define CONTROLLER_SCREEN_Y_OFFSET 65.0f

#define CONTROLLER_SCREEN_WIDTH 280.0f
#define CONTROLLER_SCREEN_HEIGHT 252.0f

//the '320.0f -' is because of the way x is measured here.
//NOTE: these coordinates are measured relative to PORTRAIT orientation
#define JOY_X (320.0f - 171.0f )
#define JOY_Y 62.0f
#define JOY_RADIUS 90.0f
#define JOY_DEAD 15.0f

#define B_X ( 320.0f - 190.0f )
#define B_Y 440.0f
#define B_RADIUS 30.0f

#define A_X ( 320.0f - 120.0f )
#define A_Y 440.0f
#define A_RADIUS 30.0f

#define START_X ( 320.0f - 296.0f )
#define START_Y 88.0f
#define START_RADIUS 20.0f

#define SELECT_X ( 320.0f - 296.0f )
#define SELECT_Y 33.0f
#define SELECT_RADIUS 20.0f

#define L_X ( 320.0f - 37.0f )
#define L_Y 65.0f
#define L_RADIUS 40.0f

#define R_X ( 320.0f - 30.0f )
#define R_Y 440.0f
#define R_RADIUS 40.0f

/*-----------------------------------------------------------------------------
 *  Combinations
 *-----------------------------------------------------------------------------*/
//For now, define AB to be between A and B.
//Started out as me being too lazy to fire up an graphical editor for this
//But actually might be a useful idea :)
#define AB_X ( ( B_X + A_X ) / 2.0f )
#define AB_Y ( ( B_Y + A_Y ) / 2.0f )
//Note: this radius is too big for it to make sense normally--
//but we count on the fact that we check a/b FIRST leaving this zone
//behind/around/what's left over, so this larger radius makes sense in that context
#define AB_RADIUS 30.0f

/*-----------------------------------------------------------------------------
 *  Some helpful macros
 *-----------------------------------------------------------------------------*/
#define HIT( X, Y, C_X, C_Y, C_RADIUS ) \
    ( ( X - C_X ) * ( X - C_X ) + \
      ( Y - C_Y ) * ( Y - C_Y )  \
      < \
      C_RADIUS * C_RADIUS ) 

#define HIT_A( X, Y ) HIT( X, Y, A_X, A_Y, A_RADIUS )
#define HIT_B( X, Y ) HIT( X, Y, B_X, B_Y, B_RADIUS )
#define HIT_AB( X, Y ) HIT( X, Y, AB_X, AB_Y, AB_RADIUS )

#define HIT_START( X, Y ) HIT( X, Y, START_X, START_Y, START_RADIUS )
#define HIT_SELECT( X, Y ) HIT( X, Y, SELECT_X, SELECT_Y, SELECT_RADIUS )
#define HIT_L( X, Y ) HIT( X, Y, L_X, L_Y, L_RADIUS )
#define HIT_R( X, Y ) HIT( X, Y, R_X, R_Y, R_RADIUS )

//d-pad is more complicated...
#define HIT_JOY( X, Y ) \
    HIT( X, Y, JOY_X, JOY_Y, JOY_RADIUS ) && \
    ! HIT( X, Y, JOY_X, JOY_Y, JOY_DEAD )

//These are for portrait mode..
//#define HIT_UP( X, Y ) HIT_JOY( X, Y ) && ( Y < JOY_Y - JOY_DEAD )
//#define HIT_DOWN( X, Y ) HIT_JOY( X, Y ) && ( Y > JOY_Y + JOY_DEAD )
//#define HIT_LEFT( X, Y ) HIT_JOY( X, Y ) && ( X < JOY_X - JOY_DEAD )
//#define HIT_RIGHT( X, Y ) HIT_JOY( X, Y ) && ( X > JOY_X + JOY_DEAD )

//Landscape_r mode...
#define HIT_UP( X, Y ) HIT_JOY( X, Y ) && ( X > JOY_X + JOY_DEAD )
#define HIT_DOWN( X, Y ) HIT_JOY( X, Y ) && ( X < JOY_X - JOY_DEAD )
#define HIT_LEFT( X, Y ) HIT_JOY( X, Y ) && ( Y < JOY_Y - JOY_DEAD )
#define HIT_RIGHT( X, Y ) HIT_JOY( X, Y ) && ( Y > JOY_Y + JOY_DEAD )

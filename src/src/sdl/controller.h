/*
 * ===========================================================================
 *
 *       Filename:  controller.h
 *
 *    Description:  Contains constants that are specific to a particular
 *                  controller layout.
 *
 *        Version:  1.1
 *        Created:  01/22/2010 03:46:06 AM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

//Here are x/y/radius for each control element.

int CONTROLLER_SCREEN_X_OFFSET = 121;
int CONTROLLER_SCREEN_Y_OFFSET = 0;

int CONTROLLER_SCREEN_WIDTH = 240;
int CONTROLLER_SCREEN_HEIGHT = 216;

//the '320 -' is because of the way x is measured here.
//NOTE: these coordinates are measured relative to PORTRAIT orientation
int JOY_X = (320 - 145 );
int JOY_Y = 62;
int JOY_RADIUS = 70;
int JOY_DEAD = 25;

int B_X = ( 320 - 263 );
int B_Y = 359;
int B_RADIUS = 40;

int A_X = ( 320 - 228 );
int A_Y = 434;
int A_RADIUS = 40;

int START_X = ( 320 - 293 );
int START_Y = 88;
int START_RADIUS = 20;

int SELECT_X = ( 320 - 293 );
int SELECT_Y = 43;
int SELECT_RADIUS = 20;

int L_X = ( 320 - 34 );
int L_Y = 62;
int L_RADIUS = 40;

int R_X = ( 320 - 34 );
int R_Y = 422;
int R_RADIUS = 40;

int TURBO_X = 10000;
int TURBO_Y = 10000;
int TURBO_RADIUS = 0;

int CAPTURE_X = 10000;
int CAPTURE_Y = 10000;
int CAPTURE_RADIUS = 0;

/*-----------------------------------------------------------------------------
 *  Combinations
 *-----------------------------------------------------------------------------*/
//For now, define AB to be between A and B.
//Started out as me being too lazy to fire up an graphical editor for this
//But actually might be a useful idea :)
int AB_X = ( ( B_X + A_X ) / 2.0f );
int AB_Y = ( ( B_Y + A_Y ) / 2.0f );
//Note: this radius is too big for it to make sense normally--
//but we count on the fact that we check a/b FIRST leaving this zone
//behind/around/what's left over, so this larger radius makes sense in that context
int AB_RADIUS = 30;

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

#define HIT_TURBO( X, Y ) HIT( X, Y, TURBO_X, TURBO_Y, TURBO_RADIUS )
#define HIT_CAPTURE( X, Y ) HIT( X, Y, CAPTURE_X, CAPTURE_Y, CAPTURE_RADIUS )

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

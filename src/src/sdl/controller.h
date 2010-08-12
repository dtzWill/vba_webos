/*
 * ===========================================================================
 *
 *       Filename:  controller.h
 *
 *    Description:  Contains types and functions regarding controller skins
 *
 *        Version:  2.0
 *        Created:  01/22/2010 03:46:06 AM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "options.h"

//No need to pull in a math library for this...
#define SIN_30 ( 0.5f )
#define SIN_60 ( 0.866025404f )
#define COS_30 SIN_60
#define COS_60 SIN_30

struct controller_skin;
typedef struct controller_skin controller_skin;

struct controller_skin
{
char * name;
char * image_path;

int controller_screen_x_offset;
int controller_screen_y_offset;

int controller_screen_width;
int controller_screen_height;

int joy_x;
int joy_y;
int joy_radius;
int joy_dead;

int b_x;
int b_y;
int b_radius;

int a_x;
int a_y;
int a_radius;

int start_x;
int start_y;
int start_radius;

int select_x;
int select_y;
int select_radius;

int l_x;
int l_y;
int l_radius;

int r_x;
int r_y;
int r_radius;

int turbo_x;
int turbo_y;
int turbo_radius;

int capture_x;
int capture_y;
int capture_radius;

int ab_x;
int ab_y;
int ab_radius;

//circular linked list
controller_skin * next;

//end struct definition
};

/*-----------------------------------------------------------------------------
 *  Some helpful macros
 *-----------------------------------------------------------------------------*/
static bool hit( int x, int y, int c_x, int c_y, int c_radius )
{
    return
        ( x - c_x ) * ( x - c_x ) +
        ( y - c_y ) * ( y - c_y )
        <
        c_radius * c_radius;
}


static bool hit_a( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->a_x, skin->a_y, skin->a_radius );
}

static bool hit_b( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->b_x, skin->b_y, skin->b_radius );
}

static bool hit_ab( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->ab_x, skin->ab_y, skin->ab_radius );
}

static bool hit_start( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->start_x, skin->start_y, skin->start_radius );
}

static bool hit_select( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->select_x, skin->select_y, skin->select_radius );
}

static bool hit_l( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->l_x, skin->l_y, skin->l_radius );
}

static bool hit_r( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->r_x, skin->r_y, skin->r_radius );
}

static bool hit_turbo( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->turbo_x, skin->turbo_y, skin->turbo_radius );
}

static bool hit_capture( controller_skin * skin, int x, int y )
{
    return hit( x, y, skin->capture_x, skin->capture_y, skin->capture_radius );
}

//D-pad
static bool hit_joy( controller_skin * skin, int x, int y )
{
    //We hit the joy if we're within the larger radius,
    //but outside the dead zone circle
    return
        hit( x, y, skin->joy_x, skin->joy_y, skin->joy_radius ) &&
        ! hit( x, y, skin->joy_x, skin->joy_y, skin->joy_dead );
}

//Landscape_r mode...
static bool hit_up( controller_skin * skin, int x, int y )
{
    int relx = ( x - skin->joy_x );
    if ( relx < 0 ) relx = -relx;
    return
        hit_joy( skin, x, y ) &&
        ( y < skin->joy_y - relx * SIN_30 );
}
static bool hit_down( controller_skin * skin, int x, int y )
{
    int relx = ( x - skin->joy_x );
    if ( relx < 0 ) relx = -relx;
    return
        hit_joy( skin, x, y ) &&
        ( y > skin->joy_y + relx * SIN_30 );
}
static bool hit_left( controller_skin * skin, int x, int y )
{
    int rely = ( y - skin->joy_y );
    if ( rely < 0 ) rely = -rely;
    return
        hit_joy( skin, x, y ) &&
        ( x < skin->joy_x - rely * COS_30 );
}
static bool hit_right( controller_skin * skin, int x, int y )
{
    int rely = ( y - skin->joy_y );
    if ( rely < 0 ) rely = -rely;
    return
        hit_joy( skin, x, y ) &&
        ( x > skin->joy_x + rely * COS_30 );
}

controller_skin tmp_skin;

vba_option controller_options[] =
{
    //Controller stuff
    { "touch_screen_x_offset", &tmp_skin.controller_screen_x_offset },
    { "touch_screen_y_offset", &tmp_skin.controller_screen_y_offset },
    { "touch_screen_width", &tmp_skin.controller_screen_width },
    { "touch_screen_height", &tmp_skin.controller_screen_height },
    { "touch_joy_x", &tmp_skin.joy_x },
    { "touch_joy_y", &tmp_skin.joy_y },
    { "touch_joy_radius", &tmp_skin.joy_radius },
    { "touch_joy_deadzone", &tmp_skin.joy_dead },
    { "touch_b_x", &tmp_skin.b_x },
    { "touch_b_y", &tmp_skin.b_y },
    { "touch_b_radius", &tmp_skin.b_radius },
    { "touch_a_x", &tmp_skin.a_x },
    { "touch_a_y", &tmp_skin.a_y },
    { "touch_a_radius", &tmp_skin.a_radius },
    { "touch_start_x", &tmp_skin.start_x },
    { "touch_start_y", &tmp_skin.start_y },
    { "touch_start_radius", &tmp_skin.start_radius },
    { "touch_select_x", &tmp_skin.select_x },
    { "touch_select_y", &tmp_skin.select_y },
    { "touch_select_radius", &tmp_skin.select_radius },
    { "touch_l_x", &tmp_skin.l_x },
    { "touch_l_y", &tmp_skin.l_y },
    { "touch_l_radius", &tmp_skin.l_radius },
    { "touch_r_x", &tmp_skin.r_x },
    { "touch_r_y", &tmp_skin.r_y },
    { "touch_r_radius", &tmp_skin.r_radius },
    { "touch_ab_x", &tmp_skin.ab_x },
    { "touch_ab_y", &tmp_skin.ab_y },
    { "touch_ab_radius", &tmp_skin.ab_radius },
    { "touch_turbo_x", &tmp_skin.turbo_x },
    { "touch_turbo_y", &tmp_skin.turbo_y },
    { "touch_turbo_radius", &tmp_skin.turbo_radius },
    { "touch_capture_x", &tmp_skin.capture_x },
    { "touch_capture_y", &tmp_skin.capture_y },
    { "touch_capture_radius", &tmp_skin.capture_radius }
};

/*-----------------------------------------------------------------------------
 *  Skin loading
 *-----------------------------------------------------------------------------*/

//Returns the skin name w/o the leading period, doesn't allocate anything
char * strip_leading_period( char * str )
{
    char * ret = str; 

    if( ret && *ret == '.' ) ret++;

    return ret;
}

void load_skin( char * skin_cfg, char * skin_img, char * skin_name, char * skin_folder, controller_skin ** skin )
{
    bool success = false;

    if ( !skin_cfg || !skin_img || !skin_name || !skin_folder || !skin )
    {
        //Come on now, call this correctly :)
        return;
    }

    char full_skin_cfg[strlen( skin_folder ) + strlen( skin_cfg ) + 2];
    char full_skin_img[strlen( skin_folder ) + strlen( skin_img ) + 2];
    strcpy( full_skin_cfg, skin_folder );
    strcpy( full_skin_cfg + strlen( skin_folder ), "/" );
    strcpy( full_skin_cfg + strlen( skin_folder ) + 1, skin_cfg );

    strcpy( full_skin_img, skin_folder );
    strcpy( full_skin_img + strlen( skin_folder ), "/" );
    strcpy( full_skin_img + strlen( skin_folder ) + 1, skin_img );

    int all_options = sizeof( controller_options ) / sizeof ( vba_option );
    int options_read = readOptions(
            full_skin_cfg,
            controller_options,
            all_options,
            false );
    if ( options_read != all_options )
    {
        fprintf( stderr, "Error reading in %s! Read %d/%d\n", full_skin_cfg, options_read, all_options );
        return;
    }

    //Before adding to list of skins, verify we don't have this one already
    if ( *skin )
    {
        controller_skin * cur = *skin;
        do
        {
            fprintf( stderr, "%s vs %s\n", cur->name, skin_name );
            bool duplicate = !strcmp(
                strip_leading_period(cur->name),
                strip_leading_period(skin_name));
            if ( duplicate )
            {
                fprintf( stderr, "Duplicate skin \"%s\" found, skipping!\n", skin_name );
                return;
            }
            cur = cur->next;
        } while (cur != *skin);
    }

    //Well we read it in, let's add it to the list.
    controller_skin * newskin = (controller_skin *)malloc( sizeof( controller_skin ) );
    memcpy( newskin, &tmp_skin, sizeof( controller_skin ) );
    //populate non-option fields...
    newskin->name = strdup( skin_name );
    newskin->image_path = strdup( full_skin_img );
    if ( *skin == NULL )
    {
        //Okay so our skin list is empty.
        *skin = newskin;
        //loop back, circular linked list
        (*skin)->next = newskin;
    }
    else
    {
        //We have a list of length >= 1
        //So we insert this one at the end, and set the 'skin' pointer to the end of the list.
        controller_skin * next = (*skin)->next;
        (*skin)->next = newskin;
        newskin->next = next;
        *skin = newskin;
    }

    printf( "Loaded skin %s\n", skin_name );
}

#endif //_CONTROLLER_H_

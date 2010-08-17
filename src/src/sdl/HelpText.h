/*
 * ===========================================================================
 *
 *       Filename:  HelpText.h
 *
 *    Description:  Text for the help screens...
 *                  Meant to be #include'd only in OptionMenu
 *
 *        Version:  1.0
 *        Created:  08/17/2010 02:10:01 AM
 *
 *         Author:  Will Dietz (WD), w@wdtz.org
 *        Company:  dtzTech
 *
 * ===========================================================================
 */


typedef struct
{
  char * msg;
  SDL_Color color;
} line;

// "14" lines is what it takes
// to fill the screen nicely at current resolution
// and current font size.

line helpROMs[][14] =
{
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
},
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
},
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
}
};

line helpControls[][14] =
{
    {"How do I play?",                              textColor },
    {" ",                                           textColor },
    {"VBA initially gives you onscreen controls.",  textColor },
    {"You can also use the physical keyboard.",     textColor },
    {"W/A/S/D --- up/down/left/right (d-pad)",      textColor },
    {"K/L --- B/A (action buttons)",                textColor },
    {"Q/P --- L/R (trigger buttons) folder called", textColor },
    {" ",                                           textColor },
    {" ",                                           textColor },
    {" ",                                           textColor },
    {" ",                                           textColor },
    {" ",                                           textColor },
    {" ",                                           textColor },
    {"(Click to go to next screen)",                linkColor }
};

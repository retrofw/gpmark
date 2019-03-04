#ifndef MAIN_H
#define MAIN_H

//#define ProjectWin
//#define ProjectCaanoo
//#define ProjectWiz

#define ProjectGP2X
//#define ProjectDingoo

#define ScreenWidth 320
#define ScreenHeight 240
#define ScreenSize (ScreenWidth * ScreenHeight)

#define pi 3.14151693
#define d2r (180.0/pi)

enum {MENU, BENCH, DESC, RES};

enum {TEST_BLITTING, TEST_PLASMA, TEST_ROTOZOOMER, TEST_ROTOZOOMER_NEAR, TEST_ROTOZOOMER_FAR, TEST_RADIALBLUR, TEST_BUNNY3D, NUM_TESTS};

#ifdef ProjectCaanoo
    #define BUTTON_A    0
    #define BUTTON_HOME 6
#endif

#if defined ProjectWiz || defined ProjectGP2X
    #define BUTTON_A    12
    #define BUTTON_HOME 8
#endif

void ClearScreen(unsigned short *vram);

#endif

#include <SDL/SDL.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "main.h"
#include "bitfonts.h"
#include "menu.h"

#include "blitting.h"
#include "plasma.h"
#include "rotozoomer.h"
#include "radialblur.h"
#include "bunny3d.h"
#include "render3d.h"

#ifndef SDL_TRIPLEBUF
#define SDL_TRIPLEBUF SDL_DOUBLEBUF
#endif

#if defined ProjectCaanoo || defined ProjectWiz || defined ProjectGP2X
    #include <unistd.h>
#endif

#ifdef ProjectWin
    #undef main
#endif

SDL_Surface *screen = NULL;
SDL_Joystick *joy = NULL;

int fps = 0, pframe = 0, nframe = 0, atime = 0;
int frametime = 0;
int quit = 0;
int desc_next = 0;

int where_is = MENU;

// -------------------------------------

char sbuffer[256];
unsigned char fonts[59*64];

void InitFonts()
{
    int i = 0;
	for (int n=0; n<59; n++)
	{
		for (int y=0; y<8; y++)
		{
			int c = bitfonts[i++];
			for (int x=0; x<8; x++)
			{
				fonts[(n << 6) + x + (y<<3)] = ((c >>  (7 - x)) & 1) * 255;
			}
		}
	}
}

void DrawFontX2(int xp, int yp, int ch, unsigned short *vram)
{
    if (xp <0 || xp > 312) return;
    vram += xp + yp * ScreenWidth;
    for (int y=0; y<8; y++)
    {
        int yc = yp + y;
        if ((yc>=1) && (yc<ScreenHeight - 1))
        {
            int yi = y << 3;
            for (int x=0; x<8; x++)
            {
                unsigned char c = fonts[(ch << 6) + yi + x] * 0x01010101;
                *vram |= c;
                *(vram+1) |= c;
                *(vram+ScreenWidth) |= c;
                *(vram+ScreenWidth+1) |= c;
                vram+=2;
            }
            vram-=16;
        }
        vram+=2*ScreenWidth;
    }
}

void DrawFont(int xp, int yp, int ch, unsigned short *vram)
{
    if (xp <0 || xp > 312) return;
    vram += xp + yp * ScreenWidth;
    for (int y=0; y<8; y++)
    {
        int yc = yp + y;
        if ((yc>=1) && (yc<ScreenHeight - 1))
        {
            int yi = y << 3;
            for (int x=0; x<8; x++)
            {
                *vram++ |= (fonts[(ch << 6) + yi + x] * 0x01010101);
            }
            vram-=8;
        }
        vram+=ScreenWidth;
    }
}

void DrawText(int xtp, int ytp, int cn, bool zoom, char *text, unsigned short *vram)
{
	for (int n = 0; n<cn; n++)
	{
		char c = *text++;
        if (c>96 && c<123) c-=32;

        if (!zoom)
        {
    		if (c>31 && c<92) DrawFont(xtp, ytp, c - 32, vram);
    			else if (c==0) n = cn;
    		xtp+=8; if (xtp>ScreenWidth -8 -1) n = cn;
        }
        else
        {
    		if (c>31 && c<92) DrawFontX2(xtp, ytp, c - 32, vram);
    			else if (c==0) n = cn;
    		xtp+=16; if (xtp>ScreenWidth -16 -1) n = cn;
        }
	}
}

void ShowFPS(unsigned short *vram)
{
    if (SDL_GetTicks() - atime >= 1000)
    {
        atime = SDL_GetTicks();
        fps = nframe - pframe;
        pframe = nframe;
    }
	sprintf(sbuffer, "FPS = %d", fps);
    DrawText(8, 8, 16, false, sbuffer, vram);
}

void ClearScreen(unsigned short *vram)
{
    memset(vram, 0, 2*ScreenSize);
}

void speeddown(int ticks)
{
    do{}while(SDL_GetTicks() - frametime < ticks);
    frametime = SDL_GetTicks();
}

// -------------------------------------

void InitEffects()
{
    InitBlitting();
    InitPlasma();
    InitRotozoomer();
    InitRadialblur();

    initRender3D();
    InitBunny3D();
}

void mmu_hack()
{
    #if defined ProjectGP2X
        int mmufd;
        system("/sbin/rmmod mmuhack");
        system("/sbin/insmod mmuhack.o");
        mmufd = open("/dev/mmuhack", O_RDWR);
        close(mmufd);
    #endif
}

void mmu_unhack()
{
    #if defined ProjectGP2X
        system("/sbin/rmmod mmuhack");
    #endif
}

void Init()
{
    mmu_hack();
    InitFonts();
    InitEffects();

    SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
	SDL_ShowCursor(SDL_DISABLE);
    // if (SDL_NumJoysticks() > 0) joy = SDL_JoystickOpen(0);
    screen = SDL_SetVideoMode (ScreenWidth, ScreenHeight, 16, SDL_SWSURFACE);
}

void KeyCommands()
{
	SDL_Event event;

	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
            case SDL_KEYDOWN:
    			if (event.key.keysym.sym==SDLK_ESCAPE)
                {
                    if (where_is==RES)
        				quit = 1;
                    else if (where_is==BENCH)
                        where_is = RES;
                }
                if (event.key.keysym.sym==SDLK_SPACE)
                    if (where_is==DESC)
                        desc_next = 1;

                if (event.key.keysym.sym==SDLK_RETURN)
                    quit = 1;
			break;

            case SDL_QUIT:
				quit = 1;
			break;

            case SDL_JOYBUTTONDOWN:
    			if ( event.jbutton.button == BUTTON_HOME )
    			{
    				if (where_is==RES)
    					quit = 1;
    				else if (where_is==BENCH)
    					where_is = RES;
    			}

    			if (event.jbutton.button==BUTTON_A)
    				if (where_is==DESC)
    					desc_next = 1;
    			break;
            default:
               break;
        }
	}
}

// -------------------------------------

char* test_name[NUM_TESTS];
char* test_description[NUM_TESTS];
int total_frames[NUM_TESTS];
int start_frames[NUM_TESTS+1];
float test_fps[NUM_TESTS];

int test_time_start;

void InitBench()
{
    nframe = 0;

    test_name[TEST_BLITTING] = "Blitting Test";
    test_description[TEST_BLITTING] = "A simple memset blitting test. Tests how fast it is to write stuff to the video ram.";
    total_frames[TEST_BLITTING] = 3000;

    test_name[TEST_PLASMA] = "Plasma";
    test_description[TEST_PLASMA] = "A lightweight demo effect. Addition of integer sine values read from small LUTs.";
    total_frames[TEST_PLASMA] = 3000;

    test_name[TEST_ROTOZOOMER] = "Rotozoomer";
    test_description[TEST_ROTOZOOMER] = "Rotozoomer test 1. Zooming in and out.";
    total_frames[TEST_ROTOZOOMER] = 2000;

    test_name[TEST_ROTOZOOMER_NEAR] = "Rotozoomer Near";
    test_description[TEST_ROTOZOOMER_NEAR] = "Rotozoomer test 2. Close up, optimum speed, cache does not affect.";
    total_frames[TEST_ROTOZOOMER_NEAR] = 2500;

    test_name[TEST_ROTOZOOMER_FAR] = "Rotozoomer Far";
    test_description[TEST_ROTOZOOMER_FAR] = "Rotozoomer test 3. Far in the distance. Slowest version because of cache misses. Good tests for cache performance.";
    total_frames[TEST_ROTOZOOMER_FAR] = 1500;

    test_name[TEST_RADIALBLUR] = "Radial Blur";
    test_description[TEST_RADIALBLUR] = "A true RGB effect with lot's of reads and huge arrays. The way this effect was written is not cache friendly and that could be also a good test for cache or memory read performance.";
    total_frames[TEST_RADIALBLUR] = 500;

    test_name[TEST_BUNNY3D] = "3D Bunny";
    test_description[TEST_BUNNY3D] = "This is a massive model (69451 polygons) and a total overkill for most handhelds. Also, this is software rendering.";
    total_frames[TEST_BUNNY3D] = 80;

    int sum_frames = 0;
    int i = 0;
    for (i=0; i<NUM_TESTS; i++)
    {
        test_fps[i] = 0;
        start_frames[i] = sum_frames;
        sum_frames += total_frames[i];
    }
    start_frames[i] = sum_frames;
}

void RasterScreen(int ntime, int sr, int sg, int sb, unsigned short *vram)
{
    int yp;
	for (int y=0; y<ScreenHeight; y++)
	{
        if ((y & 15) == 0) yp = (int)(sin((y+ntime)/15.0f)*cos((y-(ntime >> 1))/63.0f)*15.0f + 16.0f);
	    for (int x=0; x<ScreenWidth; x++)
		{
			*vram++ = ((yp>>sr)<<11) | (((yp << 1)>>sg) << 5) | (yp>>sb);
		}
	}
}

void SquaresScreen(int ntime, unsigned short *vram)
{
	int t = ntime >> 1;

	for (int y=0; y<ScreenHeight; y++)
	{
	    for (int x=0; x<ScreenWidth; x++)
		{
            int c = (((x - t) & 15)==0 || ((y - t) & 15)==0) * 15;
			*vram++ = ((c>>3)<<11) | (((c << 1)>>2) << 5) | (c>>1);
		}
	}
}

void RenderTestDescription(int test_num, int ntime, unsigned short *vram)
{
	sprintf(sbuffer, test_name[test_num]);
    DrawText(8, 8, 16, true, sbuffer, vram);

	sprintf(sbuffer, test_description[test_num]);
    DrawText(384 - 4 * ntime, 160, 256, false, sbuffer, vram);
}

void DrawBar(int char_y, float percentage, unsigned short *vram)
{
    int xp = (int)((ScreenWidth - 4) * percentage);

    unsigned short cbar = (7 << 11) | (31 << 5) | 15;

    for (int y=0; y<9; y++)
    {
        int yi = char_y * 11 + y + 2;
        for (int x=0; x<=xp; x++)
        {
            int xi = 2 + x;
            int i = yi * ScreenWidth + xi;
            *(vram + i) |= cbar;
        }
    }
}

void RenderTestResults(int ntime, float res_max, unsigned short *vram)
{
    int tmaxframe = 64;
    float tscale = (float)ntime / float(tmaxframe);
    if (tscale > 1.0f) tscale = 1.0f;

    for (int i=0; i<NUM_TESTS; i++)
    {
    	sprintf(sbuffer, "%s", test_name[i]);
        DrawText(8, i * 11 + 3, 32, false, sbuffer, vram);

    	sprintf(sbuffer, "%.1f", test_fps[i]);
        DrawText(256, i * 11 + 3, 32, false, sbuffer, vram);

        DrawBar(i, (test_fps[i] / res_max) * tscale, vram);
    }
}

void show_test_description(int test_num, unsigned short *vram)
{
    where_is = DESC;

    int tframe = 0;
    desc_next = 0;

    srand(SDL_GetTicks());

    int sr = (rand() % 2) + 1;
    int sg = (rand() % 2) + 1;
    int sb = (rand() % 2) + 1;

    int l = 2 * strlen(test_description[test_num]) + 2 * 56;

    while (!desc_next && tframe < l)
    {
        RasterScreen(tframe, sr, sg, sb, vram);
        RenderTestDescription(test_num, tframe, vram);
        SDL_Flip(screen);
        speeddown(20);
        KeyCommands();
        tframe++;
    }
}

float find_max_result()
{
    float max = 0.0f;
    for (int i=0; i<NUM_TESTS; i++)
    {
        if (test_fps[i] > max) max = test_fps[i];
    }

    return max;
}

void show_test_results(unsigned short *vram)
{
    screen = SDL_SetVideoMode (ScreenWidth, ScreenHeight, 16, SDL_HWSURFACE | SDL_TRIPLEBUF);
    SDL_Surface *res = SDL_ConvertSurface(screen, screen->format, screen->flags);
    vram = (unsigned short*)res->pixels;

    where_is = RES;

    int tframe = 0;

    float fmax = find_max_result();

    while (!quit)
    {
        SquaresScreen(tframe, vram);
        RenderTestResults(tframe, fmax, vram);
        SDL_BlitSurface(res, NULL, screen, NULL);
        SDL_Flip(screen);
        speeddown(20);
        KeyCommands();
        tframe++;
    }

}


void RunBenchScript(int ntime, unsigned short *vram)
{
    for (int i=0; i<=NUM_TESTS; i++)
    {
        if (nframe == start_frames[i])
        {
            if (i<NUM_TESTS)
                show_test_description(i, vram);
            test_time_start = SDL_GetTicks();
        }

        if (nframe == start_frames[i]-1)
            test_fps[i-1] = (float)total_frames[i-1] / ((float)(SDL_GetTicks() - test_time_start) / 1000.0f);
    }

    if (where_is==RES)
        show_test_results(vram);

    if (nframe >= start_frames[TEST_BLITTING] && nframe < start_frames[TEST_PLASMA])
    {
        RunBlitting(ntime, vram);
    }
    else if (nframe >= start_frames[TEST_PLASMA] && nframe < start_frames[TEST_ROTOZOOMER])
    {
        RunPlasma(ntime, vram);
    }
    else if (nframe >= start_frames[TEST_ROTOZOOMER] && nframe < start_frames[TEST_ROTOZOOMER_NEAR])
    {
        RunRotozoomerNormal(ntime, vram);
    }
    else if (nframe >= start_frames[TEST_ROTOZOOMER_NEAR] && nframe < start_frames[TEST_ROTOZOOMER_FAR])
    {
        RunRotozoomerNear(ntime, vram);
    }
    else if (nframe >= start_frames[TEST_ROTOZOOMER_FAR] && nframe < start_frames[TEST_RADIALBLUR])
    {
        RunRotozoomerFar(ntime, vram);
    }
    else if (nframe >= start_frames[TEST_RADIALBLUR] && nframe < start_frames[TEST_BUNNY3D])
    {
        RunRadialblur(ntime, vram);
    }
    else if (nframe >= start_frames[TEST_BUNNY3D] && nframe < start_frames[NUM_TESTS])
    {
        RunBunny3D(ntime, vram);
    }
    else
    {
        show_test_results(vram);
    }
}

void RunBench()
{
	while (!quit)
	{
        where_is = BENCH;
        KeyCommands();

        unsigned short *vram = (unsigned short*)screen->pixels;
        RunBenchScript(nframe, vram);

        ShowFPS(vram);

        SDL_Flip(screen);
        nframe++;
	}
}

void BackToSystem()
{
    mmu_unhack();

    if (where_is == RES) {
        system("fbgrab $HOME/gpmark.png");
    }

    #if defined ProjectCaanoo || defined ProjectWiz || defined ProjectGP2X
    	SDL_Quit();
    	chdir("/usr/gp2x");
    	execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
    #endif
}

int main (int argc, char *argv[])
{
    Init();

    menu();

    InitBench();
    RunBench();

    BackToSystem();
}

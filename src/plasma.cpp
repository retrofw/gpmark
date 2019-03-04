#include <math.h>
#include "main.h"

int fsin1[4096],fsin2[4096],fsin3[4096];
unsigned short rgb[256];

void InitPlasma()
{
	float l = 4.0;
	for (int i=0; i<4096; i++)
	{
		fsin1[i] = (int)(sin(i/(l*15.0))*96.0+96.0);
		fsin2[i] = (int)(sin(i/(l*20.0))*112.0+112.0);
		fsin3[i] = (int)(sin(i/(l*35.0))*128.0+128.0);
	}

	for (int i=0; i<64; i++)
	{
        rgb[i] = ((i>>1)<<11) | ((i>>1)<<5);
	    rgb[64+i] = ((31-(i>>2))<<11) | (((i>>1)+32)<<5) | (i>>1);
	    rgb[128+i] = ((16-(i>>2))<<11) | ((63-(i>>1))<<5) | 31;
	    rgb[192+i] = ((31-(i>>1))<<5) | (31-(i>>1));
	}
}

void RunPlasma(int ntime, unsigned short *vram)
{
 	int k1 = ntime;
    int k3 = 3 * ntime;
 	if (k1>503) k1 = k1-(k1/503)*503;
 	if (k3>880) k3 = k3-(k3/880)*880;

	for (int y=0; y<ScreenHeight; y++)
	{
		for (int x=0; x<ScreenWidth; x++)
		{
			unsigned char c = fsin1[x] + fsin2[y+k1] + fsin3[x+y+k3];
			*vram++ = rgb[c];
		}
	}
}

#include <math.h>
#include <stdio.h>
#include "main.h"

unsigned short draculin[128 * 128];

void InitRotozoomer()
{
	unsigned short dracol[256];

	FILE *dracdata;
	dracdata=fopen("draculf.bin","rb");

	int x0=fgetc(dracdata);
	int y0=fgetc(dracdata);

	int r,g,b;
	for (int i=0;i<256;i++)
		dracol[i]=(((fgetc(dracdata)>>1)<<11) | (fgetc(dracdata)<<5) | (fgetc(dracdata))>>1);

	for (int y=0;y<y0;y++)
		for (int x=0;x<x0;x++)
			draculin[(y<<7) + x]=dracol[fgetc(dracdata)];

	fclose(dracdata);
}

void RunRotozoomer(float rot, float zoom, unsigned short *vram)
{
	int fp = 16;

	int mx = 0, my = 0, mmx = 50<<fp, mmy = 20<<fp;
	int dx = (int)((cos(rot/d2r)*zoom) * pow(2,fp));
	int dy = (int)((sin(rot/d2r)*zoom) * pow(2,fp));

    int x,y;
	unsigned short c;

    for (y=0; y<ScreenHeight; y++)
    {
        mmx = mmx - dy;
        mmy = mmy + dx;
        mx = mmx;
        my = mmy;
        for (x=0; x<ScreenWidth; x++)
        {
            mx = mx + dx;
            my = my + dy;
            c = draculin[((mx>>fp)&127) + (((my>>fp)&127) << 7)];
            *vram++=c;
        }
    }
}

void RunRotozoomerNormal(int ntime, unsigned short *vram)
{
    float rot = sin(ntime/478.0f) * 280.0f;
    float zoom = sin(ntime/227.0f)*0.75f + 0.76f;

    RunRotozoomer(rot, zoom, vram);
}

void RunRotozoomerNear(int ntime, unsigned short *vram)
{
    float rot = sin(ntime/478.0f) * 280.0f;
    float zoom = 0.05f;

    RunRotozoomer(rot, zoom, vram);
}

void RunRotozoomerFar(int ntime, unsigned short *vram)
{
    float rot = sin(ntime/478.0f) * 280.0f;
    float zoom = 8.0f;

    RunRotozoomer(rot, zoom, vram);
}

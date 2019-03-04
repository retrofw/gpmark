#include "main.h"

void InitBlitting()
{
}

void RunBlitting(int ntime, unsigned short *vram)
{
    unsigned char cc = (ntime & 255) >> 3;
    unsigned short c = (cc << 11) | ((cc << 1) << 5) | cc;

    for (int i=0; i<ScreenSize; i++)
    {
		*vram++ = c;
	}
}

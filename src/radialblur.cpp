#include <math.h>

#include "main.h"
#include "sky1.h"

unsigned char rbmp[ScreenSize];
unsigned char gbmp[ScreenSize];
unsigned char bbmp[ScreenSize];

unsigned char rbuffer[ScreenSize];
unsigned char gbuffer[ScreenSize];
unsigned char bbuffer[ScreenSize];

short radir0[ScreenSize];
short radir1[ScreenSize];
short radir2[ScreenSize];

short radir3[ScreenSize];
short radir4[ScreenSize];
short radir5[ScreenSize];

void InitRadialBitmap1()
{
    int x, y, i = 0;
    unsigned short c;
    for (y=0; y<ScreenHeight; y++)
    {
        for (x=0; x<ScreenWidth; x ++)
        {
            c = sky1[(x&255) + (((ScreenHeight-1)-y)<<8)];
            rbmp[i] = ((c>>11) & 31) << 3;
            gbmp[i] = ((c>>5) & 63) << 2;
            bbmp[i] = (c & 31) << 3;
            i++;
        }
    }
}

void InitRadialblur()
{
    InitRadialBitmap1();

    float rb_shortness = 4.0f;

    int xi, yi;
    int i=0;
    int x, y;
    for (y=0; y<ScreenHeight; y++)
    {
        for (x=0; x<ScreenWidth; x++)
        {
            xi = (int)(-(x/rb_shortness));
            yi = (int)(-(y/rb_shortness));
            radir0[i] = (xi>>1) + (yi>>1)*ScreenWidth;
            radir1[i] = (xi>>2) + (yi>>2)*ScreenWidth;
            radir2[i] = (xi>>3) + (yi>>3)*ScreenWidth;
            i++;
        }
    }

    i=0;
    for (y=0; y<ScreenHeight; y++)
    {
        for (x=0; x<ScreenWidth; x++)
        {
            xi = (int)(-(x/rb_shortness));
            yi = (int)(y/rb_shortness);
            radir3[i] = (xi>>1) + (yi>>1)*ScreenWidth;
            radir4[i] = (xi>>2) + (yi>>2)*ScreenWidth;
            radir5[i] = (xi>>3) + (yi>>3)*ScreenWidth;
            i++;
        }
    }
}


void InitRadialBitmapCopy()
{
    int i;
    unsigned int *rbuffer32 = (unsigned int*)rbuffer, *gbuffer32 = (unsigned int*)gbuffer, *bbuffer32 = (unsigned int*)bbuffer;
	for (i=0; i<ScreenSize; i+=4)
	{
        *rbuffer32++ = rbmp[i] | (rbmp[i+1]<<8) | (rbmp[i+2]<<16) | (rbmp[i+3]<<24);
        *gbuffer32++ = gbmp[i] | (gbmp[i+1]<<8) | (gbmp[i+2]<<16) | (gbmp[i+3]<<24);
        *bbuffer32++ = bbmp[i] | (bbmp[i+1]<<8) | (bbmp[i+2]<<16) | (bbmp[i+3]<<24);
    }
}

void Radial(int ntime, unsigned short *vram)
{
	int i,ri;

    int dcx = (int)(sin((float)ntime/23.0f)*(ScreenWidth>>1));
    int dcy = (int)(sin((float)ntime/32.0f)*(ScreenHeight>>1));
    int dci = dcx + dcy*ScreenWidth;

    InitRadialBitmapCopy();

    unsigned short *avram;

// 1o tetarthmorio

 	avram=(unsigned short*)vram + (ScreenWidth/2-1) + (ScreenHeight/2-1)*ScreenWidth + dci;

	i=(ScreenWidth/2-1) + (ScreenHeight/2-1)*ScreenWidth + dci;
	ri=0;
      for (int y=ScreenHeight/2-1+dcy; y>=0; y--)
      {
            for (int x=ScreenWidth/2-1+dcx; x>=0; x--)
            {

                rbuffer[i] = (rbuffer[i] + rbuffer[i - radir0[ri]] + rbuffer[i - radir1[ri]] + rbuffer[i - radir2[ri]])>>2;
                gbuffer[i] = (gbuffer[i] + gbuffer[i - radir0[ri]] + gbuffer[i - radir1[ri]] + gbuffer[i - radir2[ri]])>>2;
                bbuffer[i] = (bbuffer[i] + bbuffer[i - radir0[ri]] + bbuffer[i - radir1[ri]] + bbuffer[i - radir2[ri]])>>2;
                *avram-- = ((rbuffer[i]>>3)<<11) | ((gbuffer[i]>>2)<<5) | (bbuffer[i]>>3);
                i--;
                ri++;
            }
            i=i-ScreenWidth/2+dcx;
            ri+=ScreenWidth/2-dcx;
            avram=avram-ScreenWidth/2+dcx;
      }


// 2 tetarthmorio

 	avram=(unsigned short*)vram + ScreenWidth/2 + (ScreenHeight/2-1)*ScreenWidth + dci;

	i=ScreenWidth/2 + (ScreenHeight/2-1)*ScreenWidth + dci;
	ri=0;
      for (int y=ScreenHeight/2-1+dcy; y>=0; y--)
      {
            for (int x=ScreenWidth/2+dcx; x<ScreenWidth; x++)
            {
                rbuffer[i] = (rbuffer[i] + rbuffer[i + radir3[ri]] + rbuffer[i + radir4[ri]] + rbuffer[i + radir5[ri]])>>2;
                gbuffer[i] = (gbuffer[i] + gbuffer[i + radir3[ri]] + gbuffer[i + radir4[ri]] + gbuffer[i + radir5[ri]])>>2;
                bbuffer[i] = (bbuffer[i] + bbuffer[i + radir3[ri]] + bbuffer[i + radir4[ri]] + bbuffer[i + radir5[ri]])>>2;
                *avram++ = ((rbuffer[i]>>3)<<11) | ((gbuffer[i]>>2)<<5) | (bbuffer[i]>>3);
                i++;
                ri++;
            }
            i=i-3*(ScreenWidth/2)+dcx;
            ri+=ScreenWidth/2+dcx;
            avram=avram-3*(ScreenWidth/2)+dcx;
      }


// 3 tetarthmorio

 	avram=(unsigned short*)vram + ScreenWidth/2 + (ScreenHeight/2)*ScreenWidth + dci;

	i=ScreenWidth/2 + (ScreenHeight/2)*ScreenWidth + dci;
	ri=0;
      for (int y=ScreenHeight/2+dcy; y<ScreenHeight; y++)
      {
            for (int x=ScreenWidth/2+dcx; x<ScreenWidth; x++)
            {
                rbuffer[i] = (rbuffer[i] + rbuffer[i + radir0[ri]] + rbuffer[i + radir1[ri]] + rbuffer[i + radir2[ri]])>>2;
                gbuffer[i] = (gbuffer[i] + gbuffer[i + radir0[ri]] + gbuffer[i + radir1[ri]] + gbuffer[i + radir2[ri]])>>2;
                bbuffer[i] = (bbuffer[i] + bbuffer[i + radir0[ri]] + bbuffer[i + radir1[ri]] + bbuffer[i + radir2[ri]])>>2;
                *avram++ = ((rbuffer[i]>>3)<<11) | ((gbuffer[i]>>2)<<5) | (bbuffer[i]>>3);
                i++;
                ri++;
            }
            i+=ScreenWidth/2+dcx;
            ri+=ScreenWidth/2+dcx;
            avram+=ScreenWidth/2+dcx;
      }



// 4o tetarthmorio

 	avram=(unsigned short*)vram + (ScreenWidth/2-1) + (ScreenHeight/2)*ScreenWidth + dci;

	i=(ScreenWidth/2-1) + (ScreenHeight/2)*ScreenWidth + dci;
	ri=0;
      for (int y=ScreenHeight/2+dcy; y<ScreenHeight; y++)
      {
            for (int x=ScreenWidth/2-1+dcx; x>=0; x--)
            {
                rbuffer[i] = (rbuffer[i] + rbuffer[i - radir3[ri]] + rbuffer[i - radir4[ri]] + rbuffer[i - radir5[ri]])>>2;
                gbuffer[i] = (gbuffer[i] + gbuffer[i - radir3[ri]] + gbuffer[i - radir4[ri]] + gbuffer[i - radir5[ri]])>>2;
                bbuffer[i] = (bbuffer[i] + bbuffer[i - radir3[ri]] + bbuffer[i - radir4[ri]] + bbuffer[i - radir5[ri]])>>2;
                *avram-- = ((rbuffer[i]>>3)<<11) | ((gbuffer[i]>>2)<<5) | (bbuffer[i]>>3);
                i--;
                ri++;
            }
            i+=3*(ScreenWidth/2)+dcx;
            ri+=ScreenWidth/2-dcx;
            avram+=3*(ScreenWidth/2)+dcx;
      }
}

void RunRadialblur(int ntime, unsigned short *vram)
{
    Radial(ntime, vram);
}

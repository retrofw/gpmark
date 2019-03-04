#include <math.h>
#include <stdio.h>

#include "engine3d.h"
#include "generate3d.h"
#include "render3d.h"
#include "main.h"

point3d fpts[MAXDATA];
point3d norms[MAXDATA];
point3d pt_norms[MAXDATA];
point2d spts[MAXDATA];
point3d spls[MAXDATA];

unsigned short swp[MAXDATA];

int lightcalc = LIGHTVIEW;

#define fp_mul 256
#define fp_shr 8
#define proj_shr 8


vector3d CrossProduct(vector3d v1, vector3d v2)
{
	vector3d v;
	v.x=v1.y*v2.z-v1.z*v2.y;
	v.y=v1.z*v2.x-v1.x*v2.z;
	v.z=v1.x*v2.y-v1.y*v2.x;
	return v;
}


int DotProduct(vector3d v1, vector3d v2)
{
	return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}


vector3d Normalize(vector3d v)
{
	int d=(int)(sqrt(v.x*v.x+v.y*v.y+v.z*v.z));
	if (d!=0)
	{
	   v.x=(v.x<<fp_shr)/d;
	   v.y=(v.y<<fp_shr)/d;
	   v.z=(v.z<<fp_shr)/d;
    }
    else
    {
        v.x = 0;
        v.y = 0;
        v.z = 0;
    }    
	return v;
}


vector3d NegVec(vector3d v)
{
	v.x=-v.x;
	v.y=-v.y;
	v.z=-v.z;
	return v;
}


void translate3d (object3d *obj)
{
	int i;
	int objposx = (int)(obj->pos.x * fp_mul);
	int objposy = (int)(obj->pos.y * fp_mul);
	int objposz = (int)(obj->pos.z * fp_mul);

    for (i=0; i<obj->npts; i++)
    {
       fpts[i].x += objposx;
       fpts[i].y += objposy;
       fpts[i].z += objposz;
    }
}

void rotate3d (object3d *obj)
{
    float cosxr = cos(obj->rot.x); float cosyr = cos(obj->rot.y); float coszr = cos(obj->rot.z);
    float sinxr = sin(obj->rot.x); float sinyr = sin(obj->rot.y); float sinzr = sin(obj->rot.z);

    int xvx = (int)((cosyr * coszr) * fp_mul); int xvy = (int)((sinxr * sinyr * coszr - cosxr * sinzr) * fp_mul); int xvz = (int)((cosxr * sinyr * coszr + sinxr * sinzr) * fp_mul);
    int yvx = (int)((cosyr * sinzr) * fp_mul); int yvy = (int)((cosxr * coszr + sinxr * sinyr * sinzr) * fp_mul); int yvz = (int)((-sinxr * coszr + cosxr * sinyr * sinzr) * fp_mul);
    int zvx = (int)((-sinyr) * fp_mul); int zvy = (int)((sinxr * cosyr) * fp_mul); int zvz = (int)((cosxr * cosyr) * fp_mul);

    int x, y, z;
    int i;
    for (i=0; i<obj->npts; i++)
    {
        x = obj->point[i].x;
        y = obj->point[i].y;
        z = obj->point[i].z;
        fpts[i].x = x * xvx + y * xvy + z * xvz;
        fpts[i].y = x * yvx + y * yvy + z * yvz;
        fpts[i].z = x * zvx + y * zvy + z * zvz;
    }
}


void rotate3d_normals (object3d *obj)
{
    float cosxr = cos(obj->rot.x); float cosyr = cos(obj->rot.y); float coszr = cos(obj->rot.z);
    float sinxr = sin(obj->rot.x); float sinyr = sin(obj->rot.y); float sinzr = sin(obj->rot.z);

    int xvx = (int)((cosyr * coszr) * fp_mul); int xvy = (int)((sinxr * sinyr * coszr - cosxr * sinzr) * fp_mul); int xvz = (int)((cosxr * sinyr * coszr + sinxr * sinzr) * fp_mul);
    int yvx = (int)((cosyr * sinzr) * fp_mul); int yvy = (int)((cosxr * coszr + sinxr * sinyr * sinzr) * fp_mul); int yvz = (int)((-sinxr * coszr + cosxr * sinyr * sinzr) * fp_mul);
    int zvx = (int)((-sinyr) * fp_mul); int zvy = (int)((sinxr * cosyr) * fp_mul); int zvz = (int)((cosxr * cosyr) * fp_mul);

    int x, y, z;
    int i;
    for (i=0; i<obj->npls; i++)
    {
        x = obj->normal[i].x;
        y = obj->normal[i].y;
        z = obj->normal[i].z;
        norms[i].x = x * xvx + y * xvy + z * xvz;
        norms[i].y = x * yvx + y * yvy + z * yvz;
        norms[i].z = x * zvx + y * zvy + z * zvz;
    }
}


void rotate3d_pt_normals (object3d *obj)
{
    float cosxr = cos(obj->rot.x); float cosyr = cos(obj->rot.y); float coszr = cos(obj->rot.z);
    float sinxr = sin(obj->rot.x); float sinyr = sin(obj->rot.y); float sinzr = sin(obj->rot.z);

    int xvx = (int)((cosyr * coszr) * fp_mul); int xvy = (int)((sinxr * sinyr * coszr - cosxr * sinzr) * fp_mul); int xvz = (int)((cosxr * sinyr * coszr + sinxr * sinzr) * fp_mul);
    int yvx = (int)((cosyr * sinzr) * fp_mul); int yvy = (int)((cosxr * coszr + sinxr * sinyr * sinzr) * fp_mul); int yvz = (int)((-sinxr * coszr + cosxr * sinyr * sinzr) * fp_mul);
    int zvx = (int)((-sinyr) * fp_mul); int zvy = (int)((sinxr * cosyr) * fp_mul); int zvz = (int)((cosxr * cosyr) * fp_mul);

    int x, y, z;
    int i;
    for (i=0; i<obj->npts; i++)
    {
        x = obj->pt_normal[i].x;
        y = obj->pt_normal[i].y;
        z = obj->pt_normal[i].z;
        pt_norms[i].x = x * xvx + y * xvy + z * xvz;
        pt_norms[i].y = x * yvx + y * yvy + z * yvz;
        pt_norms[i].z = x * zvx + y * zvy + z * zvz;
    }
}

void project3d (object3d *obj)
{
	int i;
    int smul = 1;

	for (i=0; i<obj->npts; i++)
		if (fpts[i].z > 0)
		{
            spts[i].x = ((fpts[i].x << proj_shr) / (fpts[i].z/smul)) + (ScreenWidth>>1);
            spts[i].y = ((fpts[i].y << proj_shr) / (fpts[i].z/smul)) + (ScreenHeight>>1);
		}
}

void CalcPolyColorStatic(object3d *obj)
{
    int i, c;
    for (i=0; i<obj->npls; i++)
    {
        c = norms[i].z>>8;
        if (c<0) c=0;
        if (c>255) c=255;
        spls[i].c = c;
    }
}

void CalcPointColorStatic(object3d *obj)
{
    int i, c;
    for (i=0; i<obj->npts; i++)
    {
        c = pt_norms[i].z>>8;
        if (c<0) c=0;
        if (c>255) c=255;
        spts[i].c = c;
    }
}

void CalcPolyColorDynamic(object3d *obj)
{
    vector3d light, v;
    light.x = 0;
    light.y = 0;
    light.z = 256;
    float c;

    int i;
    for (i=0; i<obj->npls; i++)
    {
        v.x = norms[i].x;
        v.y = norms[i].y;
        v.z = norms[i].z;
        c = DotProduct(v,light);
        spls[i].c = (int)c;
        if (c<0) spls[i].c = 0;
        if (spls[i].c>255) spls[i].c = 255;
    }
}

void CalcPointColorDynamic(object3d *obj)
{
    vector3d light, v;
    light.x = 0;
    light.y = 0;
    light.z = 256;
    float c;

    int i;
    for (i=0; i<obj->npts; i++)
    {
        v.x = pt_norms[i].x;
        v.y = pt_norms[i].y;
        v.z = pt_norms[i].z;
        c = DotProduct(v,light);
        if (c<0) c = 0;
        if (c>255) c = 255;
        spts[i].c = (int)c;
    }
}

void CalcPointColor(object3d *obj)
{
    if (lightcalc==LIGHTVIEW) CalcPointColorStatic(obj);
    if (lightcalc==LIGHTMOVE) CalcPointColorDynamic(obj);
}

void CalcPolyColor(object3d *obj)
{
    if (lightcalc==LIGHTVIEW) CalcPolyColorStatic(obj);
    if (lightcalc==LIGHTMOVE) CalcPolyColorDynamic(obj);
}

void Calc3d(object3d *obj)
{
	rotate3d(obj);
	translate3d(obj);
	project3d(obj);
}

void quicksort (int lo, int hi, int data[])
{
	int m1 = lo;
	int m2 = hi;
	int temp0;
	unsigned short temp1;

	int mp = data[(lo + hi)>>1];

	while (m1<=m2)
	{
		while (data[m1] < mp) m1++;
		while (mp < data[m2]) m2--;

		if (m1<=m2)
		{
			temp0 = data[m1]; data[m1] = data[m2]; data[m2] = temp0;
			temp1 = swp[m1]; swp[m1] = swp[m2]; swp[m2] = temp1;
			m1++;
			m2--;
		}
	}

	if (m2>lo) quicksort(lo, m2, data);
	if (m1<hi) quicksort(m1, hi, data);
}

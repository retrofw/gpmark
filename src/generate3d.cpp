#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine3d.h"
#include "generate3d.h"

unsigned char addtimes[MAXDATA];

void CalcNorms(object3d *obj, int neg)
{
    vector3d v1,v2;

    obj->normal = (vector3d*)malloc(obj->npls * sizeof(vector3d));

    int i;
    for (i=0; i<obj->npls; i++)
    {
        v1.x = obj->point[obj->poly[i].p2].x - obj->point[obj->poly[i].p1].x;
        v1.y = obj->point[obj->poly[i].p2].y - obj->point[obj->poly[i].p1].y;
        v1.z = obj->point[obj->poly[i].p2].z - obj->point[obj->poly[i].p1].z;

        v2.x = obj->point[obj->poly[i].p1].x - obj->point[obj->poly[i].p0].x;
        v2.y = obj->point[obj->poly[i].p1].y - obj->point[obj->poly[i].p0].y;
        v2.z = obj->point[obj->poly[i].p1].z - obj->point[obj->poly[i].p0].z;

        obj->normal[i] = Normalize(CrossProduct(v1,v2));
        if (neg==1) obj->normal[i] = NegVec(obj->normal[i]);
    }
}

void CalcPtNorms(object3d *obj)
{
    obj->pt_normal = (vector3d*)malloc(obj->npts * sizeof(vector3d));

    int i;
    for (i=0; i<MAXDATA; i++)
        addtimes[i] = 0;

    for (i=0; i<obj->npts; i++)
    {
        obj->pt_normal[i].x = 0;
        obj->pt_normal[i].y = 0;
        obj->pt_normal[i].z = 0;
    }

    for (i=0; i<obj->npls; i++)
    {
        obj->pt_normal[obj->poly[i].p0].x += obj->normal[i].x;
        obj->pt_normal[obj->poly[i].p0].y += obj->normal[i].y;
        obj->pt_normal[obj->poly[i].p0].z += obj->normal[i].z;
        obj->pt_normal[obj->poly[i].p1].x += obj->normal[i].x;
        obj->pt_normal[obj->poly[i].p1].y += obj->normal[i].y;
        obj->pt_normal[obj->poly[i].p1].z += obj->normal[i].z;
        obj->pt_normal[obj->poly[i].p2].x += obj->normal[i].x;
        obj->pt_normal[obj->poly[i].p2].y += obj->normal[i].y;
        obj->pt_normal[obj->poly[i].p2].z += obj->normal[i].z;
        addtimes[obj->poly[i].p0]++;
        addtimes[obj->poly[i].p1]++;
        addtimes[obj->poly[i].p2]++;
    }

    for (i=0; i<obj->npts; i++)
    {
        if (addtimes[i]!=0)
        {
            obj->pt_normal[i].x /= addtimes[i];
            obj->pt_normal[i].y /= addtimes[i]; 
            obj->pt_normal[i].z /= addtimes[i];
            obj->pt_normal[i] = Normalize(obj->pt_normal[i]);
        }
    }
}

void ReversePolygonOrder(object3d *obj)
{
    int i, a, b, c;
    for (i=0; i<obj->npls; i++)
    {
        a = obj->poly[i].p0;
        b = obj->poly[i].p1;
        c = obj->poly[i].p2;
        obj->poly[i].p0 = c;
        obj->poly[i].p1 = b;
        obj->poly[i].p2 = a;
    }
}


void Load32bit3doFile(char* filename, object3d *obj, int neg, float stdiv)
{
	FILE *obj3d;
	obj3d=fopen(filename,"rb");

	obj->npts=fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
	obj->nlns=fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
	obj->npls=fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);

    obj->point = (point3d*)malloc(obj->npts * sizeof(point3d));
    obj->line = (line2d*)malloc(obj->nlns * sizeof(line2d));
    obj->poly = (poly2d*)malloc(obj->npls * sizeof(poly2d));

    int i;
	for (i=0; i<obj->npts; i++)
	{
		obj->point[i].x = (int)((fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24) - 65536)/stdiv);
		obj->point[i].y = (int)((fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24) - 65536)/stdiv);
		obj->point[i].z = (int)((fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24) - 65536)/stdiv);
		obj->point[i].c = 0xFFFFFF;
	}

	for (i=0; i<obj->nlns; i++)
	{
        obj->line[i].p0 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
        obj->line[i].p1 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
        obj->line[i].c = 0xFFFFFF;
	}

	for (i=0; i<obj->npls; i++)
	{
        obj->poly[i].p0 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
        obj->poly[i].p1 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
        obj->poly[i].p2 = fgetc(obj3d)+(fgetc(obj3d)<<8)+(fgetc(obj3d)<<16)+(fgetc(obj3d)<<24);
	}

	fclose(obj3d);

    CalcNorms(obj, neg);
    CalcPtNorms(obj);
}

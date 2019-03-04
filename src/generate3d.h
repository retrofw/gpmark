#ifndef GENERATE3D_H
#define GENERATE3D_H

#include "engine3d.h"

void Load32bit3doFile(char* filename, object3d *obj, int neg, float stdiv);

void CalcNorms(object3d *obj, int neg);
void CalcPtNorms(object3d *obj);
void ReversePolygonOrder(object3d *obj);

#endif

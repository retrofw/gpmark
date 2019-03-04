#ifndef RENDER3D_H
#define RENDER3D_H

#include "engine3d.h"

void initRender3D();
void Render(object3d *obj, unsigned short shade[], unsigned short texture[], unsigned short texture_size, unsigned short *vram);

#endif


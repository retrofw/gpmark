#ifndef ROTOZOOMER_H
#define ROTOZOOMER_H

void InitRotozoomer();
void RunRotozoomerNormal(int ntime, unsigned short *vram);
void RunRotozoomerNear(int ntime, unsigned short *vram);
void RunRotozoomerFar(int ntime, unsigned short *vram);

#endif

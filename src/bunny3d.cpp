#include "main.h"
#include "bunny3d.h"
#include "engine3d.h"
#include "generate3d.h"
#include "render3d.h"
#include "env1.h"

object3d bunny3d;

void InitBunny3D()
{
    Load32bit3doFile("./bunnybig.3do", &bunny3d, 0, 2);
    ReversePolygonOrder(&bunny3d);
    bunny3d.pos.x = 0; bunny3d.pos.y = 4096; bunny3d.pos.z = 8192;
    bunny3d.rot.x = 0; bunny3d.rot.y = 0; bunny3d.rot.z = 0;
}

void RunBunny3D(int ntime, unsigned short *vram)
{
    ClearScreen(vram);

    bunny3d.rot.x = PI;
    bunny3d.rot.y = -ntime / 8.0;
    Calc3d(&bunny3d);
    Render(&bunny3d, 0, env1, 64, vram);
}

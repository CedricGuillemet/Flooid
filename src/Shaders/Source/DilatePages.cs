#include "bgfx_compute.sh"
#include "Allocation.sh"

NUM_THREADS(16, 16, 1)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);

    ivec2 dx = ivec2(1, 0);
    ivec2 dy = ivec2(0, 1);

    float tag = imageLoad(s_worldToPageTags, coord.xy).x;
    if (tag == 0.)
    {
        for (int y = -1; y < 2; y++)
        {
            for (int x = -1; x < 2; x++)
            {
                if (imageLoad(s_worldToPageTags, coord.xy + ivec2(x,y)).x == 1./255.)
                {
                    AllocatePage(coord, 2.);
                    return;
                }
            }
        }
    }
}
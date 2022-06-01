#include "bgfx_compute.sh"
#include "Allocation.sh"

IMAGE2D_WR(s_texPages, rgba32f, 5);

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
                if (imageLoad(s_worldToPageTags, coord.xy + ivec2(x,y)).x == 2./255.)
                {
                    uint page = AllocatePage(coord, 1.);

                    // clear page
                    for (int y = 0; y < 16; y++)
                    {
                        for (int x = 0; x < 16; x++)
                        {
                            ivec3 destOut = ivec3(page & 0xF, page >> 4, 0) * 16 + ivec3(x, y, 0);
                            imageStore(s_texPages, destOut, vec4(0, 0, 0, 0));
                        }
                    }

                    return;
                }
            }
        }
    }
}

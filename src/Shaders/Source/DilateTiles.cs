#include "bgfx_compute.sh"
#include "Allocation.sh"
#include "CommonFluid.sh"
#include "Paging.sh"

IMAGE3D_WR(s_texTiles, rgba32f, 5);

NUM_THREADS(8, 8, 8)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);

    float tag = imageLoad(s_worldToTileTags, coord.xyz).x;
    if (tag == 0.)
    {
        for (int z = -1; z < 2; z++)
        {
            for (int y = -1; y < 2; y++)
            {
                for (int x = -1; x < 2; x++)
                {
                    if (imageLoad(s_worldToTileTags, coord.xyz + ivec3(x,y,z)).x == 2./255.)
                    {
                        uint tile = AllocateTile(coord, 1.);

                        // clear tile
                        for (int tz = 0; tz < 16; tz++)
                        {
                            for (int ty = 0; ty < 16; ty++)
                            {
                                for (int tx = 0; tx < 16; tx++)
                                {
                                    ivec3 destOut = GetOutAddr(tile, ivec3(tx, ty, tz));
                                    imageStore(s_texTiles, destOut, vec4(0, 0, 0, 0));
                                }
                            }
                        }
                        return;
                    }
                }
            }
        }
        // clear world to tile or it will be used when sampling
        imageStore(s_worldToTiles, coord.xyz, vec4(0., 0., 0., 0.));
    }
}

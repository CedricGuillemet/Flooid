#include "bgfx_compute.sh"
//#include "Allocation.sh"

BUFFER_WR(bufferAddressTiles, uint, 0);
IMAGE3D_WR(s_worldToTilesCoarser, rgba8, 1);
BUFFER_WR(bufferTiles, uint, 2);
BUFFER_RW(bufferCounter, uint, 3);
IMAGE2D_RO(s_worldToTileTags, r8, 4);
IMAGE2D_WR(s_worldToTileTagsCoarser, r8, 5);

void AllocateTile(ivec3 addr, float tileTag)
{
    uint counter;
    atomicFetchAndAdd(bufferCounter[0], 1, counter);

    uint tileAddress = addr.x + (addr.y << 10) + (addr.z << 20);
    uint tile = bufferTiles[counter];

    //bufferTiles[counter] = tile;
    bufferAddressTiles[counter] = tileAddress;

    imageStore(s_worldToTilesCoarser, addr.xyz, ivec4(tile & 0xF, (tile >> 4) & 0xF, 0, 0) / 255.);
    imageStore(s_worldToTileTagsCoarser, addr.xyz, vec4(tileTag, 0., 0., 0.)/255.);
}

NUM_THREADS(16, 16, 1)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    ivec3 dx = ivec3(1, 0, 0);
    ivec3 dy = ivec3(0, 1, 0);

    ivec3 addr = coord * 2;

    vec4 tag00 = imageLoad(s_worldToTileTags, addr);
    vec4 tag10 = imageLoad(s_worldToTileTags, addr + dx);
    vec4 tag01 = imageLoad(s_worldToTileTags, addr + dy);
    vec4 tag11 = imageLoad(s_worldToTileTags, addr + dx + dy);

    if (tag00.x > 0. || tag10.x > 0. || tag01.x > 0. || tag11.x > 0.)
    {
        AllocateTile(coord, 1.);
    } else {
        imageStore(s_worldToTilesCoarser, coord.xyz, vec4(0., 0., 0., 0.));
        imageStore(s_worldToTileTagsCoarser, coord.xyz, vec4(0., 0., 0., 0.));
    }
}

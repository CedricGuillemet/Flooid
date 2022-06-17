#include "bgfx_compute.sh"


BUFFER_WR(bufferAddressTiles, uint, 0);
IMAGE2D_WR(s_worldToTiles, rgba8, 1);
BUFFER_WR(bufferTiles, uint, 2);
BUFFER_RW(bufferCounter, uint, 3);
IMAGE2D_WR(s_worldToTileTags, r8, 4);

uint AllocateTile(ivec3 addr, float tileTag)
{
    uint counter;
    atomicFetchAndAdd(bufferCounter[0], 1, counter);

    uint tileAddress = addr.x + (addr.y << 10) + (addr.z << 20);
    uint tile = bufferTiles[counter];

    //bufferTiles[counter] = tile;
    bufferAddressTiles[counter] = tileAddress;

    imageStore(s_worldToTiles, addr.xy, ivec4(tile & 0xF, (tile >> 4) & 0xF, 0, 0) / 255.);
    imageStore(s_worldToTileTags, addr.xy, vec4(tileTag, 0, 0, 0)/255.);

    return tile;
}

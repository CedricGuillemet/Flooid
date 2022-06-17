#include "bgfx_compute.sh"
//#include "Allocation.sh"
#include "Paging.sh"

//uniform vec4 groupMin;//, worldMax;
BUFFER_RO(bufferAddressTiles, uint, 0);
BUFFER_RO(bufferTiles, uint, 1);
IMAGE2D_WR(s_worldToTileTags, r8, 2);
IMAGE2D_RO(s_texTiles, rgba32f, 3);
BUFFER_RW(bufferCounter, uint, 4);

BUFFER_RW(bufferActiveTiles, uint, 5);
BUFFER_RW(bufferFreedTiles, uint, 6);
BUFFER_RW(bufferActiveTileAdresses, uint, 7);

NUM_THREADS(1, 1, 1)
void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    
    uint tile = bufferTiles[coord.y];
    uint tileAddress = bufferAddressTiles[coord.y];

    ivec3 base = ivec3(tile&0xF, tile>>4, 0) * 16;// + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    ivec3 tagPosition = TileAddress(tileAddress);
    
    float threshold = 0.01;
    for (int y = 0; y < 16; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            ivec3 addr = base + ivec3(x, y, 0);
            vec4 value = imageLoad(s_texTiles, addr.xy);
            if (value.x > threshold)
            {
                uint counter;
                atomicFetchAndAdd(bufferCounter[1], 1, counter);
                bufferActiveTiles[counter] = tile;
                bufferActiveTileAdresses[counter] = tileAddress;
                imageStore(s_worldToTileTags, tagPosition.xy, vec4(2, 0, 0, 0)/ 255.);
                return;
            }
        }
    }
    
    uint counter;
    atomicFetchAndAdd(bufferCounter[2], 1, counter);
    bufferFreedTiles[counter] = tile;

    
    imageStore(s_worldToTileTags, tagPosition.xy, vec4(0, 0, 0, 0));
}

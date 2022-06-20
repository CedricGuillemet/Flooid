#include "bgfx_compute.sh"
//#include "Allocation.sh"
#include "Paging.sh"

//uniform vec4 groupMin;//, worldMax;
BUFFER_RO(bufferAddressTiles, uint, 0);
BUFFER_RO(bufferTiles, uint, 1);
IMAGE3D_WR(s_worldToTileTags, r8, 2);
IMAGE3D_RO(s_texTiles, rgba32f, 3);
BUFFER_RW(bufferCounter, uint, 4);

BUFFER_RW(bufferActiveTiles, uint, 5);
BUFFER_RW(bufferFreedTiles, uint, 6);
BUFFER_RW(bufferActiveTileAdresses, uint, 7);

NUM_THREADS(1, 1, 1)
void main()
{
    ivec3 coord = ivec3(gl_GlobalInvocationID.xyz);
    
    uint tile = bufferTiles[coord.y];
    uint tileAddress = bufferAddressTiles[coord.y];

    ivec3 base = ivec3(tile&0xF, (tile>>4) & 0xF, (tile>>8) & 0xF) * 16;// + ivec3(coord.x & 0xF, coord.y & 0xF, 0);

    ivec3 tagPosition = TileAddress(tileAddress);
    
    float threshold = 0.01;
    for (int z = 0; z < 16; z++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int x = 0; x < 16; x++)
            {
                ivec3 addr = base + ivec3(x, y, z);
                vec4 value = imageLoad(s_texTiles, addr.xyz);
                if (value.x > threshold)
                {
                    uint counter;
                    atomicFetchAndAdd(bufferCounter[1], 1, counter);
                    bufferActiveTiles[counter] = tile;
                    bufferActiveTileAdresses[counter] = tileAddress;
                    imageStore(s_worldToTileTags, tagPosition.xyz, vec4(2, 0, 0, 0)/ 255.);
                    return;
                }
            }
        }
    }
    
    uint counter;
    atomicFetchAndAdd(bufferCounter[2], 1, counter);
    bufferFreedTiles[counter] = tile;

    
    imageStore(s_worldToTileTags, tagPosition.xyz, vec4(0, 0, 0, 0));
}
